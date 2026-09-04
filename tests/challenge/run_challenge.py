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
        match_dir = HERE / "tasks" / match_id
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
    output_root = HERE / league["id"]
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
            raw, session = pty_session(
                argv, code, env, prompt, session_cfg, timeout, agent_dir
            )
            (agent_dir / "transcript.txt").write_text(clean_transcript(raw), encoding="utf-8")
            session.pop("stream_trace")
            session.pop("logic_actions")
            shutil.copytree(code, archive_code)
            report = judge(match_dir, archive_code)
            artifact = archive_code / str(match["artifact"])
            record["agents"][agent_id] = {
                **version,
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
        argv0 = expand(str(cfg["argv"][0]), variables)
        state = "found" if command_exists(argv0) else "MISSING"
        print(f"agent:  {agent_id:9s} {state:7s} {argv0}")
    for match in matches:
        seed = match["_dir"] / "seed"
        print(
            f"match:  {match['id']:9s} difficulty={match.get('difficulty')} "
            f"artifact={match['artifact']} order={','.join(match['order'])} "
            f"seed={tree_digest(seed)[:12]} prompt={len(match['_prompt'].encode())}B"
        )
    print("check only: no agent was started and no archived result was changed")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command")
    sub.add_parser("check", help="validate configuration without starting agents")
    run = sub.add_parser("run", help="run both agents sequentially")
    run.add_argument("--match", action="append", dest="matches", help="match id; repeatable")
    run.add_argument("--run-id", default=utc_run_id())
    run.add_argument("--timeout", type=float, help="override per-agent wall-clock seconds")
    sub.add_parser("scoreboard", help="rebuild the scoreboard from archived result.json files")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    command = args.command or "check"
    try:
        league = load_json(LEAGUE_FILE)
        matches = validate(league)
        output_root = HERE / league["id"]
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
