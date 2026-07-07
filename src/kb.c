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
 *   - explicit negative ground facts                (gen17)
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

/* KB_MAX_BODY is declared in kb.h (part of kb_assert_rule_n's contract). */
#define KB_MAX_GOALS 64 /* resolvent size ceiling                      */
#define KB_MAX_DEPTH 64 /* resolution recursion guard (cyclic rules)   */
#define KB_MAX_BIND  128/* bindings per substitution                   */
#define KB_PROOF_LEN 480/* max length of a rendered proof string       */
#define KB_PROOF_PG  16 /* goals tracked while building an explanation */

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
    Fact  *neg;
    size_t nn;
    size_t ncap;
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
    free(kb->neg);
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

/* A variable is marked by a leading '$' (gen280, U1b: an EXPLICIT, case-blind
 * sigil that real data never uses — see NEXT.md / teach-comprehension-via-mcp.md
 * §5.5), OR by the legacy convention (leading uppercase / '_'). Dual-accept:
 * both work, so no fixture needs migrating yet; the flip to '$'-only (which frees
 * capitalized constants like "Madrid") is a later generation. '_' stays as the
 * anonymous-variable marker. */
static int is_var(const char *s) {
    return s && (s[0] == '$' || isupper((unsigned char)s[0]) || s[0] == '_');
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

static const Fact *fact_find(const Fact *facts, size_t n, const Fact *needle) {
    for (size_t i = 0; i < n; i++) {
        if (fact_eq(&facts[i], needle)) return &facts[i];
    }
    return NULL;
}

static const Fact *kb_find(const KB *kb, const Fact *needle) {
    return fact_find(kb->facts, kb->n, needle);
}

static const Fact *kb_find_neg(const KB *kb, const Fact *needle) {
    return fact_find(kb->neg, kb->nn, needle);
}

static int fact_remove(Fact *facts, size_t *n, const Fact *needle) {
    for (size_t i = 0; i < *n; i++) {
        if (fact_eq(&facts[i], needle)) {
            memmove(&facts[i], &facts[i + 1], (*n - i - 1) * sizeof *facts);
            (*n)--;
            return 1;
        }
    }
    return 0;
}

static int fact_remove_origin(Fact *facts, size_t *n, const Fact *needle,
                              int origin) {
    for (size_t i = 0; i < *n; i++) {
        if (fact_eq(&facts[i], needle) && facts[i].origin == origin) {
            memmove(&facts[i], &facts[i + 1], (*n - i - 1) * sizeof *facts);
            (*n)--;
            return 1;
        }
    }
    return 0;
}

static int fact_append(Fact **facts, size_t *n, size_t *cap, const Fact *f) {
    if (*n == *cap) {
        size_t next = *cap ? *cap * 2 : 16;
        Fact *grown = realloc(*facts, next * sizeof *grown);
        if (!grown) return 0;
        *facts = grown;
        *cap = next;
    }
    (*facts)[(*n)++] = *f;
    return 1;
}

int kb_assert(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    fact_remove_origin(kb->neg, &kb->nn, &f, kb->origin);
    if (kb_find(kb, &f)) return 1; /* already known — idempotent */
    f.origin = kb->origin;
    return fact_append(&kb->facts, &kb->n, &kb->cap, &f);
}

int kb_retract(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return fact_remove(kb->facts, &kb->n, &f);
}

int kb_assert_neg(KB *kb, const char *pred, const char *const *args,
                  size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    fact_remove_origin(kb->facts, &kb->n, &f, kb->origin);
    if (kb_find_neg(kb, &f)) return 1; /* already known false */
    f.origin = kb->origin;
    return fact_append(&kb->neg, &kb->nn, &kb->ncap, &f);
}

int kb_is_negated(const KB *kb, const char *pred, const char *const *args,
                  size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find_neg(kb, &f) != NULL;
}

int kb_is_conflicted(const KB *kb, const char *pred,
                     const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find(kb, &f) != NULL && kb_find_neg(kb, &f) != NULL;
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
    strcpy(r.head.pred, head);  r.head.argc = 1;     strcpy(r.head.args[0], "$X");
    r.nbody = 1;
    strcpy(r.body[0].pred, body); r.body[0].argc = 1; strcpy(r.body[0].args[0], "$X");
    r.origin = kb->origin;
    return kb_add_rule(kb, &r);
}

int kb_assert_rule_n(KB *kb, const char *head,
                     const char *const *bodies, size_t nbody) {
    if (!kb || !term_ok(head) || nbody == 0 || nbody > KB_MAX_BODY) return 0;
    for (size_t i = 0; i < nbody; i++)
        if (!term_ok(bodies[i])) return 0;
    if (nbody == 1) return kb_assert_rule(kb, head, bodies[0]); /* idempotent path */

    /* idempotent: an identical conjunctive rule is not duplicated */
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->nbody != nbody || strcmp(R->head.pred, head) != 0) continue;
        int same = 1;
        for (size_t b = 0; b < nbody; b++)
            if (strcmp(R->body[b].pred, bodies[b]) != 0) { same = 0; break; }
        if (same) return 1;
    }

    Rule r;
    memset(&r, 0, sizeof r);
    strcpy(r.head.pred, head); r.head.argc = 1; strcpy(r.head.args[0], "$X");
    r.nbody = nbody;
    for (size_t b = 0; b < nbody; b++) {
        strcpy(r.body[b].pred, bodies[b]);
        r.body[b].argc = 1;
        strcpy(r.body[b].args[0], "$X");
    }
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

/* Rename a rule's variables to a unique frame (standardize-apart). Each named
 * variable becomes "name_<frame>" (shared across the clause); each anonymous
 * "_" becomes a FRESH variable (via *anon), so distinct "_" never alias. */
static void rename_term(const Term *src, int frame, int *anon, Term *dst) {
    strcpy(dst->pred, src->pred);
    dst->argc = src->argc;
    for (size_t i = 0; i < src->argc; i++) {
        const char *a = src->args[i];
        if (strcmp(a, "_") == 0)
            snprintf(dst->args[i], KB_TERM_LEN, "_A%d_%d", frame, (*anon)++);
        else if (is_var(a))
            snprintf(dst->args[i], KB_TERM_LEN, "%s_%d", a, frame);
        else
            strcpy(dst->args[i], a);
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
        int anon = 0; /* fresh-anonymous counter, shared across this clause */
        Term rhead;
        rename_term(&R->head, fr, &anon, &rhead);
        Subst s2 = *s;
        if (!unify_term_term(&s2, g, &rhead)) continue;

        Term ng[KB_MAX_GOALS];
        size_t m = 0;
        int overflow = 0;
        for (size_t b = 0; b < R->nbody; b++) {
            if (m >= KB_MAX_GOALS) { overflow = 1; break; }
            rename_term(&R->body[b], fr, &anon, &ng[m++]);
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
    if (kb_is_negated(kb, pred, args, argc)) return 0;

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
    /* Each NULL slot is a DISTINCT fresh variable; we collect the first one.
     * (Naming every NULL the same would force those slots to be equal — e.g.
     * cont(prev, ?, ?) would wrongly require word == count.) */
    int hasvar = 0;
    for (size_t i = 0; i < argc; i++) {
        if (args[i] == NULL) {
            if (!hasvar) { strcpy(g.args[i], "Q"); hasvar = 1; }
            else snprintf(g.args[i], KB_TERM_LEN, "Q%zu", i);
        }
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
 * explanation: prove a goal AND render its proof tree (gen14)
 * ------------------------------------------------------------------------- */

/* Render a goal grounded by the substitution into "pred(a, b)" form. */
static void render_goal(const Subst *s, const Term *g, char *buf, size_t sz) {
    int off = snprintf(buf, sz, "%s(", g->pred);
    for (size_t i = 0; i < g->argc && off > 0 && (size_t)off < sz; i++) {
        const char *v = resolve(s, g->args[i]);
        off += snprintf(buf + off, sz - (size_t)off, "%s%s", i ? ", " : "", v);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

/* Prove goals[idx..n-1] and, on success, render each goal's proof into
 * out[idx..n-1]. A goal proven by a fact renders as the ground goal; a goal
 * proven by a rule renders as "<goal> because <sub0> and <sub1> ...". The
 * resolvent trick (body ++ continuation) keeps backtracking correct; the first
 * `nbody` resulting proofs belong to the rule's body, the rest to the
 * continuation. */
static int prove_seq_ex(KB *kb, const Term *goals, size_t n, size_t idx,
                        const Subst *s, int depth, int *frame,
                        char out[][KB_PROOF_LEN]) {
    if (idx == n) return 1;
    if (idx >= KB_PROOF_PG) return 0;
    const Term *g = &goals[idx];

    for (size_t i = 0; i < kb->n; i++) {            /* close by a fact */
        Subst s2 = *s;
        if (unify_term_fact(&s2, g, &kb->facts[i])) {
            if (prove_seq_ex(kb, goals, n, idx + 1, &s2, depth, frame, out)) {
                render_goal(&s2, g, out[idx], KB_PROOF_LEN);
                return 1;
            }
        }
    }

    if (depth > KB_MAX_DEPTH) return 0;
    for (size_t r = 0; r < kb->nr; r++) {           /* expand a rule */
        const Rule *R = &kb->rules[r];
        if (R->head.argc != g->argc || strcmp(R->head.pred, g->pred) != 0)
            continue;
        int fr = ++(*frame), anon = 0;
        Term rhead;
        rename_term(&R->head, fr, &anon, &rhead);
        Subst s2 = *s;
        if (!unify_term_term(&s2, g, &rhead)) continue;

        Term comb[KB_PROOF_PG];
        size_t m = 0;
        int overflow = 0;
        for (size_t b = 0; b < R->nbody; b++) {
            if (m >= KB_PROOF_PG) { overflow = 1; break; }
            rename_term(&R->body[b], fr, &anon, &comb[m++]);
        }
        for (size_t k = idx + 1; k < n && !overflow; k++) {
            if (m >= KB_PROOF_PG) { overflow = 1; break; }
            comb[m++] = goals[k];
        }
        if (overflow) continue;

        char cout[KB_PROOF_PG][KB_PROOF_LEN];
        if (prove_seq_ex(kb, comb, m, 0, &s2, depth + 1, frame, cout)) {
            char head[KB_PROOF_LEN];
            render_goal(&s2, g, head, sizeof head);
            int off = snprintf(out[idx], KB_PROOF_LEN, "%s because ", head);
            for (size_t b = 0; b < R->nbody && off > 0 &&
                               (size_t)off < KB_PROOF_LEN; b++) {
                off += snprintf(out[idx] + off, KB_PROOF_LEN - (size_t)off,
                                "%s%s", b ? " and " : "", cout[b]);
            }
            for (size_t k = idx + 1; k < n; k++)
                strcpy(out[k], cout[R->nbody + (k - (idx + 1))]);
            return 1;
        }
    }
    return 0;
}

int kb_explain(KB *kb, const char *pred, const char *const *args,
               size_t argc, char *out, size_t out_size) {
    if (!kb || argc > KB_MAX_ARGS || out_size == 0) return 0;
    Term g;
    if (!term_make(&g, pred, args, argc)) return 0;

    char proofs[KB_PROOF_PG][KB_PROOF_LEN];
    Subst s = { .n = 0 };
    int frame = 0;
    if (!prove_seq_ex(kb, &g, 1, 0, &s, 0, &frame, proofs)) return 0;

    snprintf(out, out_size, "%s", proofs[0]);
    return 1;
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
        /* gen73: skip meta-knowledge predicates (lexicon, social, reflective).
         * Induction should operate on domain facts, not on curated word lists. */
        const char *mp = kb->facts[i].pred;
        if (strcmp(mp, "stopword") == 0 ||
            strcmp(mp, "question_word") == 0 ||
            strcmp(mp, "reaction_word") == 0 ||
            strcmp(mp, "social_marker") == 0 ||
            strcmp(mp, "social_pattern") == 0 ||
            strcmp(mp, "i_am") == 0 ||
            strcmp(mp, "module") == 0 ||
            strcmp(mp, "cmd") == 0 ||
            strcmp(mp, "flag") == 0 ||
            strcmp(mp, "cont") == 0 ||
            strcmp(mp, "cont2") == 0) continue;
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
        if (p < rp && *p == '"') {
            /* gen152: a quoted string literal is ONE argument — commas (and any
             * other punctuation) inside the quotes are content, not separators.
             * The stored atom keeps its quotes, so the description renderer can
             * recognise it. Without this, prose descriptions with commas were
             * shredded into garbage multi-arg facts. */
            p++;                                   /* opening quote */
            while (p < rp && *p != '"') p++;
            if (p < rp) p++;                       /* closing quote */
        } else {
            while (p < rp && *p != ',') p++;
        }
        const char *end = p;
        while (end > start && isspace((unsigned char)end[-1])) end--;
        size_t alen = (size_t)(end - start);
        if (alen == 0 || alen >= KB_TERM_LEN || *argc >= KB_MAX_ARGS) return 0;
        memcpy(args[*argc], start, alen);
        args[*argc][alen] = '\0';
        (*argc)++;
        while (p < rp && *p != ',') p++;           /* advance to the separator */
        if (p < rp && *p == ',') p++;
    }
    return *argc > 0;
}

static int parse_to_term(const char *s, Term *t) {
    return parse_term(s, t->pred, t->args, &t->argc);
}

static int parse_neg_term(const char *s, char *pred,
                          char args[][KB_TERM_LEN], size_t *argc) {
    size_t n = strlen(s);
    if (n < 7 || strncmp(s, "not(", 4) != 0 || s[n - 1] != ')') return 0;
    char inner[1024];
    size_t len = n - 5; /* strip "not(" and the final ')' */
    if (len == 0 || len >= sizeof inner) return 0;
    memcpy(inner, s + 4, len);
    inner[len] = '\0';
    return parse_term(inner, pred, args, argc);
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

    /* Resolve the directory of the current file for relative includes. */
    char dir[512] = {0};
    {   const char *slash = strrchr(path, '/');
        if (slash) {
            size_t dlen = (size_t)(slash - path);
            if (dlen < sizeof dir) { memcpy(dir, path, dlen); dir[dlen] = '\0'; }
        }
    }

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

        /* gen150: :- include(relative_path). directive.
         * Resolves the path relative to the directory of the current file,
         * then recursively loads the included file. Skip in the count (the
         * included file counts its own clauses). */
        if (strncmp(s, ":- include(", 11) == 0) {
            char inc_path[512];
            size_t plen = n;
            const char *start = s + 11;
            const char *end = s + plen - 1; /* before the trailing '.' we stripped */
            if (*end == ')') {
                size_t ilen = (size_t)(end - start);
                if (ilen > 0 && ilen < sizeof inc_path) {
                    memcpy(inc_path, start, ilen);
                    inc_path[ilen] = '\0';
                    /* trim whitespace/quotes around the path */
                    char *ip = inc_path;
                    while (*ip && isspace((unsigned char)*ip)) ip++;
                    if (*ip == '"' || *ip == '\'') { ip++; inc_path[ilen-1] = '\0'; }
                    size_t iplen = strlen(ip);
                    while (iplen > 0 && (isspace((unsigned char)ip[iplen-1]) ||
                           ip[iplen-1] == '"' || ip[iplen-1] == '\''))
                        ip[--iplen] = '\0';
                    /* resolve relative to dir */
                    char full[768];
                    if (dir[0] && ip[0] != '/')
                        snprintf(full, sizeof full, "%s/%s", dir, ip);
                    else
                        snprintf(full, sizeof full, "%s", ip);
                    kb_load(kb, full);
                }
            }
            continue;
        }

        char neg_pred[KB_TERM_LEN];
        char neg_args[KB_MAX_ARGS][KB_TERM_LEN];
        size_t neg_argc;
        if (parse_neg_term(s, neg_pred, neg_args, &neg_argc)) {
            const char *argp[KB_MAX_ARGS];
            for (size_t i = 0; i < neg_argc; i++) argp[i] = neg_args[i];
            if (kb_assert_neg(kb, neg_pred, argp, neg_argc)) count++;
            continue;
        }

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
    for (size_t i = 0; i < kb->nn; i++) {
        const Fact *fa = &kb->neg[i];
        if (!(fa->origin & origin_mask)) continue;
        fprintf(f, "not(%s(", fa->pred);
        for (size_t j = 0; j < fa->argc; j++) {
            fprintf(f, "%s%s", j ? ", " : "", fa->args[j]);
        }
        fprintf(f, ")).\n");
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


/* ----------------------------------------------------------------------------
 * direct belief reports (gen18)
 * ------------------------------------------------------------------------- */

static int fact_mentions(const Fact *f, const char *entity) {
    for (size_t i = 0; i < f->argc; i++) {
        if (strcmp(f->args[i], entity) == 0) return 1;
    }
    return 0;
}

static void render_fact_direct(const Fact *f, const char *entity, int neg,
                                char *buf, size_t sz) {
    /* gen74: i_am(X) means X asserts its own identity — render "X is X." */
    if (strcmp(f->pred, "i_am") == 0 && f->argc == 1 &&
        strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is %s", entity, entity);
        return;
    }
    if (f->argc == 1 && strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is %sa %s", entity, neg ? "not " : "", f->pred);
        return;
    }

    /* gen151/gen152: description-bearing knowledge facts whose LAST argument is a
     * quoted string — the gen150 expert/skill convention pred(key, "human text")
     * and the 3-ary pred(key, category, "human text"). Speak the description
     * instead of dumping the raw clause: math_op(addition, "combining ...") ->
     * "addition is combining ...". General over every domain's description facts,
     * so held-out concepts verbalize through the same path (no phrasebook). */
    if (f->argc >= 2 && f->args[f->argc - 1][0] == '"') {
        char d[KB_TERM_LEN];
        snprintf(d, sizeof d, "%s", f->args[f->argc - 1]);
        size_t dl = strlen(d);
        if (dl > 0 && d[dl - 1] == '"') d[--dl] = '\0';
        const char *desc = (d[0] == '"') ? d + 1 : d;
        snprintf(buf, sz, "%s%s is %s", neg ? "not " : "", f->args[0], desc);
        return;
    }

    int off = snprintf(buf, sz, "%s%s(", neg ? "not " : "", f->pred);
    for (size_t i = 0; i < f->argc && off > 0 && (size_t)off < sz; i++) {
        off += snprintf(buf + off, sz - (size_t)off, "%s%s",
                        i ? ", " : "", f->args[i]);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

static void render_conflict_direct(const Fact *f, const char *entity,
                                   char *buf, size_t sz) {
    if (f->argc == 1 && strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is conflicted about being a %s", entity, f->pred);
        return;
    }

    int off = snprintf(buf, sz, "conflict: %s(", f->pred);
    for (size_t i = 0; i < f->argc && off > 0 && (size_t)off < sz; i++) {
        off += snprintf(buf + off, sz - (size_t)off, "%s%s",
                        i ? ", " : "", f->args[i]);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

static int append_piece(char *out, size_t out_size, size_t *off,
                        const char *piece) {
    if (*off >= out_size) return 0;
    int n = snprintf(out + *off, out_size - *off, "%s%s",
                     *off ? "; " : "", piece);
    if (n < 0) return 0;
    if ((size_t)n >= out_size - *off) {
        out[out_size - 1] = '\0';
        return 0;
    }
    *off += (size_t)n;
    return 1;
}

/* Continuation transitions (cont/cont2) are generative-model machinery, not
 * world beliefs about an entity (gen41): a passage fed to `read:` populates the
 * KB with both. Introspection ("what do you know about x?") should report
 * knowledge, not the language model's internals, so these are filtered out. */
static int is_model_pred(const char *pred) {
    return strcmp(pred, "cont") == 0 || strcmp(pred, "cont2") == 0 ||
           strcmp(pred, "module") == 0 ||
           strcmp(pred, "stopword") == 0 || strcmp(pred, "question_word") == 0 ||
           strcmp(pred, "reaction_word") == 0 || strcmp(pred, "social_marker") == 0 ||
           strcmp(pred, "social_pattern") == 0 ||
           strcmp(pred, "cmd") == 0 || strcmp(pred, "flag") == 0 ||
           /* gen275: dispatch vocabulary and reply phrasings are the language
            * model's internals too — as the cue-chain migrations move trigger
            * words from C into intent_cue facts, they must not masquerade as
            * concept descriptions in kb_nearest_concept (their cue strings
            * inflated the idf pass until a real recall-by-paraphrase abstained). */
           strcmp(pred, "intent_cue") == 0 || strcmp(pred, "goal_cue") == 0 ||
           strcmp(pred, "response_template") == 0 ||
           strcmp(pred, "plan_param") == 0 || strcmp(pred, "lookup_call") == 0 ||
           strcmp(pred, "codebase_lookup") == 0 || strcmp(pred, "learnable") == 0;
}

/* gen151: structural metadata predicates — registry/relation plumbing from the
 * gen150 expert/skill/profile architecture and the coding substrate. They carry
 * no conversational content about an entity (they would render as raw clauses
 * like expert_domain(arithmetic, mathematics)), so a "what is X?" description
 * skips them and speaks only the content facts. */
static int is_struct_pred(const char *pred) {
    static const char *const s[] = {
        "expert", "expert_domain", "skill", "skill_domain",
        "profile", "profile_domain", "profile_description",
        "expert_description", "skill_description",
        "code_action", "code_template", "code_target", "code_pattern",
        "language", "keyword", "ctype", "py_builtin", "c_stdlib", "c_header",
        "compiled_language", "interpreted_language", "paradigm", "typed",
        "data_structure", "complexity", "faster_than",
        "fix", "fix_suggestion", "review_check", "review_pattern",
        "tr", "gender", "trait",
        "part_of", /* gen158: a derived relation, not a describable concept */
        "category_member", /* gen230: mod_namestart substrate, not describable */
        "opposite", /* gen231: antonym relation, queried not described */
        "color_of", /* gen231: colour facts, queried not described */
        "because", /* gen232: causal-completion reasons, queried not described */
        "grows_with", "increases", /* gen233: qualitative-change substrate */
        "capital_of_country", "kind_is", "borders", "no_land_border",
        "landmark_of", "planet_superlative", "world_superlative",
        "distance_between", /* gen240/gen251: queried world commons */
        "scene_cue", "continuation_template",
        "tr_es", "gender_es", "tr_fr", "gender_fr", "very_cold_result",
        "historical_figure", "figure_domain", "figure_reason",
        "paint_mix", /* gen239: curated world commons, not describable */
        "haiku_open", "haiku_mid", "haiku_close", /* gen240: poetic image lines */
        "couplet", /* gen240: two-line poems, queried not described */
        "quantity", /* gen240: known-fact counts, queried not described */
        "default_color", /* gen240: parrot0's offered pick, not a world fact */
        "default_pick", "landmark_city", /* gen240 */
        "magnitude", "magnitude_cue", "difference_between", "sound_of", /* gen240 */
        "language_marker", "language_name", "current_language", /* gen240 */
        "utterance", /* gen240: session conversation log */
        "artifact",
        "process_pid", "os_language", /* gen240: process/locale session context */
        "compound_word", /* gen240: compound-word riddle facts */
        "appearance", /* gen240: sensory descriptions, queried not described */
        "taste_of",
        "synonym", /* gen231/236: synonym relation, queried not described */
        "idiom_meaning", "boils_at", "freezes_at", "historical_fact", /* gen241 */
        "river_of", "ocean_west_of", "ocean_borders", "moon_of", "anagram_of", /* gen241 */
        "process_step", "process_topic", "limerick_l1", "limerick_l2", "limerick_l3", /* gen241 */
        "limerick_l4", "limerick_l5", "poem4", "completion_exact", "fill_three",
        "scenario_step", "activity_topic", "activity_step", "activity_summary", "place_for",
        "sensory_topic", "sensory_phrase", "concise_topic", "concise_explain",
        NULL,
    };
    for (size_t i = 0; s[i]; i++) if (strcmp(pred, s[i]) == 0) return 1;
    return 0;
}

int kb_describe_entity(const KB *kb, const char *entity,
                       char *out, size_t out_size) {
    if (!kb || !term_ok(entity) || !out || out_size == 0) return 0;
    out[0] = '\0';

    size_t off = 0;
    int count = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (!fact_mentions(f, entity) || is_model_pred(f->pred) ||
            is_struct_pred(f->pred)) continue;
        char piece[220];
        if (kb_find_neg(kb, f)) render_conflict_direct(f, entity, piece, sizeof piece);
        else render_fact_direct(f, entity, 0, piece, sizeof piece);
        if (!append_piece(out, out_size, &off, piece)) break;
        count++;
    }
    for (size_t i = 0; i < kb->nn; i++) {
        const Fact *f = &kb->neg[i];
        if (!fact_mentions(f, entity) || kb_find(kb, f)) continue;
        char piece[220];
        render_fact_direct(f, entity, 1, piece, sizeof piece);
        if (!append_piece(out, out_size, &off, piece)) break;
        count++;
    }
    if (count == 0) return 0;
    if (off + 1 < out_size) {
        out[off++] = '.';
        out[off] = '\0';
    } else {
        out[out_size - 1] = '\0';
    }
    return 1;
}

/* gen155: the first brick of a SIMILARITY SPACE. An LLM generalises by vector
 * proximity; in discrete C the honest analogue is structural overlap derived
 * from the KB's own descriptions — not an enumerated synonym table. Two words
 * count as similar if equal or sharing a >=4-char prefix, so the match is
 * morphology- and even COGNATE-tolerant (circonferenza ~ circumference), letting
 * recall cross EN<->IT for loanwords with no translation list. */
static int word_sim(const char *a, const char *b) {
    if (strcmp(a, b) == 0) return 1;
    size_t la = strlen(a), lb = strlen(b);
    if (la < 4 || lb < 4) return 0;          /* short words must match exactly */
    size_t m = la < lb ? la : lb, p = 0;
    while (p < m && a[p] == b[p]) p++;
    return p >= 4;
}

/* Tokenise a snake_case key OR a quoted description into lowercased content
 * tokens (>=3 chars, minus a tiny stoplist and punctuation). */
static size_t concept_tokens(const char *s, char toks[][KB_TERM_LEN], size_t max) {
    static const char *const stop[] = {"the","and","its","their","that","for",
        "with","two","one","into","from","each","than","not", NULL};
    size_t n = 0;
    char buf[KB_TERM_LEN]; snprintf(buf, sizeof buf, "%s", s);
    for (char *q = buf; *q; q++) *q = (char)tolower((unsigned char)*q);
    char *p = buf;
    while (*p && n < max) {
        while (*p && !isalpha((unsigned char)*p)) p++;
        char *start = p;
        while (*p && isalpha((unsigned char)*p)) p++;
        size_t len = (size_t)(p - start);
        if (len >= 3 && len < KB_TERM_LEN) {
            char t[KB_TERM_LEN]; memcpy(t, start, len); t[len] = '\0';
            int isstop = 0;
            for (size_t i = 0; stop[i]; i++) if (strcmp(t, stop[i]) == 0) { isstop = 1; break; }
            if (!isstop) snprintf(toks[n++], KB_TERM_LEN, "%s", t);
        }
    }
    return n;
}

int kb_nearest_concept(const KB *kb, const char *const *qwords, size_t nq,
                       char *key_out, size_t key_sz,
                       char *desc_out, size_t desc_sz) {
    if (!kb || nq == 0 || nq > 64 || !key_out || !desc_out) return 0;

    /* gen156: idf-like weighting. A learned metric is what an LLM has and parrot0
     * lacks; the cheapest honest analogue derived from the corpus (the KB
     * itself) is INVERSE DOCUMENT FREQUENCY — a query word that matches many
     * concepts ("number", "blood", "system") is uninformative and must count
     * less than a rare, discriminative one. Pass 1 measures each query word's
     * document frequency across the concept descriptions; pass 2 scores overlap
     * weighted by 1/df, so ties that plain counting could not break ("the bone
     * that protects the brain": skull vs skeletal) resolve toward the concept
     * the rarer words point at. */
    size_t df[64] = {0};
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[0], ctoks, 96);
        nc += concept_tokens(f->args[f->argc - 1], ctoks + nc, 96 - nc);
        for (size_t q = 0; q < nq; q++)
            for (size_t c = 0; c < nc; c++)
                if (word_sim(qwords[q], ctoks[c])) { df[q]++; break; }
    }
    double w[64];
    for (size_t q = 0; q < nq; q++) w[q] = 1.0 / (double)(df[q] ? df[q] : 1);

    double bestw = 0.0, secondw = 0.0;
    int bestcount = 0;
    const Fact *bestf = NULL;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[0], ctoks, 96);
        nc += concept_tokens(f->args[f->argc - 1], ctoks + nc, 96 - nc);
        double sw = 0.0; int cnt = 0;
        for (size_t q = 0; q < nq; q++)
            for (size_t c = 0; c < nc; c++)
                if (word_sim(qwords[q], ctoks[c])) { sw += w[q]; cnt++; break; }
        if (sw > bestw) { secondw = bestw; bestw = sw; bestcount = cnt; bestf = f; }
        else if (sw > secondw) secondw = sw;
    }
    /* Need real evidence (>=2 overlapping words) AND a clear WEIGHTED winner over
     * the runner-up, so one coincidental token never triggers a confident guess
     * and a genuinely symmetric tie still abstains. */
    if (bestcount >= 2 && bestf && bestw - secondw >= 0.15 * bestw) {
        snprintf(key_out, key_sz, "%s", bestf->args[0]);
        char d[KB_TERM_LEN]; snprintf(d, sizeof d, "%s", bestf->args[bestf->argc - 1]);
        size_t dl = strlen(d); if (dl && d[dl - 1] == '"') d[--dl] = '\0';
        snprintf(desc_out, desc_sz, "%s", d[0] == '"' ? d + 1 : d);
        return bestcount;
    }
    return 0;
}

/* gen157: true if `term` is the key of some description-bearing concept fact. */
int kb_is_concept_key(const KB *kb, const char *term) {
    if (!kb || !term) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], term) == 0) return 1;
    }
    return 0;
}

/* gen172: fetch the (dequoted) definition of the concept whose key is `key` and
 * whose last argument is a quoted description — learned (wiki_concept) or loaded.
 * Mirrors kb_is_concept_key. Returns 1 + the text in `out`, 0 if none. Lets a
 * re-ask of a just-learned topic answer from RAM. */
int kb_concept_def(const KB *kb, const char *key, char *out, size_t out_size) {
    if (!kb || !key || !out || out_size == 0) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], key) != 0) continue;
        snprintf(out, out_size, "%s", f->args[f->argc - 1] + 1); /* skip open quote */
        size_t l = strlen(out);
        if (l > 0 && out[l - 1] == '"') out[l - 1] = '\0';       /* drop close quote */
        return 1;
    }
    return 0;
}

/* gen157: relational reasoning DERIVED from unstructured descriptions. parrot0
 * was never told "heart is part of circulatory" — but the circulatory
 * description NAMES the heart, so the containment relation can be recovered from
 * the text. Find the concept whose description mentions `term` (a different
 * concept), recovering an emergent taxonomy that was never asserted as facts. */
static const char *concept_pred(const KB *kb, const char *key);
static int valid_member(const KB *kb, const char *mem, const char *contpred);

int kb_concept_mentioning(const KB *kb, const char *term,
                          char *key_out, size_t key_sz,
                          char *desc_out, size_t desc_sz) {
    if (!kb || !term || !key_out || !desc_out) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], term) == 0) continue;   /* a concept never contains itself */
        if (!valid_member(kb, term, f->pred)) continue; /* gen159: sibling, not a part */
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        for (size_t c = 0; c < nc; c++) {
            /* EXACT mention (gen158): a containment claim must not rest on a
             * cognate near-match (respiratory ~ responses). */
            if (strcmp(term, ctoks[c]) == 0) {
                snprintf(key_out, key_sz, "%s", f->args[0]);
                char d[KB_TERM_LEN]; snprintf(d, sizeof d, "%s", f->args[f->argc - 1]);
                size_t dl = strlen(d); if (dl && d[dl - 1] == '"') d[--dl] = '\0';
                snprintf(desc_out, desc_sz, "%s", d[0] == '"' ? d + 1 : d);
                return 1;
            }
        }
    }
    return 0;
}

/* gen158: MATERIALIZE the emergent containment relation as first-class part_of/2
 * facts so the resolution engine can PROVE and query it ("is the heart part of
 * circulatory?" -> Yes; "what is part of circulatory?" -> heart). The danger is
 * that a mention is not a containment ("composite is a number that is not
 * prime"), so we only materialize from CONTAINER predicates — detected
 * structurally as predicates whose facts repeatedly name OTHER concept keys
 * (body_system names organs; number_property names almost none). No hardcoded
 * domain list; the facts are KB_REFLECTIVE so they regenerate each boot and are
 * never persisted. Returns the number of relations added. */
static int str_in(const char set[][KB_TERM_LEN], size_t n, const char *s) {
    for (size_t i = 0; i < n; i++) if (strcmp(set[i], s) == 0) return 1;
    return 0;
}

/* gen159: the predicate ("taxonomic level") of the concept keyed by `key`. */
static const char *concept_pred(const KB *kb, const char *key) {
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], key) == 0) return f->pred;
    }
    return NULL;
}

/* gen159: a valid containment crosses TWO taxonomic levels (an organ is part of
 * a system); two concepts of the SAME predicate are siblings, never nested. This
 * type gate kills the text-ambiguity false positive where a system name appears
 * as an ADJECTIVE in a sibling's description ("skeletal ... muscles" in the
 * muscular system). `mem` is a concept key, `contpred` the container's predicate. */
static int valid_member(const KB *kb, const char *mem, const char *contpred) {
    const char *mp = concept_pred(kb, mem);
    return mp && strcmp(mp, contpred) != 0;
}

int kb_derive_part_of(KB *kb) {
    if (!kb) return 0;
    const size_t n0 = kb->n;

    /* collect the concept keys once (exact membership, not cognate) */
    char keys[512][KB_TERM_LEN]; size_t nkeys = 0;
    for (size_t i = 0; i < n0 && nkeys < 512; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        int dup = 0;
        for (size_t k = 0; k < nkeys; k++) if (strcmp(keys[k], f->args[0]) == 0) { dup = 1; break; }
        if (!dup) snprintf(keys[nkeys++], KB_TERM_LEN, "%s", f->args[0]);
    }

    /* per-predicate count of facts that name some OTHER concept key */
    char preds[128][KB_TERM_LEN]; int pcnt[128]; size_t npreds = 0;
    for (size_t i = 0; i < n0; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        int names_other = 0;
        for (size_t c = 0; c < nc; c++)
            if (strcmp(ctoks[c], f->args[0]) != 0 &&
                str_in((const char (*)[KB_TERM_LEN])keys, nkeys, ctoks[c]) &&
                valid_member(kb, ctoks[c], f->pred)) { names_other = 1; break; }
        if (!names_other) continue;
        size_t p = 0; for (; p < npreds; p++) if (strcmp(preds[p], f->pred) == 0) break;
        if (p == npreds && npreds < 128) { snprintf(preds[npreds], KB_TERM_LEN, "%s", f->pred); pcnt[npreds] = 0; npreds++; }
        if (p < 128) pcnt[p]++;
    }

    /* materialize part_of for facts of container predicates (>=2 such facts) */
    int saved = kb->origin;
    kb_set_origin(kb, KB_REFLECTIVE);
    int added = 0;
    for (size_t i = 0; i < n0; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(f->pred) || is_struct_pred(f->pred)) continue;
        int container = 0;
        for (size_t p = 0; p < npreds; p++)
            if (strcmp(preds[p], f->pred) == 0) { container = (pcnt[p] >= 2); break; }
        if (!container) continue;
        /* Copy the container key and predicate BEFORE asserting: kb_assert may
         * realloc kb->facts and leave `f` (a pointer into it) dangling, which was
         * a heap-use-after-free once the KB was large enough to move on growth. */
        char key[KB_TERM_LEN], pred[KB_TERM_LEN];
        snprintf(key, sizeof key, "%s", f->args[0]);
        snprintf(pred, sizeof pred, "%s", f->pred);
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        for (size_t c = 0; c < nc; c++) {
            if (strcmp(ctoks[c], key) == 0 ||
                !str_in((const char (*)[KB_TERM_LEN])keys, nkeys, ctoks[c])) continue;
            if (!valid_member(kb, ctoks[c], pred)) continue; /* sibling, not a part */
            const char *args[2] = { ctoks[c], key };
            if (kb_assert(kb, "part_of", args, 2)) added++;
        }
    }
    kb_set_origin(kb, saved);
    return added;
}

int kb_knows_pred(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    for (size_t i = 0; i < kb->n; i++)
        if (strcmp(kb->facts[i].pred, pred) == 0) return 1;
    for (size_t i = 0; i < kb->nn; i++)
        if (strcmp(kb->neg[i].pred, pred) == 0) return 1;
    for (size_t i = 0; i < kb->nr; i++)
        if (strcmp(kb->rules[i].head.pred, pred) == 0) return 1;
    return 0;
}

int kb_rule_body_mentions(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    for (size_t r = 0; r < kb->nr; r++)
        for (size_t b = 0; b < kb->rules[r].nbody; b++)
            if (strcmp(kb->rules[r].body[b].pred, pred) == 0) return 1;
    return 0;
}

size_t kb_unary_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    if (!kb) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++)
        if (kb->facts[i].argc == 1) push_unique(out, &n, max, kb->facts[i].pred);
    for (size_t i = 0; i < kb->nr; i++)
        if (kb->rules[i].head.argc == 1)
            push_unique(out, &n, max, kb->rules[i].head.pred);
    return n;
}

size_t kb_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    if (!kb) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++)
        push_unique(out, &n, max, kb->facts[i].pred);
    for (size_t i = 0; i < kb->nr; i++)
        push_unique(out, &n, max, kb->rules[i].head.pred);
    return n;
}

int kb_dump_all(const KB *kb, char *out, size_t out_size) {
    if (!kb || !out || out_size == 0) return 0;
    size_t off = 0;
    size_t written = 0;
    for (size_t i = 0; i < kb->n && off + 1 < out_size; i++) {
        Fact *f = &kb->facts[i];
        off += (size_t)snprintf(out + off, out_size - off, "%s(", f->pred);
        for (size_t j = 0; j < f->argc; j++)
            off += (size_t)snprintf(out + off, out_size - off, "%s%s",
                                     j ? ", " : "", f->args[j]);
        off += (size_t)snprintf(out + off, out_size - off, "). ");
        written++;
    }
    if (off > 0 && off + 1 < out_size) {
        if (out[off - 1] == ' ') out[--off] = '\0';
    }
    if (written == 0) out[0] = '\0';
    return written > 0;
}

size_t kb_size(const KB *kb) {
    return kb ? kb->n : 0;
}

size_t kb_pred_fact_count(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    size_t count = 0;
    for (size_t i = 0; i < kb->n; i++)
        if (strcmp(kb->facts[i].pred, pred) == 0) count++;
    return count;
}

size_t kb_rule_body_preds(const KB *kb, const char *head, size_t argc,
                          char out_body[][KB_TERM_LEN], size_t max) {
    if (!kb || !head || max == 0) return 0;
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->head.argc != argc || strcmp(R->head.pred, head) != 0) continue;
        size_t n = 0;
        for (size_t b = 0; b < R->nbody && n < max; b++)
            snprintf(out_body[n++], KB_TERM_LEN, "%s", R->body[b].pred);
        return n;
    }
    return 0;
}

size_t kb_rules_for_head(const KB *kb, const char *head, size_t argc) {
    if (!kb || !head) return 0;
    size_t count = 0;
    for (size_t r = 0; r < kb->nr; r++)
        if (kb->rules[r].head.argc == argc &&
            strcmp(kb->rules[r].head.pred, head) == 0) count++;
    return count;
}

size_t kb_nth_rule_body_preds(const KB *kb, const char *head, size_t argc,
                              size_t idx, char out_body[][KB_TERM_LEN], size_t max) {
    if (!kb || !head || max == 0) return 0;
    size_t seen = 0;
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->head.argc != argc || strcmp(R->head.pred, head) != 0) continue;
        if (seen++ != idx) continue;
        size_t n = 0;
        for (size_t b = 0; b < R->nbody && n < max; b++)
            snprintf(out_body[n++], KB_TERM_LEN, "%s", R->body[b].pred);
        return n;
    }
    return 0;
}
