# Knowledge Base TODO

## Popular wisdom and proverbs

Add popular wisdom from multiple cultures as atomized, inferable knowledge,
not as a phrasebook or a flat collection of quotations.

The KB should separate the surface form of a proverb from the principle it
expresses. Candidate relations include:

```prolog
proverb_surface(Proverb, Language, Text).
proverb_origin(Proverb, Culture).
proverb_expresses(Proverb, Principle).
principle_condition(Principle, Condition).
principle_tendency(Principle, Consequence).
principle_recommends(Principle, Action).
principle_warns_against(Principle, Action).
principle_tradeoff(Principle, Benefit, Cost).
principle_applies_to(Principle, Domain).
principle_tension(Principle, OtherPrinciple).
```

Rules should reason over principles, conditions, consequences, advice, and
tensions independently of the original wording. Equivalent sayings from
different languages or cultures should therefore converge on a shared
principle instead of duplicating their meaning as prose.

Before considering this complete:

- cover several cultural traditions without presenting one as universal;
- preserve provenance and language for every surface form;
- model context, exceptions, and conflicting principles where applicable;
- add conversation intents and response frames in the KB, not as C literals;
- add `.p0t` coverage for inference and conversational use;
- prove runtime growth: teaching or retracting a surface form or principle must
  change recognition and inference without rebuilding Parrot0.

The review question remains: can Parrot0 learn a new proverb, map it to an
existing or newly taught principle, and use it in conversation without a C
change?
