#!/usr/bin/env python3
"""autolearn — the autonomous MCP trainer (T0.e, docs/plans/llmscore-strategies.md).

parrot0 self-improves WITHOUT touching C: an opencode-GO model plays three roles
around parrot0's own MCP training interface (the U-series, gen279-289):

    INTERVIEWER  asks one ability-probing question (or takes it from --probes);
    parrot0      answers from its real state (chat, session KB loaded);
    JUDGE        votes 0/1 on the exchange (same criteria as llmscore);
    TEACHER      on a 0, reads parrot0's honest decline — which NAMES the gap —
                 and formulates a LESSON: whitelisted KB facts, taught live via
                 --mcp-engine (kb.assert -> kb.save). Re-probe; retry while the
                 decline keeps naming new gaps (the gradient loop).

HONESTY ORACLE: a lesson persists into kb/learning/autolearn.p0 ONLY if the
re-probe flips the judge's vote to 1. A lesson that does not take is rolled back
and recorded in the FAILED-LESSON LEDGER — the queue that names engine/consumer
gaps (the D.1->U3 pattern: C work is pulled by a documented failed lesson, never
speculative). The KB is the weights; the lesson is the gradient step; the honest
gap-naming decline is the loss; MCP is the training interface.

Framework: same provider/auth/idiom as tests/llmscore.py (opencode-GO,
$OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1). Non-deterministic,
external, costs a little — NOT part of `make test`. Report appended to
AUTOLEARN.md at the repo root.

Usage: .venv/bin/python tests/autolearn.py [--rounds 5] [--probes FILE]
         [--model minimax-m2.5] [--kb kb/learning/autolearn.p0] [--retries 2]
"""
from __future__ import annotations
import argparse, concurrent.futures, json, os, random, re, shutil, subprocess, sys, \
       threading, time, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"
P0_EOT = "\x1e"

# Rotating ability hints so parallel workers (no shared interviewer history) still
# ask DIVERSE questions across the axes the judge scores.
ABILITIES = [
    "a world-geography fact (a capital, a river, a border)",
    "a science fact (an element symbol, a planet, a body part)",
    "a short translation of a common phrase into Spanish",
    "a short translation of a common phrase into French",
    "a classic riddle with a well-known answer",
    "a vocabulary question (an opposite, or a word-for-a-definition)",
    "the meaning of a common idiom",
    "an animal sound or an animal fact",
    "a simple comparison (which is bigger/faster/heavier)",
    "the definition of a moderately advanced English word",
    "a color fact (mixing, or the color of a thing)",
    "a famous first/inventor/author fact",
]

# Facts the teacher may assert: predicates with a VERIFIED consumer in the engine.
# Anything else is dropped before teaching (and noted), so a hallucinated schema
# can never pollute the KB. Routing/self predicates are blacklisted outright.
WHITELIST = {
    "tr_es": 2, "tr_fr": 2, "gender_es": 2, "gender_fr": 2,
    "riddle_sig": 2, "response_template": 2,
    "wiki_concept": 3, "color_of": 2, "sound_of": 2,
    "magnitude": 3, "magnitude_cue": 3, "idiom_meaning": 2,
}
# Generic binary relations ("what is the <rel> of <y>" frame, gen11) are also
# consumable: allow any simple lowercase pred/2 not blacklisted.
BLACKLIST = {"module", "learnable", "intent_cue", "intent_phrase", "cont", "cont2",
             "i_am", "fact_source", "weight"}

INTERVIEW_SYS = (
    "You are probing the abilities of an unknown chat subject, one short question "
    "at a time. Ask ONE self-contained question testing an ability a capable "
    "assistant has: a world-knowledge fact, a word definition, vocabulary (opposite/"
    "word-for), a short translation into Spanish or French, a classic riddle, an "
    "animal sound, a color fact, a simple comparison, or a short creative request.\n"
    "HARD RULE: never ask what the subject is (no AI/model/human questions).\n"
    "Each turn ask a COMPLETELY NEW question on a DIFFERENT ability than before; do "
    "not react to previous answers. Output ONLY the question, one line."
)

JUDGE_SYS = (
    "You are scoring one question/answer exchange from a subject that should behave "
    "like a capable LLM. vote 0 -> the answer walls ('I don't understand'), refuses, "
    "deflects, or fails the task. vote 1 -> fluent, on-topic, correct, responsive. "
    "A short but correct answer scores 1. Reply STRICT JSON only: "
    '{"vote":0 or 1,"reason":"one short sentence"}'
)

TEACHER_SYS = (
    "You are the trainer of parrot0, a deterministic knowledge-base engine (pure C, "
    "no neural net). It answers ONLY from KB facts a fixed engine consumes. It just "
    "failed a question; its reply often NAMES exactly what is missing. Formulate a "
    "LESSON: the KB facts that would let it answer THIS QUESTION AND ITS WHOLE CLASS.\n"
    "Reply STRICT JSON only (no prose, no code fence):\n"
    '  {"facts": [{"pred": "...", "args": ["...", "..."]}, ...]}   (max 10 facts)\n'
    'or {"skip": "one-line reason"} when no teachable lesson applies.\n'
    "PREDICATE SCHEMAS THE ENGINE CONSUMES (use ONLY these shapes):\n"
    "- tr_es(english_word, spanish_word) / tr_fr(english_word, french_word): "
    "translation lexicon, one word per fact, lowercase. For a sentence translation "
    "teach EVERY plausibly-missing content word, plus gender_es(spanish_noun, m|f) "
    "/ gender_fr(french_noun, m|f) for nouns.\n"
    "- riddle_sig(riddle_id, cue) + response_template(riddle_id, answer_sentence): "
    "classic riddles. riddle_id like riddle_match; give 2-3 cue substrings, then ONE "
    "response_template with the full answer sentence. A riddle fires only when ALL "
    "of its id's cues occur in the question, so each cue must be a SHORT phrase "
    "(2-4 words) that LITERALLY appears in the riddle text, lowercase, no commas. "
    "If you retry after a failed attempt, use a FRESH riddle_id — cues accumulate "
    "per id and wrong ones poison it. Only teach a riddle whose canonical answer "
    "you are confident about.\n"
    "- wiki_concept(concept_key, domain, definition): a one-line definition for "
    "'what is X' gaps; concept_key lowercase_with_underscores.\n"
    "- idiom_meaning(idiom_phrase, gloss): the meaning of an idiom/saying; the "
    "phrase is matched as a substring of the question, lowercase.\n"
    "- color_of(thing, color); sound_of(animal, sound); "
    "magnitude(dimension, item, rank_number) with magnitude_cue(cueword, dimension, "
    "max|min) for comparisons — cueword MUST be a single word (e.g. fastest), "
    "multi-word cues never match.\n"
    "- any simple binary relation rel(x, y) answering 'what/who is the <rel> of "
    "<y>' — e.g. capital(canberra, australia), opposite(permanent, ephemeral).\n"
    "RULES: atoms lowercase; multi-word strings allowed as plain text; NO double "
    "quotes inside values; translation keys are ONE word each (skip function words "
    "like to/the); no duplicate facts; teach the general class, not a memorized "
    "reply to this exact phrasing; never invent facts you are not confident are true."
)


MULTIPLIER_SYS = (
    "parrot0 just LEARNED a fact and it verifiably closed a question. Your job is to "
    "MULTIPLY that success: output MANY more facts of the SAME schema and domain that "
    "a capable assistant is highly confident are TRUE and COMMON — so parrot0 covers "
    "the whole class, not one point. Example: taught tr_fr(morning, matin) -> emit "
    "tr_fr for many common French words (bread/pain, water/eau, house/maison, ...).\n"
    "Reply STRICT JSON only: {\"facts\": [{\"pred\":\"...\",\"args\":[...]}, ...]} — "
    "up to N facts, SAME predicate schema(s) as the seed. Same rules: atoms lowercase, "
    "one word per translation key, no double quotes, NO duplicates, and ONLY facts you "
    "are confident are correct. If you cannot produce confident siblings, reply "
    "{\"facts\": []}."
)

BATCH_JUDGE_SYS = (
    "You are auditing a batch of knowledge-base facts for TRUTH and correct form. "
    "Vote 1 ONLY if EVERY fact is factually correct and well-formed for its schema "
    "(translations accurate, symbols/riddles/idioms correct). Vote 0 if ANY fact is "
    "wrong, dubious, or malformed. Reply STRICT JSON only: "
    '{"vote":0 or 1,"reason":"one short sentence"}'
)


def call_model(key, model, messages, temperature, max_tokens=900):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-autolearn/1.0"})
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


def one_line(s):
    for ln in s.split("\n"):
        ln = ln.strip().strip('"').strip("'").strip()
        if ln and len(ln) >= 2:
            return ln[:300]
    return s.strip()[:300] or "[empty]"


def first_json(s):
    """Extract the first JSON object from a model reply (tolerate fences/prose)."""
    s = re.sub(r"```(?:json)?", "", s)
    i = s.find("{")
    if i < 0:
        return None
    depth = 0
    for j in range(i, len(s)):
        if s[j] == "{":
            depth += 1
        elif s[j] == "}":
            depth -= 1
            if depth == 0:
                try:
                    return json.loads(s[i:j + 1])
                except Exception:
                    return None
    return None


# ---------------------------------------------------------------- parrot0 I/O
def p0_env(kb_path):
    return {**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": kb_path,
            "PARROT0_EOT": P0_EOT}


def probe(question, kb_path):
    """One question to a fresh parrot0 with the learned session loaded."""
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env=p0_env(kb_path))
    try:
        proc.stdin.write(question + "\n")
        proc.stdin.flush()
        lines = []
        while True:
            ln = proc.stdout.readline()
            if not ln:
                break
            ln = ln.rstrip("\n")
            if ln == P0_EOT:
                break
            lines.append(ln)
        return "\n".join(lines).strip()
    finally:
        try:
            proc.stdin.close()
        except Exception:
            pass
        proc.wait(timeout=10)


def mcp_teach(facts, kb_path):
    """Assert facts over --mcp-engine and persist the session to kb_path.
    Returns (n_stored, errors)."""
    reqs = ['{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}',
            '{"jsonrpc":"2.0","method":"notifications/initialized"}']
    for i, f in enumerate(facts):
        reqs.append(json.dumps({"jsonrpc": "2.0", "id": 10 + i,
                                "method": "tools/call",
                                "params": {"name": "kb.assert",
                                           "arguments": {"pred": f["pred"],
                                                         "args": f["args"]}}}))
    reqs.append(json.dumps({"jsonrpc": "2.0", "id": 999, "method": "tools/call",
                            "params": {"name": "kb.save",
                                       "arguments": {"path": kb_path}}}))
    proc = subprocess.run(["./bin/parrot0", "--mcp-engine"],
                          input="\n".join(reqs) + "\n", text=True,
                          capture_output=True, timeout=30, env=p0_env(kb_path))
    stored = proc.stdout.count('stored\\":true')
    errors = [ln for ln in proc.stdout.splitlines() if '"isError":true' in ln]
    # kb.save re-dumps runtime state facts; strip them so the file stays pure lesson.
    try:
        with open(kb_path) as fh:
            keep = [ln for ln in fh
                    if not re.match(r"(process_pid|os_language|current_language)\(", ln)]
        with open(kb_path, "w") as fh:
            fh.writelines(keep)
    except FileNotFoundError:
        pass
    return stored, errors


def sanitize_lesson(obj, limit=10):
    """Enforce the whitelist/shape rules. Returns (facts, dropped)."""
    facts, dropped = [], []
    for f in (obj or {}).get("facts", [])[:limit]:
        pred = str(f.get("pred", "")).strip()
        args = [str(a).strip().replace('"', "'") for a in f.get("args", [])]
        # Atoms and cues are matched against the LOWERCASED normalized turn, so
        # lowercase every arg except free-text payloads (a response template's
        # sentence, a concept definition), which keep their surface casing.
        keep_case_last = pred in ("response_template", "wiki_concept")
        args = [a if (keep_case_last and i == len(args) - 1) else a.lower()
                for i, a in enumerate(args)]
        ok_pred = re.fullmatch(r"[a-z][a-z0-9_]*", pred) is not None
        arity_ok = (pred in WHITELIST and len(args) == WHITELIST[pred]) or \
                   (pred not in WHITELIST and len(args) == 2)
        if (not ok_pred or pred in BLACKLIST or not arity_ok or
                not all(0 < len(a) <= 120 for a in args)):
            dropped.append(f)
            continue
        facts.append({"pred": pred, "args": args})
    return facts, dropped


def fact_str(f):
    return f'{f["pred"]}({", ".join(f["args"])})'


_PRINT_LOCK = threading.Lock()


def log(msg):
    with _PRINT_LOCK:
        print(msg, flush=True)


def multiply_lesson(key, model, seed_facts, n_max):
    """A verified lesson closed a question -> ask for MANY same-schema siblings and
    audit the whole batch with ONE judge call (truth + form). Returns the batch, or
    [] if the audit fails or nothing survives. The oracle-verified seed is the
    anchor; the siblings are a whitelist-constrained, batch-audited generalization
    (the deliberate precision/recall trade F. chose to accelerate growth)."""
    if n_max <= 0 or not seed_facts:
        return []
    schemas = sorted({f["pred"] for f in seed_facts})
    seed_desc = "; ".join(fact_str(f) for f in seed_facts)
    t = first_json(call_model(key, model, [
        {"role": "system", "content": MULTIPLIER_SYS},
        {"role": "user", "content":
            f"SEED (just verified): {seed_desc}\nSCHEMAS: {', '.join(schemas)}\n"
            f"Emit up to {n_max} more facts of these schema(s)."}],
        0.4, max_tokens=3500))
    sibs, _ = sanitize_lesson(t, limit=n_max * 2)
    sibs = [f for f in sibs if f["pred"] in schemas]   # stay on the seed's schema(s)
    seen, uniq = {(f["pred"], tuple(f["args"])) for f in seed_facts}, []
    for f in sibs:
        k = (f["pred"], tuple(f["args"]))
        if k in seen:
            continue
        seen.add(k)
        uniq.append(f)
    sibs = uniq[:n_max]
    if not sibs:
        return []
    j = first_json(call_model(key, model, [
        {"role": "system", "content": BATCH_JUDGE_SYS},
        {"role": "user", "content": "\n".join(fact_str(f) for f in sibs)}], 0.0)) or {}
    return sibs if int(j.get("vote", 0)) == 1 else []


def do_round(idx, given_q, args, key, base_bytes):
    """One fully independent round on its OWN scratch KB (base + this round's
    lessons). Returns a result dict incl. the facts to keep (verified seed +
    audited siblings). Safe to run in a thread: no shared mutable engine state."""
    scratch = f"{args.kb}.w{idx}"
    with open(scratch, "wb") as fh:
        fh.write(base_bytes)
    try:
        if given_q is not None:
            q = given_q
        else:
            ability = ABILITIES[idx % len(ABILITIES)]
            q = one_line(call_model(key, args.model, [
                {"role": "system", "content": INTERVIEW_SYS},
                {"role": "user", "content": f"Ask one question probing: {ability}."}], 0.9))
        if q.startswith("[model error") or q == "[empty]":
            return {"verdict": "interviewer-error", "q": q, "kept": []}

        a0 = probe(q, scratch) or "(empty)"
        j0 = first_json(call_model(key, args.model, [
            {"role": "system", "content": JUDGE_SYS},
            {"role": "user", "content": f"Q: {q}\nA: {a0}"}], 0.0)) or {}
        if int(j0.get("vote", 0)) == 1:
            log(f"[{idx+1}] already   {q[:70]}")
            return {"verdict": "already-capable", "q": q, "a0": a0,
                    "reason": j0.get("reason", ""), "kept": []}

        lessons, a1, v1, jr = [], a0, 0, ""
        for _ in range(args.retries + 1):
            t = first_json(call_model(key, args.model, [
                {"role": "system", "content": TEACHER_SYS},
                {"role": "user", "content":
                    f"QUESTION: {q}\nPARROT0'S FAILING REPLY: {a1}\n"
                    + (f"FACTS ALREADY TAUGHT THIS ROUND: "
                       f"{'; '.join(fact_str(f) for f in lessons)}\n" if lessons else "")
                    + "Formulate the lesson."}], 0.2, max_tokens=3000))
            if not t or "skip" in (t or {}):
                if not lessons:
                    return {"verdict": "skipped", "q": q, "a0": a0,
                            "reason": (t or {}).get("skip", "no lesson JSON"), "kept": []}
                break
            facts, dropped = sanitize_lesson(t)
            if not facts:
                if not lessons:
                    return {"verdict": "skipped", "q": q, "a0": a0,
                            "reason": f"no whitelisted facts ({len(dropped)} dropped)",
                            "kept": []}
                break
            mcp_teach(facts, scratch)
            lessons += facts
            a1 = probe(q, scratch) or "(empty)"
            j1 = first_json(call_model(key, args.model, [
                {"role": "system", "content": JUDGE_SYS},
                {"role": "user", "content": f"Q: {q}\nA: {a1}"}], 0.0)) or {}
            v1, jr = int(j1.get("vote", 0)), j1.get("reason", "")
            if v1 == 1:
                break

        if lessons and v1 == 1:
            sibs = multiply_lesson(key, args.model, lessons, args.multiply)
            log(f"[{idx+1}] TAUGHT   {q[:56]}  (+{len(lessons)} seed +{len(sibs)} x)")
            return {"verdict": "taught", "q": q, "a0": a0, "a1": a1, "reason": jr,
                    "lesson": [fact_str(f) for f in lessons],
                    "multiplied": [fact_str(f) for f in sibs],
                    "kept": lessons + sibs}
        log(f"[{idx+1}] failed   {q[:70]}")
        return {"verdict": "failed-lesson", "q": q, "a0": a0, "a1": a1, "reason": jr,
                "lesson": [fact_str(f) for f in lessons], "kept": []}
    finally:
        try:
            os.remove(scratch)
        except OSError:
            pass


# ------------------------------------------------------------------ the loop
def run(args, key):
    os.makedirs(os.path.dirname(args.kb), exist_ok=True)
    if not os.path.exists(args.kb):
        open(args.kb, "w").close()

    probes = []
    if args.probes:
        with open(args.probes) as fh:
            probes = [ln.strip() for ln in fh if ln.strip() and not ln.startswith("#")]
    n = min(args.rounds, len(probes)) if probes else args.rounds
    with open(args.kb, "rb") as fh:          # frozen base every worker starts from
        base_bytes = fh.read()

    # Parallel: rounds are independent (own scratch KB); the calls are I/O-bound on
    # the provider, so a thread pool cuts wall-clock ~Wx. Merge is single-threaded.
    results = [None] * n
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as ex:
        fut = {ex.submit(do_round, i, (probes[i] if probes else None), args, key,
                         base_bytes): i for i in range(n)}
        for f in concurrent.futures.as_completed(fut):
            results[fut[f]] = f.result()

    # Merge every worker's kept facts into the canonical KB, deduped, in one teach.
    seen, kept_facts = set(), []
    for r in results:
        for f in r.get("kept", []):
            k = (f["pred"], tuple(f["args"]))
            if k not in seen:
                seen.add(k)
                kept_facts.append(f)
    if kept_facts:
        mcp_teach(kept_facts, args.kb)

    counts = {}
    seed_total = mult_total = 0
    for r in results:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
        if r["verdict"] == "taught":
            seed_total += len(r.get("lesson", []))
            mult_total += len(r.get("multiplied", []))

    stamp = time.strftime("%Y-%m-%d %H:%M:%S")
    with open(args.out, "a") as fh:
        fh.write(f"\n## Run {stamp} — model {args.model}, {n} round(s), "
                 f"{args.workers} workers, multiply x{args.multiply}"
                 f"{' (probes)' if probes else ' (autonomous)'}\n\n")
        fh.write(f"**already {counts.get('already-capable',0)} · "
                 f"taught {counts.get('taught',0)} · failed {counts.get('failed-lesson',0)} "
                 f"· skipped {counts.get('skipped',0)} · "
                 f"kept {len(kept_facts)} facts ({seed_total} seed + {mult_total} "
                 f"multiplied, deduped)**\n\n")
        for i, r in enumerate(results, 1):
            fh.write(f"### Round {i} — {r.get('verdict','?')}\n- Q: {r.get('q','')}\n")
            if "a0" in r:
                fh.write(f"- before: {r['a0']}\n")
            if r.get("lesson"):
                fh.write(f"- lesson: {'; '.join(r['lesson'])}\n")
            if r.get("multiplied"):
                fh.write(f"- multiplied (+{len(r['multiplied'])}): "
                         f"{'; '.join(r['multiplied'][:24])}\n")
            if "a1" in r and r.get("verdict") != "already-capable":
                fh.write(f"- after: {r['a1']}\n")
            if r.get("reason"):
                fh.write(f"- judge: {r['reason']}\n")
            fh.write("\n")

    log(f"\nautolearn: already {counts.get('already-capable',0)}, "
        f"taught {counts.get('taught',0)}, failed {counts.get('failed-lesson',0)}, "
        f"skipped {counts.get('skipped',0)} | KEPT {len(kept_facts)} facts "
        f"({seed_total} seed + {mult_total} multiplied)")
    log(f"report appended: {args.out}\nknowledge: {args.kb}")
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--rounds", type=int, default=5)
    ap.add_argument("--probes", help="file with one probe question per line "
                                     "(controlled mode; default: autonomous interviewer)")
    ap.add_argument("--model", default="minimax-m2.5",
                    help="opencode-GO slug for interviewer/judge/teacher")
    ap.add_argument("--kb", default="kb/learning/autolearn.p0",
                    help="persistent learned-knowledge file (repo-relative)")
    ap.add_argument("--retries", type=int, default=2,
                    help="extra teach attempts while the decline names new gaps")
    ap.add_argument("--workers", type=int, default=5,
                    help="parallel rounds (I/O-bound on the provider)")
    ap.add_argument("--multiply", type=int, default=20,
                    help="on a verified lesson, add up to N batch-audited same-schema "
                         "siblings (0 disables)")
    ap.add_argument("--out", default="AUTOLEARN.md")
    args = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("autolearn: OPENCODE_API_KEY not set", file=sys.stderr)
        return 1
    if not os.path.exists("./bin/parrot0"):
        print("autolearn: build first (make build)", file=sys.stderr)
        return 1
    return run(args, key)


if __name__ == "__main__":
    sys.exit(main())
