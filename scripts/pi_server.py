#!/usr/bin/env python3
"""OpenAI-compatible HTTP server wrapping parrot0 for the pi coding agent.

Starts parrot0 as a persistent subprocess and exposes /v1/chat/completions
and /v1/models so that ``pi --provider parrot0 --model parrot0`` works.

Usage:  python scripts/pi_server.py [--port PORT] [--host HOST]
"""

from __future__ import annotations

import argparse
import http.server
import json
import os
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PARROT0_BIN = ROOT / "bin" / "parrot0"
PARROT0_PROFILE = ROOT / "kb" / "profiles" / "agi.p0"
LOG_FILE = Path(os.environ.get("PARROT0_PI_LOG", "/tmp/parrot0-pi-traffic.log"))


def _log(msg: str) -> None:
    with LOG_FILE.open("a") as f:
        f.write(msg + "\n")


def _truncate(s: str, n: int = 240) -> str:
    return s if len(s) <= n else s[:n] + "..."


class Parrot0:
    """Wraps a single parrot0 subprocess with thread-safe access."""

    def __init__(self) -> None:
        env = {
            **os.environ,
            "PARROT0_BASE": str(ROOT / "kb" / "core" / "base.p0"),
            "PARROT0_SESSION": "",
            "PARROT0_PROFILE": str(PARROT0_PROFILE),
            # gen205: run parrot0 as a LOCAL coding agent. Because parrot0 lives on
            # this machine it executes read-only filesystem tools (list/read/grep/
            # find) itself and answers in one turn — no OpenAI tool_call round-trip.
            # Opt-out by setting PARROT0_TOOLS=0 in the environment before launch.
            "PARROT0_TOOLS": os.environ.get("PARROT0_TOOLS", "1"),
        }
        self._proc = subprocess.Popen(
            [str(PARROT0_BIN)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
            env=env,
        )
        self._lock = threading.Lock()
        # drain startup stderr (greeting + version) in background
        self._stderr_drainer = threading.Thread(target=self._drain_stderr, daemon=True)
        self._stderr_drainer.start()
        # drain the initial prompt ("you>") written to stderr
        time.sleep(0.3)

    def _drain_stderr(self) -> None:
        try:
            while self._proc.stderr:
                line = self._proc.stderr.readline()
                if not line:
                    break
        except Exception:
            pass

    def ask(self, prompt: str) -> str:
        """Send a single line to parrot0 and return the response line."""
        with self._lock:
            if self._proc.poll() is not None:
                return "[parrot0 process died]"
            try:
                line = prompt.replace("\n", " ").replace("\r", " ")[:2000]
                _log(f">>> parrot0 stdin: {_truncate(line, 200)}")
                self._proc.stdin.write(line + "\n")
                self._proc.stdin.flush()
                resp = self._proc.stdout.readline()
                if not resp:
                    return "[parrot0 closed stdout]"
                out = resp.rstrip("\n")
                _log(f"<<< parrot0 stdout: {_truncate(out, 200)}")
                return out
            except (BrokenPipeError, OSError) as e:
                return f"[parrot0 I/O error: {e}]"

    def shutdown(self) -> None:
        try:
            self._proc.stdin.write("/quit\n")
            self._proc.stdin.flush()
        except Exception:
            pass
        try:
            self._proc.wait(timeout=3)
        except subprocess.TimeoutExpired:
            self._proc.kill()
            self._proc.wait()


def _content_text(content: object) -> str:
    if isinstance(content, list):
        return " ".join(
            str(p.get("text", ""))
            for p in content
            if isinstance(p, dict) and p.get("type") == "text"
        )
    return str(content or "")


def _build_prompt(messages: list[dict]) -> str:
    """Convert OpenAI messages to the prompt shape parrot0 handles best."""
    last_user: str | None = None
    system_prefix = ""

    for m in messages:
        role = m.get("role", "user")
        content = _content_text(m.get("content", ""))
        if role == "system" and not system_prefix:
            system_prefix = f"[Instructions: {content}] "
        elif role == "user":
            last_user = content

    if last_user is None:
        return ""

    # pi sends a large coding-agent system prompt. parrot0 currently performs
    # better when only the latest user text reaches its line-based protocol.
    if os.environ.get("PARROT0_PI_INCLUDE_SYSTEM") == "1":
        return f"{system_prefix}{last_user}"
    return last_user


class Handler(http.server.BaseHTTPRequestHandler):
    parrot0: Parrot0 | None = None  # set by caller

    def log_message(self, fmt: str, *args: object) -> None:
        print(f"  pi_server: {fmt % args}", file=sys.stderr, flush=True)

    def _send_json(self, code: int, body: dict) -> None:
        data = json.dumps(body).encode()
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_POST(self) -> None:
        if self.path != "/v1/chat/completions":
            self._send_json(404, {"error": "not found"})
            return
        if not Handler.parrot0:
            self._send_json(500, {"error": "parrot0 not initialised"})
            return

        cl = int(self.headers.get("Content-Length", "0"))
        body = json.loads(self.rfile.read(cl))

        messages: list[dict] = body.get("messages", [])
        prompt = _build_prompt(messages)
        streaming = body.get("stream", False)
        _log(f"=== POST stream={streaming} model={body.get('model')} "
             f"n_msgs={len(messages)} prompt_len={len(prompt)}")
        for i, m in enumerate(messages):
            c = m.get("content", "")
            if isinstance(c, list):
                c = " ".join(p.get("text", "") for p in c if p.get("type") == "text")
            _log(f"  msg[{i}] role={m.get('role')} content={_truncate(str(c), 160)}")

        if streaming:
            self._send_streaming(prompt, body)
        else:
            self._send_nonstreaming(prompt, body)

    def _send_nonstreaming(self, prompt: str, body: dict) -> None:
        model = body.get("model", "parrot0")
        content = Handler.parrot0.ask(prompt)
        resp = {
            "id": f"parrot0-{int(time.time()*1000)}",
            "object": "chat.completion",
            "created": int(time.time()),
            "model": model,
            "choices": [
                {
                    "index": 0,
                    "message": {"role": "assistant", "content": content},
                    "finish_reason": "stop",
                }
            ],
            "usage": {
                "prompt_tokens": len(prompt.split()),
                "completion_tokens": len(content.split()),
                "total_tokens": len(prompt.split()) + len(content.split()),
            },
        }
        self._send_json(200, resp)

    def _send_streaming(self, prompt: str, body: dict) -> None:
        model = body.get("model", "parrot0")
        content = Handler.parrot0.ask(prompt)
        cid = f"parrot0-{int(time.time()*1000)}"

        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream")
        self.send_header("Cache-Control", "no-cache")
        self.end_headers()

        # send chunk
        chunk = json.dumps({
            "id": cid,
            "object": "chat.completion.chunk",
            "created": int(time.time()),
            "model": model,
            "choices": [{"index": 0, "delta": {"role": "assistant"}, "finish_reason": None}],
        })
        self.wfile.write(f"data: {chunk}\n\n".encode())
        self.wfile.flush()

        # send content delta
        chunk = json.dumps({
            "id": cid,
            "object": "chat.completion.chunk",
            "created": int(time.time()),
            "model": model,
            "choices": [{"index": 0, "delta": {"content": content}, "finish_reason": None}],
        })
        self.wfile.write(f"data: {chunk}\n\n".encode())
        self.wfile.flush()

        # send done
        chunk = json.dumps({
            "id": cid,
            "object": "chat.completion.chunk",
            "created": int(time.time()),
            "model": model,
            "choices": [{"index": 0, "delta": {}, "finish_reason": "stop"}],
        })
        self.wfile.write(f"data: {chunk}\n\n".encode())
        self.wfile.write(b"data: [DONE]\n\n")
        self.wfile.flush()

    def do_GET(self) -> None:
        if self.path == "/v1/models":
            self._send_json(200, {
                "object": "list",
                "data": [
                    {
                        "id": "parrot0",
                        "object": "model",
                        "created": 0,
                        "owned_by": "parrot0",
                    }
                ],
            })
        elif self.path == "/health":
            self._send_json(200, {"status": "ok"})
        else:
            self._send_json(404, {"error": "not found"})


class Server(http.server.ThreadingHTTPServer):
    allow_reuse_address = True


def main() -> int:
    ap = argparse.ArgumentParser(description="parrot0 OpenAI-compatible HTTP server for pi")
    ap.add_argument("--port", type=int, default=9899)
    ap.add_argument("--host", default="127.0.0.1")
    args = ap.parse_args()

    if not PARROT0_BIN.is_file():
        print(f"pi_server: {PARROT0_BIN} not found - run 'make build' first", file=sys.stderr)
        return 2

    print(f"pi_server: starting parrot0 ...", file=sys.stderr, flush=True)
    parrot0 = Parrot0()
    Handler.parrot0 = parrot0

    server = Server((args.host, args.port), Handler)
    print(
        f"pi_server: pid={os.getpid()} listening on http://{args.host}:{args.port}/v1/chat/completions",
        file=sys.stderr,
        flush=True,
    )

    def _shutdown(sig: int, frame: object) -> None:
        print("\npi_server: shutting down...", file=sys.stderr, flush=True)
        parrot0.shutdown()
        threading.Thread(target=server.shutdown, daemon=True).start()

    signal.signal(signal.SIGINT, _shutdown)
    signal.signal(signal.SIGTERM, _shutdown)

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        parrot0.shutdown()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
