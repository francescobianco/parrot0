#!/usr/bin/env python3
"""chatsim — an LLM impersonates a (mutable, human) user chatting with parrot0.

This is the GENERATIVE conversation benchmark, complementing C0 (tests/chatbench.sh,
which is hand-scripted and deterministic). A small/cheap opencode-GO model role-plays
a human with a HIGHLY VARIABLE persona (identity, mood, verbosity, language, quirks,
per-turn behaviour), drives a multi-turn chat with parrot0, and every exchange is
logged. The point is to surface naturalness failures we did not anticipate, then feed
them back into the C-series (conversational intelligence) — within the main mission:
find the structure that makes conversation feel intelligent.

Provider: opencode-GO (OpenAI-compatible), base https://opencode.ai/zen/go/v1,
auth via $OPENCODE_API_KEY. Non-deterministic by design: it produces transcripts +
a naturalness summary, NOT a pass/fail gate.

Usage: .venv/bin/python tests/chatsim.py --convos 5 --turns 6 --model minimax-m2.5
"""
from __future__ import annotations
import argparse, json, os, random, re, subprocess, sys, time, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

# --- persona components: assembled randomly so the human is mutable over time ---
WHO = [
    "a curious teenager", "a busy office worker", "an elderly person",
    "a non-native speaker", "a sceptical tester", "a chatty person",
    "a tired parent", "a bored student", "a blunt engineer",
    "someone a bit grumpy", "a cheerful person",
]
MOOD = ["cheerful", "impatient", "confused", "playful", "skeptical", "warm",
        "distracted", "deadpan", "enthusiastic", "annoyed"]
LANG = ["English", "English", "Italian", "Italian and English mixed"]
STYLE = ["lowercase, no punctuation", "with occasional typos",
         "short messages", "casual", "plain"]

def persona() -> str:
    """Return a minimal system prompt — one line, no meta-task framing."""
    who = random.choice(WHO)
    mood = random.choice(MOOD)
    lang = random.choice(LANG)
    style = random.choice(STYLE)
    return (
        f"You are {who}, {mood}. Chat with parrot0 as a real person. "
        f"Reply in {lang}. Style: {style}. "
        f"Keep replies short (1-8 words). "
        f"DO NOT plan, narrate, or analyze. Just the message."
    )

def call_model(key: str, model: str, messages: list, temperature: float) -> str:
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": 120, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-chatsim/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=90) as r:
            d = json.loads(r.read())
    except urllib.error.HTTPError as e:
        return f"[model error {e.code}]"
    except Exception as e:
        return f"[model error {e}]"
    try:
        c = d["choices"][0]["message"]["content"] or ""
    except Exception:
        return "[empty]"
    # strip think tags and filter meta/planning lines
    c = re.sub(r"<think>.*?</think>", "", c, flags=re.S)
    c = c.replace("<think>", "").replace("</think>", "").strip()
    _META = ("the user", "the human", "the parrot", "the bot ", "i need to",
             "i should", "looking at", "previous", "conversation history",
             "system prompt", "my role", "role-playing", "output only",
             "key character", "key constraint", "current state:",
             "wait,", "let me", "actually,", "1.", "- system:",
             "characteristics:", "constraints to")
    for ln in c.split("\n"):
        ln = ln.strip().strip('"').strip("'")
        low = ln.lower()
        if ln and len(ln) >= 2 and not any(low.startswith(p) for p in _META):
            return ln[:200]
    return "[empty]"

def run_conversation(key, model, turns, log) -> list:
    sysmsg = persona()
    temp = round(random.uniform(0.9, 1.3), 2)
    log(f"# persona (temp={temp}): {sysmsg}")
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": ""})
    history, rows = [], []
    try:
        for t in range(turns):
            msgs = [{"role": "system", "content": sysmsg}]
            for h, b in history:
                msgs.append({"role": "assistant", "content": h})
                msgs.append({"role": "user", "content": b})
            if not history:
                msgs.append({"role": "user", "content": "say hi"})
            human = call_model(key, model, msgs, temp)
            if human.startswith("[model error") or human == "[empty]":
                time.sleep(1.5)
                human = call_model(key, model, msgs, temp)
            if human.startswith("[model error") or human == "[empty]":
                log(f"  (stopping: {human})"); break
            proc.stdin.write(human + "\n"); proc.stdin.flush()
            bot = (proc.stdout.readline() or "").rstrip("\n")
            log(f"  human> {human}")
            log(f"  parrot0> {bot}")
            history.append((human, bot)); rows.append(bot)
    finally:
        try: proc.stdin.write("/quit\n"); proc.stdin.flush()
        except Exception: pass
        proc.terminate()
    return rows

def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--convos", type=int, default=5)
    ap.add_argument("--turns", type=int, default=6)
    ap.add_argument("--model", default="minimax-m2.5")
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("chatsim: OPENCODE_API_KEY not set", file=sys.stderr); return 2
    if not os.access("./bin/parrot0", os.X_OK):
        print("chatsim: build first (make build)", file=sys.stderr); return 2

    os.makedirs("tests/chat/sim", exist_ok=True)
    out = args.out or f"tests/chat/sim/{time.strftime('%Y%m%d-%H%M%S')}.log"
    lines = []
    def log(s): lines.append(s); print(s)

    log(f"# chatsim — model={args.model} convos={args.convos} turns={args.turns}")
    random.seed()
    all_bot = []
    for i in range(args.convos):
        log(f"\n=== conversation {i+1} ===")
        all_bot += run_conversation(key, args.model, args.turns, log)

    # naturalness proxies — gen70: wall rate now catches the varied fallback
    # responses gen55 introduced, not only the classic line.
    _WALL_PATTERNS = [
        "I don't understand that yet.",
        "I'm not sure I followed",
        "I didn't quite catch that",
        "Hmm, that's a bit beyond me right now",
        "Hmm, I don't know about ",
    ]
    def _is_wall(s: str) -> bool:
        if not s: return False
        for p in _WALL_PATTERNS:
            if p in s: return True
        return False
    wall = sum(1 for b in all_bot if _is_wall(b))
    dup = sum(1 for j in range(1, len(all_bot)) if all_bot[j] and all_bot[j] == all_bot[j-1])
    n = len(all_bot) or 1
    classic_ct = sum(1 for b in all_bot if b == "I don't understand that yet.")
    reflect_ct = sum(1 for b in all_bot if b.startswith("Hmm, I don't know about "))
    other_wall = wall - classic_ct - reflect_ct
    log("\n--- summary ---")
    log(f"bot turns: {len(all_bot)}")
    log(f"wall rate (all fallback variants): {100*wall//n}% ({wall}/{n})")
    log(f'  of which classic "I don\'t understand that yet.": {classic_ct}')
    log(f"  of which word-reflecting: {reflect_ct}")
    log(f"  of which other variants: {other_wall}")
    log(f"immediate repetition rate: {100*dup//n}% ({dup}/{n})")
    with open(out, "w") as f: f.write("\n".join(lines) + "\n")
    log(f"\ntranscript saved: {out}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
