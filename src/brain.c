/*
 * brain.c - an articulated brain (gen2+), now with session memory (gen3).
 *
 * Up to gen1 the brain was a flat chain of `if`s. Per PRINCIPLES.md, reasoning
 * structure is articulated, not a uniform soup — so the brain is a *registry
 * of cooperating modules*. Each module inspects a turn and either claims it
 * (produces a response) or declines (passes it on). If nobody claims it,
 * parrot0 falls back to its oldest instinct: echo.
 *
 * Modules may be stateless (greet, farewell), stateful (memory carries the
 * user's name across turns), or front-ends to a sub-system of their own
 * (knowledge talks to the kb.* engine — facts, unification, rules + resolution,
 * and induction of rules from facts; the first fractal split). New parts (and
 * parts-of-parts) are meant to EMERGE here as future tasks demand them — we do
 * not pre-design a taxonomy.
 */
#include "brain.h"
#include "kb.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Brain {
    unsigned long turns;   /* how many exchanges we've had this session */
    char name[64];         /* the user's name, once they tell us */
    int  has_name;         /* whether `name` is set */
    KB  *kb;               /* the knowledge base (gen4+) */
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

/* --- module: knowledge ---------------------------------------------------
 * The first step of the Prolog-like spine (see PRINCIPLES.md). It translates
 * a sliver of natural language into ground facts and ground queries over the
 * knowledge base:
 *
 *   "<x> is a <y>"   /  "<x> is an <y>"   ->  assert y(x)
 *   "is <x> a <y>?"  /  "is <x> an <y>?"  ->  query  y(x)   (closed-world)
 *
 * Only single-word x and y for now; richer terms emerge in later generations.
 */

/* Split `s` (modified in place) into up to `max` whitespace-separated words,
 * storing pointers in `argv`. Returns the word count. */
static size_t split_words(char *s, char **argv, size_t max) {
    size_t n = 0;
    char *p = s;
    while (*p && n < max) {
        while (*p && isspace((unsigned char)*p)) *p++ = '\0';
        if (!*p) break;
        argv[n++] = p;
        while (*p && !isspace((unsigned char)*p)) p++;
    }
    return n;
}

/* "a" or "an" — the article that separates subject from class. */
static int is_article(const char *w) {
    return strcmp(w, "a") == 0 || strcmp(w, "an") == 0;
}

/* Answer a "why ...?" by rendering the proof, or admit there is none. */
static void explain_reply(Brain *b, const char *pred, const char *const *args,
                          size_t argc, char *out, size_t out_size) {
    char ex[512];
    if (kb_explain(b->kb, pred, args, argc, ex, sizeof ex)) {
        char msg[600];
        if (strstr(ex, " because ")) snprintf(msg, sizeof msg, "%s.", ex);
        else snprintf(msg, sizeof msg, "%s is a known fact.", ex);
        put(msg, out, out_size);
    } else {
        put("I can't show that.", out, out_size);
    }
}

static int mod_knowledge(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* Work on a mutable copy with any trailing '?' stripped. */
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* explanation: "why is <x> a/an <y>?" -> render the proof of y(x) */
    if (nw == 5 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        is_article(w[3])) {
        const char *args[] = {w[2]};
        explain_reply(b, w[4], args, 1, out, out_size);
        return 1;
    }
    /* explanation: "why is <x> the <rel> of <y>?" -> proof of rel(x, y) */
    if (nw == 7 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        strcmp(w[3], "the") == 0 && strcmp(w[5], "of") == 0) {
        const char *args[] = {w[2], w[6]};
        explain_reply(b, w[4], args, 2, out, out_size);
        return 1;
    }

    /* induction ("training"): "generalize" / "learn" -> induce rules from
     * the facts and report what was learned. */
    if (nw == 1 && (strcmp(w[0], "generalize") == 0 ||
                    strcmp(w[0], "learn") == 0)) {
        char heads[16][KB_TERM_LEN], bodies[16][KB_TERM_LEN];
        size_t k = kb_induce(b->kb, 2, heads, bodies, 16);
        if (k == 0) { put("Nothing new to generalize.", out, out_size); return 1; }
        char msg[600];
        size_t off = (size_t)snprintf(msg, sizeof msg, "Induced: ");
        for (size_t i = 0; i < k && off < sizeof msg; i++) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s%s(X) :- %s(X)", i ? "; " : "",
                                    heads[i], bodies[i]);
        }
        if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
        put(msg, out, out_size);
        return 1;
    }

    /* rule: "every <y> is a/an <z>" -> z(X) :- y(X) */
    if (nw == 5 && strcmp(w[0], "every") == 0 && strcmp(w[2], "is") == 0 &&
        is_article(w[3])) {
        const char *body = w[1], *head = w[4];
        if (kb_assert_rule(b->kb, head, body)) {
            char msg[128];
            snprintf(msg, sizeof msg, "Learned rule: %s(X) :- %s(X).",
                     head, body);
            put(msg, out, out_size);
        } else {
            put("I couldn't store that rule.", out, out_size);
        }
        return 1;
    }

    /* retract: "forget that <x> is a/an <y>" -> remove y(x) */
    if (nw == 6 && strcmp(w[0], "forget") == 0 && strcmp(w[1], "that") == 0 &&
        strcmp(w[3], "is") == 0 && is_article(w[4])) {
        const char *subj = w[2], *cl = w[5];
        const char *args[] = {subj};
        char msg[128];
        if (kb_retract(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Forgotten: %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I didn't know that anyway.");
        put(msg, out, out_size);
        return 1;
    }

    /* retract (correction): "<x> is not a/an <y>" -> remove y(x) */
    if (nw == 5 && strcmp(w[1], "is") == 0 && strcmp(w[2], "not") == 0 &&
        is_article(w[3])) {
        const char *subj = w[0], *cl = w[4];
        const char *args[] = {subj};
        char msg[128];
        if (kb_retract(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Forgotten: %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I didn't know that anyway.");
        put(msg, out, out_size);
        return 1;
    }

    /* --- binary relations: "<x> is the <rel> of <y>" (gen11) --- */
    if (nw == 6 && strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0) {
        const char *rel = w[3], *obj = w[5];

        /* variable query, subject unknown: "who is the <rel> of <y>?" ->
         * rel(X, y); object unknown: "what is the <rel> of <y>?" -> rel(y, X) */
        if ((strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
            strcmp(w[1], "is") == 0) {
            const char *who_pat[]  = {NULL, obj};   /* rel(X, y) */
            const char *what_pat[] = {obj, NULL};   /* rel(y, X) */
            const char *const *pat =
                (strcmp(w[0], "what") == 0) ? what_pat : who_pat;
            char hits[64][KB_TERM_LEN];
            size_t k = kb_match(b->kb, rel, pat, 2, hits, 64);
            if (k == 0) { put("Nobody that I know of.", out, out_size); return 1; }
            char list[512];
            size_t off = 0;
            for (size_t i = 0; i < k && off < sizeof list; i++)
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s", i ? ", " : "", hits[i]);
            char msg[600];
            snprintf(msg, sizeof msg, "%s.", list);
            put(msg, out, out_size);
            return 1;
        }

        /* ground query: "is <x> the <rel> of <y>?" -> rel(x, y)? */
        if (strcmp(w[0], "is") == 0) {
            const char *subj = w[1];
            const char *args[] = {subj, obj};
            put(kb_query(b->kb, rel, args, 2) ? "Yes." : "No.", out, out_size);
            return 1;
        }

        /* assert: "<x> is the <rel> of <y>" -> rel(x, y) */
        if (strcmp(w[1], "is") == 0) {
            const char *subj = w[0];
            const char *args[] = {subj, obj};
            char msg[160];
            if (kb_assert(b->kb, rel, args, 2))
                snprintf(msg, sizeof msg, "Learned: %s(%s, %s).", rel, subj, obj);
            else
                snprintf(msg, sizeof msg, "I couldn't store that.");
            put(msg, out, out_size);
            return 1;
        }
    }

    if (nw != 4 || !is_article(w[2])) return 0;
    const char *cls = w[3];

    /* variable query: "who/what is a <y>?" -> y(X), list the bindings */
    if ((strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
        strcmp(w[1], "is") == 0) {
        const char *pat[] = {NULL}; /* one variable in arg 0 */
        char hits[64][KB_TERM_LEN];
        size_t k = kb_match(b->kb, cls, pat, 1, hits, 64);
        if (k == 0) { put("Nobody that I know of.", out, out_size); return 1; }
        char list[512];
        size_t off = 0;
        for (size_t i = 0; i < k && off < sizeof list; i++) {
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", i ? ", " : "", hits[i]);
        }
        char msg[600];
        snprintf(msg, sizeof msg, "%s.", list);
        put(msg, out, out_size);
        return 1;
    }

    /* ground query: "is <x> a <y>?" -> y(x)? */
    if (strcmp(w[0], "is") == 0) {
        const char *subj = w[1];
        const char *args[] = {subj};
        put(kb_query(b->kb, cls, args, 1) ? "Yes." : "No.", out, out_size);
        return 1;
    }

    /* assert: "<x> is a <y>" -> y(x) */
    if (strcmp(w[1], "is") == 0) {
        const char *subj = w[0];
        const char *args[] = {subj};
        if (kb_assert(b->kb, cls, args, 1)) {
            char msg[128];
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cls, subj);
            put(msg, out, out_size);
        } else {
            put("I couldn't store that.", out, out_size);
        }
        return 1;
    }

    return 0;
}

/* --- module: self --------------------------------------------------------
 * Identity & self-reflection (PRINCIPLES.md, "I know that I am"). The agent's
 * self-model lives in the very same KB it uses for the world: `i_am(parrot0).`
 * and one `module(<name>)` per registered part (asserted at birth — see
 * brain_create). This module answers introspective questions by *querying that
 * model*, so the answers are derived from real state, never hard-coded.
 */
static int mod_self(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    const char *var[] = {NULL};

    /* "who/what are you?" -> from i_am(X) */
    if (strcmp(buf, "who are you") == 0 || strcmp(buf, "what are you") == 0) {
        char id[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        if (k > 0) {
            char msg[128];
            snprintf(msg, sizeof msg, "I am %s.", id[0]);
            put(msg, out, out_size);
        } else {
            put("I don't know what I am.", out, out_size);
        }
        return 1;
    }

    /* "do you exist?" -> resolves i_am(X) */
    if (strcmp(buf, "do you exist") == 0) {
        char id[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        if (k > 0) snprintf(msg, sizeof msg, "Yes, I am %s.", id[0]);
        else       snprintf(msg, sizeof msg, "I'm not sure.");
        put(msg, out, out_size);
        return 1;
    }

    /* "what can you do?" -> list the modules from module(X) */
    if (strcmp(buf, "what can you do") == 0 ||
        strcmp(buf, "what do you do") == 0) {
        char mods[64][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "module", var, 1, mods, 64);
        if (k == 0) { put("Nothing yet.", out, out_size); return 1; }
        char list[512];
        size_t off = 0;
        for (size_t i = 0; i < k && off < sizeof list; i++) {
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", i ? ", " : "", mods[i]);
        }
        char msg[600];
        snprintf(msg, sizeof msg, "My modules are: %s.", list);
        put(msg, out, out_size);
        return 1;
    }

    return 0;
}

/* The registry: an ordered list of cooperating parts. To add or remove a
 * behaviour, touch only this table — not brain_respond()'s control flow.
 * (This table is also reified into the KB as module(...) facts at birth, so
 * the agent's self-description cannot drift from its real structure.) */
static const Module registry[] = {
    {"memory",    mod_memory},
    {"self",      mod_self},
    {"knowledge", mod_knowledge},
    {"greet",     mod_greet},
    {"farewell",  mod_farewell},
};
static const size_t registry_len = sizeof registry / sizeof registry[0];

/* ----------------------------------------------------------------------------
 * brain lifecycle + dispatch
 * ------------------------------------------------------------------------- */

Brain *brain_create(void) {
    Brain *b = calloc(1, sizeof *b);
    if (!b) return NULL;
    b->kb = kb_create();
    if (!b->kb) { free(b); return NULL; }

    /* Reflective self-model: the agent writes itself into its own KB, derived
     * from real structure (PRINCIPLES.md). Tagged KB_REFLECTIVE so it is
     * regenerated every boot and NEVER persisted (DESIGN.md D3). */
    kb_set_origin(b->kb, KB_REFLECTIVE);
    const char *me[] = {"parrot0"};
    kb_assert(b->kb, "i_am", me, 1);
    for (size_t i = 0; i < registry_len; i++) {
        const char *m[] = {registry[i].name};
        kb_assert(b->kb, "module", m, 1);
    }
    kb_set_origin(b->kb, KB_SESSION); /* conversation default */
    return b;
}

int brain_load(Brain *b, const char *path, int as_base) {
    if (!b || !b->kb) return 0;
    kb_set_origin(b->kb, as_base ? KB_BASE : KB_SESSION);
    int n = kb_load(b->kb, path);
    kb_set_origin(b->kb, KB_SESSION); /* back to conversation default */
    return n;
}

int brain_save_session(Brain *b, const char *path) {
    if (!b || !b->kb) return -1;
    return kb_save(b->kb, path, KB_SESSION | KB_INDUCED);
}

void brain_destroy(Brain *b) {
    if (!b) return;
    kb_destroy(b->kb);
    free(b);
}

const char *brain_version(void) {
    return "gen14-explain";
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