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

/* gen175: symbolic execution (B5). Evaluate the function named `want` (or the
 * FIRST definition if `want` is NULL) in `src` on the integer arguments `argv`,
 * by reading its single `return <expr>;` and COMPUTING the expression with the
 * parameters bound positionally. Supports integers, the parameter names, the
 * operators + - * / % and parentheses. Nothing is executed — the value is
 * derived from the structure. Writes the result to *out and returns 1 on
 * success; returns 0 if it cannot (no single arithmetic return, an unknown
 * identifier, a call/unsupported form, or division by zero). */
int code_eval(const char *src, const char *want,
              const long *argv, size_t argc, long *out);

/* gen181: read a source file into `buf` so parrot0 can answer structural
 * questions about a real file, not just an inline snippet. Sandboxed to the
 * working directory: relative paths only, no '..' traversal, no absolute/home
 * paths. Returns 1 on success (whole file fit and was non-empty), 0 otherwise. */
int code_read_file(const char *path, char *buf, size_t bufsz);

/* gen182: true if `src` defines a function literally named `want`. */
int code_defines(const char *src, const char *want);

/* gen182: scan directory `dir` (sandboxed like code_read_file) for a .c/.h file
 * that defines a function named `fnname`. On success writes the file's base name
 * into `out_file` and returns 1; returns 0 if none (or dir unreadable). The first
 * "find where to work" step toward repo-scale localization. */
int code_locate(const char *dir, const char *fnname, char *out_file, size_t out_sz);

#endif /* PARROT0_CODE_H */
