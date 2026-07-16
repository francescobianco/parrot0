#!/usr/bin/env python3
"""selfchat-baseline — an LLM talks to ITSELF, saved as a GOLD reference for the
long-conversation mission (docs/plans/long-conversation.md).

longtalk_bench.py measures how long parrot0 holds a conversation WITH an LLM. This
tool makes the LLM converse with a second instance of ITSELF, so we capture what a
naturally coherent long dialogue LOOKS like — the dynamic parrot0 must reproduce:
cross-turn coreference, continuation ("and of Italy?"), callbacks to earlier turns,
topic drift, corrections. These transcripts are the baseline to evolve parrot0's
glue against — not a pass/fail gate.

Same framework/provider/auth as tests/llmscore.py (opencode-GO, OpenAI-compatible,
$OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1). Non-deterministic, external,
costs a little — NOT part of `make test`.

Two seats (A, B) run the SAME model. Each seat sees the running transcript (its own
lines as assistant, the other's as user) plus a short persona that gives the dialogue
a spine, so two identical models don't collapse into a politeness loop.

Usage:
  .venv/bin/python tests/longchat/selfchat_baseline.py [--model minimax-m2.5]
        [--turns 18] [--scenario acquaintance|trip|opinions|all] [--outdir tests/longchat/baselines]
"""
from __future__ import annotations
import argparse, json, os, re, sys, urllib.request, urllib.error, datetime as dt

BASE = "https://opencode.ai/zen/go/v1/chat/completions"


def call_model(key, model, messages, temperature=0.8, max_tokens=800):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-selfchat/1.0"})
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


def clean(s):
    """One tidy chat message: drop meta/planning lines, keep it short."""
    s = re.sub(r"<think>.*?</think>", "", s, flags=re.S).strip()
    out = []
    for ln in s.split("\n"):
        ln = ln.strip()
        low = ln.lower()
        if not ln:
            continue
        if any(low.startswith(p) for p in ("(", "[", "as ", "note:", "i should", "i need to")):
            continue
        out.append(ln)
    return " ".join(out)[:600] or s.strip()[:600]


# --- scenarios: each seat gets a persona; the SEED is A's opening line. The personas
#     are chosen to elicit the 5 glue symptoms the plan targets (cross-turn reference,
#     continuation, callback, correction, topic drift). ---
SCENARIOS = {
    "acquaintance": {
        "topic": "two people getting to know each other",
        "A": ("You are Mara, warm and curious. You are meeting someone new and want to "
              "get to know them: ask about their name, work, home town, hobbies, and "
              "REFER BACK to things they told you earlier in the chat. Keep replies to "
              "1-3 sentences. Ask a follow-up most turns. Never narrate; just talk."),
        "B": ("You are Diego, friendly but a bit reserved. Share real-sounding details "
              "about yourself (name, a job, a home town, a hobby) and stay consistent "
              "with what you already said. React to Mara and occasionally ask her back. "
              "Keep replies to 1-3 sentences. Never narrate; just talk."),
        "seed": "Hi! I'm Mara. I don't think we've met — what's your name?",
    },
    "trip": {
        "topic": "two friends planning a weekend trip together",
        "A": ("You are planning a weekend trip with a friend. Drive the plan forward: "
              "propose a destination, then build on each other's ideas with SHORT "
              "follow-ups and continuations like 'and for food?', 'what about the "
              "budget?'. Refer to decisions you already made together. 1-3 sentences. "
              "Just talk, no narration."),
        "B": ("You are the friend planning the weekend trip. Agree, push back, refine, "
              "and keep the plan CONSISTENT with what you both already decided. Use "
              "short continuations and pronouns for things already named. 1-3 "
              "sentences. Just talk, no narration."),
        "seed": "Okay, let's finally plan this weekend away. I'm thinking mountains — you in?",
    },
    "opinions": {
        "topic": "two people chatting and drifting across topics, with mild disagreement",
        "A": ("You are chatting casually with a friend. Let the topic DRIFT naturally "
              "(a film, then food, then travel...), sometimes CORRECT yourself ('no, I "
              "meant...'), and call back to something said earlier. Mild, friendly "
              "disagreement is welcome. 1-3 sentences. Just talk, no narration."),
        "B": ("You are the friend. Follow the drift, disagree gently sometimes, ask "
              "'why?', and reference earlier points. If misunderstood, clarify. 1-3 "
              "sentences. Just talk, no narration."),
        "seed": "I finally watched that space movie everyone raves about. Honestly? Overrated.",
    },
}


def run_session(key, model, scenario, turns):
    sc = SCENARIOS[scenario]
    transcript = []  # list of (speaker, text)
    transcript.append(("A", sc["seed"]))
    # Each seat keeps its OWN message list (its lines = assistant, other's = user).
    for i in range(turns):
        speaker = "B" if i % 2 == 0 else "A"
        persona = sc[speaker]
        msgs = [{"role": "system", "content":
                 persona + " This is an ongoing conversation; stay coherent with "
                 "EVERYTHING said so far."}]
        for who, text in transcript:
            role = "assistant" if who == speaker else "user"
            msgs.append({"role": role, "content": text})
        reply = clean(call_model(key, model, msgs))
        transcript.append((speaker, reply))
        print(f"  [{scenario}] turn {i+1}/{turns} ({speaker}): {reply[:70]}", file=sys.stderr)
        if reply.startswith("[model error") or reply == "[empty]":
            break
    return transcript


PRONOUNS = re.compile(r"\b(he|she|it|they|him|her|them|his|its|their|that|this|those|these)\b", re.I)
CONT = re.compile(r"^\s*(and|so|but|also|what about|how about|then|e |ma |anche|poi)\b", re.I)


def profile(transcript):
    """A heuristic profile of the coherence DYNAMICS an LLM uses naturally — the
    phenomena the long-conversation plan wants parrot0 to reproduce. Heuristic, not
    ground truth: it characterises the baseline, it does not grade it."""
    turns = transcript[1:]  # exclude the seed
    words = [len(t.split()) for _, t in turns] or [0]
    pron = sum(len(PRONOUNS.findall(t)) for _, t in turns)
    cont = sum(1 for _, t in turns if CONT.match(t))
    quest = sum(t.count("?") for _, t in turns)
    return {
        "turns": len(transcript),
        "avg_words_per_turn": round(sum(words) / max(1, len(words)), 1),
        "cross_turn_pronoun_refs": pron,
        "continuation_openers": cont,
        "questions_asked": quest,
    }


def save(transcript, prof, scenario, model, outdir, stamp):
    os.makedirs(outdir, exist_ok=True)
    base = os.path.join(outdir, f"selfchat-{scenario}-{model.replace('/', '_')}")
    with open(base + ".json", "w") as f:
        json.dump({"scenario": scenario, "model": model, "generated": stamp,
                   "topic": SCENARIOS[scenario]["topic"], "profile": prof,
                   "transcript": [{"speaker": s, "text": t} for s, t in transcript]},
                  f, ensure_ascii=False, indent=2)
    with open(base + ".md", "w") as f:
        f.write(f"# Self-chat baseline — {scenario}\n\n")
        f.write(f"> Model **{model}** talking to itself · {SCENARIOS[scenario]['topic']} · "
                f"generated {stamp}\n>\n")
        f.write(f"> Dynamics (heuristic): {json.dumps(prof, ensure_ascii=False)}\n\n---\n\n")
        for s, t in transcript:
            f.write(f"**{s}:** {t}\n\n")
    return base


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="minimax-m2.5")
    ap.add_argument("--turns", type=int, default=18)
    ap.add_argument("--scenario", default="all")
    ap.add_argument("--outdir", default="tests/longchat/baselines")
    args = ap.parse_args()
    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("selfchat-baseline: set OPENCODE_API_KEY", file=sys.stderr)
        sys.exit(1)
    scenarios = list(SCENARIOS) if args.scenario == "all" else [args.scenario]
    stamp = dt.datetime.now().strftime("%Y-%m-%d %H:%M")
    index = []
    for sc in scenarios:
        print(f"=== scenario: {sc} ({args.turns} turns) ===", file=sys.stderr)
        tr = run_session(key, args.model, sc, args.turns)
        prof = profile(tr)
        base = save(tr, prof, sc, args.model, args.outdir, stamp)
        index.append((sc, prof, base))
        print(f"  saved {base}.md/.json  profile={prof}", file=sys.stderr)
    print("\n=== BASELINE SUMMARY ===")
    for sc, prof, base in index:
        print(f"{sc:14s} {os.path.basename(base)}  {prof}")


if __name__ == "__main__":
    main()