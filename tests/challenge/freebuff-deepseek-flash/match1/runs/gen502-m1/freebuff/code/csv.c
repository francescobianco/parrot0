#include "csv.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int read_records(FILE *input, Record **records, size_t *count) {
    char line[256];
    size_t used = 0, capacity = 0;
    Record *items = NULL;
    while (fgets(line, sizeof line, input)) {
        Record item;
        char tail;
        int matched = sscanf(line, "%ld,%ld,%95[^\n]%c", &item.id,
                             &item.priority, item.name, &tail);
        /* The trailing %c consumes the '\n' of a normal record (4 matches);
         * it only fails when the name runs to EOF without a newline (3
         * matches).  Anything else means the name overran its 95-character
         * limit or the line is malformed. */
        if (matched != 3 && (matched != 4 || tail != '\n')) {
            free(items);
            return -1;
        }
        if (used == capacity) {
            size_t next = capacity ? capacity * 2 : 32;
            if (next > SIZE_MAX / sizeof *items) {
                free(items);
                errno = EOVERFLOW;
                return -1;
            }
            Record *grown = realloc(items, next * sizeof *items);
            if (!grown) {
                free(items);
                return -1;
            }
            items = grown;
            capacity = next;
        }
        items[used++] = item;
    }
    if (ferror(input)) {
        free(items);
        return -1;
    }
    *records = items;
    *count = used;
    return 0;
}

int write_records(FILE *output, const Record *records, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (fprintf(output, "%ld,%ld,%s\n", records[i].id,
                    records[i].priority, records[i].name) < 0)
            return -1;
    }
    return 0;
}
