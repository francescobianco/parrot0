#!/usr/bin/env bash
#
# gen11 multi-goal resolution: a rule whose body is a conjunction of goals
# with a shared intermediate variable, resolved by backward chaining.
#   grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "multigoal: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/family.p0"
loader="$tmp/loader.p0"
unfinished="$tmp/unfinished.p0"
saved="$tmp/saved.p0"
oversized="$tmp/oversized.p0"
bodyoverflow="$tmp/body-overflow.p0"

cat > "$base" <<'EOF'
% a two-goal rule with a shared intermediate variable $Y
parent(tom, bob).
parent(bob, ann).
grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).
EOF

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$base" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS multigoal: $1"; pass=$((pass+1))
    else echo "FAIL multigoal: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "grandparent via two-goal rule" "is tom the grandparent of ann?" "Yes."
expect "non-grandparent is false"      "is tom the grandparent of bob?" "No."
expect "variable query through rule"   "who is the grandparent of ann?" "tom."

# The loader consumes LOGICAL clauses, not physical lines. Keep this in the
# multi-goal ratchet because a continued body is only useful if the resulting
# rule can actually prove something. The long value deliberately includes `%`,
# a period and commas: all are content while quoted, never comment/split syntax.
long_value='This quoted value is deliberately longer than one hundred and twenty-seven bytes; it must remain complete through load, match, save, and reload, including 100% of this tail, a period. and two commas, here, there.'
cat > "$loader" <<'P0'
% a whole comment containing ignored(nope). must not become knowledge
seed(alpha). seed(beta). % ignored(nope). after the comment marker
enabled(alpha).
derived($X) :-
    seed($X),                 % a comment may divide physical lines
    enabled($X).
ready.
clean :- naf(blocked).
factorial(0, 1).
factorial($N, $R) :- gt($N, 0), is($N1, sub($N, 1)),
                     factorial($N1, $R1), is($R, mul($N, $R1)).
P0
printf 'quoted_marker(item, "%s").\n' "$long_value" >> "$loader"
printf 'good(ok). dangling(no_period)' > "$unfinished"
awk 'BEGIN { printf "huge(\""; for (i = 0; i < 30000; i++) printf "x";
             print "\"). after(limit)." }' > "$oversized"
cat > "$bodyoverflow" <<'P0'
kept(ok).
partial($X) :- g1($X), g2($X), g3($X), g4($X), g5($X),
               g6($X), g7($X), g8($X), g9($X).
g1(ok). g2(ok). g3(ok). g4(ok). g5(ok). g6(ok). g7(ok). g8(ok). g9(ok).
after_body(ok).
P0

cat > "$tmp/loader-driver.c" <<'EOC'
#include "kb.h"
#include <stdio.h>
#include <string.h>

static int exact_long(KB *kb, const char *raw)
{
    const char *q[] = { "item", NULL };
    char out[1][KB_TERM_LEN], expected[KB_TERM_LEN];
    if (snprintf(expected, sizeof expected, "\"%s\"", raw) < 0) return 0;
    return kb_match(kb, "quoted_marker", q, 2, out, 1) == 1 &&
           strcmp(out[0], expected) == 0;
}

int main(int argc, char **argv)
{
    KB *kb, *again, *bad;
    const char *alpha[] = { "alpha" }, *beta[] = { "beta" };
    const char *okarg[] = { "ok" }, *dangling[] = { "no_period" };
    const char *nope[] = { "nope" };
    int multiline = 0, same_line = 0, zero_arity = 0, comments = 0;
    int long_load = 0, roundtrip = 0, eof_rejected = 0, oversize_rejected = 0;
    int recursive_depth = 0, body_overflow_rejected = 0;
    if (argc != 7 || strlen(argv[6]) <= 127) return 2;

    kb = kb_create();
    if (!kb) return 3;
    kb_load(kb, argv[1]);
    multiline = kb_query(kb, "derived", alpha, 1) &&
                !kb_query(kb, "derived", beta, 1);
    same_line = kb_query(kb, "seed", alpha, 1) &&
                kb_query(kb, "seed", beta, 1);
    zero_arity = kb_query(kb, "ready", NULL, 0) &&
                 kb_query(kb, "clean", NULL, 0);
    long_load = exact_long(kb, argv[6]);
    comments = !kb_query(kb, "ignored", nope, 1) && long_load;
    {
        const char *fq[] = { "15", NULL };
        char out[1][KB_TERM_LEN];
        recursive_depth = kb_match(kb, "factorial", fq, 2, out, 1) == 1 &&
                          strcmp(out[0], "1307674368000") == 0;
    }
    if (kb_save(kb, argv[3], KB_SESSION) > 0) {
        again = kb_create();
        if (again) {
            kb_load(again, argv[3]);
            roundtrip = exact_long(again, argv[6]) &&
                        kb_query(again, "derived", alpha, 1) &&
                        kb_query(again, "ready", NULL, 0) &&
                        kb_query(again, "clean", NULL, 0);
            kb_destroy(again);
        }
    }
    kb_destroy(kb);

    bad = kb_create();
    if (!bad) return 4;
    kb_load(bad, argv[2]);
    eof_rejected = kb_query(bad, "good", okarg, 1) &&
                   !kb_query(bad, "dangling", dangling, 1);
    kb_destroy(bad);

    bad = kb_create();
    if (!bad) return 6;
    kb_load(bad, argv[5]);
    {
        const char *ok[] = { "ok" };
        body_overflow_rejected = kb_query(bad, "kept", ok, 1) &&
                                 !kb_query(bad, "partial", ok, 1) &&
                                 kb_query(bad, "after_body", ok, 1);
    }
    kb_destroy(bad);

    bad = kb_create();
    if (!bad) return 5;
    kb_load(bad, argv[4]);
    {
        const char *limit[] = { "limit" };
        const char *any[] = { NULL };
        char out[1][KB_TERM_LEN];
        oversize_rejected = kb_match(bad, "huge", any, 1, out, 1) == 0 &&
                            kb_query(bad, "after", limit, 1);
    }
    kb_destroy(bad);

    printf("multiline=%d\n", multiline);
    printf("same_line=%d\n", same_line);
    printf("zero_arity=%d\n", zero_arity);
    printf("comments=%d\n", comments);
    printf("long_roundtrip=%d\n", long_load && roundtrip);
    printf("eof_rejected=%d\n", eof_rejected);
    printf("oversize_rejected=%d\n", oversize_rejected);
    printf("recursive_depth=%d\n", recursive_depth);
    printf("body_overflow_rejected=%d\n", body_overflow_rejected);
    return 0;
}
EOC

check_loader() {
    if printf '%s\n' "$loader_out" | grep -Fxq "$1=1"; then
        echo "PASS multigoal: $2"; pass=$((pass+1))
    else
        echo "FAIL multigoal: $2" >&2; fail=$((fail+1))
    fi
}

if "${CC:-cc}" -std=c11 -Wall -Wextra -Wpedantic -Wno-format-truncation \
        -I"$ROOT/src" \
        "$tmp/loader-driver.c" "$ROOT/src/kb.c" -o "$tmp/loader-driver"; then
    loader_out="$("$tmp/loader-driver" "$loader" "$unfinished" "$saved" \
                  "$oversized" "$bodyoverflow" "$long_value" \
                  2>"$tmp/loader.stderr")"
    check_loader multiline "a multiline rule is loaded and actually queried"
    check_loader same_line "multiple clauses on one physical line all load"
    check_loader zero_arity "zero-arity facts and rule heads are queryable"
    check_loader comments "comments are ignored while quoted percent is content"
    check_loader long_roundtrip "a quoted value over 127 bytes round-trips exactly"
    check_loader eof_rejected "an unterminated EOF fragment is not partially loaded"
    check_loader oversize_rejected "an over-budget clause is dropped whole and scanning resumes"
    check_loader recursive_depth "factorial(15) preserves recursive depth with long terms"
    check_loader body_overflow_rejected "a body over KB_MAX_BODY is rejected whole, never truncated"
    if grep -Fq 'unterminated clause at EOF' "$tmp/loader.stderr"; then
        echo "PASS multigoal: unterminated EOF is rejected with an explicit diagnostic"
        pass=$((pass+1))
    else
        echo "FAIL multigoal: unterminated EOF had no explicit diagnostic" >&2
        fail=$((fail+1))
    fi
    if grep -Fq 'clause too large' "$tmp/loader.stderr"; then
        echo "PASS multigoal: an over-budget clause has an explicit bounded-load diagnostic"
        pass=$((pass+1))
    else
        echo "FAIL multigoal: an over-budget clause had no explicit diagnostic" >&2
        fail=$((fail+1))
    fi
    if grep -Fq 'too many body goals' "$tmp/loader.stderr"; then
        echo "PASS multigoal: body overflow has an explicit lossless-load diagnostic"
        pass=$((pass+1))
    else
        echo "FAIL multigoal: body overflow had no explicit diagnostic" >&2
        fail=$((fail+1))
    fi
else
    echo "FAIL multigoal: loader focal driver did not compile" >&2
    fail=$((fail+12))
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
