# parrot0 - a self-evolving conversational agent in pure C.
#
# Common targets:
#   make            build bin/parrot0
#   make chat       build, then talk to it interactively
#   make pi         build, start the parrot0 daemon, launch `pi` with parrot0 selected
#   make test       build, then run the conversation test suite
#   make long-chat-bench  build, then run the 50-turn long-chat metrics
#   make bench      build, then run all local benchmark-driver suites
#   make bench-superglue  run official SuperGLUE validation benchmark
#   make bench-mmlu       run MMLU-like local benchmark slices
#   make bench-bbh        run BIG-Bench Hard-like local benchmark slices
#   make loop       print the self-improvement loop protocol
#   make clean      remove build artifacts

CC      ?= cc
# -Wno-format-truncation: parrot0 uses snprintf throughout as a SAFE, deliberately
# bounded writer — truncating into a fixed reply/term buffer is the intended behavior,
# not a bug. At -O2 gcc emits ~50 range-based "output may be truncated" notes it cannot
# rule out (it has no value ranges), drowning the real warnings. We keep every other
# warning strict (-Wall -Wextra -Wpedantic) and silence only this high-noise category.
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -Wno-format-truncation -O2

# gen240 (universal-comprehension §7): OPTIONAL on-demand wiki learning. parrot0
# can fetch a certified static Wikipedia page over HTTPS to learn a topic it lacks
# — the SAME declarative knowledge we could have shipped a-priori, never an
# external intelligence/search API. It needs only a TLS transport: link libcurl by
# its SONAME (dev headers are not required — the brain declares the tiny ABI it
# uses). If libcurl is absent the feature compiles out and parrot0 falls back to
# the local corpus + informed decline. The fetch itself is also gated at runtime by
# PARROT0_WIKI_FETCH, so the default and all tests stay offline.
CURL_LIB := $(firstword $(wildcard /lib/*/libcurl.so.4 /usr/lib/*/libcurl.so.4 /usr/lib/libcurl.so.4))
ifneq ($(CURL_LIB),)
CFLAGS  += -DPARROT0_HAVE_CURL
CURL_LDLIBS := -l:libcurl.so.4
endif

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

.PHONY: all build chat chat-agent pi test test-engine legacy-test check gate capability-facts capability-report piagent-bench sortlearn-bench game-bench longtalk-bench glue-bench chat-bench long-chat-bench chat-sim sym-bench code-bench rulescore bench bench-superglue bench-superglue-local bench-mmlu bench-bbh impersonate simclean loop clean

all: build

build: $(BIN)

$(BIN): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(CURL_LDLIBS)

# gen317 (forge W0.4): brain_version is DERIVED, never hand-maintained — the
# generation label lives in the VERSION file (single source) and the commit is
# read from git at build time. Content-compare so an unchanged version never
# touches the header (no spurious rebuilds).
#
# gen321 (forge §15 row 7): the stamp is generated into the BUILD DIR, not src/.
# It used to be src/version.h, and since every object depends on every header, a
# semantically neutral commit invalidated all of them and rebuilt the whole brain
# (12.4 s measured, §3.5). A build artifact under src/ is what dragged it into
# every object's header closure. Now only obj/version.o sees it (-Iobj), so a
# commit recompiles one tiny TU and relinks — the engine's semantic fingerprint
# no longer depends on WHICH COMMIT it was built at.
GITHASH := $(shell git rev-parse --short HEAD 2>/dev/null || echo nogit)
obj/version_stamp.h: VERSION FORCE | obj
	@printf '#define PARROT0_GEN "%s"\n#define PARROT0_COMMIT "%s"\n' \
	  "$$(cat VERSION)" "$(GITHASH)" > obj/version_stamp.h.tmp; \
	if cmp -s obj/version_stamp.h.tmp obj/version_stamp.h; then rm -f obj/version_stamp.h.tmp; \
	else mv obj/version_stamp.h.tmp obj/version_stamp.h; fi
FORCE:

# Depend on ALL headers: KB_TERM_LEN etc. live in kb.h and define struct
# layout, so a header change must rebuild every object or the binary links
# translation units with mismatched ABIs (silent memory corruption).
obj/%.o: src/%.c $(HDR) | obj
	$(CC) $(CFLAGS) -c -o $@ $<

# The one TU that sees the generated stamp. Explicit rule: it overrides the
# pattern above so no other object gains -Iobj or a dependency on the commit.
obj/version.o: src/version.c obj/version_stamp.h $(HDR) | obj
	$(CC) $(CFLAGS) -Iobj -c -o $@ src/version.c

# brain.o additionally depends on its #include'd fragments.
obj/brain.o: $(BRAIN_PARTS)

bin obj:
	@mkdir -p $@

# gen243/335: the interactive chat is AUTONOMOUS — PARROT0_WIKI_FETCH=1 so a
# knowledge gap triggers the self-documentation plan (mod_learn -> acquire_knowledge
# -> certified Wikipedia fetch), per universal-comprehension §7. Tools are also on
# for file operations. `make test` and the benches never set these, so they stay
# offline/deterministic.
#
#   chat          conversational with tools + network (wikipedia fetch)
#   chat-agent    the coding agent — local tools on, network on. Same
#                 contracts as the API/piagent-bench.
chat: build
	@PARROT0_WIKI_FETCH=1 PARROT0_TOOLS=1 PARROT0_PROFILE=kb/profiles/agi.p0 ./$(BIN)

chat-agent: build
	@PARROT0_TOOLS=1 PARROT0_WIKI_FETCH=1 PARROT0_PROFILE=kb/profiles/agi.p0 ./$(BIN)

# gen223: build parrot0, free the port (kill any old daemon), prepare ~/.pi/agent/
# models.json (merged, never clobbering other providers), start the parrot0 daemon,
# and launch `pi` with parrot0 already selected. The daemon is stopped when pi exits.
# Override PI_PORT / PI_HOST; forward extra pi flags via ARGS="…". See docs/use-on-pi-agent.md.
pi: build
	@./scripts/pi.sh $(ARGS)

chat-bench: build
	@./tests/chatbench.sh

# gen189: basic-chat discovery harness — coverage over docs/plans/basic-chat.md,
# the catalogue of elementary prompts parrot0 still walls on. Per-category score
# to watch climb as categories are closed one structural generation at a time.
basic-chat-bench: build
	@./tests/basicchat.sh

# gen226 (docs/plans/mimic-llm.md, primo giro): mimic-llm style harness — PURE
# behavioral styling, offline. Catalogs parrot0's reaction to minimal/cryptic
# probes (sym-bench style) and shows a style profile's temperature biasing the
# FORM of a reply (argmax vs gen55 rotation). Never gates the build.
mimic-bench: build
	@chmod +x ./tests/mimic.sh; ./tests/mimic.sh

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

# e2e ratchet: drive parrot0 (mounted as `pi`) to BUILD a terminal tic-tac-toe a
# human plays against the computer, over THREE consecutive prompts (plan ->
# build+verify winner() -> assemble+build). Records a gap ledger like swe-bench;
# passes on honest behaviour, fails only on fabrication or a broken harness. The
# decline->built ratchet flips as codegen grows, with no change to the test.
game-bench: build
	@$(BENCH_PY) ./tests/piagent/game_bench.py

# LLM-simulated-user conversation benchmark (needs $OPENCODE_API_KEY + network;
# costs a little). Logs transcripts to tests/chat/sim/ and prints naturalness
# proxies. Not part of `make test` (non-deterministic, external).
chat-sim: build
	@$(BENCH_PY) ./tests/chatsim.py

# selfchat-baseline (gen335): an LLM talks to ITSELF, saved as the GOLD reference for
# the long-conversation mission (docs/plans/long-conversation.md) — what a naturally
# coherent long dialogue looks like, to evolve/measure parrot0's glue against. Same
# provider/auth as chat-sim ($OPENCODE_API_KEY). Writes tests/longchat/baselines/.
# External + non-deterministic, NOT part of `make test`. Vars: ARGS="--turns 30 ...".
selfchat-baseline:
	@$(BENCH_PY) ./tests/longchat/selfchat_baseline.py $(ARGS)

# Cryptic-stimulus challenge: a non-reasoning oracle LLM vs parrot0 on short,
# OPEN-ENDED symbolic stimuli with no checkable answer — ASCII art, odd
# symmetries, leetspeak, Morse, musical notes, incomplete code. We study HOW the
# LLM behaves (it names the register and engages), not whether it is "correct".
# Same provider/auth as chat-sim ($OPENCODE_API_KEY). Logs to tests/sym/ and
# prints an engagement report. Pass --no-llm for a free parrot0-only run.
sym-bench: build
	@$(BENCH_PY) ./tests/symbench.py

# An LLM interviews parrot0 to decide whether parrot0 is itself an LLM. The judge
# (default minimax-m2.5 on opencode-GO — a small model) asks 10 probing questions, scores
# each answer 1 (consistent with being an LLM) or 0 (evidently NOT an LLM). Writes
# LLMSCORE.md (question/answer/reason/vote grid + total /10) at the repo root. An
# HONEST mirror, not a target to game (no-deception): a low score is expected, since
# parrot0 has no LLM at runtime. Same provider/auth as chat-sim ($OPENCODE_API_KEY);
# external + non-deterministic, NOT part of `make test`.
llmscore: build
	@$(BENCH_PY) ./tests/llmscore.py

# AUTOLEARN (F., 2026-07-10) — the autonomous MCP trainer (T0.e, docs/plans/
# llmscore-strategies.md). An opencode-GO model interviews parrot0, judges each
# exchange, and on a failure formulates a LESSON (whitelisted KB facts) taught
# live over --mcp-engine; verified knowledge persists through the routed save-map
# into the curated kb/ tree (with only unrouted leftovers spilling to a fallback
# file) if the re-probe flips the judge's vote (the honesty oracle) — otherwise
# the lesson is rolled back and recorded in AUTOLEARN.md's failed-lesson ledger
# (the queue that names engine gaps). External + paid, NOT part of `make test`.
# Vars: ROUNDS=n WORKERS=n MULTIPLY=n RETRIES=n MODEL=slug KB=file LEDGER=file
#       PROBES=file (controlled mode).
autolearn: build
	@$(BENCH_PY) ./tests/autolearn.py --rounds $(or $(ROUNDS),5) \
		--workers $(or $(WORKERS),5) --multiply $(or $(MULTIPLY),20) \
		--retries $(or $(RETRIES),2) --model $(or $(MODEL),minimax-m2.5) \
		--kb $(or $(KB),kb/learning/autolearn-unrouted.p0) \
		--ledger $(or $(LEDGER),kb/learning/autolearn-ledger.jsonl) \
		--skip-list $(or $(SKIP_LIST),kb/learning/autolearn-skip.txt) \
		$(if $(PROBES),--probes $(PROBES),)

# RULESCORE (F., 2026-07-02) — on the LLMSCORE model: an LLM INVENTS 5 terminal
# mini-games described ONLY as numbered RULES; parrot0 must translate the text
# into a C implementation; the harness compiles + sample-runs whatever comes
# back, and a judge writes RULESCORE.md — a LONG report with a justified 0-5
# score per game (0 = nothing implemented, 5 = every rule encoded and running).
# Training pressure for the F4 text->code frontier; honest declines score 0 and
# are named as honest, fabrication is flagged as worse. Same provider/auth as
# llmscore ($OPENCODE_API_KEY); external + non-deterministic, NOT in `make test`.
rulescore: build
	@$(BENCH_PY) ./tests/rulescore.py

# longtalk-bench: how long can parrot0 hold a conversation with an LLM (default a
# kimi slug on opencode-GO) before it DROPS the thread on a blind wall? Progressive
# endurance score = longest run of exchanges held without "I don't understand".
# The north-star metric for docs/plans/universal-comprehension.md. Needs
# $OPENCODE_API_KEY + network; NOT part of `make test`.
longtalk-bench: build
	@$(BENCH_PY) ./tests/longtalk_bench.py $(LONGTALK_ARGS)

# gen318 (forge W0b, M-1a): the FOCAL runner — address ONE contract by id, see
# it stream, stop at the first counterexample. `make check` lists the catalog;
# `make check TEST=routing.agent-search.en` runs that contract; a prefix runs the
# family. This is the Tick's instrument: the whole suite is not a probe.
check: build
	@$(BENCH_PY) ./tests/check.py

# gen325 (forge §18): project the capability ledger into KB facts, so parrot0's
# answer about its own LIMITS is derived from gate evidence, not hand-written.
# kb/core/capabilities.p0 is GENERATED — never hand-edit it.
capability-facts:
	@$(BENCH_PY) ./tests/capability_facts.py

# gen316 (forge W0): run every benchmark the manifest (tests/benchmarks.json)
# declares as a 'gate' ratchet; red gate = baseline-broken, nothing promotes.
gate: build
	@$(BENCH_PY) ./tests/gate.py

# gen317 (forge W0.3): the capability ledger, GENERATED from gate results —
# verifies each faculty maturity claim against its evidence benchmarks and
# writes capabilities/manifest.json. Exits red if a claim regressed.
capability-report: build
	@$(BENCH_PY) ./tests/capability_report.py

# gen345 — the NEW test system. `test-engine` OWNS the background daemon's
# lifecycle: it kills a stale instance (via the pidfile) and starts a fresh one,
# so a fail-fast run that left a daemon alive is cleanly reran. TEST_SOCK must
# match TEST_ENGINE_SOCK_DEFAULT in src/testeng.h so the --test-send lines below
# stay clean (no --sock). The daemon loads the KB once; --test-send/--test-report
# load nothing. The old per-process `.chat` harness is now `legacy-test`, and its
# suites are being migrated. See docs/plans/test-engine.md.
TEST_SOCK := obj/test-engine.sock
TEST_PID  := obj/test-engine.pid

test-engine: build
	@-test -f $(TEST_PID) && kill `cat $(TEST_PID)` 2>/dev/null || true
	@rm -f $(TEST_SOCK) $(TEST_PID)
	@./$(BIN) --test-engine & echo $$! > $(TEST_PID)

# Reads as: start engine, send first file, send second file, ask for the total.
# FAIL-FAST: a --test-send exits 1 the instant its file has a failed assertion,
# so make stops on that line. Comment a --test-send line out to skip that suite.
test: test-engine
	@./$(BIN) --test-send tests/p0t/basics.p0t
	@./$(BIN) --test-send tests/p0t/conversation.p0t
	@./$(BIN) --test-send tests/p0t/greet.p0t
	@./$(BIN) --test-send tests/p0t/arith.p0t
	@./$(BIN) --test-send tests/p0t/world.p0t
	@./$(BIN) --test-send tests/p0t/facts.p0t
	@./$(BIN) --test-send tests/p0t/syllogism.p0t
	@./$(BIN) --test-send tests/p0t/casefold.p0t
	@./$(BIN) --test-send tests/p0t/contractions.p0t
	@./$(BIN) --test-send tests/p0t/numwords.p0t
	@./$(BIN) --test-send tests/p0t/initials.p0t
	@./$(BIN) --test-send tests/p0t/social_reaction.p0t
	@./$(BIN) --test-send tests/p0t/social_opener.p0t
	@./$(BIN) --test-send tests/p0t/smalltalk.p0t
	@./$(BIN) --test-send tests/p0t/chitchat.p0t
	@./$(BIN) --test-send tests/p0t/arith_flex.p0t
	@./$(BIN) --test-send tests/p0t/arith_nl.p0t
	@./$(BIN) --test-report

# legacy-test — the pre-gen345 conversation suite. These states are being
# MIGRATED to the test-engine (parrot0 --test-engine, .p0t files); each script
# here should become one or more .p0t files under tests/p0t/. See
# docs/plans/test-engine.md.
legacy-test: build
	@echo "############################################################"
	@echo "# legacy-test: these suites are being MIGRATED to the new  #"
	@echo "# test-engine (.p0t files, 'make test'). Do not add to them.#"
	@echo "############################################################"
	@./tests/run.sh
	@./tests/checkfocal.sh
	@./tests/buildstamp.sh
	@./tests/selflimits.sh
	@./tests/syllogism.sh
	@./tests/repair.sh
	@./tests/agentrepair.sh
	@./tests/toolexec.sh
	@./tests/exec-dirfd.sh
	@./tests/agentkernel.sh
	@./tests/code-task-agent.sh
	@./tests/patch-artifact.sh
	@./tests/patch-check.sh
	@./tests/agentcommit.sh
	@./tests/segment.sh
	@./tests/universal-input.sh
	@$(BENCH_PY) ./tests/openai-input-limit.py
	@$(BENCH_PY) ./tests/autolearn_structure.py
	@./tests/llmscore-kbfirst.sh
	@./tests/kb-evidence-scale.sh
	@./tests/mcp-input-payload.sh
	@./tests/learnbuild.sh
	@$(BENCH_PY) ./tests/manifest_audit.py
	@./tests/cuechains.sh
	@./tests/archetype.sh
	@./tests/persist.sh
	@./tests/restore.sh
	@./tests/mcp.sh
	@./tests/mcp-teach.sh
	@./tests/dollarvar.sh
	@./tests/assertclause.sh
	@./tests/naf.sh
	@./tests/compound.sh
	@./tests/strknow.sh
	@./tests/answerframe.sh
	@./tests/aggregate.sh
	@./tests/article.sh
	@./tests/adjagree.sh
	@./tests/artfres.sh
	@./tests/cliticfr.sh
	@./tests/vmorph.sh
	@./tests/savemap.sh
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
	@./tests/llmscore_world.sh
	@./tests/enumerate.sh

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
