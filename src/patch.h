/*
 * patch.h - immutable, transactional multi-file patch artifacts.
 *
 * This module is deliberately independent of Brain and of any command-line
 * patch program.  An artifact owns the complete before/after bytes and modes;
 * applying it therefore never means trusting a prose diff or authorizing a
 * generic `git apply`.  Policy is an explicit callback at the canonical
 * workspace boundary.
 *
 * A commit is recoverable, not crash-atomic across several directory entries:
 * each file replacement is atomic, every preimage is revalidated before the
 * first write, and a detected apply/post-check failure restores and verifies all
 * touched paths.  POSIX does not offer one atomic transaction spanning multiple
 * files, so callers must not claim durability across process/kernel failure.
 */
#ifndef PARROT0_PATCH_H
#define PARROT0_PATCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#define P0_PATCH_DIGEST_HEX 65
#define P0_PATCH_DETAIL     256
#define P0_PATCH_MAX_OPS    1024
#define P0_PATCH_MAX_BYTES  (64U * 1024U * 1024U)
#define P0_PATCH_MAX_CONTEXT_BYTES (1024ULL * 1024ULL * 1024ULL)
#define P0_PATCH_MAX_CONTEXT_FILES 200000U

typedef enum {
    P0_PATCH_EDIT = 0,
    P0_PATCH_CREATE,
    P0_PATCH_DELETE,
    P0_PATCH_RENAME
} P0PatchOpKind;

/* Input to preparation.  `path2` is used only by RENAME.  EDIT and CREATE own
 * a copy of post_data after p0_patch_prepare returns.  post_mode == 0 preserves
 * the old mode for EDIT and selects 0644 for CREATE.  RENAME preserves bytes and
 * mode; changing them is a separate EDIT operation and overlapping operations
 * are intentionally refused. */
typedef struct {
    P0PatchOpKind kind;
    const char   *path;
    const char   *path2;
    const void   *post_data;
    size_t        post_len;
    mode_t        post_mode;
} P0PatchSpec;

/* Read-only view into an immutable artifact.  Byte pointers remain owned by the
 * artifact and are valid until p0_patch_free. */
typedef struct {
    P0PatchOpKind kind;
    const char   *path;
    const char   *path2;
    int           has_before;
    const unsigned char *before;
    size_t        before_len;
    mode_t        before_mode;
    const char   *before_digest;
    int           has_after;
    const unsigned char *after;
    size_t        after_len;
    mode_t        after_mode;
    const char   *after_digest;
} P0PatchOpView;

typedef enum {
    P0_PATCH_OK = 0,
    P0_PATCH_INVALID,
    P0_PATCH_UNSAFE_PATH,
    P0_PATCH_NOT_FOUND,
    P0_PATCH_ALREADY_EXISTS,
    P0_PATCH_TOO_LARGE,
    P0_PATCH_IO_ERROR,
    P0_PATCH_CONFLICT,
    P0_PATCH_POLICY_DENIED,
    P0_PATCH_STAGE_CHECK_FAILED,
    P0_PATCH_POST_CHECK_FAILED,
    P0_PATCH_APPLY_FAILED,
    P0_PATCH_ROLLBACK_FAILED,
    P0_PATCH_CORRUPT
} P0PatchResult;

typedef struct {
    P0PatchResult result;
    int rollback_attempted;
    int rollback_ok;
    char detail[P0_PATCH_DETAIL];
} P0PatchReport;

typedef struct P0PatchArtifact P0PatchArtifact;

/* A checker receives an isolated candidate source tree during prepare and the
 * canonical root after commit.  When a stage checker is present, regular
 * workspace context is copied independently (bounded by the context limits;
 * VCS/cache/virtual-environment roots are omitted), then the artifact is applied.
 * It returns non-zero on success and may put a mechanical diagnostic in `why`.
 * The checker contract is read-only for touched source paths; any touched-path
 * mutation is detected by a second postimage verification. */
typedef int (*P0PatchCheckFn)(const char *root, void *ctx,
                              char *why, size_t why_cap);

/* The capability form of a stage check: the callback receives a BORROWED
 * directory descriptor on the isolated candidate tree root (owned by the
 * caller, valid only for the duration of the call) plus the tree path for
 * diagnostics.  This is the seam through which an oracle runs via
 * p0_exec_at(rootfd, ...): build and test execute with their cwd resolved
 * inside the candidate tree, never by trusting a path string.  rootfd is cwd
 * containment, not a chroot — the same honesty boundary as p0_exec_at. */
typedef int (*P0PatchRootCheckFn)(int rootfd, const char *root, void *ctx,
                                  char *why, size_t why_cap);

/* Canonical workspace mutation is R3.  NULL or a zero result denies it.  A Brain
 * adapter can back this callback with a KB policy without coupling this mechanics
 * module to language or to the KB representation. */
typedef int (*P0PatchAuthorizeFn)(const P0PatchArtifact *artifact, void *ctx,
                                  char *why, size_t why_cap);

const char *p0_patch_result_name(P0PatchResult result);
const char *p0_patch_op_name(P0PatchOpKind kind);

/* Snapshot the canonical workspace, build the immutable artifact, apply it to a
 * fresh isolated staging tree, verify every postimage, and optionally run a
 * stage check.  The canonical workspace is never written by this call. */
P0PatchResult p0_patch_prepare(const P0PatchSpec *specs, size_t count,
                               P0PatchCheckFn stage_check, void *check_ctx,
                               P0PatchArtifact **out, P0PatchReport *report);

/* Build a detached EDIT candidate from two in-memory source images.  This is the
 * adapter for synthesis/repair candidates whose source came from a prompt rather
 * than from a canonical workspace path.  It has the same complete images,
 * digests, diff and isolated-stage validation, but commit always refuses it:
 * there is deliberately no canonical preimage binding to revalidate. */
P0PatchResult p0_patch_prepare_image(const char *label,
                                     const void *before, size_t before_len,
                                     mode_t before_mode,
                                     const void *after, size_t after_len,
                                     mode_t after_mode,
                                     P0PatchCheckFn stage_check, void *check_ctx,
                                     P0PatchArtifact **out, P0PatchReport *report);

/* Re-materialize an artifact (including one just re-loaded from disk) into a
 * fresh isolated candidate tree: optionally copy the bounded workspace
 * context, write the recorded preimages, apply the batch, verify every
 * postimage, then hand the tree root to `check` as a borrowed rootfd.  After
 * a green check every touched postimage is verified once more, so an oracle
 * that mutated a touched path cannot certify it.  The stage is always
 * destroyed and the canonical workspace is never written.  Returns
 * P0_PATCH_OK only when the mechanics AND the check are green;
 * P0_PATCH_STAGE_CHECK_FAILED means the oracle itself ran and answered red. */
P0PatchResult p0_patch_check(const P0PatchArtifact *artifact, int with_context,
                             P0PatchRootCheckFn check, void *check_ctx,
                             P0PatchReport *report);

void   p0_patch_free(P0PatchArtifact *artifact);
size_t p0_patch_count(const P0PatchArtifact *artifact);
int    p0_patch_op(const P0PatchArtifact *artifact, size_t index,
                   P0PatchOpView *out);
const char *p0_patch_base_digest(const P0PatchArtifact *artifact);
const char *p0_patch_digest(const P0PatchArtifact *artifact);
int         p0_patch_is_detached(const P0PatchArtifact *artifact);

/* Allocates a git-style unified forward or reverse diff.  The returned buffer is
 * NUL-terminated and must be freed by the caller.  Complete bytes remain in the
 * artifact even when a binary file can only be represented by a binary marker. */
char *p0_patch_diff(const P0PatchArtifact *artifact, int reverse,
                    size_t *length_out);

/* Stable binary persistence.  Loading rehashes every byte and rejects tampering;
 * the resulting object is as immutable as one returned by prepare. */
P0PatchResult p0_patch_write(const P0PatchArtifact *artifact, FILE *stream,
                             P0PatchReport *report);
P0PatchResult p0_patch_read(FILE *stream, P0PatchArtifact **out,
                            P0PatchReport *report);

/* Revalidate ALL preimages before the first mutation, require policy approval,
 * apply, verify, run post_check, and verify again.  Any detected failure after a
 * write triggers byte-and-mode rollback plus verification. */
P0PatchResult p0_patch_commit(const P0PatchArtifact *artifact,
                              P0PatchAuthorizeFn authorize, void *authorize_ctx,
                              P0PatchCheckFn post_check, void *check_ctx,
                              P0PatchReport *report);

#endif /* PARROT0_PATCH_H */
