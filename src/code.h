#ifndef PARROT0_CODE_H
#define PARROT0_CODE_H

#include <stddef.h>
#include "kb.h"

/* gen173: AST-as-KB — the first step of docs/CODE-MASTERY.md. A code snippet is
 * just another corpus: parrot0 parses its structure DETERMINISTICALLY (the C
 * grammar is formal, so primitives-first wins — no statistics) and asserts that
 * structure into the SAME live KB it uses for the world, so the existing engine
 * can reason over code. This first cut recovers function DEFINITIONS, asserting
 * `code_function(name)` per definition (origin KB_BASE, straight into RAM).
 *
 * Writes up to `max` function names found in THIS snippet into `names` (each up
 * to KB_TERM_LEN) and returns how many were written — so the caller can answer
 * honestly about the snippet in hand, while the facts persist in the KB for
 * later cross-questions. */
size_t code_ingest(KB *kb, const char *src,
                   char names[][KB_TERM_LEN], size_t max);

#endif /* PARROT0_CODE_H */
