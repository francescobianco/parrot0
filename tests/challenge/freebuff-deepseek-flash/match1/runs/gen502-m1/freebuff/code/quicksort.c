/*
 * quicksort.c -- reusable in-place sorting backend for arrays of arbitrary
 * fixed-width records.
 *
 * quicksort_records() orders `count` records of `width` bytes each starting
 * at `base`, using the caller-supplied, context-aware `compare` function.
 * The context pointer is passed through to every comparison untouched.
 *
 * Design notes
 * ------------
 *  - Median-of-three pivot selection and three-way (Dutch national flag)
 *    partitioning collapse runs of records equal to the pivot into a single
 *    partition step, so duplicate-heavy and all-equal inputs cost O(n)
 *    comparisons rather than O(n^2).
 *  - Partitioning is driven by an explicit frame stack (no recursion, no
 *    dynamic allocation), and every frame records how many partition levels
 *    led to it.  Any frame deeper than the introsort bound
 *    ~2*floor(log2(count)) is finished with an in-place heapsort, so no
 *    input -- ordered, reverse-ordered, duplicate-heavy, or adversarial --
 *    can force quadratic comparison counts or unbounded stack growth.
 *  - No qsort/qsort_r, no subprocesses, and no non-standard libraries.
 */
#include "quicksort.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

/* Swap the two `width`-byte blocks starting at lhs and rhs. */
static void swap_bytes(unsigned char *restrict lhs, unsigned char *restrict rhs,
                       size_t width) {
    while (width >= sizeof(uint64_t)) {
        uint64_t temp;
        memcpy(&temp, lhs, sizeof temp);
        memcpy(lhs, rhs, sizeof temp);
        memcpy(rhs, &temp, sizeof temp);
        lhs += sizeof temp;
        rhs += sizeof temp;
        width -= sizeof temp;
    }
    while (width-- != 0) {
        unsigned char temp = *lhs;
        *lhs++ = *rhs;
        *rhs++ = temp;
    }
}

/* Compare the records at indices i and j of the array at `arr`. */
static int compare_at(const unsigned char *arr, size_t width,
                      quicksort_cmp_fn compare, void *context, size_t i,
                      size_t j) {
    return compare(arr + i * width, arr + j * width, context);
}

/* Return the index of the median-valued element among lo, mid and hi - 1.
 * Any of the three is a valid pivot, so ties only affect balance. */
static size_t median_of_three(const unsigned char *arr, size_t width,
                              quicksort_cmp_fn compare, void *context,
                              size_t lo, size_t hi) {
    size_t mid = lo + (hi - lo) / 2;
    size_t last = hi - 1;
    int ab = compare_at(arr, width, compare, context, lo, mid);
    int ac = compare_at(arr, width, compare, context, lo, last);
    int bc = compare_at(arr, width, compare, context, mid, last);
    if (ab <= 0) {
        if (bc <= 0) return mid;
        return ac <= 0 ? last : lo;
    }
    if (ac <= 0) return lo;
    return bc <= 0 ? last : mid;
}

/* Three-way partition of arr[lo, hi) around a median-of-three pivot.
 * Writes back the boundaries of the pivot-equal band so that
 *   arr[lo, *eq_start) < pivot, arr[*eq_start, *eq_end) == pivot,
 *   arr[*eq_end, hi) > pivot.
 * The pivot element is parked at `lo` throughout the scan (so comparisons
 * are stable) and moved into the equal band at the end. */
static void partition_three_way(unsigned char *arr, size_t lo, size_t hi,
                                size_t width, quicksort_cmp_fn compare,
                                void *context, size_t *eq_start,
                                size_t *eq_end) {
    size_t pivot = median_of_three(arr, width, compare, context, lo, hi);
    if (pivot != lo)
        swap_bytes(arr + lo * width, arr + pivot * width, width);

    size_t less = lo + 1;  /* arr[lo + 1, less) are strictly less */
    size_t greater = hi;   /* arr[greater, hi) are strictly greater */
    size_t i = lo + 1;
    while (i < greater) {
        int order = compare(arr + i * width, arr + lo * width, context);
        if (order < 0) {
            if (less != i)
                swap_bytes(arr + less * width, arr + i * width, width);
            ++less;
            ++i;
        } else if (order > 0) {
            --greater;
            if (greater != i)
                swap_bytes(arr + greater * width, arr + i * width, width);
        } else {
            ++i;
        }
    }

    /* Move the pivot element (still at lo) to the front of the equal band,
     * swapping in the last strictly-less element (a no-op when the less
     * band is empty). */
    if (less - 1 > lo)
        swap_bytes(arr + lo * width, arr + (less - 1) * width, width);
    *eq_start = less - 1;
    *eq_end = greater;
}

/* Sift arr[root] down a max-heap spanning relative indices [0, count). */
static void sift_down(unsigned char *arr, size_t root, size_t count,
                      size_t width, quicksort_cmp_fn compare, void *context) {
    while (root < count / 2) {
        size_t child = 2 * root + 1;
        if (child + 1 < count &&
            compare(arr + (child + 1) * width, arr + child * width,
                    context) > 0)
            ++child;
        if (compare(arr + root * width, arr + child * width, context) >= 0)
            break;
        swap_bytes(arr + root * width, arr + child * width, width);
        root = child;
    }
}

/* Sort arr[lo, hi) in place with heapsort (the introsort fallback). */
static void heap_sort(unsigned char *arr, size_t lo, size_t hi, size_t width,
                      quicksort_cmp_fn compare, void *context) {
    size_t count = hi - lo;
    if (count < 2) return;
    unsigned char *items = arr + lo * width;

    for (size_t start = count / 2; start > 0; --start)
        sift_down(items, start - 1, count, width, compare, context);
    for (size_t end = count; end > 1; --end) {
        swap_bytes(items, items + (end - 1) * width, width);
        sift_down(items, 0, end - 1, width, compare, context);
    }
}

typedef struct {
    size_t lo;
    size_t hi;
    size_t depth;
} Frame;

/* Push a subrange unless it is trivially sorted (fewer than two records). */
static void push_frame(Frame *frames, size_t *top, size_t lo, size_t hi,
                       size_t depth) {
    if (hi - lo < 2) return;
    frames[*top].lo = lo;
    frames[*top].hi = hi;
    frames[*top].depth = depth;
    ++*top;
}

int quicksort_records(void *base, size_t count, size_t width,
                      quicksort_cmp_fn compare, void *context) {
    if (count == 0) return 0;  /* an empty range sorts fine, even with NULLs */
    if (base == NULL || width == 0 || compare == NULL) return -1;
    if (count > SIZE_MAX / width) return -1;  /* count * width would wrap */
    if (count < 2) return 0;

    unsigned char *arr = base;

    /* Introsort depth bound: 2 * floor(log2(count)) partition levels plus a
     * small constant; deeper frames are heapsorted instead of partitioned. */
    size_t depth_limit = 2;
    for (size_t remaining = count; remaining > 1; remaining /= 2)
        depth_limit += 2;

    /* At most one pending sibling per partition level, and partition levels
     * are capped by depth_limit <= 2 * bits(size_t), so a fixed frame array
     * sized to a few times that can never overflow. */
    Frame frames[sizeof(size_t) * CHAR_BIT * 2 + 8];
    size_t top = 0;
    push_frame(frames, &top, 0, count, 0);

    while (top != 0) {
        Frame frame = frames[--top];
        size_t lo = frame.lo;
        size_t hi = frame.hi;
        if (hi - lo < 2) continue;

        if (frame.depth >= depth_limit) {
            heap_sort(arr, lo, hi, width, compare, context);
            continue;
        }

        size_t eq_start;
        size_t eq_end;
        partition_three_way(arr, lo, hi, width, compare, context, &eq_start,
                            &eq_end);

        size_t depth = frame.depth + 1;
        size_t left_size = eq_start - lo;
        size_t right_size = hi - eq_end;

        /* Push the larger side first so the smaller side is processed next;
         * deep but tiny subranges therefore never accumulate on the stack. */
        if (left_size >= right_size) {
            push_frame(frames, &top, lo, eq_start, depth);
            push_frame(frames, &top, eq_end, hi, depth);
        } else {
            push_frame(frames, &top, eq_end, hi, depth);
            push_frame(frames, &top, lo, eq_start, depth);
        }
    }
    return 0;
}
