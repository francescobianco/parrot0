#!/usr/bin/env bash
#
# gen336 (docs/plans/coding-agent-todo.md T01): the typed agent kernel.
#
# The claim T01 makes is not "there is a struct". It is that a coding task can be
# INTERROGATED instead of re-parsed out of prose: "where did the task stop?",
# "which action produced this artifact?", "what is still open?" must have answers
# that come from records. So every assertion here is a property of that claim, and
# each one fails loudly if the kernel drifts back toward rendered text:
#
#   dense ids       p0a_get is O(1) BECAUSE ids index the array; if a record were
#                   ever deleted or an id reused, v[id-1].id != id and the lookup
#                   silently returns the WRONG record. That is checked, not assumed.
#   append-only     a state change is the only mutation. Everything else about a
#                   record must survive it, or provenance is a lie.
#   total >= cap    p0a_select returns the TRUE match count (the snprintf pattern),
#                   so a caller with a small buffer learns it was truncated instead
#                   of believing it saw everything.
#   audit is data   the JSONL must PARSE BACK through the real json.c — a trail
#                   that only looks right when printed is prose with braces. A name
#                   containing a quote and a newline is the test that finds this.
#   KB projection   rec/4 + rec_source/2 + rec_name/2 must stay inside the KB's
#                   arity ceiling: atom state, separate provenance, quoted label.
#
# Mechanics only: no Brain, no natural language, exactly like exec.c (gen329).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-agent-kernel.$$"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP"

cat >"$TMP/driver.c" <<'EOC'
#include "agent.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pass = 0, fail = 0;

static void ok(const char *what)  { printf("PASS agentkernel: %s\n", what); pass++; }
static void no(const char *what)  { printf("FAIL agentkernel: %s\n", what); fail++; }
static void chk(int cond, const char *what) { if (cond) ok(what); else no(what); }

int main(void)
{
    P0AStore *s = p0a_new();
    const P0ARec *sel[4];
    const P0ARec *r;
    char buf[256];
    uint64_t task, goal, act, obs, art, ver, gap, dec, goal2;
    size_t total;

    if (!s) { printf("FAIL agentkernel: p0a_new returned NULL\n"); return 1; }

    /* ---- the workflow a coding task actually walks ------------------------ */
    task = p0a_add(s, P0A_TASK,  0,    0,    "fix the leak in parser.c", "open",   "{}");
    goal = p0a_add(s, P0A_GOAL,  task, task, "no leak under the repro",  "active", "{}");
    act  = p0a_add(s, P0A_ACTION, goal, goal, "run",                     "active", "{\"cmd\":\"make test\"}");
    obs  = p0a_add(s, P0A_OBSERVATION, act, act, "exit=1",               "done",   "{\"status\":1}");
    art  = p0a_add(s, P0A_ARTIFACT, goal, obs,  "parser.c.diff",         "candidate", "{\"digest\":\"ab\"}");
    ver  = p0a_add(s, P0A_VERDICT, art, art,   "compile_ok",             "done",   "{}");
    gap  = p0a_add(s, P0A_GAP,    goal, ver,   "no repair rule",         "open",   "{\"stage\":\"repair\"}");
    dec  = p0a_add(s, P0A_DECISION, goal, gap, "ask for help",           "done",   "{}");

    chk(task == 1 && goal == 2 && act == 3 && obs == 4 &&
        art == 5 && ver == 6 && gap == 7 && dec == 8,
        "ids are monotonic from 1, one per record");
    chk(p0a_count(s, -1) == 8, "the store counts every record");
    chk(p0a_count(s, P0A_GOAL) == 1 && p0a_count(s, P0A_ACTION) == 1,
        "counting by kind sees only that kind");

    /* Dense ids are the whole reason p0a_get is O(1). Prove the invariant that
     * makes the arithmetic legal, for EVERY id — not just the one we look up. */
    {
        int dense = 1;
        uint64_t i;
        for (i = 1; i <= 8; i++) {
            const P0ARec *g = p0a_get(s, i);
            if (!g || g->id != i) { dense = 0; break; }
        }
        chk(dense, "p0a_get(id)->id == id for every id (dense, never reused)");
    }
    chk(p0a_get(s, 0) == NULL && p0a_get(s, 9) == NULL,
        "id 0 and an out-of-range id return NULL, not a neighbouring record");

    /* ---- provenance: parent and source are distinct, and both survive ----- */
    r = p0a_get(s, art);
    chk(r && r->parent == goal && r->source == obs,
        "an artifact records BOTH its parent goal and the observation it came from");
    r = p0a_get(s, task);
    chk(r && r->parent == 0 && r->source == 0,
        "an external task is a root: parent 0, source 0");

    /* ---- "which action produced this artifact?" answered from records ----- */
    r = p0a_get(s, art);
    r = r ? p0a_get(s, r->source) : NULL;      /* artifact -> observation       */
    r = r ? p0a_get(s, r->parent) : NULL;      /* observation -> action         */
    chk(r && r->id == act && r->kind == P0A_ACTION,
        "the provenance chain walks artifact -> observation -> action");

    /* ---- a state change is the ONLY mutation ------------------------------ */
    {
        P0ARec before = *p0a_get(s, gap);
        chk(p0a_set_state(s, gap, "declined") == 1, "p0a_set_state reports success");
        r = p0a_get(s, gap);
        chk(r && strcmp(r->state, "declined") == 0, "the new state is stored");
        chk(r && r->id == before.id && r->kind == before.kind &&
            r->parent == before.parent && r->source == before.source &&
            r->seq == before.seq && strcmp(r->name, before.name) == 0 &&
            strcmp(r->payload, before.payload) == 0,
            "a state change touches NOTHING else (id/kind/parent/source/seq/name/payload)");
        chk(p0a_set_state(s, 999, "done") == 0,
            "setting the state of an unknown id fails instead of inventing a record");
    }

    /* ---- select: the count is the TRUTH, even past the caller's buffer ---- */
    goal2 = p0a_add(s, P0A_GOAL, task, task, "second goal", "open", "{}");
    total = p0a_select(s, P0A_GOAL, task, sel, 4);
    chk(total == 2 && sel[0]->id == goal && sel[1]->id == goal2,
        "select filters by kind AND parent, in creation order");
    total = p0a_select(s, -1, P0A_ANY, sel, 2);
    chk(total == 9, "select returns the TOTAL match count, not the number it wrote");
    chk(total > 2 && sel[0]->id == 1 && sel[1]->id == 2,
        "a truncated select still fills the buffer with the first matches");
    chk(p0a_select(s, P0A_SEGMENT, P0A_ANY, sel, 4) == 0,
        "selecting a kind with no records is 0, not an error");

    /* ---- "where did the task stop?" --------------------------------------- */
    r = p0a_last(s, P0A_GOAL, P0A_ANY);
    chk(r && r->id == goal2, "p0a_last returns the most recent match");
    r = p0a_last(s, -1, task);
    chk(r && r->id == goal2, "p0a_last honours a parent filter with kind < 0");
    chk(p0a_last(s, P0A_SEGMENT, P0A_ANY) == NULL, "p0a_last is NULL when nothing matches");

    /* ---- the audit trail is DATA: it must parse back ---------------------- */
    {
        /* A name that breaks a naive writer: a quote, a backslash, a newline. */
        uint64_t nasty = p0a_add(s, P0A_EVENT, 0, 0,
                                 "he said \"hi\"\\n\nsecond line", "done", "{\"k\":\"v\"}");
        FILE *f = fopen("trail.jsonl", "w");
        char line[4096];
        size_t lines = 0;
        int parsed_all = 1, found_nasty = 0;

        chk(f != NULL, "the JSONL trail opens for writing");
        chk(f && p0a_write_jsonl(s, f) == 0, "p0a_write_jsonl reports success");
        if (f) fclose(f);

        f = fopen("trail.jsonl", "r");
        while (f && fgets(line, sizeof line, f)) {
            JVal *v = json_parse(line, strlen(line));
            JVal *id, *kind, *name;
            lines++;
            if (!v || v->type != J_OBJ) { parsed_all = 0; jfree(v); continue; }
            id   = jobj_get(v, "id");
            kind = jobj_get(v, "kind");
            name = jobj_get(v, "name");
            if (!id || !kind || !name) { parsed_all = 0; jfree(v); continue; }
            if (id->type == J_NUM && (uint64_t)id->num == nasty) {
                found_nasty = name->type == J_STR &&
                              strcmp(name->str, "he said \"hi\"\\n\nsecond line") == 0 &&
                              kind->type == J_STR && strcmp(kind->str, "event") == 0;
            }
            jfree(v);
        }
        if (f) fclose(f);

        chk(lines == 10, "the trail is one line per record");
        chk(parsed_all, "EVERY line parses back through the real json.c");
        chk(found_nasty,
            "a name with a quote, a backslash and a newline round-trips byte-exact");
    }

    /* ---- the KB projection: a planner queries facts, not prose ------------ */
    r = p0a_get(s, art);
    p0a_fact(r, buf, sizeof buf);
    chk(strcmp(buf, "rec(5,artifact,2,candidate).") == 0,
        "p0a_fact renders loadable rec(Id,Kind,Parent,State) within KB arity 4");
    p0a_fact_source(r, buf, sizeof buf);
    chk(strcmp(buf, "rec_source(5,4).") == 0,
        "p0a_fact_source preserves provenance without an unloadable rec/5");
    p0a_fact_name(r, buf, sizeof buf);
    chk(strcmp(buf, "rec_name(5,\"parser.c.diff\").") == 0,
        "p0a_fact_name quotes the free-text label");

    /* An empty state must still project a legal atom: `rec(N,event,0,0,).` would
     * not parse, so the projection defaults to the open state. */
    {
        uint64_t bare = p0a_add(s, P0A_EVENT, 0, 0, "", "", "");
        r = p0a_get(s, bare);
        p0a_fact(r, buf, sizeof buf);
        chk(strstr(buf, ",open).") != NULL, "an empty state projects as `open`, never as a hole");
        p0a_fact_name(r, buf, sizeof buf);
        chk(buf[0] == 0, "a nameless record projects NO rec_name fact");
    }

    /* A quote inside a name would close the p0 quoted atom early. */
    {
        uint64_t q = p0a_add(s, P0A_GOAL, 0, 0, "the \"hard\" case\nwrapped", "open", "");
        p0a_fact_name(p0a_get(s, q), buf, sizeof buf);
        chk(strcmp(buf, "rec_name(12,\"the 'hard' case wrapped\").") == 0,
            "a quote and a newline inside a name cannot break the p0 atom");
    }

    /* ---- the kind vocabulary is closed ------------------------------------ */
    {
        int i, round_trips = 1;
        for (i = 0; i <= P0A_DECISION; i++)
            if (p0a_kind_from_name(p0a_kind_name((P0AKind)i)) != i) round_trips = 0;
        chk(round_trips, "every kind name round-trips through kind_from_name");
        chk(p0a_kind_from_name("planet") == -1 && p0a_kind_from_name(NULL) == -1,
            "an unknown kind name is -1, never a silent 0 (segment)");
        chk(strcmp(p0a_kind_name((P0AKind)99), "unknown") == 0,
            "an out-of-range kind renders `unknown`, not a wild read");
    }

    /* ---- NULL store: mechanics never crash on the empty case -------------- */
    chk(p0a_add(NULL, P0A_TASK, 0, 0, "x", "open", "") == 0 &&
        p0a_get(NULL, 1) == NULL && p0a_count(NULL, -1) == 0 &&
        p0a_select(NULL, -1, P0A_ANY, sel, 4) == 0 &&
        p0a_last(NULL, -1, P0A_ANY) == NULL &&
        p0a_set_state(NULL, 1, "done") == 0 &&
        p0a_write_jsonl(NULL, stdout) == -1,
        "every entry point is NULL-safe");

    p0a_free(s);
    p0a_free(NULL);

    printf("agentkernel: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}
EOC

if ! cc -std=c11 -Wall -Wextra -O2 -I"$ROOT/src" -o "$TMP/driver" \
        "$TMP/driver.c" "$ROOT/src/agent.c" "$ROOT/src/json.c" 2>"$TMP/cc.err"; then
    echo "FAIL agentkernel: could not build the test driver" >&2
    cat "$TMP/cc.err" >&2
    exit 1
fi

cd "$TMP" && exec "$TMP/driver"
