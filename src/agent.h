/*
 * agent.h - the typed agent kernel (coding-agent-todo T01).
 *
 * Why this file exists. The contract every brain module implements today is
 *
 *     typedef int (*Handler)(Brain *, const char *norm, const char *raw,
 *                            char *out, size_t out_size);
 *
 * — a faculty can WIN a turn and write text, or LOSE it. A coding task does not
 * fit inside one won-or-lost turn: it is a goal decomposed into actions, each
 * action producing observations, artifacts and verdicts, with gaps named along
 * the way. Until now that state has been smuggled inside rendered sentences,
 * and a rendered sentence cannot be queried: "where did the task stop?", "which
 * action produced this artifact?", "what is still open?" have no answer that is
 * not a re-parse of prose. That is the same disease universal-input cured in
 * perception (gen332), one organ up: text treated as structure.
 *
 * The spine is ten record kinds:
 *
 *     SEGMENT     a typed span of the input flow (from input_segment, gen332)
 *     EVENT       something observed, immutable, with a source
 *     TASK        an external request: constraints, workspace, budget, policy
 *     GOAL        a desired state, with a parent and an epistemic state
 *     CONSTRAINT  a property a solution must satisfy
 *     ACTION      a schema instance: arguments, risk, expected cost
 *     OBSERVATION the real result of an act (a P0Obs, or a KB query answer)
 *     ARTIFACT    a file/diff/patch/report, with a content digest
 *     VERDICT     an oracle's outcome, with its evidence
 *     GAP         a named hole: stage, missing requirement, causal trace
 *
 * plus DECISION, the record of why one candidate was chosen over the others.
 *
 * Every record carries: a stable id, a parent, a provenance link (the record it
 * was derived FROM), a logical sequence number, a short name, a state from the
 * fixed vocabulary below, and a kind-specific payload. Records are never
 * deleted; a state change is the only mutation, so the trace stays append-only.
 *
 * KB-first, by construction: this module is mechanics — ids, parentage,
 * sequence, JSONL — and like exec.c it has no Brain dependency and no natural
 * language anywhere. The vocabulary INSIDE records (goal text, constraint text)
 * is data. And the store projects every record as KB facts (p0a_fact plus the
 * provenance/name projections), so the planner queries state instead of
 * inferring it from text.
 *
 * State vocabulary (mechanics, the p0_verdict_name precedent of exec.c):
 * open, active, candidate, done, failed, declined, blocked, budget_exhausted.
 */
#ifndef PARROT0_AGENT_H
#define PARROT0_AGENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define P0A_NAME    128   /* short label: goal text, schema name, artifact path  */
#define P0A_STATE    24   /* state vocabulary word (see header comment)          */
#define P0A_PAYLOAD 768   /* kind-specific data (JSON fragment or plain text)    */

/* Select-everything sentinel for p0a_select/p0a_last parent filters. */
#define P0A_ANY UINT64_MAX

typedef enum {
    P0A_SEGMENT = 0,
    P0A_EVENT,
    P0A_TASK,
    P0A_GOAL,
    P0A_CONSTRAINT,
    P0A_ACTION,
    P0A_OBSERVATION,
    P0A_ARTIFACT,
    P0A_VERDICT,
    P0A_GAP,
    P0A_DECISION
} P0AKind;

typedef struct {
    uint64_t id;                      /* stable, monotonic from 1, never reused  */
    P0AKind  kind;
    uint64_t parent;                  /* 0 = root                                */
    uint64_t source;                  /* provenance: derived-FROM, 0 = external  */
    uint64_t seq;                     /* logical clock at creation               */
    char     name[P0A_NAME];
    char     state[P0A_STATE];
    char     payload[P0A_PAYLOAD];
} P0ARec;

typedef struct P0AStore P0AStore;

P0AStore   *p0a_new(void);
void        p0a_free(P0AStore *s);

/* Append a record; returns its id (>= 1), or 0 on allocation failure. name,
 * state and payload are copied; NULL becomes "". */
uint64_t    p0a_add(P0AStore *s, P0AKind kind, uint64_t parent, uint64_t source,
                    const char *name, const char *state, const char *payload);

/* O(1) by construction: ids are dense and records are never deleted. NULL for
 * id 0 or out of range. The pointer is invalidated by the next p0a_add. */
const P0ARec *p0a_get(const P0AStore *s, uint64_t id);

/* The only mutation allowed. Returns 1 on success, 0 if the id is unknown. */
int         p0a_set_state(P0AStore *s, uint64_t id, const char *state);

/* Count records; kind < 0 counts everything. */
size_t      p0a_count(const P0AStore *s, int kind);

/* Collect matching records in creation order. kind < 0 matches any kind;
 * parent == P0A_ANY matches any parent. Writes up to cap pointers into out
 * (invalidated by the next add) and returns the TOTAL number of matches, which
 * may exceed cap — the snprintf pattern. */
size_t      p0a_select(const P0AStore *s, int kind, uint64_t parent,
                       const P0ARec **out, size_t cap);

/* The most recent match, or NULL. */
const P0ARec *p0a_last(const P0AStore *s, int kind, uint64_t parent);

/* Serialize the whole store as JSONL (one record per line, append-friendly).
 * Returns 0 on success, -1 on a stream error. */
int         p0a_write_jsonl(const P0AStore *s, FILE *out);

/* KB projection. The engine's public arity ceiling is KB_MAX_ARGS=4, so the
 * structural record is deliberately split instead of emitting an unloadable
 * rec/5 term:
 *     rec(Id,Kind,Parent,State).
 *     rec_source(Id,Source).
 * p0a_fact_name renders the label fact (only when name is non-empty)
 *     rec_name(Id,"Name").
 * so a planner can query task state with kb_match instead of parsing text. */
void        p0a_fact(const P0ARec *r, char *buf, size_t cap);
void        p0a_fact_source(const P0ARec *r, char *buf, size_t cap);
void        p0a_fact_name(const P0ARec *r, char *buf, size_t cap);

const char *p0a_kind_name(P0AKind kind);
int         p0a_kind_from_name(const char *name);   /* P0AKind or -1            */

#endif /* PARROT0_AGENT_H */
