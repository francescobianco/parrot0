#!/usr/bin/env python3
"""gen318 (forge-master-plan W0b, M-1a §19): `make check TEST=<contract-id>` —
the FOCAL runner. The Tick orients; this is the tool that gives it direction.

What the suite could not do before this generation: address ONE contract by id,
see its verdict stream as it happens, and stop at the FIRST counterexample. The
only probe available was `make test` (171 s, 38 serial harnesses, every verdict
withheld until the final aggregation), so a wrong hypothesis cost minutes and a
red line never named the level that owned it.

Honesty (§10.6 rule 5): execution and function are different results.
  execution_state  ran | timeout | infra_error
  verdict          pass | fail | unknown
  evidence_origin  fresh   (there is no cache yet — a cache of hope is worse
                            than no cache; content-addressed evidence is #8)
A timeout or a broken harness is NOT a functional FAIL and never becomes a PASS
by re-running blind (§1.8). Exit code: 0 green, 1 functional red, 2 not-run.

The seam it asserts today is the honest one: per-turn golden text through the
real process, framed by PARROT0_EOT. It is NOT the structured
status/winner/proof seam of §10.13 — that seam does not exist yet, so this
runner does not pretend to report a `winner=`. It reports the OWNER the catalog
declares, the turn that broke, and what was expected against what was observed.
"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
# $PARROT0_CONTRACTS points the runner at a fixture catalog. The ratchet
# (tests/checkfocal.sh) uses it to feed the runner a KNOWN-RED contract and
# prove the fail-fast is real — a runner that can only say PASS is worthless.
CATALOG = Path(os.environ.get("PARROT0_CONTRACTS")
               or ROOT / "tests" / "contracts.json")
BIN = ROOT / "bin" / "parrot0"
EOT = "<<<P0_EOT>>>"

# Lane budgets (§10.6). Focal is the interactive corridor: it must stay cheap
# enough that a hypothesis is answered before it cools.
LANE_BUDGET_S = {"focal": 5.0, "impact": 10.0, "transfer": 60.0}
FIRST_EVENT_BUDGET_MS = 500.0


def load_catalog() -> dict:
    return json.loads(CATALOG.read_text())


def parse_case(path: Path) -> list[tuple[str, list[str]]]:
    """Parse a .chat case into (input, expected_lines) turns.

    Consecutive '<' lines belong to the SAME turn (a reply may span several
    lines — gen269 markdown). A turn with no '<' line is executed but not
    asserted, so a case may drive state without claiming an expectation.
    """
    turns: list[tuple[str, list[str]]] = []
    for raw in path.read_text().splitlines():
        if raw.startswith(">"):
            turns.append((raw[1:].lstrip(" "), []))
        elif raw.startswith("<") and turns:
            turns[-1][1].append(raw[1:].lstrip(" "))
    return turns


def read_reply(proc: subprocess.Popen, deadline: float) -> list[str] | None:
    """Read one framed reply: every line up to the EOT marker.

    Returns None if the process died or the deadline passed — an execution
    fact, never a functional verdict.
    """
    lines: list[str] = []
    while True:
        if time.monotonic() > deadline:
            return None
        line = proc.stdout.readline()
        if line == "":
            return None  # process closed stdout
        line = line.rstrip("\n")
        if line == EOT:
            return lines
        lines.append(line)


def run_contract(c: dict, profiles: dict) -> dict:
    """Execute one contract in its own process. Stops at the first bad turn."""
    oracle = ROOT / c["oracle"]
    if not oracle.is_file():
        return {"execution_state": "infra_error", "verdict": "unknown",
                "detail": f"oracle missing: {c['oracle']}", "ms": 0.0}

    turns = parse_case(oracle)
    if not turns:
        return {"execution_state": "infra_error", "verdict": "unknown",
                "detail": f"oracle has no turns: {c['oracle']}", "ms": 0.0}

    profile = profiles.get(c["boot_profile"])
    if profile is None:
        return {"execution_state": "infra_error", "verdict": "unknown",
                "detail": f"unknown boot_profile: {c['boot_profile']}", "ms": 0.0}

    env = {**os.environ, **profile["env"], "PARROT0_EOT": EOT}
    timeout_s = c["timeout_ms"] / 1000.0

    t0 = time.monotonic()
    deadline = t0 + timeout_s
    proc = subprocess.Popen(
        [str(BIN)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL, text=True, bufsize=1, env=env)
    try:
        # The banner and the `you> ` prompt go to stderr, so stdout carries
        # nothing but replies and EOT markers — there is no preamble to skip
        # (and reading one would deadlock: parrot0 says nothing until spoken to).
        for i, (user, expected) in enumerate(turns, start=1):
            proc.stdin.write(user + "\n")
            proc.stdin.flush()
            got = read_reply(proc, deadline)
            if got is None:
                state = "timeout" if time.monotonic() > deadline else "infra_error"
                return {"execution_state": state, "verdict": "unknown",
                        "detail": f"turn {i} produced no framed reply",
                        "ms": (time.monotonic() - t0) * 1000.0}
            if not expected:
                continue  # driven, not asserted
            if got != expected:
                return {"execution_state": "ran", "verdict": "fail",
                        "turn": i, "user": user,
                        "expected": expected, "observed": got,
                        "ms": (time.monotonic() - t0) * 1000.0}
    finally:
        try:
            proc.stdin.close()
        except Exception:
            pass
        try:
            proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()

    return {"execution_state": "ran", "verdict": "pass", "turns": len(turns),
            "ms": (time.monotonic() - t0) * 1000.0}


def select(catalog: dict, test: str) -> list[dict]:
    """Resolve TEST to contracts: exact id, else prefix (a family/lane)."""
    contracts = catalog["contracts"]
    exact = [c for c in contracts if c["id"] == test]
    if exact:
        return exact
    return [c for c in contracts if c["id"].startswith(test)]


def main() -> int:
    started = time.monotonic()
    catalog = load_catalog()
    test = os.environ.get("TEST", "").strip()

    if not test:
        print("check: name a contract — make check TEST=<id>\n")
        print(f"{'id':32} {'tier':8} {'capability':16} owner")
        for c in catalog["contracts"]:
            print(f"{c['id']:32} {c['tier']:8} {c['capability']:16} "
                  f"{', '.join(c['owner'])}")
        print(f"\n{len(catalog['contracts'])} contracts migrated to the granular "
              f"catalog (tests/contracts.json). Unmigrated cases run in the "
              f"'core' area via tests/run.sh.")
        return 0

    selected = select(catalog, test)
    if not selected:
        print(f"check: no contract matches '{test}' (see `make check` for the list)")
        return 2

    if not BIN.is_file():
        print(f"check: binary not built ({BIN})")
        return 2

    # Cheapest first: a candidate already dead should not pay for the slow rows.
    selected.sort(key=lambda c: c["timeout_ms"])
    total = len(selected)
    profiles = catalog["boot_profiles"]

    lane = selected[0]["tier"]
    first_event_ms = None
    passed = 0
    red: dict | None = None
    not_run: dict | None = None

    for n, c in enumerate(selected, start=1):
        tag = f"[S1 {n}/{total}"
        print(f"{tag} START] {c['id']}", flush=True)
        if first_event_ms is None:
            first_event_ms = (time.monotonic() - started) * 1000.0

        r = run_contract(c, profiles)
        ms = r["ms"]

        if r["verdict"] == "pass":
            print(f"{tag} PASS ] {ms:6.0f} ms  {r['turns']} turns  "
                  f"evidence=fresh", flush=True)
            passed += 1
            continue

        if r["verdict"] == "fail":
            print(f"{tag} FAIL ] {ms:6.0f} ms  turn {r['turn']}: "
                  f"{r['user']!r}", flush=True)
            for line in r["expected"]:
                print(f"        expected: {line}")
            for line in r["observed"]:
                print(f"        observed: {line}")
            print(f"first broken contract: {c['id']} "
                  f"(owner: {', '.join(c['owner'])})")
            print(f"next: stop (candidate rejected) — "
                  f"{total - n} contract(s) not run")
            red = c
            break

        # execution_state != ran: honestly NOT a functional verdict (§1.8).
        print(f"{tag} {r['execution_state'].upper():5}] {ms:6.0f} ms  "
              f"{r['detail']}", flush=True)
        print(f"not-run: {c['id']} — execution_state={r['execution_state']}, "
              f"verdict=unknown (this is not a FAIL and must not be "
              f"re-run blind)")
        not_run = c
        break

    wall = time.monotonic() - started
    budget = LANE_BUDGET_S.get(lane, 5.0)
    slo = "within" if wall <= budget else "OVER"
    fe = first_event_ms or 0.0
    fe_slo = "within" if fe <= FIRST_EVENT_BUDGET_MS else "OVER"

    print(f"---")
    print(f"check: {passed}/{total} pass — wall {wall:.2f} s "
          f"({slo} {lane} budget {budget:.0f} s), "
          f"first event {fe:.0f} ms ({fe_slo} budget "
          f"{FIRST_EVENT_BUDGET_MS:.0f} ms)")

    if not_run is not None:
        return 2
    if red is not None:
        return 1
    if wall > budget:
        # The lane budget is an invariant, not a convenience (§1.8): a contract
        # that outgrows its lane must be split, optimized or moved — it does not
        # get to quietly slow the interactive corridor down.
        print(f"check: {lane} lane over budget — split, optimize or re-tier it")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())