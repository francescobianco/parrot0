#!/usr/bin/env bash
#
# T05: PatchArtifact is durable data plus a recoverable multi-file transaction.
# This test compiles the real module without Brain, then uses separate processes
# for prepare and commit.  That distinction catches an in-memory "artifact" that
# cannot outlive one command.  The negative cases make the safety claims causal:
# a stale preimage causes zero writes, missing policy causes zero writes, and a
# red post-check restores every byte, mode and path.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-patch-artifact.$$"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP"

cat >"$TMP/driver.c" <<'EOC'
#include "patch.h"
#include "exec.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int write_file(const char *path, const char *data, mode_t mode) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    size_t n = strlen(data);
    int ok = fwrite(data, 1, n, f) == n && fclose(f) == 0 && chmod(path, mode) == 0;
    return ok;
}

static int file_is(const char *root, const char *path, const char *want, mode_t mode) {
    char full[1024];
    snprintf(full, sizeof full, "%s/%s", root, path);
    FILE *f = fopen(full, "rb");
    if (!f) return 0;
    char got[256];
    size_t n = fread(got, 1, sizeof got, f);
    int extra = fgetc(f);
    fclose(f);
    struct stat st;
    return extra == EOF && n == strlen(want) && memcmp(got, want, n) == 0 &&
           stat(full, &st) == 0 && (st.st_mode & 07777) == mode;
}

static int absent(const char *root, const char *path) {
    char full[1024]; struct stat st;
    snprintf(full, sizeof full, "%s/%s", root, path);
    return lstat(full, &st) != 0 && errno == ENOENT;
}

static int final_state(const char *root) {
    return file_is(root, "edit.txt", "new edit\n", 0640) &&
           absent(root, "rename.txt") &&
           file_is(root, "moved.txt", "move me\n", 0604) &&
           file_is(root, "created.txt", "created\n", 0750) &&
           absent(root, "delete.txt");
}

static int initial_state(const char *root) {
    return file_is(root, "edit.txt", "old edit\n", 0640) &&
           file_is(root, "rename.txt", "move me\n", 0604) &&
           absent(root, "moved.txt") && absent(root, "created.txt") &&
           file_is(root, "delete.txt", "delete me\n", 0660);
}

static int stage_ok(const char *root, void *ctx, char *why, size_t cap) {
    (void)ctx;
    if (strcmp(root, p0_root()) == 0) {
        snprintf(why, cap, "stage aliases canonical workspace");
        return 0;
    }
    if (!final_state(root) || !file_is(root, "support/context.h", "context\n", 0644)) {
        snprintf(why, cap, "staged postimages or untouched context are incomplete");
        return 0;
    }
    return 1;
}

static int authorize(const P0PatchArtifact *a, void *ctx, char *why, size_t cap) {
    (void)ctx;
    if (p0_patch_count(a) == 0) { snprintf(why, cap, "empty artifact"); return 0; }
    return 1;
}

static int check_final(const char *root, void *ctx, char *why, size_t cap) {
    (void)ctx;
    if (!final_state(root)) { snprintf(why, cap, "canonical postimage mismatch"); return 0; }
    return 1;
}

static int fail_after_observing_final(const char *root, void *ctx,
                                      char *why, size_t cap) {
    (void)ctx;
    if (!final_state(root)) snprintf(why, cap, "patch was not fully applied");
    else snprintf(why, cap, "deliberate red oracle");
    return 0;
}

static int mutate_touched_stage(const char *root, void *ctx,
                                char *why, size_t cap) {
    (void)ctx; (void)why; (void)cap;
    char path[1024];
    snprintf(path, sizeof path, "%s/mutate.txt", root);
    return write_file(path, "checker mutation\n", 0644);
}

static int image_stage(const char *root, void *ctx, char *why, size_t cap) {
    (void)ctx;
    if (!file_is(root, "candidate.c", "int answer = 42;\n", 0644) ||
        !file_is(root, "support.h", "dependency\n", 0644)) {
        snprintf(why, cap, "detached image lacks candidate context");
        return 0;
    }
    return 1;
}

static P0PatchArtifact *prepare_quad(P0PatchReport *report) {
    static const char edit[] = "new edit\n", created[] = "created\n";
    P0PatchSpec specs[] = {
        { P0_PATCH_EDIT,   "edit.txt",   NULL, edit,    sizeof edit - 1, 0 },
        { P0_PATCH_RENAME, "rename.txt", "moved.txt", NULL, 0, 0 },
        { P0_PATCH_CREATE, "created.txt",NULL, created, sizeof created - 1, 0750 },
        { P0_PATCH_DELETE, "delete.txt", NULL, NULL, 0, 0 }
    };
    P0PatchArtifact *a = NULL;
    if (p0_patch_prepare(specs, 4, stage_ok, NULL, &a, report) != P0_PATCH_OK)
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
    P0PatchArtifact *a = prepare_quad(&report);
    if (!a) { fprintf(stderr, "prepare: %s\n", report.detail); return 1; }
    if (!initial_state(p0_root())) {
        fprintf(stderr, "prepare wrote canonical workspace\n"); p0_patch_free(a); return 1;
    }
    if (p0_patch_count(a) != 4 || strlen(p0_patch_base_digest(a)) != 64 ||
        strlen(p0_patch_digest(a)) != 64) {
        fprintf(stderr, "artifact metadata incomplete\n"); p0_patch_free(a); return 1;
    }
    for (size_t i = 0; i < p0_patch_count(a); i++) {
        P0PatchOpView op;
        if (!p0_patch_op(a, i, &op) ||
            (op.has_before && strlen(op.before_digest) != 64) ||
            (op.has_after && strlen(op.after_digest) != 64)) {
            fprintf(stderr, "operation image/digest missing\n"); p0_patch_free(a); return 1;
        }
    }
    size_t nf = 0, nr = 0;
    char *forward = p0_patch_diff(a, 0, &nf);
    char *reverse = p0_patch_diff(a, 1, &nr);
    int diffs = forward && reverse && nf && nr &&
        strstr(forward, "--- a/edit.txt\n+++ b/edit.txt") &&
        strstr(forward, "--- a/rename.txt\n+++ b/moved.txt") &&
        strstr(forward, "--- /dev/null\n+++ b/created.txt") &&
        strstr(forward, "--- a/delete.txt\n+++ /dev/null") &&
        strstr(reverse, "--- a/moved.txt\n+++ b/rename.txt") &&
        strstr(reverse, "--- a/created.txt\n+++ /dev/null") &&
        strstr(reverse, "--- /dev/null\n+++ b/delete.txt");
    free(forward); free(reverse);
    if (!diffs) { fprintf(stderr, "forward/reverse diff incomplete\n"); p0_patch_free(a); return 1; }
    if (!save_artifact(artifact_path, a, &report)) {
        fprintf(stderr, "persist: %s\n", report.detail); p0_patch_free(a); return 1;
    }
    p0_patch_free(a);
    return 0;
}

static int do_commit(const char *artifact_path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(artifact_path, &report);
    if (!a) { fprintf(stderr, "load: %s\n", report.detail); return 1; }
    P0PatchResult r = p0_patch_commit(a, authorize, NULL, check_final, NULL, &report);
    p0_patch_free(a);
    if (r != P0_PATCH_OK || report.rollback_attempted || !final_state(p0_root())) {
        fprintf(stderr, "commit: %s (%s)\n", report.detail, p0_patch_result_name(r));
        return 1;
    }
    return 0;
}

static int do_conflict(void) {
    static const char born[] = "born\n", changed[] = "two\n";
    P0PatchSpec specs[] = {
        { P0_PATCH_CREATE, "side.txt", NULL, born, sizeof born - 1, 0644 },
        { P0_PATCH_EDIT, "victim.txt", NULL, changed, sizeof changed - 1, 0 }
    };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    if (p0_patch_prepare(specs, 2, NULL, NULL, &a, &report) != P0_PATCH_OK)
        return fprintf(stderr, "conflict prepare: %s\n", report.detail), 1;
    if (!write_file("victim.txt", "external\n", 0620)) return 1;
    P0PatchResult r = p0_patch_commit(a, authorize, NULL, NULL, NULL, &report);
    p0_patch_free(a);
    if (r != P0_PATCH_CONFLICT || report.rollback_attempted ||
        !absent(p0_root(), "side.txt") ||
        !file_is(p0_root(), "victim.txt", "external\n", 0620)) {
        fprintf(stderr, "conflict was not zero-write: %s\n", report.detail); return 1;
    }
    return 0;
}

static int do_policy(void) {
    static const char changed[] = "changed\n";
    P0PatchSpec spec = { P0_PATCH_EDIT, "policy.txt", NULL,
                         changed, sizeof changed - 1, 0 };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    if (p0_patch_prepare(&spec, 1, NULL, NULL, &a, &report) != P0_PATCH_OK) return 1;
    P0PatchResult r = p0_patch_commit(a, NULL, NULL, NULL, NULL, &report);
    p0_patch_free(a);
    return !(r == P0_PATCH_POLICY_DENIED && !report.rollback_attempted &&
             file_is(p0_root(), "policy.txt", "policy\n", 0644));
}

static int do_rollback(void) {
    P0PatchReport report;
    P0PatchArtifact *a = prepare_quad(&report);
    if (!a) return fprintf(stderr, "rollback prepare: %s\n", report.detail), 1;
    P0PatchResult r = p0_patch_commit(a, authorize, NULL,
                                      fail_after_observing_final, NULL, &report);
    p0_patch_free(a);
    if (r != P0_PATCH_POST_CHECK_FAILED || !report.rollback_attempted ||
        !report.rollback_ok || !initial_state(p0_root())) {
        fprintf(stderr, "rollback: %s (%s) attempted=%d ok=%d\n", report.detail,
                p0_patch_result_name(r), report.rollback_attempted, report.rollback_ok);
        return 1;
    }
    return 0;
}

static int do_stage_mutation(void) {
    static const char post[] = "post\n";
    P0PatchSpec spec = { P0_PATCH_EDIT, "mutate.txt", NULL,
                         post, sizeof post - 1, 0 };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    P0PatchResult r = p0_patch_prepare(&spec, 1, mutate_touched_stage, NULL,
                                       &a, &report);
    p0_patch_free(a);
    return !(r == P0_PATCH_STAGE_CHECK_FAILED && a == NULL &&
             file_is(p0_root(), "mutate.txt", "pre\n", 0644));
}

static int do_image(void) {
    static const char before[] = "int answer = 0;\n";
    static const char after[] = "int answer = 42;\n";
    P0PatchArtifact *a = NULL; P0PatchReport report; P0PatchOpView op;
    P0PatchResult r = p0_patch_prepare_image("candidate.c",
        before, sizeof before - 1, 0644, after, sizeof after - 1, 0644,
        image_stage, NULL, &a, &report);
    if (r != P0_PATCH_OK || !a || !p0_patch_is_detached(a) ||
        !p0_patch_op(a, 0, &op) || op.before_len != sizeof before - 1 ||
        op.after_len != sizeof after - 1) {
        p0_patch_free(a); return 1;
    }
    r = p0_patch_commit(a, authorize, NULL, NULL, NULL, &report);
    p0_patch_free(a);
    return r != P0_PATCH_POLICY_DENIED;
}

static int do_tamper(const char *artifact_path) {
    FILE *f = fopen(artifact_path, "r+b");
    if (!f || fseek(f, -1, SEEK_END) != 0) { if (f) fclose(f); return 1; }
    int byte = fgetc(f);
    if (byte == EOF || fseek(f, -1, SEEK_END) != 0 ||
        fputc(byte ^ 0x5a, f) == EOF || fflush(f) != 0) {
        fclose(f); return 1;
    }
    rewind(f);
    P0PatchArtifact *a = NULL; P0PatchReport report;
    P0PatchResult r = p0_patch_read(f, &a, &report);
    fclose(f);
    p0_patch_free(a);
    return !(r == P0_PATCH_CORRUPT && a == NULL);
}

int main(int argc, char **argv) {
    if (argc < 2) return 2;
    if (strcmp(argv[1], "prepare") == 0 && argc == 3) return do_prepare(argv[2]);
    if (strcmp(argv[1], "commit") == 0 && argc == 3) return do_commit(argv[2]);
    if (strcmp(argv[1], "conflict") == 0) return do_conflict();
    if (strcmp(argv[1], "policy") == 0) return do_policy();
    if (strcmp(argv[1], "rollback") == 0) return do_rollback();
    if (strcmp(argv[1], "stage-mutation") == 0) return do_stage_mutation();
    if (strcmp(argv[1], "image") == 0) return do_image();
    if (strcmp(argv[1], "tamper") == 0 && argc == 3) return do_tamper(argv[2]);
    return 2;
}
EOC

if ! cc -std=c11 -Wall -Wextra -Wpedantic -O2 -I"$ROOT/src" \
        -o "$TMP/driver" "$TMP/driver.c" "$ROOT/src/patch.c" \
        "$ROOT/src/exec.c" 2>"$TMP/cc.err"; then
    echo "FAIL patch-artifact: could not build structural driver" >&2
    cat "$TMP/cc.err" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS patch-artifact: $1"; pass=$((pass+1)); }
no() { echo "FAIL patch-artifact: $1" >&2; fail=$((fail+1)); }

quad() {
    mkdir -p "$1"
    mkdir -p "$1/support"
    printf 'old edit\n' >"$1/edit.txt"
    printf 'move me\n' >"$1/rename.txt"
    printf 'delete me\n' >"$1/delete.txt"
    printf 'context\n' >"$1/support/context.h"
    chmod 0640 "$1/edit.txt"
    chmod 0604 "$1/rename.txt"
    chmod 0660 "$1/delete.txt"
    chmod 0644 "$1/support/context.h"
}

quad "$TMP/persist"
if (cd "$TMP/persist" && "$TMP/driver" prepare "$TMP/artifact.p0pa"); then
    ok "prepare validates all four operations in an isolated stage without workspace writes"
else
    no "isolated prepare, complete images, or forward/reverse diff failed"
fi
if (cd "$TMP/persist" && "$TMP/driver" commit "$TMP/artifact.p0pa"); then
    ok "a serialized artifact survives a process boundary and commits edit/rename/create/delete"
else
    no "persisted multi-file artifact did not load and commit"
fi
if (cd "$TMP/persist" && "$TMP/driver" tamper "$TMP/artifact.p0pa"); then
    ok "a persisted image byte changed out of band is rejected by digest revalidation"
else
    no "tampered serialized artifact was accepted"
fi

mkdir -p "$TMP/conflict"
printf 'one\n' >"$TMP/conflict/victim.txt"
chmod 0610 "$TMP/conflict/victim.txt"
if (cd "$TMP/conflict" && "$TMP/driver" conflict); then
    ok "stale preimage is rejected before the first write, including an earlier create op"
else
    no "preimage conflict caused a partial write"
fi

mkdir -p "$TMP/policy"
printf 'policy\n' >"$TMP/policy/policy.txt"
chmod 0644 "$TMP/policy/policy.txt"
if (cd "$TMP/policy" && "$TMP/driver" policy); then
    ok "canonical mutation without an explicit policy callback is denied with zero writes"
else
    no "missing policy did not stop canonical mutation"
fi

quad "$TMP/rollback"
if (cd "$TMP/rollback" && "$TMP/driver" rollback); then
    ok "red post-check rolls every path back byte-and-mode exact and verifies the rollback"
else
    no "post-check rollback was incomplete or unverified"
fi

mkdir -p "$TMP/stage-mutation"
printf 'pre\n' >"$TMP/stage-mutation/mutate.txt"
chmod 0644 "$TMP/stage-mutation/mutate.txt"
if (cd "$TMP/stage-mutation" && "$TMP/driver" stage-mutation); then
    ok "a stage checker cannot mutate a touched postimage and still certify it"
else
    no "stage checker mutation escaped post-check verification"
fi

mkdir -p "$TMP/image"
printf 'dependency\n' >"$TMP/image/support.h"
chmod 0644 "$TMP/image/support.h"
if (cd "$TMP/image" && "$TMP/driver" image); then
    ok "detached source images get real artifact identity/context but cannot commit"
else
    no "detached in-memory candidate adapter is incomplete or writable"
fi

echo "patch-artifact: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
