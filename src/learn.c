/* gen171: parrot0's in-C dynamic learner. See learn.h for the contract and the
 * principle (static knowledge only, straight into RAM, no hot reload). */
#include "learn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void rstrip(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' ||
                     s[n-1] == ' '  || s[n-1] == '\t'))
        s[--n] = '\0';
}

/* Extract the backtick-quoted value after a label, e.g. `Domain: \`mathematics\``
 * -> "mathematics". Returns 1 on success. */
static int backtick_value(const char *line, const char *label,
                          char *out, size_t out_size) {
    const char *d = strstr(line, label);
    if (!d) return 0;
    const char *bt = strchr(d, '`');
    if (!bt) return 0;
    bt++;
    const char *end = strchr(bt, '`');
    if (!end) return 0;
    size_t l = (size_t)(end - bt);
    if (l >= out_size) l = out_size - 1;
    memcpy(out, bt, l);
    out[l] = '\0';
    return 1;
}

int learn_topic(KB *kb, const char *key, const char *title,
                char *def_out, size_t def_sz) {
    (void)title;
    if (!kb || !key || !*key) return 0;
    if (def_out && def_sz) def_out[0] = '\0';

    const char *dir = getenv("PARROT0_WIKI_DIR");
    if (!dir || !*dir) dir = "kb/learning/pages";
    char path[512];
    snprintf(path, sizeof path, "%s/%s.md", dir, key);
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char line[1024];
    char domain[KB_TERM_LEN] = "general";
    char def[KB_TERM_LEN] = "";
    int in_concept = 0;
    while (fgets(line, sizeof line, f)) {
        rstrip(line);
        if (!in_concept) {
            char tmp[KB_TERM_LEN];
            if (backtick_value(line, "Domain:", tmp, sizeof tmp))
                snprintf(domain, sizeof domain, "%s", tmp);
        }
        if (strncmp(line, "## Learned Concept", 18) == 0) { in_concept = 1; continue; }
        if (in_concept) {
            if (line[0] == '\0') continue;          /* skip blank lines */
            if (strncmp(line, "##", 2) == 0) break;  /* next section, no concept */
            snprintf(def, sizeof def, "%s", line);
            break;
        }
    }
    fclose(f);
    if (!def[0]) return 0;

    /* Guard the p0 string: a stray double quote would corrupt the fact. */
    for (char *c = def; *c; c++) if (*c == '"') *c = '\'';

    /* Assert into RAM immediately — no hot reload. KB_BASE provenance so the
     * session writer does not re-save it; persistence is handled explicitly.
     * gen172: store the def QUOTED, matching the .p0 on-disk convention
     * (parse_term keeps the surrounding quotes as part of the stored atom). This
     * makes an in-RAM learned fact indistinguishable from a loaded one, so
     * kb_is_concept_key recognises it and the description renderer speaks it — a
     * second ask answers from RAM instead of re-reading the markdown. The "%.*s"
     * cap leaves room for the two quotes + NUL within KB_TERM_LEN. */
    char qdef[KB_TERM_LEN];
    snprintf(qdef, sizeof qdef, "\"%.*s\"", (int)(sizeof qdef - 3), def);
    kb_set_origin(kb, KB_BASE);
    const char *args[] = { key, domain, qdef };
    kb_assert(kb, "wiki_concept", args, 3);
    kb_set_origin(kb, KB_SESSION);

    /* Persist for commit when a learned-KB path is configured (unset in tests,
     * so the hermetic suite writes nothing). */
    const char *kbpath = getenv("PARROT0_LEARN_KB");
    if (kbpath && *kbpath) {
        FILE *o = fopen(kbpath, "a");
        if (o) {
            fprintf(o, "wiki_concept(%s, %s, \"%s\").\n", key, domain, def);
            fclose(o);
        }
    }

    if (def_out && def_sz) snprintf(def_out, def_sz, "%s", def);
    return 1;
}