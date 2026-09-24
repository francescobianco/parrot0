# RI-020 — un nome di relazione si lega a una relazione che la KB tiene già

**Lotto:** `2026-09-24` · **Stato:** completa · **Classificazione:** trained

## Famiglia e contesto umano
Proprietà fisiche per nome: chi chiede «what is the boiling point of water?»
usa il **nome** della proprietà, mentre la KB le tiene sotto il **verbo**
(`boils_at/2`, `freezes_at/2`, da gen241). Il dato c'era, la maniglia no.

## Stato iniziale
Commit `729153c6` (chiusura di RI-019), profilo `agi`. `stato.txt`.

## Limite osservato — `prima-dialogo.txt`
- «What is the boiling point of water?» → **la definizione dell'acqua** (peggio di un muro, mantra #7);
- «What is the freezing point of mercury?» → un paragrafo sull'acqua che ghiaccia;
- la lezione naturale «the boiling point of x is y means x boils at y» → *«I cannot
  anchor that lesson yet: I do not have one unambiguous reading for x boils at y»*.

## Diagnosi con il trace unico — e dove il trace taceva
Il lavoro è stato fatto tutto dal trace (`/debug`, `/debug trace <parola>`); tre
siti erano muti e ora parlano:
1. **`read.project`** (nuovo): `answer_projection_resolve` sceglieva il topic per
   evidenza e stampava un riassunto senza lasciare righe — il trace diceva solo
   «answerframe answers». Ora: `semantic_summary via semantic_topic_cue -> water
   (score 3) because …`, `no question focus, water unchecked`, `wiki_concept(water)
   speaks`.
2. **`read.polar`** (nuovo): il verdetto polare che dice «I don't know: nothing I
   hold says …» nomina la relazione e gli argomenti che ha interrogato.
3. **`read.form … is no relation noun, asking the frame reader`**, **`lesson.anchor`**,
   **`lesson.forget`**, e la prova a secco ora dice *su quale testo* («the frame
   reader can answer «…»»).

La catena, in ordine:
- la lezione non si ancorava: il lato destro «x boils at y» non ha uno schema;
- dopo l'ancora, la domanda si legge sul verso inverso («@O is the boiling point of
  @S»), che la costruzione non dava;
- il lettore dei frame **sapeva** rispondere (prova a secco), il frasario cedeva,
  ma l'atto della forma `ask_noun_of` conosceva solo `relation_noun/2` e il turno
  finiva al verdetto polare («I don't know») — due lettori che non si accordavano;
- «What's …»: la prova a secco girava sul testo **prima** della canonizzazione.

## Supporti generali (nessuna parola di dominio nel C)
- `relation_named_by/2` (grammar.p0): una relazione binaria del mondo è nominata
  dalle sue parole; l'allineatore della lezione **la chiede** quando nessuno schema
  legge il lato destro (`p0_align_named_relation`).
- `construction_reading/2`: una costruzione che finisce con copula + valore si
  legge nei due versi (la specificazione copulare).
- l'atto `answer_noun_of` chiede allo **stesso** lettore dei frame quando il nome
  non è un `relation_noun`.
- `p0_publish_frame_answer`: una funzione sola, chiamata sul testo detto e sul
  canonizzato.
- il ritiro prova anche la frase intera («forget that THE boiling point …»).

**Lezione nuova resa possibile:** qualunque «the <nome> of x is y means x <verbo> y»
verso una relazione che la KB già tiene — un membro nuovo costa una frase.

## Curriculum (fonti: CRC Handbook; valori già in KB da gen241)
1. `the boiling point of x is y means x boils at y` (C)
2. `the freezing point of x is y means x freezes at y` (C) — relazione diversa, stessa via
3. `The boiling point of acetone is 56 degrees Celsius.` (W) — letto dalla forma insegnata

## Certificazione sul meccanismo fermo — `certificazione-dialogo.txt`
- stimolo: «100 degrees Celsius (212 degrees Fahrenheit) at sea level …»
- transfer 1 (altra formulazione, altra sostanza): «What's the boiling point of ethanol?» → 78 °C
- transfer 2 (altra relazione, contrazione): «What's the freezing point of mercury?» → −39 °C; acqua → 0 °C
- nuovo dato letto e interrogato: acetone → 56 °C
- contrasti: «What is the boiling point of mercury?» → «I don't know» (nessun fatto:
  niente invenzioni, niente riassunto); «What is water?», Paris, USB 5 V invariati
- ablazione: `forget that the boiling point of x is y means x boils at y` → la
  domanda torna alla definizione dell'acqua; il controllo indipendente (mercurio,
  congelamento) resta.

## Salvataggio e processo nuovo — `salvataggio*.txt`, `processo-nuovo-dialogo.txt`
`/save` 22 clausole: 2 `construction_frame` in english-grammar/constructions.p0
(**C=2**), `boils_at(acetone, 56_degrees_celsius)` in science-nature.p0 accanto ai
fratelli (**W=1**), provenienza e transcript (P). **L=0, X=0.** Processo nuovo:
stimolo, transfer, acetone rispondono; replay RI-016..019 verde.

## Verifiche
`make soft-test` verde in 14 s (budget 15). Banco L2 60/60.

## Limiti e prossimo problema
- «Tell me the boiling point of ethanol.» (direttiva) → «I don't know about boiling»:
  la prova a secco e la forma chiedono la faccia di una domanda.
- Senza lezione, «what is the R of X» con R ignoto riceve ancora il riassunto di X:
  il fuoco della domanda non esiste per «of» (mantra #22, «of» fuori da
  `domain_preposition`). Candidato: una regola `asked_head_misses` per «R of X».
- I valori gen241 sono frasi (`"100 degrees Celsius … steam"`), non misure; e il
  ramo C che risponde «At what temperature does water boil?» vale solo per l'acqua
  (catena compilata, gen489): debito, non toccato.
