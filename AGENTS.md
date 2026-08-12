# Working rules for agents

> ## ⛔ PRIMA DI TUTTO: [`MANTRA.md`](MANTRA.md)
>
> Qualunque sia il motivo per cui sei entrato in questa codebase — anche un
> dettaglio marginale, anche per caso — i mantra vanno passati **prima** di
> scrivere una riga. Non sono uno stile: sono il criterio con cui si decide se una
> modifica fa avanzare o regredire l'esperimento.
>
> La domanda zero: **è generalizzabile KB-first?** Il test: **"parrot0 può
> impararne un nuovo membro domani, senza ricompilare?"**

Read `PRINCIPLES.md` before changing parrot0. The KB-first rule is a release
constraint, not a preference.

## KB-first preflight (mandatory)

- Natural-language vocabulary belongs in the KB. This includes words that can
  look like parser plumbing: connectors, prepositions, quantifiers, range
  markers, question forms, synonyms, and multi-word cues.
- C may implement fixed mechanics such as tokenization, ordering, slot binding,
  arithmetic, and inference. It must not decide that literal words such as
  `between`, `from`, `and`, or `through` name those mechanics.
- Before adding `cue(...)`, `strstr(...)`, or `strcmp(...)` against a
  natural-language literal in `src/brain`, stop and put the form in a KB
  relation (`intent_cue`, `intent_phrase`, or a more specific fact), then query
  it through the shared matcher (`kb_cue_match`, `kb_intent_match`, or the
  universal evidence scorer).
- Every new linguistic recognizer needs a runtime-growth test: asserting a new
  cue must change recognition without rebuilding, and retracting/ablating the
  cue must remove that recognition. A fixed golden response alone is not proof
  of KB-first compliance.
- Generated wording follows the same rule: prefer `response_template` or another
  KB-backed frame over a new natural-language `printf`/`snprintf` literal.

The review question is: **could a user teach the new surface form at runtime and
have the existing engine use it without a C edit?** If not, the change is not
ready.
