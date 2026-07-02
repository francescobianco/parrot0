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

/* gen262/gen263: the codebase's own vocabulary-lookup primitive (what
 * kb_cue_match is inside parrot0). A migrated chain becomes ONE call to it,
 * keyed by chain site; the words live in the emitted <file>.cues.p0 — DATA,
 * not code. gen263 made it real: it scans the emitted facts file for lines
 * `intent_cue(<key>, "word").` and reports whether any word occurs in `s`,
 * which is exactly what the original chain of flag() calls computed. Kept at
 * the END of the file so the chain line numbers above stay stable. */
static int lookup(const char *s, const char *key) {
    FILE *f = fopen("tests/fixtures/orchain/foreign.c.cues.p0", "r");
    if (!f) return 0;
    char ln[256];
    size_t kl = strlen(key);
    int hit = 0;
    while (!hit && fgets(ln, sizeof ln, f)) {
        char *p = strchr(ln, '(');
        if (!p || strncmp(p + 1, key, kl) != 0 || p[1 + kl] != ',') continue;
        char *q = strchr(p, '"');
        if (!q) continue;
        char *e = strchr(q + 1, '"');
        if (!e) continue;
        *e = '\0';
        if (strstr(s, q + 1)) hit = 1;
    }
    fclose(f);
    return hit;
}
