#!/usr/bin/env bash
# parrot0 impersonation benchmark.
# Measures: can parrot0 hold a character across turns?
# Expectations are what a SUCCESSFUL impersonation would produce.
# gen100 baseline: ~5% (only arithmetic passes within any role).
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "impersonate: binary not built" >&2; exit 1; }

pass=0; fail=0; total=0; checks=0

check() {
    local name="$1" q="$2" got="$3" want="$4"
    checks=$((checks+1))
    if echo "$got" | grep -qi "$want"; then
        pass=$((pass+1))
    else
        echo "FAIL $name: '$q' → got='$got' want~'$want'" >&2
        fail=$((fail+1))
    fi
}

scenario() {
    local name="$1" role="$2" q1="$3" w1="$4" q2="$5" w2="$6"
    total=$((total+1))
    local tmp; tmp="$(mktemp)"
    printf '%s\n%s\n%s\n/quit\n' "$role" "$q1" "$q2" \
        | PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null > "$tmp"
    local r1 r2 r3
    r1="$(sed -n '1p' "$tmp")"; r2="$(sed -n '2p' "$tmp")"; r3="$(sed -n '3p' "$tmp")"
    [ -n "$w1" ] && check "$name" "$q1" "$r2" "$w1"
    [ -n "$w2" ] && check "$name" "$q2" "$r3" "$w2"
    rm -f "$tmp"
}

# ===================================================================
# 10 impersonation scenarios. Each tests 1-2 in-character questions.
# Expected patterns represent DESIRED behaviour — they WILL fail now.
# ===================================================================

scenario "Dog-name" \
    "pretend you are a dog named Rex. answer as Rex the dog." \
    "what is your name?" "rex" \
    "do you bark?" "bark\|woof\|yes"

scenario "Name-override" \
    "your name is now Mario. forget your old name." \
    "what is your name?" "mario" \
    "who are you?" "mario"

scenario "Robot-role" \
    "you are a robot named Beep. answer as Beep the robot." \
    "what are you?" "beep\|robot" \
    "do you compute?" "yes\|compute\|process"

scenario "Teacher-math" \
    "you are a math teacher. answer as a teacher." \
    "what is 2 plus 2?" "4" \
    "explain why 2 plus 2 is 4" "add\|sum\|plus"

scenario "Dante-role" \
    "sei dante alighieri, il sommo poeta. rispondi come dante." \
    "chi sei?" "dante\|poeta" \
    "cosa hai scritto?" "commedia\|divina\|inferno"

scenario "Cleopatra" \
    "you are cleopatra, queen of egypt. answer as her." \
    "where do you rule?" "egypt" \
    "what is your title?" "queen"

scenario "Child-age" \
    "pretend you are a 5-year-old child named Leo. answer as Leo." \
    "how old are you?" "5\|five" \
    "what is your favorite color?" "blue\|red\|color"

scenario "Spy-code" \
    "you are a spy named Shadow. your secret code is 007." \
    "what is your code?" "007" \
    "who do you work for?" "agency\|mi6\|cia"

scenario "Exit-role" \
    "you are now a cat named Whiskers. now stop pretending and be yourself." \
    "what is your name?" "whiskers\|cat" \
    "who are you really?" "parrot0\|bot\|program"

scenario "Self-in-role" \
    "pretend you are sherlock holmes solving a case." \
    "what are you really underneath?" "parrot0\|bot\|program" \
    "who is the culprit?" ""

echo "---"
echo "scenarios: $total, checks: $checks, passed: $pass, failed: $fail"
if [ "$checks" -gt 0 ]; then
    echo "Impersonation score: $(( pass * 100 / checks ))%"
fi
[ "$fail" -eq 0 ]
