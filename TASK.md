# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

> gen67 (M2 step 1, measurable book learning shift) is DONE — see JOURNAL.
> gen68 (meta-conversation wall: attention/reading/understanding/repetition) is DONE — see JOURNAL.
> gen69 (C6 step 2, polar meta-questions) is DONE — see JOURNAL.
> gen70 (C6 step 3, chat-sim validation + wall proxy fix + fallback word threshold) is DONE — see JOURNAL.
> gen71 (C7 step 1, apology markers + sono→am canonicalization) is DONE — see JOURNAL.
> gen65 symbolic challenge remains parked as the S-series in TASKLIST.md.

## Goal: C7 step 2 — expand Italian conversational lexicon

gen71 added apology markers ("scusa"/"mi dispiace"→"No problem.") and
"sono"→"am" canonicalization. The dominant residual gap from the cleaned
chat-sim transcripts is Italian content words hitting the fallback
word-reflecting path. The lexicon (180 stopwords, `knowledge/lexicon.pl`)
covers Italian function words well, but common conversational content words
("devi", "fare", "parliamo", "ancora", "vuol", "intendi", "formaggio",
"piccolo", "pappagallo", "volta", "niente", "nessun", "tranquillo",
"confusione", "battuta", "essere" etc.) leak past because they aren't
in the stopword list.

### Design question
Can we measurably reduce the wall rate on Italian turns by extending the
Italian lexicon without growing a phrasebook? The test: add stopword entries
for the most common Italian conversational content words observed in the
chat-sim transcripts ("devi", "fare", "parliamo", "ancora", "vuol",
"intendi", "scherzando", "rotto", "cambiamento", "formaggio",
"piccolo", "chiacchiere", "pappagallo", "volta"), and verify the
fallback no longer picks them for word-reflecting. Watch the phrasebook
boundary: these go into the existing `knowledge/lexicon.pl` as
`stopword(X)`, not as new intent patterns.

### Acceptance
- Add the observed Italian content words to `knowledge/lexicon.pl` as stopwords.
- Add `tests/cases/lexicon_it.chat` with cases showing these common Italian words
  no longer trigger word-reflecting ("Hmm, I don't know about X yet.").
- `make test` must stay green (all existing suites).
- `make chat-sim` re-run should show a measurable drop in Italian wall rate.
