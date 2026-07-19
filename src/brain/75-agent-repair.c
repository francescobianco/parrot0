/* T06: domain-neutral, typed repair control.
 *
 * Domain adapters only materialize a named transform, observe a candidate and
 * diagnose the resulting observations.  This controller owns the loop:
 * exhaustive live-rule lookup, mechanical blast selection, digest de-dup,
 * budget, replanning and the P0A provenance graph.  In particular it contains
 * no diagnosis-specific branch and emits no user-facing wording. */
#include "../patch.h"

#include <limits.h>

#define P0_REPAIR_DEFAULT_BUDGET 3U
#define P0_REPAIR_HARD_BUDGET   32U
#define P0_REPAIR_MAX_OBS         4U

typedef enum {
    P0_REPAIR_VERIFIED = 0,
    P0_REPAIR_ALREADY_GREEN,
    P0_REPAIR_ZERO_RULE,
    P0_REPAIR_NO_APPLICABLE,
    P0_REPAIR_INFRA,
    P0_REPAIR_MISSING_TOOL,
    P0_REPAIR_BUDGET_EXHAUSTED
} P0RepairTerminal;

typedef struct {
    P0Obs obs[P0_REPAIR_MAX_OBS];
    char stage[P0_REPAIR_MAX_OBS][24];
    size_t count;
} P0RepairEvidence;

typedef struct {
    /* 1 = materialized, 0 = transform does not apply, -1 = infrastructure. */
    int (*materialize)(void *ctx, const P0PatchArtifact *base,
                       const char *transform, P0PatchArtifact **candidate);
    /* 1 = real observation(s), -2 = missing live tool contract, -1 = infra. */
    int (*observe)(void *ctx, const P0PatchArtifact *candidate,
                   P0RepairEvidence *evidence);
    /* 1 = green, 0 = red diagnosis, -1 = infrastructure verdict. */
    int (*diagnose)(void *ctx, const P0RepairEvidence *evidence,
                    char *diagnosis, size_t diagnosis_cap);
} P0RepairOps;

typedef struct {
    char transform[KB_TERM_LEN];
    char before[KB_TERM_LEN];
    char after[KB_TERM_LEN];
    size_t blast;
} P0RepairAttempt;

typedef struct {
    P0RepairTerminal terminal;
    uint64_t task_id;
    uint64_t goal_id;
    uint64_t gap_id;
    unsigned budget;
    unsigned attempts;
    size_t last_rule_count;
    char initial_diagnosis[KB_TERM_LEN];
    char last_diagnosis[KB_TERM_LEN];
    char winner_transform[KB_TERM_LEN];
    P0RepairAttempt history[P0_REPAIR_HARD_BUDGET];
    P0PatchArtifact *winner;
} P0RepairResult;

typedef struct {
    P0PatchArtifact *patch;
    uint64_t action_id;
    uint64_t artifact_id;
    size_t blast;
    char transform[KB_TERM_LEN];
    char state_digest[P0_PATCH_DIGEST_HEX];
} P0RepairCandidate;

static const char *agent_repair_terminal_name(P0RepairTerminal t) {
    switch (t) {
        case P0_REPAIR_VERIFIED:         return "verified";
        case P0_REPAIR_ALREADY_GREEN:    return "already_green";
        case P0_REPAIR_ZERO_RULE:        return "zero_rule";
        case P0_REPAIR_NO_APPLICABLE:    return "no_applicable";
        case P0_REPAIR_MISSING_TOOL:     return "missing_tool_contract";
        case P0_REPAIR_BUDGET_EXHAUSTED: return "budget_exhausted";
        default:                         return "infra";
    }
}

static int agent_repair_fact(Brain *b, const char *pred,
                             const char *const *args, size_t argc) {
    if (!b || !b->kb) return 0;
    kb_set_origin(b->kb, KB_REFLECTIVE);
    int ok = kb_assert(b->kb, pred, args, argc);
    kb_set_origin(b->kb, KB_SESSION);
    return ok;
}

/* kb_match reports a capped count.  Re-query with a growing result vector until
 * the count falls below the cap; silently taking the first N rules would make
 * runtime knowledge depend on insertion order. */
static int agent_repair_match_all(const KB *kb, const char *pred,
                                  const char *const *args, size_t argc,
                                  char (**rows_out)[KB_TERM_LEN], size_t *n_out) {
    size_t cap = 8;
    char (*rows)[KB_TERM_LEN] = NULL;
    if (!rows_out || !n_out) return 0;
    *rows_out = NULL;
    *n_out = 0;
    for (;;) {
        if (cap > SIZE_MAX / sizeof *rows) { free(rows); return 0; }
        char (*grown)[KB_TERM_LEN] = realloc(rows, cap * sizeof *rows);
        if (!grown) { free(rows); return 0; }
        rows = grown;
        size_t n = kb_match(kb, pred, args, argc, rows, cap);
        if (n < cap) {
            *rows_out = rows;
            *n_out = n;
            return 1;
        }
        if (cap > SIZE_MAX / 2) { free(rows); return 0; }
        cap *= 2;
    }
}

static unsigned agent_repair_budget(Brain *b, const char *domain, int *ok) {
    unsigned budget = P0_REPAIR_DEFAULT_BUDGET;
    unsigned minimum = UINT_MAX;
    int have_valid = 0;
    char (*values)[KB_TERM_LEN] = NULL;
    size_t n = 0;
    const char *q[] = { domain, NULL };
    *ok = agent_repair_match_all(b->kb, "repair_budget", q, 2, &values, &n);
    if (!*ok) return budget;
    for (size_t i = 0; i < n; i++) {
        char *end = NULL;
        unsigned long v = strtoul(values[i], &end, 10);
        if (end != values[i] && *end == '\0' && v > 0 && v <= UINT_MAX) {
            if ((unsigned)v < minimum) minimum = (unsigned)v;
            have_valid = 1;
        }
    }
    free(values);
    if (have_valid) budget = minimum;
    if (budget > P0_REPAIR_HARD_BUDGET) budget = P0_REPAIR_HARD_BUDGET;
    return budget;
}

static size_t agent_repair_add_sat(size_t a, size_t b) {
    return b > SIZE_MAX - a ? SIZE_MAX : a + b;
}

/* Edit size is derived from immutable byte images, never trusted from the
 * domain transform.  A replacement is the larger non-common middle; an insert
 * or delete is therefore its exact byte width. */
static size_t agent_repair_blast(const P0PatchArtifact *patch) {
    size_t total = 0;
    size_t count = p0_patch_count(patch);
    for (size_t i = 0; i < count; i++) {
        P0PatchOpView op;
        if (!p0_patch_op(patch, i, &op)) return SIZE_MAX;
        const unsigned char *before = op.has_before ? op.before : NULL;
        const unsigned char *after = op.has_after ? op.after : NULL;
        size_t bn = op.has_before ? op.before_len : 0;
        size_t an = op.has_after ? op.after_len : 0;
        size_t prefix = 0;
        while (prefix < bn && prefix < an && before[prefix] == after[prefix])
            prefix++;
        size_t suffix = 0;
        while (suffix < bn - prefix && suffix < an - prefix &&
               before[bn - 1 - suffix] == after[an - 1 - suffix])
            suffix++;
        size_t bm = bn - prefix - suffix;
        size_t am = an - prefix - suffix;
        size_t delta = bm > am ? bm : am;
        if (op.kind == P0_PATCH_RENAME && delta == 0) {
            size_t p1 = op.path ? strlen(op.path) : 0;
            size_t p2 = op.path2 ? strlen(op.path2) : 0;
            delta = p1 > p2 ? p1 : p2;
        }
        total = agent_repair_add_sat(total, delta);
    }
    return total;
}

static void agent_repair_hash_bytes(uint64_t *h, const void *data, size_t n) {
    const unsigned char *p = data;
    for (size_t i = 0; i < n; i++) {
        *h ^= p[i];
        *h *= UINT64_C(1099511628211);
    }
}

/* PatchArtifact digest binds preimage+postimage.  Cycle detection instead needs
 * the resulting state: for one detached image use its SHA-256 postimage digest;
 * for a general artifact fold the ordered paths/kinds/postimage digests.  The
 * latter is only a bounded compatibility fallback (FNV64 has a collision
 * ceiling); repo/multi-file repair must switch to a PatchArtifact poststate
 * SHA-256 API before it can use this controller as a release-grade deduper. */
static int agent_repair_state_digest(const P0PatchArtifact *patch,
                                     char out[P0_PATCH_DIGEST_HEX]) {
    size_t count = p0_patch_count(patch);
    P0PatchOpView op;
    if (count == 1 && p0_patch_op(patch, 0, &op) && op.has_after &&
        op.after_digest && *op.after_digest) {
        snprintf(out, P0_PATCH_DIGEST_HEX, "%s", op.after_digest);
        return 1;
    }
    uint64_t h = UINT64_C(1469598103934665603);
    for (size_t i = 0; i < count; i++) {
        if (!p0_patch_op(patch, i, &op)) return 0;
        agent_repair_hash_bytes(&h, &op.kind, sizeof op.kind);
        if (op.path) agent_repair_hash_bytes(&h, op.path, strlen(op.path) + 1);
        if (op.path2) agent_repair_hash_bytes(&h, op.path2, strlen(op.path2) + 1);
        if (op.has_after && op.after_digest)
            agent_repair_hash_bytes(&h, op.after_digest, strlen(op.after_digest));
    }
    snprintf(out, P0_PATCH_DIGEST_HEX, "%016llx", (unsigned long long)h);
    return 1;
}

static int agent_repair_seen(char seen[][P0_PATCH_DIGEST_HEX], size_t n,
                             const char *digest) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(seen[i], digest) == 0) return 1;
    return 0;
}

static uint64_t agent_repair_observation(Brain *b, uint64_t action_id,
                                         uint64_t artifact_id,
                                         const char *stage, const P0Obs *obs) {
    char payload[P0A_PAYLOAD];
    snprintf(payload, sizeof payload,
             "{\"verdict\":\"%s\",\"exit\":%d,\"signal\":%d,"
             "\"duration_ms\":%ld,\"digest\":\"%s\","
             "\"out_truncated\":%d,\"err_truncated\":%d}",
             p0_verdict_name(obs->verdict), obs->exit_code, obs->term_signal,
             obs->duration_ms, obs->digest, obs->out_truncated,
             obs->err_truncated);
    uint64_t id = brain_agent_record(b, P0A_OBSERVATION, action_id, action_id,
                                     stage, "done", payload);
    if (id) {
        char id_s[32], artifact_s[32];
        snprintf(id_s, sizeof id_s, "%llu", (unsigned long long)id);
        snprintf(artifact_s, sizeof artifact_s, "%llu",
                 (unsigned long long)artifact_id);
        const char *args[] = { id_s, artifact_s, stage,
                               p0_verdict_name(obs->verdict) };
        agent_repair_fact(b, "repair_observation", args, 4);
    }
    return id;
}

/* Observe and diagnose one artifact, preserving every real P0Obs. */
static int agent_repair_verify(Brain *b, const P0RepairOps *ops, void *ctx,
                               uint64_t artifact_id,
                               const P0PatchArtifact *patch,
                               uint64_t *verdict_id, uint64_t *last_source,
                               char *diagnosis, size_t diagnosis_cap) {
    *verdict_id = 0;
    *last_source = artifact_id;
    uint64_t action = brain_agent_record(b, P0A_ACTION, artifact_id, artifact_id,
                                         "verify_candidate", "active", "{}");
    if (!action) return -1;

    P0RepairEvidence evidence;
    memset(&evidence, 0, sizeof evidence);
    int observed = ops->observe(ctx, patch, &evidence);
    if (observed != 1 || evidence.count == 0 ||
        evidence.count > P0_REPAIR_MAX_OBS) {
        brain_agent_set_state(b, action, "failed");
        *last_source = action;
        return observed == -2 ? -2 : -1;
    }

    uint64_t obs_id = 0;
    for (size_t i = 0; i < evidence.count; i++) {
        obs_id = agent_repair_observation(b, action, artifact_id,
                                          evidence.stage[i], &evidence.obs[i]);
        if (!obs_id) {
            brain_agent_set_state(b, action, "failed");
            return -1;
        }
    }
    *last_source = obs_id;

    int disposition = ops->diagnose(ctx, &evidence, diagnosis, diagnosis_cap);
    const char *vname = disposition == 1 ? "verified" :
                        disposition == 0 ? diagnosis : "infra_error";
    *verdict_id = brain_agent_record(b, P0A_VERDICT, artifact_id, obs_id,
                                     vname, disposition == 1 ? "done" : "failed",
                                     "{}");
    if (!*verdict_id) {
        brain_agent_set_state(b, action, "failed");
        return -1;
    }

    char verdict_s[32], artifact_s[32], obs_s[32];
    snprintf(verdict_s, sizeof verdict_s, "%llu",
             (unsigned long long)*verdict_id);
    snprintf(artifact_s, sizeof artifact_s, "%llu",
             (unsigned long long)artifact_id);
    snprintf(obs_s, sizeof obs_s, "%llu", (unsigned long long)obs_id);
    const char *vargs[] = { verdict_s, artifact_s, obs_s, vname };
    agent_repair_fact(b, "repair_verdict", vargs, 4);
    brain_agent_set_state(b, action, disposition < 0 ? "failed" : "done");
    return disposition;
}

static void agent_repair_terminal_fact(Brain *b, const P0RepairResult *result) {
    char goal_s[32], attempt_s[32], gap_s[32];
    snprintf(goal_s, sizeof goal_s, "%llu",
             (unsigned long long)result->goal_id);
    snprintf(attempt_s, sizeof attempt_s, "%u", result->attempts);
    snprintf(gap_s, sizeof gap_s, "%llu",
             (unsigned long long)result->gap_id);
    const char *args[] = { goal_s, agent_repair_terminal_name(result->terminal),
                           attempt_s, gap_s };
    agent_repair_fact(b, "repair_terminal", args, 4);
}

static void agent_repair_finish_gap(Brain *b, P0RepairResult *result,
                                    P0RepairTerminal terminal,
                                    uint64_t source) {
    const char *name = "repair_infra";
    const char *state = "open";
    if (terminal == P0_REPAIR_ZERO_RULE) name = "repair_zero_rule";
    else if (terminal == P0_REPAIR_NO_APPLICABLE) name = "repair_no_applicable";
    else if (terminal == P0_REPAIR_MISSING_TOOL) name = "missing_tool_contract";
    else if (terminal == P0_REPAIR_BUDGET_EXHAUSTED) {
        name = "repair_budget";
        state = "budget_exhausted";
    }
    result->terminal = terminal;
    result->gap_id = brain_agent_record(b, P0A_GAP, result->goal_id, source,
                                        name, state, "{}");
    const char *blocked = terminal == P0_REPAIR_BUDGET_EXHAUSTED
                            ? "budget_exhausted" : "blocked";
    brain_agent_set_state(b, result->goal_id, blocked);
    brain_agent_set_state(b, result->task_id, blocked);
    agent_repair_terminal_fact(b, result);
}

static int agent_repair_candidate_cmp(const P0RepairCandidate *a,
                                      const P0RepairCandidate *b) {
    if (a->blast != b->blast) return a->blast < b->blast ? -1 : 1;
    int by_name = strcmp(a->transform, b->transform);
    if (by_name) return by_name;
    return strcmp(a->state_digest, b->state_digest);
}

/* Consumes `initial`.  On success result->winner owns the final artifact;
 * otherwise every PatchArtifact is freed before return. */
static int agent_repair_run(Brain *b, const char *domain,
                            P0PatchArtifact *initial,
                            const P0RepairOps *ops, void *ctx,
                            P0RepairResult *result) {
    if (!b || !b->kb || !domain || !initial || !ops || !ops->materialize ||
        !ops->observe || !ops->diagnose || !result) {
        p0_patch_free(initial);
        return 0;
    }
    memset(result, 0, sizeof *result);
    result->terminal = P0_REPAIR_INFRA;
    result->task_id = brain_agent_record(b, P0A_TASK, 0, 0,
                                         "repair_task", "active", "{}");
    result->goal_id = brain_agent_record(b, P0A_GOAL, result->task_id,
                                         result->task_id, domain, "active", "{}");
    if (!result->task_id || !result->goal_id) {
        p0_patch_free(initial);
        return 0;
    }

    int budget_ok = 0;
    result->budget = agent_repair_budget(b, domain, &budget_ok);
    if (!budget_ok) {
        p0_patch_free(initial);
        agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, result->goal_id);
        return 1;
    }

    char state_digest[P0_PATCH_DIGEST_HEX];
    if (!agent_repair_state_digest(initial, state_digest)) {
        p0_patch_free(initial);
        agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, result->goal_id);
        return 1;
    }
    char payload[P0A_PAYLOAD];
    snprintf(payload, sizeof payload,
             "{\"patch_digest\":\"%s\",\"state_digest\":\"%s\"}",
             p0_patch_digest(initial), state_digest);
    uint64_t artifact_id = brain_agent_record(b, P0A_ARTIFACT, result->goal_id,
                                              result->task_id, "candidate_patch",
                                              "candidate", payload);
    if (!artifact_id) {
        p0_patch_free(initial);
        return 0;
    }

    char seen[P0_REPAIR_HARD_BUDGET + 1][P0_PATCH_DIGEST_HEX];
    size_t seen_n = 1;
    snprintf(seen[0], sizeof seen[0], "%s", state_digest);
    P0PatchArtifact *current = initial;
    char diagnosis[KB_TERM_LEN] = "";
    uint64_t verdict_id = 0, source_id = artifact_id;
    int disposition = agent_repair_verify(b, ops, ctx, artifact_id, current,
                                           &verdict_id, &source_id,
                                           diagnosis, sizeof diagnosis);
    if (disposition < 0) {
        brain_agent_set_state(b, artifact_id, "failed");
        p0_patch_free(current);
        agent_repair_finish_gap(b, result,
                                disposition == -2 ? P0_REPAIR_MISSING_TOOL
                                                  : P0_REPAIR_INFRA,
                                source_id);
        return 1;
    }
    if (disposition == 1) {
        brain_agent_set_state(b, artifact_id, "done");
        brain_agent_set_state(b, result->goal_id, "done");
        brain_agent_set_state(b, result->task_id, "done");
        result->terminal = P0_REPAIR_ALREADY_GREEN;
        result->winner = current;
        agent_repair_terminal_fact(b, result);
        return 1;
    }
    brain_agent_set_state(b, artifact_id, "failed");
    snprintf(result->initial_diagnosis, sizeof result->initial_diagnosis,
             "%s", diagnosis);
    snprintf(result->last_diagnosis, sizeof result->last_diagnosis,
             "%s", diagnosis);

    for (;;) {
        /* Query every rule for every newly observed diagnosis, even when the
         * next control decision is budget exhaustion. */
        char (*rules)[KB_TERM_LEN] = NULL;
        size_t nrules = 0;
        const char *q[] = { diagnosis, NULL };
        if (!agent_repair_match_all(b->kb, "repair_rule", q, 2,
                                    &rules, &nrules)) {
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, verdict_id);
            return 1;
        }
        result->last_rule_count = nrules;
        if (result->attempts >= result->budget) {
            free(rules);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_BUDGET_EXHAUSTED,
                                    verdict_id);
            return 1;
        }
        if (nrules == 0) {
            free(rules);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_ZERO_RULE, verdict_id);
            return 1;
        }

        P0RepairCandidate *candidates = calloc(nrules, sizeof *candidates);
        if (!candidates) {
            free(rules);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, verdict_id);
            return 1;
        }
        size_t nc = 0;
        int materialize_infra = 0;
        for (size_t i = 0; i < nrules; i++) {
            uint64_t action = brain_agent_record(b, P0A_ACTION, result->goal_id,
                                                 verdict_id, rules[i],
                                                 "candidate", "{}");
            P0PatchArtifact *patch = NULL;
            int made = action ? ops->materialize(ctx, current, rules[i], &patch) : -1;
            if (made < 0) {
                if (action) brain_agent_set_state(b, action, "failed");
                p0_patch_free(patch);
                materialize_infra = 1;
                break;
            }
            if (made == 0 || !patch) {
                if (action) brain_agent_set_state(b, action, "declined");
                p0_patch_free(patch);
                continue;
            }
            char digest[P0_PATCH_DIGEST_HEX];
            size_t blast = agent_repair_blast(patch);
            if (!agent_repair_state_digest(patch, digest) || blast == SIZE_MAX) {
                brain_agent_set_state(b, action, "failed");
                p0_patch_free(patch);
                materialize_infra = 1;
                break;
            }
            char apayload[P0A_PAYLOAD];
            snprintf(apayload, sizeof apayload,
                     "{\"patch_digest\":\"%s\",\"state_digest\":\"%s\","
                     "\"blast\":%zu}", p0_patch_digest(patch), digest, blast);
            uint64_t aid = brain_agent_record(b, P0A_ARTIFACT, action, action,
                                              "candidate_patch", "candidate",
                                              apayload);
            if (!aid) {
                brain_agent_set_state(b, action, "failed");
                p0_patch_free(patch);
                materialize_infra = 1;
                break;
            }
            char goal_s[32], artifact_s[32], blast_s[32];
            snprintf(goal_s, sizeof goal_s, "%llu",
                     (unsigned long long)result->goal_id);
            snprintf(artifact_s, sizeof artifact_s, "%llu",
                     (unsigned long long)aid);
            snprintf(blast_s, sizeof blast_s, "%zu", blast);
            const char *fargs[] = { goal_s, artifact_s, rules[i], blast_s };
            agent_repair_fact(b, "repair_candidate", fargs, 4);

            int duplicate = agent_repair_seen(seen, seen_n, digest);
            for (size_t j = 0; !duplicate && j < nc; j++)
                duplicate = strcmp(candidates[j].state_digest, digest) == 0;
            if (duplicate) {
                brain_agent_set_state(b, aid, "declined");
                brain_agent_set_state(b, action, "declined");
                p0_patch_free(patch);
                continue;
            }
            candidates[nc].patch = patch;
            candidates[nc].action_id = action;
            candidates[nc].artifact_id = aid;
            candidates[nc].blast = blast;
            snprintf(candidates[nc].transform, sizeof candidates[nc].transform,
                     "%s", rules[i]);
            snprintf(candidates[nc].state_digest,
                     sizeof candidates[nc].state_digest, "%s", digest);
            nc++;
        }
        free(rules);
        if (materialize_infra) {
            for (size_t i = 0; i < nc; i++) p0_patch_free(candidates[i].patch);
            free(candidates);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, verdict_id);
            return 1;
        }
        if (nc == 0) {
            free(candidates);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_NO_APPLICABLE,
                                    verdict_id);
            return 1;
        }

        size_t selected = 0;
        for (size_t i = 1; i < nc; i++)
            if (agent_repair_candidate_cmp(&candidates[i],
                                           &candidates[selected]) < 0)
                selected = i;
        for (size_t i = 0; i < nc; i++) {
            if (i == selected) continue;
            brain_agent_set_state(b, candidates[i].artifact_id, "declined");
            brain_agent_set_state(b, candidates[i].action_id, "declined");
            p0_patch_free(candidates[i].patch);
        }
        P0RepairCandidate chosen = candidates[selected];
        free(candidates);
        brain_agent_set_state(b, chosen.action_id, "done");
        brain_agent_set_state(b, chosen.artifact_id, "active");
        uint64_t decision = brain_agent_record(b, P0A_DECISION, result->goal_id,
                                               chosen.artifact_id, "min_blast",
                                               "done", "{}");
        if (!decision) {
            p0_patch_free(chosen.patch);
            p0_patch_free(current);
            agent_repair_finish_gap(b, result, P0_REPAIR_INFRA, verdict_id);
            return 1;
        }
        char goal_s[32], artifact_s[32], blast_s[32];
        snprintf(goal_s, sizeof goal_s, "%llu",
                 (unsigned long long)result->goal_id);
        snprintf(artifact_s, sizeof artifact_s, "%llu",
                 (unsigned long long)chosen.artifact_id);
        snprintf(blast_s, sizeof blast_s, "%zu", chosen.blast);
        const char *dargs[] = { goal_s, artifact_s, chosen.transform, blast_s };
        agent_repair_fact(b, "repair_decision", dargs, 4);

        P0RepairAttempt *history = &result->history[result->attempts];
        snprintf(history->transform, sizeof history->transform, "%s",
                 chosen.transform);
        snprintf(history->before, sizeof history->before, "%s", diagnosis);
        history->blast = chosen.blast;
        result->attempts++;
        p0_patch_free(current);
        current = chosen.patch;
        artifact_id = chosen.artifact_id;
        snprintf(seen[seen_n++], sizeof seen[0], "%s", chosen.state_digest);

        char next_diag[KB_TERM_LEN] = "";
        source_id = decision;
        disposition = agent_repair_verify(b, ops, ctx, artifact_id, current,
                                           &verdict_id, &source_id,
                                           next_diag, sizeof next_diag);
        if (disposition < 0) {
            brain_agent_set_state(b, artifact_id, "failed");
            p0_patch_free(current);
            agent_repair_finish_gap(b, result,
                                    disposition == -2 ? P0_REPAIR_MISSING_TOOL
                                                      : P0_REPAIR_INFRA,
                                    source_id);
            return 1;
        }
        char attempt_s[32];
        snprintf(attempt_s, sizeof attempt_s, "%u", result->attempts);
        snprintf(history->after, sizeof history->after, "%s",
                 disposition == 1 ? "verified" : next_diag);
        const char *aargs[] = { goal_s, attempt_s, history->before,
                                history->after };
        agent_repair_fact(b, "repair_attempt", aargs, 4);

        if (disposition == 1) {
            brain_agent_set_state(b, artifact_id, "done");
            brain_agent_set_state(b, result->goal_id, "done");
            brain_agent_set_state(b, result->task_id, "done");
            result->terminal = P0_REPAIR_VERIFIED;
            result->winner = current;
            snprintf(result->winner_transform,
                     sizeof result->winner_transform, "%s", chosen.transform);
            snprintf(result->last_diagnosis, sizeof result->last_diagnosis,
                     "%s", "verified");
            agent_repair_terminal_fact(b, result);
            return 1;
        }
        brain_agent_set_state(b, artifact_id, "failed");
        snprintf(diagnosis, sizeof diagnosis, "%s", next_diag);
        snprintf(result->last_diagnosis, sizeof result->last_diagnosis,
                 "%s", diagnosis);
    }
}
