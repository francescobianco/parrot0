/* env.c — runtime configuration layer (gen346). See p0env.h. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"

#define P0ENV_MAX 64
#define P0ENV_NAME 64
#define P0ENV_VAL 1024

typedef struct { char name[P0ENV_NAME]; char val[P0ENV_VAL]; int set; } Override;

static Override g_over[P0ENV_MAX];
static size_t   g_n;

/* The names read while the brain boots (99-registry brain_create/brain_boot/
 * brain_policy): a change to any of these re-derives the loaded knowledge, so the
 * test-engine reloads after setting one. Per-turn reads (PARROT0_ORACLE, HOME,
 * PARROT0_WIKI_DIR, PARROT0_DEEP_BUDGET, …) are pilotable too but take effect on
 * the next read with no reload, so they are deliberately absent here. */
static const char *const MEMORY_VARS[] = {
    "PARROT0_BASE", "PARROT0_SESSION", "PARROT0_PROFILE", "PARROT0_LEXICON",
    "PARROT0_WORLD_FACTS", "PARROT0_KB_ROOT",
    "PARROT0_LANG", "LANG", "LC_ALL", "LC_MESSAGES",
    "PARROT0_TOOLS", "PARROT0_WIKI_FETCH", "PARROT0_PID",
    NULL
};

int p0env_affects_memory(const char *name) {
    if (!name) return 0;
    for (size_t i = 0; MEMORY_VARS[i]; i++)
        if (strcmp(name, MEMORY_VARS[i]) == 0) return 1;
    return 0;
}

static Override *find(const char *name) {
    for (size_t i = 0; i < g_n; i++)
        if (g_over[i].set && strcmp(g_over[i].name, name) == 0) return &g_over[i];
    return NULL;
}

const char *p0env(const char *name) {
    if (!name) return NULL;
    Override *o = find(name);
    if (o) return o->val;
    return getenv(name);
}

int p0env_set(const char *name, const char *value) {
    if (!name || !*name) return 0;
    if (!value) value = "";

    /* effective value BEFORE the change, to decide whether anything moved */
    const char *cur = p0env(name);
    int changed = (cur == NULL) || strcmp(cur, value) != 0;

    Override *o = find(name);
    if (!o) {
        for (size_t i = 0; i < g_n; i++) if (!g_over[i].set) { o = &g_over[i]; break; }
        if (!o && g_n < P0ENV_MAX) o = &g_over[g_n++];
        if (!o) return 0;                 /* table full: silently drop */
        snprintf(o->name, sizeof o->name, "%s", name);
        o->set = 1;
    }
    snprintf(o->val, sizeof o->val, "%s", value);

    return changed && p0env_affects_memory(name);
}

int p0env_clear(void) {
    int had_mem = 0;
    for (size_t i = 0; i < g_n; i++)
        if (g_over[i].set && p0env_affects_memory(g_over[i].name)) had_mem = 1;
    for (size_t i = 0; i < g_n; i++) g_over[i].set = 0;
    g_n = 0;
    return had_mem;
}

void p0env_mem_signature(char *out, size_t n) {
    if (!out || n == 0) return;
    size_t o = 0;
    out[0] = '\0';
    for (size_t i = 0; MEMORY_VARS[i] && o + 1 < n; i++) {
        const char *v = p0env(MEMORY_VARS[i]);
        /* control-char separators keep the signature strcmp-comparable (no NUL):
         * '\x1f' between fields, '\x1e' before the value, and '\x02' as the "unset"
         * marker so an unset variable never collides with one set to empty (""). */
        o += (size_t)snprintf(out + o, n - o, "%s\x1e%s\x1f", MEMORY_VARS[i],
                              v ? v : "\x02");
    }
}