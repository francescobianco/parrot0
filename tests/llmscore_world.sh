#!/usr/bin/env bash
# gen235: common world facts must improve llmscore without destroying dynamic
# learning tests. Prove the layer can be present, suppressed, and relearned.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "llmscore-world: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc env_flag input expected_first_line
    local desc="$1" world="$2" input="$3" want="$4" got
    got="$(printf '%s\n/quit\n' "$input" | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS="$world" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$want" ]; then
        echo "PASS llmscore-world: $desc"; pass=$((pass+1))
    else
        echo "FAIL llmscore-world: $desc - want [$want] got [$got]" >&2; fail=$((fail+1))
    fi
}

expect "base world knows france capital" "1" \
    "what is the capital of france?" \
    "Paris."

expect "world layer can be suppressed" "0" \
    "what is the capital of france?" \
    "I do not know the relation capital yet, so I cannot answer the capital of france. You can teach me with thing is the capital of france, or give facts/rules to reason from."

learned="$(printf '%s\n%s\n/quit\n' \
    "paris is the capital of france" \
    "what is the capital of france?" \
    | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 "$BIN" 2>/dev/null | tail -1)"
if [ "$learned" = "Paris." ]; then
    echo "PASS llmscore-world: suppressed world can relearn capital"
    pass=$((pass+1))
else
    echo "FAIL llmscore-world: suppressed world can relearn capital - want [Paris.] got [$learned]" >&2
    fail=$((fail+1))
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
