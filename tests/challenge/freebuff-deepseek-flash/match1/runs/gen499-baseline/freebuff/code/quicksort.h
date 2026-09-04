#ifndef CHALLENGE_QUICKSORT_H
#define CHALLENGE_QUICKSORT_H

#include <stddef.h>

typedef int (*quicksort_cmp_fn)(const void *left, const void *right, void *context);

int quicksort_records(void *base, size_t count, size_t width,
                      quicksort_cmp_fn compare, void *context);

#endif
