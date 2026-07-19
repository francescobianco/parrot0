#!/usr/bin/env python3
"""End-to-end T02 ratchet for the OpenAI daemon's prompt boundary.

The daemon must preserve structured/multiline user content beyond the old 2000
byte cut, while rejecting an over-budget prompt atomically: no prefix may reach
the stateful Brain.  The test uses only localhost and the standard library.
"""

from __future__ import annotations

import json
import os
import socket
import subprocess
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
PROMPT_LIMIT = 64 * 1024 - 1


def fnv1a64(data: bytes) -> str:
    """Match serve.c/the execution kernel's stable unsigned-64-bit digest."""
    value = 1469598103934665603
    for byte in data:
        value ^= byte
        value = (value * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return f"{value:016x}"


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def request_json(url: str, content: Any) -> tuple[int, dict[str, Any]]:
    body = json.dumps(
        {"model": "parrot0", "messages": [{"role": "user", "content": content}]},
        ensure_ascii=False,
    ).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=15) as response:
            return response.status, json.loads(response.read())
    except urllib.error.HTTPError as error:
        return error.code, json.loads(error.read())


def answer(url: str, content: Any) -> str:
    status, payload = request_json(url, content)
    if status != 200:
        raise AssertionError(f"expected HTTP 200, got {status}: {payload}")
    return str(payload["choices"][0]["message"]["content"])


def main() -> int:
    binary = ROOT / "bin" / "parrot0"
    if not binary.is_file():
        print("openai-input-limit: bin/parrot0 missing; run make", file=sys.stderr)
        return 2

    port = free_port()
    base = f"http://127.0.0.1:{port}"
    env = {
        **os.environ,
        "PARROT0_BASE": "",
        "PARROT0_SESSION": "",
        "PARROT0_PROFILE": "",
        "PARROT0_WORLD_FACTS": "0",
        "PARROT0_LANG": "en",
        "PARROT0_TOOLS": "0",
    }
    env.pop("PARROT0_PI_INCLUDE_SYSTEM", None)
    server = subprocess.Popen(
        [str(binary), "--daemon", "--host", "127.0.0.1", "--port", str(port)],
        cwd=ROOT,
        env=env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    passed = 0
    failed = 0

    def check(label: str, condition: bool, detail: str = "") -> None:
        nonlocal passed, failed
        if condition:
            passed += 1
            print(f"PASS openai-input-limit: {label}")
        else:
            failed += 1
            print(f"FAIL openai-input-limit: {label}: {detail}", file=sys.stderr)

    try:
        healthy = False
        for _ in range(80):
            try:
                with urllib.request.urlopen(f"{base}/health", timeout=1) as response:
                    healthy = json.loads(response.read()).get("status") == "ok"
                if healthy:
                    break
            except (OSError, urllib.error.URLError, json.JSONDecodeError):
                time.sleep(0.05)
        if not healthy:
            print("FAIL openai-input-limit: daemon did not start", file=sys.stderr)
            return 1

        url = f"{base}/v1/chat/completions"

        # A structured OpenAI content array, including CRLF multiline text and
        # an ignored non-text part.  The decisive fact starts beyond byte 2000
        # but remains below both mod_reader's 4096-byte passage buffer and T02's
        # explicit daemon limit.
        long_head = "read:\r\n" + ("padding.\r\n" * 260)
        tail_fact = "tailprobe is a pilot."
        normalized_prompt = (long_head + " " + tail_fact).replace("\r\n", "\n").replace("\r", "\n")
        structured = [
            {"type": "text", "text": long_head},
            {"type": "image_url", "image_url": {"url": "data:,ignored"}},
            {"type": "text", "text": tail_fact},
        ]
        check(
            "fixture crosses the former boundary",
            2000 < len(normalized_prompt.encode()) <= PROMPT_LIMIT,
            f"bytes={len(normalized_prompt.encode())}",
        )
        learned = answer(url, structured)
        recalled = answer(url, "is tailprobe a pilot?")
        check(
            "structured multiline tail reaches the Brain intact",
            learned.startswith("Learned 1 fact") and recalled.strip() == "Yes.",
            f"learn=[{learned}] recall=[{recalled}]",
        )

        # Establish real conversational state.  If the rejected request were
        # silently truncated and dispatched, its prefix would overwrite this
        # name exactly as a normal `my name is ...` turn does.
        answer(url, "my name is BaselineKeeper")
        baseline = answer(url, "what is my name?")
        check("control side effect is live", "BaselineKeeper" in baseline, baseline)

        prefix = "my name is OversizePoison\n"
        oversize = prefix + ("x" * (PROMPT_LIMIT + 257 - len(prefix)))
        oversize_bytes = oversize.encode("utf-8")
        status, error = request_json(url, oversize)
        info = error.get("error", {})
        expected_digest = "fnv1a64:" + fnv1a64(oversize_bytes)
        check("oversize returns HTTP 413", status == 413, f"status={status} body={error}")
        check(
            "oversize error reports complete bytes and explicit limit",
            info.get("code") == "input_too_large"
            and info.get("bytes") == len(oversize_bytes)
            and info.get("limit") == PROMPT_LIMIT,
            str(error),
        )
        check(
            "oversize digest is stable and covers the complete prompt",
            info.get("digest") == expected_digest,
            f"want={expected_digest} got={info.get('digest')}",
        )

        after = answer(url, "what is my name?")
        check(
            "rejected prefix caused no conversational side effect",
            "BaselineKeeper" in after and "OversizePoison" not in after,
            after,
        )
    except Exception as error:  # keep the daemon cleanup reliable on any failure
        failed += 1
        print(f"FAIL openai-input-limit: unexpected error: {error}", file=sys.stderr)
    finally:
        server.terminate()
        try:
            server.wait(timeout=3)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait(timeout=3)

    print("---")
    print(f"passed: {passed}, failed: {failed}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
