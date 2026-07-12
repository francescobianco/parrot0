/* gen173: parrot0's in-C structural code reader. See code.h for the contract and
 * the principle (AST-as-KB, deterministic parse, straight into RAM). This is the
 * F2 (grammar/structure) faculty of docs/CODE-MASTERY.md, the one place where the
 * formal, decidable grammar of code lets primitives-first dominate outright. */
#include "code.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
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
    /* gen324 (TODO.md P2, corrected): COMPOUND ASSIGNMENT — `x += e` and friends.
     *
     * The plan said the evaluator "cannot execute a loop". It can: gen178 added a
     * bounded while, gen179 a three-clause for. What it could not do was the one
     * statement almost every loop BODY is made of. These two differ by nothing
     * else, and only the first was computable:
     *
     *   for (i=1; i<=n; i++) { s = s + i; }   -> sum(3) = 6
     *   for (i=1; i<=n; i++) { s += i; }      -> "beyond my arithmetic evaluator"
     *
     * So the wall was not iteration; it was `+=`. Desugared to `x = x OP e`,
     * reusing the same expression parser — no second path.
     *
     * Division and modulo by zero latch err, so the evaluator refuses honestly
     * instead of returning a fabricated number. */
    if (e->c + 1 < e->end && e->c[1] == '=' &&
        (*e->c == '+' || *e->c == '-' || *e->c == '*' ||
         *e->c == '/' || *e->c == '%')) {
        char op = *e->c;
        e->c += 2;
        long v = ev_rel(e);
        if (e->err) return;
        if (!known) { e->err = 1; return; }      /* update of an unknown name */
        long r;
        switch (op) {
            case '+': r = cur + v; break;
            case '-': r = cur - v; break;
            case '*': r = cur * v; break;
            case '/': if (v == 0) { e->err = 1; return; } r = cur / v; break;
            default:  if (v == 0) { e->err = 1; return; } r = cur % v; break;
        }
        ev_set_local(e, w, r);
        return;
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

/* gen210: structural CASE-FOLDING localization (see code.h). The smell is a parser
 * that matches ALL-CAPS keyword literals case-SENSITIVELY in two mechanisms at once —
 * a flagless `re.compile(...)` AND an `IDENT == "ALLCAPS"` equality on input-derived
 * data. Firing only on that coupling keeps it a real, general inconsistency signal
 * (not an enum/sentinel false positive, not a fit to one instance). Edits are sliced
 * from the real source so spacing is exact; the real test suite disposes. */
int code_find_case_folding(const char *src_path,
                           char olds[][256], char news[][256], size_t max) {
    if (!src_path || !olds || !news || max == 0) return -1;
    static char buf[262144];
    if (!code_read_file(src_path, buf, sizeof buf)) return -1;

    char a_old[256] = "", a_new[256] = "";   /* Shape A: re.compile(...) + IGNORECASE */
    char b_old[256] = "", b_new[256] = "";   /* Shape B: IDENT == "UPPER" -> .upper() */
    int have_a = 0, have_b = 0;

    /* ---- Shape A: re.compile( <single top-level arg, no flags> ) ---- */
    for (const char *p = strstr(buf, "re.compile("); p && !have_a;
         p = strstr(p + 1, "re.compile(")) {
        if (p != buf && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) continue;
        const char *open = p + 10;            /* the '(' of re.compile( */
        const char *q = open, *close = NULL; int depth = 0, top_comma = 0;
        for (; *q; q++) {
            char c = *q;
            if (c == '"' || c == '\'') {      /* skip a string literal (incl triple) */
                char qq = c;
                if (q[1] == qq && q[2] == qq) {
                    q += 3; while (*q && !(q[0]==qq && q[1]==qq && q[2]==qq)) q++;
                    if (*q) q += 2;
                } else {
                    q++; while (*q && *q != qq) { if (*q=='\\' && q[1]) q++; q++; }
                }
                continue;
            }
            if (c == '(') depth++;
            else if (c == ')') { depth--; if (depth == 0) { close = q; break; } }
            else if (c == ',' && depth == 1) top_comma++;
        }
        if (!close || top_comma != 0) continue;          /* must be a SINGLE-arg call */
        const char *arg0 = open + 1; while (arg0 < close && (*arg0==' '||*arg0=='\t')) arg0++;
        if (arg0 >= close) continue;                     /* empty () */
        size_t calllen = (size_t)(close - p + 1);
        if (calllen + 20 >= sizeof a_old) continue;
        memcpy(a_old, p, calllen); a_old[calllen] = '\0';
        size_t pre = (size_t)(close - p);                /* up to, not incl, ')' */
        memcpy(a_new, p, pre);
        int wn = snprintf(a_new + pre, sizeof a_new - pre, ", re.IGNORECASE)");
        if (wn > 0 && pre + (size_t)wn < sizeof a_new) have_a = 1;
    }

    /* ---- Shape B: IDENT == "ALLCAPS"  (>=2 uppercase letters) ---- */
    for (const char *p = buf; *p && !have_b; p++) {
        if (*p == '#') { while (*p && *p != '\n') p++; if (!*p) break; continue; }
        if (*p != '"' && *p != '\'') continue;
        char qq = *p;
        if (p[1] == qq && p[2] == qq) {                  /* skip triple-quoted block */
            p += 3; while (*p && !(p[0]==qq && p[1]==qq && p[2]==qq)) p++;
            if (*p) p += 2;          /* land on the last quote; the for-loop's p++ steps off */
            continue;
        }
        const char *s = p + 1, *e = s;
        while (*e && *e != qq) { if (*e=='\\' && e[1]) e++; e++; }
        if (*e != qq) break;                             /* unterminated literal */
        size_t ll = (size_t)(e - s); int allup = ll >= 2;
        for (const char *c = s; c < e && allup; c++) if (!(*c >= 'A' && *c <= 'Z')) allup = 0;
        if (allup) {
            const char *bk = p; while (bk > buf && (bk[-1]==' '||bk[-1]=='\t')) bk--;
            if (bk - 2 >= buf && bk[-1] == '=' && bk[-2] == '=') {
                const char *id_e = bk - 2; while (id_e > buf && (id_e[-1]==' '||id_e[-1]=='\t')) id_e--;
                const char *id_s = id_e; while (id_s > buf && (isalnum((unsigned char)id_s[-1])||id_s[-1]=='_')) id_s--;
                /* a plain identifier (not an attribute access a.b == ...) */
                if (id_e > id_s && (id_s == buf ||
                    !(isalnum((unsigned char)id_s[-1]) || id_s[-1]=='_' || id_s[-1]=='.'))) {
                    size_t oldlen = (size_t)(e - id_s + 1);
                    size_t idlen  = (size_t)(id_e - id_s);
                    if (oldlen + 8 < sizeof b_old && idlen < 200) {
                        memcpy(b_old, id_s, oldlen); b_old[oldlen] = '\0';
                        char id[200]; memcpy(id, id_s, idlen); id[idlen] = '\0';
                        size_t restlen = (size_t)(e - id_e + 1);   /* spaces..== .."UPPER" */
                        int wn = snprintf(b_new, sizeof b_new, "%s.upper()%.*s",
                                          id, (int)restlen, id_e);
                        if (wn > 0 && (size_t)wn < sizeof b_new) have_b = 1;
                    }
                }
            }
        }
        p = e;                                            /* resume after the literal */
    }

    if (!(have_a && have_b)) return 0;                    /* coupling absent -> not ours */

    size_t n = 0;
    if (n < max) { snprintf(olds[n], 256, "%s", a_old); snprintf(news[n], 256, "%s", a_new); n++; }
    if (n < max) { snprintf(olds[n], 256, "%s", b_old); snprintf(news[n], 256, "%s", b_new); n++; }
    return (int)n;
}

/* gen256: does ANY structural bug smell fire on this one file? The chain is the
 * same one the brain's "fix the bug in <path>" branch walks (symmetry break,
 * discarded result, condition asymmetry, case folding); here it is only a yes/no
 * probe, so a firing file can be REPORTED and then re-analyzed by the detailed
 * per-file path. Unreadable/non-source files simply decline. */
static int smell_fires(const char *path) {
    char o[256], n[256];
    if (code_symmetry_fix(path, NULL, o, sizeof o, n, sizeof n) > 0) return 1;
    if (code_find_discarded_result(path, NULL, o, sizeof o, n, sizeof n) > 0) return 1;
    if (code_find_cond_asymmetry(path, NULL, o, sizeof o, n, sizeof n) > 0) return 1;
    char co[2][256], cn[2][256];
    if (code_find_case_folding(path, co, cn, 2) > 0) return 1;
    return 0;
}

static int smell_tree_rec(const char *dir, char *out_file, size_t out_sz, int depth) {
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
        struct stat st;
        int isdir = (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
        if (isdir) {
            if (smell_tree_rec(path, out_file, out_sz, depth + 1)) found = 1;
        } else {
            size_t l = strlen(nm);
            int is_c  = (l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h'));
            int is_py = (l >= 4 && nm[l-3] == '.' && nm[l-2] == 'p' && nm[l-1] == 'y');
            if ((is_c || is_py) && smell_fires(path)) {
                snprintf(out_file, out_sz, "%s", path);
                found = 1;
            }
        }
    }
    closedir(d);
    return found;
}

int code_smell_tree(const char *dir, char *out_file, size_t out_sz) {
    if (!dir || !*dir || !out_file || out_sz == 0) return -1;
    /* same sandbox as code_locate: relative paths under the working dir only */
    if (dir[0] == '/' || dir[0] == '~' || strstr(dir, "..")) return -1;
    return smell_tree_rec(dir, out_file, out_sz, 0);
}

/* gen257 (Track 5.1, outer-circle codebase work): OR-chain perception. A run of
 * >=2 calls to the SAME function joined only by `||` is a vocabulary-shaped
 * condition — the structural signature of a C phrasebook (a word list encoded as
 * code). The detector is GENERIC: `fnname` is a parameter; it knows nothing of
 * "cue" or of this codebase. Perception only — it reads, counts and locates,
 * it changes nothing. Comments and string literals are blanked first
 * (code_strip), so a `||` in a comment never miscounts. */
static int at_fn_call(const char *buf, const char *p, const char *fnname, size_t fl) {
    if (strncmp(p, fnname, fl) != 0) return 0;
    if (p != buf && (isalnum((unsigned char)p[-1]) || p[-1] == '_')) return 0;
    if (isalnum((unsigned char)p[fl]) || p[fl] == '_') return 0;
    const char *q = p + fl;
    while (*q == ' ' || *q == '\t') q++;
    return *q == '(';
}

int code_find_or_chains(const char *src_path, const char *fnname,
                        int *lines, size_t max, int *total_calls) {
    if (!src_path || !fnname || !*fnname) return -1;
    static char buf[262144];
    if (!code_read_file(src_path, buf, sizeof buf)) return -1;
    code_strip(buf);
    size_t fl = strlen(fnname);
    int line = 1, nchains = 0, ncalls = 0;
    const char *p = buf;
    while (*p) {
        if (*p == '\n') { line++; p++; continue; }
        if (!((isalpha((unsigned char)*p) || *p == '_') && at_fn_call(buf, p, fnname, fl))) {
            p++; continue;
        }
        int startline = line, run = 0;
        for (;;) {                                   /* consume one call of the run */
            p += fl;
            while (*p == ' ' || *p == '\t') p++;     /* to the '(' */
            int depth = 0;
            for (; *p; p++) {
                if (*p == '\n') line++;
                else if (*p == '(') depth++;
                else if (*p == ')') { depth--; if (depth == 0) { p++; break; } }
            }
            run++;
            const char *q = p; int ln = line;        /* peek past ws for `|| fnname(` */
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!(q[0] == '|' && q[1] == '|')) break;
            q += 2;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!at_fn_call(buf, q, fnname, fl)) break;
            p = q; line = ln;                        /* the run continues */
        }
        if (run >= 2) {
            if (lines && (size_t)nchains < max) lines[nchains] = startline;
            nchains++; ncalls += run;
        }
    }
    if (total_calls) *total_calls = ncalls;
    return nchains;
}

static int orchain_word_seen(char words[][KB_TERM_LEN], size_t n, const char *w) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(words[i], w) == 0) return 1;
    return 0;
}

static void orchain_add_word(char words[][KB_TERM_LEN], size_t max,
                             size_t *nwords, const char *w) {
    if (!words || !nwords || !w || !*w || *nwords >= max) return;
    if (orchain_word_seen(words, *nwords, w)) return;
    snprintf(words[(*nwords)++], KB_TERM_LEN, "%s", w);
}

static void orchain_literals_between(const char *start, const char *end,
                                     char words[][KB_TERM_LEN], size_t max,
                                     size_t *nwords) {
    for (const char *p = start; p && p < end && *p; p++) {
        if (*p != '"') continue;
        p++;
        char lit[KB_TERM_LEN];
        size_t o = 0;
        while (p < end && *p && *p != '"') {
            char c = *p;
            if (c == '\\' && p + 1 < end && p[1]) {
                p++;
                c = *p;                 /* keep escaped char as its surface char */
            }
            if (o + 1 < sizeof lit && c != '\n' && c != '\r')
                lit[o++] = c;
            p++;
        }
        lit[o] = '\0';
        orchain_add_word(words, max, nwords, lit);
    }
}

int code_orchain_vocabulary(const char *src_path, const char *fnname,
                            char words[][KB_TERM_LEN], size_t max,
                            int *total_chains, int *total_calls) {
    if (!src_path || !fnname || !*fnname || !words) return -1;
    static char orig[262144];
    static char buf[262144];
    if (!code_read_file(src_path, orig, sizeof orig)) return -1;
    snprintf(buf, sizeof buf, "%s", orig);
    code_strip(buf);                         /* same structure view as gen257 */
    size_t fl = strlen(fnname), nwords = 0;
    int line = 1, nchains = 0, ncalls = 0;
    const char *p = buf;
    while (*p) {
        if (*p == '\n') { line++; p++; continue; }
        if (!((isalpha((unsigned char)*p) || *p == '_') && at_fn_call(buf, p, fnname, fl))) {
            p++; continue;
        }
        size_t starts[64], ends[64];
        int run = 0;
        for (;;) {
            const char *call_start = p;
            p += fl;
            while (*p == ' ' || *p == '\t') p++;
            int depth = 0;
            for (; *p; p++) {
                if (*p == '\n') line++;
                else if (*p == '(') depth++;
                else if (*p == ')') { depth--; if (depth == 0) { p++; break; } }
            }
            if (run < (int)(sizeof starts / sizeof starts[0])) {
                starts[run] = (size_t)(call_start - buf);
                ends[run] = (size_t)(p - buf);
            }
            run++;
            const char *q = p; int ln = line;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!(q[0] == '|' && q[1] == '|')) break;
            q += 2;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!at_fn_call(buf, q, fnname, fl)) break;
            p = q; line = ln;
        }
        if (run >= 2) {
            int kept = run < (int)(sizeof starts / sizeof starts[0]) ?
                       run : (int)(sizeof starts / sizeof starts[0]);
            for (int i = 0; i < kept; i++)
                orchain_literals_between(orig + starts[i], orig + ends[i],
                                         words, max, &nwords);
            nchains++; ncalls += run;
        }
    }
    if (total_chains) *total_chains = nchains;
    if (total_calls) *total_calls = ncalls;
    return (int)nwords;
}

static void orchain_vocab_tree_rec(const char *dir, const char *fnname, int depth,
                                   char words[][KB_TERM_LEN], size_t max,
                                   size_t *nwords, int *files_hit,
                                   int *chains, int *calls) {
    if (depth > 32) return;
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        const char *nm = de->d_name;
        if (nm[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, nm);
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            orchain_vocab_tree_rec(path, fnname, depth + 1,
                                   words, max, nwords, files_hit, chains, calls);
        } else {
            size_t l = strlen(nm);
            if (!(l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h'))) continue;
            char local[64][KB_TERM_LEN];
            int ch = 0, ca = 0;
            int nw = code_orchain_vocabulary(path, fnname, local, 64, &ch, &ca);
            if (nw < 0) continue;
            if (ch > 0) {
                (*files_hit)++; *chains += ch; *calls += ca;
                for (int i = 0; i < nw; i++)
                    orchain_add_word(words, max, nwords, local[i]);
            }
        }
    }
    closedir(d);
}

int code_orchain_vocabulary_tree(const char *dir, const char *fnname,
                                 char words[][KB_TERM_LEN], size_t max,
                                 int *files_hit, int *total_chains,
                                 int *total_calls) {
    if (!dir || !*dir || !fnname || !*fnname || !words) return -1;
    if (dir[0] == '/' || dir[0] == '~' || strstr(dir, "..")) return -1;
    size_t nw = 0;
    int fh = 0, ch = 0, ca = 0;
    orchain_vocab_tree_rec(dir, fnname, 0, words, max, &nw, &fh, &ch, &ca);
    if (files_hit) *files_hit = fh;
    if (total_chains) *total_chains = ch;
    if (total_calls) *total_calls = ca;
    return (int)nw;
}

static int orchain_ident_ok(const char *s) {
    if (!s || !*s) return 0;
    if (!(isalpha((unsigned char)*s) || *s == '_')) return 0;
    for (const char *c = s; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return 0;
    return 1;
}

int code_orchain_emit_facts(const char *src_path, const char *fnname,
                            const char *pred, const char *out_path,
                            int *total_chains) {
    if (!src_path || !fnname || !*fnname || !out_path || !*out_path) return -1;
    if (!orchain_ident_ok(pred)) return -1;    /* the predicate lands in a .p0 file */
    if (out_path[0] == '/' || out_path[0] == '~' || strstr(out_path, "..")) return -1;
    static char orig[262144];
    static char buf[262144];
    if (!code_read_file(src_path, orig, sizeof orig)) return -1;
    snprintf(buf, sizeof buf, "%s", orig);
    code_strip(buf);                         /* same structure view as gen257 */

    /* key stem: the file's basename as an identifier (foreign.c -> foreign) */
    const char *base = strrchr(src_path, '/');
    base = base ? base + 1 : src_path;
    char stem[64];
    size_t si = 0;
    for (const char *c = base; *c && *c != '.' && si + 1 < sizeof stem; c++)
        stem[si++] = (char)(isalnum((unsigned char)*c) ? tolower((unsigned char)*c) : '_');
    stem[si] = '\0';
    if (!si) return -1;

    FILE *f = NULL;
    size_t fl = strlen(fnname);
    int line = 1, nchains = 0, nfacts = 0;
    const char *p = buf;
    while (*p) {
        if (*p == '\n') { line++; p++; continue; }
        if (!((isalpha((unsigned char)*p) || *p == '_') && at_fn_call(buf, p, fnname, fl))) {
            p++; continue;
        }
        int chain_line = line;
        size_t starts[64], ends[64];
        int run = 0;
        for (;;) {
            const char *call_start = p;
            p += fl;
            while (*p == ' ' || *p == '\t') p++;
            int depth = 0;
            for (; *p; p++) {
                if (*p == '\n') line++;
                else if (*p == '(') depth++;
                else if (*p == ')') { depth--; if (depth == 0) { p++; break; } }
            }
            if (run < (int)(sizeof starts / sizeof starts[0])) {
                starts[run] = (size_t)(call_start - buf);
                ends[run] = (size_t)(p - buf);
            }
            run++;
            const char *q = p; int ln = line;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!(q[0] == '|' && q[1] == '|')) break;
            q += 2;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!at_fn_call(buf, q, fnname, fl)) break;
            p = q; line = ln;
        }
        if (run >= 2) {
            char cw[64][KB_TERM_LEN];        /* per-chain: one key per chain site */
            size_t ncw = 0;
            int kept = run < (int)(sizeof starts / sizeof starts[0]) ?
                       run : (int)(sizeof starts / sizeof starts[0]);
            for (int i = 0; i < kept; i++)
                orchain_literals_between(orig + starts[i], orig + ends[i],
                                         cw, 64, &ncw);
            if (ncw > 0) {
                if (!f) {
                    f = fopen(out_path, "w");
                    if (!f) return -1;
                    fprintf(f, "%% %s facts derived by parrot0 from OR-chains of "
                               "calls to %s in %s (one key per chain site)\n",
                            pred, fnname, src_path);
                }
                for (size_t i = 0; i < ncw; i++, nfacts++)
                    fprintf(f, "%s(%s_chain%d, \"%s\").\n",
                            pred, stem, chain_line, cw[i]);
            }
            nchains++;
        }
    }
    if (f) fclose(f);
    if (total_chains) *total_chains = nchains;
    return nfacts;
}

/* gen271: render one patched call from the codebase's call-shape template.
 * Whole-word tokens FN, ARG, KEY become the lookup function, the chain's
 * scrutinee and the QUOTED key; everything else is copied verbatim. The shape
 * is knowledge (a lookup_call/2 fact) because each codebase's lookup primitive
 * has its own signature — parrot0's kb_cue_match(b, "key", s) is not the
 * fixture's lookup(s, "key"). */
static int orchain_render_call(const char *tpl, const char *fn,
                               const char *arg, const char *key,
                               char *out, size_t out_sz) {
    size_t o = 0;
    const char *p = tpl;
    while (*p) {
        const char *sub = NULL;
        size_t tok = 0;
        int word_start = (p == tpl) ||
                         !(isalnum((unsigned char)p[-1]) || p[-1] == '_');
        if (word_start) {
            if (strncmp(p, "ARG", 3) == 0 &&
                !(isalnum((unsigned char)p[3]) || p[3] == '_')) { sub = arg; tok = 3; }
            else if (strncmp(p, "KEY", 3) == 0 &&
                     !(isalnum((unsigned char)p[3]) || p[3] == '_')) { sub = key; tok = 3; }
            else if (strncmp(p, "FN", 2) == 0 &&
                     !(isalnum((unsigned char)p[2]) || p[2] == '_')) { sub = fn; tok = 2; }
        }
        if (sub) {
            int m = (sub == key)   /* only the key renders quoted */
                    ? snprintf(out + o, out_sz - o, "\"%s\"", sub)
                    : snprintf(out + o, out_sz - o, "%s", sub);
            if (m < 0 || (size_t)m >= out_sz - o) return 0;
            o += (size_t)m;
            p += tok;
        } else {
            if (o + 1 >= out_sz) return 0;
            out[o++] = *p++;
        }
    }
    out[o] = '\0';
    return 1;
}

/* gen274: the identifiers a call template IMPORTS from the surrounding scope —
 * every C identifier in the template that is not a placeholder (FN/ARG/KEY) and
 * not the leading function name itself (e.g. `b` in "kb_cue_match(b, KEY, ARG)",
 * `ctx` in "vocab_hit(ctx, KEY, ARG)"). If one of these is not visible where a
 * chain lives, the rendered call cannot compile there. */
static int orchain_tpl_scope_idents(const char *tpl, char idents[][64], int max) {
    int n = 0, first = 1;
    const char *p = tpl ? tpl : "";
    while (*p) {
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *s = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t l = (size_t)(p - s);
        if (first) { first = 0; continue; }      /* the lookup call's own name */
        if ((l == 2 && strncmp(s, "FN", 2) == 0) ||
            (l == 3 && (strncmp(s, "ARG", 3) == 0 ||
                        strncmp(s, "KEY", 3) == 0)))
            continue;
        if (l >= 64 || n >= max) continue;
        char w[64];
        memcpy(w, s, l);
        w[l] = '\0';
        int dup = 0;
        for (int i = 0; i < n; i++) if (strcmp(idents[i], w) == 0) { dup = 1; break; }
        if (!dup) { strcpy(idents[n], w); n++; }
    }
    return n;
}

/* gen274: whole-word occurrence of `word` inside hay[0..len) — identifier
 * boundaries on both sides, so `b` never matches inside `buf`. */
static int orchain_word_in(const char *hay, size_t len, const char *word) {
    size_t wl = strlen(word);
    if (!wl || wl > len) return 0;
    for (size_t i = 0; i + wl <= len; i++) {
        if (strncmp(hay + i, word, wl) != 0) continue;
        if (i > 0 && (isalnum((unsigned char)hay[i - 1]) || hay[i - 1] == '_'))
            continue;
        char after = (i + wl < len) ? hay[i + wl] : '\0';
        if (isalnum((unsigned char)after) || after == '_') continue;
        return 1;
    }
    return 0;
}

int code_orchain_patch(const char *src_path, const char *fnname,
                       const char *lookup_fn, const char *call_tpl,
                       const char *out_path,
                       int *compiles, char *err_out, size_t err_sz,
                       int *skipped, char *skip_ident, size_t skip_sz) {
    if (compiles) *compiles = 0;
    if (err_out && err_sz) err_out[0] = '\0';
    if (skipped) *skipped = 0;
    if (skip_ident && skip_sz) skip_ident[0] = '\0';
    if (!src_path || !fnname || !*fnname || !out_path || !*out_path) return -1;
    if (!orchain_ident_ok(lookup_fn)) return -1;  /* lands verbatim in C source */
    if (out_path[0] == '/' || out_path[0] == '~' || strstr(out_path, "..")) return -1;
    static char orig[262144];
    static char buf[262144];
    static char out[393216];
    if (!code_read_file(src_path, orig, sizeof orig)) return -1;
    snprintf(buf, sizeof buf, "%s", orig);
    code_strip(buf);                         /* same structure view as gen257 */

    /* key stem: MUST match code_orchain_emit_facts so the patched sites point at
     * the keys the emitted facts actually use */
    const char *base = strrchr(src_path, '/');
    base = base ? base + 1 : src_path;
    char stem[64];
    size_t si = 0;
    for (const char *c = base; *c && *c != '.' && si + 1 < sizeof stem; c++)
        stem[si++] = (char)(isalnum((unsigned char)*c) ? tolower((unsigned char)*c) : '_');
    stem[si] = '\0';
    if (!si) return -1;

    size_t cs[64], ce[64], ctop[64];
    int cline[64];
    char carg[64][128];
    int nchains = 0;

    size_t fl = strlen(fnname);
    int line = 1;
    /* gen274: per-chain applicability needs the chain's ENCLOSING-FUNCTION
     * region — buf[top_start..chain_start). top_start moves past every
     * top-level `}` during the same linear walk (code_strip already blanked
     * braces in comments/strings), so the region starts at the current
     * function's signature and covers everything above the chain. */
    int bdepth = 0;
    size_t top_start = 0;
    const char *p = buf;
    while (*p) {
        if (*p == '\n') { line++; p++; continue; }
        if (*p == '{') { bdepth++; p++; continue; }
        if (*p == '}') {
            if (bdepth > 0 && --bdepth == 0) top_start = (size_t)(p - buf) + 1;
            p++; continue;
        }
        if (!((isalpha((unsigned char)*p) || *p == '_') && at_fn_call(buf, p, fnname, fl))) {
            p++; continue;
        }
        int chain_line = line;
        size_t chain_start = (size_t)(p - buf), chain_end = chain_start;
        int run = 0;
        for (;;) {
            p += fl;
            while (*p == ' ' || *p == '\t') p++;
            int depth = 0;
            for (; *p; p++) {
                if (*p == '\n') line++;
                else if (*p == '(') depth++;
                else if (*p == ')') { depth--; if (depth == 0) { p++; break; } }
            }
            chain_end = (size_t)(p - buf);
            run++;
            const char *q = p; int ln = line;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!(q[0] == '|' && q[1] == '|')) break;
            q += 2;
            while (*q == ' ' || *q == '\t' || *q == '\n') { if (*q == '\n') ln++; q++; }
            if (!at_fn_call(buf, q, fnname, fl)) break;
            p = q; line = ln;
        }
        if (run >= 2 && nchains < (int)(sizeof cs / sizeof cs[0])) {
            /* first argument of the FIRST call, read from the original text: the
             * scrutinee every call in the chain tests (e.g. `s`, `norm`) */
            char arg[128] = "";
            const char *a = orig + chain_start + fl;
            while (*a == ' ' || *a == '\t') a++;
            if (*a == '(') {
                a++;
                while (*a && isspace((unsigned char)*a)) a++;
                int d = 1;
                size_t o = 0;
                while (*a && !(d == 1 && (*a == ',' || *a == ')'))) {
                    if (*a == '(') d++;
                    else if (*a == ')') d--;
                    if (o + 1 < sizeof arg) arg[o++] = *a;
                    a++;
                }
                while (o > 0 && isspace((unsigned char)arg[o - 1])) o--;
                arg[o] = '\0';
            }
            cs[nchains] = chain_start;
            ce[nchains] = chain_end;
            ctop[nchains] = top_start;
            cline[nchains] = chain_line;
            snprintf(carg[nchains], sizeof carg[nchains], "%s", arg);
            nchains++;
        }
    }
    if (nchains == 0) return 0;

    /* gen274: which context identifiers the call shape imports (none for the
     * default placeholder-only templates, so gen262 targets are untouched) */
    char idents[4][64];
    int nid = orchain_tpl_scope_idents(call_tpl, idents, 4);

    size_t oi = 0, prev = 0;
    for (int i = 0; i < nchains; i++) {
        size_t seg = cs[i] - prev;
        if (oi + seg >= sizeof out) return -1;
        memcpy(out + oi, orig + prev, seg);
        oi += seg;
        /* gen274: a chain whose enclosing function does not see every imported
         * identifier is SKIPPED honestly (kept verbatim), not patched into code
         * that cannot compile. Heuristic scope check; the codebase's own
         * build/test suite stays the final judge. */
        int miss = -1;
        for (int k = 0; k < nid && miss < 0; k++)
            if (!orchain_word_in(buf + ctop[i], cs[i] - ctop[i], idents[k]))
                miss = k;
        if (miss >= 0) {
            size_t clen = ce[i] - cs[i];
            if (oi + clen >= sizeof out) return -1;
            memcpy(out + oi, orig + cs[i], clen);
            oi += clen;
            prev = ce[i];
            if (skipped) (*skipped)++;
            if (skip_ident && skip_sz)
                snprintf(skip_ident, skip_sz, "%s", idents[miss]);
            continue;
        }
        char key[128];
        snprintf(key, sizeof key, "%s_chain%d", stem, cline[i]);
        const char *tpl = (call_tpl && *call_tpl) ? call_tpl
                        : (carg[i][0] ? "FN(ARG, KEY)" : "FN(KEY)");
        char call[512];
        if (!orchain_render_call(tpl, lookup_fn, carg[i], key,
                                 call, sizeof call)) return -1;
        int m = snprintf(out + oi, sizeof out - oi, "%s", call);
        if (m < 0 || (size_t)m >= sizeof out - oi) return -1;
        oi += (size_t)m;
        prev = ce[i];
    }
    size_t tail = strlen(orig + prev);
    if (oi + tail >= sizeof out) return -1;
    memcpy(out + oi, orig + prev, tail);
    oi += tail;

    FILE *f = fopen(out_path, "w");
    if (!f) return -1;
    if (fwrite(out, 1, oi, f) != oi) { fclose(f); remove(out_path); return -1; }
    fclose(f);

    /* mechanical verdict: the patched copy must still be valid C. cc silently
     * ignores unknown suffixes like .p0fix (exit 0 without compiling — a false
     * pass), so the check runs on a .c-suffixed temp copy, removed after.
     * gen271 honesty: if the patched copy fails, compile the ORIGINAL the same
     * way; when that fails too, the file is a FRAGMENT of a larger translation
     * unit and the standalone judge does not apply (*compiles = -1) — reporting
     * "broke the build" would be a false verdict; the codebase's own build is
     * the judge. */
    char tmp[64];   /* gen278: per-process temp so parallel instances don't race */
    snprintf(tmp, sizeof tmp, ".p0_patch_tmp_%ld.c", (long)getpid());
    FILE *t = fopen(tmp, "w");
    if (t) {
        size_t w = fwrite(out, 1, oi, t);
        fclose(t);
        if (w == oi && compiles) {
            *compiles = code_compile(tmp, err_out, err_sz) == 1;
            if (!*compiles &&
                code_compile(src_path, NULL, 0) != 1)
                *compiles = -1;
        }
        remove(tmp);
    }
    return nchains;
}

int code_orchain_verify(const char *orig_path, const char *patched_path,
                        const char *chain_fn, const char *probe_fn,
                        int *nprobes, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (nprobes) *nprobes = 0;
    if (!orig_path || !patched_path || !chain_fn || !*chain_fn) return -1;
    if (!orchain_ident_ok(probe_fn)) return -1;   /* lands verbatim in the harness */
    if (patched_path[0] == '/' || patched_path[0] == '~' ||
        strstr(patched_path, "..")) return -1;

    /* Probes: the vocabulary the chains encode — each word exercises the branch
     * that used to test it — plus one guaranteed miss. Derived from the ORIGINAL,
     * so the patched version is judged against what the code really said. */
    char words[32][KB_TERM_LEN];
    int ch = 0, ca = 0;
    int nw = code_orchain_vocabulary(orig_path, chain_fn, words, 32, &ch, &ca);
    if (nw < 0) return -1;
    if (ch == 0 || nw == 0) return -1;

    static char probes[8192];
    size_t pi = 0;
    for (int i = 0; i <= nw; i++) {
        const char *w = (i < nw) ? words[i] : "p0nomatch";
        if (pi + 4 >= sizeof probes) return -1;
        if (i) probes[pi++] = ',';
        probes[pi++] = '"';
        for (const char *c = w; *c; c++) {
            if (pi + 4 >= sizeof probes) return -1;
            if (*c == '"' || *c == '\\') probes[pi++] = '\\';
            probes[pi++] = *c;
        }
        probes[pi++] = '"';
    }
    probes[pi] = '\0';
    if (nprobes) *nprobes = nw + 1;

    /* Build ONE program per version: the version's full text + a generated main
     * that prints probe_fn's result for every probe. Both outputs must be
     * byte-identical (the original is the oracle; printf side effects count). */
    static char outa[8192], outb[8192];
    int exa = 0, exb = 0;
    const char *paths[2] = { orig_path, patched_path };
    char va[64], vb[64];   /* gen278: per-process temps (parallel-safe) */
    snprintf(va, sizeof va, ".p0_verify_a_%ld.c", (long)getpid());
    snprintf(vb, sizeof vb, ".p0_verify_b_%ld.c", (long)getpid());
    const char *tmps[2]  = { va, vb };
    char *outs[2] = { outa, outb };
    int  *exs[2]  = { &exa, &exb };
    for (int v = 0; v < 2; v++) {
        static char text[262144];
        if (!code_read_file(paths[v], text, sizeof text)) return -1;
        FILE *f = fopen(tmps[v], "w");
        if (!f) return -1;
        fprintf(f, "%s\n#include <stdio.h>\n"
                   "int main(void){\n"
                   "    static const char *p0probes[] = {%s};\n"
                   "    unsigned i;\n"
                   "    for (i = 0; i < sizeof p0probes / sizeof p0probes[0]; i++)\n"
                   "        printf(\"%%u:%%d\\n\", i, %s(p0probes[i]));\n"
                   "    return 0;\n}\n",
                text, probes, probe_fn);
        fclose(f);
        int r = code_run_capture(tmps[v], NULL, outs[v], sizeof outa, exs[v],
                                 err_out, err_sz);
        remove(tmps[v]);
        if (r != 1) return -1;   /* build failure or crash/timeout: not a verdict */
    }
    return (exa == exb && strcmp(outa, outb) == 0) ? 1 : 0;
}

static void orchain_tree_rec(const char *dir, const char *fnname, int depth,
                             int *files_hit, int *chains, int *calls,
                             char *top_file, size_t top_sz, int *top_chains) {
    if (depth > 32) return;
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        const char *nm = de->d_name;
        if (nm[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", dir, nm);
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            orchain_tree_rec(path, fnname, depth + 1,
                             files_hit, chains, calls, top_file, top_sz, top_chains);
        } else {
            size_t l = strlen(nm);
            /* `||` is C-family syntax; Python's `or` chains are a later pull. */
            if (!(l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h'))) continue;
            int tc = 0;
            int n = code_find_or_chains(path, fnname, NULL, 0, &tc);
            if (n > 0) {
                (*files_hit)++; *chains += n; *calls += tc;
                if (n > *top_chains) {
                    *top_chains = n;
                    snprintf(top_file, top_sz, "%s", path);
                }
            }
        }
    }
    closedir(d);
}

int code_orchain_tree(const char *dir, const char *fnname,
                      int *files_hit, int *calls,
                      char *top_file, size_t top_sz, int *top_chains) {
    if (!dir || !*dir || !fnname || !*fnname) return -1;
    if (dir[0] == '/' || dir[0] == '~' || strstr(dir, "..")) return -1;
    int fh = 0, ch = 0, ca = 0, tc = 0;
    if (top_file && top_sz) top_file[0] = '\0';
    orchain_tree_rec(dir, fnname, 0, &fh, &ch, &ca, top_file, top_sz, &tc);
    if (files_hit) *files_hit = fh;
    if (calls) *calls = ca;
    if (top_chains) *top_chains = tc;
    return ch;
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

/* gen322: ask the real compiler about a snippet held in memory. See code.h —
 * this exists to REFUTE a syntax claim, never to make one. */
int code_syntax_ok(const char *src) {
    if (!src || !*src) return -1;

    /* A bare statement/expression fragment is not a translation unit; cc would
     * reject it for reasons that say nothing about the user's question. Only a
     * snippet that at least declares something is submitted to the oracle —
     * anything else returns -1 (unknown), never a verdict. */
    if (!strchr(src, '{')) return -1;

    char tmp[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(tmp, sizeof tmp, ".p0_syntax_%ld.c", (long)getpid());
    FILE *f = fopen(tmp, "w");
    if (!f) return -1;
    fputs(src, f);
    fputc('\n', f);
    fclose(f);

    int r = code_compile(tmp, NULL, 0);
    remove(tmp);
    return r;
}

int code_build(const char *src_path, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!src_path || !*src_path) return -1;
    if (src_path[0] == '/' || src_path[0] == '~' || strstr(src_path, "..")) return -1;
    for (const char *c = src_path; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '/' || *c == '.' ||
              *c == '_' || *c == '-')) return -1;

    char exe[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(exe, sizeof exe, ".p0_build_tmp_%ld.out", (long)getpid());
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

    char exe[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(exe, sizeof exe, "./.p0_run_tmp_%ld.out", (long)getpid());

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

/* gen263: code_run with the program's STDOUT captured (stderr discarded). Kept
 * as a sibling of code_run, not a change to it: the exit-code-only contract of
 * code_run is load-bearing for the check_sort judge (which deliberately never
 * trusts the output channel), while the differential behavior judge needs the
 * output BYTES to compare two versions. Same sandbox, timeouts and cleanup.
 * gen266: optional `stdin_data` — when non-NULL it is piped to the program's
 * stdin (an interactive game is judged by feeding it scripted input). */
int code_run_capture(const char *src_path, const char *stdin_data,
                     char *out, size_t out_sz,
                     int *exit_code, char *err_out, size_t err_sz) {
    if (out && out_sz) out[0] = '\0';
    if (err_out && err_sz) err_out[0] = '\0';
    if (exit_code) *exit_code = 0;
    if (!src_path || !*src_path || !out || out_sz == 0) return -1;
    if (src_path[0] == '/' || src_path[0] == '~' || strstr(src_path, "..")) return -1;
    for (const char *c = src_path; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '/' || *c == '.' ||
              *c == '_' || *c == '-')) return -1;

    char exe[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(exe, sizeof exe, "./.p0_runcap_tmp_%ld.out", (long)getpid());

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

    int rf[2], inf[2] = { -1, -1 };
    if (pipe(rf) != 0) { remove(exe); return -1; }
    if (stdin_data && pipe(inf) != 0) { close(rf[0]); close(rf[1]); remove(exe); return -1; }
    pid_t rpid = fork();
    if (rpid < 0) {
        close(rf[0]); close(rf[1]);
        if (stdin_data) { close(inf[0]); close(inf[1]); }
        remove(exe); return -1;
    }
    if (rpid == 0) {
        dup2(rf[1], STDOUT_FILENO);
        close(rf[0]); close(rf[1]);
        if (stdin_data) { dup2(inf[0], STDIN_FILENO); close(inf[0]); close(inf[1]); }
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) dup2(devnull, STDERR_FILENO);
        alarm(15);
        execl(exe, exe, (char *)NULL);
        _exit(127);
    }
    close(rf[1]);
    if (stdin_data) {                /* feed the script, then signal EOF */
        close(inf[0]);
        size_t sl = strlen(stdin_data), wi = 0;
        while (wi < sl) {
            ssize_t w = write(inf[1], stdin_data + wi, sl - wi);
            if (w <= 0) break;
            wi += (size_t)w;
        }
        close(inf[1]);
    }
    size_t oi = 0;
    ssize_t n;
    while (oi + 1 < out_sz && (n = read(rf[0], out + oi, out_sz - 1 - oi)) > 0)
        oi += (size_t)n;
    out[oi] = '\0';
    {   char sink[256];              /* drain so the child never blocks on write */
        while (read(rf[0], sink, sizeof sink) > 0) { }
    }
    close(rf[0]);
    int rstatus;
    if (waitpid(rpid, &rstatus, 0) < 0) { remove(exe); return -1; }
    remove(exe);
    if (WIFEXITED(rstatus)) {
        if (exit_code) *exit_code = WEXITSTATUS(rstatus);
        return 1;
    }
    return 0;                    /* killed by a signal / timed out */
}

int code_synth_game_counter(const char *token, int threshold, const char *winword,
                            char *out, size_t out_sz) {
    if (out && out_sz) out[0] = '\0';
    if (!out || out_sz == 0) return 0;
    if (threshold < 1 || threshold > 99) return 0;
    if (!orchain_ident_ok(token) || !orchain_ident_ok(winword)) return 0;
    if (strlen(token) > 32 || strlen(winword) > 32) return 0;
    /* gen269: emitted as PRETTY multi-line C (markdown-fenced in replies). */
    int n = snprintf(out, out_sz,
        "#include <stdio.h>\n"
        "#include <string.h>\n"
        "\n"
        "int main(void) {\n"
        "    char t[64];\n"
        "    const char *w = \"%s\";\n"
        "    int s = 0;\n"
        "    while (s < %d && scanf(\"%%63s\", t) == 1) {\n"
        "        if (strcmp(t, w) == 0)\n"
        "            s++;\n"
        "    }\n"
        "    if (s >= %d)\n"
        "        printf(\"%s\\n\");\n"
        "    return 0;\n"
        "}",
        token, threshold, threshold, winword);
    return (n > 0 && (size_t)n < out_sz) ? 1 : 0;
}

int code_check_counter_game(const char *src, const char *token, int threshold,
                            const char *winword, char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!src || !*src || threshold < 1 || threshold > 99) return -1;
    if (!orchain_ident_ok(token) || !orchain_ident_ok(winword)) return -1;

    char path[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(path, sizeof path, ".p0_gamecheck_%ld.c", (long)getpid());
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "%s\n", src);
    fclose(f);

    /* Scripted plays built INDEPENDENTLY from the parameters: the noise token
     * is the real token plus a suffix, so it can never count. */
    char hit[512] = "", miss[512] = "";
    size_t ho = 0, mo = 0;
    for (int i = 0; i < threshold; i++) {
        ho += (size_t)snprintf(hit + ho, sizeof hit - ho, "%sx %s ", token, token);
        if (i < threshold - 1)
            mo += (size_t)snprintf(miss + mo, sizeof miss - mo, "%sx %s ", token, token);
        if (ho >= sizeof hit - 40 || mo >= sizeof miss - 40) { remove(path); return -1; }
    }
    snprintf(hit + ho, sizeof hit - ho, "\n");
    snprintf(miss + mo, sizeof miss - mo, "\n");

    char outa[2048], outb[2048];
    int exa = 0, exb = 0;
    int ra = code_run_capture(path, hit, outa, sizeof outa, &exa, err_out, err_sz);
    int rb = code_run_capture(path, miss, outb, sizeof outb, &exb, err_out, err_sz);
    remove(path);
    if (ra != 1 || rb != 1) return -1;      /* build failure / crash / timeout */
    if (exa != 0) return 0;                  /* the winning play must end cleanly */
    if (!strstr(outa, winword)) return 0;    /* threshold hits must print the word */
    if (strstr(outb, winword)) return 0;     /* one hit short must NOT */
    return 1;
}

int code_create_empty_file(const char *name) {
    if (!name || !*name) return -1;
    size_t l = strlen(name);
    if (l > 64) return -1;
    if (name[0] == '.' || name[0] == '-') return -1;   /* no dotfiles, no flags */
    for (const char *c = name; *c; c++)                /* plain basename only */
        if (!(isalnum((unsigned char)*c) || *c == '.' || *c == '_' || *c == '-'))
            return -1;
    if (strstr(name, "..")) return -1;
    int fd = open(name, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0) return (errno == EEXIST) ? 0 : -1;
    close(fd);
    return 1;
}

static int print_msg_ok(const char *m) {
    if (!m || !*m || strlen(m) > 120) return 0;
    for (const char *c = m; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == ' ' || *c == ',' ||
              *c == '.' || *c == '!' || *c == '?' || *c == '\'' || *c == '-'))
            return 0;
    return 1;
}

int code_synth_print_program(const char *message, char *out, size_t out_sz) {
    if (out && out_sz) out[0] = '\0';
    if (!out || out_sz == 0 || !print_msg_ok(message)) return 0;
    /* gen269: emitted as PRETTY multi-line C — replies carry it in a markdown
     * fence, so clients (opencode, pi) render real indented code. */
    int n = snprintf(out, out_sz,
        "#include <stdio.h>\n"
        "\n"
        "int main(void) {\n"
        "    printf(\"%s\\n\");\n"
        "    return 0;\n"
        "}", message);
    return (n > 0 && (size_t)n < out_sz) ? 1 : 0;
}

int code_check_print_program(const char *src, const char *message,
                             char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!src || !*src || !print_msg_ok(message)) return -1;
    char path[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(path, sizeof path, ".p0_printcheck_%ld.c", (long)getpid());
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "%s\n", src);
    fclose(f);
    char got[512];
    int ex = 0;
    int r = code_run_capture(path, NULL, got, sizeof got, &ex, err_out, err_sz);
    remove(path);
    if (r != 1) return -1;
    char want[160];
    snprintf(want, sizeof want, "%s\n", message);
    return (ex == 0 && strcmp(got, want) == 0) ? 1 : 0;
}

/* gen209 (Track B/B0): see code.h. Build a complete program = the candidate function
 * + a generated main that exercises it on fixed vectors and self-checks each result,
 * write it to a sandboxed temp .c, and run it through code_run. The exit status is the
 * verdict: 0 = all cases sorted+permuted, non-zero = the index (1-based) of the first
 * failing case. We never trust the candidate's output channel — the harness recomputes
 * sortedness and the permutation relation itself. */
int code_check_sort_diag(const char *func_src, const char *fnname,
                         char *diag, size_t dsz,
                         char *err_out, size_t err_sz) {
    if (err_out && err_sz) err_out[0] = '\0';
    if (!func_src || !*func_src || !fnname || !*fnname) return -1;
    if (strlen(func_src) > 4096) return -1;
    /* fnname must be a plain C identifier (so it can't smuggle code into the call). */
    if (!(isalpha((unsigned char)fnname[0]) || fnname[0] == '_')) return -1;
    for (const char *c = fnname; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return -1;

    /* Fixed test vectors: edge cases a real sort must all satisfy. The harness keeps
     * the ORIGINAL of each, calls the candidate on a working copy, then verifies the
     * working copy is non-decreasing AND a multiset-permutation of the original. */
    static const char *const harness =
        "%s\n"  /* the candidate function, passed as an ARG (never a format string) */
        "static int __p0_sorted(const int*a,int n){\n"
        "  for(int i=1;i<n;i++) if(a[i-1]>a[i]) return 0;\n"
        "  return 1;\n}\n"
        "static int __p0_perm(const int*o,const int*b,int n){\n"
        "  int used[64]; for(int i=0;i<n;i++) used[i]=0;\n"
        "  for(int i=0;i<n;i++){int f=0; for(int j=0;j<n;j++){\n"
        "    if(!used[j]&&o[j]==b[i]){used[j]=1;f=1;break;}} if(!f) return 0;}\n"
        "  return 1;\n}\n"
        "int main(void){\n"
        "  int cases[][16]={\n"
        "    {5,4,3,2,1}, {1,2,3,4,5}, {3,1,2}, {42}, {0},\n"
        "    {2,2,1,3,1}, {-3,7,-1,0,-3,4}, {9,8,7,6,5,4,3,2,1,0}};\n"
        "  int lens[]={5,5,3,1,0,5,6,10};\n"
        "  int nc=8;\n"
        "  for(int c=0;c<nc;c++){\n"
        "    int n=lens[c]; int orig[16], work[16];\n"
        "    for(int i=0;i<n;i++){orig[i]=cases[c][i]; work[i]=cases[c][i];}\n"
        "    %s(work,n);\n"
        "    if(!__p0_sorted(work,n)) return 10+c;\n"
        "    if(!__p0_perm(orig,work,n)) return 40+c;\n"
        "  }\n"
        "  return 0;\n}\n";

    /* The harness format has exactly two %s: the candidate function body and the
     * call site. Both are arguments, so candidate code is never interpreted. */
    char full[8192];
    int m = snprintf(full, sizeof full, harness, func_src, fnname);
    if (m < 0 || (size_t)m >= sizeof full) return -1;

    char path[64];   /* gen278: per-process temp (parallel-safe) */
    snprintf(path, sizeof path, ".p0_sortcheck_%ld.c", (long)getpid());
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    if (fwrite(full, 1, (size_t)m, f) != (size_t)m) { fclose(f); remove(path); return -1; }
    fclose(f);

    int exit_code = 0;
    int r = code_run(path, &exit_code, err_out, err_sz);
    remove(path);

    /* gen327 (TODO.md P4): the verdict is STRUCTURED. "it did not sort" is not a
     * diagnosis and cannot direct a repair — the loop needs to know WHICH
     * contract broke. The harness now encodes that in its exit status:
     *   10+c  the result is not non-decreasing  -> the ORDER is wrong
     *   40+c  the result is not a permutation   -> an ELEMENT was lost or dupd
     * Two different defects with two different fixes, and the oracle is what
     * tells them apart — never a guess about the source. */
    if (diag && dsz) diag[0] = '\0';
    if (r < 0) { if (diag && dsz) snprintf(diag, dsz, "build_failed"); return -1; }
    if (r == 0) { if (diag && dsz) snprintf(diag, dsz, "crashed"); return 0; }
    if (exit_code == 0) return 1;
    if (diag && dsz) {
        if (exit_code >= 40)      snprintf(diag, dsz, "not_permutation");
        else if (exit_code >= 10) snprintf(diag, dsz, "not_ordered");
        else                      snprintf(diag, dsz, "failed");
    }
    return 0;
}

/* gen327 (TODO.md P4, forge plan §9.3): apply ONE named repair transformation to
 * a candidate's source. These are structural rewrites, not a library of answers:
 * the C knows only HOW to perform a transformation, never WHICH one to try — that
 * comes from repair_rule(Diagnosis, Fix) facts in the KB, chosen by the oracle's
 * verdict. Teaching a new diagnosis->fix mapping costs a fact.
 *
 * `blast` receives the size of the edit (characters changed), so the loop can
 * prefer the smallest change that could explain the failure — a repair that
 * rewrites half the function is a rewrite, not a repair.
 *
 * Returns 1 if the transformation applied and changed something, 0 if it does not
 * apply to this source (the candidate is then left untouched and the loop moves
 * on), -1 on a bad argument.
 */
int code_repair_apply(const char *src, const char *fix, char *out, size_t osz,
                      int *blast) {
    if (!src || !fix || !out || osz == 0) return -1;
    if (blast) *blast = 0;

    /* flip_comparator: the ORDER is wrong, so the relational operator that decides
     * a swap is backwards. Flip the first '>'/'<' that sits inside a comparison of
     * two array elements — that is the node the ordering contract depends on. */
    if (strcmp(fix, "flip_comparator") == 0) {
        const char *br = strstr(src, "if (");
        if (!br) br = strstr(src, "if(");
        if (!br) return 0;
        const char *p = br;
        while (*p && *p != '>' && *p != '<' && *p != '\n') p++;
        if (*p != '>' && *p != '<') return 0;
        if (p[1] == '=') return 0;              /* >= / <= : not this node */
        size_t at = (size_t)(p - src);
        if (at + 1 >= osz) return -1;
        snprintf(out, osz, "%s", src);
        out[at] = (*p == '>') ? '<' : '>';
        if (blast) *blast = 1;
        return 1;
    }

    /* tighten_inner_bound: an ELEMENT was lost, the signature of reading one slot
     * past the live range — `a[j+1]` with `j` allowed to reach the end. Narrow the
     * inner loop's bound by one. */
    if (strcmp(fix, "tighten_inner_bound") == 0) {
        const char *n1 = strstr(src, "- i;");
        const char *n2 = strstr(src, "- i ;");
        const char *hit = n1 ? n1 : n2;
        if (!hit) return 0;
        size_t at = (size_t)(hit - src);
        size_t keep = at + (n1 ? 4 : 5);        /* past "- i;" */
        if (keep + 8 >= osz) return -1;
        snprintf(out, osz, "%.*s", (int)at, src);
        size_t o = strlen(out);
        int w = snprintf(out + o, osz - o, "- i - 1;%s", src + keep);
        if (w < 0) return -1;
        if (blast) *blast = 4;
        return 1;
    }

    return 0;                                   /* unknown fix: apply nothing */
}

/* The gen209 signature, kept so every existing caller compiles unchanged
 * (keep-secondary): the diagnosis is optional, not a breaking change. */
int code_check_sort(const char *func_src, const char *fnname,
                    char *err_out, size_t err_sz) {
    return code_check_sort_diag(func_src, fnname, NULL, 0, err_out, err_sz);
}

/* gen209 (Track B/B2): see code.h. Emit a concrete C function from a GENERAL schema.
 * Today one schema is known — nested_loop_compare_swap — which compares each adjacent
 * pair and swaps when the comparator holds, repeated over shrinking passes. With '>'
 * the larger element bubbles up (ascending); with '<' it descends. The comparator is
 * the SINGLE parameter that distinguishes the variants, so the schema is plainly not
 * "the bubble sort printed back". */
int code_synth_from_shape(const char *shape, const char *name, char comparator,
                          char *out, size_t out_sz) {
    if (out && out_sz) out[0] = '\0';
    if (!shape || !*shape || !name || !*name || !out || out_sz == 0) return 0;
    if (comparator != '>' && comparator != '<') return 0;
    if (!(isalpha((unsigned char)name[0]) || name[0] == '_')) return 0;
    for (const char *c = name; *c; c++)
        if (!(isalnum((unsigned char)*c) || *c == '_')) return 0;

    if (strcmp(shape, "nested_loop_compare_swap") == 0) {
        int n = snprintf(out, out_sz,
            "void %s(int a[], int n) {"
            " for (int i = 0; i < n; i++)"
            " for (int j = 0; j + 1 < n - i; j++)"
            " if (a[j] %c a[j + 1]) {"
            " int t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; } }",
            name, comparator);
        if (n < 0 || (size_t)n >= out_sz) { out[0] = '\0'; return 0; }
        return 1;
    }
    return 0;                                  /* unknown schema */
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
