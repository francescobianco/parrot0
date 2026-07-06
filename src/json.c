/*
 * json.c - tiny tree JSON parser + string escaper (see json.h).
 *
 * Moved verbatim out of src/serve.c at gen277 so both the HTTP OpenAI server and
 * the MCP JSON-RPC engine link one implementation. Behaviour is unchanged.
 */
#define _POSIX_C_SOURCE 200809L

#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { const char *p, *end; } JParser;

static JVal *jparse_value(JParser *j);

void jfree(JVal *v) {
    if (!v) return;
    free(v->str);
    for (size_t i = 0; i < v->n; i++) {
        jfree(v->items[i]);
        if (v->keys) free(v->keys[i]);
    }
    free(v->items);
    free(v->keys);
    free(v);
}

static void jskip_ws(JParser *j) {
    while (j->p < j->end &&
           (*j->p == ' ' || *j->p == '\t' || *j->p == '\n' || *j->p == '\r'))
        j->p++;
}

/* Append one Unicode code point to a growing UTF-8 buffer. */
static int utf8_emit(char **buf, size_t *len, size_t *cap, unsigned cp) {
    if (*len + 4 >= *cap) {
        size_t nc = *cap ? *cap * 2 : 32;
        char *g = realloc(*buf, nc);
        if (!g) return 0;
        *buf = g; *cap = nc;
    }
    char *o = *buf + *len;
    if (cp < 0x80) { *o++ = (char)cp; }
    else if (cp < 0x800) {
        *o++ = (char)(0xC0 | (cp >> 6));
        *o++ = (char)(0x80 | (cp & 0x3F));
    } else {
        *o++ = (char)(0xE0 | (cp >> 12));
        *o++ = (char)(0x80 | ((cp >> 6) & 0x3F));
        *o++ = (char)(0x80 | (cp & 0x3F));
    }
    *len = (size_t)(o - *buf);
    return 1;
}

static char *jparse_string_raw(JParser *j) {
    if (j->p >= j->end || *j->p != '"') return NULL;
    j->p++;
    char *buf = NULL; size_t len = 0, cap = 0;
    while (j->p < j->end && *j->p != '"') {
        unsigned char c = (unsigned char)*j->p++;
        if (c == '\\' && j->p < j->end) {
            char e = *j->p++;
            switch (e) {
                case 'n': c = '\n'; break;
                case 't': c = '\t'; break;
                case 'r': c = '\r'; break;
                case 'b': c = '\b'; break;
                case 'f': c = '\f'; break;
                case '/': c = '/';  break;
                case '\\': c = '\\'; break;
                case '"': c = '"';  break;
                case 'u': {
                    unsigned cp = 0;
                    for (int k = 0; k < 4 && j->p < j->end; k++) {
                        char h = *j->p++;
                        cp <<= 4;
                        if (h >= '0' && h <= '9') cp |= (unsigned)(h - '0');
                        else if (h >= 'a' && h <= 'f') cp |= (unsigned)(h - 'a' + 10);
                        else if (h >= 'A' && h <= 'F') cp |= (unsigned)(h - 'A' + 10);
                    }
                    if (!utf8_emit(&buf, &len, &cap, cp)) { free(buf); return NULL; }
                    continue;
                }
                default: c = (unsigned char)e; break;
            }
        }
        if (!utf8_emit(&buf, &len, &cap, c)) { free(buf); return NULL; }
    }
    if (j->p < j->end && *j->p == '"') j->p++;
    if (!buf) { buf = malloc(1); if (!buf) return NULL; }
    buf[len] = '\0';
    return buf;
}

static JVal *jnew(JType t) {
    JVal *v = calloc(1, sizeof *v);
    if (v) v->type = t;
    return v;
}

static int jpush(JVal *arr, JVal *child, char *key) {
    JVal **gi = realloc(arr->items, (arr->n + 1) * sizeof *gi);
    if (!gi) return 0;
    arr->items = gi;
    if (key) {
        char **gk = realloc(arr->keys, (arr->n + 1) * sizeof *gk);
        if (!gk) return 0;
        arr->keys = gk;
        arr->keys[arr->n] = key;
    }
    arr->items[arr->n] = child;
    arr->n++;
    return 1;
}

static JVal *jparse_value(JParser *j) {
    jskip_ws(j);
    if (j->p >= j->end) return NULL;
    char c = *j->p;
    if (c == '"') {
        char *s = jparse_string_raw(j);
        if (!s) return NULL;
        JVal *v = jnew(J_STR);
        if (!v) { free(s); return NULL; }
        v->str = s;
        return v;
    }
    if (c == '{') {
        j->p++;
        JVal *v = jnew(J_OBJ);
        if (!v) return NULL;
        jskip_ws(j);
        if (j->p < j->end && *j->p == '}') { j->p++; return v; }
        for (;;) {
            jskip_ws(j);
            char *key = jparse_string_raw(j);
            if (!key) { jfree(v); return NULL; }
            jskip_ws(j);
            if (j->p >= j->end || *j->p != ':') { free(key); jfree(v); return NULL; }
            j->p++;
            JVal *child = jparse_value(j);
            if (!child) { free(key); jfree(v); return NULL; }
            if (!jpush(v, child, key)) { free(key); jfree(child); jfree(v); return NULL; }
            jskip_ws(j);
            if (j->p < j->end && *j->p == ',') { j->p++; continue; }
            if (j->p < j->end && *j->p == '}') { j->p++; break; }
            jfree(v); return NULL;
        }
        return v;
    }
    if (c == '[') {
        j->p++;
        JVal *v = jnew(J_ARR);
        if (!v) return NULL;
        jskip_ws(j);
        if (j->p < j->end && *j->p == ']') { j->p++; return v; }
        for (;;) {
            JVal *child = jparse_value(j);
            if (!child) { jfree(v); return NULL; }
            if (!jpush(v, child, NULL)) { jfree(child); jfree(v); return NULL; }
            jskip_ws(j);
            if (j->p < j->end && *j->p == ',') { j->p++; continue; }
            if (j->p < j->end && *j->p == ']') { j->p++; break; }
            jfree(v); return NULL;
        }
        return v;
    }
    if (c == 't' || c == 'f') {
        int istrue = (c == 't');
        const char *lit = istrue ? "true" : "false";
        size_t ll = strlen(lit);
        if ((size_t)(j->end - j->p) < ll || strncmp(j->p, lit, ll) != 0) return NULL;
        j->p += ll;
        JVal *v = jnew(J_BOOL);
        if (v) v->b = istrue;
        return v;
    }
    if (c == 'n') {
        if ((size_t)(j->end - j->p) < 4 || strncmp(j->p, "null", 4) != 0) return NULL;
        j->p += 4;
        return jnew(J_NULL);
    }
    /* number */
    char *endp = NULL;
    double d = strtod(j->p, &endp);
    if (endp == j->p) return NULL;
    j->p = endp;
    JVal *v = jnew(J_NUM);
    if (v) v->num = d;
    return v;
}

JVal *json_parse(const char *s, size_t n) {
    JParser j = { s, s + n };
    return jparse_value(&j);
}

JVal *jobj_get(const JVal *o, const char *key) {
    if (!o || o->type != J_OBJ) return NULL;
    for (size_t i = 0; i < o->n; i++)
        if (o->keys[i] && strcmp(o->keys[i], key) == 0) return o->items[i];
    return NULL;
}

char *json_escape(const char *s) {
    size_t cap = strlen(s) * 2 + 16, len = 0;
    char *o = malloc(cap);
    if (!o) return NULL;
    for (const char *p = s; *p; p++) {
        unsigned char c = (unsigned char)*p;
        const char *esc = NULL; char tmp[8];
        switch (c) {
            case '"':  esc = "\\\""; break;
            case '\\': esc = "\\\\"; break;
            case '\n': esc = "\\n";  break;
            case '\r': esc = "\\r";  break;
            case '\t': esc = "\\t";  break;
            case '\b': esc = "\\b";  break;
            case '\f': esc = "\\f";  break;
            default:
                if (c < 0x20) { snprintf(tmp, sizeof tmp, "\\u%04x", c); esc = tmp; }
        }
        size_t add = esc ? strlen(esc) : 1;
        if (len + add + 1 >= cap) {
            cap = (len + add + 1) * 2;
            char *g = realloc(o, cap);
            if (!g) { free(o); return NULL; }
            o = g;
        }
        if (esc) { memcpy(o + len, esc, add); len += add; }
        else o[len++] = (char)c;
    }
    o[len] = '\0';
    return o;
}