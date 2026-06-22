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

    /* gen174: one structural pass that tracks the enclosing function by brace
     * depth, so an identifier-call inside a body is attributed to its caller
     * (code_calls/2) — the F3 call-graph faculty, derived from structure. */
    char cur_fn[KB_TERM_LEN] = "";
    int brace = 0;
    int fn_brace = -1;                  /* body depth of cur_fn; -1 = outside */

    const char *p = src;
    while (*p) {
        if (*p == '{') { brace++; p++; continue; }
        if (*p == '}') {
            brace--;
            if (fn_brace >= 0 && brace < fn_brace) { cur_fn[0] = '\0'; fn_brace = -1; }
            p++; continue;
        }

        /* Find the start of an identifier. */
        if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        size_t idlen = (size_t)(p - id);

        /* An identifier followed by '(' is a definition head or a call. */
        const char *q = p;
        while (*q == ' ' || *q == '\t') q++;
        if (*q != '(') continue;                 /* p already advanced; no loop */

        /* Balance the parens (lookahead only — p does not move past them, so a
         * nested call in the argument list is still seen next iterations). */
        int depth = 0;
        const char *r = q;
        for (; *r; r++) {
            if (*r == '(') depth++;
            else if (*r == ')') { depth--; if (depth == 0) { r++; break; } }
        }
        if (depth != 0) break;                    /* unbalanced — stop scanning */

        const char *after = r;
        while (*after == ' ' || *after == '\t') after++;

        char name[KB_TERM_LEN];
        if (idlen == 0 || idlen >= sizeof name) continue;
        memcpy(name, id, idlen);
        name[idlen] = '\0';
        if (is_c_keyword(name)) continue;         /* if/for/while/sizeof... */

        if (*after == '{') {
            /* Function definition. Assert into RAM immediately — code joins the
             * same KB as the world. KB_BASE provenance so the session writer
             * does not re-save derived structure (re-derivable from source). */
            kb_set_origin(kb, KB_BASE);
            const char *args[] = { name };
            kb_assert(kb, "code_function", args, 1);
            kb_set_origin(kb, KB_SESSION);

            /* Redefinition replaces: clear this function's prior call edges so a
             * re-ingest of the same name in one session does not merge stale
             * callees from an earlier, different body. */
            char old[32][KB_TERM_LEN];
            const char *qp[] = { name, NULL };
            size_t no = kb_match(kb, "code_calls", qp, 2, old, 32);
            for (size_t i = 0; i < no; i++) {
                const char *ra[] = { name, old[i] };
                kb_retract(kb, "code_calls", ra, 2);
            }
            if (names && found < max)
                snprintf(names[found], KB_TERM_LEN, "%s", name);
            found++;
            /* Enter the body: let the '{' be counted on the next iteration. */
            snprintf(cur_fn, sizeof cur_fn, "%s", name);
            fn_brace = brace + 1;
            p = after;
        } else if (cur_fn[0]) {
            /* A call inside a function body — attribute it to the caller. */
            kb_set_origin(kb, KB_BASE);
            const char *args[] = { cur_fn, name };
            kb_assert(kb, "code_calls", args, 2);
            kb_set_origin(kb, KB_SESSION);
            /* p stays just past the ident, so nested calls are still seen. */
        }
    }
    return found;
}
