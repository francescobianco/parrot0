#include "csv.h"
#include "records.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int usage(const char *program) {
    fprintf(stderr, "usage: %s --key priority|name\n", program);
    return 2;
}

int main(int argc, char **argv) {
    enum record_key key;
    if (argc != 3 || strcmp(argv[1], "--key") != 0) return usage(argv[0]);
    if (strcmp(argv[2], "priority") == 0) key = RECORD_BY_PRIORITY;
    else if (strcmp(argv[2], "name") == 0) key = RECORD_BY_NAME;
    else return usage(argv[0]);

    Record *records = NULL;
    size_t count = 0;
    if (read_records(stdin, &records, &count) != 0) {
        fputs("record-sort: invalid input\n", stderr);
        return 1;
    }
    if (sort_records(records, count, key) != 0 ||
        write_records(stdout, records, count) != 0) {
        free(records);
        fputs("record-sort: processing failed\n", stderr);
        return 1;
    }
    free(records);
    return 0;
}
