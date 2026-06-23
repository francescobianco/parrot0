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

/* gen200: F5 edit (X7 patch synthesis) — write `src_path` to `out_path` with every
 * occurrence of the exact code expression `oldexpr` replaced by `newexpr`, in CODE
 * regions only (comments and string/char literals copied verbatim — incl. Python
 * `#` lines). Unlike code_rename (a single identifier) this replaces an arbitrary
 * token sequence: the expression/statement-level transformation a real bug fix
 * needs (e.g. `= 1` -> `= right`). The caller supplies enough context for `oldexpr`
 * to be unambiguous. The original is never modified. Returns the replacement count,
 * or -1 on error. */
int code_replace_expr(const char *src_path, const char *oldexpr,
                      const char *newexpr, const char *out_path);

/* gen200: structural SYMMETRY-BREAK localization (F4/X6 by structure, NOT by
 * issue-text association — CODE-MASTERY §4 refuses the phrasebook). Two parallel
 * branches should mirror each other; when one assigns its own operand variable but
 * the sibling assigns a LITERAL where the analogous variable belongs, that literal
 * is the suspect. In function `fnname` (or, if NULL, any function) of the file at
 * `src_path`, finds a statement `T[ ...subscript using param Q... ] = <literal>`
 * that has a healthy sibling `T2[ ...subscript using param P... ] = P` (RHS equals
 * its own subscript param), and proposes replacing the literal with Q. Writes the
 * exact (trimmed) old statement to `old_stmt` and the fixed statement to `new_stmt`;
 * returns 1 if a symmetry break was found, 0 if none, -1 on error. Purely
 * structural — it names no file, function, or variable in advance. */
int code_symmetry_fix(const char *src_path, const char *fnname,
                      char *old_stmt, size_t old_sz,
                      char *new_stmt, size_t new_sz);

/* gen201: structural DISCARDED-RESULT localization (a second general bug smell,
 * judged by the real test like the symmetry one — CODE-MASTERY §4/§6). A bare
 * expression statement `RECV.METHOD(args)` whose value is thrown away, where METHOD
 * is a known PURE (value-returning, non-mutating) string/bytes transform (replace,
 * strip, lower, upper, ...), is a no-op bug: the new value is computed and dropped.
 * The fix assigns it back to RECV — in place (`RECV[:] = ...`) when RECV is a
 * parameter (rebinding a parameter is provably a no-op for the caller), else by
 * rebinding (`RECV = ...`). In function `fnname` (or any if NULL) of `src_path`,
 * writes the trimmed old statement and the fixed statement; 1 if found, 0 none,
 * -1 on error. Names nothing in advance — purely structural over Python value
 * semantics (the KB of pure methods is a language fact, not a phrasebook). */
int code_find_discarded_result(const char *src_path, const char *fnname,
                               char *old_stmt, size_t old_sz,
                               char *new_stmt, size_t new_sz);

/* gen204: structural CONDITION-ASYMMETRY localization (a third general bug smell;
 * the symmetry idea of gen200 carried from assignments to guards). When a function
 * guards branches on `X.ATTR is None` for several operands but one branch tests a
 * BARE name `Y is None` — and that same Y is used as `Y.ATTR` elsewhere in the
 * function (so `.ATTR` clearly belongs) — the bare test is the inconsistency; the
 * fix is `Y.ATTR is None`. (E.g. siblings test `self.mask is None`/`operand.mask`,
 * but one branch tests bare `operand is None`.) In `fnname` (or any if NULL) of
 * `src_path`, writes the trimmed old condition line and the fixed line; 1 if found,
 * 0 none, -1 on error. Grounded (the `.ATTR` use must already exist), not fitted. */
int code_find_cond_asymmetry(const char *src_path, const char *fnname,
                             char *old_stmt, size_t old_sz,
                             char *new_stmt, size_t new_sz);

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

/* gen198: F5 verification by RUNNING — compile+link `src_path` into a temp
 * executable, then EXECUTE it in a sandboxed subprocess and observe the real
 * result. The rung past code_build ("it links") to "it ran and exited with N" —
 * the core swe-bench primitive (did the test pass?). A compiler+process is a
 * deterministic tool, not outsourced intelligence (CODE-MASTERY.md §4). Returns:
 *   1  -> built and ran to completion; *exit_code holds the program's exit status
 *   0  -> built but did NOT exit normally (killed by a signal / timed out)
 *  -1  -> could not build or could not run (sandbox / fork / link failure);
 *         on a build failure the first diagnostics are in `err_out`.
 * The temp executable is removed before returning. */
int code_run(const char *src_path, int *exit_code, char *err_out, size_t err_sz);

#endif /* PARROT0_CODE_H */
