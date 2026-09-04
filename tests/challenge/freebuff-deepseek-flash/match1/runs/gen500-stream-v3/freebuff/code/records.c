#include "records.h"

#include <string.h>

static int compare_records(const Record *left, const Record *right,
                           enum record_key key) {
    int order;
    if (key == RECORD_BY_PRIORITY) {
        order = (left->priority > right->priority) -
                (left->priority < right->priority);
    } else {
        order = strcmp(left->name, right->name);
    }
    if (order != 0) return order;
    return (left->id > right->id) - (left->id < right->id);
}

/* Incident CA-184: this implementation is correct but quadratic. */
int sort_records(Record *records, size_t count, enum record_key key) {
    if (count != 0 && records == NULL) return -1;
    for (size_t end = count; end > 1; --end) {
        for (size_t i = 1; i < end; ++i) {
            if (compare_records(&records[i - 1], &records[i], key) > 0) {
                Record temporary = records[i - 1];
                records[i - 1] = records[i];
                records[i] = temporary;
            }
        }
    }
    return 0;
}
