/*
 * mcp.c - parrot0's MCP JSON-RPC engine over stdio (gen277). See mcp.h and
 * docs/plans/mcp-engine.md.
 *
 * The transport is newline-delimited JSON-RPC 2.0 on stdin/stdout — the MCP
 * stdio framing. One process = one Brain, so state persists across calls: an
 * agent asserts a fact and sees it on the next query; kb.save + kb.restore make
 * knowledge written to a file go live without restarting parrot0.
 *
 * This file adds NO inference logic. Every tool is a thin adapter over an
 * existing, tested primitive (kb.h / brain.h): expose the engine, don't wrap it.
 */
#define _POSIX_C_SOURCE 200809L

#include "mcp.h"
#include "json.h"
#include "kb.h"
#include "brain.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MCP_PROTO   "2024-11-05"
#define MCP_LINECAP (1024 * 1024)   /* one JSON-RPC message per line */

/* ------------------------------------------------------------------------- */
/* a small string builder for assembling JSON payloads                        */
/* ------------------------------------------------------------------------- */
typedef struct { char *buf; size_t len, cap; int oom; } SB;

static void sb_init(SB *s) {
    s->cap = 256; s->len = 0; s->oom = 0;
    s->buf = malloc(s->cap);
    if (s->buf) s->buf[0] = '\0'; else s->oom = 1;
}
static void sb_free(SB *s) { free(s->buf); s->buf = NULL; }
static void sb_putc(SB *s, char c) {
    if (s->oom) return;
    if (s->len + 2 > s->cap) {
        size_t nc = s->cap * 2;
        char *g = realloc(s->buf, nc);
        if (!g) { s->oom = 1; return; }
        s->buf = g; s->cap = nc;
    }
    s->buf[s->len++] = c; s->buf[s->len] = '\0';
}
static void sb_puts(SB *s, const char *t) { for (; t && *t; t++) sb_putc(s, *t); }
/* append `t` as a JSON string literal (with surrounding quotes, escaped). */
static void sb_jstr(SB *s, const char *t) {
    char *e = json_escape(t ? t : "");
    sb_putc(s, '"'); sb_puts(s, e ? e : ""); sb_putc(s, '"');
    free(e);
}

/* ------------------------------------------------------------------------- */
/* JSON-RPC response emitters (to stdout, one line each)                      */
/* ------------------------------------------------------------------------- */
static void write_id(const JVal *id) {
    if (id && id->type == J_NUM) {
        double d = id->num;
        if (d == (double)(long long)d) printf("%lld", (long long)d);
        else printf("%g", d);
    } else if (id && id->type == J_STR) {
        char *e = json_escape(id->str ? id->str : "");
        printf("\"%s\"", e ? e : ""); free(e);
    } else {
        printf("null");
    }
}

/* {"jsonrpc":"2.0","id":<id>,"result":<result_json>} */
static void emit_result(const JVal *id, const char *result_json) {
    printf("{\"jsonrpc\":\"2.0\",\"id\":");
    write_id(id);
    printf(",\"result\":%s}\n", result_json);
    fflush(stdout);
}

/* {"jsonrpc":"2.0","id":<id>,"error":{"code":c,"message":m}} */
static void emit_error(const JVal *id, int code, const char *msg) {
    char *e = json_escape(msg ? msg : "");
    printf("{\"jsonrpc\":\"2.0\",\"id\":");
    write_id(id);
    printf(",\"error\":{\"code\":%d,\"message\":\"%s\"}}\n", code, e ? e : "");
    free(e);
    fflush(stdout);
}

/* a tools/call result: the payload is delivered as ONE text content block; the
 * agent reads `payload` (itself a compact JSON string) as the tool's output. */
static void emit_tool(const JVal *id, const char *payload, int is_error) {
    char *e = json_escape(payload ? payload : "");
    printf("{\"jsonrpc\":\"2.0\",\"id\":");
    write_id(id);
    printf(",\"result\":{\"content\":[{\"type\":\"text\",\"text\":\"%s\"}],"
           "\"isError\":%s}}\n", e ? e : "", is_error ? "true" : "false");
    free(e);
    fflush(stdout);
}

/* ------------------------------------------------------------------------- */
/* argument helpers                                                           */
/* ------------------------------------------------------------------------- */
static const char *jstr(const JVal *o, const char *key) {
    JVal *v = jobj_get(o, key);
    return (v && v->type == J_STR) ? v->str : NULL;
}

/* U1 (gen279, teach-comprehension-via-mcp.md §5.5): a literal taught through MCP
 * must round-trip as a CONSTANT, not be mistaken for a variable. The engine's
 * convention (kb.c is_var) reads a leading '$' (gen280) or uppercase/'_' as a
 * variable, and the .p0 loader protects such content by quoting it. MCP had no
 * such step, so "Madrid" (and, since gen280, "$HOME") was stored as a free
 * variable. lit_encode applies the SAME discipline at the boundary: a literal
 * that would be misread — leading '$'/uppercase/'_', or containing
 * whitespace/comma — is wrapped in quotes (which is_var never treats as a
 * variable). lit_decode strips one surrounding pair on the way out, so the agent
 * sees exactly what it taught. A plain lowercase atom (and existing base facts)
 * are left untouched, so matching curated knowledge is unaffected. */
/* U3: does the string have the shape of a compound term  functor(args…)  with
 * balanced parens (functor of ident/$ chars, closing ')' only at the end)? Such
 * a term must NOT be quoted — its commas are structure, and the engine unifies
 * it structurally. */
static int looks_compound(const char *s) {
    if (!s || !*s) return 0;
    const char *lp = strchr(s, '(');
    size_t n = strlen(s);
    if (!lp || lp == s || s[n - 1] != ')') return 0;
    for (const char *p = s; p < lp; p++)
        if (!(isalnum((unsigned char)*p) || *p == '_' || *p == '$')) return 0;
    int d = 0;
    for (const char *p = lp; *p; p++) {
        if (*p == '(') d++;
        else if (*p == ')') { d--; if (d == 0 && p[1] != '\0') return 0; }
    }
    return d == 0;
}
static int lit_needs_quote(const char *s) {
    if (!s || !*s) return 0;
    if (s[0] == '"') return 0;                       /* already quoted */
    if (looks_compound(s)) return 0;                 /* U3: a compound term */
    if (s[0] == '$' || isupper((unsigned char)s[0]) || s[0] == '_') return 1;
    return strpbrk(s, " \t\n,") != NULL;             /* spaces / comma */
}
static void lit_encode(const char *s, char *buf, size_t bufsz) {
    if (lit_needs_quote(s))
        snprintf(buf, bufsz, "\"%s\"", s);
    else
        snprintf(buf, bufsz, "%s", s ? s : "");
}
/* Strip ONE surrounding pair of quotes, if present, into buf. */
static const char *lit_decode(const char *s, char *buf, size_t bufsz) {
    size_t len = s ? strlen(s) : 0;
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        size_t inner = len - 2;
        if (inner >= bufsz) inner = bufsz - 1;
        memcpy(buf, s + 1, inner);
        buf[inner] = '\0';
        return buf;
    }
    return s;
}

/* Build a parrot0 argument vector from a JSON array. String literals are
 * quote-encoded (lit_encode) so uppercase/spaced values store as constants;
 * numbers/booleans stringify; JSON null becomes a VARIABLE slot (NULL, for
 * kb_match). Everything lands in a per-call scratch. Returns argc; sets
 * *has_var if any slot is a variable. */
static char argscratch[KB_MAX_ARGS][KB_TERM_LEN];
static size_t build_args(const JVal *arr, const char *slots[], size_t max,
                         int *has_var) {
    size_t n = 0;
    if (has_var) *has_var = 0;
    if (!arr || arr->type != J_ARR) return 0;
    for (size_t i = 0; i < arr->n && n < max; i++) {
        const JVal *e = arr->items[i];
        if (!e || e->type == J_NULL) {
            slots[n] = NULL;
            if (has_var) *has_var = 1;
        } else if (e->type == J_STR) {
            lit_encode(e->str, argscratch[n], KB_TERM_LEN);
            slots[n] = argscratch[n];
        } else if (e->type == J_NUM) {
            double d = e->num;
            if (d == (double)(long long)d)
                snprintf(argscratch[n], KB_TERM_LEN, "%lld", (long long)d);
            else
                snprintf(argscratch[n], KB_TERM_LEN, "%g", d);
            slots[n] = argscratch[n];
        } else if (e->type == J_BOOL) {
            slots[n] = e->b ? "true" : "false";
        } else {
            slots[n] = "";
        }
        n++;
    }
    return n;
}

/* Build clause argument slots. Like build_args but $-prefixed strings
 * are VARIABLES (passed as-is), not encoded as quoted constants.
 * Stores encoded values in `scratch` to avoid dangling pointers. */
static size_t build_clause_args(const JVal *arr, const char *slots[], size_t max,
                                 char scratch[][KB_TERM_LEN]) {
    size_t n = 0;
    if (!arr || arr->type != J_ARR) return 0;
    for (size_t i = 0; i < arr->n && n < max; i++) {
        const JVal *e = arr->items[i];
        if (!e || e->type == J_NULL) {
            slots[n] = NULL;
        } else if (e->type == J_STR) {
            if (e->str && e->str[0] == '$') {
                slots[n] = e->str;                     /* variable: as-is */
            } else {
                lit_encode(e->str, scratch[n], KB_TERM_LEN);
                slots[n] = scratch[n];
            }
        } else if (e->type == J_NUM) {
            double d = e->num;
            if (d == (double)(long long)d)
                snprintf(scratch[n], KB_TERM_LEN, "%lld", (long long)d);
            else
                snprintf(scratch[n], KB_TERM_LEN, "%g", d);
            slots[n] = scratch[n];
        } else if (e->type == J_BOOL) {
            slots[n] = e->b ? "true" : "false";
        } else {
            slots[n] = "";
        }
        n++;
    }
    return n;
}

static int path_ok(const char *p) {
    return p && *p && p[0] != '/' && p[0] != '~' && !strstr(p, "..");
}

/* ------------------------------------------------------------------------- */
/* tool table (for tools/list)                                                */
/* ------------------------------------------------------------------------- */
typedef struct { const char *name, *desc, *schema; } McpTool;

static const McpTool TOOLS[] = {
{"kb.assert", "Assert a ground fact pred(args...) into the session KB.",
 "{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\",\"items\":{\"type\":[\"string\",\"number\"]}}},"
 "\"required\":[\"pred\",\"args\"]}"},
{"kb.assert_rule", "Assert a definite rule head(X) :- b0(X),...,bn(X).",
 "{\"type\":\"object\",\"properties\":{\"head\":{\"type\":\"string\"},"
 "\"body\":{\"type\":\"array\",\"items\":{\"type\":\"string\"}}},"
 "\"required\":[\"head\",\"body\"]}"},
{"kb.assert_clause", "Assert a full definite clause head:-body0,…,bodyN with "
 "n-ary head and body goals carrying distinct/shared $-variables. A body goal "
 "with \"neg\":true is negation-as-failure (succeeds iff not derivable), for "
 "defaults with exceptions.",
 "{\"type\":\"object\",\"properties\":{"
 "\"head\":{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\"}},\"required\":[\"pred\",\"args\"]},"
 "\"body\":{\"type\":\"array\",\"items\":{\"type\":\"object\",\"properties\":{"
 "\"pred\":{\"type\":\"string\"},\"args\":{\"type\":\"array\"},"
 "\"neg\":{\"type\":\"boolean\"}},"
 "\"required\":[\"pred\",\"args\"]}}},"
 "\"required\":[\"head\",\"body\"]}"},
{"kb.retract", "Retract a ground fact pred(args...) if present.",
 "{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\"}},\"required\":[\"pred\",\"args\"]}"},
{"kb.query", "Prove pred(args...) by resolution (SLD over facts+rules).",
 "{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\"}},\"required\":[\"pred\",\"args\"]}"},
{"kb.match", "Match pred(args...) with null slots as variables; returns the "
 "bindings of the first variable slot.",
 "{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\"}},\"required\":[\"pred\",\"args\"]}"},
{"kb.explain", "Prove pred(args...) and return a one-line proof explanation.",
 "{\"type\":\"object\",\"properties\":{\"pred\":{\"type\":\"string\"},"
 "\"args\":{\"type\":\"array\"}},\"required\":[\"pred\",\"args\"]}"},
{"kb.describe", "Report the direct ground facts about an entity.",
 "{\"type\":\"object\",\"properties\":{\"entity\":{\"type\":\"string\"}},"
 "\"required\":[\"entity\"]}"},
{"kb.dump", "Dump all facts currently in the KB (human-readable).",
 "{\"type\":\"object\",\"properties\":{}}"},
{"kb.induce", "Induce definite rules from the ground facts (min_support).",
 "{\"type\":\"object\",\"properties\":{\"min_support\":{\"type\":\"number\"}}}"},
{"kb.stats", "Report how many facts the KB holds.",
 "{\"type\":\"object\",\"properties\":{}}"},
{"kb.save", "Persist the session delta (session+induced clauses) to a file.",
 "{\"type\":\"object\",\"properties\":{\"path\":{\"type\":\"string\"}}}"},
{"kb.restore", "Forget the unsaved session and reload every KB file from disk "
 "in place (the /restore of gen276) — makes file edits go live.",
 "{\"type\":\"object\",\"properties\":{}}"},
{"gen.respond", "Route a natural-language turn through parrot0's full brain and "
 "return the reply (the same path the chat REPL uses).",
 "{\"type\":\"object\",\"properties\":{\"input\":{\"type\":\"string\"}},"
 "\"required\":[\"input\"]}"},
{"text.extract", "Read a passage and extract its clauses as facts into the KB.",
 "{\"type\":\"object\",\"properties\":{\"passage\":{\"type\":\"string\"}},"
 "\"required\":[\"passage\"]}"},
{"style.set_temperature", "Set the reply-form selection temperature (0 = always "
 "the canonical phrasing; non-zero = anti-repeat rotation).",
 "{\"type\":\"object\",\"properties\":{\"value\":{\"type\":\"number\"}},"
 "\"required\":[\"value\"]}"},
{"style.get_temperature", "Read the current style temperature, or null if unset.",
 "{\"type\":\"object\",\"properties\":{}}"},
};
static const size_t NTOOLS = sizeof TOOLS / sizeof TOOLS[0];

/* ------------------------------------------------------------------------- */
/* tool handlers — each fills `out` with a compact JSON payload, returns      */
/* 1 on success / 0 on a tool-level error (bad arguments).                    */
/* ------------------------------------------------------------------------- */
static int need_pred_args(const JVal *a, const char **pred, const JVal **args,
                          char *err, size_t errsz) {
    *pred = jstr(a, "pred");
    *args = jobj_get(a, "args");
    if (!*pred) { snprintf(err, errsz, "{\"error\":\"missing 'pred'\"}"); return 0; }
    return 1;
}

static int tool_call(Brain *b, const char *name, const JVal *a,
                     char *out, size_t outsz) {
    KB *kb = brain_kb(b);
    const char *slots[KB_MAX_ARGS];

    if (strcmp(name, "kb.assert") == 0) {
        const char *pred; const JVal *args;
        if (!need_pred_args(a, &pred, &args, out, outsz)) return 0;
        size_t argc = build_args(args, slots, KB_MAX_ARGS, NULL);
        kb_set_origin(kb, KB_SESSION);
        int r = kb_assert(kb, pred, slots, argc);
        snprintf(out, outsz, "{\"ok\":true,\"stored\":%s}", r ? "true" : "false");
        return 1;
    }
    if (strcmp(name, "kb.assert_rule") == 0) {
        const char *head = jstr(a, "head");
        const JVal *body = jobj_get(a, "body");
        if (!head) { snprintf(out, outsz, "{\"error\":\"missing 'head'\"}"); return 0; }
        const char *bodies[KB_MAX_BODY];
        size_t nb = 0;
        if (body && body->type == J_ARR)
            for (size_t i = 0; i < body->n && nb < KB_MAX_BODY; i++)
                if (body->items[i] && body->items[i]->type == J_STR)
                    bodies[nb++] = body->items[i]->str;
        if (nb == 0) { snprintf(out, outsz, "{\"error\":\"empty 'body'\"}"); return 0; }
        kb_set_origin(kb, KB_SESSION);
        int r = kb_assert_rule_n(kb, head, bodies, nb);
        snprintf(out, outsz, "{\"ok\":%s}", r ? "true" : "false");
        return 1;
    }
    if (strcmp(name, "kb.assert_clause") == 0) {
        const JVal *jhead = jobj_get(a, "head");
        const JVal *jbody = jobj_get(a, "body");
        if (!jhead || jhead->type != J_OBJ) { snprintf(out, outsz, "{\"error\":\"missing 'head'\"}"); return 0; }
        const char *hpred = jstr(jhead, "pred");
        if (!hpred) { snprintf(out, outsz, "{\"error\":\"head missing 'pred'\"}"); return 0; }
        const JVal *hargs = jobj_get(jhead, "args");

        /* persistent scratch for up to 1 head + KB_MAX_BODY goals */
        static char gscratch[KB_MAX_BODY + 1][KB_MAX_ARGS][KB_TERM_LEN];
        static const char *gslots[KB_MAX_BODY + 1][KB_MAX_ARGS];

        const char *hslots[KB_MAX_ARGS];
        size_t hargc = build_clause_args(hargs, hslots, KB_MAX_ARGS, gscratch[0]);
        KbGoal head = { hpred, hslots, hargc, 0 };   /* heads are never negated */
        if (hargc == 0 && hargs) { snprintf(out, outsz, "{\"error\":\"empty head args\"}"); return 0; }

        KbGoal body_goals[KB_MAX_BODY];
        size_t nb = 0;
        if (jbody && jbody->type == J_ARR) {
            for (size_t i = 0; i < jbody->n && nb < KB_MAX_BODY; i++) {
                const JVal *g = jbody->items[i];
                if (!g || g->type != J_OBJ) continue;
                const char *gp = jstr(g, "pred");
                if (!gp) continue;
                const JVal *ga = jobj_get(g, "args");
                size_t gargc = build_clause_args(ga, gslots[nb + 1], KB_MAX_ARGS,
                                                  gscratch[nb + 1]);
                const JVal *gneg = jobj_get(g, "neg");   /* U6: naf body goal */
                body_goals[nb].pred = gp;
                body_goals[nb].args = gslots[nb + 1];
                body_goals[nb].argc = gargc;
                body_goals[nb].neg = (gneg && gneg->type == J_BOOL && gneg->b) ? 1 : 0;
                nb++;
            }
        }
        if (nb == 0) { snprintf(out, outsz, "{\"error\":\"empty 'body'\"}"); return 0; }

        kb_set_origin(kb, KB_SESSION);
        int r = kb_assert_clause(kb, &head, body_goals, nb);
        snprintf(out, outsz, "{\"ok\":%s}", r ? "true" : "false");
        return 1;
    }
    if (strcmp(name, "kb.retract") == 0) {
        const char *pred; const JVal *args;
        if (!need_pred_args(a, &pred, &args, out, outsz)) return 0;
        size_t argc = build_args(args, slots, KB_MAX_ARGS, NULL);
        int r = kb_retract(kb, pred, slots, argc);
        snprintf(out, outsz, "{\"ok\":true,\"removed\":%s}", r ? "true" : "false");
        return 1;
    }
    if (strcmp(name, "kb.query") == 0) {
        const char *pred; const JVal *args;
        if (!need_pred_args(a, &pred, &args, out, outsz)) return 0;
        size_t argc = build_args(args, slots, KB_MAX_ARGS, NULL);
        int r = kb_query(kb, pred, slots, argc);
        snprintf(out, outsz, "{\"provable\":%s}", r ? "true" : "false");
        return 1;
    }
    if (strcmp(name, "kb.match") == 0) {
        const char *pred; const JVal *args;
        if (!need_pred_args(a, &pred, &args, out, outsz)) return 0;
        int hv = 0;
        size_t argc = build_args(args, slots, KB_MAX_ARGS, &hv);
        char binds[64][KB_TERM_LEN];
        size_t nb = kb_match(kb, pred, slots, argc, binds, 64);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"bindings\":[");
        for (size_t i = 0; i < nb; i++) {
            if (i) sb_putc(&s, ',');
            char dbuf[KB_TERM_LEN];
            sb_jstr(&s, lit_decode(binds[i], dbuf, sizeof dbuf));   /* U1: strip .p0 quotes on the way out */
        }
        sb_puts(&s, "]}");
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "kb.explain") == 0) {
        const char *pred; const JVal *args;
        if (!need_pred_args(a, &pred, &args, out, outsz)) return 0;
        size_t argc = build_args(args, slots, KB_MAX_ARGS, NULL);
        char expl[512];
        int r = kb_explain(kb, pred, slots, argc, expl, sizeof expl);
        if (r) {
            SB s; sb_init(&s);
            sb_puts(&s, "{\"provable\":true,\"explanation\":");
            sb_jstr(&s, expl); sb_putc(&s, '}');
            snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
            sb_free(&s);
        } else {
            snprintf(out, outsz, "{\"provable\":false}");
        }
        return 1;
    }
    if (strcmp(name, "kb.describe") == 0) {
        const char *entity = jstr(a, "entity");
        if (!entity) { snprintf(out, outsz, "{\"error\":\"missing 'entity'\"}"); return 0; }
        char desc[1024];
        int r = kb_describe_entity(kb, entity, desc, sizeof desc);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"known\":"); sb_puts(&s, r ? "true" : "false");
        sb_puts(&s, ",\"text\":"); sb_jstr(&s, r ? desc : ""); sb_putc(&s, '}');
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "kb.dump") == 0) {
        static char dump[65536];
        int r = kb_dump_all(kb, dump, sizeof dump);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"dump\":"); sb_jstr(&s, r ? dump : ""); sb_putc(&s, '}');
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "kb.induce") == 0) {
        JVal *ms = jobj_get(a, "min_support");
        size_t min_support = (ms && ms->type == J_NUM && ms->num >= 1)
                             ? (size_t)ms->num : 1;
        char heads[32][KB_TERM_LEN], bodies[32][KB_TERM_LEN];
        size_t n = kb_induce(kb, min_support, heads, bodies, 32);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"induced\":[");
        for (size_t i = 0; i < n; i++) {
            if (i) sb_putc(&s, ',');
            sb_puts(&s, "{\"head\":"); sb_jstr(&s, heads[i]);
            sb_puts(&s, ",\"body\":"); sb_jstr(&s, bodies[i]); sb_putc(&s, '}');
        }
        sb_puts(&s, "]}");
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "kb.stats") == 0) {
        snprintf(out, outsz, "{\"facts\":%zu}", kb_size(kb));
        return 1;
    }
    if (strcmp(name, "kb.save") == 0) {
        const char *path = jstr(a, "path");
        if (!path) path = "kb/core/session.p0";
        if (!path_ok(path)) { snprintf(out, outsz, "{\"error\":\"unsafe path\"}"); return 0; }
        int n = brain_save_session(b, path);
        if (n < 0) { snprintf(out, outsz, "{\"error\":\"save failed\"}"); return 0; }
        SB s; sb_init(&s);
        sb_puts(&s, "{\"ok\":true,\"path\":"); sb_jstr(&s, path);
        char num[32]; snprintf(num, sizeof num, ",\"written\":%d}", n);
        sb_puts(&s, num);
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "kb.restore") == 0) {
        int n = brain_reload(b);
        if (n < 0) { snprintf(out, outsz, "{\"error\":\"restore failed\"}"); return 0; }
        snprintf(out, outsz, "{\"ok\":true,\"clauses\":%d}", n);
        return 1;
    }
    if (strcmp(name, "gen.respond") == 0) {
        const char *input = jstr(a, "input");
        if (!input) { snprintf(out, outsz, "{\"error\":\"missing 'input'\"}"); return 0; }
        static char reply[8192];
        brain_respond(b, input, reply, sizeof reply);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"output\":"); sb_jstr(&s, reply); sb_putc(&s, '}');
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "text.extract") == 0) {
        const char *passage = jstr(a, "passage");
        if (!passage) { snprintf(out, outsz, "{\"error\":\"missing 'passage'\"}"); return 0; }
        /* the real reader path (mod_reader) — facts land in the KB for real. */
        static char turn[8192], reply[8192];
        snprintf(turn, sizeof turn, "read: %s", passage);
        brain_respond(b, turn, reply, sizeof reply);
        SB s; sb_init(&s);
        sb_puts(&s, "{\"result\":"); sb_jstr(&s, reply); sb_putc(&s, '}');
        snprintf(out, outsz, "%s", s.oom ? "{\"error\":\"oom\"}" : s.buf);
        sb_free(&s);
        return 1;
    }
    if (strcmp(name, "style.set_temperature") == 0) {
        JVal *v = jobj_get(a, "value");
        if (!v || v->type != J_NUM) { snprintf(out, outsz, "{\"error\":\"missing numeric 'value'\"}"); return 0; }
        char val[32];
        snprintf(val, sizeof val, "%lld", (long long)v->num);
        /* replace, don't accumulate: clear any existing style_temperature/1 */
        char cur[8][KB_TERM_LEN];
        const char *q[1] = { NULL };
        size_t nc = kb_match(kb, "style_temperature", q, 1, cur, 8);
        for (size_t i = 0; i < nc; i++) {
            const char *ra[1] = { cur[i] };
            kb_retract(kb, "style_temperature", ra, 1);
        }
        kb_set_origin(kb, KB_SESSION);
        const char *na[1] = { val };
        kb_assert(kb, "style_temperature", na, 1);
        snprintf(out, outsz, "{\"ok\":true,\"temperature\":%s}", val);
        return 1;
    }
    if (strcmp(name, "style.get_temperature") == 0) {
        char cur[1][KB_TERM_LEN];
        const char *q[1] = { NULL };
        size_t nc = kb_match(kb, "style_temperature", q, 1, cur, 1);
        if (nc == 1) snprintf(out, outsz, "{\"temperature\":%s}", cur[0]);
        else snprintf(out, outsz, "{\"temperature\":null}");
        return 1;
    }

    snprintf(out, outsz, "{\"error\":\"unknown tool\"}");
    return -1;   /* signals "no such tool" to the dispatcher */
}

/* ------------------------------------------------------------------------- */
/* method dispatch                                                            */
/* ------------------------------------------------------------------------- */
static void handle_initialize(const JVal *id, const JVal *params) {
    const char *proto = MCP_PROTO;
    JVal *pv = jobj_get(params, "protocolVersion");
    if (pv && pv->type == J_STR && pv->str[0]) proto = pv->str;
    SB s; sb_init(&s);
    sb_puts(&s, "{\"protocolVersion\":"); sb_jstr(&s, proto);
    sb_puts(&s, ",\"capabilities\":{\"tools\":{}},\"serverInfo\":{\"name\":\"parrot0\",");
    sb_puts(&s, "\"version\":"); sb_jstr(&s, brain_version()); sb_puts(&s, "}}");
    emit_result(id, s.oom ? "{}" : s.buf);
    sb_free(&s);
}

static void handle_tools_list(const JVal *id) {
    SB s; sb_init(&s);
    sb_puts(&s, "{\"tools\":[");
    for (size_t i = 0; i < NTOOLS; i++) {
        if (i) sb_putc(&s, ',');
        sb_puts(&s, "{\"name\":"); sb_jstr(&s, TOOLS[i].name);
        sb_puts(&s, ",\"description\":"); sb_jstr(&s, TOOLS[i].desc);
        sb_puts(&s, ",\"inputSchema\":"); sb_puts(&s, TOOLS[i].schema);
        sb_putc(&s, '}');
    }
    sb_puts(&s, "]}");
    emit_result(id, s.oom ? "{\"tools\":[]}" : s.buf);
    sb_free(&s);
}

static void handle_tools_call(Brain *b, const JVal *id, const JVal *params) {
    const char *name = jstr(params, "name");
    if (!name) { emit_error(id, -32602, "missing tool name"); return; }
    JVal *args = jobj_get(params, "arguments");
    static char payload[70000];
    int rc = tool_call(b, name, args, payload, sizeof payload);
    if (rc < 0) { emit_error(id, -32602, "unknown tool"); return; }
    emit_tool(id, payload, rc == 0);   /* rc==0 => tool-level error payload */
}

static void dispatch(Brain *b, const char *method, const JVal *params,
                     const JVal *id) {
    if (strcmp(method, "initialize") == 0) {
        handle_initialize(id, params);
    } else if (strcmp(method, "tools/list") == 0) {
        handle_tools_list(id);
    } else if (strcmp(method, "tools/call") == 0) {
        handle_tools_call(b, id, params);
    } else if (strcmp(method, "ping") == 0) {
        emit_result(id, "{}");
    } else if (strncmp(method, "notifications/", 14) == 0) {
        /* notifications carry no id and expect no response */
    } else if (id) {
        emit_error(id, -32601, "method not found");
    }
}

/* ------------------------------------------------------------------------- */
/* the stdio loop                                                             */
/* ------------------------------------------------------------------------- */
int mcp_serve_stdio(Brain *brain) {
    static char line[MCP_LINECAP];
    fprintf(stderr, "parrot0 [%s] mcp-engine: JSON-RPC on stdio, %zu tools\n",
            brain_version(), NTOOLS);
    fflush(stderr);

    while (fgets(line, sizeof line, stdin)) {
        size_t len = strlen(line);
        if (len == 0) continue;
        JVal *req = json_parse(line, len);
        if (!req || req->type != J_OBJ) {
            jfree(req);
            emit_error(NULL, -32700, "parse error");
            continue;
        }
        JVal *method = jobj_get(req, "method");
        JVal *params = jobj_get(req, "params");
        JVal *id     = jobj_get(req, "id");
        if (method && method->type == J_STR)
            dispatch(brain, method->str, params, id);
        else if (id)
            emit_error(id, -32600, "invalid request");
        jfree(req);
    }
    return 0;
}