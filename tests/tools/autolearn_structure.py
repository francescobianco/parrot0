#!/usr/bin/env python3
"""Deterministic oracle for autolearn's gen332 structural training path."""

import os
import sys
import tempfile

sys.path.insert(0, os.path.dirname(__file__))
import autolearn as al


passed = 0
failed = 0


def check(condition, label):
    global passed, failed
    if condition:
        passed += 1
        print(f"PASS autolearn-structure: {label}")
    else:
        failed += 1
        print(f"FAIL autolearn-structure: {label}", file=sys.stderr)


def main():
    env = al.p0_env(os.path.join(al.ROOT, "kb", "core", "base.p0"), "",
                    os.path.join(al.ROOT, "kb"))
    sess = al.McpSession(env, al.ROOT)
    try:
        question = "How many prime numbers are there interbetween 1 and 10?"
        fact = {"pred": "intent_cue",
                "args": ["arith_range_between_open", "keyword(interbetween)"]}
        snap = al.inspect_input(question, sess)
        check((snap.get("segments") or {}).get("spans"),
              "trainer reads proof-bearing input spans through MCP")
        check("arith_range_between_open" in snap["catalog"]["intents"],
              "live intent catalog exposes only existing routing targets")
        family = {row["target"]: row for row in
                  snap["intent_families"]["range_property_count"]}
        check(family["arith_count_request"]["status"] == "winner" and
              family["arith_property_prime"]["status"] == "winner" and
              family["arith_range_between_close"]["status"] == "winner" and
              family["arith_range_between_open"]["status"] == "gap",
              "consumer family isolates the one missing frame component")
        check(any(s["target"] == "arith_range_between_open" for s in
                  snap["structural_suggestions"]),
              "KB consumer contract recommends the paired Gap to the teacher")
        check(al.classify_structure_fact(sess, question, fact).get("status") == "gap",
              "novel surface starts as a routing Gap")
        check(al.audit_lesson(question, {}, [fact], snap).get("ok"),
              "existing intent target survives the structural audit")

        invented = {"pred": "intent_cue", "args": ["invented_intent", "betwixt"]}
        check(not al.audit_lesson(question, {}, [invented], snap).get("ok"),
              "invented intent without a consumer is rejected")
        content = {"pred": "describe_cue", "args": ["explain"]}
        kept, dropped = al.filter_structural_targets([invented, content], snap)
        check(kept == [content] and dropped == [invented],
              "unsafe structural row cannot discard an independent content lesson")
        tiny = {"pred": "intent_cue", "args": ["arith_range_between_open", "x"]}
        check(not al.audit_lesson(question, {}, [tiny], snap).get("ok"),
              "one-character structural cue is rejected")

        before_reply = al.probe(question, sess)
        accepted, errors, effects = al.session_teach(sess, [fact], question)
        check(not errors and accepted == [fact],
              "trainer accepts a cue only after a live proof change")
        check(effects and effects[0]["before"].get("status") == "gap" and
              effects[0]["after"].get("status") == "winner",
              "structural effect records Gap to winner provenance")
        check(al.classify_structure_fact(sess, question, fact).get("status") == "winner",
              "new surface is recognized without rebuild")
        after_reply = al.probe(question, sess)
        check("4 prime numbers" not in before_reply and "4 prime numbers" in after_reply,
              "proof-changing cue activates the existing answer consumer")
        sess.call("kb.retract", {"pred": fact["pred"], "args": fact["args"]})
        check(al.classify_structure_fact(sess, question, fact).get("status") == "gap",
              "retraction removes recognition in the same process")
        check("4 prime numbers" not in al.probe(question, sess),
              "ablated cue removes the learned response without rebuild")

        ineffective = {"pred": "intent_cue",
                       "args": ["arith_range_between_open", "keyword(absentword)"]}
        accepted, errors, effects = al.session_teach(sess, [ineffective], question)
        check(not errors and not accepted and effects and not effects[0]["accepted"],
              "non-matching structural lesson is rejected and rolled back")
        args = {"pred": ineffective["pred"], "args": ineffective["args"]}
        check(not sess.call("kb.query", args).get("provable"),
              "rolled-back cue cannot pollute the live KB")

        role_q = "fix: int f(void) { return 1; } provided that it stays fast"
        role_fact = {"pred": "segment_role", "args": ["constraint", "provided that"]}
        role_snap = al.inspect_input(role_q, sess)
        accepted, errors, effects = al.session_teach(sess, [role_fact], role_q)
        check(al.audit_lesson(role_q, {}, [role_fact], role_snap).get("ok") and
              not errors and accepted == [role_fact],
              "existing open role learns a new surface through the same scorer")
        sess.call("kb.retract", {"pred": role_fact["pred"], "args": role_fact["args"]})
    finally:
        sess.close()

    with tempfile.TemporaryDirectory(prefix="p0tmp_autolearn_", dir=al.ROOT) as tree:
        intents = os.path.join(tree, "intents.p0")
        fallback = os.path.join(tree, "session.p0")
        with open(intents, "w") as fh:
            fh.write('intent_cue(arith_range_between_open, keyword(between)).\n')
        with open(fallback, "w"):
            pass
        learned = {"pred": "intent_cue",
                   "args": ["arith_range_between_open", "keyword(transrange)"]}
        rel_fallback = os.path.relpath(fallback, al.ROOT)
        stored, errors = al.persist_facts(
            [learned], rel_fallback, al.p0_env("", "", tree), al.ROOT)
        persisted = ""
        for root, _, files in os.walk(tree):
            for name in files:
                with open(os.path.join(root, name), errors="replace") as fh:
                    persisted += fh.read()
        check(stored == 1 and not errors and "keyword(transrange)" in persisted,
              "verified structural lesson persists through the routed save-map")
        check("policy(" not in persisted and "machinery(policy)" not in persisted,
              "runtime execution policy is excluded from learned knowledge")

    print("---")
    print(f"passed: {passed}, failed: {failed}")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
