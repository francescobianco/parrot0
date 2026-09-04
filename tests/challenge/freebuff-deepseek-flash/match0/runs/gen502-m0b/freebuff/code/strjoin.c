/* strjoin.c — backend for strjoin.h. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "strjoin.h"

char *str_join(const char **parts, size_t count, const char *sep) {
    /* count == 0: a malloc'd empty string, never reading parts or sep. */
    if (count == 0) {
        char *empty = malloc(1);
        if (empty)
            empty[0] = '\0';
        return empty;
    }

    if (sep == NULL || parts == NULL)
        return NULL;

    size_t sep_len = strlen(sep);

    /* total length: all parts, a separator between each pair, plus the NUL. */
    size_t total = 1;
    for (size_t i = 0; i < count; i++) {
        if (parts[i] == NULL)
            return NULL;
        size_t len = strlen(parts[i]);
        if (len > SIZE_MAX - total)
            return NULL; /* size_t overflow */
        total += len;
        if (i + 1 < count) {
            if (sep_len > SIZE_MAX - total)
                return NULL; /* size_t overflow */
            total += sep_len;
        }
    }

    char *out = malloc(total);
    if (out == NULL)
        return NULL;

    char *p = out;
    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            memcpy(p, sep, sep_len);
            p += sep_len;
        }
        size_t len = strlen(parts[i]);
        memcpy(p, parts[i], len);
        p += len;
    }
    *p = '\0';
    return out;
}
