/* join — CLI over the str_join backend.
 *
 *   ./join SEP [WORD...]     prints the words joined by SEP, then a newline
 *   ./join                   usage on stderr, exit 2
 *
 * On a NULL result from the backend the tool prints nothing and exits 1.
 */
#include <stdio.h>
#include <stdlib.h>
#include "strjoin.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: join SEP [WORD...]\n");
        return 2;
    }
    const char **parts = NULL;
    size_t count = (size_t)(argc - 2);
    if (count > 0) {
        parts = malloc(count * sizeof *parts);
        if (!parts) return 1;
        for (size_t i = 0; i < count; i++) parts[i] = argv[i + 2];
    }
    char *joined = str_join(parts, count, argv[1]);
    free((void *)parts);
    if (!joined) return 1;
    printf("%s\n", joined);
    free(joined);
    return 0;
}
