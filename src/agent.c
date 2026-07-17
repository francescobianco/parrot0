/*
 * agent.c - implementation of the typed agent kernel. See agent.h for the why.
 *
 * Deliberately boring: a dense dynamic array, a monotonic clock, bounded
 * copies. No hashes, no threads, no Brain. Indexing beyond the O(1) id lookup
 * arrives only when a profile on a declared scale asks for it (the same rule as
 * KB_MAX_*: limits become observable gaps before they become speculation).
 */
#include "agent.h"
#include "json.h"

#include <stdlib.h>
#include <string.h>

struct P0AStore {
    P0ARec  *v;
    size_t   n;
    size_t   cap;
    uint64_t clock;
};

static const char *const KIND_NAMES[] = {
    "segment", "event", "task", "goal", "constraint",
    "action", "observation", "artifact", "verdict", "gap", "decision"
};

#define P0A_N_KINDS (sizeof(KIND_NAMES) / sizeof(KIND_NAMES[0]))

P0AStore *p0a_new(void)
{
    return calloc(1, sizeof(P0AStore));
}

void p0a_free(P0AStore *s)
{
    if (!s) return;
    free(s->v);
    free(s);
}

uint64_t p0a_add(P0AStore *s, P0AKind kind, uint64_t parent, uint64_t source,
                 const char *name, const char *state, const char *payload)
{
    P0ARec *r;

    if (!s) return 0;
    if (s->n == s->cap) {
        size_t ncap = s->cap ? s->cap * 2 : 16;
        P0ARec *nv = realloc(s->v, ncap * sizeof(*nv));
        if (!nv) return 0;
        s->v = nv;
        s->cap = ncap;
    }
    r = &s->v[s->n];
    memset(r, 0, sizeof(*r));
    r->id     = ++s->clock;
    r->seq    = s->clock;
    r->kind   = kind;
    r->parent = parent;
    r->source = source;
    if (name)    snprintf(r->name, sizeof(r->name), "%s", name);
    if (state)   snprintf(r->state, sizeof(r->state), "%s", state);
    if (payload) snprintf(r->payload, sizeof(r->payload), "%s", payload);
    s->n++;
    return r->id;
}

const P0ARec *p0a_get(const P0AStore *s, uint64_t id)
{
    if (!s || id == 0 || id > s->n) return NULL;
    return &s->v[id - 1];            /* ids are dense: v[id-1].id == id        */
}

int p0a_set_state(P0AStore *s, uint64_t id, const char *state)
{
    P0ARec *r;
    if (!s || id == 0 || id > s->n) return 0;
    r = &s->v[id - 1];
    snprintf(r->state, sizeof(r->state), "%s", state ? state : "");
    return 1;
}

size_t p0a_count(const P0AStore *s, int kind)
{
    size_t i, n = 0;
    if (!s) return 0;
    if (kind < 0) return s->n;
    for (i = 0; i < s->n; i++)
        if ((int)s->v[i].kind == kind) n++;
    return n;
}

size_t p0a_select(const P0AStore *s, int kind, uint64_t parent,
                  const P0ARec **out, size_t cap)
{
    size_t i, total = 0;
    if (!s) return 0;
    for (i = 0; i < s->n; i++) {
        if (kind >= 0 && (int)s->v[i].kind != kind) continue;
        if (parent != P0A_ANY && s->v[i].parent != parent) continue;
        if (out && total < cap) out[total] = &s->v[i];
        total++;
    }
    return total;
}

const P0ARec *p0a_last(const P0AStore *s, int kind, uint64_t parent)
{
    size_t i;
    if (!s) return NULL;
    for (i = s->n; i-- > 0; ) {
        if (kind >= 0 && (int)s->v[i].kind != kind) continue;
        if (parent != P0A_ANY && s->v[i].parent != parent) continue;
        return &s->v[i];
    }
    return NULL;
}

int p0a_write_jsonl(const P0AStore *s, FILE *out)
{
    size_t i;
    if (!s || !out) return -1;
    for (i = 0; i < s->n; i++) {
        const P0ARec *r = &s->v[i];
        char *ename = json_escape(r->name);
        char *estate = json_escape(r->state);
        char *epayload = json_escape(r->payload);
        int rc;
        if (!ename || !estate || !epayload) {
            free(ename); free(estate); free(epayload);
            return -1;
        }
        rc = fprintf(out,
            "{\"id\":%llu,\"kind\":\"%s\",\"parent\":%llu,\"source\":%llu,"
            "\"seq\":%llu,\"state\":\"%s\",\"name\":\"%s\",\"payload\":\"%s\"}\n",
            (unsigned long long)r->id, p0a_kind_name(r->kind),
            (unsigned long long)r->parent, (unsigned long long)r->source,
            (unsigned long long)r->seq, estate, ename, epayload);
        free(ename); free(estate); free(epayload);
        if (rc < 0) return -1;
    }
    return ferror(out) ? -1 : 0;
}

/* p0 atoms are lowercase unquoted words; the state vocabulary already is. */
void p0a_fact(const P0ARec *r, char *buf, size_t cap)
{
    if (!r || !buf || cap == 0) return;
    snprintf(buf, cap, "rec(%llu,%s,%llu,%llu,%s).",
             (unsigned long long)r->id, p0a_kind_name(r->kind),
             (unsigned long long)r->parent, (unsigned long long)r->source,
             r->state[0] ? r->state : "open");
}

void p0a_fact_name(const P0ARec *r, char *buf, size_t cap)
{
    char esc[P0A_NAME];
    size_t i, j = 0;
    if (!r || !buf || cap == 0) { if (buf && cap) buf[0] = 0; return; }
    if (!r->name[0]) { buf[0] = 0; return; }
    for (i = 0; r->name[i] && j + 1 < sizeof(esc); i++) {
        char c = r->name[i];
        if (c == '"') c = '\'';                 /* keep the p0 quoted atom sane */
        if (c == '\n' || c == '\r') c = ' ';
        esc[j++] = c;
    }
    esc[j] = 0;
    snprintf(buf, cap, "rec_name(%llu,\"%s\").",
             (unsigned long long)r->id, esc);
}

const char *p0a_kind_name(P0AKind kind)
{
    if (kind < 0 || (size_t)kind >= P0A_N_KINDS) return "unknown";
    return KIND_NAMES[kind];
}

int p0a_kind_from_name(const char *name)
{
    size_t i;
    if (!name) return -1;
    for (i = 0; i < P0A_N_KINDS; i++)
        if (strcmp(name, KIND_NAMES[i]) == 0) return (int)i;
    return -1;
}
