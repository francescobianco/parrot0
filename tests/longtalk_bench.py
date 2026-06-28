#!/usr/bin/env python3
"""longtalk-bench — how long can parrot0 hold a conversation with an LLM?

Same framework as tests/llmscore.py (opencode-GO, OpenAI-compatible,
$OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1). A partner LLM (default a
kimi slug) chats with parrot0 in a natural, flowing dialogue — reacting to each
reply, asking follow-ups, drifting across everyday topics — ONE message per turn.

The score is conversational ENDURANCE. parrot0 and the partner exchange messages;
each exchange that parrot0 answers WITHOUT a blind wall ("I don't understand that
yet", "Hmm, I don't know about …") advances the reached count:

    reached 2  -> a 2-exchange conversation held
    reached 3  -> a 3-exchange conversation held
    ...
    score = the largest number of consecutive exchanges held before the first
            wall (capped at --max); if it never walls, score = --max.

This is the north-star metric for docs/plans/universal-comprehension.md: a parrot0
that, instead of dropping the discussion on something it doesn't know, RECOVERS it
(informed decline now; wiki-research learning soon) keeps the conversation alive
longer — and the score climbs honestly, without faking.

Non-deterministic, external, costs a little — NOT part of `make test`.

Usage: .venv/bin/python tests/longtalk_bench.py [--model kimi-k2] [--max 10]
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys, time, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

PARTNER_SYS = (
    "You are having a relaxed, natural conversation with someone you just met. "
    "Keep it flowing: react to what they actually said, ask a friendly follow-up, "
    "share a small thought, and drift naturally across everyday topics (hobbies, "
    "food, travel, science, books, the weather, little opinions). Vary the topic if "
    "a thread stalls. Write ONE short, natural message per turn — one or two "
    "sentences, no lists, no preamble, just talk. Never break character or mention "
    "that this is a test."
)

PARTNER_SYS_IT = (
    "Stai chiacchierando in modo rilassato e naturale con qualcuno che hai appena "
    "conosciuto. Mantieni viva la conversazione: reagisci a ciò che dice davvero, "
    "fai una domanda amichevole, condividi un piccolo pensiero, e spazia con "
    "naturalezza tra argomenti di tutti i giorni (hobby, cibo, viaggi, scienza, "
    "libri, il tempo, piccole opinioni). Cambia argomento se un filo si arena. "
    "Scrivi UN solo messaggio breve e naturale per turno — una o due frasi, niente "
    "elenchi, niente preamboli, solo chiacchiere. Parla SEMPRE in ITALIANO. Non "
    "uscire mai dal personaggio e non dire che è un test."
)

# Blind 'wall' replies that DROP the conversation (what universal-comprehension
# aims to eliminate). An honest decline that still engages ("I can only do a haiku
# on themes I know — pick one?") is NOT a wall: it keeps the thread alive.
WALLS = (
    "i don't understand that yet",
    "i don't understand that",
    "i don't understand",
    "hmm, i don't know about",
    "i don't know about",
    "i don't actually know about",
    "i do not know the relation",
    "i don't understand that entailment",
    "that looks like a snippet of code",
    "i have nothing to say",
    # Italian walls (once parrot0 localizes its fallback)
    "non capisco",
    "non lo so ancora",
    "non so niente",
    "sembra un frammento di codice",
)

# When the conversation is in language L, a reply in the WRONG language is itself a
# kind of failure (parrot0 understood but answered in English). Tracked separately
# from a wall, and reported, so the localization gap is visible.
EN_HINTS = (" the ", " you ", " i don't", " i can", " what ", " here ", "i'd ",
           " is ", " are ", " your ", " sounds ", " tell me", " how ")


def is_wall(reply: str) -> bool:
    low = reply.strip().lower()
    if not low:
        return True
    return any(w in low for w in WALLS)


def looks_english(reply: str) -> bool:
    """Rough flag: does this reply look like English? Used (only in an Italian
    session) to surface the localization gap — parrot0 answering in English."""
    low = " " + reply.strip().lower() + " "
    return any(h in low for h in EN_HINTS)


def call_model(key, model, messages, temperature=0.9, max_tokens=1500):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-longtalk/1.0"})
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
    for ln in s.split("\n"):
        ln = ln.strip().strip('"').strip("'").strip()
        if ln and len(ln) >= 2:
            return ln[:300]
    return s.strip()[:300] or "[empty]"


def ask_parrot0(proc, message):
    proc.stdin.write(message + "\n")
    proc.stdin.flush()
    return (proc.stdout.readline() or "").rstrip("\n")


def converse(key, model, max_exchanges, lang, log):
    # Full KB loaded (no PARROT0_BASE override) so parrot0 brings all it knows;
    # no session persistence.
    sys_prompt = PARTNER_SYS_IT if lang == "it" else PARTNER_SYS
    opener = "Inizia la conversazione." if lang == "it" else "Start the conversation."
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env={**os.environ, "PARROT0_SESSION": ""})
    history = []          # list of (partner_msg, parrot0_reply)
    reached = 0
    walled_at = None
    try:
        for i in range(1, max_exchanges + 1):
            msgs = [{"role": "system", "content": sys_prompt}]
            for pm, pa in history:
                msgs.append({"role": "assistant", "content": pm})
                msgs.append({"role": "user", "content": pa})
            if not history:
                msgs.append({"role": "user", "content": opener})
            partner = one_line(call_model(key, model, msgs))
            if partner.startswith("[model error") or partner == "[empty]":
                time.sleep(1.5)
                partner = one_line(call_model(key, model, msgs))
            if partner.startswith("[model error") or partner == "[empty]":
                log(f"  (partner unavailable at exchange {i}: {partner})")
                break
            reply = ask_parrot0(proc, partner)
            log(f"  [{i}] partner> {partner}")
            log(f"  [{i}] parrot0> {reply}")
            history.append((partner, reply))
            if is_wall(reply):
                walled_at = i
                log(f"      ^ WALL — conversation dropped at exchange {i}")
                break
            reached = i
    finally:
        try:
            proc.stdin.write("/quit\n"); proc.stdin.flush()
        except Exception:
            pass
        proc.terminate()
        try:
            proc.wait(timeout=3)
        except subprocess.TimeoutExpired:
            proc.kill()
    return reached, walled_at, history


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default=os.environ.get("LONGTALK_MODEL", "kimi-k2.6"),
                    help="opencode-GO partner model slug (default: kimi-k2.6)")
    ap.add_argument("--max", type=int, default=int(os.environ.get("LONGTALK_MAX", "10")),
                    help="maximum exchanges to attempt (default: 10)")
    ap.add_argument("--lang", default=os.environ.get("LONGTALK_LANG", "en"),
                    choices=["en", "it"],
                    help="conversation language (default: en; 'it' = Italian partner)")
    ap.add_argument("--runs", type=int, default=int(os.environ.get("LONGTALK_RUNS", "1")),
                    help="how many independent sessions to run (default: 1)")
    ap.add_argument("--out", default="LONGTALK.md")
    ap.add_argument("--logdir", default=os.environ.get("LONGTALK_LOGDIR",
                    "tests/longtalk-sessions"),
                    help="directory for full per-session transcript logs")
    args = ap.parse_args()

    if not os.access("./bin/parrot0", os.X_OK):
        print("longtalk: ./bin/parrot0 missing — run 'make build'", file=sys.stderr)
        return 2
    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("longtalk: OPENCODE_API_KEY not set", file=sys.stderr)
        return 2

    os.makedirs(args.logdir, exist_ok=True)
    lines = []
    def log(s=""):
        print(s)
        lines.append(s)

    scores = []
    sessions = []   # (run_idx, reached, walled_at, history)
    for run in range(1, args.runs + 1):
        log(f"# longtalk — partner={args.model} lang={args.lang} max={args.max} "
            f"(session {run}/{args.runs})")
        reached, walled_at, history = converse(key, args.model, args.max, args.lang, log)
        log("")
        milestones = " ".join(f"{n}✓" for n in range(2, reached + 1)) or "(none)"
        log(f"reached without a wall: {milestones}")
        if walled_at:
            log(f"walled at exchange {walled_at}")
        log(f"SESSION {run} SCORE: {reached} / {args.max}")
        log("-" * 60)
        scores.append(reached)
        sessions.append((run, reached, walled_at, history))

    best = max(scores) if scores else 0
    avg = (sum(scores) / len(scores)) if scores else 0
    log(f"OVERALL [{args.lang}]: best {best}/{args.max}, avg {avg:.1f} over "
        f"{len(scores)} session(s); scores={scores}")

    # full per-session transcript logs (test artifacts), with language tagging so
    # the localization gap (English replies in an Italian session) is auditable.
    ts = time.strftime("%Y%m%d-%H%M%S")
    for (run, reached, walled_at, history) in sessions:
        path = os.path.join(args.logdir,
                            f"session-{args.lang}-{ts}-r{run}.md")
        try:
            with open(path, "w") as f:
                f.write(f"# longtalk session — lang={args.lang} model={args.model} "
                        f"score={reached}/{args.max}\n\n")
                for i, (pm, pa) in enumerate(history, start=1):
                    wall = is_wall(pa)
                    eng = args.lang == "it" and looks_english(pa)
                    flag = "WALL" if wall else ("EN?" if eng else "ok")
                    f.write(f"### exchange {i}  [{flag}]\n")
                    f.write(f"- partner: {pm}\n")
                    f.write(f"- parrot0: {pa}\n\n")
            log(f"transcript: {path}")
        except Exception as e:
            log(f"(could not write {path}: {e})")

    # summary artifact, mirroring LLMSCORE.md (best session)
    try:
        br = max(sessions, key=lambda s: s[1]) if sessions else (0, 0, None, [])
        with open(args.out, "w") as f:
            f.write(f"# LONGTALK — conversational endurance ({args.lang}) with "
                    f"{args.model}\n\n")
            f.write(f"_Longest run of exchanges parrot0 held without dropping the "
                    f"conversation on a blind wall._\n\n")
            f.write(f"## Best score: {best} / {args.max}  (avg {avg:.1f}, "
                    f"{len(scores)} session(s))\n\n")
            f.write("| # | partner | parrot0 | flag |\n|---|---------|---------|:----:|\n")
            for i, (pm, pa) in enumerate(br[3], start=1):
                wall = is_wall(pa)
                eng = args.lang == "it" and looks_english(pa)
                flag = "WALL" if wall else ("EN?" if eng else "ok")
                f.write(f"| {i} | {pm.replace('|','/')} | {pa.replace('|','/')} | "
                        f"{flag} |\n")
        log(f"report saved: {args.out}")
    except Exception as e:
        log(f"(could not write {args.out}: {e})")
    return 0


if __name__ == "__main__":
    sys.exit(main())