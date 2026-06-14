/*
 * brain.c - generation 0: the parrot.
 *
 * This is the starting point of the experiment. parrot0 does the most
 * honest thing a "zero" agent can do: it repeats what it heard. Every
 * future generation grows out of this file through the self-improvement
 * loop (see LOOP.md). The goal is for genuine conversational behaviour to
 * EMERGE here as plain C algorithms - no model, no magic.
 */
#include "brain.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct Brain {
    unsigned long turns; /* how many exchanges we've had this session */
};

/* Copy `in` into `out` lowercased and with surrounding blanks trimmed, so the
 * brain can match on intent without caring about case or stray spaces. */
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

Brain *brain_create(void) {
    Brain *b = calloc(1, sizeof *b);
    return b; /* may be NULL; caller handles it */
}

void brain_destroy(Brain *b) {
    free(b);
}

const char *brain_version(void) {
    return "gen1-greet";
}

/* Write a fixed reply into out, returning its length. */
static size_t reply(const char *text, char *out, size_t out_size) {
    size_t n = strlen(text);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, text, n);
    out[n] = '\0';
    return n;
}

size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size) {
    if (out_size == 0) return 0;
    if (b) b->turns++;

    /* Generation 1: recognise greetings and farewells; otherwise still
     * parrot. The first hint of intent over pure echo. */
    static const char *const greetings[] = {"hello", "hi", "hey", NULL};
    static const char *const farewells[] = {"bye", "goodbye", NULL};

    char norm[256];
    normalize(input, norm, sizeof norm);

    if (matches_any(norm, greetings)) {
        return reply("Hi there!", out, out_size);
    }
    if (matches_any(norm, farewells)) {
        return reply("Goodbye!", out, out_size);
    }

    /* Fallback (gen0 behaviour): echo the input back, verbatim. */
    size_t n = strlen(input);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, input, n);
    out[n] = '\0';
    return n;
}