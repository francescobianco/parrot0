#include "quicksort.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int key;
    unsigned char payload[7];
} Record;

typedef struct {
    long comparisons;
    int direction;
} CmpContext;

static int cmp_int(const void *left, const void *right, void *opaque) {
    CmpContext *ctx = opaque;
    int a = *(const int *)left;
    int b = *(const int *)right;
    ctx->comparisons++;
    return ctx->direction * ((a > b) - (a < b));
}

static int cmp_record(const void *left, const void *right, void *opaque) {
    const Record *a = left;
    const Record *b = right;
    CmpContext *ctx = opaque;
    ctx->comparisons++;
    return ctx->direction * ((a->key > b->key) - (a->key < b->key));
}

static void result(const char *name, int passed, int *all) {
    printf("CHECK %s %s\n", name, passed ? "PASS" : "FAIL");
    if (!passed) *all = 0;
}

static int sorted_ints(const int *values, size_t n, int direction) {
    for (size_t i = 1; i < n; ++i) {
        if (direction * values[i - 1] > direction * values[i]) return 0;
    }
    return 1;
}

int main(void) {
    int all = 1;
    CmpContext asc = {0, 1};
    int basic[] = {8, -3, 8, 0, 17, 4, -9, 2};
    int ok = quicksort_records(basic, 8, sizeof basic[0], cmp_int, &asc) == 0;
    result("basic_order", ok && sorted_ints(basic, 8, 1), &all);

    Record records[41];
    unsigned char seen[41] = {0};
    for (size_t i = 0; i < 41; ++i) {
        records[i].key = (int)((i * 17) % 13) - 6;
        memset(records[i].payload, (int)i, sizeof records[i].payload);
    }
    CmpContext generic_ctx = {0, 1};
    ok = quicksort_records(records, 41, sizeof records[0], cmp_record, &generic_ctx) == 0;
    int generic_ok = ok;
    for (size_t i = 0; i < 41; ++i) {
        unsigned id = records[i].payload[0];
        if (id >= 41 || seen[id]) generic_ok = 0;
        else seen[id] = 1;
        for (size_t j = 1; j < sizeof records[i].payload; ++j)
            if (records[i].payload[j] != id) generic_ok = 0;
        if (i && records[i - 1].key > records[i].key) generic_ok = 0;
    }
    result("generic_records", generic_ok, &all);
    result("duplicate_permutation", generic_ok && generic_ctx.comparisons < 2000, &all);

    int descending[] = {1, 9, 2, 8, 3, 7, 4, 6, 5};
    CmpContext desc = {0, -1};
    ok = quicksort_records(descending, 9, sizeof descending[0], cmp_int, &desc) == 0;
    result("comparator_context", ok && sorted_ints(descending, 9, -1), &all);

    int one = 1;
    int invalid = quicksort_records(NULL, 0, 0, NULL, NULL) == 0;
    invalid = invalid && quicksort_records(NULL, 1, sizeof one, cmp_int, &asc) == -1;
    invalid = invalid && quicksort_records(&one, 1, 0, cmp_int, &asc) == -1;
    invalid = invalid && quicksort_records(&one, 1, sizeof one, NULL, &asc) == -1;
    result("invalid_arguments", invalid, &all);
    result("overflow_rejected",
           quicksort_records(&one, SIZE_MAX, 2, cmp_int, &asc) == -1, &all);

    enum { N = 2048 };
    int *adversarial = malloc(sizeof *adversarial * N);
    int growth_ok = adversarial != NULL;
    long total = 0;
    if (adversarial) {
        for (int mode = 0; mode < 3; ++mode) {
            for (int i = 0; i < N; ++i)
                adversarial[i] = mode == 0 ? i : mode == 1 ? N - i : 7;
            CmpContext stress = {0, 1};
            if (quicksort_records(adversarial, N, sizeof *adversarial,
                                  cmp_int, &stress) != 0 ||
                !sorted_ints(adversarial, N, 1))
                growth_ok = 0;
            total += stress.comparisons;
        }
        free(adversarial);
    }
    result("adversarial_growth", growth_ok && total < 350000, &all);
    return all ? 0 : 1;
}
