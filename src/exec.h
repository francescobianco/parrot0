/*
 * exec.h - the tool-execution kernel (gen329, TODO.md P0/04-05-06-07).
 *
 * Everything parrot0 does to the machine goes through here, and comes back as an
 * Observation — never as a sentence. Four counterexamples, all reproduced on the
 * real binary at gen328, forced this file into existence:
 *
 *   04  `run git status; printf P0_CHAINED`  -> the second command RAN. The
 *       command was a STRING handed to popen(3), and a string is a program the
 *       shell is free to reinterpret. There is no whitelist of prefixes that can
 *       survive that: the fix is to never build a shell line at all. p0_exec
 *       takes argv[] and execve's it. A ';' is now just a character in argv[1].
 *
 *   05  `read /etc/os-release` -> parrot0 read a file outside the repository.
 *       safe_pathish() only checked the CHARACTERS of a path, which says nothing
 *       about where it points. Containment is not a character class; it is a
 *       resolution discipline. p0_open_in_root walks the path one component at a
 *       time with openat(O_NOFOLLOW) from a dirfd on the workspace root, so an
 *       absolute path, a '..', a symlink chain, and a rename racing the check are
 *       all refused BY CONSTRUCTION — and the fd we return is the very object we
 *       validated, not a name we hope still means the same file.
 *
 *   06  Untrusted code was compiled and run with no bound of any kind. A child is
 *       now its own process group with CPU/AS/FSIZE/NOFILE/NPROC limits, a wall
 *       clock, a scrubbed environment and a killpg on timeout, so a fork bomb, an
 *       infinite loop or a runaway allocation terminates as a TYPED verdict
 *       instead of taking the box with it.
 *
 *   07  `run make bogus-target` printed make's error as if it were a result. The
 *       exit status was never even read. An Observation carries argv, cwd, exit
 *       code, terminating signal, stdout and stderr SEPARATELY, truncation flags,
 *       duration and a digest — so "it failed" is a fact of the record, and a
 *       failure cannot be rendered as a success.
 *
 * The module has no Brain dependency on purpose: it is the boundary between
 * parrot0 and the operating system, and it must be testable as such.
 */
#ifndef PARROT0_EXEC_H
#define PARROT0_EXEC_H

#include <stddef.h>

#define P0_OBS_OUT   16384   /* captured stdout, bytes                          */
#define P0_OBS_ERR    4096   /* captured stderr, bytes (kept SEPARATE from out) */
#define P0_OBS_CMD     768   /* rendered argv, for display and proof            */

/* The verdict of an act. Every one of these is a DIFFERENT thing to say to the
 * user, which is precisely why they were collapsing into "here is the output"
 * before this file existed. P0_OK means the tool ran AND exited 0 — nothing
 * else may be reported as a success. */
typedef enum {
    P0_OK = 0,          /* ran, exit 0                                          */
    P0_EXIT_NONZERO,    /* ran, exit != 0 — a real, citable failure             */
    P0_SIGNALED,        /* killed by a signal (segv, abrt, …)                   */
    P0_TIMEOUT,         /* exceeded its wall clock; the process group was killed*/
    P0_SPAWN_FAILED,    /* never started (no such tool, fork/exec failed)       */
    P0_UNSAFE_PATH,     /* the path escapes the workspace root                  */
    P0_UNSAFE_COMMAND,  /* argv is not backed by a tool contract in the KB      */
    P0_TOO_LARGE        /* refused before running: input over budget            */
} P0Verdict;

/* The record of one act against the machine. */
typedef struct {
    P0Verdict verdict;
    char      cmd[P0_OBS_CMD];      /* argv rendered for humans (display/proof) */
    char      cwd[512];             /* where it ran, relative to the root       */
    int       exit_code;            /* meaningful iff verdict is OK/EXIT_NONZERO*/
    int       term_signal;          /* meaningful iff verdict is SIGNALED       */
    char      out[P0_OBS_OUT];      /* stdout, verbatim bytes                   */
    size_t    out_len;
    int       out_truncated;
    char      err[P0_OBS_ERR];      /* stderr, verbatim bytes                   */
    size_t    err_len;
    int       err_truncated;
    long      duration_ms;
    int       net_isolated;         /* 1 iff the child really had no network.   */
                                    /* Best effort: we do NOT claim it when the */
                                    /* kernel refused us the namespace.         */
    char      digest[17];           /* FNV-1a of stdout — a stable artifact id  */
    char      detail[160];          /* why it did not run (errno, denied arg…)  */
} P0Obs;

/* Human/proof-facing name of a verdict ("ok", "exit_nonzero", "unsafe_path", …). */
const char *p0_verdict_name(P0Verdict v);

/* The ONE predicate the rest of the brain may use to decide whether an act
 * succeeded. It is a function and not a field so that no caller can "forget" to
 * check the exit code the way run_shell() did for 328 generations. */
int p0_obs_ok(const P0Obs *o);

/* ---- workspace containment (04/05) ------------------------------------------ */

/* Capture the workspace root (the cwd at startup). Idempotent; called lazily. */
void        p0_root_init(void);
const char *p0_root(void);

/* Validate `path` as a location INSIDE the workspace root and open it.
 * Returns P0_OK and stores a read-only fd in *fd_out (caller closes), or
 * P0_UNSAFE_PATH. `want_dir` selects a directory instead of a regular file.
 * Refuses: absolute paths, any '..' component, symlinks anywhere on the path,
 * and non-regular targets (device, FIFO, socket) — a FIFO would hang the reader
 * and a device is not the workspace's to hand out. */
P0Verdict p0_open_in_root(const char *path, int want_dir, int *fd_out);

/* The same discipline, without opening: is `path` a safe in-root location?
 * Writes the cleaned relative form (never absolute, never '..') to `rel_out`.
 * Used to build argv for tools that take a path (grep/find), so the argument we
 * pass is one we have actually resolved. */
P0Verdict p0_safe_rel(const char *path, int want_dir, char *rel_out, size_t cap);

/* Read a contained file's bytes directly — no `head`, no subprocess, no shell.
 * Truncates at cap-1 and NUL-terminates; sets *truncated when there was more. */
P0Verdict p0_read_in_root(const char *path, char *buf, size_t cap, int *truncated);

/* ---- execution (04/06/07) ---------------------------------------------------- */

/* Run argv[] (NULL-terminated) with cwd `cwd` (relative to the root, NULL = root),
 * a wall-clock budget of `timeout_ms`, and feed stdin from `stdin_data` (may be
 * NULL). NEVER goes through a shell. Always fills *obs — including when the tool
 * could not start. Returns obs->verdict for convenience.
 *
 * The child gets: its own process group (so a timeout kills the whole tree), a
 * scrubbed environment (PATH/HOME/LANG=C only — LANG=C also stops a localized
 * `make` from making its own error message unparseable), rlimits on CPU, address
 * space, file size, open files and processes, and — when the kernel allows an
 * unprivileged network namespace — no network at all. */
P0Verdict p0_exec(char *const argv[], const char *cwd, int timeout_ms,
                  const char *stdin_data, P0Obs *obs);

/* Append the Observation to the JSONL trace at $PARROT0_TRACE, if that is set.
 * The trace is the replayable record the capability report and the future typed
 * kernel (TODO 12) read; it is written for every act, success or not. */
void p0_obs_trace(const P0Obs *o);

/* Render the Observation as the one line a proof may cite: verdict, exit status,
 * duration and stdout digest. This is what "how do you know?" quotes, so it must
 * describe the RUN, never the answer. */
void p0_obs_proof(const P0Obs *o, char *buf, size_t cap);

#endif /* PARROT0_EXEC_H */