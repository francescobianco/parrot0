# Distance report — SWE-bench_Lite `astropy__astropy-12907`

The gap between **what parrot0 does today** and **what solving this real instance
requires**, measured on the REAL file (fetched at the base commit; the gold patch
names `astropy/modeling/separable.py::_cstack`). gen196 added Python "by delta"
(one front-end emitting the same abstract facts the C engine reasons over).

Repro: `make swe-bench`; the snippets below are real parrot0 output (gen196,
`PARROT0_BASE= PARROT0_SESSION=`).

---

## The instance
- **Issue:** *Modeling's `separability_matrix` does not compute separability
  correctly for nested CompoundModels.*
- **Gold patch** (1 line, in `_cstack`):
  ```diff
  -        cright[-right.shape[0]:, -right.shape[1]:] = 1
  +        cright[-right.shape[0]:, -right.shape[1]:] = right
  ```
- **Hidden test:** `astropy/modeling/tests/test_separable.py::test_separable[...]`.

---

## What parrot0 CAN do today (gen196) — real output on the real file

| Faculty | Question | parrot0 |
|---|---|---|
| F2 read defs (Python) | "which functions are defined in …/separable.py" | `it defines is_separable, separability_matrix, _compute_n_outputs, _arith_oper, _n_inputs_outputs, _coord_matrix, _cstack, _cdot, … and _separable.` |
| F3 call graph (Python) | "what does _cstack call …" | `_cstack calls array, sum, where, ones, isinstance, format, zeros, append, vstack, roll, hstack and dot.` |
| F4 locate by name | "which file under … defines _cstack" | `_cstack is defined in astropy/modeling/separable.py.` |

So a real 317-line astropy Python file already enters the **same** KB substrate as
C and answers structural questions — the language-as-delta bet works at the
read/structure layer.

## What parrot0 CANNOT do yet — submit the issue, get the wall

| Question | parrot0 | Target |
|---|---|---|
| the issue text as a turn | `I don't understand that yet.` | localize the issue to `_cstack` and propose the fix |

---

## The distance, itemized (each is a TASKLIST X-pull)

1. **Issue → code localization (F4 semantic, X6).** Today locate works only by an
   exact function NAME the user supplies. The issue text never says "_cstack"; it
   describes a *symptom* ("nested CompoundModels"). Mapping symptom → `_cstack` is
   the associative step CODE-MASTERY §3/§4 flags as the hard frontier. **Largest gap.**
2. **Python semantics (F3 deep).** parrot0 sees `_cstack` *calls* numpy ops but has
   no model of array assignment / broadcasting, so it cannot reason that `= 1` vs
   `= right` changes the result. C symbolic-eval (`code_eval`) does not cover Python.
3. **Patch synthesis (X7).** No transformation yet rewrites `= 1` → `= right`
   (rename/delete exist; an expression-level edit does not). The *what* to change is
   gated on (1)+(2); the *how* is a new AST transformation.
4. **Run grounding (X1) + repo/env.** Verifying the fix needs the astropy repo at
   the base commit + a Python env to run pytest. parrot0 has no Python runtime
   oracle; today the only grounded oracle is the C compiler.

## One-line summary
gen196 closed the **read/structure** distance for Python (defines, calls, locate
on the real file) by deriving Python as a delta of C on one shared engine. The
remaining distance is **meaning + action**: semantic Python reasoning, issue→code
localization, patch synthesis, and a Python run-oracle — the ordered X-series pulls.
