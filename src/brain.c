/*
 * brain.c - an articulated brain (gen2+), now with session memory (gen3).
 *
 * Up to gen1 the brain was a flat chain of `if`s. Per PRINCIPLES.md, reasoning
 * structure is articulated, not a uniform soup — so the brain is a *registry
 * of cooperating modules*. Each module inspects a turn and either claims it
 * (produces a response) or declines (passes it on). If nobody claims it,
 * parrot0 falls back to its oldest instinct: echo.
 *
 * Modules may be stateless (greet, farewell) or stateful (memory carries the
 * user's name across turns via `Brain`). New parts (and parts-of-parts) are
 * meant to EMERGE here as future tasks demand them — we do not pre-design a
 * taxonomy.
 */
#include "brain.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Brain {
    unsigned long turns;   /* how many exchanges we've had this session */
    char name[64];         /* the user's name, once they tell us */
    int  has_name;         /* whether `name` is set */
};

/* ----------------------------------------------------------------------------
 * small text utilities shared by modules
 * ------------------------------------------------------------------------- */

/* Copy `in` into `out` lowercased and with surrounding blanks trimmed, so
 * modules can match on intent without caring about case or stray spaces. */
static void normalize(const char *in, char *out, size_t out_size) {
    if (out_size == 0) return;
    while (*in && isspace((unsigned char)*in)) in++;       /* skip leading */
    size_t n = 0;
    for (; in[n] && n + 1 < out_size; n++) {
        out[n] = (char)tolower((unsigned char)in[n]);
    }
    while (n > 0 && isspace((unsigned char)out[n - 1])) n--; /* trim trailing */
    out[n] = '\0';
}

/* True if `s` equals any of the NULL-terminated list of words. */
static int matches_any(const char *s, const char *const *words) {
    for (; *words; words++) {
        if (strcmp(s, *words) == 0) return 1;
    }
    return 0;
}

/* Return a pointer past leading whitespace in `s`. */
static const char *skip_ws(const char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

/* Copy `src` into `dst` (size `dst_size`), trimming trailing whitespace. */
static void copy_trim(char *dst, size_t dst_size, const char *src) {
    if (dst_size == 0) return;
    size_t n = 0;
    for (; src[n] && n + 1 < dst_size; n++) dst[n] = src[n];
    while (n > 0 && isspace((unsigned char)dst[n - 1])) n--;
    dst[n] = '\0';
}

/* Write a fixed reply into out, returning its length. */
static size_t put(const char *text, char *out, size_t out_size) {
    size_t n = strlen(text);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, text, n);
    out[n] = '\0';
    return n;
}

/* ----------------------------------------------------------------------------
 * module protocol
 *
 * A module looks at one turn and returns 1 if it claims it (having written a
 * response into out), or 0 to decline and pass the turn to the next module.
 * `norm` is the normalized input; `raw` is the original; `b` is brain state.
 * ------------------------------------------------------------------------- */

typedef int (*Handler)(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size);

typedef struct {
    const char *name;
    Handler     handle;
} Module;

/* --- module: greeting ---------------------------------------------------- */
static int mod_greet(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)b; (void)raw;
    static const char *const greetings[] = {"hello", "hi", "hey", NULL};
    if (!matches_any(norm, greetings)) return 0;
    put("Hi there!", out, out_size);
    return 1;
}

/* --- module: farewell ---------------------------------------------------- */
static int mod_farewell(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)b; (void)raw;
    static const char *const farewells[] = {"bye", "goodbye", NULL};
    if (!matches_any(norm, farewells)) return 0;
    put("Goodbye!", out, out_size);
    return 1;
}

/* --- module: memory ------------------------------------------------------
 * The first *stateful* part: it learns the user's name and recalls it. This
 * is where the brain stops being purely reactive and starts carrying context
 * across turns. */
static int mod_memory(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;

    static const char *const ask_name[] = {
        "what is my name",  "what is my name?",
        "what's my name",   "what's my name?",
        "whats my name",    "whats my name?",
        NULL,
    };

    /* Teach: "my name is <X>" */
    static const char *const prefix = "my name is ";
    size_t plen = strlen(prefix);
    if (strncmp(norm, prefix, plen) == 0) {
        const char *name = skip_ws(skip_ws(raw) + plen);
        if (*name == '\0') {
            put("I didn't catch your name.", out, out_size);
            return 1;
        }
        copy_trim(b->name, sizeof b->name, name);
        b->has_name = 1;
        char msg[128];
        snprintf(msg, sizeof msg, "Nice to meet you, %s!", b->name);
        put(msg, out, out_size);
        return 1;
    }

    /* Recall: "what is my name?" */
    if (matches_any(norm, ask_name)) {
        if (b->has_name) {
            char msg[128];
            snprintf(msg, sizeof msg, "Your name is %s.", b->name);
            put(msg, out, out_size);
        } else {
            put("I don't know your name yet.", out, out_size);
        }
        return 1;
    }

    return 0;
}

/* The registry: an ordered list of cooperating parts. To add or remove a
 * behaviour, touch only this table — not brain_respond()'s control flow. */
static const Module registry[] = {
    {"memory",   mod_memory},
    {"greet",    mod_greet},
    {"farewell", mod_farewell},
};
static const size_t registry_len = sizeof registry / sizeof registry[0];

/* ----------------------------------------------------------------------------
 * brain lifecycle + dispatch
 * ------------------------------------------------------------------------- */

Brain *brain_create(void) {
    Brain *b = calloc(1, sizeof *b);
    return b; /* may be NULL; caller handles it */
}

void brain_destroy(Brain *b) {
    free(b);
}

const char *brain_version(void) {
    return "gen3-memory";
}

size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size) {
    if (out_size == 0) return 0;
    if (b) b->turns++;

    char norm[256];
    normalize(input, norm, sizeof norm);

    /* Walk the registry; first module to claim the turn wins. */
    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, norm, input, out, out_size)) {
            return strlen(out);
        }
    }

    /* Fallback (gen0 instinct): parrot the input back, verbatim. */
    size_t n = strlen(input);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, input, n);
    out[n] = '\0';
    return n;
}