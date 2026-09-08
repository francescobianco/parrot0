# Il banco di comprensione — `tests/comprehension-probe/`

Dieci famiglie di prompt lunghi (F., settembre 2026; prima in `var/probe/`),
sedici item l'una, dieci blocchi multi-turno per `f05`: 154 item. Non e' un
cricchetto: e' un **banco di scoperta**, che registra la risposta verbatim di
parrot0 e la classifica. Il cricchetto lo si scrive DOPO, in `tests/p0t/`, per
la forma che si e' capito come leggere (esempio: `conversation/compound_inquiry.p0t`).

| famiglia | che cosa chiede |
|---|---|
| f01 | sillogismi e quantificatori, con due domande alla fine |
| f02 | cause, tempi, stati che cambiano |
| f03 | riferimenti ambigui («they», «it», «his»), letture multiple |
| f04 | entita' inventate, catene, sinonimi, relazioni |
| f05 | multi-turno: ricorda / dimentica / insegna / regole ritirate |
| f06 | vincoli e piani: ordini, orari, risorse |
| f07 | proporzioni, percentuali, sequenze, equazioni |
| f08 | formati: CSV, JSON, YAML, SQL, regex, tabelle |
| f09 | difetti di codice: default mutabili, race, cache, parser |
| f10 | riscrittura: passivo, riassunto in N parole, controesempi, poesia |

## Come si lancia

```
tests/comprehension-probe/probe.py smoke      # IL BANCO PICCOLO (smoke.txt): ~1 minuto, a ogni modifica
tests/comprehension-probe/probe.py all        # tutte le famiglie, ~10 minuti — a fine giornata, con il LEDGER
tests/comprehension-probe/probe.py f01 f03    # alcune
tests/comprehension-probe/probe.py summary    # riclassifica results/*.tsv: la tabella per il LEDGER
PARROT0_BIN=obj/p0next tests/comprehension-probe/probe.py all   # un binario candidato (make build BIN=obj/p0next)
```

Per famiglia parte UN demone `.p0t` fresco (profilo di `make chat`, rete
spenta), si aspetta che risponda a un **health check vero** (il socket c'e'
prima che la KB sia caricata: era la causa dei 16 «cannot reach engine» del
gen505), poi ogni item e' un test isolato — cervello pulito per prompt. Il
cane da guardia del demone (`PARROT0_TE_HARD`, qui 120s) ferma un turno che
non torna e il runner riavvia il demone.

Esito per item, in `results/FAM.tsv` (gitignorato — si rigenera):

| stato | significa |
|---|---|
| `answered` | c'e' una risposta e non e' un muro dichiarato. **Non vuol dire giusta**: va letta |
| `wall` | un muro onesto (non capisco / non so / vuoi che impari / quel turno unisce…) |
| `hung` | il turno ha superato il budget duro: difetto del motore, non incomprensione |
| `transport` | il demone non ha risposto: difetto dell'infrastruttura, non incomprensione |

Il riassunto di ogni corsa che conta va in `LEDGER.md`, a mano, con data e
commit: e' la memoria di quanto parrot0 capiva. Il muro del turno composto
(«That turn joins several statements…») e la clausola non letta («I couldn't
read «…»») contano come `wall` solo se TUTTA la risposta e' un muro: una
risposta composta che ha imparato due fatti e murato sulla domanda e'
`answered`, e va letta.

## Come si usa per far crescere la comprensione

**Incrementale, non a blocchi (F., 8 settembre).** Il ciclo e': una modifica
-> `probe.py smoke` (un minuto, risposte lette una per una, con le bandiere
⚠ ECO / ⚠ risposta di registro / ⚠ TRANSPORT) -> il cricchetto `.p0t` della
forma -> `make soft-test`. La suite intera e `probe.py all` NON stanno nel
ciclo: sono la chiusura di giornata. Un prompt disfunzionale scoperto dal
banco intero entra in `smoke.txt` come riga, e ci resta.

1. Leggere le risposte `answered` una per una: una risposta plausibile e fuori
   tema («I don't have any of my own — I'm parrot0») e' peggio di un muro
   (PRINCIPLES.md). Chi l'ha detta lo dice `> why that way?` subito dopo.
2. Ogni misclaim di una facolta' che ha riconosciuto una cue in un turno di
   quattro clausole e' una CONDOTTA da scrivere in KB (`faculty_yield_force`,
   intents.p0), non un `if` nel modulo.
3. Ogni `hung` si profila (`gdb` con il binario figlio, vedi LEARN_TODO
   gen506b) prima di alzare un budget.
4. Una forma che il muro nomina bene e' pronta per il cricchetto: si scrive la
   sezione `.p0t` con l'ablazione della riga di KB che la legge.
5. Dal gen506c il turno composto si legge per clausole (`compound_turn_lead`,
   99-registry.c): ogni clausola e' un turno intero. Per vedere cosa fa
   ciascuna, `P0_READ_TRACE=1` sul demone stampa `[compound] clause=… module=…
   resp=…` e `[wrapper] residue=… polar=…`.
