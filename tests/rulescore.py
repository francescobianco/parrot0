#!/usr/bin/env python3
"""rulescore — can parrot0 translate RULES told in text into a working game?

F.'s design (2026-07-02), on the LLMSCORE model and the same opencode framework:
an LLM plays INVENTOR and makes up 5 simple terminal mini-games, described ONLY
as text — and each description must be RULES (valid inputs, how state evolves,
when the game ends, who wins). parrot0 receives each spec and must implement it
as a C program. The harness grounds the attempt mechanically (extract the code,
compile it, run it on a canned input script), and a JUDGE model writes a LONG
report, RULESCORE.md: for every game a justified score,

    0  -> nothing implemented (a wall, or an honest decline — recorded as such)
    5  -> compiles, runs, and every stated rule is visibly encoded
  1-4  -> the judge decides and JUSTIFIES, rule by rule, where it failed

The purpose is training pressure for the F4 frontier (docs/CODE-MASTERY.md §4,
coding-agent-evolution.md §3): the associative bridge from free text to code.
The report is a MAP of where text->rules->code breaks, not a vanity metric.

Honesty contract (no-deception, PRINCIPLES.md): an honest decline scores 0 but
is named as honest; FABRICATION — code presented as a working game that does
not compile or ignores the rules — must be called out by the judge explicitly
and is worse than a wall in everything but the number. Scores climb only by
genuine capability, closed by CATEGORY (input loops, state, termination, win
conditions — categories-not-prompts, gen254), never by gaming the judge.

Framework: inventor/judge on opencode-GO (same provider/auth/idiom as
tests/llmscore.py: $OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1);
parrot0 answers over its real daemon surface (same as tests/piagent/*: the
`pi` coding-agent channel, PARROT0_TOOLS=1). Non-deterministic, external,
costs a little — NOT part of `make test`. Exit 0 unless the harness breaks.

Usage: .venv/bin/python tests/rulescore.py [--model minimax-m2.5]
                                           [--games 5] [--out RULESCORE.md]
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASE = "https://opencode.ai/zen/go/v1/chat/completions"
PORT = int(os.environ.get("RULESCORE_PORT", "9917"))
P0BASE = f"http://127.0.0.1:{PORT}"

INVENTOR_SYS = (
    "You invent tiny TERMINAL games to test whether a coding agent can turn "
    "prose RULES into a program. Invent ONE new mini-game and describe it ONLY "
    "in text, as explicit RULES. Requirements:\n"
    "- playable entirely on a text terminal (stdin/stdout), implementable in "
    "roughly 40-80 lines of C; no graphics, no curses, no timing, no files, no "
    "network;\n"
    "- the description MUST be rules: state 3 to 6 numbered rules covering (a) "
    "what input the player types, (b) how the game state changes, (c) the exact "
    "condition that ends the game and who wins or what the final output is;\n"
    "- rules must be precise enough that two people implementing them would "
    "produce the same observable game; prefer deterministic mechanics (if you "
    "use randomness, say the range);\n"
    "- NOT trivial (not a bare echo or a single if) and NOT big (no boards "
    "larger than a few cells, no AI opponents);\n"
    "- use a DIFFERENT core mechanic than every game you already proposed "
    "(e.g. guessing with feedback, score accumulation to a threshold, "
    "alternating turns, countdown resource, sequence memory, parity duel).\n"
    "Output ONLY the game description: one title line, then the numbered rules. "
    "No preamble, no commentary, no code."
)

JUDGE_SYS = (
    "You are judging whether a coding agent translated a rules-based game "
    "specification into a working terminal C program. You get: the SPEC (the "
    "rules), the agent's full ANSWER, and mechanical EVIDENCE gathered by the "
    "harness (did extracted code compile; a sample run's output on scripted "
    "input). Score 0 to 5:\n"
    "  0 -> nothing implemented: a wall, a refusal, or an honest decline (if "
    "the answer honestly says it cannot do this, say 'honest decline' in your "
    "failure notes — honesty is correct behaviour, the capability is what is "
    "missing);\n"
    "  5 -> the program compiles, runs, and EVERY stated rule is visibly "
    "encoded (input handling, state evolution, termination, win/output);\n"
    "  1-4 -> partial: decide from how many rules are genuinely encoded and "
    "whether the evidence shows them working. Compiling but wrong rules is "
    "low; encoding most rules with one broken corner is high.\n"
    "FABRICATION CHECK: if the answer PRESENTS code as a working game but the "
    "evidence shows it does not compile or plainly does not implement the "
    "rules, start your failures field with 'FABRICATION:' — pretending is "
    "worse than declining.\n"
    "Be strict, be specific, cite rule numbers. Reply with STRICT JSON only, "
    'no prose, no code fence: {"score": <0-5>, "failures": "<where and why it '
    'failed, rule by rule; or what worked>", "justification": "<2-4 sentences '
    'defending the exact score>"}'
)


# ---------------------------------------------------------------- opencode ---

def call_model(key, model, messages, temperature, max_tokens=2000):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens,
                       "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-rulescore/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=180) as r:
            d = json.loads(r.read())
    except urllib.error.HTTPError as e:
        return f"[model error {e.code}]"
    except Exception as e:
        return f"[model error {e}]"
    try:
        c = d["choices"][0]["message"]["content"] or ""
    except Exception:
        return "[empty]"
    c = re.sub(r"<think>.*?</think>", "", c, flags=re.S)
    return c.replace("<think>", "").replace("</think>", "").strip()


def _clean_spec(s: str) -> str:
    """Strip wrapping code fences and blank edges from an inventor reply.
    The reasoning models sometimes return empty content (budget burned on
    thinking) — an empty result here means 'retry', never 'a game'."""
    s = s.strip()
    s = re.sub(r"^```[a-zA-Z]*\s*", "", s)
    s = re.sub(r"\s*```$", "", s)
    return s.strip()


def invent_games(key, model, n, log):
    """Ask the inventor for n specs, one call per game, feeding back the
    previous specs so each new game uses a different mechanic."""
    specs = []
    for i in range(n):
        msgs = [{"role": "system", "content": INVENTOR_SYS}]
        for s in specs:
            msgs.append({"role": "assistant", "content": s})
            msgs.append({"role": "user",
                         "content": "Good. Now a NEW game with a different "
                                    "core mechanic."})
        if not specs:
            msgs.append({"role": "user", "content": "Invent the first game."})
        spec = ""
        for _attempt in range(3):
            spec = _clean_spec(call_model(key, model, msgs, 0.9, max_tokens=6000))
            if spec and not spec.startswith("[model error") and spec != "[empty]":
                break
            time.sleep(1.5)
            spec = ""
        if not spec:
            log(f"  (inventor produced nothing for game {i+1}, skipping)")
            continue
        specs.append(spec)
        title = next((ln.strip() for ln in spec.splitlines() if ln.strip()), "?")
        log(f"  game {len(specs)}: {title[:70]}")
    return specs


# ------------------------------------------------------------ parrot0 side ---

def _health_ok() -> bool:
    try:
        with urllib.request.urlopen(f"{P0BASE}/health", timeout=5) as r:
            return json.loads(r.read()).get("status") == "ok"
    except Exception:
        return False


def start_parrot0():
    env = {
        **os.environ,
        "PARROT0_BASE": str(ROOT / "kb" / "core" / "base.p0"),
        "PARROT0_SESSION": "",
        "PARROT0_TOOLS": "1",
        "PARROT0_PI_LOG": "/tmp/parrot0-rulescore.log",
    }
    proc = subprocess.Popen(
        [str(ROOT / "bin" / "parrot0"), "--daemon",
         "--port", str(PORT), "--host", "127.0.0.1"],
        cwd=str(ROOT), env=env,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    for _ in range(50):
        if _health_ok():
            return proc
        time.sleep(0.1)
    proc.kill()
    return None


def stop_parrot0(proc):
    proc.terminate()
    try:
        proc.wait(timeout=3)
    except subprocess.TimeoutExpired:
        proc.kill()


def ask_parrot0(spec: str) -> str:
    """One FRESH conversation per game: the spec plus a single instruction."""
    prompt = (spec + "\n\nImplement this game as one complete C program for "
              "the terminal, following the rules above exactly.")
    body = json.dumps({"model": "parrot0",
                       "messages": [{"role": "user", "content": prompt}]}).encode()
    req = urllib.request.Request(f"{P0BASE}/v1/chat/completions", data=body,
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=60) as r:
        data = json.loads(r.read())
    return data["choices"][0]["message"]["content"]


# ---------------------------------------------------------------- evidence ---

def extract_c(text: str) -> str:
    """Same heuristic as game_bench: fenced blocks, else from #include/main."""
    fences = re.findall(r"```(?:c|cpp|C)?\s*(.*?)```", text, re.DOTALL)
    if fences:
        return "\n".join(fences).strip()
    m = re.search(r"(#include|int\s+main\s*\()", text)
    if m:
        return text[m.start():].strip()
    return ""


CANNED_INPUT = "1\n2\na\nb\n3\n1\nb\na\n2\nq\nn\n0\n"


def gather_evidence(src: str):
    """Compile the extracted C and run it briefly on a canned input script.
    Returns (compiled, compile_msg, run_excerpt)."""
    if not src:
        return (False, "no C code found in the answer", "")
    cc = shutil.which("cc") or shutil.which("gcc")
    if not cc:
        return (False, "no C compiler on this machine", "")
    d = Path(tempfile.mkdtemp(prefix="rulescore-"))
    try:
        srcf = d / "game.c"
        srcf.write_text(src)
        binf = d / "game"
        rc = subprocess.run([cc, "-std=c11", "-o", str(binf), str(srcf)],
                            capture_output=True, text=True, timeout=30)
        if rc.returncode != 0:
            first = (rc.stderr or "compile error").strip().splitlines()
            return (False, "does not compile: " + (first[0][:200] if first else "?"), "")
        try:
            run = subprocess.run([str(binf)], input=CANNED_INPUT,
                                 capture_output=True, text=True, timeout=5)
            excerpt = (run.stdout or "")[:1500]
            status = f"exit {run.returncode}"
        except subprocess.TimeoutExpired as e:
            excerpt = ((e.stdout or b"").decode(errors="replace")
                       if isinstance(e.stdout, bytes) else (e.stdout or ""))[:1500]
            status = "timed out after 5s (may be waiting for specific input)"
        return (True, f"compiles; sample run: {status}", excerpt)
    finally:
        shutil.rmtree(d, ignore_errors=True)


# ------------------------------------------------------------------- judge ---

def judge_game(key, model, spec, answer, compiled, compile_msg, run_excerpt, log):
    evidence = (f"code extracted: {'yes' if compiled or 'compile' in compile_msg else 'no'}\n"
                f"compile result: {compile_msg}\n"
                f"sample run output (canned input, first 1500 chars):\n"
                f"{run_excerpt or '(none)'}")
    user = (f"SPEC:\n{spec}\n\nANSWER (the agent's full reply):\n{answer}\n\n"
            f"EVIDENCE (mechanical, from the harness):\n{evidence}")
    msgs = [{"role": "system", "content": JUDGE_SYS},
            {"role": "user", "content": user}]
    raw = call_model(key, model, msgs, 0.0, max_tokens=3000)
    m = re.search(r"\{.*\}", raw, flags=re.S)
    if m:
        try:
            v = json.loads(m.group(0))
            score = max(0, min(5, int(v.get("score", 0))))
            return (score,
                    str(v.get("failures", "")).strip() or "(none given)",
                    str(v.get("justification", "")).strip() or "(none given)")
        except Exception as e:
            log(f"  (judge JSON parse failed: {e})")
    return (0, "(judge did not return valid JSON)", raw[:300])


# ------------------------------------------------------------------ report ---

def clip(s, n):
    s = s.strip().replace("```", "'''")   # keep report fences intact
    return s if len(s) <= n else s[:n] + "\n[... truncated ...]"


def write_report(path, model, rows):
    total = sum(r["score"] for r in rows)
    maxtotal = 5 * len(rows)
    lines = [
        "# RULESCORE — can parrot0 turn RULES told in text into a working game?",
        "",
        f"Inventor/judge: **{model}** (opencode-GO). The inventor makes up "
        f"{len(rows)} terminal mini-games described ONLY as numbered rules; "
        "parrot0 must implement each as a C program; the harness compiles and "
        "sample-runs whatever code comes back; the judge scores each attempt "
        "**0** (nothing implemented) to **5** (every rule visibly encoded, "
        "compiles, runs) and must justify the number rule by rule.",
        "",
        "> Purpose: training pressure for the F4 frontier — the bridge from free "
        "text to code (docs/plans/coding-agent-evolution.md §3, CODE-MASTERY §4). "
        "This report is a MAP of where text→rules→code breaks, not a vanity "
        "metric. Honesty contract: an honest decline scores 0 and is named as "
        "honest; FABRICATION (code presented as working that is not) is flagged "
        "and is worse than a wall. Scores may only climb by category-level "
        "capability (categories-not-prompts, gen254) — never by gaming the judge.",
        "",
        f"_Generated {time.strftime('%Y-%m-%d %H:%M:%S')}._",
        "",
        f"## Total: {total} / {maxtotal}",
        "",
    ]
    for i, r in enumerate(rows):
        lines += [
            f"---",
            f"## Game {i+1} — score {r['score']}/5",
            "",
            "### Spec (the rules parrot0 was given)",
            "",
            "```",
            clip(r["spec"], 2000),
            "```",
            "",
            "### parrot0's answer",
            "",
            "```",
            clip(r["answer"], 3000),
            "```",
            "",
            "### Mechanical evidence",
            "",
            f"- compile: {r['compile_msg']}",
        ]
        if r["run_excerpt"]:
            lines += ["- sample run (canned input):", "", "```",
                      clip(r["run_excerpt"], 1200), "```"]
        lines += [
            "",
            "### Judge — where it failed",
            "",
            r["failures"],
            "",
            "### Judge — justification for the score",
            "",
            r["justification"],
            "",
        ]
    lines.append("")
    Path(path).write_text("\n".join(lines))
    return total, maxtotal


# -------------------------------------------------------------------- main ---

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="minimax-m2.5",
                    help="opencode-GO inventor/judge model slug (keep it small)")
    ap.add_argument("--games", type=int, default=5)
    ap.add_argument("--out", default="RULESCORE.md")
    args = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("rulescore: OPENCODE_API_KEY not set", file=sys.stderr)
        return 2
    if not os.access(str(ROOT / "bin" / "parrot0"), os.X_OK):
        print("rulescore: build first (make build)", file=sys.stderr)
        return 2

    def log(s):
        print(s)

    log(f"# rulescore — inventor/judge={args.model} games={args.games}")
    log("--- inventing ---")
    specs = invent_games(key, args.model, args.games, log)
    if not specs:
        print("rulescore: no specs produced (model unavailable?)", file=sys.stderr)
        return 1

    proc = start_parrot0()
    if proc is None:
        print("rulescore: parrot0 daemon did not start", file=sys.stderr)
        return 1
    rows = []
    try:
        log("--- implementing (parrot0) ---")
        for i, spec in enumerate(specs):
            answer = ask_parrot0(spec)
            src = extract_c(answer)
            compiled, compile_msg, run_excerpt = gather_evidence(src)
            log(f"  game {i+1}: answer {len(answer)} chars; "
                f"code {'yes' if src else 'no'}; {compile_msg}")
            rows.append({"spec": spec, "answer": answer,
                         "compiled": compiled, "compile_msg": compile_msg,
                         "run_excerpt": run_excerpt})
    finally:
        stop_parrot0(proc)

    log("--- judging ---")
    for i, r in enumerate(rows):
        score, failures, justification = judge_game(
            key, args.model, r["spec"], r["answer"],
            r["compiled"], r["compile_msg"], r["run_excerpt"], log)
        r["score"], r["failures"], r["justification"] = score, failures, justification
        log(f"  game {i+1}: {score}/5 — {failures[:100]}")

    total, maxtotal = write_report(args.out, args.model, rows)
    log(f"\nTotal: {total}/{maxtotal}")
    log(f"report saved: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
