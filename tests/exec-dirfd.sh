#!/bin/sh
# Focal ratchet for the staged-tree executor capability.
set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/p0-exec-dirfd.XXXXXX") || exit 1
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

CAND="$TMP/candidate"
TRACE="$TMP/trace.jsonl"
RESULTS="$TMP/results"
PROBE="$TMP/exec-dirfd-probe"
MARKER=".p0-exec-dirfd-$$"
mkdir -p "$CAND/nested/deeper"
printf 'not a directory\n' > "$CAND/plain-root"
ln -s nested "$CAND/linkdir"

pass=0
fail=0
ok() { echo "PASS exec-dirfd: $1"; pass=$((pass + 1)); }
no() { echo "FAIL exec-dirfd: $1" >&2; fail=$((fail + 1)); }

cat > "$TMP/probe.c" <<'EOF'
#define _POSIX_C_SOURCE 200809L
#include "exec.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static int child_mode(int argc, char **argv) {
    if (argc < 2 || strncmp(argv[1], "--child-", 8) != 0) return -1;
    if (strcmp(argv[1], "--child-pwd") == 0) {
        char cwd[PATH_MAX];
        if (!getcwd(cwd, sizeof cwd)) return 20;
        puts(cwd);
        return 0;
    }
    if (strcmp(argv[1], "--child-write") == 0) {
        if (argc < 3) return 21;
        int fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd < 0) return 22;
        if (write(fd, "candidate\n", 10) != 10) { close(fd); return 23; }
        close(fd);
        return 0;
    }
    if (strcmp(argv[1], "--child-output") == 0) {
        fputs("candidate-output\n", stdout);
        return 0;
    }
    if (strcmp(argv[1], "--child-fail") == 0) return 9;
    if (strcmp(argv[1], "--child-sleep") == 0) {
        struct timespec left = {3, 0};
        while (nanosleep(&left, &left) != 0 && errno == EINTR) { }
        return 0;
    }
    return 24;
}

static void flag(const char *name, int value) {
    printf("%s=%d\n", name, value ? 1 : 0);
}

int main(int argc, char **argv) {
    int child = child_mode(argc, argv);
    if (child >= 0) return child;
    if (argc != 3) return 64;

    const char *candidate = argv[1];
    const char *marker = argv[2];
    int rootfd = open(candidate, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (rootfd < 0) return 65;
    char regular_path[PATH_MAX];
    snprintf(regular_path, sizeof regular_path, "%s/plain-root", candidate);
    int regularfd = open(regular_path, O_RDONLY | O_CLOEXEC);
    if (regularfd < 0) { close(rootfd); return 66; }

    P0Obs o;
    char expect[PATH_MAX];
    char *pwd_argv[] = {argv[0], (char *)"--child-pwd", NULL};
    p0_exec_at(rootfd, pwd_argv, "nested/deeper", 2000, NULL, &o);
    snprintf(expect, sizeof expect, "%s/nested/deeper\n", candidate);
    flag("nested", p0_obs_ok(&o) && strcmp(o.out, expect) == 0 &&
                   strcmp(o.cwd, "nested/deeper") == 0);

    char *write_argv[] = {argv[0], (char *)"--child-write", (char *)marker, NULL};
    p0_exec_at(rootfd, write_argv, ".", 2000, NULL, &o);
    flag("candidate_write", p0_obs_ok(&o) &&
                            faccessat(rootfd, marker, F_OK, 0) == 0);

    char *output_argv[] = {argv[0], (char *)"--child-output", NULL};
    p0_exec_at(rootfd, output_argv, ".", 2000, NULL, &o);
    flag("stdout_verdict", p0_obs_ok(&o) &&
                           strcmp(o.out, "candidate-output\n") == 0 &&
                           o.exit_code == 0);

    char *fail_argv[] = {argv[0], (char *)"--child-fail", NULL};
    p0_exec_at(rootfd, fail_argv, ".", 2000, NULL, &o);
    flag("nonzero_verdict", o.verdict == P0_EXIT_NONZERO && o.exit_code == 9 &&
                            !p0_obs_ok(&o));

    p0_exec_at(rootfd, pwd_argv, "nested/../deeper", 2000, NULL, &o);
    flag("dotdot_rejected", o.verdict == P0_UNSAFE_PATH);
    p0_exec_at(rootfd, pwd_argv, "/tmp", 2000, NULL, &o);
    flag("absolute_rejected", o.verdict == P0_UNSAFE_PATH);
    p0_exec_at(rootfd, pwd_argv, "linkdir", 2000, NULL, &o);
    flag("symlink_rejected", o.verdict == P0_UNSAFE_PATH);
    p0_exec_at(regularfd, pwd_argv, ".", 2000, NULL, &o);
    flag("nondir_rejected", o.verdict == P0_UNSAFE_PATH);

    char *sleep_argv[] = {argv[0], (char *)"--child-sleep", NULL};
    p0_exec_at(rootfd, sleep_argv, ".", 150, NULL, &o);
    flag("timeout", o.verdict == P0_TIMEOUT && o.duration_ms < 2000);

    struct stat st;
    flag("caller_owns_rootfd", fstat(rootfd, &st) == 0 && S_ISDIR(st.st_mode));

    /* Legacy entry point must still use the startup workspace and the same core. */
    p0_exec(output_argv, ".", 2000, NULL, &o);
    flag("legacy", p0_obs_ok(&o) && strcmp(o.out, "candidate-output\n") == 0);

    close(regularfd);
    close(rootfd);
    return 0;
}
EOF

if ! cc -std=c11 -Wall -Wextra -Wpedantic -O0 -I"$ROOT/src" \
        -o "$PROBE" "$TMP/probe.c" "$ROOT/src/exec.c"; then
    no "probe builds"
    echo "exec-dirfd: $pass passed, $fail failed"
    exit 1
fi

if (cd "$ROOT" && PARROT0_TRACE="$TRACE" "$PROBE" "$CAND" "$MARKER") \
        > "$RESULTS"; then
    ok "probe runs"
else
    no "probe runs"
fi

check_flag() {
    key=$1
    description=$2
    if grep -qx "$key=1" "$RESULTS"; then ok "$description"; else no "$description"; fi
}

check_flag nested "nested cwd executes in the candidate root"
check_flag candidate_write "relative writes land in the candidate tree"
check_flag stdout_verdict "stdout and success verdict are observed"
check_flag nonzero_verdict "a nonzero exit remains a typed failure"
check_flag dotdot_rejected "a cwd containing .. is rejected"
check_flag absolute_rejected "an absolute cwd is rejected"
check_flag symlink_rejected "a symlink cwd is rejected"
check_flag nondir_rejected "a non-directory root descriptor is rejected"
check_flag timeout "the dirfd path preserves the wall-clock timeout"
check_flag caller_owns_rootfd "p0_exec_at does not consume the caller's rootfd"
check_flag legacy "p0_exec still uses the same execution core"

if [ -e "$ROOT/$MARKER" ]; then
    no "candidate execution wrote into the canonical workspace"
else
    ok "candidate execution leaves the canonical workspace untouched"
fi

if [ -f "$TRACE" ] && grep -q '"verdict":"ok"' "$TRACE" &&
        grep -q '"verdict":"timeout"' "$TRACE" &&
        grep -q '"cwd":"nested/deeper"' "$TRACE"; then
    ok "success, timeout and candidate cwd reach the observation trace"
else
    no "the dirfd executions were not faithfully traced"
fi

echo "exec-dirfd: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
