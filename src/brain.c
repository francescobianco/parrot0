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
    char last_entity[KB_TERM_LEN]; /* most recent concrete KB entity */
    int  has_last_entity;  /* whether `last_entity` is set */
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

/* Minimal discourse coreference (gen22): pronouns resolve to the most recent
 * concrete entity mentioned in the knowledge surface. */
static int is_entity_pronoun(const char *w) {
    return strcmp(w, "he") == 0 || strcmp(w, "she") == 0 ||
           strcmp(w, "it") == 0 || strcmp(w, "they") == 0 ||
           strcmp(w, "him") == 0 || strcmp(w, "her") == 0 ||
           strcmp(w, "them") == 0;
}

static int resolve_entity(Brain *b, const char *word, const char **entity,
                          char *out, size_t out_size) {
    if (!is_entity_pronoun(word)) { *entity = word; return 1; }
    if (b->has_last_entity) { *entity = b->last_entity; return 1; }

    char msg[160];
    snprintf(msg, sizeof msg, "I don't know who %s refers to.", word);
    put(msg, out, out_size);
    return 0;
}

static void remember_entity(Brain *b, const char *word, const char *entity) {
    if (is_entity_pronoun(word) || !entity || strlen(entity) >= KB_TERM_LEN)
        return;
    snprintf(b->last_entity, sizeof b->last_entity, "%s", entity);
    b->has_last_entity = 1;
}

/* Admit ignorance about a predicate we've never heard of (gen16 scaffold;
 * see DESIGN.md D6 — to become emergent meta-knowledge). */
static void idk(const char *pred, char *out, size_t out_size) {
    char msg[160];
    snprintf(msg, sizeof msg, "I don't know about %s.", pred);
    put(msg, out, out_size);
}

/* Answer a "why ...?" by rendering the proof, or admit there is none. */
static void explain_reply(Brain *b, const char *pred, const char *const *args,
                          size_t argc, char *out, size_t out_size) {
    if (kb_is_conflicted(b->kb, pred, args, argc)) {
        put("I have conflicting evidence for that.", out, out_size);
        return;
    }

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

/* Answer "how do you know <goal>?" by reading the proof trace and reporting
 * its *depth* — the number of inference steps. A goal proved straight from a
 * stored fact is DIRECT (zero rule applications); a goal proved through rules
 * is MULTI-STEP, and we report how many steps. The step count is the number of
 * " because " links the proof renderer emits, i.e. one per rule application
 * along the chain — so this is a property of the actual proof structure, not a
 * canned label. A goal with no proof is refused, never invented (gen26). */
static void howknow_reply(Brain *b, const char *pred, const char *const *args,
                          size_t argc, char *out, size_t out_size) {
    if (kb_is_conflicted(b->kb, pred, args, argc)) {
        put("I have conflicting evidence for that.", out, out_size);
        return;
    }

    char ex[512];
    if (!kb_explain(b->kb, pred, args, argc, ex, sizeof ex)) {
        put("I can't show that.", out, out_size);
        return;
    }

    size_t steps = 0;
    for (const char *p = ex; (p = strstr(p, " because ")) != NULL; p += 9)
        steps++;

    char msg[640];
    if (steps == 0)
        snprintf(msg, sizeof msg, "Directly: %s is a known fact.", ex);
    else
        snprintf(msg, sizeof msg, "By %zu step%s of reasoning: %s.",
                 steps, steps == 1 ? "" : "s", ex);
    put(msg, out, out_size);
}

static int mod_knowledge(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size);

static char *trim_mut(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    return s;
}

static int apply_premise_clause(Brain *tmp, char *clause) {
    clause = trim_mut(clause);
    if (*clause == '\0') return 1;

    int origin = KB_SESSION;
    if (strncmp(clause, "base says ", 10) == 0) {
        origin = KB_BASE;
        clause = trim_mut(clause + 10);
    } else if (strncmp(clause, "session says ", 13) == 0) {
        origin = KB_SESSION;
        clause = trim_mut(clause + 13);
    }

    kb_set_origin(tmp->kb, origin);
    char discard[256];
    int claimed = mod_knowledge(tmp, clause, clause, discard, sizeof discard);
    kb_set_origin(tmp->kb, KB_SESSION);
    return claimed && strncmp(discard, "I couldn't", 10) != 0 &&
           strncmp(discard, "I don't understand", 18) != 0;
}

static int apply_premises(Brain *tmp, char *premises) {
    char *p = premises;
    while (p && *p) {
        char *next = strstr(p, " and ");
        if (next) {
            *next = '\0';
            next += 5;
        }
        if (!apply_premise_clause(tmp, p)) return 0;
        p = next;
    }
    return 1;
}

/* Entailment surface modes. PLAIN/EXPLAIN speak parrot0's own 4-valued
 * epistemic vocabulary; LABEL collapses it into SuperGLUE CB's 3-way label
 * space (entailment / contradiction / neutral), discovered from the CB probe.
 * The collapse is a real decision: both "unknown" (predicate never seen) and
 * "conflicted" (contradictory evidence) become neutral — see Decision
 * D-2026-06-15b. */
enum { ENT_PLAIN = 0, ENT_EXPLAIN = 1, ENT_LABEL = 2 };

static void entailment_status(Brain *tmp, const char *hyp, int mode,
                              char *out, size_t out_size) {
    char hbuf[256];
    size_t len = strlen(hyp);
    if (len >= sizeof hbuf) { put("I don't understand that entailment yet.", out, out_size); return; }
    memcpy(hbuf, hyp, len + 1);
    if (len > 0 && hbuf[len - 1] == '?') hbuf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(hbuf, w, 8);
    const char *pred = NULL;
    const char *args[2];
    size_t argc = 0;

    if (nw == 4 && strcmp(w[0], "is") == 0 && is_article(w[2])) {
        pred = w[3];
        args[0] = w[1];
        argc = 1;
    } else if (nw == 6 && strcmp(w[0], "is") == 0 &&
               strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0) {
        pred = w[3];
        args[0] = w[1];
        args[1] = w[5];
        argc = 2;
    } else {
        put("I don't understand that entailment yet.", out, out_size);
        return;
    }

    if (!kb_knows_pred(tmp->kb, pred))
        put(mode == ENT_LABEL ? "Neutral." : "Unknown.", out, out_size);
    else if (kb_is_conflicted(tmp->kb, pred, args, argc))
        put(mode == ENT_LABEL ? "Neutral." : "Conflicted.", out, out_size);
    else if (kb_query(tmp->kb, pred, args, argc)) {
        if (mode == ENT_LABEL) {
            put("Entailment.", out, out_size);
        } else if (mode == ENT_PLAIN) {
            put("Entailed.", out, out_size);
        } else {
            char ex[512];
            if (kb_explain(tmp->kb, pred, args, argc, ex, sizeof ex)) {
                char msg[640];
                if (strstr(ex, " because "))
                    snprintf(msg, sizeof msg, "Entailed: %s.", ex);
                else
                    snprintf(msg, sizeof msg, "Entailed: %s is a known fact.", ex);
                put(msg, out, out_size);
            } else {
                put("Entailed.", out, out_size);
            }
        }
    }
    else if (kb_is_negated(tmp->kb, pred, args, argc))
        put(mode == ENT_LABEL ? "Contradiction." : "Contradicted.", out, out_size);
    else
        put(mode == ENT_LABEL ? "Neutral." : "Not entailed.", out, out_size);
}

static int entailment_reply(const char *premises, const char *hypothesis,
                            int mode, char *out, size_t out_size) {
    Brain tmp;
    memset(&tmp, 0, sizeof tmp);
    tmp.kb = kb_create();
    if (!tmp.kb) { put("I couldn't evaluate that entailment.", out, out_size); return 1; }

    char pbuf[512];
    size_t plen = strlen(premises);
    if (plen >= sizeof pbuf) {
        kb_destroy(tmp.kb);
        put("I don't understand that entailment yet.", out, out_size);
        return 1;
    }
    memcpy(pbuf, premises, plen + 1);

    if (!apply_premises(&tmp, pbuf)) {
        kb_destroy(tmp.kb);
        put("I don't understand that entailment yet.", out, out_size);
        return 1;
    }

    entailment_status(&tmp, trim_mut((char *)hypothesis), mode, out, out_size);
    kb_destroy(tmp.kb);
    return 1;
}

static int mod_knowledge(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* Work on a mutable copy with any trailing '?' stripped. */
    char buf[512];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    int entail_mode = ENT_PLAIN;
    char *premise_start = NULL;
    if (strncmp(buf, "explain premise:", 16) == 0) {
        entail_mode = ENT_EXPLAIN;
        premise_start = buf + 16;
    } else if (strncmp(buf, "label premise:", 14) == 0) {
        entail_mode = ENT_LABEL;
        premise_start = buf + 14;
    } else if (strncmp(buf, "premise:", 8) == 0) {
        premise_start = buf + 8;
    }
    if (premise_start) {
        char *hyp = strstr(buf, "; hypothesis:");
        if (!hyp) {
            put("I don't understand that entailment yet.", out, out_size);
            return 1;
        }
        *hyp = '\0';
        hyp += strlen("; hypothesis:");
        return entailment_reply(trim_mut(premise_start), trim_mut(hyp),
                                entail_mode, out, out_size);
    }

    char *choice_start = NULL;
    if (strncmp(buf, "which is a ", 11) == 0) choice_start = buf + 11;
    else if (strncmp(buf, "which is an ", 12) == 0) choice_start = buf + 12;
    if (choice_start) {
        char *colon = strchr(choice_start, ':');
        if (!colon) {
            put("I don't understand that question yet.", out, out_size);
            return 1;
        }
        *colon = '\0';
        const char *cls = trim_mut(choice_start);
        if (!kb_knows_pred(b->kb, cls)) { idk(cls, out, out_size); return 1; }

        char *choices = colon + 1;
        char list[512];
        size_t off = 0, hits = 0;
        while (choices && *choices) {
            char *next = strchr(choices, ',');
            if (next) *next++ = '\0';
            char *choice = trim_mut(choices);
            if (*choice && strlen(choice) < KB_TERM_LEN) {
                const char *args[] = {choice};
                if (!kb_is_conflicted(b->kb, cls, args, 1) &&
                    kb_query(b->kb, cls, args, 1)) {
                    off += (size_t)snprintf(list + off, sizeof list - off,
                                            "%s%s", hits ? ", " : "", choice);
                    hits++;
                }
            }
            choices = next;
        }
        if (hits == 0) put("None of them.", out, out_size);
        else {
            char msg[600];
            snprintf(msg, sizeof msg, "%s.", list);
            put(msg, out, out_size);
        }
        return 1;
    }

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* explanation: "why is <x> a/an <y>?" -> render the proof of y(x) */
    if (nw == 5 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        is_article(w[3])) {
        const char *subj;
        if (!resolve_entity(b, w[2], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        explain_reply(b, w[4], args, 1, out, out_size);
        remember_entity(b, w[2], subj);
        return 1;
    }
    /* explanation: "why is <x> the <rel> of <y>?" -> proof of rel(x, y) */
    if (nw == 7 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        strcmp(w[3], "the") == 0 && strcmp(w[5], "of") == 0) {
        const char *args[] = {w[2], w[6]};
        explain_reply(b, w[4], args, 2, out, out_size);
        return 1;
    }

    /* proof depth (gen26): "how do you know <x> is a/an <y>?" -> classify the
     * proof of y(x) as direct (fact) vs multi-step (rule chain) reasoning. */
    if (nw == 8 && strcmp(w[0], "how") == 0 && strcmp(w[1], "do") == 0 &&
        strcmp(w[2], "you") == 0 && strcmp(w[3], "know") == 0 &&
        strcmp(w[5], "is") == 0 && is_article(w[6])) {
        const char *subj;
        if (!resolve_entity(b, w[4], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        howknow_reply(b, w[7], args, 1, out, out_size);
        remember_entity(b, w[4], subj);
        return 1;
    }

    /* direct belief report: "what do you know about <x>?" */
    if (nw == 6 && strcmp(w[0], "what") == 0 && strcmp(w[1], "do") == 0 &&
        strcmp(w[2], "you") == 0 && strcmp(w[3], "know") == 0 &&
        strcmp(w[4], "about") == 0) {
        const char *entity;
        if (!resolve_entity(b, w[5], &entity, out, out_size)) return 1;
        char desc[1024];
        if (kb_describe_entity(b->kb, entity, desc, sizeof desc)) {
            put(desc, out, out_size);
        } else {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know anything about %s.", entity);
            put(msg, out, out_size);
        }
        remember_entity(b, w[5], entity);
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
        const char *subj, *cl = w[5];
        if (!resolve_entity(b, w[2], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        if (kb_retract(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Forgotten: %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I didn't know that anyway.");
        put(msg, out, out_size);
        remember_entity(b, w[2], subj);
        return 1;
    }

    /* explicit negative correction: "<x> is not a/an <y>" -> not y(x) */
    if (nw == 5 && strcmp(w[1], "is") == 0 && strcmp(w[2], "not") == 0 &&
        is_article(w[3])) {
        const char *subj, *cl = w[4];
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        if (kb_assert_neg(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Learned: not %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* --- binary relations: "<x> is the <rel> of <y>" (gen11) --- */
    if (nw == 6 && strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0) {
        const char *rel = w[3], *obj = w[5];

        /* variable query, subject unknown: "who is the <rel> of <y>?" ->
         * rel(X, y); object unknown: "what is the <rel> of <y>?" -> rel(y, X) */
        if ((strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
            strcmp(w[1], "is") == 0) {
            if (!kb_knows_pred(b->kb, rel)) { idk(rel, out, out_size); return 1; }
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
            if (!kb_knows_pred(b->kb, rel)) idk(rel, out, out_size);
            else if (kb_is_conflicted(b->kb, rel, args, 2))
                put("Conflicted.", out, out_size);
            else put(kb_query(b->kb, rel, args, 2) ? "Yes." : "No.",
                     out, out_size);
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
        if (!kb_knows_pred(b->kb, cls)) { idk(cls, out, out_size); return 1; }
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
        const char *subj;
        if (!resolve_entity(b, w[1], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        if (!kb_knows_pred(b->kb, cls)) idk(cls, out, out_size);
        else if (kb_is_conflicted(b->kb, cls, args, 1))
            put("Conflicted.", out, out_size);
        else put(kb_query(b->kb, cls, args, 1) ? "Yes." : "No.", out, out_size);
        remember_entity(b, w[1], subj);
        return 1;
    }

    /* assert: "<x> is a <y>" -> y(x) */
    if (strcmp(w[1], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        if (kb_assert(b->kb, cls, args, 1)) {
            char msg[128];
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cls, subj);
            put(msg, out, out_size);
        } else {
            put("I couldn't store that.", out, out_size);
        }
        remember_entity(b, w[0], subj);
        return 1;
    }

    return 0;
}

/* --- module: compare -----------------------------------------------------
 * Ordinal reasoning over quantities (gen27). Discovered by domain-pull: the
 * first official SuperGLUE/BoolQ question parrot0 was shown asks whether
 * ethanol "take[s] more energy ... than [it] produces" — a *comparison of two
 * magnitudes*, a kind of reasoning the KB (symbolic atoms only) could not do.
 * This part answers the comparison itself: "is <a> more/less than <b>?" over
 * numbers, returning a closed yes/no. It is the reasoning primitive on the
 * path to such questions; turning a passage into the two numbers is a separate,
 * larger feature (NL extraction) we deliberately do NOT fake here. */
static int parse_num(const char *s, double *out) {
    if (!*s) return 0;
    char *end;
    double v = strtod(s, &end);
    if (end == s) return 0;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return 0;
    *out = v;
    return 1;
}

/* The shared magnitude test: is `a` more (greater=1) / less (greater=0) than
 * `c`? Both mod_compare (literal numbers) and mod_quantity (numbers looked up
 * from the KB) route their decision through this one comparator. */
static int magnitude_more(double a, double c, int greater) {
    return greater ? (a > c) : (a < c);
}

/* Map "more"/"greater" -> 1, "less"/"fewer" -> 0, anything else -> -1. */
static int compare_word(const char *w) {
    if (strcmp(w, "more") == 0 || strcmp(w, "greater") == 0) return 1;
    if (strcmp(w, "less") == 0 || strcmp(w, "fewer") == 0) return 0;
    return -1;
}

static int mod_compare(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)b; (void)raw;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);
    if (nw != 5 || strcmp(w[0], "is") != 0 || strcmp(w[3], "than") != 0)
        return 0;

    int greater = compare_word(w[2]);
    if (greater < 0) return 0;

    double a, c;
    if (!parse_num(w[1], &a) || !parse_num(w[4], &c)) return 0; /* not numbers */

    put(magnitude_more(a, c, greater) ? "Yes." : "No.", out, out_size);
    return 1;
}

/* --- module: arith -------------------------------------------------------
 * Arithmetic over numbers (gen35). gen27/gen28 could *order* magnitudes but
 * never *compute* with them — numbers were inert (Decision D-2026-06-15a). The
 * first SuperGLUE BoolQ #6 ("can an odd number be divided by an even number")
 * is about arithmetic. This part computes `what is <a> plus/minus/times <b>?`
 * and decides `is <a> divisible by <b>?` by integer remainder, over literal
 * numbers (parsed with the shared `parse_num`). Operator set kept tiny on
 * purpose; non-numbers are declined and fall through. */

/* Format a value as a clean integer when it is integral, else compactly. */
static void format_num(double v, char *buf, size_t sz) {
    long long iv = (long long)v;
    if ((double)iv == v) snprintf(buf, sz, "%lld", iv);
    else                 snprintf(buf, sz, "%g", v);
}

static int mod_arith(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)b; (void)raw;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* "what is <a> plus/minus/times <b>?" -> the computed value */
    if (nw == 5 && strcmp(w[0], "what") == 0 && strcmp(w[1], "is") == 0) {
        double a, c;
        if (!parse_num(w[2], &a) || !parse_num(w[4], &c)) return 0;
        double r;
        if (strcmp(w[3], "plus") == 0)       r = a + c;
        else if (strcmp(w[3], "minus") == 0) r = a - c;
        else if (strcmp(w[3], "times") == 0) r = a * c;
        else return 0;
        char num[64], msg[80];
        format_num(r, num, sizeof num);
        snprintf(msg, sizeof msg, "%s.", num);
        put(msg, out, out_size);
        return 1;
    }

    /* "is <a> divisible by <b>?" -> yes/no via integer remainder */
    if (nw == 5 && strcmp(w[0], "is") == 0 && strcmp(w[2], "divisible") == 0 &&
        strcmp(w[3], "by") == 0) {
        double a, c;
        if (!parse_num(w[1], &a) || !parse_num(w[4], &c)) return 0;
        if (c == 0) { put("I can't divide by zero.", out, out_size); return 1; }
        long long ai = (long long)a, ci = (long long)c;
        int divisible;
        if ((double)ai == a && (double)ci == c) {
            divisible = (ai % ci == 0);
        } else {
            double q = a / c;
            double rem = a - c * (double)(long long)q;
            divisible = (rem > -1e-9 && rem < 1e-9);
        }
        put(divisible ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: quantity ----------------------------------------------------
 * Quantities as knowledge (gen28). gen27 could compare two literal numbers;
 * this part lets a magnitude be *stated, recalled, and compared as a fact*, so
 * the comparison primitive can be driven from language rather than from
 * pre-extracted numbers — the next step the BoolQ probe pulled. A quantity is
 * a 3-ary fact `quantity(entity, unit, value)`; the value rides in the KB as a
 * string atom and is parsed back with parse_num when compared. Turning prose
 * into these facts (open-domain extraction) is deliberately still out of scope:
 * we build the reasoning, not a passage parser. */
static int mod_quantity(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<x> has <n> <unit>" -> quantity(x, unit, n) */
    if (nw == 4 && strcmp(w[1], "has") == 0) {
        double v;
        if (!parse_num(w[2], &v)) return 0; /* not a quantity; let others try */
        const char *args[] = {w[0], w[3], w[2]};
        char msg[160];
        if (kb_assert(b->kb, "quantity", args, 3))
            snprintf(msg, sizeof msg, "Learned: %s has %s %s.", w[0], w[2], w[3]);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* recall: "how many <unit> does <x> have" -> quantity(x, unit, ?) */
    if (nw == 6 && strcmp(w[0], "how") == 0 && strcmp(w[1], "many") == 0 &&
        strcmp(w[3], "does") == 0 && strcmp(w[5], "have") == 0) {
        const char *unit = w[2], *x = w[4];
        const char *pat[] = {x, unit, NULL};
        char hits[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "quantity", pat, 3, hits, 4);
        char msg[160];
        if (k == 0)
            snprintf(msg, sizeof msg, "I don't know how many %s %s has.", unit, x);
        else
            snprintf(msg, sizeof msg, "%s has %s %s.", x, hits[0], unit);
        put(msg, out, out_size);
        return 1;
    }

    /* compare: "does <x> have more/less <unit> than <y>" */
    if (nw == 7 && strcmp(w[0], "does") == 0 && strcmp(w[2], "have") == 0 &&
        strcmp(w[5], "than") == 0) {
        int greater = compare_word(w[3]);
        if (greater < 0) return 0;
        const char *unit = w[4], *x = w[1], *y = w[6];
        const char *px[] = {x, unit, NULL}, *py[] = {y, unit, NULL};
        char hx[4][KB_TERM_LEN], hy[4][KB_TERM_LEN];
        size_t kx = kb_match(b->kb, "quantity", px, 3, hx, 4);
        size_t ky = kb_match(b->kb, "quantity", py, 3, hy, 4);
        if (kx == 0 || ky == 0) {
            char msg[200];
            snprintf(msg, sizeof msg, "I don't know how many %s %s has.",
                     unit, kx == 0 ? x : y);
            put(msg, out, out_size);
            return 1;
        }
        double a, c;
        if (!parse_num(hx[0], &a) || !parse_num(hy[0], &c)) return 0;
        put(magnitude_more(a, c, greater) ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: cause -------------------------------------------------------
 * Causal reasoning (gen30). Pulled by the first SuperGLUE COPA question ("The
 * man turned on the faucet. effect: toilet filled / water flowed") — picking a
 * plausible cause or effect, a directed relation parrot0 never had. A causal
 * link is the binary fact `causes(a, b)` (a causes b). This part asserts and
 * queries it in both directions and runs the COPA-shaped two-way chooser over
 * *stated* causal facts. Commonsense causality (COPA's real difficulty —
 * knowing faucets fill spouts) is deliberately out of scope; we build the
 * relation and the chooser, not a world model. */
static int mod_cause(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    /* chooser: "effect of <a>: <c1> or <c2>" / "cause of <a>: <c1> or <c2>" */
    int eff = -1;
    char *rest = NULL;
    if (strncmp(buf, "effect of ", 10) == 0) { eff = 1; rest = buf + 10; }
    else if (strncmp(buf, "cause of ", 9) == 0) { eff = 0; rest = buf + 9; }
    if (rest) {
        char *colon = strchr(rest, ':');
        char *orp = colon ? strstr(colon, " or ") : NULL;
        if (colon && orp) {
            *colon = '\0';
            *orp = '\0';
            const char *a = trim_mut(rest);
            const char *c1 = trim_mut(colon + 1);
            const char *c2 = trim_mut(orp + 4);
            const char *p1[2], *p2[2];
            if (eff) { p1[0]=a; p1[1]=c1; p2[0]=a; p2[1]=c2; }
            else     { p1[0]=c1; p1[1]=a; p2[0]=c2; p2[1]=a; }
            int ok1 = kb_query(b->kb, "causes", p1, 2);
            int ok2 = kb_query(b->kb, "causes", p2, 2);
            char msg[160];
            if (ok1 && !ok2)      snprintf(msg, sizeof msg, "%s.", c1);
            else if (ok2 && !ok1) snprintf(msg, sizeof msg, "%s.", c2);
            else if (ok1 && ok2)  snprintf(msg, sizeof msg, "Both.");
            else                  snprintf(msg, sizeof msg, "Neither.");
            put(msg, out, out_size);
            return 1;
        }
    }

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<a> causes <b>" -> causes(a, b) */
    if (nw == 3 && strcmp(w[1], "causes") == 0) {
        const char *args[] = {w[0], w[2]};
        char msg[160];
        if (kb_assert(b->kb, "causes", args, 2))
            snprintf(msg, sizeof msg, "Learned: causes(%s, %s).", w[0], w[2]);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* query: "what is the effect/cause of <x>?" -> causes(x, ?) / causes(?, x) */
    if (nw == 6 && strcmp(w[0], "what") == 0 && strcmp(w[1], "is") == 0 &&
        strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0 &&
        (strcmp(w[3], "effect") == 0 || strcmp(w[3], "cause") == 0)) {
        int want_eff = strcmp(w[3], "effect") == 0;
        const char *x = w[5];
        const char *pat_eff[] = {x, NULL};   /* causes(x, ?) -> effects */
        const char *pat_cause[] = {NULL, x}; /* causes(?, x) -> causes  */
        char hits[64][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "causes", want_eff ? pat_eff : pat_cause,
                            2, hits, 64);
        if (k == 0) {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know the %s of %s.", w[3], x);
            put(msg, out, out_size);
            return 1;
        }
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

    return 0;
}

/* --- module: same --------------------------------------------------------
 * Equivalence between entities (gen33). Pulled by BoolQ #1 ("is house tax and
 * property tax are same"): a question about *sameness*, which parrot0 had no
 * way to represent or answer without conflating it with class membership. A
 * `same(a, b)` fact is stored in both directions so the relation is symmetric;
 * `are <x> and <y> the same?` answers from it (identical names are trivially
 * the same). It is NOT transitively closed — see Decision D-2026-06-15f. */
static int mod_same(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<x> is the same as <y>" -> same(x, y) (stored both ways) */
    if (nw == 6 && strcmp(w[1], "is") == 0 && strcmp(w[2], "the") == 0 &&
        strcmp(w[3], "same") == 0 && strcmp(w[4], "as") == 0) {
        const char *fwd[] = {w[0], w[5]}, *bwd[] = {w[5], w[0]};
        int ok = kb_assert(b->kb, "same", fwd, 2) &&
                 kb_assert(b->kb, "same", bwd, 2);
        char msg[160];
        if (ok) snprintf(msg, sizeof msg, "Learned: same(%s, %s).", w[0], w[5]);
        else    snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* query: "are <x> and <y> the same?" -> same(x, y)? */
    if (nw == 6 && strcmp(w[0], "are") == 0 && strcmp(w[2], "and") == 0 &&
        strcmp(w[4], "the") == 0 && strcmp(w[5], "same") == 0) {
        const char *x = w[1], *y = w[3];
        if (strcmp(x, y) == 0) { put("Yes.", out, out_size); return 1; }
        const char *args[] = {x, y};
        put(kb_query(b->kb, "same", args, 2) ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: conj --------------------------------------------------------
 * Conjunctive membership (gen34). Multi-fact reasoning (the MultiRC family)
 * needs combining several facts in one judgement; every prior query asked about
 * a single goal. This part answers two AND-shaped questions — `are <x> and <y>
 * both a <z>?` (z(x) AND z(y)) and `is <x> both a <y> and a <z>?` (y(x) AND
 * z(x)) — by routing EACH conjunct through `kb_query`, so rule-derived
 * membership counts exactly as it does for a single query. No new solver:
 * AND-composition is just two resolver calls. An unknown class is admitted. */
static int mod_conj(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[10];
    size_t nw = split_words(buf, w, 10);

    /* "are <x> and <y> both a/an <z>?" -> z(x) AND z(y) */
    if (nw == 7 && strcmp(w[0], "are") == 0 && strcmp(w[2], "and") == 0 &&
        strcmp(w[4], "both") == 0 && is_article(w[5])) {
        const char *z = w[6], *x = w[1], *y = w[3];
        if (!kb_knows_pred(b->kb, z)) { idk(z, out, out_size); return 1; }
        const char *ax[] = {x}, *ay[] = {y};
        int yes = kb_query(b->kb, z, ax, 1) && kb_query(b->kb, z, ay, 1);
        put(yes ? "Yes." : "No.", out, out_size);
        return 1;
    }

    /* "is <x> both a/an <y> and a/an <z>?" -> y(x) AND z(x) */
    if (nw == 8 && strcmp(w[0], "is") == 0 && strcmp(w[2], "both") == 0 &&
        is_article(w[3]) && strcmp(w[5], "and") == 0 && is_article(w[6])) {
        const char *x = w[1], *y = w[4], *z = w[7];
        if (!kb_knows_pred(b->kb, y)) { idk(y, out, out_size); return 1; }
        if (!kb_knows_pred(b->kb, z)) { idk(z, out, out_size); return 1; }
        const char *a[] = {x};
        int yes = kb_query(b->kb, y, a, 1) && kb_query(b->kb, z, a, 1);
        put(yes ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: gen ---------------------------------------------------------
 * Generative inference loop (gen36 — DESIGN D-prop1). The brain's other modules
 * infer ONCE and emit a whole reply. This part generates *autoregressively*,
 * the shape an LLM decodes in, but driven by repeated deterministic inference
 * instead of a neural forward pass: emit a word, append it to the working
 * context, re-infer the next word conditioned on what was just produced, and
 * repeat until no continuation is provable (or a step bound).
 *
 * The continuation knowledge is NOT hand-authored (that would be the phrasebook
 * impostor PRINCIPLES.md rejects): it is *induced from examples* as facts
 * `cont(prev, word, count)`. `learn sequence: a b c` asserts the adjacent
 * transitions; `say <w>` runs the loop from a seed. gen36 chooses the first
 * provable continuation (insertion order); frequency-weighted choice and longer
 * context arrive in later generations. */

/* Learn one transition prev->word, keeping a frequency count. The KB has no
 * in-place update, so we read the current count, retract the old fact, and
 * assert the incremented one (gen37). */
static void learn_transition(Brain *b, const char *prev, const char *word) {
    const char *pat[] = {prev, word, NULL};
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont", pat, 3, cnt, 4);
    long n = 1;
    if (k > 0) {
        n = strtol(cnt[0], NULL, 10) + 1;
        const char *old[] = {prev, word, cnt[0]};
        kb_retract(b->kb, "cont", old, 3);
    }
    char ns[32];
    snprintf(ns, sizeof ns, "%ld", n);
    const char *args[] = {prev, word, ns};
    kb_assert(b->kb, "cont", args, 3);
}

/* Pick the next word after `prev`, or return 0 if no continuation is known.
 * gen37 policy: the highest-count continuation (the deterministic analogue of
 * the most-probable next token), tie-broken by insertion order. */
static int next_word(Brain *b, const char *prev, char *word, size_t wsize) {
    const char *pat[] = {prev, NULL, NULL};
    char words[64][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont", pat, 3, words, 64);
    if (k == 0) return 0;

    long best = -1;
    size_t bi = 0;
    for (size_t i = 0; i < k; i++) {
        const char *cpat[] = {prev, words[i], NULL};
        char cnt[4][KB_TERM_LEN];
        size_t ck = kb_match(b->kb, "cont", cpat, 3, cnt, 4);
        long c = (ck > 0) ? strtol(cnt[0], NULL, 10) : 0;
        if (c > best) { best = c; bi = i; } /* strict > keeps the first on ties */
    }
    snprintf(word, wsize, "%s", words[bi]);
    return 1;
}

static void generate_from(Brain *b, const char *seed, char *out, size_t out_size) {
    char cur[KB_TERM_LEN];
    snprintf(cur, sizeof cur, "%s", seed);

    char line[1024];
    size_t off = (size_t)snprintf(line, sizeof line, "%s", cur);

    for (int step = 0; step < 24; step++) {        /* bound guards cycles */
        char nxt[KB_TERM_LEN];
        if (!next_word(b, cur, nxt, sizeof nxt)) break;
        if (off < sizeof line)
            off += (size_t)snprintf(line + off, sizeof line - off, " %s", nxt);
        snprintf(cur, sizeof cur, "%s", nxt);
    }
    put(line, out, out_size);
}

static int mod_gen(Brain *b, const char *norm, const char *raw,
                   char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* learn the continuation relation from an example: "learn sequence: a b c" */
    if (strncmp(norm, "learn sequence:", 15) == 0) {
        char rem[512];
        snprintf(rem, sizeof rem, "%s", norm + 15);
        char *w[64];
        size_t nw = split_words(rem, w, 64);
        size_t pairs = 0;
        for (size_t i = 0; i + 1 < nw; i++) {
            if (strlen(w[i]) >= KB_TERM_LEN || strlen(w[i + 1]) >= KB_TERM_LEN)
                continue;
            learn_transition(b, w[i], w[i + 1]);
            pairs++;
        }
        char msg[128];
        snprintf(msg, sizeof msg, "Learned %zu transition(s).", pairs);
        put(msg, out, out_size);
        return 1;
    }

    /* generate from a seed: "say <word>" */
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';
    char *w[8];
    size_t nw = split_words(buf, w, 8);
    if (nw == 2 && strcmp(w[0], "say") == 0) {
        generate_from(b, w[1], out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: reader ------------------------------------------------------
 * The text -> facts bridge (gen32). The gen28–gen31 domain-pull run reached one
 * conclusion four times: the reasoning primitives exist, but nothing turns a
 * passage into the `pred(args)` facts they consume. This part is the smallest
 * honest extractor: `read: <passage>` splits the passage into clauses on
 * sentence punctuation and feeds each, in turn, to the existing assertion
 * parsers (quantity, cause, knowledge). Whatever parses is asserted into the
 * real session KB; whatever does not is skipped and *counted*, never invented.
 * It does NOT understand open-domain prose — it lifts only the sentence shapes
 * parrot0 already knows. The honest signal is the skipped count: on real
 * SuperGLUE prose it will be high. */
static int extract_clause(Brain *b, char *clause) {
    char *c = trim_mut(clause);
    if (!*c) return 0;
    char norm[256];
    normalize(c, norm, sizeof norm);
    if (!*norm) return 0;

    char resp[256];
    if (mod_quantity(b, norm, c, resp, sizeof resp) ||
        mod_cause(b, norm, c, resp, sizeof resp) ||
        mod_same(b, norm, c, resp, sizeof resp) ||
        mod_knowledge(b, norm, c, resp, sizeof resp)) {
        return strncmp(resp, "Learned", 7) == 0; /* an assertion, not a query */
    }
    return 0;
}

static int mod_reader(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;
    if (strncmp(norm, "read:", 5) != 0) return 0;

    /* Take the passage from `raw` (not the 255-char-truncated `norm`) so long
     * passages survive; per-clause normalization lowercases/trims afterwards. */
    const char *colon = strchr(raw, ':');
    const char *passage = colon ? colon + 1 : raw;
    char buf[4096];
    size_t plen = strlen(passage);
    if (plen >= sizeof buf) plen = sizeof buf - 1;
    memcpy(buf, passage, plen);
    buf[plen] = '\0';

    size_t learned = 0, skipped = 0;
    char *p = buf;
    while (*p) {
        char *q = p;
        while (*q) {
            char ch = *q;
            if (ch == ';' || ch == '!' || ch == '?') break;
            if (ch == '.') {
                /* a decimal point between digits (1.3) is not a boundary */
                char prev = (q > p) ? q[-1] : '\0';
                if (!(isdigit((unsigned char)prev) &&
                      isdigit((unsigned char)q[1]))) break;
            }
            q++;
        }
        char saved = *q;
        *q = '\0';
        if (extract_clause(b, p)) learned++;
        else if (*trim_mut(p)) skipped++;
        if (saved == '\0') break;
        p = q + 1;
    }

    char msg[128];
    snprintf(msg, sizeof msg, "Learned %zu fact(s), skipped %zu.",
             learned, skipped);
    put(msg, out, out_size);
    return 1;
}

/* --- module: coref -------------------------------------------------------
 * Coreference decision (gen31). gen22 gave parrot0 a discourse model — a
 * pronoun resolves to the most recent concrete entity — but nothing could *ask*
 * whether two mentions co-refer. The first SuperGLUE WSC question is exactly
 * that yes/no judgement. This part answers `does <a> refer to <b>?` from the
 * existing salience state: a pronoun co-refers with `b` iff its resolved
 * antecedent is `b`; two concrete mentions co-refer iff they are the same
 * entity; a pronoun with no antecedent is admitted, not guessed. Full WSC-style
 * syntactic binding (which mention a pronoun is bound to by grammar) is out of
 * scope — we judge against the last-entity model already in place. */
static int mod_coref(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);
    if (nw != 5 || strcmp(w[0], "does") != 0 || strcmp(w[2], "refer") != 0 ||
        strcmp(w[3], "to") != 0)
        return 0;

    const char *a = w[1], *target = w[4];
    if (is_entity_pronoun(a)) {
        if (!b->has_last_entity) {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know who %s refers to.", a);
            put(msg, out, out_size);
            return 1;
        }
        put(strcmp(b->last_entity, target) == 0 ? "Yes." : "No.", out, out_size);
        return 1;
    }

    /* two concrete mentions co-refer iff they name the same entity */
    put(strcmp(a, target) == 0 ? "Yes." : "No.", out, out_size);
    return 1;
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
    {"compare",   mod_compare},
    {"arith",     mod_arith},
    {"quantity",  mod_quantity},
    {"cause",     mod_cause},
    {"same",      mod_same},
    {"conj",      mod_conj},
    {"gen",       mod_gen},
    {"coref",     mod_coref},
    {"reader",    mod_reader},
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
    return "gen37-frequency";
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

    /* Not-understood fallback. gen0 parroted the input; from gen15 the agent
     * outgrows the parrot and admits non-understanding honestly instead of
     * mirroring (TASKLIST T16). Distinguishing the *not-known* (a well-formed
     * query about something absent) from the *not-understood* (unparseable
     * input) is the remaining, subtler half left in T16. */
    size_t n = (size_t)snprintf(out, out_size,
                                "I don't understand that yet.");
    return n < out_size ? n : out_size - 1;
}
