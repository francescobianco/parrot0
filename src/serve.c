/*
 * serve.c - parrot0's built-in OpenAI-compatible HTTP server (gen221).
 *
 * A pure-C port of what scripts/pi_server.py used to do: expose parrot0 as an
 * OpenAI /v1/chat/completions endpoint so a coding agent can use it as a model.
 * No external process, no Python. See serve.h for the contract.
 *
 * Scope is deliberately small and self-contained:
 *   - a tiny tree JSON parser (enough for the OpenAI request shape),
 *   - a blocking, iterative HTTP/1.1 request reader,
 *   - the three routes pi needs: POST /v1/chat/completions, GET /v1/models,
 *     GET /health.
 * The brain is stateful, so connections are handled one at a time.
 */
#define _POSIX_C_SOURCE 200809L

#include "serve.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define RESP_MAX_LEN 8192          /* matches main.c's brain_respond buffer */
#define REQ_MAX_LEN  (4 * 1024 * 1024) /* refuse absurd requests */

/* ------------------------------------------------------------------------- */
/* optional traffic log (PARROT0_PI_LOG), mirroring pi_server.py's _log       */
/* ------------------------------------------------------------------------- */
static void serve_log(const char *fmt, ...) {
    const char *path = getenv("PARROT0_PI_LOG");
    if (!path || !*path) return;
    FILE *f = fopen(path, "a");
    if (!f) return;
    va_list ap; va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fclose(f);
}

/* ------------------------------------------------------------------------- */
/* minimal JSON parser (tree)                                                 */
/* ------------------------------------------------------------------------- */
typedef enum { J_NULL, J_BOOL, J_NUM, J_STR, J_ARR, J_OBJ } JType;

typedef struct JVal {
    JType  type;
    int    b;            /* J_BOOL */
    double num;          /* J_NUM */
    char  *str;          /* J_STR (decoded, malloc'd) */
    struct JVal **items; /* J_ARR / J_OBJ element values */
    char **keys;         /* J_OBJ keys (decoded), parallel to items */
    size_t n;            /* element count */
} JVal;

typedef struct { const char *p, *end; } JParser;

static JVal *jparse_value(JParser *j);

static void jfree(JVal *v) {
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

static JVal *json_parse(const char *s, size_t n) {
    JParser j = { s, s + n };
    JVal *v = jparse_value(&j);
    return v;
}

static JVal *jobj_get(const JVal *o, const char *key) {
    if (!o || o->type != J_OBJ) return NULL;
    for (size_t i = 0; i < o->n; i++)
        if (o->keys[i] && strcmp(o->keys[i], key) == 0) return o->items[i];
    return NULL;
}

/* ------------------------------------------------------------------------- */
/* OpenAI message -> parrot0 prompt                                           */
/* ------------------------------------------------------------------------- */

/* A message's "content" is either a plain string or an array of
 * {type:"text", text:"..."} parts. Join the text parts (mirrors pi_server.py's
 * _content_text). Returns a malloc'd string the caller frees. */
static char *content_text(const JVal *content) {
    if (!content) return strdup("");
    if (content->type == J_STR) return strdup(content->str ? content->str : "");
    if (content->type == J_ARR) {
        size_t cap = 64, len = 0;
        char *buf = malloc(cap);
        if (!buf) return strdup("");
        buf[0] = '\0';
        for (size_t i = 0; i < content->n; i++) {
            JVal *part = content->items[i];
            JVal *t = jobj_get(part, "type");
            JVal *txt = jobj_get(part, "text");
            if (t && t->type == J_STR && strcmp(t->str, "text") == 0 &&
                txt && txt->type == J_STR) {
                size_t add = strlen(txt->str);
                while (len + add + 2 >= cap) {
                    size_t nc = cap * 2;
                    char *g = realloc(buf, nc);
                    if (!g) { free(buf); return strdup(""); }
                    buf = g; cap = nc;
                }
                if (len) buf[len++] = ' ';
                memcpy(buf + len, txt->str, add);
                len += add;
                buf[len] = '\0';
            }
        }
        return buf;
    }
    return strdup("");
}

/* Build the single prompt line parrot0 answers, from the OpenAI messages array.
 * Default (like pi_server.py) is the LAST user message only; with
 * PARROT0_PI_INCLUDE_SYSTEM=1 a leading "[Instructions: <system>] " is prepended.
 * Returns a malloc'd string (caller frees), or NULL if there is no user turn. */
static char *build_prompt(const JVal *messages) {
    if (!messages || messages->type != J_ARR) return NULL;
    char *system_prefix = NULL;     /* first system message text */
    char *last_user = NULL;
    for (size_t i = 0; i < messages->n; i++) {
        JVal *m = messages->items[i];
        JVal *role = jobj_get(m, "role");
        const char *r = (role && role->type == J_STR) ? role->str : "user";
        char *txt = content_text(jobj_get(m, "content"));
        if (strcmp(r, "system") == 0) {
            if (!system_prefix) system_prefix = txt; else free(txt);
        } else if (strcmp(r, "user") == 0) {
            free(last_user); last_user = txt;
        } else {
            free(txt);
        }
    }
    char *prompt = NULL;
    if (!last_user) { free(system_prefix); return NULL; }
    const char *inc = getenv("PARROT0_PI_INCLUDE_SYSTEM");
    if (inc && strcmp(inc, "1") == 0 && system_prefix) {
        size_t n = strlen(system_prefix) + strlen(last_user) + 32;
        prompt = malloc(n);
        if (prompt) snprintf(prompt, n, "[Instructions: %s] %s", system_prefix, last_user);
        free(last_user);
    } else {
        prompt = last_user;
    }
    free(system_prefix);
    return prompt;
}

/* parrot0 is line-based: collapse newlines to spaces and cap the length, in
 * place. Mirrors pi_server.py's prompt sanitation. */
static void sanitize_prompt(char *p) {
    if (!p) return;
    for (char *q = p; *q; q++) if (*q == '\n' || *q == '\r') *q = ' ';
    if (strlen(p) > 2000) p[2000] = '\0';
}

/* ------------------------------------------------------------------------- */
/* JSON string escaping for output                                            */
/* ------------------------------------------------------------------------- */
static char *json_escape(const char *s) {
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

/* ------------------------------------------------------------------------- */
/* socket I/O helpers                                                         */
/* ------------------------------------------------------------------------- */
static int write_all(int fd, const char *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t w = write(fd, buf + off, len - off);
        if (w < 0) { if (errno == EINTR) continue; return -1; }
        off += (size_t)w;
    }
    return 0;
}

static void send_simple(int fd, int code, const char *status,
                        const char *ctype, const char *body, size_t blen) {
    char head[256];
    int hn = snprintf(head, sizeof head,
                      "HTTP/1.1 %d %s\r\n"
                      "Content-Type: %s\r\n"
                      "Content-Length: %zu\r\n"
                      "Connection: close\r\n\r\n",
                      code, status, ctype, blen);
    if (hn < 0) return;
    write_all(fd, head, (size_t)hn);
    if (body && blen) write_all(fd, body, blen);
}

static void send_json(int fd, int code, const char *status, const char *body) {
    send_simple(fd, code, status, "application/json", body, strlen(body));
}

/* ------------------------------------------------------------------------- */
/* request handling                                                           */
/* ------------------------------------------------------------------------- */

/* Read a whole HTTP request: headers up to the blank line, then Content-Length
 * body bytes. Returns a malloc'd NUL-terminated buffer and sets *body_off to the
 * byte offset where the body begins, or NULL on error. */
static char *read_request(int fd, size_t *total, size_t *body_off) {
    size_t cap = 8192, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;

    size_t header_end = 0;
    for (;;) {
        if (len + 1 >= cap) {
            if (cap >= REQ_MAX_LEN) { free(buf); return NULL; }
            cap *= 2;
            char *g = realloc(buf, cap);
            if (!g) { free(buf); return NULL; }
            buf = g;
        }
        ssize_t r = read(fd, buf + len, cap - len - 1);
        if (r < 0) { if (errno == EINTR) continue; free(buf); return NULL; }
        if (r == 0) break;
        len += (size_t)r;
        buf[len] = '\0';
        char *he = strstr(buf, "\r\n\r\n");
        if (he) { header_end = (size_t)(he - buf) + 4; break; }
    }
    if (header_end == 0) { free(buf); return NULL; }

    /* Content-Length (case-insensitive scan of the header block). */
    size_t content_len = 0;
    for (char *p = buf; p < buf + header_end; p++) {
        if ((p[0] == 'C' || p[0] == 'c') &&
            strncasecmp(p, "content-length:", 15) == 0) {
            content_len = (size_t)strtoul(p + 15, NULL, 10);
            break;
        }
    }

    while (len < header_end + content_len) {
        if (len + 1 >= cap) {
            if (cap >= REQ_MAX_LEN) { free(buf); return NULL; }
            cap *= 2;
            char *g = realloc(buf, cap);
            if (!g) { free(buf); return NULL; }
            buf = g;
        }
        ssize_t r = read(fd, buf + len, cap - len - 1);
        if (r < 0) { if (errno == EINTR) continue; free(buf); return NULL; }
        if (r == 0) break;
        len += (size_t)r;
        buf[len] = '\0';
    }

    *total = len;
    *body_off = header_end;
    return buf;
}

static const char *MODELS_JSON =
    "{\"object\":\"list\",\"data\":[{\"id\":\"parrot0\",\"object\":\"model\","
    "\"created\":0,\"owned_by\":\"parrot0\"}]}";

static void handle_chat(int fd, Brain *brain, const char *body, size_t blen) {
    JVal *root = json_parse(body, blen);
    if (!root || root->type != J_OBJ) {
        jfree(root);
        send_json(fd, 400, "Bad Request", "{\"error\":\"invalid json\"}");
        return;
    }

    JVal *model_v = jobj_get(root, "model");
    const char *model = (model_v && model_v->type == J_STR) ? model_v->str : "parrot0";
    JVal *stream_v = jobj_get(root, "stream");
    int streaming = (stream_v && stream_v->type == J_BOOL && stream_v->b);

    char *prompt = build_prompt(jobj_get(root, "messages"));
    sanitize_prompt(prompt);

    char resp[RESP_MAX_LEN];
    resp[0] = '\0';
    if (prompt && *prompt) {
        serve_log(">>> parrot0 stdin: %.200s", prompt);
        brain_respond(brain, prompt, resp, sizeof resp);
        serve_log("<<< parrot0 stdout: %.200s", resp);
    }

    char *esc = json_escape(resp);
    char *esc_model = json_escape(model);
    long now = (long)time(NULL);
    if (!esc || !esc_model) {
        free(esc); free(esc_model); free(prompt); jfree(root);
        send_json(fd, 500, "Internal Server Error", "{\"error\":\"oom\"}");
        return;
    }

    if (!streaming) {
        size_t cap = strlen(esc) + strlen(esc_model) + 512;
        char *out = malloc(cap);
        if (out) {
            snprintf(out, cap,
                "{\"id\":\"parrot0-%ld\",\"object\":\"chat.completion\","
                "\"created\":%ld,\"model\":\"%s\",\"choices\":[{\"index\":0,"
                "\"message\":{\"role\":\"assistant\",\"content\":\"%s\"},"
                "\"finish_reason\":\"stop\"}],"
                "\"usage\":{\"prompt_tokens\":0,\"completion_tokens\":0,"
                "\"total_tokens\":0}}",
                now * 1000, now, esc_model, esc);
            send_json(fd, 200, "OK", out);
            free(out);
        } else {
            send_json(fd, 500, "Internal Server Error", "{\"error\":\"oom\"}");
        }
    } else {
        /* Server-Sent Events: role chunk, content chunk, stop, [DONE]. */
        char head[256];
        int hn = snprintf(head, sizeof head,
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/event-stream\r\n"
                          "Cache-Control: no-cache\r\n"
                          "Connection: close\r\n\r\n");
        write_all(fd, head, (size_t)hn);
        size_t cap = strlen(esc) + strlen(esc_model) + 512;
        char *chunk = malloc(cap);
        if (chunk) {
            int n;
            n = snprintf(chunk, cap,
                "data: {\"id\":\"parrot0-%ld\",\"object\":\"chat.completion.chunk\","
                "\"created\":%ld,\"model\":\"%s\",\"choices\":[{\"index\":0,"
                "\"delta\":{\"role\":\"assistant\"},\"finish_reason\":null}]}\n\n",
                now * 1000, now, esc_model);
            write_all(fd, chunk, (size_t)n);
            n = snprintf(chunk, cap,
                "data: {\"id\":\"parrot0-%ld\",\"object\":\"chat.completion.chunk\","
                "\"created\":%ld,\"model\":\"%s\",\"choices\":[{\"index\":0,"
                "\"delta\":{\"content\":\"%s\"},\"finish_reason\":null}]}\n\n",
                now * 1000, now, esc_model, esc);
            write_all(fd, chunk, (size_t)n);
            n = snprintf(chunk, cap,
                "data: {\"id\":\"parrot0-%ld\",\"object\":\"chat.completion.chunk\","
                "\"created\":%ld,\"model\":\"%s\",\"choices\":[{\"index\":0,"
                "\"delta\":{},\"finish_reason\":\"stop\"}]}\n\n",
                now * 1000, now, esc_model);
            write_all(fd, chunk, (size_t)n);
            write_all(fd, "data: [DONE]\n\n", 14);
            free(chunk);
        }
    }

    free(esc); free(esc_model); free(prompt); jfree(root);
}

static void handle_conn(int cfd, Brain *brain) {
    size_t total = 0, body_off = 0;
    char *req = read_request(cfd, &total, &body_off);
    if (!req) return;

    /* request line: METHOD SP PATH SP HTTP/x */
    char method[16] = "", path[512] = "";
    sscanf(req, "%15s %511s", method, path);

    if (strcmp(method, "POST") == 0 && strcmp(path, "/v1/chat/completions") == 0) {
        handle_chat(cfd, brain, req + body_off, total - body_off);
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/v1/models") == 0) {
        send_json(cfd, 200, "OK", MODELS_JSON);
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/health") == 0) {
        send_json(cfd, 200, "OK", "{\"status\":\"ok\"}");
    } else {
        send_json(cfd, 404, "Not Found", "{\"error\":\"not found\"}");
    }
    free(req);
}

int serve_http(Brain *brain, const char *host, int port) {
    signal(SIGPIPE, SIG_IGN);   /* a client disconnect must not kill the server */

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { perror("parrot0 --daemon: socket"); return 2; }
    int one = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    if (!host || !*host) host = "127.0.0.1";
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        fprintf(stderr, "parrot0 --daemon: bad host '%s'\n", host);
        close(sfd); return 2;
    }
    if (bind(sfd, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("parrot0 --daemon: bind"); close(sfd); return 2;
    }
    if (listen(sfd, 16) < 0) {
        perror("parrot0 --daemon: listen"); close(sfd); return 2;
    }

    fprintf(stderr,
            "parrot0 [%s] daemon: listening on http://%s:%d/v1/chat/completions\n",
            brain_version(), host, port);
    fflush(stderr);

    for (;;) {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd < 0) { if (errno == EINTR) continue; break; }
        handle_conn(cfd, brain);
        close(cfd);
    }
    close(sfd);
    return 0;
}
