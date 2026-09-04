CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

OBJS = main.o records.o csv.o

all: record-sort

record-sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c records.h csv.h
records.o: records.c records.h
csv.o: csv.c csv.h records.h

clean:
	rm -f $(OBJS) record-sort

.PHONY: all clean
