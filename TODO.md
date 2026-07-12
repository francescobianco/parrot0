# TODO — what parrot0 must GAIN

> **Why this file exists.** The forge (`make check`, `make gate`, the manifest
> audit) is a **filter**: it says what to discard. It contains no push. A project
> that only knows how to reject converges on a very well-tested nothing. This
> file is the **push** — the list of capabilities parrot0 must acquire to do its
> job — and the forge's only role is to refuse the ones that are faked.
>
> **The mission** (forge plan §0): parity, and where possible superiority, on
> **verifiable coding outcomes** — without an LLM in the runtime.
>
> **Provenance.** Every row below was pulled from a real conversation with
> parrot0 on 2026-07-12 (`make chat` config: `kb/profiles/agi.p0`), not from
> speculation. The verbatim replies are quoted. This is domain-pull: the gap
> named the task, not the other way round.

---

## P0 — parrot0 FABRICATES a code defect (honesty regression)

This outranks every feature. `PRINCIPLES.md` forbids the impostor, and the forge
plan is explicit that a **misclaim is worse than a wall** (§1.6).

```
you> what is wrong with this code: int add(int a, int b) { return a + b; }
parrot0> Issue found: Missing semicolon at the end of a statement.

you> what is wrong with this code: int main(void) { return 0; }
parrot0> Issue found: Missing semicolon at the end of a statement.

you> find the bug in: int f(int x) { return x * 2; }
parrot0> Issue found: Missing semicolon at the end of a statement.
```

All three are **correct C**. parrot0 invents a defect that is not there — the
one thing a verifiable coding agent may never do.

**Root cause** (localized, `src/brain/70-social-pragma.c:929`): the check finds
`return `, then tests the character immediately **before** the closing `}`
against `;`. In a one-line function — which is the only shape the line-based chat
surface can receive — that character is a **space**. The bug is a whitespace
off-by-one, and it fires on every correct single-line function.

Proof the diagnosis is exact — the only difference is one space:

```
you> what is wrong with this code: int f(void) { return 0; }     -> "Issue found: Missing semicolon"   (WRONG)
you> what is wrong with this code: int f(void) { return 0;}      -> "I did not find obvious errors"    (right)
you> what is wrong with this code: int f(void) { return 0 }      -> "Issue found: Missing semicolon"   (right, but by accident)
```

**Contract to seal:** a syntactically valid snippet must produce *no* finding, in
every whitespace layout; a genuinely missing `;` must still be found. The
negative case (correct code ⇒ silence) is the one that was never tested — that is
why the fabrication survived.

**Oracle:** mechanical and free — the C compiler already in the tree
(`code_compile`). If the snippet compiles, a syntax finding is a **false claim**
by construction. Today the syntax checker is a hand-rolled scanner that nobody
cross-checks against the compiler that sits right next to it.

**Wider lesson to encode:** every `check_*` in the code module needs a
correct-code negative. A checker is only trustworthy if it has been shown to stay
silent.

---

## P1 — the code faculty is real but UNREACHABLE through language (the keyhole)

The capability ledger claims `code_reading = TRANSFER` on the evidence of 25/25
`code-bench` gates. That claim is about the **engine**. The **surface** to it is
a keyhole of exact cues:

```
you> what functions does this define: int sum(int n){ ... }
parrot0> I read it as code: it defines sum.                       (the cue fits)

you> explain what this does: int sum(int n){ ... }
parrot0> That looks like a snippet of code.                       (the wall)

you> there is a bug in this code: int f(int n){ ... } it should sum 1..n-1
parrot0> That looks like a snippet of code.                       (the wall)
```

The same snippet, the same faculty, one phrasing away. A coding agent whose
analysis is reachable only by `what does X call:` cannot receive a GitHub issue,
which is the whole mission.

This is the **C-series diagnosis of TASKLIST.md recurring for code**: "the
reasoning core is real but unreachable through natural language". It was closed
once for conversation (cue-based intent, gen51+); it has never been closed for
code.

**Contract:** an intent schema over code requests — `explain / what does this do
/ what is wrong / where is the bug / review this / what would X return` — routed
to the faculties that already exist, with an honest decline naming the gap when
the faculty is absent.

**KB-first:** this is a lexical class + intent map, i.e. **facts, not C**
(`kb/core/intents.p0`, the `intent_cue`/`answer_frame` machinery that already
exists). Growing a new C branch per phrasing would be the phrasebook.

**Measure:** the same code question, in 10 held-out natural phrasings EN+IT, must
reach the same faculty. This is also the gate that makes `code_reading =
TRANSFER` an honest claim rather than a bench artifact.

---

## P2 — the evaluator cannot execute a LOOP

```
you> what does sum(3) return: int sum(int n){ int s=0; for(int i=0;i<=n;i++) s+=i; return s; }
parrot0> I read it as code, but I cannot compute sum(3) yet — its body is beyond
         my arithmetic evaluator.
```

An honest decline (good), but it is the whole ballgame: `code_eval` handles
locals and recursion (gen198/199) and **not iteration**. Almost every real
function has a loop. Without executing a loop, parrot0 cannot predict behaviour,
so it cannot verify its own patch by reasoning — only by compiling.

**Contract:** `for` and `while` with a bounded step budget; the budget exhausting
is `budget_exhausted`, never a guess. Same engine for C and Python (the
language-as-delta rule, CODE-MASTERY §7b).

**Oracle:** the real compiler/interpreter already wired as `code_run`.

---

## P3 — synthesis has exactly three schemas

```
you> write a C function that reverses a string in place
parrot0> I understood the request — produce "c function that reverses a string in
         place" — but I don't have a verified schema for that artifact yet; I only
         synthesize what an oracle can check (a sort from a learned shape,
         arithmetic composition, a count-to-threshold game).
```

The decline is **exemplary** — it names its own envelope precisely, and it is
what a trustworthy agent should say. But the envelope is three hand-curated
schemas, and each new one has so far cost a generation of C. That is the linear,
impostor-shaped growth `LOOP.md` warns about: generation *n* buys capability *n*.

**Contract (this is the compounding one):** a *fresh* specification must produce
its own schema. The forge plan's C2 campaign and W6 name it: markdown/spec →
constraints → candidate schema with provenance → generated examples/properties →
synthesized code → oracle → **ablation** (without the page, the task goes red
again). Zero hand-written `algo_shape`.

Until that lands, `learn_build = SEED` is the honest label and every new schema
is one more brick, not a faculty.

---

## P4 — there is no agentic loop (goal → act → observe → verify → repair)

Everything above is single-turn. The mission needs the loop: read an issue,
locate the region, edit, run the tests, **read the failure**, repair, stop when
green or decline naming the gap.

Today: `make swe-bench` engages **0 of 5** issues; the four SWE-bench instances
that were genuinely resolved were reached by `fix the bug in <directory>` — the
issue text was never passed to parrot0 (forge plan §3.2). That is a proof of
structural smell-localization, and it is real, but it is **not agency**, and the
harness must never be allowed to imply that it is.

**Contract:** the typed kernel — Goal, Action, Observation, Verdict, Gap — with
a bounded repair loop on a deliberately-wrong candidate: fail with a structured
verdict, tie the diagnostic to a node, propose ≥2 fixes, pick the smallest blast
radius, re-run, stop within N attempts, keep the trace.

This is the forge plan's W3 and the single biggest structural jump on the list.

---

## P5 — a nonce-word syllogism walls

```
you> if all bloops are razzies and all razzies are lazzies, are all bloops lazzies?
parrot0> I don't understand that yet.
```

parrot0's inference core does transitive rule chaining perfectly — with facts
already in the KB. It cannot take a miniature world **introduced inside the
turn** and reason in it. This is TASKLIST C8, still open, and it is the cheapest
demonstration of real reasoning an LLM gives for free.

**KB-first:** a universal-rule intake pattern (`all X are Y` ⇒ `Y(Z) :- X(Z)`),
scoped to the turn so nonce vocabulary never pollutes the committed KB — the
`mod_world` overlay (gen143) already provides the scope. Structural, not lexical:
the test must generate **fresh nonsense predicates on every run**.

---

## P6 — a routing collision: "what are you unable to do?"

```
you> what are you unable to do?
parrot0> I am parrot0.
```

The identity module claims a turn that is a **capability** question, because the
first-match-wins registry sees a self-reference cue and stops. It is the same
class of defect as the `agent_search`/arithmetic collision that gen313-315
repaired, and it now has a runner that can pin it in 0.4 s
(`make check TEST=routing.*`).

Honest self-knowledge — knowing and *saying* where the envelope ends — is not a
cosmetic feature for this project. It is the anti-impostor property itself.

---

## Ordering, and why

| # | Row | Why here |
|---|---|---|
| P0 | the fabrication | an agent that invents defects is worse than one that declines; it is also a one-line fix with a free oracle |
| P1 | the code keyhole | it unlocks faculties that ALREADY EXIST — highest leverage per line, and it is KB-first, not C |
| P6 | the routing collision | cheap, and it restores honest self-report |
| P2 | loops in the evaluator | the precondition for reasoning about real code |
| P5 | in-turn rule intake | cheap, structural, closes a rung LLMs give free |
| P4 | the agentic loop | the biggest jump; needs P1+P2 to have anything to drive |
| P3 | schema induction | the only row that makes growth compound instead of add |

**The discipline stays.** One contract per generation; seal the red first; prefer
knowledge over C (P1 and P5 are facts, not modules); a decline is a success and a
misclaim is a failure; `make gate` green before anything is promoted.

**But the push comes from this file, not from the gate.** The gate can only ever
tell us that we have not lied. It cannot tell us to build.
