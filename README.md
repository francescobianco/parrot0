a# parrot0

A self-evolving conversational agent written in **pure C**.

parrot0 is an experiment. It starts as a parrot — it just repeats what you
say — and grows, one small generation at a time, into something more
conversational. The catch: there is **no language model inside it at
runtime**. Every behaviour is a plain C algorithm that *emerged* from an AI
agent iterating a self-improvement loop.

The wager behind it — that an LLM can reconstruct, in deterministic C and
through dialogue alone, the reasoning structure it already runs internally — is
spelled out in **[PRINCIPLES.md](PRINCIPLES.md)**. Start there.

## Try it

```sh
make chat        # build and talk to the current generation
```

```
parrot0 [gen0-parrot] - say something ('/quit' to exit)
you> hello
hello
you> /quit
```

## How it grows

The whole project is structured around a loop:

1. `TASK.md` holds the single current goal.
2. The agent edits `src/brain.c` (the part that evolves) to chase it.
3. `tests/cases/*.chat` lock in each new behaviour so nothing regresses.
4. `JOURNAL.md` records every generation.

See **[LOOP.md](LOOP.md)** for the full protocol.

## Layout

| Path              | Role                                              |
|-------------------|---------------------------------------------------|
| `src/main.c`      | Stable I/O shell (REPL, chat protocol)            |
| `src/brain.c/.h`  | The brain — **this is what evolves**              |
| `tests/`          | Conversation test harness + cases                 |
| `Makefile`        | `make` / `chat` / `test` / `loop` / `clean`       |
| `TASK.md`         | The current goal for the loop                      |
| `LOOP.md`         | The self-improvement protocol                      |
| `JOURNAL.md`      | Generation-by-generation history                   |

## Build targets

```sh
make            # build bin/parrot0
make chat       # build + interactive chat
make test       # build + run conversation tests
make loop       # print the loop protocol
make clean      # remove build artifacts
```