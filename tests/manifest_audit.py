#!/usr/bin/env python3
"""gen320 (forge-master-plan §15 row 3): audit the benchmark manifest against
what the scripts ACTUALLY do. Red on a false classification.

The manifest is supposed to be the machine-readable source of truth about what
each benchmark measures. It was not (§3.4): `glue` was declared a gate while its
script always exited 0 even with a GAP — ~12.8 s of every `make gate` buying a
ratchet that could not fire; `mimic`, `mmlu` and `bbh` were declared `external`
while running entirely offline on local fixtures; and `basic-chat` named a script
(tests/basic_chat_bench.sh) that does not exist. A manifest nobody checks drifts
into decoration, and a decorative gate is worse than a missing one — it launders
an unverified claim as evidence.

Three mechanical checks, each one a lie the manifest actually told:

  1. the script a row names must EXIST;
  2. `semantics: gate` requires `exit_contract: red-on-failure`, and the script's
     exit code must really be load-bearing — a script that ends in an
     unconditional success cannot go red, so it is not a gate;
  3. `semantics: external` must name the resources it needs in `requires`, and
     the script must actually reference one of them — a target that touches no
     API key, no network and no docker is not external, whatever it claims.
     Symmetrically, an offline row that DOES reach for an API key is mislabelled.

The limit of check 2, stated rather than hidden: it reads the script's exit
contract statically. It proves a script CAN fail, not that it fails on every
wrong answer. Mutation-proving each oracle is a Nightly job (§10.6), not a
`make test` one.
"""
import json
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
# $PARROT0_MANIFEST points the audit at a fixture manifest. The ratchet
# (tests/checkfocal.sh) feeds it manifests that lie, and asserts it goes RED —
# an audit nobody has seen fail is just another unverified claim.
MANIFEST = Path(os.environ.get("PARROT0_MANIFEST")
                or ROOT / "tests" / "benchmarks.json")
MAKEFILE = ROOT / "Makefile"

# Tokens that betray a genuine outside dependency: an LLM provider key, the
# network, a container runtime, a dataset download.
EXTERNAL_TOKENS = ("OPENCODE_API_KEY", "docker", "http://", "https://",
                   "requests", "urllib", "curl", "load_dataset", "huggingface")

# A shell script whose last effective statement is one of these propagates its
# own failure; one that ends on an unconditional `echo`/`exit 0` cannot.
FAIL_PROPAGATING = re.compile(
    r"""^(\[\s|\[\[\s|test\s|exit\s+\$|exit\s+[1-9]|sys\.exit|return\s+\$)"""
)


def last_statement(path: Path) -> str:
    for line in reversed(path.read_text().splitlines()):
        s = line.strip()
        if s and not s.startswith("#"):
            return s
    return ""


def script_can_fail(path: Path) -> bool:
    """Is the script's exit code load-bearing?"""
    text = path.read_text()
    if path.suffix == ".py":
        # A python harness fails through a non-zero sys.exit / raised assertion.
        return bool(re.search(r"sys\.exit\([^0)]|sys\.exit\(\s*1|raise\b", text))
    last = last_statement(path)
    if FAIL_PROPAGATING.match(last):
        return True
    # An explicit `exit 1` anywhere, reachable from a failure branch, also counts.
    return bool(re.search(r"^\s*exit\s+1\b", text, re.M))


def dependency_text(script: Path) -> str:
    """The script's text PLUS that of the repo scripts it invokes (one hop).

    A target's real dependency can live one level down: swe-solve's entry point
    does not speak docker, it hands off to tests/swebench/oracle.sh, which does
    (11 times). Judging the entry point alone would have called a docker-bound
    benchmark 'offline' — so the audit follows the delegation instead of
    weakening the rule.
    """
    text = script.read_text()
    for ref in re.findall(r"[\w./-]*(?:tests|scripts)/[\w./-]+\.(?:sh|py)", text):
        dep = ROOT / ref.lstrip("./")
        if dep.is_file() and dep != script:
            text += "\n" + dep.read_text()
    return text


def make_targets() -> set[str]:
    targets = set()
    for line in MAKEFILE.read_text().splitlines():
        m = re.match(r"^([a-zA-Z0-9_.-]+)\s*:(?!=)", line)
        if m:
            targets.add(m.group(1))
    return targets


def main() -> int:
    manifest = json.loads(MANIFEST.read_text())
    targets = make_targets()
    problems: list[str] = []
    checked = 0

    for e in manifest["benchmarks"]:
        bid = e["id"]
        semantics = e["semantics"]

        if e["target"] not in targets:
            problems.append(f"{bid}: names make target '{e['target']}', "
                            f"which the Makefile does not define")
            continue

        script = ROOT / e["script"]
        if not script.is_file():
            problems.append(f"{bid}: names script '{e['script']}', "
                            f"which does not exist")
            continue

        text = dependency_text(script)
        contract = e["exit_contract"]
        requires = e["requires"]
        checked += 1

        # 2. a gate must be able to go red.
        if semantics == "gate":
            if contract != "red-on-failure":
                problems.append(f"{bid}: declared a gate but exit_contract is "
                                f"'{contract}' — a gate must fail on failure")
            elif not script_can_fail(script):
                problems.append(
                    f"{bid}: declared a gate, but {e['script']} ends on an "
                    f"unconditional success and has no failing exit — it "
                    f"CANNOT go red, so it is an instrument, not a ratchet")
        elif contract == "red-on-failure" and semantics != "external":
            # Not fatal, but a discovery row that fails the build is a surprise.
            if not script_can_fail(script):
                problems.append(f"{bid}: exit_contract says red-on-failure but "
                                f"{e['script']} has no failing exit")

        # 3. external must be genuinely external — and offline must be offline.
        touches_external = any(t in text for t in EXTERNAL_TOKENS)
        if semantics == "external":
            if not requires:
                problems.append(f"{bid}: declared external but 'requires' is "
                                f"empty — name what it needs")
            elif not touches_external:
                problems.append(
                    f"{bid}: declared external (requires {requires}) but "
                    f"{e['script']} references no API key, network, docker or "
                    f"dataset — it runs offline, so it is not external")
        else:
            if "OPENCODE_API_KEY" in text:
                problems.append(
                    f"{bid}: declared '{semantics}' (offline) but "
                    f"{e['script']} reaches for OPENCODE_API_KEY — an LLM "
                    f"provider is an external dependency")
            if requires:
                problems.append(f"{bid}: declared '{semantics}' but 'requires' "
                                f"names {requires}")

    print(f"manifest-audit: {checked} benchmark rows checked against their scripts")
    for p in problems:
        print(f"FAIL manifest-audit: {p}", file=sys.stderr)
    if problems:
        print(f"---\npassed: {checked - len(problems)}, failed: {len(problems)}")
        print("manifest-audit: the manifest claims something its scripts do not do")
        return 1
    print(f"---\npassed: {checked}, failed: 0")
    print("manifest-audit: every declared semantics matches the script's real "
          "exit contract and real dependencies")
    return 0


if __name__ == "__main__":
    sys.exit(main())