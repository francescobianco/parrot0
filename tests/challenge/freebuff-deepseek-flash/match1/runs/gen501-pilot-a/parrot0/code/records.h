#ifndef CHALLENGE_RECORDS_H
#define CHALLENGE_RECORDS_H

#include <stddef.h>

enum record_key {
    RECORD_BY_PRIORITY,
    RECORD_BY_NAME
};

typedef struct {
    long id;
    long priority;
    char name[96];
} Record;

int sort_records(Record *records, size_t count, enum record_key key);

#endif
