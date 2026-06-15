# parrot0 - a self-evolving conversational agent in pure C.
#
# Common targets:
#   make            build bin/parrot0
#   make chat       build, then talk to it interactively
#   make test       build, then run the conversation test suite
#   make bench      build, then run all local benchmark-driver suites
#   make bench-superglue  run official SuperGLUE validation benchmark
#   make bench-mmlu       run MMLU-like local benchmark slices
#   make bench-bbh        run BIG-Bench Hard-like local benchmark slices
#   make loop       print the self-improvement loop protocol
#   make clean      remove build artifacts

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:src/%.c=obj/%.o)
BIN     := bin/parrot0
BENCH_PY ?= $(shell test -x .venv/bin/python && echo .venv/bin/python || echo python3)
BENCH_CACHE ?= .cache/huggingface/datasets

.PHONY: all build chat test chat-bench chat-sim bench bench-superglue bench-superglue-local bench-mmlu bench-bbh loop clean

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

chat-bench: build
	@./tests/chatbench.sh

# LLM-simulated-user conversation benchmark (needs $OPENCODE_API_KEY + network;
# costs a little). Logs transcripts to tests/chat/sim/ and prints naturalness
# proxies. Not part of `make test` (non-deterministic, external).
chat-sim: build
	@$(BENCH_PY) ./tests/chatsim.py

test: build
	@./tests/run.sh
	@./tests/persist.sh
	@./tests/multigoal.sh
	@./tests/grammar.sh
	@./tests/anon.sh
	@./tests/explain.sh
	@./tests/howknow.sh
	@./tests/posix.sh
	@PARROT0_ORACLE=1 ./tests/posix_oracle.sh

bench: build
	@./tests/bench.sh all

bench-superglue: build
	@$(BENCH_PY) ./tests/bench_superglue.py --binary ./$(BIN) --cache-dir $(BENCH_CACHE)

bench-superglue-local: build
	@./tests/bench.sh superglue

bench-mmlu: build
	@./tests/bench.sh mmlu

bench-bbh: build
	@./tests/bench.sh bbh

loop:
	@cat LOOP.md

clean:
	@rm -rf obj bin
