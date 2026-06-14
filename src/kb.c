/*
 * kb.c - ground fact store (gen4).
 *
 * The simplest knowledge base that is still honestly a knowledge base: a flat
 * set of ground facts with assert + closed-world query. No variables, no
 * rules, no inference yet — those are later generations. The point of gen4 is
 * the substrate everything else will stand on.
 */
#include "kb.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
} Fact;

struct KB {
    Fact  *facts;
    size_t n;
    size_t cap;
};

KB *kb_create(void) {
    KB *kb = calloc(1, sizeof *kb);
    return kb; /* may be NULL; caller handles it */
}

void kb_destroy(KB *kb) {
    if (!kb) return;
    free(kb->facts);
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

int kb_query(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find(kb, &f) != NULL; /* closed-world: absent => false */
}

size_t kb_size(const KB *kb) {
    return kb ? kb->n : 0;
}