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
    char full[1024];
    struct stat st;
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
        { P0_PATCH_EDIT, "edit.txt", NULL, edit, sizeof edit - 1, 0 },
        { P0_PATCH_RENAME, "rename.txt", "moved.txt", NULL, 0, 0 },
        { P0_PATCH_CREATE, "created.txt", NULL, created, sizeof created - 1, 0750 },
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

static int do_prepare(const char *path) {
    P0PatchReport report;
    P0PatchArtifact *a = prepare_quad(&report);
    if (!a || !initial_state(p0_root()) || p0_patch_count(a) != 4 ||
        strlen(p0_patch_base_digest(a)) != 64 || strlen(p0_patch_digest(a)) != 64)
        goto fail;
    for (size_t i = 0; i < p0_patch_count(a); i++) {
        P0PatchOpView op;
        if (!p0_patch_op(a, i, &op) ||
            (op.has_before && strlen(op.before_digest) != 64) ||
            (op.has_after && strlen(op.after_digest) != 64)) goto fail;
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
    if (!diffs || !save_artifact(path, a, &report)) goto fail;
    p0_patch_free(a);
    puts("prepare-ok");
    return 0;
fail:
    p0_patch_free(a);
    return 1;
}

static int do_commit(const char *path) {
    P0PatchReport report;
    P0PatchArtifact *a = load_artifact(path, &report);
    if (!a) return 1;
    P0PatchResult r = p0_patch_commit(a, authorize, NULL, check_final, NULL, &report);
    int ok = r == P0_PATCH_OK && !report.rollback_attempted && final_state(p0_root());
    p0_patch_free(a);
    if (ok) puts("commit-ok");
    return !ok;
}

static int do_conflict(void) {
    static const char born[] = "born\n", changed[] = "two\n";
    P0PatchSpec specs[] = {
        { P0_PATCH_CREATE, "side.txt", NULL, born, sizeof born - 1, 0644 },
        { P0_PATCH_EDIT, "victim.txt", NULL, changed, sizeof changed - 1, 0 }
    };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    if (p0_patch_prepare(specs, 2, NULL, NULL, &a, &report) != P0_PATCH_OK) return 1;
    int setup = write_file("victim.txt", "external\n", 0620);
    P0PatchResult r = setup ? p0_patch_commit(a, authorize, NULL, NULL, NULL, &report) : P0_PATCH_IO_ERROR;
    int ok = r == P0_PATCH_CONFLICT && !report.rollback_attempted &&
             absent(p0_root(), "side.txt") && file_is(p0_root(), "victim.txt", "external\n", 0620);
    p0_patch_free(a);
    if (ok) puts("conflict-ok");
    return !ok;
}

static int do_policy(void) {
    static const char changed[] = "changed\n";
    P0PatchSpec spec = { P0_PATCH_EDIT, "policy.txt", NULL, changed, sizeof changed - 1, 0 };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    if (p0_patch_prepare(&spec, 1, NULL, NULL, &a, &report) != P0_PATCH_OK) return 1;
    P0PatchResult r = p0_patch_commit(a, NULL, NULL, NULL, NULL, &report);
    int ok = r == P0_PATCH_POLICY_DENIED && !report.rollback_attempted &&
             file_is(p0_root(), "policy.txt", "policy\n", 0644);
    p0_patch_free(a);
    if (ok) puts("policy-ok");
    return !ok;
}

static int do_rollback(void) {
    P0PatchReport report; P0PatchArtifact *a = prepare_quad(&report);
    if (!a) return 1;
    P0PatchResult r = p0_patch_commit(a, authorize, NULL, fail_after_observing_final, NULL, &report);
    int ok = r == P0_PATCH_POST_CHECK_FAILED && report.rollback_attempted &&
             report.rollback_ok && initial_state(p0_root());
    p0_patch_free(a);
    if (ok) puts("rollback-ok");
    return !ok;
}

static int do_stage_mutation(void) {
    static const char post[] = "post\n";
    P0PatchSpec spec = { P0_PATCH_EDIT, "mutate.txt", NULL, post, sizeof post - 1, 0 };
    P0PatchArtifact *a = NULL; P0PatchReport report;
    P0PatchResult r = p0_patch_prepare(&spec, 1, mutate_touched_stage, NULL, &a, &report);
    int ok = r == P0_PATCH_STAGE_CHECK_FAILED && a == NULL &&
             file_is(p0_root(), "mutate.txt", "pre\n", 0644);
    p0_patch_free(a);
    if (ok) puts("stage-mutation-ok");
    return !ok;
}

static int do_image(void) {
    static const char before[] = "int answer = 0;\n", after[] = "int answer = 42;\n";
    P0PatchArtifact *a = NULL; P0PatchReport report; P0PatchOpView op;
    P0PatchResult r = p0_patch_prepare_image("candidate.c", before, sizeof before - 1, 0644,
        after, sizeof after - 1, 0644, image_stage, NULL, &a, &report);
    int ok = r == P0_PATCH_OK && a && p0_patch_is_detached(a) && p0_patch_op(a, 0, &op) &&
             op.before_len == sizeof before - 1 && op.after_len == sizeof after - 1;
    if (ok) ok = p0_patch_commit(a, authorize, NULL, NULL, NULL, &report) == P0_PATCH_POLICY_DENIED;
    p0_patch_free(a);
    if (ok) puts("image-ok");
    return !ok;
}

static int do_tamper(const char *path) {
    FILE *f = fopen(path, "r+b");
    if (!f || fseek(f, -1, SEEK_END) != 0) { if (f) fclose(f); return 1; }
    int byte = fgetc(f);
    int changed = byte != EOF && fseek(f, -1, SEEK_END) == 0 && fputc(byte ^ 0x5a, f) != EOF && fflush(f) == 0;
    if (!changed) { fclose(f); return 1; }
    rewind(f); P0PatchArtifact *a = NULL; P0PatchReport report;
    P0PatchResult r = p0_patch_read(f, &a, &report);
    fclose(f); int ok = r == P0_PATCH_CORRUPT && a == NULL;
    p0_patch_free(a);
    if (ok) puts("tamper-ok");
    return !ok;
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
