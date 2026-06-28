/* gen171: parrot0's in-C dynamic learner. See learn.h for the contract and the
 * principle (static knowledge only, straight into RAM, no hot reload). */
#include "learn.h"

#include <ctype.h>
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
/* gen240 (universal-comprehension §7): on-demand fetch of a CERTIFIED static
 * Wikipedia page, written into the SAME pages/<key>.md the shipped corpus uses —
 * so learning a topic on demand is provably identical to having shipped it
 * a-priori. Only Wikipedia, only declarative summary text; never a search engine
 * or an intelligence API. Build-gated by PARROT0_HAVE_CURL (libcurl present),
 * run-gated by PARROT0_WIKI_FETCH. */
#ifdef PARROT0_HAVE_CURL
/* Minimal libcurl ABI — the dev headers are not required; we declare only what we
 * use and link the SONAME (-l:libcurl.so.4). Option/constant values are stable
 * libcurl ABI. */
typedef void CURL;
extern CURL *curl_easy_init(void);
extern int   curl_easy_setopt(CURL *, int, ...);
extern int   curl_easy_perform(CURL *);
extern void  curl_easy_cleanup(CURL *);
#define CURLOPT_URL            10002
#define CURLOPT_WRITEFUNCTION  20011
#define CURLOPT_WRITEDATA      10001
#define CURLOPT_USERAGENT      10018
#define CURLOPT_FOLLOWLOCATION 52
#define CURLOPT_TIMEOUT        13

struct fetch_buf { char *p; size_t n; };

static size_t fetch_wr(char *ptr, size_t sz, size_t nm, void *ud) {
    size_t add = sz * nm;
    struct fetch_buf *b = ud;
    if (b->n + add > 4u * 1024u * 1024u) return 0;   /* hard 4MB cap */
    char *q = realloc(b->p, b->n + add + 1);
    if (!q) return 0;
    b->p = q; memcpy(b->p + b->n, ptr, add); b->n += add; b->p[b->n] = '\0';
    return add;
}

/* GET url, returning a malloc'd NUL-terminated body (caller frees), or NULL. */
static char *http_get(const char *url) {
    CURL *c = curl_easy_init();
    if (!c) return NULL;
    struct fetch_buf b = { NULL, 0 };
    curl_easy_setopt(c, CURLOPT_URL, url);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, fetch_wr);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &b);
    curl_easy_setopt(c, CURLOPT_USERAGENT,
                     "parrot0-wiki-learner/1.0 (https://github.com/francescobianco/parrot0)");
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 20L);
    int rc = curl_easy_perform(c);
    curl_easy_cleanup(c);
    if (rc != 0) { free(b.p); return NULL; }
    return b.p;
}

/* Decode the JSON string value of "extract" into ASCII `out`. Handles the escapes
 * Wikipedia emits (\" \\ \/ \n \t \r \uXXXX); non-ASCII is dropped. */
static int json_extract_field(const char *json, const char *field,
                              char *out, size_t out_sz) {
    char needle[32];
    snprintf(needle, sizeof needle, "\"%s\"", field);
    const char *e = strstr(json, needle);
    if (!e) return 0;
    e = strchr(e, ':');
    if (!e) return 0;
    e++;
    while (*e == ' ' || *e == '\t') e++;
    if (*e != '"') return 0;
    e++;
    size_t o = 0;
    while (*e && *e != '"' && o + 1 < out_sz) {
        if (*e == '\\') {
            e++;
            if (*e == 'n' || *e == 't' || *e == 'r') { out[o++] = ' '; e++; }
            else if (*e == 'u') {
                e++;
                int v = 0, ok = 1;
                for (int i = 0; i < 4 && *e; i++, e++) {
                    char h = *e; int d;
                    if (h >= '0' && h <= '9') d = h - '0';
                    else if (h >= 'a' && h <= 'f') d = h - 'a' + 10;
                    else if (h >= 'A' && h <= 'F') d = h - 'A' + 10;
                    else { ok = 0; break; }
                    v = v * 16 + d;
                }
                if (ok && v >= 32 && v < 128) out[o++] = (char)v;   /* ASCII only */
            } else if (*e) { out[o++] = *e; e++; }                  /* \" \\ \/  */
        } else {
            unsigned char uc = (unsigned char)*e;
            if (uc >= 32 && uc < 128) out[o++] = *e;                /* drop non-ASCII */
            e++;
        }
    }
    while (o > 0 && out[o - 1] == ' ') o--;
    out[o] = '\0';
    return o > 0;
}

int wiki_fetch_topic(const char *key) {
    const char *en = getenv("PARROT0_WIKI_FETCH");
    if (!en || !*en || !strcmp(en, "0")) return 0;
    if (!key || !*key) return 0;

    /* sanitize the key to [a-z0-9_] (it already is, from mod_learn) */
    char k[96]; size_t kn = 0;
    for (const char *p = key; *p && kn + 1 < sizeof k; p++) {
        char c = *p;
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') k[kn++] = c;
        else if (c >= 'A' && c <= 'Z') k[kn++] = (char)(c - 'A' + 'a');
    }
    k[kn] = '\0';
    if (kn < 2) return 0;

    /* Wikipedia capitalizes the leading letter; FOLLOWLOCATION resolves redirects. */
    char title[96]; snprintf(title, sizeof title, "%s", k);
    if (title[0]) title[0] = (char)toupper((unsigned char)title[0]);

    char url[256];
    snprintf(url, sizeof url,
             "https://en.wikipedia.org/api/rest_v1/page/summary/%s", title);
    char *json = http_get(url);
    if (!json) return 0;

    char extract[4096];
    int got = json_extract_field(json, "extract", extract, sizeof extract);
    free(json);
    if (!got || strlen(extract) < 10) return 0;

    /* first sentence (>= 30 chars) as the lead concept, matching the corpus */
    size_t el = strlen(extract), cut = 0;
    for (size_t i = 0; i < el; i++)
        if ((extract[i] == '.' || extract[i] == '!' || extract[i] == '?') &&
            (i + 1 >= el || extract[i + 1] == ' ')) {
            if (i + 1 >= 30) { cut = i + 1; break; }
        }
    if (cut == 0) cut = el;
    char concept[1024];
    if (cut >= sizeof concept) cut = sizeof concept - 1;
    memcpy(concept, extract, cut); concept[cut] = '\0';

    const char *dir = getenv("PARROT0_WIKI_DIR");
    if (!dir || !*dir) dir = "kb/learning/pages";
    char path[512];
    snprintf(path, sizeof path, "%s/%s.md", dir, k);
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fprintf(f,
            "# %s\n\n- Domain: `general`\n- Source: "
            "https://en.wikipedia.org/wiki/%s\n\n"
            "## Learned Concept\n\n%s\n\n## Extract\n\n%s\n",
            title, title, concept, extract);
    fclose(f);
    return 1;
}
#else
int wiki_fetch_topic(const char *key) { (void)key; return 0; }
#endif /* PARROT0_HAVE_CURL */
