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

## P0 — parrot0 FABRICATES a code defect (honesty regression) — **DONE, gen322**

> **Closed.** The syntax claim is now vetoed by `cc -fsyntax-only`
> (`code_syntax_ok()`), the oracle that was already in the tree: if the compiler
> accepts the translation unit, a syntax finding is false by construction. The
> veto can only refute a finding, never invent one. The off-by-one at
> `70-social-pragma.c` is fixed too, but that alone would have left the whole
> class alive. The reply now names its own limit — *"I compiled it: no syntax
> errors. That is all I checked: I cannot yet tell you whether it computes what
> you intended."* — so the deliberately-wrong `i <= n` is ratcheted as a case
> parrot0 must NOT claim to solve.
>
> **The lesson that generalizes, and the reason this was invisible:** every
> existing case fed the checkers BROKEN code. Not one fed them CORRECT code. A
> checker is only trustworthy once it has been shown to stay **silent**, and
> 25/25 green gates were measuring the half of the contract that could not fail.
> **Every `check_*` in the tree still needs its correct-input negative.** That is
> the next honesty sweep, and it is cheap.

<details>
<summary>original report (kept as the counterexample that produced the fix)</summary>

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

</details>

---

## P1 — the code faculty is real but UNREACHABLE through language (the keyhole) — **DONE, gen323**

> **Closed.** The five question types are now `intent_cue` FACTS in
> `kb/core/intents.p0` (54 cues, EN+IT) read through `kb_cue_match`, so the
> surface grows at runtime: a new way of asking costs a fact, not a generation.
> The C cue-chain counter fell **317 → 248** — the surface grew while the
> phrasebook shrank.
>
> **What widening exposed, and why it had to be paired.** Loosening the cues
> without tightening the register would have handed prose to the code module. It
> turned out prose was ALREADY being taken: `identify_code_lang` tallied
> `keyword()` facts word by word, and `is`/`in`/`not`/`and`/`for` are Python
> keywords *and* ordinary English words — so *"what language is this: the sky is
> blue"* answered *"This looks like Python code."*, and *"what is wrong with
> this: the sky is blue"* answered *"I did not find obvious errors in this code
> snippet."* A second live misclaim, found while fixing the first.
> `has_code_register()` now demands a mark prose does not make; no structural
> evidence ⇒ the module declines the turn (TASKLIST C14).
>
> **Still open here:** the explainer itself is thin — *"This is a C code snippet.
> It returns a value. It contains a loop."* is reachable now, but it is not an
> explanation. It should name the function, its parameters, and what the loop
> accumulates. That needs P2 (the evaluator must understand a loop before it can
> describe one), which is why it is next.

<details>
<summary>original report (kept as the counterexample that produced the fix)</summary>

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

</details>

---

## P2 — ~~the evaluator cannot execute a LOOP~~ → it cannot do `s += i` — **DONE, gen324**

> **This row was MISDIAGNOSED, and the code refuted it.** The evaluator executes
> loops fine (gen178 bounded `while`, gen179 three-clause `for`). I inferred the
> cause from parrot0's symptom instead of reading the evaluator. The real wall was
> COMPOUND ASSIGNMENT — `s += i`, the one statement almost every loop body is made
> of, which is why every real loop looked like a loop failure. Desugared to
> `x = x OP e` through the same expression parser; `/=` and `%=` by zero refuse
> rather than fabricate an integer. The wrong diagnosis is left visible below: a
> plan is allowed to be wrong, but not allowed to hide it.

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

## P3 — synthesis has exactly three schemas — **DONE (seed), gen328**

> **The impostor was the curation.** `algo_shape(bubblesort, …)` was written by a
> human, so every new algorithm cost a generation — growth n buys capability n and
> nothing compounds.
>
> A page of steps now produces its OWN schema: the page is reduced to the set of
> step kinds it describes (`step_cue` facts), and a shape is induced only when
> every kind its signature requires is present (`shape_signature` facts). Both are
> knowledge. **The algorithm's name is never read** — the ratchet generates a
> nonsense name per run and asserts it appears in no knowledge file. The chain is
> causal both ways: without the page it cannot be built (ablation), with the page
> it is synthesized and disposed by a real run on 8 vectors.
>
> A page for a DIFFERENT structure (insertion sort shifts and inserts) is declined
> naming the step it could not find, rather than force-fitted into the one shape
> we have. Inventing a schema to have one is gen322's fabrication in a new costume.
>
> **Still curated: the SHAPE LIBRARY itself** (one entry). Inducing a shape nobody
> has implemented is synthesis of a control structure, not recognition of one — a
> larger problem, and claiming otherwise would overstate this row.

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

## P4 — there is no agentic loop (goal → act → observe → verify → repair) — **DONE (seed), gen327**

> **Closed at its smallest honest seam.** A candidate the oracle REFUTED used to
> be abandoned — no diagnosis, no alternative, no second attempt: a one-shot
> generator with a test attached. Now the loop runs, and every step is grounded in
> a real compile-and-run: OBSERVE (the judge) → DIAGNOSE (a STRUCTURED verdict:
> `not_ordered` vs `not_permutation` — "it failed" cannot direct a repair) →
> PROPOSE (`repair_rule(Diagnosis, Fix)` FACTS; the C knows only HOW to transform,
> never WHICH) → SELECT (smallest blast radius) → ACT → VERIFY (re-run the same
> judge) → STOP within N, trace kept.
>
> Correct code is run, passes, and nothing is invented to fix it. A defect no rule
> repairs is DECLINED naming the real diagnosis, and **no patch is emitted** — a
> patch nobody ran is a fabrication.
>
> **What this is NOT:** it is a repair loop over one oracle (sort), not the typed
> Event/Goal/Action/Observation kernel of forge §6, and not issue→localize→patch
> on a cold repo. `make swe-bench` still engages 0 of 5. The seed is real; the
> field is not claimed.

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

## P5 — a nonce-word syllogism walls — **DONE, gen326**

> **Closed — and the row was half wrong, in an instructive way.** parrot0 has had
> premises-in-the-turn reasoning since gen231 (nonce vocabulary, multi-link
> chains, honest refusals, a scratch KB that pollutes nothing). I did not check,
> and nearly shipped a SECOND syllogism module beside it — the duplication
> PRINCIPLES forbids. It was already stealing `transitivity.chat` turns before I
> found the original. Deleted.
>
> The real gap was ONE question form: gen231 resolved only a GROUND conclusion
> ("is socrates a mortal?"). The UNIVERSAL form its own premises entail ("are all
> bloops lazzies?") walled, because it names no individual to look up. A universal
> is now proved the way a universal is proved — an ARBITRARY WITNESS of the
> subject class, asserted into the scratch KB the core already builds, resolved by
> the SAME query path. 55 lines, one helper, both surfaces.
>
> **Method note for the next row: read the code before writing the plan.** Two of
> my six TODO rows (P2, P5) misdiagnosed a capability parrot0 already had.

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

## P6 — a routing collision: "what are you unable to do?" — **DONE, gen325**

> **Closed, and it was worse than a collision.** In Italian, "cosa non sai fare?"
> returned the list of what parrot0 CAN do — the question inverted, the answer
> still a brochure. The real defect: parrot0 had no notion of its envelope,
> because the self-model derived only from `module(name)`.
>
> The answer is now DERIVED from the capability ledger (forge §18) —
> `capability/2` + `capability_wall/2`, projected by `tests/capability_facts.py`
> from the manifest the gates verify. No faculty name appears in the C.
>
> Two non-obvious things. **Normalization erases the negation** ("cosa non sai
> fare" and "cosa sai fare" canonicalize identically), so the branch reads the RAW
> turn. And **the self-model is not world knowledge** — loaded as KB_BASE it made
> a hermetic brain claim "I know 24 fact(s)", so it joins `module`/`i_am` in the
> internal-predicate filter. The ratchet is an ABLATION (8/8): add a faculty and
> it appears, remove one and it disappears. A self-description that cannot shrink
> is a brochure.

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
| ~~P0~~ | ~~the fabrication~~ | **DONE gen322** — grounded the syntax claim in the compiler that was already in the tree. Left behind: every other `check_*` still needs its correct-input negative |
| ~~P1~~ | ~~the code keyhole~~ | **DONE gen323** — cues became FACTS (cue-chains 317 -> 248); tightening the register in the same gen killed a second live misclaim (prose classified as Python because `is` is a keyword) |
| ~~P6~~ | ~~the routing collision~~ | **DONE gen325** — the self-model now derives its LIMITS from the ledger the gates verify (forge §18) |
| ~~P2~~ | ~~loops in the evaluator~~ | **DONE gen324** — the row was misdiagnosed: loops already worked, `+=` did not |
| ~~P5~~ | ~~in-turn rule intake~~ | **DONE gen326** — it existed since gen231; the real gap was the UNIVERSAL conclusion, 55 lines inside the existing core |
| ~~P4~~ | ~~the agentic loop~~ | **DONE (seed) gen327** — observe/diagnose/propose/verify, bounded, over one real oracle. NOT the typed kernel, NOT cold-repo agency: swe-bench still engages 0/5 |
| ~~P3~~ | ~~schema induction~~ | **DONE (seed) gen328** — a fresh page induces its own schema, structurally (the name is never read). The SHAPE LIBRARY is still curated |

**The discipline stays.** One contract per generation; seal the red first; prefer
knowledge over C (P1 and P5 are facts, not modules); a decline is a success and a
misclaim is a failure; `make gate` green before anything is promoted.

**But the push comes from this file, not from the gate.** The gate can only ever
tell us that we have not lied. It cannot tell us to build.
