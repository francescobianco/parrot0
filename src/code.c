/* gen173: parrot0's in-C structural code reader. See code.h for the contract and
 * the principle (AST-as-KB, deterministic parse, straight into RAM). This is the
 * F2 (grammar/structure) faculty of docs/CODE-MASTERY.md, the one place where the
 * formal, decidable grammar of code lets primitives-first dominate outright. */
#include "code.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* A C control-flow / type keyword can sit right before '(' (e.g. "if (", "for (",
 * "sizeof (") and look like a definition head; it is not a function name, so it
 * is excluded. The function's own name is never a keyword. */
static int is_c_keyword(const char *w) {
    static const char *const kw[] = {
        "if", "else", "for", "while", "do", "switch", "case", "return",
        "sizeof", "struct", "union", "enum", "typedef", "static", "const",
        "unsigned", "signed", "void", "int", "char", "short", "long", "float",
        "double", "goto", "break", "continue", "default", "extern", "register",
        "volatile", "inline", NULL,
    };
    for (const char *const *k = kw; *k; k++)
        if (strcmp(w, *k) == 0) return 1;
    return 0;
}

size_t code_ingest(KB *kb, const char *src,
                   char names[][KB_TERM_LEN], size_t max) {
    if (!kb || !src) return 0;
    size_t found = 0;

    const char *p = src;
    while (*p) {
        /* Find the start of an identifier. */
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);

        /* A definition head is an identifier immediately followed by '(' ... */
        const char *q = p;
        while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;                 /* p already advanced; no loop */

        /* ... a balanced parameter list ... */
        int depth = 0;
        const char *r = q;
        for (; *r; r++) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
        }
        if (depth != 0) break;                    /* unbalanced — stop scanning */

        /* ... and a '{' body right after ')'. A call is followed by ';'/operator,
         * a prototype by ';' — only a definition opens a block here. */
        while (*r == ' ' || *r == '\t') r++;
        if (*r != '{') continue;

        /* It is a function definition (unless the head is a keyword like "if"). */
        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) { p = r; continue; }
        memcpy(name, id, idlen);
        name[idlen] = '\0';
        if (is_c_keyword(name)) { p = r; continue; }

        /* Assert into RAM immediately — code joins the same KB as the world.
         * KB_BASE provenance so the session writer does not re-save derived
         * structure (it is re-derivable from the source). */
        kb_set_origin(kb, KB_BASE);
        const char *args[] = { name };
        kb_assert(kb, "code_function", args, 1);
        kb_set_origin(kb, KB_SESSION);

        if (names && found < max)
            snprintf(names[found], KB_TERM_LEN, "%s", name);
        found++;

        p = r;                                     /* continue past '{' into body */
    }
    return found;
}
