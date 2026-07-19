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
#include "json.h"

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
/* main.c hands brain_respond a NUL-terminated buffer of 65536 bytes, and the
 * Brain's raw-input code paths use the same capacity.  The HTTP adapter must
 * therefore preserve every prompt byte through this inclusive surface limit,
 * and reject larger prompts atomically instead of feeding a plausible prefix. */
#define PROMPT_MAX_LEN (64 * 1024 - 1)

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

/* Preserve the user's text stream byte structure.  Collapsing newlines here
 * destroys indentation, fences, tracebacks and pytest/diff line prefixes before
 * the universal segmenter can see them. Normalize CRLF/CR to LF in place and
 * return the exact number of bytes that would be handed to brain_respond. */
static size_t sanitize_prompt(char *p) {
    if (!p) return 0;
    char *r = p, *w = p;
    while (*r) {
        if (*r == '\r') {
            if (r[1] == '\n') r++;
            *w++ = '\n'; r++;
        } else {
            *w++ = *r++;
        }
    }
    *w = '\0';
    return (size_t)(w - p);
}

/* Same stable 64-bit digest used by the execution kernel.  It identifies the
 * complete rejected prompt without logging or echoing potentially sensitive
 * content, and is deliberately computed before any Brain state can change. */
static void prompt_digest(const char *data, size_t n, char out[17]) {
    unsigned long long h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; i++) {
        h ^= (unsigned char)data[i];
        h *= 1099511628211ULL;
    }
    snprintf(out, 17, "%016llx", h);
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
    size_t prompt_len = sanitize_prompt(prompt);
    if (prompt_len > PROMPT_MAX_LEN) {
        char digest[17], error[256];
        prompt_digest(prompt, prompt_len, digest);
        snprintf(error, sizeof error,
                 "{\"error\":{\"code\":\"input_too_large\",\"bytes\":%zu,"
                 "\"limit\":%d,\"digest\":\"fnv1a64:%s\"}}",
                 prompt_len, PROMPT_MAX_LEN, digest);
        serve_log("input_too_large bytes=%zu limit=%d digest=fnv1a64:%s",
                  prompt_len, PROMPT_MAX_LEN, digest);
        free(prompt);
        jfree(root);
        send_json(fd, 413, "Payload Too Large", error);
        return;
    }

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
