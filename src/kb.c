/*
 * kb.c - parrot0's logic engine (gen4-gen9).
 *
 * Grown one generation at a time into a small Prolog-like core:
 *   - ground facts + closed-world query           (gen4)
 *   - unification / variable queries (kb_match)   (gen5)
 *   - definite rules + backward-chaining (prove1) (gen6)
 *   - induction of rules from facts (kb_induce)   (gen7)
 *   - human-readable persistence + provenance     (gen9)
 * Everything is held in RAM (DESIGN.md D1); the file format is transparent
 * text so knowledge stays inspectable, diffable and hand-editable.
 */
#include "kb.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
    int    origin;
} Fact;

/* A definite rule  head(X) :- body(X)  (unary, one shared variable). */
typedef struct {
    char head[KB_TERM_LEN];
    char body[KB_TERM_LEN];
    int  origin;
} Rule;

#define KB_MAX_DEPTH 64 /* resolution recursion guard (cyclic rules) */

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

/* True if `s` fits in a term slot (non-empty, not too long). */
static int term_ok(const char *s) {
    if (!s || s[0] == '\0') return 0;
    return strlen(s) < KB_TERM_LEN;
}

/* Build a Fact from predicate + args; returns 0 on any term-too-long. */
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

int kb_assert_rule(KB *kb, const char *head, const char *body) {
    if (!kb || !term_ok(head) || !term_ok(body)) return 0;

    for (size_t i = 0; i < kb->nr; i++) { /* idempotent */
        if (strcmp(kb->rules[i].head, head) == 0 &&
            strcmp(kb->rules[i].body, body) == 0) return 1;
    }
    if (kb->nr == kb->rcap) {
        size_t cap = kb->rcap ? kb->rcap * 2 : 8;
        Rule *grown = realloc(kb->rules, cap * sizeof *grown);
        if (!grown) return 0;
        kb->rules = grown;
        kb->rcap = cap;
    }
    Rule r;
    memset(&r, 0, sizeof r);
    strcpy(r.head, head);
    strcpy(r.body, body);
    r.origin = kb->origin;
    kb->rules[kb->nr++] = r;
    return 1;
}

/* True if the unary goal pred(arg) is provable from facts + rules. */
static int prove1(KB *kb, const char *pred, const char *arg, int depth) {
    if (depth > KB_MAX_DEPTH) return 0;

    const char *a[] = {arg};
    Fact f;
    if (fact_make(&f, pred, a, 1) && kb_find(kb, &f)) return 1; /* base: fact */

    for (size_t i = 0; i < kb->nr; i++) { /* chain: head :- body */
        if (strcmp(kb->rules[i].head, pred) == 0) {
            if (prove1(kb, kb->rules[i].body, arg, depth + 1)) return 1;
        }
    }
    return 0;
}

int kb_query(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    if (argc == 1) return prove1(kb, pred, args[0], 0); /* facts + rules */

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find(kb, &f) != NULL; /* closed-world: absent => false */
}

/* Append `c` to out[] (size tracked by *count, capped at max) unless already
 * present — keeps variable-query results distinct and in first-seen order. */
static void push_unique(char out[][KB_TERM_LEN], size_t *count, size_t max,
                        const char *c) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp(out[i], c) == 0) return;
    }
    if (*count < max) strcpy(out[(*count)++], c);
}

size_t kb_match(const KB *kb, const char *pred, const char *const *args,
                size_t argc, char out[][KB_TERM_LEN], size_t max) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    int varpos = -1;
    for (size_t i = 0; i < argc; i++) {
        if (args[i] == NULL) { varpos = (int)i; break; }
    }

    /* Unary "pred(X)?": resolve over facts AND rules. Walk every constant we
     * know (in first-seen order) and keep those for which pred is provable. */
    if (argc == 1 && varpos == 0) {
        size_t count = 0;
        for (size_t i = 0; i < kb->n && count < max; i++) {
            for (size_t j = 0; j < kb->facts[i].argc; j++) {
                const char *c = kb->facts[i].args[j];
                if (prove1((KB *)kb, pred, c, 0)) {
                    push_unique(out, &count, max, c);
                }
            }
        }
        return count;
    }

    size_t count = 0;
    for (size_t i = 0; i < kb->n && count < max; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;

        int ok = 1;
        for (size_t j = 0; j < argc; j++) {
            if (args[j] == NULL) continue;            /* variable: matches */
            if (strcmp(f->args[j], args[j]) != 0) { ok = 0; break; }
        }
        if (!ok) continue;

        if (varpos >= 0) strcpy(out[count], f->args[varpos]);
        else             out[count][0] = '\0';
        count++;
    }
    return count;
}

/* True if ground fact pred(c) is directly stored (not derived). */
static int fact_present(KB *kb, const char *pred, const char *c) {
    const char *a[] = {c};
    Fact f;
    return fact_make(&f, pred, a, 1) && kb_find(kb, &f) != NULL;
}

static int rule_exists(KB *kb, const char *head, const char *body) {
    for (size_t i = 0; i < kb->nr; i++) {
        if (strcmp(kb->rules[i].head, head) == 0 &&
            strcmp(kb->rules[i].body, body) == 0) return 1;
    }
    return 0;
}

size_t kb_induce(KB *kb, size_t min_support,
                 char out_head[][KB_TERM_LEN], char out_body[][KB_TERM_LEN],
                 size_t max) {
    if (!kb) return 0;

    int saved_origin = kb->origin;
    kb->origin = KB_INDUCED; /* tag everything we induce */

    /* Distinct unary predicates, in first-seen order (deterministic). */
    char preds[256][KB_TERM_LEN];
    size_t np = 0;
    for (size_t i = 0; i < kb->n; i++) {
        if (kb->facts[i].argc == 1) {
            push_unique(preds, &np, 256, kb->facts[i].pred);
        }
    }

    size_t found = 0;
    for (size_t bi = 0; bi < np; bi++) {              /* candidate body P */
        const char *P = preds[bi];
        for (size_t hi = 0; hi < np; hi++) {          /* candidate head Q */
            if (hi == bi) continue;
            const char *Q = preds[hi];
            if (rule_exists(kb, Q, P)) continue;

            /* Does every constant with P(c) also have Q(c)? */
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

/* Parse "pred(a, b, ...)" into predicate + ground args. Returns 1 on success.
 * The shared variable in rule terms (e.g. the X in "mortal(X)") is parsed as
 * an ordinary arg; callers that only need the predicate ignore it. */
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
        if (n == 0 || s[0] == '%') continue;       /* blank / comment */
        if (s[n - 1] == '.') {                      /* drop the clause dot */
            s[--n] = '\0';
            while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
        }
        if (n == 0) continue;

        char pred[KB_TERM_LEN], pred2[KB_TERM_LEN];
        char a[KB_MAX_ARGS][KB_TERM_LEN];
        size_t ac;

        char *arrow = strstr(s, ":-");
        if (arrow) {                                /* rule: head :- body */
            *arrow = '\0';
            if (parse_term(s, pred, a, &ac) &&
                parse_term(arrow + 2, pred2, a, &ac)) {
                if (kb_assert_rule(kb, pred, pred2)) count++;
            }
        } else {                                    /* fact */
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
        fprintf(f, "%s(X) :- %s(X).\n", r->head, r->body);
        count++;
    }
    fclose(f);
    return count;
}

size_t kb_size(const KB *kb) {
    return kb ? kb->n : 0;
}