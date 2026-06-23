/* gen173: parrot0's in-C structural code reader. See code.h for the contract and
 * the principle (AST-as-KB, deterministic parse, straight into RAM). This is the
 * F2 (grammar/structure) faculty of docs/CODE-MASTERY.md, the one place where the
 * formal, decidable grammar of code lets primitives-first dominate outright. */
#include "code.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

/* A C control-flow / type keyword can sit right before '(' (e.g. "if (", "for (",
 * "sizeof (") and look like a definition head; it is not a function name, so it
 * is excluded. The function's own name is never a keyword. */
static int is_c_keyword(const char *w) {
    static const char *const kw[] = {
        "if", "else", "for", "while", "do", "switch", "case", "return",
        "sizeof", "struct", "union", "enum", "typedef", "static", "const",
        "unsigned", "signed", "void", "int", "char", "short", "long", "float",
        "double", "goto", "break", "continue", "default", "extern", "register",
        "volatile", "inline", NULL,
    };
    for (const char *const *k = kw; *k; k++)
        if (strcmp(w, *k) == 0) return 1;
    return 0;
}

size_t code_ingest(KB *kb, const char *src,
                   char names[][KB_TERM_LEN], size_t max) {
    if (!kb || !src) return 0;
    size_t found = 0;

    /* gen174: one structural pass that tracks the enclosing function by brace
     * depth, so an identifier-call inside a body is attributed to its caller
     * (code_calls/2) — the F3 call-graph faculty, derived from structure. */
    char cur_fn[KB_TERM_LEN] = "";
    int brace = 0;
    int fn_brace = -1;                  /* body depth of cur_fn; -1 = outside */

    const char *p = src;
    while (*p) {
        if (*p == '{') { brace++; p++; continue; }
        if (*p == '}') {
            brace--;
            if (fn_brace >= 0 && brace < fn_brace) { cur_fn[0] = '\0'; fn_brace = -1; }
            p++; continue;
        }

        /* Find the start of an identifier. */
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);

        /* An identifier followed by '(' is a definition head or a call. */
        const char *q = p;
        while (*q && isspace((unsigned char)*q)) q++;
        if (*q != '(') continue;                 /* p already advanced; no loop */

        /* Balance the parens (lookahead only — p does not move past them, so a
         * nested call in the argument list is still seen next iterations). */
        int depth = 0;
        const char *r = q;
        for (; *r; r++) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
        }
        if (depth != 0) break;                    /* unbalanced — stop scanning */

        const char *after = r;
        while (*after && isspace((unsigned char)*after)) after++;

        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) continue;
        memcpy(name, id, idlen);
        name[idlen] = '\0';
        if (is_c_keyword(name)) continue;         /* if/for/while/sizeof... */

        if (*after == '{') {
            /* Function definition. Assert into RAM immediately — code joins the
             * same KB as the world. KB_BASE provenance so the session writer
             * does not re-save derived structure (re-derivable from source). */
            kb_set_origin(kb, KB_BASE);
            const char *args[] = { name };
            kb_assert(kb, "code_function", args, 1);
            kb_set_origin(kb, KB_SESSION);

            /* Redefinition replaces: clear this function's prior call edges so a
             * re-ingest of the same name in one session does not merge stale
             * callees from an earlier, different body. */
            char old[32][KB_TERM_LEN];
            const char *qp[] = { name, NULL };
            size_t no = kb_match(kb, "code_calls", qp, 2, old, 32);
            for (size_t i = 0; i < no; i++) {
                const char *ra[] = { name, old[i] };
                kb_retract(kb, "code_calls", ra, 2);
            }
            if (names && found < max)
                snprintf(names[found], KB_TERM_LEN, "%s", name);
            found++;
            /* Enter the body: let the '{' be counted on the next iteration. */
            snprintf(cur_fn, sizeof cur_fn, "%s", name);
            fn_brace = brace + 1;
            p = after;
        } else if (cur_fn[0]) {
            /* A call inside a function body — attribute it to the caller. */
            kb_set_origin(kb, KB_BASE);
            const char *args[] = { cur_fn, name };
            kb_assert(kb, "code_calls", args, 2);
            kb_set_origin(kb, KB_SESSION);
            /* p stays just past the ident, so nested calls are still seen. */
        }
    }
    return found;
}

/* gen196: is `w` a Python keyword (so `name(` after it is not a call)? */
static int is_py_keyword(const char *w) {
    static const char *const kw[] = {
        "def","class","if","elif","else","for","while","return","import","from",
        "with","try","except","finally","lambda","and","or","not","in","is",
        "assert","yield","raise","global","nonlocal","pass","break","continue",
        "del","as","async","await","print","range","len","True","False","None",
        NULL };
    for (size_t i = 0; kw[i]; i++) if (strcmp(w, kw[i]) == 0) return 1;
    return 0;
}

/* gen196: scan one Python statement line for calls `name(...)` and attribute each
 * to `caller`, mirroring code_ingest's call-edge assertion (KB_BASE provenance). */
static void py_calls_in_line(KB *kb, const char *caller, const char *line) {
    const char *p = line;
    while (*p) {
        if (*p == '#') break;                                  /* comment */
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t l = (size_t)(p - id);
        const char *q = p; while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;
        char name[KB_TERM_LEN];
        if (l == 0 || l >= sizeof name) continue;
        memcpy(name, id, l); name[l] = '\0';
        if (is_py_keyword(name)) continue;
        kb_set_origin(kb, KB_BASE);
        const char *args[] = { caller, name };
        kb_assert(kb, "code_calls", args, 2);
        kb_set_origin(kb, KB_SESSION);
    }
}

size_t code_ingest_py(KB *kb, const char *src,
                      char names[][KB_TERM_LEN], size_t max) {
    if (!kb || !src) return 0;
    size_t found = 0;

    /* A stack of enclosing `def`s by indentation: a line at indent I closes every
     * def whose body-indent is >= I (the Python delta to C's brace depth). */
    struct { int indent; char name[KB_TERM_LEN]; } stack[64];
    int sp = 0;

    const char *p = src;
    while (*p) {
        /* one physical line: [line, eol) */
        const char *line = p;
        const char *eol = line;
        while (*eol && *eol != '\n') eol++;

        /* indent = leading whitespace columns; skip blank / comment-only lines. */
        const char *c = line;
        int indent = 0;
        while (c < eol && (*c == ' ' || *c == '\t')) { indent++; c++; }
        if (c == eol || *c == '#') { p = (*eol ? eol + 1 : eol); continue; }

        /* close scopes this line has dedented out of. */
        while (sp > 0 && stack[sp - 1].indent >= indent) sp--;

        int is_def = 0;
        const char *d = c;
        if (strncmp(d, "async ", 6) == 0) d += 6;
        if (strncmp(d, "def ", 4) == 0) { is_def = 1; d += 4; }

        if (is_def) {
            while (d < eol && (*d == ' ' || *d == '\t')) d++;
            const char *id = d;
            while (d < eol && (isalnum((unsigned char)*d) || *d == '_')) d++;
            size_t l = (size_t)(d - id);
            if (l > 0 && l < KB_TERM_LEN) {
                char name[KB_TERM_LEN];
                memcpy(name, id, l); name[l] = '\0';

                kb_set_origin(kb, KB_BASE);
                const char *args[] = { name };
                kb_assert(kb, "code_function", args, 1);
                kb_set_origin(kb, KB_SESSION);

                /* redefinition replaces: clear prior call edges (as code_ingest). */
                char old[32][KB_TERM_LEN];
                const char *qp[] = { name, NULL };
                size_t no = kb_match(kb, "code_calls", qp, 2, old, 32);
                for (size_t i = 0; i < no; i++) {
                    const char *ra[] = { name, old[i] };
                    kb_retract(kb, "code_calls", ra, 2);
                }
                if (names && found < max)
                    snprintf(names[found], KB_TERM_LEN, "%s", name);
                found++;

                if (sp < 64) { stack[sp].indent = indent;
                               snprintf(stack[sp].name, KB_TERM_LEN, "%s", name); sp++; }
            }
        } else if (sp > 0) {
            /* a statement inside the nearest enclosing def — attribute its calls.
             * (operate on a NUL-bounded copy of the line for the scanner). */
            char buf[1024];
            size_t ll = (size_t)(eol - c);
            if (ll < sizeof buf) {
                memcpy(buf, c, ll); buf[ll] = '\0';
                py_calls_in_line(kb, stack[sp - 1].name, buf);
            }
        }

        p = (*eol ? eol + 1 : eol);
    }
    return found;
}

/* --- gen175/176: symbolic execution of a function body ------------------ */

/* A recursive-descent evaluator over the function's parameters (bound to the
 * argument values) plus a tiny statement interpreter (gen176) for if/else and
 * return, so a branch is followed to the return path the condition selects.
 * Grammar:
 *   rel    := add  (RELOP add)*           RELOP = < > <= >= == !=  (yields 0/1)
 *   add    := term (('+' | '-') term)*
 *   term   := factor (('*' | '/' | '%') factor)*
 *   factor := number | ident | '(' rel ')' | ('+' | '-') factor
 *   stmt   := '{' stmt* '}' | "if" '(' rel ')' stmt ["else" stmt]
 *           | "return" rel ';' | <unsupported>
 * `err` latches on anything unsupported (unknown ident, call, div-by-zero, a
 * non-if/return statement), so the caller refuses honestly rather than guess.
 * `ret` latches when a return fires, carrying its value in `retval`. */
typedef struct {
    const char *c, *end;
    char (*params)[KB_TERM_LEN];
    const long *vals;
    size_t np, nv;
    /* gen177: a writable environment of integer locals (declarations and
     * assignments), looked up before the read-only parameters. */
    char locals[16][KB_TERM_LEN];
    long locvals[16];
    size_t nl;
    /* gen180: the whole source + recursion depth, so a call in an expression can
     * re-enter the evaluator on the named function. */
    const char *src;
    int depth;
    int err;
    int ret;
    long retval;
} EvalCtx;

/* gen180: evaluate the function `want` in `src` on `argv`; forward-declared so a
 * call inside an expression (ev_factor) can recurse. */
static int eval_fn(const char *src, const char *want,
                   const long *argv, size_t argc, long *out, int depth);

/* gen199 (language-as-delta §7b): evaluate `want` in EITHER language — try the C
 * front-end, then the Python one. Forward-declared so a call inside an expression
 * recurses through the SAME dispatch, regardless of the source language. */
static int eval_any(const char *src, const char *want,
                    const long *argv, size_t argc, long *out, int depth);

/* gen177: local-variable environment helpers. */
static long *ev_local_slot(EvalCtx *e, const char *name) {
    for (size_t i = 0; i < e->nl; i++)
        if (strcmp(e->locals[i], name) == 0) return &e->locvals[i];
    return NULL;
}
static void ev_set_local(EvalCtx *e, const char *name, long v) {
    long *s = ev_local_slot(e, name);
    if (s) { *s = v; return; }
    if (e->nl < 16) {
        snprintf(e->locals[e->nl], KB_TERM_LEN, "%s", name);
        e->locvals[e->nl] = v; e->nl++;
    } else e->err = 1;
}
static int ev_is_type_kw(const char *w) {
    static const char *const t[] = {
        "int", "long", "short", "char", "unsigned", "signed", "const", NULL,
    };
    for (const char *const *k = t; *k; k++) if (strcmp(w, *k) == 0) return 1;
    return 0;
}

static void ev_ws(EvalCtx *e) {
    /* gen181: skip all whitespace (incl. newlines) so the evaluator works on code
     * read from a real multi-line file, not just one-line snippets. */
    while (e->c < e->end && isspace((unsigned char)*e->c)) e->c++;
}

static long ev_rel(EvalCtx *e);

static long ev_factor(EvalCtx *e) {
    ev_ws(e);
    if (e->c >= e->end) { e->err = 1; return 0; }
    if (*e->c == '(') {
        e->c++;
        long v = ev_rel(e);
        ev_ws(e);
        if (e->c < e->end && *e->c == ')') e->c++;
        else e->err = 1;
        return v;
    }
    if (*e->c == '-') { e->c++; return -ev_factor(e); }
    if (*e->c == '+') { e->c++; return  ev_factor(e); }
    if (isdigit((unsigned char)*e->c)) {
        long v = 0;
        while (e->c < e->end && isdigit((unsigned char)*e->c)) {
            v = v * 10 + (*e->c - '0');
            e->c++;
        }
        return v;
    }
    if (isalpha((unsigned char)*e->c) || *e->c == '_') {
        const char *s = e->c;
        while (e->c < e->end && (isalnum((unsigned char)*e->c) || *e->c == '_')) e->c++;
        size_t l = (size_t)(e->c - s);
        char w[KB_TERM_LEN];
        if (l >= sizeof w) { e->err = 1; return 0; }
        memcpy(w, s, l); w[l] = '\0';
        ev_ws(e);
        /* gen180: an identifier followed by '(' is a call — evaluate the argument
         * expressions in the current environment, then re-enter the evaluator on
         * the named function (recursion is fine, under a depth ceiling). */
        if (e->c < e->end && *e->c == '(') {
            e->c++;
            long args[8]; size_t na = 0;
            ev_ws(e);
            if (e->c < e->end && *e->c != ')') {
                for (;;) {
                    long a = ev_rel(e);
                    if (e->err) return 0;
                    if (na < 8) args[na++] = a; else { e->err = 1; return 0; }
                    ev_ws(e);
                    if (e->c < e->end && *e->c == ',') { e->c++; ev_ws(e); continue; }
                    break;
                }
            }
            if (e->c < e->end && *e->c == ')') e->c++; else { e->err = 1; return 0; }
            if (e->depth >= 1000) { e->err = 1; return 0; }   /* recursion ceiling */
            long res;
            if (!eval_any(e->src, w, args, na, &res, e->depth + 1)) { e->err = 1; return 0; }
            return res;
        }
        long *ls = ev_local_slot(e, w);          /* locals shadow params */
        if (ls) return *ls;
        for (size_t i = 0; i < e->np; i++)
            if (strcmp(w, e->params[i]) == 0) {
                if (i < e->nv) return e->vals[i];
                e->err = 1; return 0;            /* parameter with no argument */
            }
        e->err = 1; return 0;                    /* unknown identifier */
    }
    e->err = 1; return 0;
}

static long ev_term(EvalCtx *e) {
    long v = ev_factor(e);
    for (;;) {
        ev_ws(e);
        if (e->c >= e->end) break;
        char op = *e->c;
        if (op == '*' || op == '/' || op == '%') {
            e->c++;
            long r = ev_factor(e);
            if (op == '*') v *= r;
            else { if (r == 0) { e->err = 1; return 0; } v = (op == '/') ? v / r : v % r; }
        } else break;
    }
    return v;
}

static long ev_add(EvalCtx *e) {
    long v = ev_term(e);
    for (;;) {
        ev_ws(e);
        if (e->c >= e->end) break;
        char op = *e->c;
        if (op == '+' || op == '-') { e->c++; long r = ev_term(e); v += (op == '+') ? r : -r; }
        else break;
    }
    return v;
}

/* Relational / equality layer (lower precedence than +-), yielding 0/1 — the
 * boolean an `if` condition needs. */
static long ev_rel(EvalCtx *e) {
    long v = ev_add(e);
    for (;;) {
        ev_ws(e);
        if (e->c >= e->end) break;
        char a = *e->c;
        char b = (e->c + 1 < e->end) ? e->c[1] : '\0';
        int op;
        if      (a == '<' && b == '=') { op = 3; e->c += 2; }
        else if (a == '>' && b == '=') { op = 4; e->c += 2; }
        else if (a == '=' && b == '=') { op = 5; e->c += 2; }
        else if (a == '!' && b == '=') { op = 6; e->c += 2; }
        else if (a == '<')             { op = 1; e->c += 1; }
        else if (a == '>')             { op = 2; e->c += 1; }
        else break;
        long r = ev_add(e);
        switch (op) {
            case 1: v = (v <  r); break;  case 2: v = (v >  r); break;
            case 3: v = (v <= r); break;  case 4: v = (v >= r); break;
            case 5: v = (v == r); break;  case 6: v = (v != r); break;
        }
    }
    return v;
}

/* gen176: a tiny statement interpreter for the conditional-return family
 * (max/min/abs/sign/clamp). It runs if/else and return; any other statement
 * (assignment, declaration, loop, bare call) is refused, since skipping it could
 * silently change the result. */
static int ev_kw(EvalCtx *e, const char *w) {
    size_t l = strlen(w);
    if (e->c + l > e->end || strncmp(e->c, w, l) != 0) return 0;
    char after = (e->c + l < e->end) ? e->c[l] : '\0';
    return !(isalnum((unsigned char)after) || after == '_');
}

static void ev_skip_stmt(EvalCtx *e) {
    ev_ws(e);
    if (e->c >= e->end) return;
    if (*e->c == '{') {
        int d = 0;
        for (; e->c < e->end; e->c++) {
            if (*e->c == '{') d++;
            else if (*e->c == '}') { d--; if (d == 0) { e->c++; return; } }
        }
        return;
    }
    if (ev_kw(e, "if")) {
        e->c += 2; ev_ws(e);
        if (e->c < e->end && *e->c == '(') {
            int d = 0;
            for (; e->c < e->end; e->c++) {
                if (*e->c == '(') d++;
                else if (*e->c == ')') { d--; if (d == 0) { e->c++; break; } }
            }
        }
        ev_skip_stmt(e);                  /* then */
        ev_ws(e);
        if (ev_kw(e, "else")) { e->c += 4; ev_skip_stmt(e); }
        return;
    }
    while (e->c < e->end && *e->c != ';') e->c++;   /* simple statement */
    if (e->c < e->end) e->c++;
}

static void ev_run_seq(EvalCtx *e);

/* True if the token at the cursor is a type keyword (so a statement is a local
 * declaration), without consuming it. */
static int ev_peek_is_type(EvalCtx *e) {
    const char *s = e->c;
    if (!(isalpha((unsigned char)*s) || *s == '_')) return 0;
    const char *t = s;
    while (t < e->end && (isalnum((unsigned char)*t) || *t == '_')) t++;
    size_t l = (size_t)(t - s);
    if (l >= KB_TERM_LEN) return 0;
    char w[KB_TERM_LEN]; memcpy(w, s, l); w[l] = '\0';
    return ev_is_type_kw(w);
}

/* gen179: execute one simple update statement (no terminating ';' consumed):
 * an assignment `x = expr`, or prefix/postfix `++x`/`x++`/`--x`/`x--`. The name
 * must already be a local or parameter. Used by the for-post clause and, with a
 * trailing ';', by ordinary statements. Sets err on anything else. */
static void ev_simple(EvalCtx *e) {
    ev_ws(e);
    int pre = 0;
    if (e->c + 1 < e->end && e->c[0] == '+' && e->c[1] == '+') { pre = 1;  e->c += 2; ev_ws(e); }
    else if (e->c + 1 < e->end && e->c[0] == '-' && e->c[1] == '-') { pre = -1; e->c += 2; ev_ws(e); }
    if (!(isalpha((unsigned char)*e->c) || *e->c == '_')) { e->err = 1; return; }
    const char *s = e->c;
    while (e->c < e->end && (isalnum((unsigned char)*e->c) || *e->c == '_')) e->c++;
    size_t l = (size_t)(e->c - s);
    char w[KB_TERM_LEN]; if (l >= sizeof w) { e->err = 1; return; }
    memcpy(w, s, l); w[l] = '\0';

    long cur = 0; int known = 0;
    long *ls = ev_local_slot(e, w);
    if (ls) { known = 1; cur = *ls; }
    else for (size_t i = 0; i < e->np; i++)
        if (strcmp(w, e->params[i]) == 0) {
            if (i >= e->nv) { e->err = 1; return; }
            known = 1; cur = e->vals[i]; break;
        }

    if (pre) { if (!known) { e->err = 1; return; } ev_set_local(e, w, cur + pre); return; }
    ev_ws(e);
    if (e->c + 1 < e->end && e->c[0] == '+' && e->c[1] == '+') {
        e->c += 2; if (!known) { e->err = 1; return; } ev_set_local(e, w, cur + 1); return;
    }
    if (e->c + 1 < e->end && e->c[0] == '-' && e->c[1] == '-') {
        e->c += 2; if (!known) { e->err = 1; return; } ev_set_local(e, w, cur - 1); return;
    }
    if (e->c < e->end && *e->c == '=' && !(e->c + 1 < e->end && e->c[1] == '=')) {
        e->c++;
        long v = ev_rel(e);
        if (e->err) return;
        if (!known) { e->err = 1; return; }      /* assignment to unknown name */
        ev_set_local(e, w, v);
        return;
    }
    e->err = 1;
}

static void ev_run_stmt(EvalCtx *e) {
    ev_ws(e);
    if (e->c >= e->end || e->err || e->ret) return;
    if (*e->c == '{') {
        e->c++;
        ev_run_seq(e);
        if (e->c < e->end && *e->c == '}') e->c++;
        return;
    }
    if (ev_kw(e, "return")) {
        e->c += 6;
        long v = ev_rel(e);
        ev_ws(e);
        if (e->c < e->end && *e->c == ';') e->c++;
        if (!e->err) { e->ret = 1; e->retval = v; }
        return;
    }
    if (ev_kw(e, "if")) {
        e->c += 2; ev_ws(e);
        if (!(e->c < e->end && *e->c == '(')) { e->err = 1; return; }
        e->c++;
        long cond = ev_rel(e);
        ev_ws(e);
        if (!(e->c < e->end && *e->c == ')')) { e->err = 1; return; }
        e->c++;
        if (cond) {
            ev_run_stmt(e);                       /* then */
            ev_ws(e);
            if (ev_kw(e, "else")) { e->c += 4; ev_skip_stmt(e); }
        } else {
            ev_skip_stmt(e);                      /* skip then */
            ev_ws(e);
            if (ev_kw(e, "else")) { e->c += 4; ev_run_stmt(e); }
        }
        return;
    }

    /* gen178: a bounded while-loop. The condition and body text spans are
     * captured once, then re-run until the condition is false, a return fires, or
     * a step ceiling is hit (so a non-terminating loop is refused, never hangs). */
    if (ev_kw(e, "while")) {
        e->c += 5; ev_ws(e);
        if (!(e->c < e->end && *e->c == '(')) { e->err = 1; return; }
        const char *cond_start = e->c + 1;
        int d = 0; const char *cp = e->c; const char *cond_end = NULL;
        for (; cp < e->end; cp++) {
            if (*cp == '(') d++;
            else if (*cp == ')') { d--; if (d == 0) { cond_end = cp; break; } }
        }
        if (!cond_end) { e->err = 1; return; }
        const char *body_start = cond_end + 1;
        e->c = body_start;
        ev_skip_stmt(e);                      /* locate the end of the body once */
        const char *body_end = e->c;

        long guard = 0;
        for (;;) {
            if (e->err) return;
            if (e->ret) { e->c = body_end; return; }
            e->c = cond_start;
            long cond = ev_rel(e);
            if (e->err) return;
            if (!cond) break;
            e->c = body_start;
            ev_run_stmt(e);
            if (++guard > 1000000L) { e->err = 1; return; }  /* termination ceiling */
        }
        e->c = body_end;
        return;
    }

    /* gen179: a three-clause for-loop, desugared to init + while(cond){body;post}.
     * Empty init/cond/post are allowed (empty cond means true). Same step ceiling
     * as while, for honest termination. */
    if (ev_kw(e, "for")) {
        e->c += 3; ev_ws(e);
        if (!(e->c < e->end && *e->c == '(')) { e->err = 1; return; }
        int d = 0; const char *hp = e->c; const char *rparen = NULL;
        for (; hp < e->end; hp++) {
            if (*hp == '(') d++;
            else if (*hp == ')') { d--; if (d == 0) { rparen = hp; break; } }
        }
        if (!rparen) { e->err = 1; return; }
        const char *init_start = e->c + 1;
        const char *semic1 = NULL, *semic2 = NULL;
        d = 0;
        for (const char *q = e->c; q < rparen; q++) {
            if (*q == '(') d++;
            else if (*q == ')') d--;
            else if (*q == ';' && d == 1) { if (!semic1) semic1 = q; else if (!semic2) semic2 = q; }
        }
        if (!semic1 || !semic2) { e->err = 1; return; }
        const char *cond_start = semic1 + 1, *post_start = semic2 + 1;
        const char *body_start = rparen + 1;
        e->c = body_start; ev_skip_stmt(e);
        const char *body_end = e->c;

        /* init (once), if non-empty — it carries its own ';' at semic1 */
        { const char *ii = init_start; while (ii < semic1 && (*ii == ' ' || *ii == '\t')) ii++;
          if (ii < semic1) { e->c = init_start; ev_run_stmt(e); if (e->err) return; } }

        long guard = 0;
        for (;;) {
            if (e->err) return;
            if (e->ret) { e->c = body_end; return; }
            long cond = 1;
            { const char *cc = cond_start; while (cc < semic2 && (*cc == ' ' || *cc == '\t')) cc++;
              if (cc < semic2) { e->c = cond_start; cond = ev_rel(e); if (e->err) return; } }
            if (!cond) break;
            e->c = body_start; ev_run_stmt(e);
            if (e->err) return;
            if (e->ret) { e->c = body_end; return; }
            { const char *pp = post_start; while (pp < rparen && (*pp == ' ' || *pp == '\t')) pp++;
              if (pp < rparen) {
                  const char *saved = e->end; e->c = post_start; e->end = rparen;
                  ev_simple(e); e->end = saved; if (e->err) return;
              } }
            if (++guard > 1000000L) { e->err = 1; return; }
        }
        e->c = body_end;
        return;
    }

    /* gen177/179: a local declaration, or a simple update statement (assignment,
     * ++/--), both terminated by ';'. */
    if (ev_peek_is_type(e)) {
        /* declaration: <type...> name [= expr] ; */
        char name[KB_TERM_LEN] = "";
        for (;;) {
            ev_ws(e);
            if (!(isalpha((unsigned char)*e->c) || *e->c == '_')) { e->err = 1; return; }
            const char *t = e->c;
            while (e->c < e->end && (isalnum((unsigned char)*e->c) || *e->c == '_')) e->c++;
            size_t tl = (size_t)(e->c - t);
            if (tl >= sizeof name) { e->err = 1; return; }
            char tw[KB_TERM_LEN]; memcpy(tw, t, tl); tw[tl] = '\0';
            if (ev_is_type_kw(tw)) continue;              /* still part of the type */
            snprintf(name, sizeof name, "%s", tw);        /* this is the var name */
            break;
        }
        ev_ws(e);
        long val = 0;
        if (e->c < e->end && *e->c == '=' &&
            !(e->c + 1 < e->end && e->c[1] == '=')) { e->c++; val = ev_rel(e); }
        ev_ws(e);
        if (e->c < e->end && *e->c == ',') { e->err = 1; return; }     /* multi-declarator */
        if (e->c < e->end && *e->c == ';') e->c++; else { e->err = 1; return; }
        if (!e->err) ev_set_local(e, name, val);
        return;
    }

    /* assignment / ++ / -- as a full statement */
    if (isalpha((unsigned char)*e->c) || *e->c == '_' ||
        (e->c + 1 < e->end && (e->c[0] == '+' || e->c[0] == '-') && e->c[0] == e->c[1])) {
        ev_simple(e);
        if (e->err) return;
        ev_ws(e);
        if (e->c < e->end && *e->c == ';') e->c++; else e->err = 1;
        return;
    }

    e->err = 1;                                   /* unsupported statement */
}

static void ev_run_seq(EvalCtx *e) {
    for (;;) {
        ev_ws(e);
        if (e->c >= e->end || e->err || e->ret) return;
        if (*e->c == '}') return;
        if (*e->c == ';') { e->c++; continue; }
        ev_run_stmt(e);
    }
}

/* Extract the last identifier in [seg, end) — the parameter NAME in a declarator
 * like "int a", "const char *s". Returns 1 + the name (without "void"), 0 if
 * none. */
static int last_ident(const char *seg, const char *end, char *out, size_t out_sz) {
    const char *lastid = NULL; size_t lastlen = 0;
    const char *t = seg;
    while (t < end) {
        if (isalpha((unsigned char)*t) || *t == '_') {
            const char *s = t;
            while (t < end && (isalnum((unsigned char)*t) || *t == '_')) t++;
            lastid = s; lastlen = (size_t)(t - s);
        } else t++;
    }
    if (!lastid || lastlen == 0 || lastlen >= out_sz) return 0;
    memcpy(out, lastid, lastlen); out[lastlen] = '\0';
    return strcmp(out, "void") != 0;
}

static int eval_fn(const char *src, const char *want,
                   const long *argv, size_t argc, long *out, int depth) {
    if (!src || !out) return 0;

    /* Locate the target function definition: its parameter list and its body. */
    const char *params_beg = NULL, *params_end = NULL, *body = NULL;
    const char *p = src;
    while (*p) {
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);

        const char *q = p;
        while (*q && isspace((unsigned char)*q)) q++;
        if (*q != '(') continue;

        int pd = 0; const char *r = q;
        for (; *r; r++) {
            if (*r == '(') pd++;
            else if (*r == ')') { pd--; if (pd == 0) { r++; break; } }
        }
        if (pd != 0) return 0;
        const char *after = r;
        while (*after && isspace((unsigned char)*after)) after++;
        if (*after != '{') continue;

        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) { p = after; continue; }
        memcpy(name, id, idlen); name[idlen] = '\0';
        if (is_c_keyword(name)) { p = after; continue; }

        if (!want || strcmp(name, want) == 0) {
            params_beg = q + 1; params_end = r - 1;   /* between '(' and ')' */
            body = after;                             /* at '{' */
            break;
        }
        p = after;
    }
    if (!body) return 0;

    /* Parameter names, in order. */
    char params[8][KB_TERM_LEN]; size_t np = 0;
    {
        const char *s = params_beg;
        while (s < params_end && np < 8) {
            const char *seg = s;
            while (s < params_end && *s != ',') s++;
            char nm[KB_TERM_LEN];
            if (last_ident(seg, s, nm, sizeof nm)) {
                snprintf(params[np], KB_TERM_LEN, "%s", nm); np++;
            }
            if (s < params_end && *s == ',') s++;
        }
    }

    /* Run the body as statements (gen176): if/else are followed, the first
     * return that fires on the taken path wins. `end` is the whole source — the
     * sequence stops at the function's own closing '}'. */
    EvalCtx e;
    e.c = body + 1;                       /* past the opening '{' */
    e.end = src + strlen(src);
    e.params = params; e.vals = argv;
    e.np = np; e.nv = argc; e.nl = 0;
    e.src = src; e.depth = depth;
    e.err = 0; e.ret = 0; e.retval = 0;
    ev_run_seq(&e);
    if (e.err || !e.ret) return 0;        /* unsupported, or no return reached */
    *out = e.retval;
    return 1;
}

/* gen199 (language-as-delta §7b): the PYTHON front-end for F3 semantics. The
 * shared expression evaluator (ev_rel and friends) is reused UNCHANGED — only the
 * concrete syntax is the delta from C: a `def name(params):` head, identifier-FIRST
 * params (`a`, `a: int`, `a=1` — no leading C type), and newline-terminated
 * statements instead of braces and ';'. The body is a run of simple `name = expr`
 * assignments followed by `return expr`. Anything else latches `err`, so the
 * caller refuses honestly rather than guess (same discipline as eval_fn). This is
 * the same semantics, reached through a mirror surface — not a second evaluator. */
static int eval_py_fn(const char *src, const char *want,
                      const long *argv, size_t argc, long *out, int depth) {
    if (!src || !out) return 0;

    /* Locate `def want(`: a "def" token at a word boundary, a name, then '('. */
    const char *params_beg = NULL, *params_end = NULL, *colon = NULL;
    for (const char *p = strstr(src, "def "); p; p = strstr(p + 1, "def ")) {
        if (p != src && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        const char *d = p + 4; while (*d == ' ' || *d == '\t') d++;
        const char *id = d; while (isalnum((unsigned char)*d) || *d == '_') d++;
        size_t idl = (size_t)(d - id);
        const char *q = d; while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;
        int pd = 0; const char *r = q;                  /* matching ')' */
        for (; *r; r++) { if (*r == '(') pd++; else if (*r == ')') { pd--; if (!pd) { r++; break; } } }
        if (pd != 0) return 0;
        const char *after = r; while (*after == ' ' || *after == '\t') after++;
        if (*after != ':') continue;
        char name[KB_TERM_LEN]; if (idl == 0 || idl >= sizeof name) continue;
        memcpy(name, id, idl); name[idl] = '\0';
        if (!want || strcmp(name, want) == 0) {
            params_beg = q + 1; params_end = r - 1; colon = after; break;
        }
    }
    if (!colon) return 0;

    /* Parameter names: the FIRST identifier of each comma segment (the delta from
     * C's last_ident — Python writes `a` or `a: int`, not `int a`). */
    char params[8][KB_TERM_LEN]; size_t np = 0;
    {
        const char *s = params_beg;
        while (s < params_end && np < 8) {
            const char *seg = s; while (s < params_end && *s != ',') s++;
            const char *t = seg; while (t < s && !(isalpha((unsigned char)*t) || *t == '_')) t++;
            const char *u = t; while (u < s && (isalnum((unsigned char)*u) || *u == '_')) u++;
            if (u > t) { size_t l = (size_t)(u - t);
                if (l < KB_TERM_LEN) { memcpy(params[np], t, l); params[np][l] = '\0'; np++; } }
            if (s < params_end && *s == ',') s++;
        }
    }

    EvalCtx e;
    e.params = params; e.vals = argv; e.np = np; e.nv = argc; e.nl = 0;
    e.src = src; e.depth = depth; e.err = 0; e.ret = 0; e.retval = 0;
    e.c = NULL; e.end = NULL;

    /* Walk statements line by line from just after the ':' (covers both the inline
     * `def f(...): return x` form and an indented multi-line body). */
    const char *p = colon + 1;
    const char *src_end = src + strlen(src);
    while (p < src_end && !e.ret && !e.err) {
        while (p < src_end && (*p == ' ' || *p == '\t')) p++;   /* indentation */
        if (p >= src_end) break;
        if (*p == '\n') { p++; continue; }                      /* blank line */
        const char *ls = p, *le = p; while (le < src_end && *le != '\n') le++;

        if (strncmp(ls, "return", 6) == 0 && (ls + 6 == le || isspace((unsigned char)ls[6]))) {
            const char *ex = ls + 6; while (ex < le && isspace((unsigned char)*ex)) ex++;
            e.c = ex; e.end = le;
            long v = ev_rel(&e);
            if (!e.err) { e.retval = v; e.ret = 1; }
            break;
        }

        /* `name = expr` (a single '=', not '==') — an assignment to a local. */
        const char *t = ls; while (t < le && (isalnum((unsigned char)*t) || *t == '_')) t++;
        if (t > ls && (isalpha((unsigned char)*ls) || *ls == '_')) {
            const char *q = t; while (q < le && (*q == ' ' || *q == '\t')) q++;
            if (q < le && *q == '=' && (q + 1 >= le || q[1] != '=')) {
                char nm[KB_TERM_LEN]; size_t l = (size_t)(t - ls);
                if (l < sizeof nm) {
                    memcpy(nm, ls, l); nm[l] = '\0';
                    const char *ex = q + 1; while (ex < le && isspace((unsigned char)*ex)) ex++;
                    e.c = ex; e.end = le;
                    long v = ev_rel(&e);
                    if (e.err) break;
                    ev_set_local(&e, nm, v);
                    p = (le < src_end) ? le + 1 : le;
                    continue;
                }
            }
        }
        e.err = 1; break;          /* anything else: refuse honestly */
    }
    if (e.err || !e.ret) return 0;
    *out = e.retval;
    return 1;
}

static int eval_any(const char *src, const char *want,
                    const long *argv, size_t argc, long *out, int depth) {
    if (eval_fn(src, want, argv, argc, out, depth)) return 1;
    return eval_py_fn(src, want, argv, argc, out, depth);
}

int code_eval(const char *src, const char *want,
              const long *argv, size_t argc, long *out) {
    return eval_any(src, want, argv, argc, out, 0);
}

/* gen184: blank (in place, with spaces) the contents of line/block comments and
 * string/char literals, preserving every other byte and the overall length. A
 * single robustness pass applied at the entry points, so every downstream scanner
 * (code_ingest, code_defines, the evaluator) sees only real code — a '(' or '{'
 * or '}' inside a comment or a string no longer derails paren/brace tracking.
 * Real source files are full of these; this is the prerequisite for them. */
void code_strip(char *s) {
    if (!s) return;
    char *p = s;
    while (*p) {
        if (p[0] == '/' && p[1] == '/') {                 /* line comment */
            while (*p && *p != '\n') { *p = ' '; p++; }
        } else if (p[0] == '/' && p[1] == '*') {          /* block comment */
            p[0] = p[1] = ' '; p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/')) { if (*p != '\n') *p = ' '; p++; }
            if (p[0] == '*' && p[1] == '/') { p[0] = p[1] = ' '; p += 2; }
        } else if (*p == '"' || *p == '\'') {             /* string / char literal */
            char q = *p; *p = ' '; p++;
            while (*p && *p != q) {
                if (*p == '\\' && p[1]) { p[0] = ' '; p[1] = ' '; p += 2; }
                else { if (*p != '\n') *p = ' '; p++; }
            }
            if (*p == q) { *p = ' '; p++; }
        } else p++;
    }
}

/* gen182: true if `src` defines a function literally named `want`. A focused
 * copy of code_ingest's definition-head detection, without a KB (used by
 * code_locate to test a candidate file). */
int code_defines(const char *src, const char *want) {
    if (!src || !want || !*want) return 0;

    /* gen196 (language-as-delta): the Python definition head `def want(` — a
     * "def" token at a word boundary, then `want`, then `(`. Checked first so a
     * Python file is recognized through the same code_locate path as C. */
    size_t wl = strlen(want);
    for (const char *p = strstr(src, "def "); p; p = strstr(p + 1, "def ")) {
        if (p != src && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue; /* not a boundary */
        const char *d = p + 4;
        while (*d == ' ' || *d == '\t') d++;
        if (strncmp(d, want, wl) == 0) {
            const char *a = d + wl;
            if (!(isalnum((unsigned char)*a) || *a == '_')) {        /* whole word */
                while (*a == ' ' || *a == '\t') a++;
                if (*a == '(') return 1;
            }
        }
    }

    const char *p = src;
    while (*p) {
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);
        const char *q = p; while (*q && isspace((unsigned char)*q)) q++;
        if (*q != '(') continue;
        int d = 0; const char *r = q;
        for (; *r; r++) {
            if (*r == '(') d++;
            else if (*r == ')') { d--; if (d == 0) { r++; break; } }
        }
        if (d != 0) break;
        const char *after = r; while (*after && isspace((unsigned char)*after)) after++;
        if (*after != '{') continue;
        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) { p = after; continue; }
        memcpy(name, id, idlen); name[idlen] = '\0';
        if (is_c_keyword(name)) { p = after; continue; }
        if (strcmp(name, want) == 0) return 1;
        p = after;
    }
    return 0;
}

/* gen183: walk `dir` (and its subdirectories) for a .c/.h file defining `fnname`.
 * `rel` is the path so far relative to the search root, so the answer names the
 * file's location in the tree (e.g. "fixtures/sample.c"). Hidden entries (".",
 * "..", ".git", ...) are skipped; a depth ceiling bounds the walk. */
static int locate_rec(const char *dir, const char *rel, const char *fnname,
                      char *out_file, size_t out_sz, int depth) {
    if (depth > 32) return 0;
    DIR *d = opendir(dir);
    if (!d) return 0;
    int found = 0;
    struct dirent *de;
    while (!found && (de = readdir(d)) != NULL) {
        const char *nm = de->d_name;
        if (nm[0] == '.') continue;                       /* skip . .. and hidden */
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, nm);
        char relchild[768];
        if (rel && *rel) snprintf(relchild, sizeof relchild, "%s/%s", rel, nm);
        else snprintf(relchild, sizeof relchild, "%s", nm);

        struct stat st;
        int isdir = (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
        if (isdir) {
            if (locate_rec(path, relchild, fnname, out_file, out_sz, depth + 1)) found = 1;
        } else {
            size_t l = strlen(nm);
            int is_c  = (l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h'));
            int is_py = (l >= 4 && nm[l-3] == '.' && nm[l-2] == 'p' && nm[l-1] == 'y'); /* gen196 */
            if (is_c || is_py) {
                static char buf[262144];          /* fits normal source files */
                if (code_read_file(path, buf, sizeof buf)) {
                    code_strip(buf);
                    if (code_defines(buf, fnname)) {
                        snprintf(out_file, out_sz, "%s", relchild);
                        found = 1;
                    }
                }
            }
        }
    }
    closedir(d);
    return found;
}

int code_locate(const char *dir, const char *fnname,
                char *out_file, size_t out_sz) {
    if (!dir || !*dir || !fnname || !*fnname || !out_file || out_sz == 0) return 0;
    /* same sandbox as code_read_file: relative paths under the working dir only */
    if (dir[0] == '/' || dir[0] == '~' || strstr(dir, "..")) return 0;
    return locate_rec(dir, "", fnname, out_file, out_sz, 0);
}

/* gen185: record (deduped) the functions in `src` whose body calls `target` —
 * the reverse of code_calls. Same brace-tracked pass as code_ingest, KB-less. */
static void callers_in_src(const char *src, const char *target,
                           char out[][KB_TERM_LEN], size_t max, size_t *pn) {
    char cur_fn[KB_TERM_LEN] = "";
    int brace = 0, fn_brace = -1;
    const char *p = src;
    while (*p) {
        if (*p == '{') { brace++; p++; continue; }
        if (*p == '}') {
            brace--;
            if (fn_brace >= 0 && brace < fn_brace) { cur_fn[0] = '\0'; fn_brace = -1; }
            p++; continue;
        }
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);
        const char *q = p; while (*q && isspace((unsigned char)*q)) q++;
        if (*q != '(') continue;
        int d = 0; const char *r = q;
        for (; *r; r++) {
            if (*r == '(') d++;
            else if (*r == ')') { d--; if (d == 0) { r++; break; } }
        }
        if (d != 0) break;
        const char *after = r; while (*after && isspace((unsigned char)*after)) after++;
        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) continue;
        memcpy(name, id, idlen); name[idlen] = '\0';
        if (is_c_keyword(name)) continue;
        if (*after == '{') {
            snprintf(cur_fn, sizeof cur_fn, "%s", name);
            fn_brace = brace + 1; p = after;
        } else if (cur_fn[0] && strcmp(name, target) == 0) {
            int dup = 0;
            for (size_t i = 0; i < *pn; i++) if (strcmp(out[i], cur_fn) == 0) dup = 1;
            if (!dup && *pn < max) { snprintf(out[*pn], KB_TERM_LEN, "%s", cur_fn); (*pn)++; }
        }
    }
}

static void callers_rec(const char *dir, const char *target,
                        char out[][KB_TERM_LEN], size_t max, size_t *pn, int depth) {
    if (depth > 32 || *pn >= max) return;
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        const char *nm = de->d_name;
        if (nm[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, nm);
        struct stat st;
        int isdir = (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
        if (isdir) callers_rec(path, target, out, max, pn, depth + 1);
        else {
            size_t l = strlen(nm);
            if (l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h')) {
                static char buf[262144];
                if (code_read_file(path, buf, sizeof buf)) {
                    code_strip(buf);
                    callers_in_src(buf, target, out, max, pn);
                }
            }
        }
    }
    closedir(d);
}

size_t code_find_callers(const char *dir, const char *target,
                         char out[][KB_TERM_LEN], size_t max) {
    if (!dir || !*dir || !target || !*target || !out || max == 0) return 0;
    if (dir[0] == '/' || dir[0] == '~' || strstr(dir, "..")) return 0;
    size_t n = 0;
    callers_rec(dir, target, out, max, &n, 0);
    return n;
}

/* gen187: F5 edit — rewrite `src_path` with whole-word identifier `oldname`
 * renamed to `newname`, writing the result to `out_path` (the original is never
 * touched). Occurrences inside comments and string/char literals are left alone
 * (a state machine tracks context), so only real code is renamed. Returns the
 * number of replacements, or -1 on error (bad names, sandbox, read/write fail). */
int code_rename(const char *src_path, const char *oldname,
                const char *newname, const char *out_path) {
    if (!src_path || !oldname || !*oldname || !newname || !*newname || !out_path)
        return -1;
    if (out_path[0] == '/' || out_path[0] == '~' || strstr(out_path, "..")) return -1;
    for (const char *c = oldname; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return -1;
    for (const char *c = newname; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return -1;

    static char in[262144];
    if (!code_read_file(src_path, in, sizeof in)) return -1;
    static char out[393216];
    size_t oi = 0, replaced = 0;
    size_t oldlen = strlen(oldname), newlen = strlen(newname);
    const char *p = in;
#define EMIT(ch) do { if (oi + 1 >= sizeof out) return -1; out[oi++] = (ch); } while (0)
    while (*p) {
        if (p[0] == '/' && p[1] == '/') {                 /* line comment: verbatim */
            EMIT(*p); p++; EMIT(*p); p++;
            while (*p && *p != '\n') { EMIT(*p); p++; }
        } else if (p[0] == '/' && p[1] == '*') {          /* block comment: verbatim */
            EMIT(*p); p++; EMIT(*p); p++;
            while (*p && !(p[0] == '*' && p[1] == '/')) { EMIT(*p); p++; }
            if (*p) { EMIT(*p); p++; EMIT(*p); p++; }
        } else if (*p == '"' || *p == '\'') {             /* literal: verbatim */
            char q = *p; EMIT(*p); p++;
            while (*p && *p != q) {
                if (*p == '\\' && p[1]) { EMIT(*p); p++; EMIT(*p); p++; }
                else { EMIT(*p); p++; }
            }
            if (*p == q) { EMIT(*p); p++; }
        } else if (isalpha((unsigned char)*p) || *p == '_') {
            const char *id = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            size_t l = (size_t)(p - id);
            if (l == oldlen && strncmp(id, oldname, l) == 0) {
                for (size_t i = 0; i < newlen; i++) EMIT(newname[i]);
                replaced++;
            } else {
                for (const char *c = id; c < p; c++) EMIT(*c);
            }
        } else { EMIT(*p); p++; }
    }
#undef EMIT
    FILE *f = fopen(out_path, "w");
    if (!f) return -1;
    fwrite(out, 1, oi, f);
    fclose(f);
    return (int)replaced;
}

/* gen200: F5 edit (X7) — replace an exact code expression/statement with another,
 * in CODE regions only. A generalization of code_rename from one identifier to an
 * arbitrary token sequence — the transformation a real one-line bug fix needs. */
int code_replace_expr(const char *src_path, const char *oldexpr,
                      const char *newexpr, const char *out_path) {
    if (!src_path || !oldexpr || !*oldexpr || !newexpr || !out_path) return -1;
    if (out_path[0] == '/' || out_path[0] == '~' || strstr(out_path, "..")) return -1;
    static char in[262144];
    if (!code_read_file(src_path, in, sizeof in)) return -1;
    static char out[393216];
    size_t oi = 0, replaced = 0, oldlen = strlen(oldexpr), newlen = strlen(newexpr);
    const char *p = in;
#define EMIT(ch) do { if (oi + 1 >= sizeof out) return -1; out[oi++] = (ch); } while (0)
    while (*p) {
        if (p[0] == '/' && p[1] == '/') {                 /* C line comment */
            EMIT(*p); p++; EMIT(*p); p++; while (*p && *p != '\n') { EMIT(*p); p++; }
        } else if (p[0] == '/' && p[1] == '*') {          /* C block comment */
            EMIT(*p); p++; EMIT(*p); p++;
            while (*p && !(p[0] == '*' && p[1] == '/')) { EMIT(*p); p++; }
            if (*p) { EMIT(*p); p++; EMIT(*p); p++; }
        } else if (*p == '#') {                            /* Python line comment */
            while (*p && *p != '\n') { EMIT(*p); p++; }
        } else if ((p[0] == '"' && p[1] == '"' && p[2] == '"') ||
                   (p[0] == '\'' && p[1] == '\'' && p[2] == '\'')) {  /* triple-quoted */
            char q = *p; EMIT(*p); EMIT(p[1]); EMIT(p[2]); p += 3;
            while (*p && !(p[0] == q && p[1] == q && p[2] == q)) { EMIT(*p); p++; }
            if (*p) { EMIT(*p); EMIT(p[1]); EMIT(p[2]); p += 3; }
        } else if (*p == '"' || *p == '\'') {             /* string / char literal */
            char q = *p; EMIT(*p); p++;
            while (*p && *p != q) {
                if (*p == '\\' && p[1]) { EMIT(*p); p++; EMIT(*p); p++; }
                else { EMIT(*p); p++; }
            }
            if (*p == q) { EMIT(*p); p++; }
        } else if (strncmp(p, oldexpr, oldlen) == 0) {     /* the replacement */
            for (size_t i = 0; i < newlen; i++) EMIT(newexpr[i]);
            p += oldlen; replaced++;
        } else { EMIT(*p); p++; }
    }
#undef EMIT
    FILE *f = fopen(out_path, "w");
    if (!f) return -1;
    fwrite(out, 1, oi, f);
    fclose(f);
    return (int)replaced;
}

/* gen200: structural SYMMETRY-BREAK localization (see code.h). A general bug
 * smell: two parallel branches that should mirror each other, where one assigns
 * its own operand variable (`T[..P..] = P`, the healthy mirror) and the sibling
 * assigns a LITERAL where the analogous variable belongs (`T2[..Q..] = <lit>`).
 * The literal is the suspect; the fix mirrors the healthy branch (`= Q`). Names
 * nothing in advance — it reads the structure of whatever function it is given. */
static int sym_param_in_range(const char *a, const char *b,
                              char params[][KB_TERM_LEN], size_t np,
                              char *out, size_t outsz) {
    out[0] = '\0';
    for (const char *x = a; x < b; ) {
        if (isalpha((unsigned char)*x) || *x == '_') {
            const char *y = x; while (y < b && (isalnum((unsigned char)*y) || *y == '_')) y++;
            size_t l = (size_t)(y - x); char w[KB_TERM_LEN];
            if (l < sizeof w) { memcpy(w, x, l); w[l] = '\0';
                for (size_t i = 0; i < np; i++)
                    if (!strcmp(w, params[i])) { snprintf(out, outsz, "%s", w); return 1; } }
            x = y;
        } else x++;
    }
    return 0;
}

int code_symmetry_fix(const char *src_path, const char *fnname,
                      char *old_stmt, size_t old_sz,
                      char *new_stmt, size_t new_sz) {
    if (!src_path || !old_stmt || !new_stmt || !old_sz || !new_sz) return -1;
    old_stmt[0] = new_stmt[0] = '\0';
    static char buf[262144];
    if (!code_read_file(src_path, buf, sizeof buf)) return -1;

    for (const char *p = strstr(buf, "def "); p; p = strstr(p + 1, "def ")) {
        if (p != buf && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        const char *d = p + 4; while (*d == ' ' || *d == '\t') d++;
        const char *id = d; while (isalnum((unsigned char)*d) || *d == '_') d++;
        size_t idl = (size_t)(d - id); char fname[KB_TERM_LEN];
        if (idl == 0 || idl >= sizeof fname) continue;
        memcpy(fname, id, idl); fname[idl] = '\0';
        if (fnname && strcmp(fname, fnname) != 0) continue;
        const char *q = d; while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;
        int pd = 0; const char *r = q;
        for (; *r; r++) { if (*r == '(') pd++; else if (*r == ')') { pd--; if (!pd) { r++; break; } } }
        if (pd != 0) continue;

        char params[8][KB_TERM_LEN]; size_t np = 0;
        { const char *s = q + 1, *pe = r - 1;
          while (s < pe && np < 8) { const char *seg = s; while (s < pe && *s != ',') s++;
            const char *t = seg; while (t < s && !(isalpha((unsigned char)*t) || *t == '_')) t++;
            const char *u = t; while (u < s && (isalnum((unsigned char)*u) || *u == '_')) u++;
            if (u > t) { size_t l = (size_t)(u - t);
                if (l < KB_TERM_LEN) { memcpy(params[np], t, l); params[np][l] = '\0'; np++; } }
            if (s < pe && *s == ',') s++; } }

        const char *colon = r; while (*colon && *colon != ':' && *colon != '\n') colon++;
        if (*colon != ':') continue;
        const char *ls = p; while (ls > buf && ls[-1] != '\n') ls--;
        size_t def_indent = (size_t)(p - ls);
        const char *bp = colon; while (*bp && *bp != '\n') bp++; if (*bp == '\n') bp++;
        const char *bend = bp;
        while (*bend) {
            const char *lst = bend, *le = bend; while (*le && *le != '\n') le++;
            size_t ind = 0; const char *c = lst; while (*c == ' ' || *c == '\t') { ind++; c++; }
            int blank = (c == le || *c == '\0');
            if (!blank && ind <= def_indent) break;       /* dedent ends the function */
            bend = (*le == '\n') ? le + 1 : le;
            if (!*le) break;
        }

        int have_healthy = 0; char healthy_p[KB_TERM_LEN] = "";
        int have_broken = 0; char broken_line[256] = "", broken_q[KB_TERM_LEN] = "";
        for (const char *lp = bp; lp < bend; ) {
            const char *le = lp; while (le < bend && *le != '\n') le++;
            const char *t0 = lp; while (t0 < le && (*t0 == ' ' || *t0 == '\t')) t0++;
            const char *t1 = le; while (t1 > t0 && isspace((unsigned char)t1[-1])) t1--;
            if (t0 < t1 && (isalpha((unsigned char)*t0) || *t0 == '_')) {
                const char *ide = t0; while (ide < t1 && (isalnum((unsigned char)*ide) || *ide == '_')) ide++;
                const char *br = ide; while (br < t1 && (*br == ' ' || *br == '\t')) br++;
                if (br < t1 && *br == '[') {               /* T[ subscript ] = RHS */
                    int bd = 0; const char *se = br;
                    for (; se < t1; se++) { if (*se == '[') bd++; else if (*se == ']') { bd--; if (!bd) { se++; break; } } }
                    const char *eq = se; while (eq < t1 && (*eq == ' ' || *eq == '\t')) eq++;
                    if (eq < t1 && *eq == '=' && (eq + 1 >= t1 || eq[1] != '=')) {
                        const char *rhs = eq + 1; while (rhs < t1 && (*rhs == ' ' || *rhs == '\t')) rhs++;
                        char subp[KB_TERM_LEN]; sym_param_in_range(br, se, params, np, subp, sizeof subp);
                        const char *re = rhs; while (re < t1 && (isalnum((unsigned char)*re) || *re == '_')) re++;
                        int rhs_ident = (re > rhs && re == t1 &&
                                         (isalpha((unsigned char)*rhs) || *rhs == '_'));
                        int rhs_num = (rhs < t1); for (const char *x = rhs; x < t1; x++) if (!isdigit((unsigned char)*x)) { rhs_num = 0; break; }
                        if (rhs_ident) {
                            char rid[KB_TERM_LEN]; size_t l = (size_t)(re - rhs);
                            if (l < sizeof rid) { memcpy(rid, rhs, l); rid[l] = '\0';
                                if (subp[0] && !strcmp(rid, subp)) { have_healthy = 1; snprintf(healthy_p, sizeof healthy_p, "%s", rid); } }
                        } else if (rhs_num && subp[0]) {
                            size_t l = (size_t)(t1 - t0);
                            if (l < sizeof broken_line) { memcpy(broken_line, t0, l); broken_line[l] = '\0';
                                snprintf(broken_q, sizeof broken_q, "%s", subp); have_broken = 1; }
                        }
                    }
                }
            }
            lp = (le < bend) ? le + 1 : bend;
        }

        if (have_healthy && have_broken && strcmp(healthy_p, broken_q) != 0) {
            snprintf(old_stmt, old_sz, "%s", broken_line);
            char *lasteq = strrchr(broken_line, '=');
            if (!lasteq) return 0;
            size_t pre = (size_t)(lasteq - broken_line) + 1;
            snprintf(new_stmt, new_sz, "%.*s %s", (int)pre, broken_line, broken_q);
            return 1;
        }
        if (fnname) return 0;          /* requested fn analyzed, no symmetry break */
    }
    return 0;
}

/* gen201: known PURE (value-returning, non-mutating) str/bytes methods. Calling
 * one and discarding the result is always a no-op bug. A Python language fact (a
 * small KB), not a phrasebook of answers. */
static int is_pure_method(const char *m) {
    static const char *const pure[] = {
        "replace", "strip", "lstrip", "rstrip", "lower", "upper", "title",
        "capitalize", "swapcase", "casefold", "expandtabs", "zfill", "center",
        "ljust", "rjust", "format", "encode", "decode", NULL,
    };
    for (const char *const *p = pure; *p; p++) if (!strcmp(m, *p)) return 1;
    return 0;
}

int code_find_discarded_result(const char *src_path, const char *fnname,
                               char *old_stmt, size_t old_sz,
                               char *new_stmt, size_t new_sz) {
    if (!src_path || !old_stmt || !new_stmt || !old_sz || !new_sz) return -1;
    old_stmt[0] = new_stmt[0] = '\0';
    static char buf[262144];
    if (!code_read_file(src_path, buf, sizeof buf)) return -1;

    for (const char *p = strstr(buf, "def "); p; p = strstr(p + 1, "def ")) {
        if (p != buf && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        const char *d = p + 4; while (*d == ' ' || *d == '\t') d++;
        const char *id = d; while (isalnum((unsigned char)*d) || *d == '_') d++;
        size_t idl = (size_t)(d - id); char fname[KB_TERM_LEN];
        if (idl == 0 || idl >= sizeof fname) continue;
        memcpy(fname, id, idl); fname[idl] = '\0';
        if (fnname && strcmp(fname, fnname) != 0) continue;
        const char *q = d; while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;
        int pd = 0; const char *r = q;
        for (; *r; r++) { if (*r == '(') pd++; else if (*r == ')') { pd--; if (!pd) { r++; break; } } }
        if (pd != 0) continue;

        char params[8][KB_TERM_LEN]; size_t np = 0;
        { const char *s = q + 1, *pe = r - 1;
          while (s < pe && np < 8) { const char *seg = s; while (s < pe && *s != ',') s++;
            const char *t = seg; while (t < s && !(isalpha((unsigned char)*t) || *t == '_')) t++;
            const char *u = t; while (u < s && (isalnum((unsigned char)*u) || *u == '_')) u++;
            if (u > t) { size_t l = (size_t)(u - t);
                if (l < KB_TERM_LEN) { memcpy(params[np], t, l); params[np][l] = '\0'; np++; } }
            if (s < pe && *s == ',') s++; } }

        const char *colon = r; while (*colon && *colon != ':' && *colon != '\n') colon++;
        if (*colon != ':') continue;
        const char *ls = p; while (ls > buf && ls[-1] != '\n') ls--;
        size_t def_indent = (size_t)(p - ls);
        const char *bp = colon; while (*bp && *bp != '\n') bp++; if (*bp == '\n') bp++;
        const char *bend = bp;
        while (*bend) {
            const char *lst = bend, *le = bend; while (*le && *le != '\n') le++;
            size_t ind = 0; const char *c = lst; while (*c == ' ' || *c == '\t') { ind++; c++; }
            int blank = (c == le || *c == '\0');
            if (!blank && ind <= def_indent) break;
            bend = (*le == '\n') ? le + 1 : le;
            if (!*le) break;
        }

        for (const char *lp = bp; lp < bend; ) {
            const char *le = lp; while (le < bend && *le != '\n') le++;
            const char *t0 = lp; while (t0 < le && (*t0 == ' ' || *t0 == '\t')) t0++;
            const char *t1 = le; while (t1 > t0 && isspace((unsigned char)t1[-1])) t1--;
            if (t0 < t1 && (isalpha((unsigned char)*t0) || *t0 == '_')) {
                const char *rcv = t0; while (rcv < t1 && (isalnum((unsigned char)*rcv) || *rcv == '_')) rcv++;
                if (rcv < t1 && *rcv == '.') {
                    const char *m = rcv + 1, *me = m;
                    while (me < t1 && (isalnum((unsigned char)*me) || *me == '_')) me++;
                    if (me < t1 && *me == '(') {
                        int bd = 0; const char *e = me;
                        for (; e < t1; e++) { if (*e == '(') bd++; else if (*e == ')') { bd--; if (!bd) { e++; break; } } }
                        if (e == t1) {                 /* the call IS the whole statement */
                            char meth[KB_TERM_LEN]; size_t ml = (size_t)(me - m);
                            char recv[KB_TERM_LEN]; size_t rl = (size_t)(rcv - t0);
                            if (ml < sizeof meth && rl < sizeof recv) {
                                memcpy(meth, m, ml); meth[ml] = '\0';
                                memcpy(recv, t0, rl); recv[rl] = '\0';
                                if (is_pure_method(meth)) {
                                    int is_param = 0;
                                    for (size_t i = 0; i < np; i++) if (!strcmp(recv, params[i])) { is_param = 1; break; }
                                    size_t l = (size_t)(t1 - t0);
                                    if (l < old_sz) { memcpy(old_stmt, t0, l); old_stmt[l] = '\0'; }
                                    if (is_param) snprintf(new_stmt, new_sz, "%s[:] = %.*s", recv, (int)l, t0);
                                    else          snprintf(new_stmt, new_sz, "%s = %.*s", recv, (int)l, t0);
                                    return 1;
                                }
                            }
                        }
                    }
                }
            }
            lp = (le < bend) ? le + 1 : bend;
        }
        if (fnname) return 0;
    }
    return 0;
}

/* gen204: does the token `name.attr` occur in [a,b)? (whole-word `name`). */
static int has_attr_use(const char *a, const char *b, const char *name, const char *attr) {
    size_t nl = strlen(name), al = strlen(attr);
    for (const char *p = a; p + nl + al + 1 < b; p++) {
        if (p != a && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        if (strncmp(p, name, nl) != 0) continue;
        if (p[nl] != '.') continue;
        if (strncmp(p + nl + 1, attr, al) != 0) continue;
        char after = p[nl + 1 + al];
        if (isalnum((unsigned char)after) || after == '_') continue;  /* attr is whole word */
        return 1;
    }
    return 0;
}

int code_find_cond_asymmetry(const char *src_path, const char *fnname,
                             char *old_stmt, size_t old_sz,
                             char *new_stmt, size_t new_sz) {
    if (!src_path || !old_stmt || !new_stmt || !old_sz || !new_sz) return -1;
    old_stmt[0] = new_stmt[0] = '\0';
    static char buf[262144];
    if (!code_read_file(src_path, buf, sizeof buf)) return -1;

    for (const char *p = strstr(buf, "def "); p; p = strstr(p + 1, "def ")) {
        if (p != buf && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        const char *d = p + 4; while (*d == ' ' || *d == '\t') d++;
        const char *id = d; while (isalnum((unsigned char)*d) || *d == '_') d++;
        size_t idl = (size_t)(d - id); char fname[KB_TERM_LEN];
        if (idl == 0 || idl >= sizeof fname) continue;
        memcpy(fname, id, idl); fname[idl] = '\0';
        if (fnname && strcmp(fname, fnname) != 0) continue;
        const char *q = d; while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;
        int pdp = 0; const char *r = q;
        for (; *r; r++) { if (*r == '(') pdp++; else if (*r == ')') { pdp--; if (!pdp) { r++; break; } } }
        if (pdp != 0) continue;
        const char *colon = r; while (*colon && *colon != ':' && *colon != '\n') colon++;
        if (*colon != ':') continue;
        const char *ls = p; while (ls > buf && ls[-1] != '\n') ls--;
        size_t def_indent = (size_t)(p - ls);
        const char *bp = colon; while (*bp && *bp != '\n') bp++; if (*bp == '\n') bp++;
        const char *bend = bp;
        while (*bend) {
            const char *lst = bend, *le = bend; while (*le && *le != '\n') le++;
            size_t ind = 0; const char *c = lst; while (*c == ' ' || *c == '\t') { ind++; c++; }
            int blank = (c == le || *c == '\0');
            if (!blank && ind <= def_indent) break;
            bend = (*le == '\n') ? le + 1 : le;
            if (!*le) break;
        }

        /* Sibling attrs: every ATTR appearing as `<ident>.ATTR is None` in the body. */
        char attrs[16][KB_TERM_LEN]; size_t nattr = 0;
        for (const char *s = bp; (s = strstr(s, " is None")) != NULL && s < bend; s++) {
            const char *e = s; const char *as = e;            /* walk back over attr */
            while (as > bp && (isalnum((unsigned char)as[-1]) || as[-1] == '_')) as--;
            if (as == e || as <= bp || as[-1] != '.') continue;  /* must be X.ATTR */
            size_t al = (size_t)(e - as); if (al == 0 || al >= KB_TERM_LEN) continue;
            char a[KB_TERM_LEN]; memcpy(a, as, al); a[al] = '\0';
            int dup = 0; for (size_t k = 0; k < nattr; k++) if (!strcmp(attrs[k], a)) { dup = 1; break; }
            if (!dup && nattr < 16) snprintf(attrs[nattr++], KB_TERM_LEN, "%s", a);
        }

        /* Candidate: a sole bare `if/elif NAME is None:` line. */
        for (const char *lp = bp; lp < bend; ) {
            const char *le = lp; while (le < bend && *le != '\n') le++;
            const char *t0 = lp; while (t0 < le && (*t0 == ' ' || *t0 == '\t')) t0++;
            const char *t1 = le; while (t1 > t0 && isspace((unsigned char)t1[-1])) t1--;
            const char *kw = NULL, *cs = NULL;
            if (t1 - t0 > 3 && strncmp(t0, "if ", 3) == 0) { kw = "if"; cs = t0 + 3; }
            else if (t1 - t0 > 5 && strncmp(t0, "elif ", 5) == 0) { kw = "elif"; cs = t0 + 5; }
            if (kw && t1[-1] == ':') {
                while (cs < t1 && (*cs == ' ' || *cs == '\t')) cs++;
                const char *ne = cs; while (ne < t1 && (isalnum((unsigned char)*ne) || *ne == '_')) ne++;
                const char *rest = ne; while (rest < t1 && (*rest == ' ' || *rest == '\t')) rest++;
                /* exactly `NAME is None :` — sole, bare */
                if (ne > cs && rest + 7 <= t1 && strncmp(rest, "is None", 7) == 0) {
                    const char *after = rest + 7; while (after < t1 && (*after == ' ' || *after == '\t')) after++;
                    if (after == t1 - 1 && *after == ':') {
                        char name[KB_TERM_LEN]; size_t nl = (size_t)(ne - cs);
                        if (nl < sizeof name) {
                            memcpy(name, cs, nl); name[nl] = '\0';
                            for (size_t k = 0; k < nattr; k++) {
                                if (has_attr_use(bp, bend, name, attrs[k])) {
                                    size_t l = (size_t)(t1 - t0);
                                    if (l < old_sz) { memcpy(old_stmt, t0, l); old_stmt[l] = '\0'; }
                                    snprintf(new_stmt, new_sz, "%s %s.%s is None:", kw, name, attrs[k]);
                                    return 1;
                                }
                            }
                        }
                    }
                }
            }
            lp = (le < bend) ? le + 1 : bend;
        }
        if (fnname) return 0;
    }
    return 0;
}

/* gen191: F5 edit — delete the top-level definition of `fnname`. The same engine
 * as rename (locate -> edit -> the caller compiles to verify), a SECOND
 * transformation proving the edit loop is rule-shaped, not one hardcoded op. We
 * scan top level only (brace depth 0), comment/string aware, find the identifier
 * that immediately precedes the parameter '(' of a definition (the next char that
 * opens a body is '{'), and cut from the start of that declaration through the
 * matching '}'. */
int code_delete_function(const char *src_path, const char *fnname,
                         const char *out_path) {
    if (!src_path || !fnname || !*fnname || !out_path) return -1;
    if (out_path[0] == '/' || out_path[0] == '~' || strstr(out_path, "..")) return -1;
    for (const char *c = fnname; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return -1;

    static char in[262144];
    if (!code_read_file(src_path, in, sizeof in)) return -1;

    size_t i = 0, decl_start = 0;
    while (in[decl_start] && isspace((unsigned char)in[decl_start])) decl_start++;
    i = decl_start;
    int brace = 0, name_locked = 0;
    char cur_name[KB_TERM_LEN] = "";
    long region_start = -1, region_end = -1;

    while (in[i]) {
        if (in[i] == '/' && in[i+1] == '/') { i += 2; while (in[i] && in[i] != '\n') i++; continue; }
        if (in[i] == '/' && in[i+1] == '*') { i += 2; while (in[i] && !(in[i] == '*' && in[i+1] == '/')) i++; if (in[i]) i += 2; continue; }
        if (in[i] == '"' || in[i] == '\'') {
            char q = in[i++];
            while (in[i] && in[i] != q) { if (in[i] == '\\' && in[i+1]) i += 2; else i++; }
            if (in[i] == q) i++;
            continue;
        }
        char c = in[i];
        if (brace == 0) {
            if (isalpha((unsigned char)c) || c == '_') {
                size_t s = i;
                while (isalnum((unsigned char)in[i]) || in[i] == '_') i++;
                if (!name_locked) {
                    size_t l = i - s;
                    if (l < KB_TERM_LEN) { memcpy(cur_name, in + s, l); cur_name[l] = '\0'; }
                }
                continue;
            }
            if (c == '(') { name_locked = 1; i++; continue; }
            if (c == '{') {
                if (name_locked && strcmp(cur_name, fnname) == 0) {
                    /* found the definition: cut decl_start .. matching '}'. */
                    region_start = (long)decl_start;
                    int d = 0; size_t j = i;
                    while (in[j]) {
                        if (in[j] == '/' && in[j+1] == '/') { j += 2; while (in[j] && in[j] != '\n') j++; continue; }
                        if (in[j] == '/' && in[j+1] == '*') { j += 2; while (in[j] && !(in[j] == '*' && in[j+1] == '/')) j++; if (in[j]) j += 2; continue; }
                        if (in[j] == '"' || in[j] == '\'') {
                            char q = in[j++];
                            while (in[j] && in[j] != q) { if (in[j] == '\\' && in[j+1]) j += 2; else j++; }
                            if (in[j] == q) j++;
                            continue;
                        }
                        if (in[j] == '{') d++;
                        else if (in[j] == '}') { d--; if (d == 0) { j++; break; } }
                        j++;
                    }
                    region_end = (long)j;
                    break;
                }
                brace++; i++; continue;
            }
            if (c == ';') { i++; decl_start = i; while (in[decl_start] && isspace((unsigned char)in[decl_start])) decl_start++; i = decl_start; name_locked = 0; cur_name[0] = '\0'; continue; }
            i++; continue;
        } else {
            if (c == '{') brace++;
            else if (c == '}') { brace--; if (brace == 0) { i++; decl_start = i; while (in[decl_start] && isspace((unsigned char)in[decl_start])) decl_start++; i = decl_start; name_locked = 0; cur_name[0] = '\0'; continue; } }
            i++; continue;
        }
    }

    if (region_start < 0 || region_end < 0) return 0;     /* not found */

    /* drop a run of blank lines left behind so the result stays tidy. */
    while (in[region_end] == '\n' || in[region_end] == '\r') region_end++;

    FILE *f = fopen(out_path, "w");
    if (!f) return -1;
    fwrite(in, 1, (size_t)region_start, f);
    fwrite(in + region_end, 1, strlen(in + region_end), f);
    fclose(f);
    return 1;
}

/* gen186: F5 verification — syntax-check `path` by running the C compiler as a
 * sandboxed subprocess (no shell, so no injection; a strict char whitelist on the
 * path; -fsyntax-only so nothing is written; a child alarm() bounds the time). A
 * compiler is a DETERMINISTIC tool, not outsourced intelligence (CODE-MASTERY.md
 * §4), so this is allowed. Returns 1 if it compiles, 0 if not (first diagnostics
 * in `err_out`), -1 if it could not be run (bad path / fork failure). */
int code_compile(const char *path, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!path || !*path) return -1;
    if (path[0] == '/' || path[0] == '~' || strstr(path, "..")) return -1;
    for (const char *c = path; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '/' || *c == '.' ||
              *c == '_' || *c == '-')) return -1;

    int pf[2];
    if (pipe(pf) != 0) return -1;
    pid_t pid = fork();
    if (pid < 0) { close(pf[0]); close(pf[1]); return -1; }
    if (pid == 0) {                       /* child: compiler, output -> pipe */
        dup2(pf[1], STDOUT_FILENO);
        dup2(pf[1], STDERR_FILENO);
        close(pf[0]); close(pf[1]);
        alarm(15);                        /* never hang the agent */
        execlp("cc", "cc", "-fsyntax-only", "-w", path, (char *)NULL);
        _exit(127);                       /* exec failed */
    }
    close(pf[1]);
    if (err_out && err_sz) {
        ssize_t n = read(pf[0], err_out, err_sz - 1);
        if (n < 0) n = 0;
        err_out[n] = '\0';
    } else {
        char sink[256];
        while (read(pf[0], sink, sizeof sink) > 0) { }
    }
    close(pf[0]);
    int status;
    if (waitpid(pid, &status, 0) < 0) return -1;
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) return 1;
    return 0;
}

int code_build(const char *src_path, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!src_path || !*src_path) return -1;
    if (src_path[0] == '/' || src_path[0] == '~' || strstr(src_path, "..")) return -1;
    for (const char *c = src_path; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '/' || *c == '.' ||
              *c == '_' || *c == '-')) return -1;

    const char *exe = ".p0_build_tmp.out";
    int pf[2];
    if (pipe(pf) != 0) return -1;
    pid_t pid = fork();
    if (pid < 0) { close(pf[0]); close(pf[1]); return -1; }
    if (pid == 0) {                       /* child: compile+link, output -> pipe */
        dup2(pf[1], STDOUT_FILENO);
        dup2(pf[1], STDERR_FILENO);
        close(pf[0]); close(pf[1]);
        alarm(20);                        /* never hang the agent */
        execlp("cc", "cc", "-w", src_path, "-o", exe, (char *)NULL);
        _exit(127);                       /* exec failed */
    }
    close(pf[1]);
    if (err_out && err_sz) {
        ssize_t n = read(pf[0], err_out, err_sz - 1);
        if (n < 0) n = 0;
        err_out[n] = '\0';
    } else {
        char sink[256];
        while (read(pf[0], sink, sizeof sink) > 0) { }
    }
    close(pf[0]);
    int status;
    if (waitpid(pid, &status, 0) < 0) { remove(exe); return -1; }
    remove(exe);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) return 1;
    return 0;
}

/* gen198: build then RUN. Compile+link into a private temp executable (capturing
 * any build diagnostics), and if that succeeds, fork+exec the executable itself
 * in a sandboxed child (output discarded, an alarm() bounds the time) and read
 * back its exit status. Same path whitelist as the rest of the file. The two
 * subprocesses are kept separate so a build failure is reported distinctly from
 * a run that crashed. */
int code_run(const char *src_path, int *exit_code, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (exit_code) *exit_code = 0;
    if (!src_path || !*src_path) return -1;
    if (src_path[0] == '/' || src_path[0] == '~' || strstr(src_path, "..")) return -1;
    for (const char *c = src_path; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '/' || *c == '.' ||
              *c == '_' || *c == '-')) return -1;

    const char *exe = "./.p0_run_tmp.out";

    /* Stage 1: compile+link, capturing diagnostics. */
    int pf[2];
    if (pipe(pf) != 0) return -1;
    pid_t bpid = fork();
    if (bpid < 0) { close(pf[0]); close(pf[1]); return -1; }
    if (bpid == 0) {
        dup2(pf[1], STDOUT_FILENO);
        dup2(pf[1], STDERR_FILENO);
        close(pf[0]); close(pf[1]);
        alarm(20);
        execlp("cc", "cc", "-w", src_path, "-o", exe, (char *)NULL);
        _exit(127);
    }
    close(pf[1]);
    if (err_out && err_sz) {
        ssize_t n = read(pf[0], err_out, err_sz - 1);
        if (n < 0) n = 0;
        err_out[n] = '\0';
    } else {
        char sink[256];
        while (read(pf[0], sink, sizeof sink) > 0) { }
    }
    close(pf[0]);
    int bstatus;
    if (waitpid(bpid, &bstatus, 0) < 0) { remove(exe); return -1; }
    if (!(WIFEXITED(bstatus) && WEXITSTATUS(bstatus) == 0)) { remove(exe); return -1; }

    /* Stage 2: execute the built program and capture its exit status. */
    pid_t rpid = fork();
    if (rpid < 0) { remove(exe); return -1; }
    if (rpid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) { dup2(devnull, STDOUT_FILENO); dup2(devnull, STDERR_FILENO); }
        alarm(15);
        execl(exe, exe, (char *)NULL);
        _exit(127);
    }
    int rstatus;
    if (waitpid(rpid, &rstatus, 0) < 0) { remove(exe); return -1; }
    remove(exe);
    if (WIFEXITED(rstatus)) {
        if (exit_code) *exit_code = WEXITSTATUS(rstatus);
        return 1;
    }
    return 0;                    /* killed by a signal / timed out */
}

int code_read_file(const char *path, char *buf, size_t bufsz) {
    if (!path || !*path || !buf || bufsz == 0) return 0;
    /* gen181 sandbox: relative paths under the working directory only — no
     * absolute paths, no parent traversal. parrot0 reads the project it lives in,
     * nothing outside it. */
    if (path[0] == '/' || path[0] == '~' || strstr(path, "..")) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    size_t n = fread(buf, 1, bufsz - 1, f);
    int more = (fgetc(f) != EOF);     /* did it not fit? */
    fclose(f);
    buf[n] = '\0';
    if (more) return 0;               /* too big to reason about reliably */
    return n > 0;
}
