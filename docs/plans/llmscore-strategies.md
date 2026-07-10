# LLMSCORE — la strategia profonda: il punteggio si ADDESTRA, non si programma

> 2026-07-10, v2 dopo lo steer di F.: la prima stesura proponeva handler C per
> classe di domanda — la leva LINEARE, il collo di bottiglia umano, l'impostore di
> LOOP.md. Ignorava il principio cardinale (kb-first) e soprattutto il **processo
> di addestramento via MCP** che l'intera serie U (gen279-289) ha costruito. Questa
> versione mette quello al centro. La v1 sopravvive come catalogo dei bersagli in
> appendice, riletta con la lente giusta: *lezione* o *motore*?

## 1. Il principio (ciò che la v1 mancava)

LLMSCORE misura un repertorio di abilità con domande sempre nuove. Due modi per
far crescere il repertorio:

- **Via superficiale**: un handler C per classe. Lineare (una gen = una classe),
  umano-limitata, e tradisce il cardinale: la capacità finisce compilata, cieca,
  non insegnabile.
- **Via profonda**: la capacità è CONOSCENZA (fatti, tabelle, clausole) sul motore
  fisso e arricchito — e il motore di oggi non è quello di un mese fa: U1..U6
  hanno dato quoting dei letterali, variabili `$`, clausole n-arie via MCP,
  unificazione strutturale (computazione ricorsiva: Peano!), `chars/2`, `naf`.
  Quella serie è stata costruita **esattamente perché un agente esterno possa
  ADDESTRARE parrot0 a runtime** (`docs/plans/teach-comprehension-via-mcp.md`).

> **La tesi: LLMSCORE è una misura di ADDESTRABILITÀ.** Si massimizza insegnando
> — via MCP, in sessioni di training che producono conoscenza committata — e si
> scrive C solo quando una lezione NON è esprimibile sul motore attuale: allora
> il fallimento della lezione NOMINA l'upgrade di motore (com'è già successo:
> D.1→U3, D.2→U6).

## 2. La prova dal vivo (2026-07-10, zero C, motore attuale)

Due dei sei zero del run fresco chiusi/mossi **solo insegnando via MCP**
(`kb.assert` → `kb.save` → chat con la sessione):

**Q8 (riddle match/candle/fireplace) — CHIUSO, zero C.** I riddle sono già pure
KB (gen254: `riddle_sig` + `response_template`). Lezione di 3 fatti:
```
riddle_sig(riddle_match, "dark room").
riddle_sig(riddle_match, "light first").
response_template(riddle_match, "The match -- you have to light it before it can light the candle or the fireplace.").
```
→ domanda del giudice verbatim → risposta esatta.

**Q10 (traduzione ES) — da muro a frase intera in 3 lezioni.** E qui la scoperta
metodologica: **il declino onesto è il segnale di gradiente**. parrot0 NOMINA il
fatto mancante ("I don't know the Spanish for *sleeping*") → si insegna
`tr_es(sleeping, durmiendo)` → nomina il prossimo ("*rug*") → `tr_es(rug,
alfombra)` → frase completa: `El gato esta durmiendo en el cálido alfombra.`

**E il residuo è la parte più istruttiva.** `el cálido` davanti a un femminile:
l'articolo ES prende il genere dal token adiacente (l'aggettivo) invece che dal
sostantivo-testa, e manca l'accordo aggettivale ES. Cioè: l'addestramento ha
FUNZIONATO sulle lezioni esprimibili e ha NOMINATO con precisione il gap di
motore residuo — che peraltro è il gemello ES di `agree_f`/`article` IT
(gen286-287): un'altra tabella `grammar.p0`, di nuovo conoscenza. Il ciclo si
regge da solo.

## 3. T0 — il TRAINER LOOP (la strategia madre)

```
probe (domanda-tipo del giudice, dalla banca §5)
  → risposta di parrot0
  → se corretta: inchioda (llmscore_world.sh) e passa oltre
  → se declino onesto: parrot0 ha NOMINATO il gap
       → formula la LEZIONE (fatti/tabelle/clausole)
       → insegna via MCP (kb.assert / kb.assert_clause)
       → ri-proba; se chiude: kb.save → conoscenza COMMITTATA (kb/*.p0)
  → se la lezione non attacca / non è esprimibile:
       → hai scoperto un GAP DI MOTORE, con il caso minimo già in mano
       → quello (e solo quello) diventa una generazione C, stile serie U
```

L'analogia è letterale e onesta: **il KB sono i pesi, la lezione è il passo di
gradiente, il declino onesto è la loss, MCP è l'interfaccia di training.** E il
loop è AUTOMATIZZABILE: l'agente esterno (o, riflessivamente, il deep-reasoning
loop di parrot0 stesso, che con `extract_page_facts` già *si insegna* fatti dalle
pagine) può condurre centinaia di sessioni di training senza toccare il C — la
leva diventa cumulativa invece che lineare. Le "generazioni" restano per il
motore; il repertorio cresce come **dati versionati** (`kb/knowledge/*.p0`,
disciplina knowledge-missions).

Prerequisito di igiene (dal §2): il trainer dipende dalla qualità del segnale —
i decline che NOMINANO il gap (il path ES lo fa; altri path murano generici).
"Ogni muro deve nominare ciò che manca" è quindi parte di T0, non cosmesi: è ciò
che rende l'addestramento automatizzabile.

## 4. I sei zero di oggi, riletti: lezione o motore?

| # | Classe | Lezione (KB, via MCP) | Motore (C, solo se serve) |
|---|---|---|---|
| Q4 | detto famoso | `saying/2` + cue — puri fatti | serve il piccolo consumer che consulta `saying` (una volta, generico: poi ogni detto è dato) |
| Q8 | riddle classico | ✅ **fatto oggi, zero C** | — |
| Q10 | gloss ES | ✅ 3 fatti oggi | accordo genere/aggettivo ES = tabelle grammar.p0 + fix del lookup sul sostantivo-testa |
| Q2 | area+perimetro | `formula(rectangle, area, …)` come dato; i VALORI via motore | valutatore aritmetico minimo se le clausole (Peano/`chars`) non bastano in pratica |
| Q6 | word-problem multi-step | `verb_op(eat, sub)`, `verb_op(buy, add)` — lessico-operazioni come KB (precedente: `magnitude_cue` gen250) | il walker di stato che applica la catena (una volta, generico) |
| Q9 | qualità/opinione | `quality_of(friend, loyalty)` + perché relazionale — puri fatti | il compositore della risposta (una volta, generico) |

Il pattern è costante: **il contenuto è sempre dati; il C ammesso è solo il
consumer generico che li consulta** (come `kb_intent_match`, `riddle_sig`,
`magnitude_cue` — scritti una volta, estesi per sempre via dati). Mai un branch
per-domanda.

## 5. La misura (ex-S6, ridimensionata al suo posto)

Il ratchet esiste già (`tests/llmscore_world.sh`: ogni domanda-giudice risolta è
inchiodata, gira in `make test`). Manca la **mappa rossa**: le domande ancora
aperte, per asse di abilità, con coverage % (il gemello di `basic-chat.md`).
Nel quadro T0 la mappa rossa è la coda di training: ogni riga rossa è un probe
del trainer loop, e la domanda giusta su ogni rosso è "qual è la lezione?" —
non "quale handler scrivo?".

## 6. Roadmap (per leva, versione profonda)

1. **T0.a — igiene del segnale**: i muri generici sulle classi llmscore diventano
   decline che nominano il gap (il pattern ES/IT già in casa). Rende ogni rosso
   addestrabile.
2. **T0.b — la mappa rossa** (ex-S6): catalogo `llm-abilities.md` per asse +
   bench di coverage; i 6 zero di oggi sono le prime righe (Q8 già verde, oggi).
3. **T0.c — sessioni di training via MCP** sui rossi a pura-lezione (Q4, Q9,
   glossari, detti, riddle, qualità): conoscenza committata in `kb/knowledge/`.
   Dove serve il consumer generico mancante (saying, quality_of), UNA gen C lo
   apre e poi tutto il resto è dati.
4. **T0.d — i gap di motore nominati dal training** (accordo ES, valutatore
   aritmetico per formule, walker di stato): generazioni C classiche, gate-first,
   ognuna tirata da una lezione fallita documentata — mai speculativa.
5. **T0.e — il trainer automatizzato — FATTO (gen305, `make autolearn`).**
   `tests/autolearn.py`: un modello opencode-GO (`$OPENCODE_API_KEY`, default
   minimax-m2.5) gioca interviewer/judge/teacher attorno a `--mcp-engine`.
   Probe → giudizio → su 0 il teacher legge il declino che nomina il gap e
   formula la LEZIONE (soli predicati whitelisted con consumer verificato) →
   `kb.assert`+`kb.save` → re-probe, con retry finché il declino nomina gap nuovi
   (il gradient loop). **Oracolo di onestà**: la conoscenza persiste in
   `kb/learning/autolearn.p0` SOLO se il giudizio flippa 0→1; altrimenti rollback
   + voce nel FAILED-LESSON LEDGER (`AUTOLEARN.md`) — la coda che nomina i gap di
   motore. Provato dal vivo: riddle nuovo inventato dall'interviewer → muro →
   lezione → chiuso e persistito, zero C; la traduzione ES è finita onestamente
   nel ledger (accordo di genere = gap di motore, confermato in autonomia).
   Modalità controllata: `make autolearn ROUNDS=n PROBES=file`. Lezioni imparate
   sul trainer stesso: cue dei riddle in minuscolo (il match è sul norm), id
   freschi nei retry (i cue si accumulano per id), token generosi al teacher
   (reasoning server-side). Qui la curva cambia pendenza:
   punti-per-generazione → punti-per-sessione-di-training.
6. **Convergenza col deep-reasoning**: S1 ("mai il muro") resta il raccolto del
   piano parallelo — al fallback, parrot0 *si addestra da solo* in-turn
   (acquisizione+estrazione = auto-lezione). Stessa forma di lezione, insegnante
   diverso (il mondo invece dell'agente).

## 7. Guardrail (invariati, più uno)

- **No deception**: si insegna conoscenza vera e citabile (`fact_source`); il
  punto si vince con capacità onesta o non si vince.
- **Categorie, non prompt** (gen254): la lezione copre la classe (il riddle
  generico via `riddle_sig`, non la stringa del giudice); i probe della banca
  sono rappresentanti, non risposte da recitare.
- **Additività**: i consumer generici si aggiungono, non sostituiscono.
- **Ratchet bilingue**: le lezioni valgono per path, non per lingua.
- **Nuovo — confine lezione/motore esplicito**: ogni gen C su questo fronte deve
  citare la lezione fallita che l'ha tirata (come U3 citava D.1). C senza una
  lezione fallita a monte = tornati alla via superficiale.

---

## Appendice — catalogo v1 (i bersagli restano validi, la via cambia)

S1 mai-il-muro (→ §6.6, auto-lezione in-turn) · S2 campagna di lettura (→ T0.c di
massa: il teacher automatico) · S3 istruzioni-come-programmi (→ regole del gioco
come clausole; il motore U3 già computa — Peano dimostrato; eventuale builtin
aritmetico = gap di motore da lezione fallita) · S4 rendering (→ `response_template`
è già KB; l'elaborazione deriva da `kb_explain`) · S5 routing-che-impara (→
`intent_cue`/`learnable` sono già il routing KB: estenderli, Track 5) · S6 sparring
(→ §5, la mappa rossa del trainer) · S7 creatività acquisita (→ imagery come fatti
estratti) · S8 word-problem (→ `verb_op` lessico + walker generico) · S9 qualità
(→ `quality_of` fatti + compositore generico).

Dati storici: run 2026-06-28/07-02/07-10 tutti 4/10 ma su classi DIVERSE — il
progresso c'è (sillogismi, riddle logici, word-problem semplici, composte ora
passano) e conferma che si vince per classe. Nota di onestà: in Q3 il giudice ha
accettato un fatto storto (Sydney Opera House a Canberra) — non si sfrutta; la
qualità dei fatti conta anche quando il giudice non se ne accorge.
