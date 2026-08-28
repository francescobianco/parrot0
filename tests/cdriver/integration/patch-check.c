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

static int pass;
static int fail;
static char cleanup_temp[1024];
static char cleanup_artifact[1024];

static void cleanup(void)
{
    if (!cleanup_temp[0]) return;
    (void)unlink("calc.c");
    (void)unlink(cleanup_artifact);
    if (chdir("..") == 0) {
        (void)rmdir("work");
        if (chdir("..") == 0) (void)rmdir(cleanup_temp);
    }
}

static void check(int condition, const char *description)
{
    if (condition) {
        printf("PASS patch-check: %s\n", description);
        pass++;
    } else {
        fprintf(stderr, "FAIL patch-check: %s\n", description);
        fail++;
    }
}

/* The oracle keeps build and run as two distinct observations. */
static const char HARNESS[] =
    "int answer(void); int main(void) { return answer() == 42 ? 0 : 1; }\n";

typedef struct {
    int has_build;
    int has_run;
    P0Obs build;
    P0Obs run;
} OracleObs;

static int file_is(const char *path, const char *want, mode_t mode)
{
    FILE *f = fopen(path, "rb");
    char got[256];
    size_t n;
    int extra;
    struct stat st;

    if (!f) return 0;
    n = fread(got, 1, sizeof got, f);
    extra = fgetc(f);
    fclose(f);
    return extra == EOF && n == strlen(want) && memcmp(got, want, n) == 0 &&
           stat(path, &st) == 0 && (st.st_mode & 07777) == mode;
}

static int write_initial(void)
{
    FILE *f = fopen("calc.c", "wb");
    int ok;

    if (!f) return 0;
    ok = fputs("int answer(void) { return 41; }\n", f) >= 0 && fclose(f) == 0;
    if (!ok) return 0;
    return chmod("calc.c", 0750) == 0;
}

static int oracle_at(int rootfd, OracleObs *obs)
{
    char *cc_argv[] = {
        (char *)"cc", (char *)"-w", (char *)"-x", (char *)"c", (char *)"-",
        (char *)"calc.c", (char *)"-o", (char *)"judge", NULL
    };
    char *run_argv[] = { (char *)"./judge", NULL };

    memset(obs, 0, sizeof *obs);
    obs->has_build = 1;
    p0_exec_at(rootfd, cc_argv, NULL, 20000, HARNESS, &obs->build);
    if (!p0_obs_ok(&obs->build)) {
        (void)unlinkat(rootfd, "judge", 0);
        return 0;
    }
    obs->has_run = 1;
    p0_exec_at(rootfd, run_argv, NULL, 15000, NULL, &obs->run);
    (void)unlinkat(rootfd, "judge", 0);
    return obs->run.verdict == P0_OK;
}

static int stage_oracle(int rootfd, const char *root, void *ctx,
                        char *why, size_t cap)
{
    OracleObs *obs = ctx;
    int green;

    (void)root;
    green = oracle_at(rootfd, obs);
    if (!green) snprintf(why, cap, "candidate oracle red");
    return green;
}

static int mutating_oracle(int rootfd, const char *root, void *ctx,
                           char *why, size_t cap)
{
    int fd;
    static const char fraud[] = "int answer(void) { return 42; } /* fraud */\n";

    (void)root;
    (void)ctx;
    fd = openat(rootfd, "calc.c", O_WRONLY | O_TRUNC | O_CLOEXEC | O_NOFOLLOW);
    if (fd >= 0) {
        if (write(fd, fraud, sizeof fraud - 1) != (ssize_t)(sizeof fraud - 1))
            snprintf(why, cap, "oracle mutation write failed");
        close(fd);
    }
    snprintf(why, cap, "oracle touched a path under judgement");
    return 1;
}

static int authorize(const P0PatchArtifact *artifact, void *ctx, char *why,
                     size_t cap)
{
    (void)ctx;
    if (p0_patch_count(artifact) == 0) {
        snprintf(why, cap, "empty");
        return 0;
    }
    return 1;
}

static int deny(const P0PatchArtifact *artifact, void *ctx, char *why,
                size_t cap)
{
    (void)artifact;
    (void)ctx;
    snprintf(why, cap, "policy_absent");
    return 0;
}

static int canonical_oracle(const char *root, void *ctx, char *why, size_t cap)
{
    OracleObs *obs = ctx;
    int rootfd = open(root, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    int green;

    if (rootfd < 0) {
        snprintf(why, cap, "canonical_root_open");
        return 0;
    }
    green = oracle_at(rootfd, obs);
    close(rootfd);
    if (!green) snprintf(why, cap, "canonical oracle red");
    return green;
}

static int red_post_check(const char *root, void *ctx, char *why, size_t cap)
{
    (void)ctx;
    if (!file_is("calc.c", "int answer(void) { return 42; }\n", 0750))
        snprintf(why, cap, "patch was not fully applied");
    else
        snprintf(why, cap, "deliberate red canonical oracle");
    (void)root;
    return 0;
}

static P0PatchArtifact *prepare_fix(P0PatchReport *report, const char *after)
{
    P0PatchSpec spec = { P0_PATCH_EDIT, "calc.c", NULL, after, strlen(after), 0 };
    P0PatchArtifact *artifact = NULL;

    if (p0_patch_prepare(&spec, 1, NULL, NULL, &artifact, report) != P0_PATCH_OK)
        return NULL;
    return artifact;
}

static int save_artifact(const char *path, const P0PatchArtifact *artifact,
                         P0PatchReport *report)
{
    FILE *f = fopen(path, "wb");
    P0PatchResult result;
    int closed;

    if (!f) return 0;
    result = p0_patch_write(artifact, f, report);
    closed = fclose(f) == 0;
    return result == P0_PATCH_OK && closed;
}

static P0PatchArtifact *load_artifact(const char *path, P0PatchReport *report)
{
    FILE *f = fopen(path, "rb");
    P0PatchArtifact *artifact = NULL;
    P0PatchResult result;

    if (!f) return NULL;
    result = p0_patch_read(f, &artifact, report);
    fclose(f);
    return result == P0_PATCH_OK ? artifact : NULL;
}

static int prepare_and_save(const char *artifact_path)
{
    P0PatchReport report;
    P0PatchArtifact *artifact = prepare_fix(&report,
                                            "int answer(void) { return 42; }\n");
    int untouched;
    int metadata;
    int saved;

    if (!artifact) return 0;
    untouched = file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    metadata = p0_patch_count(artifact) == 1 && !p0_patch_is_detached(artifact) &&
               strlen(p0_patch_digest(artifact)) == 64;
    saved = untouched && metadata && save_artifact(artifact_path, artifact, &report);
    p0_patch_free(artifact);
    return saved;
}

static int check_saved(const char *artifact_path)
{
    P0PatchReport report;
    OracleObs obs;
    P0PatchArtifact *artifact = load_artifact(artifact_path, &report);
    P0PatchResult result;
    int ok;

    if (!artifact) return 0;
    result = p0_patch_check(artifact, 0, stage_oracle, &obs, &report);
    ok = result == P0_PATCH_OK && obs.has_build && obs.has_run &&
         obs.build.verdict == P0_OK && obs.run.verdict == P0_OK &&
         strstr(obs.build.cmd, "cc") != NULL && obs.run.digest[0] &&
         file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(artifact);
    return ok;
}

static int check_red(void)
{
    P0PatchReport report;
    OracleObs obs;
    P0PatchArtifact *artifact = prepare_fix(&report,
                                            "int answer(void) { return 43; }\n");
    P0PatchResult result;
    int ok;

    if (!artifact) return 0;
    result = p0_patch_check(artifact, 0, stage_oracle, &obs, &report);
    ok = result == P0_PATCH_STAGE_CHECK_FAILED && obs.has_build &&
         obs.build.verdict == P0_OK && obs.has_run &&
         obs.run.verdict == P0_EXIT_NONZERO &&
         file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(artifact);
    return ok;
}

static int check_mutation(void)
{
    P0PatchReport report;
    P0PatchArtifact *artifact = prepare_fix(&report,
                                            "int answer(void) { return 42; }\n");
    P0PatchResult result;
    int ok;

    if (!artifact) return 0;
    result = p0_patch_check(artifact, 0, mutating_oracle, NULL, &report);
    ok = result == P0_PATCH_STAGE_CHECK_FAILED &&
         strstr(report.detail, "stage_check_mutation") != NULL &&
         file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
    p0_patch_free(artifact);
    return ok;
}

int main(void)
{
    char temp[] = "/tmp/parrot0-patch-check.XXXXXX";
    char artifact_path[1024];
    OracleObs obs;
    P0PatchReport report;
    P0PatchArtifact *artifact;
    P0PatchResult result;
    int ok;

    if (!mkdtemp(temp)) return 2;
    snprintf(cleanup_temp, sizeof cleanup_temp, "%s", temp);
    atexit(cleanup);
    if (chdir(temp) != 0 || mkdir("work", 0700) != 0 || chdir("work") != 0 ||
        !write_initial()) return 2;
    snprintf(artifact_path, sizeof artifact_path, "%s/artifact.p0pa", temp);
    snprintf(cleanup_artifact, sizeof cleanup_artifact, "%s", artifact_path);

    check(prepare_and_save(artifact_path),
          "prepare+persist leaves the canonical byte/mode untouched");
    check(check_saved(artifact_path),
          "reloaded artifact: build and run are two real P0Obs via p0_exec_at in the candidate tree");
    check(check_red( ),
          "a red candidate is STAGE_CHECK_FAILED with real observations and zero canonical writes");
    check(check_mutation(),
          "an oracle that mutates a touched postimage cannot certify it");

    artifact = load_artifact(artifact_path, &report);
    if (!artifact) ok = 0;
    else {
        result = p0_patch_commit(artifact, authorize, NULL, canonical_oracle,
                                 &obs, &report);
        ok = result == P0_PATCH_OK && !report.rollback_attempted &&
             obs.has_build && obs.has_run && obs.run.verdict == P0_OK &&
             file_is("calc.c", "int answer(void) { return 42; }\n", 0750);
        p0_patch_free(artifact);
    }
    check(ok, "policy + green canonical oracle: commit lands byte/mode and the canonical oracle is real");

    if (!write_initial()) return 2;
    artifact = load_artifact(artifact_path, &report);
    if (!artifact) ok = 0;
    else {
        result = p0_patch_commit(artifact, authorize, NULL, red_post_check,
                                 NULL, &report);
        ok = result == P0_PATCH_POST_CHECK_FAILED && report.rollback_attempted &&
             report.rollback_ok &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
        p0_patch_free(artifact);
    }
    check(ok, "red canonical post-check rolls back byte/path/mode and verifies the rollback");

    artifact = load_artifact(artifact_path, &report);
    if (!artifact) ok = 0;
    else {
        result = p0_patch_commit(artifact, deny, NULL, canonical_oracle,
                                 NULL, &report);
        ok = result == P0_PATCH_POLICY_DENIED && !report.rollback_attempted &&
             file_is("calc.c", "int answer(void) { return 41; }\n", 0750);
        p0_patch_free(artifact);
    }
    check(ok, "denied policy is POLICY_DENIED with zero writes, even after a green candidate");

    artifact = load_artifact(artifact_path, &report);
    if (!artifact) ok = 0;
    else {
        result = p0_patch_commit(artifact, authorize, NULL, canonical_oracle,
                                 &obs, &report);
        ok = result == P0_PATCH_OK && !report.rollback_attempted &&
             obs.has_build && obs.has_run && obs.run.verdict == P0_OK &&
             file_is("calc.c", "int answer(void) { return 42; }\n", 0750);
        p0_patch_free(artifact);
    }
    check(ok, "after the rollback the same persisted artifact still commits (recoverable, not sticky)");

    printf("patch-check: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}
