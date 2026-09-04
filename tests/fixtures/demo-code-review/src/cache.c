/* A tiny in-memory cache: a hash table with linear probing. */
#include <string.h>
#include <stdlib.h>

#define SLOTS 512

static char *keys[SLOTS];
static int   vals[SLOTS];

static unsigned hash_table_slot(const char *key) {
    unsigned h = 2166136261u;
    while (*key) { h ^= (unsigned char)*key++; h *= 16777619u; }
    return h % SLOTS;
}

int hash_table_get(const char *key, int *out) {
    unsigned i = hash_table_slot(key);
    for (unsigned n = 0; n < SLOTS; n++) {
        unsigned p = (i + n) % SLOTS;
        if (!keys[p]) return 0;
        if (!strcmp(keys[p], key)) { *out = vals[p]; return 1; }
    }
    return 0;
}

int hash_table_put(const char *key, int val) {
    unsigned i = hash_table_slot(key);
    for (unsigned n = 0; n < SLOTS; n++) {
        unsigned p = (i + n) % SLOTS;
        if (!keys[p]) { keys[p] = strdup(key); vals[p] = val; return 1; }
        if (!strcmp(keys[p], key)) { vals[p] = val; return 1; }
    }
    return 0;
}
