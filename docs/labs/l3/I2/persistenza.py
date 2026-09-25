#!/usr/bin/env python3
"""Probe L3: save/restart on a writable COPY of the complete living KB.

Prints observations, does not certify assertions. Never saves to the source KB.
The teacher uses natural language; MCP is only transport and diagnostics.
Run from any directory after `make build`.
"""
import importlib.util
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[4]
spec = importlib.util.spec_from_file_location("self_questions", ROOT / "scripts/self-questions.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def start(work, phase):
    engine = module.Engine.__new__(module.Engine)
    engine.n = 0
    env = {k: v for k, v in os.environ.items() if not k.startswith("PARROT0_")}
    env.update(PARROT0_SESSION="", PARROT0_PROFILE="kb/profiles/agi.p0",
               PARROT0_LANG="en", PARROT0_WIKI_FETCH="0", PARROT0_TOOLS="1")
    with (work / f"{phase}.log").open("w") as log:
        engine.p = subprocess.Popen(
            [str(ROOT / "bin/parrot0"), "--mcp-engine"], cwd=work, env=env,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=log,
            text=True, bufsize=1,
        )
    engine.rpc("initialize", {})
    return engine


def say(engine, text):
    reply = engine.respond(text)
    print(f"> {text}\n{reply}", flush=True)
    return reply


def state(engine):
    for pred, args in (
        ("contact_episode", ["geburtsort", "born_in", None]),
        ("contact_counter", ["geburtsort", "born_in", None]),
        ("contact_support", ["geburtsort", "born_in", None]),
        ("relation_noun", ["born_in", None]),
    ):
        print(pred, engine.match(pred, args), flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--fallback",
                        help="Override MCP's default; historical broken value: kb/core/session.p0")
    args = parser.parse_args()
    # Keep the copy and logs for inspection; print its exact path for handoff.
    work = Path(tempfile.mkdtemp(prefix="parrot0-l3-persistence-"))
    shutil.copytree(ROOT / "kb", work / "kb", symlinks=False)
    print(f"Complete KB copy: {work}", flush=True)
    for phase in ("learn", "withdraw", "reopened"):
        print(f"--- {phase} ---", flush=True)
        engine = start(work, phase)
        try:
            if phase == "learn":
                say(engine, "Einstein was born in Ulm, so his Geburtsort is Ulm.")
            say(engine, "What is the Geburtsort of Napoleon?")
            state(engine)
            if phase == "withdraw":
                say(engine, "Marie Curie was born in Warsaw, but Warsaw is not her Geburtsort.")
                say(engine, "What is the Geburtsort of Napoleon?")
                state(engine)
            if phase != "reopened":
                save_args = {"path": args.fallback} if args.fallback else {}
                print("SAVE", engine.call("kb.save", save_args), flush=True)
        finally:
            engine.close()
        log = (work / f"{phase}.log").read_text()
        errors = [line for line in log.splitlines() if "PARSE ERROR" in line]
        print("PARSE ERRORS", errors, flush=True)


if __name__ == "__main__":
    main()
