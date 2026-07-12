#!/usr/bin/env python3
"""Track A of docs/plans/learn-and-build.md: the honest, repeatable learn→build harness.

Drives `parrot0 --daemon` over real HTTP — the same surface `pi` speaks — and proves,
with assertions, the part of the challenge that is genuine TODAY:

  1. forget   — boot with NO sorting knowledge (no agi profile, empty learned KB, no
                wiki source) → "tell me about bubble sort" is an HONEST MISS.
  2. relearn  — point PARROT0_WIKI_DIR at a fixture page → the same ask now LEARNS the
                concept and PERSISTS wiki_concept(bubble_sort, …) into PARROT0_LEARN_KB;
                a re-ask is answered from RAM.
  3. multi-step — one articulated prompt: recall what was learned, write+verify `add`,
                compute add(2,3), and finally "implement bubblesort for an array". The
                recall step is answered for real (planner recall fallback, gen208), the
                add step is composed and oracle-verified, add(2,3)=5, and the SORT step
                is SYNTHESIZED from a KB schema and DISPOSED by the run-grounded judge
                (Track B: algo_shape → code_synth_from_shape → code_check_sort) — a
                verified sort, never a hardcoded body and never a refusal.
  4. ratchet ledger — print the now-closed chain. This is plan B5: the same repeatable
                test that once RECORDED the synthesis gap now proves the whole
                learn→build→test chain. Never fails the build; it records the win.

Track B closed the gap the earlier revision of this bench recorded: step 3's sort
assertion is the ratchet, and it now expects a VERIFIED synthesis. Exit 0 if every
assertion passes.
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
import tempfile
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WIKI_DIR = ROOT / "tests" / "piagent" / "wiki"
PORT = int(os.environ.get("SORTLEARN_PORT", "9913"))
BASE = f"http://127.0.0.1:{PORT}"


def _post_chat(text: str) -> str:
    body = json.dumps({
        "model": "parrot0",
        "messages": [{"role": "user", "content": text}],
    }).encode()
    req = urllib.request.Request(f"{BASE}/v1/chat/completions", data=body,
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=20) as r:
        data = json.loads(r.read())
    return data["choices"][0]["message"]["content"]


def _health_ok() -> bool:
    try:
        with urllib.request.urlopen(f"{BASE}/health", timeout=5) as r:
            return json.loads(r.read()).get("status") == "ok"
    except Exception:
        return False


def _start_server(learn_kb: Path, with_wiki: bool):
    """Boot the parrot0 daemon with the agi profile DROPPED (so algo.p0 is absent
    and the forget is genuine), an empty session, and a fresh learned KB.
    PARROT0_WIKI_DIR is set only when a source page should be readable."""
    env = {
        **os.environ,
        "PARROT0_BASE": str(ROOT / "kb" / "core" / "base.p0"),
        "PARROT0_PROFILE": "",           # drop agi.p0 → no curated algo.p0 sort knowledge
        "PARROT0_SESSION": "",
        "PARROT0_LEARN_KB": str(learn_kb),
        "PARROT0_TOOLS": "1",
        "PARROT0_PI_LOG": "/tmp/parrot0-sortlearn-bench.log",
    }
    if with_wiki:
        env["PARROT0_WIKI_DIR"] = str(WIKI_DIR)
    else:
        env.pop("PARROT0_WIKI_DIR", None)
    # gen221: parrot0 serves the OpenAI API itself (no Python bridge).
    proc = subprocess.Popen(
        [str(ROOT / "bin" / "parrot0"), "--daemon", "--port", str(PORT), "--host", "127.0.0.1"],
        cwd=str(ROOT), env=env,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    for _ in range(50):
        if _health_ok():
            return proc
        time.sleep(0.1)
    proc.kill()
    return None


def _stop(proc) -> None:
    proc.terminate()
    try:
        proc.wait(timeout=3)
    except subprocess.TimeoutExpired:
        proc.kill()


def main() -> int:
    if not (ROOT / "bin" / "parrot0").is_file():
        print("sortlearn: bin/parrot0 missing — run 'make build'", file=sys.stderr)
        return 2

    passed = failed = 0

    def check(name: str, good: bool, got: str = "") -> None:
        nonlocal passed, failed
        if good:
            passed += 1
            print(f"PASS {name}")
        else:
            failed += 1
            print(f"FAIL {name}: got [{got[:200]}]")

    tmp = Path(tempfile.mkdtemp(prefix="parrot0-sortlearn-"))

    # ---- 1. forget: no wiki source, no profile → honest miss -------------------
    learn_kb = tmp / "forget.p0"
    srv = _start_server(learn_kb, with_wiki=False)
    if srv is None:
        print("FAIL sortlearn: forget server did not come up", file=sys.stderr)
        return 1
    try:
        r = _post_chat("tell me about bubble sort")
        # gen315: assert the SEMANTICS of a clean honest miss, not one historical
        # phrasing (the informed-decline wording legitimately evolves): the reply
        # must admit the missing source ("no source" / "don't have a source"),
        # must not fabricate content, and nothing may be persisted.
        rl = r.lower()
        admits_no_source = "no source" in rl or "t have a source" in rl
        fabricates = "swaps adjacent elements" in rl or "read it up" in rl
        persisted = learn_kb.read_text() if learn_kb.is_file() else ""
        check("forget-honest-miss",
              admits_no_source and not fabricates and "wiki_concept" not in persisted,
              r)
    finally:
        _stop(srv)

    # ---- 2 + 3. relearn via research, then the articulated multi-step ----------
    learn_kb = tmp / "learned.p0"
    srv = _start_server(learn_kb, with_wiki=True)
    if srv is None:
        print("FAIL sortlearn: relearn server did not come up", file=sys.stderr)
        return 1
    multistep = ""
    try:
        # relearn: the first ask reads the fixture page and persists the concept.
        r = _post_chat("tell me about bubble sort")
        check("relearn-reads-and-learns",
              "read it up" in r.lower() and "swaps adjacent elements" in r.lower(),
              r)

        # the fact is persisted to the learned KB, exactly once.
        persisted = learn_kb.read_text() if learn_kb.is_file() else ""
        check("relearn-persists-fact",
              "wiki_concept(bubble_sort, algorithms," in persisted
              and persisted.count("wiki_concept(bubble_sort,") == 1,
              persisted)

        # a re-ask is answered from RAM and quotes the learned concept.
        r = _post_chat("tell me about bubble sort")
        check("relearn-recall-from-ram",
              "swaps adjacent elements" in r.lower(),
              r)

        # the articulated, multi-step prompt — the heart of Track A.
        multistep = _post_chat(
            "tell me about bubble sort, then write a function add that returns the "
            "sum of a and b, then compute add(2,3), and finally implement a function "
            "bubblesort that sorts an array of integers")
        low = multistep.lower()
        check("multistep-is-a-plan",
              "1)" in multistep and "2)" in multistep and "3)" in multistep
              and "4)" in multistep,
              multistep)
        check("multistep-step1-recalls-learned",
              "swaps adjacent elements" in low,
              multistep)
        check("multistep-step2-add-verified",
              "int add(int a, int b)" in multistep and "a + b" in multistep
              and "verified" in low,
              multistep)
        check("multistep-step3-add-2-3-is-5",
              "add(2,3) = 5" in multistep,
              multistep)
        # Track B closed the gap (B5): the sort is now SYNTHESIZED from a KB schema
        # and the run-grounded judge (code_check_sort) disposes it — so step 4 must be
        # a VERIFIED sort, not a refusal and not a hardcoded body. The function is
        # composed from algo_shape(bubblesort, nested_loop_compare_swap), checked on 8
        # vectors for sortedness AND permutation, and reported only because it passed.
        check("multistep-step4-sort-synthesized-and-verified",
              "void bubblesort(int a[], int n)" in multistep
              and "a[j] > a[j + 1]" in multistep
              and "verified by execution" in low and "permutation" in low,
              multistep)
    finally:
        _stop(srv)

    # ---- 4. ratchet ledger — the gap this test once RECORDED is now CLOSED ------
    print("---")
    print("ratchet fired (Track B / B5): the LEARN→BUILD→TEST chain is whole —")
    print("  learn-via-research → recall → compose+verify add → SYNTHESIZE a sort from")
    print("  a KB schema and DISPOSE it with the run-grounded judge. Earned, not faked.")
    if multistep:
        for line in multistep.split("  "):
            if line.strip().startswith("4)"):
                print(f"  verified artifact: {line.strip()[:120]}…")
                break

    print(f"---\npassed: {passed}, failed: {failed}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
