/* gen173: parrot0's in-C structural code reader. See code.h for the contract and
 * the principle (AST-as-KB, deterministic parse, straight into RAM). This is the
 * F2 (grammar/structure) faculty of docs/CODE-MASTERY.md, the one place where the
 * formal, decidable grammar of code lets primitives-first dominate outright. */
#include "code.h"

#include <ctype.h>
#include <dirent.h>
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
            if (!eval_fn(e->src, w, args, na, &res, e->depth + 1)) { e->err = 1; return 0; }
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

int code_eval(const char *src, const char *want,
              const long *argv, size_t argc, long *out) {
    return eval_fn(src, want, argv, argc, out, 0);
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
            if (l >= 3 && nm[l-2] == '.' && (nm[l-1] == 'c' || nm[l-1] == 'h')) {
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
