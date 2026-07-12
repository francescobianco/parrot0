#!/usr/bin/env bash
#
# Oracle for docs/plans/universal-input.md (U1-U8).  One MCP engine process keeps
# a single live KB, so assert -> segment -> retract proves runtime growth rather
# than accidentally testing three rebuilt/fresh binaries.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "universal-input: binary not built ($BIN)" >&2; exit 1; }
cd "$ROOT" || exit 1

pass=0 fail=0
ok() { echo "PASS universal-input: $1"; pass=$((pass+1)); }
no() { echo "FAIL universal-input: $1" >&2; fail=$((fail+1)); }
call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'

  # U1/U2/U7: a role absent from the curated KB, taught and retracted live.
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix: int f(void) { return 1; } a condizione che sia veloce"}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"segment_role","args":["constraint","a condizione che"]}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix: int f(void) { return 1; } a condizione che sia veloce"}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"segment_role","args":["repro","per ricreare"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix: int f(void) { return 1; } per ricreare usa -1"}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"segment_role","args":["repro","per ricreare"]}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix: int f(void) { return 1; } per ricreare usa -1"}}}'

  # U3-U6: competing registers, arbitrary delimiters, indentation and logs.
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: {\"answer\": 42} expected 42"}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix this:\ndef f(x):\n    return x\nIt should return x"}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"pytest output:\nE   assert 1 == 2\nFAILED test_x.py::test_x"}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: {}"}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"the sky is blue"}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"settings.conf"}}}'

  # A never-before-named line register costs ONE fact.
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"register_evidence","args":["notice","line_prefix(NOTICE:)"]}}}'
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"NOTICE: hello"}}}'

  # A delimiter-driven register is also facts only, including its consumer.
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"delim_pair","args":["angle","<",">"]}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"code_register","args":["anglelang","angle"]}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"register_evidence","args":["anglelang","balanced(angle)"]}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"faculty_for","args":["code(anglelang)","custom_reader"]}}}'
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: <outer <inner>> after"}}}'

  # U8: intent_cue and register_evidence expose the same winner/tie/Gap shape.
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["alpha_intent","same cue"]}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["beta_intent","same cue"]}}}'
  call '{"jsonrpc":"2.0","id":24,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"intent_cue","text":"please use same cue now","candidates":["alpha_intent","beta_intent"]}}}'
  call '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["beta_intent","same cue"]}}}'
  call '{"jsonrpc":"2.0","id":26,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"intent_cue","text":"please use same cue now","candidates":["alpha_intent","beta_intent"]}}}'
  call '{"jsonrpc":"2.0","id":27,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["alpha_intent","same cue"]}}}'
  call '{"jsonrpc":"2.0","id":28,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"intent_cue","text":"please use same cue now","candidates":["alpha_intent","beta_intent"]}}}'

  # The other non-source registers named by U6 use the same line engine.
  call '{"jsonrpc":"2.0","id":29,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"patch:\ndiff --git a/a.c b/a.c\n@@ -1 +1 @@\n-old\n+new"}}}'
  call '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"compiler:\nmain.c:3:5: error: expected ; before }"}}}'
  call '{"jsonrpc":"2.0","id":31,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"failure:\nTraceback (most recent call last):\n  File \"x.py\", line 1\nValueError: bad"}}}'
  call '{"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"fix this code: int f(void) { return 1; } without recursion"}}}'
  call '{"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"how do you know?"}}}'
  call '{"jsonrpc":"2.0","id":34,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"explain:\n```c\nint f(void) { return 1; }\n```\nIt should return 1"}}}'
  call '{"jsonrpc":"2.0","id":35,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"register_evidence","args":["notice","line_prefix(NOTICE:)"]}}}'
  call '{"jsonrpc":"2.0","id":36,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"register_evidence","text":"NOTICE: hello","candidates":["notice"]}}}'

  # Boundary/safety regressions found by the implementation audit.
  call '{"jsonrpc":"2.0","id":37,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix this code\nint f(void) { return 1; } It should return 1"}}}'
  call '{"jsonrpc":"2.0","id":38,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"fix this code\nint f(void) { return 1; } It should return 1"}}}'
  call '{"jsonrpc":"2.0","id":39,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"```\n```"}}}'
  call '{"jsonrpc":"2.0","id":40,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"```c\nconst char *s = \"```\";\n```"}}}'

  # Independent delimiter regions, unclosed input and overlapping role facts.
  call '{"jsonrpc":"2.0","id":41,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"fix this:\n{\"expected\":1}\nint f(void){return 1;}"}}}'
  call '{"jsonrpc":"2.0","id":42,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: {\"answer\":42"}}}'
  call '{"jsonrpc":"2.0","id":43,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: [1,2"}}}'
  call '{"jsonrpc":"2.0","id":44,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"segment_role","args":["alpha","foo bar"]}}}'
  call '{"jsonrpc":"2.0","id":45,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"segment_role","args":["beta","bar baz"]}}}'
  call '{"jsonrpc":"2.0","id":46,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"foo bar baz tail"}}}'
  call '{"jsonrpc":"2.0","id":47,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what functions does this define: int f(void) {"}}}'
  call '{"jsonrpc":"2.0","id":48,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"don\u0027t touch:\nint f(void) { return 1; }"}}}'

  # A symmetric delimiter and its consumer are runtime facts too.
  call '{"jsonrpc":"2.0","id":49,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"delim_pair","args":["pipe","|","|"]}}}'
  call '{"jsonrpc":"2.0","id":50,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"code_register","args":["pipelang","pipe"]}}}'
  call '{"jsonrpc":"2.0","id":51,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"register_evidence","args":["pipelang","balanced(pipe)"]}}}'
  call '{"jsonrpc":"2.0","id":52,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"faculty_for","args":["code(pipelang)","pipe_reader"]}}}'
  call '{"jsonrpc":"2.0","id":53,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"inspect: | \"a|b\" | after"}}}'

  # Scorer semantics: quoted default is surface text; bare default is fallback.
  call '{"jsonrpc":"2.0","id":54,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"quoted_default_test","args":["q","\"default\""]}}}'
  call '{"jsonrpc":"2.0","id":55,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"quoted_default_test","text":"unrelated","candidates":["q"]}}}'
  call '{"jsonrpc":"2.0","id":56,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"quoted_default_test","text":"the default value","candidates":["q"]}}}'
  call '{"jsonrpc":"2.0","id":57,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"default_test","args":["a","default"]}}}'
  call '{"jsonrpc":"2.0","id":58,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"default_test","args":["a","hit"]}}}'
  call '{"jsonrpc":"2.0","id":59,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"default_test","args":["b","hit"]}}}'
  call '{"jsonrpc":"2.0","id":60,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"default_test","text":"hit","candidates":["a","b"]}}}'
  call '{"jsonrpc":"2.0","id":61,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"register_evidence","text":"{} then {\"x\":1}","candidates":["json"]}}}'
  call '{"jsonrpc":"2.0","id":62,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"malformed_evidence_test","args":["m","keyword(x)junk"]}}}'
  call '{"jsonrpc":"2.0","id":63,"method":"tools/call","params":{"name":"input.classify","arguments":{"relation":"malformed_evidence_test","text":"x","candidates":["m"]}}}'
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
has() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fq "$pattern"; then ok "$label"; else no "$label: $(line "$id")"; fi
}
lacks() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fq "$pattern"; then no "$label: $(line "$id")"; else ok "$label"; fi
}

has 2  '\"role\":\"expected\"'   "unknown role cue uses the declared after-default"
has 4  '\"role\":\"constraint\"' "a runtime fact changes segmentation without rebuild"
has 4  'segment_role(constraint'        "constraint span cites its exact supporting fact"
has 6  '\"role\":\"repro\"'      "an open runtime role crosses the C API unchanged"
has 6  'segment_role(repro'             "open role carries provenance"
has 8  '\"role\":\"expected\"'   "retraction changes the next segmentation"
lacks 8 '\"role\":\"repro\"'      "retracted role is not cached"

has 9  '\"register\":\"json\"'   "JSON is isolated by KB delimiter/evidence facts"
has 9  '\"faculty\":\"structured_reader\"' "faculty_for routes the typed JSON span"
has 10 '\"register\":\"python\"' "indentation closes Python before dedented prose"
has 10 '\"role\":\"expected\"'  "dedented prose remains a separate role span"
has 11 '\"register\":\"pytest\"' "pytest output is a non-source typed register"
has 11 '\"faculty\":\"verdict_builder\"' "pytest span is consumable without a parser branch"
has 12 '\"ambiguous\":true'             "equal C/JSON evidence produces ambiguous_input"
has 12 'c=4 because register_evidence'        "ambiguity proof names the tied support"
has 13 '\"role\":\"prose\"'         "ordinary prose stays prose"
lacks 13 '\"register\":\"python\"'  "common prose words do not imply Python"
has 14 '\"role\":\"prose\"'         ".conf is not classified as .c"
lacks 14 '\"register\":\"c\"'       "extension matching is exact"

has 16 '\"register\":\"notice\"'    "a new line register works from one runtime fact"
has 16 'register_evidence(notice'           "new register cites its runtime evidence"
has 21 '\"register\":\"anglelang\"' "an arbitrary nested delimiter pair needs zero C"
has 21 '\"faculty\":\"custom_reader\"' "runtime faculty routing is honored"

has 24 '\"status\":\"ambiguous\"'   "intent hypotheses use the no-tiebreak scorer"
has 24 'ambiguous_hypothesis(intent_cue)'   "intent ambiguity has the same proof shape"
has 26 '\"status\":\"winner\"'      "ablating one intent support leaves the survivor"
has 26 '\"hypothesis\":\"alpha_intent\"' "winner is derived, not fact-order selected"
has 28 '\"status\":\"gap\"'         "ablating all intent evidence produces a typed Gap"
has 28 'gap(intent_cue)'                    "intent Gap mirrors register-evidence Gap"
has 29 '\"register\":\"diff\"'       "diff is isolated by line evidence"
has 29 '\"faculty\":\"patch_reader\"' "diff routes to its declarative consumer"
has 30 '\"register\":\"cc\"'         "compiler diagnostics use the same line engine"
has 31 '\"register\":\"traceback\"'  "tracebacks use the same line engine"
has 33 'segment_role(constraint'             "a conversational why cites the role fact"
has 34 '\"register\":\"c\"'           "explicit fences use the same register scorer"
has 34 '\"role\":\"expected\"'        "prose after a fence remains a separate span"
has 36 '\"status\":\"gap\"'           "ablating register evidence produces the same Gap status"
has 36 'gap(register_evidence)'               "register Gap mirrors intent-cue Gap"
has 37 '\"start\":14'                       "a structured head does not absorb prose from the previous line"
has 37 '\"register\":\"c\"'              "the no-colon multiline source remains a C span"
has 38 'I compiled it'                         "the compiler receives only the no-colon source span"
lacks 38 'unknown type name'                   "preceding prose is never compiled as a C type"
has 39 '\"ambiguous\":true'                 "an untyped empty fence remains explicitly ambiguous"
has 39 '\"start\":4,\"len\":0'          "an empty ambiguous span stays within the raw byte stream"
has 40 '\"ambiguous\":false'                "backticks inside a fenced literal do not close the fence"
has 40 '\"register\":\"c\"'              "the real line fence still closes the C span"
has 41 '\"register\":\"json\"'           "adjacent JSON is classified independently"
has 41 '\"register\":\"c\"'              "adjacent C is not merged into the JSON span"
has 42 '\"ambiguous\":true'                 "an unclosed object is ambiguous even before balanced evidence can fire"
has 42 'delim_pair(brace, {, })'                "unclosed-object proof cites the delimiter fact"
has 43 '\"ambiguous\":true'                 "an unclosed array is ambiguous"
has 46 '\"ambiguous\":true'                 "equal overlapping role cues compete instead of truncating provenance"
has 46 'segment_role(alpha'                      "overlap ambiguity cites the first role fact"
has 46 'segment_role(beta'                       "overlap ambiguity cites the second role fact"
has 47 'ambiguous_input'                         "structural code questions preserve segmentation ambiguity"
lacks 47 'looks like a snippet'                  "codeast does not replace an unclosed span with a generic guess"
has 48 '\"register\":\"c\"'              "an apostrophe in preceding prose does not hide a structured span"
has 53 '\"register\":\"pipelang\"'       "a symmetric delimiter pair is facts-only"
has 53 '\"faculty\":\"pipe_reader\"'     "a symmetric runtime register reaches its declared consumer"
has 55 '\"status\":\"gap\"'              "quoted default is not the fallback sentinel"
has 56 '\"status\":\"winner\"'           "quoted default remains ordinary surface evidence"
has 60 '\"status\":\"ambiguous\"'        "bare default does not inflate a specifically supported hypothesis"
has 61 'paired_contains(brace'                   "paired evidence scans beyond the first balanced block"
has 63 '\"status\":\"gap\"'               "malformed compound evidence is opaque, not executable structure"

echo "universal-input: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
