#!/usr/bin/env bash
# Evidence-engine API regression test: discovery/supports must grow with the KB,
# and a bounded rendered proof must disclose (rather than hide) truncation.
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

cat >"$TMP/driver.c" <<'EOF'
#include "kb.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pass, fail;

static void check(int ok, const char *label) {
    if (ok) {
        printf("PASS kb-evidence-scale: %s\n", label);
        pass++;
    } else {
        fprintf(stderr, "FAIL kb-evidence-scale: %s\n", label);
        fail++;
    }
}

int main(void) {
    KB *kb = kb_create();
    if (!kb) return 2;

    char name[32];
    for (int i = 0; i < 257; i++) {
        snprintf(name, sizeof name, "h%03d", i);
        const char *a[] = { name, "\"x\"" };
        if (!kb_assert(kb, "scale_match", a, 2)) return 2;
    }
    KbEvidenceMatch hits[300];
    size_t n = kb_evidence_matches(kb, "scale_match", NULL, "x", hits, 300);
    check(n == 257 && strcmp(hits[256].hypothesis, "h256") == 0,
          "kb_evidence_matches discovers more than 256 hypotheses");

    char candidate_names[257][KB_TERM_LEN];
    const char *candidate_ptrs[257];
    for (int i = 0; i < 257; i++) {
        snprintf(candidate_names[i], KB_TERM_LEN, "b%03d", i);
        candidate_ptrs[i] = candidate_names[i];
        const char *a[] = { candidate_names[i], i == 256 ? "\"needle\"" : "default" };
        if (!kb_assert(kb, "scale_best", a, 2)) return 2;
    }
    char winner[KB_TERM_LEN], proof[8192];
    int score = 0;
    int r = kb_hypothesis_best(kb, "scale_best", "needle", NULL, 0,
                               winner, sizeof winner, &score,
                               proof, sizeof proof);
    check(r == 1 && score == 1 && strcmp(winner, "b256") == 0,
          "discovered hypothesis 257 can beat 256 defaults");

    r = kb_hypothesis_best(kb, "scale_best", "needle",
                           candidate_ptrs, 257,
                           winner, sizeof winner, &score,
                           proof, sizeof proof);
    check(r == 1 && strcmp(winner, "b256") == 0,
          "explicit candidate lists are not capped at 256");

    char evidence[64];
    for (int i = 0; i < 256; i++) {
        snprintf(evidence, sizeof evidence, "\"miss%03d\"", i);
        const char *a[] = { "only", evidence };
        if (!kb_assert(kb, "scale_support", a, 2)) return 2;
    }
    { const char *a[] = { "only", "\"needle\"" };
      if (!kb_assert(kb, "scale_support", a, 2)) return 2; }
    r = kb_hypothesis_best(kb, "scale_support", "needle", NULL, 0,
                           winner, sizeof winner, &score,
                           proof, sizeof proof);
    check(r == 1 && strstr(proof, "needle") != NULL,
          "support 257 participates in scoring and proof");

    char text[1024] = "";
    for (int i = 0; i < 13; i++) {
        snprintf(evidence, sizeof evidence, "\"token%02d\"", i);
        const char *a[] = { "proof_class", evidence };
        if (!kb_assert(kb, "scale_proof", a, 2)) return 2;
        char raw[32];
        snprintf(raw, sizeof raw, " token%02d", i);
        strncat(text, raw, sizeof text - strlen(text) - 1);
    }
    r = kb_hypothesis_best(kb, "scale_proof", text, NULL, 0,
                           winner, sizeof winner, &score,
                           proof, sizeof proof);
    check(r == 1 && score == 13 && strstr(proof, "token12") != NULL,
          "proof retains every support beyond the old twelve-row cap");

    char tiny[96];
    r = kb_hypothesis_best(kb, "scale_proof", text, NULL, 0,
                           winner, sizeof winner, &score,
                           tiny, sizeof tiny);
    check(r == 1 && strstr(tiny, "[truncated]") != NULL,
          "a bounded proof marks explicit truncation");

    { const char *a[] = { "f($X)", "\"x\"" };
      if (!kb_assert(kb, "scale_nonground", a, 2)) return 2; }
    r = kb_hypothesis_best(kb, "scale_nonground", "x", NULL, 0,
                           winner, sizeof winner, &score,
                           proof, sizeof proof);
    check(r == 0 && winner[0] == '\0',
          "discovery rejects partially-ground hypothesis names");

    { const char *a[] = { "$X", "$X" };
      if (!kb_assert(kb, "scale_unit", a, 2)) return 2; }
    const char *unit_aa[] = { "a", "a" };
    const char *unit_ab[] = { "a", "b" };
    const char *unit_pattern[] = { "a", NULL };
    char bindings[4][KB_TERM_LEN];
    size_t nb = kb_match(kb, "scale_unit", unit_pattern, 2, bindings, 4);
    check(kb_query(kb, "scale_unit", unit_aa, 2) == 1 &&
          kb_query(kb, "scale_unit", unit_ab, 2) == 0 &&
          nb == 1 && strcmp(bindings[0], "a") == 0,
          "fast paths preserve variable-bearing unit-clause unification");

    { const char *a[] = { "f(a,b)" };
      if (!kb_assert(kb, "scale_struct_query", a, 1)) return 2; }
    { const char *a[] = { "f(a,b)", "ok" };
      if (!kb_assert(kb, "scale_struct_match", a, 2)) return 2; }
    const char *struct_query[] = { "f(a, b)" };
    const char *struct_pattern[] = { "f(a, b)", NULL };
    nb = kb_match(kb, "scale_struct_match", struct_pattern, 2, bindings, 4);
    check(kb_query(kb, "scale_struct_query", struct_query, 1) == 1 &&
          nb == 1 && strcmp(bindings[0], "ok") == 0,
          "fast paths retain canonical structural unification");

    const char *null_arg[] = { NULL };
    const char *valid_arg[] = { "x" };
    check(kb_query(kb, NULL, valid_arg, 1) == 0 &&
          kb_query(kb, "scale_struct_query", null_arg, 1) == 0 &&
          kb_match(kb, NULL, null_arg, 1, bindings, 4) == 0,
          "invalid query predicates/arguments return zero without crashing");

    { const char *a[] = { "cue", "2147483647" };
      if (!kb_assert(kb, "evidence_weight", a, 2)) return 2; }
    { const char *a[] = { "maxed", "\"left\"" };
      if (!kb_assert(kb, "scale_weight", a, 2)) return 2; }
    { const char *a[] = { "maxed", "\"right\"" };
      if (!kb_assert(kb, "scale_weight", a, 2)) return 2; }
    r = kb_hypothesis_best(kb, "scale_weight", "left right", NULL, 0,
                           winner, sizeof winner, &score,
                           proof, sizeof proof);
    check(r == 1 && score == INT_MAX && strcmp(winner, "maxed") == 0,
          "large evidence sums saturate the public score without signed overflow");

    kb_destroy(kb);
    printf("kb-evidence-scale: %d passed, %d failed\n", pass, fail);
    return fail != 0;
}
EOF

if [[ -n "${KB_EVIDENCE_CFLAGS:-}" ]]; then
    read -r -a compile_flags <<<"$KB_EVIDENCE_CFLAGS"
else
    compile_flags=(-std=c11 -Wall -Wextra -Wpedantic -Wno-format-truncation -O2)
fi
"${CC:-cc}" "${compile_flags[@]}" -I"$ROOT/src" \
    "$ROOT/src/kb.c" "$TMP/driver.c" -o "$TMP/driver"
"$TMP/driver"
