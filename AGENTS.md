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

## Le trappole del dialetto `.p0` (misurate, non teoriche)

Un file `.p0` che non carica **non dice quasi niente**: una riga su stderr al
boot, che nessuno guarda. Prima di dare la colpa al motore, controllala:

```sh
echo '/quit' | PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 \
  ./bin/parrot0 2>&1 >/dev/null | grep 'PARSE ERROR'
```

I due soffitti sono dichiarati in `src/kb.h`, e sono l'unica ragione per cui una
regola ben scritta viene scartata:

| limite | valore | che cosa cade |
|---|---|---|
| `KB_MAX_ARGS` | **4** | un goal con 5+ argomenti — testa **o corpo** |
| `KB_MAX_BODY` | **8** | un corpo con 9+ goal |

⚠ Sforare l'**arietà** non produce un messaggio dedicato: dice solo `bad rule,
dropped`, e la regola non esiste. Al gen505q ho perso un giro dietro alla lore
sbagliata («troppe variabili») che avevo scritto io stesso: il numero di
variabili non conta, contano gli **argomenti per goal**. La cura è dare un nome
ai pezzi intermedi — un predicato di arietà 3 che calcola un lato — non togliere
variabili.

Le altre due, già annotate in `kb/core/composition.p0`:

- **`naf` con una variabile libera non lega.** `naf(p($X, $Any))` fallisce
  sempre; serve un aiutante di arietà 1 interamente legato.
- **La canonicalizzazione abbassa le maiuscole**, quindi un literal `.p0` citato
  in un turno perde le maiuscole delle variabili.

Tutte e tre si mascherano allo stesso modo: **non un errore, zero soluzioni.**
