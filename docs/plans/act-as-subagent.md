# parrot0 come subagent — il protocollo del round guidato

> **Stato:** primo round completo eseguito a **gen337 (2026-07-18)** — T16 di
> [[coding-agent-todo]] chiuso (parte catene `cue(`) **da parrot0 stesso**,
> guidato a prompt da un LLM supervisore (Claude).
> **Ruolo di questo file:** è il documento **richiamabile** per eseguire un
> altro round: un LLM (o un umano) lo legge, prepara la sessione, insegna,
> comanda, integra e verifica. Ogni sezione è operativa.
> **Subordinato a** [[kb-first-manifesto]] (engine fixed, knowledge learns),
> [[universal-input]] (nessun flusso si classifica nel C),
> [[the-linguistic-glue]] (i sintomi di colla assente sono la diagnostica del
> round), e alla direttiva di F. sul doppio piano di conoscenza (§1).

---

## 1. Il principio: due piani di conoscenza (direttiva di F., 2026-07-18)

- **Conoscenza di sessione** — ciò che parrot0 scopre della codebase su cui
  lavora (la propria inclusa: outer circle, nessun accesso privilegiato). Si
  ricostruisce ogni round con i suoi tool (`grep`, `read`, `list`, lo scanner
  orchain). NON si committa, NON si cura a mano in `kb/`.
- **Conoscenza strutturale** — le *strutture cognitive* che rendono possibile
  **insegnare via prompt**: i teach-frame, le classi lessicali, i consumer di
  comprensione, il dominio di piano. Questa si conserva (è in `kb/core/`,
  `kb/experts/codebase/actions.p0` e nel motore) e cresce solo quando un round
  scopre un frame mancante.
- **Corollario (l'errore da non ripetere):** NON infilare i *concetti* (cosa
  significa kb-first, cosa è una catena cue) come fatti curati in `kb/` — quel
  contenuto si insegna a prompt col preambolo (§3). In `kb/` vanno solo le
  strutture che rendono quell'insegnamento comprensibile e richiamabile.

## 2. La sessione: come si avvia e si pilota

Ambiente (lo stesso di `make chat-agent`):

```sh
PARROT0_TOOLS=1 PARROT0_PROFILE=kb/profiles/agi.p0 PARROT0_EOT='<<EOT>>' ./bin/parrot0
```

Tecnica di pilotaggio **replay-driver** (robusta per un LLM che lavora a
passi): tieni un file `transcript.txt` con un prompt per riga; a ogni passo
appendi i prompt nuovi e ridai in pasto l'intero transcript a un processo
fresco (il motore è deterministico: lo stato si ricostruisce identico).
`PARROT0_EOT` incornicia ogni turno, così ogni risposta si allinea al suo
prompt:

```sh
{ cat transcript.txt; printf '/quit\n'; } | PARROT0_TOOLS=1 \
  PARROT0_PROFILE=kb/profiles/agi.p0 PARROT0_EOT='<<EOT>>' ./bin/parrot0
```

## 3. Il preambolo: insegnare kb-first PRIMA di comandare

Il preambolo conservato è **`scripts/prompts/kbfirst-preamble.txt`**: 8 prompt
`learn the definition of <termine>: "<definizione>"` che danno significato a
kb-first, catena-cue, impostore, intent-cue, migrazione, outer-circle e alla
distinzione sessione/strutturale. Si dà in testa a OGNI round:

```sh
grep -v '^#' scripts/prompts/kbfirst-preamble.txt | grep -v '^$' > transcript.txt
```

Poi si **verifica che la lezione sia attecchita** (comprensione = conoscenza):

```
what is kb-first?          → kb-first is engine fixed, knowledge learns - …
what is cue-migration?     → cue-migration is replacing a cue chain with …
```

**La regola del passo indietro:** se un tuo comando dà per scontato un
concetto e parrot0 mura o risponde fuori contesto, NON riformulare a forza il
comando: torna al preambolo e insegna il concetto mancante con un prompt
`learn the definition of …`, poi ripeti il comando. Se il *frame* stesso non
esiste (parrot0 non capisce nemmeno l'insegnamento), quello è un gap
STRUTTURALE: si chiude nel motore/KB core (vedi §6) e si annota qui.

## 4. I comandi di lavoro (il round T16 come modello)

1. **Consapevolezza dell'albero** (conoscenza di sessione):
   `execute the kb-first plan for chains of calls to cue in src/brain`
   → parrot0 esegue il piano derivato (dominio `kbfirst_migration` in
   `kb/experts/codebase/actions.p0`): scan dell'albero, file più denso,
   vocabolario estratto; si ferma onesto dove il passo chiede un file singolo.
2. **Piano completo sul file bersaglio:**
   `execute the kb-first plan for chains of calls to cue in src/brain/60-agent-tools.c`
   → scan → vocab → `emit_facts` (scrive `<file>.cues.p0`) → `patch_chains`
   (scrive `<file>.p0fix` con `kb_cue_match(b, KEY, ARG)`, forma letta da
   `lookup_call/2` in KB) → stop onesto al verify (file = frammento di TU:
   giudica la suite del progetto).
3. **Domande di controllo** (facoltative ma utili): parrot0 ha i tool read-only
   (`grep …`, `read …`, `list files …`) per rispondere su ciò che ha visto.

## 5. L'integrazione (le mani del supervisore, guidate dagli artefatti)

parrot0 produce artefatti non distruttivi; il supervisore li integra:

1. `cp <file>.p0fix <file>` — la patch diventa il sorgente.
2. I fatti di `<file>.cues.p0` si accodano a **`kb/core/intents.p0`** (che è
   caricata SEMPRE da `brain_create`, anche nei test ermetici) con un header
   che dice chi li ha derivati; le chiavi `<stem>_chainNN` sono la convenzione
   gen271.
3. Dove sensato, righe **`learnable/3`** (EN+IT) sulle chiavi migrate, così il
   vocabolario cresce a prompt («learn "…" as a letter-count phrase»).
4. `rm` degli artefatti; rebuild.

## 6. La verifica (differenziale, mai autoassolutoria)

- **Baseline PRIMA di toccare:** `make test` + `tests/cuechains.sh` sull'albero
  pulito; salva la LISTA dei FAIL, non solo il conteggio. (A gen337 la baseline
  main era rossa: 73 casi `.it` ereditati da gen334 — per questo il criterio è
  il **fail-set byte-identico**, non il verde assoluto, finché quel debito non
  è risolto.)
- **Dopo l'integrazione:** build a 0 warning; fail-set identico alla baseline
  (`diff` delle liste); `cuechains` DEVE scendere → abbassa `MAX` in
  `tests/cuechains.sh` al conteggio nuovo.
- **Runtime-growth (obbligatorio per ogni migrazione):** un ratchet `.chat`
  ermetico che insegna una frase NUOVA a runtime e mostra che il riconoscitore
  migrato scatta senza rebuild (modello: `tests/cases/toolvocab_growth.chat`).
- **Aggiorna i piani:** la voce del task in `coding-agent-todo.md` (stato +
  residui onesti), questo file (gotcha nuovi in §7), VERSION.

## 7. I gotcha scoperti (leggili PRIMA del prossimo round)

- **`KB_TERM_LEN` = 128:** una definizione insegnata sopra ~125 caratteri non
  entra in un fatto. Il motore ora declina onesto («too long … can you shorten
  it?», gen337) invece di lasciare il turno al generatore di storie. Tieni le
  definizioni del preambolo corte.
- **Il gate narrativo mangia i comandi lunghi:** un turno >80 caratteri senza
  `?` veniva reclamato come "storia da continuare". Cura strutturale gen337:
  classe KB `imperative_opener/1` (lexicon.p0) interrogata dal gate — un
  opener nuovo è un fatto. Se un comando lungo riceve una storia, insegna
  l'opener, non riscrivere il comando.
- **Cap dei registri:** `learnable/3` era letto con un cap di 32 righe (il
  registry ne ha 48+): le righe oltre il cap erano silenziosamente
  non-insegnabili. Alzato a 96 (gen337). Se un teach non attecchisce, sospetta
  un cap di lettura.
- **`emit_facts`/`patch_chains` vogliono un file singolo**, lo scan accetta
  l'albero: prima l'albero (dove), poi file per file (cosa).
- **Gli `strstr` non sono catene `cue(`:** la macchina Track 5 non li vede.
  Vanno migrati a mano come classi lessicali (il modello è
  `imperative_opener/1`). Residuo aperto in 60-agent-tools:
  `until/finch/fino a`, `even/pari/odd/dispari`, ` in `.
- **Il profilo `agi` è il profilo della sessione** (`make chat-agent`): carica
  gli expert. Senza profilo le risposte "what is <concetto di dominio>"
  cambiano. Confronta sempre a parità di ambiente.
- **Baseline rossa ereditata (da gen334):** la canonicalizzazione IT→EN via
  `tr/2` (`b11373b`) ha lasciato 73 casi `.it` rossi (si aspettano predicati
  italiani conservati: `uomo(socrate)`, non `man(socrate)`). Decisione di
  design di F., non regressione dei round: finché non viene sciolta, ogni
  verifica è differenziale sul fail-set.

## 8. La coda del prossimo round (dal più tirato)

1. **Residuo strstr di 60-agent-tools** (chiude T16 al 100%): classi lessicali
   KB per `until/finch/fino a` e `even/pari/odd/dispari`.
2. **T17 — `80-code.c`** (31 catene, stessa tecnica: il piano derivato già
   accetta qualunque file).
3. **T18 — i grandi residui**: `25-wordmath-reasoning.c` è il file più denso
   (71 catene), poi `10-memory`, `40-meta`, `70-social`, `30-generation`.
   Migrazione per categorie, mai cieca: ogni file un round, fail-set identico.
4. Ogni round: stesso protocollo §2-§6, preambolo incluso — la conoscenza di
   sessione non sopravvive, e va bene così.

## Appendice — il transcript del round gen337 (riassunto)

```
[preambolo: 8 × learn the definition of …]        → 8 × "Got it - <termine>: …"
what is kb-first?                                  → kb-first is engine fixed, knowledge learns - …
what is cue-migration?                             → cue-migration is replacing a cue chain with …
what is an impostor?                               → I already know about impostor: …
execute the kb-first plan for chains of calls to cue in src/brain
  → orchain_scan: 251 OR-chains, 10 file, densest 25-wordmath (71);
    stop onesto: emit_facts vuole un file singolo
execute the kb-first plan for chains of calls to cue in src/brain/60-agent-tools.c
  → 25 catene (90 call) → 90 intent_cue in .cues.p0 → 25 kb_cue_match in .p0fix
    → stop onesto al verify: "its own build must judge"
[integrazione supervisore] → intents.p0 + patch + learnable EN/IT
[verifica] → build 0 warning; fail-set identico (73=73); cuechains 251→226;
             toolvocab_growth.chat verde (crescita runtime senza rebuild)
```