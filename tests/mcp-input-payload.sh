#!/usr/bin/env bash
# Regression: input.segment can return more than the historical 70,000-byte
# tools/call scratch buffer.  Both the outer JSON-RPC response and its inner
# JSON text payload must remain complete and parseable.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
PY="$(test -x "$ROOT/.venv/bin/python" && echo "$ROOT/.venv/bin/python" || echo python3)"
[ -x "$BIN" ] || { echo "mcp-input-payload: binary not built ($BIN)" >&2; exit 1; }
cd "$ROOT" || exit 1

pad_a="$(printf '%090d' 0)"; pad_a="${pad_a//0/a}"
pad_b="$(printf '%090d' 0)"; pad_b="${pad_b//0/b}"
cue_a="$(printf '%0100d' 0)"; cue_a="${cue_a//0/A}"
cue_b="$(printf '%0100d' 0)"; cue_b="${cue_b//0/B}"
reg_a="register_a_${pad_a}"
reg_b="register_b_${pad_b}"
faculty_a="faculty_a_${pad_a}"
faculty_b="faculty_b_${pad_b}"

text=""
for ((i = 0; i < 128; i++)); do
    if ((i % 2 == 0)); then
        text+="${cue_a}${i}\\n"
    else
        text+="${cue_b}${i}\\n"
    fi
done

out="$( {
    printf '{"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"register_evidence","args":["%s","line_prefix(%s)"]}}}\n' "$reg_a" "$cue_a"
    printf '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"register_evidence","args":["%s","line_prefix(%s)"]}}}\n' "$reg_b" "$cue_b"
    printf '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"faculty_for","args":["%s","%s"]}}}\n' "$reg_a" "$faculty_a"
    printf '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"faculty_for","args":["%s","%s"]}}}\n' "$reg_b" "$faculty_b"
    printf '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"input.segment","arguments":{"text":"%s"}}}\n' "$text"
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

response="$(printf '%s\n' "$out" | tail -n 1)"
validator='import json,sys; o=json.load(sys.stdin); assert o["id"] == 5 and not o["result"]["isError"]; inner=o["result"]["content"][0]["text"]; p=json.loads(inner); assert len(p["spans"]) == 128; assert p["spans"][0]["register"] == sys.argv[1]; assert p["spans"][-1]["register"] == sys.argv[2]; print(len(inner))'

if bytes="$(printf '%s\n' "$response" | "$PY" -c "$validator" "$reg_a" "$reg_b" 2>/dev/null)" \
   && [ "$bytes" -gt 70000 ]; then
    echo "PASS mcp-input-payload: complete parseable ${bytes}-byte inner payload"
    exit 0
fi

echo "FAIL mcp-input-payload: response was truncated, malformed, or too small (${bytes:-invalid})" >&2
exit 1
