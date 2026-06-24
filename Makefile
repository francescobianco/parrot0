# parrot0 - a self-evolving conversational agent in pure C.
#
# Common targets:
#   make            build bin/parrot0
#   make chat       build, then talk to it interactively
#   make test       build, then run the conversation test suite
#   make long-chat-bench  build, then run the 50-turn long-chat metrics
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
HDR     := $(wildcard src/*.h)
# brain.c is split into #include'd fragments (one translation unit, not separate
# objects — see src/brain.c). They are NOT in SRC (the wildcard is non-recursive),
# so they are never compiled standalone; but obj/brain.o must rebuild when one
# changes, so they are an explicit prerequisite below.
BRAIN_PARTS := $(wildcard src/brain/*.c)
BIN     := bin/parrot0
BENCH_PY ?= $(shell test -x .venv/bin/python && echo .venv/bin/python || echo python3)
BENCH_CACHE ?= .cache/huggingface/datasets

.PHONY: all build chat test piagent-bench sortlearn-bench glue-bench chat-bench long-chat-bench chat-sim sym-bench code-bench bench bench-superglue bench-superglue-local bench-mmlu bench-bbh impersonate simclean loop clean

all: build

build: $(BIN)

$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Depend on ALL headers: KB_TERM_LEN etc. live in kb.h and define struct
# layout, so a header change must rebuild every object or the binary links
# translation units with mismatched ABIs (silent memory corruption).
obj/%.o: src/%.c $(HDR) | obj
	$(CC) $(CFLAGS) -c -o $@ $<

# brain.o additionally depends on its #include'd fragments.
obj/brain.o: $(BRAIN_PARTS)

bin obj:
	@mkdir -p $@

chat: build
	@PARROT0_PROFILE=kb/profiles/agi.p0 ./$(BIN)

chat-bench: build
	@./tests/chatbench.sh

# gen189: basic-chat discovery harness — coverage over docs/plans/basic-chat.md,
# the catalogue of elementary prompts parrot0 still walls on. Per-category score
# to watch climb as categories are closed one structural generation at a time.
basic-chat-bench: build
	@./tests/basicchat.sh

long-chat-bench: build
	@./tests/longchatbench.sh

# gen160 (E1): compositional emergence benchmark — held-out dialogues where
# success requires >=3 independently-evolved subsystems to cooperate, scored as
# composes-unchanged / generic-parser / special-case. Ratchets the composing
# dialogues and records the open gaps.
compose-bench: build
	@./tests/composebench.sh

# gen173 (CODE-MASTERY.md): code-mastery discovery harness — submit code snippets
# and observe how far the AST-as-KB engine reaches. Ratchets the holding gates
# and records the open gaps (the next faculty to pull). See docs/CODE-MASTERY.md.
code-bench: build
	@./tests/codebench.sh

# gen194: the SWE-bench north star (docs/CODE-MASTERY.md §8) in DEGRADE mode —
# static, offline instances (no network), scoring sub-goals we can check and
# printing the first unsolved instance as the next concrete task. Never fails the
# build; it is a discovery instrument, not a gate.
swe-bench: build
	@./tests/swebench.sh

# gen215: the LINGUISTIC-GLUE discovery harness (docs/plans/the-linguistic-glue.md, G1).
# Multi-turn held-out dialogues where the essay's five absence-symptoms become metrics:
# it reports, per symptom, whether cross-turn continuity carried — the gap map that pulls
# the next connective mechanism (G2). Degrade mode: never fails the build, not in `make
# test`. EN+IT.
glue-bench: build
	@./tests/gluebench.sh

# gen200: end-to-end real solve — parrot0 derives a patch from STRUCTURE and the
# OFFICIAL SWE-bench Docker image judges it (FAIL_TO_PASS + PASS_TO_PASS). Needs
# docker + jq + network the first time (image pull is a curation step; parrot0
# itself never touches the net). Not part of `make test` (heavy, external).
swe-solve: build
	@./tests/swebench/parrot_solve.sh $(INSTANCE)

# gen205: pi-agent battery — parrot0 mounted as pi's model, driven over real HTTP
# through parrot0's built-in `--daemon` server (gen221; no Python bridge, no pi
# install, no network). Proves the read-only coding tools (list/read/grep/find)
# run LOCALLY in one turn and that answers are grounded in real fixture content.
# See docs/use-on-pi-agent.md.
piagent-bench: build
	@$(BENCH_PY) ./tests/piagent/piagent_bench.py

# Track A of docs/plans/learn-and-build.md: the honest, repeatable learn->build harness.
# Drives parrot0 (mounted as `pi`) through forget -> relearn-via-research -> articulated
# multi-step, asserting the synthesis gap is DECLARED (sort refused honestly), not faked.
# Prints a gap ledger like swe-bench; the sort assertion is the ratchet Track B flips.
sortlearn-bench: build
	@$(BENCH_PY) ./tests/piagent/sortlearn_bench.py

# LLM-simulated-user conversation benchmark (needs $OPENCODE_API_KEY + network;
# costs a little). Logs transcripts to tests/chat/sim/ and prints naturalness
# proxies. Not part of `make test` (non-deterministic, external).
chat-sim: build
	@$(BENCH_PY) ./tests/chatsim.py

# Cryptic-stimulus challenge: a non-reasoning oracle LLM vs parrot0 on short,
# OPEN-ENDED symbolic stimuli with no checkable answer — ASCII art, odd
# symmetries, leetspeak, Morse, musical notes, incomplete code. We study HOW the
# LLM behaves (it names the register and engages), not whether it is "correct".
# Same provider/auth as chat-sim ($OPENCODE_API_KEY). Logs to tests/sym/ and
# prints an engagement report. Pass --no-llm for a free parrot0-only run.
sym-bench: build
	@$(BENCH_PY) ./tests/symbench.py

test: build
	@./tests/run.sh
	@./tests/persist.sh
	@./tests/multigoal.sh
	@./tests/grammar.sh
	@./tests/anon.sh
	@./tests/explain.sh
	@./tests/howknow.sh
	@./tests/booklearn.sh
	@./tests/wiki_learning.sh
	@./tests/research_learn.sh
	@./tests/posix.sh
	@./tests/synth.sh
	@./tests/check_sort.sh
	@PARROT0_ORACLE=1 ./tests/posix_oracle.sh
	@./tests/experts.sh
	@./tests/profiles.sh
	@./tests/skills.sh
	@./tests/knowledge.sh

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

impersonate: build
	@./tests/impersonate.sh

# Autonomous chatsim-log janitor: replay each tests/chat/sim/*.log against the
# CURRENT parrot0 and delete the ones that no longer wall ("I don't understand")
# — they have graduated and no longer expose a growth edge. Logs that still wall
# are kept and their failing inputs printed. Pass ARGS=-n for a dry run.
simclean: build
	@./tests/simclean.sh $(ARGS)

loop:
	@cat LOOP.md

clean:
	@rm -rf obj bin
