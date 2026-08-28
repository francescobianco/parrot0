#!/usr/bin/env bash
#
# exec_kernel.sh — il kernel di esecuzione provato DIRETTAMENTE, non da un
# prompt. Estratto da tests/toolexec.sh (gen444), che per il resto e' diventato
# tests/p0t/tools/toolexec.p0t.
#
# Resta uno script per una ragione precisa: compila un driver contro src/exec.c
# e chiama `p0_exec` con argv arbitrari. Portarlo in .p0t vorrebbe dire esporre
# l'esecutore sul layer MCP, e questo darebbe a QUALUNQUE client il permesso di
# eseguire argv arbitrari — una decisione di sicurezza, non una conversione.
#
# Prova le quattro cose che nessun prompt dovrebbe poter chiedere:
#   un ciclo infinito torna come timeout TIPIZZATO, in tempo limitato;
#   il timeout uccide il GRUPPO di processi, senza orfani;
#   un programma inesistente e' spawn_failed, non un successo vuoto;
#   una cwd fuori dal workspace e' unsafe_path.
set -u

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 1
pass=0
fail=0
ok() { echo "PASS exec_kernel: $1"; pass=$((pass+1)); }
no() { echo "FAIL exec_kernel: $1" >&2; fail=$((fail+1)); }

# ---- 06: the sandbox actually bounds a runaway --------------------------------
# Exercised directly against the executor, because no prompt should be able to
# ask for this. An infinite loop must come back as a TYPED timeout, in bounded
# time, with no orphan left behind.
cat > /tmp/p0_exec_probe.c <<'EOF'
#include "P0_ROOT/src/exec.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    (void)argc;
    P0Obs o;
    if (strcmp(argv[1], "spin") == 0) {
        char *a[] = {(char*)"sh", (char*)"-c", (char*)"while :; do :; done", NULL};
        p0_exec(a, ".", 1500, NULL, &o);
    } else if (strcmp(argv[1], "tree") == 0) {
        /* a child that spawns children: the whole GROUP must die, not just the head */
        char *a[] = {(char*)"sh", (char*)"-c",
                     (char*)"sleep 60 & sleep 60 & while :; do :; done", NULL};
        p0_exec(a, ".", 1500, NULL, &o);
    } else if (strcmp(argv[1], "nosuch") == 0) {
        char *a[] = {(char*)"definitely-not-a-program-xyz", NULL};
        p0_exec(a, ".", 2000, NULL, &o);
    } else if (strcmp(argv[1], "escape") == 0) {
        char *a[] = {(char*)"ls", NULL};
        p0_exec(a, "../../..", 2000, NULL, &o);
    }
    printf("%s|%ld\n", p0_verdict_name(o.verdict), o.duration_ms);
    return 0;
}
EOF
sed -i "s|P0_ROOT|$ROOT|" /tmp/p0_exec_probe.c
if cc -O0 -o /tmp/p0_exec_probe /tmp/p0_exec_probe.c "$ROOT/src/exec.c" 2>/dev/null; then
    t0=$(date +%s)
    res="$(/tmp/p0_exec_probe spin)"
    t1=$(date +%s)
    case "$res" in
      timeout*) ok "an infinite loop terminates as verdict=timeout (not a hang)" ;;
      *)        no "an infinite loop did not time out: $res" ;;
    esac
    [ $((t1 - t0)) -le 5 ] \
        && ok "the timeout is enforced in bounded wall-clock time" \
        || no "the timeout took $((t1-t0))s to fire"

    /tmp/p0_exec_probe tree >/dev/null
    sleep 0.2
    if pgrep -f "sleep 60" >/dev/null 2>&1; then
        pkill -f "sleep 60"
        no "a timed-out command left ORPHAN processes behind"
    else
        ok "the timeout kills the whole process GROUP — no orphans"
    fi

    res="$(/tmp/p0_exec_probe nosuch)"
    case "$res" in
      spawn_failed*) ok "a tool that does not exist is spawn_failed, not empty success" ;;
      *)             no "a missing tool was not reported as spawn_failed: $res" ;;
    esac

    res="$(/tmp/p0_exec_probe escape)"
    case "$res" in
      unsafe_path*) ok "a cwd outside the workspace is unsafe_path" ;;
      *)            no "a command ran with a cwd outside the workspace: $res" ;;
    esac
    rm -f /tmp/p0_exec_probe /tmp/p0_exec_probe.c
else
    no "could not build the executor probe"
fi

echo "exec_kernel: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
[ "$fail" -eq 0 ]