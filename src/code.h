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

/* gen196 (language-as-delta, CODE-MASTERY.md §7b): the PYTHON front-end. It emits
 * the SAME abstract facts as code_ingest — `code_function(name)` per `def` and
 * `code_calls(caller, callee)` per call inside a body — so EVERY downstream
 * analyzer (defines/locate/call-graph) works on Python unchanged. Only the
 * concrete syntax differs from C (the delta): `def name(...):` instead of a
 * brace head, and body scope tracked by INDENTATION instead of braces; `#` runs
 * to end of line. The reasoning is shared; the surface is derived. */
size_t code_ingest_py(KB *kb, const char *src,
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

/* gen184: blank (in place, with spaces) comments and string/char literals so
 * every scanner sees only real code. Applied at the entry points. */
void code_strip(char *s);

/* gen182: true if `src` defines a function literally named `want`. */
int code_defines(const char *src, const char *want);

/* gen182: scan directory `dir` (sandboxed like code_read_file) for a .c/.h file
 * that defines a function named `fnname`. On success writes the file's base name
 * into `out_file` and returns 1; returns 0 if none (or dir unreadable). The first
 * "find where to work" step toward repo-scale localization. */
int code_locate(const char *dir, const char *fnname, char *out_file, size_t out_sz);

/* gen187: F5 edit — write `src_path` to `out_path` with the whole-word identifier
 * `oldname` renamed to `newname` (comments and string/char literals untouched).
 * The original is never modified. Returns the replacement count, or -1 on error. */
int code_rename(const char *src_path, const char *oldname,
                const char *newname, const char *out_path);

/* gen191: F5 edit — write `src_path` to `out_path` with the top-level definition
 * of function `fnname` (its signature through the matching closing brace) removed.
 * Comments/string literals are skipped so a brace inside them never miscounts. The
 * original is never modified. Returns 1 if a definition was removed, 0 if none was
 * found, -1 on error (bad name, sandbox, read/write fail). */
int code_delete_function(const char *src_path, const char *fnname,
                         const char *out_path);

/* gen186: F5 verification — syntax-check `path` by running the C compiler in a
 * sandboxed subprocess (no shell, path whitelist, -fsyntax-only, timeout). A
 * compiler is a deterministic tool, not outsourced intelligence. Returns 1 if it
 * compiles, 0 if not (diagnostics in err_out), -1 if it could not be run. */
int code_compile(const char *path, char *err_out, size_t err_sz);

/* gen192: F5 verification by BUILDING — compile AND link `src_path` into a temp
 * executable (sandboxed subprocess, no shell, path whitelist, timeout; the temp
 * executable is removed before returning). Unlike code_compile's -fsyntax-only,
 * this reaches the LINK stage, so a call to a function that no longer exists is an
 * undefined reference (a real error), not just a warning. Returns 1 if it links,
 * 0 if not (first diagnostics in err_out), -1 if it could not be run. */
int code_build(const char *src_path, char *err_out, size_t err_sz);

/* gen185: reverse call graph across a directory (sandboxed, recursive) — the
 * functions whose body calls `target`. Writes up to `max` deduped caller names
 * into `out` and returns the count. The "blast radius" before an edit. */
size_t code_find_callers(const char *dir, const char *target,
                         char out[][KB_TERM_LEN], size_t max);

#endif /* PARROT0_CODE_H */
