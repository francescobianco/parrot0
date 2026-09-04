#include "records.h"
#include "quicksort.h"

#include <string.h>

/* Context-aware comparator: orders by the requested key (priority or name)
 * and breaks every tie on id, yielding a deterministic total order.  The
 * context argument carries the active record_key. */
static int compare_records(const void *left, const void *right,
                           void *context) {
    const Record *a = left;
    const Record *b = right;
    const enum record_key *key = context;

    int order;
    if (*key == RECORD_BY_PRIORITY) {
        order = (a->priority > b->priority) - (a->priority < b->priority);
    } else {
        order = strcmp(a->name, b->name);
    }
    if (order != 0) return order;
    return (a->id > b->id) - (a->id < b->id);
}

/* Incident CA-184: the legacy bubble-sort backend was quadratic on ordered
 * and duplicate-heavy exports.  Sorting now delegates to the generic
 * in-place quicksort module (quicksort.h); the total order -- primary key,
 * then id -- is unchanged, so output remains deterministic. */
int sort_records(Record *records, size_t count, enum record_key key) {
    return quicksort_records(records, count, sizeof *records,
                             compare_records, &key);
}
