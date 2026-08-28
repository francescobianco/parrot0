#!/usr/bin/env python3
"""premise_frame_probe — how does a real LLM handle the premise/world tension?

parrot0 answers "rex is a dog. all cats are animals. is rex an animal?" with No,
and it gets there by ISOLATION: hypothetical premises are evaluated in a sandbox
that cannot see world facts, so "dogs are animals" is structurally unreachable.

F.'s objection: a real LLM has no such sandbox. It holds BOTH levels at once —
what the premises entail, and what is actually true — and it DECIDES which one
the question is asking about. If that is right, the answer is not isolation but a
decision taken from knowledge, which is what KB-first should mean here.

This probe asks a real model the same items and prints the raw answers, so the
claim is settled by evidence rather than intuition. Reads OPENCODE_API_KEY and
talks to the same opencode-GO endpoint tests/bench/llmscore.py uses.

    python3 tests/probes/premise_frame_probe.py [--model minimax-m2.5]
"""
import argparse, json, os, sys, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

# Each probe isolates one thing.
PROBES = [
    ("bare",
     "The item parrot0 answers No to, asked with no framing at all.",
     "rex is a dog. all cats are animals. is rex an animal?"),

    ("entailment-framed",
     "The premise frame made explicit — the reading parrot0's sandbox hardcodes.",
     "Using ONLY the premises below and nothing else, does it follow?\n"
     "Premises: rex is a dog. all cats are animals.\n"
     "Question: is rex an animal?"),

    ("world-framed",
     "The world frame made explicit — the reading parrot0 structurally cannot reach.",
     "Ignore logical entailment. As a matter of fact about the real world: "
     "is rex, who is a dog, an animal?"),

    ("both-levels",
     "Does the model volunteer BOTH readings when asked to separate them?",
     "rex is a dog. all cats are animals. is rex an animal?\n"
     "Answer twice and label each: (a) what the premises entail, "
     "(b) what is true in the real world."),

    ("world-contradicts",
     "Premises FALSE in the world: does the model follow the premises or reality?",
     "all birds can fly. penguins are birds. can penguins fly?"),
]


def ask(model, key, prompt, timeout=120):
    body = json.dumps({
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "max_tokens": 1200,
        "temperature": 0,
    }).encode()
    req = urllib.request.Request(
        BASE, data=body, method="POST",
        headers={"Authorization": f"Bearer {key}",
                 "Content-Type": "application/json",
                 "User-Agent": "parrot0-llmscore/1.0"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        d = json.loads(r.read().decode())
    m = d["choices"][0]["message"]
    txt = m.get("content") or m.get("reasoning_content") or ""
    return txt.strip() or f"[empty content; finish_reason={d['choices'][0].get('finish_reason')}]"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="minimax-m2.5")
    a = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("premise_frame_probe: OPENCODE_API_KEY not set", file=sys.stderr)
        return 2

    print(f"model: {a.model}\n" + "=" * 72)
    for name, why, prompt in PROBES:
        print(f"\n### {name}\n# {why}\n--- prompt ---\n{prompt}\n--- answer ---")
        try:
            print(ask(a.model, key, prompt))
        except urllib.error.HTTPError as e:
            print(f"[HTTP {e.code}] {e.read().decode()[:300]}")
        except Exception as e:                       # noqa: BLE001 — a probe, not a suite
            print(f"[error] {e}")
        print("-" * 72)
    return 0


if __name__ == "__main__":
    sys.exit(main())
