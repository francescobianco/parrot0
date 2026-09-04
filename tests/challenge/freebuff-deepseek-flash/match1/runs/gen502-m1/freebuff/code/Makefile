CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

OBJS = main.o records.o csv.o quicksort.o

all: record-sort

record-sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c records.h csv.h
records.o: records.c records.h quicksort.h
csv.o: csv.c csv.h records.h
quicksort.o: quicksort.c quicksort.h

clean:
	rm -f $(OBJS) record-sort

.PHONY: all clean
