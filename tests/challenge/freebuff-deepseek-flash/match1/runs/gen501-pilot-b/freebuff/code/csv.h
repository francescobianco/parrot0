#ifndef CHALLENGE_CSV_H
#define CHALLENGE_CSV_H

#include "records.h"

#include <stdio.h>

int read_records(FILE *input, Record **records, size_t *count);
int write_records(FILE *output, const Record *records, size_t count);

#endif
