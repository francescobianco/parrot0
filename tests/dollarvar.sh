#!/usr/bin/env bash
#
# gen280 (U1b, NEXT.md) — '$' as an EXPLICIT variable marker in the prolog-like
# engine, independent of letter case. This is the dual-accept step: is_var now
# also treats a leading '$' as a variable, so a rule written with $-variables
# resolves. Uppercase/'_' still work (no migration of legacy fixtures needed),
# so the whole suite stays green; the case-freeing flip to $-only is gen B.
#
# RED before the change: is_var("$X") is false, so "$X" is a constant, the rule
# is about the atom "$X", and the query fails. GREEN after: $X is a variable.
#
# Loads rules from a temp .p0 via PARROT0_BASE and queries them over MCP.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "dollarvar: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS dollarvar: $1"; pass=$((pass+1)); }
no() { echo "FAIL dollarvar: $1" >&2; fail=$((fail+1)); }

base="$(mktemp /tmp/dollarvar.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
man(socrates).
mortal($X) :- man($X).
parent(tom, bob).
parent(bob, ann).
grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).
q(a).
uses_dollar($X) :- q($X).
uses_upper(X) :- q(X).
P0

out="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"mortal","args":["socrates"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"grandparent","args":["tom","ann"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"grandparent","args":["tom","bob"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"uses_dollar","args":["a"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"uses_upper","args":["a"]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1"; }

# GATE A: a unary $-variable rule resolves (mortal($X) :- man($X))
if line 2 | grep -q 'provable\\":true'; then
    ok "unary \$-variable rule resolves (mortal/man)"
else
    no "unary \$-rule did not resolve: $(line 2)"
fi

# GATE B: a multi-variable $-rule with a join resolves (grandparent)
if line 3 | grep -q 'provable\\":true'; then
    ok "multi-var \$-rule with join resolves (grandparent)"
else
    no "multi-var \$-rule did not resolve: $(line 3)"
fi

# GATE C: the join is real, not vacuous (tom is not his own grandparent's child)
if line 4 | grep -q 'provable\\":false'; then
    ok "join is discriminating (grandparent(tom,bob) is false)"
else
    no "join gave a false positive: $(line 4)"
fi

# GATE D: a $-variable rule still resolves (uses_dollar($X) :- q($X))
if line 5 | grep -q 'provable\\":true'; then
    ok "\$-variable rule resolves (uses_dollar)"
else
    no "\$-rule did not resolve: $(line 5)"
fi

# GATE E ($-only): a BARE UPPERCASE 'variable' is now a CONSTANT, so a rule
# written with it does NOT generalize — uses_upper(X):-q(X) cannot prove
# uses_upper(a). This is the end of dual-accept.
if line 6 | grep -q 'provable\\":false'; then
    ok "bare uppercase is a constant, not a variable (uses_upper(a) false)"
else
    no "uppercase still behaves as a variable (dual-accept not removed): $(line 6)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
