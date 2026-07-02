/* gen257 fixture: a FOREIGN codebase (nothing of parrot0 in it) whose conditions
 * encode a word list as code — runs of calls to the same predicate joined by ||.
 * The or-chain detector must count here exactly as it does on src/brain; if it
 * only worked there, the faculty would be fitted, not general (anti-impostor). */
#include <stdio.h>
#include <string.h>
static int lookup(const char *s, const char *key); /* gen262: defined at end of file */
static int flag(const char *s, const char *w) { return strstr(s, w) != NULL; }
static int other(const char *s) { return s[0] == '!'; }

int classify(const char *s) {
    /* a 3-call chain: vocabulary shaped as code */
    if (flag(s, "alpha") || flag(s, "beta") || flag(s, "gamma"))
        return 1;
    /* mixed condition: the run of flag() calls is 1 long — NOT a chain */
    if (flag(s, "delta") || other(s))
        return 2;
    /* a 2-call chain, spread over two lines */
    if (flag(s, "epsilon") ||
        flag(s, "zeta"))
        return 3;
    /* a lone call — not a chain; and a || in a comment: x() || y() */
    if (flag(s, "eta"))
        return 4;
    printf("unclassified: %s\n", s);
    return 0;
}

/* gen262: the codebase's own vocabulary-lookup primitive (what kb_cue_match is
 * inside parrot0). A migrated chain becomes ONE call to it, keyed by chain site;
 * the key's words live in the emitted <file>.cues.p0, not in code. Kept at the
 * END of the file so the chain line numbers above stay stable for the ratchets.
 * The stub body is enough for the compile judge; wiring it to actually read the
 * emitted cues is the run_test_suite pull. */
static int lookup(const char *s, const char *key) { (void)s; (void)key; return 0; }
