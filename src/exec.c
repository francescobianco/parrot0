/*
 * exec.c - the tool-execution kernel (gen329, TODO.md P0/04-05-06-07).
 *
 * See exec.h for the four counterexamples that forced this file. The shape of the
 * fix, in one sentence: parrot0 stops SPEAKING to the operating system in a
 * language (a shell string, a path spelled as text) and starts HANDING it typed
 * objects (an argv vector, a file descriptor it resolved itself) — and gets back a
 * typed object (an Observation) instead of a hopeful paragraph.
 */
#define _GNU_SOURCE
#include "exec.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const char *p0_verdict_name(P0Verdict v) {
    switch (v) {
        case P0_OK:             return "ok";
        case P0_EXIT_NONZERO:   return "exit_nonzero";
        case P0_SIGNALED:       return "signaled";
        case P0_TIMEOUT:        return "timeout";
        case P0_SPAWN_FAILED:   return "spawn_failed";
        case P0_UNSAFE_PATH:    return "unsafe_path";
        case P0_UNSAFE_COMMAND: return "unsafe_command";
        case P0_TOO_LARGE:      return "too_large";
    }
    return "unknown";
}

/* The single gate. A caller cannot conclude "it worked" from anything else. */
int p0_obs_ok(const P0Obs *o) {
    return o && o->verdict == P0_OK && o->exit_code == 0;
}

/* ---------------------------------------------------------------- containment */

static char  g_root[PATH_MAX];
static int   g_root_fd = -1;

/* The workspace root is the cwd at startup. parrot0 is an agent that works ON a
 * repository; everything it may read is under that directory, and the fact that
 * the machine has an /etc is none of its business. */
void p0_root_init(void) {
    if (g_root_fd >= 0) return;
    if (!getcwd(g_root, sizeof g_root)) snprintf(g_root, sizeof g_root, ".");
    g_root_fd = open(g_root, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
}

const char *p0_root(void) { p0_root_init(); return g_root; }

/* Walk `path` component by component from the root dirfd.
 *
 * Why component-wise and not realpath()+strncmp: realpath resolves symlinks and
 * hands back a NAME, and between the moment you approve that name and the moment
 * you open it, the name can be made to mean a different file (the rename race in
 * TODO 05). Here every step is an openat(O_NOFOLLOW) on a descriptor we already
 * hold, so the path cannot be swapped out from under the check, and a symlink
 * ANYWHERE along it — not just at the leaf — fails with ELOOP. The fd we return
 * IS the thing we validated.
 *
 * `want_dir` picks the type of the final component. `rel_out` (optional) receives
 * the cleaned relative path, so a caller building argv passes on the form it
 * actually resolved rather than the user's spelling. */
static P0Verdict walk_in_root(const char *path, int want_dir,
                              int *fd_out, char *rel_out, size_t rel_cap) {
    p0_root_init();
    if (g_root_fd < 0 || !path || !*path) return P0_UNSAFE_PATH;
    if (path[0] == '/') return P0_UNSAFE_PATH;         /* absolute: not ours     */
    if (strlen(path) >= PATH_MAX) return P0_UNSAFE_PATH;

    char work[PATH_MAX];
    snprintf(work, sizeof work, "%s", path);

    char rel[PATH_MAX]; size_t rl = 0; rel[0] = '\0';
    int dirfd = dup(g_root_fd);                        /* never close g_root_fd  */
    if (dirfd < 0) return P0_UNSAFE_PATH;

    char *save = NULL;
    char *comp = strtok_r(work, "/", &save);
    P0Verdict v = P0_OK;
    int fd = -1;

    while (comp) {
        char *next = strtok_r(NULL, "/", &save);
        if (strcmp(comp, ".") == 0) { comp = next; continue; }
        if (strcmp(comp, "..") == 0) { v = P0_UNSAFE_PATH; break; }  /* no escape */

        int last = (next == NULL);
        int flags = O_RDONLY | O_NOFOLLOW | O_CLOEXEC;
        if (!last) flags |= O_DIRECTORY;
        else if (want_dir) flags |= O_DIRECTORY;

        int nfd = openat(dirfd, comp, flags);
        if (nfd < 0) { v = P0_UNSAFE_PATH; break; }    /* ELOOP == symlink, too  */

        if (rl + strlen(comp) + 2 < rel_cap ? 1 : 0) {
            if (rl) rel[rl++] = '/';
            size_t cl = strlen(comp);
            memcpy(rel + rl, comp, cl); rl += cl; rel[rl] = '\0';
        }

        close(dirfd);
        dirfd = nfd;

        if (last) {
            struct stat st;
            if (fstat(nfd, &st) != 0) { v = P0_UNSAFE_PATH; break; }
            /* A FIFO would block the reader forever and a device is not the
             * workspace's to hand out; only real files and directories exist
             * as far as the agent is concerned. */
            if (want_dir ? !S_ISDIR(st.st_mode) : !S_ISREG(st.st_mode)) {
                v = P0_UNSAFE_PATH; break;
            }
            fd = nfd; dirfd = -1;                      /* ownership moves to fd  */
        }
        comp = next;
    }

    if (dirfd >= 0) {
        /* The path was "." (or all-dots): that IS the root. */
        if (v == P0_OK && fd < 0 && rl == 0 && want_dir) { fd = dirfd; }
        else close(dirfd);
    }
    if (v != P0_OK) { if (fd >= 0) close(fd); return v; }
    if (fd < 0) return P0_UNSAFE_PATH;

    if (rel_out && rel_cap) snprintf(rel_out, rel_cap, "%s", rl ? rel : ".");
    if (fd_out) *fd_out = fd; else close(fd);
    return P0_OK;
}

P0Verdict p0_open_in_root(const char *path, int want_dir, int *fd_out) {
    return walk_in_root(path, want_dir, fd_out, NULL, 0);
}

P0Verdict p0_safe_rel(const char *path, int want_dir, char *rel_out, size_t cap) {
    return walk_in_root(path, want_dir, NULL, rel_out, cap);
}

P0Verdict p0_read_in_root(const char *path, char *buf, size_t cap,
                          int *truncated) {
    if (truncated) *truncated = 0;
    if (!buf || cap == 0) return P0_UNSAFE_PATH;
    buf[0] = '\0';
    int fd = -1;
    P0Verdict v = walk_in_root(path, 0, &fd, NULL, 0);
    if (v != P0_OK) return v;
    size_t n = 0;
    while (n + 1 < cap) {
        ssize_t r = read(fd, buf + n, cap - 1 - n);
        if (r < 0) { if (errno == EINTR) continue; break; }
        if (r == 0) break;
        n += (size_t)r;
    }
    buf[n] = '\0';
    if (n + 1 >= cap) {                                 /* is there more?        */
        char probe;
        if (read(fd, &probe, 1) == 1 && truncated) *truncated = 1;
    }
    close(fd);
    return P0_OK;
}

/* ------------------------------------------------------------------ execution */

static void fnv1a_hex(const char *data, size_t n, char out[17]) {
    unsigned long long h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; i++) {
        h ^= (unsigned char)data[i];
        h *= 1099511628211ULL;
    }
    snprintf(out, 17, "%016llx", h);
}

static long ms_since(const struct timespec *t0) {
    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    return (t1.tv_sec - t0->tv_sec) * 1000L + (t1.tv_nsec - t0->tv_nsec) / 1000000L;
}

/* Whether an unprivileged network namespace is obtainable on THIS machine.
 * -1 unknown, 0 no, 1 yes. Learned from a real attempt, never assumed. */
static int g_netns = -1;

/* Drop the child off the network, if the kernel lets an unprivileged process do
 * it. Returns 1 on success, 0 if the namespace was never entered, and -1 for the
 * one case that must not be papered over: the namespace WAS entered but the uid
 * map could not be written.
 *
 * That third case is not hypothetical — it is what this machine does. Ubuntu's
 * apparmor_restrict_unprivileged_userns lets unshare(CLONE_NEWUSER) succeed and
 * then denies the write to /proc/self/uid_map, which strands the child in a
 * namespace where it is the overflow uid (65534): it can no longer read the
 * repository it was sent to work on, and `make`'s own /bin/sh comes back as
 * "Invalid argument". My first version treated that as "no isolation, carry on"
 * — and carried on INSIDE the broken namespace, which is exactly the kind of
 * claim-without-a-check this generation exists to abolish. A namespace is not
 * revertible, so the only honest move is to report the failure to the parent and
 * die; the parent retries once, with the network, and says so. */
static int child_drop_network(void) {
    uid_t uid = getuid(); gid_t gid = getgid();
    if (unshare(CLONE_NEWUSER | CLONE_NEWNET) != 0) return 0;   /* never entered */

    char buf[64]; int fd;
    fd = open("/proc/self/setgroups", O_WRONLY);
    if (fd >= 0) { ssize_t w = write(fd, "deny", 4); (void)w; close(fd); }

    fd = open("/proc/self/gid_map", O_WRONLY);
    if (fd < 0) return -1;
    int n = snprintf(buf, sizeof buf, "%u %u 1\n", (unsigned)gid, (unsigned)gid);
    if (write(fd, buf, (size_t)n) != n) { close(fd); return -1; }
    close(fd);

    fd = open("/proc/self/uid_map", O_WRONLY);
    if (fd < 0) return -1;
    n = snprintf(buf, sizeof buf, "%u %u 1\n", (unsigned)uid, (unsigned)uid);
    if (write(fd, buf, (size_t)n) != n) { close(fd); return -1; }
    close(fd);
    return 1;
}

/* The bounds of TODO 06. A candidate program parrot0 did not write gets a fixed
 * budget of everything: without these a fork bomb or a runaway allocation is not
 * a failed test, it is a lost machine.
 *
 * RLIMIT_NPROC deserves a warning, because I got it wrong first: it caps the
 * processes of the whole UID, not of this child. Setting it to 64 — a number that
 * sounds generous for one test — instantly denied forks to the user's ENTIRE
 * desktop session. So it is set to a coarse ceiling (2048) whose only job is to
 * keep a fork bomb from becoming unbounded, and it is NOT the real containment:
 * the real containment is the wall clock plus killpg(SIGKILL) on the child's own
 * process group, which reaps the whole tree whatever it spawned. */
static void child_limits(int timeout_ms) {
    struct rlimit rl;
    long cpu = timeout_ms / 1000 + 2;                   /* CPU <= wall + slack   */
    rl.rlim_cur = rl.rlim_max = (rlim_t)cpu;            setrlimit(RLIMIT_CPU, &rl);
    rl.rlim_cur = rl.rlim_max = 4UL * 1024 * 1024 * 1024; setrlimit(RLIMIT_AS, &rl);
    rl.rlim_cur = rl.rlim_max = 256UL * 1024 * 1024;    setrlimit(RLIMIT_FSIZE, &rl);
    rl.rlim_cur = rl.rlim_max = 1024;                   setrlimit(RLIMIT_NOFILE, &rl);
    rl.rlim_cur = rl.rlim_max = 2048;                   setrlimit(RLIMIT_NPROC, &rl);
    rl.rlim_cur = rl.rlim_max = 0;                      setrlimit(RLIMIT_CORE, &rl);
}

/* Append into a bounded sink, remembering that it overflowed. Truncation is a
 * FACT about the observation, not a detail to hide: an answer read off a
 * truncated capture must be able to say so. */
static void sink(char *buf, size_t cap, size_t *len, int *trunc,
                 const char *data, size_t n) {
    size_t room = (cap > *len + 1) ? cap - *len - 1 : 0;
    if (n > room) { n = room; *trunc = 1; }
    if (n) { memcpy(buf + *len, data, n); *len += n; }
    buf[*len] = '\0';
}

static P0Verdict exec_once(char *const argv[], const char *cwd, int timeout_ms,
                           const char *stdin_data, P0Obs *obs) {
    if (!obs) return P0_SPAWN_FAILED;
    memset(obs, 0, sizeof *obs);
    obs->exit_code = -1;
    snprintf(obs->cwd, sizeof obs->cwd, "%s", cwd ? cwd : ".");
    if (timeout_ms <= 0) timeout_ms = 10000;

    if (!argv || !argv[0]) {
        obs->verdict = P0_SPAWN_FAILED;
        snprintf(obs->detail, sizeof obs->detail, "empty argv");
        return obs->verdict;
    }
    /* Render argv for humans. Note what this rendering is NOT: it is not a command
     * that will be run. Nothing built from it is ever executed — it exists so a
     * proof can quote the act. That distinction is the whole of TODO 04. */
    size_t o = 0;
    for (int i = 0; argv[i] && o + 1 < sizeof obs->cmd; i++)
        o += (size_t)snprintf(obs->cmd + o, sizeof obs->cmd - o, "%s%s",
                              i ? " " : "", argv[i]);

    /* The cwd must itself be inside the root — an act cannot be sent to run
     * somewhere the agent is not allowed to look. */
    char rel[PATH_MAX] = ".";
    if (cwd && strcmp(cwd, ".") != 0) {
        if (p0_safe_rel(cwd, 1, rel, sizeof rel) != P0_OK) {
            obs->verdict = P0_UNSAFE_PATH;
            snprintf(obs->detail, sizeof obs->detail, "cwd escapes the workspace: %s", cwd);
            p0_obs_trace(obs);
            return obs->verdict;
        }
    }
    char abs_cwd[PATH_MAX * 2];
    snprintf(abs_cwd, sizeof abs_cwd, "%s/%s", p0_root(), rel);

    int op[2], ep[2], ip[2];
    if (pipe(op) != 0) { obs->verdict = P0_SPAWN_FAILED; return obs->verdict; }
    if (pipe(ep) != 0) { close(op[0]); close(op[1]);
                         obs->verdict = P0_SPAWN_FAILED; return obs->verdict; }
    if (pipe(ip) != 0) { close(op[0]); close(op[1]); close(ep[0]); close(ep[1]);
                         obs->verdict = P0_SPAWN_FAILED; return obs->verdict; }

    struct timespec t0; clock_gettime(CLOCK_MONOTONIC, &t0);

    /* A pipe the child uses to tell us the truth about its own isolation (and
     * about an exec that failed): one byte, written before exec, closed by
     * O_CLOEXEC if exec succeeds. */
    int sp[2];
    if (pipe(sp) != 0) { close(op[0]); close(op[1]); close(ep[0]); close(ep[1]);
                         close(ip[0]); close(ip[1]);
                         obs->verdict = P0_SPAWN_FAILED; return obs->verdict; }
    fcntl(sp[1], F_SETFD, FD_CLOEXEC);

    pid_t pid = fork();
    if (pid < 0) {
        close(op[0]); close(op[1]); close(ep[0]); close(ep[1]);
        close(ip[0]); close(ip[1]); close(sp[0]); close(sp[1]);
        obs->verdict = P0_SPAWN_FAILED;
        snprintf(obs->detail, sizeof obs->detail, "fork: %s", strerror(errno));
        p0_obs_trace(obs);
        return obs->verdict;
    }

    if (pid == 0) {                                     /* ---- child ---------- */
        setpgid(0, 0);                  /* its own group: a timeout kills the TREE */
        int netiso = (g_netns == 0) ? 0 : child_drop_network();
        char note = (netiso == 1) ? 'N' : (netiso == 0) ? 'n' : 'F';
        ssize_t w = write(sp[1], &note, 1); (void)w;
        /* 'F': stranded in a namespace we could not map. Unfixable from in here —
         * hand the fact to the parent and die rather than exec as the wrong user. */
        if (netiso < 0) _exit(126);

        if (chdir(abs_cwd) != 0) _exit(127);
        dup2(ip[0], STDIN_FILENO);
        dup2(op[1], STDOUT_FILENO);
        dup2(ep[1], STDERR_FILENO);
        close(op[0]); close(op[1]); close(ep[0]); close(ep[1]);
        close(ip[0]); close(ip[1]); close(sp[0]);
        child_limits(timeout_ms);

        /* A scrubbed environment. LANG=C is not cosmetic: at gen328 a failing
         * `make` reported itself in Italian, which no verdict parser could read.
         * An observation must be in a language the observer knows. */
        const char *home = getenv("HOME");
        char envhome[512]; snprintf(envhome, sizeof envhome, "HOME=%s", home ? home : "/tmp");
        char *envp[] = {
            (char *)"PATH=/usr/local/bin:/usr/bin:/bin",
            envhome,
            (char *)"LANG=C", (char *)"LC_ALL=C",
            (char *)"PARROT0_SANDBOX=1",
            NULL
        };
        execvpe(argv[0], argv, envp);
        _exit(127);                                     /* exec failed           */
    }

    /* ---- parent ------------------------------------------------------------ */
    setpgid(pid, pid);                                  /* race-free with child  */
    close(op[1]); close(ep[1]); close(ip[0]); close(sp[1]);

    if (stdin_data && *stdin_data) {
        size_t n = strlen(stdin_data), off = 0;
        while (off < n) {
            ssize_t w = write(ip[1], stdin_data + off, n - off);
            if (w <= 0) break;
            off += (size_t)w;
        }
    }
    close(ip[1]);                                       /* EOF for the child     */

    char note = 0;
    if (read(sp[0], &note, 1) == 1) obs->net_isolated = (note == 'N');
    close(sp[0]);

    if (note == 'F') {
        /* The child was stranded in an unmappable user namespace and exited. Learn
         * it once, for the life of the process, and let the caller retry — WITH the
         * network, and saying so (net_isolated stays 0). */
        g_netns = 0;
        int st; while (waitpid(pid, &st, 0) < 0 && errno == EINTR) { }
        close(op[0]); close(ep[0]);
        obs->verdict = P0_SPAWN_FAILED;
        snprintf(obs->detail, sizeof obs->detail, "userns_stranded");
        return obs->verdict;
    }
    if (note == 'N') g_netns = 1;

    struct pollfd pfd[2] = { {op[0], POLLIN, 0}, {ep[0], POLLIN, 0} };
    int open_fds = 2;
    int timed_out = 0;

    while (open_fds > 0) {
        long elapsed = ms_since(&t0);
        long left = timeout_ms - elapsed;
        if (left <= 0) { timed_out = 1; break; }

        int n = poll(pfd, 2, (int)left);
        if (n < 0) { if (errno == EINTR) continue; break; }
        if (n == 0) { timed_out = 1; break; }

        for (int i = 0; i < 2; i++) {
            if (pfd[i].fd < 0 || !pfd[i].revents) continue;
            char chunk[4096];
            ssize_t r = read(pfd[i].fd, chunk, sizeof chunk);
            if (r > 0) {
                if (i == 0) sink(obs->out, sizeof obs->out, &obs->out_len,
                                 &obs->out_truncated, chunk, (size_t)r);
                else        sink(obs->err, sizeof obs->err, &obs->err_len,
                                 &obs->err_truncated, chunk, (size_t)r);
            } else if (r == 0 || (r < 0 && errno != EINTR && errno != EAGAIN)) {
                close(pfd[i].fd); pfd[i].fd = -1; open_fds--;
            }
        }
    }

    int status = 0;
    if (timed_out) {
        /* The whole process group, not just the child: a build that spawned a
         * compiler must not outlive the act that started it. */
        kill(-pid, SIGKILL);
        kill(pid, SIGKILL);
    }
    while (waitpid(pid, &status, 0) < 0 && errno == EINTR) { }
    for (int i = 0; i < 2; i++) if (pfd[i].fd >= 0) close(pfd[i].fd);

    obs->duration_ms = ms_since(&t0);
    fnv1a_hex(obs->out, obs->out_len, obs->digest);

    if (timed_out) {
        obs->verdict = P0_TIMEOUT;
        snprintf(obs->detail, sizeof obs->detail,
                 "killed after %d ms (process group SIGKILL)", timeout_ms);
    } else if (WIFSIGNALED(status)) {
        obs->verdict = P0_SIGNALED;
        obs->term_signal = WTERMSIG(status);
        snprintf(obs->detail, sizeof obs->detail, "terminated by signal %d",
                 obs->term_signal);
    } else if (WIFEXITED(status)) {
        obs->exit_code = WEXITSTATUS(status);
        if (obs->exit_code == 127 && obs->out_len == 0 && obs->err_len == 0) {
            obs->verdict = P0_SPAWN_FAILED;
            snprintf(obs->detail, sizeof obs->detail, "could not execute `%s`", argv[0]);
        } else {
            obs->verdict = obs->exit_code == 0 ? P0_OK : P0_EXIT_NONZERO;
        }
    } else {
        obs->verdict = P0_SPAWN_FAILED;
    }

    p0_obs_trace(obs);
    return obs->verdict;
}

/* One retry, and only for one reason: the first attempt discovered that this
 * kernel hands out a user namespace it will not let us map (see
 * child_drop_network). That discovery is cached in g_netns, so it costs one
 * wasted fork per PROCESS, not per act. Any other failure is the act's real
 * verdict and is returned as it stands — a retry loop over genuine failures is
 * how an agent talks itself into a success it never had. */
P0Verdict p0_exec(char *const argv[], const char *cwd, int timeout_ms,
                  const char *stdin_data, P0Obs *obs) {
    P0Verdict v = exec_once(argv, cwd, timeout_ms, stdin_data, obs);
    if (v == P0_SPAWN_FAILED && obs && strcmp(obs->detail, "userns_stranded") == 0)
        v = exec_once(argv, cwd, timeout_ms, stdin_data, obs);
    return v;
}

/* ---------------------------------------------------------------- the record */

static void json_esc(const char *s, size_t n, FILE *f) {
    for (size_t i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
            case '"':  fputs("\\\"", f); break;
            case '\\': fputs("\\\\", f); break;
            case '\n': fputs("\\n", f);  break;
            case '\r': fputs("\\r", f);  break;
            case '\t': fputs("\\t", f);  break;
            default:
                if (c < 0x20) fprintf(f, "\\u%04x", c);
                else fputc((int)c, f);
        }
    }
}

void p0_obs_trace(const P0Obs *o) {
    const char *path = getenv("PARROT0_TRACE");
    if (!path || !*path || !o) return;
    FILE *f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "{\"kind\":\"observation\",\"verdict\":\"%s\",\"cmd\":\"",
            p0_verdict_name(o->verdict));
    json_esc(o->cmd, strlen(o->cmd), f);
    fprintf(f, "\",\"cwd\":\"");
    json_esc(o->cwd, strlen(o->cwd), f);
    fprintf(f, "\",\"exit\":%d,\"signal\":%d,\"ms\":%ld,\"net_isolated\":%d,"
               "\"out_digest\":\"%s\",\"out_bytes\":%zu,\"out_truncated\":%d,"
               "\"err_bytes\":%zu,\"stderr\":\"",
            o->exit_code, o->term_signal, o->duration_ms, o->net_isolated,
            o->digest, o->out_len, o->out_truncated, o->err_len);
    json_esc(o->err, o->err_len < 400 ? o->err_len : 400, f);
    fprintf(f, "\",\"detail\":\"");
    json_esc(o->detail, strlen(o->detail), f);
    fprintf(f, "\"}\n");
    fclose(f);
}

void p0_obs_proof(const P0Obs *o, char *buf, size_t cap) {
    if (!buf || !cap) return;
    if (!o) { snprintf(buf, cap, "no observation"); return; }
    if (o->verdict == P0_OK || o->verdict == P0_EXIT_NONZERO)
        snprintf(buf, cap,
                 "Observation: `%s` in %s -> %s (exit %d, %ld ms, %zu bytes on stdout, digest %s)",
                 o->cmd, o->cwd, p0_verdict_name(o->verdict), o->exit_code,
                 o->duration_ms, o->out_len, o->digest);
    else
        snprintf(buf, cap, "Observation: `%s` in %s -> %s (%s, %ld ms)",
                 o->cmd, o->cwd, p0_verdict_name(o->verdict), o->detail,
                 o->duration_ms);
}