/* gen173: parrot0's in-C structural code reader. See code.h for the contract and
 * the principle (AST-as-KB, deterministic parse, straight into RAM). This is the
 * F2 (grammar/structure) faculty of docs/CODE-MASTERY.md, the one place where the
 * formal, decidable grammar of code lets primitives-first dominate outright. */
#include "code.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

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
        while (*q == ' ' || *q == '\t') q++;
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
        while (*after == ' ' || *after == '\t') after++;

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

/* --- gen175: symbolic execution of a single arithmetic return ----------- */

/* A recursive-descent evaluator over an integer expression with the function's
 * parameters bound to argument values. Grammar:
 *   expr   := term  (('+' | '-') term)*
 *   term   := factor (('*' | '/' | '%') factor)*
 *   factor := number | ident | '(' expr ')' | ('+' | '-') factor
 * `err` latches on anything unsupported (unknown ident, call, div-by-zero), so
 * the caller can refuse honestly rather than guess. */
typedef struct {
    const char *c, *end;
    char (*params)[KB_TERM_LEN];
    const long *vals;
    size_t np, nv;
    int err;
} EvalCtx;

static void ev_ws(EvalCtx *e) {
    while (e->c < e->end && (*e->c == ' ' || *e->c == '\t')) e->c++;
}

static long ev_expr(EvalCtx *e);

static long ev_factor(EvalCtx *e) {
    ev_ws(e);
    if (e->c >= e->end) { e->err = 1; return 0; }
    if (*e->c == '(') {
        e->c++;
        long v = ev_expr(e);
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
        ev_ws(e);
        /* An identifier immediately followed by '(' is a call — not supported. */
        if (e->c < e->end && *e->c == '(') { e->err = 1; return 0; }
        char w[KB_TERM_LEN];
        if (l >= sizeof w) { e->err = 1; return 0; }
        memcpy(w, s, l); w[l] = '\0';
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

static long ev_expr(EvalCtx *e) {
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

int code_eval(const char *src, const char *want,
              const long *argv, size_t argc, long *out) {
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
        while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;

        int depth = 0; const char *r = q;
        for (; *r; r++) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
        }
        if (depth != 0) return 0;
        const char *after = r;
        while (*after == ' ' || *after == '\t') after++;
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

    /* The single `return <expr>;` (whole-word "return"). */
    const char *rp = body;
    const char *ret = NULL;
    while ((rp = strstr(rp, "return")) != NULL) {
        char before = (rp == body) ? ' ' : rp[-1];
        char afterc = rp[6];
        if (!(isalnum((unsigned char)before) || before == '_') &&
            !(isalnum((unsigned char)afterc) || afterc == '_')) { ret = rp + 6; break; }
        rp += 6;
    }
    if (!ret) return 0;
    const char *semi = strchr(ret, ';');
    if (!semi) return 0;

    EvalCtx e;
    e.c = ret; e.end = semi; e.params = params; e.vals = argv;
    e.np = np; e.nv = argc; e.err = 0;
    long v = ev_expr(&e);
    ev_ws(&e);
    if (e.err || e.c != e.end) return 0;   /* leftover/unsupported => refuse */
    *out = v;
    return 1;
}
