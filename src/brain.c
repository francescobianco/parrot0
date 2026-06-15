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

static void entailment_status(Brain *tmp, const char *hyp, int explain,
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

    if (!kb_knows_pred(tmp->kb, pred)) put("Unknown.", out, out_size);
    else if (kb_is_conflicted(tmp->kb, pred, args, argc)) put("Conflicted.", out, out_size);
    else if (kb_query(tmp->kb, pred, args, argc)) {
        if (!explain) {
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
    else if (kb_is_negated(tmp->kb, pred, args, argc)) put("Contradicted.", out, out_size);
    else put("Not entailed.", out, out_size);
}

static int entailment_reply(const char *premises, const char *hypothesis,
                            int explain, char *out, size_t out_size) {
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

    entailment_status(&tmp, trim_mut((char *)hypothesis), explain, out, out_size);
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

    int explain_entailment = 0;
    char *premise_start = NULL;
    if (strncmp(buf, "explain premise:", 16) == 0) {
        explain_entailment = 1;
        premise_start = buf + 16;
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
                                explain_entailment, out, out_size);
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
    {"quantity",  mod_quantity},
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
    return "gen28-quantity-facts";
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
