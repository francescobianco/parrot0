# parrot0 - a self-evolving conversational agent in pure C.
#
# Common targets:
#   make            build bin/parrot0
#   make chat       build, then talk to it interactively
#   make test       build, then run the conversation test suite
#   make loop       print the self-improvement loop protocol
#   make clean      remove build artifacts

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:src/%.c=obj/%.o)
BIN     := bin/parrot0

.PHONY: all build chat test loop clean

all: build

build: $(BIN)

$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJ)

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c -o $@ $<

bin obj:
	@mkdir -p $@

chat: build
	@./$(BIN)

test: build
	@./tests/run.sh

loop:
	@cat LOOP.md

clean:
	@rm -rf obj bin