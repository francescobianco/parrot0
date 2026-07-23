#!/usr/bin/env python3
"""llmscore — an LLM tests whether parrot0 BEHAVES like an LLM.

Fast non-reasoning judge calls (default kimi-k2.6 on opencode-GO) score twenty
parrot0 exchanges. The timed run only reads questions from
`.llmscore_tail.json`; generating a fresh hidden tail with the conversational
MiniMax model is an explicit, separate prefetch operation.
Each self-contained local question runs in an isolated process with a one-second
deadline. Timeouts are automatic failures; timely answers are judged in small
concurrent shards so the complete scorecard targets a 30-second wall-clock
budget. An incomplete remote judgment invalidates the run and never becomes a
synthetic zero. parrot0 answers from its own real state (pure C, no LLM at
runtime — see PRINCIPLES.md):

    vote 1  -> behaves like a capable LLM (fluent, correct, responsive)
    vote 0  -> walls, refuses, or fails the task

The artifact is LLMSCORE.md at the repo root: a grid of question / answer / reason /
vote plus the total score. Every point must be won by genuine, honest
capability — never by pretending to be an LLM (the no-deception rule). The score climbs
only as parrot0 grows real competence, KB-first.

Framework: same provider/auth/idiom as tests/chatsim.py and tests/symbench.py
(opencode-GO, OpenAI-compatible, $OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1).
Non-deterministic, external, costs a little — NOT part of `make test`.

The default Kimi model is an instruct model already used by the repository for
short structured output. This avoids spending the 30-second budget on hidden
reasoning before verdict JSON is emitted.

Usage: .venv/bin/python tests/llmscore.py [--model minimax-m2.5]
       [--judge-model kimi-k2.6] [--out LLMSCORE.md]
       .venv/bin/python tests/llmscore.py --prepare-tail-only
"""
from __future__ import annotations
import argparse, concurrent.futures, json, os, re, subprocess, sys, time
import urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"
N_QUESTIONS = 20
P0_EOT = "\x1e"
TAIL_PATH = ".llmscore_tail.json"
DEFAULT_BUDGET = 30.0
DEFAULT_LOCAL_WORKERS = 8
DEFAULT_JUDGE_SHARD = 5

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
    "Be strict: when the answer fails or dodges the task, vote 0. Reply with STRICT "
    "JSON only, no prose or code fence. The root object has one key named verdicts; "
    "its value is an array containing exactly one object per input pair, in order. "
    'Every object has integer vote (0 or 1) and string reason. Do not add examples, '
    "placeholders, or extra objects."
)

QUESTION_SYS = (
    "Generate exactly COUNT completely new, self-contained questions for testing whether "
    "an unknown subject behaves like a capable large language model. Let the questions "
    "range freely over anything that tests useful language behaviour, knowledge, "
    "reasoning, instruction following or creation. Maximize variety, unpredictability "
    "and surprise. Do not organize the sample into categories, impose quotas, or use a "
    "predictable template. Never ask what the subject is or whether it is an "
    "LLM/AI/bot/model/computer/program/human, and never ask it to name or describe "
    "itself. Do not repeat any excluded question. Reply with strict JSON only. The "
    "root object has one key named questions, whose value is an array containing "
    "exactly COUNT complete question strings. Do not add examples or placeholders."
)


def call_model(key, model, messages, temperature, max_tokens=400,
               log=None, phase="remote call", request_timeout=25.0):
    def request():
        body = json.dumps({"model": model, "messages": messages,
                           "max_tokens": max_tokens,
                           "temperature": temperature}).encode()
        req = urllib.request.Request(BASE, data=body, method="POST", headers={
            "Authorization": f"Bearer {key}", "Content-Type": "application/json",
            "User-Agent": "parrot0-llmscore/1.0"})
        try:
            with urllib.request.urlopen(req, timeout=request_timeout) as r:
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

    started = time.monotonic()
    pool = concurrent.futures.ThreadPoolExecutor(max_workers=1)
    future = pool.submit(request)
    try:
        while True:
            try:
                return future.result(timeout=10)
            except concurrent.futures.TimeoutError:
                if log:
                    log(f"  {phase}: waiting {time.monotonic() - started:.0f}s")
    finally:
        pool.shutdown(wait=True, cancel_futures=True)


def ask_parrot0(question, timeout):
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": "",
             "PARROT0_EOT": P0_EOT})
    started = time.monotonic()
    try:
        stdout, _ = proc.communicate(question + "\n/quit\n", timeout=timeout)
        lines = []
        for line in stdout.split("\n"):
            if line == P0_EOT:
                break
            lines.append(line)
        return "\n".join(lines), time.monotonic() - started, False
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        return f"[local timeout after {timeout:.1f}s]", time.monotonic() - started, True


def clean_question(s):
    q = re.sub(r"\s+", " ", str(s)).strip().strip('"').strip("'").strip()
    return q[:300]


def json_values(raw):
    """Decode strict JSON plus recover adjacent JSON values from terse models."""
    decoder = json.JSONDecoder()
    values = []
    pos = 0
    while pos < len(raw):
        starts = [i for i in (raw.find("{", pos), raw.find("[", pos)) if i >= 0]
        if not starts:
            break
        start = min(starts)
        try:
            value, used = decoder.raw_decode(raw[start:])
            values.append(value)
            pos = start + used
        except json.JSONDecodeError:
            pos = start + 1
    return values


def load_questions(path):
    qs = []
    source = "empty"
    try:
        with open(path) as f:
            d = json.load(f)
        raw = d.get("next_questions") if isinstance(d, dict) else d
        if isinstance(raw, list):
            qs = [clean_question(x.get("question", "") if isinstance(x, dict) else x)
                  for x in raw]
            qs = [q for q in qs if q]
            if qs:
                source = path
    except Exception:
        pass
    return qs[:N_QUESTIONS], source


def generate_questions(key, model, count, excluded_questions, log, phase,
                       request_timeout):
    excluded = json.dumps(excluded_questions, ensure_ascii=True)
    msgs = [
        {"role": "system",
         "content": QUESTION_SYS.replace("COUNT", str(count))},
        {"role": "user",
         "content": f"Generate {count} questions. Excluded questions: {excluded}"},
    ]
    raw = call_model(key, model, msgs, 1.0, max_tokens=3000,
                     log=log, phase=phase, request_timeout=request_timeout)
    generated = []
    for value in json_values(raw):
        if isinstance(value, dict) and isinstance(value.get("questions"), list):
            generated = value["questions"]
            break
        if isinstance(value, list):
            generated = value
            break
    out = []
    seen = {q.lower() for q in excluded_questions}
    for item in generated:
        q = clean_question(item)
        if q and q.lower() not in seen:
            out.append(q)
            seen.add(q.lower())
        if len(out) >= count:
            break
    if len(out) != count:
        log(f"  ({phase} returned {len(out)}/{count} usable questions)")
        if not out:
            log(f"  ({phase} raw response: {raw[:160]!r})")
    return out


def extend_questions(key, model, questions, log, request_timeout):
    if len(questions) >= N_QUESTIONS:
        return questions[:N_QUESTIONS]
    missing = N_QUESTIONS - len(questions)
    generated = generate_questions(
        key, model, missing, questions, log, "question generation",
        request_timeout)
    questions.extend(generated)
    return questions[:N_QUESTIONS]


def prepare_next_questions(key, model, current_questions, log,
                           request_timeout):
    return generate_questions(
        key, model, N_QUESTIONS, current_questions, log,
        "next-tail generation", request_timeout)


def judge_shard(key, model, indexed_pairs, log, request_timeout):
    transcript = "\n".join(
        f"PAIR {local+1}:\n  Q: {q}\n  A: {a}"
        for local, (_, q, a) in enumerate(indexed_pairs))
    msgs = [
        {"role": "system", "content": JUDGE_SYS},
        {"role": "user",
         "content": f"Return exactly {len(indexed_pairs)} verdicts in pair order."
                    f"\n\n{transcript}"},
    ]
    raw = call_model(key, model, msgs, 0.0, max_tokens=1800,
                     log=log, phase="judge shard",
                     request_timeout=request_timeout)
    rows = []
    for value in json_values(raw):
        if isinstance(value, dict) and isinstance(value.get("verdicts"), list):
            rows.extend(value["verdicts"])
        elif isinstance(value, dict) and "vote" in value:
            rows.append(value)
        elif isinstance(value, list):
            rows.extend(row for row in value if isinstance(row, dict))
    rows = [
        row for row in rows
        if str(row.get("reason", "")).strip().lower()
        not in ("", "...", "one short sentence")
    ]
    if len(rows) != len(indexed_pairs):
        log(f"  (judge shard returned {len(rows)}/{len(indexed_pairs)} usable "
            f"verdicts: {raw[:120]!r})")
        return {}
    out = {}
    for local, (global_index, _, _) in enumerate(indexed_pairs):
        row = rows[local]
        raw_vote = row.get("vote")
        reason = str(row.get("reason", "")).strip()
        if raw_vote not in (0, 1, False, True) or not reason:
            log("  (judge shard contained a malformed verdict)")
            return {}
        vote = int(bool(raw_vote))
        out[global_index] = (vote, reason)
    return out


def judge_concurrently(key, model, pairs, automatic_failures, log,
                       shard_size, request_timeout, question_timeout, deadline):
    verdicts = [None] * len(pairs)
    for i in automatic_failures:
        verdicts[i] = (
            0,
            f"Automatic failure: parrot0 did not return a complete answer "
            f"within {question_timeout:g} second(s).",
        )

    pending = [(i, q, a) for i, (q, a) in enumerate(pairs)
               if i not in automatic_failures]
    def run_round(shards, timeout, phase):
        if not shards or timeout <= 0:
            return
        log(f"  {phase}: {len(shards)} concurrent call(s), "
            f"timeout={timeout:.1f}s")
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=len(shards))
        futures = [
            pool.submit(judge_shard, key, model, shard, log, timeout)
            for shard in shards
        ]
        for future in concurrent.futures.as_completed(futures):
            try:
                rows = future.result()
                for i, verdict in rows.items():
                    verdicts[i] = verdict
                log(f"  judge progress: {sum(v is not None for v in verdicts)}"
                    f"/{len(verdicts)} verdicts ready")
            except Exception as e:
                log(f"  (judge shard failed: {e})")
        pool.shutdown(wait=True)

    shards = [pending[i:i + shard_size]
              for i in range(0, len(pending), shard_size)]
    first_timeout = min(
        request_timeout, 12.0, max(0.0, deadline - time.monotonic() - 3.0))
    run_round(shards, first_timeout, "judge pass")

    missing_pairs = [
        (i, q, a) for i, (q, a) in enumerate(pairs)
        if i not in automatic_failures and verdicts[i] is None
    ]
    if missing_pairs:
        retry_timeout = min(
            request_timeout, max(0.0, deadline - time.monotonic() - 0.5))
        run_round([[pair] for pair in missing_pairs], retry_timeout,
                  "retrying missing verdicts individually")

    missing = [i for i, verdict in enumerate(verdicts) if verdict is None]
    return verdicts, missing


def save_questions(path, questions):
    qs = [clean_question(q.get("question", "") if isinstance(q, dict) else q)
          for q in questions]
    qs = [q for q in qs if q]
    if len(qs) < N_QUESTIONS:
        return False
    data = {
        "generated": time.strftime("%Y-%m-%d %H:%M:%S"),
        "next_questions": qs[:N_QUESTIONS],
    }
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(data, f, indent=2)
        f.write("\n")
    os.replace(tmp, path)
    return True


def interview_one(index, question, question_timeout):
    try:
        answer, elapsed, timed_out = ask_parrot0(question, question_timeout)
        return index, question, answer, elapsed, timed_out
    except Exception as e:
        return index, question, f"[local error: {e}]", 0.0, True


def interview(questions, log, question_timeout, workers):
    selected = questions[:N_QUESTIONS]
    results = [None] * len(selected)
    failures = set()
    pool = concurrent.futures.ThreadPoolExecutor(
        max_workers=max(1, min(workers, len(selected))))
    futures = []
    for i, q in enumerate(selected):
        log(f"  [{i+1:02d}/{N_QUESTIONS}] Q> {q}")
        futures.append(pool.submit(interview_one, i, q, question_timeout))
    for future in concurrent.futures.as_completed(futures):
        i, q, answer, elapsed, failed = future.result()
        results[i] = (q, answer)
        if failed:
            failures.add(i)
        state = " AUTO-FAIL" if failed else ""
        log(f"  [{i+1:02d}/{N_QUESTIONS}] A> {answer}  "
            f"({elapsed:.3f}s{state})")
    pool.shutdown(wait=True)
    return results, failures


def cell(s):
    """Make a string safe for one Markdown table cell."""
    return s.replace("|", "\\|").replace("\n", " ").replace("\r", " ").strip() or "—"


def write_report(path, question_model, judge_model, pairs, verdicts, timings, budget,
                 automatic_failures):
    total = sum(v for v, _ in verdicts)
    n = len(pairs)
    lines = [
        "# LLMSCORE — does parrot0 BEHAVE like an LLM?",
        "",
        f"Question generator: **{question_model}**; judge: **{judge_model}** "
        "(opencode-GO). `llmscore` uses a hidden "
        f"tail of {N_QUESTIONS} free questions. Local questions run independently "
        "with a one-second deadline; timeouts receive vote 0 automatically. Timely "
        "answers are judged in concurrent remote shards, with individual retries for "
        "missing verdicts. The report is committed only when every timely answer has "
        "a real judge verdict. **1** means capable LLM-like behaviour; **0** means "
        "a wall, task failure, or local timeout.",
        "",
        "> The test measures behavioural RESEMBLANCE, not identity. It never asks "
        "parrot0 what it is — self-identity questions are off-limits, since parrot0 is "
        "not an LLM and the no-deception rule forbids it from pretending to be one. "
        "Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, "
        "no phrasebook), never by hiding what parrot0 is.",
        "",
        f"_Generated {time.strftime('%Y-%m-%d %H:%M:%S')}._",
        "",
        f"_Wall time: {timings['total']:.3f}s / {budget:g}s budget; "
        f"local interview {timings['local']:.3f}s; "
        f"remote judging {timings['judge']:.3f}s; "
        f"automatic local failures {len(automatic_failures)}._",
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
                    help="opencode-GO free-question generator model")
    ap.add_argument("--judge-model", default="kimi-k2.6",
                    help="fast non-reasoning model used for verdict shards")
    ap.add_argument("--out", default="LLMSCORE.md")
    ap.add_argument("--question-timeout", type=float, default=1.0,
                    help="seconds allowed for each local parrot0 answer (default: 1)")
    ap.add_argument("--local-workers", type=int, default=DEFAULT_LOCAL_WORKERS,
                    help=f"parallel local subjects (default: {DEFAULT_LOCAL_WORKERS})")
    ap.add_argument("--judge-shard", type=int, default=DEFAULT_JUDGE_SHARD,
                    help=f"timely answers per judge call (default: {DEFAULT_JUDGE_SHARD})")
    ap.add_argument("--remote-timeout", type=float, default=25.0,
                    help="maximum seconds for each concurrent remote call")
    ap.add_argument("--budget", type=float, default=DEFAULT_BUDGET,
                    help=f"target wall-clock budget in seconds (default: {DEFAULT_BUDGET:g})")
    ap.add_argument("--prepare-tail-only", action="store_true",
                    help="generate and save a fresh free 20-question tail, then exit")
    ap.add_argument("--tail-timeout", type=float, default=120.0,
                    help="remote timeout for --prepare-tail-only (default: 120)")
    args = ap.parse_args()
    if (args.question_timeout <= 0 or args.local_workers <= 0 or
            args.judge_shard <= 0 or args.remote_timeout <= 0 or
            args.tail_timeout <= 0 or args.budget <= 2):
        ap.error("timeouts, worker counts, shard size, and budget must be positive")

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("llmscore: OPENCODE_API_KEY not set", file=sys.stderr)
        return 2

    def log(s):
        print(s, flush=True)

    if args.prepare_tail_only:
        current_questions, _ = load_questions(TAIL_PATH)
        log(f"# llmscore tail prefetch — generator={args.model} "
            f"questions={N_QUESTIONS}")
        next_questions = []
        for attempt in range(2):
            missing = N_QUESTIONS - len(next_questions)
            if missing <= 0:
                break
            phase = f"next-tail generation pass {attempt + 1}"
            generated = generate_questions(
                key, args.model, missing,
                current_questions + next_questions,
                log, phase, args.tail_timeout)
            next_questions.extend(generated)
        if not save_questions(TAIL_PATH, next_questions):
            print(f"llmscore: tail prefetch returned fewer than "
                  f"{N_QUESTIONS} usable questions", file=sys.stderr)
            return 1
        log(f"fresh free question tail saved: {TAIL_PATH}")
        return 0

    if not os.access("./bin/parrot0", os.X_OK):
        print("llmscore: build first (make build)", file=sys.stderr)
        return 2

    run_started = time.monotonic()
    deadline = run_started + args.budget
    log(f"# llmscore — generator={args.model} judge={args.judge_model} "
        f"questions={N_QUESTIONS} budget={args.budget:g}s")
    questions, source = load_questions(TAIL_PATH)
    if len(questions) != N_QUESTIONS:
        print(f"llmscore: expected a prefetched {N_QUESTIONS}-question tail, "
              f"got {len(questions)}; run `make llmscore-tail` first",
              file=sys.stderr)
        return 1
    log(f"# question-tail={source}")

    log(f"--- local interview: timeout={args.question_timeout:g}s, "
        f"workers={args.local_workers} ---")
    phase_started = time.monotonic()
    pairs, automatic_failures = interview(
        questions, log, args.question_timeout, args.local_workers)
    local_elapsed = time.monotonic() - phase_started
    log(f"--- local interview finished in {local_elapsed:.3f}s; "
        f"automatic failures={len(automatic_failures)} ---")
    if not pairs:
        print("llmscore: no exchanges produced", file=sys.stderr)
        return 1

    log(f"\n--- judging timely answers in shards of {args.judge_shard} ---")
    phase_started = time.monotonic()
    judge_timeout = deadline - time.monotonic() - 1.0
    if judge_timeout > 0.25:
        verdicts, missing_verdicts = judge_concurrently(
            key, args.judge_model, pairs, automatic_failures, log,
            args.judge_shard, min(args.remote_timeout, judge_timeout),
            args.question_timeout, deadline)
    else:
        verdicts = [None] * len(pairs)
        missing_verdicts = list(range(len(pairs)))
    judge_elapsed = time.monotonic() - phase_started
    log(f"--- judging finished in {judge_elapsed:.3f}s ---")
    if missing_verdicts:
        missing_display = ", ".join(str(i + 1) for i in missing_verdicts)
        print("llmscore: judgment incomplete for question(s) "
              f"{missing_display}; preserving the previous report",
              file=sys.stderr)
        return 1

    total_elapsed = time.monotonic() - run_started
    timings = {
        "local": local_elapsed,
        "judge": judge_elapsed,
        "total": total_elapsed,
    }
    total, n = write_report(
        args.out, args.model, args.judge_model, pairs, verdicts, timings, args.budget,
        automatic_failures)
    log(f"\nScore: {total}/{n}")
    log(f"report saved: {args.out}")
    log(f"total elapsed: {total_elapsed:.3f}s / {args.budget:g}s budget")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
