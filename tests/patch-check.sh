#!/usr/bin/env bash
#
# T06 bridge (dossier §10.4): the candidate->oracle->policy->commit boundary.
# A persisted repository-bound PatchArtifact is re-materialized by
# p0_patch_check into an isolated candidate tree, the oracle runs build and
# run as two distinct P0Obs via p0_exec_at on the borrowed stage rootfd, and
# only a live policy callback plus a green canonical post-check lets a byte
# cross into the canonical workspace.  The negative cases are the causal
# claims: a red candidate moves nothing, an oracle that mutates a touched
# path cannot certify it, a denied policy writes zero bytes, and a red
# canonical post-check restores byte/path/mode.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-patch-check.$$"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP"

cat >"$TMP/driver.c" <<'EOC'
#define _GNU_SOURCE
#include "patch.h"
#include "exec.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* The oracle: the tree's calc.c plus a fixed harness fed to cc over stdin;
 * build and run stay two separate P0Obs.  exit 0 iff answer() == 42. */
static const char HARNESS[] =
    "int answer(void); int main(void) { return answer() == 42 ? 0 : 1; }\n";

typedef struct {
    int has_build;
    int has_run;
    P0Obs build;
    P0Obs run;
} OracleObs;

static int file_is(const char *path, const char *want, mode_t mode) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    char got[256];
    size_t n = fread(got, 1, sizeof got, f);
    int extra = fgetc(f);
    fclose(f);
    struct stat st;
    return extra == EOF && n == strlen(want) && memcmp(got, want, n) == 0 &&
           stat(path, &st) == 0 && (st.st_mode & 07777) == mode;
}

/* Run the oracle inside a candidate tree (borrowed rootfd) via p0_exec_at. */
static int oracle_at(int rootfd, OracleObs *obs) {
    memset(obs, 0, sizeof *obs);
    char *cc_argv[] = {
        (char *)"cc", (char *)"-w", (char *)"-x", (char *)"c",
        (char *)"-", (char *)"calc.c", (char *)"-o", (char *)"judge", NULL
    };
    obs->has_build = 1;
    p0_exec_at(rootfd, cc_argv, NULL, 20000, HARNESS, &obs->build);
    if (!p0_obs_ok(&obs->build)) {
        (void)unlinkat(rootfd, "judge", 0);
        return 0;
    }
    char *run_argv[] = { (char *)"./judge", NULL };
    obs->has_run = 1;
    p0_exec_at(rootfd, run_argv, NULL, 15000, NULL, &obs->run);
    (void)unlinkat(rootfd, "judge", 0);
    return obs->run.verdict == P0_OK;
}

static int stage_oracle(int rootfd, const char *root, void *ctx,
                        char *why, size_t cap) {
    (void)root;
    OracleObs *obs = ctx;
    int green = oracle_at(rootfd, obs);
    if (!green) snprintf(why, cap, "candidate oracle red");
    return green;
}

static int mutating_oracle(int rootfd, const char *root, void *ctx,
                           char *why, size_t cap) {
    (void)root; (void)ctx;
    int fd = openat(rootfd, "calc.c",
                    O_WRONLY | O_TRUNC | O_CLOEXEC | O_NOFOLLOW);
    if (fd >= 0) {
        const char fraud[] = "int answer(void) { return 42; } /* fraud */\n";
        (void)write(fd, fraud, sizeof fraud - 1);
        close(fd);
    }
    snprintf(why, cap, "oracle touched a path under judgement");
    return 1;   /* claims green after mutating: the re-verify must catch it */
}

static int authorize(const P0PatchArtifact *a, void *ctx, char *why,
                     size_t cap) {
    (void)ctx;
    if (p0_patch_count(a) == 0) { snprintf(why, cap, "empty"); return 0; }
    return 1;
}

static int deny(const P0PatchArtifact *a, void *ctx, char *why, size_t cap) {
    (void)a; (void)ctx;
    snprintf(why, cap, "policy_absent");
    return 0;
}

/* The canonical post-check: re-run the same oracle against the COMMITTED
 * bytes, opening the canonical root as the capability. */
static int canonical_oracle(const char *root, void *ctx, char *why,
                            size_t cap) {
    OracleObs *obs = ctx;
    int rootfd = open(root, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (rootfd < 0) { snprintf(why, cap, "canonical_root_open"); return 0; }
    int green = oracle_at(rootfd, obs);
    close(rootfd);
    if (!green) snprintf(why, cap, "canonical oracle red");
    return green;
}

static int red_post_check(const char *root, void *ctx, char *why,
                          size_t cap) {
    (void)ctx;
    if (!file_is("calc.c", "int answer(void) { return 42; }\n", 0750))
        snprintf(why, cap, "patch was not fully applied");
    else
        snprintf(why, cap, "deliberate red canonical oracle");
    (void)root;
    return 0;
}

static P0PatchArtifact *prepare_fix(P0PatchReport *report, const char *after) {
    P0PatchSpec spec = { P0_PATCH_EDIT, "calc.c", NULL,
                         after, strlen(after), 0 };
    P0PatchArtifact *a = NULL;
    if (p0_patch_prepare(&spec, 1, NULL, NULL, &a, report) != P0_PATCH_OK)
        return NULL;
    return a;
}

static int save_artifact(const char *path, const P0PatchArtifact *a,
                         P0PatchReport *report) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    P0PatchResult r = p0_patch_write(a, f, report);
    int closed = fclose(f) == 0;
    return r == P0_PATCH_OK && closed;
}

static P0PatchArtifact *load_artifact(const char *path, P0PatchReport *report) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    P0PatchArtifact *a = NULL;
    P0PatchResult r = p0_patch_read(f, &a, report);
    fclose(f);
    return r == P0_PATCH_OK ? a : NULL;
}

static int do_prepare(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = prepare_fix(&report, "int answer(void) { return 42; }\n");
    if (!a) { fprintf(stderr, "prepare: %s\n", report.detail); return 1; }
    int untouched = file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    int meta = p0_patch_count(a) == 1 && !p0_patch_is_detached(a) &&
               strlen(p0_patch_digest(a)) == 64;
    int saved = untouched && meta && save_artifact(artifact_path, a, &report);
    p0_patch_free(a);
    if (!saved) fprintf(stderr, "prepare touched canonical or persist failed\n");
    return !saved;
}

static int do_check(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(artifact_path, &report);
    if (!a) { fprintf(stderr, "load: %s\n", report.detail); return 1; }
    OracleObs obs;
    P0PatchResult r = p0_patch_check(a, 0, stage_oracle, &obs, &report);
    int ok = r == P0_PATCH_OK && obs.has_build && obs.has_run &&
             obs.build.verdict == P0_OK && obs.run.verdict == P0_OK &&
             strstr(obs.build.cmd, "cc") != NULL && obs.run.digest[0] &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "check: %s (%s)\n", report.detail,
                     p0_patch_result_name(r));
    return !ok;
}

static int do_check_red(void) {
    P0PatchReport report;
    P0PatchArtifact *a = prepare_fix(&report, "int answer(void) { return 43; }\n");
    if (!a) return 1;
    OracleObs obs;
    P0PatchResult r = p0_patch_check(a, 0, stage_oracle, &obs, &report);
    int ok = r == P0_PATCH_STAGE_CHECK_FAILED && obs.has_build &&
             obs.build.verdict == P0_OK && obs.has_run &&
             obs.run.verdict == P0_EXIT_NONZERO &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "check-red: %s (%s)\n", report.detail,
                     p0_patch_result_name(r));
    return !ok;
}

static int do_check_mutation(void) {
    P0PatchReport report;
    P0PatchArtifact *a = prepare_fix(&report, "int answer(void) { return 42; }\n");
    if (!a) return 1;
    OracleObs obs;
    P0PatchResult r = p0_patch_check(a, 0, mutating_oracle, &obs, &report);
    int ok = r == P0_PATCH_STAGE_CHECK_FAILED &&
             strstr(report.detail, "stage_check_mutation") != NULL &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "check-mutation: %s (%s)\n", report.detail,
                     p0_patch_result_name(r));
    return !ok;
}

static int do_commit(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(artifact_path, &report);
    if (!a) { fprintf(stderr, "load: %s\n", report.detail); return 1; }
    OracleObs obs;
    P0PatchResult r = p0_patch_commit(a, authorize, NULL,
                                      canonical_oracle, &obs, &report);
    int ok = r == P0_PATCH_OK && !report.rollback_attempted &&
             obs.has_build && obs.has_run && obs.run.verdict == P0_OK &&
             file_is("calc.c", "int answer(void) { return 42; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "commit: %s (%s)\n", report.detail,
                     p0_patch_result_name(r));
    return !ok;
}

static int do_commit_policy(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(artifact_path, &report);
    if (!a) { fprintf(stderr, "load: %s\n", report.detail); return 1; }
    P0PatchResult r = p0_patch_commit(a, deny, NULL,
                                      canonical_oracle, NULL, &report);
    int ok = r == P0_PATCH_POLICY_DENIED && !report.rollback_attempted &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "commit-policy: %s (%s)\n", report.detail,
                     p0_patch_result_name(r));
    return !ok;
}

static int do_commit_rollback(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(artifact_path, &report);
    if (!a) { fprintf(stderr, "load: %s\n", report.detail); return 1; }
    P0PatchResult r = p0_patch_commit(a, authorize, NULL,
                                      red_post_check, NULL, &report);
    int ok = r == P0_PATCH_POST_CHECK_FAILED && report.rollback_attempted &&
             report.rollback_ok &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(a);
    if (!ok) fprintf(stderr, "commit-rollback: %s (%s) attempted=%d ok=%d\n",
                     report.detail, p0_patch_result_name(r),
                     report.rollback_attempted, report.rollback_ok);
    return !ok;
}

int main(int argc, char **argv) {
    if (argc < 2) return 2;
    if (strcmp(argv[1], "prepare") == 0 && argc == 3)
        return do_prepare(argv[2]);
    if (strcmp(argv[1], "check") == 0 && argc == 3)
        return do_check(argv[2]);
    if (strcmp(argv[1], "check-red") == 0) return do_check_red();
    if (strcmp(argv[1], "check-mutation") == 0) return do_check_mutation();
    if (strcmp(argv[1], "commit") == 0 && argc == 3)
        return do_commit(argv[2]);
    if (strcmp(argv[1], "commit-policy") == 0 && argc == 3)
        return do_commit_policy(argv[2]);
    if (strcmp(argv[1], "commit-rollback") == 0 && argc == 3)
        return do_commit_rollback(argv[2]);
    return 2;
}
EOC

if ! cc -std=c11 -Wall -Wextra -Wpedantic -O2 -I"$ROOT/src" \
        -o "$TMP/driver" "$TMP/driver.c" "$ROOT/src/patch.c" \
        "$ROOT/src/exec.c" 2>"$TMP/cc.err"; then
    echo "FAIL patch-check: could not build structural driver" >&2
    cat "$TMP/cc.err" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS patch-check: $1"; pass=$((pass+1)); }
no() { echo "FAIL patch-check: $1" >&2; fail=$((fail+1)); }

ws() {
    mkdir -p "$1"
    printf 'int answer(void) { return 41; }\n' >"$1/calc.c"
    chmod 0750 "$1/calc.c"
}

ws "$TMP/work"
if (cd "$TMP/work" && "$TMP/driver" prepare "$TMP/artifact.p0pa"); then
    ok "prepare+persist leaves the canonical byte/mode untouched"
else
    no "prepare or persistence touched the canonical workspace"
fi
if (cd "$TMP/work" && "$TMP/driver" check "$TMP/artifact.p0pa"); then
    ok "reloaded artifact: build and run are two real P0Obs via p0_exec_at in the candidate tree"
else
    no "candidate-root oracle did not run green across the process boundary"
fi
if (cd "$TMP/work" && "$TMP/driver" check-red); then
    ok "a red candidate is STAGE_CHECK_FAILED with real observations and zero canonical writes"
else
    no "red candidate was not typed or moved bytes"
fi
if (cd "$TMP/work" && "$TMP/driver" check-mutation); then
    ok "an oracle that mutates a touched postimage cannot certify it"
else
    no "oracle mutation escaped the post-check re-verification"
fi
if (cd "$TMP/work" && "$TMP/driver" commit "$TMP/artifact.p0pa"); then
    ok "policy + green canonical oracle: commit lands byte/mode and the canonical run is real"
else
    no "governed commit did not land or canonical oracle did not pass"
fi

# Back to the recorded base: the persisted artifact binds the broken preimage,
# so every later phase re-validates against exactly these bytes and this mode.
ws "$TMP/work"
if (cd "$TMP/work" && "$TMP/driver" commit-rollback "$TMP/artifact.p0pa"); then
    ok "red canonical post-check rolls back byte/path/mode and verifies the rollback"
else
    no "red post-check did not restore the workspace"
fi
if (cd "$TMP/work" && "$TMP/driver" commit-policy "$TMP/artifact.p0pa"); then
    ok "denied policy is POLICY_DENIED with zero writes, even after a green candidate"
else
    no "denied policy still moved bytes"
fi
if (cd "$TMP/work" && "$TMP/driver" commit "$TMP/artifact.p0pa"); then
    ok "after the rollback the same persisted artifact still commits (recoverable, not sticky)"
else
    no "the rollback left a state the persisted artifact could not commit from"
fi

echo "patch-check: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
