#!/usr/bin/env bash
#
# gen283 (U3, NEXT.md / teach-comprehension-via-mcp.md §6.1) — STRUCTURAL
# unification over compound terms. Until now an argument was a flat atom, so
# "s(z)" could not unify with "s($N)" and a recursive COMPUTATION was
# unteachable (Secchio D.1: memorization, not computation). U3 makes unify SEE
# structure in the string: f(a…) is a compound; unify recurses; a variable binds
# to a whole sub-structure. Peano arithmetic and list recursion become teachable
# knowledge — via .p0 AND via MCP kb.assert_clause.
#
# RED before U3: nat(s(z)) false, add(...) empty. GREEN after: they compute.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "compound: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS compound: $1"; pass=$((pass+1)); }
no() { echo "FAIL compound: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: Peano + lists from a .p0 file ----
base="$(mktemp /tmp/compound.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
nat(z).
nat(s($N)) :- nat($N).
add(z, $Y, $Y).
add(s($X), $Y, s($Z)) :- add($X, $Y, $Z).
len(nil, z).
len(cons($H, $T), s($N)) :- len($T, $N).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"nat","args":["s(s(z))"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"add","args":["s(z)","s(z)",null]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"len","args":["cons(a, cons(b, nil))",null]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":true'; then
    ok ".p0: recursion over structure (nat(s(s(z))) true)"
else
    no ".p0: nat(s(s(z))) not provable: $(l1 2)"
fi
if l1 3 | grep -q 's(s(z))'; then
    ok ".p0: Peano addition computes (1+1=s(s(z)))"
else
    no ".p0: add(s(z),s(z),?) wrong: $(l1 3)"
fi
if l1 4 | grep -q 's(s(z))'; then
    ok ".p0: list length computes (2 = s(s(z)))"
else
    no ".p0: len(cons(a,cons(b,nil)),?) wrong: $(l1 4)"
fi

# ---- Path 2: the RECURSIVE Peano clause taught over MCP (compound-term args) ----
# The base case add(z,$Y,$Y) is a variable-carrying unit clause, so it comes from
# the .p0 (kb.assert quotes $-literals, as facts must stay ground). The point of
# this path is that kb.assert_clause carries COMPOUND-TERM args (s($X), s($Z))
# through the MCP boundary and they unify structurally. 2+1 = s(s(s(z))).
base2="$(mktemp /tmp/compound2.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
printf 'add(z, $Y, $Y).\n' > "$base2"

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"add","args":["s($X)","$Y","s($Z)"]},"body":[{"pred":"add","args":["$X","$Y","$Z"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"add","args":["s(s(z))","s(z)",null]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 3 | grep -q '\\"ok\\":true'; then
    ok "MCP: kb.assert_clause accepts compound-term args (s(\$X))"
else
    no "MCP: kb.assert_clause rejected compound args: $(l2 3)"
fi
if l2 4 | grep -q 's(s(s(z)))'; then
    ok "MCP: Peano recursion computes over MCP-taught clause (2+1=s(s(s(z))))"
else
    no "MCP: add(s(s(z)),s(z),?) wrong: $(l2 4)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
