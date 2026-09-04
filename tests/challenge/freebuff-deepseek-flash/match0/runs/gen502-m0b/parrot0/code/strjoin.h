/* strjoin.h — the contract.  Supplied; do not rename or weaken it. */
#ifndef STRJOIN_H
#define STRJOIN_H

#include <stddef.h>

/* Join `count` strings from `parts` with `sep` between them.
 *
 * Returns a freshly malloc'd NUL-terminated string the caller must free,
 * or NULL when the request cannot be honoured.
 *
 *   count == 0            -> an empty string "" (still malloc'd, never NULL)
 *   parts == NULL, n > 0  -> NULL
 *   sep   == NULL         -> NULL
 *   any parts[i] == NULL  -> NULL
 *   total length overflow -> NULL
 *
 * The function must not read parts[] when count is 0.
 */
char *str_join(const char **parts, size_t count, const char *sep);

#endif
