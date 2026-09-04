#!/usr/bin/env python3
"""Run reproducible, sequential coding-agent matches in one neutral workspace.

The controller deliberately knows agent names while the task prompt, current
working directory and artifact names do not.  Each competitor receives the
same flattened prompt and a byte-identical copy of ``seed/``.  The first result
is archived before ``code/`` is rebuilt for the second competitor.

This is an observational benchmark, not a sandbox.  Candidate code is executed
by match-specific judges and agents may run local commands, so use it only with
agents and tasks you trust.
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import os
import pty
import re
import selectors
import shutil
import signal
import struct
import subprocess
import sys
import tempfile
import termios
import time
from pathlib import Path
from typing import Any


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
LEAGUE_FILE = HERE / "league.json"
# La directory che contiene la league in uso: `tasks/` e gli archivi dei run si
# risolvono rispetto a QUESTA, non allo script.  E' cio' che rende possibile una
# league finta (`selftest/`) accanto a quella vera, senza copiare il pilota.
BASE = HERE
ANSI_RE = re.compile(
    rb"(?:\x1B\[[0-?]*[ -/]*[@-~]|\x1B\][^\x07]*(?:\x07|\x1B\\)|\x1B[@-_])"
)
SAFE_ID_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._-]{0,79}$")


class ChallengeError(RuntimeError):
    pass


def load_json(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ChallengeError(f"cannot read {path}: {exc}") from exc
    if not isinstance(value, dict):
        raise ChallengeError(f"{path} must contain a JSON object")
    return value


def utc_run_id() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")


def expand(value: str, variables: dict[str, str]) -> str:
    try:
        return value.format_map(variables)
    except KeyError as exc:
        raise ChallengeError(f"unknown configuration placeholder {exc} in {value!r}") from exc


def command_exists(argv0: str) -> bool:
    return Path(argv0).is_file() if os.path.sep in argv0 else shutil.which(argv0) is not None


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def tree_digest(root: Path) -> str:
    digest = hashlib.sha256()
    if not root.exists():
        return digest.hexdigest()
    for path in sorted(p for p in root.rglob("*") if p.is_file()):
        rel = path.relative_to(root).as_posix().encode()
        digest.update(len(rel).to_bytes(4, "big"))
        digest.update(rel)
        digest.update(bytes.fromhex(sha256_file(path)))
    return digest.hexdigest()


def flatten_prompt(text: str) -> str:
    """One physical line works in both readline and terminal-TUI clients."""
    return " ".join(text.split())


def clean_transcript(raw: bytes) -> str:
    data = ANSI_RE.sub(b"", raw).replace(b"\r", b"\n")
    text = data.decode("utf-8", errors="replace")
    # Full-screen TUIs redraw aggressively.  Preserve order while removing only
    # adjacent duplicate blank lines; raw.log remains the lossless authority.
    lines: list[str] = []
    previous_blank = False
    for line in text.splitlines():
        line = "".join(ch for ch in line if ch == "\t" or ord(ch) >= 32).rstrip()
        blank = not line
        if blank and previous_blank:
            continue
        lines.append(line)
        previous_blank = blank
    return "\n".join(lines).strip() + "\n"


def copy_seed(seed: Path, code: Path) -> None:
    if code.exists():
        shutil.rmtree(code)
    code.mkdir(parents=True)
    if seed.is_dir():
        shutil.copytree(seed, code, dirs_exist_ok=True)


def terminate_process(proc: subprocess.Popen[bytes], master_fd: int, quit_text: str) -> None:
    if proc.poll() is not None:
        return
    if quit_text:
        try:
            os.write(master_fd, quit_text.encode())
            proc.wait(timeout=2)
            return
        except (OSError, subprocess.TimeoutExpired):
            pass
    try:
        os.killpg(proc.pid, signal.SIGINT)
        proc.wait(timeout=2)
        return
    except (ProcessLookupError, subprocess.TimeoutExpired):
        pass
    try:
        os.killpg(proc.pid, signal.SIGTERM)
        proc.wait(timeout=2)
    except (ProcessLookupError, subprocess.TimeoutExpired):
        if proc.poll() is None:
            os.killpg(proc.pid, signal.SIGKILL)
            proc.wait(timeout=2)


def pty_session(
    argv: list[str],
    cwd: Path,
    env: dict[str, str],
    prompt: str,
    config: dict[str, Any],
    timeout: float,
    trace_dir: Path,
) -> tuple[bytes, dict[str, Any]]:
    """Drive one client while a three-second lease watches its byte stream.

    Spinner/gauge redraws are bytes and renew the lease.  Silence is acceptable
    only when the visible state matches a declared completion or interaction
    action; otherwise the session is invalid and the whole race is cancelled.
    """
    master_fd, slave_fd = pty.openpty()
    # A stable large terminal reduces FreeBuff's redraw noise and line wrapping.
    term_size = struct.pack("HHHH", 45, 160, 0, 0)
    import fcntl

    fcntl.ioctl(slave_fd, termios.TIOCSWINSZ, term_size)
    started = time.monotonic()
    proc = subprocess.Popen(
        argv,
        cwd=cwd,
        env=env,
        stdin=slave_fd,
        stdout=slave_fd,
        stderr=slave_fd,
        start_new_session=True,
        close_fds=True,
    )
    os.close(slave_fd)
    selector = selectors.DefaultSelector()
    selector.register(master_fd, selectors.EVENT_READ)
    trace_dir.mkdir(parents=True, exist_ok=True)
    raw_log = (trace_dir / "raw.log").open("wb")
    stream_log = (trace_dir / "stream.jsonl").open("w", encoding="utf-8")
    action_log = (trace_dir / "logic-actions.jsonl").open("w", encoding="utf-8")
    chunks: list[bytes] = []
    after_prompt = bytearray()
    ready_pattern = config.get("ready_pattern", "")
    done_pattern = config.get("done_pattern", "")
    boot_wait = float(config.get("boot_wait_seconds", 2.0))
    min_runtime = float(config.get("min_runtime_seconds", 4.0))
    stream_watch = float(config.get("stream_watch_seconds", 3.0))
    # gen501 — un agente REPL non finisce un compito in un turno.
    #
    # `max_turns` e' l'ADATTATORE dell'interfaccia, non un vantaggio: il TESTO
    # del compito resta identico byte per byte, e la continuazione non aggiunge
    # nessuna informazione sul task (`continue_text` e' neutro e dichiarato nella
    # league). Un agente che lavora da solo dopo un submit usa `max_turns: 1` e
    # non vede nessuna differenza. Il numero di turni usati finisce nel result,
    # perche' un confronto in cui un lato ha avuto piu' giri deve dirlo.
    max_turns = int(config.get("max_turns", 1))
    continue_text = str(config.get("continue_text", ""))
    # Quanto silenzio serve per dire «e' fermo, non sta pensando».
    idle_settle = float(config.get("idle_settle_seconds", 1.5))
    turns_used = 0
    prompt_sent_at: float | None = None
    last_output = started
    reason = "process_exit"
    startup_ready = False
    task_submitted = False
    action_uses: dict[str, int] = {}
    action_trace: list[dict[str, Any]] = []
    stream_trace: list[dict[str, Any]] = []

    def read_available(wait: float) -> bool:
        nonlocal last_output
        got = False
        for key, _ in selector.select(wait):
            try:
                data = os.read(key.fd, 65536)
            except OSError:
                data = b""
            if data:
                offset = sum(len(chunk) for chunk in chunks)
                chunks.append(data)
                raw_log.write(data)
                raw_log.flush()
                if prompt_sent_at is not None:
                    after_prompt.extend(data)
                last_output = time.monotonic()
                event = {
                    "at_seconds": round(last_output - started, 3),
                    "offset": offset,
                    "bytes": len(data),
                    "sha256": hashlib.sha256(data).hexdigest(),
                }
                stream_trace.append(event)
                stream_log.write(json.dumps(event) + "\n")
                stream_log.flush()
                got = True
        return got

    def fire_logic_actions(phase: str, visible: str) -> bool:
        nonlocal last_output
        fired = False
        for rule in config.get("logic_actions", []):
            if rule.get("phase", "run") not in (phase, "any"):
                continue
            action_id = str(rule.get("id", "unnamed_action"))
            used = action_uses.get(action_id, 0)
            if used >= int(rule.get("max_uses", 1)):
                continue
            literal = str(rule.get("when", ""))
            regex = str(rule.get("when_regex", ""))
            matched = bool(literal and literal in visible)
            if regex:
                matched = re.search(
                    regex, visible, re.IGNORECASE | re.MULTILINE | re.DOTALL
                ) is not None
            if not matched:
                continue
            sent = ""
            if "send_text" in rule:
                sent = str(rule["send_text"])
                os.write(master_fd, sent.encode("utf-8"))
                if rule.get("submit", True):
                    os.write(master_fd, b"\r")
            key = str(rule.get("send_key", "")).upper()
            key_bytes = {"ENTER": b"\r", "ESC": b"\x1b", "CTRL_C": b"\x03"}.get(key)
            if key_bytes:
                os.write(master_fd, key_bytes)
                sent = f"<{key}>"
            action_uses[action_id] = used + 1
            last_output = time.monotonic()
            event = {
                "at_seconds": round(last_output - started, 3),
                "phase": phase,
                "state_match": literal or regex,
                "action": action_id,
                "sent": sent,
                "visible_sha256": hashlib.sha256(visible.encode()).hexdigest(),
            }
            action_trace.append(event)
            action_log.write(json.dumps(event, ensure_ascii=False) + "\n")
            action_log.flush()
            print(f"  logic-action: {phase}/{action_id} -> {sent}", flush=True)
            fired = True
        return fired

    try:
        while proc.poll() is None:
            read_available(0.1)
            elapsed = time.monotonic() - started
            visible = clean_transcript(b"".join(chunks))
            fire_logic_actions("startup", visible)
            if ready_pattern and ready_pattern in visible:
                startup_ready = True
                print("  state: task_ready", flush=True)
                break
            if not ready_pattern and elapsed >= boot_wait:
                startup_ready = True
                print("  state: task_ready (settled)", flush=True)
                break
            if time.monotonic() - last_output >= stream_watch:
                reason = "stream_stalled_startup"
                print("  state: interaction_required; byte stream stalled", flush=True)
                break
            if elapsed >= min(timeout, float(config.get("startup_timeout_seconds", 60))):
                reason = "startup_timeout"
                break

        if proc.poll() is not None:
            read_available(0)
            reason = "startup_exit"
        elif startup_ready:
            prompt_sent_at = time.monotonic()
            last_output = prompt_sent_at
            if config.get("bracketed_paste", False):
                os.write(master_fd, b"\x1b[200~" + prompt.encode("utf-8") + b"\x1b[201~")
            else:
                os.write(master_fd, prompt.encode("utf-8"))
            submit_delay = float(config.get("submit_delay_seconds", 0.15))
            until_submit = time.monotonic() + submit_delay
            while time.monotonic() < until_submit and proc.poll() is None:
                read_available(max(0.0, min(0.05, until_submit - time.monotonic())))
            os.write(master_fd, b"\r")
            task_submitted = True
            print("  state: working; task submitted", flush=True)
            turns_used = 1
            while proc.poll() is None:
                read_available(0.25)
                now = time.monotonic()
                active = now - prompt_sent_at
                visible_after = clean_transcript(bytes(after_prompt))
                fire_logic_actions("run", visible_after)
                if active >= timeout:
                    reason = "timeout"
                    break
                idle = now - last_output
                # gen501 — «FERMO AL PROMPT» NON E' «HA STAMPATO IL PROMPT».
                #
                # Prima bastava che il marcatore comparisse ovunque dopo il
                # submit: per parrot0 il marcatore E' il suo prompt, quindi la
                # sessione finiva dopo la prima risposta — sette secondi, zero
                # file toccati. Ora servono DUE cose insieme: il marcatore in
                # CODA a cio' che si vede, e lo stream fermo abbastanza da dire
                # che non sta piu' lavorando. Un prompt ristampato in mezzo al
                # lavoro non chiude piu' niente.
                at_prompt = bool(done_pattern) and visible_after.rstrip().endswith(
                    done_pattern.rstrip()
                )
                if at_prompt and active >= min_runtime and idle >= idle_settle:
                    if turns_used < max_turns and continue_text:
                        turns_used += 1
                        os.write(master_fd, continue_text.encode("utf-8") + b"\r")
                        last_output = time.monotonic()
                        print(f"  state: continuing (turn {turns_used})", flush=True)
                        continue
                    reason = "completion_marker"
                    print("  state: completion", flush=True)
                    break
                if idle >= stream_watch:
                    # Lo stream fermo mentre il prompt e' in coda non e' un
                    # blocco: e' un agente che aspetta. Sono due stati diversi e
                    # confonderli annullava gare valide.
                    if at_prompt:
                        continue
                    reason = "stream_stalled"
                    print("  state: interaction_required; byte stream stalled", flush=True)
                    break
            read_available(0.2)
    finally:
        terminate_process(proc, master_fd, str(config.get("quit", "")))
        read_available(0)
        selector.close()
        os.close(master_fd)
        raw_log.close()
        stream_log.close()
        action_log.close()

    meta = {
        "argv": argv,
        "duration_seconds": round(time.monotonic() - started, 3),
        "exit_code": proc.returncode,
        "termination_reason": reason,
        "timed_out": reason == "timeout",
        "startup_ready": startup_ready,
        "task_submitted": task_submitted,
        "turns_used": turns_used,
        "max_turns": max_turns,
        "valid": task_submitted and reason in ("completion_marker", "process_exit"),
        "stream_watch_seconds": stream_watch,
        "stream_events": len(stream_trace),
        "stream_trace": stream_trace,
        "logic_actions": action_trace,
    }
    return b"".join(chunks), meta


# ---------------------------------------------------------------------------
# Driver tmux — leggere lo SCHERMO, non il flusso
#
# gen502.  La corsa `gen501-pilot-b` diceva che freebuff «non lavora»: timeout,
# 738 eventi di stream, cartella intatta.  Rigiocando `raw.log` dentro un pane
# tmux si vede invece che freebuff aveva ricevuto il task INTERO, letto sette
# file e stava ragionando sul budget di profondita' dell'introsort al secondo
# 43 — con il timeout a 45.  Non era rotto: eravamo ciechi.
#
# Ciechi per una ragione precisa e strutturale: `clean_transcript` concatena i
# byte, ma un TUI a schermo alternato non aggiunge byte, RIDISEGNA.  Su 591 KB
# di `raw.log` la riga di prompt compare tre volte, tutte nei primi 92 KB; il
# transcript «pulito» risultante e' 169 KB con un solo glifo visibile.  Nessun
# pattern poteva funzionare la' dentro.
#
# Quindi non scriviamo un emulatore di terminale: ne usiamo uno installato.
# `capture-pane -p` da' lo schermo renderizzato, e `pipe-pane` conserva lo
# stesso `raw.log` lossless di prima.  Il PTY resta il driver di default: a
# parrot0, che e' un REPL di riga, non serve niente di tutto questo.
#
# In regalo, due cose che servivano davvero: `tmux attach` per guardare una
# gara mentre corre, e un segnale di quiete migliore dei byte — lo schermo di
# freebuff ha un cronometro che ticchetta MENTRE lavora e si ferma quando ha
# finito, quindi «schermo immobile» e' un'evidenza di fine, non un'ipotesi.
# ---------------------------------------------------------------------------

TMUX_BIN = shutil.which("tmux")


def tmux_available() -> bool:
    return TMUX_BIN is not None


def _tmux(socket: str, *args: str, env: dict[str, str] | None = None,
          check: bool = False) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [str(TMUX_BIN), "-L", socket, *args],
        text=True, capture_output=True, timeout=15, check=check,
        env=env if env is not None else os.environ.copy(),
    )


def _capture(socket: str, session: str) -> str | None:
    """Lo schermo visibile, come lo vedrebbe un umano.  None se il pane e' morto."""
    result = _tmux(socket, "capture-pane", "-p", "-t", session)
    if result.returncode != 0:
        return None
    return "\n".join(line.rstrip() for line in result.stdout.split("\n")).strip("\n")


def _pane_alive(socket: str, session: str) -> bool:
    result = _tmux(socket, "list-panes", "-t", session, "-F", "#{pane_dead}")
    if result.returncode != 0:
        return False
    return result.stdout.strip().splitlines()[:1] != ["1"]


def tmux_session(
    argv: list[str],
    cwd: Path,
    env: dict[str, str],
    prompt: str,
    config: dict[str, Any],
    timeout: float,
    trace_dir: Path,
) -> tuple[bytes, dict[str, Any]]:
    """Come `pty_session`, ma lo stato si legge da `capture-pane`.

    La forma di `meta` e' identica di proposito: il resto del banco (archivio,
    judge, scoreboard, validita') non deve sapere quale driver ha corso.
    """
    if not tmux_available():
        raise ChallengeError("driver tmux richiesto ma tmux non e' installato")
    trace_dir.mkdir(parents=True, exist_ok=True)
    raw_path = trace_dir / "raw.log"
    raw_path.write_bytes(b"")
    screens_log = (trace_dir / "screens.jsonl").open("w", encoding="utf-8")
    action_log = (trace_dir / "logic-actions.jsonl").open("w", encoding="utf-8")
    # «Il transcript e' la parte preziosa» (§4.6) — ma per un TUI il transcript
    # NON e' la concatenazione dei byte: e' la successione delle schermate.  Qui
    # ogni schermata distinta viene archiviata per intero, con un tetto in byte
    # perche' una gara lunga non riempia il disco.  I digest in `screens.jsonl`
    # restano completi anche dopo il tetto, cosi' si sa sempre quante ne mancano.
    frames_log = (trace_dir / "screens.log").open("w", encoding="utf-8")
    frames_budget = int(config.get("frames_budget_bytes", 8_000_000))
    frames_written = 0
    frames_dropped = 0

    socket = f"challenge-{os.getpid()}-{int(time.time())}"
    session = "match"
    cols = int(config.get("cols", 160))
    rows = int(config.get("rows", 45))
    ready_pattern = config.get("ready_pattern", "")
    done_pattern = config.get("done_pattern", "")
    boot_wait = float(config.get("boot_wait_seconds", 2.0))
    min_runtime = float(config.get("min_runtime_seconds", 4.0))
    # Con lo schermo renderizzato «fermo» smette di essere un'ipotesi sui byte:
    # e' lo schermo che non cambia.  Il nome resta per compatibilita' del result.
    stream_watch = float(config.get("stream_watch_seconds", 3.0))
    idle_settle = float(config.get("idle_settle_seconds", 1.5))
    max_turns = int(config.get("max_turns", 1))
    continue_text = str(config.get("continue_text", ""))
    poll = float(config.get("poll_seconds", 0.25))

    started = time.monotonic()
    turns_used = 0
    reason = "process_exit"
    startup_ready = False
    task_submitted = False
    action_uses: dict[str, int] = {}
    action_trace: list[dict[str, Any]] = []
    screen_trace: list[dict[str, Any]] = []
    last_change = started
    last_digest = ""
    visible = ""

    def note_screen(current: str) -> bool:
        """Registra lo schermo; ritorna True se e' cambiato da quello prima."""
        nonlocal last_change, last_digest, visible, frames_written, frames_dropped
        visible = current
        digest = hashlib.sha256(current.encode()).hexdigest()
        changed = digest != last_digest
        if changed:
            last_change = time.monotonic()
            last_digest = digest
            at = round(last_change - started, 3)
            event = {"at_seconds": at, "sha256": digest, "chars": len(current)}
            screen_trace.append(event)
            screens_log.write(json.dumps(event) + "\n")
            screens_log.flush()
            frame = f"\n===== frame {len(screen_trace)} @ {at}s {digest[:12]} =====\n{current}\n"
            if frames_written + len(frame) <= frames_budget:
                frames_log.write(frame)
                frames_log.flush()
                frames_written += len(frame)
            else:
                frames_dropped += 1
        return changed

    def send_literal(text: str) -> None:
        _tmux(socket, "send-keys", "-t", session, "-l", "--", text)

    def send_key(name: str) -> None:
        _tmux(socket, "send-keys", "-t", session, name)

    def fire_logic_actions(phase: str, screen: str) -> bool:
        nonlocal last_change
        fired = False
        for rule in config.get("logic_actions", []):
            if rule.get("phase", "run") not in (phase, "any"):
                continue
            action_id = str(rule.get("id", "unnamed_action"))
            used = action_uses.get(action_id, 0)
            if used >= int(rule.get("max_uses", 1)):
                continue
            literal = str(rule.get("when", ""))
            regex = str(rule.get("when_regex", ""))
            matched = bool(literal and literal in screen)
            if regex:
                matched = re.search(
                    regex, screen, re.IGNORECASE | re.MULTILINE | re.DOTALL
                ) is not None
            if not matched:
                continue
            sent = ""
            if "send_text" in rule:
                sent = str(rule["send_text"])
                send_literal(sent)
                if rule.get("submit", True):
                    send_key("Enter")
            key = str(rule.get("send_key", "")).upper()
            key_name = {"ENTER": "Enter", "ESC": "Escape", "CTRL_C": "C-c"}.get(key)
            if key_name:
                send_key(key_name)
                sent = f"<{key}>"
            action_uses[action_id] = used + 1
            last_change = time.monotonic()
            event = {
                "at_seconds": round(last_change - started, 3),
                "phase": phase,
                "state_match": literal or regex,
                "action": action_id,
                "sent": sent,
                "visible_sha256": hashlib.sha256(screen.encode()).hexdigest(),
            }
            action_trace.append(event)
            action_log.write(json.dumps(event, ensure_ascii=False) + "\n")
            action_log.flush()
            print(f"  logic-action: {phase}/{action_id} -> {sent}", flush=True)
            fired = True
        return fired

    exit_code: int | None = None
    try:
        declared = tuple(config.get("_declared_env", ()))
        create = _tmux(
            socket, "new-session", "-d", "-s", session,
            "-x", str(cols), "-y", str(rows), "-c", str(cwd),
            *[arg for key in declared if key in env for arg in ("-e", f"{key}={env[key]}")],
            "--", *argv,
            env=env,
        )
        if create.returncode != 0:
            raise ChallengeError(f"tmux new-session failed: {create.stderr.strip()}")
        # `remain-on-exit` tiene il pane in vita dopo la fine del processo, cosi'
        # l'ULTIMO schermo — quello che di solito spiega tutto — non evapora.
        _tmux(socket, "set-option", "-t", session, "remain-on-exit", "on")
        _tmux(socket, "pipe-pane", "-o", "-t", session, f"cat >> {raw_path}")

        while _pane_alive(socket, session):
            time.sleep(poll)
            current = _capture(socket, session)
            if current is None:
                break
            note_screen(current)
            elapsed = time.monotonic() - started
            fire_logic_actions("startup", current)
            if ready_pattern and ready_pattern in current:
                startup_ready = True
                print("  state: task_ready", flush=True)
                break
            if not ready_pattern and elapsed >= boot_wait:
                startup_ready = True
                print("  state: task_ready (settled)", flush=True)
                break
            if time.monotonic() - last_change >= stream_watch:
                reason = "screen_stalled_startup"
                print("  state: interaction_required; screen frozen", flush=True)
                break
            if elapsed >= min(timeout, float(config.get("startup_timeout_seconds", 60))):
                reason = "startup_timeout"
                break

        if not _pane_alive(socket, session):
            reason = "startup_exit"
        elif startup_ready:
            send_literal(prompt)
            submit_delay = float(config.get("submit_delay_seconds", 0.15))
            time.sleep(submit_delay)
            send_key("Enter")
            prompt_sent_at = time.monotonic()
            last_change = prompt_sent_at
            task_submitted = True
            turns_used = 1
            print("  state: working; task submitted", flush=True)
            while _pane_alive(socket, session):
                time.sleep(poll)
                current = _capture(socket, session)
                if current is None:
                    break
                note_screen(current)
                now = time.monotonic()
                active = now - prompt_sent_at
                fire_logic_actions("run", current)
                if active >= timeout:
                    reason = "timeout"
                    break
                idle = now - last_change
                at_prompt = bool(done_pattern) and done_pattern in current
                if at_prompt and active >= min_runtime and idle >= idle_settle:
                    if turns_used < max_turns and continue_text:
                        turns_used += 1
                        send_literal(continue_text)
                        send_key("Enter")
                        last_change = time.monotonic()
                        print(f"  state: continuing (turn {turns_used})", flush=True)
                        continue
                    reason = "completion_marker"
                    print("  state: completion", flush=True)
                    break
                if idle >= stream_watch and not at_prompt:
                    reason = "screen_stalled"
                    print("  state: interaction_required; screen frozen", flush=True)
                    break
            final = _capture(socket, session)
            if final is not None:
                note_screen(final)
    finally:
        quit_text = str(config.get("quit", ""))
        if quit_text and _pane_alive(socket, session):
            if quit_text in ("\u0003", "\x03"):
                send_key("C-c")
            else:
                send_literal(quit_text.rstrip("\r"))
                send_key("Enter")
            time.sleep(0.6)
            final = _capture(socket, session)
            if final is not None:
                note_screen(final)
        codes = _tmux(socket, "list-panes", "-t", session, "-F", "#{pane_dead_status}")
        if codes.returncode == 0 and codes.stdout.strip():
            try:
                exit_code = int(codes.stdout.strip().splitlines()[0])
            except ValueError:
                exit_code = None
        _tmux(socket, "kill-server")
        # `kill-server` ferma il server ma lascia il file di socket: senza questo
        # /tmp/tmux-$UID si riempie di sessioni morte, una per corsa.
        try:
            Path(f"/tmp/tmux-{os.getuid()}/{socket}").unlink(missing_ok=True)
        except OSError:
            pass
        screens_log.close()
        if frames_dropped:
            frames_log.write(
                f"\n===== {frames_dropped} schermate successive non archiviate "
                f"(tetto {frames_budget} byte); i digest restano in screens.jsonl =====\n"
            )
        frames_log.close()

    # Lo schermo finale e' l'artefatto piu' utile del driver: e' cio' che un
    # umano avrebbe visto guardando la gara nell'istante in cui e' finita.
    (trace_dir / "screen.txt").write_text(visible + "\n", encoding="utf-8")
    raw = raw_path.read_bytes() if raw_path.is_file() else b""

    meta = {
        "argv": argv,
        "driver": "tmux",
        "duration_seconds": round(time.monotonic() - started, 3),
        "exit_code": exit_code,
        "termination_reason": reason,
        "timed_out": reason == "timeout",
        "startup_ready": startup_ready,
        "task_submitted": task_submitted,
        "turns_used": turns_used,
        "max_turns": max_turns,
        "valid": task_submitted and reason in ("completion_marker", "process_exit"),
        "stream_watch_seconds": stream_watch,
        "stream_events": len(screen_trace),
        "stream_trace": screen_trace,
        "logic_actions": action_trace,
        "screen_rows": rows,
        "screen_cols": cols,
        "final_screen_sha256": hashlib.sha256(visible.encode()).hexdigest(),
        "frames_archived": len(screen_trace) - frames_dropped,
        "frames_dropped": frames_dropped,
    }
    return raw, meta


def open_session(
    argv: list[str],
    cwd: Path,
    env: dict[str, str],
    prompt: str,
    config: dict[str, Any],
    timeout: float,
    trace_dir: Path,
) -> tuple[bytes, dict[str, Any]]:
    """Sceglie il driver dichiarato dall'agente.  PTY resta il default."""
    driver = str(config.get("driver", "pty"))
    if driver == "tmux":
        return tmux_session(argv, cwd, env, prompt, config, timeout, trace_dir)
    if driver != "pty":
        raise ChallengeError(f"unknown agent driver: {driver!r}")
    raw, meta = pty_session(argv, cwd, env, prompt, config, timeout, trace_dir)
    meta["driver"] = "pty"
    return raw, meta


def run_version(argv: list[str], env: dict[str, str]) -> str:
    try:
        result = subprocess.run(argv, env=env, text=True, capture_output=True, timeout=8)
    except (OSError, subprocess.TimeoutExpired) as exc:
        return f"unavailable: {exc}"
    text = (result.stdout + result.stderr).strip()
    return text.splitlines()[0][:240] if text else f"exit {result.returncode}"


def judge(match_dir: Path, archive_code: Path) -> dict[str, Any]:
    judge_path = match_dir / "judge.py"
    result = subprocess.run(
        [sys.executable, str(judge_path), str(archive_code)],
        cwd=match_dir,
        text=True,
        capture_output=True,
        timeout=45,
    )
    lines = [line for line in result.stdout.splitlines() if line.strip()]
    try:
        report = json.loads(lines[-1]) if lines else {}
    except json.JSONDecodeError as exc:
        raise ChallengeError(f"judge {judge_path} emitted invalid JSON: {result.stdout}") from exc
    if not isinstance(report, dict) or "score" not in report:
        raise ChallengeError(
            f"judge {judge_path} returned no score (exit {result.returncode}): {result.stderr}"
        )
    report["judge_exit_code"] = result.returncode
    if result.stderr.strip():
        report["judge_stderr"] = result.stderr.strip()[-2000:]
    return report


def validate(league: dict[str, Any]) -> list[dict[str, Any]]:
    required_agents = league.get("agents")
    if not isinstance(required_agents, dict) or len(required_agents) != 2:
        raise ChallengeError("league.json must define exactly two agents")
    match_ids = league.get("matches")
    if not isinstance(match_ids, list) or not match_ids:
        raise ChallengeError("league.json must define a non-empty matches list")
    matches: list[dict[str, Any]] = []
    for match_id in match_ids:
        if not isinstance(match_id, str) or not SAFE_ID_RE.match(match_id):
            raise ChallengeError(f"unsafe match id: {match_id!r}")
        match_dir = BASE / "tasks" / match_id
        manifest = load_json(match_dir / "match.json")
        if manifest.get("id") != match_id:
            raise ChallengeError(f"id mismatch in {match_dir / 'match.json'}")
        for filename in ("task.md", "judge.py"):
            if not (match_dir / filename).is_file():
                raise ChallengeError(f"missing {match_dir / filename}")
        prompt = flatten_prompt((match_dir / "task.md").read_text(encoding="utf-8"))
        if not prompt or len(prompt.encode()) > 16000:
            raise ChallengeError(f"prompt for {match_id} is empty or over 16 KiB")
        order = manifest.get("order")
        if sorted(order or []) != sorted(required_agents):
            raise ChallengeError(f"{match_id} order must name each configured agent once")
        manifest["_dir"] = match_dir
        manifest["_prompt"] = prompt
        matches.append(manifest)
    return matches


def agent_argv_env(
    agent: dict[str, Any], code: Path, runtime: Path
) -> tuple[list[str], dict[str, str], dict[str, str]]:
    variables = {
        "repo": str(ROOT),
        "code": str(code),
        "runtime": str(runtime),
    }
    argv = [expand(str(arg), variables) for arg in agent.get("argv", [])]
    if not argv:
        raise ChallengeError("agent argv cannot be empty")
    argv = resolve_argv0(agent, argv, variables)
    env = dict(os.environ)
    configured_env = agent.get("env", {})
    if not isinstance(configured_env, dict):
        raise ChallengeError("agent env must be an object")
    env.update({str(key): expand(str(value), variables) for key, value in configured_env.items()})
    # Avoid inherited state making either run secretly non-fresh.
    for key in agent.get("unset_env", []):
        env.pop(str(key), None)
    version_argv = [expand(str(arg), variables) for arg in agent.get("version_argv", argv[:1])]
    return argv, env, {"version": run_version(version_argv, env)}


# ---------------------------------------------------------------------------
# Preflight — lo stato si legge dai FILE, non si negozia col TUI
#
# §3-bis.2.  La `logic_action` che mandava ENTER sul selettore del modello
# indovinava dallo schermo uno stato che sta scritto in un file di config: nel
# migliore dei casi inutile, nel peggiore un invio dove non serve.  Un controllo
# che fallisce PRIMA di avviare vale piu' di venti azioni che tirano a indovinare.
#
# Le due cose che il preflight compra:
#  - una gara non parte se il login manca o il modello non e' quello dichiarato;
#  - il modello finisce nel `result.json`.  Finora la league si chiamava
#    `freebuff-deepseek-flash` e nessuno verificava che fosse davvero quello:
#    un confronto che non sa contro CHI ha gareggiato non e' un confronto.
#
# I controlli sono dichiarati nella league, non qui: il pilota non deve sapere
# dove freebuff tiene le sue cose.  `must_exist` non legge mai il contenuto —
# per le credenziali l'esistenza e' tutto cio' che serve, e tutto cio' che e'
# lecito guardare.
# ---------------------------------------------------------------------------


def preflight_check(rule: dict[str, Any], variables: dict[str, str]) -> dict[str, Any]:
    check_id = str(rule.get("id", "unnamed_check"))
    path = Path(os.path.expanduser(expand(str(rule.get("file", "")), variables)))
    report: dict[str, Any] = {"id": check_id, "file": str(path), "passed": True}
    if not path.exists():
        report.update(passed=not rule.get("must_exist", True), detail="file assente")
        return report
    # gen502 — un binario piu' vecchio dei suoi sorgenti e' l'inganno peggiore
    # del banco: la gara SEMBRA regolare e misura un altro programma.  Nella
    # corsa di riferimento `gen501-pilot-b` correva un `bin/parrot0` di gen459
    # con 13519 fatti mentre il repo era a gen501 con 24692 — meta' della
    # conoscenza in meno, e `version_argv` diceva comunque `git describe`, cioe'
    # la versione del REPOSITORY, non quella del programma avviato.
    glob_pattern = rule.get("newer_than_glob")
    if glob_pattern:
        sources = sorted(Path("/").glob(
            expand(str(glob_pattern), variables).lstrip("/")
        ))
        newest = max((src.stat().st_mtime for src in sources), default=0.0)
        stale = path.stat().st_mtime < newest
        report.update(
            passed=not stale,
            detail=("piu' vecchio dei sorgenti: ricompilare" if stale
                    else f"aggiornato rispetto a {len(sources)} sorgenti"),
        )
        return report
    key = rule.get("json_key")
    if key is None:
        report["detail"] = "presente"
        return report
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        report.update(passed=False, detail=f"JSON illeggibile: {exc}")
        return report
    observed = data.get(str(key)) if isinstance(data, dict) else None
    report["observed"] = observed
    expected = rule.get("equals")
    if expected is not None and observed != expected:
        report.update(passed=False, detail=f"atteso {expected!r}, trovato {observed!r}")
    else:
        report["detail"] = f"{key}={observed!r}"
    return report


def run_preflight(
    agent_id: str, agent: dict[str, Any], variables: dict[str, str]
) -> dict[str, Any]:
    rules = agent.get("preflight", [])
    if not isinstance(rules, list):
        raise ChallengeError(f"{agent_id}: preflight must be a list")
    checks = [preflight_check(rule, variables) for rule in rules]
    recorded = {
        str(rule["record_as"]): checks[index].get("observed")
        for index, rule in enumerate(rules)
        if rule.get("record_as")
    }
    # gen502 — un pin puo' essere DICHIARATO senza fermare la gara. Il modello
    # osservato finisce comunque nel result.json: cio' che non deve succedere e'
    # correre senza sapere contro chi, non correre contro un altro.
    failed = [
        check["id"] for index, check in enumerate(checks)
        if not check["passed"] and not rules[index].get("warn_only", False)
    ]
    for check in checks:
        mark = "ok  " if check["passed"] else "FAIL"
        print(f"  preflight {mark} {agent_id}/{check['id']}: {check.get('detail', '')}", flush=True)
    return {"checks": checks, "recorded": recorded, "failed": failed}


def resolve_argv0(agent: dict[str, Any], argv: list[str], variables: dict[str, str]) -> list[str]:
    """`FREE_BUFF_BIN` -> config -> PATH, dichiarato invece che indovinato."""
    candidates = agent.get("argv0_search")
    if not candidates:
        return argv
    for candidate in candidates:
        text = os.path.expanduser(os.path.expandvars(expand(str(candidate), variables)))
        if not text or "$" in text:
            continue
        found = str(Path(text)) if Path(text).is_file() else shutil.which(text)
        if found:
            return [found, *argv[1:]]
    return argv


def diagnostic_lines(reports: dict[str, dict[str, Any]], agents: list[str]) -> list[str]:
    left, right = agents
    lchecks = {c["name"]: bool(c["passed"]) for c in reports[left].get("checks", [])}
    rchecks = {c["name"]: bool(c["passed"]) for c in reports[right].get("checks", [])}
    left_only = sorted(name for name in rchecks if rchecks[name] and not lchecks.get(name, False))
    right_only = sorted(name for name in lchecks if lchecks[name] and not rchecks.get(name, False))
    shared = sorted(name for name in lchecks if not lchecks[name] and not rchecks.get(name, False))
    return [
        f"- Evidenza discriminante a sfavore di `{left}`: " + (", ".join(left_only) or "nessuna"),
        f"- Evidenza discriminante a sfavore di `{right}`: " + (", ".join(right_only) or "nessuna"),
        "- Fallimenti condivisi (possibile difficoltà comune o contratto da riesaminare): "
        + (", ".join(shared) or "nessuno"),
    ]


def record_is_valid(record: dict[str, Any], agents: list[str]) -> bool:
    if record.get("valid") is False:
        return False
    reports = record.get("agents", {})
    if any(agent not in reports for agent in agents):
        return False
    # Old controller runs did not record task_submitted.  They cannot establish
    # that both agents ever received the task and must not enter the standings.
    return all(reports[a].get("session", {}).get("task_submitted") is True
               for a in agents)


def write_analysis(path: Path, record: dict[str, Any], agents: list[str]) -> None:
    reports = record["agents"]
    if not record_is_valid(record, agents):
        lines = [
            f"# Gara annullata — {record['match_id']} / {record['run_id']}",
            "",
            "Nessun vincitore e nessun punto: il controller non ha potuto provare che",
            "entrambi gli agenti abbiano ricevuto e completato lo stesso task.",
            "",
            f"Motivo: `{record.get('cancellation', 'legacy run without task-ready evidence')}`.",
            "",
            "## Tracce disponibili",
            "",
        ]
        for agent, data in reports.items():
            session = data["session"]
            lines.append(
                f"- `{agent}`: `{session['termination_reason']}`, "
                f"task_submitted={session.get('task_submitted', 'unknown')}, "
                f"`./{agent}/raw.log`, `./{agent}/stream.jsonl`, "
                f"`./{agent}/logic-actions.jsonl`, `./{agent}/code/`."
            )
        path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        return
    scores = {agent: reports[agent]["judge"]["score"] for agent in agents}
    winner = "tie" if len(set(scores.values())) == 1 else max(scores, key=scores.get)
    lines = [
        f"# Diagnosi {record['match_id']} — run {record['run_id']}",
        "",
        f"Vincitore verificato: **{winner}**. Punteggi: "
        + ", ".join(f"{agent} {scores[agent]}/100" for agent in agents)
        + ".",
        "",
        "Questa diagnosi confronta soltanto esiti osservabili. Il transcript conserva il",
        "ragionamento reso visibile dalla CLI, non pensieri interni non esposti.",
        "",
        *diagnostic_lines({a: reports[a]["judge"] for a in agents}, agents),
        "",
        "## Artefatti",
        "",
    ]
    for agent in agents:
        data = reports[agent]
        lines.append(
            f"- `{agent}`: `./{agent}/transcript.txt`, `./{agent}/raw.log`, "
            f"`./{agent}/code/` — {data['session']['termination_reason']}, "
            f"{data['session']['duration_seconds']} s."
        )
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def collect_records(output_root: Path) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    if not output_root.exists():
        return records
    for path in sorted(output_root.glob("*/runs/*/result.json")):
        try:
            records.append(load_json(path))
        except ChallengeError:
            continue
    return records


def render_scoreboard(league: dict[str, Any], output_root: Path) -> None:
    agents = list(league["agents"])
    records = collect_records(output_root)
    latest: dict[str, dict[str, Any]] = {}
    for record in records:
        if record_is_valid(record, agents):
            latest[record["match_id"]] = record
    totals = {agent: 0 for agent in agents}
    wins = {agent: 0 for agent in agents}
    rows: list[str] = []
    for match_id in league["matches"]:
        record = latest.get(match_id)
        if not record:
            rows.append(f"| {match_id} | — | — | — | — | non disputato |")
            continue
        scores = {agent: int(record["agents"][agent]["judge"]["score"]) for agent in agents}
        for agent in agents:
            totals[agent] += scores[agent]
        winner = "pareggio" if len(set(scores.values())) == 1 else max(scores, key=scores.get)
        if winner != "pareggio":
            wins[winner] += 1
        parrot_report = record["agents"].get("parrot0", {}).get("judge", {})
        gaps = [c["name"] for c in parrot_report.get("checks", []) if not c.get("passed")]
        rows.append(
            f"| {match_id} | {record['run_id']} | {scores[agents[0]]} | "
            f"{scores[agents[1]]} | {winner} | {', '.join(gaps) or 'nessuno'} |"
        )
    history = []
    for record in records:
        valid = record_is_valid(record, agents)
        scores = [
            str(record["agents"].get(a, {}).get("judge", {}).get("score", "—"))
            if valid else "annullato"
            for a in agents
        ]
        history.append(
            f"| {record['run_id']} | {record['match_id']} | {' | '.join(scores)} | "
            f"[{record['match_id']}/runs/{record['run_id']}/analysis.md]"
            f"({record['match_id']}/runs/{record['run_id']}/analysis.md) |"
        )
    now = dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds")
    content = [
        f"# Scoreboard — {league['title']}",
        "",
        f"Aggiornata: `{now}`. Conta l’ultimo run completo di ogni match; lo storico non viene sovrascritto.",
        "",
        f"| agente | punti | vittorie |",
        "|---|---:|---:|",
        *[f"| {a} | {totals[a]} | {wins[a]} |" for a in agents],
        "",
        f"| match | run | {agents[0]} | {agents[1]} | vincitore | gap osservati di parrot0 |",
        "|---|---|---:|---:|---|---|",
        *rows,
        "",
        "## Storico completo",
        "",
        f"| run | match | {agents[0]} | {agents[1]} | diagnosi |",
        "|---|---|---:|---:|---|",
        *(history or ["| — | — | — | — | nessuna gara eseguita |"]),
        "",
        "I punti provengono dai judge deterministici del match. Tempo e stile del transcript",
        "restano evidenza diagnostica e non modificano il punteggio di correttezza.",
        "",
    ]
    (output_root / "scoreboard.md").write_text("\n".join(content), encoding="utf-8")


def run_match(
    league: dict[str, Any],
    match: dict[str, Any],
    run_id: str,
    timeout_override: float | None,
) -> dict[str, Any]:
    match_id = match["id"]
    match_dir: Path = match["_dir"]
    output_root = BASE / league["id"]
    run_dir = output_root / match_id / "runs" / run_id
    if run_dir.exists():
        raise ChallengeError(f"run already exists: {run_dir}")
    run_dir.mkdir(parents=True)
    prompt = match["_prompt"]
    (run_dir / "prompt.md").write_text(prompt + "\n", encoding="utf-8")
    timeout = float(timeout_override or match.get("timeout_seconds", 600))
    agents = list(league["agents"])
    record: dict[str, Any] = {
        "league_id": league["id"],
        "match_id": match_id,
        "run_id": run_id,
        "difficulty": match.get("difficulty"),
        "order": match["order"],
        "prompt_sha256": hashlib.sha256(prompt.encode()).hexdigest(),
        "valid": True,
        "status": "running",
        "agents": {},
    }

    with tempfile.TemporaryDirectory(prefix="coding-match-") as tmp_text:
        neutral_root = Path(tmp_text)
        code = neutral_root / "code"
        runtime = neutral_root / "runtime"
        runtime.mkdir()
        seed = match_dir / "seed"
        expected_seed_digest = tree_digest(seed)

        for agent_id in match["order"]:
            copy_seed(seed, code)
            if tree_digest(code) != expected_seed_digest:
                raise ChallengeError(f"failed to reproduce neutral seed for {match_id}")
            shutil.rmtree(runtime)
            runtime.mkdir()
            agent_cfg = league["agents"][agent_id]
            argv, env, version = agent_argv_env(agent_cfg, code, runtime)
            if not command_exists(argv[0]):
                raise ChallengeError(f"agent executable not found: {argv[0]}")
            preflight = run_preflight(
                agent_id, agent_cfg,
                {"repo": str(ROOT), "code": str(code), "runtime": str(runtime)},
            )
            if preflight["failed"]:
                raise ChallengeError(
                    f"{agent_id}: preflight fallito ({', '.join(preflight['failed'])}); "
                    "nessun agente avviato"
                )
            print(
                f"{match_id}: running {agent_id} in position "
                f"{match['order'].index(agent_id) + 1}", flush=True
            )
            session_cfg = dict(agent_cfg)
            match_actions = match.get("logic_actions", {})
            if isinstance(match_actions, dict):
                scoped_actions = [
                    *match_actions.get("all", []),
                    *match_actions.get(agent_id, []),
                ]
            elif isinstance(match_actions, list):
                scoped_actions = match_actions
            else:
                raise ChallengeError(f"{match_id} logic_actions must be an object or list")
            session_cfg["logic_actions"] = [
                *agent_cfg.get("logic_actions", []),
                *scoped_actions,
            ]
            agent_dir = run_dir / agent_id
            archive_code = agent_dir / "code"
            agent_dir.mkdir()
            session_cfg["_declared_env"] = tuple(agent_cfg.get("env", {}))
            raw, session = open_session(
                argv, code, env, prompt, session_cfg, timeout, agent_dir
            )
            # Per un TUI la concatenazione dei byte non e' leggibile (169 KB con
            # un solo glifo, in `gen501-pilot-b`): il transcript e' lo schermo.
            transcript = (
                (agent_dir / "screen.txt").read_text(encoding="utf-8")
                if session.get("driver") == "tmux" and (agent_dir / "screen.txt").is_file()
                else clean_transcript(raw)
            )
            (agent_dir / "transcript.txt").write_text(transcript, encoding="utf-8")
            session.pop("stream_trace")
            session.pop("logic_actions")
            shutil.copytree(code, archive_code)
            report = judge(match_dir, archive_code)
            artifact = archive_code / str(match["artifact"])
            record["agents"][agent_id] = {
                **version,
                **preflight["recorded"],
                "preflight": preflight["checks"],
                "session": session,
                "seed_sha256": expected_seed_digest,
                "result_tree_sha256": tree_digest(archive_code),
                "artifact_sha256": sha256_file(artifact) if artifact.is_file() else None,
                "judge": report,
            }
            # Stable latest-transcript paths are convenient for manual comparison;
            # the authoritative historical copy above is never overwritten.
            shutil.copy2(agent_dir / "transcript.txt", output_root / match_id / f"{agent_id}.log")
            if not session["valid"]:
                record["valid"] = False
                record["status"] = "cancelled"
                record["cancellation"] = (
                    f"{agent_id}:{session['termination_reason']} after "
                    f"{session['stream_events']} stream events"
                )
                print(f"{match_id}: race cancelled — {record['cancellation']}", flush=True)
                break

    if record["valid"] and all(agent in record["agents"] for agent in agents):
        record["status"] = "complete"
    else:
        record["valid"] = False
        record["status"] = "cancelled"

    (run_dir / "result.json").write_text(
        json.dumps(record, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    write_analysis(run_dir / "analysis.md", record, agents)
    render_scoreboard(league, output_root)
    return record


def print_check(league: dict[str, Any], matches: list[dict[str, Any]]) -> None:
    variables = {"repo": str(ROOT), "code": "/tmp/neutral/code", "runtime": "/tmp/neutral/runtime"}
    print(f"league: {league['id']} ({league['title']})")
    for agent_id, cfg in league["agents"].items():
        argv0 = resolve_argv0(cfg, [expand(str(cfg["argv"][0]), variables)], variables)[0]
        state = "found" if command_exists(argv0) else "MISSING"
        driver = str(cfg.get("driver", "pty"))
        print(f"agent:  {agent_id:9s} {state:7s} driver={driver:5s} {argv0}")
        run_preflight(agent_id, cfg, variables)
    for match in matches:
        seed = match["_dir"] / "seed"
        print(
            f"match:  {match['id']:9s} difficulty={match.get('difficulty')} "
            f"artifact={match['artifact']} order={','.join(match['order'])} "
            f"seed={tree_digest(seed)[:12]} prompt={len(match['_prompt'].encode())}B"
        )
    print("check only: no agent was started and no archived result was changed")


# ---------------------------------------------------------------------------
# Cricchetto del pilota (`selftest`)
#
# CHALLENGE_TODO C6: senza questo, ogni ipotesi sul pilota costava una corsa
# vera — minuti, e una sessione di un agente reale.  Qui le stesse ipotesi si
# falsificano in secondi contro `fake_agent.py`, che riproduce le forme di
# comportamento osservate sul campo.  Un pilota che non passa il selftest non
# va portato su una gara vera.
# ---------------------------------------------------------------------------

FAKE_AGENT = HERE / "fake_agent.py"

SELFTEST_PROMPT = flatten_prompt(
    "Implement a stable three-way quicksort in quicksort.c against the supplied "
    "quicksort.h, wire it into the Makefile and keep the build clean under -Werror. "
    * 8
)

# Tempi stretti di proposito: il selftest deve stare sotto il minuto, altrimenti
# smette di essere usato dentro il ciclo di modifica.
FAST = {
    "boot_wait_seconds": 1.0,
    "min_runtime_seconds": 0.2,
    "stream_watch_seconds": 2.0,
    "startup_timeout_seconds": 8,
    "idle_settle_seconds": 0.4,
}


def _fake_config(mode: str, **overrides: Any) -> dict[str, Any]:
    config = dict(FAST)
    config.update(overrides)
    config["_mode"] = mode
    return config


SELFTEST_CASES: dict[str, dict[str, Any]] = {
    # L'agente REPL: avvio, submit, turni, completamento.
    "repl_turns": {
        "config": _fake_config(
            "repl", ready_pattern=">>>", done_pattern=">>>", quit="/quit\r",
            max_turns=3, continue_text="continue",
        ),
        "expect": {
            "startup_ready": True, "task_submitted": True,
            "turns_used": 3, "termination_reason": "completion_marker", "valid": True,
        },
        "expect_visible": ["TASK-RECEIVED", "turn 3 done"],
    },
    # Un turno solo: `max_turns` non deve gonfiare da solo.
    "repl_single_turn": {
        "config": _fake_config(
            "repl", ready_pattern=">>>", done_pattern=">>>", quit="/quit\r", max_turns=1,
        ),
        "expect": {"turns_used": 1, "termination_reason": "completion_marker", "valid": True},
    },
    # Muto all'avvio: e' un blocco, e va detto.
    "silent_startup": {
        "config": _fake_config("silent", ready_pattern=">>>", done_pattern=">>>"),
        "expect": {
            "startup_ready": False, "task_submitted": False,
            "termination_reason": "stream_stalled_startup", "valid": False,
        },
    },
    # Esce prima di partire.
    "startup_exit": {
        "config": _fake_config("exit", ready_pattern=">>>", done_pattern=">>>"),
        "expect": {"task_submitted": False, "termination_reason": "startup_exit", "valid": False},
    },
    # ⭐ §3-bis ipotesi 1, resa falsificabile: un TUI che CAPISCE il bracketed
    # paste riceve il task.
    "tui_bracketed_ok": {
        "config": _fake_config(
            "tui", ready_pattern="Enter a coding task or / for commands",
            done_pattern="Enter a coding task or / for commands",
            bracketed_paste=True, submit_delay_seconds=0.2, quit="\u0003", max_turns=1,
        ),
        "expect": {"task_submitted": True},
        "expect_visible": ["TASK-RECEIVED"],
    },
    # ⭐ Lo stesso TUI che NON lo capisce: il testo entra, l'invio non parte, lo
    # schermo ridisegna, il task non arriva mai.  E' la forma del sintomo vero.
    "tui_bracketed_broken": {
        "config": _fake_config(
            "tui_nobracket", ready_pattern="Enter a coding task or / for commands",
            done_pattern="Enter a coding task or / for commands",
            bracketed_paste=True, submit_delay_seconds=0.2, quit="\u0003", max_turns=1,
        ),
        "expect": {"task_submitted": True},
        "expect_missing": ["TASK-RECEIVED"],
    },
    # ⭐ E la cura proposta dal §3-bis: submit nudo, e lo stesso TUI riceve.
    "tui_plain_submit": {
        "config": _fake_config(
            "tui_nobracket", ready_pattern="Enter a coding task or / for commands",
            done_pattern="Enter a coding task or / for commands",
            bracketed_paste=False, submit_delay_seconds=0.2, quit="\u0003", max_turns=1,
        ),
        "expect": {"task_submitted": True},
        "expect_visible": ["TASK-RECEIVED"],
    },
    # ⭐ Lo stesso TUI, letto con `capture-pane` invece che dai byte: qui il
    # transcript e' lo SCHERMO, e quello che si e' visto e' verificabile.
    "tmux_tui_screen": {
        "config": _fake_config(
            "tui", driver="tmux",
            ready_pattern="Enter a coding task or / for commands",
            done_pattern="Enter a coding task or / for commands",
            submit_delay_seconds=0.2, quit="\u0003", max_turns=1, rows=20, cols=100,
        ),
        "expect": {"driver": "tmux", "startup_ready": True, "task_submitted": True},
        "expect_visible": ["TASK-RECEIVED", "fake-tui frame"],
    },
    # Il driver tmux deve saper dire «muto» come lo dice il PTY.
    "tmux_silent_startup": {
        "config": _fake_config(
            "silent", driver="tmux", ready_pattern=">>>", done_pattern=">>>",
            rows=20, cols=100,
        ),
        "expect": {
            "driver": "tmux", "startup_ready": False, "task_submitted": False,
            "termination_reason": "screen_stalled_startup", "valid": False,
        },
    },
    # Anche un REPL di riga deve restare pilotabile via tmux: se il driver non
    # e' intercambiabile, non e' un driver — e' una biforcazione del banco.
    "tmux_repl_turns": {
        "config": _fake_config(
            "repl", driver="tmux", ready_pattern=">>>", done_pattern=">>>",
            quit="/quit\r", max_turns=3, continue_text="continue", rows=20, cols=100,
        ),
        "expect": {"driver": "tmux", "turns_used": 3,
                   "termination_reason": "completion_marker", "valid": True},
        "expect_visible": ["turn 3 done"],
    },
    # Un agente che scrive davvero un file nel workspace neutro.
    "worker_writes_artifact": {
        "config": _fake_config(
            "worker", ready_pattern=">>>", done_pattern=">>>", quit="/quit\r", max_turns=1,
        ),
        "artifact": "quicksort.c",
        "expect": {"task_submitted": True, "valid": True},
        "expect_file": "quicksort.c",
    },
}


def run_selftest(cases: list[str] | None) -> int:
    if not FAKE_AGENT.is_file():
        print(f"challenge: missing {FAKE_AGENT}", file=sys.stderr)
        return 2
    selected = list(cases or SELFTEST_CASES)
    unknown = [name for name in selected if name not in SELFTEST_CASES]
    if unknown:
        print(f"challenge: unknown selftest cases: {', '.join(unknown)}", file=sys.stderr)
        return 2
    started = time.monotonic()
    failures: list[str] = []
    with tempfile.TemporaryDirectory(prefix="challenge-selftest-") as tmp_text:
        tmp = Path(tmp_text)
        for name in selected:
            spec = SELFTEST_CASES[name]
            config = dict(spec["config"])
            mode = config.pop("_mode")
            work = tmp / name
            work.mkdir(parents=True)
            argv = [sys.executable, str(FAKE_AGENT), "--mode", mode]
            if spec.get("artifact"):
                argv += ["--artifact", spec["artifact"]]
            raw, meta = open_session(
                argv, work, dict(os.environ), SELFTEST_PROMPT, config, 12.0, work / "trace"
            )
            if meta.get("driver") == "tmux":
                # Per tmux cio' che conta e' cio' che si e' VISTO, non i byte:
                # tutte le schermate distinte, come le archivia una gara vera.
                visible = (work / "trace" / "screens.log").read_text(encoding="utf-8")
            else:
                visible = clean_transcript(raw)
            problems = [
                f"{key}={meta.get(key)!r} (atteso {want!r})"
                for key, want in spec.get("expect", {}).items()
                if meta.get(key) != want
            ]
            problems += [f"non visto {text!r}" for text in spec.get("expect_visible", [])
                         if text not in visible]
            problems += [f"visto ma non atteso {text!r}" for text in spec.get("expect_missing", [])
                         if text in visible]
            wanted_file = spec.get("expect_file")
            if wanted_file and not (work / wanted_file).is_file():
                problems.append(f"file mancante {wanted_file!r}")
            if problems:
                failures.append(name)
                print(f"FAIL {name}: " + "; ".join(problems), flush=True)
                (work / "trace" / "transcript.txt").write_text(visible, encoding="utf-8")
            else:
                print(f"ok   {name} ({meta['duration_seconds']}s)", flush=True)
    elapsed = round(time.monotonic() - started, 1)
    print(f"selftest: {len(selected) - len(failures)}/{len(selected)} in {elapsed}s")
    return 1 if failures else 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--league",
        type=Path,
        default=LEAGUE_FILE,
        help="league.json to use; tasks/ and archives resolve next to it",
    )
    sub = parser.add_subparsers(dest="command")
    sub.add_parser("check", help="validate configuration without starting agents")
    run = sub.add_parser("run", help="run both agents sequentially")
    run.add_argument("--match", action="append", dest="matches", help="match id; repeatable")
    run.add_argument("--run-id", default=utc_run_id())
    run.add_argument("--timeout", type=float, help="override per-agent wall-clock seconds")
    sub.add_parser("scoreboard", help="rebuild the scoreboard from archived result.json files")
    selftest = sub.add_parser(
        "selftest", help="exercise the pilot against fake agents; no network, no real agent"
    )
    selftest.add_argument("--case", action="append", dest="cases", help="case name; repeatable")
    return parser.parse_args()


def main() -> int:
    global BASE
    args = parse_args()
    command = args.command or "check"
    league_file = Path(args.league).resolve()
    BASE = league_file.parent
    try:
        if command == "selftest":
            return run_selftest(getattr(args, "cases", None))
        league = load_json(league_file)
        matches = validate(league)
        output_root = BASE / league["id"]
        output_root.mkdir(parents=True, exist_ok=True)
        if command == "check":
            print_check(league, matches)
            return 0
        if command == "scoreboard":
            render_scoreboard(league, output_root)
            print(output_root / "scoreboard.md")
            return 0
        if not SAFE_ID_RE.match(args.run_id):
            raise ChallengeError("run id must be 1-80 safe filename characters")
        selected = set(args.matches or league["matches"])
        unknown = selected.difference(league["matches"])
        if unknown:
            raise ChallengeError(f"unknown matches: {', '.join(sorted(unknown))}")
        for match in matches:
            if match["id"] in selected:
                record = run_match(league, match, args.run_id, args.timeout)
                if record_is_valid(record, list(league["agents"])):
                    scores = {a: record["agents"][a]["judge"]["score"] for a in league["agents"]}
                    print(f"{match['id']}: " + ", ".join(f"{a}={s}/100" for a, s in scores.items()))
                else:
                    print(f"{match['id']}: cancelled; no score")
        return 0
    except (ChallengeError, OSError, subprocess.TimeoutExpired) as exc:
        print(f"challenge: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
