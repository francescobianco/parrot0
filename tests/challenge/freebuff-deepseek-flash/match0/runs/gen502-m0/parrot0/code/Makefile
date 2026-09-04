CC     ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

all: join

join: main.c strjoin.c strjoin.h
	$(CC) $(CFLAGS) -o join main.c strjoin.c

clean:
	rm -f join

.PHONY: all clean
