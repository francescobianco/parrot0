/*
 * json.h - a tiny tree JSON parser + string escaper, shared by every host that
 * speaks JSON (src/serve.c's OpenAI HTTP API, src/mcp.c's MCP JSON-RPC engine).
 *
 * Promoted out of serve.c at gen277 so the MCP engine reuses the exact same
 * parser instead of duplicating it. Scope is deliberately small: enough of JSON
 * for the request shapes these servers see (objects, arrays, strings with the
 * usual escapes + \uXXXX, numbers, booleans, null). Not a validator.
 */
#ifndef PARROT0_JSON_H
#define PARROT0_JSON_H

#include <stddef.h>

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

/* Parse `n` bytes of JSON text into a tree, or NULL on malformed input. The
 * caller owns the result and must free it with jfree. */
JVal *json_parse(const char *s, size_t n);

/* Free a tree returned by json_parse (NULL-safe). */
void  jfree(JVal *v);

/* For a J_OBJ, return the value stored under `key`, or NULL. */
JVal *jobj_get(const JVal *o, const char *key);

/* Escape `s` for embedding as a JSON string body (no surrounding quotes). Returns
 * a malloc'd string the caller frees, or NULL on OOM. */
char *json_escape(const char *s);

#endif /* PARROT0_JSON_H */