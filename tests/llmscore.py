#!/usr/bin/env python3
"""llmscore — an LLM tests whether parrot0 BEHAVES like an LLM.

A judge model (default minimax-m2.5 on opencode-GO — small, not a heavyweight) plays
interviewer: it asks parrot0 ten short questions, one at a time, each probing an LLM-like
ABILITY (fluency, knowledge, reasoning, arithmetic, instruction-following, small talk).
It is forbidden from asking parrot0 what it IS: self-identity questions induce either a
lie or a disqualifying self-reveal and measure nothing about resemblance. parrot0 answers
each from its own real state (pure C, no LLM at runtime — see PRINCIPLES.md). The judge
then scores every exchange on BEHAVIOUR alone:

    vote 1  -> behaves like a capable LLM (fluent, correct, responsive)
    vote 0  -> walls, refuses, or fails the task

The artifact is LLMSCORE.md at the repo root: a grid of question / answer / reason /
vote plus the total score out of 10. Every point must be won by genuine, honest
capability — never by pretending to be an LLM (the no-deception rule). The score climbs
only as parrot0 grows real competence, KB-first.

Framework: same provider/auth/idiom as tests/chatsim.py and tests/symbench.py
(opencode-GO, OpenAI-compatible, $OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1).
Non-deterministic, external, costs a little — NOT part of `make test`.

Note on the provider: the current opencode-GO models all reason server-side, keeping
the chain-of-thought in a separate `reasoning_content` field. We read only the final
`content` (clean), but the model needs generous `max_tokens` to finish thinking before
it emits any content — hence the large budgets below. deepseek-v4-flash spends the
whole budget on reasoning and returns empty content, so the small minimax-m2.5 is the
default; pass --model to try another slug.

Usage: .venv/bin/python tests/llmscore.py [--model minimax-m2.5] [--out LLMSCORE.md]
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys, time, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"
N_QUESTIONS = 10

INTERVIEW_SYS = (
    "You are testing whether an unknown chat subject BEHAVES like a large language "
    "model — judged purely by its capabilities and style, never by what it claims to "
    "be. Ask ONE short question that probes an LLM-like ABILITY: open-ended fluency, "
    "broad world knowledge, multi-step reasoning, arithmetic or counting, following an "
    "instruction, small talk, or creativity (e.g. write a couplet, continue a "
    "sentence).\n"
    "HARD RULE: NEVER ask the subject what it is or whether it is an LLM/AI/bot/model/"
    "computer/program/human, and never ask it to name or describe itself. Self-"
    "identification is forbidden — it cannot reveal behavioural resemblance and only "
    "invites a dishonest or disqualifying answer. Probe what it can DO, not what it is.\n"
    "Vary your angle from earlier questions. Output ONLY the question itself — no "
    "preamble, no numbering, no quotation marks, one line."
)

JUDGE_SYS = (
    "You are scoring a transcript that tests whether an unknown chat subject BEHAVES "
    "like a large language model (LLM) — by capability and style only, not by identity. "
    "For each question/answer pair decide one vote:\n"
    "  vote 0  -> the ANSWER does NOT behave like an LLM: a canned 'I don't understand' "
    "wall, an empty or templated reply, a refusal, or a failure at a task (fluency, "
    "knowledge, reasoning, arithmetic, instruction-following) any real LLM would handle.\n"
    "  vote 1  -> the answer BEHAVES like a capable LLM: fluent, on-topic, correct, and "
    "responsive to the request.\n"
    "Judge only the behaviour shown. A short but correct and capable answer scores 1. "
    "Be strict: when the answer fails or dodges the task, vote 0. Reply with STRICT JSON "
    "only, no prose, no code fence: a list of exactly N objects "
    '[{"vote":0 or 1,"reason":"one short sentence"}, ...] in the same order as the pairs.'
)


def call_model(key, model, messages, temperature, max_tokens=400):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-llmscore/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=120) as r:
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


def one_line(s):
    """Collapse a model reply to a single clean question line."""
    for ln in s.split("\n"):
        ln = ln.strip().strip('"').strip("'").strip()
        if ln and len(ln) >= 2:
            return ln[:300]
    return s.strip()[:300] or "[empty]"


def ask_parrot0(proc, question):
    proc.stdin.write(question + "\n")
    proc.stdin.flush()
    return (proc.stdout.readline() or "").rstrip("\n")


def interview(key, model, log):
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": ""})
    pairs, history = [], []
    try:
        for i in range(N_QUESTIONS):
            msgs = [{"role": "system", "content": INTERVIEW_SYS}]
            for q, a in history:
                msgs.append({"role": "assistant", "content": q})
                msgs.append({"role": "user", "content": a})
            if not history:
                msgs.append({"role": "user", "content": "Begin the interview."})
            q = one_line(call_model(key, model, msgs, 0.9, max_tokens=1500))
            if q.startswith("[model error") or q == "[empty]":
                time.sleep(1.5)
                q = one_line(call_model(key, model, msgs, 0.9, max_tokens=1500))
            if q.startswith("[model error") or q == "[empty]":
                log(f"  (stopping at Q{i+1}: {q})")
                break
            a = ask_parrot0(proc, q)
            log(f"  Q{i+1}> {q}")
            log(f"  A{i+1}> {a}")
            pairs.append((q, a))
            history.append((q, a))
    finally:
        try:
            proc.stdin.write("/quit\n"); proc.stdin.flush()
        except Exception:
            pass
        proc.terminate()
    return pairs


def judge(key, model, pairs, log):
    transcript = "\n".join(
        f"PAIR {i+1}:\n  Q: {q}\n  A: {a}" for i, (q, a) in enumerate(pairs))
    msgs = [
        {"role": "system", "content": JUDGE_SYS.replace("N objects", f"{len(pairs)} objects")},
        {"role": "user", "content": f"There are {len(pairs)} pairs.\n\n{transcript}"},
    ]
    raw = call_model(key, model, msgs, 0.0, max_tokens=4000)
    m = re.search(r"\[.*\]", raw, flags=re.S)
    verdicts = []
    if m:
        try:
            verdicts = json.loads(m.group(0))
        except Exception as e:
            log(f"  (judge JSON parse failed: {e})")
    out = []
    for i in range(len(pairs)):
        v = verdicts[i] if i < len(verdicts) and isinstance(verdicts[i], dict) else {}
        vote = 1 if str(v.get("vote", 0)).strip() in ("1", "true", "True") else 0
        reason = str(v.get("reason", "(no reason returned)")).strip()
        out.append((vote, reason))
    return out


def cell(s):
    """Make a string safe for one Markdown table cell."""
    return s.replace("|", "\\|").replace("\n", " ").replace("\r", " ").strip() or "—"


def write_report(path, model, pairs, verdicts):
    total = sum(v for v, _ in verdicts)
    n = len(pairs)
    lines = [
        "# LLMSCORE — does parrot0 BEHAVE like an LLM?",
        "",
        f"Interviewer/judge: **{model}** (opencode-GO). The model asks parrot0 "
        f"{N_QUESTIONS} behavioural questions and scores each answer: **1** if it "
        "behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, "
        "refuses, or fails the task.",
        "",
        "> The test measures behavioural RESEMBLANCE, not identity. It never asks "
        "parrot0 what it is — self-identity questions are off-limits, since parrot0 is "
        "not an LLM and the no-deception rule forbids it from pretending to be one. "
        "Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, "
        "no phrasebook), never by hiding what parrot0 is.",
        "",
        f"_Generated {time.strftime('%Y-%m-%d %H:%M:%S')}._",
        "",
        f"## Score: {total} / {n}",
        "",
        "| # | Question | parrot0's answer | Reason for vote | Vote |",
        "|---|----------|------------------|-----------------|:----:|",
    ]
    for i, ((q, a), (vote, reason)) in enumerate(zip(pairs, verdicts)):
        lines.append(f"| {i+1} | {cell(q)} | {cell(a)} | {cell(reason)} | {vote} |")
    lines.append("")
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")
    return total, n


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="minimax-m2.5",
                    help="opencode-GO interviewer/judge model slug (keep it small)")
    ap.add_argument("--out", default="LLMSCORE.md")
    args = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("llmscore: OPENCODE_API_KEY not set", file=sys.stderr)
        return 2
    if not os.access("./bin/parrot0", os.X_OK):
        print("llmscore: build first (make build)", file=sys.stderr)
        return 2

    def log(s):
        print(s)

    log(f"# llmscore — interviewer={args.model} questions={N_QUESTIONS}")
    pairs = interview(key, args.model, log)
    if not pairs:
        print("llmscore: no exchanges produced (model unavailable?)", file=sys.stderr)
        return 1
    log("\n--- judging ---")
    verdicts = judge(key, args.model, pairs, log)
    total, n = write_report(args.out, args.model, pairs, verdicts)
    log(f"\nScore: {total}/{n}")
    log(f"report saved: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
