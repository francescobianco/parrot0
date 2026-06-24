#!/usr/bin/env bash
#
# gen209 (docs/plans/learn-and-build.md Track B/B0): the run-grounded JUDGE for array
# code. code_check_sort() wraps a candidate sort in a generated main, runs it on fixed
# vectors, and verifies each result is sorted AND a permutation of the input. This test
# feeds it a KNOWN-GOOD sort and several KNOWN-BAD functions and asserts the verdicts —
# proving the oracle catches "does nothing" (unsorted) and "zeroes everything" (sorted
# but not a permutation). No synthesis here: only the judge is exercised.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-check-sort.$$"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP"

cat >"$TMP/driver.c" <<'EOC'
#include "code.h"
#include <stdio.h>

int main(void) {
    char err[512];

    /* known-good: textbook bubble sort, ascending */
    const char *good =
        "void mysort(int a[], int n){"
        " for(int i=0;i<n;i++) for(int j=0;j+1<n-i;j++)"
        "  if(a[j]>a[j+1]){int t=a[j];a[j]=a[j+1];a[j+1]=t;} }";

    /* known-bad #1: does nothing -> input stays unsorted */
    const char *noop = "void mysort(int a[], int n){ (void)a; (void)n; }";

    /* known-bad #2: zeroes everything -> sorted (all equal) but NOT a permutation */
    const char *zero = "void mysort(int a[], int n){ for(int i=0;i<n;i++) a[i]=0; }";

    /* known-bad #3: descending sort -> a permutation but NOT ascending */
    const char *desc =
        "void mysort(int a[], int n){"
        " for(int i=0;i<n;i++) for(int j=0;j+1<n-i;j++)"
        "  if(a[j]<a[j+1]){int t=a[j];a[j]=a[j+1];a[j+1]=t;} }";

    int rg = code_check_sort(good, "mysort", err, sizeof err);
    int rn = code_check_sort(noop, "mysort", err, sizeof err);
    int rz = code_check_sort(zero, "mysort", err, sizeof err);
    int rd = code_check_sort(desc, "mysort", err, sizeof err);

    /* a build failure must be reported as -1, not a false pass */
    const char *broken = "void mysort(int a[], int n){ this is not c }";
    int rb = code_check_sort(broken, "mysort", err, sizeof err);

    printf("good=%d noop=%d zero=%d desc=%d broken=%d\n", rg, rn, rz, rd, rb);
    if (rg == 1 && rn == 0 && rz == 0 && rd == 0 && rb == -1) {
        printf("PASS check_sort: judge accepts a real sort, rejects noop/zero/desc, flags a build error\n");
        return 0;
    }
    printf("FAIL check_sort\n");
    return 1;
}
EOC

# Compile the driver against the REAL code.c (+ kb.c for its KB symbols).
if ! cc -std=c11 -O2 -I"$ROOT/src" -o "$TMP/driver" \
        "$TMP/driver.c" "$ROOT/src/code.c" "$ROOT/src/kb.c" 2>"$TMP/cc.err"; then
    echo "FAIL check_sort: could not build the test driver" >&2
    cat "$TMP/cc.err" >&2
    exit 1
fi

# Run from a writable scratch dir (code_check_sort writes a sandboxed temp .c there).
cd "$TMP" && exec "$TMP/driver"
