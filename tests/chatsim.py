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
    "a curious teenager", "a busy office worker on a coffee break",
    "an elderly person new to computers", "a non-native English speaker",
    "a sceptical software tester", "a chatty extrovert", "a tired parent",
    "a bored student", "a poet who likes wordplay", "a blunt engineer",
    "a kid asking endless why-questions", "someone a bit grumpy today",
    "a cheerful small-talker",
]
MOOD = ["cheerful", "impatient", "confused", "playful", "skeptical", "warm",
        "distracted", "deadpan", "enthusiastic", "a little annoyed"]
VERBOSITY = ["Keep most messages very short (1-6 words).",
             "Write casual one-liners.",
             "Sometimes ramble, sometimes go terse.",
             "Mix short and medium messages."]
STYLE = ["lowercase, no punctuation", "with occasional typos",
         "with an emoji now and then", "fairly formal", "slangy and casual",
         "plain and neutral"]
LANG = ["Write in English.", "Write in English.",
        "Write in Italian.", "Mix Italian and English casually."]
GOAL = ["just make small talk", "test if the bot is smart by probing it",
        "ask it a couple of real questions", "vent a little then chat",
        "ask it to do something simple", "see if it remembers what you said",
        "greet it and see what happens", "try to confuse it for fun"]
QUIRK = ["Occasionally change the topic abruptly.",
         "Sometimes answer a question with a question.",
         "Drop in a personal detail (a pet, a city, a hobby).",
         "Now and then react to how the bot is doing (bored/impressed/puzzled).",
         "Use the bot's name sometimes.",
         "Occasionally be a bit cheeky.", ""]

def persona() -> str:
    return (
        f"You are role-playing a HUMAN in a chat with 'parrot0', a tiny "
        f"experimental chatbot. You are {random.choice(WHO)}, feeling "
        f"{random.choice(MOOD)}. Your aim: {random.choice(GOAL)}. "
        f"{random.choice(VERBOSITY)} Style: {random.choice(STYLE)}. "
        f"{random.choice(LANG)} {random.choice(QUIRK)} "
        f"Behave like a real, unpredictable person — vary your wording every turn. "
        f"If the bot is confusing or dull, react like a human (impatience, a joke, "
        f"change subject, push back). Output ONLY your next chat message: no quotes, "
        f"no narration, no role labels."
    ).replace("\n", " ")

def call_model(key: str, model: str, messages: list, temperature: float) -> str:
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": 120, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        # the gateway 403s urllib's default UA; send an explicit one
        "User-Agent": "parrot0-chatsim/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=90) as r:
            d = json.loads(r.read())
    except urllib.error.HTTPError as e:
        return f"[model error {e.code}]"
    except Exception as e:  # network/timeout
        return f"[model error {e}]"
    try:
        c = d["choices"][0]["message"]["content"] or ""
    except Exception:
        return "[empty]"
    # some models leak chain-of-thought into content; strip it
    c = re.sub(r"<think>.*?</think>", "", c, flags=re.S)
    c = c.replace("<think>", "").replace("</think>", "").strip()
    for ln in c.split("\n"):
        ln = ln.strip()
        if ln and not ln.lower().startswith(("the user", "the human", "the parrot")):
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
                msgs.append({"role": "user",
                             "content": "(You open the chat with parrot0. Write your first message.)"})
            human = call_model(key, model, msgs, temp)
            if human.startswith("[model error") or human == "[empty]":
                time.sleep(1.5)  # transient: one retry
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

    # naturalness proxies
    wall = sum(1 for b in all_bot if b == "I don't understand that yet.")
    dup = sum(1 for j in range(1, len(all_bot)) if all_bot[j] and all_bot[j] == all_bot[j-1])
    n = len(all_bot) or 1
    log("\n--- summary ---")
    log(f"bot turns: {len(all_bot)}")
    log(f'"I don\'t understand that yet." wall rate: {100*wall//n}% ({wall}/{n})')
    log(f"immediate repetition rate: {100*dup//n}% ({dup}/{n})")
    with open(out, "w") as f: f.write("\n".join(lines) + "\n")
    log(f"\ntranscript saved: {out}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
