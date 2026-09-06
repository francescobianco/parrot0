# Quanto costa una sessione, e come si ottimizza

*Aperto il 2026-09-06 su richiesta di F.: «dobbiamo capire quanto ci costa una
sessione di addestramento come questa, dove sono le criticita', e costruire una
procedura ottimizzata di crescita della KB — non nei contenuti, nella
**procedura**».*

Misurato sulla sessione del 6 settembre 2026 (`143805c`…`7a24471`), l'unica di
cui abbiamo insieme il transcript, i commit e i tempi.

---

## ⚡ In testa, per chi non legge il resto

**Le cinque cose che cambiano il costo di una sessione, in ordine di leva:**

1. **Il ciclo KB e' gratis, quello C costa 18,3 s.** Una modifica `.p0` non
   richiede nessuna ricompilazione: si riavvia il binario e c'e'. Una riga di C
   ricompila **53.605 righe** in una sola unita' di traduzione. → *esaurire le
   ipotesi KB prima di toccare il C.* Il mantra #1 non e' solo una regola di
   qualita': e' la via **piu' veloce**, e nessuno l'aveva mai detto cosi'.
2. **Una sonda in KB batte una `fprintf`.** Stessa informazione, 18,3 s contro
   0. Al `gen505t` tre righe di `debug_probe` hanno raccontato un caso intero
   che tre ipotesi ragionate non avevano chiuso.
3. **Le misure invecchiano e nessuno le ridata.** 76 affermazioni «misurato» nel
   codice, **5** con una data o una generazione. Una di queste ha tenuto ferma
   una classe della KB per settimane dopo che era diventata falsa.
4. **La diagnosi ereditata va rifatta, non creduta.** Due handoff di questa
   stessa giornata portavano diagnosi sbagliate, entrambe mie.
5. **Un reperto di F. vale un'ora di esplorazione.** Tre turni veri incollati →
   tre difetti veri, tutti chiusi. Costo di scoperta: zero.

**La procedura in sei passi e' al §4.** Il resto e' il perche'.

---

## 1. La sessione, misurata

| | |
|---|---|
| commit | 12 |
| finestra attiva | ~4,2 h (09:45–13:15, 20:16–20:59) |
| media | ~21 min per commit |
| righe di documentazione | **890** |
| righe di KB | **533** |
| righe di C | **527**, di cui **197 (42%) commento** |
| righe di test | 16 |

Il codice vero scritto in una giornata piena e' **~277 righe**. Tutto il resto e'
conoscenza e spiegazione — che e' come dev'essere, ed e' la prima cosa che una
procedura deve smettere di trattare come un sottoprodotto.

### Il costo meccanico di un giro

| operazione | costo |
|---|---|
| ricompilare dopo **una riga di C** | **18,3 s** |
| ricompilare dopo una modifica **solo KB** | **0 s** — non serve |
| sonda CLI (boot + un turno) | 3,1 s |
| avvio del demone di test | 3,7 s |
| un `.p0t` mirato | 4,3 s |
| `make soft-test` | **32 s** (il suo budget dichiarato e' 15 s) |
| suite intera | minuti, fail-fast, 4 rossi preesistenti |

I 18,3 s hanno una causa strutturale: `src/brain.c` include tutti i tredici
`brain/*.c`, e l'unita' di traduzione e' di **53.605 righe**. Toccare
`70-social-pragma.c` (1.467 righe) ricompila anche le 17.706 di
`10-memory-knowledge.c`.

---

## 2. Le criticita', con la misura accanto

### C1 — I due cicli hanno prezzi diversi e li trattiamo uguale

Il ciclo KB non ha compilazione. Il ciclo C ne ha una da 18,3 s. In questa
sessione li ho alternati di continuo, spesso ricompilando per provare
un'ipotesi che un fatto `.p0` avrebbe falsificato gratis.

> **Regola:** ogni ipotesi si prova prima nella forma KB. Se non e' esprimibile
> in KB, quello **e'** il risultato: hai appena scoperto che serve il motore, e
> lo sai senza aver pagato una compilazione.

### C2 — Le sonde di debug sono in KB, e le usiamo poco

`/debug` legge `debug_probe/4`: una sonda nuova e' **una riga di `.p0`**, senza
ricompilare. Nella sessione ho invece usato tre volte una `fprintf` usa-e-getta,
pagando 18,3 s per aggiungerla e altri 18,3 per toglierla — e una volta l'ho
dimenticata dentro un commit.

Quando finalmente ho messo le tre sonde di `turn_focus`, il caso si e' letto da
solo:

```
turn_focus            DI CHE COSA chiede il turno — the arrocco
turn_focus_input      su quale testo e' stato letto — what is the arrocco in chess
turn_focus_rejected   risposte ritirate: fuori dal fuoco — chess
```

### C3 — 76 misure, 5 con una data

```
«misurat*» nei commenti di kb/ e src/     76
di cui con una generazione o una data      5
numeri di prestazione citati              43
```

Il caso concreto: `verb_suffix/1` era fermo a un membro con la ragione scritta
accanto — «con -ed e -d il budget di un TURNO passava a 1,85 s». Vera quando fu
scritta. Dal `gen491` le viste si scaldano al **boot**, quindi quel costo si era
spostato e il vincolo era diventato falso. Nessuno l'aveva ridatato. Rimuoverlo
ha fatto crescere la morfologia **e** sceso il boot da 3,74 s a 2,64 s.

> **Regola:** una misura senza data non e' una misura, e' un ricordo. Ogni
> «misurato» porta la generazione. Ogni vincolo giustificato da una misura si
> **ridata** prima di rispettarlo.

### C4 — La diagnosi si eredita invece di rifarla

Due handoff di questa giornata portavano diagnosi sbagliate:

| handoff diceva | era |
|---|---|
| «la coreferenza rivendica la frase citata» | il secondo passo del thinking sovrascriveva la risposta |
| «il thinking costa 2 s per la morfologia» | costa 2 s per il passo che rilegge la propria risposta |

Entrambe scritte da me, in buona fede, con la sicurezza di chi ha appena visto
il sintomo. Entrambe hanno indirizzato male il giro successivo.

> **Regola:** un handoff scrive la diagnosi **e la sonda che la falsifica**. Se
> falsificarla costa piu' di un minuto, la diagnosi va marcata `da riverificare`.

### C5 — La lore sbagliata costa due volte

Avevo annotato in `composition.p0` che le clausole con «troppe variabili» non
rendono nulla. Falso: i soffitti veri sono `KB_MAX_ARGS = 4` e
`KB_MAX_BODY = 8`. Ci sono **ricascato in un altro file**, cercando di togliere
variabili invece che argomenti, e una regola e' rimasta non caricata per giorni
con un solo `bad rule, dropped` su stderr al boot.

> **Regola:** una trappola del dialetto si annota **con il limite esatto e il
> file dove sta**, mai con la sensazione. Sono in `AGENTS.md`, e il censimento
> costa una riga:
> ```sh
> echo '/quit' | PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 \
>   ./bin/parrot0 2>&1 >/dev/null | grep 'PARSE ERROR'
> ```

### C6 — Il reperto vale piu' dell'esplorazione

F. ha incollato tre turni veri di `make chat`. Hanno prodotto **tre difetti
veri**, tutti e tre chiusi, e uno era un misclaim — la categoria peggiore, che
l'esplorazione dall'interno non trova quasi mai perche' si sonda cio' che si
sospetta.

L'esplorazione autonoma ha trovato cose ottime (il matcher a sottostringa,
l'ordine delle viste), ma con un costo per scoperta molto piu' alto.

> **Regola:** una sessione comincia da un transcript reale, non da un piano.
> Se non c'e', il primo atto e' produrne uno.

### C7 — I registri crescono e non si potano

`LEARN_TODO.md`: **7.355 righe**, +1.024 (**+16%**) in un giorno, **6 handoff
impilati** di cui uno solo vivo. `JOURNAL.md`: 7.907 righe, non toccato oggi.
60 piani in `docs/plans/`.

Il costo non e' il disco: e' che ogni sessione nuova deve **leggere** per
sapere da dove riprendere, e la parte viva e' il 2% del file.

### C8 — Manca il livello intermedio di verifica

Ci sono tre gradini e sono troppo distanti: un `.p0t` mirato (4,3 s), `soft-test`
(32 s, oltre il proprio budget), la suite intera (minuti, con 4 rossi noti).

Lo strumento che ho usato davvero — e che non e' scritto in nessuna procedura —
e' il **differenziale con lo stash**:

```sh
git stash && make -s bin/parrot0 && ./bin/parrot0 --test FILE  # baseline
git stash pop && make -s bin/parrot0 && ./bin/parrot0 --test FILE  # dopo
```

E' l'unico modo onesto di dire «questo rosso e' mio» invece di «questo rosso
c'e'». Costa due ricompilazioni (36,6 s) e vale ogni volta.

---

## 3. Che cosa **non** e' una criticita'

Da dire, perche' l'istinto porta a ottimizzare la cosa sbagliata.

- **Le 890 righe di documentazione non sono spreco.** Sono il motivo per cui
  questa analisi e' possibile: i commit di questa sessione contengono le misure,
  le strade sbagliate e il perche'. Il difetto non e' scriverne troppa, e'
  non **datarla** (C3) e non **potarla** (C7).
- **I 12 commit non sono troppi.** Ogni commit e' un'unita' causale leggibile,
  come chiede `LEARN_PROTOCOL.md` §8.
- **I 4 rossi preesistenti non vanno chiusi adesso.** Sono un debito noto e
  tracciato in `TEST_TODO.md`. Il costo che infliggono e' che ogni spot-check
  deve ristabilirli come baseline — e quello si paga una volta, non ogni volta,
  se la baseline e' scritta.

---

## 4. La procedura ottimizzata, in sei passi

> Vale per una sessione **mista** — comprensione, metacomprensione, KB viva —
> cioe' quella che facciamo davvero. Per una sessione di solo insegnamento resta
> `LEARN_PROTOCOL.md`, che questo documento non sostituisce.

### Passo 0 — Aprire con un reperto, non con un piano *(2 min)*

Un transcript reale di `make chat`. Se non c'e', se ne produce uno: dieci turni
nel dominio del giorno, incollati grezzi. **Ogni turno che mente vale piu' di
ogni turno che mura**, e i misclaim si trovano solo cosi'.

### Passo 1 — Fissare la baseline, una volta *(1 min)*

```sh
make -s bin/parrot0 && echo '/quit' | PARROT0_SESSION= \
  PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 2>&1 >/dev/null | grep 'PARSE ERROR'
{ time (printf '' | PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 >/dev/null 2>&1); }
```

Zero `PARSE ERROR`, e il tempo di boot annotato. Poi i 4-6 `.p0t` del dominio,
**una volta**, e i loro rossi scritti nel primo commento della sessione. Da qui
in poi «e' rosso» non e' piu' un'informazione: lo e' «e' rosso **diversamente**».

### Passo 2 — Diagnosi con sonde, non con ipotesi *(5 min per reperto)*

Nell'ordine, e ci si ferma al primo che risponde:

1. `/debug` sul turno — `turn_module`, `turn_outcome`, `turn_focus`, le sonde
   che ci sono gia';
2. `lang.canonical` via MCP — meta' dei difetti italiani si vedono qui;
3. una **sonda nuova**: una riga di `debug_probe/4`, zero ricompilazioni;
4. solo adesso, se serve, una `fprintf` — e si toglie **nello stesso giro**.

⛔ Vietato formulare piu' di **una** ipotesi prima di aver guardato. Al gen505t
tre ipotesi ragionate hanno perso contro una sonda.

### Passo 3 — Provare la cura **in KB per prima** *(0 s di compilazione)*

Si scrive il fatto o la regola, si riavvia il binario, si guarda. Tre esiti:

- **funziona** → e' finita, e non hai compilato niente;
- **non funziona** → hai falsificato un'ipotesi al costo di un riavvio;
- **non e' esprimibile** → *questo* e' il risultato: serve il motore, e lo sai
  senza aver pagato 18,3 s per scoprirlo.

Al gen505u una riga di `phrase_canon` ha chiuso un reperto che sembrava un
difetto del registro sociale. Al gen505s una riga di `inflection_suffix` ha
chiuso meta' della morfologia.

### Passo 4 — Se serve il C: **una** modifica, poi misurare *(18,3 s a giro)*

Il rebuild e' il collo di bottiglia, quindi si raggruppa: si scrivono tutte le
modifiche di quel giro, **poi** si compila una volta. E la regola che questa
sessione ha pagato per imparare:

> **Il fuoco verifica un vincitore, non restringe un ingresso.**
> Aggiungere una verifica e' additivo. Cambiare cio' che entra in una gara
> cambia chi vince: due tentativi di questo tipo hanno rotto «what is water» e
> sono stati annullati.

Vale oltre il caso: davanti a un turno rubato, **verificare dopo** costa meno e
rompe meno che **filtrare prima**.

### Passo 5 — Verifica differenziale, non suite *(≈40 s)*

I 4-6 `.p0t` del dominio, e se uno diventa rosso il differenziale con lo stash
**prima** di accusare il proprio codice. Due volte in questa sessione il rosso
era preesistente; una volta era mio ed era **una scoperta** (`you keep saying
the same thing` — la lettura era giusta, mancava `participant_pronoun/1`).

Mai la suite intera senza che l'operatore la chieda.

### Passo 6 — Chiudere il registro, non farlo crescere *(5 min)*

- Il commit porta: il reperto, la misura **prima/dopo**, le strade provate e
  **misurate come sbagliate**, e i rossi con il loro differenziale.
- L'handoff **sostituisce** quello della sessione precedente; il vecchio si
  archivia sotto una riga sola. Un solo `# 🏁 HANDOFF` vivo per file.
- Ogni «misurato» porta la generazione.
- Ogni diagnosi porta la sonda che la falsifica, o il marchio
  `da riverificare`.

---

## 5. Le tre modifiche strutturali che ripagherebbero da sole

Non sono procedura: sono lavoro, e vanno pianificate.

1. **Spezzare l'unita' di traduzione.** 53.605 righe in un `.o` costano 18,3 s a
   ogni riga di C toccata. Tredici oggetti separati porterebbero il giro tipico
   nell'ordine dei 2-3 s. E' la singola ottimizzazione con il ritorno piu' alto
   sull'intero progetto, e non tocca ne' il motore ne' la KB.
2. **Un gradino di verifica fra i 4 s e i 32 s.** `make dominio-test DOM=x` che
   gira i `.p0t` di una cartella e stampa il **delta** rispetto a una baseline
   salvata, invece di un elenco di rossi da interpretare a mano.
3. **`make measure-boot`**, che stampa boot e turni separatamente. Tutte e tre
   le sorprese di prestazione di questa sessione (il vincolo scaduto, l'ordine
   delle viste, il costo del thinking) sono state trovate cosi', a mano.

---

## 6. La riga sola, se se ne ricorda una

> **Guarda prima di ipotizzare, prova in KB prima di compilare, e data ogni
> misura — perche' la prossima sessione le credera'.**
