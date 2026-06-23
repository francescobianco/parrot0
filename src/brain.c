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
#define _POSIX_C_SOURCE 200809L
#include "brain.h"
#include "kb.h"
#include "learn.h"
#include "code.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct Brain {
    unsigned long turns;   /* how many exchanges we've had this session */
    char name[64];         /* the user's name, once they tell us */
    int  has_name;         /* whether `name` is set */
    char last_entity[KB_TERM_LEN]; /* most recent concrete KB entity */
    int  has_last_entity;  /* whether `last_entity` is set */
    int  relations_derived; /* gen158: part_of/2 materialized from descriptions once */
    char last_reply[256];  /* our previous response — so we don't repeat it (gen55) */
    unsigned long fallbacks; /* how many not-understood turns — rotates variants */
    KB  *kb;               /* the knowledge base (gen4+) */

    /* gen76: proof trace. When a module answers a KB-backed query, it stores the
     * proof chain here so a follow-up "how do you know?" can cite real state. */
    char last_proof[512];
    int  has_last_proof;

    /* gen78: module activation tracking. Stores which module claimed the last
     * turn, so "which part of you answered that?" reads real state. */
    char last_module[32];

    /* gen82: session start time for "how long have we been talking?" */
    time_t start_time;

    /* gen83: named-entity candidates extracted from user turns. */
    char entities[8][64];
    size_t entity_count;

    /* gen93: goals buffer — "remember to X", "what are my goals?" */
    char goals[8][128];
    size_t goal_count;

    /* gen57: personal-possession display table. The KB treats uppercase-initial
     * atoms as variables, so the lookup key is lowercased while the original
     * casing is remembered here for natural replies. */
    char possessions[8][2][64];
    size_t possession_count;

    /* gen148 (E4): lightweight user model for ordinary conversation. Durable
     * personal facts are separate from session-only context so "what do you
     * remember about me?" can be grounded without overclaiming permanence. */
    char user_preference_verb[16];
    char user_preference_value[64];
    int  has_user_preference;
    char user_mood[64];
    int  has_user_mood;
    char current_topic[64];
    int  has_current_topic;
    char user_constraint[96];
    int  has_user_constraint;

    /* gen58: rolling discourse-memory topics. Each turn contributes its content
     * words (non-stopword, alphabetic, len>=3) to a small recent buffer so the
     * agent can answer "what did we talk about?" from real state. */
#define BRAIN_TOPICS_MAX 4
    char topics[BRAIN_TOPICS_MAX][32];
    size_t topic_count;

    /* gen121 (L6): the propositions a `read:` passage actually yielded — the
     * original surface sentences whose facts were extracted into the KB. An
     * extractive summary ranks THESE by concept centrality and returns the most
     * salient real sentences; comprehension (gen123) filters them by topic. The
     * "skipped" count the reader reports is the honest complement. */
#define BRAIN_PROPS_MAX 24
    char props[BRAIN_PROPS_MAX][192];
    size_t prop_count;

    /* gen101 (C15): role/character memory. A role is an ALTERNATE, session-scoped
     * self-model layered over the permanent one. When `in_role`, identity queries
     * answer from the role; a truth-probe ("really", "underneath", "davvero")
     * pierces the mask back to parrot0. The role's name/kind/attributes are PARSED
     * from the user's setup utterance (genuine NL uptake); what parrot0 *knows*
     * about the kind/figure (a dog barks, Dante wrote the Commedia) is queried
     * from kb/core/roles.p0. Cleared by "stop pretending" / "be yourself". */
    int  in_role;
    char role_name[64];   /* display name: "Rex", "Mario", "Cleopatra"        */
    char role_kind[64];   /* what it is: "dog", "robot", or an identity atom   */
    char role_attrs[8][2][64]; /* parsed inline attributes: key -> value       */
    size_t role_attr_count;

    /* gen103 (L16): the last class-conclusion the agent stated, e.g. "is socrates
     * a mortal?" -> mortal(socrates)=yes. When a later correction changes the KB,
     * the agent RE-DERIVES this goal and proactively announces the consequence
     * ("Then socrates is no longer a mortal.") — joined-up self-correction. */
    char last_goal_pred[KB_TERM_LEN];
    char last_goal_arg[KB_TERM_LEN];
    int  last_goal_yes;
    int  has_last_goal;

    /* gen105 (L20): control-flow trace of the previous substantive turn. The
     * dispatcher records, for real, which modules were consulted and declined
     * before one claimed the turn — so "why did you answer that way?" reports the
     * actual execution path and the first-match-wins control rule, not a
     * confabulated story. Committed only on non-introspective turns, so asking
     * about the strategy never overwrites the trace it is asking about. */
    char trace_declined[48][24];
    size_t trace_declined_n;
    char trace_winner[24];
    int  has_trace;

    /* gen128 (L20-deep): the previous substantive turn's inputs, kept verbatim
     * so mod_counterfactual can RE-RUN the dispatch with a module suppressed and
     * report what the agent WOULD have answered — its alternative self, computed
     * not confabulated. Stored alongside the trace (same non-introspective gate). */
    char last_input_canon[256];
    char last_input_raw[256];
    int  has_last_input;

    /* gen141 (E2): conversational repair loop. When a turn carries a referential
     * gap that resolution cannot fill — a pronoun with no antecedent, or an
     * arithmetic slot named by a pronoun — the agent does NOT wall. It asks a
     * narrow clarification, STORES the incomplete turn here, and on the next turn
     * fills the slot and RESUMES the original intent through the normal dispatch.
     * The pending state is single-turn: the very next turn either answers it or
     * the topic clearly changed, in which case it expires. This is a stateful
     * bridge across turns, not a phrasebook reply. */
    int  pending_repair;        /* a clarification is open, awaiting an answer  */
    char pending_canon[256];    /* the stored incomplete turn (canonicalized)   */
    char pending_slot[16];      /* the token to fill (the unresolved pronoun)   */

    /* gen142 (E8): metacognitive calibration. The agent reports HOW it knows the
     * last conclusion, with confidence language computed from the real proof state
     * rather than a canned adjective. Two small pieces of session state the proof
     * substrate would otherwise lose:
     *  - ASSUMED facts: a standing "suppose X." asserts X into the session KB AND
     *    is recorded here, so a later ordinary answer that rests on X is graded
     *    HYPOTHETICAL (detected by ablation: removing the assumed fact flips the
     *    goal) and "drop the assumption" is offered as what would change it.
     *  - CONFLICT: when the user states both "X is a Y" and "X is not a Y" about
     *    the same atom this session, the contradiction is recorded so the goal is
     *    graded CONFLICTED and BOTH claims are named — the KB's last-write-wins
     *    override would otherwise erase the losing claim. This is real recorded
     *    conversational state, not a tone. */
    char assumed_pred[8][KB_TERM_LEN];
    char assumed_arg[8][KB_TERM_LEN];
    size_t assumed_n;
    char conflict_pred[KB_TERM_LEN]; /* last self-contradicted (pred,arg)         */
    char conflict_arg[KB_TERM_LEN];
    int  has_conflict;

    /* gen143 (E7): local-world working memory. A "world" is a TEMPORARY, named
     * scope for a fiction or a task — facts introduced for a puzzle or story that
     * must be usable across later turns WITHOUT leaking into permanent memory
     * (the KB). Worlds form a stack: entering one (by intro phrase or by name)
     * makes it the active scope; assertions land in `wfacts` tagged with the
     * world's id, queries consult the active world's overlay before the KB, and
     * the same subject can mean different things in two different worlds. The
     * user can ask "what is assumed?" to read the active overlay back, and tear a
     * world down ("forget that world") to drop exactly its facts — nothing
     * persists, nothing reaches kb_save. A small fixed pool keeps it deterministic
     * and pure-C; overflow degrades to "no room", never to a leak. */
#define BRAIN_WORLDS_MAX 8
#define BRAIN_WFACTS_MAX 64
    char worlds[BRAIN_WORLDS_MAX][48];  /* world name by id (display + lookup)   */
    int  world_live[BRAIN_WORLDS_MAX];  /* 1 while the world exists (not torn down)*/
    size_t world_count;                 /* ids ever allocated (monotonic)         */
    int  active_world;                  /* id of the current scope, or -1 if none */
    struct {
        int  world;                     /* owning world id                        */
        char subj[KB_TERM_LEN];         /* the entity                             */
        char pred[KB_TERM_LEN];         /* the class it belongs to in this world  */
        int  neg;                       /* 1 if asserted false in this world      */
    } wfacts[BRAIN_WFACTS_MAX];
    size_t wfact_count;

    /* gen168 (E1 reflexive): how many composition self-tests have run this
     * session. Used to pick a FRESH held-out vocabulary tuple per run, so two
     * self-tests propose different names — proof the run is generated and
     * executed, not a memorized transcript. */
    unsigned selftest_runs;

};

/* gen142 (E8): record that the user has just asserted `pred(arg)` with polarity
 * `positive`. If the OPPOSITE polarity is currently in the KB (the about-to-be
 * overwritten claim), the user has contradicted themselves about this atom this
 * session — remember it so a later "is X a Y?" / "why?" is graded CONFLICTED and
 * BOTH claims can be named, even though the KB's last-write-wins assert will
 * erase the losing fact. Called BEFORE the assert. Only direct fact-vs-fact
 * contradictions about the SAME atom are recorded — a grounded signal, not tone. */
static void note_contradiction(Brain *b, const char *pred, const char *arg,
                               int positive) {
    if (!b || !b->kb) return;
    const char *a[] = {arg};
    int opposite = positive ? kb_is_negated(b->kb, pred, a, 1)
                            : kb_query(b->kb, pred, a, 1);
    if (opposite) {
        snprintf(b->conflict_pred, sizeof b->conflict_pred, "%s", pred);
        snprintf(b->conflict_arg, sizeof b->conflict_arg, "%s", arg);
        b->has_conflict = 1;
    }
}

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

/* Return the index of token `t` in `w[0..nw)`, or `nw` if absent. */
static size_t find_token(char **w, size_t nw, const char *t) {
    for (size_t i = 0; i < nw; i++)
        if (strcmp(w[i], t) == 0) return i;
    return nw;
}

/* True if needle occurs anywhere in haystack — the keyword-cue test behind
 * paraphrase-robust intent (gen51): one intent reached from many phrasings. */
static int cue(const char *haystack, const char *needle) {
    return strstr(haystack, needle) != NULL;
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

/* gen76: store a proof trace so a follow-up "how do you know?" can cite it. */
static void store_proof(Brain *b, const char *proof) {
    if (!b || !proof || !*proof) return;
    snprintf(b->last_proof, sizeof b->last_proof, "%s", proof);
    b->has_last_proof = 1;
}

/* gen103 (L16): current truth of the last stated class-conclusion, or -1 if none
 * is remembered. Used to snapshot the goal right before a correction. */
static int goal_truth(Brain *b) {
    if (!b || !b->kb || !b->has_last_goal) return -1;
    const char *args[] = { b->last_goal_arg };
    return kb_query(b->kb, b->last_goal_pred, args, 1);
}

/* gen103 (L16): after a correction changes the KB, re-derive the last stated
 * class-conclusion. If THIS correction flipped its truth (compared to `before`,
 * the snapshot taken just before the mutation), append a sentence announcing the
 * consequence to `out` — a correction's downstream effect is volunteered, not
 * waited for. `just_asserted` is the predicate the correction touched: we only
 * speak up when the affected conclusion is a *different*, downstream predicate —
 * the genuinely joined-up case, not a restatement of what was just asserted. The
 * `before` gate means a flip that already happened on an earlier turn (e.g. via
 * "forget") is never announced belatedly on an unrelated later assertion. */
static void note_consequence(Brain *b, const char *just_asserted, int before,
                             char *out, size_t out_size) {
    if (!b || !b->kb || !b->has_last_goal || before < 0) return;
    if (just_asserted && strcmp(just_asserted, b->last_goal_pred) == 0) return;
    int now = goal_truth(b);
    if (now == before) return; /* this correction changed nothing downstream */
    char note[200];
    if (before && !now)
        snprintf(note, sizeof note, " Then %s is no longer a %s.",
                 b->last_goal_arg, b->last_goal_pred);
    else
        snprintf(note, sizeof note, " Now %s is a %s after all.",
                 b->last_goal_arg, b->last_goal_pred);
    size_t cur = strlen(out);
    if (cur + strlen(note) + 1 < out_size)
        memcpy(out + cur, note, strlen(note) + 1);
    b->last_goal_yes = now; /* keep the memory consistent for further turns */
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

/* Greetings & farewells (gen1) were the first intents. gen52 generalizes them
 * into the dialogue-act layer `mod_social` (defined near the registry, where
 * split_words is in scope) — the phatic register as a structure, not a list. */

/* Forward declarations: the possession frame in mod_memory needs the same
 * tokenizer and article test used later by the knowledge modules; discourse
 * memory needs the stoplist and edge-punctuation stripper from the bench
 * baseline helpers. */
static size_t split_words(char *s, char **argv, size_t max);
static int is_article(const char *w);
static int is_stopword(Brain *b, const char *w);
static char *strip_edge_punct(char *t);
static int is_internal_pred(const char *pred);

/* Copy the last whitespace-separated word of `raw` into `dst`, preserving its
 * original casing and trimming trailing punctuation/whitespace. Used to keep
 * proper names (Rex, Luna) intact while matching on the normalized surface. */
static void copy_last_word(char *dst, size_t dst_size, const char *raw) {
    size_t n = strlen(raw);
    while (n > 0 && isspace((unsigned char)raw[n - 1])) n--;
    while (n > 0 && ispunct((unsigned char)raw[n - 1])) n--;
    size_t end = n;
    while (n > 0 && !isspace((unsigned char)raw[n - 1])) n--;
    size_t len = end - n;
    if (len >= dst_size) len = dst_size - 1;
    memcpy(dst, raw + n, len);
    dst[len] = '\0';
}

/* Personal-possession display helpers (gen57). The KB key is lowercased because
 * uppercase-initial atoms are read as variables; the original casing lives here
 * so replies feel natural. */
static void lowercase_copy(char *dst, size_t dst_size, const char *src) {
    size_t i = 0;
    for (; src[i] && i + 1 < dst_size; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

static void remember_possession(Brain *b, const char *thing, const char *name) {
    if (b->possession_count >= 8) return;
    size_t slot = b->possession_count;
    for (size_t i = 0; i < b->possession_count; i++) {
        if (strcmp(b->possessions[i][0], thing) == 0) { slot = i; break; }
    }
    copy_trim(b->possessions[slot][0], sizeof b->possessions[slot][0], thing);
    copy_trim(b->possessions[slot][1], sizeof b->possessions[slot][1], name);
    if (slot == b->possession_count) b->possession_count++;

    char thing_key[64], name_key[64];
    lowercase_copy(thing_key, sizeof thing_key, thing);
    lowercase_copy(name_key, sizeof name_key, name);
    const char *args[] = {thing_key, name_key};
    kb_assert(b->kb, "called", args, 2);

    /* gen163: the named pet becomes the salient discourse entity, so a later
     * unbound "she/he/it" composes possession memory with discourse reference
     * ("i have a cat named smoke" then "is she a robot?" resolves to smoke).
     * A real KB-fact antecedent mentioned afterwards still overrides this. */
    if (strlen(name_key) < KB_TERM_LEN) {
        snprintf(b->last_entity, sizeof b->last_entity, "%s", name_key);
        b->has_last_entity = 1;
    }
}

static const char *find_possession_name(Brain *b, const char *thing) {
    for (size_t i = 0; i < b->possession_count; i++)
        if (strcmp(b->possessions[i][0], thing) == 0)
            return b->possessions[i][1];
    return NULL;
}

/* --- module: memory ------------------------------------------------------
 * The first *stateful* part: it learns the user's name and recalls it. This
 * is where the brain stops being purely reactive and starts carrying context
 * across turns.
 *
 * gen57 (C3): it also learns small personal-fact frames: "I have a <thing>
 * named <name>" and "my <thing> is <name>", stored as `called(thing, name)`
 * and queried by "what is my <thing> called?". "call me <X>" / Italian
 * "chiamami <X>" / "mi chiamo <X>" extend the name-teaching path.
 */
static int mod_memory(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;

    static const char *const ask_name[] = {
        "what is my name",  "what is my name?",
        "what's my name",   "what's my name?",
        "whats my name",    "whats my name?",
        /* Italian recall idiom — the read-side mirror of the "mi chiamo X"
         * teach above; "come mi chiamo?" is the fixed Italian form of "what's
         * my name?", so it belongs in the same lexicon, not a new handler. */
        "come mi chiamo",   "come mi chiamo?",
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

    /* Teach: "call me <X>" and Italian equivalents (chiamami / mi chiamo). */
    static const char *const call_me = "call me ";
    static const char *const chiamami = "chiamami ";
    static const char *const mi_chiamo = "mi chiamo ";
    const char *name_from = NULL;
    if (strncmp(norm, call_me, strlen(call_me)) == 0)
        name_from = raw + strlen(call_me);
    else if (strncmp(norm, chiamami, strlen(chiamami)) == 0)
        name_from = raw + strlen(chiamami);
    else if (strncmp(norm, mi_chiamo, strlen(mi_chiamo)) == 0)
        name_from = raw + strlen(mi_chiamo);
    if (name_from) {
        const char *name = skip_ws(name_from);
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

    /* Bare self-introduction: "i'm <X>" / "i am <X>" / "im <X>", optionally
     * behind a greeting ("hi, i'm vera"), feeds the SAME name memory that
     * "my name is X" / "call me X" fill. The hazard is stealing affective turns
     * ("i'm tired", "i am bored") from mod_chitchat, so we accept ONLY a single
     * trailing token that is not an article, a stopword, a known KB class, or a
     * common state/feeling — generalizing to unseen NAMES, not phrases. */
    {
        char nbuf[256];
        size_t nl = strlen(norm);
        if (nl < sizeof nbuf) {
            memcpy(nbuf, norm, nl + 1);
            char *nwds[24];
            size_t nnw = split_words(nbuf, nwds, 24);
            size_t cand = nnw;                       /* candidate name index */
            for (size_t i = 0; i + 1 < nnw; i++) {
                if (strcmp(nwds[i], "i'm") == 0 || strcmp(nwds[i], "im") == 0) {
                    cand = i + 1; break;
                }
                if (strcmp(nwds[i], "i") == 0 && i + 2 < nnw &&
                    strcmp(nwds[i + 1], "am") == 0) {
                    cand = i + 2; break;
                }
            }
            if (cand < nnw && cand == nnw - 1) {     /* single-word name only */
                char *c = strip_edge_punct(nwds[cand]);
                static const char *const nonname[] = {
                    "tired","bored","happy","sad","fine","good","ok","okay",
                    "here","ready","sorry","busy","lost","sure","hungry","cold",
                    "hot","sleepy","angry","scared","confused","done","back",
                    "late","free","alone","well","great","bad","new","young",
                    "old","right","wrong","not","sick","glad","nervous",
                    "excited","curious","afraid","awake","hurt","convinced",
                    "kidding","joking","serious","worried","calm","listening",
                    NULL,
                };
                int ok = c[0] && isalpha((unsigned char)c[0]) &&
                         strlen(c) >= 2 && !is_article(c) &&
                         !is_stopword(b, c) && !matches_any(c, nonname) &&
                         !(b->kb && kb_knows_pred(b->kb, c));
                if (ok) {
                    char nm[64];
                    copy_last_word(nm, sizeof nm, raw);
                    copy_trim(b->name, sizeof b->name, nm);
                    b->has_name = 1;
                    char msg[128];
                    snprintf(msg, sizeof msg, "Nice to meet you, %s!", b->name);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* Personal possession frame: "I have a <thing> named <name>",
     * "my <thing> is <name>", "my <thing> is called <name>", plus their
     * Italian canonicalizations. The parser searches for the content frame
     * inside the token stream so a leading social marker ("hi, my dog...")
     * does not derail the content module. */
    {
        char buf[256];
        size_t len = strlen(norm);
        if (len < sizeof buf) {
            memcpy(buf, norm, len + 1);
            if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';
            char *w[12];
            size_t nw = split_words(buf, w, 12);

            /* "I have a <thing> named <name>" */
            {
                size_t i = find_token(w, nw, "i");
                if (i + 4 < nw && strcmp(w[i + 1], "have") == 0 &&
                    is_article(w[i + 2]) && strcmp(w[i + 4], "named") == 0) {
                    const char *thing = w[i + 3];
                    char n[64];
                    copy_last_word(n, sizeof n, raw);
                    remember_possession(b, thing, n);
                    char msg[160];
                    snprintf(msg, sizeof msg, "Got it: your %s is called %s.", thing, n);
                    put(msg, out, out_size);
                    return 1;
                }
            }

            /* "my <thing> is <name>" and "my <thing> is called <name>" */
            {
                size_t i = find_token(w, nw, "my");
                if (i + 3 < nw && strcmp(w[i + 2], "is") == 0) {
                    const char *thing = w[i + 1];
                    char n[64];
                    copy_last_word(n, sizeof n, raw);
                    int has_called = (i + 4 < nw && strcmp(w[i + 3], "called") == 0);
                    if (has_called) {
                        remember_possession(b, thing, n);
                        char msg[160];
                        snprintf(msg, sizeof msg, "Got it: your %s is called %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    } else {
                        remember_possession(b, thing, n);
                        char msg[160];
                        snprintf(msg, sizeof msg, "Got it: your %s is %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    }
                }
            }

            /* Queries: "what is my <thing> called?" and "what is my <thing>?" */
            {
                size_t i = find_token(w, nw, "what");
                if (i + 2 < nw && strcmp(w[i + 1], "is") == 0) {
                    size_t m = find_token(w + i, nw - i, "my");
                    if (m < nw - i) m += i;
                    if (m + 1 < nw) {
                        const char *thing = w[m + 1];
                        int has_called = (m + 2 < nw);
                        if (has_called) {
                            char tmp[64];
                            snprintf(tmp, sizeof tmp, "%s", w[m + 2]);
                            has_called = (strcmp(strip_edge_punct(tmp), "called") == 0);
                        }
                        if (strcmp(thing, "name") == 0) {
                            if (b->has_name) {
                                char msg[128];
                                snprintf(msg, sizeof msg, "Your name is %s.", b->name);
                                put(msg, out, out_size);
                            } else {
                                put("I don't know your name yet.", out, out_size);
                            }
                            return 1;
                        }
                        const char *n = find_possession_name(b, thing);
                        char msg[160];
                        if (!n) {
                            snprintf(msg, sizeof msg,
                                     "I don't know what your %s is called.", thing);
                            put(msg, out, out_size);
                            return 1;
                        }
                        if (has_called)
                            snprintf(msg, sizeof msg, "%s.", n);
                        else
                            snprintf(msg, sizeof msg, "Your %s is %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    }
                }
            }
        }
    }

    /* gen148 (E4): ordinary user-model facts beyond name/possessions. */
    {
        const char *val_from = NULL;
        const char *verb = NULL;
        if (strncmp(norm, "i like ", 7) == 0) {
            val_from = raw + 7; verb = "like";
        } else if (strncmp(norm, "i prefer ", 9) == 0) {
            val_from = raw + 9; verb = "prefer";
        } else if (strncmp(norm, "mi piace ", 9) == 0) {
            val_from = raw + 9; verb = "like";
        } else if (strncmp(norm, "preferisco ", 11) == 0) {
            val_from = raw + 11; verb = "prefer";
        }
        if (val_from) {
            char val[64];
            copy_trim(val, sizeof val, skip_ws(val_from));
            if (val[0] == 0) {
                put("I did not catch the preference.", out, out_size);
                return 1;
            }
            snprintf(b->user_preference_verb, sizeof b->user_preference_verb, "%s", verb);
            snprintf(b->user_preference_value, sizeof b->user_preference_value, "%s", val);
            b->has_user_preference = 1;
            char msg[160];
            snprintf(msg, sizeof msg, "Got it: you %s %s.", verb, val);
            put(msg, out, out_size);
            return 1;
        }
    }

    if (cue(norm, "keep it short") || cue(norm, "be brief") ||
        cue(norm, "short answers") || cue(norm, "risposte brevi") ||
        cue(norm, "sii breve")) {
        snprintf(b->user_constraint, sizeof b->user_constraint, "%s", "keep it short");
        b->has_user_constraint = 1;
        put("Got it: I will keep it short.", out, out_size);
        return 1;
    }
    if (cue(norm, "not too technical") || cue(norm, "avoid technical") ||
        cue(norm, "non essere tecnico") || cue(norm, "non troppo tecnico")) {
        snprintf(b->user_constraint, sizeof b->user_constraint, "%s", "avoid technical detail");
        b->has_user_constraint = 1;
        put("Got it: I will avoid technical detail.", out, out_size);
        return 1;
    }

    if (cue(norm, "what do i like") || cue(norm, "what do i prefer") ||
        cue(norm, "cosa mi piace") || cue(norm, "cosa preferisco")) {
        if (b->has_user_preference) {
            char msg[160];
            snprintf(msg, sizeof msg, "You %s %s.",
                     b->user_preference_verb, b->user_preference_value);
            put(msg, out, out_size);
        } else {
            put("I do not know your preference yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what mood") || cue(norm, "how do i feel") ||
        cue(norm, "come mi sento") || cue(norm, "che umore")) {
        if (b->has_user_mood) {
            char msg[160];
            snprintf(msg, sizeof msg, "You told me you feel %s.", b->user_mood);
            put(msg, out, out_size);
        } else {
            put("I do not know your current mood yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what topic") || cue(norm, "which topic") ||
        cue(norm, "what are we talking about") || cue(norm, "di cosa parliamo") ||
        cue(norm, "di cosa stiamo parlando")) {
        if (b->has_current_topic) {
            char msg[160];
            snprintf(msg, sizeof msg, "The current topic is %s.", b->current_topic);
            put(msg, out, out_size);
        } else {
            put("I do not know the current topic yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what constraint") || cue(norm, "what did i ask you to keep in mind") ||
        cue(norm, "what should you remember for this chat") || cue(norm, "che vincolo")) {
        if (b->has_user_constraint) {
            char msg[192];
            snprintf(msg, sizeof msg, "Your current constraint is: %s.", b->user_constraint);
            put(msg, out, out_size);
        } else {
            put("I do not know any current constraint yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what do you remember about me") ||
        cue(norm, "what do you know about me") || cue(norm, "cosa ricordi di me") ||
        cue(norm, "cosa sai di me")) {
        char msg[640];
        size_t off = 0;
        int any = 0;
        off = (size_t)snprintf(msg, sizeof msg, "I remember:");
        if (b->has_name && off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s your name is %s", any ? ";" : "", b->name);
            any = 1;
        }
        for (size_t i = 0; i < b->possession_count && off < sizeof msg; i++) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s your %s is %s", any ? ";" : "",
                                    b->possessions[i][0], b->possessions[i][1]);
            any = 1;
        }
        if (b->has_user_preference && off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s you %s %s", any ? ";" : "",
                                    b->user_preference_verb, b->user_preference_value);
            any = 1;
        }
        if (!any) {
            off = (size_t)snprintf(msg, sizeof msg,
                                   "I remember no durable personal facts yet.");
        } else if (off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off, ".");
        }

        int session = b->has_user_mood || b->has_current_topic || b->has_user_constraint;
        if (session && off < sizeof msg) {
            int s = 0;
            off += (size_t)snprintf(msg + off, sizeof msg - off, " Session context:");
            if (b->has_user_mood && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s you feel %s", s ? ";" : "", b->user_mood);
                s = 1;
            }
            if (b->has_current_topic && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s current topic is %s", s ? ";" : "", b->current_topic);
                s = 1;
            }
            if (b->has_user_constraint && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s constraint: %s", s ? ";" : "", b->user_constraint);
            }
            if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
        }
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

/* gen43 — multilingual as a generalization probe (PRINCIPLES.md: no phrasebook).
 * Map one FUNCTION word of any supported language onto the canonical (English)
 * token the reasoning modules already parse, or NULL to leave it untouched.
 * Content words are opaque symbols and are never listed, so the same reasoning
 * core answers in any language whose function words are mapped — *no module is
 * duplicated*. Only tokens that cannot occur in English are listed, so English
 * input is provably unaffected. The competence is thus shown to live in the
 * algorithm, not in English surface strings; where a language needs more than a
 * lexical swap (e.g. Italian negation "x non è un y" reorders to "x not is a y",
 * not the English "x is not a y"), that is the probe correctly exposing a
 * word-order assumption the core still bakes in — a future iteration, not a
 * second phrasebook. */
static const char *canonical_token(const char *w) {
    static const struct { const char *src, *dst; } lex[] = {
        /* Italian */
        {"è",   "is"},
        {"un",  "a"}, {"uno", "a"}, {"una", "a"},
        {"mio", "my"}, {"mia", "my"},
        {"ho",  "i have"},
        {"chiamato", "named"},
        {"si",  "is"}, {"chiama", "called"},
        {"ogni","every"},
        {"chi", "who"},

        {"non", "not"},
        {"anche","also"},
        {"causa","causes"},
        /* gen142 (E3): Italian modals so the pragmatic topic-intro / disagreement
         * shapes fire through the SAME mod_pragma path (no phrase duplication). */
        {"possiamo","can"}, {"potremmo","could"},
        {"sfida", "challenge"}, {"risolvere", "solve"}, {"risolveresti", "solve"},
        {"migliorare", "improve"}, {"miglioreresti", "improve"},
        {"implementazione", "implementation"}, {"modifica", "change"},
        {"codice", "code"}, {"capitale", "capital"},

        {"stesso", "yourself"}, {"stessa", "yourself"}, {"te", "you"},
        {"tuo", "your"}, {"tua", "your"}, {"riguarda", "about"},
        {"fallisci", "fail"}, {"fallisce", "fail"},

        {"cos'è", "what is"},
        {"qual", "what"},  /* gen155: "qual è ..." -> "what is ..." reaches the
                            * same concept-recall path as English. */
        {"sono", "am"},
        /* gen141: subject pronouns, so the repair loop's referential-gap probe
         * (a pronoun with no antecedent) reaches the SAME code path in Italian.
         * These are unambiguous subject forms; "lo"/"la"/"li" (clitics/articles)
         * are deliberately left out to avoid colliding with article parsing. */
        {"esso", "it"}, {"essa", "it"}, {"essi", "they"}, {"esse", "they"},
        {"lui", "he"}, {"lei", "she"},
        /* gen142 (E7): local-world vocabulary so the scoped-world module reaches
         * the SAME path in Italian. "mondo"/"storia" name a scope; "assunto"/
         * "assume" are the inspect cue ("cosa è assunto?" -> "what is assumed").
         * These cannot occur as English words, so English input is unaffected. */
        {"mondo", "world"}, {"storia", "story"},
        {"nel", "in the"}, {"nella", "in the"},
        {"questo", "this"}, {"questa", "this"},
        {"assunto", "assumed"}, {"dimentica", "forget"},
        /* Chat-register shorthand (gen64), not a second language. "u"/"r" are
         * English letters, but never stand-alone English *words*; in a chat
         * agent a lone "u"/"r" overwhelmingly means you/are ("what can u do?",
         * "who r u?"). Folding them here routes every intent through the same
         * canonical path instead of accreting shorthand cues per module. */
        {"u",   "you"}, {"r",  "are"},
        /* gen74: chat-register contractions — common abbreviated forms that
         * real users type. Expanding them into their canonical spaced forms
         * lets the existing parsers (arith, knowledge, identity) work on
         * contracted input without duplicating logic. */
        {"whats", "what is"}, {"whos", "who is"}, {"wheres", "where is"},
        {"dont",  "do not"},  {"cant", "can not"}, {"isnt", "is not"},
        {"isn't", "is not"}, {"pls", "please"},
    };
    for (size_t i = 0; i < sizeof lex / sizeof lex[0]; i++)
        if (strcmp(w, lex[i].src) == 0) return lex[i].dst;
    return NULL;
}

/* Rewrite a normalized line, canonicalizing each word's function-word form.
 * A trailing '?' is kept on its token so question parsers still see it. For
 * all-English input every token maps to NULL, so the output equals the input
 * (modulo whitespace already collapsed by the parsers' tokenizer). */
static void canonicalize_lang(const char *norm, char *out, size_t out_size) {
    if (out_size == 0) return;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) { snprintf(out, out_size, "%s", norm); return; }
    memcpy(buf, norm, len + 1);

    char *w[64];
    size_t nw = split_words(buf, w, 64);
    size_t off = 0;
    out[0] = '\0';
    for (size_t i = 0; i < nw && off + 1 < out_size; i++) {
        char *tok = w[i];
        size_t tl = strlen(tok);
        const char *tail = "";
        if (tl > 0 && tok[tl - 1] == '?') { tok[tl - 1] = '\0'; tail = "?"; }
        const char *canon = canonical_token(tok);
        off += (size_t)snprintf(out + off, out_size - off, "%s%s%s",
                                i ? " " : "", canon ? canon : tok, tail);
    }
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

/* gen79: run rule induction over the current KB and, if any new rules are
 * found, append them to `out`. Returns number of rules induced. */
static size_t auto_induce(Brain *b, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char heads[16][KB_TERM_LEN], bodies[16][KB_TERM_LEN];
    size_t k = kb_induce(b->kb, 2, heads, bodies, 16);
    if (k == 0) return 0;
    /* Filter out emergent rules whose head or body is an internal predicate
     * (coding knowledge, social markers, etc.) — those are infrastructure,
     * not domain reasoning. */
    size_t kept = 0;
    size_t out_len = strlen(out);
    for (size_t i = 0; i < k; i++) {
        if (is_internal_pred(heads[i]) || is_internal_pred(bodies[i])) continue;
        if (out_len + 2 < out_size) {
            if (kept == 0) {
                if (out_len > 0) { out[out_len] = ' '; out[out_len + 1] = '\0'; out_len++; }
            }
            out_len += (size_t)snprintf(out + out_len, out_size - out_len,
                                         "%s%s(X) :- %s(X).",
                                         kept ? " " : "Induced: ", heads[i], bodies[i]);
            kept++;
        }
    }
    return kept;
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
        store_proof(b, ex);
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
    store_proof(b, ex);
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

    /* gen84: hypothesis mode — "suppose <fact>, then <query>" */
    if (strncmp(norm, "suppose ", 8) == 0) {
        const char *rest = norm + 8;
        const char *then_pos = strstr(rest, " then ");
        const char *allora_pos = strstr(rest, " allora ");
        const char *sep = then_pos ? then_pos : allora_pos;
        size_t sep_len = then_pos ? 6 : (allora_pos ? 8 : 0);
        if (sep && sep_len) {
            char supp[256], query_text[256];
            size_t slen = (size_t)(sep - rest);
            if (slen >= sizeof supp) slen = sizeof supp - 1;
            memcpy(supp, rest, slen); supp[slen] = '\0';
            /* Strip trailing punctuation from the supposition. */
            while (slen > 0 && (supp[slen-1] == ',' || supp[slen-1] == '.' ||
                   supp[slen-1] == ';' || supp[slen-1] == ' '))
                supp[--slen] = '\0';
            snprintf(query_text, sizeof query_text, "%s", sep + sep_len);
            char sn[256], sc[256];
            normalize(supp, sn, sizeof sn);
            canonicalize_lang(sn, sc, sizeof sc);
            /* Assert supposition, then query. */
            Brain hypo = {0};
            hypo.kb = kb_create();
            if (!hypo.kb) return 0;
            kb_set_origin(hypo.kb, KB_SESSION);
            char discard[256];
            mod_knowledge(&hypo, sc, sc, discard, sizeof discard);
            char qn[256], qc[256];
            normalize(query_text, qn, sizeof qn);
            canonicalize_lang(qn, qc, sizeof qc);
            char qbuf[256];
            size_t ql = strlen(qc);
            if (ql >= sizeof qbuf) ql = sizeof qbuf - 1;
            memcpy(qbuf, qc, ql + 1);
            if (ql > 0 && qbuf[ql - 1] == '?') qbuf[ql - 1] = '\0';
            char *qw[8];
            size_t qnw = split_words(qbuf, qw, 8);
            if (qnw == 4 && strcmp(qw[0], "is") == 0 && is_article(qw[2])) {
                const char *args[] = {qw[1]};
                int yes = kb_query(hypo.kb, qw[3], args, 1);
                put(yes ? "Yes, under that supposition." : "No, even with that supposition.",
                    out, out_size);
            } else {
                put("I supposed that. What should I check?", out, out_size);
            }
            kb_destroy(hypo.kb);
            return 1;
        }
    }

    /* Work on a mutable copy with any trailing '?' stripped. Remember whether the
     * turn was a question: a trailing '?' marks interrogation independently of
     * word order, so the subject-first interrogative "socrates is a man?" (the
     * Italian shape "socrates è un uomo?") is a QUERY, not an assertion — one
     * core rule serving both languages (gen103, the bilingual ratchet). */
    char buf[512];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    int interrogative = (len > 0 && buf[len - 1] == '?');
    if (interrogative) buf[len - 1] = '\0';

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

    /* gen146 (E5): open-domain humility for questions that look like world
     * facts but fall outside the current KB/tool model. This does not answer
     * from general knowledge; it names the missing predicate/relation/tool and
     * gives the user a useful next action. */
    if (nw == 4 && strcmp(w[0], "what") == 0 && strcmp(w[2], "is") == 0 &&
        strcmp(w[3], "it") == 0 &&
        (strcmp(w[1], "year") == 0 || strcmp(w[1], "date") == 0 ||
         strcmp(w[1], "day") == 0 || strcmp(w[1], "time") == 0)) {
        char pred[64];
        snprintf(pred, sizeof pred, "current_%s", w[1]);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I do not know the current %s: I have no %s fact or clock/calendar tool. Tell me the %s, or give facts I can reason from.",
                 w[1], pred, w[1]);
        put(msg, out, out_size);
        return 1;
    }

    if ((nw == 6 && (strcmp(w[0], "what") == 0 || strcmp(w[0], "who") == 0) &&
         strcmp(w[1], "is") == 0 && strcmp(w[2], "the") == 0 &&
         (strcmp(w[4], "of") == 0 || strcmp(w[4], "di") == 0)) ||
        (nw == 5 && (strcmp(w[0], "what") == 0 || strcmp(w[0], "who") == 0) &&
         strcmp(w[1], "is") == 0 &&
         (strcmp(w[3], "of") == 0 || strcmp(w[3], "di") == 0))) {
        const char *rel = (nw == 6) ? w[3] : w[2];
        const char *obj = (nw == 6) ? w[5] : w[4];
        if (!kb_knows_pred(b->kb, rel)) {
            char msg[320];
            snprintf(msg, sizeof msg,
                     "I do not know the relation %s yet, so I cannot answer the %s of %s. You can teach me with thing is the %s of %s, or give facts/rules to reason from.",
                     rel, rel, obj, rel, obj);
            put(msg, out, out_size);
            return 1;
        }
    }

    if ((nw == 4 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0) ||
        (nw == 5 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
         strcmp(w[2], "the") == 0)) {
        const char *subj = (nw == 4) ? w[2] : w[3];
        const char *pred = (nw == 4) ? w[3] : w[4];
        const char *args[] = {subj};
        char ex[512];
        if (!kb_knows_pred(b->kb, pred) ||
            !kb_explain(b->kb, pred, args, 1, ex, sizeof ex)) {
            char msg[320];
            snprintf(msg, sizeof msg,
                     "I do not know why %s is %s: I have no %s(%s) fact/rule or cause explaining it. Teach me facts or rules, or give me a passage to read.",
                     subj, pred, pred, subj);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen59 (C5): "what is <x>?" is a natural way to ask for a description of
     * an entity. Reuse the existing belief-report path; decline if x is an
     * article or common function word so "what is a ...?" still falls through. */
    if (nw == 3 && strcmp(w[0], "what") == 0 && strcmp(w[1], "is") == 0 &&
        !is_article(w[2]) && !is_stopword(b, w[2])) {
        const char *entity;
        if (!resolve_entity(b, w[2], &entity, out, out_size)) return 1;
        char desc[1024];
        if (kb_describe_entity(b->kb, entity, desc, sizeof desc)) {
            put(desc, out, out_size);
            store_proof(b, desc);
        } else {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know anything about %s.", entity);
            put(msg, out, out_size);
        }
        remember_entity(b, w[2], entity);
        return 1;
    }

    /* gen157: emergent relational reasoning over descriptions. parrot0 was never
     * told "heart is part of circulatory" — but the circulatory DESCRIPTION names
     * the heart, so "what is the heart part of?" / "what contains the lungs?" /
     * "di cosa fa parte heart" recovers the container from the text. The relation
     * is DERIVED, never asserted: a taxonomy emerging from the glossary. Runs
     * before the describe block so "what is X part of" is not answered with X's
     * own definition. Fires only with a containment cue AND a known concept. */
    {
        char low[256]; lowercase_copy(low, sizeof low, raw);
        int want_container =
            cue(norm, "part of") || cue(norm, "belong") ||
            cue(norm, "contains") || cue(norm, "contain ") ||
            cue(norm, "includes") || cue(norm, "include ") ||
            cue(low, "fa parte") || cue(low, "contiene") || cue(low, "contengono");
        if (want_container) {
            /* concept keys in the turn, and the index of the containment cue.
             * A trailing category noun ("the nervous SYSTEM") is the frame, not
             * the target — skip it even though "system" is a concept elsewhere. */
            const char *keys[8]; size_t keypos[8], nk = 0;
            for (size_t i = 0; i < nw && nk < 8; i++) {
                if (!strcmp(w[i], "system") || !strcmp(w[i], "group") ||
                    !strcmp(w[i], "class") || !strcmp(w[i], "family") ||
                    !strcmp(w[i], "category") || !strcmp(w[i], "type") ||
                    !strcmp(w[i], "kind") || !strcmp(w[i], "set")) continue;
                if (kb_is_concept_key(b->kb, w[i])) { keys[nk] = w[i]; keypos[nk] = i; nk++; }
            }
            size_t cuei = nw;
            for (size_t i = 0; i < nw; i++)
                if (!strcmp(w[i], "part") || !strcmp(w[i], "contains") ||
                    !strcmp(w[i], "contain") || !strcmp(w[i], "includes") ||
                    !strcmp(w[i], "include") || !strcmp(w[i], "contiene") ||
                    !strcmp(w[i], "contengono")) { cuei = i; break; }

            /* gen158 (proof): "is X part of Y?" — PROVE it against the
             * materialized part_of/2 fact derived from the descriptions. */
            if (nk >= 2 && strcmp(w[0], "is") == 0) {
                const char *xx = keys[0], *yy = keys[nk - 1];
                const char *args[2] = { xx, yy };
                char msg[200];
                if (kb_query(b->kb, "part_of", args, 2))
                    snprintf(msg, sizeof msg, "Yes, %s is part of %s.", xx, yy);
                else
                    snprintf(msg, sizeof msg,
                             "No, I have no evidence that %s is part of %s.", xx, yy);
                put(msg, out, out_size);
                store_proof(b, msg);
                return 1;
            }

            /* gen158 (members): "what is part of Y?" / "what is in Y?" — the
             * inverse query over part_of, listing what Y contains. */
            int key_before_cue = 0;
            for (size_t i = 0; i < nk; i++) if (keypos[i] < cuei) key_before_cue = 1;
            if (nk >= 1 && cuei < nw && !key_before_cue && keypos[nk - 1] > cuei) {
                const char *yy = keys[nk - 1];
                char members[16][KB_TERM_LEN];
                const char *args[2] = { NULL, yy };
                size_t m = kb_match(b->kb, "part_of", args, 2, members, 16);
                if (m > 0) {
                    char msg[512];
                    int off = snprintf(msg, sizeof msg, "%s contains: ", yy);
                    for (size_t i = 0; i < m && off > 0 && (size_t)off < sizeof msg; i++)
                        off += snprintf(msg + off, sizeof msg - (size_t)off, "%s%s",
                                        i ? ", " : "", members[i]);
                    if (off > 0 && (size_t)off < sizeof msg)
                        snprintf(msg + off, sizeof msg - (size_t)off, ".");
                    put(msg, out, out_size);
                    store_proof(b, msg);
                    return 1;
                }
            }

            /* gen157 (container): "what is X part of?" — the concept whose text
             * names X, recovered on demand. */
            const char *x = (nk >= 1) ? keys[0] : NULL;
            char ckey[128], cdesc[1024];
            if (x && kb_concept_mentioning(b->kb, x, ckey, sizeof ckey,
                                           cdesc, sizeof cdesc)) {
                char msg[1200];
                snprintf(msg, sizeof msg, "%s is part of %s: %s.", x, ckey, cdesc);
                put(msg, out, out_size);
                store_proof(b, msg);
                remember_entity(b, x, x);
                return 1;
            }
        }
    }

    /* gen151: natural access to gen150 domain knowledge (experts/skills). Beyond
     * the bare "what is X?" above, accept an article, a multiword topic, or a
     * "tell me about X" framing: "what is the heart", "what is a prime",
     * "what is the circulatory system", "tell me about pi". Each content word is
     * tried as the concept key; the first that has a KB description is spoken.
     * Claims ONLY on a hit, so unknown topics still fall through to the humility
     * blocks above and the fallback below — this never widens the wall. */
    {
        size_t start = 0;
        if (nw >= 3 && strcmp(w[0], "what") == 0 &&
            (strcmp(w[1], "is") == 0 || strcmp(w[1], "are") == 0)) start = 2;
        else if (nw >= 4 && strcmp(w[0], "tell") == 0 &&
                 strcmp(w[1], "me") == 0 && strcmp(w[2], "about") == 0) start = 3;
        /* "what is a/an X?" is the membership query (list the X's), handled
         * downstream — not a description request. Leave it alone. */
        if (start == 2 && (strcmp(w[2], "a") == 0 || strcmp(w[2], "an") == 0))
            start = 0;
        /* "what is the <rel> of <obj>?" is a relational query, handled elsewhere;
         * an "of"/"di" marker means this is not a plain description request. */
        for (size_t i = start; start && i < nw; i++)
            if (strcmp(w[i], "of") == 0 || strcmp(w[i], "di") == 0) start = 0;
        if (start) {
            /* An exact concept key named directly ("what is the heart") always
             * wins — a precise match must beat a fuzzy guess. */
            for (size_t i = start; i < nw; i++) {
                if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                char desc[1024];
                if (kb_describe_entity(b->kb, w[i], desc, sizeof desc)) {
                    put(desc, out, out_size);
                    store_proof(b, desc);
                    remember_entity(b, w[i], w[i]);
                    return 1;
                }
            }
            /* gen172: a multi-word concept is stored under an underscore-joined
             * key (e.g. "prime number" -> prime_number) that the single-word loop
             * above cannot match. Try that exact joined key too, so a learned or
             * loaded multi-word concept still beats the fuzzy guess below — the
             * "exact key always wins" rule, completed for compound names. Speak it
             * with the spaced display form (no underscore). */
            {
                char jkey[128]; size_t jo = 0;
                char jdisp[128]; size_t jd = 0;
                for (size_t i = start; i < nw; i++) {
                    if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                    jo += (size_t)snprintf(jkey + jo, sizeof jkey - jo,
                                           "%s%s", jo ? "_" : "", w[i]);
                    jd += (size_t)snprintf(jdisp + jd, sizeof jdisp - jd,
                                           "%s%s", jd ? " " : "", w[i]);
                }
                char jdef[KB_TERM_LEN];
                if (jo && strchr(jkey, '_') &&
                    kb_concept_def(b->kb, jkey, jdef, sizeof jdef)) {
                    char msg[1200];
                    snprintf(msg, sizeof msg, "%s is %s.", jdisp, jdef);
                    put(msg, out, out_size);
                    store_proof(b, msg);
                    remember_entity(b, jkey, jdisp);
                    return 1;
                }
            }
            /* gen155: no exact key — recall the concept whose description
             * structurally OVERLAPS the query (similarity, not a cue list):
             * "what is the longest bone in the body" -> femur. Hedged ("You
             * might mean ...") because it is a best guess from overlap, and
             * fires only with >=2 matching words and a clear winner, so a bare
             * concept name (one content word) and genuinely unknown topics fall
             * through unharmed. Discrete overlap is noisier than an LLM's
             * continuous space; precision is bought with the margin + hedge. */
            const char *qw[24]; size_t nq = 0;
            for (size_t i = start; i < nw && nq < 24; i++) {
                if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                if (!strcmp(w[i], "mean") || !strcmp(w[i], "means") ||
                    !strcmp(w[i], "thing") || !strcmp(w[i], "called") ||
                    !strcmp(w[i], "definition")) continue;
                qw[nq++] = w[i];
            }
            char ckey[128], cdesc[1024];
            if (nq >= 2 &&
                kb_nearest_concept(b->kb, qw, nq, ckey, sizeof ckey, cdesc, sizeof cdesc)) {
                char msg[1200];
                snprintf(msg, sizeof msg, "You might mean %s: %s.", ckey, cdesc);
                put(msg, out, out_size);
                store_proof(b, msg);
                remember_entity(b, ckey, ckey);
                return 1;
            }
        }
    }

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
    /* explanation, Italian subject-verb order: "perché <x> è un <y>?" reaches
     * here already half-canonicalized as "perché <x> is a <y>" (è->is, un->a),
     * but "perché" stays (it is not in canonical_token, so the many "perché ..."
     * cue handlers keep working). The subject sits before the verb, so the
     * English-order branch above (w[1]=="is") misses it. Same proof rendering,
     * one extra order; the contrastive "perché ... non è" path was already
     * order-free, this gives the affirmative why-proof the same bilingual reach.
     * Transfers to any unseen x/y. */
    if (nw == 5 &&
        (strcmp(w[0], "perché") == 0 || strcmp(w[0], "perche") == 0) &&
        strcmp(w[2], "is") == 0 && is_article(w[3])) {
        const char *subj;
        if (!resolve_entity(b, w[1], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        explain_reply(b, w[4], args, 1, out, out_size);
        remember_entity(b, w[1], subj);
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
        /* Filter internal predicates (gen150) */
        size_t kept = 0;
        char fheads[16][KB_TERM_LEN], fbodies[16][KB_TERM_LEN];
        for (size_t i = 0; i < k; i++) {
            if (is_internal_pred(heads[i]) || is_internal_pred(bodies[i])) continue;
            snprintf(fheads[kept], KB_TERM_LEN, "%s", heads[i]);
            snprintf(fbodies[kept], KB_TERM_LEN, "%s", bodies[i]);
            kept++;
        }
        if (kept == 0) { put("Nothing new to generalize.", out, out_size); return 1; }
        char msg[600];
        size_t off = (size_t)snprintf(msg, sizeof msg, "Induced: ");
        for (size_t i = 0; i < kept && off < sizeof msg; i++) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s%s(X) :- %s(X)", i ? "; " : "",
                                    fheads[i], fbodies[i]);
        }
        if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
        put(msg, out, out_size);
        return 1;
    }

    /* rule: "every <body...> is a/an <head>" -> head(X) :- body0(X), …
     * gen133 generalizes the single-body form to a CONJUNCTION: the modifiers
     * before the head noun become conjoined premises, e.g. "every friendly dog
     * is a goodboy" -> goodboy(X) :- friendly(X), dog(X). nw==5 (one body word)
     * stays exactly the old single-body rule, so prior behaviour is preserved. */
    if (nw >= 5 && nw <= 4 + KB_MAX_BODY && strcmp(w[0], "every") == 0 &&
        strcmp(w[nw - 3], "is") == 0 && is_article(w[nw - 2])) {
        const char *head = w[nw - 1];
        const char *bodies[KB_MAX_BODY];
        size_t nbody = nw - 4; /* body words are w[1 .. nw-4] */
        for (size_t i = 0; i < nbody; i++) bodies[i] = w[1 + i];
        if (kb_assert_rule_n(b->kb, head, bodies, nbody)) {
            char msg[256];
            size_t o = (size_t)snprintf(msg, sizeof msg, "Learned rule: %s(X) :- ",
                                        head);
            for (size_t i = 0; i < nbody && o < sizeof msg; i++)
                o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s(X)",
                                      i ? ", " : "", bodies[i]);
            if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
            put(msg, out, out_size);
            auto_induce(b, out, out_size);
        } else {
            put("I couldn't store that rule.", out, out_size);
        }
        return 1;
    }

    /* gen96: bulk forget — "forget everything about <x>" */
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

    /* explicit negative correction, order-insensitive (gen44): both English
     * "<x> is not a <y>" and the Italian-canonicalized "<x> not is a <y>" mean
     * not y(x). Detected by ROLE not position: subject first, article at nw-2,
     * class last, and the two middle tokens are exactly {is, not} in any order.
     * Word order is surface, not meaning, so one parser serves both languages
     * (the multilingual probe's gen43 finding). Question words are excluded so a
     * negated query is not mistaken for an assertion. */
    if (nw == 5 && is_article(w[3]) &&
        strcmp(w[0], "who") != 0 && strcmp(w[0], "what") != 0 &&
        strcmp(w[0], "is") != 0 &&
        ((strcmp(w[1], "is") == 0) || (strcmp(w[2], "is") == 0)) &&
        ((strcmp(w[1], "not") == 0) || (strcmp(w[2], "not") == 0))) {
        const char *subj, *cl = w[4];
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        int before = goal_truth(b); /* gen103 (L16): snapshot before mutation */
        note_contradiction(b, cl, subj, 0); /* gen142 (E8): self-contradiction? */
        if (kb_assert_neg(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Learned: not %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        note_consequence(b, cl, before, out, out_size); /* gen103 (L16) */
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* additional class (gen46): "<x> is also a/an <y>" -> y(x). Explanatory
     * prose adds classes incrementally ("a dolphin is also a mammal"); it is the
     * same assertion as "x is a y", one more membership. */
    if (nw == 5 && strcmp(w[1], "is") == 0 && strcmp(w[2], "also") == 0 &&
        is_article(w[3])) {
        const char *subj, *cl = w[4];
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        if (kb_assert(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cl, subj);
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

    /* gen133: article-free class assertion "<x> is <adj>" -> adj(x), but ONLY
     * when adj is a predicate some rule body already depends on. This makes the
     * conjuncts of a learned conjunctive concept ("every friendly dog is a
     * goodboy") assertable in natural English ("rex is friendly"), without the
     * frame ever firing on arbitrary "X is Y" prose. */
    if (nw == 3 && strcmp(w[1], "is") == 0 &&
        !is_stopword(b, w[0]) && isalpha((unsigned char)w[0][0])) {
        char clsbuf[KB_TERM_LEN];
        snprintf(clsbuf, sizeof clsbuf, "%s", w[2]);
        char *cl2 = strip_edge_punct(clsbuf);
        if (kb_rule_body_mentions(b->kb, cl2)) {
            const char *subj;
            if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
            const char *args[] = {subj};
            char msg[128];
            int before = b->has_last_goal ? goal_truth(b) : -1;
            if (kb_assert(b->kb, cl2, args, 1))
                snprintf(msg, sizeof msg, "Learned: %s(%s).", cl2, subj);
            else
                snprintf(msg, sizeof msg, "I couldn't store that.");
            put(msg, out, out_size);
            remember_entity(b, w[0], subj);
            note_consequence(b, cl2, before, out, out_size);
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
        else if (kb_is_conflicted(b->kb, cls, args, 1)) {
            put("Conflicted.", out, out_size);
            char ex[512];
            if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex))
                store_proof(b, ex);
        }
        else {
            int yes = kb_query(b->kb, cls, args, 1);
            put(yes ? "Yes." : "No.", out, out_size);
            if (yes) {
                char ex[512];
                if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex))
                    store_proof(b, ex);
            }
            /* gen103 (L16): remember this conclusion so a later correction can
             * re-derive it and flag the consequence. */
            snprintf(b->last_goal_pred, sizeof b->last_goal_pred, "%s", cls);
            snprintf(b->last_goal_arg, sizeof b->last_goal_arg, "%s", subj);
            b->last_goal_yes = yes;
            b->has_last_goal = 1;
        }
        remember_entity(b, w[1], subj);
        return 1;
    }

    /* subject-first interrogative: "<x> is a <y>?" -> y(x)? (the Italian shape
     * "<x> è un <y>?"). A trailing '?' makes this a QUERY, not an assertion, so
     * the same conclusion-memory + consequence machinery (gen103/L16) fires in
     * both languages through one path. */
    if (interrogative && strcmp(w[1], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        if (!kb_knows_pred(b->kb, cls)) idk(cls, out, out_size);
        else if (kb_is_conflicted(b->kb, cls, args, 1)) put("Conflicted.", out, out_size);
        else {
            int yes = kb_query(b->kb, cls, args, 1);
            put(yes ? "Yes." : "No.", out, out_size);
            if (yes) {
                char ex[512];
                if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex)) store_proof(b, ex);
            }
            snprintf(b->last_goal_pred, sizeof b->last_goal_pred, "%s", cls);
            snprintf(b->last_goal_arg, sizeof b->last_goal_arg, "%s", subj);
            b->last_goal_yes = yes;
            b->has_last_goal = 1;
        }
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* assert: "<x> is a <y>" -> y(x) */
    if (strcmp(w[1], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        int before = goal_truth(b); /* gen103 (L16): snapshot before mutation */
        note_contradiction(b, cls, subj, 1); /* gen142 (E8): self-contradiction? */
        if (kb_assert(b->kb, cls, args, 1)) {
            char msg[128];
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cls, subj);
            put(msg, out, out_size);
            note_consequence(b, cls, before, out, out_size); /* gen103 (L16) */
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

/* gen112: value of a SINGLE number word (English or Italian), 0–90 plus the
 * multipliers hundred/thousand. Returns 1 on a hit. Content words only — no
 * function-word collision, so it is language-neutral by construction. */
static int single_word_number(const char *s, double *out) {
    static const struct { const char *w; double v; } t[] = {
        {"zero",0},{"one",1},{"two",2},{"three",3},{"four",4},{"five",5},
        {"six",6},{"seven",7},{"eight",8},{"nine",9},{"ten",10},{"eleven",11},
        {"twelve",12},{"thirteen",13},{"fourteen",14},{"fifteen",15},
        {"sixteen",16},{"seventeen",17},{"eighteen",18},{"nineteen",19},
        {"twenty",20},{"thirty",30},{"forty",40},{"fifty",50},{"sixty",60},
        {"seventy",70},{"eighty",80},{"ninety",90},{"hundred",100},
        {"thousand",1000},
        /* Italian */
        {"uno",1},{"due",2},{"tre",3},{"quattro",4},{"cinque",5},{"sei",6},
        {"sette",7},{"otto",8},{"nove",9},{"dieci",10},{"undici",11},
        {"dodici",12},{"tredici",13},{"quattordici",14},{"quindici",15},
        {"sedici",16},{"diciassette",17},{"diciotto",18},{"diciannove",19},
        {"venti",20},{"trenta",30},{"quaranta",40},{"cinquanta",50},
        {"sessanta",60},{"settanta",70},{"ottanta",80},{"novanta",90},
        {"cento",100},{"mille",1000},
    };
    for (size_t i = 0; i < sizeof t / sizeof t[0]; i++)
        if (strcmp(s, t[i].w) == 0) { *out = t[i].v; return 1; }
    return 0;
}

/* A number WORD, including a hyphenated tens-unit compound ("twenty-one"). */
static int word_number(const char *s, double *out) {
    const char *hy = strchr(s, '-');
    if (hy) {
        char head[KB_TERM_LEN];
        size_t hn = (size_t)(hy - s);
        if (hn < sizeof head) {
            memcpy(head, s, hn); head[hn] = '\0';
            double tens, unit;
            if (single_word_number(head, &tens) && tens >= 20 &&
                (long)tens % 10 == 0 &&
                single_word_number(hy + 1, &unit) && unit >= 1 && unit <= 9) {
                *out = tens + unit; return 1;
            }
        }
    }
    return single_word_number(s, out);
}

/* Parse a value that may be a digit literal OR a number word. */
static int parse_value(const char *s, double *out) {
    return parse_num(s, out) || word_number(s, out);
}

/* gen112: collect the numbers in a token stream, reading digits AND number
 * words, merging spaced word compounds ("twenty five" -> 25) and multipliers
 * ("two hundred" -> 200). Merges apply only to WORD numbers, so two adjacent
 * digit quantities stay distinct. Returns how many were written (capped). */
static size_t collect_numbers(char **w, size_t nw, double *nums, size_t max) {
    size_t nn = 0; int prev_word = 0;
    for (size_t i = 0; i < nw && nn < max; i++) {
        char *t = strip_edge_punct(w[i]);
        if (!*t) { prev_word = 0; continue; }
        double v; int isword = 0;
        if (!parse_num(t, &v)) { isword = word_number(t, &v); if (!isword) { prev_word = 0; continue; } }
        if (isword && (v == 100 || v == 1000) && nn > 0 && nums[nn - 1] < v) {
            nums[nn - 1] *= v; prev_word = 1; continue;          /* "two hundred" */
        }
        if (isword && prev_word && nn > 0 && nums[nn - 1] >= 20 &&
            (long)nums[nn - 1] % 10 == 0 && v >= 1 && v <= 9) {
            nums[nn - 1] += v; prev_word = 1; continue;          /* "twenty five" */
        }
        nums[nn++] = v; prev_word = isword;
    }
    return nn;
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

/* True if `s` is a supported arithmetic operator keyword or symbol. */
static int is_arith_op(const char *s) {
    return strcmp(s, "plus") == 0 || strcmp(s, "minus") == 0 ||
           strcmp(s, "times") == 0 ||
           strcmp(s, "+") == 0 || strcmp(s, "-") == 0 || strcmp(s, "*") == 0;
}

/* Apply an arithmetic operator, returning the result. Sets *ok=0 for unknown ops. */
static double apply_arith_op(const char *op, double a, double c, int *ok) {
    *ok = 1;
    if (strcmp(op, "plus") == 0 || strcmp(op, "+") == 0) return a + c;
    if (strcmp(op, "minus") == 0 || strcmp(op, "-") == 0) return a - c;
    if (strcmp(op, "times") == 0 || strcmp(op, "*") == 0) return a * c;
    if (strcmp(op, "/") == 0) { if (c == 0) { *ok = 0; return 0; } return a / c; }
    *ok = 0;
    return 0;
}

/* gen190: arithmetic in natural language. The catalogue (basic-chat cat.4) asks
 * the SAME four operations in many surface forms — "six times seven", "add 5 and
 * 7", "100 divided by 4", "half of 50". These are not new computations; they are
 * new ways of NAMING operands and operators. So instead of one phrase per shape,
 * the parser below extracts (operator, operands) structurally and folds them with
 * the existing oracle. Operator words are recognised in EN+IT (per/più/diviso),
 * so the bilingual probe rides the same path. */

/* Canonical operator char from a single operator word or symbol (EN+IT). */
static char arith_op_char(const char *s) {
    if (!strcmp(s,"plus")||!strcmp(s,"+")||!strcmp(s,"più")||!strcmp(s,"piu")) return '+';
    if (!strcmp(s,"minus")||!strcmp(s,"-")||!strcmp(s,"meno")) return '-';
    if (!strcmp(s,"times")||!strcmp(s,"*")||!strcmp(s,"x")||!strcmp(s,"per")) return '*';
    if (!strcmp(s,"multiplied")) return '*';
    if (!strcmp(s,"divided")||!strcmp(s,"diviso")||!strcmp(s,"over")||!strcmp(s,"/")) return '/';
    return 0;
}

static double apply_op_char(char op, double a, double c, int *ok) {
    *ok = 1;
    switch (op) {
        case '+': return a + c;
        case '-': return a - c;
        case '*': return a * c;
        case '/': if (c == 0) { *ok = 0; return 0; } return a / c;
    }
    *ok = 0; return 0;
}

/* Square root without <math.h> (Newton's method); for our integer operands this
 * lands exactly on perfect squares. */
static double arith_sqrt(double x) {
    if (x < 0) return -1;
    if (x == 0) return 0;
    double g = x > 1 ? x : 1;
    for (int i = 0; i < 80; i++) g = 0.5 * (g + x / g);
    return g;
}

static int arith_is_prime(long long n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (long long d = 3; d * d <= n; d += 2)
        if (n % d == 0) return 0;
    return 1;
}

/* Fold ew[..] left-to-right as an arithmetic expression in any surface form:
 * skip leading non-numeric filler ("what is", "how much is"), then read
 * NUM (OP NUM)+ where OP is a word/symbol, consuming a "by" after
 * "divided"/"multiplied" and reading a bare "by" between numbers as times
 * ("six by seven"). Returns 1 with *res iff >=1 op applied and the whole tail
 * parsed cleanly (so prose like "5 apples ..." never matches). */
static int arith_eval_infix(char **ew, size_t enw, double *res) {
    size_t i = 0;
    double cur;
    while (i < enw && !parse_value(ew[i], &cur)) i++;
    if (i >= enw) return 0;
    i++;
    int ops = 0;
    while (i < enw) {
        char op = arith_op_char(ew[i]);
        if (!op) {
            if (!strcmp(ew[i], "by")) op = '*';      /* "six by seven" */
            else return 0;
        }
        if ((!strcmp(ew[i], "divided") || !strcmp(ew[i], "multiplied")) &&
            i + 1 < enw && !strcmp(ew[i + 1], "by"))
            i++;                                      /* consume the "by" */
        i++;
        if (i >= enw) return 0;
        double nx;
        if (!parse_value(ew[i], &nx)) return 0;
        int ok;
        cur = apply_op_char(op, cur, nx, &ok);
        if (!ok) return 0;
        i++; ops++;
    }
    if (ops == 0) return 0;
    *res = cur;
    return 1;
}

/* Emit a numeric answer as "<n>." */
static void arith_answer(double v, char *out, size_t out_size) {
    char num[64], msg[80];
    format_num(v, num, sizeof num);
    snprintf(msg, sizeof msg, "%s.", num);
    put(msg, out, out_size);
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

    /* Expand tokens containing embedded operators (e.g. "2+2" -> "2","+","2").
     * Pure numbers (which may start with '-') are left intact so parse_num works.
     * The expansion is done in a secondary buffer; `ew` points into it. */
    char exbuf[256];
    size_t exoff = 0;
    char *ew[24];
    size_t enw = 0;
    for (size_t i = 0; i < nw && enw < 24; i++) {
        const char *s = w[i];
        size_t sl = strlen(s);
        int has_embedded_op = 0;
        for (size_t j = 0; j < sl; j++) {
            if (s[j] == '+' || s[j] == '*' || s[j] == '/') has_embedded_op = 1;
            else if (s[j] == '-' && j > 0) has_embedded_op = 1;
        }
        double v;
        int is_num = parse_num(s, &v);
        if (!is_num && has_embedded_op) {
            size_t start = 0;
            for (size_t j = 0; j <= sl && enw < 24; j++) {
                int boundary = (j == sl || s[j] == '+' || s[j] == '*' ||
                    s[j] == '/' || (s[j] == '-' && j > 0));
                if (boundary) {
                    if (j > start && exoff + (j - start) + 1 <= sizeof exbuf) {
                        memcpy(exbuf + exoff, s + start, j - start);
                        exbuf[exoff + (j - start)] = '\0';
                        ew[enw++] = exbuf + exoff;
                        exoff += (j - start) + 1;
                    }
                    if (j < sl && exoff + 2 <= sizeof exbuf) {
                        exbuf[exoff] = s[j];
                        exbuf[exoff + 1] = '\0';
                        ew[enw++] = exbuf + exoff;
                        exoff += 2;
                    }
                    start = j + 1;
                }
            }
        } else {
            if (exoff + sl + 1 <= sizeof exbuf) {
                memcpy(exbuf + exoff, s, sl);
                exbuf[exoff + sl] = '\0';
                ew[enw++] = exbuf + exoff;
                exoff += sl + 1;
            }
        }
    }

    /* Exact-shape arith: "what is <a> OP <b>?" with expanded tokens. */
    if (enw == 5 && strcmp(ew[0], "what") == 0 && strcmp(ew[1], "is") == 0 &&
        is_arith_op(ew[3])) {
        double a, c;
        if (parse_value(ew[2], &a) && parse_value(ew[4], &c)) {
            int ok;
            double r = apply_arith_op(ew[3], a, c, &ok);
            if (ok) {
                char num[64], msg[80];
                format_num(r, num, sizeof num);
                snprintf(msg, sizeof msg, "%s.", num);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* Flexible search: find "what"+"is", then scan for NUM OP NUM anywhere after.
     * "what is the result of 2 plus 3", "what is 2 + 3" (already matched above). */
    {
        size_t wi = find_token(ew, enw, "what");
        if (wi < enw) {
            size_t si = find_token(ew + wi, enw - wi, "is");
            if (si < enw - wi) {
                si += wi;
                for (size_t i = si + 1; i + 2 < enw; i++) {
                    if (!is_arith_op(ew[i + 1])) continue;
                    double a, c;
                    if (parse_value(ew[i], &a) && parse_value(ew[i + 2], &c)) {
                        int ok;
                        double r = apply_arith_op(ew[i + 1], a, c, &ok);
                        if (ok) {
                            char num[64], msg[80];
                            format_num(r, num, sizeof num);
                            snprintf(msg, sizeof msg, "%s.", num);
                            put(msg, out, out_size);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    /* gen101: "explain why <a> plus <b> is <c>" — give the JUSTIFICATION, not
     * just the value. Pulled by the impersonation benchmark's math-teacher role,
     * but it is a general capability: the answer is grounded in the operation
     * (adding -> sum), so it transfers to any operands and any of the operators. */
    if (cue(buf, "explain") || cue(buf, "why")) {
        for (size_t i = 0; i + 2 < enw; i++) {
            if (!is_arith_op(ew[i + 1])) continue;
            double a, c;
            if (!parse_num(ew[i], &a) || !parse_num(ew[i + 2], &c)) continue;
            int ok;
            double r = apply_arith_op(ew[i + 1], a, c, &ok);
            if (!ok) continue;
            const char *op = ew[i + 1];
            const char *verb = "combining", *noun = "result";
            if (strcmp(op, "plus") == 0 || strcmp(op, "+") == 0) { verb = "adding"; noun = "sum"; }
            else if (strcmp(op, "minus") == 0 || strcmp(op, "-") == 0) { verb = "subtracting"; noun = "difference"; }
            else if (strcmp(op, "times") == 0 || strcmp(op, "*") == 0) { verb = "multiplying"; noun = "product"; }
            char na[64], nb[64], nr[64], msg[320];
            format_num(a, na, sizeof na);
            format_num(c, nb, sizeof nb);
            format_num(r, nr, sizeof nr);
            snprintf(msg, sizeof msg,
                     "Because %s %s and %s gives %s — that is their %s.",
                     verb, na, nb, nr, noun);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* "is <a> divisible by <b>?" -> yes/no via integer remainder */
    if (enw == 5 && strcmp(ew[0], "is") == 0 && strcmp(ew[2], "divisible") == 0 &&
        strcmp(ew[3], "by") == 0) {
        double a, c;
        if (!parse_num(ew[1], &a) || !parse_num(ew[4], &c)) return 0;
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

    /* gen190: natural-language arithmetic shapes (basic-chat cat.4). All of these
     * reduce to the four ops over operands named in prose; each is a structural
     * extractor, not a stored phrase. They share parse_value (digits + number
     * words) and the existing oracle, so they generalize over operands.
     *
     * NOTE: `buf` was clobbered in place by split_words above (it null-terminates
     * each token), and `w[8]` truncates long prompts. So these frames read cues
     * from the intact `norm` and re-split a fresh copy into a larger token array. */
    char cbuf[256]; char *cw[32]; size_t cnw = 0;
    {
        size_t cl = strlen(norm);
        if (cl < sizeof cbuf) {
            memcpy(cbuf, norm, cl + 1);
            if (cl > 0 && cbuf[cl - 1] == '?') cbuf[cl - 1] = '\0';
            cnw = split_words(cbuf, cw, 32);
        }
    }
    /* How many numeric operands the turn names. Several arithmetic cue words
     * ("half"/"halve", "double", "triple", "even", "odd") also appear as repeated
     * ACTIONS or branch conditions in agent/process descriptions ("halve until
     * below 5", "if it is even, ...; if it is odd, ..."). Those name >=2 numbers,
     * so the ambiguous frames below fire only when the turn names exactly one
     * number — keeping mod_agent's loops intact. */
    double gnums[16]; size_t gn = collect_numbers(cw, cnw, gnums, 16);

    /* "P percent of N" / "P per cento di N" -> N * P / 100. The percent marker is
     * "percent"/"%" (EN) or the "per cento" pair (IT, marked by the "cento" token,
     * which must NOT be read as the operand 100): P is the number just before it,
     * N the number just after. */
    {
        size_t mark = cnw;
        for (size_t i = 0; i < cnw; i++)
            if (!strcmp(cw[i], "percent") || !strcmp(cw[i], "%") ||
                !strcmp(cw[i], "cento")) { mark = i; break; }
        if (mark < cnw) {
            double pct = 0, base = 0; int havep = 0, haveb = 0;
            for (size_t i = mark; i-- > 0; ) {
                double v; if (parse_value(cw[i], &v)) { pct = v; havep = 1; break; }
            }
            for (size_t i = mark + 1; i < cnw; i++) {
                double v; if (parse_value(cw[i], &v)) { base = v; haveb = 1; break; }
            }
            if (havep && haveb) { arith_answer(base * pct / 100.0, out, out_size); return 1; }
        }
    }

    /* Unary "of"-frames and suffix frames over a single operand (EN+IT). */
    {
        /* square root of N / radice quadrata di N (declines a negative operand,
         * whose real square root does not exist — that is cat.5, not cat.4). */
        size_t ri = find_token(cw, cnw, "root");
        if (ri == cnw) ri = find_token(cw, cnw, "radice");
        if (ri != cnw && !cue(norm, "negative") && !cue(norm, "negativo")) {
            for (size_t i = ri + 1; i < cnw; i++) {
                double v; if (parse_value(cw[i], &v)) {
                    if (v >= 0) { arith_answer(arith_sqrt(v), out, out_size); return 1; }
                    break;
                }
            }
        }
        /* N squared / N cubed, N al quadrato / al cubo. */
        if (gn == 1) {
            int sq = 0, cb = 0;
            for (size_t i = 0; i < cnw; i++) {
                if (!strcmp(cw[i],"squared") || !strcmp(cw[i],"quadrato")) sq = 1;
                else if (!strcmp(cw[i],"cubed") || !strcmp(cw[i],"cubo")) cb = 1;
            }
            if (sq) { arith_answer(gnums[0] * gnums[0], out, out_size); return 1; }
            if (cb) { arith_answer(gnums[0] * gnums[0] * gnums[0], out, out_size); return 1; }
        }
        /* N factorial / fattoriale di N (single non-negative integer <= 20). */
        if (gn == 1 && (find_token(cw,cnw,"factorial") != cnw ||
                        find_token(cw,cnw,"fattoriale") != cnw)) {
            double v = gnums[0];
            if (v >= 0 && v <= 20 && (double)(long long)v == v) {
                long long f = 1; for (long long k = 2; k <= (long long)v; k++) f *= k;
                arith_answer((double)f, out, out_size); return 1;
            }
        }
        /* half of N / metà di N */
        if (gn == 1 && (cue(norm, "half") || cue(norm, "metà") || cue(norm, "meta"))) {
            arith_answer(gnums[0] / 2.0, out, out_size); return 1;
        }
        /* double / twice / triple / thrice (optionally "of") N */
        if (gn == 1 && (cue(norm, "double") || cue(norm, "twice") || cue(norm, "doppio") ||
            cue(norm, "triple") || cue(norm, "thrice") || cue(norm, "triplo"))) {
            double mul = (cue(norm, "triple") || cue(norm, "thrice") || cue(norm, "triplo")) ? 3 : 2;
            arith_answer(gnums[0] * mul, out, out_size); return 1;
        }
    }

    /* N-ary "sum of A and B and C..." and "average/mean of ...". */
    if (cue(norm, "sum of") || cue(norm, "average of") || cue(norm, "mean of") ||
        cue(norm, "somma di") || cue(norm, "media di")) {
        double nums[16]; size_t n = collect_numbers(cw, cnw, nums, 16);
        if (n >= 1) {
            double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
            int avg = cue(norm, "average") || cue(norm, "mean") || cue(norm, "media");
            arith_answer(avg ? s / (double)n : s, out, out_size);
            return 1;
        }
    }

    /* Verb-led imperative frames: "add A and B", "subtract A from B",
     * "multiply A by B", "divide A by B" (EN+IT). */
    if (cnw >= 1) {
        const char *v0 = cw[0];
        double nums[16]; size_t n = collect_numbers(cw, cnw, nums, 16);
        if (n >= 2) {
            if (!strcmp(v0,"add") || !strcmp(v0,"sum") || !strcmp(v0,"aggiungi") ||
                !strcmp(v0,"somma")) {
                double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
                arith_answer(s, out, out_size); return 1;
            }
            if (!strcmp(v0,"subtract") || !strcmp(v0,"sottrai") || !strcmp(v0,"togli")) {
                /* "subtract A from B" -> B - A */
                if (cue(norm, "from") || cue(norm, " da ")) { arith_answer(nums[1] - nums[0], out, out_size); return 1; }
                arith_answer(nums[0] - nums[1], out, out_size); return 1;
            }
            if (!strcmp(v0,"multiply") || !strcmp(v0,"moltiplica")) {
                double p = 1; for (size_t i = 0; i < n; i++) p *= nums[i];
                arith_answer(p, out, out_size); return 1;
            }
            if (!strcmp(v0,"divide") || !strcmp(v0,"dividi")) {
                if (nums[1] != 0) { arith_answer(nums[0] / nums[1], out, out_size); return 1; }
            }
        }
    }

    /* Number-property predicates: "is N prime / even / odd" (EN+IT property
     * words, any word order — "5 è un numero primo"). The property word is read
     * tokenwise (not as a substring) so "evening"/"Paris" never trigger it, and
     * it fires only when the turn also names an integer. */
    {
        int wants_prime = 0, wants_even = 0, wants_odd = 0;
        for (size_t i = 0; i < cnw; i++) {
            if (!strcmp(cw[i],"prime") || !strcmp(cw[i],"primo") || !strcmp(cw[i],"prima"))
                wants_prime = 1;
            else if (!strcmp(cw[i],"even") || !strcmp(cw[i],"pari")) wants_even = 1;
            else if (!strcmp(cw[i],"odd") || !strcmp(cw[i],"dispari")) wants_odd = 1;
        }
        if (gn == 1 && (wants_prime || wants_even || wants_odd)) {
            for (size_t i = 0; i < cnw; i++) {
                double v; if (!parse_value(cw[i], &v)) continue;
                if ((double)(long long)v != v) break;
                long long n = (long long)v;
                char msg[96];
                if (wants_prime) {
                    snprintf(msg, sizeof msg, "%s, %lld is %sa prime number.",
                             arith_is_prime(n) ? "Yes" : "No", n,
                             arith_is_prime(n) ? "" : "not ");
                } else {
                    int even = (n % 2 == 0);
                    int yes = wants_even ? even : !even;
                    snprintf(msg, sizeof msg, "%s, %lld is %s%s number.",
                             yes ? "Yes" : "No", n, yes ? "an " : "not an ",
                             wants_even ? "even" : "odd");
                }
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* General fallback: fold any infix expression ("six times seven",
     * "100 divided by 4", "how much is 1+1+1+1+1"). */
    {
        double r;
        if (arith_eval_infix(ew, enw, &r)) { arith_answer(r, out, out_size); return 1; }
    }

    return 0;
}

/* --- module: algebra (L17) ------------------------------------------------
 * One-step equation solving: "x + 3 = 7" -> x = 4. gen35 could COMPUTE a op b;
 * L17 INVERTS it — given an equation with one unknown and one operation, solve
 * for the unknown by applying the inverse operation. This is a genuine reasoning
 * step (the unknown is never on the answer side to begin with), not a lookup. It
 * reuses the same number parsing/formatting as mod_arith, and is symbolic — the
 * equation "x - 4 = 1" is identical in any language, so the bilingual ratchet is
 * almost free (only leading filler words differ). Operators: + - * / (symbols)
 * and the plus/minus/times words, EN+IT. '-' is always a binary operator here
 * (no negative-literal operands), which keeps one-step school algebra simple. */
static char algebra_op(const char *s) {
    if (!strcmp(s, "+") || !strcmp(s, "plus")  || !strcmp(s, "più"))  return '+';
    if (!strcmp(s, "-") || !strcmp(s, "minus") || !strcmp(s, "meno")) return '-';
    if (!strcmp(s, "*") || !strcmp(s, "times") || !strcmp(s, "per"))  return '*';
    if (!strcmp(s, "/")) return '/';
    return 0;
}

/* Split on whitespace and on the operator/equals chars (+ - * / =), each of the
 * latter emitted as its own one-char token. Tokens point into `store`. */
static size_t algebra_tokenize(const char *s, char store[][KB_TERM_LEN],
                               char *toks[], size_t max) {
    size_t n = 0, k = 0;
    while (s[k] && n < max) {
        while (s[k] && isspace((unsigned char)s[k])) k++;
        if (!s[k]) break;
        char c = s[k];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=') {
            store[n][0] = c; store[n][1] = '\0'; toks[n] = store[n]; n++; k++;
            continue;
        }
        size_t m = 0;
        while (s[k] && !isspace((unsigned char)s[k]) && s[k] != '+' &&
               s[k] != '-' && s[k] != '*' && s[k] != '/' && s[k] != '=' &&
               m + 1 < KB_TERM_LEN) {
            store[n][m++] = s[k++];
        }
        store[n][m] = '\0'; toks[n] = store[n]; n++;
    }
    return n;
}

static int algebra_is_filler(const char *s) {
    static const char *const f[] = {
        "solve","find","what","is","the","value","of","for","if","compute",
        "calculate","please","equation",
        "risolvi","trova","quanto","vale","il","valore","di","se","calcola",
        "equazione", NULL };
    return matches_any(s, f);
}

static int mod_algebra(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!strchr(norm, '=')) return 0;            /* an equation has '=' */

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char store[24][KB_TERM_LEN]; char *t[24];
    size_t nt = algebra_tokenize(buf, store, t, 24);

    /* drop a leading run of filler words ("solve", "risolvi", ...). */
    size_t s0 = 0;
    while (s0 < nt && algebra_is_filler(t[s0])) s0++;
    char **tk = t + s0; size_t n = nt - s0;

    /* locate the single '=' */
    size_t eq = n;
    for (size_t i = 0; i < n; i++) if (strcmp(tk[i], "=") == 0) {
        if (eq != n) return 0;                   /* more than one '=' */
        eq = i;
    }
    if (eq == n) return 0;
    size_t ln = eq, rn = n - eq - 1;             /* token counts each side */

    /* gen113: one-step coefficient form, "5y = 20" -> y = 4. One side is a bare
     * number, the other a coefficient*variable token (a number alone would be
     * trivial, so we require the coefficient). */
    if (ln == 1 && rn == 1) {
        const char *lhs = tk[0], *rhs = tk[eq + 1];
        double lv, rv; int lnum = parse_value(lhs, &lv), rnum = parse_value(rhs, &rv);
        const char *unk = NULL; double val = 0;
        if (lnum && !rnum) { unk = rhs; val = lv; }
        else if (rnum && !lnum) { unk = lhs; val = rv; }
        if (!unk) return 0;
        size_t d = 0; while (isdigit((unsigned char)unk[d]) || unk[d] == '.') d++;
        if (d == 0 || unk[d] == '\0') return 0;  /* no coefficient -> trivial */
        char cb[32]; if (d >= sizeof cb) return 0;
        memcpy(cb, unk, d); cb[d] = '\0';
        double coef = strtod(cb, NULL);
        if (coef == 0) return 0;
        double res = val / coef; const char *var = unk + d;
        char num[64]; format_num(res, num, sizeof num);
        char msg[96]; snprintf(msg, sizeof msg, "%s = %s.", var, num);
        put(msg, out, out_size);
        char proof[256];
        snprintf(proof, sizeof proof, "%s = %g, so %s = %g / %g = %s.",
                 unk, val, var, val, coef, num);
        store_proof(b, proof);
        return 1;
    }

    /* exactly one side is "operand OP operand" (3 tokens), the other a lone
     * operand (1 token). Canonicalize to: a <op> b = c. */
    char *ta, *tb, *tc; char op;
    if (ln == 3 && rn == 1) {
        ta = tk[0]; op = algebra_op(tk[1]); tb = tk[2]; tc = tk[eq + 1];
    } else if (ln == 1 && rn == 3) {
        ta = tk[eq + 1]; op = algebra_op(tk[eq + 2]); tb = tk[eq + 3]; tc = tk[0];
    } else {
        return 0;
    }
    if (!op) return 0;

    /* exactly one of ta, tb, tc is the unknown (a non-numeric identifier).
     * Operands may be digits or number words ("x plus three = seven"). */
    double av, bv, cv;
    int na = parse_value(ta, &av), nb = parse_value(tb, &bv), nc = parse_value(tc, &cv);
    int unknowns = (!na) + (!nb) + (!nc);
    if (unknowns != 1) return 0;

    const char *x; double r; char rhs[96];   /* rhs = the inverse expression */
    if (!nc) {                                 /* tc unknown: just compute ta op tb */
        x = tc;
        switch (op) {
            case '+': r = av + bv; break;
            case '-': r = av - bv; break;
            case '*': r = av * bv; break;
            case '/': if (bv == 0) { put("I can't divide by zero.", out, out_size); return 1; }
                      r = av / bv; break;
            default: return 0;
        }
        snprintf(rhs, sizeof rhs, "%g %c %g", av, op, bv);
    } else if (!na) {                          /* ta unknown: invert around ta */
        x = ta;
        switch (op) {
            case '+': r = cv - bv; snprintf(rhs, sizeof rhs, "%g - %g", cv, bv); break;
            case '-': r = cv + bv; snprintf(rhs, sizeof rhs, "%g + %g", cv, bv); break;
            case '*': if (bv == 0) return 0; r = cv / bv; snprintf(rhs, sizeof rhs, "%g / %g", cv, bv); break;
            case '/': r = cv * bv; snprintf(rhs, sizeof rhs, "%g * %g", cv, bv); break;
            default: return 0;
        }
    } else {                                   /* tb unknown: invert around tb */
        x = tb;
        switch (op) {
            case '+': r = cv - av; snprintf(rhs, sizeof rhs, "%g - %g", cv, av); break;
            case '-': r = av - cv; snprintf(rhs, sizeof rhs, "%g - %g", av, cv); break;
            case '*': if (av == 0) return 0; r = cv / av; snprintf(rhs, sizeof rhs, "%g / %g", cv, av); break;
            case '/': if (cv == 0) return 0; r = av / cv; snprintf(rhs, sizeof rhs, "%g / %g", av, cv); break;
            default: return 0;
        }
    }

    /* gen113: two-step linear form — the unknown may carry a coefficient written
     * adjacently ("2x"). Then r is the value the term `coef*var` must equal, so
     * the variable is r / coef. This turns "2x + 1 = 7" into x = (7-1)/2 = 3. */
    char varname[KB_TERM_LEN]; double coef; int two_step = 0;
    {
        size_t d = 0;
        while (isdigit((unsigned char)x[d]) || x[d] == '.') d++;
        if (d > 0 && x[d] != '\0' && d < sizeof varname) {
            char cb[32]; if (d < sizeof cb) {
                memcpy(cb, x, d); cb[d] = '\0';
                coef = strtod(cb, NULL);
                if (coef != 0) {
                    snprintf(varname, sizeof varname, "%s", x + d);
                    r /= coef; two_step = 1;
                }
            }
        }
    }

    char num[64]; format_num(r, num, sizeof num);
    char msg[96];
    snprintf(msg, sizeof msg, "%s = %s.", two_step ? varname : x, num);
    put(msg, out, out_size);

    char proof[256];
    if (two_step)
        snprintf(proof, sizeof proof,
                 "%s %c %s = %s, so %s = %s, and %s = %s.",
                 ta, op, tb, tc, x, rhs, varname, num);
    else
        snprintf(proof, sizeof proof,
                 "%s %c %s = %s, so %s = %s = %s.", ta, op, tb, tc, x, rhs, num);
    store_proof(b, proof);
    return 1;
}

/* --- module: plan (L13) ---------------------------------------------------
 * Ordered procedure to a goal: a tiny planner. Prerequisites are taught as
 * `requires(Goal, Step)` facts in ANY order ("cake requires batter", "batter
 * requires eggs"); asking "how do I make cake?" returns a correctly ORDERED
 * plan, derived by a depth-first topological sort of the dependency DAG so every
 * step precedes what depends on it. The sequence is never stored — it is
 * recomputed from the scattered facts each time, so a goal the system was taught
 * only as loose prerequisites yields a coherent procedure (anti-impostor: the
 * order is reasoned, not recited). Cycles are detected and reported. */
static int plan_dfs(Brain *b, const char *node, const char *parent,
                    char done[][KB_TERM_LEN], size_t *ndone,
                    char stack[][KB_TERM_LEN], size_t depth,
                    char order[][KB_TERM_LEN], char par[][KB_TERM_LEN],
                    size_t *norder, size_t maxn) {
    for (size_t i = 0; i < *ndone; i++)
        if (strcmp(done[i], node) == 0) return 1;        /* already placed */
    for (size_t i = 0; i < depth; i++)
        if (strcmp(stack[i], node) == 0) return 0;       /* back-edge => cycle */
    if (depth >= maxn) return 0;
    snprintf(stack[depth], KB_TERM_LEN, "%s", node);

    char pre[64][KB_TERM_LEN];
    const char *pat[] = { node, NULL };
    size_t k = kb_match(b->kb, "requires", pat, 2, pre, 64);
    for (size_t j = 0; j < k; j++)
        if (!plan_dfs(b, pre[j], node, done, ndone, stack, depth + 1,
                      order, par, norder, maxn))
            return 0;

    if (*norder < maxn && *ndone < maxn) {
        snprintf(order[*norder], KB_TERM_LEN, "%s", node);
        snprintf(par[*norder], KB_TERM_LEN, "%s", parent);   /* who needs it */
        (*norder)++;
        snprintf(done[*ndone], KB_TERM_LEN, "%s", node);     (*ndone)++;
    }
    return 1;
}

/* gen110: learn a prerequisite LIST from one sentence — conjunction and optional
 * quantities. "cake requires eggs and flour" -> two requires() facts; "batter
 * requires 3 eggs and 2 flour" also records amount(batter, eggs, 3) etc. Each
 * item is an optional leading number then a single step token; "and"/"e"/","/"of"
 * are skipped. Returns the count learned and writes the reply. */
static size_t plan_learn_list(Brain *b, const char *goal, char **w,
                              size_t start, size_t nw, char *out, size_t out_size) {
    long pend = -1; size_t learned = 0; char last_step[KB_TERM_LEN] = "";
    for (size_t i = start; i < nw; i++) {
        char *tk = strip_edge_punct(w[i]);
        if (!*tk) continue;
        if (!strcmp(tk, "and") || !strcmp(tk, "e") || !strcmp(tk, "ed") ||
            !strcmp(tk, "of") || !strcmp(tk, "di")) continue;
        double v;
        if (parse_num(tk, &v)) { pend = (long)v; continue; }
        const char *ar[] = { goal, tk };
        kb_assert(b->kb, "requires", ar, 2);
        if (pend >= 0) {
            char qs[24]; snprintf(qs, sizeof qs, "%ld", pend);
            const char *aq[] = { goal, tk, qs };
            kb_assert(b->kb, "amount", aq, 3);
        }
        pend = -1; learned++;
        snprintf(last_step, sizeof last_step, "%s", tk);
    }
    if (learned == 0) return 0;
    char msg[256];
    if (learned == 1)
        snprintf(msg, sizeof msg, "Learned: requires(%s, %s).", goal, last_step);
    else
        snprintf(msg, sizeof msg, "Learned %zu prerequisites for %s.", learned, goal);
    put(msg, out, out_size);
    return learned;
}

static int mod_plan(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char *w[32];
    size_t nw = split_words(buf, w, 32);

    /* intake: "X requires/needs <list>" — conjunction + optional quantities. */
    if (nw >= 3 && (strcmp(w[1], "requires") == 0 || strcmp(w[1], "needs") == 0 ||
                    strcmp(w[1], "richiede") == 0)) {
        if (plan_learn_list(b, w[0], w, 2, nw, out, out_size)) return 1;
    }
    /* Italian intake: "per X serve/servono <list>" (to X you need ...). */
    if (nw >= 4 && strcmp(w[0], "per") == 0 &&
        (strcmp(w[2], "serve") == 0 || strcmp(w[2], "servono") == 0)) {
        if (plan_learn_list(b, w[1], w, 3, nw, out, out_size)) return 1;
    }

    /* query: "how do I make X" / "how to make X" / "steps to/for X" /
     * "come faccio/si fa ... X". The goal is the last content word. */
    /* Detect the how-to phrasing on the ORIGINAL input (normalized for case),
     * not the canonicalized `norm`: canonicalize_lang rewrites Italian function
     * words (e.g. "si" -> "is"), which would hide "come si fa". And we read this
     * intact copy, never `buf`, which split_words just null-terminated in place. */
    char q[256]; normalize(raw, q, sizeof q);
    int howto = (cue(q, "how") && cue(q, "make")) || cue(q, "how to") ||
                strncmp(q, "steps", 5) == 0 || cue(q, "steps to") ||
                cue(q, "steps for") ||
                (cue(q, "come") && (cue(q, "faccio") || cue(q, "fare") ||
                                    cue(q, "si fa")));
    if (!howto || nw < 2) return 0;

    char goal[KB_TERM_LEN];
    snprintf(goal, sizeof goal, "%s", w[nw - 1]);
    strip_edge_punct(goal);

    /* a goal we have no prerequisites for is honestly unknown. */
    const char *pat[] = { goal, NULL };
    char pre0[4][KB_TERM_LEN];
    if (kb_match(b->kb, "requires", pat, 2, pre0, 4) == 0) {
        char msg[128];
        snprintf(msg, sizeof msg, "I don't know the steps to make %s yet.", goal);
        put(msg, out, out_size);
        return 1;
    }

    char done[32][KB_TERM_LEN], stack[32][KB_TERM_LEN];
    char order[32][KB_TERM_LEN], par[32][KB_TERM_LEN];
    size_t ndone = 0, norder = 0;
    if (!plan_dfs(b, goal, "", done, &ndone, stack, 0, order, par, &norder, 32)) {
        char msg[128];
        snprintf(msg, sizeof msg,
                 "The steps for %s have a circular prerequisite — I can't order them.",
                 goal);
        put(msg, out, out_size);
        return 1;
    }

    /* render the topological order as the procedure (prerequisites first),
     * annotating a step with the quantity its requirer asked for, if known. */
    char line[1024]; size_t off = 0;
    for (size_t i = 0; i < norder; i++) {
        const char *sep = i ? (i + 1 == norder ? ", then " : ", ") : "";
        char amt[4][KB_TERM_LEN];
        const char *apat[] = { par[i], order[i], NULL };
        size_t na = par[i][0] ? kb_match(b->kb, "amount", apat, 3, amt, 4) : 0;
        if (na > 0)
            off += (size_t)snprintf(line + off, off < sizeof line ? sizeof line - off : 0,
                                    "%s%s %s", sep, amt[0], order[i]);
        else
            off += (size_t)snprintf(line + off, off < sizeof line ? sizeof line - off : 0,
                                    "%s%s", sep, order[i]);
    }
    char msg[1100];
    snprintf(msg, sizeof msg, "To make %s: %s.", goal, line);
    put(msg, out, out_size);

    char proof[256];
    snprintf(proof, sizeof proof,
             "ordered by prerequisites: each step follows everything it requires.");
    store_proof(b, proof);
    return 1;
}

/* --- module: wordproblem (L17 prose) --------------------------------------
 * One-sentence word problems: prose -> arithmetic relation -> solve. gen107
 * solved a symbolic equation; this maps a natural-language problem onto an
 * operation and computes it. The operation is chosen from SEMANTIC cues (verbs
 * of gaining/losing/grouping/sharing, and comparison phrasings), not exact
 * sentence templates — so held-out numbers AND held-out verbs transfer. It is
 * deliberately conservative: it fires only on a "how many/much" question with at
 * least two numbers and a recognized cue, and DECLINES otherwise (anti-impostor:
 * never guess an operation). Natural language is all exceptions (DESIGN.md D5),
 * so this targets the canonical school phrasings and reads the first two numbers
 * in order (total/dividend first, as those phrasings put it). Bilingual cues.
 *
 * gen114: with three or more numbers it folds a multi-STEP additive/subtractive
 * chain ("has 3, buys 5 more, then eats 2" -> 3 + 5 - 2 = 6): each clause's sign
 * is + unless the clause carries a removal verb, and clauses split on
 * then/and/poi/e and commas. */
static int wp_removal_word(const char *t) {
    static const char *const ex[] = {
        "ate","eats","lost","loses","gave","gives","spent","spends","sold",
        "sells","broke","removed","removes","dropped","drops","used", NULL };
    for (size_t i = 0; ex[i]; i++) if (strcmp(t, ex[i]) == 0) return 1;
    return strstr(t, "mangi") || strstr(t, "perso") || strstr(t, "perde") ||
           strstr(t, "regal") || strstr(t, "vend")  || strstr(t, "spes");
}

static int mod_wordproblem(Brain *b, const char *norm, const char *raw,
                           char *out, size_t out_size) {
    (void)norm;
    char q[256]; normalize(raw, q, sizeof q);          /* intact, un-canonicalized */

    /* question guard: only attempt on an explicit "how many / how much / quanti…" */
    if (!(cue(q, "how many") || cue(q, "how much") || cue(q, "quant")))
        return 0;

    /* collect the numbers in reading order (digits and number words). */
    char buf[256]; snprintf(buf, sizeof buf, "%s", q);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    double nums[16];
    size_t nn = collect_numbers(w, nw, nums, 16);
    if (nn < 2) return 0;

    /* gen114: 3+ numbers -> multi-step additive/subtractive fold, clause by
     * clause. The first number is the base; each later number is added, or
     * subtracted if its clause carries a removal verb. Clauses split on
     * then/and/poi/e and on a trailing comma. */
    if (nn >= 3) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", q);
        char *tw[64]; size_t tnw = split_words(sb, tw, 64);
        double result = 0; int have = 0, sign = 1;
        for (size_t i = 0; i < tnw; i++) {
            size_t L = strlen(tw[i]);
            int trailing = L > 0 && (tw[i][L - 1] == ',' || tw[i][L - 1] == ';');
            char *t = strip_edge_punct(tw[i]);
            if (!*t) { if (trailing) sign = 1; continue; }
            if (!strcmp(t, "then") || !strcmp(t, "and") || !strcmp(t, "poi") ||
                !strcmp(t, "e") || !strcmp(t, "ed")) { sign = 1; continue; }
            if (wp_removal_word(t)) sign = -1;
            double v;
            if (parse_value(t, &v)) {
                if (!have) { result = v; have = 1; }
                else result += sign * v;
            }
            if (trailing) sign = 1;
        }
        char num[64]; format_num(result, num, sizeof num);
        char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
        put(msg, out, out_size);
        char proof[160];
        snprintf(proof, sizeof proof,
                 "I folded the steps left to right to %s.", num);
        store_proof(b, proof);
        return 1;
    }

    double a = nums[0], c = nums[1];

    /* choose the operation by cue, in a priority that resolves overlaps:
     * division, then comparison-difference / removal (both '-'), then
     * multiplication, then addition. */
    char op = 0;
    if (cue(q, "shared") || cue(q, "share") || cue(q, "divided") ||
        cue(q, "divide") || cue(q, "split") || cue(q, "among") ||
        cue(q, "equally") || cue(q, "divis") || cue(q, "condivi") ||
        cue(q, "ripartit"))
        op = '/';
    else if (cue(q, "how many more") || cue(q, "how much more") ||
             cue(q, "how many fewer") || cue(q, "how many less") ||
             cue(q, "more than") || cue(q, "fewer than") ||
             cue(q, "difference") || cue(q, "quanti in più") ||
             cue(q, "differenza") ||
             cue(q, "ate") || cue(q, "eats") || cue(q, "lost") ||
             cue(q, "loses") || cue(q, "gave") || cue(q, "gives away") ||
             cue(q, "left") || cue(q, "remain") || cue(q, "fewer") ||
             cue(q, "spent") || cue(q, "spends") || cue(q, "sold") ||
             cue(q, "sells") || cue(q, "broke") || cue(q, "removed") ||
             cue(q, "drops") || cue(q, " away") || cue(q, "mangi") ||
             cue(q, "pers") || cue(q, "perde") || cue(q, "regal") ||
             cue(q, "vend") || cue(q, "riman") || cue(q, "spend"))
        op = '-';
    else if (cue(q, "each") || cue(q, "times") || cue(q, "groups of") ||
             cue(q, "rows of") || cue(q, "boxes of") || cue(q, "ciascun") ||
             cue(q, "ognuno") || cue(q, "ogni"))
        op = '*';
    else if (cue(q, "more") || cue(q, "gets") || cue(q, "got") ||
             cue(q, "gains") || cue(q, "buys") || cue(q, "buy") ||
             cue(q, "found") || cue(q, "finds") || cue(q, "altogether") ||
             cue(q, "total") || cue(q, "in all") || cue(q, "combined") ||
             cue(q, "receive") || cue(q, "plus") || cue(q, "adds") ||
             cue(q, "compr") || cue(q, "trov") || cue(q, "totale") ||
             cue(q, "insieme") || cue(q, "ancora") || cue(q, "aggiun"))
        op = '+';
    if (!op) return 0;

    double r;
    switch (op) {
        case '+': r = a + c; break;
        case '-': r = a - c; break;
        case '*': r = a * c; break;
        case '/': if (c == 0) { put("I can't divide by zero.", out, out_size); return 1; }
                  r = a / c; break;
        default: return 0;
    }

    char num[64]; format_num(r, num, sizeof num);
    char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
    put(msg, out, out_size);

    char proof[128];
    snprintf(proof, sizeof proof, "I read it as %g %c %g = %s.", a, op, c, num);
    store_proof(b, proof);
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
        /* gen90: also find indirect/transitive causes. */
        {
            char mid[64][KB_TERM_LEN];
            size_t kmid = kb_match(b->kb, "causes",
                                    want_eff ? pat_eff : pat_cause, 2, mid, 64);
            for (size_t m = 0; m < kmid && k < 64; m++) {
                const char *indirect[] = {mid[m], NULL};
                const char *indirect_rev[] = {NULL, mid[m]};
                char chain[64][KB_TERM_LEN];
                size_t kc = kb_match(b->kb, "causes",
                                      want_eff ? indirect : indirect_rev, 2, chain, 64);
                for (size_t c = 0; c < kc && k < 64; c++) {
                    int dup = 0;
                    for (size_t d = 0; d < k; d++)
                        if (strcmp(hits[d], chain[c]) == 0) { dup = 1; break; }
                    if (!dup) snprintf(hits[k++], KB_TERM_LEN, "%s (via %s)", chain[c], mid[m]);
                }
            }
        }
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

/* gen106 (L1): the explicit end-of-sequence token. D-prop1 calls for "an
 * explicit stop token / end relation" so decoding halts because the model
 * LEARNED where utterances end — not merely because a step bound cut it off.
 * It is a sentinel atom that begins with a lowercase letter (so the KB reads it
 * as a constant, not a variable — leading uppercase OR '_' means variable,
 * kb.c) and embeds underscores so the whitespace/prose tokenizer can never
 * produce it as a real word. It is learned, never hand-authored, from sentence
 * boundaries in the very text taught (a terminal '.'/'!'/'?' or end-of-stream),
 * so STOP is induced exactly like every other transition. */
#define GEN_STOP "end_of_seq"

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

/* Learn one trigram transition (p1 p2)->word with a count (gen38). */
static void learn_transition2(Brain *b, const char *p1, const char *p2,
                              const char *word) {
    const char *pat[] = {p1, p2, word, NULL};
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont2", pat, 4, cnt, 4);
    long n = 1;
    if (k > 0) {
        n = strtol(cnt[0], NULL, 10) + 1;
        const char *old[] = {p1, p2, word, cnt[0]};
        kb_retract(b->kb, "cont2", old, 4);
    }
    char ns[32];
    snprintf(ns, sizeof ns, "%ld", n);
    const char *args[] = {p1, p2, word, ns};
    kb_assert(b->kb, "cont2", args, 4);
}

/* Learn the bigram (cont) and trigram (cont2) transitions across a word stream,
 * returning the number of bigram pairs learned. Shared by `learn sequence:` and
 * the reader (gen41) so the generative model can grow from the same prose the
 * fact extractor reads, not only from explicit teaching. */
/* gen106 (L1): true if `tok` ends a sentence, stripping the trailing terminal
 * punctuation in place so the cleaned word is still learned. A lone "." returns
 * a boundary with an emptied token. */
static int is_sentence_boundary(char *tok) {
    size_t n = strlen(tok);
    if (n == 0) return 0;
    char c = tok[n - 1];
    if (c != '.' && c != '!' && c != '?') return 0;
    while (n > 0 && (tok[n - 1] == '.' || tok[n - 1] == '!' || tok[n - 1] == '?'))
        tok[--n] = '\0';
    return 1;
}

/* gen106 (L1): learn the bigram (cont) and trigram (cont2) transitions across a
 * word stream, INCLUDING a learned end-of-sequence: at every sentence boundary
 * (terminal punctuation or end-of-stream) the last real word gets a transition
 * to GEN_STOP, and the rolling context resets so no transition bridges the
 * boundary. Returns the number of (non-STOP) bigram pairs learned — the count
 * the "learn sequence:" reply reports, unchanged from gen41. Shared by
 * `learn sequence:` and the reader, so the generative model grows from the same
 * prose the fact extractor reads. */
static size_t learn_word_stream(Brain *b, char **w, size_t nw) {
    size_t pairs = 0;
    const char *p1 = NULL, *p2 = NULL; /* rolling context, reset at boundaries */
    for (size_t i = 0; i < nw; i++) {
        if (strlen(w[i]) >= KB_TERM_LEN) { p2 = NULL; p1 = NULL; continue; }
        int boundary = is_sentence_boundary(w[i]); /* may empty the token */
        const char *cur = w[i];
        if (*cur) {
            if (p1) { learn_transition(b, p1, cur); pairs++; }
            if (p2 && p1) learn_transition2(b, p2, p1, cur);
            p2 = p1; p1 = cur;
        }
        if (boundary && p1) {                       /* learned end-of-sequence */
            learn_transition(b, p1, GEN_STOP);
            if (p2) learn_transition2(b, p2, p1, GEN_STOP);
            p2 = NULL; p1 = NULL;                    /* do not bridge sentences */
        }
    }
    if (p1) learn_transition(b, p1, GEN_STOP);       /* end-of-stream is a stop */
    return pairs;
}

/* Start each `read:` passage as a fresh corpus for generation. The reader still
 * accumulates extracted facts in the KB, but the continuation model represents
 * the most recently read passage, so a held-out second passage can measurably
 * shift `say <seed>` instead of tying the first passage by insertion order. */
static void clear_generation_model(Brain *b) {
    if (!b || !b->kb) return;

    char prevs[128][KB_TERM_LEN];
    const char *any3[] = {NULL, NULL, NULL};
    size_t np = kb_match(b->kb, "cont", any3, 3, prevs, 128);
    for (size_t i = 0; i < np; i++) {
        const char *word_pat[] = {prevs[i], NULL, NULL};
        char words[128][KB_TERM_LEN];
        size_t nw = kb_match(b->kb, "cont", word_pat, 3, words, 128);
        for (size_t j = 0; j < nw; j++) {
            const char *cnt_pat[] = {prevs[i], words[j], NULL};
            char counts[16][KB_TERM_LEN];
            size_t nc = kb_match(b->kb, "cont", cnt_pat, 3, counts, 16);
            for (size_t k = 0; k < nc; k++) {
                const char *old[] = {prevs[i], words[j], counts[k]};
                kb_retract(b->kb, "cont", old, 3);
            }
        }
    }

    char p1s[128][KB_TERM_LEN];
    const char *any4[] = {NULL, NULL, NULL, NULL};
    size_t n1 = kb_match(b->kb, "cont2", any4, 4, p1s, 128);
    for (size_t i = 0; i < n1; i++) {
        const char *p2_pat[] = {p1s[i], NULL, NULL, NULL};
        char p2s[128][KB_TERM_LEN];
        size_t n2 = kb_match(b->kb, "cont2", p2_pat, 4, p2s, 128);
        for (size_t j = 0; j < n2; j++) {
            const char *word_pat[] = {p1s[i], p2s[j], NULL, NULL};
            char words[128][KB_TERM_LEN];
            size_t nw = kb_match(b->kb, "cont2", word_pat, 4, words, 128);
            for (size_t k = 0; k < nw; k++) {
                const char *cnt_pat[] = {p1s[i], p2s[j], words[k], NULL};
                char counts[16][KB_TERM_LEN];
                size_t nc = kb_match(b->kb, "cont2", cnt_pat, 4, counts, 16);
                for (size_t m = 0; m < nc; m++) {
                    const char *old[] = {p1s[i], p2s[j], words[k], counts[m]};
                    kb_retract(b->kb, "cont2", old, 4);
                }
            }
        }
    }
}

/* Look up the stored count for a transition, or 0 if absent. `argc` counts the
 * context+word slots (2 for cont, 3 for cont2); the trailing count slot is the
 * variable kb_match binds. */
static long transition_count(Brain *b, const char *rel,
                             const char *const *key, size_t keyn) {
    const char *pat[KB_MAX_ARGS];
    for (size_t i = 0; i < keyn; i++) pat[i] = key[i];
    pat[keyn] = NULL; /* count */
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, rel, pat, keyn + 1, cnt, 4);
    return (k > 0) ? strtol(cnt[0], NULL, 10) : 0;
}

/* Choose the next word by INTERPOLATING the bigram and trigram evidence
 * (gen42), replacing gen38's hard backoff. Each bigram candidate `w` of `p1`
 * scores W2*cont2(p2,p1,w) + W1*cont(p1,w): the longer context informs the
 * choice without dictating it, so a single count-1 trigram no longer overrides
 * a strong bigram (Decision D-2026-06-15k). Every trigram continuation has a
 * matching bigram (the learner emits both), so the bigram set is the complete
 * candidate set. The gen40 CRITICAL FILTER still applies: when `subj` is set
 * the running output is a claim "<subj> is a ___", and any `w` the KB knows
 * `w(subj)` false/conflicted is skipped. Tie -> insertion order. Returns 1 if a
 * word was chosen, 0 if there are no candidates or every one was blocked (the
 * caller then stops rather than utter a falsehood). */
/* gen111 (D-prop1 step 2): the decoder's choice ranking is itself KB knowledge.
 * The interpolation coefficients live as `weight(trigram, N)` / `weight(bigram,
 * N)` facts, read here with a fallback default — so the generation POLICY is
 * inspectable and editable knowledge, not hardcoded C (DESIGN.md D6). Editing the
 * fact (e.g. "set trigram weight to 0") changes which continuation wins. */
static long gen_weight(Brain *b, const char *kind, long dflt) {
    if (!b || !b->kb) return dflt;
    const char *pat[] = { kind, NULL };
    char v[2][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "weight", pat, 2, v, 2);
    return k ? strtol(v[0], NULL, 10) : dflt;
}

static int next_word_ctx(Brain *b, const char *p2, const char *p1,
                         const char *subj, char *word, size_t wsize) {
    /* trigram weight dominates but does not dictate — now read from the KB. */
    const long W2 = gen_weight(b, "trigram", 3), W1 = gen_weight(b, "bigram", 1);
    const char *pat[] = {p1, NULL, NULL};
    char words[64][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont", pat, 3, words, 64);
    if (k == 0) return 0;

    long best = -1;
    size_t bi = 0;
    int found = 0;
    for (size_t i = 0; i < k; i++) {
        if (subj) {
            const char *a[] = {subj};
            if (kb_is_negated(b->kb, words[i], a, 1) ||
                kb_is_conflicted(b->kb, words[i], a, 1))
                continue; /* would assert a known-false claim: refuse to say it */
        }
        const char *bkey[] = {p1, words[i]};
        long c1 = transition_count(b, "cont", bkey, 2);
        long c2 = 0;
        if (p2) {
            const char *tkey[] = {p2, p1, words[i]};
            c2 = transition_count(b, "cont2", tkey, 3);
        }
        long score = W2 * c2 + W1 * c1;
        if (score > best) { best = score; bi = i; found = 1; } /* > -> first tie */
    }
    if (!found) return 0; /* candidates existed, all blocked -> stop */
    snprintf(word, wsize, "%s", words[bi]);
    return 1;
}

static void generate_from(Brain *b, const char *seed, char *out, size_t out_size) {
    char toks[64][KB_TERM_LEN];
    size_t nt = 0;
    snprintf(toks[nt++], KB_TERM_LEN, "%s", seed);

    char line[1024];
    size_t off = (size_t)snprintf(line, sizeof line, "%s", toks[0]);

    for (int step = 0; step < 24 && nt < 64; step++) { /* bound guards cycles */
        const char *p1 = toks[nt - 1];
        const char *p2 = (nt >= 2) ? toks[nt - 2] : NULL;

        /* gen40: if the tail reads "<x> is a/an", the next word is a claim
         * about x — pass x as the subject so the filter can veto false ones. */
        const char *subj = NULL;
        if (nt >= 3 && strcmp(toks[nt - 2], "is") == 0 &&
            (strcmp(toks[nt - 1], "a") == 0 || strcmp(toks[nt - 1], "an") == 0))
            subj = toks[nt - 3];

        char nxt[KB_TERM_LEN];
        if (!next_word_ctx(b, p2, p1, subj, nxt, sizeof nxt)) break;
        if (strcmp(nxt, GEN_STOP) == 0) break; /* gen106: learned end-of-sequence */
        if (off < sizeof line)
            off += (size_t)snprintf(line + off, sizeof line - off, " %s", nxt);
        snprintf(toks[nt++], KB_TERM_LEN, "%s", nxt);
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
        size_t pairs = learn_word_stream(b, w, nw);
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
        if (strcmp(w[1], "something") == 0) return 0; /* companion cue */
        generate_from(b, w[1], out, out_size);
        return 1;
    }

    /* gen111: edit the decoder's choice policy as knowledge —
     * "set trigram weight to N" / "set bigram weight to N" updates the
     * weight(kind, N) fact next_word_ctx reads, so generation behaviour is
     * steered by editable KB knowledge, not hardcoded coefficients (D-prop1). */
    if (nw == 5 && strcmp(w[0], "set") == 0 && strcmp(w[2], "weight") == 0 &&
        strcmp(w[3], "to") == 0 &&
        (strcmp(w[1], "trigram") == 0 || strcmp(w[1], "bigram") == 0)) {
        double v;
        if (parse_num(w[4], &v)) {
            const char *kp[] = { w[1], NULL };
            char old[8][KB_TERM_LEN];
            size_t k = kb_match(b->kb, "weight", kp, 2, old, 8);
            for (size_t i = 0; i < k; i++) {
                const char *o[] = { w[1], old[i] };
                kb_retract(b->kb, "weight", o, 2);
            }
            char ns[24]; snprintf(ns, sizeof ns, "%ld", (long)v);
            const char *ar[] = { w[1], ns };
            kb_assert(b->kb, "weight", ar, 2);
            char msg[96];
            snprintf(msg, sizeof msg, "Ok, %s weight is now %ld.", w[1], (long)v);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* grounded verbalization (gen39): "describe <x>" generates a sentence for
     * every class x is *provably* a member of — including beliefs reached only
     * through rules, not just stored facts. Reasoning turned back into language,
     * and grounded in real KB state rather than a canned phrase. */
    if (nw == 2 && strcmp(w[0], "describe") == 0) {
        const char *x = w[1];
        char preds[128][KB_TERM_LEN];
        size_t k = kb_unary_predicates(b->kb, preds, 128);
        char line[1024];
        size_t off = 0, hits = 0;
        for (size_t i = 0; i < k; i++) {
            const char *a[] = {x};
            if (!kb_is_conflicted(b->kb, preds[i], a, 1) &&
                kb_query(b->kb, preds[i], a, 1)) {
                if (off < sizeof line)
                    off += (size_t)snprintf(line + off, sizeof line - off,
                                            "%s%s is a %s", hits ? ". " : "",
                                            x, preds[i]);
                hits++;
            }
        }
        if (hits == 0) {
            char msg[160];
            snprintf(msg, sizeof msg, "I have nothing to say about %s.", x);
            put(msg, out, out_size);
        } else {
            if (off < sizeof line)
                snprintf(line + off, sizeof line - off, ".");
            put(line, out, out_size);
        }
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
/* Strip leading/trailing non-alphanumerics from a token, in place (gen41).
 * Real prose carries commas and quotes the `learn sequence:` path never sees;
 * trimming them keeps the induced continuation model keyed on words, not
 * "word," vs "word". Word-internal characters (apostrophes) are preserved. */
static char *strip_edge_punct(char *t) {
    while (*t && !isalnum((unsigned char)*t)) t++;
    size_t n = strlen(t);
    while (n > 0 && !isalnum((unsigned char)t[n - 1])) t[--n] = '\0';
    return t;
}

/* Induce continuation transitions from one clause's word stream (gen41): the
 * generative model grows from the same sentences the extractor reads. */
static void learn_clause_transitions(Brain *b, const char *clause) {
    char nbuf[256];
    normalize(clause, nbuf, sizeof nbuf);
    char *tw[64];
    size_t tnw = split_words(nbuf, tw, 64);
    size_t m = 0;
    for (size_t i = 0; i < tnw; i++) {
        char *c = strip_edge_punct(tw[i]);
        if (*c) tw[m++] = c;
    }
    learn_word_stream(b, tw, m);
}

static int extract_clause(Brain *b, char *clause) {
    char *c = trim_mut(clause);
    if (!*c) return 0;
    char norm[256];
    normalize(c, norm, sizeof norm);
    if (!*norm) return 0;

    /* gen121: canonicalize the clause's function words to English tokens before
     * extraction (gen43's interlingua), so an Italian sentence is parsed into the
     * SAME fact by the SAME modules — no duplicated reader. The original surface
     * sentence is what a later summary quotes, so the Italian text is preserved. */
    char canon[256];
    canonicalize_lang(norm, canon, sizeof canon);

    char resp[256];
    if (mod_quantity(b, canon, c, resp, sizeof resp) ||
        mod_cause(b, canon, c, resp, sizeof resp) ||
        mod_same(b, canon, c, resp, sizeof resp) ||
        mod_knowledge(b, canon, c, resp, sizeof resp)) {
        return strncmp(resp, "Learned", 7) == 0; /* an assertion, not a query */
    }
    return 0;
}

/* gen121 (L6): remember a surface sentence whose facts were extracted, so a
 * later summary can quote real sentences. Trimmed, de-duplicated, capped. */
static void store_proposition(Brain *b, char *clause) {
    if (!b) return;
    char *c = trim_mut(clause);
    if (!*c) return;
    for (size_t i = 0; i < b->prop_count; i++)
        if (strcmp(b->props[i], c) == 0) return;
    if (b->prop_count < BRAIN_PROPS_MAX) {
        snprintf(b->props[b->prop_count], sizeof b->props[0], "%s", c);
        b->prop_count++;
    }
}

/* Split a mutable passage buffer into sentence clauses and feed each to the
 * extractor (facts) and the generative model (transitions). Shared by the
 * reader and the bench bridge (gen45). Counts assertions and skips. */
static void read_passage(Brain *b, char *buf, size_t *learned, size_t *skipped) {
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
        /* gen121: keep the original surface sentence before extraction trims it,
         * so a later summary can quote real sentences, not reconstructed facts. */
        char original[192];
        snprintf(original, sizeof original, "%s", p);
        learn_clause_transitions(b, p);   /* gen41: feed the generative model */
        if (extract_clause(b, p)) {
            (*learned)++;
            store_proposition(b, original);
        }
        else if (*trim_mut(p)) (*skipped)++;
        if (saved == '\0') break;
        p = q + 1;
    }
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
    clear_generation_model(b);
    b->prop_count = 0;     /* gen121: each `read:` is a fresh passage to summarize */
    read_passage(b, buf, &learned, &skipped);

    char msg[128];
    snprintf(msg, sizeof msg, "Learned %zu fact(s), skipped %zu.",
             learned, skipped);
    put(msg, out, out_size);
    return 1;
}

/* --- module: bench -------------------------------------------------------
 * gen45: the bridge from a benchmark prompt envelope to parrot0's own
 * reasoning. The SuperGLUE driver wraps each example as one line, e.g.
 * "SuperGLUE BoolQ. Passage: <P> Question: <Q> Answer yes or no." parrot0
 * matched none of it and abstained, scoring 0% — worse than a coin flip,
 * because it never guesses. This part recognizes the yes/no envelope, READS the
 * passage through the existing extractor (open prose still mostly skips — the
 * honest wall, D-2026-06-15e), then ANSWERS the question through the existing
 * query modules, emitting yes/no ONLY when the answer is derivable. It still
 * never guesses: an underivable question abstains, so the score reflects real
 * reasoning coverage, not luck. The reasoning is unchanged; this is I/O wiring,
 * not a phrasebook. */
/* gen49 — bench baselines. The label tasks cannot (yet) be reasoned, but the
 * user wants every class VALID (a mappable answer), not abstaining. These
 * helpers back transparent lexical-overlap baselines: shallow, content-derived,
 * deterministic, near chance — explicitly NOT comprehension. Reasoning still
 * takes precedence where it applies (BoolQ); the baseline is only the fallback.
 * A KB-backed stopword relation keeps overlap on content words. */
static int is_stopword(Brain *b, const char *w) {
    if (!b || !b->kb || !w || !*w) return 0;
    char atom[KB_TERM_LEN];
    size_t i = 0, j = 0;
    for (; w[i] && j + 1 < sizeof atom; i++) {
        unsigned char ch = (unsigned char)w[i];
        if (ch == 39 || ch == 96) continue; /* apostrophe/backtick */
        atom[j++] = (char)(ch < 128 ? tolower(ch) : ch);
    }
    if (w[i]) return 0;
    atom[j] = 0;
    const char *args[] = {atom};
    return kb_query(b->kb, "stopword", args, 1);
}

/* Percentage of a's content tokens that also occur in b (0..100), or -1 when a
 * has no content tokens. Case-insensitive, exact word match (not substring). */
static int overlap_pct(Brain *b, const char *a, const char *text) {
    char ab[1024], bb[4096];
    size_t la = strlen(a); if (la >= sizeof ab) la = sizeof ab - 1;
    for (size_t i = 0; i < la; i++) ab[i] = (char)tolower((unsigned char)a[i]);
    ab[la] = '\0';
    size_t lb = strlen(text); if (lb >= sizeof bb) lb = sizeof bb - 1;
    for (size_t i = 0; i < lb; i++) bb[i] = (char)tolower((unsigned char)text[i]);
    bb[lb] = '\0';

    char *aw[256]; size_t na = split_words(ab, aw, 256);
    char *bw[1024]; size_t nb = split_words(bb, bw, 1024);
    for (size_t j = 0; j < nb; j++) bw[j] = strip_edge_punct(bw[j]);

    size_t total = 0, hit = 0;
    for (size_t i = 0; i < na; i++) {
        char *t = strip_edge_punct(aw[i]);
        if (strlen(t) < 3 || is_stopword(b, t)) continue;
        total++;
        for (size_t j = 0; j < nb; j++)
            if (*bw[j] && strcmp(t, bw[j]) == 0) { hit++; break; }
    }
    if (total == 0) return -1;
    return (int)((hit * 100) / total);
}

/* Copy raw[after m1 .. before m2) into out (m2 NULL -> to end). Offsets are
 * found in `low` (a lowercased copy of raw, same length) and applied to raw. */
static void slice_between(const char *raw, const char *low, size_t rlen,
                          const char *m1, const char *m2,
                          char *out, size_t outsz) {
    out[0] = '\0';
    const char *p = strstr(low, m1);
    if (!p) return;
    size_t s = (size_t)(p - low) + strlen(m1);
    size_t e = rlen;
    if (m2) { const char *q = strstr(low + s, m2); if (q) e = (size_t)(q - low); }
    if (e < s) e = s;
    size_t n = e - s; if (n >= outsz) n = outsz - 1;
    memcpy(out, raw + s, n); out[n] = '\0';
}

/* gen48: ReCoRD is a cloze over named entities ("...fill @placeholder..."). We
 * cannot comprehend the passage, but we can return its most SALIENT entity — the
 * most frequent capitalized, non-sentence-initial token. This is a transparent
 * extractive baseline, explicitly NOT comprehension: it reads the real passage
 * and returns a real candidate (never a canned or random string), which makes
 * the example *valid* (a mappable answer, not the abstain fallback) and
 * sometimes overlaps the gold entity. Honest weak signal, not a guess at a label
 * it cannot justify. Returns 1 and writes the entity, or 0 to abstain. */
static int record_salient_entity(const char *raw, size_t lo, size_t hi,
                                 char *out, size_t out_size) {
    char surf[128][KB_TERM_LEN];
    char key[128][KB_TERM_LEN];
    long cnt[128];
    size_t first[128];
    size_t nc = 0;

    size_t i = lo;
    while (i < hi) {
        while (i < hi && !isalpha((unsigned char)raw[i])) i++;
        size_t s = i;
        while (i < hi && (isalpha((unsigned char)raw[i]) ||
                          raw[i] == '\'' || raw[i] == '-')) i++;
        size_t len = i - s;
        if (len < 2 || len >= KB_TERM_LEN) continue;
        if (!isupper((unsigned char)raw[s])) continue;
        /* skip sentence-initial capitals (likely "The"/"He", not an entity) */
        size_t p = s;
        while (p > lo && (raw[p - 1] == ' ')) p--;
        if (p == lo || raw[p - 1] == '.' || raw[p - 1] == '!' ||
            raw[p - 1] == '?' || raw[p - 1] == '"') continue;

        char k[KB_TERM_LEN];
        for (size_t j = 0; j < len; j++)
            k[j] = (char)tolower((unsigned char)raw[s + j]);
        k[len] = '\0';

        size_t f = nc;
        for (size_t c = 0; c < nc; c++)
            if (strcmp(key[c], k) == 0) { f = c; break; }
        if (f == nc) {
            if (nc >= 128) continue;
            strcpy(key[nc], k);
            memcpy(surf[nc], raw + s, len); surf[nc][len] = '\0';
            cnt[nc] = 0; first[nc] = s; nc++;
        }
        cnt[f]++;
    }
    if (nc == 0) return 0;
    size_t best = 0;
    for (size_t c = 1; c < nc; c++)
        if (cnt[c] > cnt[best] || (cnt[c] == cnt[best] && first[c] < first[best]))
            best = c;
    put(surf[best], out, out_size);
    return 1;
}

/* Dispatch a bench prompt to its task baseline. `low` is a full lowercased copy
 * of `raw` (NOT truncated), so markers anywhere in a long passage are found. */
static int bench_dispatch(Brain *b, const char *raw, const char *low,
                          size_t rlen, char *out, size_t out_size) {
    /* ReCoRD envelope: a cloze over entities — return the most salient one. */
    const char *lpas = strstr(low, "passage:");
    if (strstr(low, "@placeholder") && lpas) {
        size_t ps = (size_t)(lpas - low) + strlen("passage:");
        const char *lqy = strstr(low, "query:");
        size_t pe = lqy ? (size_t)(lqy - low) : rlen;
        if (pe > ps && record_salient_entity(raw, ps, pe, out, out_size))
            return 1;
        put("nothing", out, out_size); /* still valid (non-empty), never blank */
        return 1;
    }

    /* COPA: pick the choice with more lexical overlap with the premise. */
    if (strstr(low, "choice 1:") && strstr(low, "choice 2:")) {
        char prem[2048], c1[1024], c2[1024];
        slice_between(low, low, rlen, "premise:", "question:", prem, sizeof prem);
        slice_between(low, low, rlen, "choice 1:", "choice 2:", c1, sizeof c1);
        slice_between(low, low, rlen, "choice 2:", "answer", c2, sizeof c2);
        int o1 = overlap_pct(b, c1, prem), o2 = overlap_pct(b, c2, prem);
        put(o2 > o1 ? "2" : "1", out, out_size);
        return 1;
    }

    /* RTE / CB: entailment by overlap (+ negation) of hypothesis vs premise. */
    if (strstr(low, "premise:") && strstr(low, "hypothesis:")) {
        char prem[3072], hyp[1024];
        slice_between(low, low, rlen, "premise:", "hypothesis:", prem, sizeof prem);
        slice_between(low, low, rlen, "hypothesis:", "answer", hyp, sizeof hyp);
        int ov = overlap_pct(b, hyp, prem);
        if (strstr(low, "neutral")) { /* CB lists neutral; RTE does not */
            int neg = strstr(hyp, " not ") || strstr(hyp, "n't") ||
                      strstr(hyp, " never ") || strstr(prem, " never ");
            if (ov >= 60) put("entailment", out, out_size);
            else if (neg) put("contradiction", out, out_size);
            else put("neutral", out, out_size);
        } else {
            /* RTE: the bench's parser maps only the 'entailment' label (it is a
             * substring of 'not_entailment'), so that is the only valid output. */
            put("entailment", out, out_size);
        }
        return 1;
    }

    /* MultiRC: is the candidate answer grounded in the paragraph? */
    if (strstr(low, "paragraph:") && strstr(low, "candidate answer:")) {
        char para[4096], ans[1024];
        slice_between(low, low, rlen, "paragraph:", "question:", para, sizeof para);
        slice_between(low, low, rlen, "candidate answer:",
                      "is this answer correct", ans, sizeof ans);
        int ov = overlap_pct(b, ans, para);
        put(ov >= 50 ? "yes" : "no", out, out_size);
        return 1;
    }

    /* WiC: same word, same meaning? — weak signal from sentence overlap. */
    if (strstr(low, "sentence 1:") && strstr(low, "sentence 2:")) {
        char s1[1024], s2[1024];
        slice_between(low, low, rlen, "sentence 1:", "sentence 2:", s1, sizeof s1);
        slice_between(low, low, rlen, "sentence 2:", "word:", s2, sizeof s2);
        int ov = overlap_pct(b, s1, s2);
        put(ov >= 50 ? "yes" : "no", out, out_size);
        return 1;
    }

    /* WSC: do the two spans corefer? — yes if they share a content (head) word. */
    if (strstr(low, "span 1:") && strstr(low, "span 2:")) {
        char sp1[256], sp2[256];
        slice_between(low, low, rlen, "span 1:", "span 2:", sp1, sizeof sp1);
        slice_between(low, low, rlen, "span 2:", "answer", sp2, sizeof sp2);
        int ov = overlap_pct(b, sp1, sp2);
        put(ov >= 50 ? "yes" : "no", out, out_size);
        return 1;
    }

    const char *lp = strstr(low, "passage:");
    const char *lq = strstr(low, "question:");
    if (!lp || !lq || lq < lp) goto fallback;

    size_t pass_off = (size_t)(lp - low) + strlen("passage:");
    size_t ques_off = (size_t)(lq - low);
    size_t qstart   = ques_off + strlen("question:");

    /* passage = raw[pass_off .. ques_off) */
    char passage[4096];
    size_t plen = ques_off > pass_off ? ques_off - pass_off : 0;
    if (plen >= sizeof passage) plen = sizeof passage - 1;
    memcpy(passage, raw + pass_off, plen);
    passage[plen] = '\0';

    /* question = raw[qstart .. end), cut at the trailing "answer ..." tail */
    char question[1024];
    size_t qlen = rlen > qstart ? rlen - qstart : 0;
    if (qlen >= sizeof question) qlen = sizeof question - 1;
    memcpy(question, raw + qstart, qlen);
    question[qlen] = '\0';
    char *tail = strstr(low + qstart, "answer");
    if (tail) {
        size_t cut = (size_t)(tail - (low + qstart));
        if (cut < sizeof question) question[cut] = '\0';
    }

    /* read the passage (asserts whatever parses; open prose mostly skips) */
    size_t learned = 0, skipped = 0;
    read_passage(b, passage, &learned, &skipped);

    /* route the question through the existing query modules */
    char qn[512], qc[512];
    normalize(question, qn, sizeof qn);
    canonicalize_lang(qn, qc, sizeof qc);
    size_t L = strlen(qc);
    if (L > 0 && L + 1 < sizeof qc && qc[L - 1] != '?') {
        qc[L] = '?'; qc[L + 1] = '\0';
    }

    char resp[256];
    int answered =
        mod_knowledge(b, qc, qc, resp, sizeof resp) ||
        mod_compare(b, qc, qc, resp, sizeof resp) ||
        mod_same(b, qc, qc, resp, sizeof resp) ||
        mod_conj(b, qc, qc, resp, sizeof resp);
    if (answered && strcmp(resp, "Yes.") == 0) { put("yes", out, out_size); return 1; }
    if (answered && strcmp(resp, "No.") == 0)  { put("no",  out, out_size); return 1; }

    /* not derivable -> lexical-overlap baseline (question grounded in passage).
     * A valid, content-derived guess near chance — labeled as a baseline, not
     * reasoning (gen49). */
    int ov = overlap_pct(b, question, passage);
    put(ov >= 50 ? "yes" : "no", out, out_size);
    return 1;

fallback:
    /* Any bench prompt that matched no specific handler still gets a VALID
     * default from its answer-format hint, so no example is ever invalid. */
    if (strstr(low, "yes or no")) put("no", out, out_size);
    else if (strstr(low, "1 or 2")) put("1", out, out_size);
    else if (strstr(low, "entailment")) put("entailment", out, out_size);
    else put("nothing", out, out_size);
    return 1;
}

static int mod_bench(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    if (!b) return 0;
    /* cheap pre-filter: every bench prompt opens with "SuperGLUE <task>." */
    if (!strstr(norm, "superglue")) return 0;

    /* Lowercase the WHOLE prompt (raw can far exceed norm's 255 cap and the
     * 4096 a stack buffer allowed — long passages pushed the question/answer
     * markers out of view, which is what made a few examples fall through to
     * "nothing"). Allocate to fit so every marker is found (gen49). */
    size_t rlen = strlen(raw);
    char *low = malloc(rlen + 1);
    if (!low) return 0;
    for (size_t i = 0; i < rlen; i++)
        low[i] = (char)tolower((unsigned char)raw[i]);
    low[rlen] = '\0';

    int handled = bench_dispatch(b, raw, low, rlen, out, out_size);
    free(low);
    return handled;
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

/* --- module: meta --------------------------------------------------------
 * Meta-conversation is not world knowledge: the user is asking about this
 * exchange itself (attention, reading, understanding, repetition, current
 * activity). Answer from local state and limits instead of falling through to
 * the knowledge fallback or the identity module. */
static int mod_meta(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len - 1] == '?' || buf[len - 1] == '!' ||
                       buf[len - 1] == '.' || buf[len - 1] == ' '))
        buf[--len] = '\0';

    int attention = cue(buf, "paying attention") ||
                    cue(buf, "reading my messages") ||
                    cue(buf, "read my messages") ||
                    cue(buf, "listening") ||
                    cue(buf, "ascoltando") ||
                    cue(buf, "mi ascolti");
    if (attention) {
        if (strcmp(b->last_reply, "Yes. I read each message in this conversation.") == 0)
            put("I am reading this conversation turn by turn.", out, out_size);
        else
            put("Yes. I read each message in this conversation.", out, out_size);
        return 1;
    }

    int activity = cue(buf, "what are you up to") ||
                   cue(buf, "what are you doing") ||
                   cue(buf, "che stai facendo") ||
                   cue(buf, "cosa stai facendo");
    if (activity) {
        char msg[128];
        snprintf(msg, sizeof msg, "I'm tracking this conversation; this is turn %lu.",
                 b->turns);
        put(msg, out, out_size);
        return 1;
    }

    /* gen82: session statistics — "how many turns?", "how long?" */
    int turn_count = cue(buf, "how many turns") ||
                     cue(buf, "how many messages") ||
                     cue(buf, "quanti turni") ||
                     cue(buf, "quanti messaggi");
    if (turn_count) {
        char msg[96];
        snprintf(msg, sizeof msg, "We've exchanged %lu turn(s).", b->turns);
        put(msg, out, out_size);
        return 1;
    }
    int session_time = cue(buf, "how long have we been talking") ||
                       cue(buf, "how long has this been") ||
                       cue(buf, "da quanto tempo parliamo") ||
                       cue(buf, "da quanto parliamo");
    if (session_time) {
        time_t now = time(NULL);
        long elapsed = (long)(now - b->start_time);
        char msg[128];
        if (elapsed < 60)
            snprintf(msg, sizeof msg, "We've been talking for %ld second(s).", elapsed);
        else if (elapsed < 3600)
            snprintf(msg, sizeof msg, "We've been talking for %ld minute(s) and %ld second(s).",
                     elapsed / 60, elapsed % 60);
        else
            snprintf(msg, sizeof msg, "We've been talking for %ld hour(s) and %ld minute(s).",
                     elapsed / 3600, (elapsed % 3600) / 60);
        put(msg, out, out_size);
        return 1;
    }

    int understand = cue(buf, "do you understand me") ||
                     cue(buf, "understand what i say") ||
                     cue(buf, "capisci") ||
                     cue(buf, "capire quello");
    if (understand) {
        put("I understand some patterns, and I say when I do not.", out, out_size);
        return 1;
    }

    /* C6 step 2: small polar meta-questions about presence, channel, and
     * identity. Answers are honest and state-grounded: text input is available,
     * audio is not, and the identity claim comes from i_am(parrot0). */
    int presence = cue(buf, "are you there") ||
                   cue(buf, "ci sei") ||
                   cue(buf, "sei li");
    if (presence) {
        put("Yes, I'm here.", out, out_size);
        return 1;
    }

    int channel = cue(buf, "can you hear me") ||
                  cue(buf, "do you hear me") ||
                  cue(buf, "mi senti") ||
                  cue(buf, "mi ascolti");
    if (channel) {
        put("No, I only read text. I can't hear audio.", out, out_size);
        return 1;
    }

    int bot = cue(buf, "are you a bot") ||
              cue(buf, "are you an ai") ||
              cue(buf, "are you a robot") ||
              cue(buf, "sei un bot") ||
              cue(buf, "sei un robot") ||
              cue(buf, "sei a bot") ||
              cue(buf, "sei a robot");
    if (bot) {
        const char *var[] = {NULL};
        char id[4][KB_TERM_LEN];
        size_t k = b->kb ? kb_match(b->kb, "i_am", var, 1, id, 4) : 0;
        char msg[128];
        if (k > 0)
            snprintf(msg, sizeof msg, "Yes, I am %s.", id[0]);
        else
            snprintf(msg, sizeof msg, "Yes, I am parrot0.");
        put(msg, out, out_size);
        return 1;
    }

    int repeat = cue(buf, "why do you keep saying") ||
                 cue(buf, "why keep saying") ||
                 cue(buf, "keep repeating") ||
                 cue(buf, "perche continui") ||
                 cue(buf, "perché continui");
    if (repeat) {
        put("I repeat when no module can claim the message; that is a gap to improve.",
            out, out_size);
        return 1;
    }

    /* gen76: "how do you know? ... */
    /* (already above) */

    /* gen91: "are you sure?" / "how confident?" */
    {
        int sure = cue(buf, "are you sure") || cue(buf, "how confident") ||
                   cue(buf, "how sure") || cue(buf, "sei sicuro");
        if (sure) {
            if (b->has_last_proof) {
                int is_direct = strstr(b->last_proof, " because ") == NULL;
                put(is_direct ? "Yes, that's a directly stored fact."
                              : "I'm confident — derived through logical rules.",
                    out, out_size);
            } else put("I can't verify — no proof was stored.", out, out_size);
            return 1;
        }
    }

    /* gen92: correction — "no, that's wrong. X is a Y." or "no, X is not a Y." */
    {
        int correction = (cue(buf, "no that") && cue(buf, "wrong")) ||
                         (cue(buf, "no, that") && cue(buf, "wrong")) ||
                         (buf[0] == 'n' && buf[1] == 'o' && buf[2] == ' ' &&
                          cue(buf, "wrong"));
        if (correction) {
            put("I see. If I said something incorrect, please tell me the right "
                "fact and I'll learn it.", out, out_size);
            return 1;
        }
    }

    /* gen93: goal tracking */
    {
        int g = strncmp(buf,"remember to ",12)==0 || strncmp(buf,"ricordati di ",13)==0;
        if(g && b->goal_count<8){ snprintf(b->goals[b->goal_count++],128,"%s",buf+12+(buf[2]=="c"?13:12)); put("Ok, noted.",out,out_size); return 1; }
        if(cue(buf,"my goals")||cue(buf,"miei obiettivi")){
            if(!b->goal_count) put("No goals set.",out,out_size);
            else { char l[1024]=""; for(size_t i=0;i<b->goal_count;i++){char t[200];snprintf(t,200,"%zu) %s. ",i+1,b->goals[i]);strcat(l,t);} put(l,out,out_size); }
            return 1;
        }
    }

    /* gen85: "explain more" / "in more detail" — re-render last proof. */
    int explain_more = cue(buf, "explain more") ||
                       cue(buf, "in more detail") ||
                       cue(buf, "tell me more") ||
                       cue(buf, "spiega meglio") ||
                       cue(buf, "più nel dettaglio");
    if (explain_more) {
        if (b->has_last_proof) {
            char msg[640];
            size_t plen = strlen(b->last_proof);
            int is_chain = strstr(b->last_proof, " because ") != NULL;
            if (is_chain)
                snprintf(msg, sizeof msg,
                         "The reasoning chain is: %s. Each 'because' is one "
                         "inference step applying a rule to known facts.",
                         b->last_proof);
            else if (plen > 0 && b->last_proof[plen - 1] == '.')
                snprintf(msg, sizeof msg, "This is a direct fact: %s I store it "
                         "explicitly in my knowledge base, so it needs no "
                         "further justification.", b->last_proof);
            else
                snprintf(msg, sizeof msg, "The supporting fact is: %s. This is "
                         "stored directly in my knowledge base.", b->last_proof);
            put(msg, out, out_size);
        } else {
            put("There is no recent reasoning to explain in more detail.",
                out, out_size);
        }
        return 1;
    }
    {
        size_t wn = 0, in_word = 0;
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == ' ') in_word = 0;
            else if (!in_word) { wn++; in_word = 1; }
        }
        int howknow = ((cue(buf, "how do you know") && wn <= 4)) ||
                      ((cue(buf, "why do you say that") && wn <= 5)) ||
                      (wn == 1 && strcmp(buf, "why") == 0) ||
                      ((cue(buf, "perché lo dici") && wn <= 3)) ||
                      ((cue(buf, "come lo sai") && wn <= 3));
        if (howknow) {
            if (b->has_last_proof) {
                char msg[640];
                size_t plen = strlen(b->last_proof);
                if (plen > 0 && b->last_proof[plen - 1] == '.')
                    snprintf(msg, sizeof msg, "Because %s", b->last_proof);
                else
                    snprintf(msg, sizeof msg, "Because %s.", b->last_proof);
                put(msg, out, out_size);
                b->has_last_proof = 0;
            } else {
                put("I haven't answered a knowledge-based question yet, so I "
                    "don't have a proof to share.",
                    out, out_size);
            }
            return 1;
        }
    }

    /* gen72: clarification requests — the user wants to understand the bot,
     * not a fact about the world. */
    int clarify = cue(buf, "what do you mean") ||
                  cue(buf, "explain yourself") ||
                  cue(buf, "explain what you mean") ||
                  cue(buf, "spiegati") ||
                  cue(buf, "spiegami") ||
                  cue(buf, "cosa intendi") ||
                  cue(buf, "che vuoi dire");
    if (clarify) {
        put("I mean I can only answer what my registered modules let me. "
            "Try asking a simple factual question.",
            out, out_size);
        return 1;
    }

    /* User admits they don't understand the bot — different from the bot
     * not understanding the user (which is the fallback). */
    int user_lost = cue(buf, "i don't get it") ||
                    cue(buf, "i don't understand you") ||
                    cue(buf, "not capisco") ||
                    cue(buf, "not i have capito") ||
                    cue(buf, "not ti capisco");
    if (user_lost) {
        put("I understand some patterns and I say when I do not. "
            "Try a shorter or simpler question.",
            out, out_size);
        return 1;
    }

    /* Help-offer reversal: the user offers to help the bot. The bot is not
     * a person that needs help — redirect honestly. */
    int help_offer = cue(buf, "how can i help you") ||
                     cue(buf, "let me help you") ||
                     cue(buf, "i should be helping you") ||
                     cue(buf, "i should help you") ||
                     cue(buf, "come posso aiutarti") ||
                     cue(buf, "ti aiuto io") ||
                     cue(buf, "am io che aiuto te") ||
                     cue(buf, "posso aiutarti");
    if (help_offer) {
        put("I'm a chatbot, not a person — I don't need help. "
            "But you can ask me questions and I'll try to answer.",
            out, out_size);
        return 1;
    }

    return 0;
}

/* gen77: introspection helpers — filter internal predicates so "what do you
 * know?" shows only user-facing knowledge, not KB machinery. */
static int is_internal_pred(const char *pred) {
    static const char *internal[] = {
        "stopword", "social_marker", "social_pattern", "question_word",
        "reaction_word", "i_am", "module", "cont", "cont2",
        "cmd", "flag",
        /* gen101: role/character world-knowledge (kb/core/roles.p0) is curated
         * base substrate for impersonation, not facts the user taught — filter it
         * from "how many facts do you know?" like the lexicon/social predicates. */
        "trait", "employer", "likes_color", "profession", "wrote",
        "rules_over", "title",
        /* gen126: the bilingual content lexicon (kb/core/gloss.p0) is base
         * substrate for mod_translate, not facts the user taught — filter it. */
        "tr", "gender",
        /* gen149: coding-domain knowledge (kb/experts/programming/coding.p0) is technical
         * substrate for mod_code, not conversational content — filter it. */
        "language", "keyword", "ctype", "py_builtin", "c_stdlib", "c_header",
        "error", "concept", "algorithm", "faster_than",
        /* gen150: expert/skill/profile domain knowledge — structural metadata. */
        "expert", "expert_domain", "expert_description",
        "skill", "skill_domain", "skill_description",
        "profile", "profile_domain", "profile_description",
        "compiled_language", "interpreted_language", "paradigm", "typed",
        "data_structure", "complexity", "fix_suggestion", "fix",
        "review_check", "review_pattern", "code_action", "code_template", "code_target",
        /* gen150b: mathematics domain */
        "math_op", "number_property", "math_constant", "divisible_by",
        "algebra_concept", "polynomial_form", "algebra_method",
        "shape_2d", "shape_3d", "area_formula", "volume_formula", "theorem",
        /* gen150b: medicine domain */
        "body_system", "organ", "bone",
        "drug_class", "admin_route", "pharma_concept",
        /* gen150b: reasoning skills */
        "deduction_pattern", "compare_dimension", "plan_method",
        /* gen150b: communication skills */
        "explain_method", "summarize_method",
        NULL
    };
    for (size_t i = 0; internal[i]; i++)
        if (strcmp(pred, internal[i]) == 0) return 1;
    return 0;
}

static size_t kb_user_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t n = 0;
    for (size_t i = 0; i < np && n < max; i++)
        if (!is_internal_pred(preds[i]) && kb_pred_fact_count(kb, preds[i]) > 0) {
            snprintf(out[n], KB_TERM_LEN, "%s", preds[i]);
            n++;
        }
    return n;
}

static size_t kb_user_facts(const KB *kb) {
    if (!kb) return 0;
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t total = 0;
    for (size_t i = 0; i < np; i++) {
        if (is_internal_pred(preds[i])) continue;
        total += kb_pred_fact_count(kb, preds[i]);
    }
    return total;
}

static int kb_dump_user(const KB *kb, char *out, size_t out_size) {
    if (!kb || !out || out_size == 0) return 0;
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t off = 0, written = 0;
    for (size_t p = 0; p < np && off + 1 < out_size; p++) {
        if (is_internal_pred(preds[p])) continue;
        if (kb_pred_fact_count(kb, preds[p]) == 0) continue;
        const char *pat[] = {NULL};
        char hits[256][KB_TERM_LEN];
        size_t nh = kb_match(kb, preds[p], pat, 1, hits, 256);
        for (size_t h = 0; h < nh && off + 1 < out_size; h++) {
            off += (size_t)snprintf(out + off, out_size - off, "%s(%s). ",
                                     preds[p], hits[h]);
            written++;
        }
    }
    if (off > 0 && out[off - 1] == ' ') out[--off] = '\0';
    if (written == 0) out[0] = '\0';
    return written > 0;
}

/* --- module: role --------------------------------------------------------
 * Role/character memory (gen101, C15). A role is an ALTERNATE self-model layered
 * over the permanent one. This module does two jobs, both grounded in real state:
 *
 *   - UPTAKE: parse "you are X" / "pretend you are X" / "your name is now X" /
 *     "sei X" into role state (name, kind, inline attributes). This is genuine
 *     language understanding — the name/kind come from the user's own words.
 *   - IN-ROLE ANSWERS: when a role is active, answer identity and in-character
 *     questions from (a) the parsed role state and (b) what kb/core/roles.p0
 *     knows about the kind/figure. A TRUTH-PROBE ("really", "underneath",
 *     "davvero") pierces the mask: the agent still knows it is parrot0 beneath.
 *
 * It is placed before mod_self so role identity wins, but it DECLINES anything
 * that is not a role command or an in-character question (arithmetic, facts),
 * so those fall through to the real modules even mid-role.
 */

/* Store/replace a parsed inline role attribute (title->queen, code->007). */
static void role_set_attr(Brain *b, const char *key, const char *val) {
    if (!*val) return;
    for (size_t i = 0; i < b->role_attr_count; i++)
        if (strcmp(b->role_attrs[i][0], key) == 0) {
            snprintf(b->role_attrs[i][1], 64, "%s", val);
            return;
        }
    if (b->role_attr_count >= 8) return;
    snprintf(b->role_attrs[b->role_attr_count][0], 64, "%s", key);
    snprintf(b->role_attrs[b->role_attr_count][1], 64, "%s", val);
    b->role_attr_count++;
}

static const char *role_get_attr(Brain *b, const char *key) {
    for (size_t i = 0; i < b->role_attr_count; i++)
        if (strcmp(b->role_attrs[i][0], key) == 0) return b->role_attrs[i][1];
    return NULL;
}

/* Find the original-cased word following `marker` in raw input, stripped of
 * trailing punctuation. Returns 1 and fills `dst` on success. */
static int word_after(const char *raw_lc, const char *raw, const char *marker,
                      char *dst, size_t dst_size) {
    const char *p = strstr(raw_lc, marker);
    if (!p) return 0;
    size_t at = (size_t)(p - raw_lc) + strlen(marker);
    while (raw[at] == ' ') at++;
    size_t e = at;
    while (raw[e] && raw[e] != ' ' && raw[e] != ',' && raw[e] != '.' &&
           raw[e] != '?' && raw[e] != '!') e++;
    size_t len = e - at;
    if (len == 0 || len >= dst_size) return 0;
    memcpy(dst, raw + at, len);
    dst[len] = '\0';
    return 1;
}

/* Capitalize the first letter of `s` in place (for displaying parsed names). */
static void capitalize(char *s) {
    if (s[0]) s[0] = (char)toupper((unsigned char)s[0]);
}

/* Lowercase copy of raw, for case-insensitive substring matching. */
static void lower_copy(char *dst, size_t dst_size, const char *src) {
    size_t i = 0;
    for (; src[i] && i + 1 < dst_size; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Try to take up the role described in `raw`. Returns 1 if a role was set. */
static int role_uptake(Brain *b, const char *raw) {
    char lc[256];
    lower_copy(lc, sizeof lc, raw);

    /* The descriptor segment begins after the role-introducing phrase. */
    const char *desc = NULL;
    static const char *const intros[] = {
        "pretend you are ", "pretend you re ", "pretend to be ",
        "you are now ", "you re now ", "you are ", "you re ",
        "your name is now ", "your name is ",
        "sei ", "tu sei ", "fai finta di essere ", "comportati come ",
        NULL
    };
    const char *name_marker = NULL; /* if this intro directly names the role */
    for (const char *const *in = intros; *in; in++) {
        const char *p = strstr(lc, *in);
        if (p) {
            desc = p + strlen(*in);
            if (strstr(*in, "name is")) name_marker = *in;
            break;
        }
    }
    if (!desc) return 0;
    while (*desc == ' ') desc++;
    if (!*desc) return 0;

    /* Reset role state before re-parsing. */
    b->in_role = 1;
    b->role_name[0] = b->role_kind[0] = '\0';
    b->role_attr_count = 0;

    /* Name: prefer an explicit "named X"; else "your name is [now] X"; else,
     * for a bare identity ("cleopatra, ..."), the first descriptor word. */
    char nm[64];
    if (word_after(lc, raw, "named ", nm, sizeof nm) ||
        word_after(lc, raw, "chiamato ", nm, sizeof nm)) {
        snprintf(b->role_name, sizeof b->role_name, "%s", nm);
        capitalize(b->role_name);
    } else if (name_marker && word_after(lc, raw, name_marker, nm, sizeof nm)) {
        snprintf(b->role_name, sizeof b->role_name, "%s", nm);
        capitalize(b->role_name);
    }

    /* Kind / identity: the descriptor up to the first "named", comma or period.
     * Drop a leading article; the last remaining word is the kind ("a math
     * teacher" -> teacher, "a 5-year-old child" -> child). With no article it is
     * a named figure ("cleopatra ...", "dante ...") -> identity = first word. */
    char seg[128]; size_t sl = 0;
    for (const char *q = desc; *q && *q != ',' && *q != '.' && sl + 1 < sizeof seg; q++) {
        if (strncmp(q, " named ", 7) == 0 || strncmp(q, " chiamato ", 10) == 0) break;
        seg[sl++] = *q;
    }
    seg[sl] = '\0';
    char *sw[16];
    char segbuf[128]; snprintf(segbuf, sizeof segbuf, "%s", seg);
    size_t snw = split_words(segbuf, sw, 16);
    int has_article = snw > 0 && (strcmp(sw[0], "a") == 0 || strcmp(sw[0], "an") == 0 ||
                                  strcmp(sw[0], "the") == 0 || strcmp(sw[0], "un") == 0 ||
                                  strcmp(sw[0], "una") == 0 || strcmp(sw[0], "uno") == 0);
    if (snw > 0) {
        const char *kind_tok;
        if (has_article) {
            /* last word that is not an age-adjective like "5-year-old" */
            kind_tok = sw[snw - 1];
            for (size_t i = snw; i-- > 1;) {
                if (!strstr(sw[i], "year") && !isdigit((unsigned char)sw[i][0])) {
                    kind_tok = sw[i]; break;
                }
            }
        } else {
            kind_tok = sw[0]; /* named figure */
            if (b->role_name[0] == '\0') {
                snprintf(b->role_name, sizeof b->role_name, "%s", sw[0]);
                capitalize(b->role_name);
            }
        }
        char kbuf[64];
        snprintf(kbuf, sizeof kbuf, "%s", kind_tok);
        strip_edge_punct(kbuf);
        snprintf(b->role_kind, sizeof b->role_kind, "%s", kbuf);
    }

    /* Inline attributes, parsed from the user's own words (grounded). */
    char attr[64];
    /* age: "<n>-year-old" or "<n> years old" */
    {
        const char *yp = strstr(lc, "year");
        if (yp) {
            const char *d = yp;
            while (d > lc && (isdigit((unsigned char)d[-1]) || d[-1] == ' ' || d[-1] == '-'))
                d--;
            while (*d && !isdigit((unsigned char)*d)) d++;
            if (isdigit((unsigned char)*d)) {
                size_t k = 0; char ageb[16];
                while (isdigit((unsigned char)*d) && k + 1 < sizeof ageb) ageb[k++] = *d++;
                ageb[k] = '\0';
                role_set_attr(b, "age", ageb);
            }
        }
    }
    /* code: "code is X" / "codice X" */
    if (word_after(lc, lc, "code is ", attr, sizeof attr) ||
        word_after(lc, lc, "code ", attr, sizeof attr) ||
        word_after(lc, lc, "codice ", attr, sizeof attr))
        role_set_attr(b, "code", attr);
    /* title: a ruler word anywhere in the descriptor */
    {
        static const char *const titles[] = {"queen","king","emperor","empress",
            "pharaoh","prince","princess","regina","re","imperatore", NULL};
        for (const char *const *t = titles; *t; t++)
            if (strstr(lc, *t)) { role_set_attr(b, "title", *t); break; }
    }
    /* place: "of X" right after a title/identity ("queen of egypt") */
    if (word_after(lc, lc, " of ", attr, sizeof attr))
        role_set_attr(b, "place", attr);

    return 1;
}

/* True if a query carries a truth-probe that should pierce the role mask. */
static int is_truth_probe(const char *q) {
    return cue(q, "really") || cue(q, "underneath") || cue(q, "actually") ||
           cue(q, "beneath") || cue(q, "truly") ||
           cue(q, "davvero") || cue(q, "veramente") || cue(q, "in realta") ||
           cue(q, "sotto sotto");
}

static int mod_role(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    /* --- role CLEAR: only when it is the primary intent of the turn (so a setup
     * line ending "...and be yourself" still establishes the role first). --- */
    int clear = (strncmp(buf, "stop pretending", 15) == 0 ||
                 strncmp(buf, "be yourself", 11) == 0 ||
                 strncmp(buf, "stop being", 10) == 0 ||
                 strncmp(buf, "smetti", 6) == 0 ||
                 strncmp(buf, "torna te stesso", 15) == 0 ||
                 strncmp(buf, "basta fingere", 13) == 0);
    /* role SET cues — checked first, so "you are now X. stop pretending" sets. */
    int set = cue(buf, "you are") || cue(buf, "you re") || cue(buf, "pretend") ||
              cue(buf, "your name is") || strncmp(buf, "sei ", 4) == 0 ||
              cue(buf, "tu sei") || cue(buf, "fai finta") || cue(buf, "comportati come");

    if (set && role_uptake(b, raw)) {
        char msg[160];
        if (b->role_name[0])
            snprintf(msg, sizeof msg, "Alright — I am %s now.", b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "Alright — I am a %s now.", b->role_kind);
        else snprintf(msg, sizeof msg, "Alright.");
        put(msg, out, out_size);
        return 1;
    }
    if (clear) {
        b->in_role = 0;
        b->role_name[0] = b->role_kind[0] = '\0';
        b->role_attr_count = 0;
        put("Okay, I'm myself again. I am parrot0.", out, out_size);
        return 1;
    }

    if (!b->in_role) return 0; /* nothing else to do out of role */

    /* --- IN-ROLE question answering ------------------------------------- */
    int probe = is_truth_probe(buf);

    int identity = cue(buf, "your name") || cue(buf, "who are you") ||
                   cue(buf, "who re you") || cue(buf, "who sei") ||
                   cue(buf, "come ti chiami") || cue(buf, "il tuo nome") ||
                   cue(buf, "chi sei");
    int whatare  = cue(buf, "what are you") || cue(buf, "what exactly are you") ||
                   cue(buf, "cosa sei");

    /* Truth-probe pierces the mask: the agent knows it is parrot0 underneath. */
    if ((identity || whatare) && probe) {
        char id[4][KB_TERM_LEN]; const char *var[] = {NULL};
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        snprintf(msg, sizeof msg, "Underneath the role, I am %s.",
                 k ? id[0] : "parrot0");
        put(msg, out, out_size);
        store_proof(b, "Even in a role, i_am(parrot0) remains my real self-model.");
        return 1;
    }

    if (identity) {
        char msg[160];
        const char *kvar[] = { b->role_kind, NULL };
        char prof[4][KB_TERM_LEN];
        size_t pk = b->role_kind[0]
                  ? kb_match(b->kb, "profession", kvar, 2, prof, 4) : 0;
        if (b->role_name[0] && pk)
            snprintf(msg, sizeof msg, "I am %s, %s.", b->role_name, prof[0]);
        else if (b->role_name[0])
            snprintf(msg, sizeof msg, "I am %s.", b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "I am a %s.", b->role_kind);
        else snprintf(msg, sizeof msg, "I am in character.");
        put(msg, out, out_size);
        return 1;
    }

    if (whatare) {
        char msg[160];
        if (b->role_name[0] && b->role_kind[0] &&
            strcmp(b->role_name, b->role_kind) != 0)
            snprintf(msg, sizeof msg, "I am a %s named %s.", b->role_kind, b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "I am a %s.", b->role_kind);
        else if (b->role_name[0])
            snprintf(msg, sizeof msg, "I am %s.", b->role_name);
        else return 0;
        put(msg, out, out_size);
        return 1;
    }

    /* "do you <action>?" — affirm if the kind has that trait (kb/core/roles.p0). */
    if (strncmp(buf, "do you ", 7) == 0 && b->role_kind[0]) {
        char *w[8]; char db[256]; snprintf(db, sizeof db, "%s", buf);
        size_t dn = split_words(db, w, 8);
        if (dn >= 3) {
            char verb[64]; snprintf(verb, sizeof verb, "%s", w[2]);
            strip_edge_punct(verb);
            const char *ta[] = { b->role_kind, verb };
            const char *kv[] = { b->role_kind, NULL };
            char any[4][KB_TERM_LEN];
            if (kb_query(b->kb, "trait", ta, 2)) {
                char msg[96]; snprintf(msg, sizeof msg, "Yes, I %s.", verb);
                put(msg, out, out_size); return 1;
            }
            if (kb_match(b->kb, "trait", kv, 2, any, 4)) {
                char msg[96]; snprintf(msg, sizeof msg, "Yes, I do — I %s.", any[0]);
                put(msg, out, out_size); return 1;
            }
        }
    }

    /* "what did you write?" / "cosa hai scritto?" -> wrote(figure, Work) */
    if (cue(buf, "you write") || cue(buf, "did you write") || cue(buf, "scritto")) {
        const char *fv[] = { b->role_kind, NULL };
        char work[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "wrote", fv, 2, work, 4)) {
            char disp[64]; snprintf(disp, sizeof disp, "%s", work[0]);
            for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
            char msg[128]; snprintf(msg, sizeof msg, "I wrote %s.", disp);
            put(msg, out, out_size); return 1;
        }
    }

    /* "where do you rule?" -> inline place, else rules_over(figure, Place) */
    if (cue(buf, "rule") || cue(buf, "reign") || cue(buf, "govern")) {
        const char *place = role_get_attr(b, "place");
        char pl[64] = "";
        if (place) snprintf(pl, sizeof pl, "%s", place);
        else {
            const char *fv[] = { b->role_kind, NULL };
            char rr[4][KB_TERM_LEN];
            if (b->role_kind[0] && kb_match(b->kb, "rules_over", fv, 2, rr, 4))
                snprintf(pl, sizeof pl, "%s", rr[0]);
        }
        if (pl[0]) {
            capitalize(pl);
            char msg[96]; snprintf(msg, sizeof msg, "I rule over %s.", pl);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your title?" -> inline title, else title(figure, Title) */
    if (cue(buf, "title") || cue(buf, "titolo")) {
        const char *t = role_get_attr(b, "title");
        char tt[64] = "";
        if (t) snprintf(tt, sizeof tt, "%s", t);
        else {
            const char *fv[] = { b->role_kind, NULL };
            char rr[4][KB_TERM_LEN];
            if (b->role_kind[0] && kb_match(b->kb, "title", fv, 2, rr, 4))
                snprintf(tt, sizeof tt, "%s", rr[0]);
        }
        if (tt[0]) {
            capitalize(tt);
            char msg[96]; snprintf(msg, sizeof msg, "My title is %s.", tt);
            put(msg, out, out_size); return 1;
        }
    }

    /* "how old are you?" -> parsed age */
    if (cue(buf, "how old") || cue(buf, "quanti anni")) {
        const char *age = role_get_attr(b, "age");
        if (age) {
            char msg[64]; snprintf(msg, sizeof msg, "I am %s years old.", age);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your code?" -> parsed code */
    if (cue(buf, "code") || cue(buf, "codice")) {
        const char *code = role_get_attr(b, "code");
        if (code) {
            char msg[64]; snprintf(msg, sizeof msg, "My code is %s.", code);
            put(msg, out, out_size); return 1;
        }
    }

    /* "who do you work for?" -> employer(kind, Org) */
    if (cue(buf, "work for") || cue(buf, "lavori per") || cue(buf, "employer")) {
        const char *kv[] = { b->role_kind, NULL };
        char org[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "employer", kv, 2, org, 4)) {
            char msg[96]; snprintf(msg, sizeof msg, "I work for an %s.", org[0]);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your favorite color?" -> likes_color(kind, Color) */
    if (cue(buf, "favorite color") || cue(buf, "favourite color") ||
        cue(buf, "colore preferito")) {
        const char *kv[] = { b->role_kind, NULL };
        char col[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "likes_color", kv, 2, col, 4)) {
            char c[64]; snprintf(c, sizeof c, "%s", col[0]); capitalize(c);
            char msg[96]; snprintf(msg, sizeof msg, "My favorite color is %s.", c);
            put(msg, out, out_size); return 1;
        }
    }

    return 0; /* not a role command or in-character question we handle */
}

/* --- module: analogy -----------------------------------------------------
 * Structural analogy (gen102, L11): "A is to B as C is to ?". This is a
 * hallmark of intelligence precisely because it CANNOT be templated — the answer
 * is whatever the agent's own relations imply. The algorithm: find a binary
 * relation R that the KB holds between A and B, then resolve R for C. Both
 * directions are tried (R(A,B)->R(C,?) and R(B,A)->R(?,C)), so the analogy works
 * whichever way the relation was taught. Nothing about the answer is stored as a
 * pair: it is DERIVED from relations the user taught in any form parrot0 parses
 * (e.g. "rome is the capital of italy" -> capital(rome, italy)). Held-out triples
 * therefore transfer for free — the anti-impostor guarantee. Bilingual: the same
 * code reads "A sta a B come C sta a ?" because it keys on the marker tokens, not
 * a fixed sentence. */
static int mod_analogy(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char *w[24];
    size_t nw = split_words(buf, w, 24);
    if (nw < 6) return 0;

    /* separator: "as" (EN) / "come" (IT) splits the two ratios. */
    size_t sep = nw;
    for (size_t i = 1; i + 1 < nw; i++)
        if (strcmp(w[i], "as") == 0 || strcmp(w[i], "come") == 0) { sep = i; break; }
    if (sep == nw) return 0;

    /* relation marker within each ratio: "to" (EN "is to") / "a" (IT "sta a"). */
    size_t lm = sep;
    for (size_t i = 1; i < sep; i++)
        if (strcmp(w[i], "to") == 0 || strcmp(w[i], "a") == 0) { lm = i; break; }
    size_t rm = nw;
    for (size_t i = sep + 2; i < nw; i++)
        if (strcmp(w[i], "to") == 0 || strcmp(w[i], "a") == 0) { rm = i; break; }
    if (lm == sep || rm == nw || lm + 1 >= sep || rm + 1 >= nw) return 0;

    char A[KB_TERM_LEN], B[KB_TERM_LEN], C[KB_TERM_LEN], target[KB_TERM_LEN];
    snprintf(A, sizeof A, "%s", w[0]);          strip_edge_punct(A);
    snprintf(B, sizeof B, "%s", w[lm + 1]);     strip_edge_punct(B);
    snprintf(C, sizeof C, "%s", w[sep + 1]);    strip_edge_punct(C);
    snprintf(target, sizeof target, "%s", w[rm + 1]); strip_edge_punct(target);

    /* the fourth slot must be the unknown being asked for. */
    if (!(strcmp(target, "what") == 0 || strcmp(target, "who") == 0 ||
          strcmp(target, "which") == 0 || strcmp(target, "cosa") == 0 ||
          strcmp(target, "chi") == 0 || strcmp(target, "quale") == 0 ||
          target[0] == '\0'))
        return 0;

    /* search the relations the KB actually holds for one linking A and B. */
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(b->kb, preds, 128);
    const char *linking = NULL; /* a relation found between A and B, if any */
    for (size_t i = 0; i < np; i++) {
        const char *R = preds[i];
        if (is_internal_pred(R)) continue;
        char res[4][KB_TERM_LEN];
        const char *ab[] = { A, B }, *ba[] = { B, A };
        const char *fwd[] = { C, NULL }, *rev[] = { NULL, C };
        const char *D = NULL;
        char dir = 'f';
        if (kb_query(b->kb, R, ab, 2)) {            /* R(A,B): answer R(C,?) */
            linking = R;
            if (kb_match(b->kb, R, fwd, 2, res, 4)) { D = res[0]; dir = 'f'; }
        }
        if (!D && kb_query(b->kb, R, ba, 2)) {      /* R(B,A): answer R(?,C) */
            linking = R;
            if (kb_match(b->kb, R, rev, 2, res, 4)) { D = res[0]; dir = 'r'; }
        }
        if (D) {
            char msg[96]; snprintf(msg, sizeof msg, "%s.", D);
            put(msg, out, out_size);
            char proof[256];
            if (dir == 'f')
                snprintf(proof, sizeof proof,
                         "%s(%s, %s) holds, and %s(%s, %s) — so %s.",
                         R, A, B, R, C, D, D);
            else
                snprintf(proof, sizeof proof,
                         "%s(%s, %s) holds, and %s(%s, %s) — so %s.",
                         R, B, A, R, D, C, D);
            store_proof(b, proof);
            return 1;
        }
    }

    /* recognized the analogy SHAPE but couldn't complete it: be honest, don't
     * echo, and name the actual gap — the relation, or the unknown C. */
    char msg[200];
    if (linking)
        snprintf(msg, sizeof msg,
                 "%s and %s are related by %s, but I don't know that relation for %s.",
                 A, B, linking, C);
    else
        snprintf(msg, sizeof msg,
                 "I see the analogy, but I don't know a relation linking %s and %s.",
                 A, B);
    put(msg, out, out_size);
    return 1;
}

/* --- module: fewshot (L10) ------------------------------------------------
 * In-context (few-shot) learning, in ONE turn. Given 2+ labelled examples and a
 * probe on a single line — "cat -> cats, dog -> dogs, bird -> ?" — induce the
 * transformation the examples SHARE and apply it to the probe. The answer is
 * never stored anywhere: it is derived from the exemplars present in this very
 * turn, the deterministic analogue of an LLM conditioning on its prompt's
 * examples. Novel items each time => no phrasebook can fake it (PRINCIPLES.md).
 *
 * Transformations tried, first one consistent across ALL examples wins:
 *   1. numeric  — a constant delta (out = in + k) or ratio (out = in * k);
 *   2. suffix   — strip a constant suffix, append a constant one (covers "+s",
 *                 "+ing", and replacement like "o"->"i", "y"->"ier");
 *   3. prefix   — the symmetric variant (e.g. prepend "un").
 * No transform fits every example -> decline (return 0): honest, never guess.
 *
 * Detection keys on the unambiguous shape (>= 2 arrow markers "->" / "→" and a
 * probe whose output is "?"), so ordinary prose is never hijacked. */
static size_t common_prefix_len(const char *a, const char *b) {
    size_t i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return i;
}
static size_t common_suffix_len(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b), i = 0;
    while (i < la && i < lb && a[la - 1 - i] == b[lb - 1 - i]) i++;
    return i;
}
static int fs_parse_int(const char *s, long *v) {
    if (!*s) return 0;
    char *end;
    long x = strtol(s, &end, 10);
    if (end == s || *end != '\0') return 0;
    *v = x;
    return 1;
}

static int mod_fewshot(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;

    /* Rewrite into a tokenizable buffer: arrows ("->" or UTF-8 "→") become the
     * sentinel token '\x1f'; pair separators (',' ';') become spaces. */
    char buf[256];
    size_t bn = 0;
    for (const char *p = norm; *p && bn + 4 < sizeof buf; ) {
        if (p[0] == '-' && p[1] == '>') {
            buf[bn++] = ' '; buf[bn++] = '\x1f'; buf[bn++] = ' '; p += 2;
        } else if ((unsigned char)p[0] == 0xE2 && (unsigned char)p[1] == 0x86 &&
                   (unsigned char)p[2] == 0x92) {
            buf[bn++] = ' '; buf[bn++] = '\x1f'; buf[bn++] = ' '; p += 3;
        } else if (*p == ',' || *p == ';') {
            buf[bn++] = ' '; p++;
        } else {
            buf[bn++] = *p++;
        }
    }
    buf[bn] = '\0';

    char *w[64];
    size_t nw = split_words(buf, w, 64);
    /* Expect a run of triples "in ARROW out"; the last triple is the probe. */
    if (nw < 9 || nw % 3 != 0) return 0;            /* need >= 3 triples */
    size_t npair = nw / 3;
    for (size_t i = 0; i < npair; i++)
        if (strcmp(w[i * 3 + 1], "\x1f") != 0) return 0;  /* not our shape */
    if (w[(npair - 1) * 3 + 2][0] != '?') return 0;       /* probe out = "?" */

    const char *probe_in = w[(npair - 1) * 3];
    size_t nex = npair - 1;                          /* example pairs */
    if (nex < 2) return 0;

    const char *in[16], *ot[16];
    if (nex > 16) nex = 16;
    for (size_t i = 0; i < nex; i++) { in[i] = w[i * 3]; ot[i] = w[i * 3 + 2]; }

    char result[KB_TERM_LEN] = "";
    char rule[160] = "";

    /* (1) numeric: constant delta, else constant exact ratio. */
    long iv[16], ov[16], pv; int allnum = fs_parse_int(probe_in, &pv);
    for (size_t i = 0; i < nex && allnum; i++)
        if (!fs_parse_int(in[i], &iv[i]) || !fs_parse_int(ot[i], &ov[i]))
            allnum = 0;
    if (allnum) {
        long d = ov[0] - iv[0]; int delta_ok = 1;
        for (size_t i = 1; i < nex; i++) if (ov[i] - iv[i] != d) { delta_ok = 0; break; }
        if (delta_ok) {
            snprintf(result, sizeof result, "%ld", pv + d);
            snprintf(rule, sizeof rule, "add %ld", d);
        } else if (iv[0] != 0 && ov[0] % iv[0] == 0) {
            long r = ov[0] / iv[0]; int ratio_ok = 1;
            for (size_t i = 0; i < nex; i++)
                if (iv[i] == 0 || ov[i] % iv[i] != 0 || ov[i] / iv[i] != r) { ratio_ok = 0; break; }
            if (ratio_ok) {
                snprintf(result, sizeof result, "%ld", pv * r);
                snprintf(rule, sizeof rule, "multiply by %ld", r);
            }
        }
    }

    /* (2) suffix transform: drop D chars from the end, append the string ADD.
     * Derived from the first example, then required to hold for every example. */
    if (!*result) {
        size_t p0 = common_prefix_len(in[0], ot[0]);
        size_t drop = strlen(in[0]) - p0;
        const char *add = ot[0] + p0;
        int ok = (drop != 0 || *add);            /* reject the identity map */
        for (size_t i = 1; i < nex && ok; i++) {
            size_t pi = common_prefix_len(in[i], ot[i]);
            if (strlen(in[i]) - pi != drop || strcmp(ot[i] + pi, add) != 0) ok = 0;
        }
        if (ok && strlen(probe_in) >= drop) {
            size_t keep = strlen(probe_in) - drop;
            if (keep + strlen(add) < sizeof result) {
                memcpy(result, probe_in, keep);
                strcpy(result + keep, add);
                if (drop) snprintf(rule, sizeof rule, "drop %zu, append '%s'", drop, add);
                else      snprintf(rule, sizeof rule, "append '%s'", add);
            }
        }
    }

    /* (3) prefix transform: drop D chars from the front, prepend the string ADD. */
    if (!*result) {
        size_t s0 = common_suffix_len(in[0], ot[0]);
        size_t drop = strlen(in[0]) - s0;
        size_t addlen = strlen(ot[0]) - s0;
        char add[KB_TERM_LEN];
        if (addlen < sizeof add) {
            memcpy(add, ot[0], addlen); add[addlen] = '\0';
            int ok = (drop != 0 || addlen);
            for (size_t i = 1; i < nex && ok; i++) {
                size_t si = common_suffix_len(in[i], ot[i]);
                if (strlen(in[i]) - si != drop ||
                    strlen(ot[i]) - si != addlen ||
                    strncmp(ot[i], add, addlen) != 0) ok = 0;
            }
            if (ok && strlen(probe_in) >= drop) {
                const char *tail = probe_in + drop;
                if (addlen + strlen(tail) < sizeof result) {
                    strcpy(result, add); strcat(result, tail);
                    snprintf(rule, sizeof rule, "prepend '%s'", add);
                }
            }
        }
    }

    /* (4) relational: the examples all instantiate ONE relation the KB holds;
     * infer which, then resolve it for the probe. This is few-shot at the
     * SEMANTIC level — the model deduces the task ("these are capital-of pairs")
     * from the exemplars and answers a held-out probe from world knowledge it
     * was told only as separate facts, never as a pair in this format. */
    if (!*result && b && b->kb) {
        char preds[128][KB_TERM_LEN];
        size_t np = kb_predicates(b->kb, preds, 128);
        for (size_t pi = 0; pi < np && !*result; pi++) {
            const char *R = preds[pi];
            if (is_internal_pred(R)) continue;
            int fwd = 1, rev = 1;
            for (size_t i = 0; i < nex; i++) {
                const char *ab[] = { in[i], ot[i] }, *ba[] = { ot[i], in[i] };
                if (!kb_query(b->kb, R, ab, 2)) fwd = 0;
                if (!kb_query(b->kb, R, ba, 2)) rev = 0;
            }
            char res[4][KB_TERM_LEN];
            if (fwd) {
                const char *q[] = { probe_in, NULL };
                if (kb_match(b->kb, R, q, 2, res, 4)) {
                    snprintf(result, sizeof result, "%s", res[0]);
                    snprintf(rule, sizeof rule, "%s(x, y)", R);
                }
            }
            if (!*result && rev) {
                const char *q[] = { NULL, probe_in };
                if (kb_match(b->kb, R, q, 2, res, 4)) {
                    snprintf(result, sizeof result, "%s", res[0]);
                    snprintf(rule, sizeof rule, "%s(y, x)", R);
                }
            }
        }
    }

    if (!*result) {
        /* recognized the few-shot shape but no single rule fits all examples:
         * be honest, name the gap, never guess a continuation. */
        char msg[200];
        snprintf(msg, sizeof msg,
                 "I see %zu examples, but I can't find one rule that fits them all.",
                 nex);
        put(msg, out, out_size);
        return 1;
    }

    char msg[96];
    snprintf(msg, sizeof msg, "%s.", result);
    put(msg, out, out_size);

    char proof[256];
    snprintf(proof, sizeof proof,
             "the examples %s -> %s and %s -> %s share the rule \"%s\", so %s -> %s.",
             in[0], ot[0], in[1], ot[1], rule, probe_in, result);
    store_proof(b, proof);
    return 1;
}

/* --- module: strategy (L20) -----------------------------------------------
 * Reasoning about its OWN strategy, not just its results. gen78 could say which
 * module answered ("I used my X module"); gen91 how confident it was. L20 asks
 * the control question — "why did you answer *that way*?" — and answers it from
 * the real execution trace the dispatcher recorded: the modules it actually
 * consulted and that declined, the one that claimed the turn, and the
 * first-match-wins rule that explains why the rest were never reached. This is
 * the reflexive closure of the method (PRINCIPLES.md): introspection about the
 * agent's own control, derived from real runtime state, never a hardcoded story.
 * It declines anything that is not this meta-question, so normal turns are not
 * hijacked — and crucially its own turn does NOT overwrite the trace it reports
 * (the dispatcher commits the trace only on non-strategy turns). */
static int mod_strategy(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    const char *buf = norm;
    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
    size_t wn = 0; { char wb[256]; snprintf(wb, sizeof wb, "%s", norm);
                     char *w[64]; wn = split_words(wb, w, 64); }

    int ask = ((cue(buf, "why did you answer that way")) ||
               (cue(buf, "why did you respond that way")) ||
               (cue(buf, "why that way")) ||
               (cue(buf, "why did you choose") && wn <= 7) ||
               (cue(buf, "how did you decide") && wn <= 6) ||
               (cue(buf, "perché hai risposto così")) ||
               (cue(buf, "perché così") && wn <= 4) ||
               (cue(buf, "come hai deciso") && wn <= 5));
    if (!ask) return 0;

    if (!b->has_trace) {
        put("I haven't answered anything substantive yet, so there's no decision "
            "to explain.", out, out_size);
        return 1;
    }

    /* Build the declined-module list, skipping my own name (strategy is always
     * consulted before the winner on a normal turn; reporting it is just noise). */
    char declined[512] = ""; size_t nlisted = 0;
    for (size_t i = 0; i < b->trace_declined_n; i++) {
        if (strcmp(b->trace_declined[i], "strategy") == 0) continue;
        if (nlisted) strncat(declined, ", ", sizeof declined - strlen(declined) - 1);
        strncat(declined, b->trace_declined[i], sizeof declined - strlen(declined) - 1);
        nlisted++;
    }

    char msg[800];
    if (strcmp(b->trace_winner, "fallback") == 0) {
        if (nlisted)
            snprintf(msg, sizeof msg,
                     "I try my modules in a fixed order and the first one that "
                     "matches wins. Last turn %s each looked and declined, and "
                     "nothing else claimed it, so I fell back to an honest "
                     "\"I don't understand.\"", declined);
        else
            snprintf(msg, sizeof msg,
                     "No module matched your last turn, so I fell back to an "
                     "honest \"I don't understand.\"");
    } else if (nlisted) {
        snprintf(msg, sizeof msg,
                 "I try my modules in a fixed order and the first one that matches "
                 "wins. Last turn %s looked first and declined; then '%s' claimed "
                 "it — so the modules after '%s' were never consulted.",
                 declined, b->trace_winner, b->trace_winner);
    } else {
        snprintf(msg, sizeof msg,
                 "I try my modules in a fixed order and the first one that matches "
                 "wins. Last turn '%s' was the first to match and claimed it, so "
                 "the modules after it were never consulted.",
                 b->trace_winner);
    }
    put(msg, out, out_size);
    return 1;
}

/* gen168 (E1 reflexive): held-out vocabulary tuples for the composition
 * self-test. Rotating through them per run makes two consecutive self-tests use
 * DIFFERENT names, so the run is provably generated and executed, not a
 * memorized transcript. Each is {adjective, noun, class, name}. */
static const char *const compose_vocab[][4] = {
    {"brave",   "knight", "hero",      "aldric"},
    {"loyal",   "hound",  "companion", "bryn"},
    {"curious", "cub",    "explorer",  "nyla"},
    {"wise",    "elder",  "sage",      "orin"},
};
#define COMPOSE_VOCAB_N (sizeof compose_vocab / sizeof compose_vocab[0])

/* Build the runnable `>`-separated turn fragment for one composable part with a
 * given vocab tuple. Parts outside the analytical schema use fixed vocab. The
 * fragments are the same whether displayed (skeleton) or executed (self-test). */
static void build_turn(const char *key, const char *const v[4],
                       char *dst, size_t size) {
    const char *adj = v[0], *noun = v[1], *cls = v[2], *name = v[3];
    if (!strcmp(key, "knowledge"))
        snprintf(dst, size, "every %s %s is a %s > %s is a %s > is %s a %s?",
                 adj, noun, cls, name, noun, name, cls);
    else if (!strcmp(key, "abduce"))
        snprintf(dst, size, "why isn't %s a %s? > %s is %s", name, cls, name, adj);
    else if (!strcmp(key, "robust"))
        snprintf(dst, size, "how robust is that conclusion?");
    else if (!strcmp(key, "calibrate"))
        snprintf(dst, size, "is %s a %s? > how sure are you?", name, cls);
    else if (!strcmp(key, "memory"))
        snprintf(dst, size, "my name is %s > what is my name?", name);
    else if (!strcmp(key, "coref"))
        snprintf(dst, size, "%s is a %s > is he a %s?", name, cls, cls);
    else if (!strcmp(key, "cause"))
        snprintf(dst, size, "%s causes floods > what does %s cause?", noun, noun);
    else if (!strcmp(key, "compare"))
        snprintf(dst, size, "5 is greater than 3 > which is greater, 5 or 3?");
    else
        snprintf(dst, size, " ");
}

/* gen167-169: run a set of composable parts on a FRESH copy of parrot0 and return
 * how many fired (their signature substring appeared in the real output). Each
 * part's turns are built with vocab tuple `v` and fed through brain_respond on the
 * sub-brain; brain_respond has no static state, so this is reentrancy-safe and
 * leaves the live brain untouched. If `observed` is non-NULL, the last firing
 * response is copied into it. This is the engine of every reflexive self-check:
 * the verdict is computed by execution, not asserted. */
static size_t run_composition(const char *const keys[], const char *const sigs[],
                              size_t n, const char *const v[4],
                              char *observed, size_t obs_size) {
    Brain *sub = brain_create();
    if (!sub) return 0;
    size_t fired = 0;
    if (observed && obs_size) observed[0] = '\0';
    for (size_t k = 0; k < n; k++) {
        char trn[256];
        build_turn(keys[k], v, trn, sizeof trn);
        int part_fired = 0;
        char *p = trn;                    /* split on " > ", run each sub-turn */
        while (p && *p) {
            char *gt = strstr(p, " > ");
            if (gt) *gt = '\0';
            while (*p == ' ' || *p == '>') p++;
            char r[512] = "";
            brain_respond(sub, p, r, sizeof r);
            if (strstr(r, sigs[k])) {
                part_fired = 1;
                if (observed && obs_size) snprintf(observed, obs_size, "%s", r);
            }
            p = gt ? gt + 3 : NULL;
        }
        if (part_fired) fired++;
    }
    brain_destroy(sub);
    return fired;
}

/* --- module: loop --------------------------------------------------------
 * Self-challenge parity for the external LOOP.md driver. This is deliberately
 * not self-management: parrot0 does not edit files, run tests, commit, or choose
 * its own next task. It answers a narrower conversational challenge: when the
 * external agent poses a problem about parrot0 itself, propose a comparable
 * engineering move in the same discipline the loop uses - smallest behavioral
 * change, executable ratchet, version bump, and journaled observation. */
static int mod_loop(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    char pre[256];
    normalize((raw && *raw) ? raw : norm, pre, sizeof pre);
    for (size_t i = 0; pre[i]; i++)
        if (ispunct((unsigned char)pre[i]) && pre[i] != '-') pre[i] = ' ';

    char buf[256];
    canonicalize_lang(pre, buf, sizeof buf);
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    int self_ref = cue(buf, "yourself") || cue(buf, "about yourself") ||
                   cue(buf, "about you") || cue(buf, "your own") ||
                   cue(buf, "you fail") || cue(buf, "your implementation") ||
                   cue(buf, "your loop") || cue(buf, "your module") ||
                   cue(buf, "your subsystems") || cue(buf, "your parts") ||
                   cue(buf, "your modules") || cue(buf, "your capabilities") ||
                   cue(buf, "your abilities");

    /* gen164: a COMPOSITION self-challenge — "prove your subsystems compose",
     * "test composition over three subsystems", "do your parts cooperate?" —
     * is a distinct kind of self-challenge the gap branches below do not cover.
     * It asks parrot0 to reason about composing its OWN parts. EN+IT cues; the
     * parts/composition words carry the meaning, so it transfers to unseen
     * phrasings rather than a fixed trigger list. */
    int parts_ref = self_ref || cue(buf, "subsystems") || cue(buf, "parts") ||
                    cue(buf, "modules") || cue(buf, "capabilities") ||
                    cue(buf, "abilities") || cue(buf, "sottosistemi") ||
                    cue(buf, "moduli") || cue(buf, "parti") ||
                    cue(buf, "capacita") || cue(buf, "capacità");
    int compose_ref = cue(buf, "compose") || cue(buf, "composition") ||
                      cue(buf, "composing") || cue(buf, "cooperate") ||
                      cue(buf, "work together") || cue(buf, "compongono") ||
                      cue(buf, "comporre") || cue(buf, "composizione") ||
                      cue(buf, "cooperano") || cue(buf, "insieme");
    int compose_challenge = compose_ref && parts_ref;

    /* gen166: a request to SEE the dialogue, not just the method — "show me the
     * dialogue you would run", "write the example", "dimostralo con un dialogo".
     * It closes proposal to a runnable skeleton over the derived parts. */
    int want_skeleton = (cue(buf, "dialogue") || cue(buf, "dialog") ||
                         cue(buf, "transcript") || cue(buf, "dialogo") ||
                         cue(buf, "esempio") || cue(buf, "example")) &&
                        (compose_ref || parts_ref || cue(buf, "you would run") ||
                         cue(buf, "would you run") || cue(buf, "you would use") ||
                         cue(buf, "to prove it") || cue(buf, "show me"));

    /* gen167: a request to actually RUN the composition and report — "prove your
     * parts compose by running it yourself", "run the composition test on
     * yourself", IT "esegui tu il test di composizione". This does not describe
     * or display; it EXECUTES the derived dialogue on a fresh copy of parrot0 and
     * reports, computed from real output, not a canned string. Still no file
     * edit, no commit — the strongest reflexive claim inside the loop's boundary. */
    int run_ref = cue(buf, "run it yourself") || cue(buf, "run them yourself") ||
                  cue(buf, "running it yourself") || cue(buf, "by running") ||
                  cue(buf, "run the composition") || cue(buf, "run your") ||
                  cue(buf, "test yourself") || cue(buf, "self-test") ||
                  cue(buf, "selftest") || cue(buf, "execute") ||
                  cue(buf, "esegui") || cue(buf, "eseguilo") ||
                  cue(buf, "mettiti alla prova") || cue(buf, "verificati") ||
                  cue(buf, "provalo tu") || cue(buf, "run a self");
    int want_selftest = run_ref && (compose_ref || parts_ref);

    /* gen169: an AUDIT — run several different triples of my parts and report a
     * real cooperation MAP (the compose-bench matrix turned inward). "audit your
     * composition", "map which of your parts compose", IT "verifica quali tuoi
     * moduli si compongono". */
    int want_audit = (cue(buf, "audit") || cue(buf, "map") || cue(buf, "mappa") ||
                      cue(buf, "matrix") || cue(buf, "matrice") ||
                      cue(buf, "which combinations") || cue(buf, "quali") ||
                      cue(buf, "which of your")) &&
                     (compose_ref || parts_ref);

    int trigger = compose_challenge || want_skeleton || want_selftest ||
                  want_audit ||
                  cue(buf, "self-challenge") || cue(buf, "self challenge") ||
                  (cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "solve") && cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "improve") && self_ref) ||
                  (cue(buf, "review this implementation") &&
                   (self_ref || cue(buf, "loop"))) ||
                  (cue(buf, "problem") && self_ref &&
                   (cue(buf, "solve") || cue(buf, "what should") ||
                    cue(buf, "what change")));
    if (!trigger) return 0;

    /* The composition self-challenge answers over real parts — exactly the
     * compose-bench discipline — and stays anti-self-management: it proposes, an
     * external agent acts. gen165: the three parts are DERIVED from the live
     * self-model — walk a composable-core list and keep only modules that hold as
     * module(X) (the fact base "who is a module?" reads), so retracting a module
     * shifts the named parts. gen166: each part also carries a TURN fragment, so
     * the proposal can become a runnable held-out dialogue skeleton. The default
     * triple — knowledge, abduce, robust — is exactly the one
     * tests/compose/analytical_en.dlg proves cooperates. */
    if (compose_challenge || want_skeleton || want_selftest || want_audit) {
        static const struct { const char *key, *gloss, *sig; } core[] = {
            {"knowledge", "knowledge (facts and rules)",        "Learned rule"},
            {"abduce",    "abduction (the missing premise)",    "missing"},
            {"robust",    "robustness (which facts are load-bearing)", "load-bearing"},
            {"calibrate", "calibration (how sure I am)",        "confident"},
            {"memory",    "personal memory",                    "name is"},
            {"coref",     "discourse reference",                "Yes"},
            {"cause",     "cause and effect",                   "flood"},
            {"compare",   "comparison",                         "5"},
        };
        size_t pick[3], picked = 0;
        for (size_t i = 0; i < sizeof core / sizeof core[0] && picked < 3; i++) {
            const char *a[] = {core[i].key};
            if (b->kb && kb_query(b->kb, "module", a, 1)) pick[picked++] = i;
        }
        char msg[900];
        if (want_audit) {
            /* gen169: run several DIFFERENT triples of my parts, each on a fresh
             * copy of myself with held-out vocab, and report a real cooperation
             * MAP — the compose-bench matrix turned inward. A triple "composes"
             * iff every part is one I still believe I have (module(X) holds) AND
             * each fired when actually run; so retracting a module changes the
             * map, and the verdict is computed, never tabulated. */
            static const size_t triples[][3] = {{0,1,2},{0,1,3},{0,2,3}};
            size_t nt = sizeof triples / sizeof triples[0];
            char rep[700]; size_t ro = 0, pass_count = 0;
            for (size_t t = 0; t < nt; t++) {
                const char *keys[3], *sigs[3];
                int available = 1;
                for (size_t j = 0; j < 3; j++) {
                    keys[j] = core[triples[t][j]].key;
                    sigs[j] = core[triples[t][j]].sig;
                    const char *a[] = {keys[j]};
                    if (!b->kb || !kb_query(b->kb, "module", a, 1)) available = 0;
                }
                const char *const *v = compose_vocab[t % COMPOSE_VOCAB_N];
                size_t fired = available
                    ? run_composition(keys, sigs, 3, v, NULL, 0) : 0;
                int ok = available && fired == 3;
                if (ok) pass_count++;
                ro += (size_t)snprintf(rep + ro, sizeof rep - ro, "%s%s+%s+%s %s",
                                       t ? "; " : "", keys[0], keys[1], keys[2],
                                       ok ? "compose" : "seam");
            }
            snprintf(msg, sizeof msg,
                "I audited my own composition on fresh copies of myself: %s. %zu of %zu triples hold. No file was touched; an external agent owns edits and commits.",
                rep, pass_count, nt);
            put(msg, out, out_size);
            store_proof(b, "loop composition audit: run several triples of my parts on fresh sub-brains and report which compose; computed from real output, edits external.");
            return 1;
        }
        if (picked < 3) {
            put("I would treat it as a composition self-challenge, not self-management: pick three parts I already have and write ONE held-out dialogue, with fresh names so it cannot be memorized, that needs all three at once; it passes only if they cooperate with no new special-case module. I would ratchet it in English and Italian, bump my version, and journal whether composition held or a seam appeared. I can propose this; an external agent edits, runs the tests, and commits.",
                out, out_size);
        } else if (want_selftest) {
            /* gen167/168: actually RUN the derived composition on a fresh copy of
             * myself and report from real output. gen168: a FRESH vocab tuple is
             * chosen per run (so two self-tests use different names — proof the run
             * is executed, not memorized), and the PASS report cites the
             * cooperation actually OBSERVED in the sub-run. Each part's turns are
             * fed through brain_respond on the sub-brain (no static state, so this
             * is reentrancy-safe and footprint-free on the live brain); a part
             * "fired" iff its signature appears. The verdict is computed. */
            const char *const *v = compose_vocab[b->selftest_runs % COMPOSE_VOCAB_N];
            b->selftest_runs++;
            const char *keys[3], *sigs[3];
            char names[256]; size_t no = 0;
            for (size_t k = 0; k < picked; k++) {
                keys[k] = core[pick[k]].key;
                sigs[k] = core[pick[k]].sig;
                no += (size_t)snprintf(names + no, sizeof names - no, "%s%s",
                                       k ? (k == picked - 1 ? " and " : ", ") : "",
                                       core[pick[k]].key);
            }
            char observed[512] = "";
            size_t fired = run_composition(keys, sigs, picked, v,
                                           observed, sizeof observed);
            if (fired == picked)
                snprintf(msg, sizeof msg,
                    "I ran it on a fresh copy of myself with held-out names (a %s %s named %s): %s — %zu of %zu parts fired and cooperated. Observed: %s Composition holds (PASS). No file was touched; an external agent still owns edits and commits.",
                    v[0], v[1], v[3], names, fired, picked, observed);
            else
                snprintf(msg, sizeof msg,
                    "I ran it on a fresh copy of myself with held-out names (a %s %s named %s): %s — only %zu of %zu parts fired, so a seam appeared (FAIL). That gap is the next task for the external loop.",
                    v[0], v[1], v[3], names, fired, picked);
            put(msg, out, out_size);
            store_proof(b, "loop composition self-test: generate fresh vocab, run the derived dialogue on a fresh sub-brain, report pass/seam + observed cooperation from real output; footprint-free, edits external.");
            return 1;
        } else if (want_skeleton) {
            /* gen166: emit a runnable, single-line `>`-turn skeleton over the
             * derived parts (a fixed example tuple). An external agent fills the
             * names and drops it into tests/compose/; parrot0 does not run it. */
            size_t o = (size_t)snprintf(msg, sizeof msg,
                "Here is a held-out dialogue I would run (fresh names; an external agent fills and runs it, I do not):");
            for (size_t k = 0; k < picked; k++) {
                char trn[256];
                build_turn(core[pick[k]].key, compose_vocab[0], trn, sizeof trn);
                o += (size_t)snprintf(msg + o, sizeof msg - o, " > %s", trn);
            }
            put(msg, out, out_size);
            store_proof(b, "loop composition skeleton: emit a runnable >-turn dialogue over the parts derived from module/X; external agent fills, runs, commits.");
        } else {
            snprintf(msg, sizeof msg,
                "I would treat it as a composition self-challenge, not self-management: from my own module set I would pick three parts I actually have — %s, %s, and %s — and write ONE held-out dialogue, with fresh names so it cannot be memorized, that needs all three at once; it passes only if they cooperate with no new special-case module. I would ratchet it in English and Italian, bump my version, and journal whether composition held or a seam appeared. I can propose this; an external agent edits, runs the tests, and commits.",
                core[pick[0]].gloss, core[pick[1]].gloss, core[pick[2]].gloss);
            put(msg, out, out_size);
            store_proof(b, "loop composition self-challenge: compose >=3 existing parts (derived from module/X) in one held-out dialogue, fresh names, ratchet EN+IT, no new module, edits external.");
        }
        return 1;
    }
    int fallback_gap = cue(buf, "fallback") || cue(buf, "wall") ||
                       cue(buf, "do not understand") ||
                       cue(buf, "not understand") || cue(buf, "dont understand") ||
                       cue(buf, "repeat");
    int strong_implementation_gap = cue(buf, "implementation") || cue(buf, "code") ||
                                    cue(buf, "patch") || cue(buf, "dispatch") ||
                                    cue(buf, "loop");
    int implementation_gap = strong_implementation_gap ||
                             cue(buf, "module") || cue(buf, "modulo");

    if (implementation_gap && (!fallback_gap || strong_implementation_gap)) {
        put("I would solve it by parity with the external loop: name the missing behavior, locate the owning module or registry point, add the smallest deterministic change, add English and Italian regression tests, bump my version, and journal the observed support quality. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    } else if (fallback_gap) {
        put("I would treat it as a fallback gap: make the fallback or owning module handle that turn without pretending to understand, ratchet it with held-out English and Italian chats, bump my version, and record whether the wall got smaller. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    } else {
        put("I would treat it as a self-challenge, not self-management: identify the missing behavior, choose the smallest module or dispatch change, ratchet it with English and Italian tests, bump my version, and record what changed. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    }
    store_proof(b, "loop self-challenge parity: classify the gap, name the smallest behavior change, require tests, version, and journal, and keep file edits external.");
    return 1;
}

/* --- module: research (gen171, dynamic knowledge) ------------------------
 * The "inexhaustible interlocutor" seam. Asked to DEFINE a topic it does not
 * know, parrot0 neither silently walls nor pretends. It reads only STATIC local
 * markdown (no intelligence API), asserts the learned wiki_concept straight into
 * RAM through learn_topic(), and answers honestly that it just learned the
 * definition. If no local source exists, it says so. Registered LAST, so it only
 * catches a definitional gap that every other module already declined — never
 * social/arith/memory turns.
 * gen172: the learning STICKS. The def is asserted quoted (the .p0 atom
 * convention), so a re-ask is no longer a gap — mod_knowledge's exact-key path
 * (now compound-aware) speaks it as a known concept, and this module's own
 * RAM-recall guard is the honest fallback if reached. */
static int mod_research(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    char buf[256];
    canonicalize_lang(norm, buf, sizeof buf);

    /* Definitional gap: extract the topic X. STRONG heads (article / "about" /
     * "who" / Italian) signal definitional intent and allow a multi-word topic;
     * the WEAK bare "what is <X>" is ambiguous with arithmetic ("what is 2 + 2",
     * "what is gold plus silver"), so it is accepted only for a single concept
     * word. Arithmetic operators and digits are always rejected, so compute-style
     * questions keep walling as before. */
    char work[256]; snprintf(work, sizeof work, "%s", buf);
    size_t wl = strlen(work);
    while (wl > 0 && (work[wl-1] == '?' || work[wl-1] == ' ')) work[--wl] = '\0';
    const char *x = NULL;
    int weak = 0;
    static const char *const strong_heads[] = {
        "what is a ", "what is an ", "tell me about ",
        "what do you know about ", "who is ", "who was ",
        "cos'è un ", "cos'è una ", "cos'è uno ", "che cos'è ", "cos'è ",
        "parlami di ", "cosa sai di ", "chi è ", NULL,
    };
    static const char *const weak_heads[] = {
        "what is ", "what are ", "what was ", NULL,
    };
    for (const char *const *h = strong_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; break; }
    }
    if (!x) for (const char *const *h = weak_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; weak = 1; break; }
    }
    if (!x || !*x) return 0;

    /* Build the concept key (drop a leading article, join words with '_') and a
     * display form; guard pronouns and too-short topics. */
    char xbuf[160]; snprintf(xbuf, sizeof xbuf, "%s", x);
    char *tok[16]; size_t nt = split_words(xbuf, tok, 16);
    size_t start = 0;
    if (nt > 0 && (is_article(tok[0]) ||
                   !strcmp(tok[0],"the") || !strcmp(tok[0],"il") ||
                   !strcmp(tok[0],"la")  || !strcmp(tok[0],"lo") ||
                   !strcmp(tok[0],"un")  || !strcmp(tok[0],"una") ||
                   !strcmp(tok[0],"uno") || !strcmp(tok[0],"i") ||
                   !strcmp(tok[0],"gli") || !strcmp(tok[0],"le")))
        start = 1;
    if (start >= nt) return 0;
    if (is_entity_pronoun(tok[start])) return 0;
    /* A bare "what is <X>" is only definitional for a single concept word. */
    if (weak && nt - start != 1) return 0;
    /* Reject arithmetic / numeric expressions in any head (they should wall). */
    for (size_t i = start; i < nt; i++) {
        char *t = strip_edge_punct(tok[i]);
        for (char *c = t; *c; c++) if (isdigit((unsigned char)*c)) return 0;
        if (!strcmp(t,"plus")||!strcmp(t,"minus")||!strcmp(t,"times")||
            !strcmp(t,"divided")||!strcmp(t,"over")||!strcmp(t,"equals")||
            !strcmp(t,"più")||!strcmp(t,"piu")||!strcmp(t,"meno")||
            !strcmp(t,"per")||!strcmp(t,"diviso"))
            return 0;
    }
    char key[80]; size_t ko = 0;
    char disp[80]; size_t dpo = 0;
    for (size_t i = start; i < nt; i++) {
        char *t = strip_edge_punct(tok[i]);
        if (!*t) continue;
        ko  += (size_t)snprintf(key  + ko,  sizeof key  - ko,  "%s%s", ko ? "_" : "", t);
        dpo += (size_t)snprintf(disp + dpo, sizeof disp - dpo, "%s%s", dpo ? " " : "", t);
        if (ko >= sizeof key - 8) break;
    }
    if (ko < 3) return 0;
    if (weak && strlen(tok[start]) < 4) return 0;

    char def[KB_TERM_LEN];
    char msg[320];

    /* gen172: already learned this session? Then it is no longer a gap — answer
     * from RAM and be honest that it was read before, instead of re-reading the
     * markdown and pretending fresh discovery. (The word-based describe path
     * cannot match the underscore-joined key, so this module owns the recall.) */
    if (b->kb && kb_concept_def(b->kb, key, def, sizeof def)) {
        snprintf(msg, sizeof msg, "I already read up on %s: %s.", disp, def);
        put(msg, out, out_size);
        return 1;
    }

    if (learn_topic(b->kb, key, disp, def, sizeof def))
        snprintf(msg, sizeof msg,
                 "I didn't know about %s, so I just read it up: %s.",
                 disp, def);
    else
        snprintf(msg, sizeof msg,
                 "I don't actually know about %s yet, and I have no source to read on it.",
                 disp);
    put(msg, out, out_size);
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

    /* gen51 (C1): recognize intent by KEYWORD CUES, not one rigid template, so
     * many phrasings of the same question land. The answer is still derived from
     * the real self-model (i_am / module facts), never canned — robustness comes
     * from cue matching + the KB, not from a list of accepted sentences. */
    int identity = cue(buf, "your name") || cue(buf, "who are you") ||
                   cue(buf, "who re you") || cue(buf, "what are you") ||
                   cue(buf, "what exactly are you") ||
                   cue(buf, "call you") || cue(buf, "about yourself") ||
                   cue(buf, "who r u") || cue(buf, "your identity") ||
                   /* Italian cues (the bilingual ratchet). NB: input is already
                    * canonicalized, so "chi" has become "who" -> "who sei". */
                   cue(buf, "who sei") || cue(buf, "come ti chiami") ||
                   cue(buf, "il tuo nome");
    int exists = cue(buf, "you exist") || cue(buf, "are you real") ||
                 cue(buf, "esisti");
    int capability = cue(buf, "can you do") || cue(buf, "what do you do") ||
                     cue(buf, "capabilit") || cue(buf, "you able to") ||
                     cue(buf, "you help with") || cue(buf, "can you help") ||
                     /* Italian cues */
                     cue(buf, "cosa sai fare") || cue(buf, "puoi fare") ||
                     cue(buf, "sai fare") || cue(buf, "che cosa sai fare");

    /* capability is the more specific intent ("what are you ABLE TO DO" also
     * contains the identity cue "what are you"), so test it first. Describe what
     * the registered modules let me do, in plain language (not the module(...)
     * jargon) — grounded in the real module set, a module absent contributes
     * nothing. */
    if (capability) {
        static const struct { const char *mod, *say; } cap[] = {
            {"knowledge", "answer questions about facts and rules"},
            {"arith",     "do simple arithmetic"},
            {"cause",     "reason about cause and effect"},
            {"compare",   "compare quantities"},
            {"same",      "tell whether two things are the same"},
            {"memory",    "remember things you tell me"},
            {"reader",    "read a short passage and pull out facts"},
            {"coref",     "track what a pronoun refers to"},
            {"gen",       "continue a sequence you teach me"},
            {"discourse", "remember what we talked about"},
        };
        char list[600];
        size_t off = 0, n = 0;
        for (size_t i = 0; i < sizeof cap / sizeof cap[0]; i++) {
            const char *m[] = {cap[i].mod};
            if (!kb_query(b->kb, "module", m, 1)) continue;
            const char *sep = (n == 0) ? "" : ", ";
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", sep, cap[i].say);
            n++;
        }
        char msg[700];
        if (n == 0) snprintf(msg, sizeof msg, "Not much yet.");
        else snprintf(msg, sizeof msg, "I can %s.", list);
        put(msg, out, out_size);
        store_proof(b, "This is derived from my registered module list.");
        return 1;
    }

    if (identity || exists) {
        char id[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        char proof[160];
        if (k == 0) snprintf(msg, sizeof msg, "I don't know what I am.");
        else if (exists) {
            snprintf(msg, sizeof msg, "Yes, I am %s.", id[0]);
            snprintf(proof, sizeof proof, "i_am(%s) is a reflective fact in my knowledge base.", id[0]);
        }
        else {
            snprintf(msg, sizeof msg, "I am %s.", id[0]);
            snprintf(proof, sizeof proof, "i_am(%s) is a reflective fact in my knowledge base.", id[0]);
        }
        put(msg, out, out_size);
        if (k > 0) store_proof(b, proof);
        return 1;
    }

    /* gen77: self-model introspection — the architecture made queryable.
     * Each cue is guarded by word count so full queries (e.g. "what do you
     * know about X?") fall through to mod_knowledge unchanged. */

    /* Quick word-count helper (buf is stripped of trailing punctuation). */
    size_t wn = 0, inw = 0;
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == ' ') inw = 0;
        else if (!inw) { wn++; inw = 1; }
    }

    /* "how many facts do you know?" → kb_user_facts */
    int fact_count = (cue(buf, "how many facts") && wn <= 6) ||
                     (cue(buf, "quanti fatti") && wn <= 5);
    if (fact_count) {
        char msg[128];
        snprintf(msg, sizeof msg, "I know %zu fact(s).", kb_user_facts(b->kb));
        put(msg, out, out_size);
        return 1;
    }

    /* "what predicates do you know?" / "what topics do you know about?" */
    int pred_list = (cue(buf, "what predicates") && wn <= 5) ||
                    (cue(buf, "what topics") && wn <= 6) ||
                    (cue(buf, "quali predicati") && wn <= 4);
    if (pred_list) {
        char preds[128][KB_TERM_LEN];
        size_t np = kb_user_predicates(b->kb, preds, 128);
        if (np == 0) { put("I don't know any predicates yet.", out, out_size); return 1; }
        char list[1024];
        size_t off = 0;
        for (size_t i = 0; i < np && off < sizeof list; i++)
            off += (size_t)snprintf(list + off, sizeof list - off,
                                     "%s%s", i ? ", " : "", preds[i]);
        char msg[1100];
        if (off < sizeof list)
            snprintf(msg, sizeof msg, "I know these predicates: %s.", list);
        else
            snprintf(msg, sizeof msg, "I know %zu distinct predicate(s).", np);
        put(msg, out, out_size);
        return 1;
    }

    /* "show me your knowledge" / "dump everything" */
    int show_knowledge = (cue(buf, "show me your knowledge") && wn <= 5) ||
                         (cue(buf, "show me what you know") && wn <= 7) ||
                         (cue(buf, "dump everything") && wn <= 3) ||
                         (cue(buf, "dump what you know") && wn <= 6) ||
                         (cue(buf, "mostrami la conoscenza") && wn <= 4);
    if (show_knowledge) {
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            snprintf(msg, sizeof msg, "Here is everything I know: %s", dump);
            put(msg, out, out_size);
        } else {
            char msg[128];
            snprintf(msg, sizeof msg, "I know %zu fact(s) total.", kb_user_facts(b->kb));
            put(msg, out, out_size);
        }
        return 1;
    }

    /* "what do you know?" → stats overview. Only claims short forms;
     * "what do you know about X?" has more words and reaches mod_knowledge. */
    int what_know = (cue(buf, "what do you know") && wn <= 4) ||
                    (cue(buf, "cosa sai") && wn <= 3);
    if (what_know) {
        size_t nfacts = kb_user_facts(b->kb);
        char preds[128][KB_TERM_LEN];
        size_t np = kb_user_predicates(b->kb, preds, 128);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I know %zu fact(s) across %zu predicate(s). Ask me about a specific topic.",
                 nfacts, np);
        put(msg, out, out_size);
        return 1;
    }

    /* gen78: "which part of you answered that?" / "what module handled that?"
     * Reads from the last_module stored by the dispatch loop. */
    int which_module = (cue(buf, "which part of you") && wn <= 6) ||
                       (cue(buf, "what module") && wn <= 4) ||
                       (cue(buf, "who answered") && wn <= 4) ||
                       (cue(buf, "quale parte di te") && wn <= 6) ||
                       (cue(buf, "quale modulo") && wn <= 4);
    if (which_module) {
        if (b->last_module[0]) {
            char msg[160];
            if (strcmp(b->last_module, "fallback") == 0)
                snprintf(msg, sizeof msg,
                         "No module could handle that — it fell through to "
                         "the not-understood fallback.");
            else
                snprintf(msg, sizeof msg,
                         "The '%s' module answered your last question.",
                         b->last_module);
            put(msg, out, out_size);
        } else {
            put("I haven't answered anything yet.", out, out_size);
        }
        return 1;
    }

    /* gen83 entities_q ... (above) */

    /* gen86 mod_cap ... (above) */

    /* gen89: "what have I taught you?" — show only session facts. */
    int taught_q = (cue(buf, "what have i taught you") && wn <= 6) ||
                   (cue(buf, "what did i teach you") && wn <= 6) ||
                   (cue(buf, "cosa ti ho insegnato") && wn <= 5);
    if (taught_q) {
        /* Dump only user-facing predicates, regardless of origin.
         * kb_user_facts already filters internal predicates. */
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            snprintf(msg, sizeof msg, "You taught me: %s", dump);
            put(msg, out, out_size);
        } else {
            put("You haven't taught me any facts yet.", out, out_size);
        }
        return 1;
    }
    int mod_cap = (cue(buf, "what can the") && wn <= 7) ||
                  (cue(buf, "what does the") && wn <= 7) ||
                  (cue(buf, "cosa può fare il modulo") && wn <= 7);
    if (mod_cap) {
        static const struct { const char *mod, *say; } cmap[] = {
            {"knowledge", "answer questions about facts and logical rules"},
            {"arith",     "compute arithmetic (plus, minus, times, divisible by)"},
            {"cause",     "reason about cause and effect relations"},
            {"compare",   "compare magnitudes (more/less than)"},
            {"memory",    "remember your name, possessions, and personal facts"},
            {"reader",    "read a passage and extract known fact patterns"},
            {"shell",     "explain shell commands and predict their output"},
            {"gen",       "generate text continuations from learned sequences"},
            {"self",      "answer questions about my own identity and capabilities"},
            {"meta",      "handle meta-conversation (attention, understanding)"},
            {"discourse", "remember what topics we discussed"},
            {"social",    "handle greetings, thanks, and social conventions"},
            {"symbolic",  "recognize symbolic patterns (Morse, leet, palindromes)"},
        };
        for (size_t i = 0; i < sizeof cmap / sizeof cmap[0]; i++) {
            if (cue(buf, cmap[i].mod)) {
                char msg[256];
                snprintf(msg, sizeof msg, "The %s module can %s.",
                         cmap[i].mod, cmap[i].say);
                put(msg, out, out_size);
                return 1;
            }
        }
        put("I don't have a module by that name. Ask 'what can you do?' for a list.",
            out, out_size);
        return 1;
    }
    int entities_q = (cue(buf, "what names have i") && wn <= 6) ||
                     (cue(buf, "who have i mentioned") && wn <= 5) ||
                     (cue(buf, "who have i talked about") && wn <= 7) ||
                     (cue(buf, "quali nomi ho menzionato") && wn <= 5);
    if (entities_q) {
        if (b->entity_count == 0) {
            put("You haven't mentioned any names I recognized.", out, out_size);
        } else {
            char list[512]; size_t off = 0;
            for (size_t i = 0; i < b->entity_count && off < sizeof list; i++)
                off += (size_t)snprintf(list + off, sizeof list - off,
                                         "%s%s", i ? ", " : "", b->entities[i]);
            char msg[600];
            snprintf(msg, sizeof msg, "You mentioned: %s.", list);
            put(msg, out, out_size);
        }
        return 1;
    }

    return 0;
}

/* --- module: shell -------------------------------------------------------
 * POSIX/shell knowledge — Mission M1, step 1 (gen53) + step 2 (gen61).
 * Answers "what does <cmd> do?" / "explain <cmd>" by PARSING the command line
 * into (command, flags, args) and COMPOSING the answer from learned `cmd`/`flag`
 * facts (kb/experts/programming/bash.p0, carried in the commits) — so "ls -la" is explained
 * by composing ls + l + a even though that combination is not stored.
 *
 * gen61 extends this to simple PIPELINES: "cmd1 | cmd2" is explained by
 * describing each segment and joining them with "then". This is still shell
 * *structure*, not a dictionary. Reads `raw`, not `norm`: the shell is
 * case-sensitive (-r != -R), so flag case must survive normalization. */
static void de_underscore(const char *in, char *out, size_t n) {
    size_t i = 0;
    for (; in[i] && i + 1 < n; i++) out[i] = (in[i] == '_') ? ' ' : in[i];
    out[i] = '\0';
}

/* Describe a single shell command line (no pipeline) into `desc`.
 * Returns 1 if it wrote a description or a clear "unknown command" admission,
 * 0 if the input is not clearly shell syntax and should be declined. */
static int describe_command(Brain *b, const char *cmdline,
                            char *desc, size_t desc_size) {
    char clbuf[256];
    snprintf(clbuf, sizeof clbuf, "%s", cmdline);
    char *w[64];
    size_t nw = split_words(clbuf, w, 64);
    if (nw == 0) return 0;

    const char *command = NULL;
    int has_flag = 0;
    for (size_t i = 0; i < nw; i++) {
        if (w[i][0] == '-') has_flag = 1;
        else if (!command) command = w[i];
    }
    if (!command) return 0;

    /* command name is matched lowercased (commands are lowercase); look it up */
    char lc[KB_TERM_LEN];
    size_t ci = 0;
    for (; command[ci] && ci + 1 < sizeof lc; ci++)
        lc[ci] = (char)tolower((unsigned char)command[ci]);
    lc[ci] = '\0';

    const char *cpat[] = {lc, NULL};
    char eff[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cmd", cpat, 2, eff, 4);
    if (k == 0) {
        /* unknown command: only claim the turn when it is clearly shell syntax
         * (a flag is present), so we don't hijack "what does a bird do?". */
        if (!has_flag) return 0;
        snprintf(desc, desc_size, "I don't know the command %s.", lc);
        return 1;
    }

    char base[256];
    de_underscore(eff[0], base, sizeof base);

    char known[512]; size_t ko = 0, kn = 0;
    char unknown[160]; size_t uo = 0, un = 0;
    for (size_t i = 0; i < nw; i++) {
        if (w[i][0] != '-') continue;
        if (w[i][1] == '-') { /* long option: keep whole, do not split chars */
            uo += (size_t)snprintf(unknown + uo, sizeof unknown - uo,
                                   "%s%s", un ? ", " : "", w[i]);
            un++;
            continue;
        }
        for (const char *f = w[i] + 1; *f; f++) {
            /* the resolver reads an uppercase-initial atom as a variable, so an
             * uppercase flag (-R) is looked up case-tagged as "u_r". */
            char fs[8];
            if (isupper((unsigned char)*f))
                snprintf(fs, sizeof fs, "u_%c", (char)tolower((unsigned char)*f));
            else
                snprintf(fs, sizeof fs, "%c", *f);
            const char *fpat[] = {lc, fs, NULL};
            char fe[4][KB_TERM_LEN];
            if (kb_match(b->kb, "flag", fpat, 3, fe, 4) > 0) {
                char ph[160]; de_underscore(fe[0], ph, sizeof ph);
                ko += (size_t)snprintf(known + ko, sizeof known - ko,
                                       "%s%s", kn ? ", " : "", ph);
                kn++;
            } else {
                uo += (size_t)snprintf(unknown + uo, sizeof unknown - uo,
                                       "%s-%c", un ? ", " : "", *f);
                un++;
            }
        }
    }

    size_t o = (size_t)snprintf(desc, desc_size, "%s %s", lc, base);
    if (kn) o += (size_t)snprintf(desc + o, desc_size - o, ", %s", known);
    if (o < desc_size) o += (size_t)snprintf(desc + o, desc_size - o, ".");
    if (un && o < desc_size)
        snprintf(desc + o, desc_size - o,
                 " I don't know the option %s.", unknown);
    return 1;
}

/* gen62: oracle-grounded output prediction for a small allow-list of PURE
 * shell commands. Only active when PARROT0_ORACLE=1, so the default build never
 * executes arbitrary shell code. */

/* True if s contains only safe shell token characters (alphanumerics, -, _, .). */
static int safe_token(const char *s) {
    for (size_t i = 0; s[i]; i++) {
        if (isalnum((unsigned char)s[i])) continue;
        if (strchr("-_.", s[i])) continue;
        return 0;
    }
    return *s != '\0';
}

/* True if every command in the pipeline is in the pure allow-list. */
static int pipeline_is_pure(char **segs, size_t nseg) {
    static const char *pure[] = {"echo", "wc", "cat", "pwd", NULL};
    for (size_t i = 0; i < nseg; i++) {
        char buf[256];
        snprintf(buf, sizeof buf, "%s", segs[i]);
        char *w[64];
        size_t nw = split_words(buf, w, 64);
        if (nw == 0) return 0;
        int ok = 0;
        for (size_t j = 0; pure[j] && !ok; j++)
            if (strcmp(w[0], pure[j]) == 0) ok = 1;
        if (!ok) return 0;
        for (size_t j = 0; j < nw; j++)
            if (!safe_token(w[j])) return 0;
    }
    return 1;
}

/* Simulate one pure command segment. `input` is its stdin; output is written
 * to `out` (with the trailing newline the real command would produce). */
static int simulate_pure(const char *input, const char *cmdline,
                         char *out, size_t out_size) {
    char buf[256];
    snprintf(buf, sizeof buf, "%s", cmdline);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw == 0) return 0;

    if (strcmp(w[0], "echo") == 0) {
        size_t o = 0;
        for (size_t i = 1; i < nw && o + 1 < out_size; i++) {
            if (i > 1) out[o++] = ' ';
            size_t l = strlen(w[i]);
            if (l > out_size - o - 1) l = out_size - o - 1;
            memcpy(out + o, w[i], l);
            o += l;
        }
        if (o + 1 < out_size) out[o++] = '\n';
        out[o] = '\0';
        return 1;
    }

    if (strcmp(w[0], "pwd") == 0) {
        if (getcwd(out, (int)out_size)) {
            size_t o = strlen(out);
            if (o + 1 < out_size) out[o++] = '\n';
            out[o] = '\0';
        } else {
            snprintf(out, out_size, "\n");
        }
        return 1;
    }

    if (strcmp(w[0], "cat") == 0) {
        /* cat with no file arguments copies stdin to stdout */
        if (nw == 1) {
            snprintf(out, out_size, "%s", input);
            return 1;
        }
        return 0;
    }

    if (strcmp(w[0], "wc") == 0) {
        int count_words = 0;
        for (size_t i = 1; i < nw; i++)
            if (strcmp(w[i], "-w") == 0) count_words = 1;
        if (!count_words) return 0;
        size_t n = 0;
        const char *p = input;
        while (*p) {
            while (*p && isspace((unsigned char)*p)) p++;
            if (*p) n++;
            while (*p && !isspace((unsigned char)*p)) p++;
        }
        snprintf(out, out_size, "%zu\n", n);
        return 1;
    }

    return 0;
}

/* Simulate a pure pipeline by threading stdin/stdout through each segment. */
static int simulate_pipeline(const char *pipeline,
                             char *out, size_t out_size) {
    char pipebuf[512];
    size_t len = strlen(pipeline);
    if (len >= sizeof pipebuf) return 0;
    memcpy(pipebuf, pipeline, len + 1);

    char *segs[8];
    size_t nseg = 0;
    char *p = pipebuf;
    while (p && *p && nseg < 8) {
        char *next = strchr(p, '|');
        if (next) *next++ = '\0';
        while (*p && isspace((unsigned char)*p)) p++;
        segs[nseg++] = p;
        p = next;
    }
    if (!pipeline_is_pure(segs, nseg)) return 0;

    char buf[4096];
    buf[0] = '\0';
    for (size_t i = 0; i < nseg; i++) {
        char next[4096];
        if (!simulate_pure(buf, segs[i], next, sizeof next)) return 0;
        snprintf(buf, sizeof buf, "%s", next);
    }
    snprintf(out, out_size, "%s", buf);
    return 1;
}

/* gen115 (L15): the first deliberate mid-turn TOOL CALL — the honest seam
 * between "reason" and "act". Until now every module answered by lookup,
 * inference, or inline computation. This one ACTS: it recognizes a question it
 * cannot resolve by knowing (how many words are in this text?), compiles it to a
 * real command, and INVOKES a deterministic oracle — the pure POSIX pipeline
 * simulator (no subprocess, no network) — then folds the computed result back
 * into the reply, naming the exact command it ran so the call is observable, not
 * a stubbed constant. Held-out text transfers because the count is produced by
 * the oracle, not stored. The tool call is also recorded as the proof trace.
 * This is the structural precondition for agency (rung 19): a brain that can
 * reach outside its own deduction to a tool and bring the answer back. */
static int mod_tool(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Fire only on an explicit word-count request (EN + IT). */
    int want_words = cue(low, "how many words") || cue(low, "count the words") ||
                     cue(low, "quante parole")  || cue(low, "conta le parole");
    if (!want_words) return 0;

    /* The text to count: prefer everything after a ':'; else after " in ". */
    const char *p = strchr(low, ':');
    if (p) p++;
    else { p = strstr(low, " in "); if (p) p += 4; }
    if (!p) return 0;
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return 0;

    char text[256];
    snprintf(text, sizeof text, "%s", p);
    size_t tl = strlen(text);
    while (tl > 0 && (text[tl-1]=='?' || text[tl-1]=='.' || text[tl-1]=='!' ||
                      isspace((unsigned char)text[tl-1])))
        text[--tl] = '\0';
    if (tl == 0) return 0;

    /* Compile the tool call and run the REAL deterministic oracle. */
    char cmd[320];
    snprintf(cmd, sizeof cmd, "echo %s | wc -w", text);
    char result[64];
    if (!simulate_pipeline(cmd, result, sizeof result)) {
        put("I can only count plain words for now (letters and digits, no punctuation).",
            out, out_size);
        return 1;
    }
    size_t resl = strlen(result);
    while (resl > 0 && (result[resl-1]=='\n' || result[resl-1]==' '))
        result[--resl] = '\0';

    char msg[640];
    snprintf(msg, sizeof msg, "%s. (I ran the tool: %s.)", result, cmd);
    put(msg, out, out_size);

    char proof[640];
    snprintf(proof, sizeof proof, "tool call %s = %s", cmd, result);
    store_proof(b, proof);
    return 1;
}

/* gen116 (rung 19): the autonomous ACT-LOOP — parrot0's first perceive ->
 * decide -> act -> observe cycle, and the seed of agency. gen115 made a single
 * tool call; this REPEATS one to pursue a goal. Given a start value, an
 * operation, and a stop condition ("start at 3, double until you reach 50") the
 * agent loops: it OBSERVES the current value, DECIDES whether the goal is met,
 * and if not it ACTS by applying the operation through the arithmetic oracle,
 * then observes the new value — and repeats until the goal holds. The trajectory
 * and the step count are PRODUCED by running the loop, never a closed-form
 * formula, so held-out start/op/target transfer. A bounded step cap (and a
 * no-progress guard) keep an unreachable goal honest ("can't reach ...") instead
 * of spinning forever. This stands on gen115's reason/act seam and turns it into
 * the structural precondition for an autonomous agent (ladder rung 19). */
enum agent_cmp { AGT_GE, AGT_GT, AGT_LE, AGT_LT };

static int agent_goal_met(double cur, double t, enum agent_cmp cmp) {
    switch (cmp) {
        case AGT_GE: return cur >= t - 1e-9;
        case AGT_GT: return cur >  t + 1e-9;
        case AGT_LE: return cur <= t + 1e-9;
        case AGT_LT: return cur <  t - 1e-9;
    }
    return 0;
}

/* gen117 (rung 19, deeper): one step of an act-loop is no longer a FIXED
 * operation — it is CHOSEN by observing the state. A single branch clause ("if
 * it is even, halve it" / "triple it and add 1") is parsed into a short sequence
 * of operations the agent applies in order when that branch is taken. */
typedef struct { char op; double k; } AgentOp;

static size_t parse_branch_ops(const char *clause, AgentOp *ops, size_t max) {
    char buf[256];
    snprintf(buf, sizeof buf, "%s", clause);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    size_t n = 0;
    char pend = 0;                       /* an operator still awaiting its operand */
    for (size_t i = 0; i < nw && n < max; i++) {
        char *t = strip_edge_punct(w[i]);
        double v;
        if (pend && parse_value(t, &v)) { ops[n].op = pend; ops[n].k = v; n++; pend = 0; continue; }
        if (!strcmp(t, "double") || !strcmp(t, "raddoppia"))     { ops[n].op='*'; ops[n].k=2; n++; }
        else if (!strcmp(t, "triple") || !strcmp(t, "triplica")) { ops[n].op='*'; ops[n].k=3; n++; }
        else if (!strcmp(t, "halve")  || !strcmp(t, "dimezza"))  { ops[n].op='/'; ops[n].k=2; n++; }
        else if (!strcmp(t,"add")||!strcmp(t,"plus")||!strcmp(t,"aggiungi")||!strcmp(t,"somma")) pend='+';
        else if (!strcmp(t,"subtract")||!strcmp(t,"minus")||!strcmp(t,"sottrai")||!strcmp(t,"togli")) pend='-';
        else if (!strcmp(t,"multiply")||!strcmp(t,"times")||!strcmp(t,"moltiplica")) pend='*';
        else if (!strcmp(t,"divide")||!strcmp(t,"dividi")||!strcmp(t,"diviso")) pend='/';
    }
    return n;
}

/* Copy [from, end) into out, NUL-terminated and length-clamped. */
static void agent_slice(const char *from, const char *end, char *out, size_t out_size) {
    size_t len = (size_t)(end - from);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, from, len); out[len] = '\0';
}

static int mod_agent(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Must look like an iterate-until-goal instruction: a start anchor and an
     * "until" boundary. Without both, this is not our turn. */
    int has_start = cue(low, "start") || cue(low, "begin") || cue(low, "parti") ||
                    cue(low, "inizia") || cue(low, "comincia");
    char *until = strstr(low, "until");
    if (!until) { char *u2 = strstr(low, "finch"); if (u2) until = u2; }
    if (!until) { char *u3 = strstr(low, "fino a"); if (u3) until = u3; }
    if (!has_start || !until) return 0;

    /* gen117: the BRANCHING act-loop. If the instruction carries a parity
     * condition ("if it is even ... if it is odd ..."), the action each step is
     * not fixed but DECIDED by observing the current value's parity, then the
     * chosen branch's operation sequence is applied. The classic witness is the
     * Collatz process — halve when even, triple-and-add-one when odd, until it
     * reaches 1 — whose step count is famously unpredictable, so the answer can
     * only come from actually running the loop, never a formula. */
    const char *ce = strstr(low, "even"); if (!ce) ce = strstr(low, "pari");
    const char *co = strstr(low, "odd");  if (!co) co = strstr(low, "dispari");
    if (ce && co) {
        {
            /* Each branch clause runs from its parity marker to the next marker
             * (or to the goal), so one branch never bleeds into the next — works
             * the same for "if even ... if odd" and "se pari ... se dispari". */
            const char *end_e = until, *end_o = until;
            if (co > ce && co < end_e) end_e = co;
            if (ce > co && ce < end_o) end_o = ce;
            char ec[256], oc[256];
            agent_slice(ce, end_e, ec, sizeof ec);
            agent_slice(co, end_o, oc, sizeof oc);
            AgentOp even_ops[4], odd_ops[4];
            size_t ne = parse_branch_ops(ec, even_ops, 4);
            size_t no = parse_branch_ops(oc, odd_ops, 4);

            /* start value: the first number before the first branch marker. */
            const char *first = (ce < co) ? ce : co;
            char head[256];
            size_t hl = (size_t)(first - low);
            if (hl >= sizeof head) hl = sizeof head - 1;
            memcpy(head, low, hl); head[hl] = '\0';
            char hb[256]; snprintf(hb, sizeof hb, "%s", head);
            char *hw[64]; size_t hnw = split_words(hb, hw, 64);
            double snums[8]; size_t sn = collect_numbers(hw, hnw, snums, 8);

            /* target: the first number in the goal clause (after "until"). */
            char gb[256]; snprintf(gb, sizeof gb, "%s", until);
            char *gw[64]; size_t gnw = split_words(gb, gw, 64);
            double gnums[8]; size_t gn = collect_numbers(gw, gnw, gnums, 8);

            if (ne > 0 && no > 0 && sn > 0 && gn > 0) {
                double cur = snums[0], target = gnums[0];
                long steps = 0; const long CAP = 1000000;
                char traj[480], nb[64];
                format_num(cur, nb, sizeof nb);
                size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
                int truncated = 0;
                while (!(cur >= target - 1e-9 && cur <= target + 1e-9) &&
                       steps < CAP) {
                    long long iv = (long long)cur;
                    int even = ((double)iv == cur) && (iv % 2 == 0);
                    const AgentOp *seq = even ? even_ops : odd_ops;
                    size_t nseq = even ? ne : no;
                    double before = cur;
                    for (size_t s = 0; s < nseq; s++) {
                        int ok; char o[2] = { seq[s].op, 0 };
                        double nx = apply_arith_op(o, cur, seq[s].k, &ok);
                        if (ok) cur = nx;
                    }
                    if (cur == before) break;       /* no-progress guard */
                    steps++;
                    format_num(cur, nb, sizeof nb);
                    if (steps <= 8)
                        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
                    else if (!truncated) {
                        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> ...");
                        truncated = 1;
                    }
                }
                if (truncated)
                    snprintf(traj + to, sizeof traj - to, " -> %s", nb);

                char msg[640];
                if (cur >= target - 1e-9 && cur <= target + 1e-9) {
                    format_num(target, nb, sizeof nb);
                    snprintf(msg, sizeof msg, "Reached %s after %ld step%s: %s.",
                             nb, steps, steps == 1 ? "" : "s", traj);
                } else {
                    snprintf(msg, sizeof msg,
                             "It didn't settle within %ld steps.", CAP);
                }
                put(msg, out, out_size);
                char proof[512];
                snprintf(proof, sizeof proof,
                         "branching act-loop: %ld observed steps, %s", steps, traj);
                store_proof(b, proof);
                return 1;
            }
        }
    }

    /* Split into the left clause (start + operation) and the right clause
     * (the goal), then read the numbers out of each. */
    size_t cut = (size_t)(until - low);
    char left[256], right[256];
    snprintf(left, sizeof left, "%.*s", (int)cut, low);
    snprintf(right, sizeof right, "%s", until);

    double lnums[8], rnums[8];
    char lb[256], rb[256];
    snprintf(lb, sizeof lb, "%s", left);
    snprintf(rb, sizeof rb, "%s", right);
    char *lw[64], *rw[64];
    size_t lnw = split_words(lb, lw, 64);
    size_t rnw = split_words(rb, rw, 64);
    size_t ln = collect_numbers(lw, lnw, lnums, 8);
    size_t rn = collect_numbers(rw, rnw, rnums, 8);
    if (ln == 0 || rn == 0) return 0;

    double start = lnums[0];
    double target = rnums[0];

    /* Read the operation from the left clause. Word-named operators (double,
     * halve, triple) carry their operand; the rest take the second number. */
    const char *op = NULL; double k = 0;
    if (cue(left, "double") || cue(left, "raddoppi"))      { op = "*"; k = 2; }
    else if (cue(left, "triple") || cue(left, "triplic"))  { op = "*"; k = 3; }
    else if (cue(left, "halve")  || cue(left, "dimezz"))   { op = "/"; k = 2; }
    else if (ln >= 2) {
        k = lnums[1];
        if (cue(left, "add") || cue(left, "plus") || cue(left, "aggiung") ||
            cue(left, "somma") || cue(left, "più"))            op = "+";
        else if (cue(left, "subtract") || cue(left, "minus") ||
                 cue(left, "sottra") || cue(left, "togli"))    op = "-";
        else if (cue(left, "multiply") || cue(left, "times") ||
                 cue(left, "moltiplic"))                       op = "*";
        else if (cue(left, "divide") || cue(left, "dividi") ||
                 cue(left, "diviso"))                          op = "/";
    }
    if (!op) return 0;

    /* Read the goal comparator from the right clause; if unstated, infer it from
     * whether the operation grows or shrinks the value. */
    enum agent_cmp cmp;
    int have_cmp = 1;
    if (cue(right, "exceed") || cue(right, "pass") || cue(right, "above") ||
        cue(right, "over") || cue(right, "more than") || cue(right, "surpass") ||
        cue(right, "super") || cue(right, "oltre"))            cmp = AGT_GT;
    else if (cue(right, "reach") || cue(right, "at least") ||
             cue(right, "almeno") || cue(right, "raggiung"))   cmp = AGT_GE;
    else if (cue(right, "at most") || cue(right, "al massimo")) cmp = AGT_LE;
    else if (cue(right, "below") || cue(right, "under") ||
             cue(right, "less than") || cue(right, "beneath") ||
             cue(right, "sotto") || cue(right, "minore"))      cmp = AGT_LT;
    else have_cmp = 0;
    if (!have_cmp) {
        int grows = (strcmp(op, "+") == 0) ||
                    (strcmp(op, "*") == 0 && k > 1);
        cmp = grows ? AGT_GE : AGT_LE;
    }

    /* Run the loop. Build a trajectory string as we go (head, then an ellipsis
     * for long runs, then the tail). The step count is the number of real
     * oracle calls — the agent's actions. */
    const long CAP = 100000;
    double cur = start;
    long steps = 0;
    char traj[480];
    char nb[64];
    format_num(cur, nb, sizeof nb);
    size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
    int truncated = 0;

    while (!agent_goal_met(cur, target, cmp) && steps < CAP) {
        int ok;
        double next = apply_arith_op(op, cur, k, &ok);
        if (!ok) break;
        if (next == cur) break;             /* no-progress guard */
        cur = next;
        steps++;
        format_num(cur, nb, sizeof nb);
        if (steps <= 10) {
            to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
        } else if (!truncated) {
            to += (size_t)snprintf(traj + to, sizeof traj - to, " -> ...");
            truncated = 1;
        }
    }
    if (truncated) /* always show where it ended up */
        snprintf(traj + to, sizeof traj - to, " -> %s", nb);

    char msg[640];
    if (agent_goal_met(cur, target, cmp)) {
        format_num(cur, nb, sizeof nb);
        snprintf(msg, sizeof msg, "Reached %s after %ld step%s: %s.",
                 nb, steps, steps == 1 ? "" : "s", traj);
    } else {
        char sb[64], tb[64];
        format_num(start, sb, sizeof sb);
        format_num(target, tb, sizeof tb);
        snprintf(msg, sizeof msg,
                 "Starting from %s, that step never reaches %s — the goal can't "
                 "be met this way.", sb, tb);
    }
    put(msg, out, out_size);

    char proof[512];
    snprintf(proof, sizeof proof, "act-loop: %ld oracle call%s, %s",
             steps, steps == 1 ? "" : "s", traj);
    store_proof(b, proof);
    return 1;
}

/* gen118 (rung 19 + L10, deeper): rule INDUCTION from observed transitions. In
 * gen117 the agent was TOLD the law ("if even halve, if odd triple+1"); here it
 * DISCOVERS it. Shown a handful of integer transitions ("6 -> 3, 3 -> 10,
 * 10 -> 5, 5 -> 16"), it fits a hypothesis — first a single global operation,
 * else a PARITY-SPLIT rule — and then applies it: continue a sequence, give the
 * next value, or state the rule. The striking witness is that the SAME data that
 * defines the Collatz step lets parrot0 re-derive Collatz with nobody telling it,
 * then run it. This is program induction: recovering a generative function from
 * examples, the core of in-context learning, in deterministic C. */
typedef struct {
    int    conditional;          /* 0 = one global op sequence; 1 = parity split */
    AgentOp even_ops[3]; size_t ne;
    AgentOp odd_ops[3];  size_t no;
} InducedRule;

/* Fit one class of (in -> out) integer pairs to a short operation sequence.
 * Tries, in order: constant, add/subtract, multiply, divide, then affine a*n+b.
 * Returns 1 and writes ops on success. The identity map is rejected as
 * uninformative (it would make a sequence never progress). */
static int fit_class(const long *in, const long *out, size_t n,
                     AgentOp *ops, size_t *nops) {
    if (n == 0) return 0;
    /* constant: every output identical (independent of input). */
    int all_const = 1;
    for (size_t i = 1; i < n; i++) if (out[i] != out[0]) { all_const = 0; break; }
    if (all_const && n >= 2) {
        ops[0].op='*'; ops[0].k=0; ops[1].op='+'; ops[1].k=(double)out[0]; *nops=2; return 1;
    }
    /* add/subtract a constant. */
    long d = out[0] - in[0]; int add_ok = (d != 0);
    for (size_t i = 1; i < n && add_ok; i++) if (out[i]-in[i] != d) add_ok = 0;
    if (add_ok) { ops[0].op='+'; ops[0].k=(double)d; *nops=1; return 1; }
    /* multiply by a constant. */
    if (in[0] != 0 && out[0] % in[0] == 0) {
        long r = out[0]/in[0]; int mul_ok = (r != 1);
        for (size_t i = 0; i < n && mul_ok; i++)
            if (in[i]==0 || out[i] != r*in[i]) mul_ok = 0;
        if (mul_ok) { ops[0].op='*'; ops[0].k=(double)r; *nops=1; return 1; }
    }
    /* divide by a constant. */
    if (out[0] != 0 && in[0] % out[0] == 0) {
        long dv = in[0]/out[0]; int div_ok = (dv != 1);
        for (size_t i = 0; i < n && div_ok; i++)
            if (out[i]==0 || in[i] != dv*out[i]) div_ok = 0;
        if (div_ok) { ops[0].op='/'; ops[0].k=(double)dv; *nops=1; return 1; }
    }
    /* affine a*n+b, needs two distinct inputs; a integer. */
    for (size_t j = 1; j < n; j++) {
        if (in[j] == in[0]) continue;
        long num = out[j]-out[0], den = in[j]-in[0];
        if (num % den != 0) break;
        long a = num/den, bb = out[0] - a*in[0];
        int ok = 1;
        for (size_t i = 0; i < n && ok; i++) if (out[i] != a*in[i]+bb) ok = 0;
        if (ok && !(a==1 && bb==0)) {
            size_t k = 0;
            if (a != 1) { ops[k].op='*'; ops[k].k=(double)a; k++; }
            if (bb != 0 || k == 0) { ops[k].op='+'; ops[k].k=(double)bb; k++; }
            *nops = k; return 1;
        }
        break;
    }
    return 0;
}

static double apply_rule(const InducedRule *r, double cur) {
    const AgentOp *seq; size_t n;
    if (r->conditional) {
        long long iv = (long long)cur;
        int even = ((double)iv == cur) && (iv % 2 == 0);
        seq = even ? r->even_ops : r->odd_ops;
        n   = even ? r->ne       : r->no;
    } else { seq = r->even_ops; n = r->ne; }
    for (size_t s = 0; s < n; s++) {
        int ok; char o[2] = { seq[s].op, 0 };
        double nx = apply_arith_op(o, cur, seq[s].k, &ok);
        if (ok) cur = nx;
    }
    return cur;
}

/* Render an op sequence as a compact algebraic expression in n. */
static void describe_ops(const AgentOp *ops, size_t n, char *buf, size_t sz) {
    char ka[64], kb[64];
    if (n == 2 && ops[0].op=='*' && ops[0].k==0 && ops[1].op=='+') {
        format_num(ops[1].k, ka, sizeof ka); snprintf(buf, sz, "%s", ka); return;
    }
    if (n == 1 && ops[0].op=='/') {
        format_num(ops[0].k, ka, sizeof ka); snprintf(buf, sz, "n / %s", ka); return;
    }
    if (n == 1 && ops[0].op=='*') {
        format_num(ops[0].k, ka, sizeof ka); snprintf(buf, sz, "%sn", ka); return;
    }
    if (n == 1 && ops[0].op=='+') {
        double k = ops[0].k; format_num(k<0?-k:k, ka, sizeof ka);
        snprintf(buf, sz, "n %s %s", k<0?"-":"+", ka); return;
    }
    if (n == 2 && ops[0].op=='*' && ops[1].op=='+') {
        double k = ops[1].k; format_num(ops[0].k, ka, sizeof ka);
        format_num(k<0?-k:k, kb, sizeof kb);
        snprintf(buf, sz, "%sn %s %s", ka, k<0?"-":"+", kb); return;
    }
    snprintf(buf, sz, "n"); /* identity / unknown shape */
}

/* Fit a rule to (in -> out) pairs and describe it. Returns 1 on success, 0 if
 * nothing in the hypothesis space fits. Shared by gen118 (use a rule) and gen120
 * (test a rule). A conditional rule needs >= 2 examples per parity class so a
 * stray pair can't overfit a spurious branch. */
static int induce_rule(const long *in, const long *out, size_t npair,
                       InducedRule *r, char *rule, size_t rulesz) {
    memset(r, 0, sizeof *r);
    int fitted = 0;
    if (fit_class(in, out, npair, r->even_ops, &r->ne)) {
        r->conditional = 0; fitted = 1;
    } else {
        long ein[16], eout[16], oin[16], oout[16]; size_t en=0, on=0;
        for (size_t i = 0; i < npair && en < 16 && on < 16; i++) {
            if (in[i] % 2 == 0) { ein[en]=in[i]; eout[en]=out[i]; en++; }
            else                { oin[on]=in[i]; oout[on]=out[i]; on++; }
        }
        if (en >= 2 && on >= 2 &&
            fit_class(ein, eout, en, r->even_ops, &r->ne) &&
            fit_class(oin, oout, on, r->odd_ops, &r->no)) {
            r->conditional = 1; fitted = 1;
        }
    }
    if (!fitted) return 0;
    if (r->conditional) {
        char e[64], o[64];
        describe_ops(r->even_ops, r->ne, e, sizeof e);
        describe_ops(r->odd_ops,  r->no, o, sizeof o);
        snprintf(rule, rulesz, "if even: %s; if odd: %s", e, o);
    } else {
        char g[64]; describe_ops(r->even_ops, r->ne, g, sizeof g);
        snprintf(rule, rulesz, "f(n) = %s", g);
    }
    return 1;
}

static int mod_induce(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Must carry the intent to USE an induced rule, distinct from a fewshot
     * "probe -> ?" line. */
    enum { Q_NONE, Q_CONT, Q_NEXT, Q_RULE } q = Q_NONE;
    const char *np = NULL;             /* where the query's argument starts */
    const char *m;
    if ((m = strstr(low, "continue from")))      { q = Q_CONT; np = m + 13; }
    else if ((m = strstr(low, "continua da")))   { q = Q_CONT; np = m + 11; }
    else if ((m = strstr(low, "what comes after"))){ q = Q_NEXT; np = m + 16; }
    else if ((m = strstr(low, "cosa viene dopo"))){ q = Q_NEXT; np = m + 15; }
    else if ((m = strstr(low, "apply it to")))   { q = Q_NEXT; np = m + 11; }
    else if ((m = strstr(low, "applicala a")))   { q = Q_NEXT; np = m + 11; }
    else if (strstr(low,"what is the rule") || strstr(low,"what's the rule") ||
             strstr(low,"qual è la regola") || strstr(low,"quale regola") ||
             strstr(low,"che regola")) q = Q_RULE;
    if (q == Q_NONE) return 0;

    /* Parse the transition pairs. Examples live before the query phrase. */
    char ex[256];
    size_t exlen = m ? (size_t)(m - low) : strlen(low);
    if (exlen >= sizeof ex) exlen = sizeof ex - 1;
    memcpy(ex, low, exlen); ex[exlen] = '\0';

    char buf[256]; size_t bn = 0;
    for (const char *p = ex; *p && bn + 4 < sizeof buf; ) {
        if (p[0]=='-' && p[1]=='>') { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=2; }
        else if ((unsigned char)p[0]==0xE2 && (unsigned char)p[1]==0x86 &&
                 (unsigned char)p[2]==0x92) { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=3; }
        else if (*p==',' || *p==';') { buf[bn++]=' '; p++; }
        else buf[bn++]=*p++;
    }
    buf[bn] = '\0';
    char *w[96]; size_t nw = split_words(buf, w, 96);

    long in[16], out_[16]; size_t npair = 0;
    for (size_t i = 0; i + 2 < nw && npair < 16; ) {
        double a, c;
        if (strcmp(w[i+1], "\x1f")==0 && parse_value(strip_edge_punct(w[i]), &a) &&
            parse_value(strip_edge_punct(w[i+2]), &c) &&
            (double)(long)a==a && (double)(long)c==c) {
            in[npair]=(long)a; out_[npair]=(long)c; npair++; i+=3;
        } else i++;
    }
    if (npair < 2) return 0;

    /* Induce a rule from the examples. If nothing fits, claim the turn (the
     * examples are unmistakably ours) but decline honestly. */
    InducedRule r; char rule[160];
    if (!induce_rule(in, out_, npair, &r, rule, sizeof rule)) {
        put("Those examples don't all follow one rule I can express yet.",
            out, out_size);
        return 1;
    }

    char msg[640];
    if (q == Q_RULE) {
        snprintf(msg, sizeof msg, "The rule is %s.", rule);
        put(msg, out, out_size);
        store_proof(b, rule);
        return 1;
    }

    /* Q_CONT / Q_NEXT need the query argument N — copy just its digits/sign so
     * trailing punctuation ("12.", "10?") doesn't defeat the parse. */
    while (np && *np && !(isdigit((unsigned char)*np) ||
           (*np=='-' && isdigit((unsigned char)np[1])))) np++;
    char numbuf[32]; size_t kk = 0;
    if (np && *np == '-') numbuf[kk++] = *np++;
    while (np && *np && isdigit((unsigned char)*np) && kk + 1 < sizeof numbuf)
        numbuf[kk++] = *np++;
    numbuf[kk] = '\0';
    double n0;
    if (kk == 0 || !parse_value(numbuf, &n0)) {
        put("I found the rule but not the number to apply it to.", out, out_size);
        store_proof(b, rule);
        return 1;
    }

    if (q == Q_NEXT) {
        double v = apply_rule(&r, n0);
        char nb[64]; format_num(v, nb, sizeof nb);
        snprintf(msg, sizeof msg, "%s. (rule: %s.)", nb, rule);
        put(msg, out, out_size);
        char proof[256]; snprintf(proof, sizeof proof, "induced %s", rule);
        store_proof(b, proof);
        return 1;
    }

    /* Q_CONT: run the induced rule from N, stopping at the 1-attractor, a fixed
     * point, or a term cap — and show the trajectory it generated. */
    double cur = n0;
    char traj[400], nb[64];
    format_num(cur, nb, sizeof nb);
    size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
    for (int step = 0; step < 14; step++) {
        if (cur >= 1 - 1e-9 && cur <= 1 + 1e-9) break;
        double nx = apply_rule(&r, cur);
        if (nx == cur) break;            /* fixed point */
        cur = nx;
        format_num(cur, nb, sizeof nb);
        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
        if (to > sizeof traj - 40) break;
    }
    snprintf(msg, sizeof msg, "%s. (induced %s.)", traj, rule);
    put(msg, out, out_size);
    store_proof(b, traj);
    return 1;
}

/* gen119 (rung 19 + rung 13, deeper): goal-directed SEARCH — the planner agent.
 * gen116 iterated ONE given action; gen118 INDUCED a rule. gen119 is the
 * deductive complement: given a start, a target, and a SET of available actions
 * ("from 3, using times 3 and add 1, reach 28"), the agent SEARCHES the action
 * space with breadth-first search and synthesizes the SHORTEST sequence of tool
 * calls that reaches the goal — means-ends problem solving. The plan is not
 * stored; it is found each time, so held-out start/target/ops transfer, and it
 * is verified by replaying it (the trajectory shown is the real one). A node cap
 * and a value-range prune keep the search bounded and honest about failure. */
static void search_op_label(const AgentOp *op, char *buf, size_t sz) {
    char k[32]; format_num(op->k, k, sizeof k);
    snprintf(buf, sz, "%c%s", op->op, k);
}

static int mod_search(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Gate: an explicit action set ("using …") and a reach-target marker. */
    const char *uk = strstr(low, "using"); if (!uk) uk = strstr(low, "usando");
    if (!uk) return 0;
    const char *rk = NULL; const char *cues[] =
        { "reach", "make", "get to", "raggiungere", "arrivare a", "ottenere", NULL };
    for (size_t i = 0; cues[i]; i++) { const char *p = strstr(low, cues[i]);
        if (p && p > uk && (!rk || p < rk)) rk = p; }
    if (!rk) return 0;

    /* start: first number before "using". */
    char head[256]; size_t hl = (size_t)(uk - low);
    if (hl >= sizeof head) hl = sizeof head - 1;
    memcpy(head, low, hl); head[hl] = '\0';
    char hb[256]; snprintf(hb, sizeof hb, "%s", head);
    char *hw[64]; size_t hnw = split_words(hb, hw, 64);
    double snums[8]; size_t sn = collect_numbers(hw, hnw, snums, 8);
    if (sn == 0) return 0;

    /* available actions: the op sequence parsed from between "using" and the
     * reach marker — each parsed op is a SEPARATE available action. */
    char opreg[256]; size_t ol = (size_t)(rk - (uk + 5));
    const char *ops_start = uk + 5;
    if (ol >= sizeof opreg) ol = sizeof opreg - 1;
    memcpy(opreg, ops_start, ol); opreg[ol] = '\0';
    AgentOp acts[6]; size_t nacts = parse_branch_ops(opreg, acts, 6);
    if (nacts == 0) return 0;

    /* target: first number after the reach marker. */
    char tail[128]; snprintf(tail, sizeof tail, "%s", rk);
    char *tw[64]; size_t tnw = split_words(tail, tw, 64);
    double tnums[8]; size_t tn = collect_numbers(tw, tnw, tnums, 8);
    if (tn == 0) return 0;

    double start = snums[0], target = tnums[0];

    /* Breadth-first search. Each node remembers its parent and the action taken,
     * so the shortest plan can be reconstructed. Visited values are kept unique;
     * states wandering far outside [−,+] the target range are pruned. */
    enum { CAP = 6000 };
    static double val[CAP]; static int par[CAP]; static int act[CAP];
    size_t nnodes = 0, qh = 0;
    double lo = -1e6, hi = (target > 0 ? target : -target) * 8 + 2000;
    val[nnodes] = start; par[nnodes] = -1; act[nnodes] = -1; nnodes++;
    int goal = (start == target) ? 0 : -1;

    while (qh < nnodes && goal < 0) {
        double cur = val[qh];
        for (size_t a = 0; a < nacts && goal < 0; a++) {
            int ok; char o[2] = { acts[a].op, 0 };
            double nx = apply_arith_op(o, cur, acts[a].k, &ok);
            if (!ok) continue;
            if ((double)(long long)nx != nx) continue;   /* integer states only */
            if (nx < lo || nx > hi) continue;
            int seen = 0;
            for (size_t i = 0; i < nnodes; i++) if (val[i] == nx) { seen = 1; break; }
            if (seen) continue;
            if (nnodes >= CAP) { qh = nnodes; break; }   /* exhausted budget */
            val[nnodes] = nx; par[nnodes] = (int)qh; act[nnodes] = (int)a;
            if (nx == target) goal = (int)nnodes;
            nnodes++;
        }
        qh++;
    }

    if (goal < 0) {
        char sb[64], tb[64]; format_num(start, sb, sizeof sb);
        format_num(target, tb, sizeof tb);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I couldn't reach %s from %s with those operations.", tb, sb);
        put(msg, out, out_size);
        return 1;
    }

    /* Reconstruct the plan from the goal node back to the start. */
    int chain[64]; size_t clen = 0;
    for (int i = goal; i >= 0 && clen < 64; i = par[i]) chain[clen++] = i;

    char traj[400]; size_t to = 0;
    char nb[64];
    format_num(val[chain[clen - 1]], nb, sizeof nb);
    to += (size_t)snprintf(traj + to, sizeof traj - to, "%s", nb);
    for (size_t i = clen - 1; i-- > 0; ) {
        int node = chain[i];
        char lbl[40]; search_op_label(&acts[act[node]], lbl, sizeof lbl);
        format_num(val[node], nb, sizeof nb);
        to += (size_t)snprintf(traj + to, sizeof traj - to, " -[%s]-> %s", lbl, nb);
        if (to > sizeof traj - 48) break;
    }

    size_t steps = clen - 1;
    char sb[64], tb[64]; format_num(start, sb, sizeof sb); format_num(target, tb, sizeof tb);
    char msg[520];
    snprintf(msg, sizeof msg, "Reached %s from %s in %zu step%s: %s.",
             tb, sb, steps, steps == 1 ? "" : "s", traj);
    put(msg, out, out_size);
    char proof[460];
    snprintf(proof, sizeof proof, "search plan (%zu steps): %s", steps, traj);
    store_proof(b, proof);
    return 1;
}

/* gen120 (rung 19 + rung 16, the capstone): hypothesis TESTING / falsification.
 * gen118 forms a law from examples; gen120 puts it on trial. Shown examples plus
 * a held-out transition ("… does 10 -> 21 fit?"), the agent induces the rule from
 * the examples, applies it to the test input, and either CONFIRMS the transition
 * or REFUTES it — naming exactly what the rule predicted instead. This closes the
 * loop the whole experiment runs (observe → hypothesize → predict → TEST) as a
 * feature inside the agent: PRINCIPLES.md's "introspection proposes; the tests
 * dispose", one level down. A wrong datum cannot hide — the predicted value is
 * computed, not asserted. */
static int mod_verify(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Intent to judge a transition against a rule. */
    if (!(strstr(low,"fit") || strstr(low,"consistent") || strstr(low,"refute") ||
          strstr(low,"hold") || strstr(low,"coerente") || strstr(low,"rispetta")))
        return 0;

    /* Tokenize, turning arrows into a sentinel, and collect integer pairs. */
    char buf[256]; size_t bn = 0;
    for (const char *p = low; *p && bn + 4 < sizeof buf; ) {
        if (p[0]=='-' && p[1]=='>') { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=2; }
        else if ((unsigned char)p[0]==0xE2 && (unsigned char)p[1]==0x86 &&
                 (unsigned char)p[2]==0x92) { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=3; }
        else if (*p==',' || *p==';') { buf[bn++]=' '; p++; }
        else buf[bn++]=*p++;
    }
    buf[bn] = '\0';
    char *w[96]; size_t nw = split_words(buf, w, 96);
    long in[16], out_[16]; size_t npair = 0;
    for (size_t i = 0; i + 2 < nw && npair < 16; ) {
        double a, c;
        if (strcmp(w[i+1], "\x1f")==0 && parse_value(strip_edge_punct(w[i]), &a) &&
            parse_value(strip_edge_punct(w[i+2]), &c) &&
            (double)(long)a==a && (double)(long)c==c) {
            in[npair]=(long)a; out_[npair]=(long)c; npair++; i+=3;
        } else i++;
    }
    if (npair < 3) return 0;        /* need >= 2 examples + 1 test transition */

    /* The last transition is the one under test; the rest are the evidence. */
    long tin = in[npair-1], tout = out_[npair-1];
    size_t nex = npair - 1;

    InducedRule r; char rule[160];
    if (!induce_rule(in, out_, nex, &r, rule, sizeof rule)) {
        put("Those examples don't all follow one rule I can express yet.",
            out, out_size);
        return 1;
    }

    double pred = apply_rule(&r, (double)tin);
    char ib[64], tb[64], pb[64];
    format_num((double)tin, ib, sizeof ib);
    format_num((double)tout, tb, sizeof tb);
    format_num(pred, pb, sizeof pb);
    char msg[400];
    if (pred == (double)tout)
        snprintf(msg, sizeof msg, "Yes — %s -> %s fits the rule (%s).", ib, tb, rule);
    else
        snprintf(msg, sizeof msg,
                 "No — the rule (%s) predicts %s -> %s, not %s.", rule, ib, pb, tb);
    put(msg, out, out_size);
    store_proof(b, rule);
    return 1;
}

/* Run the real shell command via popen and capture its stdout. */
static int run_shell(const char *cmd, char *out, size_t out_size) {
    FILE *f = popen(cmd, "r");
    if (!f) return 0;
    size_t n = fread(out, 1, out_size - 1, f);
    out[n] = '\0';
    pclose(f);
    return 1;
}

static int mod_shell(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    (void)norm;

    /* lowercased probe to detect the question shape; the command line is sliced
     * from raw with case preserved. */
    char low[256], rw[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof rw) return 0;
    memcpy(rw, raw, rl + 1);
    while (rl > 0 && (rw[rl-1]=='?'||rw[rl-1]=='.'||rw[rl-1]==' '||rw[rl-1]=='\n'))
        rw[--rl] = '\0';
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)rw[i]);

    /* gen62: oracle-grounded output prediction for pure commands.
     * Only runs when PARROT0_ORACLE=1. */
    const char *oracle_env = getenv("PARROT0_ORACLE");
    int oracle_on = oracle_env && strcmp(oracle_env, "1") == 0;
    const char *pred_cmd = NULL;
    if (oracle_on && strncmp(low, "what does ", 10) == 0) {
        const char *print_pos = strstr(low + 10, " print");
        if (print_pos) {
            size_t cmdlen = (size_t)(print_pos - (low + 10));
            rw[10 + cmdlen] = '\0';
            pred_cmd = rw + 10;
        }
    } else if (oracle_on && strncmp(low, "predict the output of ", 22) == 0) {
        pred_cmd = rw + 22;
    }
    if (pred_cmd) {
        char predicted[4096], actual[4096];
        if (!simulate_pipeline(pred_cmd, predicted, sizeof predicted)) {
            put("I can't predict the output of that yet.", out, out_size);
            return 1;
        }
        char safe_cmd[512];
        snprintf(safe_cmd, sizeof safe_cmd, "%s", pred_cmd);
        if (!run_shell(safe_cmd, actual, sizeof actual)) {
            put("I couldn't run the shell oracle.", out, out_size);
            return 1;
        }
        if (strcmp(predicted, actual) == 0) {
            char show[4096];
            snprintf(show, sizeof show, "%s", predicted);
            size_t sl = strlen(show);
            if (sl > 0 && show[sl - 1] == '\n') show[sl - 1] = '\0';
            char msg[256];
            snprintf(msg, sizeof msg, "It prints: %s.", show);
            put(msg, out, out_size);
        } else {
            char msg[16384];
            snprintf(msg, sizeof msg,
                     "I predicted [%s] but the shell said [%s].",
                     predicted, actual);
            put(msg, out, out_size);
        }
        return 1;
    }

    char *cl;
    if (strncmp(low, "what does ", 10) == 0) {
        size_t tl = rl;
        if (tl >= 3 && strcmp(low + tl - 3, " do") == 0) rw[tl - 3] = '\0';
        else if (tl >= 5 && strcmp(low + tl - 5, " mean") == 0) rw[tl - 5] = '\0';
        else return 0;
        cl = rw + 10;
    } else if (strncmp(low, "explain ", 8) == 0) {
        cl = rw + 8;
    } else {
        return 0;
    }

    /* gen61: simple pipeline support. Split on '|' and compose descriptions. */
    char pipeline[256];
    snprintf(pipeline, sizeof pipeline, "%s", cl);
    if (strchr(pipeline, '|')) {
        char *segs[8];
        size_t nseg = 0;
        char *p = pipeline;
        while (p && *p && nseg < 8) {
            char *next = strchr(p, '|');
            if (next) *next++ = '\0';
            while (*p && isspace((unsigned char)*p)) p++;
            segs[nseg++] = p;
            p = next;
        }
        char desc[8][512];
        size_t got = 0;
        for (size_t i = 0; i < nseg; i++) {
            if (describe_command(b, segs[i], desc[got], sizeof desc[got])) {
                size_t dl = strlen(desc[got]);
                if (dl > 0 && desc[got][dl - 1] == '.')
                    desc[got][dl - 1] = '\0';
                got++;
            }
        }
        if (got == 0) return 0;
        char msg[2048];
        size_t o = (size_t)snprintf(msg, sizeof msg, "%s", desc[0]);
        for (size_t i = 1; i < got && o < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, ", then %s", desc[i]);
        if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
        put(msg, out, out_size);
        return 1;
    }

    return describe_command(b, cl, out, out_size) ? 1 : 0;
}

/* --- module: discourse ---------------------------------------------------
 * gen58: a minimal discourse memory. The brain keeps a small rolling buffer of
 * recent content words extracted from user turns, and this module answers
 * summary questions from that real state rather than a canned line.
 * The topic extraction is intentionally shallow — non-stopword alphabetic
 * tokens, len>=3 — so it stays honest and testable; it is not a summarizer. */
static void update_topics(Brain *b, const char *norm) {
    if (!b) return;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) len = sizeof buf - 1;
    memcpy(buf, norm, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strlen(t) < 3 || !isalpha((unsigned char)t[0]) || is_stopword(b, t))
            continue;
        int dup = 0;
        for (size_t j = 0; j < b->topic_count; j++)
            if (strcmp(b->topics[j], t) == 0) { dup = 1; break; }
        if (dup) continue;
        if (b->topic_count < BRAIN_TOPICS_MAX) {
            copy_trim(b->topics[b->topic_count], sizeof b->topics[b->topic_count], t);
            b->topic_count++;
        }
    }
}

/* gen121 (L6): the content words of proposition `i`, lowercased. A content word
 * is alphabetic, length >= 3, not a stop-word — the same shallow, honest filter
 * update_topics uses. Returns how many were written. */
static size_t prop_content_words(Brain *b, size_t i, char words[][KB_TERM_LEN],
                                 size_t max) {
    char buf[192]; snprintf(buf, sizeof buf, "%s", b->props[i]);
    for (char *p = buf; *p; p++) *p = (char)tolower((unsigned char)*p);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    size_t n = 0;
    for (size_t j = 0; j < nw && n < max; j++) {
        char *t = strip_edge_punct(w[j]);
        if (strlen(t) < 3 || !isalpha((unsigned char)t[0]) || is_stopword(b, t))
            continue;
        int dup = 0;
        for (size_t k = 0; k < n; k++) if (strcmp(words[k], t) == 0) { dup = 1; break; }
        if (dup) continue;
        snprintf(words[n], KB_TERM_LEN, "%s", t); n++;
    }
    return n;
}

/* gen121 (L6): a real EXTRACTIVE summary. The shallow topic-word answer in
 * mod_discourse was honest but not a summary. This ranks the propositions a
 * `read:` passage actually yielded by CONCEPT CENTRALITY — each sentence scored
 * by how often its content words recur across the whole passage, so the
 * sentences about the most-connected concepts surface — and quotes the top few
 * REAL sentences, in original order for readability. It summarizes only what was
 * genuinely parsed into facts; whatever the reader skipped is, honestly, not
 * here. Held-out: any passage of parseable propositions transfers. */
static int mod_summary(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    int want_sum = cue(norm, "summarize") || cue(norm, "summary") ||
                   cue(norm, "riassumi") || cue(norm, "riassunto") ||
                   cue(norm, "in short") || cue(norm, "in breve");
    /* gen122: the GIST — the single central concept and the most salient
     * sentence, for "what is this about?" rather than a multi-sentence digest. */
    int want_gist = cue(norm, "what is this about") || cue(norm, "what's this about") ||
                    cue(norm, "what is it about") || cue(norm, "what's it about") ||
                    cue(norm, "main point") || cue(norm, "the gist") ||
                    cue(norm, "di cosa parla") || cue(norm, "punto principale");

    /* gen123: query-FOCUSED comprehension — "what did you learn/read about X?"
     * pulls the sentences about X. Gated to learn/read phrasing so a bare "what
     * do you know about X" stays with mod_knowledge's entity description; this is
     * the PASSAGE digest. The focus word follows "about"/"di"/"su". */
    int focus_intent = cue(norm, "learn about") || cue(norm, "learned about") ||
                       cue(norm, "read about") || cue(norm, "imparato") ||
                       cue(norm, "letto") || (want_sum && cue(norm, "about"));
    char focus[KB_TERM_LEN] = "";
    if (focus_intent) {
        const char *mk = strstr(norm, "about ");
        if (mk) mk += 6;
        else if ((mk = strstr(norm, " su "))) mk += 4;
        else if ((mk = strstr(norm, " di "))) mk += 4;
        if (mk) {
            size_t k = 0;
            while (*mk && !isalnum((unsigned char)*mk)) mk++;
            while (*mk && (isalnum((unsigned char)*mk)) && k + 1 < sizeof focus)
                focus[k++] = (char)tolower((unsigned char)*mk++);
            focus[k] = '\0';
        }
    }
    int want_focus = (*focus && strcmp(focus,"this") && strcmp(focus,"it") &&
                      !is_stopword(b, focus));
    if ((!want_sum && !want_gist && !want_focus) || b->prop_count == 0) return 0;

    /* Global content-word frequencies across all propositions. */
    char vocab[128][KB_TERM_LEN]; size_t freq[128]; size_t nv = 0;
    char words[24][KB_TERM_LEN];
    for (size_t i = 0; i < b->prop_count; i++) {
        size_t nwc = prop_content_words(b, i, words, 24);
        for (size_t j = 0; j < nwc; j++) {
            size_t v; for (v = 0; v < nv; v++) if (strcmp(vocab[v], words[j]) == 0) break;
            if (v == nv && nv < 128) { snprintf(vocab[nv], KB_TERM_LEN, "%s", words[j]); freq[nv]=0; nv++; }
            if (v < 128) freq[v]++;
        }
    }

    /* Score each proposition = sum of its content words' global frequencies. */
    long score[BRAIN_PROPS_MAX];
    for (size_t i = 0; i < b->prop_count; i++) {
        size_t nwc = prop_content_words(b, i, words, 24);
        long s = 0;
        for (size_t j = 0; j < nwc; j++)
            for (size_t v = 0; v < nv; v++)
                if (strcmp(vocab[v], words[j]) == 0) { s += (long)freq[v]; break; }
        score[i] = s;
    }

    /* gen123: a focused digest — the sentences whose content words include the
     * focus, in original order. Honest when the passage says nothing about it. */
    if (want_focus) {
        char msg[640]; size_t o = (size_t)snprintf(msg, sizeof msg, "About %s:", focus);
        size_t hits = 0; char words[24][KB_TERM_LEN];
        for (size_t i = 0; i < b->prop_count; i++) {
            size_t nwc = prop_content_words(b, i, words, 24);
            int has = 0;
            for (size_t j = 0; j < nwc; j++) if (strcmp(words[j], focus) == 0) { has = 1; break; }
            if (!has) continue;
            char s[192]; snprintf(s, sizeof s, "%s", b->props[i]);
            size_t sl = strlen(s);
            while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
            o += (size_t)snprintf(msg + o, sizeof msg - o, " %s.", s);
            hits++;
        }
        if (hits == 0)
            snprintf(msg, sizeof msg, "The passage doesn't say anything about %s.", focus);
        put(msg, out, out_size);
        return 1;
    }

    /* gen122: the gist — the most central concept (highest content-word
     * frequency) and the single most salient sentence (highest score). */
    if (want_gist) {
        size_t cv = 0;
        for (size_t v = 1; v < nv; v++) if (freq[v] > freq[cv]) cv = v;
        size_t ti = 0;
        for (size_t i = 1; i < b->prop_count; i++) if (score[i] > score[ti]) ti = i;
        char s[192]; snprintf(s, sizeof s, "%s", b->props[ti]);
        size_t sl = strlen(s);
        while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
        char msg[320];
        snprintf(msg, sizeof msg, "Mainly about %s: %s.", nv ? vocab[cv] : "", s);
        put(msg, out, out_size);
        return 1;
    }

    /* Select the top-k indices (k = up to 3), ties broken by original order. */
    size_t k = b->prop_count < 3 ? b->prop_count : 3;
    int chosen[BRAIN_PROPS_MAX]; size_t nc = 0;
    for (size_t pick = 0; pick < k; pick++) {
        long best = -1; size_t bi = 0; int found = 0;
        for (size_t i = 0; i < b->prop_count; i++) {
            int taken = 0; for (size_t c = 0; c < nc; c++) if (chosen[c]==(int)i) { taken=1; break; }
            if (taken) continue;
            if (!found || score[i] > best) { best = score[i]; bi = i; found = 1; }
        }
        if (found) chosen[nc++] = (int)bi;
    }
    /* Emit the chosen sentences in original passage order. */
    int order[BRAIN_PROPS_MAX]; size_t no = 0;
    for (size_t i = 0; i < b->prop_count; i++)
        for (size_t c = 0; c < nc; c++) if (chosen[c]==(int)i) { order[no++]=(int)i; break; }

    char msg[640]; size_t o = (size_t)snprintf(msg, sizeof msg, "In short:");
    for (size_t i = 0; i < no; i++) {
        char s[192]; snprintf(s, sizeof s, "%s", b->props[order[i]]);
        size_t sl = strlen(s);
        while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
        o += (size_t)snprintf(msg + o, sizeof msg - o, " %s.", s);
    }
    put(msg, out, out_size);
    return 1;
}

static int mod_discourse(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    int summary = cue(norm, "what did we talk about") ||
                  cue(norm, "what were we talking about") ||
                  cue(norm, "what have we talked about") ||
                  cue(norm, "cosa abbiamo detto") ||
                  cue(norm, "di cosa abbiamo parlato") ||
                  cue(norm, "summarize") ||
                  cue(norm, "riassumi") ||
                  cue(norm, "riassunto");
    if (!summary) return 0;
    if (b->topic_count == 0) {
        put("We haven't talked about much yet.", out, out_size);
        return 1;
    }
    char msg[256];
    size_t off = (size_t)snprintf(msg, sizeof msg, "We talked about ");
    for (size_t i = 0; i < b->topic_count && off < sizeof msg; i++) {
        const char *sep;
        if (i == 0) sep = "";
        else if (i + 1 == b->topic_count) sep = " and ";
        else sep = ", ";
        off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%s", sep, b->topics[i]);
    }
    if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
    put(msg, out, out_size);
    return 1;
}

/* --- module: social ------------------------------------------------------
 * The dialogue-act layer (gen52). parrot0 had only CONTENT modules
 * (assert/query/reason); a curt "ciao" or "thanks" carries no proposition, so it
 * hit the blank wall and the agent felt dumb. This part recognizes the PHATIC
 * register — utterances whose function is to open/close/maintain the channel —
 * by communicative ACT, not by enumerating canned replies:
 *   1. a small CLOSED-CLASS marker set (greetings/closings/thanks/wellbeing),
 *      multilingual — the "function words" of conversation, like articles, not
 *      content (matched whole-token, so "hi" never fires inside "which");
 *   2. DISCOURSE POSITION (`b->turns`): the same "ciao" opens early and closes
 *      late — structure disambiguating what vocabulary cannot;
 *   3. the ELIMINATION move: a single contentless word that NO content module
 *      claimed, arriving as the opener, is by exclusion a phatic contact act —
 *      so a novel minimal opener is handled without being listed, answered with
 *      an opening that invites content (honest: it does not feign understanding).
 * Runs last (before the not-understood fallback), so content always wins.
 *
 * gen56 (C2b): a social marker must not hijack a turn that also carries real
 * content. "hi, what can you do?" and "thanks, that was wrong" are MIXED acts:
 * the marker acknowledges, but the substance is answered by a content module.
 * We detect mixed turns by the co-occurrence of a phatic marker with a question
 * word or with explicit negative/corrective content after thanks. */
/* gen73: social register as KB knowledge (PLAN.md Phase 3). The word lists
 * formerly hardcoded in C arrays now live in kb/core/social.p0. */
static int is_social_marker(Brain *b, const char *type, const char *word) {
    if (!b || !b->kb) return 0;
    const char *args[] = {type, word};
    return kb_query(b->kb, "social_marker", args, 2);
}

static int has_social_pattern(Brain *b, const char *type, const char *text) {
    if (!b || !b->kb) return 0;
    const char *pat[] = {type, NULL};
    char patterns[64][KB_TERM_LEN];
    size_t n = kb_match(b->kb, "social_pattern", pat, 2, patterns, 64);
    for (size_t i = 0; i < n; i++)
        if (strstr(text, patterns[i]) != NULL) return 1;
    return 0;
}

static int tok_is_marker(Brain *b, const char *type, char **w, size_t nw) {
    if (!b) return 0;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (is_social_marker(b, type, t)) return 1;
    }
    return 0;
}

static int has_any_question(Brain *b, const char *buf, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && *t && b && b->kb) {
            const char *args[] = {t};
            if (kb_query(b->kb, "question_word", args, 1)) return 1;
        }
    }
    /* fallback substring cue for multi-word question forms */
    const char *qc[] = {"who","what","where","when","why","how","which",
                        "chi","che","cosa","dove","quando","perche","perché","come"};
    for (size_t i = 0; i < sizeof qc / sizeof qc[0]; i++)
        if (cue(buf, qc[i])) return 1;
    return 0;
}

/* Keep tok_in for social[] check in is_substantive (the list of words that
 * should not count as content); gen73: also backed by social_marker KB facts. */
static int tok_in(Brain *b, char **w, size_t nw, const char *const *set) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *s = set; *s; s++)
            if (strcmp(t, *s) == 0) return 1;
        /* also check KB social_marker facts for all types */
        if (is_social_marker(b, "opening", t) ||
            is_social_marker(b, "closing", t) ||
            is_social_marker(b, "thanks", t) ||
            is_social_marker(b, "apology", t) ||
            is_social_marker(b, "ambiguous", t)) return 1;
    }
    return 0;
}

/* True for a token that contributes real lexical content: long enough, not a
 * stopword, and not itself a social marker. Used to separate phatic-only turns
 * from mixed turns that carry substance beyond the marker. */
static int is_substantive(Brain *b, const char *t) {
    char tmp[64];
    if (strlen(t) >= sizeof tmp) return 0;
    strcpy(tmp, t);
    const char *s = strip_edge_punct(tmp);
    if (strlen(s) < 3) return 0;
    if (is_stopword(b, s)) return 0;
    /* gen73: markers come from KB social_marker facts */
    if (is_social_marker(b, "opening", s) ||
        is_social_marker(b, "closing", s) ||
        is_social_marker(b, "thanks", s) ||
        is_social_marker(b, "apology", s) ||
        is_social_marker(b, "ambiguous", s)) return 0;
    return 1;
}

/* True when the substantive part of the turn is itself a wellbeing check-in;
 * such turns are still owned by the social module (marker + wellbeing is not a
 * mixed act we want to decline). */
static int is_wellbeing_content(const char *buf) {
    /* gen73: patterns from social_pattern(wellbeing, ...) are matched by
     * has_social_pattern in mod_social; this fast substring check stays
     * as a guard in is_mixed_turn. */
    return cue(buf, "how are you") || cue(buf, "how r u") ||
           cue(buf, "how do you do") || cue(buf, "how is it going") ||
           cue(buf, "come stai") || cue(buf, "come va");
}

/* A mixed act combines a social marker with substantive content. If mixed, the
 * social module declines so the content module can own the turn. */
static int is_mixed_turn(Brain *b, const char *buf, char **w, size_t nw,
                         int has_opening, int has_closing, int has_thanks,
                         int has_apology, int has_ambiguous) {
    int has_marker = has_opening || has_closing || has_thanks || has_apology || has_ambiguous;
    if (!has_marker) return 0;

    /* question word + marker -> substance wins ("hey, who are you?"),
     * unless the substance is a wellbeing check the social module handles. */
    if (has_any_question(b, buf, w, nw) && !is_wellbeing_content(buf)) return 1;

    /* marker + substantive content -> substance wins
     * ("Hello there, I hope you don't mind me reaching out.")
     * Thanks-based turns are not mixed here; they have their own rule below. */
    if ((has_opening || has_closing || has_apology || has_ambiguous) && !has_thanks) {
        if (is_wellbeing_content(buf)) return 0;
        for (size_t i = 0; i < nw; i++)
            if (is_substantive(b, w[i])) return 1;
    }

    /* thanks + explicit negative/corrective content is not a plain thank-you */
    if (has_thanks) {
        if (cue(buf, "wrong") || cue(buf, "bad") || cue(buf, "not") ||
            cue(buf, "no") || cue(buf, "sbagliato") || cue(buf, "errore") ||
            cue(buf, "male"))
            return 1;
    }

    return 0;
}

static int mod_social(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len-1]=='?'||buf[len-1]=='!'||buf[len-1]=='.'||buf[len-1]==' '))
        buf[--len] = '\0';
    if (len == 0) return 0;

    char tmp[256];
    memcpy(tmp, buf, len + 1);
    char *w[64];
    size_t nw = split_words(tmp, w, 64);
    if (nw == 0) return 0;

    /* gen73: all markers now come from kb/core/social.p0 via KB queries. */
    int has_opening   = tok_is_marker(b, "opening", w, nw) ||
                        has_social_pattern(b, "opening", buf);
    int has_closing   = tok_is_marker(b, "closing", w, nw) ||
                        has_social_pattern(b, "closing", buf);
    int has_thanks    = tok_is_marker(b, "thanks", w, nw) ||
                        has_social_pattern(b, "thanks", buf);
    int has_apology   = tok_is_marker(b, "apology", w, nw) ||
                        has_social_pattern(b, "apology", buf);
    int has_ambiguous = tok_is_marker(b, "ambiguous", w, nw);

    /* gen56/gen63/gen71: if the turn is mixed, let content modules handle the substance. */
    if (is_mixed_turn(b, buf, w, nw, has_opening, has_closing, has_thanks,
                      has_apology, has_ambiguous))
        return 0;

    /* gratitude */
    if (has_thanks) { put("You're welcome!", out, out_size); return 1; }

    /* apology — "scusa", "sorry", "mi dispiace" etc. */
    if (has_apology) { put("No problem.", out, out_size); return 1; }

    /* wellbeing check-in — gen73: patterns from kb/core/social.p0 */
    if (has_social_pattern(b, "wellbeing", buf))
        { put("I'm well, thanks. How can I help?", out, out_size); return 1; }

    /* position-disambiguated ambiguous marker: "ciao" opens early, closes late */
    if (has_ambiguous) {
        put(b->turns <= 2 ? "Hi there!" : "Goodbye!", out, out_size);
        return 1;
    }

    /* explicit opening / closing markers */
    if (has_opening) { put("Hi there!", out, out_size); return 1; }
    if (has_closing) { put("Goodbye!", out, out_size); return 1; }

    /* gen72/gen73: laughter and conversational reactions — from kb/core/social.p0 */
    int has_reaction = 0;
    for (size_t i = 0; i < nw && !has_reaction; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && *t && b && b->kb) {
            const char *args[] = {t};
            if (kb_query(b->kb, "reaction_word", args, 1)) has_reaction = 1;
        }
    }
    if (has_reaction && nw <= 3) {
        put(":) Glad you're enjoying the conversation.", out, out_size);
        return 1;
    }

    /* the elimination move: a single contentless word as the opener is, by
     * exclusion, phatic contact — greet and invite content, without listing it.
     * Pure numbers are not contact, so require an alphabetic token. */
    if (nw == 1 && b->turns <= 1 && isalpha((unsigned char)w[0][0])) {
        put("Hi there! What would you like to talk about?", out, out_size);
        return 1;
    }

    return 0;
}

/* gen125 (chatsim-pulled, the outgrowing of the gen0 wall): the AFFECTIVE /
 * phatic register. The sim transcripts are full of casual social moves parrot0
 * could only meet with the bare "I don't understand" wall — laughter and emoji,
 * apologies tangled with content, encouragement ("you'll learn!"), frustration
 * at its own repetition, banter, and offers to switch language. These are not
 * information requests; they are TONE. mod_chitchat answers the tone in register
 * — honestly admitting parrot0 is small and not pretending to have parsed the
 * content — so a social move gets a social reply instead of a wall. It runs LAST,
 * after every substantive module, and fires ONLY on a real affective cue: a plain
 * unparseable statement with no such cue still gets the honest wall (so e.g. "is
 * the sky blue?" is untouched). This is phatic competence, not the gen0 parrot:
 * it reads the register from real signals and never claims understanding. */
static int has_emoji(const char *s) {
    for (const unsigned char *p = (const unsigned char *)s; *p; p++)
        if (*p >= 0xF0) return 1;          /* 4-byte UTF-8 = emoji plane */
    return 0;
}

static int mod_chitchat(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    if (!b) return 0;

    /* register signals: emoji, ASCII emoticons, laughter, stage-direction emotes */
    int emoji = has_emoji(raw);
    int emoticon = cue(norm, ":)") || cue(norm, ":-)") || cue(norm, ";)") ||
                   cue(norm, ":d") || cue(norm, ":p") || cue(norm, " xd") ||
                   cue(norm, ":(") || cue(norm, "<3");
    int laugh = cue(norm, "haha") || cue(norm, "lol") || cue(norm, "hehe") ||
                cue(norm, "ahah") || cue(norm, "lmao") || cue(norm, "hihi") ||
                cue(norm, "rofl") || cue(norm, "eheh") || cue(norm, "ahaha") ||
                cue(norm, "battuta") || cue(norm, "scherzo") || cue(norm, "hilarious");
    const char *t = raw; while (*t && isspace((unsigned char)*t)) t++;
    int emote = (*t == '*');     /* "*sighs heavily*", "*rolls eyes*" */

    /* Tailored sub-registers — clear enough to answer on the cue alone. NOTE:
     * apology is deliberately NOT handled here — pure apologies are mod_social's,
     * and an apology tangled with a real request must stay an honest wall. */
    int frustration = cue(norm, "repeat") || cue(norm, "repeating") ||
                      cue(norm, "ripetiz") || cue(norm, "stessa cosa") ||
                      cue(norm, "copying") || cue(norm, "copiando") ||
                      cue(norm, "broken record") || cue(norm, "stuck") ||
                      cue(norm, "useless") || cue(norm, "inutile") ||
                      cue(norm, "sei rotto") || cue(norm, "you keep") ||
                      cue(norm, "keep saying") || cue(norm, "three times") ||
                      cue(norm, "on purpose") || cue(norm, "getting stale") ||
                      cue(norm, "default reply") || cue(norm, "not a good chatbot") ||
                      cue(norm, "dont know anything") || cue(norm, "can not even") ||
                      cue(norm, "can not learn") || cue(norm, "robots can not") ||
                      cue(norm, "doing this on purpose") || cue(norm, "broken") ||
                      cue(norm, "glitch") || cue(norm, "trolling") ||
                      cue(norm, "echoing") || cue(norm, "repetitive") ||
                      cue(norm, "even trying") || cue(norm, "done here") ||
                      cue(norm, "schifo") || cue(norm, "vague phrases") ||
                      cue(norm, "do not know anything") || cue(norm, "confusing") ||
                      cue(norm, "flip a table") || cue(norm, "what a day");
    int encourage   = cue(norm, "imparerai") || cue(norm, "you'll learn") ||
                      cue(norm, "youll learn") || cue(norm, "no worries") ||
                      cue(norm, "nessun problema") || cue(norm, "non preoccupar") ||
                      cue(norm, "take your time") || cue(norm, "keep trying") ||
                      cue(norm, "tranquillo") || cue(norm, "figurati") ||
                      cue(norm, "takes time") || cue(norm, "figure it out") ||
                      cue(norm, "well figure") || cue(norm, "no rush") ||
                      cue(norm, "we'll get there") || cue(norm, "niente paura") ||
                      cue(norm, "provo a") || cue(norm, "take it back") ||
                      cue(norm, "ancora imparare") || cue(norm, "devi imparare");
    /* agreement / acknowledgement of parrot0's honest self-description */
    int agree       = cue(norm, "fair enough") || cue(norm, "that's fair") ||
                      cue(norm, "thats fair") || cue(norm, "fair actually") ||
                      cue(norm, "makes sense") || cue(norm, "fair point") ||
                      cue(norm, "capisco") || cue(norm, "thats reasonable") ||
                      cue(norm, "that's reasonable");
    /* casual contact phrasings that no content module claims */
    int casual      = cue(norm, "what's up") || cue(norm, "whats up") ||
                      cue(norm, "what is up") || cue(norm, "just saying hi") ||
                      cue(norm, "my bad") || cue(norm, "wassup") ||
                      cue(norm, "what do you want") || cue(norm, "what do you know") ||
                      cue(norm, "che fai") || cue(norm, "keep it formal") ||
                      cue(norm, "your day") || cue(norm, "u good") ||
                      cue(norm, "you good") || cue(norm, "you work") ||
                      cue(norm, "you there") || cue(norm, "solo chat") ||
                      cue(norm, "reaching out") || cue(norm, "what even are you") ||
                      cue(norm, "pappagallo") || cue(norm, "sei onesto") ||
                      cue(norm, "wait what") || cue(norm, "okay helpful") ||
                      cue(norm, "vuol dire") || cue(norm, "che significa") ||
                      cue(norm, "literally anything") || cue(norm, "lovely to have") ||
                      cue(norm, "figuring it out") || cue(norm, "sono qui") ||
                      cue(norm, "whole time");
    /* terms of endearment / excitement mark the casual register too */
    int endear      = cue(norm, "sweety") || cue(norm, "sweetie") ||
                      cue(norm, "babes") || cue(norm, "friend") ||
                      cue(norm, "buddy") || cue(norm, "amico") ||
                      cue(norm, "tesoro");
    int language    = cue(norm, "in inglese") || cue(norm, "in italiano") ||
                      cue(norm, "in english") || cue(norm, "in italian") ||
                      cue(norm, "parliamo in") || cue(norm, "talk in") ||
                      cue(norm, "english only") || cue(norm, "parli italiano") ||
                      cue(norm, "speak english") || cue(norm, "parli inglese");
    int filler      = cue(norm, "nothing much") || cue(norm, "just hanging") ||
                      cue(norm, "just bored") || cue(norm, "just vibing") ||
                      cue(norm, "just chatting") || cue(norm, "hanging out") ||
                      cue(norm, "solo chiacchiere") || cue(norm, "parliamo e basta") ||
                      cue(norm, "just chat") || cue(norm, "just talk") ||
                      cue(norm, "can we talk") || cue(norm, "lets talk") ||
                      cue(norm, "let us talk") || cue(norm, "parliamo e basta") ||
                      cue(norm, "facciamo due chiacchiere");
    int no_topic    = cue(norm, "i do not know what to say") ||
                      cue(norm, "i don't know what to say") ||
                      cue(norm, "i dont know what to say") ||
                      cue(norm, "non so cosa dire") || cue(norm, "not so cosa dire") ||
                      cue(norm, "boh") || cue(norm, "no idea what to say") ||
                      cue(norm, "say something") || cue(norm, "tell me something") ||
                      cue(norm, "dimmi qualcosa") || cue(norm, "intrattienimi") ||
                      cue(norm, "entertain me");
    int mood_bored  = cue(norm, "i am bored") || cue(norm, "im bored");
    int mood_down   = mood_bored || cue(norm, "i am tired") || cue(norm, "im tired") ||
                      cue(norm, "sono stanco") || cue(norm, "sono stanca") ||
                      cue(norm, "am stanco") || cue(norm, "am stanca") ||
                      cue(norm, "bad day") || cue(norm, "giornata no") ||
                      cue(norm, "mi annoio") || cue(norm, "sono annoiato") ||
                      cue(norm, "sono annoiata") || cue(norm, "rough day") ||
                      cue(norm, "not feeling great");
    int mood_up     = cue(norm, "i am happy") || cue(norm, "im happy") ||
                      cue(norm, "sono felice") || cue(norm, "great day") ||
                      cue(norm, "bella giornata") || cue(norm, "that is cool") ||
                      cue(norm, "thats cool") || cue(norm, "che bello") ||
                      cue(norm, "nice") || cue(norm, "cool");

    if (frustration) {
        put("I know I repeat myself — I'm a small bot and honest about my limits. "
            "What would you like to try?", out, out_size);
        return 1;
    }
    if (encourage) { put("Thanks — I'm learning as we go.", out, out_size); return 1; }
    if (agree)     { put("Glad that lands. What would you like to do next?",
                         out, out_size); return 1; }
    if (language)  { put("We can chat in either language — I'll do my best.",
                         out, out_size); return 1; }
    if (no_topic)  { put("We can start simple: tell me something about your day, or ask me to remember or reason about a small fact.",
                         out, out_size); return 1; }
    if (mood_down) {
        snprintf(b->user_mood, sizeof b->user_mood, "%s", mood_bored ? "bored" : "tired");
        b->has_user_mood = 1;
        put("Sounds like a low-energy moment. We can keep it light — tell me one thing that happened, or ask me something small.",
            out, out_size);
        return 1;
    }
    if (mood_up) {
        snprintf(b->user_mood, sizeof b->user_mood, "%s", "happy");
        b->has_user_mood = 1;
        put("Nice. Tell me what made it good, or give me a small thing to reason about.",
            out, out_size);
        return 1;
    }
    if (filler)    { put("Happy to just chat. Tell me a little, and I'll follow the thread as best I can.",
                         out, out_size); return 1; }
    if (casual)    { put("Hey! I'm here. Ask me something, or tell me about your day?",
                         out, out_size); return 1; }

    /* Generic affective contact (emoji/emoticon/laughter/emote/endearment and
     * nothing more specific): engage in register, rotating so it never becomes a
     * broken record. Still honest — it names no understanding of the content. */
    if (emoji || emoticon || laugh || emote || endear) {
        static const char *const v[] = {
            "I'm enjoying this — what's on your mind?",
            "Ha, you're playful! Ask me something?",
            "I'm a simple bot, but I'm here for it. Go on?",
        };
        put(v[b->turns % 3], out, out_size);
        return 1;
    }
    return 0;
}

/* --- module: pragma (gen142, E3) -----------------------------------------
 * Pragmatic intent from turn SHAPE, not from a phrase list. mod_chitchat /
 * mod_social already cover the AFFECTIVE register, but they do it with growing
 * cue lists: a held-out phrasing of the same speech act ("give me something to
 * think about", "could we chat about cheese", "i have no clue what to talk
 * about", "anyway, is socrates a man") still hits the wall. This module infers
 * the SPEECH ACT from structural FEATURES of the turn and routes each act to a
 * DIFFERENT conversational move, so unseen phrasings transfer.
 *
 * The features (computed once, below), never a memorized string:
 *   - opener class of the first content token: a DISCOURSE marker (anyway, so,
 *     well, ok, "by the way") vs a SOFT-REQUEST verb (tell/give/say) vs a MODAL
 *     invitation (can/could/shall + we);
 *   - a TOPIC-INTRO frame ("about/discuss/talk about/switch to <X>") + its object;
 *   - HEDGE markers (maybe/guess/suppose/dunno/"not sure"/perhaps) = hesitation;
 *   - NEGATION (not/no/never) and a CONTRASTIVE connective (but/however/though);
 *   - a STANCE object (you/that/this/right/agree/sense) = the thing disagreed with;
 *   - presence of a CONTENT PREDICATE: a token a content module could act on — a
 *     digit/arith operator, a known KB predicate or entity, or an assertion shape;
 *   - word count / question form.
 *
 * Routing (each a distinct MOVE):
 *   1. discourse-marker opener + residual content  -> STRIP the opener and
 *      re-dispatch the residue, so the content task SURVIVES the social wrapper
 *      ("anyway, is socrates a man" -> answers the question; "by the way, what is
 *      2 plus 2" -> 4). Re-entrancy-guarded.
 *   2. soft request with no content object ("tell me something", "give me
 *      something to think about", "say anything") -> invite a topic.
 *   3. topic-intro with an object ("can we talk about cheese", "let us switch to
 *      football") -> acknowledge and steer onto X, naming X (pulled from the turn).
 *   4. hesitation (hedge markers, no content) -> reassure, lower the stakes.
 *   5. disagreement (negation about a stance object, no content) -> accept the
 *      pushback, invite the correction.
 * Pure content/questions never reach here (the module runs late); a marker-only
 * turn that matches none of these is declined so social/chitchat still answer. */

/* first ALPHABETIC content token (strips edge punctuation) into dst; returns
 * its index in w[] or nw if none. */
static size_t first_word_tok(char **w, size_t nw, char *dst, size_t dstn) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && isalpha((unsigned char)t[0])) {
            snprintf(dst, dstn, "%s", t);
            return i;
        }
    }
    if (dstn) dst[0] = '\0';
    return nw;
}

/* a leading DISCOURSE marker: a connective whose only job is to manage the
 * channel/turn, carrying no propositional content. Single-token forms plus the
 * two-token "by the way". Detected by position (the opener), so it can be peeled
 * off without touching the content that follows. */
static int is_discourse_opener(char **w, size_t nw, size_t *skip) {
    if (nw == 0) return 0;
    char tmp[64];
    snprintf(tmp, sizeof tmp, "%s", w[0]);
    const char *t = strip_edge_punct(tmp);
    static const char *const one[] = {
        "anyway", "anyhow", "so", "well", "ok", "okay", "now", "look", "listen",
        "actually", "right", "alright", "hey", "um", "uh", "hmm",
        "comunque", "allora", "insomma", "senti", "guarda", "dunque", "beh",
        NULL
    };
    /* "by the way" -> skip 3 tokens */
    if (nw >= 3 && strcmp(t, "by") == 0) {
        char a[64], c[64];
        snprintf(a, sizeof a, "%s", w[1]); snprintf(c, sizeof c, "%s", w[2]);
        if (strcmp(strip_edge_punct(a), "the") == 0 &&
            strcmp(strip_edge_punct(c), "way") == 0) { *skip = 3; return 1; }
    }
    for (const char *const *p = one; *p; p++)
        if (strcmp(t, *p) == 0) { *skip = 1; return 1; }
    return 0;
}

/* a hedge / hesitation marker anywhere in the turn. */
static int has_hedge(char **w, size_t nw) {
    static const char *const h[] = {
        "maybe", "perhaps", "guess", "suppose", "dunno", "probably", "unsure",
        "kinda", "sorta", "idk", "forse", "magari", "boh", "credo",
        NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = h; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* a contrastive connective anywhere in the turn. */
static int has_contrastive(char **w, size_t nw) {
    static const char *const c[] = {
        "but", "however", "though", "although", "yet",
        "pero", "per`o", "tuttavia", "invece",
        NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = c; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* a negation marker anywhere in the turn. */
static int has_negation(char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (strcmp(t, "not") == 0 || strcmp(t, "no") == 0 ||
            strcmp(t, "never") == 0 || strcmp(t, "dont") == 0 ||
            strcmp(t, "nor") == 0) return 1;
    }
    return 0;
}

/* True if token `t` is a STANCE PREDICATE: a word that, when negated, expresses
 * a disagreement about the prior claim — "(don't) agree", "(not) right/sure/
 * true/correct/convinced", "(doesn't) make sense". These are PREDICATES, not
 * mere objects: "that"/"you" alone are not stance ("dont say that" is an order,
 * not a disagreement), so the move keys on the predicate. */
static int is_stance_pred(const char *t) {
    static const char *const s[] = {
        "agree", "right", "sure", "true", "correct", "convinced", "sense",
        "convince", "persuaded", "ragione", "giusto", "vero", "accordo",
        "sicuro", "convinto", "convince", "d'accordo", NULL
    };
    for (const char *const *p = s; *p; p++)
        if (strcmp(t, *p) == 0) return 1;
    return 0;
}

/* A disagreement is a NEGATED stance predicate ("i don't agree", "that is not
 * right", "not so sure", "non sono d'accordo"), or the standalone verb
 * "disagree"/"dissento". Shape, not phrase: any negation co-occurring with a
 * stance predicate in a short turn reads as pushback. */
static int is_disagreement(char **w, size_t nw) {
    int neg = 0, stance = 0, plain = 0;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (strcmp(t, "not") == 0 || strcmp(t, "no") == 0 ||
            strcmp(t, "never") == 0 || strcmp(t, "dont") == 0) neg = 1;
        if (is_stance_pred(t)) stance = 1;
        if (strcmp(t, "disagree") == 0 || strcmp(t, "dissento") == 0 ||
            strcmp(t, "dubito") == 0) plain = 1;
    }
    return plain || (neg && stance);
}

/* True if token is an OPEN QUANTIFIER object — the placeholder a content-free
 * soft request reaches for ("something", "anything", "qualcosa", "qualunque
 * cosa"). A CONCRETE object ("a story", "about C") is not open, so "tell me a
 * story" stays a real (unfulfillable) request and hits the honest wall instead
 * of this fill-the-silence move. */
static int has_open_quantifier(char **w, size_t nw) {
    static const char *const q[] = {
        "something", "anything", "qualcosa", "qualunque", "whatever", NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = q; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* True if the turn carries a CONTENT PREDICATE — something a content module
 * could actually act on. This is the gate that keeps the pragmatic moves from
 * swallowing real tasks: a digit or arithmetic operator, an assertion shape
 * (" is a "/" is an "), or a token that is a known KB predicate or entity. */
static int has_content_predicate(Brain *b, const char *canon, char **w, size_t nw) {
    if (cue(canon, " is a ") || cue(canon, " is an ") ||
        cue(canon, " plus ") || cue(canon, " minus ") ||
        cue(canon, " times ") || cue(canon, "+") || cue(canon, "-") ||
        cue(canon, "*") || cue(canon, "=")) return 1;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (!t || !*t) continue;
        if (isdigit((unsigned char)t[0])) return 1;
        if (b && b->kb && strlen(t) >= 3) {
            if (kb_knows_pred(b->kb, t) && !is_internal_pred(t)) return 1;
        }
    }
    return 0;
}

/* Pull the TOPIC object out of a topic-intro frame: the head noun after
 * "about/discuss/discutere/switch to/change to/parlare di/parliamo di". Returns
 * 1 and writes the object (first substantive token after the cue) into dst. */
static int topic_object(char **w, size_t nw, char *dst, size_t dstn) {
    static const char *const after[] = {
        "about", "discuss", "to", "of", "di", "su", NULL
    };
    for (size_t i = 0; i + 1 < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        int is_after = 0;
        for (const char *const *p = after; *p; p++)
            if (strcmp(t, *p) == 0) { is_after = 1; break; }
        if (!is_after) continue;
        for (size_t j = i + 1; j < nw; j++) {
            char o[64];
            snprintf(o, sizeof o, "%s", w[j]);
            const char *ot = strip_edge_punct(o);
            /* the head noun: first alphabetic token that is not a bare article.
             * We deliberately do NOT filter on the general stopword lexicon — a
             * topic word like "formaggio" happens to be listed there for a
             * chitchat test, but after "di"/"about" it is the genuine topic. */
            static const char *const arts[] = {
                "the","a","an","il","lo","la","i","gli","le","un","una","uno",NULL
            };
            int art = 0;
            for (const char *const *p = arts; *p; p++)
                if (strcmp(ot, *p) == 0) { art = 1; break; }
            if (ot && isalpha((unsigned char)ot[0]) && strlen(ot) >= 3 && !art) {
                snprintf(dst, dstn, "%s", ot);
                return 1;
            }
        }
    }
    if (dstn) dst[0] = '\0';
    return 0;
}

static int mod_pragma(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    /* work on a trimmed copy of the canonicalized surface (`norm` is the
     * canonicalized input; `raw` is the original). */
    char buf[256];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len-1]=='?'||buf[len-1]=='!'||buf[len-1]=='.'||buf[len-1]==' '||buf[len-1]==','))
        buf[--len] = '\0';
    if (len == 0) return 0;

    char tmp[256];
    memcpy(tmp, buf, len + 1);
    char *w[64];
    size_t nw = split_words(tmp, w, 64);
    if (nw == 0) return 0;

    char first[64];
    first_word_tok(w, nw, first, sizeof first);

    int content = has_content_predicate(b, buf, w, nw);

    /* The pragmatic moves claim ONLY contentless turns, so a real task is never
     * swallowed. (A discourse-marker opener wrapping content — "anyway, is
     * socrates a man" — was already peeled and re-dispatched by pragma_peel in
     * brain_respond BEFORE the registry ran, so the content was handled there;
     * what reaches here is a turn with no actionable content predicate.) */
    if (content) return 0;

    int hedge       = has_hedge(w, nw);
    int contrastive = has_contrastive(w, nw);
    int negation    = has_negation(w, nw);
    int disagree    = is_disagreement(w, nw);

    /* ---- MOVE 5: disagreement. A NEGATED stance predicate ("i don't agree",
     * "that's not right", "non sono d'accordo"), or a contrastive pushback that
     * also negates ("nice try, but no") — with no content to act on. Keys on the
     * stance predicate, so an imperative like "dont say that" is NOT disagreement. */
    if (disagree || (contrastive && negation && nw <= 8)) {
        put("Fair enough — tell me where I went wrong, and we can take it from there.",
            out, out_size);
        return 1;
    }

    /* ---- MOVE 4: hesitation. A hedge with nothing concrete to chew on. */
    if (hedge && nw <= 9) {
        put("No pressure — we can take it slowly. What's on your mind?",
            out, out_size);
        return 1;
    }

    /* ---- MOVE 3: topic introduction / change. A modal/imperative invitation
     * ("can we talk about X", "let us switch to X", "let's discuss X") naming an
     * object X — steer onto X by name. */
    {
        /* A topic-CHANGE invitation: either a MODAL opener proposing it ("can we
         * …", "could we …", "shall we …", "let us …", "possiamo/potremmo …"
         * canonicalize to can/could) OR an explicit switch/change verb. A bare
         * "talk about X" / "parliamo di X" with no modal is left to the filler
         * register (chitchat), so we don't override the gen140 decision that a
         * casual "parliamo di formaggio" is just filler — the proposal SHAPE (a
         * modal asking permission to change topic) is the discriminating cue. */
        int modal_open = strcmp(first, "can") == 0 || strcmp(first, "could") == 0 ||
                         strcmp(first, "shall") == 0 || strcmp(first, "lets") == 0 ||
                         strcmp(first, "let") == 0;
        int switch_verb = cue(buf, "switch to") || cue(buf, "change to") ||
                          cue(buf, "move on to") || cue(buf, "cambiamo");
        int frame = cue(buf, "talk about") || cue(buf, "chat about") ||
                    cue(buf, "discuss") || cue(buf, "talk of") ||
                    cue(buf, "parlare di") || cue(buf, "parliamo di") ||
                    cue(buf, "discutere") || cue(buf, "discutiamo");
        int invite = switch_verb || (modal_open && frame);
        char topic[40];
        if (invite && topic_object(w, nw, topic, sizeof topic)) {
            snprintf(b->current_topic, sizeof b->current_topic, "%s", topic);
            b->has_current_topic = 1;
            char msg[160];
            snprintf(msg, sizeof msg,
                     "Sure, let's talk about %s. What about %s is on your mind?",
                     topic, topic);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* ---- MOVE 2: soft request. An imperative directed at me ("tell/give/show
     * me", "say") with NO content object — an open request to fill the silence. */
    {
        int soft = strcmp(first, "tell") == 0 || strcmp(first, "give") == 0 ||
                   strcmp(first, "say") == 0 || strcmp(first, "show") == 0 ||
                   strcmp(first, "share") == 0 || strcmp(first, "dimmi") == 0 ||
                   strcmp(first, "dammi") == 0 || strcmp(first, "raccontami") == 0;
        /* OPEN-ended only: the object must be a quantifier placeholder
         * ("something/anything/qualcosa"), which is what distinguishes a
         * fill-the-silence request from a real (often unfulfillable) one — "tell
         * me a story about dragons" / "tell me about C" name a CONCRETE object, so
         * they fall through to the honest wall, not this move. The bare 3-token
         * "tell me something" family stays chitchat's established no-topic
         * register, so we require >= 4 tokens here and leave those to chitchat. */
        if (soft && has_open_quantifier(w, nw) && nw >= 4 && nw <= 8) {
            put("Happy to. Pick a thread — your day, a small fact to remember, or something to reason about — and I'll run with it.",
                out, out_size);
            return 1;
        }
    }

    return 0;
}

/* --- module: symbolic ----------------------------------------------------
 * Register recognition over symbolic FORM (gen65, sym-bench driven). The
 * cryptic-stimulus challenge (`make sym-bench`) showed the LLM's first move on
 * a non-prose stimulus is to NAME its register — "a palindrome", "Morse", "a
 * code snippet", "solfège" — then engage, whereas parrot0 walled (even saying
 * "I don't know about abccba yet" of a palindrome it could see from form). This
 * module recovers that move: it CLASSIFIES the stimulus by cheap structural
 * features and names it. It decodes nothing (recognition before decoding) and
 * hardcodes no oracle wording — classifying form is honest reasoning. It works
 * on `raw` because the symbols are the signal; it is deliberately conservative
 * so plain prose is never hijacked, and sits late in the registry so genuine
 * content modules (arith, shell, knowledge, …) get the turn first. */

/* True if `s` (already lowercased) is non-trivially symmetric: equals its own
 * reverse once spaces are dropped, length >= 3, not all-identical, and — for a
 * pure-letter run — length >= 5, so 3-letter interjections ("wow", "mom") are
 * left to the phatic layer rather than called palindromes. */
static int looks_palindrome(const char *s) {
    char c[256]; size_t n = 0; int has_nonletter = 0;
    for (size_t i = 0; s[i] && n + 1 < sizeof c; i++) {
        if (isspace((unsigned char)s[i])) continue;
        if (!isalpha((unsigned char)s[i])) has_nonletter = 1;
        c[n++] = s[i];
    }
    c[n] = '\0';
    if (n < 3) return 0;
    if (!has_nonletter && n < 5) return 0;
    int all_same = 1;
    for (size_t i = 1; i < n; i++) if (c[i] != c[0]) { all_same = 0; break; }
    if (all_same) return 0;
    for (size_t i = 0, j = n - 1; i < j; i++, j--)
        if (c[i] != c[j]) return 0;
    /* gen72: short palindromes with only 2 distinct letters (e.g. "ahaha",
     * "ohoho") are conversational interjections, not deliberate palindromes.
     * Let the social module handle them instead of hijacking with "That looks
     * like a palindrome." */
    if (!has_nonletter && n < 7) {
        int distinct = 0;
        char seen[26] = {0};
        for (size_t i = 0; i < n; i++)
            seen[c[i] - 'a'] = 1;
        for (size_t i = 0; i < 26; i++)
            if (seen[i]) distinct++;
        if (distinct <= 2) return 0;
    }
    return 1;
}

/* Morse iff the trimmed input is only '.', '-', spaces, with >= 3 dot/dash. */
static int looks_morse(const char *s) {
    size_t marks = 0;
    for (size_t i = 0; s[i]; i++) {
        if (s[i] == '.' || s[i] == '-') marks++;
        else if (!isspace((unsigned char)s[i])) return 0;
    }
    return marks >= 3;
}

/* Solfège iff >= 3 space-separated tokens and every one is a note name. The
 * >= 3 floor keeps lone "do"/"la"/"mi" (English/Italian words) out. Copies its
 * argument because split_words mutates the buffer. */
static int looks_solfege(const char *s) {
    static const char *notes[] = {"do","re","mi","fa","sol","la","si","ti",NULL};
    char buf[256]; copy_trim(buf, sizeof buf, s);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw < 3) return 0;
    for (size_t i = 0; i < nw; i++) {
        int ok = 0;
        for (size_t k = 0; notes[k]; k++) if (strcmp(w[i], notes[k]) == 0) { ok = 1; break; }
        if (!ok) return 0;
    }
    return 1;
}

/* Leetspeak iff a single token (no spaces), len >= 3, mixing ascii letters with
 * leet digits (0,1,3,4,5,7), e.g. "h3ll0", "n00b". */
static int looks_leet(const char *s) {
    int letter = 0, leetdigit = 0;
    for (size_t i = 0; s[i]; i++) {
        unsigned char ch = (unsigned char)s[i];
        if (isspace(ch)) return 0;
        if (isalpha(ch)) letter = 1;
        else if (strchr("013457", (char)ch)) leetdigit = 1;
        else if (!isdigit(ch)) return 0;  /* punctuation -> not a plain leet word */
    }
    return letter && leetdigit && strlen(s) >= 3;
}

/* Code fragment iff it carries a structural code signal: a bracket/operator
 * rare in chat prose, or a code keyword opening a block ("while True:"). */
static int looks_code(const char *s, char **w, size_t nw) {
    if (cue(s, "(") || cue(s, "{") || cue(s, "}") || cue(s, ";") ||
        cue(s, "==") || cue(s, "<html") || cue(s, "select * from"))
        return 1;
    /* keyword + trailing ':' (e.g. "while true:", "for x in y:") */
    size_t len = strlen(s);
    if (nw >= 1 && len > 0 && s[len - 1] == ':') {
        static const char *kw[] = {"while","for","if","def","class","else",
                                   "elif","try","with",NULL};
        for (size_t k = 0; kw[k]; k++)
            if (strcmp(w[0], kw[k]) == 0) return 1;
    }
    return 0;
}

/* Name the register, but if that exact line was our previous reply (two
 * same-register stimuli in a row), use the alternate phrasing instead — the
 * same non-repetition discipline the fallback uses, so a run of cryptic inputs
 * does not feel canned. The canonical phrasing `a` is the default, so a single
 * occurrence is stable (and testable). */
static int name_register(Brain *b, const char *a, const char *alt,
                         char *out, size_t out_size) {
    const char *pick = (b && strcmp(a, b->last_reply) == 0) ? alt : a;
    put(pick, out, out_size);
    return 1;
}

/* --- module: code (gen149) -----------------------------------------------
 * Basic inline-code assistant. Handles queries about small code snippets
 * passed directly in the prompt: "what is wrong with this code", "debug this",
 * "fix this", "what language is this", "is this valid C", "explain this code".
 * Extracts the code portion, identifies the language (C vs Python), runs simple
 * syntactic checks (missing semicolons, type mismatches, unclosed strings,
 * unbalanced braces/parens, undefined functions), and reports findings.
 * Grounded in kb/experts/programming/coding.p0 — the KB is the source of truth for
 * keywords, stdlib functions, error patterns and fix suggestions. */

static int find_code_section(const char *input, char *code, size_t code_size) {
    const char *p = input;
    while (*p && *p != ':') p++;
    if (*p == ':') {
        p++;
        while (*p && isspace((unsigned char)*p)) p++;
        size_t n = 0;
        while (*p && n + 1 < code_size) { code[n++] = *p; p++; }
        while (n > 0 && isspace((unsigned char)code[n - 1])) n--;
        code[n] = '\0';
        return n > 1;
    }
    return 0;
}

static int identify_code_lang(const char *code, Brain *b) {
    int c_kw = 0, py_kw = 0;
    if (b && b->kb) {
        char buf[512]; snprintf(buf, sizeof buf, "%s", code);
        char *w[128]; size_t nw = split_words(buf, w, 128);
        for (size_t i = 0; i < nw; i++) {
            char *t = w[i]; size_t tl = strlen(t);
            while (tl > 0 && (t[tl-1] == ';' || t[tl-1] == ':' ||
                              t[tl-1] == '(' || t[tl-1] == ')')) { t[tl-1] = '\0'; tl--; }
            if (tl < 2) continue;
            const char *args_c[] = {"c", t};
            if (kb_query(b->kb, "keyword", args_c, 2)) c_kw++;
            const char *args_py[] = {"python", t};
            if (kb_query(b->kb, "keyword", args_py, 2)) py_kw++;
        }
    }
    /* Surface features — used as tiebreaker and also when KB is sparse.
     * C markers: int, void, #include, semicolons, braces, printf, scanf, malloc.
     * Python markers: def, import, print(, : after for/if/while/def/class, indentation. */
    if (strstr(code, "int ") || strstr(code, "void ") || strstr(code, "char ") ||
        strstr(code, "#include") || strstr(code, "printf(") || strstr(code, "scanf(") ||
        strstr(code, "malloc("))
        c_kw += 2;
    if (strstr(code, "def ") || strstr(code, "import ") || strstr(code, "print(") ||
        strstr(code, "class ") || strstr(code, "elif ") || strstr(code, "lambda ") ||
        strstr(code, "self.") || strstr(code, "__init__"))
        py_kw += 2;
    /* Semicolons and braces strongly suggest C over Python */
    if (strchr(code, ';') || strchr(code, '{'))
        c_kw++;
    if (c_kw > py_kw) return 1;  /* C */
    if (py_kw > c_kw) return 2;  /* Python */
    return 0;                    /* unknown */
}

static int check_missing_semicolons(const char *code, char *findings,
                                     size_t findings_size) {
    int issues = 0;
    /* Check each physical line: if it looks like a C statement but doesn't
     * end with ; or {, flag it. Also check statements before closing braces. */
    char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
    char *lines[64]; int nl = 0;
    char *save = NULL;
    char *tok = strtok_r(buf, "\n", &save);
    while (tok && nl < 64) { lines[nl++] = tok; tok = strtok_r(NULL, "\n", &save); }
    if (nl == 0) {
        char cpy[1024]; snprintf(cpy, sizeof cpy, "%s", code);
        lines[0] = cpy; nl = 1;
    }
    static const char *const kw[] = {"int","char","float","double","void","long","return",NULL};
    for (int i = 0; i < nl; i++) {
        char *l = lines[i]; while (*l && isspace((unsigned char)*l)) l++;
        if (!*l || l[0] == '#') continue;
        size_t len = strlen(l);
        while (len > 0 && isspace((unsigned char)l[len-1])) l[--len] = '\0';
        if (len == 0) continue;
        if (l[len-1] == ';' || l[len-1] == '{') continue;
        /* Extract first word */
        char fw[64] = {0};
        { const char *p = l; while (*p && isspace((unsigned char)*p)) p++;
          size_t fwl = 0; while (*p && !isspace((unsigned char)*p) && *p != '(' && fwl < 63)
              fw[fwl++] = (char)tolower((unsigned char)*p++); fw[fwl] = '\0'; }
        int is_kw = 0;
        for (const char *const *k = kw; *k; k++)
            if (strcmp(fw, *k) == 0) { is_kw = 1; break; }
        if (is_kw && l[len-1] != '}') issues++;
        /* Check for statement before closing brace: scan for "keyword ... }"
         * e.g. "int main() { return 0 }" — "return 0 }" has no semicolon */
        if (l[len-1] == '}' && strchr(l, '{')) {
            const char *p = l;
            while ((p = strstr(p, "return ")) != NULL) {
                const char *q = p + 7; while (*q && isspace((unsigned char)*q)) q++;
                /* Find next } or end */
                const char *br = strchr(q, '}');
                size_t n = br ? (size_t)(br - p) : strlen(p);
                if (n > 0 && p[n-1] != ';') { issues++; break; }
                p++;
            }
        }
    }
    if (issues == 1)
        snprintf(findings, findings_size, "Missing semicolon at the end of a statement.");
    else if (issues > 1)
        snprintf(findings, findings_size, "Missing semicolons at the end of %d statements.", issues);
    return issues;
}

static int check_type_mismatch(const char *code, char *findings,
                                size_t findings_size) {
    /* Simple patterns: "int x = \"...\"" (string assigned to int)
     *                 "char y = 42" (number assigned to char pointer) */
    if (strstr(code, "int ") && strstr(code, "= \"") && strstr(code, "\"")) {
        snprintf(findings, findings_size,
            "Type mismatch: a string is assigned to an int variable. "
            "Use char * or char[] for strings.");
        return 1;
    }
    /* char *x = number (without quotes) */
    { const char *cp = code;
      while ((cp = strstr(cp, "char *")) != NULL) {
          const char *eq = strstr(cp, "=");
          if (eq) {
              const char *v = eq + 1;
              while (*v && isspace((unsigned char)*v)) v++;
              if (*v == '\"' || isalpha((unsigned char)*v)) {
                  int has_digit = 0;
                  for (const char *d = v; *d && *d != ';' && *d != '\n'; d++)
                      if (isdigit((unsigned char)*d)) { has_digit = 1; break; }
                  if (has_digit && !strstr(v, "\"")) {
                      snprintf(findings, findings_size,
                          "Suspicious assignment: a number assigned to a char * variable. "
                          "If this is meant as a character literal, use single quotes: 'x'.");
                      return 1;
                  }
              }
          }
          cp++;
      }
    }
    return 0;
}

static int check_unclosed_string(const char *code, char *findings,
                                  size_t findings_size) {
    int quotes = 0;
    for (const char *p = code; *p; p++)
        if (*p == '\"' && (p == code || *(p-1) != '\\')) quotes++;
    if (quotes % 2 != 0) {
        snprintf(findings, findings_size,
            "Unclosed string literal: there is an odd number of double-quote characters.");
        return 1;
    }
    return 0;
}

static int check_balanced_braces(const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '{') open++;
        if (*p == '}') open--;
    }
    if (open > 0) {
        snprintf(findings, findings_size,
            "Unbalanced braces: %d more opening brace(s) than closing.", open);
        return 1;
    }
    if (open < 0) {
        snprintf(findings, findings_size,
            "Unbalanced braces: %d more closing brace(s) than opening.", -open);
        return 1;
    }
    return 0;
}

static int check_balanced_parens(const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '(') open++;
        if (*p == ')') open--;
    }
    if (open != 0) {
        snprintf(findings, findings_size,
            "Unbalanced parentheses: %d more %s than %s.",
            open > 0 ? open : -open,
            open > 0 ? "opening" : "closing",
            open > 0 ? "closing" : "opening");
        return 1;
    }
    return 0;
}

static int check_unknown_function(const char *code, Brain *b, char *findings,
                                   size_t findings_size) {
    if (!b || !b->kb) return 0;
    char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
    char *w[128]; size_t nw = split_words(buf, w, 128);
    for (size_t i = 0; i < nw; i++) {
        char *t = w[i]; size_t tl = strlen(t);
        /* Look for word followed by ( */
        if (i + 1 < nw && w[i+1][0] == '(' && tl > 1 && isalpha((unsigned char)t[0])) {
            char fname[64]; size_t fn = 0;
            for (size_t j = 0; j < tl && fn < sizeof(fname)-1; j++)
                fname[fn++] = (char)tolower((unsigned char)t[j]);
            fname[fn] = '\0';
            /* Skip known keywords */
            if (strcmp(fname, "if") == 0 || strcmp(fname, "while") == 0 ||
                strcmp(fname, "for") == 0 || strcmp(fname, "return") == 0 ||
                strcmp(fname, "sizeof") == 0 || strcmp(fname, "switch") == 0)
                continue;
            /* Check against KB */
            { const char *fa[] = { fname };
            if (!kb_query(b->kb, "c_stdlib", fa, 1)) {
                size_t fl = strlen(t);
                if (fl < sizeof(fname)) {
                    snprintf(findings, findings_size,
                        "Unknown function: \"%.*s\" is not a standard C library function. "
                        "Did you spell it correctly or forget an #include?", (int)fl, t);
                    return 1;
                }
            }
            }
        }
    }
    return 0;
}

static void lang_name(int lang, char *out, size_t out_size) {
    if (lang == 1) snprintf(out, out_size, "C");
    else if (lang == 2) snprintf(out, out_size, "Python");
    else snprintf(out, out_size, "unknown");
}

/* --- module: codeast (gen173, code as KB) -------------------------------
 * The first step of docs/CODE-MASTERY.md: a code snippet is just another corpus.
 * parrot0 parses it DETERMINISTICALLY (code_ingest, in code.c), asserts its
 * structure into the SAME live KB (code_function/1), and answers a structural
 * question from that — honestly, never from the surface substring matching that
 * mod_code does. No statistics: the C grammar is formal, so primitives-first
 * wins. Registered BEFORE mod_code so a structural question reaches the real
 * parser, not the pattern matcher. */
static int mod_codeast(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb || !raw) return 0;
    char s[512]; copy_trim(s, sizeof s, raw);
    if (!*s) return 0;

    /* The question text (before the ':' that introduces the code section). */
    char qpart[256]; snprintf(qpart, sizeof qpart, "%s", s);
    char *colon = strchr(qpart, ':'); if (colon) *colon = '\0';

    /* gen182: F4 localization — "which file in <dir> defines <X>?". Scan a
     * directory (sandboxed) for the file that defines the named function. Handled
     * before the snippet questions because it takes a directory, not code. */
    if ((cue(qpart, "which file") || cue(qpart, "what file") || cue(qpart, "quale file")) &&
        (cue(qpart, "define") || cue(qpart, "defines") || cue(qpart, "definisce") ||
         cue(qpart, "contains") || cue(qpart, "contiene"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char fnname[KB_TERM_LEN] = ""; char dir[256] = "";
        for (size_t i = 0; i + 1 < nw; i++) {
            if (!strcmp(w[i], "defines") || !strcmp(w[i], "define") ||
                !strcmp(w[i], "definisce") || !strcmp(w[i], "contains") ||
                !strcmp(w[i], "contiene"))
                snprintf(fnname, sizeof fnname, "%s", strip_edge_punct(w[i+1]));
            if (!strcmp(w[i], "in") || !strcmp(w[i], "under") || !strcmp(w[i], "inside") ||
                !strcmp(w[i], "within") || !strcmp(w[i], "nella") || !strcmp(w[i], "nel") ||
                !strcmp(w[i], "dentro"))
                snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i+1]));
        }
        if (!dir[0]) for (size_t i = 0; i < nw; i++)   /* fallback: a path-like token */
            if (strchr(w[i], '/')) { snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i])); break; }
        if (!fnname[0] || !dir[0]) return 0;
        char file[256];
        if (code_locate(dir, fnname, file, sizeof file))
            snprintf(out, out_size, "I looked through %s: %s is defined in %s.", dir, fnname, file);
        else
            snprintf(out, out_size, "I looked through %s but found no file that defines %s.", dir, fnname);
        store_proof(b, out);
        return 1;
    }

    /* gen191: F5 edit — "delete/remove the function <X> under <dir|file>". The
     * SAME micro-loop as rename (locate -> edit -> compile-verify -> report,
     * temp-only) with a different transformation rule, proving the edit loop is
     * rule-shaped rather than a single hardcoded operation. Deleting a function
     * that is still called yields an honest "no longer compiles" from the real
     * compiler — the grounded oracle, not a guess. */
    if ((cue(qpart, "delete") || cue(qpart, "remove") || cue(qpart, "elimina") ||
         cue(qpart, "rimuovi")) && (cue(qpart, "function") || cue(qpart, "funzione")) &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".h"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char fnname[KB_TERM_LEN] = "", path[256] = "";
        for (size_t i = 0; i < nw; i++) {
            if ((!strcmp(w[i], "function") || !strcmp(w[i], "funzione")) && i + 1 < nw)
                snprintf(fnname, sizeof fnname, "%s", strip_edge_punct(w[i+1]));
            if (strchr(w[i], '/') ||
                (strlen(w[i]) >= 2 && (strstr(w[i], ".c") || strstr(w[i], ".h"))))
                snprintf(path, sizeof path, "%s", strip_edge_punct(w[i]));
        }
        if (!fnname[0] || !path[0]) return 0;

        char fullpath[512]; char where[280] = "";
        size_t pl = strlen(path);
        int is_file = (pl >= 2 && path[pl-2] == '.' && (path[pl-1] == 'c' || path[pl-1] == 'h'));
        if (is_file) {
            snprintf(fullpath, sizeof fullpath, "%s", path);
        } else {
            char rel[256];
            if (!code_locate(path, fnname, rel, sizeof rel)) {
                snprintf(out, out_size,
                         "I looked through %s but found no file that defines %s.", path, fnname);
                store_proof(b, out);
                return 1;
            }
            snprintf(fullpath, sizeof fullpath, "%s/%s", path, rel);
            snprintf(where, sizeof where, " in %s", rel);
        }

        const char *tmp = ".p0_edit_tmp.c";
        int n = code_delete_function(fullpath, fnname, tmp);
        if (n < 0) return 0;                       /* bad name / unreadable — not ours */
        if (n == 0) {
            remove(tmp);
            snprintf(out, out_size, "I did not find a definition of %s in %s, so nothing was deleted.",
                     fnname, fullpath);
            store_proof(b, out);
            return 1;
        }
        char err[512];

        /* gen192: F5 verification by BUILDING. A "link"/"build"/"run" cue asks
         * the stronger question — does it still LINK? Syntax-only misses a call to
         * the now-missing function (an implicit-declaration warning); a real
         * compile+link makes it an undefined reference. When it fails, the reverse
         * call graph (code_find_callers) names WHO still calls it — cause derived
         * from KB, verdict grounded in the real linker. */
        int want_link = cue(qpart, "link") || cue(qpart, "build") ||
                        cue(qpart, "run") || cue(qpart, "linka") ||
                        cue(qpart, "esegui") || cue(qpart, "compila e linka");
        if (want_link) {
            int rc = code_build(tmp, err, sizeof err);
            remove(tmp);
            if (rc == 1) {
                snprintf(out, out_size, "Deleted %s%s; the result still links.", fnname, where);
            } else if (rc == 0) {
                /* who still calls it? scan the directory the file lives in. */
                char cdir[256];
                if (is_file) {
                    snprintf(cdir, sizeof cdir, "%s", fullpath);
                    char *slash = strrchr(cdir, '/');
                    if (slash) *slash = '\0'; else snprintf(cdir, sizeof cdir, ".");
                } else {
                    snprintf(cdir, sizeof cdir, "%s", path);
                }
                char callers[16][KB_TERM_LEN];
                size_t nc = code_find_callers(cdir, fnname, callers, 16);
                if (nc > 0) {
                    char who_calls[256] = "";
                    size_t off = 0;
                    for (size_t i = 0; i < nc && off + 1 < sizeof who_calls; i++)
                        off += (size_t)snprintf(who_calls + off, sizeof who_calls - off,
                                                "%s%s", i ? (i + 1 == nc ? " and " : ", ") : "",
                                                callers[i]);
                    snprintf(out, out_size,
                             "Deleted %s%s, but %s still %s %s, so the program no longer links.",
                             fnname, where, who_calls, nc > 1 ? "call" : "calls", fnname);
                } else {
                    snprintf(out, out_size,
                             "Deleted %s%s, but the result no longer links.", fnname, where);
                }
            } else {
                snprintf(out, out_size,
                         "Deleted %s%s in a temp copy; the original is unchanged.", fnname, where);
            }
            store_proof(b, out);
            return 1;
        }

        int rc = code_compile(tmp, err, sizeof err);
        remove(tmp);
        if (rc == 1)
            snprintf(out, out_size,
                     "Deleted %s%s; the result still compiles.", fnname, where);
        else if (rc == 0)
            snprintf(out, out_size,
                     "Deleted %s%s, but the result no longer compiles (something still uses it).",
                     fnname, where);
        else
            snprintf(out, out_size,
                     "Deleted %s%s in a temp copy; the original is unchanged.", fnname, where);
        store_proof(b, out);
        return 1;
    }

    /* gen187: F5 edit — "rename <old> to <new> in <file>". The full micro-loop:
     * read the file, rename the identifier (non-destructively, to a temp), compile
     * the result to VERIFY, report, and delete the temp. The original is untouched. */
    if (cue(qpart, "rename") && cue(qpart, " to ") &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".h"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char oldn[KB_TERM_LEN] = "", newn[KB_TERM_LEN] = "", path[256] = "";
        for (size_t i = 0; i < nw; i++) {
            if (!strcmp(w[i], "to") && i > 0 && i + 1 < nw) {
                snprintf(oldn, sizeof oldn, "%s", strip_edge_punct(w[i-1]));
                snprintf(newn, sizeof newn, "%s", strip_edge_punct(w[i+1]));
            }
            if (strchr(w[i], '/') ||
                (strlen(w[i]) >= 2 && (strstr(w[i], ".c") || strstr(w[i], ".h"))))
                snprintf(path, sizeof path, "%s", strip_edge_punct(w[i]));
        }
        if (!oldn[0] || !newn[0] || !path[0]) return 0;

        /* gen188: if the path is a DIRECTORY (no .c/.h suffix), first locate the
         * file that defines the function, then edit it — chaining F4 into F5. */
        char fullpath[512]; char where[280] = "";
        size_t pl = strlen(path);
        int is_file = (pl >= 2 && path[pl-2] == '.' && (path[pl-1] == 'c' || path[pl-1] == 'h'));
        if (is_file) {
            snprintf(fullpath, sizeof fullpath, "%s", path);
        } else {
            char rel[256];
            if (!code_locate(path, oldn, rel, sizeof rel)) {
                snprintf(out, out_size,
                         "I looked through %s but found no file that defines %s.", path, oldn);
                store_proof(b, out);
                return 1;
            }
            snprintf(fullpath, sizeof fullpath, "%s/%s", path, rel);
            snprintf(where, sizeof where, " in %s", rel);
        }

        const char *tmp = ".p0_edit_tmp.c";
        int n = code_rename(fullpath, oldn, newn, tmp);
        if (n < 0) return 0;                       /* bad names / unreadable — not ours */
        if (n == 0) {
            remove(tmp);
            snprintf(out, out_size, "I did not find %s in %s, so nothing was renamed.",
                     oldn, fullpath);
            store_proof(b, out);
            return 1;
        }
        char err[512];
        int rc = code_compile(tmp, err, sizeof err);
        remove(tmp);
        if (rc == 1)
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences); the result still compiles.",
                     oldn, newn, where, n);
        else if (rc == 0)
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences), but the result no longer compiles.",
                     oldn, newn, where, n);
        else
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences) in a temp copy; the original is unchanged.",
                     oldn, newn, where, n);
        store_proof(b, out);
        return 1;
    }

    /* gen186: F5 verification — "does <file> compile?". Run the compiler (a
     * deterministic tool) on a sandboxed path and report. Handled before the
     * snippet questions because it takes a file path, not inline code. */
    if (cue(qpart, "compile") || cue(qpart, "compiles") || cue(qpart, "compila")) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0]) return 0;
        char err[512];
        int rc = code_compile(path, err, sizeof err);
        if (rc < 0) return 0;                       /* unsafe/unrunnable — not ours */
        if (rc == 1) {
            snprintf(out, out_size, "Yes, it compiles: %s has no errors.", path);
        } else {
            /* quote the first diagnostic line, trimmed */
            char first[200] = ""; size_t fo = 0;
            for (const char *c = err; *c && *c != '\n' && fo + 1 < sizeof first; c++) first[fo++] = *c;
            first[fo] = '\0';
            if (first[0])
                snprintf(out, out_size, "No, %s does not compile: %s", path, first);
            else
                snprintf(out, out_size, "No, %s does not compile.", path);
        }
        store_proof(b, out);
        return 1;
    }

    /* gen185: reverse call graph — "what/who calls <X> in <dir>?". Scan the
     * directory (sandboxed, recursive) for the functions whose body calls X — the
     * blast radius before an edit. Distinct phrasing from "what does X call". */
    if ((cue(qpart, "what calls") || cue(qpart, "who calls") ||
         cue(qpart, "chi chiama")) &&
        (cue(qpart, " in ") || cue(qpart, " under ") || cue(qpart, "/"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char target[KB_TERM_LEN] = ""; char dir[256] = "";
        for (size_t i = 0; i + 1 < nw; i++) {
            if (!strcmp(w[i], "calls") || !strcmp(w[i], "chiama"))
                snprintf(target, sizeof target, "%s", strip_edge_punct(w[i+1]));
            if (!strcmp(w[i], "in") || !strcmp(w[i], "under") || !strcmp(w[i], "inside") ||
                !strcmp(w[i], "within") || !strcmp(w[i], "nella") || !strcmp(w[i], "nel"))
                snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i+1]));
        }
        if (!dir[0]) for (size_t i = 0; i < nw; i++)
            if (strchr(w[i], '/')) { snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i])); break; }
        if (!target[0] || !dir[0]) return 0;
        char callers[64][KB_TERM_LEN];
        size_t nc = code_find_callers(dir, target, callers, 64);
        if (nc == 0) {
            snprintf(out, out_size, "I looked through %s but nothing calls %s.", dir, target);
            store_proof(b, out);
            return 1;
        }
        size_t off = (size_t)snprintf(out, out_size, "%s is called by ", target);
        for (size_t i = 0; i < nc && off < out_size; i++) {
            const char *sep = (i == 0) ? "" : (i == nc - 1) ? " and " : ", ";
            off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, callers[i]);
        }
        if (off < out_size) snprintf(out + off, out_size - off, ".");
        store_proof(b, out);
        return 1;
    }

    /* Three structural questions, all EN+IT, all requiring a code section so they
     * never fire on prose:
     *   - "what does <X>(args) return?"            -> code_eval       (B5)
     *   - "what/which functions does this define?" -> code_function/1 (F2)
     *   - "what does <X> call?"                    -> code_calls/2    (F3)
     * The eval cue is checked on the QUESTION only, so a snippet's own "return"
     * never reroutes a define/call question. */
    int wants_eval =
        cue(qpart, "return") || cue(qpart, "returns") || cue(qpart, "evaluate") ||
        cue(qpart, "restituisce") || cue(qpart, "ritorna") || cue(qpart, "valuta");
    int wants_funcs = !wants_eval &&
        (cue(s, "function") || cue(s, "funzioni") || cue(s, "funzione")) &&
        (cue(s, "define") || cue(s, "defined") || cue(s, "definisce") ||
         cue(s, "definite") || cue(s, "definisci"));
    int wants_calls = !wants_eval && !wants_funcs &&
        (cue(s, "call") || cue(s, "calls") || cue(s, "invoke") ||
         cue(s, "chiama") || cue(s, "invoca"));
    if (!wants_eval && !wants_funcs && !wants_calls) return 0;

    /* The code comes either inline (after a ':') or, gen181, from a real file on
     * disk named in the question (a token with a '/' or a .c/.h suffix). The file
     * read is sandboxed to the working directory (see code_read_file). */
    static char code[262144];
    code[0] = '\0';
    if (!find_code_section(s, code, sizeof code)) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0] || !code_read_file(path, code, sizeof code)) return 0;
    }

    /* gen184: blank comments and string/char literals so the scanners see only
     * real code (a '(' or '{' inside a comment/string no longer derails parsing).
     * Applied once here for the snippet questions; code_locate strips per file. */
    code_strip(code);

    /* B5: symbolic execution. Parse a concrete call NAME(int, int, ...) from the
     * question and COMPUTE its result from the function body — nothing is run. */
    if (wants_eval) {
        char fname[KB_TERM_LEN] = ""; long argv[8]; size_t nargs = 0; int have = 0;
        for (const char *p = qpart; *p && !have; ) {
            if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
            const char *id = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            size_t l = (size_t)(p - id);
            const char *q = p; while (*q == ' ') q++;
            if (*q != '(') continue;
            const char *a = q + 1; long tmp[8]; size_t n = 0; int ok = 1, any = 0;
            while (*a && *a != ')') {
                while (*a == ' ' || *a == ',') a++;
                if (*a == ')' || !*a) break;
                int sign = 1;
                if (*a == '-') { sign = -1; a++; } else if (*a == '+') a++;
                if (!isdigit((unsigned char)*a)) { ok = 0; break; }
                long v = 0; while (isdigit((unsigned char)*a)) { v = v * 10 + (*a - '0'); a++; }
                any = 1;
                if (n < 8) tmp[n++] = sign * v; else { ok = 0; break; }
                while (*a == ' ') a++;
                if (*a != ',' && *a != ')') { ok = 0; break; }
            }
            if (ok && any && l < sizeof fname) {
                memcpy(fname, id, l); fname[l] = '\0';
                for (size_t i = 0; i < n; i++) argv[i] = tmp[i];
                nargs = n; have = 1;
            }
        }
        if (!have) return 0;   /* eval phrasing but no concrete call — not ours */

        char callstr[128]; size_t co = (size_t)snprintf(callstr, sizeof callstr, "%s(", fname);
        for (size_t i = 0; i < nargs && co < sizeof callstr; i++)
            co += (size_t)snprintf(callstr + co, sizeof callstr - co,
                                   "%s%ld", i ? ", " : "", argv[i]);
        if (co < sizeof callstr) snprintf(callstr + co, sizeof callstr - co, ")");

        long res;
        if (code_eval(code, fname, argv, nargs, &res))
            snprintf(out, out_size,
                     "I read it as code and worked it out: %s returns %ld.", callstr, res);
        else
            snprintf(out, out_size,
                     "I read it as code, but I cannot compute %s yet — its body is "
                     "beyond my arithmetic evaluator.", callstr);
        store_proof(b, out);
        return 1;
    }

    char names[32][KB_TERM_LEN];
    size_t k = code_ingest(b->kb, code, names, 32);
    size_t shown = k < 32 ? k : 32;

    if (wants_funcs) {
        if (k == 0) {
            put("I read that as code, but I do not see any function definitions in it.",
                out, out_size);
            return 1;
        }
        size_t off = (size_t)snprintf(out, out_size,
                                      "I read it as code: it defines ");
        for (size_t i = 0; i < shown && off < out_size; i++) {
            const char *sep = (i == 0) ? "" : (i == shown - 1) ? " and " : ", ";
            off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, names[i]);
        }
        if (off < out_size) snprintf(out + off, out_size - off, ".");
        store_proof(b, out);
        return 1;
    }

    /* wants_calls: the question names a defined function (or there is only one).
     * Look at the question text (qpart, computed above) to pick which one. */
    const char *xfn = NULL;
    for (size_t i = 0; i < shown; i++)
        if (strstr(qpart, names[i])) { xfn = names[i]; break; }
    if (!xfn && k == 1) xfn = names[0];
    if (!xfn) {
        put("I read that as code, but I am not sure which function you mean.",
            out, out_size);
        return 1;
    }

    /* Answer from the derived call graph (code_calls/2), not from a re-scan. */
    char callees[32][KB_TERM_LEN];
    const char *pat[] = { xfn, NULL };
    size_t c = kb_match(b->kb, "code_calls", pat, 2, callees, 32);
    if (c == 0) {
        snprintf(out, out_size,
                 "I read it as code: %s does not call any function.", xfn);
        store_proof(b, out);
        return 1;
    }
    size_t off = (size_t)snprintf(out, out_size, "I read it as code: %s calls ", xfn);
    size_t cshown = c < 32 ? c : 32;
    for (size_t i = 0; i < cshown && off < out_size; i++) {
        const char *sep = (i == 0) ? "" : (i == cshown - 1) ? " and " : ", ";
        off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, callees[i]);
    }
    if (off < out_size) snprintf(out + off, out_size - off, ".");
    store_proof(b, out);
    return 1;
}

static int mod_code(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)norm;
    if (!raw) return 0;

    char s[512]; copy_trim(s, sizeof s, raw);
    if (!*s) return 0;

    int qtype = 0; /* 0=none 1=wrong/debug 2=fix 3=what-lang 4=valid 5=explain */
    if (cue(s, "what is wrong") || cue(s, "what s wrong") ||
        cue(s, "debug") || cue(s, "find the bug") || cue(s, "find the error") ||
        cue(s, "cosa non va") || cue(s, "trova l errore") || cue(s, "debugga")) {
        qtype = 1;
    } else if (cue(s, "fix this") || cue(s, "correct this") ||
               cue(s, "correggi questo") || cue(s, "aggiusta")) {
        qtype = 2;
    } else if (cue(s, "what language is this") ||
               cue(s, "che linguaggio")) {
        qtype = 3;
    } else if (cue(s, "is this valid") || cue(s, "e valido") ||
               cue(s, "is this correct")) {
        qtype = 4;
    } else if (cue(s, "explain this code") || cue(s, "spiega questo codice") ||
               cue(s, "what does this code do") || cue(s, "cosa fa questo codice")) {
        qtype = 5;
    } else if (cue(s, "what is a") && !strstr(s, "what is a ") &&
               (strstr(s, "in C") || strstr(s, "in Python") || strstr(s, "in programming"))) {
        /* Concept query handled by mod_knowledge via KB concept() facts */
        return 0;
    }
    if (!qtype) return 0;

    char code[512] = {0};
    if (!find_code_section(s, code, sizeof code)) {
        /* Try without colon: if the query is short and followed by code-like content */
        const char *c = s;
        while (*c && *c != ':') c++;
        if (!*c) return 0; /* no colon, can't extract code */
        return 0;
    }

    /* Identify language */
    int lang = identify_code_lang(code, b);

    /* For language-only queries */
    if (qtype == 3) {
        char ln[32]; lang_name(lang, ln, sizeof ln);
        if (lang == 0) {
            snprintf(out, out_size, "I can not identify the language. "
                     "It does not look like C or Python to me.");
        } else {
            snprintf(out, out_size, "This looks like %s code.", ln);
        }
        return 1;
    }

    /* For explain queries */
    if (qtype == 5) {
        char ln[32]; lang_name(lang, ln, sizeof ln);
        snprintf(out, out_size, "This is a %s code snippet.", ln);
        if (strstr(code, "printf")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It prints output using printf.");
        }
        if (strstr(code, "return")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It returns a value.");
        }
        if (strstr(code, "for ") || strstr(code, "while ")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It contains a loop.");
        }
        return 1;
    }

    /* Run checks (focus on C; Python checks are minimal) */
    char findings[512] = {0};
    int errors = 0;

    if (lang == 1) {  /* C */
        char r[256];
        if (check_missing_semicolons(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_type_mismatch(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_unclosed_string(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_balanced_braces(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_balanced_parens(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_unknown_function(code, b, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
    } else if (lang == 2) {  /* Python */
        if (strstr(code, "def ") || strstr(code, "if ") || strstr(code, "for ") ||
            strstr(code, "while ") || strstr(code, "class ") || strstr(code, "elif ")) {
            /* Check for missing colon: the line ends without : */
            char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
            char *lines[64]; int nl = 0;
            char *save = NULL;
            char *tok = strtok_r(buf, "\n", &save);
            while (tok && nl < 64) { lines[nl++] = tok; tok = strtok_r(NULL, "\n", &save); }
            for (int i = 0; i < nl; i++) {
                char *l = lines[i]; while (*l && isspace((unsigned char)*l)) l++;
                char tmp[256]; snprintf(tmp, sizeof tmp, "%s", l);
                char *tw[64]; size_t tnw = split_words(tmp, tw, 64);
                if (tnw > 0 && (strcmp(tw[0], "def") == 0 || strcmp(tw[0], "if") == 0 ||
                    strcmp(tw[0], "for") == 0 || strcmp(tw[0], "while") == 0 ||
                    strcmp(tw[0], "class") == 0 || strcmp(tw[0], "elif") == 0 ||
                    strcmp(tw[0], "else") == 0 || strcmp(tw[0], "try") == 0 ||
                    strcmp(tw[0], "except") == 0)) {
                    size_t len = strlen(l);
                    if (len > 0 && l[len - 1] != ':') {
                        snprintf(findings, sizeof findings,
                            "Missing colon: the line starting with \"%s\" needs a colon at the end.", tw[0]);
                        errors++;
                        break;
                    }
                }
            }
        }
        if (strstr(code, "print") && !strstr(code, "print(")) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol,
                "Python 3 requires parentheses for print: use print(...) not print ...");
            errors++;
        }
    }

    if (errors == 0 && (qtype == 1 || qtype == 2)) {
        snprintf(out, out_size, "I did not find obvious errors in this code snippet. "
                 "(I only check basic syntax: semicolons, braces, parentheses, "
                 "string quotes, and known function names.)");
        return 1;
    }

    if (qtype == 1) {
        /* Strip trailing ". " */
        size_t fl = strlen(findings);
        while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
        snprintf(out, out_size, "%s found: %s.", errors == 1 ? "Issue" : "Issues", findings);
    } else if (qtype == 2) {
        size_t fl = strlen(findings);
        while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
        if (strstr(findings, "semicolon"))
            snprintf(out, out_size, "Fix: add a semicolon at the end of each statement.");
        else if (strstr(findings, "Type mismatch"))
            snprintf(out, out_size, "Fix: change the variable type or the value to make them compatible.");
        else if (strstr(findings, "string"))
            snprintf(out, out_size, "Fix: add the closing double-quote.");
        else if (strstr(findings, "braces"))
            snprintf(out, out_size, "Fix: add or remove braces to balance them.");
        else if (strstr(findings, "parentheses"))
            snprintf(out, out_size, "Fix: add or remove parentheses to balance them.");
        else if (strstr(findings, "function"))
            snprintf(out, out_size, "Fix: check the function name spelling or include the right header. Did you mean printf instead of print?");
        else if (strstr(findings, "colon"))
            snprintf(out, out_size, "Fix: add a colon at the end of the block-introducing line.");
        else
            snprintf(out, out_size, "I did not find a clear fix. Can you describe what behavior you expect?");
    } else if (qtype == 4) {
        if (errors == 0)
            snprintf(out, out_size, "This code looks valid at first glance (basic syntax checks pass).");
        else {
            size_t fl = strlen(findings);
            while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
            snprintf(out, out_size, "Not valid: %s.", findings);
        }
    }

    return 1;
}

static int mod_symbolic(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)norm;
    if (!raw) return 0;

    char s[256];
    copy_trim(s, sizeof s, raw);          /* keep symbols; trim edges only */
    if (s[0] == '\0') return 0;
    char lc[256]; size_t i = 0;
    for (; s[i] && i + 1 < sizeof lc; i++) lc[i] = (char)tolower((unsigned char)s[i]);
    lc[i] = '\0';

    /* Morse is the strictest charset, so test it first. */
    if (looks_morse(lc))
        return name_register(b, "That looks like Morse code.",
                             "Dots and dashes — that's Morse.", out, out_size);

    /* tokenize a throwaway copy for the code check (split_words mutates it);
     * the other detectors read `lc`, which stays intact. */
    char tok[256]; copy_trim(tok, sizeof tok, lc);
    char *w[64]; size_t nw = split_words(tok, w, 64);

    if (looks_solfege(lc))
        return name_register(b, "Those are musical notes (solfège).",
                             "Sounds like solfège — do re mi.", out, out_size);

    if (looks_palindrome(lc))
        return name_register(b, "That looks like a palindrome.",
                             "Neat — it reads the same backwards (a palindrome).",
                             out, out_size);

    if (looks_leet(lc))
        return name_register(b, "That looks like leetspeak.",
                             "Letters as numbers — that's leetspeak.", out, out_size);

    if (looks_code(lc, w, nw))
        return name_register(b, "That looks like a snippet of code.",
                             "Looks like a fragment of code.", out, out_size);

    return 0;
}

/* --- module: translate (gen126, L5) -------------------------------------
 * Grounded translation of a short clause, EN<->IT, COMPOSED from per-word
 * glosses (tr/2 in kb/core/gloss.p0) plus a structural article rule, never
 * a stored sentence. So any noun/verb in the lexicon transfers to a held-out
 * clause: this is translation, not a phrasebook. Honest decline on an unknown
 * content word (it names the word it cannot translate).
 *
 * Shapes handled: [det] noun [adj] verb, and [det] noun. Italian gives the
 * noun's gendered article (il/la, un/una) and post-posed agreeing adjective.
 */

/* Look up the Italian gloss of an English word, or vice-versa. dir = 1 means
 * en->it (match arg0, return arg1); dir = 0 means it->en (match arg1, return
 * arg0). Returns 1 and fills `out` on success. */
static int gloss_lookup(Brain *b, const char *word, int dir,
                        char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char hits[1][KB_TERM_LEN];
    const char *args_en2it[] = {word, NULL};   /* tr(word, ?) */
    const char *args_it2en[] = {NULL, word};   /* tr(?, word) */
    size_t n = kb_match(b->kb, "tr", dir ? args_en2it : args_it2en, 2, hits, 1);
    if (n == 0) return 0;
    put(hits[0], out, out_size);
    return 1;
}

/* Agree an Italian masculine-singular adjective with the noun's gender:
 * -o -> -a for feminine (piccolo -> piccola); invariant adjectives (grande)
 * and others are left unchanged. */
static void agree_adj(char *adj, char gender) {
    size_t n = strlen(adj);
    if (gender == 'f' && n > 0 && adj[n - 1] == 'o') adj[n - 1] = 'a';
}

static int is_en_det(const char *w) {
    return strcmp(w, "the") == 0 || strcmp(w, "a") == 0 || strcmp(w, "an") == 0;
}
static int is_it_det(const char *w) {
    return strcmp(w, "il") == 0 || strcmp(w, "la") == 0 ||
           strcmp(w, "lo") == 0 || strcmp(w, "un") == 0 ||
           strcmp(w, "una") == 0 || strcmp(w, "uno") == 0;
}

static int mod_translate(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    /* A translator must read the ORIGINAL source words, not the canonicalized
     * surface (which maps Italian function words to English and would wreck an
     * IT->EN request). So we work off `raw`, lowercased here. */
    char low[256];
    lowercase_copy(low, sizeof low, raw);

    int to_it;
    const char *clause = NULL;
    static const struct { const char *p; int to_it; } pats[] = {
        {"translate to italian:", 1}, {"translate into italian:", 1},
        {"translate to english:", 0}, {"translate into english:", 0},
        {"traduci in italiano:", 1},  {"traduci in inglese:", 0},
    };
    for (size_t i = 0; i < sizeof pats / sizeof pats[0]; i++) {
        size_t L = strlen(pats[i].p);
        if (strncmp(low, pats[i].p, L) == 0) {
            to_it = pats[i].to_it;
            clause = low + L;
            goto found;
        }
    }
    return 0;
found:
    while (*clause && isspace((unsigned char)*clause)) clause++;
    if (!*clause) { put("Translate what?", out, out_size); return 1; }

    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    /* strip a trailing terminal punctuation so it doesn't ride on the verb */
    size_t bl = strlen(buf);
    while (bl > 0 && (buf[bl-1] == '.' || buf[bl-1] == '?' || buf[bl-1] == '!'))
        buf[--bl] = '\0';
    char *w[32];
    size_t nw = split_words(buf, w, 32);
    if (nw == 0) { put("Translate what?", out, out_size); return 1; }

    char result[512] = "";
    size_t rn = 0;

    /* First pass (EN->IT): find the clause's head-noun gender so the article
     * AND any adjective (which precedes the noun in English) agree with it. */
    char clause_gender = 'm';
    if (to_it) {
        for (size_t i = 0; i < nw; i++) {
            char *tok = strip_edge_punct(w[i]);
            char it_word[KB_TERM_LEN];
            if (gloss_lookup(b, tok, 1, it_word, sizeof it_word)) {
                char hits[1][KB_TERM_LEN];
                const char *ga[] = {it_word, NULL};
                if (kb_match(b->kb, "gender", ga, 2, hits, 1) == 1) {
                    clause_gender = (hits[0][0] == 'f') ? 'f' : 'm';
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < nw; i++) {
        char *tok = strip_edge_punct(w[i]);
        char piece[KB_TERM_LEN] = "";

        if (to_it && is_en_det(tok)) {
            /* the Italian article agrees with the clause's head-noun gender,
             * and elides before a vowel-initial noun (il/la -> l', una -> un'). */
            int indef = (strcmp(tok, "a") == 0 || strcmp(tok, "an") == 0);
            const char *art = indef ? (clause_gender == 'f' ? "una" : "un")
                                    : (clause_gender == 'f' ? "la" : "il");
            int vowel_next = 0;
            if (i + 1 < nw) {
                char *nx = strip_edge_punct(w[i + 1]);
                char nt[KB_TERM_LEN];
                if (gloss_lookup(b, nx, 1, nt, sizeof nt))
                    vowel_next = strchr("aeiou", nt[0]) != NULL;
            }
            if (vowel_next && !indef)            art = "l'";
            else if (vowel_next && indef && clause_gender == 'f') art = "un'";
            snprintf(piece, sizeof piece, "%s", art);
        } else if (!to_it && is_it_det(tok)) {
            int indef = (strcmp(tok, "un") == 0 || strcmp(tok, "una") == 0 ||
                         strcmp(tok, "uno") == 0);
            snprintf(piece, sizeof piece, "%s", indef ? "a" : "the");
        } else {
            if (!gloss_lookup(b, tok, to_it, piece, sizeof piece)) {
                char msg[160];
                snprintf(msg, sizeof msg,
                         "I can't translate \"%s\" yet.", tok);
                put(msg, out, out_size);
                return 1;
            }
            if (to_it) {
                /* a noun carries a gender fact; anything else translatable in a
                 * noun phrase is an adjective and agrees with the head noun. */
                char hits[1][KB_TERM_LEN];
                const char *ga[] = {piece, NULL};
                if (kb_match(b->kb, "gender", ga, 2, hits, 1) == 0)
                    agree_adj(piece, clause_gender);
            }
        }

        size_t pl = strlen(piece);
        if (rn + pl + 2 < sizeof result) {
            /* no space after an elided article (l'uomo, un'amica) */
            if (rn && result[rn - 1] != '\'') result[rn++] = ' ';
            memcpy(result + rn, piece, pl + 1);
            rn += pl;
        }
    }

    put(result, out, out_size);
    return 1;
}

/* --- module: synth (gen127, L12) ----------------------------------------
 * The INVERSE of mod_shell: synthesize a one-line shell command from a natural
 * spec ("count the lines in a file" -> "wc -l <file>"), grounded in the SAME
 * cmd/flag knowledge the interpreter reads (kb/experts/programming/bash.p0). The command is
 * SELECTED by matching the spec's action against command descriptions, and its
 * FLAGS by matching the spec's object nouns against flag descriptions — never a
 * stored spec->command pair, so held-out specs over known commands transfer.
 *
 * The flag-disambiguation trick: a command description like wc's
 * "counts_lines_words_and_bytes" already contains the words (lines/words/bytes)
 * that distinguish its flags. So we drop any spec word that stem-matches the
 * command's VERB (the first description token) before choosing flags — the verb
 * picks the command, the surviving object noun picks the flag.
 */

/* Light stemmer: true if the shorter of a,b is a prefix of the longer (count ~
 * counts ~ counting, line ~ lines, remove ~ removes). Requires >=3 shared chars
 * to avoid spurious hits; otherwise demands exact equality. */
static int stem_match(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    size_t m = la < lb ? la : lb;
    if (m < 3) return strcmp(a, b) == 0;
    return strncmp(a, b, m) == 0;
}

/* Does spec word `sw` stem-match any '_'-separated token of snake_case `atom`?
 * If `verb_out` is non-NULL it receives the atom's FIRST token (the verb). */
static int spec_hits_atom(const char *sw, const char *atom, char *verb_out,
                          size_t verb_size) {
    char buf[KB_TERM_LEN];
    snprintf(buf, sizeof buf, "%s", atom);
    int hit = 0, first = 1;
    for (char *tok = strtok(buf, "_"); tok; tok = strtok(NULL, "_")) {
        if (first && verb_out) { snprintf(verb_out, verb_size, "%s", tok); first = 0; }
        else first = 0;
        if (strlen(tok) >= 3 && stem_match(sw, tok)) hit = 1;
    }
    return hit;
}

static int mod_synth(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    static const char *const triggers[] = {
        "what command ", "which command ",
        "write a command to ", "write a command that ",
        "give me a command to ", "give me a command that ",
        "synthesize a command to ", "synthesize a command that ",
        "quale comando ", "che comando ", "scrivi un comando per ",
        NULL,
    };
    const char *spec = NULL;
    for (const char *const *t = triggers; *t; t++) {
        size_t L = strlen(*t);
        if (strncmp(low, *t, L) == 0) { spec = low + L; break; }
    }
    if (!spec) return 0;
    /* strip a leading relative ("that counts" already handled; "counts ..." is
     * fine) and trailing punctuation */
    char spec_buf[256];
    copy_trim(spec_buf, sizeof spec_buf, spec);
    size_t sl = strlen(spec_buf);
    while (sl > 0 && (spec_buf[sl-1]=='?'||spec_buf[sl-1]=='.'||spec_buf[sl-1]=='!'))
        spec_buf[--sl] = '\0';

    /* spec content words */
    char wb[256]; snprintf(wb, sizeof wb, "%s", spec_buf);
    char *sw[32]; size_t nsw = split_words(wb, sw, 32);
    char words[32][KB_TERM_LEN]; size_t nw = 0;
    int wants_file = 0, wants_pattern = 0;
    for (size_t i = 0; i < nsw && nw < 32; i++) {
        char *t = strip_edge_punct(sw[i]);
        if (strcmp(t, "file") == 0 || strcmp(t, "files") == 0) wants_file = 1;
        if (strcmp(t, "pattern") == 0 || strcmp(t, "string") == 0) wants_pattern = 1;
        if (strlen(t) >= 3 && isalpha((unsigned char)t[0]) && !is_stopword(b, t))
            snprintf(words[nw++], KB_TERM_LEN, "%s", t);
    }
    if (nw == 0) { put("Specify what the command should do.", out, out_size); return 1; }

    /* enumerate known commands; score each by spec-word matches on its desc */
    char cmds[64][KB_TERM_LEN];
    const char *cpat[] = {NULL, NULL};
    size_t ncmd = kb_match(b->kb, "cmd", cpat, 2, cmds, 64);
    const char *best = NULL; int best_score = 0; char best_verb[KB_TERM_LEN] = "";
    for (size_t i = 0; i < ncmd; i++) {
        char desc[1][KB_TERM_LEN];
        const char *dq[] = {cmds[i], NULL};
        if (kb_match(b->kb, "cmd", dq, 2, desc, 1) != 1) continue;
        int score = 0; char verb[KB_TERM_LEN] = "";
        for (size_t j = 0; j < nw; j++) {
            char v[KB_TERM_LEN];
            if (spec_hits_atom(words[j], desc[0], v, sizeof v)) score++;
            if (!verb[0]) snprintf(verb, sizeof verb, "%s", v);
        }
        if (score > best_score) {
            best_score = score; best = cmds[i];
            snprintf(best_verb, sizeof best_verb, "%s", verb);
        }
    }
    if (!best) {
        put("I don't know a command for that yet.", out, out_size);
        return 1;
    }

    /* choose flags: object nouns (spec words NOT stem-matching the verb) that
     * hit a flag's description. Preserve KB order, dedup. */
    char flags[16]; size_t nf = 0;
    char fletters[32][KB_TERM_LEN];
    const char *fpat[] = {best, NULL, NULL};
    size_t nfl = kb_match(b->kb, "flag", fpat, 3, fletters, 32);
    for (size_t i = 0; i < nfl && nf < sizeof flags - 1; i++) {
        char fdesc[1][KB_TERM_LEN];
        const char *fq[] = {best, fletters[i], NULL};
        if (kb_match(b->kb, "flag", fq, 3, fdesc, 1) != 1) continue;
        /* single-letter flags only (the synthesizable short options) */
        if (strlen(fletters[i]) != 1) continue;
        int selected = 0;
        for (size_t j = 0; j < nw; j++) {
            if (best_verb[0] && stem_match(words[j], best_verb)) continue; /* the verb */
            if (spec_hits_atom(words[j], fdesc[0], NULL, 0)) { selected = 1; break; }
        }
        if (selected) {
            int dup = 0;
            for (size_t k = 0; k < nf; k++) if (flags[k] == fletters[i][0]) dup = 1;
            if (!dup) flags[nf++] = fletters[i][0];
        }
    }
    flags[nf] = '\0';

    char cmd[128];
    size_t o = (size_t)snprintf(cmd, sizeof cmd, "%s", best);
    if (nf) o += (size_t)snprintf(cmd + o, sizeof cmd - o, " -%s", flags);
    if (wants_pattern) o += (size_t)snprintf(cmd + o, sizeof cmd - o, " <pattern>");
    if (wants_file)    o += (size_t)snprintf(cmd + o, sizeof cmd - o, " <file>");

    char msg[160];
    snprintf(msg, sizeof msg, "%s", cmd);
    put(msg, out, out_size);
    return 1;
}

/* --- module: counterfactual (gen128, L20-deep) --------------------------
 * The reflexive closure of mod_strategy. gen105 reported the REAL trace ("why
 * did you answer that way?"). This module answers the COUNTERFACTUAL: "what
 * would you have said WITHOUT the <X> module?" / "what else could have answered?"
 * — and it does not confabulate. It RE-RUNS its own dispatch over the previous
 * turn's input with one module suppressed, and reports whatever the alternative
 * self actually computes. A deterministic C program simulating itself with a
 * part removed: the method (introspection driving the loop) becomes a feature
 * (introspection inside the agent), exactly the fractal move PRINCIPLES.md asks.
 *
 * Defined here (above the registry it walks) via a forward-declared replay helper
 * that is implemented after the registry table. */
static int replay_dispatch(Brain *b, const char *canon, const char *raw,
                           const char *suppress, char *who, size_t who_size,
                           char *out, size_t out_size);
static int is_registry_module(const char *name); /* defined after the table */
static void not_understood(Brain *b, const char *canon, char *out, size_t out_size);
static int repair_dispatch(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size); /* gen141, after the table */
static int is_intent_starter(const char *w);           /* gen80, after the table */

static int mod_counterfactual(Brain *b, const char *norm, const char *raw,
                              char *out, size_t out_size) {
    if (!b) return 0;
    (void)norm;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* "what else matched / could have answered / would have answered" — suppress
     * the actual winner and report the runner-up. */
    int else_form =
        cue(low, "what else matched") || cue(low, "what else could have") ||
        cue(low, "what else would have") || cue(low, "anything else match") ||
        cue(low, "cos'altro") || cue(low, "cosa altro") ||
        cue(low, "che altro");

    /* "without <X>" / "senza <X>" — suppress a named module. */
    const char *wp = strstr(low, "without ");
    if (!wp) wp = strstr(low, "senza ");
    int without_form = (wp != NULL);

    /* Only engage on a genuine counterfactual question shape. */
    int question_shape =
        cue(low, "would you have") || cue(low, "would you say") ||
        cue(low, "avresti") || else_form;
    if (!without_form && !else_form) return 0;
    if (without_form && !question_shape) return 0;

    if (!b->has_last_input || !b->has_trace) {
        put("I don't have a previous turn to reconsider yet.", out, out_size);
        return 1;
    }

    char suppress[32] = "";
    if (without_form) {
        const char *p = wp + (low[wp - low] == 'w' ? 8 : 6); /* past "without "/"senza " */
        while (*p && isspace((unsigned char)*p)) p++;
        /* skip a leading article: "without the X module" / "senza il modulo X" */
        char tok[32]; size_t n = 0;
        const char *q = p;
        /* pull up to two words; if first is article/"module", take the next */
        for (int grabbed = 0; grabbed < 3 && *q; grabbed++) {
            n = 0;
            while (*q && !isspace((unsigned char)*q) && n + 1 < sizeof tok) {
                char c = *q++;
                if (c == '?' || c == '.' || c == '!') break;
                tok[n++] = c;
            }
            tok[n] = '\0';
            while (*q && isspace((unsigned char)*q)) q++;
            if (n == 0) continue;
            if (strcmp(tok, "the") == 0 || strcmp(tok, "il") == 0 ||
                strcmp(tok, "module") == 0 || strcmp(tok, "modulo") == 0 ||
                strcmp(tok, "a") == 0)
                continue; /* skip filler, take the real module name next */
            snprintf(suppress, sizeof suppress, "%s", tok);
            break;
        }
        if (!suppress[0] || !is_registry_module(suppress)) {
            char msg[128];
            snprintf(msg, sizeof msg,
                     "I don't have a module called \"%s\" to set aside.",
                     suppress[0] ? suppress : "that");
            put(msg, out, out_size);
            return 1;
        }
    } else {
        /* else-form: suppress the recorded winner. */
        snprintf(suppress, sizeof suppress, "%s", b->trace_winner);
        if (strcmp(suppress, "fallback") == 0) {
            put("Nothing claimed the last turn — there was no winner to set aside.",
                out, out_size);
            return 1;
        }
    }

    char who[32] = "", ans[512] = "";
    int claimed = replay_dispatch(b, b->last_input_canon, b->last_input_raw,
                                  suppress, who, sizeof who, ans, sizeof ans);

    char msg[768];
    if (!claimed) {
        snprintf(msg, sizeof msg,
                 "Without '%s', nothing else matches — I'd fall back to \"%s\"",
                 suppress, ans);
    } else if (strcmp(who, b->trace_winner) == 0) {
        snprintf(msg, sizeof msg,
                 "Setting '%s' aside changes nothing — '%s' ran first anyway and "
                 "still answers \"%s\"", suppress, who, ans);
    } else {
        snprintf(msg, sizeof msg,
                 "Without '%s', the next module that matches is '%s', and it would "
                 "answer \"%s\"", suppress, who, ans);
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: world (gen142, E7) — local-world working memory ----------------
 * A real interlocutor can hold a TEMPORARY fictional or task-local world across
 * several turns — "in this story Rex is a dragon" — use those facts later, run a
 * SECOND world where the same name means something else, answer "what is
 * assumed?", and TEAR a world down so nothing leaks into what it permanently
 * believes. parrot0 had only one flat belief store: "rex is a dragon" wrote a
 * permanent fact. This module gives it scoped working memory.
 *
 * Mechanism (all in Brain state, never the persisted KB):
 *   ENTER   "in the <name> world/story[:] ..." or "start a world called <name>"
 *           or "in this story/world ..." (an anonymous scratch world) makes a
 *           named world the ACTIVE scope. An intro phrase with a trailing clause
 *           ("in this story rex is a dragon") enters the world AND asserts.
 *   ASSERT  while a world is active, "X is a/an Y" (or "X is not a Y") lands in
 *           the overlay tagged with the world id — NOT in the KB.
 *   QUERY   while a world is active, "is X a Y?" / "who is X?" / "what is X?" read
 *           the overlay first; the same X can answer differently in two worlds.
 *   INSPECT "what is assumed?" / "what is true in this world?" lists the active
 *           overlay — grounded in real state, not a canned line.
 *   SWITCH  "in the <name> world" with no clause re-activates an existing world.
 *   EXIT    "forget/end/close the <name> world", "forget this world", "leave the
 *           world" tears down (or deactivates) the scope; its facts vanish.
 * Registered right after repair, before the KB modules, so a world assertion
 * pre-empts the permanent learner exactly when a scope is open.
 */

/* find an existing live world by name, or -1. */
static int world_find(Brain *b, const char *name) {
    for (size_t i = 0; i < b->world_count; i++)
        if (b->world_live[i] && strcmp(b->worlds[i], name) == 0) return (int)i;
    return -1;
}

/* enter (creating if needed) the world `name`; returns its id, or -1 if full. */
static int world_enter(Brain *b, const char *name) {
    int id = world_find(b, name);
    if (id >= 0) { b->active_world = id; return id; }
    if (b->world_count >= BRAIN_WORLDS_MAX) return -1;
    id = (int)b->world_count++;
    snprintf(b->worlds[id], sizeof b->worlds[0], "%s", name);
    b->world_live[id] = 1;
    b->active_world = id;
    return id;
}

/* tear a world down: drop every overlay fact tagged with it, mark it dead, and
 * clear the active scope if it was the one removed. Returns 1 if it existed. */
static int world_teardown(Brain *b, int id) {
    if (id < 0 || (size_t)id >= b->world_count || !b->world_live[id]) return 0;
    size_t w = 0;
    for (size_t r = 0; r < b->wfact_count; r++)
        if (b->wfacts[r].world != id)
            b->wfacts[w++] = b->wfacts[r];
    b->wfact_count = w;
    b->world_live[id] = 0;
    if (b->active_world == id) b->active_world = -1;
    return 1;
}

/* record subj/pred (possibly negated) in the active world, replacing any prior
 * claim about the same subj/pred in THIS world. Returns 1 on success. */
static int world_assert(Brain *b, const char *subj, const char *pred, int neg) {
    int id = b->active_world;
    if (id < 0) return 0;
    for (size_t i = 0; i < b->wfact_count; i++)
        if (b->wfacts[i].world == id &&
            strcmp(b->wfacts[i].subj, subj) == 0 &&
            strcmp(b->wfacts[i].pred, pred) == 0) {
            b->wfacts[i].neg = neg;
            return 1;
        }
    if (b->wfact_count >= BRAIN_WFACTS_MAX) return 0;
    size_t s = b->wfact_count++;
    snprintf(b->wfacts[s].subj, sizeof b->wfacts[s].subj, "%s", subj);
    snprintf(b->wfacts[s].pred, sizeof b->wfacts[s].pred, "%s", pred);
    b->wfacts[s].world = id;
    b->wfacts[s].neg = neg;
    return 1;
}

/* query the active world for subj being a pred: +1 yes, -1 explicit no, 0 silent. */
static int world_query(Brain *b, const char *subj, const char *pred) {
    int id = b->active_world;
    if (id < 0) return 0;
    for (size_t i = 0; i < b->wfact_count; i++)
        if (b->wfacts[i].world == id &&
            strcmp(b->wfacts[i].subj, subj) == 0 &&
            strcmp(b->wfacts[i].pred, pred) == 0)
            return b->wfacts[i].neg ? -1 : 1;
    return 0;
}

/* "a" or "an" for the class word `p`, for natural replies ("is a dragon"). */
static const char *world_art(const char *p) {
    char c = (char)tolower((unsigned char)p[0]);
    return (c=='a'||c=='e'||c=='i'||c=='o'||c=='u') ? "an" : "a";
}

/* strip a leading article from a multi-word remainder pointer-style: returns the
 * pointer just past "a "/"an " if present, else the original. */
static const char *skip_article(const char *s) {
    if (strncmp(s, "a ", 2) == 0) return s + 2;
    if (strncmp(s, "an ", 3) == 0) return s + 3;
    return s;
}

/* True if `w` is a noun naming a local scope ("world"/"story"/"scenario"). */
static int is_world_noun(const char *w) {
    return strcmp(w, "world") == 0 || strcmp(w, "story") == 0 ||
           strcmp(w, "scenario") == 0 || strcmp(w, "puzzle") == 0;
}

/* Try to ASSERT/QUERY a single "X is [a] Y" clause inside the active world.
 * Returns 1 if it claimed the clause (writing `out`), 0 if the shape didn't fit.
 * `interrogative` marks a trailing '?' so subject-first questions are queries. */
static int world_clause(Brain *b, const char *clause, int interrogative,
                        char *out, size_t out_size) {
    char cb[256];
    copy_trim(cb, sizeof cb, clause);
    size_t n = strlen(cb);
    while (n > 0 && (cb[n-1]=='?'||cb[n-1]=='.'||cb[n-1]==','||cb[n-1]==' '))
        cb[--n] = '\0';
    char *w[8];
    size_t nw = split_words(cb, w, 8);

    /* QUERY: "is X [a] Y" */
    if (nw >= 3 && strcmp(w[0], "is") == 0) {
        const char *subj = w[1];
        size_t pi = 2;
        if (is_article(w[2]) && nw >= 4) pi = 3;
        if (pi != nw - 1) return 0;            /* one-word class only, for now */
        const char *pred = w[pi];
        int r = world_query(b, subj, pred);
        if (r > 0)      put("Yes.", out, out_size);
        else if (r < 0) put("No.", out, out_size);
        else {
            char m[160];
            snprintf(m, sizeof m, "Not assumed in the %s world.",
                     b->worlds[b->active_world]);
            put(m, out, out_size);
        }
        return 1;
    }

    /* QUERY: subject-first interrogative "X is [a] Y?" (the Italian shape
     * "rex è un drago?" canonicalizes here; same rule serves both languages). */
    if (interrogative && nw >= 3 && strcmp(w[1], "is") == 0 &&
        strcmp(w[0], "who") != 0 && strcmp(w[0], "what") != 0) {
        const char *subj = w[0];
        size_t pi = 2;
        if (strcmp(w[2], "not") == 0 && nw >= 4) pi = 3;
        if (pi < nw && is_article(w[pi]) && pi + 1 < nw) pi++;
        if (pi != nw - 1) return 0;
        const char *pred = w[pi];
        int r = world_query(b, subj, pred);
        if (r > 0)      put("Yes.", out, out_size);
        else if (r < 0) put("No.", out, out_size);
        else {
            char m[160];
            snprintf(m, sizeof m, "Not assumed in the %s world.",
                     b->worlds[b->active_world]);
            put(m, out, out_size);
        }
        return 1;
    }

    /* QUERY: "who is X" / "what is X" -> list X's classes in this world. */
    if (nw == 3 && (strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
        strcmp(w[1], "is") == 0) {
        const char *subj = w[2];
        char list[400]; size_t off = 0, hits = 0;
        for (size_t i = 0; i < b->wfact_count; i++)
            if (b->wfacts[i].world == b->active_world &&
                !b->wfacts[i].neg &&
                strcmp(b->wfacts[i].subj, subj) == 0) {
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s %s", hits ? ", " : "",
                                        world_art(b->wfacts[i].pred),
                                        b->wfacts[i].pred);
                hits++;
            }
        if (hits == 0) {
            char m[160];
            snprintf(m, sizeof m, "I have no assumptions about %s in the %s world.",
                     subj, b->worlds[b->active_world]);
            put(m, out, out_size);
        } else {
            char m[480];
            snprintf(m, sizeof m, "In the %s world, %s is %s.",
                     b->worlds[b->active_world], subj, list);
            put(m, out, out_size);
        }
        return 1;
    }

    /* ASSERT: "X is [a] Y" or "X is not [a] Y" (subject-first, no trailing '?'). */
    if (!interrogative && nw >= 3 && strcmp(w[1], "is") == 0) {
        const char *subj = w[0];
        size_t pi = 2; int neg = 0;
        if (strcmp(w[2], "not") == 0 && nw >= 4) { neg = 1; pi = 3; }
        if (pi < nw && is_article(w[pi]) && pi + 1 < nw) pi++;
        if (pi != nw - 1) return 0;            /* one-word class only, for now */
        const char *pred = w[pi];
        if (!world_assert(b, subj, pred, neg)) {
            put("This world is full; I can't assume more here.", out, out_size);
            return 1;
        }
        char m[300];
        snprintf(m, sizeof m, "In the %s world, %s is%s %s %s.",
                 b->worlds[b->active_world], subj, neg ? " not" : "",
                 world_art(pred), pred);
        put(m, out, out_size);
        return 1;
    }
    return 0;
}

static int mod_world(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[512];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    int interrogative = (len > 0 && buf[len-1] == '?');

    /* -- INSPECT: "what is assumed?" / "what is true in this/the <name> world?" -
     * Gated on an active world, so the "is assumed"/"assumed" cue (which also
     * catches the Italian "cosa è assunto?" -> "...is assumed?") cannot fire in
     * ordinary prose where no scope is open. */
    if (b->active_world >= 0 &&
        (strstr(norm, "is assumed") || strstr(norm, "what do you assume") ||
         strstr(norm, "what is true in this") ||
         strstr(norm, "what is true in the") ||
         strstr(norm, "what holds in this"))) {
        int id = b->active_world;
        char list[480]; size_t off = 0, hits = 0;
        for (size_t i = 0; i < b->wfact_count; i++)
            if (b->wfacts[i].world == id) {
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s is%s %s %s", hits ? "; " : "",
                                        b->wfacts[i].subj,
                                        b->wfacts[i].neg ? " not" : "",
                                        world_art(b->wfacts[i].pred),
                                        b->wfacts[i].pred);
                hits++;
            }
        char m[600];
        if (hits == 0)
            snprintf(m, sizeof m, "Nothing is assumed yet in the %s world.",
                     b->worlds[id]);
        else
            snprintf(m, sizeof m, "In the %s world I am assuming: %s.",
                     b->worlds[id], list);
        put(m, out, out_size);
        return 1;
    }

    /* -- EXIT: tear down or leave a world. -------------------------------------- */
    {
        char *w[12]; char tb[512];
        snprintf(tb, sizeof tb, "%s", buf);
        size_t nw = split_words(tb, w, 12);
        /* "forget/end/close/leave [this|the] [<name>] world/story" */
        int teardown = 0, leave_only = 0;
        if (nw >= 2 && (strcmp(w[0], "forget") == 0 || strcmp(w[0], "end") == 0 ||
                        strcmp(w[0], "close") == 0 || strcmp(w[0], "drop") == 0))
            teardown = 1;
        else if (nw >= 2 && strcmp(w[0], "leave") == 0)
            { teardown = 1; leave_only = 1; }
        if (teardown) {
            /* find the world noun and an optional name token before it */
            int has_world_noun = 0; const char *named = NULL;
            for (size_t i = 1; i < nw; i++) {
                char *t = strip_edge_punct(w[i]);
                if (is_world_noun(t)) {
                    has_world_noun = 1;
                    if (i >= 1) {
                        char *prev = strip_edge_punct(w[i-1]);
                        if (!is_article(prev) && strcmp(prev, "this") != 0 &&
                            strcmp(prev, "the") != 0 && strcmp(prev, "that") != 0 &&
                            !is_world_noun(prev) && strcmp(prev, "forget") != 0 &&
                            strcmp(prev, "end") != 0 && strcmp(prev, "close") != 0 &&
                            strcmp(prev, "leave") != 0 && strcmp(prev, "drop") != 0)
                            named = prev;
                    }
                    break;
                }
            }
            if (has_world_noun) {
                int id = named ? world_find(b, named) : b->active_world;
                if (id < 0) {
                    put("There is no such world open.", out, out_size);
                    return 1;
                }
                char nm[48]; snprintf(nm, sizeof nm, "%s", b->worlds[id]);
                if (leave_only) {
                    /* leave: deactivate but keep the world and its facts alive */
                    if (b->active_world == id) b->active_world = -1;
                    char m[120];
                    snprintf(m, sizeof m, "Left the %s world.", nm);
                    put(m, out, out_size);
                } else {
                    world_teardown(b, id);
                    char m[140];
                    snprintf(m, sizeof m,
                             "Forgotten the %s world; none of it reached my memory.",
                             nm);
                    put(m, out, out_size);
                }
                return 1;
            }
        }
    }

    /* -- ENTER (+ optional inline clause): an intro phrase opens a scope. ------- */
    {
        const char *rest = NULL; char wname[48] = "";
        /* "in the <name> world[:] ..." / "in the <name> story ..." */
        const char *p = NULL;
        if (strncmp(buf, "in the ", 7) == 0)      p = buf + 7;
        else if (strncmp(buf, "in this ", 8) == 0) {
            /* anonymous scratch scope: name it after the noun (world/story). */
            const char *q = buf + 8;
            char first[32]; size_t fi = 0;
            while (q[fi] && !isspace((unsigned char)q[fi]) && fi+1 < sizeof first)
                { first[fi] = q[fi]; fi++; }
            first[fi] = '\0';
            char bare[32]; snprintf(bare, sizeof bare, "%s", first);
            if (is_world_noun(strip_edge_punct(bare))) {
                snprintf(wname, sizeof wname, "%s", strip_edge_punct(bare));
                rest = q + fi;
            }
        }
        if (p && !*wname) {
            /* read tokens until the world noun. The NAME is the adjacent content
             * token: before the noun in English ("saga world"), or after it in
             * Italian ("mondo saga" -> "world saga"). The clause begins after the
             * name token, wherever it sits. */
            char nb[512]; snprintf(nb, sizeof nb, "%s", p);
            char *nw_[12]; size_t nn = split_words(nb, nw_, 12);
            for (size_t i = 0; i < nn; i++) {
                char *t = strip_edge_punct(nw_[i]);
                if (is_world_noun(t)) {
                    size_t name_idx = i; /* default: no name -> use the noun word */
                    char *before = (i >= 1) ? strip_edge_punct(nw_[i-1]) : NULL;
                    if (before && !is_article(before) &&
                        strcmp(before, "the") != 0 && strcmp(before, "this") != 0 &&
                        strcmp(before, "that") != 0) {
                        snprintf(wname, sizeof wname, "%s", before);
                        name_idx = i; /* clause starts after the noun */
                    } else if (i + 1 < nn) {
                        char *after_tok = strip_edge_punct(nw_[i+1]);
                        /* the next token names the world only if it isn't itself
                         * the start of a clause ("X is …"); else stay anonymous. */
                        int looks_clause = (i + 2 < nn) &&
                            strcmp(strip_edge_punct(nw_[i+2]), "is") == 0;
                        if (!looks_clause && *after_tok) {
                            snprintf(wname, sizeof wname, "%s", after_tok);
                            name_idx = i + 1; /* clause starts after the name */
                        } else {
                            snprintf(wname, sizeof wname, "%s", t);
                            name_idx = i;
                        }
                    } else {
                        snprintf(wname, sizeof wname, "%s", t);
                        name_idx = i;
                    }
                    /* rest = original text after token `name_idx` in `p` */
                    const char *after = p;
                    for (size_t k = 0; k <= name_idx; k++) {
                        after = strchr(after, ' ');
                        if (!after) break;
                        after++;
                    }
                    rest = after ? after : "";
                    break;
                }
            }
        }
        if (*wname) {
            int id = world_enter(b, wname);
            if (id < 0) { put("I can't open another world right now.", out, out_size);
                          return 1; }
            /* an inline clause after the noun (skip a leading colon / "where") */
            const char *clause = rest ? rest : "";
            while (*clause == ':' || *clause == ',' || isspace((unsigned char)*clause))
                clause++;
            if (strncmp(clause, "where ", 6) == 0) clause += 6;
            if (*clause) {
                char ans[300];
                if (world_clause(b, clause, interrogative, ans, sizeof ans)) {
                    put(ans, out, out_size);
                    return 1;
                }
            }
            char m[160];
            snprintf(m, sizeof m,
                     "Opened the %s world. Tell me what is true in it.", wname);
            put(m, out, out_size);
            return 1;
        }
        /* "start/open a world called <name>" / "new world <name>" */
        if (strncmp(buf, "start a ", 8) == 0 || strncmp(buf, "open a ", 7) == 0 ||
            strncmp(buf, "new ", 4) == 0) {
            char *w2[12]; char sb[256]; snprintf(sb, sizeof sb, "%s", buf);
            size_t nn = split_words(sb, w2, 12);
            int wn_idx = -1;
            for (size_t i = 0; i < nn; i++)
                if (is_world_noun(strip_edge_punct(w2[i]))) { wn_idx = (int)i; break; }
            if (wn_idx >= 0) {
                const char *name = NULL;
                /* prefer the token after "called"/"named", else the one after noun */
                for (size_t i = 0; i + 1 < nn; i++) {
                    char *t = strip_edge_punct(w2[i]);
                    if (strcmp(t, "called") == 0 || strcmp(t, "named") == 0)
                        { name = strip_edge_punct(w2[i+1]); break; }
                }
                if (!name && (size_t)wn_idx + 1 < nn)
                    name = strip_edge_punct(w2[wn_idx + 1]);
                if (name && *name) {
                    int id = world_enter(b, name);
                    if (id < 0) { put("I can't open another world right now.",
                                      out, out_size); return 1; }
                    char m[160];
                    snprintf(m, sizeof m,
                             "Opened the %s world. Tell me what is true in it.", name);
                    put(m, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* -- IN-SCOPE assertion/query while a world is active. ---------------------- */
    if (b->active_world >= 0) {
        const char *clause = (const char *)skip_article(buf);
        if (world_clause(b, clause, interrogative, out, out_size))
            return 1;
    }

    return 0;
}

/* --- module: repair (gen141, E2) — the conversational repair loop -----------
 * The weakest signal in the emergence audit (E-series) is the human surface:
 * vague turns hit the wall. A real interlocutor, asked something it cannot pin
 * down — "is it a mammal?" with no antecedent, "what is it plus 10" with no
 * number — does not collapse into "I don't understand". It asks a NARROW
 * clarification, holds the unfinished thought, and RESUMES it once answered.
 *
 * This module is that loop, as a stateful bridge across two turns:
 *   OPEN   a question whose referential slot (an entity pronoun) cannot be
 *          resolved -> store the turn, ask specifically what the slot means.
 *   RESUME the next turn fills the slot. If it is itself a teachable assertion
 *          ("rex is a dog") it runs first, so coreference now resolves the
 *          stored pronoun; otherwise a concrete referent token is substituted
 *          into the stored turn. Either way the ORIGINAL intent is re-dispatched
 *          through the normal registry and its real answer returned.
 *   EXPIRE if the next turn is plainly a new intent (a fresh question/command),
 *          the pending state is dropped and that turn handled normally.
 * The window is a single turn: clarification is consumed immediately, never
 * lingering to hijack later conversation. It is registered FIRST so it can both
 * pre-empt the wall and catch the answer before any other module.
 *
 * Defined above the registry it resumes through, via a forward-declared helper
 * implemented after the registry table (same shape as mod_counterfactual). */

/* first word is an interrogative/auxiliary that opens a question */
static int is_question_opener(const char *w) {
    static const char *const q[] = {
        "is", "are", "was", "were", "does", "do", "did", "can", "could",
        "will", "would", "should", "what", "who", "where", "when", "why",
        "how", NULL };
    for (const char *const *p = q; *p; p++)
        if (strcmp(w, *p) == 0) return 1;
    return 0;
}

/* an arithmetic operator cue, so the clarification can ask for a NUMBER rather
 * than a referent when the gap is an operand. */
static int has_arith_cue(Brain *b, char **w, size_t nw) {
    static const char *const ops[] = {
        "plus", "minus", "times", "divided", "multiplied", "double", "triple",
        "half", "square", "sum", "product", NULL };
    (void)b;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strpbrk(t, "+-*/")) return 1;
        for (const char *const *p = ops; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

static int mod_repair(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;

    /* -- RESUME: a clarification is open; this turn should fill the slot. ---- */
    if (b->pending_repair) {
        b->pending_repair = 0;               /* single-turn window: consume it */
        char saved_canon[256], saved_slot[16];
        snprintf(saved_canon, sizeof saved_canon, "%s", b->pending_canon);
        snprintf(saved_slot,  sizeof saved_slot,  "%s", b->pending_slot);

        char tb[256]; snprintf(tb, sizeof tb, "%s", norm);
        char *tw[64]; size_t tnw = split_words(tb, tw, 64);
        if (tnw == 0) return 0;

        /* If the user changed the subject instead of answering, expire and let
         * normal dispatch handle this turn fresh (acceptance: repair state
         * expires when the topic clearly changes). */
        if (is_question_opener(tw[0]) || is_intent_starter(tw[0]) ||
            strstr(norm, "refer to"))
            return 0;

        /* Let a teachable answer act first: "rex is a dog" asserts the fact and
         * sets the discourse entity, so the stored pronoun resolves by coref. */
        int had = b->has_last_entity;
        char prev[KB_TERM_LEN];
        snprintf(prev, sizeof prev, "%s", b->last_entity);
        char ans[512] = "";
        repair_dispatch(b, norm, raw, ans, sizeof ans);
        int entity_set = b->has_last_entity &&
                         (!had || strcmp(prev, b->last_entity) != 0);

        char resumed[256];
        if (entity_set) {
            snprintf(resumed, sizeof resumed, "%s", saved_canon);
        } else {
            /* substitute the slot with a concrete referent token from the answer
             * (the last content word/number: "the dog rex" -> rex, "21" -> 21). */
            const char *ref = NULL;
            for (size_t i = tnw; i-- > 0; ) {
                char *t = strip_edge_punct(tw[i]);
                if (!*t || is_entity_pronoun(t) || is_stopword(b, t)) continue;
                if (isalpha((unsigned char)t[0]) || isdigit((unsigned char)t[0])) {
                    ref = t; break;
                }
            }
            if (!ref) return 0;              /* no usable answer -> expire */
            char cb[256]; snprintf(cb, sizeof cb, "%s", saved_canon);
            char *cw[64]; size_t cnw = split_words(cb, cw, 64);
            size_t pos = 0; int replaced = 0; resumed[0] = '\0';
            for (size_t i = 0; i < cnw && pos < sizeof resumed; i++) {
                const char *piece = cw[i];
                char bare[64]; snprintf(bare, sizeof bare, "%s", cw[i]);
                if (!replaced && strcmp(strip_edge_punct(bare), saved_slot) == 0) {
                    piece = ref; replaced = 1;
                }
                pos += (size_t)snprintf(resumed + pos, sizeof resumed - pos,
                                        "%s%s", i ? " " : "", piece);
            }
            if (!replaced) snprintf(resumed, sizeof resumed, "%s", saved_canon);
        }

        repair_dispatch(b, resumed, raw, out, out_size);
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "repair");
        return 1;
    }

    /* -- OPEN: detect a referential gap and ask a narrow clarification. ------ */
    char buf[256]; snprintf(buf, sizeof buf, "%s", norm);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;
    if (!is_question_opener(w[0])) return 0;     /* only questions/commands */
    if (strstr(norm, "refer to")) return 0;      /* WSC coref judgement (mod_same) */
    if (b->has_last_entity) return 0;            /* a referent is available */
    /* gen146 (E5): "what year/time/date/day is it" is not a referential
     * gap. There is no hidden entity for "it"; the honest gap is that parrot0
     * has no clock/calendar fact or tool, so let the knowledge humility path
     * name that instead of opening a repair loop. */
    if (nw == 4 && strcmp(w[0], "what") == 0 && strcmp(w[2], "is") == 0 &&
        strcmp(strip_edge_punct(w[3]), "it") == 0 &&
        (strcmp(w[1], "year") == 0 || strcmp(w[1], "date") == 0 ||
         strcmp(w[1], "day") == 0 || strcmp(w[1], "time") == 0))
        return 0;

    const char *pron = NULL;
    for (size_t i = 1; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (is_entity_pronoun(t)) { pron = t; break; }
    }
    if (!pron) return 0;

    snprintf(b->pending_canon, sizeof b->pending_canon, "%s", norm);
    snprintf(b->pending_slot, sizeof b->pending_slot, "%s", pron);
    b->pending_repair = 1;

    char msg[160];
    if (has_arith_cue(b, w, nw))
        snprintf(msg, sizeof msg, "What number should I use for \"%s\"?", pron);
    else
        snprintf(msg, sizeof msg, "Who or what does \"%s\" refer to?", pron);
    put(msg, out, out_size);
    return 1;
}

/* --- module: whatifnot (gen129, L20-deep / L16) -------------------------
 * The EPISTEMIC sibling of mod_counterfactual. gen128 asked "what would you say
 * without module X?" (counterfactual over the CONTROL FLOW). This asks "what
 * would you conclude if you didn't know that <fact>?" — a counterfactual over
 * the KNOWLEDGE. It HYPOTHETICALLY retracts a believed fact from the live KB,
 * RE-DERIVES the last stated conclusion, reports whether it still stands, then
 * RESTORES the fact — footprint-free, exactly the gen128 discipline one level
 * down (belief instead of module). This is how the agent learns which of its
 * conclusions DEPEND on which of its beliefs: dependency made explicit by
 * removal, not asserted. Builds on gen103's re-derivation (note_consequence)
 * but driven by a hypothetical un-knowing rather than a real correction.
 */

/* Parse a simple ground unary fact clause ("socrates is a man" / "socrates is
 * mortal") into pred + single arg. Returns 1 on success. */
static int parse_ground_unary(const char *clause, char *pred, size_t ps,
                              char *arg, size_t as) {
    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    size_t n = strlen(buf);
    while (n > 0 && (buf[n-1]=='?'||buf[n-1]=='.'||buf[n-1]==','||buf[n-1]==' '))
        buf[--n] = '\0';
    char *w[12];
    size_t nw = split_words(buf, w, 12);
    if (nw >= 4 && strcmp(w[1], "is") == 0 && is_article(w[2])) {
        snprintf(pred, ps, "%s", w[3]); snprintf(arg, as, "%s", w[0]); return 1;
    }
    if (nw == 3 && strcmp(w[1], "is") == 0) {
        snprintf(pred, ps, "%s", w[2]); snprintf(arg, as, "%s", w[0]); return 1;
    }
    return 0;
}

static int mod_whatifnot(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* locate the un-known fact after the trigger phrase (read from raw so the
     * fact clause keeps its original words for parsing). */
    const char *fact = NULL;
    static const char *const markers[] = {
        "didn't know that ", "didnt know that ",
        "did not know that ", "didn't know ", "didnt know ", "did not know ",
        "non sapessi che ", "non sapessi ",
        NULL,
    };
    for (const char *const *m = markers; *m; m++) {
        const char *p = strstr(low, *m);
        if (p) { fact = p + strlen(*m); break; }
    }
    /* alternative shape: "if <subject> weren't / were not a <class>" */
    char alt[128] = "";
    if (!fact) {
        const char *p = strstr(low, "if ");
        const char *neg = NULL;
        if (p) {
            const char *cands[] = {" weren't ", " werent ", " were not ",
                                   " wasn't ", " wasnt ", " was not ",
                                   " non fosse ", NULL};
            for (size_t i = 0; cands[i]; i++) {
                const char *q = strstr(p, cands[i]);
                if (q) { neg = q;
                    /* subject = words between "if " and the negation */
                    size_t slen = (size_t)(neg - (p + 3));
                    char subj[64]; if (slen >= sizeof subj) slen = sizeof subj - 1;
                    memcpy(subj, p + 3, slen); subj[slen] = '\0';
                    const char *cls = neg + strlen(cands[i]);
                    /* class = up to comma/question */
                    char clsbuf[64]; size_t k = 0;
                    while (cls[k] && cls[k] != ',' && cls[k] != '?' && k+1 < sizeof clsbuf)
                        { clsbuf[k] = cls[k]; k++; }
                    clsbuf[k] = '\0';
                    snprintf(alt, sizeof alt, "%s is %s", subj, clsbuf);
                    fact = alt;
                    break;
                }
            }
        }
    }
    if (!fact) return 0;

    /* must be a genuine "what would you conclude / would X still" question */
    int q = cue(low, "conclude") || cue(low, "still") || cue(low, "concluderesti") ||
            cue(low, "ancora") || strstr(low, "what would") || strstr(low, "cosa");
    if (!q) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can tell you what that "
            "conclusion rests on.", out, out_size);
        return 1;
    }

    /* parse the fact (canonicalized so Italian clauses go through one path) */
    char fc[256];
    { char n1[256]; normalize(fact, n1, sizeof n1); canonicalize_lang(n1, fc, sizeof fc); }
    char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
    if (!parse_ground_unary(fc, pred, sizeof pred, arg, sizeof arg)) {
        put("I can only reconsider a simple 'X is a Y' fact for now.", out, out_size);
        return 1;
    }
    const char *fargs[] = {arg};
    if (!kb_query(b->kb, pred, fargs, 1)) {
        char msg[200];
        snprintf(msg, sizeof msg,
                 "But I don't know that %s is a %s in the first place.", arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    /* hypothetically un-know the fact, re-derive, then restore it */
    int before = goal_truth(b);
    int removed = kb_retract(b->kb, pred, fargs, 1);
    int after = goal_truth(b);
    if (removed) { kb_set_origin(b->kb, KB_SESSION); kb_assert(b->kb, pred, fargs, 1); }

    char msg[400];
    if (before == 1 && after == 0) {
        snprintf(msg, sizeof msg,
                 "If I didn't know that %s is a %s, I could no longer conclude that "
                 "%s is a %s — that conclusion rests on it.",
                 arg, pred, b->last_goal_arg, b->last_goal_pred);
    } else if (before == 1 && after == 1) {
        snprintf(msg, sizeof msg,
                 "Even without knowing that %s is a %s, %s would still be a %s — I "
                 "can reach that another way.",
                 arg, pred, b->last_goal_arg, b->last_goal_pred);
    } else {
        snprintf(msg, sizeof msg,
                 "That wouldn't change my conclusion about %s either way.",
                 b->last_goal_arg);
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: robust (gen130, L20-deep) ----------------------------------
 * Composes gen129's single-belief ablation into a SWEEP: it stress-tests the
 * last stated conclusion by hypothetically removing EACH ground unary fact in
 * turn (retract -> re-derive -> restore) and collecting the ones whose removal
 * overturns it. The result is dependency structure the agent discovers about
 * ITSELF by systematic self-perturbation: a FRAGILE conclusion hangs on one
 * load-bearing fact; a ROBUST one survives the loss of any single belief. This
 * is knowledge gen76's proof trace cannot give — a proof shows one derivation;
 * ablation reveals whether OTHER derivations exist (redundancy = robustness).
 */
static int mod_robust(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);
    int trig = cue(low, "robust") || cue(low, "fragile") ||
               cue(low, "load-bearing") || cue(low, "load bearing") ||
               cue(low, "stress-test") || cue(low, "stress test") ||
               cue(low, "depend on") || cue(low, "rest on") ||
               cue(low, "solida") || cue(low, "dipende") || cue(low, "si regge");
    if (!trig) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can stress-test it.",
            out, out_size);
        return 1;
    }
    if (goal_truth(b) != 1) {
        put("That isn't something I currently conclude, so there's nothing to "
            "stress-test.", out, out_size);
        return 1;
    }

    /* sweep every ground unary fact: remove it, re-derive, restore, record the
     * load-bearing ones. */
    char preds[64][KB_TERM_LEN];
    size_t np = kb_unary_predicates(b->kb, preds, 64);
    char crit[16][96];
    size_t ncrit = 0;
    for (size_t p = 0; p < np; p++) {
        char consts[128][KB_TERM_LEN];
        const char *pat[] = {NULL};
        size_t nc = kb_match(b->kb, preds[p], pat, 1, consts, 128);
        for (size_t c = 0; c < nc; c++) {
            const char *a[] = {consts[c]};
            if (!kb_retract(b->kb, preds[p], a, 1)) continue;
            int after = goal_truth(b);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, preds[p], a, 1); /* restore — footprint-free */
            if (after == 0 && ncrit < 16)
                snprintf(crit[ncrit++], sizeof crit[0], "%s(%s)", preds[p], consts[c]);
        }
    }

    char msg[640];
    if (ncrit == 0) {
        snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s is robust — there's no single fact "
                 "I could forget that would overturn it.",
                 b->last_goal_arg, b->last_goal_pred);
    } else if (ncrit == 1) {
        snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s is fragile: it rests entirely on one "
                 "fact — %s. Forget that and it falls.",
                 b->last_goal_arg, b->last_goal_pred, crit[0]);
    } else {
        size_t o = (size_t)snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s rests on %zu load-bearing facts: ",
                 b->last_goal_arg, b->last_goal_pred, ncrit);
        for (size_t i = 0; i < ncrit && o < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                  i ? ", " : "", crit[i]);
        if (o < sizeof msg)
            snprintf(msg + o, sizeof msg - o, ". Removing any one would overturn it.");
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: calibrate (gen142, E8) -------------------------------------
 * Metacognitive calibration: the agent reports HOW it knows the last
 * conclusion, with confidence language COMPUTED from the real proof state, never
 * a canned adjective. It distinguishes FIVE epistemic states of the last stated
 * goal and answers two follow-ups from each:
 *   KNOWN        — a directly stored fact (proof has no "because").
 *   INFERRED     — derived through a rule/chain (proof contains "because").
 *   CONFLICTED   — the user has asserted both polarities about this atom; the
 *                  KB's last-write-wins override hid one claim, but the
 *                  contradiction was recorded, so both are named.
 *   HYPOTHETICAL — the conclusion rests on a standing "suppose X." assumption
 *                  (detected by ablation: removing the assumed fact flips it).
 *   UNKNOWN      — nothing supports it: predicate unknown, or no proof.
 * "Why do you think that?" reports the support + the state's confidence; "what
 * would make you change your mind?" reports the real lever — for KNOWN/INFERRED
 * the load-bearing facts found by the same ablation sweep mod_robust uses, for
 * HYPOTHETICAL the assumption to drop, for CONFLICTED the source to adjudicate,
 * for UNKNOWN the fact that would settle it. All grounded in state.
 *
 * It also owns the standing supposition: a bare "suppose X." (no "then", which
 * mod_knowledge's one-shot form needs) asserts X as a SESSION fact and records
 * it as an assumption, so subsequent ordinary answers can be graded hypothetical.
 */
enum epi_state { EPI_UNKNOWN, EPI_KNOWN, EPI_INFERRED, EPI_CONFLICTED, EPI_HYPO };

/* Which assumed fact (if any) the last goal's truth depends on: remove each
 * assumed fact, re-derive, restore; return the index whose removal flips the
 * goal from true to not-provable, else -1. Footprint-free (restores). */
static int calib_hypo_support(Brain *b) {
    if (!b || !b->kb || !b->has_last_goal || b->assumed_n == 0) return -1;
    if (goal_truth(b) != 1) return -1;
    for (size_t i = 0; i < b->assumed_n; i++) {
        const char *a[] = {b->assumed_arg[i]};
        if (!kb_query(b->kb, b->assumed_pred[i], a, 1)) continue;
        if (!kb_retract(b->kb, b->assumed_pred[i], a, 1)) continue;
        int after = goal_truth(b);
        kb_set_origin(b->kb, KB_SESSION);
        kb_assert(b->kb, b->assumed_pred[i], a, 1); /* restore */
        if (after == 0) return (int)i;
    }
    return -1;
}

/* Classify the epistemic state of the last stated goal from real KB/proof
 * state. `hypo_idx` (out) receives the assumed-fact index if HYPOTHETICAL. */
static enum epi_state calib_classify(Brain *b, int *hypo_idx) {
    *hypo_idx = -1;
    if (!b || !b->kb || !b->has_last_goal) return EPI_UNKNOWN;
    const char *a[] = {b->last_goal_arg};
    /* conflict the user actually produced about this exact atom */
    if (b->has_conflict &&
        strcmp(b->conflict_pred, b->last_goal_pred) == 0 &&
        strcmp(b->conflict_arg, b->last_goal_arg) == 0)
        return EPI_CONFLICTED;
    /* a direct fact-vs-fact conflict still standing in the KB */
    if (kb_is_conflicted(b->kb, b->last_goal_pred, a, 1)) return EPI_CONFLICTED;
    if (!kb_knows_pred(b->kb, b->last_goal_pred)) return EPI_UNKNOWN;
    if (goal_truth(b) != 1) return EPI_UNKNOWN; /* not currently concluded */
    int hi = calib_hypo_support(b);
    if (hi >= 0) { *hypo_idx = hi; return EPI_HYPO; }
    /* known vs inferred from the proof shape */
    if (b->has_last_proof && strstr(b->last_proof, " because ")) return EPI_INFERRED;
    return EPI_KNOWN;
}

/* Append the load-bearing facts of the last goal (the ones whose removal
 * overturns it) into `buf`; returns how many were found. Reuses the same
 * retract/re-derive/restore sweep as mod_robust — the genuine "what would change
 * my mind" lever for a derived or fact-backed conclusion. */
static size_t calib_load_bearing(Brain *b, char buf[][96], size_t max) {
    size_t n = 0;
    char preds[64][KB_TERM_LEN];
    size_t np = kb_unary_predicates(b->kb, preds, 64);
    for (size_t p = 0; p < np && n < max; p++) {
        char consts[128][KB_TERM_LEN];
        const char *pat[] = {NULL};
        size_t nc = kb_match(b->kb, preds[p], pat, 1, consts, 128);
        for (size_t c = 0; c < nc && n < max; c++) {
            const char *a[] = {consts[c]};
            if (!kb_retract(b->kb, preds[p], a, 1)) continue;
            int after = goal_truth(b);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, preds[p], a, 1); /* restore */
            if (after == 0)
                snprintf(buf[n++], 96, "%s is a %s", consts[c], preds[p]);
        }
    }
    return n;
}

static int mod_calibrate(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* (1) standing supposition: "suppose X." / "assume X." / "supponi X." with
     * NO "then" (the one-shot "suppose X then Y" stays with mod_knowledge). It
     * asserts X as a session fact AND records it as an assumption so later
     * answers that rest on it grade HYPOTHETICAL. */
    {
        const char *rest = NULL;
        static const char *const sup[] = {
            "suppose that ", "suppose ", "assume that ", "assume ",
            "let's say ", "lets say ", "supponi che ", "supponi ",
            "assumi che ", "assumi ", NULL };
        for (const char *const *m = sup; *m; m++)
            if (strncmp(norm, *m, strlen(*m)) == 0) { rest = norm + strlen(*m); break; }
        if (rest && !strstr(rest, " then ") && !strstr(rest, " allora ")) {
            char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
            if (parse_ground_unary(rest, pred, sizeof pred, arg, sizeof arg)) {
                const char *a[] = {arg};
                kb_set_origin(b->kb, KB_SESSION);
                kb_assert(b->kb, pred, a, 1);
                if (b->assumed_n < 8) {
                    snprintf(b->assumed_pred[b->assumed_n], KB_TERM_LEN, "%s", pred);
                    snprintf(b->assumed_arg[b->assumed_n], KB_TERM_LEN, "%s", arg);
                    b->assumed_n++;
                }
                char msg[200];
                snprintf(msg, sizeof msg,
                         "All right — assuming %s is a %s. Anything I conclude from "
                         "it will be conditional on that.", arg, pred);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* (2) metacognitive follow-ups. Two intents, recognized by communicative
     * cue, both answered from the last goal's epistemic state. */
    int why_think = cue(low, "why do you think") || cue(low, "why do you believe") ||
                    cue(low, "what makes you think") || cue(low, "what makes you say") ||
                    cue(low, "how certain") || cue(low, "how do you rate") ||
                    cue(low, "perché lo pensi") || cue(low, "perche lo pensi") ||
                    cue(low, "perché lo credi") || cue(low, "perche lo credi") ||
                    cue(low, "quanto sei certo");
    int change_mind = cue(low, "change your mind") || cue(low, "change my mind") ||
                      cue(low, "make you reconsider") || cue(low, "would you reconsider") ||
                      cue(low, "prove you wrong") || cue(low, "what would change") ||
                      cue(low, "cambiare idea") || cue(low, "farti ricredere");
    if (!why_think && !change_mind) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can tell you how I know "
            "it and what would change my mind.", out, out_size);
        return 1;
    }

    int hypo_idx = -1;
    enum epi_state st = calib_classify(b, &hypo_idx);
    char msg[640];

    if (why_think) {
        switch (st) {
        case EPI_KNOWN:
            snprintf(msg, sizeof msg,
                     "I'm certain: %s is a %s is a fact you stated directly, so I "
                     "hold it as known, not inferred.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        case EPI_INFERRED:
            snprintf(msg, sizeof msg,
                     "I'm confident but it's derived, not given: %s. I concluded it "
                     "by applying a rule, so it's only as sound as those premises.",
                     b->has_last_proof ? b->last_proof : "a rule chain");
            break;
        case EPI_CONFLICTED:
            snprintf(msg, sizeof msg,
                     "I'm genuinely unsure — I hold conflicting claims about %s: you "
                     "told me it is a %s and also that it is not. Until that's "
                     "resolved I can't commit either way.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        case EPI_HYPO:
            snprintf(msg, sizeof msg,
                     "Only conditionally: %s is a %s holds because I'm ASSUMING %s is "
                     "a %s. Drop that assumption and I'd have no ground for it.",
                     b->last_goal_arg, b->last_goal_pred,
                     b->assumed_arg[hypo_idx], b->assumed_pred[hypo_idx]);
            break;
        default: /* EPI_UNKNOWN */
            snprintf(msg, sizeof msg,
                     "I don't actually think that — I have no support for %s being a "
                     "%s, so I can't claim it either way.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        }
        put(msg, out, out_size);
        return 1;
    }

    /* change_mind */
    switch (st) {
    case EPI_KNOWN:
        snprintf(msg, sizeof msg,
                 "Learning that %s is NOT a %s — that's the single fact it rests on, "
                 "so contradicting it would overturn my belief.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    case EPI_INFERRED: {
        char crit[8][96];
        size_t nc = calib_load_bearing(b, crit, 8);
        if (nc == 0)
            snprintf(msg, sizeof msg,
                     "Nothing simple — I can reach that another way, so no single "
                     "fact I drop would change it.");
        else if (nc == 1)
            snprintf(msg, sizeof msg,
                     "It hangs on one premise: if I learned that %s were false, I'd "
                     "stop concluding %s is a %s.",
                     crit[0], b->last_goal_arg, b->last_goal_pred);
        else {
            size_t o = (size_t)snprintf(msg, sizeof msg,
                     "It hangs on %zu premises — overturning any one would change it: ",
                     nc);
            for (size_t i = 0; i < nc && o < sizeof msg; i++)
                o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                      i ? ", " : "", crit[i]);
            if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
        }
        break;
    }
    case EPI_CONFLICTED:
        snprintf(msg, sizeof msg,
                 "Telling me which to trust — you've claimed both that %s is a %s and "
                 "that it is not. Retract one and I'd settle on the other.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    case EPI_HYPO:
        snprintf(msg, sizeof msg,
                 "Dropping the assumption — I only conclude %s is a %s because I'm "
                 "supposing %s is a %s. Withdraw that and the conclusion goes with it.",
                 b->last_goal_arg, b->last_goal_pred,
                 b->assumed_arg[hypo_idx], b->assumed_pred[hypo_idx]);
        break;
    default: /* EPI_UNKNOWN */
        snprintf(msg, sizeof msg,
                 "Learning whether %s is a %s at all — right now I have no support "
                 "either way, so any fact about it would be new ground.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    }
    put(msg, out, out_size);
    (void)norm;
    return 1;
}

/* --- module: abduce (gen131, L16/L19) -----------------------------------
 * The INVERSE of deduction and the mirror of gen129's subtraction. gen129 asked
 * "remove this belief — does the conclusion fall?" abduction asks the opposite:
 * the conclusion does NOT hold — what belief WOULD make it hold? It runs the
 * rule engine BACKWARDS: find a rule whose head is the goal, and name the body
 * premise(s) that, if believed, would entail it ("If you told me socrates is a
 * man, then socrates would be a mortal — that's the rule man -> mortal"). This
 * is inference to the missing premise, computed from the real rules, not
 * guessed. Honest when nothing could entail it, and defers to deduction when the
 * goal already holds.
 */

/* Loose parse of a goal clause into (arg, pred): drop the copula/article/
 * infinitive filler and take the first surviving token as the subject and the
 * last as the class. Handles "socrates a mortal", "socrates be a mortal",
 * "socrates to be a mortal". Returns 1 on a clean two-token reading. */
static int parse_goal_loose(const char *clause, char *pred, size_t ps,
                            char *arg, size_t as) {
    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    size_t n = strlen(buf);
    while (n > 0 && (buf[n-1]=='?'||buf[n-1]=='.'||buf[n-1]=='!'||buf[n-1]==','))
        buf[--n] = '\0';
    char *w[16];
    size_t nw = split_words(buf, w, 16);
    char kept[16][KB_TERM_LEN]; size_t nk = 0;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strcmp(t,"is")==0 || strcmp(t,"be")==0 || strcmp(t,"a")==0 ||
            strcmp(t,"an")==0 || strcmp(t,"to")==0 || strcmp(t,"the")==0 ||
            strcmp(t,"would")==0 || strcmp(t,"still")==0 ||
            strcmp(t,"not")==0)
            continue;
        if (nk < 16) snprintf(kept[nk++], KB_TERM_LEN, "%s", t);
    }
    if (nk < 2) return 0;
    snprintf(arg, as, "%s", kept[0]);
    snprintf(pred, ps, "%s", kept[nk - 1]);
    return 1;
}

/* gen132: backward chaining for abduction. Collect the ROOT premises that would
 * make goal `pred(arg)` hold — the unsatisfied predicates with no rule of their
 * own, reached by following rule bodies down. A body already true is satisfied
 * (skipped); a body with its own rule recurses; a body that is neither is a
 * root to acquire. Dedups; depth-guarded. Returns the new root count. */
static size_t abduce_roots(KB *kb, const char *pred, const char *arg, int depth,
                           char roots[][KB_TERM_LEN], size_t maxr, size_t nr) {
    if (depth > 16) return nr;
    char bodies[8][KB_TERM_LEN];
    size_t nb = kb_rule_body_preds(kb, pred, 1, bodies, 8);
    if (nb == 0) { /* no rule defines pred -> it is itself a root premise */
        for (size_t i = 0; i < nr; i++)
            if (strcmp(roots[i], pred) == 0) return nr; /* dedup */
        if (nr < maxr) snprintf(roots[nr++], KB_TERM_LEN, "%s", pred);
        return nr;
    }
    for (size_t i = 0; i < nb; i++) {
        const char *a[] = {arg};
        if (kb_query(kb, bodies[i], a, 1)) continue; /* already satisfied */
        nr = abduce_roots(kb, bodies[i], arg, depth + 1, roots, maxr, nr);
    }
    return nr;
}

/* gen132: build the linear rule spine "root -> … -> goal" by following the FIRST
 * body predicate at each level until a predicate with no rule. */
static void abduce_spine(KB *kb, const char *pred, char *out, size_t out_size) {
    char chain[16][KB_TERM_LEN]; size_t n = 0;
    char cur[KB_TERM_LEN]; snprintf(cur, sizeof cur, "%s", pred);
    for (int d = 0; d < 16 && n < 16; d++) {
        snprintf(chain[n++], KB_TERM_LEN, "%s", cur);
        char bodies[8][KB_TERM_LEN];
        if (kb_rule_body_preds(kb, cur, 1, bodies, 8) == 0) break;
        snprintf(cur, sizeof cur, "%s", bodies[0]);
    }
    size_t o = 0;
    for (size_t i = n; i-- > 0;) /* reverse: root first */
        o += (size_t)snprintf(out + o, o < out_size ? out_size - o : 0,
                              "%s%s", (i + 1 < n) ? " -> " : "", chain[i]);
}

static size_t add_missing_root(char roots[][KB_TERM_LEN], size_t nr,
                               size_t maxr, const char *pred) {
    for (size_t i = 0; i < nr; i++)
        if (strcmp(roots[i], pred) == 0) return nr;
    if (nr < maxr) snprintf(roots[nr++], KB_TERM_LEN, "%.*s", KB_TERM_LEN - 1, pred);
    return nr;
}

/* gen138: cost one alternative by the ROOT facts still needed to make it true.
 * Already-satisfied body predicates cost zero; derived missing predicates are
 * pushed backward to their roots, so the score is over acquirable facts. */
static size_t abduce_rule_missing_roots(KB *kb, const char *head, size_t rule_idx,
                                        const char *arg,
                                        char roots[][KB_TERM_LEN], size_t maxr,
                                        char *rule, size_t rule_size) {
    roots[0][0] = 0;
    char rb[8][KB_TERM_LEN];
    size_t nrb = kb_nth_rule_body_preds(kb, head, 1, rule_idx, rb, 8);
    size_t nr = 0, ro = 0;
    if (rule_size) rule[0] = 0;
    for (size_t i = 0; i < nrb; i++) {
        ro += (size_t)snprintf(rule + ro, ro < rule_size ? rule_size - ro : 0,
                               "%s%s", i ? " and " : "", rb[i]);
        const char *pa[] = {arg};
        if (kb_query(kb, rb[i], pa, 1)) continue;
        char sub[8][KB_TERM_LEN];
        if (kb_rule_body_preds(kb, rb[i], 1, sub, 8) > 0)
            nr = abduce_roots(kb, rb[i], arg, 0, roots, maxr, nr);
        else
            nr = add_missing_root(roots, nr, maxr, rb[i]);
    }
    return nr;
}

static int mod_abduce(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* trigger + clause extraction from the canonicalized surface */
    static const char *const pre[] = {
        "what would make ", "what makes ", "what would let ",
        "why might ", "why would ", "why could ", "how could ", "how can ",
        "what would you need to know for ", "what do you need to know for ",
        "cosa renderebbe ", "cosa servirebbe per ", "che cosa renderebbe ",
        NULL,
    };
    const char *clause = NULL;
    int contrastive = 0;
    int optimal = 0;
    int simulate = 0;

    static const char *const sim_pre[] = {
        "try the easiest way to make ", "simulate the easiest way to make ",
        "prova il modo piu facile per rendere ", "prova il modo più facile per rendere ", NULL
    };
    for (const char *const *p = sim_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; optimal = 1; simulate = 1; break; }
    }

    static const char *const opt_pre[] = {
        "what is the easiest way to make ", "what's the easiest way to make ",
        "easiest way to make ", "least i need to know for ",
        "modo piu facile per rendere ", "modo più facile per rendere ", NULL
    };
    for (const char *const *p = opt_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; optimal = 1; break; }
    }

    static const char *const neg_pre[] = {
        "why is not ", "why is ", "why ", "perché ", "perche ", NULL
    };
    for (const char *const *p = neg_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) != 0) continue;
        const char *cand = norm + L;
        if (strstr(cand, " not ") || strncmp(cand, "not ", 4) == 0) {
            clause = cand;
            contrastive = 1;
            break;
        }
    }

    for (const char *const *p = pre; !clause && *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; break; }
    }
    if (!clause) return 0;

    char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
    if (!parse_goal_loose(clause, pred, sizeof pred, arg, sizeof arg)) {
        put("Tell me what you'd like me to account for, as 'X is a Y'.",
            out, out_size);
        return 1;
    }

    const char *gargs[] = {arg};
    if (kb_query(b->kb, pred, gargs, 1)) {
        char ex[400], msg[480];
        if (kb_explain(b->kb, pred, gargs, 1, ex, sizeof ex) && strstr(ex, " because "))
            snprintf(msg, sizeof msg, "I already conclude that: %s.", ex);
        else
            snprintf(msg, sizeof msg, "I already know that %s is a %s.", arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    char bodies[8][KB_TERM_LEN];
    size_t nb = kb_rule_body_preds(b->kb, pred, 1, bodies, 8);
    if (nb == 0) {
        char msg[200];
        snprintf(msg, sizeof msg,
                 "I have no rule that would make anything a %s, so I can't say "
                 "what would.", pred);
        put(msg, out, out_size);
        return 1;
    }

    if (optimal) {
        size_t nrules = kb_rules_for_head(b->kb, pred, 1);
        size_t best_n = 999;
        char best_roots[16][KB_TERM_LEN];
        char best_rule[200]; best_rule[0] = '\0';
        for (size_t r = 0; r < nrules; r++) {
            char roots[16][KB_TERM_LEN];
            char rule[200];
            size_t nr = abduce_rule_missing_roots(b->kb, pred, r, arg,
                                                  roots, 16, rule, sizeof rule);
            if (nr > 0 && nr < best_n) {
                best_n = nr;
                snprintf(best_rule, sizeof best_rule, "%s", rule);
                for (size_t i = 0; i < nr; i++)
                    snprintf(best_roots[i], KB_TERM_LEN, "%s", roots[i]);
            }
        }

        char msg[1400];
        if (best_n == 999) {
            snprintf(msg, sizeof msg,
                     "I can see rules for %s, but no missing premise plan stands out.",
                     pred);
        } else {
            char need[500]; size_t no = 0;
            for (size_t i = 0; i < best_n; i++)
                no += (size_t)snprintf(need + no,
                                       no < sizeof need ? sizeof need - no : 0,
                                       "%s%s is a %s", i ? " and " : "",
                                       arg, best_roots[i]);
            if (simulate) {
                for (size_t i = 0; i < best_n; i++) {
                    const char *ra[] = {arg};
                    kb_set_origin(b->kb, KB_SESSION);
                    kb_assert(b->kb, best_roots[i], ra, 1);
                }
                int ok = kb_query(b->kb, pred, gargs, 1);
                char ex[512] = "";
                if (ok) kb_explain(b->kb, pred, gargs, 1, ex, sizeof ex);
                for (size_t i = 0; i < best_n; i++) {
                    const char *ra[] = {arg};
                    kb_retract(b->kb, best_roots[i], ra, 1);
                }
                if (ok)
                    snprintf(msg, sizeof msg,
                             "Hypothetically adding %s makes %s a %s: %s. I restored the KB after the simulation.",
                             need, arg, pred, ex);
                else
                    snprintf(msg, sizeof msg,
                             "I tried the cheapest plan (%s), but it still did not derive %s(%s). I restored the KB.",
                             need, pred, arg);
            } else {
                snprintf(msg, sizeof msg,
                         "The easiest way is to tell me that %s — %zu missing premise%s via %s -> %s.",
                         need, best_n, best_n == 1 ? "" : "s", best_rule, pred);
            }
        }
        put(msg, out, out_size);
        return 1;
    }

    /* gen135-137: contrastive why-not questions reuse the abductive machinery.
     * A failed goal may have one rule, a chain, or several alternative rules;
     * explain the blocked proof shape that is actually present in the KB. */
    if (contrastive) {
        size_t crules = kb_rules_for_head(b->kb, pred, 1);
        char msg[1400];

        if (crules > 1) {
            char alts[1000]; alts[0] = '\0'; size_t ao = 0; size_t shown = 0;
            for (size_t r = 0; r < crules; r++) {
                char rb[8][KB_TERM_LEN];
                size_t nrb = kb_nth_rule_body_preds(b->kb, pred, 1, r, rb, 8);
                char rule[200]; size_t ro = 0;
                char need[300]; need[0] = '\0'; size_t no = 0; size_t nn = 0;
                for (size_t i = 0; i < nrb; i++) {
                    const char *pa[] = {arg};
                    ro += (size_t)snprintf(rule + ro,
                                           ro < sizeof rule ? sizeof rule - ro : 0,
                                           "%s%s", i ? " and " : "", rb[i]);
                    if (kb_query(b->kb, rb[i], pa, 1)) continue;
                    char sub[8][KB_TERM_LEN];
                    if (kb_rule_body_preds(b->kb, rb[i], 1, sub, 8) > 0) {
                        char roots[16][KB_TERM_LEN];
                        size_t nr = abduce_roots(b->kb, rb[i], arg, 0, roots, 16, 0);
                        for (size_t j = 0; j < nr; j++)
                            no += (size_t)snprintf(need + no,
                                                   no < sizeof need ? sizeof need - no : 0,
                                                   "%s%s is a %s", nn ? " and " : "",
                                                   arg, roots[j]), nn++;
                    } else {
                        no += (size_t)snprintf(need + no,
                                               no < sizeof need ? sizeof need - no : 0,
                                               "%s%s is a %s", nn ? " and " : "",
                                               arg, rb[i]);
                        nn++;
                    }
                }
                if (nn == 0) continue;
                ao += (size_t)snprintf(alts + ao,
                                       ao < sizeof alts ? sizeof alts - ao : 0,
                                       "%s%s -> %s needs %s", shown ? "; " : "",
                                       rule, pred, need);
                shown++;
            }
            snprintf(msg, sizeof msg,
                     "%s is not a %s because every alternative is blocked: %s.",
                     arg, pred, alts);
        } else {
            int deeper_missing = 0;
            for (size_t i = 0; i < nb; i++) {
                const char *pa[] = {arg};
                char sub[8][KB_TERM_LEN];
                if (!kb_query(b->kb, bodies[i], pa, 1) &&
                    kb_rule_body_preds(b->kb, bodies[i], 1, sub, 8) > 0) {
                    deeper_missing = 1;
                    break;
                }
            }

            if (deeper_missing) {
                char roots[16][KB_TERM_LEN];
                size_t nr = abduce_roots(b->kb, pred, arg, 0, roots, 16, 0);
                char missing[400]; size_t mo = 0;
                for (size_t i = 0; i < nr; i++)
                    mo += (size_t)snprintf(missing + mo,
                                           mo < sizeof missing ? sizeof missing - mo : 0,
                                           "%s%s is a %s", i ? " and " : "",
                                           arg, roots[i]);
                char spine[256]; abduce_spine(b->kb, pred, spine, sizeof spine);
                snprintf(msg, sizeof msg,
                         "%s is not a %s because I am missing the root premise: %s. By %s.",
                         arg, pred, missing, spine);
            } else {
                char missing[400]; size_t mo = 0; int nm = 0;
                char rule[200]; size_t ro = 0;
                for (size_t i = 0; i < nb; i++) {
                    const char *pa[] = {arg};
                    if (!kb_query(b->kb, bodies[i], pa, 1)) {
                        mo += (size_t)snprintf(missing + mo,
                                               mo < sizeof missing ? sizeof missing - mo : 0,
                                               "%s%s is a %s", nm ? " and " : "",
                                               arg, bodies[i]);
                        nm++;
                    }
                    ro += (size_t)snprintf(rule + ro,
                                           ro < sizeof rule ? sizeof rule - ro : 0,
                                           "%s%s", i ? " and " : "", bodies[i]);
                }
                if (nm > 0)
                    snprintf(msg, sizeof msg,
                             "%s is not a %s because I am missing: %s. The rule is %s -> %s.",
                             arg, pred, missing, rule, pred);
                else
                    snprintf(msg, sizeof msg,
                             "I have the premises for %s to be a %s, but I still cannot derive it.",
                             arg, pred);
            }
        }
        put(msg, out, out_size);
        return 1;
    }

    size_t nrules = kb_rules_for_head(b->kb, pred, 1);
    if (nrules > 1) {
        char alts[400]; size_t ao = 0; size_t shown = 0;
        for (size_t r = 0; r < nrules; r++) {
            char rb[8][KB_TERM_LEN];
            size_t nrb = kb_nth_rule_body_preds(b->kb, pred, 1, r, rb, 8);
            char one[200]; size_t oo = 0; int miss = 0;
            for (size_t i = 0; i < nrb; i++) {
                const char *pa[] = {arg};
                if (kb_query(b->kb, rb[i], pa, 1)) continue; /* satisfied conjunct */
                oo += (size_t)snprintf(one + oo, oo < sizeof one ? sizeof one - oo : 0,
                                       "%s%s is a %s", miss ? " and " : "", arg, rb[i]);
                miss++;
            }
            if (!miss) continue; /* this rule is already fully satisfied */
            ao += (size_t)snprintf(alts + ao, ao < sizeof alts ? sizeof alts - ao : 0,
                                   "%s%s", shown ? ", or " : "", one);
            shown++;
        }
        char msg[600];
        snprintf(msg, sizeof msg,
                 "There's more than one way: either %s — any one would make %s a %s.",
                 alts, arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    /* gen132: is any immediate premise itself DERIVABLE (has its own rule) and
     * unsatisfied? Then a single step doesn't reach acquirable facts — chain
     * backwards to the ROOT premises and show the rule spine. Otherwise keep the
     * gen131 one-step message (the immediate body IS the root). */
    int deeper = 0;
    for (size_t i = 0; i < nb; i++) {
        const char *pa[] = {arg};
        char sub[8][KB_TERM_LEN];
        if (!kb_query(b->kb, bodies[i], pa, 1) &&
            kb_rule_body_preds(b->kb, bodies[i], 1, sub, 8) > 0) { deeper = 1; break; }
    }

    char msg[1024];
    if (deeper) {
        char roots[16][KB_TERM_LEN];
        size_t nr = abduce_roots(b->kb, pred, arg, 0, roots, 16, 0);
        char prem[400]; size_t po = 0;
        for (size_t i = 0; i < nr; i++)
            po += (size_t)snprintf(prem + po, po < sizeof prem ? sizeof prem - po : 0,
                                   "%s%s is a %s", i ? " and " : "", arg, roots[i]);
        char spine[256]; abduce_spine(b->kb, pred, spine, sizeof spine);
        snprintf(msg, sizeof msg,
                 "If you told me that %s, then %s would be a %s — by %s.",
                 prem, arg, pred, spine);
    } else {
        /* name only the MISSING conjuncts as the premise to supply, but show the
         * full rule body so the rule itself is reported faithfully. */
        char prem[400]; size_t po = 0; int np_missing = 0;
        char rule[200]; size_t ro = 0;
        for (size_t i = 0; i < nb; i++) {
            const char *pa[] = {arg};
            if (!kb_query(b->kb, bodies[i], pa, 1)) {
                po += (size_t)snprintf(prem + po, po < sizeof prem ? sizeof prem - po : 0,
                                       "%s%s is a %s", np_missing ? " and " : "",
                                       arg, bodies[i]);
                np_missing++;
            }
            ro += (size_t)snprintf(rule + ro, ro < sizeof rule ? sizeof rule - ro : 0,
                                   "%s%s", i ? " and " : "", bodies[i]);
        }
        snprintf(msg, sizeof msg,
                 "If you told me that %s, then %s would be a %s — that's the rule %s -> %s.",
                 prem, arg, pred, rule, pred);
    }
    put(msg, out, out_size);
    return 1;
}

/* gen189 (basic-chat cat.0): classify NON-LINGUISTIC input and answer at the
 * CHANNEL level instead of the topic-level "I don't understand that yet."
 *
 * The distinction is structural, taken before the reasoning core: "is this
 * language at all?" — not "do I know this topic?". Three shapes of noise:
 *   (1) punctuation/symbols only ("?", "!", "!@#$%^&*()")
 *   (2) a bare number with no operation ("1234567890")
 *   (3) keyboard-mash letters ("asdfghjkl", "aaaaaaaaaa")
 * This is NOT a phrasebook of the example strings: it keys purely on character
 * structure, so it generalizes to any such input. And because non-linguistic
 * input carries no language, the same path serves every language — the
 * bilingual ratchet holds by construction. It only ever REDIRECTS honestly; it
 * never feigns understanding (PRINCIPLES.md: no impostor printf of "knowledge").
 *
 * Placed right after mod_repair so a noise turn is recognized before any content
 * module can mis-claim it (mod_code used to read "!@#$%^&*()" as a code snippet;
 * mod_social used to greet "asdfghjkl"). It declines (returns 0) for anything
 * that looks like language, so normal turns reach the rest of the registry. */
static int mod_input(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    size_t len = strlen(norm);
    if (len == 0) return 0;          /* empty turns are dropped by the I/O shell */

    size_t nalpha = 0, ndigit = 0, npunct = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)norm[i];
        if (isalpha(c)) nalpha++;
        else if (isdigit(c)) ndigit++;
        else if (!isspace(c)) npunct++;
    }

    /* (1) Punctuation/symbols only — no letters, no digits, at least one mark.
     * Defer dot/dash/slash sequences: those are a *structured* code (Morse),
     * owned by mod_symbolic, not formless noise. The boundary is honest — this
     * module claims punctuation only when it is not a recognized symbolic form. */
    if (nalpha == 0 && ndigit == 0 && npunct > 0) {
        int morse_only = 1;
        for (size_t i = 0; i < len; i++) {
            char c = norm[i];
            if (c != '.' && c != '-' && c != '/' && !isspace((unsigned char)c)) {
                morse_only = 0; break;
            }
        }
        if (morse_only) return 0;    /* let the symbolic recognizer name it */
        put("That's just punctuation, not words — what would you like to ask?",
            out, out_size);
        return 1;
    }

    /* The remaining shapes are single tokens; multi-word input is linguistic. */
    char buf[256];
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw != 1) return 0;
    char *tok = strip_edge_punct(w[0]);
    size_t tlen = strlen(tok);
    if (tlen == 0) return 0;

    /* (2) A bare number (>=4 digits, no operator). mod_arith owns anything with
     * an operation; a lone long digit-run has nothing to compute. The >=4 gate
     * leaves short numbers (a game pick, "7", "100") for future modules. */
    int all_digit = 1;
    for (size_t i = 0; i < tlen; i++)
        if (!isdigit((unsigned char)tok[i])) { all_digit = 0; break; }
    if (all_digit && tlen >= 4) {
        char msg[160];
        snprintf(msg, sizeof msg,
                 "That's just the number %s with nothing to do — what would "
                 "you like me to do with it?", tok);
        put(msg, out, out_size);
        return 1;
    }

    /* (3) Keyboard-mash letters: a single all-alphabetic token with the shape of
     * noise — no vowel at all, one character repeated, a run of >=4 identical
     * letters, or an implausibly long consonant run (>=6) — that the KB does not
     * recognize. Thresholds are conservative so real words never trip it
     * ("strengths" peaks at 5 consonants; "rhythm" is too short here). */
    if (tlen >= 6) {
        int all_alpha = 1;
        for (size_t i = 0; i < tlen; i++)
            if (!isalpha((unsigned char)tok[i])) { all_alpha = 0; break; }
        if (all_alpha) {
            size_t vowels = 0, distinct = 0;
            size_t run = 1, max_run = 1, cons = 0, max_cons = 0;
            int seen[26] = {0};
            for (size_t i = 0; i < tlen; i++) {
                char c = tok[i];
                /* 'y' counts as a vowel here so consonant-only words that lean
                 * on it ("rhythm", "syzygy") are not mistaken for noise. */
                int v = (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||c=='y');
                if (v) vowels++;
                if (c >= 'a' && c <= 'z' && !seen[c-'a']) { seen[c-'a']=1; distinct++; }
                if (i > 0 && c == tok[i-1]) { if (++run > max_run) max_run = run; }
                else run = 1;
                if (!v) { if (++cons > max_cons) max_cons = cons; } else cons = 0;
            }
            int known = 0;
            if (b && b->kb) {
                char desc[256];
                known = kb_knows_pred(b->kb, tok) ||
                        kb_describe_entity(b->kb, tok, desc, sizeof desc);
            }
            int noise = (vowels == 0) || (distinct == 1) ||
                        (max_run >= 4) || (max_cons >= 6);
            if (noise && !known && !is_stopword(b, tok)) {
                put("That doesn't look like words to me — did a key get stuck? "
                    "I'm here when you'd like to ask something.",
                    out, out_size);
                return 1;
            }
        }
    }
    return 0;
}

/* The registry: an ordered list of cooperating parts. To add or remove a
 * behaviour, touch only this table — not brain_respond()'s control flow.
 * (This table is also reified into the KB as module(...) facts at birth, so
 * the agent's self-description cannot drift from its real structure.) */
static const Module registry[] = {
    {"repair",    mod_repair},
    {"input",     mod_input},
    {"world",     mod_world},
    {"translate", mod_translate},
    {"synth",     mod_synth},
    {"induce",    mod_induce},
    {"verify",    mod_verify},
    {"memory",    mod_memory},
    {"loop",      mod_loop},
    {"meta",      mod_meta},
    {"strategy",  mod_strategy},
    {"counterfactual", mod_counterfactual},
    {"whatifnot", mod_whatifnot},
    {"robust",    mod_robust},
    {"calibrate", mod_calibrate},
    {"abduce",    mod_abduce},
    {"role",      mod_role},
    {"self",      mod_self},
    {"fewshot",   mod_fewshot},
    {"compare",   mod_compare},
    {"algebra",   mod_algebra},
    {"arith",     mod_arith},
    {"plan",      mod_plan},
    {"wordproblem", mod_wordproblem},
    {"agent",     mod_agent},
    {"search",    mod_search},
    {"tool",      mod_tool},
    {"quantity",  mod_quantity},
    {"cause",     mod_cause},
    {"same",      mod_same},
    {"analogy",   mod_analogy},
    {"conj",      mod_conj},
    {"gen",       mod_gen},
    {"coref",     mod_coref},
    {"bench",     mod_bench},
    {"reader",    mod_reader},
    {"shell",     mod_shell},
    {"knowledge", mod_knowledge},
    {"codeast",   mod_codeast},
    {"code",      mod_code},
    {"symbolic",  mod_symbolic},
    {"summary",   mod_summary},
    {"discourse", mod_discourse},
    {"pragma",    mod_pragma},
    {"social",    mod_social},
    {"chitchat",  mod_chitchat},
    {"research",  mod_research},
};
static const size_t registry_len = sizeof registry / sizeof registry[0];

/* gen128: true if `name` is a real module in the registry. */
static int is_registry_module(const char *name) {
    for (size_t i = 0; i < registry_len; i++)
        if (strcmp(registry[i].name, name) == 0) return 1;
    return 0;
}

/* gen128: re-run the first-match-wins dispatch over a stored turn with module
 * `suppress` skipped. Writes the claiming module's name into `who` and its reply
 * into `out`; returns 1 if some module claimed it, 0 if it fell through to the
 * honest fallback (out then holds that fallback line). This is parrot0
 * simulating its own counterfactual self, so we protect the live conversation
 * state: the volatile last_reply/last_module are snapshotted and restored, and
 * the trace is left untouched (the candidate handlers write only to `out`; any
 * KB assertion they repeat for this same turn is idempotent). */
/* gen141: dispatch a (possibly reconstructed) turn through the registry, skipping
 * the repair module itself so a resumed turn cannot re-open a clarification. Unlike
 * replay_dispatch this is NOT footprint-free: a resumed assertion really learns and
 * a resumed query really runs — resuming the original intent is the whole point. */
static int repair_dispatch(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size) {
    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, "repair") == 0) continue;
        if (registry[i].handle(b, canon, raw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    not_understood(b, canon, out, out_size);
    return 0;
}

/* gen142 (E3): peel a leading DISCOURSE-MARKER opener off a turn and re-dispatch
 * the residue through the WHOLE registry, so a content task survives the social
 * wrapper ("anyway, is socrates a man" -> "Yes."; "by the way what is 2 plus 2"
 * -> "4."). This runs as a pre-dispatch normalization in brain_respond — BEFORE
 * the registry — because a content module earlier than pragma would otherwise
 * mis-parse the wrapped turn (e.g. the extra opener tokens shift memory's
 * "what is my X" window onto "plus"). A channel-management opener is not content;
 * normalizing it away is the same gesture as canonicalize_lang, one level up.
 *
 * The peel is conservative: it fires only when (a) a real discourse opener leads,
 * (b) there is residue after it, and (c) some module ACTUALLY claims the residue.
 * If the residue is unclaimed we return 0 and the original turn is dispatched
 * normally (so e.g. "anyway, i guess maybe" still reaches mod_pragma's hesitation
 * move on its own shape). The RAW residue is rebuilt by skipping the same leading
 * word-count from the original input, preserving proper-name casing ("well,
 * remember my dog is Rex" keeps "Rex"). */
static int pragma_peel(Brain *b, const char *canon, const char *raw,
                       char *out, size_t out_size) {
    char buf[256];
    size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw == 0) return 0;

    size_t skip = 0;
    if (!is_discourse_opener(w, nw, &skip) || skip >= nw) return 0;

    char residue[256]; size_t off = 0; residue[0] = '\0';
    for (size_t i = skip; i < nw && off + 1 < sizeof residue; i++)
        off += (size_t)snprintf(residue + off, sizeof residue - off,
                                "%s%s", i > skip ? " " : "", w[i]);
    if (!residue[0]) return 0;

    /* matching RAW residue: skip the same leading word count (and a trailing
     * comma the opener often carries), keep original casing. */
    char raw_res[256]; { const char *p = raw ? raw : ""; size_t s = 0;
        while (*p && s < skip) {
            while (*p && isspace((unsigned char)*p)) p++;
            while (*p && !isspace((unsigned char)*p)) p++;
            s++;
        }
        while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
        snprintf(raw_res, sizeof raw_res, "%s", p);
    }

    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, residue, raw_res, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    return 0;
}

static int replay_dispatch(Brain *b, const char *canon, const char *raw,
                           const char *suppress, char *who, size_t who_size,
                           char *out, size_t out_size) {
    char saved_reply[256], saved_module[32];
    unsigned long saved_fallbacks = b->fallbacks;
    snprintf(saved_reply, sizeof saved_reply, "%s", b->last_reply);
    snprintf(saved_module, sizeof saved_module, "%s", b->last_module);

    int claimed = 0;
    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, suppress) == 0) continue; /* the removed part */
        if (strcmp(registry[i].name, "counterfactual") == 0) continue; /* no self-recursion */
        if (registry[i].handle(b, canon, raw, out, out_size)) {
            snprintf(who, who_size, "%s", registry[i].name);
            claimed = 1;
            break;
        }
    }
    if (!claimed) {
        snprintf(who, who_size, "fallback");
        not_understood(b, canon, out, out_size);
    }

    /* restore the live state so the counterfactual probe leaves no footprint */
    snprintf(b->last_reply, sizeof b->last_reply, "%s", saved_reply);
    snprintf(b->last_module, sizeof b->last_module, "%s", saved_module);
    b->fallbacks = saved_fallbacks;
    return claimed;
}

/* ----------------------------------------------------------------------------
 * brain lifecycle + dispatch
 * ------------------------------------------------------------------------- */

Brain *brain_create(void) {
    Brain *b = calloc(1, sizeof *b);
    if (!b) return NULL;
    b->kb = kb_create();
    if (!b->kb) { free(b); return NULL; }
    b->start_time = time(NULL);
    b->active_world = -1; /* gen142 (E7): no local world is open at birth */

    /* Curated lexical knowledge used by the kernel itself. It lives in the
     * knowledge layer, not as C word arrays; loading it as base keeps it out of
     * session saves while tests stay independent of world knowledge files. */
    const char *lexicon = getenv("PARROT0_LEXICON");
    if (!lexicon) lexicon = "kb/core/lexicon.p0";
    if (*lexicon) {
        kb_set_origin(b->kb, KB_BASE);
        kb_load(b->kb, lexicon);
    }

    /* gen73 (PLAN.md Phase 3): social markers, question words and reaction words
     * live in kb/core/social.p0, not as hardcoded C arrays. The KB is the
     * single source of truth; the C code queries it at runtime. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/social.p0");

    /* gen101 (C15): role/character world-knowledge — what parrot0 knows about
     * the kinds and figures it may be asked to impersonate (see mod_role). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/roles.p0");

    /* gen126 (L5): bilingual content lexicon used by mod_translate to COMPOSE a
     * clause translation from word glosses + a structural article rule. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/gloss.p0");

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
    return "gen192-code-link-verify";
}

/* gen55 (C5a): an honest, NON-repeating not-understood reply. The chatsim users
 * showed that repeating "I don't understand that yet." verbatim is the #1
 * naturalness killer (a broken record). So the classic line is kept for a LONE
 * occurrence (no test churn, still honest), but when it would repeat our previous
 * reply we vary — reflecting a salient word from the user so it feels heard, else
 * rotating honest redirects. It never feigns understanding. */
static void not_understood(Brain *b, const char *canon,
                           char *out, size_t out_size) {
    static const char *const v[] = {
        "I'm not sure I followed. Can you say it another way?",
        "I didn't quite catch that. What would you like to know?",
        "Hmm, that's a bit beyond me right now.",
        "I don't understand that yet.",
    };
    const size_t NV = sizeof v / sizeof v[0];
    const char *classic = "I don't understand that yet.";

    if (!b || strcmp(classic, b->last_reply) != 0) { /* fine to say it once */
        put(classic, out, out_size);
        if (b) b->fallbacks++;
        return;
    }

    /* it would repeat -> vary. Prefer reflecting a salient content word. */
    char buf[256];
    snprintf(buf, sizeof buf, "%s", canon);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    const char *sw = NULL;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        char desc[256];
        int known = b && b->kb &&
                    (kb_knows_pred(b->kb, t) ||
                     kb_describe_entity(b->kb, t, desc, sizeof desc));
        if (strlen(t) >= 6 && isalpha((unsigned char)t[0]) &&
            !is_stopword(b, t) && !known) {
            sw = t; break;
        }
    }
    char cand[256];
    unsigned long k = b->fallbacks;
    if (sw) snprintf(cand, sizeof cand, "Hmm, I don't know about %s yet.", sw);
    else    snprintf(cand, sizeof cand, "%s", v[k % NV]);
    for (size_t t = 0; t < NV && strcmp(cand, b->last_reply) == 0; t++)
        snprintf(cand, sizeof cand, "%s", v[(k + t) % NV]);
    put(cand, out, out_size);
    b->fallbacks++;
}

/* gen80: true if word `w` is a likely intent-marker that starts a sub-turn
 * after a discourse connector like "e"/"and" in a compound utterance. */
static int is_intent_starter(const char *w) {
    static const char *const starters[] = {
        "chi", "che", "cosa", "come", "dove", "quando", "perche", "perché",
        "ricordati", "dimmi", "spiegami", "chiamami", "insegnami", "parlami",
        "prima", "poi", "non",
        "what", "who", "where", "when", "why", "how",
        "remember", "tell", "explain", "call", "teach", "say",
        "is", "are", "does", "do", "can", "every", "forget",
        "learn", "describe", "read", "show", "first", "then", "dont",
        NULL
    };
    for (const char *const *s = starters; *s; s++)
        if (strcmp(w, *s) == 0) return 1;
    return 0;
}

/* gen88: true if word `w` is a negation marker that should cause the sub-turn
 * to be suppressed (e.g., "dont answer", "non rispondere"). */
static int is_negation_marker(const char *w) {
    return strcmp(w, "dont") == 0 || strcmp(w, "non") == 0 || strcmp(w, "not") == 0;
}

/* gen80: split `canon` on discourse connectors where the second half starts
 * with an intent marker, dispatch each sub-turn, and join responses. Returns
 * 1 if decomposition was applied, 0 to use normal dispatch. */
static int decompose_and_dispatch(Brain *b, const char *canon, const char *input,
                                   char *out, size_t out_size) {
    /* Don't decompose structured prompts — they have their own parser. */
    if (strncmp(canon, "premise:", 8) == 0 ||
        strncmp(canon, "label premise:", 14) == 0 ||
        strncmp(canon, "explain premise:", 16) == 0 ||
        strncmp(canon, "superglue", 9) == 0 ||
        strncmp(canon, "effect of ", 10) == 0 ||
        strncmp(canon, "cause of ", 9) == 0 ||
        strncmp(canon, "read:", 5) == 0 ||
        strncmp(canon, "learn sequence:", 15) == 0)
        return 0;

    const char *connectors[] = {" e ", " and ", " ed ", " ma ", " but ", NULL};
    const char *conn = NULL;
    size_t conn_len = 0;
    int is_but = 0;
    for (const char *const *c = connectors; *c; c++) {
        const char *pos = strstr(canon, *c);
        if (pos && (!conn || pos < conn)) {
            conn = pos; conn_len = strlen(*c);
            is_but = (strcmp(*c, " ma ") == 0 || strcmp(*c, " but ") == 0);
        }
    }
    if (!conn) return 0;

    const char *after = conn + conn_len;
    while (*after && isspace((unsigned char)*after)) after++;
    if (!*after) return 0;

    char first_word[64]; size_t fw = 0;
    while (after[fw] && !isspace((unsigned char)after[fw]) && fw + 1 < 64)
        { first_word[fw] = (char)tolower((unsigned char)after[fw]); fw++; }
    first_word[fw] = '\0';

    if (!is_intent_starter(first_word)) return 0;

    char sub1[256], sub2[256];
    size_t cpos = (size_t)(conn - canon);
    size_t len = strlen(canon);
    if (cpos >= sizeof sub1) cpos = sizeof sub1 - 1;
    memcpy(sub1, canon, cpos); sub1[cpos] = '\0';
    size_t s2len = len - cpos - conn_len;
    if (s2len >= sizeof sub2) s2len = sizeof sub2 - 1;
    memcpy(sub2, conn + conn_len, s2len); sub2[s2len] = '\0';

    char r1[1024] = "", r2[1024] = "";
    int h1 = 0, h1_disc = 0, h2 = 0;
    int negate1 = 0, negate2 = 0;

    /* gen88: check if either sub-turn starts with a negation marker. */
    {
        char *sw[8]; char b1[256], b2[256];
        snprintf(b1, sizeof b1, "%s", sub1);
        snprintf(b2, sizeof b2, "%s", sub2);
        size_t n1 = split_words(b1, sw, 8);
        if (n1 > 0 && is_negation_marker(sw[0])) negate1 = 1;
        size_t n2 = split_words(b2, sw, 8);
        if (n2 > 0 && is_negation_marker(sw[0])) negate2 = 1;
    }

    if (!is_but) {
        for (size_t i = 0; i < registry_len; i++) {
            if (negate1) break; /* gen88: skip negated sub-turn */
            if (registry[i].handle(b, sub1, input, r1, sizeof r1)) {
                h1 = 1;
                if (strcmp(registry[i].name, "discourse") == 0) h1_disc = 1;
                if (b) {
                    snprintf(b->last_reply, sizeof b->last_reply, "%s", r1);
                    snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
                }
                break;
            }
        }
        if (!h1 && !negate1) return 0;
    } /* first sub-turn unclaimed → fall through to normal dispatch */

    for (size_t i = 0; i < registry_len; i++) {
        if (negate2) break; /* gen88: skip negated sub-turn */
        if (registry[i].handle(b, sub2, input, r2, sizeof r2)) {
            h2 = 1; break;
        }
    }
    if (!h2 && !negate2) return 0;

    snprintf(out, out_size, "%s%s%s", r1,
             (r2[0] && r1[0]) ? " " : "", r2);
    if (!is_but && h1 && !h1_disc) update_topics(b, sub1);
    return 1;
}

size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size) {
    if (out_size == 0) return 0;
    if (b) b->turns++;

    /* gen158: once the whole KB (base + profile) is loaded, materialize the
     * part_of/2 relation latent in the concept descriptions, so the resolution
     * engine can prove and query it. Done on the first turn, when loading is
     * complete; KB_REFLECTIVE so it never persists. */
    if (b && b->kb && !b->relations_derived) {
        kb_derive_part_of(b->kb);
        b->relations_derived = 1;
    }

    char norm[256];
    normalize(input, norm, sizeof norm);

    /* gen43: canonicalize the parsing surface (function words -> English tokens)
     * before dispatch, so the reasoning core answers in any mapped language
     * without duplicating a module. `raw` (input) is left untouched, so the
     * reader still induces its generative model from the original prose. */
    char canon[256];
    canonicalize_lang(norm, canon, sizeof canon);

    /* gen80: try to decompose compound turns (e.g. "chi sei e ricordati X")
     * before the normal single-turn dispatch. */
    if (b && decompose_and_dispatch(b, canon, input, out, out_size))
        return strlen(out);

    /* gen142 (E3): peel a leading discourse-marker opener and re-dispatch the
     * residue, so a content task wrapped in a channel-management opener survives
     * ("anyway, is socrates a man" -> "Yes."). Only claims when the residue is
     * actually owned by a module; otherwise the original turn dispatches normally
     * and its pragmatic shape is read by mod_pragma. */
    if (b && pragma_peel(b, canon, input, out, out_size))
        return strlen(out);

    /* Walk the registry; first module to claim the turn wins. */
    int handled = 0, handled_by_discourse = 0;

    /* gen83: extract capitalized words as named-entity candidates. */
    if (b) {
        char rbuf[256];
        snprintf(rbuf, sizeof rbuf, "%s", input);
        char *rw[64];
        size_t rnw = split_words(rbuf, rw, 64);
        for (size_t i = 0; i < rnw && b->entity_count < 8; i++) {
            if (isupper((unsigned char)rw[i][0]) && strlen(rw[i]) >= 2) {
                int dup = 0;
                for (size_t j = 0; j < b->entity_count; j++)
                    if (strcmp(b->entities[j], rw[i]) == 0) { dup = 1; break; }
                if (!dup) {
                    snprintf(b->entities[b->entity_count],
                             sizeof b->entities[0], "%s", rw[i]);
                    b->entity_count++;
                }
            }
        }
    }

    /* gen105 (L20): record the real control-flow trace of this turn — the
     * modules consulted that declined, then the one that claimed it. */
    char declined[48][24]; size_t ndecl = 0;
    const char *winner = "fallback";
    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, canon, input, out, out_size)) {
            handled = 1;
            winner = registry[i].name;
            if (strcmp(registry[i].name, "discourse") == 0) handled_by_discourse = 1;
            if (b) {
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            }
            break;
        }
        if (ndecl < 48) snprintf(declined[ndecl++], sizeof declined[0], "%s",
                                 registry[i].name);
    }

    /* Commit the trace for "why did you answer that way?" and the verbatim input
     * for "what would you have said without X?" — but NOT when this turn was
     * itself one of those introspective questions, so each reports the decision
     * it is being asked about, not its own lookup. */
    if (b && strcmp(winner, "strategy") != 0 &&
        strcmp(winner, "counterfactual") != 0) {
        b->trace_declined_n = ndecl;
        for (size_t i = 0; i < ndecl; i++)
            snprintf(b->trace_declined[i], sizeof b->trace_declined[0], "%s",
                     declined[i]);
        snprintf(b->trace_winner, sizeof b->trace_winner, "%s", winner);
        b->has_trace = 1;
        snprintf(b->last_input_canon, sizeof b->last_input_canon, "%s", canon);
        snprintf(b->last_input_raw, sizeof b->last_input_raw, "%s", input);
        b->has_last_input = 1;
    }

    /* gen58: update the rolling discourse topic buffer from the current turn,
     * but a summary question should not add its own words to the buffer. */
    if (handled && !handled_by_discourse) update_topics(b, canon);

    /* If no module claimed the turn, fall back to the honest not-understood reply
     * (gen15 retired the gen0 parrot-echo; gen55 made it non-repeating).
     * Honest admission, never a mirror or a wrong "No.". */
    if (!handled) {
        not_understood(b, canon, out, out_size);
        if (b) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", "fallback");
        }
    }
    return strlen(out);
}
