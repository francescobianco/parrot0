/* See patch.h: typed patch data first, filesystem effects only at commit. */
#define _GNU_SOURCE
#include "patch.h"
#include "exec.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#ifdef __linux__
#include <linux/fs.h>
#include <sys/ioctl.h>
#endif
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    P0PatchOpKind kind;
    char *path;
    char *path2;
    int has_before;
    unsigned char *before;
    size_t before_len;
    mode_t before_mode;
    char before_digest[P0_PATCH_DIGEST_HEX];
    int has_after;
    unsigned char *after;
    size_t after_len;
    mode_t after_mode;
    char after_digest[P0_PATCH_DIGEST_HEX];
} P0PatchOp;

struct P0PatchArtifact {
    size_t count;
    P0PatchOp *ops;
    int detached;
    char base_digest[P0_PATCH_DIGEST_HEX];
    char digest[P0_PATCH_DIGEST_HEX];
};

static void report_set(P0PatchReport *r, P0PatchResult result,
                       const char *fmt, ...) {
    if (!r) return;
    r->result = result;
    r->detail[0] = '\0';
    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(r->detail, sizeof r->detail, fmt, ap);
        va_end(ap);
    }
}

static void report_init(P0PatchReport *r) {
    if (!r) return;
    memset(r, 0, sizeof *r);
    r->result = P0_PATCH_OK;
}

const char *p0_patch_result_name(P0PatchResult result) {
    switch (result) {
        case P0_PATCH_OK:                 return "ok";
        case P0_PATCH_INVALID:            return "invalid";
        case P0_PATCH_UNSAFE_PATH:        return "unsafe_path";
        case P0_PATCH_NOT_FOUND:          return "not_found";
        case P0_PATCH_ALREADY_EXISTS:     return "already_exists";
        case P0_PATCH_TOO_LARGE:          return "too_large";
        case P0_PATCH_IO_ERROR:           return "io_error";
        case P0_PATCH_CONFLICT:           return "conflict";
        case P0_PATCH_POLICY_DENIED:      return "policy_denied";
        case P0_PATCH_STAGE_CHECK_FAILED: return "stage_check_failed";
        case P0_PATCH_POST_CHECK_FAILED:  return "post_check_failed";
        case P0_PATCH_APPLY_FAILED:       return "apply_failed";
        case P0_PATCH_ROLLBACK_FAILED:    return "rollback_failed";
        case P0_PATCH_CORRUPT:            return "corrupt";
    }
    return "unknown";
}

const char *p0_patch_op_name(P0PatchOpKind kind) {
    switch (kind) {
        case P0_PATCH_EDIT:   return "edit";
        case P0_PATCH_CREATE: return "create";
        case P0_PATCH_DELETE: return "delete";
        case P0_PATCH_RENAME: return "rename";
    }
    return "unknown";
}

/* ------------------------------------------------------------------ SHA-256 */

typedef struct {
    uint32_t h[8];
    uint64_t bits;
    unsigned char block[64];
    size_t used;
} Sha256;

static uint32_t rotr32(uint32_t x, unsigned n) {
    return (x >> n) | (x << (32U - n));
}

static void sha_transform(Sha256 *s, const unsigned char block[64]) {
    static const uint32_t k[64] = {
        0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,
        0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
        0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,
        0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
        0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,
        0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
        0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,
        0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
        0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,
        0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
        0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,
        0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
        0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,
        0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
        0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,
        0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U
    };
    uint32_t w[64];
    for (size_t i = 0; i < 16; i++)
        w[i] = ((uint32_t)block[i * 4] << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8) |
               (uint32_t)block[i * 4 + 3];
    for (size_t i = 16; i < 64; i++) {
        uint32_t a = w[i - 15], b = w[i - 2];
        uint32_t s0 = rotr32(a, 7) ^ rotr32(a, 18) ^ (a >> 3);
        uint32_t s1 = rotr32(b, 17) ^ rotr32(b, 19) ^ (b >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    uint32_t a=s->h[0], b=s->h[1], c=s->h[2], d=s->h[3];
    uint32_t e=s->h[4], f=s->h[5], g=s->h[6], h=s->h[7];
    for (size_t i = 0; i < 64; i++) {
        uint32_t s1 = rotr32(e,6) ^ rotr32(e,11) ^ rotr32(e,25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t t1 = h + s1 + ch + k[i] + w[i];
        uint32_t s0 = rotr32(a,2) ^ rotr32(a,13) ^ rotr32(a,22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = s0 + maj;
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    s->h[0]+=a; s->h[1]+=b; s->h[2]+=c; s->h[3]+=d;
    s->h[4]+=e; s->h[5]+=f; s->h[6]+=g; s->h[7]+=h;
}

static void sha_init(Sha256 *s) {
    static const uint32_t init[8] = {
        0x6a09e667U,0xbb67ae85U,0x3c6ef372U,0xa54ff53aU,
        0x510e527fU,0x9b05688cU,0x1f83d9abU,0x5be0cd19U
    };
    memcpy(s->h, init, sizeof init);
    s->bits = 0;
    s->used = 0;
}

static void sha_update(Sha256 *s, const void *data_, size_t len) {
    const unsigned char *data = data_;
    s->bits += (uint64_t)len * 8U;
    while (len) {
        size_t n = 64 - s->used;
        if (n > len) n = len;
        memcpy(s->block + s->used, data, n);
        s->used += n;
        data += n;
        len -= n;
        if (s->used == 64) {
            sha_transform(s, s->block);
            s->used = 0;
        }
    }
}

static void sha_final(Sha256 *s, unsigned char out[32]) {
    uint64_t bits = s->bits;
    s->block[s->used++] = 0x80;
    if (s->used > 56) {
        while (s->used < 64) s->block[s->used++] = 0;
        sha_transform(s, s->block);
        s->used = 0;
    }
    while (s->used < 56) s->block[s->used++] = 0;
    for (int i = 7; i >= 0; i--)
        s->block[s->used++] = (unsigned char)(bits >> (unsigned)(i * 8));
    sha_transform(s, s->block);
    for (size_t i = 0; i < 8; i++) {
        out[i * 4]     = (unsigned char)(s->h[i] >> 24);
        out[i * 4 + 1] = (unsigned char)(s->h[i] >> 16);
        out[i * 4 + 2] = (unsigned char)(s->h[i] >> 8);
        out[i * 4 + 3] = (unsigned char)s->h[i];
    }
}

static void digest_hex(const void *data, size_t len,
                       char out[P0_PATCH_DIGEST_HEX]) {
    static const char hex[] = "0123456789abcdef";
    unsigned char raw[32];
    Sha256 s;
    sha_init(&s);
    sha_update(&s, data, len);
    sha_final(&s, raw);
    for (size_t i = 0; i < sizeof raw; i++) {
        out[i * 2] = hex[raw[i] >> 4];
        out[i * 2 + 1] = hex[raw[i] & 15];
    }
    out[64] = '\0';
}

static void sha_u64(Sha256 *s, uint64_t v) {
    unsigned char b[8];
    for (int i = 7; i >= 0; i--) { b[i] = (unsigned char)v; v >>= 8; }
    sha_update(s, b, sizeof b);
}

static void sha_string(Sha256 *s, const char *str) {
    size_t n = str ? strlen(str) : 0;
    sha_u64(s, n);
    if (n) sha_update(s, str, n);
}

static void sha_to_hex(Sha256 *s, char out[P0_PATCH_DIGEST_HEX]) {
    static const char hex[] = "0123456789abcdef";
    unsigned char raw[32];
    sha_final(s, raw);
    for (size_t i = 0; i < sizeof raw; i++) {
        out[i * 2] = hex[raw[i] >> 4];
        out[i * 2 + 1] = hex[raw[i] & 15];
    }
    out[64] = '\0';
}

static void artifact_hash(P0PatchArtifact *a) {
    Sha256 base, full;
    sha_init(&base);
    sha_init(&full);
    sha_string(&base, "p0patch-base-v1");
    sha_string(&full, "p0patch-artifact-v1");
    sha_u64(&base, (uint64_t)a->detached);
    sha_u64(&full, (uint64_t)a->detached);
    sha_u64(&base, a->count);
    sha_u64(&full, a->count);
    for (size_t i = 0; i < a->count; i++) {
        P0PatchOp *op = &a->ops[i];
        sha_u64(&base, (uint64_t)op->kind);
        sha_u64(&full, (uint64_t)op->kind);
        sha_string(&base, op->path);
        sha_string(&base, op->path2);
        sha_string(&full, op->path);
        sha_string(&full, op->path2);
        sha_u64(&base, (uint64_t)op->has_before);
        sha_u64(&base, (uint64_t)op->before_mode);
        sha_u64(&base, (uint64_t)op->before_len);
        sha_string(&base, op->has_before ? op->before_digest : "");
        sha_u64(&full, (uint64_t)op->has_before);
        sha_u64(&full, (uint64_t)op->before_mode);
        sha_u64(&full, (uint64_t)op->before_len);
        sha_string(&full, op->has_before ? op->before_digest : "");
        sha_u64(&full, (uint64_t)op->has_after);
        sha_u64(&full, (uint64_t)op->after_mode);
        sha_u64(&full, (uint64_t)op->after_len);
        sha_string(&full, op->has_after ? op->after_digest : "");
    }
    sha_to_hex(&base, a->base_digest);
    sha_to_hex(&full, a->digest);
}

/* ---------------------------------------------------------- contained paths */

static int path_safe_syntax(const char *path) {
    if (!path || !*path || path[0] == '/' || strlen(path) >= PATH_MAX) return 0;
    const char *p = path;
    while (*p) {
        const char *slash = strchr(p, '/');
        size_t n = slash ? (size_t)(slash - p) : strlen(p);
        if (n == 0 || n > NAME_MAX || (n == 1 && p[0] == '.') ||
            (n == 2 && p[0] == '.' && p[1] == '.')) return 0;
        for (size_t i = 0; i < n; i++)
            if ((unsigned char)p[i] < 0x20 || p[i] == 0x7f) return 0;
        if (!slash) break;
        p = slash + 1;
        if (!*p) return 0;
    }
    return 1;
}

static int open_root_fd(const char *root) {
    return open(root, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
}

static int open_parent_fd(int rootfd, const char *path,
                          char leaf[NAME_MAX + 1]) {
    if (!path_safe_syntax(path)) { errno = EPERM; return -1; }
    char work[PATH_MAX];
    snprintf(work, sizeof work, "%s", path);
    char *last = strrchr(work, '/');
    if (last) {
        snprintf(leaf, NAME_MAX + 1, "%s", last + 1);
        *last = '\0';
    } else {
        snprintf(leaf, NAME_MAX + 1, "%s", work);
        work[0] = '\0';
    }
    int fd = dup(rootfd);
    if (fd < 0) return -1;
    if (*work) {
        char *save = NULL;
        char *part = strtok_r(work, "/", &save);
        while (part) {
            int next = openat(fd, part,
                              O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            if (next < 0) { close(fd); return -1; }
            close(fd);
            fd = next;
            part = strtok_r(NULL, "/", &save);
        }
    }
    return fd;
}

static P0PatchResult read_image(int rootfd, const char *path, int *exists,
                                unsigned char **data, size_t *len, mode_t *mode,
                                char digest[P0_PATCH_DIGEST_HEX],
                                P0PatchReport *report) {
    char leaf[NAME_MAX + 1];
    int pfd = open_parent_fd(rootfd, path, leaf);
    if (pfd < 0) {
        P0PatchResult r = errno == EPERM ? P0_PATCH_UNSAFE_PATH : P0_PATCH_IO_ERROR;
        report_set(report, r, "parent_resolve:path=%s:errno=%d", path, errno);
        return r;
    }
    struct stat st;
    if (fstatat(pfd, leaf, &st, AT_SYMLINK_NOFOLLOW) != 0) {
        if (errno == ENOENT) {
            close(pfd);
            *exists = 0; *data = NULL; *len = 0; *mode = 0; digest[0] = '\0';
            return P0_PATCH_OK;
        }
        report_set(report, P0_PATCH_IO_ERROR, "stat:path=%s:errno=%d", path, errno);
        close(pfd);
        return P0_PATCH_IO_ERROR;
    }
    if (!S_ISREG(st.st_mode)) {
        report_set(report, P0_PATCH_UNSAFE_PATH, "non_regular:path=%s", path);
        close(pfd);
        return P0_PATCH_UNSAFE_PATH;
    }
    if (st.st_size < 0 || (uintmax_t)st.st_size > P0_PATCH_MAX_BYTES) {
        report_set(report, P0_PATCH_TOO_LARGE, "image_limit:path=%s", path);
        close(pfd);
        return P0_PATCH_TOO_LARGE;
    }
    int fd = openat(pfd, leaf, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    close(pfd);
    if (fd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "open:path=%s:errno=%d", path, errno);
        return P0_PATCH_IO_ERROR;
    }
    struct stat opened;
    if (fstat(fd, &opened) != 0 || opened.st_dev != st.st_dev ||
        opened.st_ino != st.st_ino || opened.st_size != st.st_size ||
        (opened.st_mode & 07777) != (st.st_mode & 07777)) {
        close(fd);
        report_set(report, P0_PATCH_CONFLICT, "pre_read_race:path=%s", path);
        return P0_PATCH_CONFLICT;
    }
    if (opened.st_nlink != 1) {
        close(fd);
        report_set(report, P0_PATCH_UNSAFE_PATH,
                   "hardlink_topology:path=%s:nlink=%lu", path,
                   (unsigned long)opened.st_nlink);
        return P0_PATCH_UNSAFE_PATH;
    }
    size_t n = (size_t)opened.st_size;
    unsigned char *buf = malloc(n ? n : 1);
    if (!buf) {
        close(fd);
        report_set(report, P0_PATCH_IO_ERROR, "alloc:image:path=%s", path);
        return P0_PATCH_IO_ERROR;
    }
    size_t off = 0;
    while (off < n) {
        ssize_t got = read(fd, buf + off, n - off);
        if (got < 0 && errno == EINTR) continue;
        if (got <= 0) break;
        off += (size_t)got;
    }
    struct stat after;
    int stable = fstat(fd, &after) == 0 && after.st_dev == opened.st_dev &&
                 after.st_ino == opened.st_ino && after.st_size == opened.st_size &&
                 (after.st_mode & 07777) == (opened.st_mode & 07777) &&
                 after.st_mtim.tv_sec == opened.st_mtim.tv_sec &&
                 after.st_mtim.tv_nsec == opened.st_mtim.tv_nsec &&
                 after.st_ctim.tv_sec == opened.st_ctim.tv_sec &&
                 after.st_ctim.tv_nsec == opened.st_ctim.tv_nsec;
    close(fd);
    if (off != n || !stable) {
        free(buf);
        report_set(report, P0_PATCH_CONFLICT, "read_race:path=%s", path);
        return P0_PATCH_CONFLICT;
    }
    *exists = 1;
    *data = buf;
    *len = n;
    *mode = st.st_mode & 07777;
    digest_hex(buf, n, digest);
    return P0_PATCH_OK;
}

static int image_matches(int exists, const unsigned char *data, size_t len,
                         mode_t mode, int expected_exists,
                         const unsigned char *expected, size_t expected_len,
                         mode_t expected_mode) {
    if (exists != expected_exists) return 0;
    if (!exists) return 1;
    return len == expected_len && mode == expected_mode &&
           (len == 0 || memcmp(data, expected, len) == 0);
}

static int copy_bytes(unsigned char **out, const void *data, size_t len) {
    unsigned char *p = malloc(len ? len : 1);
    if (!p) return 0;
    if (len) memcpy(p, data, len);
    *out = p;
    return 1;
}

static int ensure_parent_dirs(int rootfd, const char *path) {
    if (!path_safe_syntax(path)) { errno = EPERM; return 0; }
    char work[PATH_MAX];
    snprintf(work, sizeof work, "%s", path);
    char *last = strrchr(work, '/');
    if (!last) return 1;
    *last = '\0';
    int fd = dup(rootfd);
    if (fd < 0) return 0;
    char *save = NULL;
    char *part = strtok_r(work, "/", &save);
    while (part) {
        int next = openat(fd, part,
                          O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
        if (next < 0 && errno == ENOENT) {
            if (mkdirat(fd, part, 0700) != 0 && errno != EEXIST) {
                close(fd); return 0;
            }
            next = openat(fd, part,
                          O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
        }
        if (next < 0) { close(fd); return 0; }
        close(fd);
        fd = next;
        part = strtok_r(NULL, "/", &save);
    }
    close(fd);
    return 1;
}

static unsigned long temp_seq;

static P0PatchResult write_atomic_file(int rootfd, const char *path,
                                       const unsigned char *data, size_t len,
                                       mode_t mode, int create_parents,
                                       int no_replace,
                                       P0PatchReport *report) {
    if (create_parents && !ensure_parent_dirs(rootfd, path)) {
        report_set(report, P0_PATCH_IO_ERROR, "parent_create:path=%s:errno=%d",
                   path, errno);
        return P0_PATCH_IO_ERROR;
    }
    char leaf[NAME_MAX + 1];
    int pfd = open_parent_fd(rootfd, path, leaf);
    if (pfd < 0) {
        P0PatchResult r = errno == EPERM ? P0_PATCH_UNSAFE_PATH : P0_PATCH_IO_ERROR;
        report_set(report, r, "parent_resolve:path=%s:errno=%d", path, errno);
        return r;
    }
    char tmp[96];
    int fd = -1;
    for (int tries = 0; tries < 100; tries++) {
        snprintf(tmp, sizeof tmp, ".p0patch-%ld-%lu", (long)getpid(), ++temp_seq);
        fd = openat(pfd, tmp, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW,
                    0600);
        if (fd >= 0 || errno != EEXIST) break;
    }
    if (fd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "temp_create:path=%s:errno=%d",
                   path, errno);
        close(pfd);
        return P0_PATCH_IO_ERROR;
    }
    size_t off = 0;
    while (off < len) {
        ssize_t n = write(fd, data + off, len - off);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        off += (size_t)n;
    }
    int good = off == len && fchmod(fd, mode & 07777) == 0 && fsync(fd) == 0;
    int saved = errno;
    close(fd);
    int installed = 0;
    if (good) {
        if (no_replace) {
            installed = linkat(pfd, tmp, pfd, leaf, 0) == 0;
            if (installed && unlinkat(pfd, tmp, 0) != 0) {
                saved = errno;
                (void)unlinkat(pfd, leaf, 0);
                installed = 0;
            }
        } else {
            installed = renameat(pfd, tmp, pfd, leaf) == 0;
        }
        if (!installed) saved = errno;
    }
    if (!good || !installed) {
        unlinkat(pfd, tmp, 0);
        P0PatchResult r = no_replace && saved == EEXIST ?
                          P0_PATCH_CONFLICT : P0_PATCH_IO_ERROR;
        report_set(report, r, "install:path=%s:errno=%d", path, saved);
        close(pfd);
        return r;
    }
    (void)fsync(pfd);
    close(pfd);
    return P0_PATCH_OK;
}

static P0PatchResult unlink_regular(int rootfd, const char *path, int missing_ok,
                                    P0PatchReport *report) {
    char leaf[NAME_MAX + 1];
    int pfd = open_parent_fd(rootfd, path, leaf);
    if (pfd < 0) {
        if (missing_ok && errno == ENOENT) return P0_PATCH_OK;
        P0PatchResult r = errno == EPERM ? P0_PATCH_UNSAFE_PATH : P0_PATCH_IO_ERROR;
        report_set(report, r, "parent_resolve:path=%s:errno=%d", path, errno);
        return r;
    }
    struct stat st;
    if (fstatat(pfd, leaf, &st, AT_SYMLINK_NOFOLLOW) != 0) {
        if (missing_ok && errno == ENOENT) { close(pfd); return P0_PATCH_OK; }
        report_set(report, P0_PATCH_IO_ERROR, "stat:path=%s:errno=%d", path, errno);
        close(pfd);
        return P0_PATCH_IO_ERROR;
    }
    if (!S_ISREG(st.st_mode)) {
        report_set(report, P0_PATCH_UNSAFE_PATH, "non_regular:path=%s", path);
        close(pfd);
        return P0_PATCH_UNSAFE_PATH;
    }
    if (unlinkat(pfd, leaf, 0) != 0) {
        report_set(report, P0_PATCH_IO_ERROR, "unlink:path=%s:errno=%d", path, errno);
        close(pfd);
        return P0_PATCH_IO_ERROR;
    }
    (void)fsync(pfd);
    close(pfd);
    return P0_PATCH_OK;
}

static P0PatchResult rename_regular(int rootfd, const char *from, const char *to,
                                    P0PatchReport *report) {
    char a[NAME_MAX + 1], b[NAME_MAX + 1];
    int afd = open_parent_fd(rootfd, from, a);
    if (afd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "rename_source_resolve:path=%s:errno=%d",
                   from, errno);
        return P0_PATCH_IO_ERROR;
    }
    int bfd = open_parent_fd(rootfd, to, b);
    if (bfd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "rename_target_resolve:path=%s:errno=%d",
                   to, errno);
        close(afd);
        return P0_PATCH_IO_ERROR;
    }
    struct stat st;
    if (fstatat(afd, a, &st, AT_SYMLINK_NOFOLLOW) != 0 || !S_ISREG(st.st_mode)) {
        report_set(report, P0_PATCH_IO_ERROR, "rename_source_non_regular:path=%s", from);
        close(afd); close(bfd);
        return P0_PATCH_IO_ERROR;
    }
    if (fstatat(bfd, b, &st, AT_SYMLINK_NOFOLLOW) == 0 || errno != ENOENT) {
        report_set(report, P0_PATCH_CONFLICT, "rename_target_exists:path=%s", to);
        close(afd); close(bfd);
        return P0_PATCH_CONFLICT;
    }
    /* link+unlink is the portable no-replace installation for regular files.
     * Unlike renameat, linkat can never overwrite a target created after the
     * preimage pass.  If the unlink half fails the transaction reports red and
     * rollback removes the new link while preserving/rebuilding the source. */
    if (linkat(afd, a, bfd, b, 0) != 0) {
        P0PatchResult r = errno == EEXIST ? P0_PATCH_CONFLICT : P0_PATCH_IO_ERROR;
        report_set(report, r, "rename_target_install:path=%s:errno=%d", to, errno);
        close(afd); close(bfd);
        return r;
    }
    if (unlinkat(afd, a, 0) != 0) {
        int saved = errno;
        (void)unlinkat(bfd, b, 0);
        report_set(report, P0_PATCH_IO_ERROR, "rename_source_unlink:path=%s:errno=%d",
                   from, saved);
        close(afd); close(bfd);
        return P0_PATCH_IO_ERROR;
    }
    (void)fsync(afd);
    if (bfd != afd) (void)fsync(bfd);
    close(afd); close(bfd);
    return P0_PATCH_OK;
}

static P0PatchResult verify_one(int rootfd, const char *path, int expected_exists,
                                const unsigned char *expected, size_t expected_len,
                                mode_t expected_mode, P0PatchReport *report) {
    int exists;
    unsigned char *data = NULL;
    size_t len;
    mode_t mode;
    char digest[P0_PATCH_DIGEST_HEX];
    P0PatchResult r = read_image(rootfd, path, &exists, &data, &len, &mode,
                                 digest, report);
    if (r != P0_PATCH_OK) return r;
    int match = image_matches(exists, data, len, mode, expected_exists,
                              expected, expected_len, expected_mode);
    free(data);
    if (!match) {
        report_set(report, P0_PATCH_CONFLICT, "image_conflict:path=%s", path);
        return P0_PATCH_CONFLICT;
    }
    return P0_PATCH_OK;
}

static P0PatchResult verify_all(const P0PatchArtifact *a, int rootfd, int after,
                                P0PatchReport *report) {
    for (size_t i = 0; i < a->count; i++) {
        const P0PatchOp *op = &a->ops[i];
        P0PatchResult r;
        if (!after) {
            r = verify_one(rootfd, op->path, op->has_before, op->before,
                           op->before_len, op->before_mode, report);
            if (r != P0_PATCH_OK) return r;
            if (op->kind == P0_PATCH_RENAME) {
                r = verify_one(rootfd, op->path2, 0, NULL, 0, 0, report);
                if (r != P0_PATCH_OK) return r;
            }
        } else if (op->kind == P0_PATCH_RENAME) {
            r = verify_one(rootfd, op->path, 0, NULL, 0, 0, report);
            if (r != P0_PATCH_OK) return r;
            r = verify_one(rootfd, op->path2, op->has_after, op->after,
                           op->after_len, op->after_mode, report);
            if (r != P0_PATCH_OK) return r;
        } else {
            r = verify_one(rootfd, op->path, op->has_after, op->after,
                           op->after_len, op->after_mode, report);
            if (r != P0_PATCH_OK) return r;
        }
    }
    return P0_PATCH_OK;
}

static P0PatchResult apply_all(const P0PatchArtifact *a, int rootfd,
                               int create_parents, P0PatchReport *report) {
    for (size_t i = 0; i < a->count; i++) {
        const P0PatchOp *op = &a->ops[i];
        P0PatchResult r;
        switch (op->kind) {
            case P0_PATCH_EDIT:
            case P0_PATCH_CREATE:
                r = write_atomic_file(rootfd, op->path, op->after, op->after_len,
                                      op->after_mode, create_parents,
                                      op->kind == P0_PATCH_CREATE, report);
                break;
            case P0_PATCH_DELETE:
                r = unlink_regular(rootfd, op->path, 0, report);
                break;
            case P0_PATCH_RENAME:
                if (create_parents && !ensure_parent_dirs(rootfd, op->path2)) {
                    report_set(report, P0_PATCH_IO_ERROR,
                               "parent_create:path=%s", op->path2);
                    r = P0_PATCH_IO_ERROR;
                } else {
                    r = rename_regular(rootfd, op->path, op->path2, report);
                }
                break;
            default:
                r = P0_PATCH_INVALID;
                report_set(report, r, "op_unknown");
                break;
        }
        if (r != P0_PATCH_OK) return r;
    }
    return P0_PATCH_OK;
}

static int remove_tree_fd(int dirfd) {
    (void)fchmod(dirfd, 0700);
    int scanfd = dup(dirfd);
    if (scanfd < 0) return 0;
    DIR *d = fdopendir(scanfd);
    if (!d) { close(scanfd); return 0; }
    int good = 1;
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        struct stat st;
        if (fstatat(dirfd, e->d_name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
            good = 0; continue;
        }
        if (S_ISDIR(st.st_mode)) {
            int child = openat(dirfd, e->d_name,
                               O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            if (child < 0 || !remove_tree_fd(child) ||
                unlinkat(dirfd, e->d_name, AT_REMOVEDIR) != 0) good = 0;
            if (child >= 0) close(child);
        } else if (unlinkat(dirfd, e->d_name, 0) != 0) {
            good = 0;
        }
    }
    closedir(d);
    return good;
}

static void remove_stage(const char *path) {
    int fd = open_root_fd(path);
    if (fd >= 0) {
        (void)remove_tree_fd(fd);
        close(fd);
    }
    (void)rmdir(path);
}

typedef struct {
    uint64_t bytes;
    uint64_t files;
} ContextSize;

static int context_root_omitted(const char *name) {
    return strcmp(name, ".git") == 0 || strcmp(name, ".cache") == 0 ||
           strcmp(name, ".venv") == 0;
}

static P0PatchResult copy_context_tree(int srcfd, int dstfd, int depth,
                                       ContextSize *size,
                                       P0PatchReport *report) {
    int scanfd = dup(srcfd);
    if (scanfd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "context_scan:dup");
        return P0_PATCH_IO_ERROR;
    }
    DIR *dir = fdopendir(scanfd);
    if (!dir) {
        close(scanfd);
        report_set(report, P0_PATCH_IO_ERROR, "context_scan:fdopendir");
        return P0_PATCH_IO_ERROR;
    }
    P0PatchResult result = P0_PATCH_OK;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
            (depth == 0 && context_root_omitted(name))) continue;
        if (depth >= 128 || size->files >= P0_PATCH_MAX_CONTEXT_FILES) {
            report_set(report, P0_PATCH_TOO_LARGE,
                       "context_limit:depth_or_files");
            result = P0_PATCH_TOO_LARGE;
            break;
        }
        size->files++;
        struct stat before;
        if (fstatat(srcfd, name, &before, AT_SYMLINK_NOFOLLOW) != 0) {
            report_set(report, P0_PATCH_CONFLICT,
                       "context_race:entry=%s", name);
            result = P0_PATCH_CONFLICT;
            break;
        }
        if (S_ISDIR(before.st_mode)) {
            if (mkdirat(dstfd, name, 0700) != 0) {
                report_set(report, P0_PATCH_IO_ERROR,
                           "context_mkdir:entry=%s:errno=%d", name, errno);
                result = P0_PATCH_IO_ERROR;
                break;
            }
            int s = openat(srcfd, name,
                           O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            int d = openat(dstfd, name,
                           O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
            if (s < 0 || d < 0) {
                if (s >= 0) close(s);
                if (d >= 0) close(d);
                report_set(report, P0_PATCH_UNSAFE_PATH,
                           "context_dir_open:entry=%s", name);
                result = P0_PATCH_UNSAFE_PATH;
                break;
            }
            result = copy_context_tree(s, d, depth + 1, size, report);
            if (result == P0_PATCH_OK && fchmod(d, before.st_mode & 07777) != 0) {
                report_set(report, P0_PATCH_IO_ERROR,
                           "context_dir_mode:entry=%s", name);
                result = P0_PATCH_IO_ERROR;
            }
            close(s); close(d);
            if (result != P0_PATCH_OK) break;
        } else if (S_ISREG(before.st_mode)) {
            if (before.st_size < 0 ||
                (uint64_t)before.st_size > P0_PATCH_MAX_CONTEXT_BYTES - size->bytes ||
                size->files > P0_PATCH_MAX_CONTEXT_FILES) {
                report_set(report, P0_PATCH_TOO_LARGE,
                           "context_limit:bytes_or_files");
                result = P0_PATCH_TOO_LARGE;
                break;
            }
            int s = openat(srcfd, name, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
            int d = openat(dstfd, name,
                           O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW,
                           0600);
            if (s < 0 || d < 0) {
                if (s >= 0) close(s);
                if (d >= 0) close(d);
                report_set(report, P0_PATCH_UNSAFE_PATH,
                           "context_file_open:entry=%s", name);
                result = P0_PATCH_UNSAFE_PATH;
                break;
            }
            int copied = 0;
#ifdef FICLONE
            if (ioctl(d, FICLONE, s) == 0) copied = 1;
#endif
            if (!copied) {
                unsigned char buf[65536];
                ssize_t n;
                while ((n = read(s, buf, sizeof buf)) != 0) {
                    if (n < 0 && errno == EINTR) continue;
                    if (n < 0) break;
                    size_t off = 0;
                    while (off < (size_t)n) {
                        ssize_t w = write(d, buf + off, (size_t)n - off);
                        if (w < 0 && errno == EINTR) continue;
                        if (w <= 0) { n = -1; break; }
                        off += (size_t)w;
                    }
                    if (n < 0) break;
                }
                copied = n == 0;
            }
            struct stat after;
            int stable = fstat(s, &after) == 0 &&
                         after.st_dev == before.st_dev && after.st_ino == before.st_ino &&
                         after.st_size == before.st_size &&
                         after.st_mtim.tv_sec == before.st_mtim.tv_sec &&
                         after.st_mtim.tv_nsec == before.st_mtim.tv_nsec;
            int mode_ok = fchmod(d, before.st_mode & 07777) == 0;
            close(s); close(d);
            if (!copied || !stable || !mode_ok) {
                report_set(report, stable ? P0_PATCH_IO_ERROR : P0_PATCH_CONFLICT,
                           "context_file_snapshot:entry=%s", name);
                result = stable ? P0_PATCH_IO_ERROR : P0_PATCH_CONFLICT;
                break;
            }
            size->bytes += (uint64_t)before.st_size;
        } else {
            /* The containment kernel does not follow symlinks or special files;
             * candidate materialization obeys the same boundary. */
            report_set(report, P0_PATCH_UNSAFE_PATH,
                       "context_unsupported:entry=%s", name);
            result = P0_PATCH_UNSAFE_PATH;
            break;
        }
    }
    closedir(dir);
    return result;
}

static P0PatchResult materialize_context(int stagefd, P0PatchReport *report) {
    int srcfd = open_root_fd(p0_root());
    if (srcfd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "context_root_open");
        return P0_PATCH_IO_ERROR;
    }
    ContextSize size = {0, 0};
    P0PatchResult r = copy_context_tree(srcfd, stagefd, 0, &size, report);
    close(srcfd);
    return r;
}

static int paths_overlap(const P0PatchArtifact *a) {
    for (size_t i = 0; i < a->count; i++) {
        const char *ip[2] = { a->ops[i].path, a->ops[i].path2 };
        size_t in = a->ops[i].kind == P0_PATCH_RENAME ? 2 : 1;
        for (size_t ii = 0; ii < in; ii++) {
            for (size_t j = i; j < a->count; j++) {
                const char *jp[2] = { a->ops[j].path, a->ops[j].path2 };
                size_t jn = a->ops[j].kind == P0_PATCH_RENAME ? 2 : 1;
                for (size_t jj = 0; jj < jn; jj++) {
                    if (i == j && ii == jj) continue;
                    if (strcmp(ip[ii], jp[jj]) == 0) return 1;
                }
            }
        }
    }
    return 0;
}

void p0_patch_free(P0PatchArtifact *a) {
    if (!a) return;
    for (size_t i = 0; i < a->count; i++) {
        free(a->ops[i].path);
        free(a->ops[i].path2);
        free(a->ops[i].before);
        free(a->ops[i].after);
    }
    free(a->ops);
    free(a);
}

size_t p0_patch_count(const P0PatchArtifact *a) { return a ? a->count : 0; }

int p0_patch_op(const P0PatchArtifact *a, size_t index, P0PatchOpView *out) {
    if (!a || !out || index >= a->count) return 0;
    const P0PatchOp *op = &a->ops[index];
    out->kind = op->kind;
    out->path = op->path;
    out->path2 = op->path2;
    out->has_before = op->has_before;
    out->before = op->before;
    out->before_len = op->before_len;
    out->before_mode = op->before_mode;
    out->before_digest = op->before_digest;
    out->has_after = op->has_after;
    out->after = op->after;
    out->after_len = op->after_len;
    out->after_mode = op->after_mode;
    out->after_digest = op->after_digest;
    return 1;
}

const char *p0_patch_base_digest(const P0PatchArtifact *a) {
    return a ? a->base_digest : "";
}

const char *p0_patch_digest(const P0PatchArtifact *a) {
    return a ? a->digest : "";
}

int p0_patch_is_detached(const P0PatchArtifact *a) { return a ? a->detached : 0; }

static P0PatchResult validate_stage(const P0PatchArtifact *a,
                                    P0PatchCheckFn stage_check, void *check_ctx,
                                    P0PatchReport *report) {
    char stage[] = "/tmp/parrot0-patch-stage-XXXXXX";
    if (!mkdtemp(stage)) {
        report_set(report, P0_PATCH_IO_ERROR, "stage_create:errno=%d", errno);
        return P0_PATCH_IO_ERROR;
    }
    const char *workspace = p0_root();
    size_t wr = strlen(workspace);
    if (strcmp(workspace, "/") == 0 || strcmp(stage, workspace) == 0 ||
        (wr && strncmp(stage, workspace, wr) == 0 && stage[wr] == '/')) {
        remove_stage(stage);
        report_set(report, P0_PATCH_UNSAFE_PATH,
                   "stage_nested_in_workspace");
        return P0_PATCH_UNSAFE_PATH;
    }
    int stagefd = open_root_fd(stage);
    if (stagefd < 0) {
        remove_stage(stage);
        report_set(report, P0_PATCH_IO_ERROR, "stage_open");
        return P0_PATCH_IO_ERROR;
    }
    P0PatchResult r = P0_PATCH_OK;
    if (stage_check) r = materialize_context(stagefd, report);
    for (size_t i = 0; i < a->count && r == P0_PATCH_OK; i++) {
        const P0PatchOp *op = &a->ops[i];
        if (op->has_before)
            r = write_atomic_file(stagefd, op->path, op->before, op->before_len,
                                  op->before_mode, 1, 0, report);
        if (r == P0_PATCH_OK && op->kind == P0_PATCH_RENAME &&
            !ensure_parent_dirs(stagefd, op->path2)) {
            report_set(report, P0_PATCH_IO_ERROR, "stage_path:path=%s", op->path2);
            r = P0_PATCH_IO_ERROR;
        }
    }
    if (r == P0_PATCH_OK) r = verify_all(a, stagefd, 0, report);
    if (r == P0_PATCH_OK) r = apply_all(a, stagefd, 1, report);
    if (r == P0_PATCH_OK) r = verify_all(a, stagefd, 1, report);
    if (r == P0_PATCH_OK && stage_check) {
        char why[P0_PATCH_DETAIL] = "";
        if (!stage_check(stage, check_ctx, why, sizeof why)) {
            report_set(report, P0_PATCH_STAGE_CHECK_FAILED, "%s",
                       *why ? why : "stage_check_red");
            r = P0_PATCH_STAGE_CHECK_FAILED;
        }
    }
    if (r == P0_PATCH_OK && stage_check) {
        P0PatchReport check_report;
        report_init(&check_report);
        if (verify_all(a, stagefd, 1, &check_report) != P0_PATCH_OK) {
            report_set(report, P0_PATCH_STAGE_CHECK_FAILED,
                       "stage_check_mutation:%s",
                       check_report.detail);
            r = P0_PATCH_STAGE_CHECK_FAILED;
        }
    }
    close(stagefd);
    remove_stage(stage);
    return r;
}

P0PatchResult p0_patch_prepare(const P0PatchSpec *specs, size_t count,
                               P0PatchCheckFn stage_check, void *check_ctx,
                               P0PatchArtifact **out, P0PatchReport *report) {
    report_init(report);
    if (out) *out = NULL;
    if (!out || !specs || count == 0 || count > P0_PATCH_MAX_OPS) {
        report_set(report, P0_PATCH_INVALID, "op_count:limit=%u",
                   (unsigned)P0_PATCH_MAX_OPS);
        return P0_PATCH_INVALID;
    }
    P0PatchArtifact *a = calloc(1, sizeof *a);
    if (!a) {
        report_set(report, P0_PATCH_IO_ERROR, "alloc:artifact");
        return P0_PATCH_IO_ERROR;
    }
    a->ops = calloc(count, sizeof *a->ops);
    if (!a->ops) { p0_patch_free(a); report_set(report, P0_PATCH_IO_ERROR, "alloc:ops"); return P0_PATCH_IO_ERROR; }
    a->count = count;

    for (size_t i = 0; i < count; i++) {
        const P0PatchSpec *s = &specs[i];
        P0PatchOp *op = &a->ops[i];
        if (s->kind < P0_PATCH_EDIT || s->kind > P0_PATCH_RENAME ||
            !path_safe_syntax(s->path) ||
            (s->kind == P0_PATCH_RENAME && !path_safe_syntax(s->path2)) ||
            (s->kind != P0_PATCH_RENAME && s->path2) ||
            ((s->kind == P0_PATCH_EDIT || s->kind == P0_PATCH_CREATE) &&
             s->post_len && !s->post_data) ||
            ((s->kind == P0_PATCH_DELETE || s->kind == P0_PATCH_RENAME) &&
             (s->post_data || s->post_len || s->post_mode))) {
            report_set(report, P0_PATCH_INVALID, "spec_invalid:op=%s:index=%zu",
                       p0_patch_op_name(s->kind), i);
            p0_patch_free(a);
            return P0_PATCH_INVALID;
        }
        op->kind = s->kind;
        op->path = strdup(s->path);
        op->path2 = s->path2 ? strdup(s->path2) : NULL;
        if (!op->path || (s->path2 && !op->path2)) {
            report_set(report, P0_PATCH_IO_ERROR, "alloc:path");
            p0_patch_free(a);
            return P0_PATCH_IO_ERROR;
        }
    }
    if (paths_overlap(a)) {
        report_set(report, P0_PATCH_INVALID, "path_overlap");
        p0_patch_free(a);
        return P0_PATCH_INVALID;
    }

    int rootfd = open_root_fd(p0_root());
    if (rootfd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "workspace_root_open");
        p0_patch_free(a);
        return P0_PATCH_IO_ERROR;
    }
    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        const P0PatchSpec *s = &specs[i];
        P0PatchOp *op = &a->ops[i];
        P0PatchResult r = read_image(rootfd, op->path, &op->has_before,
                                     &op->before, &op->before_len, &op->before_mode,
                                     op->before_digest, report);
        if (r != P0_PATCH_OK) { close(rootfd); p0_patch_free(a); return r; }
        if ((op->kind == P0_PATCH_EDIT || op->kind == P0_PATCH_DELETE ||
             op->kind == P0_PATCH_RENAME) && !op->has_before) {
            report_set(report, P0_PATCH_NOT_FOUND, "path_absent:path=%s", op->path);
            close(rootfd); p0_patch_free(a); return P0_PATCH_NOT_FOUND;
        }
        if (op->kind == P0_PATCH_CREATE && op->has_before) {
            report_set(report, P0_PATCH_ALREADY_EXISTS, "path_exists:path=%s", op->path);
            close(rootfd); p0_patch_free(a); return P0_PATCH_ALREADY_EXISTS;
        }
        if (op->kind == P0_PATCH_RENAME) {
            int exists; unsigned char *data = NULL; size_t len; mode_t mode;
            char digest[P0_PATCH_DIGEST_HEX];
            r = read_image(rootfd, op->path2, &exists, &data, &len, &mode, digest, report);
            free(data);
            if (r != P0_PATCH_OK) { close(rootfd); p0_patch_free(a); return r; }
            if (exists) {
                report_set(report, P0_PATCH_ALREADY_EXISTS, "path_exists:path=%s", op->path2);
                close(rootfd); p0_patch_free(a); return P0_PATCH_ALREADY_EXISTS;
            }
        }
        if (op->kind == P0_PATCH_EDIT || op->kind == P0_PATCH_CREATE) {
            if (!copy_bytes(&op->after, s->post_data, s->post_len)) {
                report_set(report, P0_PATCH_IO_ERROR, "alloc:after");
                close(rootfd); p0_patch_free(a); return P0_PATCH_IO_ERROR;
            }
            op->has_after = 1;
            op->after_len = s->post_len;
            op->after_mode = s->post_mode ? (s->post_mode & 07777) :
                             (op->kind == P0_PATCH_EDIT ? op->before_mode : 0644);
        } else if (op->kind == P0_PATCH_RENAME) {
            if (!copy_bytes(&op->after, op->before, op->before_len)) {
                report_set(report, P0_PATCH_IO_ERROR, "alloc:rename_after");
                close(rootfd); p0_patch_free(a); return P0_PATCH_IO_ERROR;
            }
            op->has_after = 1;
            op->after_len = op->before_len;
            op->after_mode = op->before_mode;
        }
        if (op->has_after) digest_hex(op->after, op->after_len, op->after_digest);
        if (op->before_len > P0_PATCH_MAX_BYTES - total) {
            report_set(report, P0_PATCH_TOO_LARGE, "preimage_limit:bytes=%u",
                       (unsigned)P0_PATCH_MAX_BYTES);
            close(rootfd); p0_patch_free(a); return P0_PATCH_TOO_LARGE;
        }
        total += op->before_len;
        if (op->after_len > P0_PATCH_MAX_BYTES - total) {
            report_set(report, P0_PATCH_TOO_LARGE, "image_limit:bytes=%u",
                       (unsigned)P0_PATCH_MAX_BYTES);
            close(rootfd); p0_patch_free(a); return P0_PATCH_TOO_LARGE;
        }
        total += op->after_len;
    }
    close(rootfd);
    artifact_hash(a);

    P0PatchResult r = validate_stage(a, stage_check, check_ctx, report);
    if (r != P0_PATCH_OK) { p0_patch_free(a); return r; }
    *out = a;
    report_set(report, P0_PATCH_OK, "prepared:ops=%zu:artifact=%s",
               a->count, a->digest);
    return P0_PATCH_OK;
}

P0PatchResult p0_patch_prepare_image(const char *label,
                                     const void *before, size_t before_len,
                                     mode_t before_mode,
                                     const void *after, size_t after_len,
                                     mode_t after_mode,
                                     P0PatchCheckFn stage_check, void *check_ctx,
                                     P0PatchArtifact **out, P0PatchReport *report) {
    report_init(report);
    if (out) *out = NULL;
    int too_large = before_len > P0_PATCH_MAX_BYTES ||
                    (before_len <= P0_PATCH_MAX_BYTES &&
                     after_len > P0_PATCH_MAX_BYTES - before_len);
    if (!out || !path_safe_syntax(label) || (before_len && !before) ||
        (after_len && !after) || too_large) {
        P0PatchResult bad = too_large ? P0_PATCH_TOO_LARGE : P0_PATCH_INVALID;
        report_set(report, bad, "detached_image_invalid");
        return bad;
    }
    P0PatchArtifact *a = calloc(1, sizeof *a);
    if (!a) { report_set(report, P0_PATCH_IO_ERROR, "alloc:artifact"); return P0_PATCH_IO_ERROR; }
    a->ops = calloc(1, sizeof *a->ops);
    if (!a->ops) { p0_patch_free(a); report_set(report, P0_PATCH_IO_ERROR, "alloc:ops"); return P0_PATCH_IO_ERROR; }
    a->count = 1;
    a->detached = 1;
    P0PatchOp *op = &a->ops[0];
    op->kind = P0_PATCH_EDIT;
    op->path = strdup(label);
    op->has_before = op->has_after = 1;
    op->before_len = before_len;
    op->after_len = after_len;
    op->before_mode = before_mode ? (before_mode & 07777) : 0644;
    op->after_mode = after_mode ? (after_mode & 07777) : op->before_mode;
    if (!op->path || !copy_bytes(&op->before, before, before_len) ||
        !copy_bytes(&op->after, after, after_len)) {
        p0_patch_free(a);
        report_set(report, P0_PATCH_IO_ERROR, "alloc:detached_images");
        return P0_PATCH_IO_ERROR;
    }
    digest_hex(op->before, op->before_len, op->before_digest);
    digest_hex(op->after, op->after_len, op->after_digest);
    artifact_hash(a);
    P0PatchResult r = validate_stage(a, stage_check, check_ctx, report);
    if (r != P0_PATCH_OK) { p0_patch_free(a); return r; }
    *out = a;
    report_set(report, P0_PATCH_OK, "prepared:detached=1:artifact=%s", a->digest);
    return P0_PATCH_OK;
}

/* ---------------------------------------------------------- forward/reverse */

typedef struct {
    char *s;
    size_t len;
    size_t cap;
    int failed;
} TextBuf;

static int tb_need(TextBuf *b, size_t extra) {
    if (b->failed) return 0;
    if (extra > SIZE_MAX - b->len - 1) { b->failed = 1; return 0; }
    size_t need = b->len + extra + 1;
    if (need <= b->cap) return 1;
    size_t cap = b->cap ? b->cap : 1024;
    while (cap < need) {
        if (cap > SIZE_MAX / 2) { cap = need; break; }
        cap *= 2;
    }
    char *p = realloc(b->s, cap);
    if (!p) { b->failed = 1; return 0; }
    b->s = p;
    b->cap = cap;
    return 1;
}

static void tb_mem(TextBuf *b, const void *data, size_t len) {
    if (!tb_need(b, len)) return;
    memcpy(b->s + b->len, data, len);
    b->len += len;
    b->s[b->len] = '\0';
}

static void tb_printf(TextBuf *b, const char *fmt, ...) {
    if (b->failed) return;
    va_list ap, cp;
    va_start(ap, fmt);
    va_copy(cp, ap);
    int n = vsnprintf(NULL, 0, fmt, cp);
    va_end(cp);
    if (n < 0 || !tb_need(b, (size_t)n)) { b->failed = 1; va_end(ap); return; }
    vsnprintf(b->s + b->len, b->cap - b->len, fmt, ap);
    va_end(ap);
    b->len += (size_t)n;
}

static size_t line_count(const unsigned char *data, size_t len) {
    if (!len) return 0;
    size_t n = 0;
    for (size_t i = 0; i < len; i++) if (data[i] == '\n') n++;
    if (data[len - 1] != '\n') n++;
    return n;
}

static void diff_lines(TextBuf *b, char prefix,
                       const unsigned char *data, size_t len) {
    size_t start = 0;
    while (start < len) {
        size_t end = start;
        while (end < len && data[end] != '\n') end++;
        tb_mem(b, &prefix, 1);
        tb_mem(b, data + start, end - start);
        if (end < len) {
            tb_mem(b, "\n", 1);
            start = end + 1;
        } else {
            tb_mem(b, "\n\\ No newline at end of file\n", 29);
            start = end;
        }
    }
}

char *p0_patch_diff(const P0PatchArtifact *a, int reverse, size_t *length_out) {
    if (length_out) *length_out = 0;
    if (!a) return NULL;
    TextBuf b = {0};
    for (size_t i = 0; i < a->count; i++) {
        const P0PatchOp *op = &a->ops[i];
        const char *old_path = op->path;
        const char *new_path = op->kind == P0_PATCH_RENAME ? op->path2 : op->path;
        int old_exists = op->has_before, new_exists = op->has_after;
        const unsigned char *old_data = op->before, *new_data = op->after;
        size_t old_len = op->before_len, new_len = op->after_len;
        mode_t old_mode = op->before_mode, new_mode = op->after_mode;
        if (reverse) {
            const char *p = old_path; old_path = new_path; new_path = p;
            int e = old_exists; old_exists = new_exists; new_exists = e;
            const unsigned char *d = old_data; old_data = new_data; new_data = d;
            size_t n = old_len; old_len = new_len; new_len = n;
            mode_t m = old_mode; old_mode = new_mode; new_mode = m;
        }
        const char *git_old = old_exists ? old_path : new_path;
        const char *git_new = new_exists ? new_path : old_path;
        tb_printf(&b, "diff --git a/%s b/%s\n", git_old, git_new);
        if (!old_exists) tb_printf(&b, "new file mode %06o\n",
                                   (unsigned)(0100000 | new_mode));
        else if (!new_exists) tb_printf(&b, "deleted file mode %06o\n",
                                        (unsigned)(0100000 | old_mode));
        else if (old_mode != new_mode) {
            tb_printf(&b, "old mode %06o\n", (unsigned)(0100000 | old_mode));
            tb_printf(&b, "new mode %06o\n", (unsigned)(0100000 | new_mode));
        }
        if (old_exists) tb_printf(&b, "--- a/%s\n", old_path);
        else tb_printf(&b, "--- /dev/null\n");
        if (new_exists) tb_printf(&b, "+++ b/%s\n", new_path);
        else tb_printf(&b, "+++ /dev/null\n");
        int binary = (old_exists && old_len && memchr(old_data, 0, old_len)) ||
                     (new_exists && new_len && memchr(new_data, 0, new_len));
        if (binary) {
            tb_printf(&b, "Binary files %s and %s differ\n",
                      old_exists ? old_path : "/dev/null",
                      new_exists ? new_path : "/dev/null");
        } else {
            size_t old_lines = line_count(old_data, old_len);
            size_t new_lines = line_count(new_data, new_len);
            tb_printf(&b, "@@ -%u,%zu +%u,%zu @@\n",
                      old_lines ? 1U : 0U, old_lines,
                      new_lines ? 1U : 0U, new_lines);
            if (old_exists) diff_lines(&b, '-', old_data, old_len);
            if (new_exists) diff_lines(&b, '+', new_data, new_len);
        }
    }
    if (b.failed) { free(b.s); return NULL; }
    if (!b.s) {
        b.s = calloc(1, 1);
        if (!b.s) return NULL;
    }
    if (length_out) *length_out = b.len;
    return b.s;
}

/* --------------------------------------------------------------- persistence */

static int put_bytes(FILE *f, const void *p, size_t n) {
    return n == 0 || fwrite(p, 1, n, f) == n;
}

static int get_bytes(FILE *f, void *p, size_t n) {
    return n == 0 || fread(p, 1, n, f) == n;
}

static int put_u32(FILE *f, uint32_t v) {
    unsigned char b[4] = {
        (unsigned char)(v >> 24), (unsigned char)(v >> 16),
        (unsigned char)(v >> 8), (unsigned char)v
    };
    return put_bytes(f, b, sizeof b);
}

static int get_u32(FILE *f, uint32_t *v) {
    unsigned char b[4];
    if (!get_bytes(f, b, sizeof b)) return 0;
    *v = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
         ((uint32_t)b[2] << 8) | (uint32_t)b[3];
    return 1;
}

static int put_u64(FILE *f, uint64_t v) {
    unsigned char b[8];
    for (int i = 7; i >= 0; i--) { b[i] = (unsigned char)v; v >>= 8; }
    return put_bytes(f, b, sizeof b);
}

static int get_u64(FILE *f, uint64_t *v) {
    unsigned char b[8];
    if (!get_bytes(f, b, sizeof b)) return 0;
    *v = 0;
    for (size_t i = 0; i < sizeof b; i++) *v = (*v << 8) | b[i];
    return 1;
}

P0PatchResult p0_patch_write(const P0PatchArtifact *a, FILE *f,
                             P0PatchReport *report) {
    static const unsigned char magic[8] = { 'P','0','P','T','C','H','0','2' };
    report_init(report);
    if (!a || !f || a->count == 0 || a->count > UINT32_MAX) {
        report_set(report, P0_PATCH_INVALID, "persist_input_invalid");
        return P0_PATCH_INVALID;
    }
    unsigned char flags[4] = { (unsigned char)a->detached, 0, 0, 0 };
    int ok = put_bytes(f, magic, sizeof magic) &&
             put_bytes(f, a->base_digest, 64) && put_bytes(f, a->digest, 64) &&
             put_u32(f, (uint32_t)a->count) && put_bytes(f, flags, sizeof flags);
    for (size_t i = 0; i < a->count && ok; i++) {
        const P0PatchOp *op = &a->ops[i];
        unsigned char head[4] = {
            (unsigned char)op->kind, (unsigned char)op->has_before,
            (unsigned char)op->has_after, 0
        };
        size_t p1 = strlen(op->path), p2 = op->path2 ? strlen(op->path2) : 0;
        ok = put_bytes(f, head, sizeof head) &&
             put_u32(f, (uint32_t)op->before_mode) &&
             put_u32(f, (uint32_t)op->after_mode) &&
             put_u32(f, (uint32_t)p1) && put_u32(f, (uint32_t)p2) &&
             put_u64(f, op->before_len) && put_u64(f, op->after_len) &&
             put_bytes(f, op->before_digest, 64) &&
             put_bytes(f, op->after_digest, 64) &&
             put_bytes(f, op->path, p1) && put_bytes(f, op->path2, p2) &&
             put_bytes(f, op->before, op->before_len) &&
             put_bytes(f, op->after, op->after_len);
    }
    if (!ok || fflush(f) != 0 || ferror(f)) {
        report_set(report, P0_PATCH_IO_ERROR, "persist_write");
        return P0_PATCH_IO_ERROR;
    }
    report_set(report, P0_PATCH_OK, "persisted:artifact=%s", a->digest);
    return P0_PATCH_OK;
}

P0PatchResult p0_patch_read(FILE *f, P0PatchArtifact **out,
                            P0PatchReport *report) {
    static const unsigned char magic[8] = { 'P','0','P','T','C','H','0','2' };
    unsigned char got_magic[8];
    unsigned char flags[4];
    char expected_base[P0_PATCH_DIGEST_HEX], expected_id[P0_PATCH_DIGEST_HEX];
    uint32_t count;
    report_init(report);
    if (out) *out = NULL;
    if (!f || !out || !get_bytes(f, got_magic, sizeof got_magic) ||
        memcmp(got_magic, magic, sizeof magic) != 0 ||
        !get_bytes(f, expected_base, 64) || !get_bytes(f, expected_id, 64) ||
        !get_u32(f, &count) || !get_bytes(f, flags, sizeof flags) ||
        flags[0] > 1 || flags[1] || flags[2] || flags[3] ||
        count == 0 || count > P0_PATCH_MAX_OPS) {
        report_set(report, P0_PATCH_CORRUPT, "persist_header_invalid");
        return P0_PATCH_CORRUPT;
    }
    expected_base[64] = expected_id[64] = '\0';
    P0PatchArtifact *a = calloc(1, sizeof *a);
    if (!a) { report_set(report, P0_PATCH_IO_ERROR, "alloc:artifact"); return P0_PATCH_IO_ERROR; }
    a->count = count;
    a->detached = flags[0];
    a->ops = calloc(count, sizeof *a->ops);
    if (!a->ops) { p0_patch_free(a); report_set(report, P0_PATCH_IO_ERROR, "alloc:ops"); return P0_PATCH_IO_ERROR; }
    size_t total = 0;
    for (size_t i = 0; i < a->count; i++) {
        P0PatchOp *op = &a->ops[i];
        unsigned char head[4];
        uint32_t bm, am, p1, p2;
        uint64_t bl, al;
        char bd[P0_PATCH_DIGEST_HEX], ad[P0_PATCH_DIGEST_HEX];
        if (!get_bytes(f, head, sizeof head) || !get_u32(f, &bm) || !get_u32(f, &am) ||
            !get_u32(f, &p1) || !get_u32(f, &p2) || !get_u64(f, &bl) ||
            !get_u64(f, &al) || !get_bytes(f, bd, 64) || !get_bytes(f, ad, 64) ||
            head[0] > P0_PATCH_RENAME || head[1] > 1 || head[2] > 1 ||
            p1 == 0 || p1 >= PATH_MAX || p2 >= PATH_MAX ||
            bl > P0_PATCH_MAX_BYTES || al > P0_PATCH_MAX_BYTES ||
            (!head[1] && bl != 0) || (!head[2] && al != 0) ||
            bl > P0_PATCH_MAX_BYTES - total ||
            al > P0_PATCH_MAX_BYTES - total - (size_t)bl) {
            report_set(report, P0_PATCH_CORRUPT, "persist_op_header:index=%zu", i);
            p0_patch_free(a); return P0_PATCH_CORRUPT;
        }
        total += (size_t)bl + (size_t)al;
        bd[64] = ad[64] = '\0';
        op->kind = (P0PatchOpKind)head[0];
        op->has_before = head[1]; op->has_after = head[2];
        op->before_mode = (mode_t)bm; op->after_mode = (mode_t)am;
        op->before_len = (size_t)bl; op->after_len = (size_t)al;
        op->path = malloc((size_t)p1 + 1);
        op->path2 = p2 ? malloc((size_t)p2 + 1) : NULL;
        if (op->has_before) op->before = malloc(op->before_len ? op->before_len : 1);
        if (op->has_after) op->after = malloc(op->after_len ? op->after_len : 1);
        if (!op->path || (p2 && !op->path2) ||
            (op->has_before && !op->before) || (op->has_after && !op->after)) {
            report_set(report, P0_PATCH_IO_ERROR, "alloc:persist_op");
            p0_patch_free(a); return P0_PATCH_IO_ERROR;
        }
        if (!get_bytes(f, op->path, p1) || !get_bytes(f, op->path2, p2) ||
            !get_bytes(f, op->before, op->has_before ? op->before_len : 0) ||
            !get_bytes(f, op->after, op->has_after ? op->after_len : 0)) {
            report_set(report, P0_PATCH_CORRUPT, "persist_truncated:index=%zu", i);
            p0_patch_free(a); return P0_PATCH_CORRUPT;
        }
        op->path[p1] = '\0';
        if (op->path2) op->path2[p2] = '\0';
        if (!path_safe_syntax(op->path) || (op->path2 && !path_safe_syntax(op->path2)) ||
            (op->kind == P0_PATCH_RENAME) != (op->path2 != NULL) ||
            (op->kind != P0_PATCH_RENAME && op->path2) ||
            ((op->kind == P0_PATCH_EDIT || op->kind == P0_PATCH_RENAME) &&
             (!op->has_before || !op->has_after)) ||
            (op->kind == P0_PATCH_CREATE && (op->has_before || !op->has_after)) ||
            (op->kind == P0_PATCH_DELETE && (!op->has_before || op->has_after))) {
            report_set(report, P0_PATCH_CORRUPT, "persist_op_semantics:index=%zu", i);
            p0_patch_free(a); return P0_PATCH_CORRUPT;
        }
        if (op->has_before) digest_hex(op->before, op->before_len, op->before_digest);
        if (op->has_after) digest_hex(op->after, op->after_len, op->after_digest);
        if (strcmp(op->has_before ? op->before_digest : "", bd) != 0 ||
            strcmp(op->has_after ? op->after_digest : "", ad) != 0) {
            report_set(report, P0_PATCH_CORRUPT, "persist_image_digest:index=%zu", i);
            p0_patch_free(a); return P0_PATCH_CORRUPT;
        }
    }
    if (paths_overlap(a)) {
        report_set(report, P0_PATCH_CORRUPT, "persist_path_overlap");
        p0_patch_free(a); return P0_PATCH_CORRUPT;
    }
    artifact_hash(a);
    if (strcmp(a->base_digest, expected_base) != 0 || strcmp(a->digest, expected_id) != 0) {
        report_set(report, P0_PATCH_CORRUPT, "persist_artifact_digest");
        p0_patch_free(a); return P0_PATCH_CORRUPT;
    }
    int tail = fgetc(f);
    if (tail != EOF) {
        report_set(report, P0_PATCH_CORRUPT, "persist_trailing_data");
        p0_patch_free(a); return P0_PATCH_CORRUPT;
    }
    *out = a;
    report_set(report, P0_PATCH_OK, "loaded:artifact=%s", a->digest);
    return P0_PATCH_OK;
}

/* --------------------------------------------------------------- transaction */

static P0PatchResult rollback_all(const P0PatchArtifact *a, int rootfd,
                                  P0PatchReport *report) {
    P0PatchReport scratch;
    /* First remove paths that did not exist in the pre-state. */
    for (size_t i = 0; i < a->count; i++) {
        const P0PatchOp *op = &a->ops[i];
        if (op->kind == P0_PATCH_CREATE) {
            P0PatchResult r = unlink_regular(rootfd, op->path, 1, &scratch);
            if (r != P0_PATCH_OK) {
                report_set(report, P0_PATCH_ROLLBACK_FAILED, "%s", scratch.detail);
                return P0_PATCH_ROLLBACK_FAILED;
            }
        } else if (op->kind == P0_PATCH_RENAME) {
            P0PatchResult r = unlink_regular(rootfd, op->path2, 1, &scratch);
            if (r != P0_PATCH_OK) {
                report_set(report, P0_PATCH_ROLLBACK_FAILED, "%s", scratch.detail);
                return P0_PATCH_ROLLBACK_FAILED;
            }
        }
    }
    /* Then reconstruct every complete preimage with its original mode. */
    for (size_t i = 0; i < a->count; i++) {
        const P0PatchOp *op = &a->ops[i];
        if (op->has_before) {
            P0PatchResult r = write_atomic_file(rootfd, op->path, op->before,
                                                op->before_len, op->before_mode,
                                                0, 0, &scratch);
            if (r != P0_PATCH_OK) {
                report_set(report, P0_PATCH_ROLLBACK_FAILED, "%s", scratch.detail);
                return P0_PATCH_ROLLBACK_FAILED;
            }
        }
    }
    P0PatchResult r = verify_all(a, rootfd, 0, &scratch);
    if (r != P0_PATCH_OK) {
        report_set(report, P0_PATCH_ROLLBACK_FAILED,
                   "rollback_verify:%s", scratch.detail);
        return P0_PATCH_ROLLBACK_FAILED;
    }
    return P0_PATCH_OK;
}

P0PatchResult p0_patch_commit(const P0PatchArtifact *a,
                              P0PatchAuthorizeFn authorize, void *authorize_ctx,
                              P0PatchCheckFn post_check, void *check_ctx,
                              P0PatchReport *report) {
    report_init(report);
    if (!a || a->count == 0) {
        report_set(report, P0_PATCH_INVALID, "artifact_invalid");
        return P0_PATCH_INVALID;
    }
    if (a->detached) {
        report_set(report, P0_PATCH_POLICY_DENIED,
                   "detached_no_canonical_preimage");
        return P0_PATCH_POLICY_DENIED;
    }
    char why[P0_PATCH_DETAIL] = "";
    if (!authorize || !authorize(a, authorize_ctx, why, sizeof why)) {
        report_set(report, P0_PATCH_POLICY_DENIED, "%s",
                   *why ? why : "canonical_write_denied");
        return P0_PATCH_POLICY_DENIED;
    }
    int rootfd = open_root_fd(p0_root());
    if (rootfd < 0) {
        report_set(report, P0_PATCH_IO_ERROR, "workspace_root_open");
        return P0_PATCH_IO_ERROR;
    }
    P0PatchResult r = verify_all(a, rootfd, 0, report);
    if (r != P0_PATCH_OK) {
        char detail[P0_PATCH_DETAIL];
        snprintf(detail, sizeof detail, "%s", report ? report->detail : "preimage conflict");
        report_set(report, P0_PATCH_CONFLICT, "%s", detail);
        close(rootfd);
        return P0_PATCH_CONFLICT;       /* all paths checked before first write */
    }

    P0PatchResult cause = P0_PATCH_APPLY_FAILED;
    r = apply_all(a, rootfd, 0, report);
    if (r == P0_PATCH_OK) r = verify_all(a, rootfd, 1, report);
    if (r == P0_PATCH_OK && post_check) {
        why[0] = '\0';
        if (!post_check(p0_root(), check_ctx, why, sizeof why)) {
            report_set(report, P0_PATCH_POST_CHECK_FAILED, "%s",
                       *why ? why : "post_check_red");
            r = P0_PATCH_POST_CHECK_FAILED;
            cause = P0_PATCH_POST_CHECK_FAILED;
        }
    }
    if (r == P0_PATCH_OK) {
        r = verify_all(a, rootfd, 1, report);
        if (r != P0_PATCH_OK) cause = P0_PATCH_POST_CHECK_FAILED;
    }
    if (r == P0_PATCH_OK) {
        close(rootfd);
        report_set(report, P0_PATCH_OK, "committed:artifact=%s", a->digest);
        return P0_PATCH_OK;
    }

    char failed_detail[P0_PATCH_DETAIL];
    snprintf(failed_detail, sizeof failed_detail, "%s",
             report && *report->detail ? report->detail : p0_patch_result_name(r));
    if (report) report->rollback_attempted = 1;
    P0PatchReport rb;
    report_init(&rb);
    P0PatchResult rr = rollback_all(a, rootfd, &rb);
    close(rootfd);
    if (rr != P0_PATCH_OK) {
        report_set(report, P0_PATCH_ROLLBACK_FAILED, "%s:rollback_failed:%s",
                   failed_detail, rb.detail);
        if (report) { report->rollback_attempted = 1; report->rollback_ok = 0; }
        return P0_PATCH_ROLLBACK_FAILED;
    }
    report_set(report, cause, "%s:rollback_verified=1", failed_detail);
    if (report) { report->rollback_attempted = 1; report->rollback_ok = 1; }
    return cause;
}
