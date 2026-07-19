#include "brain.h"
#include "kb.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pass = 0, fail = 0;

static void chk(int condition, const char *name) {
    if (condition) {
        printf("PASS brain-agent-state: %s\n", name);
        pass++;
    } else {
        fprintf(stderr, "FAIL brain-agent-state: %s\n", name);
        fail++;
    }
}

static int has_rec(KB *kb, const char *id, const char *kind,
                   const char *parent, const char *state) {
    const char *args[] = { id, kind, parent, state };
    return kb_query(kb, "rec", args, 4);
}

int main(void) {
    Brain *b = brain_create();
    char response[2048];
    char ids[2][KB_TERM_LEN] = {{0}}, parents[2][KB_TERM_LEN] = {{0}};
    const char *name_q[] = { NULL, "\"inspect_workspace\"" };
    uint64_t id;

    if (!b) return 2;
    brain_boot(b);
    brain_respond(b, "make a plan for a code task", response, sizeof response);

    KB *kb = brain_kb(b);
    size_t ni = kb_match(kb, "rec_name", name_q, 2, ids, 2);
    id = ni == 1 ? (uint64_t)strtoull(ids[0], NULL, 10) : 0;
    const char *parent_q[] = { ni == 1 ? ids[0] : "0", "action", NULL, "candidate" };
    size_t np = kb_match(kb, "rec", parent_q, 4, parents, 2);
    chk(id > 0 && np == 1 && has_rec(kb, ids[0], "action", parents[0], "candidate"),
        "the planner exposes a candidate Action record to transition");

    int moved = id > 0 && np == 1 && brain_agent_set_state(b, id, "active");
    chk(moved && !has_rec(kb, ids[0], "action", parents[0], "candidate") &&
        has_rec(kb, ids[0], "action", parents[0], "active"),
        "candidate -> active removes the old rec/4 and asserts the new rec/4");

    int reversed = moved && brain_agent_set_state(b, id, "candidate");
    chk(reversed && !has_rec(kb, ids[0], "action", parents[0], "active") &&
        has_rec(kb, ids[0], "action", parents[0], "candidate"),
        "active -> candidate is the exact inverse with no duplicate state fact");

    uint64_t bad = UINT64_MAX;
    int refused_bad = brain_agent_set_state(b, bad, "failed") == 0;
    int refused_null = brain_agent_set_state(b, id, NULL) == 0;
    chk(refused_bad && refused_null &&
        has_rec(kb, ids[0], "action", parents[0], "candidate") &&
        !has_rec(kb, ids[0], "action", parents[0], "active"),
        "invalid transitions fail without drifting the existing store/projection");

    const char *source_q[] = { ids[0], parents[0] };
    chk(ni == 1 && np == 1 && kb_query(kb, "rec_source", source_q, 2),
        "state transitions leave the separate provenance projection unchanged");

    brain_destroy(b);
    printf("brain-agent-state: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}
