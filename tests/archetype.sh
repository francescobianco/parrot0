#!/usr/bin/env bash
# Symbol-agnostic relational inference (docs/plans/generative-prolog.md).
#
# "Before knowledge there is the capacity to build the abstraction." Shown a
# worked example over ARBITRARY symbols — a>b, x:a ⇒ x:b — parrot0 must abstract
# the relational skeleton and re-instantiate it on a held-out probe (g>f, n:g ⇒ ?
# -> n:f) WITHOUT knowing what any symbol means. The turn is flattened to one
# line: segments separated by ' / ' or ';', the worked demo first, the query
# (same #premises) ending in '?'. mod_archetype owns this shape.
#
# The suite has three parts:
#   (1) the plan's own three canonical micro-examples (n:f, y:n, r:q);
#   (2) INVENTED inferences with fresh symbols/operators each time — transitivity,
#       symmetry, multi-char atoms, numeric atoms, mixed UTF-8 — proving the
#       capability generalizes, it is not a phrasebook of the plan's cases;
#   (3) honesty checks — a query that does NOT instantiate the demo's structure
#       must be declined ("I won't guess"), and ordinary prose must NOT be hijacked.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "archetype: binary not built" >&2; exit 1; }

pass=0 fail=0

ask() { printf '%s\n/quit\n' "$1" | PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null | head -1; }

# the inferred answer must appear at the head of the reply (reply is
# "<answer> — same relational pattern as …")
infers() { # desc input expected-answer
    local got; got="$(ask "$2")"
    case "$got" in
        "$3"*) echo "PASS archetype: $1 [$3]"; pass=$((pass+1));;
        *) echo "FAIL archetype: $1 — want [$3…] got [$got]" >&2; fail=$((fail+1));;
    esac
}

# the reply must be an honest refusal to guess, not an inference
declines() { # desc input
    local got; got="$(ask "$2")"
    case "$got" in
        *"won't guess"*) echo "PASS archetype: $1 (declined)"; pass=$((pass+1));;
        *) echo "FAIL archetype: $1 — want a decline, got [$got]" >&2; fail=$((fail+1));;
    esac
}

# the reply must NOT come from the archetype module (no "relational pattern")
not_hijacked() { # desc input
    local got; got="$(ask "$2")"
    case "$got" in
        *"relational pattern"*|*"won't guess"*)
            echo "FAIL archetype: $1 — prose hijacked by archetype: [$got]" >&2; fail=$((fail+1));;
        *) echo "PASS archetype: $1 (left to other modules)"; pass=$((pass+1));;
    esac
}

echo "--- the plan's canonical micro-examples ---"
infers   "substitution  a>b,x:a ⇒ x:b"            'a>b / x:a / x:b / g>f / n:g / ?'           'n:f'
infers   "invented glyph ◇ (multibyte operator)"  'α◇β / x:α / x:β / m◇n / y:m / ?'           'y:n'
infers   "currency glyph ¤ (multibyte operator)"  'µ¤ν / κ:µ / κ:ν / p¤q / r:p / ?'           'r:q'

echo "--- invented inferences (fresh symbols, not in the plan) ---"
infers   "transitivity  a>b,b>c ⇒ a>c"            'a>b / b>c / a>c / g>h / h>i / ?'           'g>i'
infers   "transitivity, 3-premise chain"          'a>b / b>c / c>d / a>d / p>q / q>r / r>s / ?' 'p>s'
infers   "symmetry  a~b ⇒ b~a"                     'a~b / b~a / m~n / ?'                       'n~m'
infers   "swap over ':'  a:b ⇒ b:a"               'a:b / b:a / pp:qq / ?'                     'qq:pp'
infers   "multi-char ASCII atoms"                  'cat>dog / x:cat / x:dog / mouse>rat / y:mouse / ?' 'y:rat'
infers   "UTF-8 letters, ASCII operator"          'α>β / x:α / x:β / µ>ν / y:µ / ?'           'y:ν'
infers   "numbers used as opaque atoms"           '1>2 / x:1 / x:2 / 7>8 / k:7 / ?'           'k:8'
infers   "invented operator '@'"                  'a@b / x:a / x:b / s@t / w:s / ?'           'w:t'
infers   "semicolon as the segment separator"     'a>b; x:a; x:b; g>f; n:g; ?'                'n:f'

echo "--- honesty: no archetype ⇒ refuse, and don't hijack prose ---"
declines "query breaks the shared 'middle term'"  'a>b / x:a / x:b / g>f / n:h / ?'
declines "conclusion uses an unwitnessed symbol"  'a-b / b-c / a=c / x-y / y-z / ?'
not_hijacked "ordinary prose question"            'what is the capital of france?'
not_hijacked "arithmetic is not an archetype turn" 'what is 2 plus 2?'

echo
echo "archetype: $pass passed, $fail failed"
[ "$fail" -eq 0 ]