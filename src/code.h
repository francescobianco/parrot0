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

/* gen210: structural CASE-FOLDING localization (a fourth general bug smell). A parser
 * that classifies input by matching ALL-CAPS keyword literals should handle case
 * CONSISTENTLY; doing it case-SENSITIVELY in two different mechanisms at once is the
 * smell. Fires only on that coupling, in `src_path`:
 *   (A) a `re.compile(<single arg>)` with no flags argument, AND
 *   (B) an `IDENT == "<ALLCAPS>"` equality (file-derived value vs an all-caps literal).
 * The coupled fix makes both case-insensitive: (A) add `, re.IGNORECASE`; (B) wrap the
 * identifier with `.upper()`. Writes up to `max` (old, new) edit pairs sliced from the
 * REAL source text (so spacing is exact), and returns the count (>=1 only when BOTH
 * shapes are present — conservative, to avoid firing on enums/sentinels), 0 if the
 * coupling is absent, -1 on error. Grounded (both mechanisms must already exist), not
 * fitted to one instance; the real test suite is the judge. */
int code_find_case_folding(const char *src_path,
                           char olds[][256], char news[][256], size_t max);

/* gen256: X6 SELF-localization across a TREE. Until now the harness handed parrot0
 * the buggy FILE (find | head -1 in parrot_solve.sh) — localization was the
 * harness's intelligence, not parrot0's. This walks `dir` (and subdirectories,
 * hidden entries skipped, depth-bounded, same sandbox as code_locate) and runs the
 * WHOLE structural-smell chain (symmetry break, discarded result, condition
 * asymmetry, case folding) on every .c/.h/.py source; the first file where any
 * smell fires is written to `out_file`. It names no file in advance — the smells
 * are the localizer. Returns 1 if a file fired, 0 if none, -1 on error/sandbox. */
int code_smell_tree(const char *dir, char *out_file, size_t out_sz);

/* gen257 (Track 5.1, outer-circle codebase work): OR-chain perception. Counts the
 * runs of >=2 calls to `fnname` joined only by `||` in the file at `src_path` —
 * the structural signature of a word list encoded as code (a C phrasebook).
 * GENERIC: `fnname` is a parameter, nothing about this codebase is baked in; it
 * works on any C-family source (Python's `or` chains are a later pull). Writes up
 * to `max` chain start lines into `lines` and the number of chained calls into
 * `total_calls` (both optional). Returns the chain count, or -1 on error.
 * Perception only: nothing is modified. */
int code_find_or_chains(const char *src_path, const char *fnname,
                        int *lines, size_t max, int *total_calls);

/* gen257: the same perception over a TREE (sandboxed like code_locate). Sums the
 * chains across every .c/.h under `dir`, reports how many files contain at least
 * one chain (`files_hit`), the total chained calls (`calls`), and the densest
 * file (`top_file`, `top_chains`). Returns the total chain count, -1 on error. */
int code_orchain_tree(const char *dir, const char *fnname,
                      int *files_hit, int *calls,
                      char *top_file, size_t top_sz, int *top_chains);

/* gen260 (Track 5.3): vocabulary extraction for an OR-chain. Uses the same
 * structural detector as code_find_or_chains, but reads the original source spans
 * of the calls in each chain and collects double-quoted string arguments (deduped,
 * in source order). This is still PERCEPTION: no patch is written and no KB facts
 * are emitted. Returns the number of words written, or -1 on error. */
int code_orchain_vocabulary(const char *src_path, const char *fnname,
                            char words[][KB_TERM_LEN], size_t max,
                            int *total_chains, int *total_calls);

/* gen260: the same vocabulary extraction over a tree of .c/.h files. */
int code_orchain_vocabulary_tree(const char *dir, const char *fnname,
                                 char words[][KB_TERM_LEN], size_t max,
                                 int *files_hit, int *total_chains,
                                 int *total_calls);

/* gen261 (Track 5.3): the first WRITING primitive of the derived plan — turn the
 * vocabulary an OR-chain encodes into loadable facts. For each chain of calls to
 * `fnname`, writes `pred(<stem>_chain<LINE>, "word").` lines to `out_path` (a new
 * .p0 file; the source is never touched; sandbox rules as code_replace_expr). One
 * key per chain SITE, so a later patch step can point each site at its own
 * vocabulary. Returns the number of facts written, 0 if there was nothing to
 * write (no file is created), -1 on error. */
int code_orchain_emit_facts(const char *src_path, const char *fnname,
                            const char *pred, const char *out_path,
                            int *total_chains);

/* gen262 (Track 5.3): the patch step of the derived plan — replace each OR-chain
 * of calls to `fnname` with ONE call to the target codebase's lookup primitive.
 * gen271 (Track 5.4): the SHAPE of that call is knowledge, not a C constant —
 * `call_tpl` is a template whose whole-word tokens FN, ARG and KEY are replaced
 * by `lookup_fn`, the chain's scrutinee (first argument of the chain's first
 * call) and the QUOTED "<stem>_chain<LINE>" key (matching what
 * code_orchain_emit_facts wrote). NULL/empty keeps the gen262 default
 * `FN(ARG, KEY)`. Non-destructive: the result goes to `out_path` (sandbox rules
 * as code_replace_expr, original untouched). The patched text is syntax-checked
 * through a .c temp (cc ignores foreign suffixes); verdict in `*compiles`:
 * 1 = compiles, 0 = the patch broke it, -1 = the standalone judge does not
 * apply because the ORIGINAL does not compile alone either (the file is a
 * fragment of a larger translation unit — its own build must judge).
 * gen274 (per-chain applicability): a template can IMPORT context identifiers
 * beyond its placeholders (the `b` of "kb_cue_match(b, KEY, ARG)"); a chain
 * whose enclosing function does not mention every imported identifier is kept
 * verbatim instead of patched into code that cannot compile — `*skipped`
 * counts those sites and `skip_ident` names the missing identifier. Heuristic
 * scope check; the codebase's own build/test suite stays the final judge.
 * Returns chains FOUND (patched = found - skipped), 0 if none (no file
 * written), -1 on error. */
int code_orchain_patch(const char *src_path, const char *fnname,
                       const char *lookup_fn, const char *call_tpl,
                       const char *out_path,
                       int *compiles, char *err_out, size_t err_sz,
                       int *skipped, char *skip_ident, size_t skip_sz);

/* gen263 (Track 5.3): the behavior-preserving judge of the derived plan. Runs
 * the ORIGINAL and the PATCHED version of a file through the same generated
 * probe harness — one probe per vocabulary word the chains encode (derived from
 * the original) plus a guaranteed miss — and compares stdout BYTES and exit
 * codes. The original is the oracle: identical output = the refactor preserved
 * behavior on every word that used to be code. Returns 1 identical, 0 divergent,
 * -1 if either version could not be built/run (diagnostics in err_out). */
int code_orchain_verify(const char *orig_path, const char *patched_path,
                        const char *chain_fn, const char *probe_fn,
                        int *nprobes, char *err_out, size_t err_sz);

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

/* gen322: the same compiler, asked about a SNIPPET held in memory rather than a
 * file on disk. It exists to REFUTE a claim, never to make one.
 *
 * parrot0's syntax "checks" were hand-rolled scanners whose findings were
 * asserted as truth with no oracle, and they fabricated: every correct one-line
 * C function was reported as "Missing semicolon at the end of a statement."
 * A compiler that can settle the question was already in the tree. If cc accepts
 * the translation unit under -fsyntax-only, then a syntax finding is FALSE by
 * construction — and a false finding is worse than a wall (PRINCIPLES.md).
 *
 * Three-way on purpose: 1 = it compiles, 0 = the compiler rejects it, -1 = the
 * oracle could not be run OR the snippet is not a standalone translation unit.
 * -1 is NOT "fine" and NOT "broken"; the caller must not read it as either. */
int code_syntax_ok(const char *src);

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

/* gen263: code_run with the program's stdout captured into `out` (stderr
 * discarded). A sibling, not a replacement: check_sort's exit-code-only contract
 * stays untouched; the differential judge needs the output bytes. gen266 adds
 * optional `stdin_data` (NULL = inherit): scripted input for judging programs
 * that READ, like a terminal game. */
int code_run_capture(const char *src_path, const char *stdin_data,
                     char *out, size_t out_sz,
                     int *exit_code, char *err_out, size_t err_sz);

/* gen266 (the first RULESCORE synthesis): instantiate the count_to_threshold
 * GAME schema — read whitespace tokens from stdin; each time the designated
 * `token` is typed a counter increments; when it reaches `threshold` print
 * `winword` and stop. The three parameters come from the RULES TEXT; the body
 * lives nowhere as a finished game (one schema, open parameter space). gen269:
 * emitted as pretty multi-line C (markdown-fenced in replies). Returns 1, or 0
 * on invalid parameters. */
int code_synth_game_counter(const char *token, int threshold, const char *winword,
                            char *out, size_t out_sz);

/* gen266: the run-grounded judge for count_to_threshold candidates. Compiles
 * `src` and plays it TWICE with scripted stdin built independently from the
 * parameters: threshold hits (mixed with noise tokens) must print `winword`;
 * threshold-1 hits must not (so unconditional printing cannot pass). Returns
 * 1 pass, 0 fail, -1 if it could not be built/run (diagnostics in err_out). */
int code_check_counter_game(const char *src, const char *token, int threshold,
                            const char *winword, char *err_out, size_t err_sz);

/* gen268 (universal-comprehension §8): the print_message PROGRAM schema — a
 * whole terminal program that prints `message` and exits 0. gen269: emitted as
 * pretty multi-line C (replies carry it inside a markdown fence). Message
 * restricted to a safe literal alphabet (no quote/backslash escaping games).
 * Returns 1, 0 on invalid message. */
int code_synth_print_program(const char *message, char *out, size_t out_sz);

/* gen268: the judge — compile and RUN the candidate; stdout must be exactly
 * `message` + newline and the exit status 0. 1 pass, 0 fail, -1 build/run. */
int code_check_print_program(const char *src, const char *message,
                             char *err_out, size_t err_sz);

/* gen270 (universal-comprehension §8, the simplest produce-artifact): create
 * an EMPTY file in the working directory. Guarded: plain basename only (no
 * paths, no dotfiles, safe charset, <=64 chars) and NEVER overwrites —
 * O_EXCL. Returns 1 created, 0 a file with that name already exists, -1
 * invalid name or OS error. */
int code_create_empty_file(const char *name);

/* gen209 (docs/plans/learn-and-build.md Track B/B0): the run-grounded JUDGE a sort
 * needs. Wraps a candidate function `func_src` — a self-contained C definition named
 * `fnname` with signature `void fnname(int a[], int n)` — in a generated main() that
 * runs it on several fixed vectors (sorted, reverse, duplicates, negatives, single,
 * empty) and, for each, asserts the result is non-decreasing AND a permutation of the
 * input, then compiles+executes via code_run and reads the exit status. The judge owns
 * the vectors and the oracle, so a candidate cannot "pass" by printing — it must really
 * sort every case. This is the "did the test pass?" rung for ARRAY code; it only
 * DISPOSES (no synthesis). Returns:
 *   1  -> built, ran, and every case passed (sorted + permutation)
 *   0  -> built and ran but at least one case failed (or it crashed / timed out)
 *  -1  -> could not build or run; on a build failure the diagnostics are in `err_out`.
 * `func_src` is rejected (-1) if it is empty/oversized or `fnname` is not a plain C
 * identifier. No temp artifact survives the call. */
int code_check_sort(const char *func_src, const char *fnname,
                    char *err_out, size_t err_sz);

/* gen209 (Track B/B2): the SYNTHESIZER. Instantiate a GENERAL algorithm schema named
 * `shape` into a concrete C function `void name(int a[], int n)`, parameterised by the
 * `comparator` ('>' yields ascending order, '<' descending), and write the source into
 * `out`. The schema is the unit of reuse — it is NOT the algorithm printed back: the
 * same `nested_loop_compare_swap` shape produces ascending and descending variants from
 * the SAME emitter, and the body is composed here from the schema, never stored as a C
 * literal in the brain. Returns 1 on success, 0 if the shape is unknown, `name` is not
 * a plain C identifier, the comparator is not one of '<'/'>', or it would not fit. The
 * artifact is a PROPOSAL — it must still be disposed by code_check_sort. */
int code_synth_from_shape(const char *shape, const char *name, char comparator,
                          char *out, size_t out_sz);

#endif /* PARROT0_CODE_H */
