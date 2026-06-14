/*
 * kb.c - parrot0's logic engine (gen4-gen11).
 *
 * Grown one generation at a time into a small Prolog-like core:
 *   - ground facts + closed-world query            (gen4)
 *   - unification / variable queries (kb_match)    (gen5)
 *   - definite rules + backward chaining           (gen6)
 *   - induction of rules from facts (kb_induce)    (gen7)
 *   - human-readable persistence + provenance      (gen9)
 *   - retraction / correction (kb_retract)         (gen10)
 *   - n-ary relations + multi-goal rules + general SLD resolution with
 *     backtracking and standardize-apart           (gen11)
 * Everything is held in RAM (DESIGN.md D1); the file format is transparent
 * text so knowledge stays inspectable, diffable and hand-editable.
 */
#include "kb.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KB_MAX_BODY  8  /* goals per rule body                         */
#define KB_MAX_GOALS 64 /* resolvent size ceiling                      */
#define KB_MAX_DEPTH 64 /* resolution recursion guard (cyclic rules)   */
#define KB_MAX_BIND  128/* bindings per substitution                   */

/* A term: predicate + args. An arg is a constant (lowercase atom) or a
 * variable (starts uppercase or '_'). Facts are ground terms; rule heads and
 * body goals may contain variables. */
typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
} Term;

typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
    int    origin;
} Fact;

/* A definite rule  head :- body[0], body[1], ...  (nbody >= 1). */
typedef struct {
    Term   head;
    Term   body[KB_MAX_BODY];
    size_t nbody;
    int    origin;
} Rule;

struct KB {
    Fact  *facts;
    size_t n;
    size_t cap;
    Rule  *rules;
    size_t nr;
    size_t rcap;
    int    origin; /* provenance tag for newly asserted clauses */
};

KB *kb_create(void) {
    KB *kb = calloc(1, sizeof *kb);
    if (kb) kb->origin = KB_SESSION; /* default provenance */
    return kb; /* may be NULL; caller handles it */
}

void kb_set_origin(KB *kb, int origin) {
    if (kb) kb->origin = origin;
}

void kb_destroy(KB *kb) {
    if (!kb) return;
    free(kb->facts);
    free(kb->rules);
    free(kb);
}

/* ----------------------------------------------------------------------------
 * terms, facts
 * ------------------------------------------------------------------------- */

static int term_ok(const char *s) {
    if (!s || s[0] == '\0') return 0;
    return strlen(s) < KB_TERM_LEN;
}

static int is_var(const char *s) {
    return s && (isupper((unsigned char)s[0]) || s[0] == '_');
}

static int fact_make(Fact *f, const char *pred, const char *const *args,
                     size_t argc) {
    if (!term_ok(pred)) return 0;
    memset(f, 0, sizeof *f);
    strcpy(f->pred, pred);
    f->argc = argc;
    for (size_t i = 0; i < argc; i++) {
        if (!term_ok(args[i])) return 0;
        strcpy(f->args[i], args[i]);
    }
    return 1;
}

static int fact_eq(const Fact *a, const Fact *b) {
    if (a->argc != b->argc) return 0;
    if (strcmp(a->pred, b->pred) != 0) return 0;
    for (size_t i = 0; i < a->argc; i++) {
        if (strcmp(a->args[i], b->args[i]) != 0) return 0;
    }
    return 1;
}

static const Fact *kb_find(const KB *kb, const Fact *needle) {
    for (size_t i = 0; i < kb->n; i++) {
        if (fact_eq(&kb->facts[i], needle)) return &kb->facts[i];
    }
    return NULL;
}

int kb_assert(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    if (kb_find(kb, &f)) return 1; /* already known — idempotent */
    f.origin = kb->origin;

    if (kb->n == kb->cap) {
        size_t cap = kb->cap ? kb->cap * 2 : 16;
        Fact *grown = realloc(kb->facts, cap * sizeof *grown);
        if (!grown) return 0;
        kb->facts = grown;
        kb->cap = cap;
    }
    kb->facts[kb->n++] = f;
    return 1;
}

int kb_retract(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    for (size_t i = 0; i < kb->n; i++) {
        if (fact_eq(&kb->facts[i], &f)) {
            memmove(&kb->facts[i], &kb->facts[i + 1],
                    (kb->n - i - 1) * sizeof *kb->facts);
            kb->n--;
            return 1;
        }
    }
    return 0;
}

/* ----------------------------------------------------------------------------
 * rules
 * ------------------------------------------------------------------------- */

static int kb_add_rule(KB *kb, const Rule *r) {
    if (kb->nr == kb->rcap) {
        size_t cap = kb->rcap ? kb->rcap * 2 : 8;
        Rule *grown = realloc(kb->rules, cap * sizeof *grown);
        if (!grown) return 0;
        kb->rules = grown;
        kb->rcap = cap;
    }
    kb->rules[kb->nr++] = *r;
    return 1;
}

/* True if the simple unary rule  head(X) :- body(X)  already exists. */
static int rule_exists(KB *kb, const char *head, const char *body) {
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (r->nbody == 1 && r->head.argc == 1 && r->body[0].argc == 1 &&
            strcmp(r->head.pred, head) == 0 &&
            strcmp(r->body[0].pred, body) == 0) return 1;
    }
    return 0;
}

int kb_assert_rule(KB *kb, const char *head, const char *body) {
    if (!kb || !term_ok(head) || !term_ok(body)) return 0;
    if (rule_exists(kb, head, body)) return 1; /* idempotent */

    Rule r;
    memset(&r, 0, sizeof r);
    strcpy(r.head.pred, head);  r.head.argc = 1;     strcpy(r.head.args[0], "X");
    r.nbody = 1;
    strcpy(r.body[0].pred, body); r.body[0].argc = 1; strcpy(r.body[0].args[0], "X");
    r.origin = kb->origin;
    return kb_add_rule(kb, &r);
}

/* ----------------------------------------------------------------------------
 * unification + SLD resolution with backtracking (gen11)
 * ------------------------------------------------------------------------- */

typedef struct { char var[KB_TERM_LEN]; char val[KB_TERM_LEN]; } Bind;
typedef struct { Bind b[KB_MAX_BIND]; size_t n; } Subst;

/* Follow the binding chain to a constant or an unbound variable. */
static const char *resolve(const Subst *s, const char *name) {
    while (is_var(name)) {
        const char *next = NULL;
        for (size_t i = 0; i < s->n; i++) {
            if (strcmp(s->b[i].var, name) == 0) { next = s->b[i].val; break; }
        }
        if (!next) break;
        name = next;
    }
    return name;
}

static int bind_add(Subst *s, const char *var, const char *val) {
    if (s->n >= KB_MAX_BIND) return 0;
    strcpy(s->b[s->n].var, var);
    strcpy(s->b[s->n].val, val);
    s->n++;
    return 1;
}

static int unify(Subst *s, const char *a, const char *b) {
    const char *ra = resolve(s, a);
    const char *rb = resolve(s, b);
    int va = is_var(ra), vb = is_var(rb);
    if (va && vb) return strcmp(ra, rb) == 0 ? 1 : bind_add(s, ra, rb);
    if (va) return bind_add(s, ra, rb);
    if (vb) return bind_add(s, rb, ra);
    return strcmp(ra, rb) == 0;
}

static int unify_term_fact(Subst *s, const Term *g, const Fact *f) {
    if (g->argc != f->argc || strcmp(g->pred, f->pred) != 0) return 0;
    for (size_t i = 0; i < g->argc; i++) {
        if (!unify(s, g->args[i], f->args[i])) return 0;
    }
    return 1;
}

static int unify_term_term(Subst *s, const Term *g, const Term *h) {
    if (g->argc != h->argc || strcmp(g->pred, h->pred) != 0) return 0;
    for (size_t i = 0; i < g->argc; i++) {
        if (!unify(s, g->args[i], h->args[i])) return 0;
    }
    return 1;
}

/* Rename a rule's variables to a unique frame (standardize-apart). */
static void rename_term(const Term *src, int frame, Term *dst) {
    strcpy(dst->pred, src->pred);
    dst->argc = src->argc;
    for (size_t i = 0; i < src->argc; i++) {
        if (is_var(src->args[i]))
            snprintf(dst->args[i], KB_TERM_LEN, "%s_%d", src->args[i], frame);
        else
            strcpy(dst->args[i], src->args[i]);
    }
}

static void push_unique(char out[][KB_TERM_LEN], size_t *count, size_t max,
                        const char *c) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp(out[i], c) == 0) return;
    }
    if (*count < max) strcpy(out[(*count)++], c);
}

typedef struct {
    const KB *kb;
    const char *qvar;          /* variable to collect, or NULL for boolean */
    char (*out)[KB_TERM_LEN];
    size_t max;
    size_t count;
    int    found;
    int    frame;
} Solver;

/* Prove the goal list under substitution `s`. Returns 1 to stop all search
 * (boolean solution found, or the collector is full). */
static int solve(Solver *S, const Term *goals, size_t ngoals, size_t idx,
                 const Subst *s, int depth) {
    if (idx == ngoals) {                       /* a complete solution */
        if (S->qvar == NULL) { S->found = 1; return 1; }
        const char *v = resolve(s, S->qvar);
        if (!is_var(v)) push_unique(S->out, &S->count, S->max, v);
        return S->count >= S->max;
    }
    if (depth > KB_MAX_DEPTH) return 0;

    const Term *g = &goals[idx];

    for (size_t i = 0; i < S->kb->n; i++) {    /* match facts */
        Subst s2 = *s;
        if (unify_term_fact(&s2, g, &S->kb->facts[i])) {
            if (solve(S, goals, ngoals, idx + 1, &s2, depth)) return 1;
        }
    }

    for (size_t r = 0; r < S->kb->nr; r++) {   /* expand rules */
        const Rule *R = &S->kb->rules[r];
        if (R->head.argc != g->argc || strcmp(R->head.pred, g->pred) != 0)
            continue;

        int fr = ++S->frame;
        Term rhead;
        rename_term(&R->head, fr, &rhead);
        Subst s2 = *s;
        if (!unify_term_term(&s2, g, &rhead)) continue;

        Term ng[KB_MAX_GOALS];
        size_t m = 0;
        int overflow = 0;
        for (size_t b = 0; b < R->nbody; b++) {
            if (m >= KB_MAX_GOALS) { overflow = 1; break; }
            rename_term(&R->body[b], fr, &ng[m++]);
        }
        for (size_t k = idx + 1; k < ngoals && !overflow; k++) {
            if (m >= KB_MAX_GOALS) { overflow = 1; break; }
            ng[m++] = goals[k];
        }
        if (overflow) continue;
        if (solve(S, ng, m, 0, &s2, depth + 1)) return 1;
    }
    return 0;
}

static int term_make(Term *t, const char *pred, const char *const *args,
                     size_t argc) {
    if (!term_ok(pred)) return 0;
    memset(t, 0, sizeof *t);
    strcpy(t->pred, pred);
    t->argc = argc;
    for (size_t i = 0; i < argc; i++) {
        if (!term_ok(args[i])) return 0;
        strcpy(t->args[i], args[i]);
    }
    return 1;
}

int kb_query(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Term g;
    if (!term_make(&g, pred, args, argc)) return 0;

    Solver S;
    memset(&S, 0, sizeof S);
    S.kb = kb;
    Subst s = { .n = 0 };
    solve(&S, &g, 1, 0, &s, 0);
    return S.found;
}

size_t kb_match(const KB *kb, const char *pred, const char *const *args,
                size_t argc, char out[][KB_TERM_LEN], size_t max) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Term g;
    memset(&g, 0, sizeof g);
    if (!term_ok(pred)) return 0;
    strcpy(g.pred, pred);
    g.argc = argc;
    int hasvar = 0;
    for (size_t i = 0; i < argc; i++) {
        if (args[i] == NULL) { strcpy(g.args[i], "Q"); hasvar = 1; }
        else { if (!term_ok(args[i])) return 0; strcpy(g.args[i], args[i]); }
    }

    Solver S;
    memset(&S, 0, sizeof S);
    S.kb = kb;
    S.qvar = hasvar ? "Q" : NULL;
    S.out = out;
    S.max = max;
    Subst s = { .n = 0 };
    solve(&S, &g, 1, 0, &s, 0);
    return S.count;
}

/* ----------------------------------------------------------------------------
 * induction ("training"): learn unary rules from facts
 * ------------------------------------------------------------------------- */

static int fact_present(KB *kb, const char *pred, const char *c) {
    const char *a[] = {c};
    Fact f;
    return fact_make(&f, pred, a, 1) && kb_find(kb, &f) != NULL;
}

size_t kb_induce(KB *kb, size_t min_support,
                 char out_head[][KB_TERM_LEN], char out_body[][KB_TERM_LEN],
                 size_t max) {
    if (!kb) return 0;

    int saved_origin = kb->origin;
    kb->origin = KB_INDUCED; /* tag everything we induce */

    char preds[256][KB_TERM_LEN];
    size_t np = 0;
    for (size_t i = 0; i < kb->n; i++) {
        if (kb->facts[i].argc == 1) push_unique(preds, &np, 256, kb->facts[i].pred);
    }

    size_t found = 0;
    for (size_t bi = 0; bi < np; bi++) {
        const char *P = preds[bi];
        for (size_t hi = 0; hi < np; hi++) {
            if (hi == bi) continue;
            const char *Q = preds[hi];
            if (rule_exists(kb, Q, P)) continue;

            size_t support = 0;
            int all_q = 1;
            for (size_t i = 0; i < kb->n && all_q; i++) {
                const Fact *f = &kb->facts[i];
                if (f->argc != 1 || strcmp(f->pred, P) != 0) continue;
                support++;
                if (!fact_present(kb, Q, f->args[0])) all_q = 0;
            }

            if (all_q && support >= min_support) {
                kb_assert_rule(kb, Q, P);
                if (found < max) {
                    strcpy(out_head[found], Q);
                    strcpy(out_body[found], P);
                }
                found++;
            }
        }
    }

    kb->origin = saved_origin;
    return found;
}

/* ----------------------------------------------------------------------------
 * persistence: human-readable, Prolog-like text (DESIGN.md D1-D3)
 * ------------------------------------------------------------------------- */

static int parse_term(const char *s, char *pred,
                      char args[][KB_TERM_LEN], size_t *argc) {
    while (*s && isspace((unsigned char)*s)) s++;
    const char *lp = strchr(s, '(');
    const char *rp = lp ? strrchr(lp, ')') : NULL;
    if (!lp || !rp || rp < lp) return 0;

    size_t plen = (size_t)(lp - s);
    while (plen > 0 && isspace((unsigned char)s[plen - 1])) plen--;
    if (plen == 0 || plen >= KB_TERM_LEN) return 0;
    memcpy(pred, s, plen);
    pred[plen] = '\0';

    *argc = 0;
    const char *p = lp + 1;
    while (p < rp) {
        while (p < rp && isspace((unsigned char)*p)) p++;
        const char *start = p;
        while (p < rp && *p != ',') p++;
        const char *end = p;
        while (end > start && isspace((unsigned char)end[-1])) end--;
        size_t alen = (size_t)(end - start);
        if (alen == 0 || alen >= KB_TERM_LEN || *argc >= KB_MAX_ARGS) return 0;
        memcpy(args[*argc], start, alen);
        args[*argc][alen] = '\0';
        (*argc)++;
        if (p < rp && *p == ',') p++;
    }
    return *argc > 0;
}

static int parse_to_term(const char *s, Term *t) {
    return parse_term(s, t->pred, t->args, &t->argc);
}

/* Split a rule body into goal strings on top-level (depth-0) commas. The
 * input buffer is modified in place. */
static size_t split_goals(char *body, char *goals[], size_t max) {
    size_t n = 0;
    int depth = 0;
    char *start = body;
    for (char *p = body;; p++) {
        if (*p == '(') depth++;
        else if (*p == ')') depth--;
        if ((*p == ',' && depth == 0) || *p == '\0') {
            char saved = *p;
            *p = '\0';
            if (n < max) goals[n++] = start;
            start = p + 1;
            if (saved == '\0') break;
        }
    }
    return n;
}

int kb_load(KB *kb, const char *path) {
    if (!kb || !path || !*path) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0; /* missing file: a no-op */

    char line[1024];
    int count = 0;
    while (fgets(line, sizeof line, f)) {
        char *s = line;
        while (*s && isspace((unsigned char)*s)) s++;
        size_t n = strlen(s);
        while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
        if (n == 0 || s[0] == '%') continue;
        if (s[n - 1] == '.') {
            s[--n] = '\0';
            while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
        }
        if (n == 0) continue;

        char *arrow = strstr(s, ":-");
        if (arrow) {                                /* rule: head :- goals */
            *arrow = '\0';
            Rule r;
            memset(&r, 0, sizeof r);
            r.origin = kb->origin;
            if (!parse_to_term(s, &r.head)) continue;

            char *goals[KB_MAX_BODY];
            size_t ng = split_goals(arrow + 2, goals, KB_MAX_BODY);
            int ok = ng > 0;
            for (size_t i = 0; i < ng && ok; i++) {
                if (!parse_to_term(goals[i], &r.body[i])) ok = 0;
            }
            if (ok) { r.nbody = ng; kb_add_rule(kb, &r); count++; }
        } else {                                    /* fact */
            char pred[KB_TERM_LEN];
            char a[KB_MAX_ARGS][KB_TERM_LEN];
            size_t ac;
            if (parse_term(s, pred, a, &ac)) {
                const char *argp[KB_MAX_ARGS];
                for (size_t i = 0; i < ac; i++) argp[i] = a[i];
                if (kb_assert(kb, pred, argp, ac)) count++;
            }
        }
    }
    fclose(f);
    return count;
}

static void write_term(FILE *f, const Term *t) {
    fprintf(f, "%s(", t->pred);
    for (size_t i = 0; i < t->argc; i++) {
        fprintf(f, "%s%s", i ? ", " : "", t->args[i]);
    }
    fputc(')', f);
}

int kb_save(const KB *kb, const char *path, int origin_mask) {
    if (!kb || !path || !*path) return -1;
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    int count = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *fa = &kb->facts[i];
        if (!(fa->origin & origin_mask)) continue;
        fprintf(f, "%s(", fa->pred);
        for (size_t j = 0; j < fa->argc; j++) {
            fprintf(f, "%s%s", j ? ", " : "", fa->args[j]);
        }
        fprintf(f, ").\n");
        count++;
    }
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (!(r->origin & origin_mask)) continue;
        write_term(f, &r->head);
        fprintf(f, " :- ");
        for (size_t j = 0; j < r->nbody; j++) {
            if (j) fprintf(f, ", ");
            write_term(f, &r->body[j]);
        }
        fprintf(f, ".\n");
        count++;
    }
    fclose(f);
    return count;
}

size_t kb_size(const KB *kb) {
    return kb ? kb->n : 0;
}
