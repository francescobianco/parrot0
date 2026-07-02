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
#include <sys/stat.h>
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
    long utter_seq;        /* gen240: monotonic index for the session conversation log */
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
    struct timespec start_ts;
    int has_start_ts;

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
    /* gen217 (glue): the most recently mentioned possession's "thing" (e.g.
     * "dog"), so a possessive-pronoun anaphor ("what is his name") resolves to
     * the salient owned entity without repeating the noun. */
    char last_possession_thing[64];
    int  has_last_possession;

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
    /* gen218 (glue): set while re-dispatching the residue of an explicit
     * correction ("no, X is not a Y"), so the negation parser knows to OVERRIDE
     * any standing positive (even curated/base) instead of leaving a conflict. */
    int  correcting;

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
    char trace_declined[64][24];   /* gen206: cap >= registry length (now ~49) */
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

    /* gen205 (pi-agent): the perceive->act->observe seam for LOCAL tools. Unlike a
     * REMOTE LLM — which cannot touch the machine and must round-trip every
     * observation through its context (emit tool_call, wait, receive result) —
     * parrot0 runs ON the box. So when a turn is a read-only coding/filesystem
     * request and parrot0 is in agent mode (PARROT0_TOOLS=1), mod_piact compiles
     * the intent to a safe command, RUNS it locally itself, and folds the REAL
     * output into a grounded answer in the SAME turn. No tool-call protocol, no
     * pending cross-turn state: the double step the OpenAI tool API forces on
     * remote models is simply unnecessary here. PRINCIPLES.md's anti-impostor rule
     * still binds — every reported file/line comes from a command that actually
     * ran (recorded as the proof trace), never from a guess. The proof of the last
     * act is kept so "how do you know?" can cite the exact command. */
    char last_tool_cmd[256];    /* the exact command the last act ran           */
    int  has_last_tool_cmd;

    /* gen206 (composer): the generative operator substrate (compose.p0) is loaded
     * lazily into the REFLECTIVE layer the first time mod_compose runs, so it never
     * touches ordinary conversation or the boot KB. This guards the one-time load. */
    int  compose_kb_loaded;

    /* gen212 (KB-first responses): rotation cursor over response_template/2 phrasings,
     * so when more than one form is registered for an intent they alternate (the gen55
     * anti-repeat instinct) and a runtime-taught phrasing actually gets used. */
    unsigned response_pick;

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

/* ---------------------------------------------------------------------------
 * Module groups. brain.c is ONE translation unit: these fragments are
 * #included in source order (NOT separately compiled), so every static
 * helper, the shared `struct Brain`, and file-scope state stay visible across
 * them exactly as when this was a single 12.8k-line file. Splitting is purely
 * physical, for navigation; the preprocessed output is unchanged. Order is
 * load-bearing (definition-before-use); do not reorder these includes.
 * ------------------------------------------------------------------------- */
#include "brain/00-lex.c"
#include "brain/10-memory-knowledge.c"
#include "brain/20-math.c"
#include "brain/25-wordmath-reasoning.c"
#include "brain/30-generation-reading.c"
#include "brain/40-meta-reflection.c"
#include "brain/50-self-research-loop.c"
#include "brain/60-agent-tools.c"
#include "brain/65-induce-verify-shell.c"
#include "brain/70-social-pragma.c"
#include "brain/80-code.c"
#include "brain/85-translate-synth-world.c"
#include "brain/90-repair-robust-abduce.c"
#include "brain/99-registry.c"
