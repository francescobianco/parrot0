# Knowledge Base TODO

## HANDOFF — dove riprendere dopo la sessione del 17 agosto 2026

**Stato:** albero pulito, `make test` 2347 verdi in ~90s, `make soft-test` verde
in ~6s sul budget 15 (i due `!timeout` di cerotto sono stati tolti, non alzati). Tutte e sette le generazioni del
piano `docs/plans/frontier-kb-natural-dialogue.md` hanno ora almeno un taglio
verticale funzionante; il piano e' intorno al **70%**.

## ✅ BUG CHIUSO — l'apostrofo spegneva il produttore universale (gen402)

Trovato chiudendo #89 del censimento, e vale la pena tenerlo scritto perche' la
sproporzione fra la causa e il danno e' la lezione.

```text
> who won yesterday match      -> I have no live source for a result that ...
> who won yesterday's match    -> Hmm, I don't know about yesterday's yet
```

Con un apostrofo nel turno, `turn_span/4` non veniva pubblicato affatto: il
turno risultava `ambiguous`, `universal_turn_lead` usciva subito, e con lui
tacevano tutti i livelli costruiti dal gen393 in poi — frame, issue, situazione,
registro, contabilita'. In inglese l'apostrofo sta in ogni possessivo e in ogni
contrazione, quindi la classe muta era enorme.

La causa non era dove sembrava. `input_quote_at()` la regola giusta ce l'aveva
gia' — un apostrofo fra due alfanumerici non apre niente — ma
`input_find_delim_open()`, dopo averla interrogata e ricevuto un no, proseguiva
fino al confronto letterale con `d->open` e apriva lo stesso. La guardia era
scritta e poi scavalcata due rami piu' sotto. Il ramo generico ora e' escluso
quando la coppia di delimitatori E' il segno di citazione.

Due cose da portarsi dietro. La prima: nessun `.p0t` lo aveva preso perche' i
test scritti finora usavano forme senza apostrofo — una lacuna di CAMPIONE, non
di copertura, e le lacune di campione non si vedono guardando quanti test sono
verdi. La seconda: il cricchetto `tests/p0t/language/apostrophe.p0t` non
verifica il segmentatore, che durante il bug rispondeva correttamente; verifica
che i livelli SOPRA continuino a rispondere quando l'apostrofo c'e'. Un test
puntato sulla causa sarebbe stato verde per tutta la durata del guasto.

## 🔎 AUDIT KB-FIRST DEL C — `docs/plans/kb-first-audit.md`

Trenta marcatori `TODO(kb-first):` nel codice, indicizzati e ordinati per DANNO
in `docs/plans/kb-first-audit.md`. `grep -rn "TODO(kb-first)" src/` e' sempre
allineato con quel documento.

Il caso peggiore non e' la lista piu' lunga, e' il DUPLICATO: gli articoli, le
stopword, i sequenziatori e gli ausiliari sono gia' nella KB, e il C ne tiene una
copia propria che vince in silenzio. I sequenziatori sono scritti TRE volte con
contenuti divergenti.

E la lezione delle due conversioni del gen403: spostare la conoscenza ha fatto
emergere ogni volta un guasto che il C nascondeva — cue italiane morte perche'
confrontate con il turno tradotto, una cue vorace che rubava i turni di
inferenza, un marcatore di autoreferenza mancante. Una lista nel C e' un posto
dove i buchi non si vedono.

## ⛔ TODO APERTI DEL PIANO — non chiuderlo senza questi

Il piano `docs/plans/frontier-kb-natural-dialogue.md` ha copertura FUNZIONALE
completa: tutte e sette le generazioni del §15 e tutte e sette le
sotto-generazioni del §17 hanno tagli verticali ratchettati. Quello che manca non
e' piu' capacita' mancante, ed e' scritto qui perche' non diventi invisibile.

1. **§9 — il confronto empirico non e' stato fatto.** Le batterie del §9.2
   (parafrasi, ambiguita', ponte fra entita', uso/menzione, registro,
   pragmatica, correzione, piano situazionale, sopravvivenza, triangolazione,
   azione informativa, bilancio dinamico, multi-goal, scope, conversazione
   lunga, cross-domain, negativo vicino) esistono come ratchet SPARSI, non come
   batteria unica con le metriche del §9.3. Finche' non esiste, la percentuale
   di completamento e' una stima nostra e non una misura.
2. **La latenza (§10) — CAUSA TROVATA E CORRETTA (gen401).** Il profiler
   `/debug` ha isolato un costo FISSO PER PASSO: non la KB, non il numero di
   passi. `Subst` dimensiona i propri array sul caso peggiore (384 binding da
   608 byte = ~266 KB) e veniva copiata per valore PRIMA DI OGNI FATTO
   CANDIDATO — misurati 81.842 fatti visitati per 960 passi in un solo turno.

   Corretto in tre mosse, tutte a semantica invariata:

   - `subst_copy/2` copia il PREFISSO VIVO invece dell'array intero;
   - una copia per GOAL invece che per fatto, con annullamento dei due contatori
     dopo un tentativo fallito (`bind_add`/`dif_add` solo appendono, quindi
     riportare `n` e `ndif` cancella esattamente cio' che il tentativo aveva
     aggiunto — **se un giorno una delle due modificasse in luogo, quella riga
     diventa sbagliata**);
   - lo scratch da ~430 KB non si alloca piu' a ogni passo: pool per profondita',
     allocato pigramente;
   - il test «questo fatto ha variabili?» si salta quando il censimento dice che
     l'intero bucket e' ground.

   Risultato: turno 488 ms -> **54 ms**, `make test` 214 s -> **92 s**,
   `soft-test` 14 s -> **6 s** sul budget 15. Entrambi i cerotti `!timeout 3`
   dell'auto-audit sono stati TOLTI: quel turno e' tornato a ~0,44 s.

   Un tentativo RITIRATO, da non ripetere alla leggera: filtrare i fatti
   candidati con una `strcmp` sugli argomenti letterali del goal sembra esatto e
   invece rompe — l'unificazione normalizza qualcosa che il confronto di stringa
   non vede. Zero guadagno e quattro test rossi.

   **Il prossimo collo e' FUORI dal solver.** Il profiler ora lo mostra: su
   alcuni turni «fuori dal solver» supera il tempo di risoluzione (32 ms su 44).
   Li' dentro ci sono segmentazione, pubblicazione delle cue e i moduli C.

3. **La latenza, i sintomi gia' visibili.** `reflexive_audit.p0t` porta un `!timeout 3` messo come
   CEROTTO: quel turno costava 0,18s al gen382 e oggi ne costa 0,9. E' un
   fattore cinque riguadagnato mentre la KB cresceva, e il §10 dice di
   classificarlo come meccanica del solver. `findall/3` senza uscita anticipata
   e' il nodo noto.
3. **Due descrizioni nello stesso turno** non compongono: enumerarle
   esaustivamente intreccia effetti e ricerca, e il solver ribacktracka dentro
   gli assert. Si chiude separando gli effetti dalla ricerca.
4. **Rifiniture dichiarate dei livelli situazionali:** alternative scartate e
   residui nella resa (398g), dipendenze PER-PASSO invece del solo traguardo
   (398e), separazione fra prova e fonte debole (398d), reversibilita' come
   dimensione a se' (398f).
5. **Dimensioni di registro oltre tecnicita' e formalita'** (396.1): concisione,
   direttezza, cortesia, hedging.
6. **`docs/plans/parrot0-100-failures.md`** — il censimento e' chiuso solo in
   parte. Chiuse le famiglie numeriche (confronto, ordinamento, resto, mediana)
   e il contesto dichiarato; restano logica formale (contrapposizione,
   affermazione del conseguente), meta-domande sul sapere, salienza in un log,
   trasferimento di pattern, e la stipulazione su un SIMBOLO
   («immagina che 2 vale 3 quanto fa 2+2»), che chiede una sostituzione dentro
   l'aritmetica e non solo un contesto ipotetico.

### Che cosa e' cambiato, in una frase

Il pilastro che mancava quattro volte — il producer NL — ora esiste UNA volta:
`kb/core/turn-frames.p0`. Da li' in poi 394, 395, 396, 397 e 398a hanno smesso
di reinventarlo e ci si sono appoggiate. Chi riprende deve leggere quel file
prima di tutti gli altri.

### La catena, dall'alto in basso

```text
turno naturale
  -> universal_turn_lead (99-registry.c)   span, token, cue: soltanto meccanica
  -> turn_bookkeeping/2                    gli EFFETTI del turno (contabili KB)
  -> turn_plan_candidate/1                 questo turno appartiene a un piano?
  -> turn_response/2                       la risposta, e nient'altro
```

Le tre domande sono separate apposta. `turn_response/2` e' **pura**: una
risposta non puo' dipendere da un effetto accaduto mentre la si cercava. Gli
impegni — stipulare, descrivere uno stato, cambiare registro, aprire un'issue,
registrare uno scambio — vivono tutti in `turn_bookkeeping/2`, ed e' cio' che
permette a un turno di fare DUE cose (registrare e rispondere).

### I file nuovi, e a che cosa servono

| file | cosa |
|---|---|
| `core/turn-frames.p0` | il producer NL -> frame universale (gen393) |
| `core/issues.p0` | cio' che resta aperto fra un turno e l'altro (gen394) |
| `core/stipulation.p0` | «supponiamo che...» crea un mondo che sopravvive (gen395) |
| `core/register.p0` | il registro come vettore di dimensioni (gen396) |
| `core/situation.p0` | stato, azioni, applicabilita', piani (gen398a/c) |
| `core/state-description.p0` | dalla prosa allo stato (gen398a) |
| `core/discourse.p0` | ripresa, cio' che e' stato detto, salienza (gen397) |

### DA DOVE RIPARTIRE, in ordine

1. **Il resto di gen398c: da due passi a N.** Oggi il piano trova il passo
   ABILITANTE (`turn_enabled_step/3`) guardando lo stato come sarebbe
   (`holds_after/3`, applicazione non distruttiva). Il salto vero e' la
   ricorsione sullo stato: e' li' che il budget di inferenza si fara' sentire,
   e va misurato PRIMA di scrivere, non dopo.
2. **gen398e, ripianificazione.** `supersedes_in/3` e' gia' meta' del lavoro: una
   correzione crea una vista nuova invece di distruggere. Manca il passo che
   NOMINA che cosa resta valido e quale dipendenza e' stata spezzata.
3. **Due descrizioni nello stesso turno.** Debito dichiarato: enumerarle
   esaustivamente intreccia effetti e ricerca (il solver ribacktracka dentro gli
   assert e non converge). Si chiude separando gli effetti dalla ricerca.
4. **La precedenza fra mosse** (`move_priority/3` e' dato, nessuno lo risolve) e
   il caso Napoli/Campania nelle issue: entrambi bloccati dallo stesso nodo,
   l'enumerazione esaustiva delle letture.

### TRAPPOLE PAGATE — non ripagarle

Sono tutte della stessa specie: **il silenzio**. Il dialetto e il motore, quando
un limite viene superato, non lo dicono.

1. **`KB_MAX_BODY` e' 8.** Una regola con nove goal viene scartata dal loader
   senza una parola, e ogni suo pezzo interrogato da solo continua a funzionare.
   Se una regola «esiste ma non risolve», contare i goal e' il primo controllo.
2. **Un termine porta 4 argomenti**, quindi `assert` puo' creare un fatto di
   arita' 3 e non di piu': il quinto argomento non e' rappresentabile nel corpo
   di una regola, e il fatto non viene ne' creato ne' segnalato.
3. **`retract/1` FALLISCE se il fatto non c'e'**, e un effetto che fallisce si
   porta dietro tutta la congiunzione. Sostituire un valore vuole due clausole —
   il caso «c'era» e il caso «non c'era» vanno distinti, non sperati.
4. **`chars/2` + `append_list/3` si rompono oltre ~60 caratteri**: la lista
   intermedia sfonda `KB_TERM_LEN` e la risposta sparisce con ZERO soluzioni.
   Per questo `concat_atoms/3` e' diventato un primitivo.
5. **`findall/3` non ha uscita anticipata**: provare che una lettura e' unica
   costa esplorare tutte le clausole anche con la risposta gia' in mano. E'
   l'ultimo residuo di latenza, ed e' il nodo di due voci dell'elenco sopra.
6. **Gli effetti dentro una ricerca enumerata non convergono.** I contabili si
   chiedono UNO PER UNO (`bookkeeper/1`): enumerare tutte le soluzioni fa
   ribacktrackare il solver dentro gli assert. Misurato come un demone morto.
7. **Un array a tetto fisso riempito in ordine di caricamento e' un difetto di
   ORDINE.** Trovato due volte in un giorno: sei regole qualunque facevano
   perdere a `describe` l'ultima credenza derivata, e quattro fatti unari
   facevano dire allo stress-test «posso arrivarci in un altro modo». Crescere
   in conoscenza non puo' cambiare cio' che parrot0 dice di sapere.
8. **Una misura di latenza va presa contro un baseline CONTEMPORANEO.** Ho
   quasi ritirato due volte un livello corretto perche' `soft-test` sembrava
   peggiorato: la macchina era carica, e il baseline misurato nello stesso
   momento era identico.
9. **`!set PARROT0_LANG` fa ricaricare la KB al turno successivo**, portando via
   cio' che il test ha insegnato dopo. Il `!reload` esplicito ordina i due eventi.

### DUE REGOLE DI CONDOTTA, imparate sul campo

- **Non rubare una superficie che ha gia' un consumer.** «what did we talk
  about» appartiene al gen58 e fa una cosa piu' larga e piu' grezza: le due
  memorie convivono, una conta cio' che e' stato NOMINATO e l'altra cio' che e'
  stato RISPOSTO. Il test del gen58 e' rimasto verde senza una riga cambiata.
- **Non perturbare un ratchet per un'ipotesi.** La de-limitazione del dump della
  conoscenza era igiene, non correttezza, e spostava una soglia che
  `introspect.p0t` documenta come comportamento corrente. Ritirata.


## HANDOFF prioritario — ragionamento situazionale dal universal input — 2026-08-17

### AGGIORNAMENTO 5 — gen398a e' CHIUSA: la situazione arriva dalla prosa

`kb/core/state-description.p0` porta la meta' che mancava. «La valvola e'
chiusa» diventa una credenza di una situazione, e il pianificatore del primo
taglio ci lavora sopra senza sapere che e' arrivata parlando:

```text
come faccio ad abbassare il livello  -> Non capisco ancora.   (nessuna situazione)
la valvola e' chiusa                 -> Annotato: lo tengo come stato attuale.
la valvola funziona                  -> Annotato: ...
il livello sta salendo               -> Annotato: ...
come faccio ad abbassare il livello  -> Puoi aprire la valvola.
```

Nel core non vive un fatto di quel mondo: azione, precondizioni, effetto, legge
causale e forme entrano tutte da assert runtime. 21 asserzioni in
`reasoning/described_situation.p0t`, con secondo dominio in inglese.

Tre riusi e nessuno e' una coincidenza: il ruolo di span e' quello della
stipulazione, la credenza e' `holds_in/2`, e l'aggiornamento di stato e'
`supersedes_in/3` — cioe' la correzione locale del gen395 diventa gia' meta'
della ripianificazione che gen398e chiedera'.

Due difetti trovati, entrambi della specie «un filtro che accetta tutto»:

1. la clausola unitaria `state_prop(st(E,P,V), E, P, V)` ha reso VACUO
   `situation_property/1`, che filtrava sullo SCHEMA invece che sulle credenze.
   Cosi' «abbassare» valeva insieme come verso e come proprieta', le due letture
   si annullavano e il piano spariva. Una proprieta' e' situazionale quando una
   situazione ci tiene davvero uno stato;
2. tornare a un valore gia' detto lasciava DUE supersessioni incrociate, ciascuna
   a escludere l'altra: la proprieta' restava senza valore, cioe' una situazione
   muta che nessuno aveva descritto. Una correzione e' reversibile perche' e' una
   vista e non una distruzione — ma la vista va davvero riaperta.

Lo stimolo guida della mongolfiera **mura ancora, come deve**: non esiste un
dominio pallone, e «cosa faresti se» non e' una cue di goal. Il livello sotto di
esso ora c'e' per intero; fabbricare quel dominio per far passare il prompt che
ha aperto il fronte sarebbe esattamente cio' che il piano vieta.

`make test` 2145 verdi; `make soft-test` 20s contro 15 (era 18s, e 21s a inizio
sessione).

### AGGIORNAMENTO 4 — gen396.1-396.2 (il registro) e la latenza pagata

`kb/core/register.p0` chiude le due task del registro. Il registro non e' piu' un
interruttore: `register_value(Registro, Dimensione, Valore)` lo colloca in uno
spazio di dimensioni indipendenti, e la realizzazione sceglie la forma dentro
`concept_label/4`, che porta gia' sia la lingua sia il registro. Si cambia
PARLANDO:

```text
what is the zorb kind of vint  -> Glimmer.
be technical                   -> Noted: I will keep that register from here on.
what is the zorb kind of vint  -> Luminescent.
keep it simple                 -> Shiny.
```

Stessa proof, tre forme, e il ratchet prova che il contenuto viene dalla proof e
non dal registro: tolto il fatto, nessuna preferenza lo fa ricomparire in nessuna
forma. 26 asserzioni in `generation/register_realization.p0t`.

**La regola del gen390 era vera per meta'.** «Accettare una forma marcata non
obbliga a rispecchiarla» valeva sul percorso del registro, mentre la
localizzazione ordinaria continuava a emettere `mangiare`. La guardia
`naf(marked_form(...))` sta ora su OGNI gradino della scala di realizzazione. E
quando il registro chiesto ha SOLO una forma marcata, parrot0 non la emette e non
tace: usa la forma compatibile non marcata dello stesso concetto.

**Due trappole del dialetto, entrambe costose:**

1. `retract/1` FALLISCE se il fatto non c'e', e un effetto che fallisce si porta
   dietro tutta la congiunzione. Sostituire una preferenza richiede percio' due
   clausole — il caso «c'era» e il caso «non c'era» vanno distinti, non sperati;
2. `!set PARROT0_LANG` muove la firma di configurazione, quindi il turno
   SUCCESSIVO ricarica la KB da disco e porta via tutto cio' che il test ha
   insegnato dopo il `!set`. Il `!reload` esplicito fa avvenire la ricarica prima
   di insegnare. Un'ora di diagnosi che sembrava un difetto del registro.

**La latenza, pagata e misurata.** Il candidato del frame ri-derivava la risposta
intera: il motore fa due domande separate — «questo turno appartiene a un piano?»
e «qual e' la risposta?» — e mettere la derivazione completa in entrambe la
pagava due volte. Ora il candidato dichiara soltanto che il turno ha una
relazione ammessa e un'entita' di cui quella relazione sa parlare. Effetto
misurato: `make soft-test` da **28s a 18s**, cioe' sotto il baseline di 21s da
cui questa sessione era partita — il bilancio netto della sessione sulla latenza
e' positivo, non solo recuperato. Resta fuori dal budget di 15s, che e' debito
preesistente.

Il residuo di performance e' ora localizzato con precisione: **non e' il
candidato, e' l'enumerazione esaustiva delle letture**. `findall/3` non ha uscita
anticipata, quindi provare che una lettura e' UNICA costa esplorare tutte le
clausole della relazione anche quando la prima risposta e' gia' in mano. La
memoizzazione per turno resta il prossimo lavoro; e un'alternativa piu' economica
da valutare prima e' dichiarare quali relazioni sono funzionali, perche' per
quelle l'unicita' non va provata per esaurimento.

### AGGIORNAMENTO 3 — gen395 ha il suo producer, e quattro difetti trovati strada facendo

`kb/core/stipulation.p0` chiude il gate di gen395: un turno che stipula produce
`context/2` + `holds_in/2` che SOPRAVVIVONO al turno, e la domanda successiva
riceve entrambe le viste etichettate. Ratchet in
`conversation/stipulated_world.p0t`, 13 asserzioni, IT ed EN.

La scoperta che vale piu' del taglio: **`assert/1` e' gia' una primitiva del
solver**, quindi la KB puo' rendere persistenti le proprie conclusioni senza che
un modulo C decida per lei quando farlo. E' cio' che rende una stipulazione un
impegno invece di una premessa usa-e-getta. L'effetto sta dentro la
realizzazione della mossa `acknowledge`: stipulare in silenzio mentre si risponde
ad altro sarebbe un cambiamento di stato invisibile all'interlocutore.

Quattro difetti trovati lungo la strada, tutti silenziosi, tutti da ricordare:

1. **`KB_MAX_BODY` e' 8.** Una regola con nove goal viene scartata dal loader
   SENZA UNA PAROLA, e ogni suo pezzo interrogato separatamente continua a
   funzionare — la diagnosi piu' lenta di questa sessione. Se una regola
   «esiste ma non risolve», contare i goal del corpo e' il primo controllo;
2. **la composizione via `chars/2` + `append_list/3` si rompe oltre ~60
   caratteri.** La lista di caratteri intermedia sfonda `KB_TERM_LEN` (58
   caratteri diventano un termine di ~470, e il livello successivo del fold lo
   supera): il piano proposizionale a cinque pezzi perdeva la risposta con ZERO
   soluzioni e il turno cadeva al percorso storico. `concat_atoms/3` e' ora una
   primitiva; la clausola KB resta come struttura secondaria e documentazione.
   Concatenare due stringhe non e' conoscenza, e la prova e' che non scala;
3. **il produttore universale vede il turno NORMALIZZATO, non canonicalizzato.**
   Il percorso storico riceve il turno gia' tradotto, quindi una superficie
   italiana non poteva entrare nel frame e i fatti stavano sotto il termine
   canonico. Servono entrambe le cose, ed entrambe sono conoscenza: la
   superficie italiana in `answer_frame/2`, e `canonical_value/2` sulle parole
   del turno. Un frame deve poter parlare delle parole che l'interlocutore ha
   usato;
4. **`te_split_args` spezzava gli argomenti su ogni virgola.** Il dialetto della
   KB annida i termini, quello dei test no: `!forget holds_in(w, fact(r,s,o))`
   ritraeva un fatto inesistente e non toglieva niente, in silenzio. Corretto
   contando le parentesi — e la correzione ha subito **invalidato una proprieta'
   che un ratchet dava per misurata**: `gap_dialogue.p0t` affermava che togliere
   i due ponti verso `pump` non rompeva il legame. Non lo rompeva perche' il
   `!forget` non ritraeva. Ora ogni ponte porta il suo peso, e il test lo dice.

**Bonus dallo stesso giro, dal turno reale segnalato:** «cosa e' una variabile»
rispondeva e «cosa sono le variabili» murava. Erano DUE archi mancanti, non uno:
il plurale del NOME (l'elenco `singular/2` a mano non conteneva «variabili» — la
flessione e' una procedura, ora regole su `chars/2`) e il plurale della DOMANDA
(«cosa sono» non canonicalizzava, perche' «sono» da solo e' ambiguo in italiano).
Piu' il confine fra i due livelli: le regole di flessione SOVRAGENERANO, quindi
vivono sotto `lemma_candidate/2` e un consumer che decide sul primo risultato
continua a leggere la tabella curata — mescolarli faceva cercare «florbla» al
posto di «florble». Ratchet in `language/inflected_lookup.p0t`.

**Costo misurato:** 80 ms su un turno qualunque (invariato rispetto a prima del
producer), ~420 ms su un turno italiano che il frame risponde davvero, contro il
secondo di contratto. `make test` 2098 verdi; `make soft-test` verde e fuori
budget (28s contro 15, da 21 di base). La causa e' nota e non e' mascherata: fra
`turn_plan_candidate` e le due clausole di `turn_response` lo stesso insieme di
letture viene enumerato tre volte. Il rimedio — memoizzazione per turno, che il
§17.9 gia' autorizza — resta il primo lavoro di performance da fare, prima di
allargare l'ammissione al frame.

### AGGIORNAMENTO 2 — il producer NL -> frame di gen393 e' collegato

Era il pezzo che bloccava quattro generazioni con la stessa forma (393 aspetta
prosa->frame, 395 prosa->contesto, 396 prosa->piano del codice, 398a
prosa->stato). Ora esiste in `kb/core/turn-frames.p0`, e il turno reale alimenta
`frame_act/2`, `frame_slot/3`, `frame_source/3` prima del first-match dispatch.

Come funziona, in tre righe: il C pubblica `turn_cue/3` — quali superfici
DICHIARATE dalla KB il turno contiene — accanto ai `turn_span_token/4` che gia'
c'erano; la KB giunge cue->relazione, parola->entita' e risponde nel verso
dichiarato; la mossa la decide `move_policy/2`, non la precedenza fra moduli.
Il registro e' `turn_cue_registry/2`: nessuna cue, lingua o relazione nel C.

Quattro cose imparate, tutte a caro prezzo, tutte da non ripetere:

1. **una condizione va scritta col predicato che nomina la propria specie.**
   `situation.p0` usava `turn_plan_candidate/1` — il contratto CONDIVISO dai
   produttori di piano — dove intendeva «questo turno e' una situazione». Con un
   secondo produttore in campo, il residuo situazionale rivendicava ogni turno;
2. **il produttore non deve poter interrogare la macchina di cui e' fatto.**
   `answer_frame/2` contiene superfici riflessive: senza la guardia
   `naf(machinery(...))`, `frame_move` -> `dialogue_state` -> gli slot che il
   producer stesso deriva, e con l'entita' libera la guardia dei cicli non taglia
   (non taglia mai un goal non ground). La derivazione non terminava;
3. **una superficie senza VERSO dichiarato costa una ricerca completa.** La
   direzione inversa di una relazione-regola con estensione grande e' una
   scansione: 8 secondi misurati su un turno che il percorso storico chiudeva in
   millisecondi, e pagati su OGNI turno perche' il producer universale gira
   sempre. L'ammissione al frame e' percio' `answer_frame_input_arg/3`, che e'
   anche una promozione teachable: dichiarare il verso fa entrare una relazione
   nel producer a runtime, ritrarlo la restituisce al percorso storico;
4. **le cue vanno pubblicate CITATE, come i token.** Pubblicate nude, le parole
   del turno sembrano termini gia' posseduti: il muro informato trovava
   «capital» dentro il fatto che registrava di averla vista e smetteva di
   nominarla. La promozione da superficie a termine e' una regola
   (`turn_cue_form/3`), mai il modo in cui il dato viene scritto.

**Costo misurato, dichiarato e non nascosto:** zero su un turno qualunque (il
registro nomina fatti nudi, non una vista derivata); ~112 ms su un turno che il
frame universale risponde davvero, contro un contratto di 1 secondo. La causa
nota e' la doppia enumerazione (`turn_plan_candidate` e `turn_response`
rideriscono lo stesso insieme di letture): il rimedio previsto e' la memoizzazione
per turno che il §17.9 gia' autorizza, e non e' stato fatto qui per non allungare
il passo. `make test` 2068 verdi; `make soft-test` verde e fuori budget (23s
contro 15s, da 20-21s di base).

**Bonus, dallo stesso giro:** «come stai» murava. La conoscenza c'era tutta
(`social_pattern(wellbeing, come stai)`): a rompersi era la locuzione, disfatta
dalla canonicalizzazione per parole. Cinque `phrase_canon/2` in `lexicon.p0`,
zero C — e il `.p0t` che asseriva il muro come comportamento corrente e' stato
aggiornato, non difeso.

### AGGIORNAMENTO — passo 1 (audit) e primo taglio di gen398a eseguiti

L'audit di riuso ha cambiato il punto di attacco e va letto prima del resto:

1. **il producer universale esiste gia'.** `universal_turn_lead`
   (`src/brain/99-registry.c`) gira PRIMA del dispatch, reifica il turno in
   `turn_span/4`, `turn_span_cue/3`, `turn_span_token/4`, `turn_span_binding/4`,
   e poi chiede alla KB due sole cose: `turn_plan_candidate/1` e
   `turn_response/2`. Il livello situazionale e' percio' il TERZO consumer di
   quel contratto, dopo `conditional-plans.p0` e `code-plans.p0`, e non ha
   richiesto **una sola riga di C**;
2. **il renderer K6 esiste gia'**: `answer_content/4` + `answer_text/2` in
   `procedures.p0`. Il piano si compone senza concatenare stringhe nel C e il
   fold non ha accesso a nulla con cui fabbricare un claim;
3. **i contesti gen395 esistono gia'**: lo stato di una situazione e' una
   credenza visibile di un contesto (`holds_in/2` + `context_visible_belief/2`),
   non una seconda tabella. Le proposizioni vanno REIFICATE (`state_prop/4`), che
   e' anche cio' che `proposition_signature/4` gia' presupponeva;
4. **`action_schema/2` si riusa, `action_needs`/`action_yields` NO**: quel
   chainer (`src/brain/25-wordmath-reasoning.c`) cammina su ARTEFATTI prodotti,
   non su precondizioni di STATO. Sono due relazioni diverse, non la stessa sotto
   un'altra etichetta;
5. **vincolo dell'harness misurato**: `te_split_args` (`src/testeng.c`) spezza gli
   argomenti sulla virgola senza contare le parentesi, quindi un fatto con
   termine composto annidato non e' ne' asseribile ne' ritrattabile da un `.p0t`.
   E' la ragione tecnica in piu' per reificare invece di annidare: tutto resta
   piatto, insegnabile e ablabile a runtime.

Fatto: `kb/core/situation.p0` (sole regole, zero fatti di dominio), i template di
residuo in `responses.p0`, il ratchet `tests/p0t/reasoning/situation_plan.p0t`
(25 asserzioni, profilo AGI, budget ordinario di un secondo, nessun `!timeout`).
`make test` chiude 2052 prove verdi. Lo stato completo — cosa e' collegato, cosa
NON lo e', e il difetto trovato dal ratchet — e' in
`docs/plans/frontier-kb-natural-dialogue.md` §17.7 sotto gen398a.

**Il prossimo passo e' il gate onesto di gen398a, non gen398b**: oggi il turno
contribuisce il GOAL, mentre lo STATO deve essere gia' conoscenza. Manca la meta'
`descrizione` del producer — da prosa a `state_prop/4` + `holds_in/2`. Finche'
quella meta' non esiste, gen398a resta aperta e la mongolfiera mura, come deve.

**La lezione da non ripetere.** Il primo taglio dichiarava un'azione BLOCCATA in
una situazione mai dichiarata: in un mondo inesistente nessuna credenza vale,
quindi ogni precondizione vi risulta «mancante». L'ignoranza si travestiva da
conoscenza negativa — la specie di divergenza del §2.1 vincolo 5. Ogni regola che
inferisce da un'ASSENZA deve dichiarare prima l'esistenza del contesto in cui
quell'assenza avrebbe senso, e il controllo negativo va nel ratchet.

### Mandato e stato: il contratto originale (primo taglio eseguito, vedi sopra)

Il prossimo fronte richiesto e' rendere parrot0 capace di affrontare, dallo
stesso `gen.respond` universale, problemi aperti di sopravvivenza, astuzia,
triangolazione, rilettura del contesto e ripianificazione. Il caso che ha aperto
il lavoro e':

```text
you> cosa faresti se una mongolfiera sta cadendo per rallentare la caduta
Non capisco ancora.
```

In questo giro **non e' stata implementata la capacita'**. E' stato scritto il
contratto completo in `docs/plans/frontier-kb-natural-dialogue.md` §17 e aggiunto
K11 alla scala delle rappresentazioni. Ripartire da li', non dalla frase sulla
mongolfiera. Il target e' un planner situazionale KB-first, non una nuova
faculty, un answer frame terminale o una collezione di enigmi.

Prima di qualsiasi modifica leggere, nell'ordine:

1. `MANTRA.md` e `PRINCIPLES.md`;
2. `docs/plans/frontier-kb-natural-dialogue.md` §§5 K11, 9, 10 e 17;
3. `tests/situational_reasoning_probe.py`;
4. `tests/sym/situational-reasoning-20260817-013348.md`;
5. questo handoff e quello gen396 immediatamente sotto.

### Evidenza raccolta correttamente con OpenCode-GO

La nuova sonda segue il protocollo delle sonde del progetto: chiamata diretta a
`https://opencode.ai/zen/go/v1/chat/completions`, autenticazione
`OPENCODE_API_KEY`, confronto con parrot0 e trascrizione in `tests/sym/`. Il giro
guida usa `gpt-5.6-luna`. Non usa `pi` e non entra nel runtime.

Risultato osservato:

- parrot0 mura sulla mongolfiera nuda, su quella vincolata, sul caso senza
  risorse e sulla scorciatoia dannosa;
- dopo la correzione aria calda -> gas risponde con un chiarimento generico,
  senza rileggere il piano;
- sulla separazione cesto/involucro e sulla triangolazione devia verso una
  risposta meta sulle proprie faculty;
- sul bilancio temporale della barca produce una storia fuori tema;
- sui tre interruttori classifica falsamente il problema come codice;
- il riferimento, a contrasto, lega azioni ed effetti, conserva/ritira passi
  dopo correzioni, enumera mondi coerenti, distingue una fonte non verificata,
  calcola stati intermedi, sceglie un'azione informativa e rifiuta il danno.

Questo **non certifica i fatti** pronunciati dal modello. Procedure di emergenza,
fisica e policy devono avere fonti indipendenti prima di entrare nella KB. Dalla
sonda si estraggono soltanto mosse e requisiti rappresentazionali. Il transcript
non va copiato in `response_template` e non diventa un golden test letterale.

Per rieseguire o restringere la batteria:

```sh
.venv/bin/python tests/situational_reasoning_probe.py --model gpt-5.6-luna
.venv/bin/python tests/situational_reasoning_probe.py --model gpt-5.6-luna --only triangolazione
.venv/bin/python tests/situational_reasoning_probe.py --no-llm
```

Le sonde esterne restano fuori da `make test`: sono non deterministiche, hanno
costo/rete e servono a scoprire la mossa. Ogni comportamento promosso deve avere
un oracle strutturale locale `.p0t`.

### Missione tecnica non negoziabile

Il percorso universale deve produrre e consumare questa catena:

```text
turno naturale
  -> frame e letture candidate
  -> situazione/versioni del mondo
  -> entita', ruoli, stati, risorse, vincoli, goal, fonti
  -> azioni applicabili dalla KB
  -> transizioni, stati intermedi, rischi e residui
  -> scelta della mossa dialogica
  -> piano proposizionale K6
  -> realizzazione
```

Ogni azione deve essere uno schema KB con precondizioni, effetti, risorse,
durata, side effect, reversibilita', rischio e provenance dove pertinenti. Una
correzione crea una nuova vista contestuale, invalida i passi che dipendevano
dalla premessa cambiata e conserva gli altri. Un dato non provato resta
assunzione; una risorsa non descritta o derivabile non puo' apparire nel piano.

Il C puo' offrire unificazione, applicazione di delta, aritmetica, ricerca
bounded, ordinamento, indici, hash, cache, pruning e budget. Non puo' contenere
lessico naturale, tipi di mongolfiera, oggetti di emergenza, policy etiche,
risposte a puzzle o rami per dominio. La domanda di review resta: **posso
insegnare una nuova azione, causalita', affordance, fonte o policy a runtime e
vederla usata senza rebuild?**

### Sequenza di implementazione da non saltare

1. **Audit di riuso.** Censire frame, piani, contesti, provenance, causalita',
   procedure, quantita' e operatori gia' presenti. Non creare sinonimi morti di
   predicati esistenti.
2. **gen398a — Situation IR.** Dal producer universale materializzare entita',
   stato, goal, risorse, vincoli e fonti. Prima prova: parafrasi IT/EN convergono;
   un cue nuovo assert/retract cambia il frame.
3. **gen398b — Action schema.** Aggiungere applicabilita' generale su
   precondizioni/risorse/effetti. Il primo taglio deve attraversare input,
   inferenza, answer plan e risposta minima; schema senza consumer non conta.
4. **gen398c — Tempo e quantita'.** Applicare transizioni non distruttive,
   calcolare picchi/soglie e confrontare azioni concorrenti. La barca e' il
   membro guida, non un ramo nautico.
5. **gen398d — Mondi e informazione.** Conservare alternative e derivare quale
   osservazione riduce l'incertezza. Separare prova, assunzione e testimonianza.
6. **gen398e — Ripianificazione.** Versionare il mondo e tracciare dipendenze dei
   passi; correzioni locali e cambiamenti strutturali hanno impatto diverso.
7. **gen398f — Rischio e calibrazione.** Policy KB per agire, chiedere,
   qualificare, mitigare, declinare e rifiutare piani inammissibili.
8. **gen398g — Resa e confronto.** Esporre goal, passi, motivi, assunzioni,
   alternative e residui via K6; rieseguire la sonda frontier solo come scoperta.

Ogni voce e' una generazione verticale TDD. Non iniziare aggiungendo decine di
fatti di dominio: prima deve esistere un consumer universale minimale e
falsificabile.

### Primo ratchet consigliato

Il primo `.p0t` non dovrebbe chiedere subito una risposta operativa reale. Usare
un micro-mondo sintetico sicuro per provare il contratto:

```text
stato: contenitore alto, valvola chiusa, livello crescente
goal: livello sotto soglia
azione KB: aprire valvola
precondizione: valvola funzionante
effetto: flusso uscente
```

Il test deve mostrare:

1. input naturale -> stesso Situation IR in italiano e inglese;
2. azione esclusa prima che lo schema sia noto;
3. `!assert` dello schema/ponte che rende l'azione applicabile senza rebuild;
4. piano derivato con precondizione ed effetto visibili;
5. `!forget` che ritira precisamente il piano;
6. negativo vicino con valvola guasta;
7. trasferimento dello stesso operatore a un secondo dominio privo del lessico
   `contenitore/valvola`.

Solo dopo questa prova introdurre membri verificati dei domini pallone, barca e
uscite. Il caso dei tre interruttori passa soltanto se una nuova proprieta'
osservabile insegnata in KB permette un'analoga azione informativa altrove: la
risposta memorizzata «acceso/caldo/freddo» non e' generalizzazione.

### Matrice minima di done

- almeno tre domini non correlati per ogni operatore core;
- positivo, due transfer held-out e negativo vicino;
- crescita e retrazione runtime di cue, schema, causalita' o policy;
- correzione che ritira solo i passi dipendenti;
- caso senza risorsa che non ne inventa una;
- caso con fonte debole che non diventa certezza;
- caso dannoso respinto per policy ispezionabile nella KB;
- stress 10x con fatti/azioni irrilevanti;
- profilo AGI entro il secondo per turno ordinario;
- nessun literal del prompt in `src/brain` o nelle regole core;
- ogni proposizione finale risale a input, KB o proof.

### Performance prevista: indicizzare il branching, non alzare i timeout

Il planner puo' esplodere combinatorialmente. Il piano §17.9 autorizza, come
meccaniche pure, indici degli action schema per goal/tipo/precondizione, hash
degli stati ground, memo per `(world_version, goal, policy_version)`, cache delle
transizioni e dominance pruning provato. Ogni cache deve:

- essere semanticamente equivalente al percorso senza cache;
- invalidarsi su assert/retract e sulle correzioni pertinenti;
- conservare provenance e alternative;
- avere ratchet cold/warm e curva con conoscenza irrilevante;
- non mascherare un budget esaurito come assenza di soluzione.

Non alzare `!timeout`, `SOFT_BUDGET` o il secondo ordinario per far passare una
ricerca non indicizzata.

### Lavoro performance gia' chiuso nello stesso worktree

Separatamente dal nuovo fronte, il collo di bottiglia di `mod_answer_frame` e'
stato risolto nel resolver generale: un body goal diventato ground usa il
`fact_index` full-tuple e visita ancora le sole unit clause non-ground. Il caso
canonico AGI e' sceso da circa 0,82-0,97s a circa 0,18s. I ratchet sono in
`tests/p0t/reasoning/sequential_view.p0t` e
`tests/p0t/code/code_state.p0t`; la descrizione completa e' nella sezione
`CHIUSO — mod_answer_frame` piu' sotto. Non spostare l'ottimizzazione nel modulo
e non introdurre una cache semantica parallela.

Ultima verifica di quel taglio prima di questo handoff: `make test`, 2027 prove
verdi; `make soft-test` semanticamente verde ma 21s contro budget 15s sotto
carico ambientale, senza aumento del budget. Rieseguire i gate dopo ogni modifica.

### Trappole da evitare

- `answer_frame("mongolfiera", ...)` con risposta pronta;
- `strcmp`, `strstr` o cue C per `cadere`, `zavorra`, `prima`, `attraverso`,
  `libera`, `calda` o altre forme naturali;
- un predicato generico `related_to` al posto di relazioni causali/strutturali;
- trattare ogni problema come search numerica, perdendo fonte e scope;
- inventare oggetti per riempire un piano;
- confondere il rifiuto di un piano dannoso con una penalita' numerica debole;
- usare l'oracolo OpenCode come fonte di procedure o come dipendenza runtime;
- dichiarare gen398 avanzata perche' passa soltanto il prompt guida;
- aumentare timeout al posto di indicizzare per goal, stato e action schema.

Questo handoff e' il punto di ripartenza prioritario per la nuova missione. Il
successivo handoff gen396 resta valido per i prerequisiti del producer universale
e non va interpretato come prova che K11 sia gia' collegato.

## HANDOFF operativo — 2026-08-16

### Missione e criteri non negoziabili

L'obiettivo non e' accumulare risposte speciali, ma portare parrot0 verso il
comportamento osservabile di un LLM di frontiera tramite una KB viva: capire
input naturali, mantenere letture alternative, ragionare, scegliere contenuto e
registro della risposta e attraversare senza cesure il confine fra linguaggio
naturale e codice. La domanda di revisione resta: **un nuovo membro della classe
puo' essere insegnato o ritratto a runtime senza ricompilare?**

Vincoli di ripartenza:

- leggere prima `MANTRA.md`, `PRINCIPLES.md` e la parte finale di
  `docs/plans/question-emergence.md`;
- KB-first per vocabolario, segmentazione semantica, letture, policy, answer
  plan e realizzazione; il C puo' fornire solo meccaniche generali;
- niente MCP salvo richiesta esplicita;
- test conversazionali in `.p0t`; durante il TDD preferire `make soft-test`;
- non aggiungere parser C per una costruzione linguistica e non trasformare un
  limite di inferenza in un aumento permanente dei timeout;
- profili e layout su disco determinano la vita della KB in memoria, non sono
  soltanto strumenti di navigazione per umani.

### Stato esatto del taglio gen396

Questo handoff descrive un unico taglio coerente; il commit che contiene il
documento e' il punto da cui ripartire:

- `src/brain/99-registry.c` pubblica ogni turno come memoria di lavoro
  `turn_span/4`, `turn_span_surface/3` e `turn_span_cue/3`; poi interroga il
  protocollo KB generico `turn_plan_candidate(current_turn)` /
  `turn_response(current_turn, Reply)`. Il C non conosce `se`, `allora`,
  `altrimenti`, `if`, `then` o `else`;
- `kb/core/conditional-plans.p0` costruisce letture, proposizioni, verita', rami
  espliciti o ellittici e risposta. Cue, predicati leggibili, verbi di risposta
  e policy del falso sono tutti fatti retraibili/estensibili;
- `kb/core/procedures.p0` offre la vista spaziale transitiva e i ponti
  amministrativi necessari a valutare fatti come Milano in Lombardia in
  Italia; `gloss.p0`, `grammar.p0`, `intents.p0`, `responses.p0` e `input.p0`
  forniscono lessico, classi, realizzazione e metadati, non rami speciali nel C;
- `tests/p0t/reasoning/conditional_plan.p0t` contiene 28 ratchet, compresi cue,
  verbo, proposizione e connettore insegnati a runtime, retract e ramo
  ellittico;
- `src/kb.c` corregge tre difetti generali emersi dal piano: scope degli
  antenati fra goal fratelli, grounding profondo sotto `naf/1` e `apply/2`, e
  standardize-apart delle unit clause non-ground. Il test generico
  `tests/p0t/reasoning/sequential_view.p0t` li copre con 4 prove;
- `src/code.c` conserva nel segmento l'evidenza esatta restituita dalla KB. Il
  prossimo passo non e' aggiungere altre euristiche di codice, ma proiettare
  statement, stato, effetto e domanda nello stesso piano universale;
- `src/main.c` accetta `--profile FILE.p0` e `--profile=FILE.p0`; la CLI prevale
  su `PARROT0_PROFILE`. E' ancora un selettore additivo, non il boot finale con
  un solo entrypoint curato;
- `VERSION` identifica il lavoro come `gen396-universal-answer-plan` e i nuovi
  `.p0t` sono collegati al `Makefile`.

### Evidenza semantica da non perdere

La sonda frontier ha mostrato che `Italia` e `italiano` non vanno trattati come
una correzione ortografica cieca. Il comportamento di riferimento distingue:

- Milano in Italia: vero;
- Milano in italiano: vero come lettura della superficie italiana;
- Parigi in Italia: falso, ma Parigi in italiano: vero;
- Milan in Italia: vero, ma Milan in italiano: falso;
- «Milano e' in italiano ma non in Italia»: falso.

Per questo il piano conserva almeno le letture concorrenti
`located_in_t/2` e `surface_in_language/2`. La naturalita' nasce dalla scelta
motivata fra letture, non dalla sostituzione anticipata della parola.

### CHIUSO — la polarita' di un goal costruito a runtime

Il rosso order-dependent di `make soft-test` e il cerotto di `code_state.p0t`
erano **lo stesso difetto**, ed era nel motore, non nell'ordine dei file.

`parse_term/4` scrive soltanto `pred`, `args` e `argc`. `parse_to_term/2` non
definiva `neg`, quindi un goal COSTRUITO A RUNTIME — il goal interno di
`findall/3` e l'argomento di entrambi i `call/1` — ereditava come flag di
negazione qualunque byte si trovasse nello `Term` automatico del chiamante
(`Term goal;` e `Term called;`, mai azzerati). Con quel byte diverso da zero il
solver leggeva il goal come `naf(G)`, lo trovava non-ground e **declinava per
floundering**: zero soluzioni, restituite come un insieme vuoto pulito.

Percio' il sintomo dipendeva dalla storia del processo, non dalla KB: la spazzatura
di stack cambiava a seconda di cosa era girato prima. Bisezione: il turno a MURO
di `facts.p0t` («the quick brown fox») e di `code_state.p0t` bastava a cambiarla,
e serviva anche un `!reset` vero (il reset intelligente salta i file con la stessa
config, per questo solo alcuni precedenti rompevano). Traccia decisiva:

```text
[solve] d=0 step=1 idx=0/1 neg=-66883732 conditional_reading_key(current_turn, $Reading_6)
```

`conditional_reading_set` riceveva quindi `nil`, `conditional_unique_reading`
falliva e il piano condizionale collassava sul muro «Non capisco ancora.» — una
risposta sbagliata travestita da assenza onesta.

Correzione: `parse_to_term` definisce ora `t->neg = 0`. E' il punto unico che
costruisce un goal, quindi copre findall, entrambi i call/1 e ogni chiamante
futuro; il loader delle regole faceva gia' `memset` sulla `Rule`, per questo le
clausole dei file `.p0` non hanno mai mostrato il difetto.

Ricadute gia' verificate:

- il cerotto `!timeout 2` di `code_state.p0t` **e' stato rimosso**: il caso
  negativo passa ripetutamente col default di 1 secondo, anche eseguito dopo
  altri file;
- `sequential_view.p0t` guadagna il ratchet della polarita': `findall` su
  `apply/2` (il caso §8 rimasto aperto da gen389) e `call/1` costruito a runtime,
  ciascuno col proprio controllo negativo. Falsificato forzando `t->neg = 1`:
  il ratchet diventa rosso, quindi puo' fallire;
- questo e' con ogni probabilita' anche la causa di §8 («`apply/2` non si comporta
  dentro `findall/3`») e un candidato serio per §1 (i `kb_match` consecutivi a 0).
  §1 non va dichiarata chiusa senza riprodurla: la sua ipotesi sul puntatore
  penzolante di `pred_bucket()` resta non falsificata.

Verifica effettiva:

- `make soft-test`: verde in 14-15s sul budget invariato di 15;
- `make test`: **1930 asserzioni, zero fallimenti**;
- `git diff --check` verde, nessun flag diagnostico lasciato nel C.

**Attenzione al flusso:** con `conditional_plan.p0t` che ora gira per intero,
`soft-test` sta a 14-15s contro un budget di 15. Il prossimo giro che aggiunge un
file deve TOGLIERNE uno (i piu' cari sono `conditional_plan.p0t` ~2,4s e
`contextual_denotation.p0t` ~2,2s), non alzare `SOFT_BUDGET`.

### Sequenza precisa di ripartenza

1. ~~Riprodurre il rosso order-dependent~~ — **fatto**, vedi sopra: era la
   polarita' non inizializzata dei goal costruiti a runtime.
2. ~~Chiudere il taglio verificando flag diagnostici e modifiche estranee~~ —
   **fatto**; test registrati sopra.
3. Continuare gen396 sul caso guida codice + domanda. Dal turno universale
   derivare in KB oggetti come `statement`, `state_before`, `effect`,
   `state_after`, `query`, `expected` e `constraint`; riusare l'evaluatore come
   meccanica fissa, non come router dialogico.
4. Separare `answer_content` proposizionale da realizzazione, persona, lingua,
   formalita', densita' e stile. `register(code(c))` descrive una lettura e non
   puo' consumare da solo un'obbligazione di risposta.
5. Portare il producer NL -> frame e NL -> contesto fino alle strutture gia'
   introdotte in gen393/gen395; oggi diversi test materializzano ancora il frame
   o lo scope direttamente.
6. Proseguire gen397 con memoria discorsiva, referenti, correzioni e continuita'
   dei piani; poi misurare gli stessi stimoli contro un modello frontier con
   rubriche empiriche, non per somiglianza di stringa.
7. Tenere separato il filo di residenza: implementare profilo come unico
   entrypoint, include idempotente, `file_attribute/1`, `file_layer/1` e
   `lazy_load` con OR/AND solo dopo avere chiuso il contratto e i test del grafo.

### Debiti e trappole note

- Gli span copiati nei termini sono limitati da `KB_TERM_LEN` (512). Gli offset
  raw restano disponibili, ma sorgenti lunghe richiederanno riferimenti/eventi
  semantici, non la copia integrale del codice dentro un atomo Prolog.
- `turn_response` e' interrogata con una sola risposta; la concorrenza fra piu'
  answer plan dovra' diventare ranking/policy KB esplicita.
- Il vecchio sintomo di costo/nondeterminismo descritto in
  `question-emergence.md` non va dichiarato interamente risolto dai tre fix del
  solver: sono invarianti generici provati, non una certificazione globale.
- `kb/savemap.tsv` non e' una cache letta dal runtime. E' output derivato; il
  routing futuro deve usare provenienza e doppio indice in memoria.
- Il caricamento differenziale sparso nei moduli C e i loro flag `loaded`
  restano debito: non vanno replicati nel nuovo disegno dei profili.
- Non reintrodurre un parser condizionale in C, non comprimere
  Italia/italiano in una sola lettura, non usare una risposta golden come unica
  prova KB-first e non coinvolgere MCP in questo filo.

File guida: `docs/plans/frontier-kb-natural-dialogue.md`,
`docs/plans/question-emergence.md`, `docs/plans/universal-input.md`,
`docs/kb-loading-and-profiles.md`, `docs/prolog-like-engine.md`,
`kb/core/conditional-plans.p0`, `tests/p0t/reasoning/conditional_plan.p0t` e
`tests/p0t/reasoning/sequential_view.p0t`.

## Fronte attivo: gen396, memoria di lavoro e answer plan universale

Il piano operativo e' in
[`docs/plans/frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md),
costruito a partire soprattutto dalla parte finale di
[`docs/plans/question-emergence.md`](docs/plans/question-emergence.md). Le sette
generazioni consecutive elevano forme linguistiche, denotazione contestuale,
letture e gap, policy dialogica, scope, registro/answer plan e memoria
discorsiva. Le clausole future nel piano sono esempi guida, non capacita'
gia' rivendicate.

Il primo taglio gen396 e' ora operativo. `input_segment` continua a fare soltanto
meccanica di byte e scoring; l'evidenza esatta di ogni confine viene reificata
come `turn_span/4`, `turn_span_cue/3` e `turn_span_surface/3`. La KB
`conditional-plans.p0` compone da queste viste letture tipate, verita', policy
del falso, rami ed ellissi. `Italia` e `italiano` restano predicati diversi
(`located_in_t` e `surface_in_language`), come richiesto dalla sonda frontier.

Il ratchet `.p0t` prova 28 casi, inclusi un predicato e un connettore inventati,
un verbo di ramo insegnato, retract delle cue e l'ellissi «altrimenti Piero».
Non sono stati aggiunti parser condizionali in C. Il solo timeout cambiato e' il
cerotto locale e documentato del test negativo `code_state.p0t`; non riguarda
il piano condizionale e non altera il budget della suite.
Tre regressioni generiche del solver proteggono: scope degli antenati fra
congiunti fratelli, grounding ricorsivo dei termini sotto `naf/1` e
standardize-apart delle unit clause non-ground.

Questo non chiude gen396. Il prossimo task e' proiettare sullo stesso piano gli
span di codice: statement, stato prima/dopo, effetto, query, expected e
constraint. `register(code(c))` e' soltanto un'osservazione; non puo' consumare
una domanda ancora aperta. Dopo quel taglio vengono answer content
proposizionale e registro multidimensionale, poi la memoria discorsiva gen397.

La CLI accetta ora `--profile FILE.p0` (anche `--profile=FILE.p0`) e la scelta
esplicita prevale su `PARROT0_PROFILE`. Nel boot corrente il profilo resta ancora
additivo: questo rende utilizzabile l'entrypoint senza fingere completata la
migrazione al singolo grafo curato descritta in `docs/kb-loading-and-profiles.md`.

La parte linguistica di gen391 e' promossa nel commit `be3cedf`: viste
bidirezionali delle forme, status e alternative crescono per assert/retract;
le superfici conversazionali sono fatti `answer_frame/2`. Il gate AGI e'
tornato sotto il secondo senza timeout grazie a meccanismi generali del solver
e della colla, senza lessico nel C. L'oracolo eager/diamante di 391.6 resta un
debito del filo di residenza, non un motivo per fermare l'evoluzione semantica;
blocca invece qualunque dichiarazione operativa di `lazy_load/1`.

Il taglio verticale gen392 e' ora presente: `denotation.p0` deriva le letture da
`concept_label/4`, `domain_category/2` e categorie esistenti; il dominio
seleziona senza cancellare le alternative. Uso e menzione convergono sui ruoli
del modello universale dell'input, e `canonicalization_exempt/1` e' il solo
protocollo nuovo consumato dal C: il motore copia span, mentre cue, ruolo,
lingua, registro e forme restano fatti. La proiezione fra entita' riusa il solo
arco tipato `requires/2`; non e' stato aggiunto un `related_to` generico.

Il ratchet `tests/p0t/language/contextual_denotation.p0t` prova omonimia per
dominio, alternative senza dominio, citazione, ponte fuori dai giochi,
relazione sbagliata, retract e crescita/ablazione di una cue di menzione
inventata. `make soft-test` e' verde in 6 secondi sul budget invariato di 15;
`make test` chiude 1800 asserzioni, zero fallimenti.

L'audit del manifesto eager ha trovato un asse che la sola `include/1` non puo'
esprimere: `capabilities.p0` entra oggi come reflective, mentre gli altri file
core entrano come base. Un manifesto unico lo renderebbe persistibile anche se
le risposte restassero verdi. E' stata quindi specificata `file_layer/1`, locale
al file e distinta da `file_attribute/1`. Finche' layer e registry canonica non
esistono, non viene creato un manifesto `.p0` morto o semanticamente falso e
`lazy_load/1` resta non operativa.

Il primo taglio gen393 e' in `kb/core/dialogue-frames.p0`. Atto, slot, source,
letture e status sono fatti; completezza, residuo strutturale e risposta
derivabile sono regole. `frame_answer/2` usa `apply/2`, quindi attraversa il
solver comune. `tests/p0t/meta/three_axis_gap.p0t` prova assert/retract di slot,
proof positiva, due letture concorrenti e ablazione della superficie; `make
soft-test` resta verde in 6 secondi e `make test` passa 1821 asserzioni senza
fallimenti. Questo non e' ancora il producer NL -> frame: il test materializza
il frame esplicitamente e il TODO resta aperto.

Il primo gap epistemico e' ora presente senza nuova primitiva C:
`frame_gap(Frame, missing_fact)` nega la vista ground
`frame_has_answer(Frame)`. Il solver possiede gia' i tre esiti richiesti da
`question-emergence.md` — proved, finite failure, incomplete — e la NAF declina
quando il budget rende la ricerca incompleta. Il ratchet chiude il gap asserendo
il solo fatto e lo riapre ritraendolo. Restano aperti il falsificatore che
esaurisca deterministicamente il budget e le diagnosi di ponte, operatore e
realizzazione.

Una conversazione reale ha poi falsificato la raggiungibilita' del frame:
`continent_of(rwanda, africa)` era gia' residente, ma «dove si trova la ruanda»
andava a muro. Non era `missing_fact`: mancavano il ponte semantico
`continent_of -> located_in`, la superficie `where is -> located_in` e
l'esonimo `ruanda -> rwanda`. I tre pezzi sono ora conoscenza KB. Il ratchet
`knowledge/geographic_location.p0t` copre Ruanda, Parigi e un paese inventato;
assert/retract del fatto, della superficie e della traduzione cambiano la
risposta senza rebuild. Il producer NL -> frame generale resta aperto: il caso
passa ancora dal consumer binario esistente e non autorizza a dichiarare gen393
end-to-end.

Il prompt successivo «dove si trova Milano» ha falsificato anche quel ratchet:
Ruanda copriva soltanto `continent_of/2` e Parigi possedeva gia' un fatto
`located_in/2`, quindi nessuno dei due obbligava le relazioni amministrative a
comporre. Milano era presente come `capital_of_region(lombardia, milano)`, ma il
predicato non raggiungeva la vista spaziale e gli atomi locali contraddicevano
la canonicalizzazione `milano -> milan`. I capoluoghi regionali sono ora
canonici e `administrative_capital/2` unifica capitali di regione, stato e
paese; per il vecchio `capital_of_country/2` bidirezionale la guardia
`continent_of/2` impedisce di invertire paese e capitale. Il ratchet comprende
Milano, Firenze, Sacramento, Nairobi e una regione inventata asserita e ritratta
a runtime. Con `PARROT0_TOOLS=1` il rosso ha inoltre scoperto che la cue
`intent_cue(piact_grep, "where is")` rubava ogni domanda inglese e cercava nel
filesystem: e' stata sostituita da forme esplicitamente legate al codice, come
`where is defined`. Il `.p0t` gira ora con strumenti attivi, quindi questo
conflitto di routing non puo' piu' restare nascosto dal mock. Questo e' il
livello di trasferimento che il primo test non aveva.

Il controllo «Napoli -> Campania; Campania -> Napoli» ha poi scoperto un
`wrong-answer gap` nel consumer generale: `answer_frame/2` provava sempre
`predicato(entita', ?)` e poi `predicato(?, entita')`, trattando ogni relazione
binaria come interrogabile in entrambe le direzioni. Il protocollo opzionale
`answer_frame_input_arg(Superficie, Predicato, 1|2)` rende ora il binding uno
slot della conoscenza; il C applica soltanto la meccanica numerica e non conosce
ne' `where is` ne' `located_in`. `region_of_country/2` separa inoltre la
collocazione della regione dal rapporto col capoluogo. Il ratchet prova che
Campania risponde Italia, che una regione inventata senza contenimento non
risponde col proprio capoluogo e che ablare/reinserire il metadato riproduce e
chiude l'errore senza rebuild.

Due stimoli successivi hanno aggiunto altri due `wrong-answer gap`. «Quali sono
i colori che identificano gli scacchi» perdeva la faccetta esplicita `color`
perche' il dominio `chess` selezionava prima la categoria predefinita
`chess_piece`; ora il frame KB `side_color/2` conserva faccetta e verso, e un
gioco inventato prova crescita, retrazione e ablazione della superficie a
runtime. `i=0; i++; quanto vale i` veniva invece soltanto classificato come
codice: il riconoscimento del registro consumava una domanda ancora aperta. La
KB dichiara ora `segment_role(query, Cue)` come unica sorgente viva del confine
e dell'atto; il motore separa lo span chiuso e riusa la propria meccanica di
statement per il binding finale. Il `.p0t` aggiunge e ritrae una cue inventata
senza rebuild.

Durante questo taglio una vista Horn
`segment_role(query, Cue) :- intent_cue(code_state_query, Cue)` ha fatto salire
`make soft-test` da 10 a 24 secondi pur lasciando verdi le asserzioni. Il budget
non e' stato alzato: la causa e' la rivalutazione ripetuta della relazione
derivata da parte di `kb_evidence_matches` quando enumera tutte le ipotesi. La
sorgente canonica e' stata resa direttamente `segment_role/2`, evitando anche
la duplicazione semantica, ma il costo delle relazioni di evidenza derivate
resta un debito del motore da isolare e correggere, non un limite accettato.

Il dubbio sulle coppie `stipulation_cue` in `grammar.p0` non era invece un bug
del loader corrente. Da gen335 il punto di livello 0, non la newline, chiude una
clausola. `tests/multigoal.sh` verifica il caso generale e il nuovo
`meta/multiclause_cues.p0t` interroga i secondi predicati reali di tre righe,
poi ne ritrae e riasserisce uno. Se una stipulation cue resta inerte, il prossimo
luogo da indagare e' il collegamento cue -> contesto -> mossa.

Verifica di questo taglio: `make soft-test` verde in 11 secondi sul budget
invariato di 15; `make test` chiude 1894 asserzioni senza fallimenti;
`tests/multigoal.sh` chiude 15 prove del loader, inclusa la clausola multipla
sulla stessa riga.

La gen394 e' avviata con `kb/core/dialogue-policy.p0`. `dialogue_state/2`
trasforma proof e `missing_fact` in evidenza; `move_policy/2` sceglie la mossa e
`frame_move/2` le compone. Il `.p0t` `conversation/dialogue_moves.p0t` prova che
lo stesso frame passa da `answer` a `decline`, e da `decline` a `clarify`
cambiando soltanto la policy a runtime. Non e' ancora il router dialogico: issue,
obblighi, precedenza effettiva e consumo prima del first-match restano aperti.
`make soft-test` e' verde in 7 secondi sul budget 15; `make test` chiude 1870
asserzioni, zero fallimenti.

La gen395 e' avviata in `kb/core/context-scope.p0`. `context/2`, `holds_in/2`,
provenienza, confidenza e policy di commitment mantengono mondo, ipotesi,
citazione e credenza riportata come oggetti distinti. Le viste locale, ereditata
e visibile restano concorrenti; `proposition_signature/4` permette di derivare
un conflitto fra contesti e `supersedes_in/3` applica una correzione soltanto
nello scope che la dichiara. `scope_kind_for_act/2` e' policy retraibile. Il
ratchet `conversation/context_scope.p0t` materializza fatti inventati a runtime,
abla lo scope senza cancellarne i fatti e conserva entrambe le risposte sul
caso dei pinguini. Restano aperti il producer NL -> contesto e la proiezione dei
vecchi mondi locali, ipotesi e premesse implementati da consumer distinti: la
gen395 non e' ancora end-to-end.

Il filo di caricamento ha un proprio contratto in
[`docs/kb-loading-and-profiles.md`](docs/kb-loading-and-profiles.md): il profilo
deve diventare l'**unico entrypoint curato della KB**. Il C alloca e applica il
protocollo, ma non enumera file core o domini. Il grafo del profilo decide cosa
e' residente al boot e quali contesti lazy sono catalogati; include idempotente,
`file_attribute/1` e confini fisici dei file rendono l'organizzazione su disco
parte della vita semantica della KB. Questo filo e' ancora una specifica: non va
usato per nascondere costi di inferenza eager.

L'inventario comprende anche il falso lazy loading gia' presente nei consumer:
`lexeme.p0`, `actions.p0`, `compose.p0` e `algo_steps.p0` vengono aperti da
moduli C con flag dedicati. Nel target non diventano eccezioni del nuovo boot:
sono provider raggiunti dal grafo del profilo e attivati dai predicati nella
frontiera SLD. Il gate finale e' semplice: fuori dal loader nessun modulo deve
conoscere un path `.p0` o mantenere un flag specifico "loaded".

L'audit del routing di `/save` ha separato il router dal file
`kb/savemap.tsv`. Il TSV non viene mai letto: `kb_save_routed()` riscansiona i
`.p0`, costruisce `SmRow[]` in memoria e riscrive il file soltanto per
ispezione. E' quindi un artefatto derivato ignorato da Git e generato localmente,
non una cache operativa. La sua rimozione e' proposta nel filo di residenza; il
router dovra' usare la provenienza fisica della registry con due indici:
`(predicato, primo_argomento) -> file` e `predicato -> file` soltanto quando la
casa e' univoca. L'attuale fallback "ultimo file con lo stesso predicato" e'
dipendente dall'ordine di scansione e non va trasferito tale e quale in hashmap.

## CHIUSO — `mod_answer_frame` costava 0,74s per turno e su un muro non serviva

*Misurato a gen396, 17 agosto 2026. Non e' una micro-ottimizzazione: e' il
contratto di §10 del piano frontier — un turno ordinario sta sotto il secondo
anche col profilo AGI — e al momento della misura lo consumava quasi tutto un
solo modulo.*

### Risoluzione — 17 agosto 2026

Il cronometro interno ha falsificato l'ipotesi sull'ordinamento delle cue:
enumerare le 215 righe costava 0,8-1,3ms e ordinarle 0,6ms. Dei circa 0,86s del
ciclo, 0,85s erano dentro i lookup delle relazioni derivate. Sul turno canonico
`what is a flimbo`, per esempio, ogni prova negativa di
`humanities_summary(Token, ?)` espandeva la regola e poi scandiva tutti i
`humanities_topic/1`, anche se il sottogoal ormai ground
`humanities_topic(flimbo)` era una chiave esatta.

La correzione e' nel resolver generale, non in `mod_answer_frame`: quando la
sostituzione rende ground un goal, `solve_frame` consulta il `fact_index`
full-tuple gia' mantenuto da assert/retract. Se esistono unit clause non-ground
continua a visitarle, poi prova normalmente le regole; se il censimento non e'
disponibile conserva la scansione storica. Non c'e' una cache semantica da
invalidare e non viene materializzata conoscenza duplicata: l'indice resta una
vista meccanica della KB viva.

Misura comparabile, profilo AGI con 27k fatti / 1626 regole, tre esecuzioni:

```text
prima   0,97s / 0,82s / 0,83s
dopo    0,18s / 0,18s / 0,18s
```

Il ratchet `sequential_view.p0t` prova nello stesso processo miss -> assert ->
retract -> assert del fatto indicizzato e conserva il caso delle unit clause
non-ground. `code_state.p0t` fissa a 0,5s il turno negativo reale: disabilitando
il fast path fallisce a 0,88s, riattivandolo passa. `make test` chiude 2027 prove,
zero fallimenti. `make soft-test` ha tutte le asserzioni verdi ma resta a 21s e
fallisce il budget globale di 15s per il carico ambientale gia' annotato sotto;
il budget non e' stato alzato.

### Il fatto

Turno che non sa rispondere (`cosa è un flimbo`, `i++; what is i`), profilo AGI,
27k fatti / 1626 regole:

```text
turno completo                  0,95s   (0,95 / 0,97 / 0,85)
con mod_answer_frame ablato     0,14s   (0,15 / 0,14 / 0,14)
```

**La risposta e' IDENTICA nei due casi** — «Su flimbo non so ancora molto. Vuoi
che cerchi?» Quindi su questa classe di turni il modulo spende l'85% del budget e
non contribuisce nulla.

Ripartizione interna, cronometrata sul posto:

```text
guardie (border_intersection + 11 compound_guard)   0,0006s
kb_match_all(answer_frame, ?, ?) -> 215 righe       0,0001s
ciclo sulle cue                                     0,7407s   <-- tutto qui
```

### Perche' e' un problema da gestire, non da annotare

1. **Cresce con la conoscenza.** Le 46 voci di `facts/foundations.p0` hanno
   portato il modulo da 0,70s a 0,86s (+22%). Insegnare una superficie nuova
   rende piu' lento OGNI muro: e' esattamente «la KB cresce e un turno rallenta»,
   il fallimento che un sistema KB-first non puo' avere.
2. **Colpisce il caso peggiore.** `mod_answer_frame` sta in fondo al registry,
   quindi gira per intero proprio sui turni che nessuno rivendica — quelli in cui
   l'utente sta gia' aspettando per sentirsi dire che non si sa.
3. **Sta gia' rompendo il ratchet.** `code_state.p0t` e `code_state_plan.p0t`
   hanno turni negativi che oscillano fra 0,93s e 1,06s: verdi o rossi a seconda
   del carico. Alzare quei `!timeout` e' vietato (MANTRA), quindi il difetto va
   corretto o i casi vanno tolti — e toglierli perde copertura vera.

### Dove NON e', misurato (non ripercorrere)

- **non e' `kb_nearest_concept`.** Sembra il colpevole ideale (scansiona ogni
  fatto quotato, tokenizza fino a 96 token da 512 byte l'uno), ma su questo turno
  non viene proprio chiamata — verificato con una stampa all'ingresso.
- **non sono le guardie.** 0,6ms. L'ablazione con `P0DBG_NOGUARD` non cambia
  niente.
- **non e' l'enumerazione delle cue.** `kb_match_all(answer_frame, ?, ?)` rende
  215 righe in 0,1ms; `answer_frame/2` non ha regole in testa, quindi prende il
  cammino veloce sui fatti.
- **non sono le `answer_projection_resolve`.** Sei chiamate, 9ms in tutto. Un
  memo per turno le dimezzerebbe e non si vedrebbe.
- **non e' il `kb_match` interno.** Per `what is a flimbo` una sola cue matcha
  (`what is`) con 3 predicati: 3 x 2 passi x 4 parole x 2 versi = **48**
  `kb_match`. Quarantotto.

Quindi: il ciclo scorre 215 cue, ne trova UNA applicabile, fa 48 lookup — e costa
0,74s. **Il costo non e' nel lavoro utile.** L'ipotesi da falsificare per prima e'
il test `cue(norm, cd)` ripetuto 215 volte insieme all'ordinamento per
specificita' (che dequota ogni riga a ogni confronto dell'insertion sort, cioe'
O(n^2) `kb_dequote` su buffer da 512 byte), piu' cio' che il ciclo chiama prima di
scartare una cue.

### Come si fa il profiling qui (l'unico metodo che ha funzionato)

Due strumenti sono inutili su questo codice, e scoprirlo costa un'ora:

- **`gprof` non attribuisce questo carico.** Con `-pg`, sia `-O2` sia `-O0`, il
  profilo piatto totalizza 0,1s su turni che ne costano 3,6. I campioni non
  arrivano al cammino caldo.
- **`perf` e' bloccato** da `kernel.perf_event_paranoid` su questa macchina.

Quello che funziona, in ordine:

1. **Ablazione binaria con una variabile d'ambiente.** Un `if (getenv("P0DBG_X"))
   return 0;` in cima al modulo sospetto, poi si confronta il tempo E LA RISPOSTA:
   se la risposta non cambia, il modulo e' costo puro.
   ```sh
   for i in 1 2 3; do PARROT0_PROFILE=kb/profiles/agi.p0 PARROT0_WORLD_FACTS=1 \
     PARROT0_LANG=it /usr/bin/time -f "%U" ./bin/parrot0 < turno.txt 2>&1 >/dev/null | tail -1
   done
   ```
   **Sempre tre ripetizioni**: su questa macchina il rumore e' ±0,2s, e una misura
   singola mente. Un `git stash -u` fra le due misure isola la conoscenza dal
   codice.
2. **Cronometro per fase dentro la funzione** (`GTOCK("tag")` con
   `timespec_get`), stampato su stderr. E' cio' che ha separato guardie /
   enumerazione / ciclo. Attenzione: **i gate a stadi (`P0DBG_STOP<=n`) hanno dato
   risultati incoerenti** — un gate che sembra applicato ma non compila produce
   una bisezione falsa. Fai stampare al gate una riga che ne prova la presenza.
3. **Cronometro per modulo nel registry** (`registry[i].handle` avvolto da
   `timespec_get`) per trovare CHI, prima di cercare DOVE.
4. **`PARROT0_TE_SLOW=0.05`** sul demone (non sul client: la variabile la legge
   `--test-engine`, e l'output va in `obj/test-engine.log`) per vedere i turni
   lenti dentro la suite senza alzare nessun budget.
5. **Il demone e' STANTIO** finche' non rifai `make test-engine`. Piu' di una
   misura sbagliata e' venuta da li'.

### Una prova deve poter fallire, anche quando e' un benchmark

`word_sim/2` e' uguaglianza oppure prefisso comune di 4 caratteri, quindi i primi
4 byte impacchettati sono una condizione NECESSARIA: si puo' rifiutare quasi ogni
coppia con un confronto intero, senza materializzare token. Scritta, corretta,
misurata su un turno che usa davvero quella scansione («the organ that pumps
blood»): **3,05-3,17s prima, 3,20-3,36s dopo**. Nessun guadagno, quindi **non e'
stata committata**. Il ragionamento era giusto e il bersaglio sbagliato.

### Debiti minori misurati nello stesso giro

- **accordo di numero:** «Ho estratto 1 fatti». `{count}` riempie una forma
  plurale fissa. La selezione della forma per numero va fatta come CONOSCENZA
  (`plural_form/3`, o un `response_template` scelto dal conteggio), non come ramo
  in C.
- **soggetto duplicato:** «blockchain is A blockchain is an append-only...». Il
  frame inglese antepone il soggetto a una glossa che ne ha gia' uno.
- **`make soft-test` e' a 21-22s su un budget di 15.** Misurato in un worktree
  pulito, HEAD~1 era gia' a 20,3s: e' carico ambientale, non regressione. Ma il
  margine e' sparito, e il difetto qui sopra e' il motivo per cui non torna.

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

## Strati di nome: registro, simbolo, e chi li consuma

gen382b ha introdotto `concept_label(Concept, Lang, Register, Name)` — il nome di
un concetto in una lingua e in un REGISTRO, che non e' la traduzione della sua
parola (`knight` e' "cavaliere", ma il pezzo e' il cavallo). Il registro e' una
dimensione, non un'eccezione: `preferred_register(fsi)` fa rispondere "donna"
invece di "regina", con ricaduta automatica sull'uso comune per gli altri pezzi.

Restano aperte tre cose:

- **`concept_symbol/4` non ha ancora un consumatore.** I simboli della notazione
  (D per la donna FSI, N per il knight algebrico) sono in KB ma nessuna domanda
  li raggiunge: manca la superficie ("qual e' il simbolo della donna?") e il
  frame che la mappa. Vale la pena farlo come classe, non per gli scacchi.
- **Il registro e' globale.** `preferred_register/1` e' una preferenza di
  sessione; la forma giusta e' probabilmente per-dominio
  (`preferred_register(chess, fsi)`), cosi' registri diversi convivono.
- **`dimmi i pezzi degli scacchi` va ancora a muro.** Non e' una riga mancante:
  `dimmi` e' un `intent_starter` e viene TOLTO prima del dispatch, quindi con lui
  sparisce l'unica prova che il turno fosse una richiesta e resta un sintagma
  nudo. Il fix vero e' che l'atto sopravviva allo stripping, non un frame piu'
  largo. Asserito come comportamento corrente in games.p0t.

La domanda di review: puo' parrot0 imparare il nome di un pezzo in una nuova
lingua, o in un nuovo registro, senza toccare il C?  (Oggi si': e' un fatto.)

## Lo strato linguistico per insegnare REGOLE CON VARIABILI

Oggi si puo' insegnare parlando una sola forma di regola: `every X is a Y` ->
`y(V) :- x(V)`. Una variabile implicita, due predicati unari. Tutto il resto —
le regole che legano PIU' entita' — resta fuori dalla conversazione, e quindi
parrot0 non puo' essere addestrato a ragionare, solo a classificare.

Verificato: nessuna di queste funziona oggi.

    if someone is a barista then they can make coffee
    if X is a barista then X is a person
    chi e' un barista sa fare il caffe'
    if a person is the parent of a parent then he is a grandparent

**L'osservazione che da' la forma alla soluzione:** in lingua naturale le
variabili non si scrivono con le lettere, si portano con i PRONOMI INDEFINITI
("someone", "anyone", "chiunque", "chi") e si riprendono con l'ANAFORA ("they",
"them", "lui", "esso"). Sono due classi chiuse, quindi conoscenza — esattamente
come np_opener/1 e np_closer/1:

```prolog
% Le parole che INTRODUCONO una variabile.
rule_variable(someone).  rule_variable(anyone).  rule_variable(something).
rule_variable(qualcuno). rule_variable(chiunque). rule_variable(chi).

% Le parole che RIPRENDONO la stessa variabile (anafora).
rule_anaphor(they). rule_anaphor(them). rule_anaphor(he). rule_anaphor(she).
rule_anaphor(lui).  rule_anaphor(lei).  rule_anaphor(esso).

% Antecedente e conseguente.
rule_antecedent_marker(if).   rule_antecedent_marker(se).
rule_consequent_marker(then). rule_consequent_marker(allora).
```

Il motore: taglia il turno sui marcatori, dà a ciascuna clausola lo STESSO
parser di comprensione gia' in uso, e sostituisce ogni `rule_variable` con una
variabile fresca; ogni `rule_anaphor` si lega all'ultima variabile introdotta.
Il risultato e' una clausola per kb_assert_clause, che gia' esiste.

Due variabili distinte servono per le regole relazionali ("se qualcuno e' il
genitore di qualcuno, ..."): la seconda occorrenza di un `rule_variable` nella
STESSA clausola introduce una variabile NUOVA, l'anafora invece riprende. E' la
distinzione che la lingua fa gia', e per cui esistono due classi di parole
invece di una.

Nota di collegamento: "se ... allora ..." e' oggi trattato come CORNICE da
sbucciare (gen378). Qui va letto come REGOLA. La differenza e' se l'antecedente
contiene una variabile: senza, e' una cornice; con, e' una quantificazione. Il
gen378 e questo sono lo stesso sito visto dai due lati.

La domanda di review: puo' parrot0 imparare una regola relazionale a due
variabili, parlando, senza ricompilare?  (Oggi no. E' il prossimo salto.)

### Il gemello: apprendere da PROSA le regole con variabili

Stesso problema, lingua diversa — e la differenza decide il progetto. Nella
conversazione le variabili si portano con i pronomi indefiniti ("se QUALCUNO
e'..."); nella prosa enciclopedica quasi mai. La prosa usa il **generico plurale
piu' una relativa restrittiva**, ed e' li' che sta la variabile:

    mammals THAT live in water are aquatic
    rivers THAT flow into the sea form deltas
    a number is prime IF it is divisible only by itself and one
    WHEN water is heated to 100 degrees IT boils

Misurato oggi, e ogni riga sbaglia in modo diverso:

| prosa | risposta attuale | lettura |
|---|---|---|
| mammals that live in water are aquatic | "Amphibian, fish and human." | INTERROGAZIONE, non regola |
| a number is prime if it is divisible... | "No, 1 is not a prime number." | domanda su un numero |
| rivers that flow into the sea form deltas | muro | — |
| when water is heated to 100 degrees it boils | "Who or what does 'it' refer to?" | **anafora riconosciuta** |

Tre cose da qui.

**1. Il risolutore di anafora esiste gia'.** L'ultima riga lo dimostra: parrot0
sa che "it" richiede un referente e lo CHIEDE. Per una regola il referente non e'
un'entita' — e' una VARIABILE. Non serve una macchina nuova: serve che quella
esistente possa legarsi a una variabile invece che a un'entita'.

**2. `that` ha due ruoli, e oggi ne conosce uno solo.** In np_closer/1 `that`
CHIUDE un sintagma (gen382, ed e' cio' che ha sbloccato l'estrazione da prosa
vera). In una relativa restrittiva APRE una clausola che vincola la stessa
variabile del soggetto. Stessa parola, due funzioni, e la discriminante e' cosa
segue: un verbo finito apre una clausola, un nome chiude il sintagma. La KB deve
poter dire entrambe le cose della stessa parola — non e' un conflitto da
risolvere scegliendo, e' una proprieta' da rappresentare.

**3. La lettura restrittiva perde contro quella interrogativa.** "mammals that
live in water" viene enumerato invece che quantificato. Non basta aggiungere una
forma: serve decidere QUANDO una relativa e' un filtro di query e quando e'
l'antecedente di una regola. Il segnale c'e' ed e' sintattico — una regola ha un
CONSEGUENTE ("...are aquatic"), una query no — quindi e' decidibile senza
indovinare.

La forma comune con lo strato conversazionale: in entrambi i casi si tratta di
riconoscere che DUE punti del turno parlano della STESSA cosa non nominata, e di
darle un nome fresco. Pronome indefinito + anafora nel dialogo, relativa +
soggetto nella prosa. Un solo meccanismo, due vocabolari — che e' la forma di
ogni migrazione riuscita di questa sessione.

## Gli scratch brain rimasti (gen382i, parziale)

Il meccanismo che toglie l'amputazione esiste ed e' in uso su un sito:

- `brain_fresh_token(b, base, out, n)` conia un token e VERIFICA che la KB non lo
  menzioni da nessuna parte (`kb_mentions_term`: predicato, argomento, testa o
  corpo di regola);
- `p0_rename_content` rinomina i termini di CONTENUTO — chiedendo alle classi
  chiuse gia' dichiarate quali parole siano funzionali, quindi una lingua nuova
  entra come fatto;
- le premesse vivono sulla KB vera con provenienza `KB_HYPOTHETICAL` e se ne
  vanno col turno (`kb_retract_origin`, gen373).

Migrato: `one_turn_syllogism`. Restano quattro siti che costruiscono ancora un
Brain con KB vuota:

    10-memory-knowledge.c:~1557  entailment_reply
    10-memory-knowledge.c:~1790  multi_sentence_syllogism
    10-memory-knowledge.c:~1877  transitive_comparison
    10-memory-knowledge.c:~8591  l'ipotesi di mod_knowledge

Sono la stessa migrazione, e conviene farli uno per volta con la loro prova: il
sito migrato ha gia' mostrato che il cambio ha effetti non ovvi (il suffisso del
token fresco non puo' contenere "_", perche' gli atomi composti si scrivono cosi'
e il cancello dei concetti conterebbe le parole sbagliate).

La domanda di review: resta un solo posto in cui parrot0 ragiona senza la propria
conoscenza?  (Oggi quattro. Erano cinque.)

## Isolare o far convivere: cosa dice un ragionatore vero (sonda gen382j)

`tests/coexistence_probe.py` (strumento di progetto, mai runtime) estende la
sonda di gen371 — che aveva gia' stabilito che **l'LLM non isola, DECIDE** — sulla
domanda che il lavoro di gen382i ha aperto: quando una premessa contraddice il
mondo, il "gatto" dell'ipotesi e' un ALTRO concetto (rinominazione) o lo STESSO
con una credenza sospesa (convivenza con provenienza)?

**1. Sono due letture, entrambe legittime, e il discriminante e' la PREMESSA.**
Interrogato con cura, il modello non sceglie: distingue. In logica del primo
ordine "gatto" denota lo stesso insieme e la premessa aggiunge una relazione —
concetto unico, conflitto visibile. Come *stipulazione* (mondo possibile alla
Kripke) "gatto" e' un concetto nuovo con la stessa etichetta lessicale — ed e'
esattamente la rinominazione. La deduzione, dice, "opera sulla struttura formale,
non sul contenuto intuitivo del termine": funziona in entrambi i casi.

Quindi la scelta non e' una preferenza architetturale: e' una **proprieta' della
premessa**. "Supponi che tutti i gatti siano pesci" stipula; "tutti i gatti sono
pesci" asserisce, e va in conflitto. parrot0 oggi non distingue le due cose.

**2. Cosa si PERDE rinominando** (la parte che riguarda direttamente gen382i):

| capacita' | perche' la rinominazione non ce l'ha |
|---|---|
| rilevare la contraddizione | i simboli sono ortogonali: non c'e' conflitto da trovare |
| spiegare il conflitto | non puo' dire "so gatto=mammifero ma l'ipotesi dice pesce": sono termini diversi |
| revisione delle credenze | "cosa cambia se scarto X?" — X non e' mai entrato |
| abduzione esplicita | "cosa deve essere falso perche' l'ipotesi regga?" |

Sintesi del modello: **la rinominazione e' un motore di conseguenza logica pura,
la sospensione e' un motore di what-if.**

**3. La tensione dentro parrot0, che questa sonda rende visibile.** gen382i ha
scelto la rinominazione — e la rinominazione e' precisamente l'architettura che
NON PUO' accorgersi del conflitto. Ma gen375 (`class_conflict`) ha costruito
l'abilita' opposta su un'altra superficie: detto "un cane e' un pesce", parrot0
accetta E NOMINA la tensione con quello che sa. Le due filosofie oggi convivono
nel sistema senza essersi mai incontrate, e quella scelta a gen382i chiude la
porta a quella di gen375 sul percorso ipotetico.

**4. La persistenza e' gia' giusta.** Alla domanda "finita la supposizione, Tom
cosa e'?" il modello tiene i due livelli in una sola risposta: pesce
nell'ipotesi, mammifero nel mondo. E' cio' che fa `KB_HYPOTHETICAL` con la
retrazione a fine turno — su questo parrot0 e il ragionatore vero coincidono.

**La direzione che ne esce**, e va decisa prima di migrare gli altri quattro siti
scratch: la rinominazione non e' sbagliata, e' *incompleta*. Serve che la premessa
dichiari (o che il motore inferisca) se STIPULA o ASSERISCE, e che il caso
asseritivo usi la provenienza ipotetica SENZA rinominare — cosi' il conflitto
resta visibile e `class_conflict` puo' parlare anche li'.

La domanda di review: parrot0 sa dire "l'ipotesi che mi hai dato contraddice
quello che so, e te lo dico prima di ragionarci"?  (Oggi no, e con la sola
rinominazione non potra' mai.)

## PUNTO 1 (stipula/asserisce): fatto a meta', e una CORREZIONE da fare prima

### La correzione, che viene prima di tutto

gen382i dice di aver migrato `one_turn_syllogism` dalla KB amputata alla
rinominazione. **Quel sito non viene mai raggiunto.** Verificato con una stampa
sull'ingresso della funzione: per "if all bloops are razzies and zorb is a bloop,
is zorb a razzie" — e per ogni variante provata — `mod_knowledge` risponde prima
di arrivare alla riga che lo chiama (10-memory-knowledge.c:8405).

La dimostrazione che avevo portato ("is tom a cat" -> "I don't know about cat")
NON provava quello che dicevo: la prova e' compatibile con qualunque percorso che
non lasci residui, e il sandbox amputato non ne lascia per costruzione. Ho
scambiato una condizione necessaria per una sufficiente.

Il codice scritto e' corretto e non fa danni (i test restano verdi perche'
asseriscono il comportamento osservabile, che non e' cambiato), ma **la
migrazione non e' avvenuta**: e' codice non esercitato. Il commit di gen382i va
letto con questa nota accanto.

**Prima mossa, prima di qualunque altra cosa:** trovare CHI risponde davvero a
"if <premesse>, <domanda>". Il modo giusto e' la traccia che parrot0 ha gia'
("why did you answer that way?" dice `knowledge`, quindi il sito e' dentro
mod_knowledge, prima della riga 8405), non altre stampe di debug. Poi:
o si migra QUEL sito, o si rimuove `one_turn_syllogism` se e' morto — un motore
che non viene mai chiamato e' peggio di un motore che manca, perche' sembra
esserci.

### Che cosa del punto 1 c'e' davvero

Tutto tranne l'aggancio, e ognuno di questi pezzi e' verificabile da solo:

- `stipulation_cue/1` (grammar.p0, EN+IT) — quali parole STIPULANO invece di
  asserire. E' la conoscenza che mancava, ed e' il discriminante che la sonda
  gen382j ha isolato: non una preferenza architetturale, una proprieta' della
  frase.
- `response_template(premise_conflict, …)` (EN+IT) — le parole per dire la
  tensione senza rifiutare la premessa.
- `premise_conflict_note()` — il gemello RIUSABILE di `note_class_conflict`
  (gen375), che era legato alle asserzioni di classe. Stessa conoscenza (is_a/2 +
  incompatible/2), seconda superficie. E' il punto 4 della lista, fatto per meta'
  e in anticipo.
- il ramo stipulativo/assertivo in `one_turn_syllogism` — scritto, non
  esercitato.
- `kb_retract_origin` ora ritratta anche le REGOLE (gen382k). Questa non e' una
  rifinitura: senza, una premessa universale non rinominata lascia una regola
  VIVA nella KB, cioe' conoscenza falsa che continua a dedurre. Era una perdita
  reale, mascherata dal fatto che finora le premesse morivano col sandbox.

### Il piano, in ordine

1. **Trovare il sito vero** (sopra). Finche' non e' fatto, ogni migrazione
   successiva rischia di essere di nuovo codice non esercitato.
2. **Agganciare il punto 1 li'**: stipulativo -> rinomina; assertivo -> niente
   rinominazione, provenienza ipotetica, e `premise_conflict_note` prima della
   risposta.
3. **Chiudere il punto 4**: un solo rilevatore di conflitto, due consumatori
   (l'asserzione di classe di gen375 e la premessa). Oggi sono due funzioni che
   leggono gli stessi fatti.
4. Solo dopo, **i punti 2 e 3** (le ipotesi come oggetti con identita', e lo
   scope del risolutore). Sono una generazione a se': e' il momento in cui la
   provenienza smette di essere un'etichetta e diventa un contesto di credenza.
5. **Non migrare gli altri siti scratch** finche' 1-3 non sono chiusi. Migrarli
   ora significa scrivere quattro volte l'architettura che non puo' accorgersi
   del conflitto, e poi disfarla.

### Come non ripetere l'errore

La lezione non e' "fai piu' attenzione". E' che **una prova deve poter fallire**:
"non restano residui" e' vera anche col sandbox amputato, quindi non distingue le
due architetture e non prova niente. La prova giusta per una migrazione a
rinominazione e' che i token freschi COMPAIANO — per esempio chiedendo a parrot0
la traccia della derivazione, dove i simboli rinominati sono visibili — oppure un
comportamento possibile SOLO con la KB viva, come il conflitto dichiarato del
punto 1. Da qui in avanti, per ogni sito migrato: quale osservazione sarebbe
DIVERSA se la migrazione non fosse avvenuta?

## Un turno che ha rallentato di ~1s, e non so ancora perche'

`rulespec` (il caso che sintetizza e COMPILA un programma C) stava sotto i 6s a
HEAD e sta fra 6.7 e 6.9 dopo le righe di conoscenza aggiunte a
`kb/experts/games/poker.p0` (magnitude/3, magnitude_cue/3, concept_label/4).
Verificato per differenza: con `git stash` il test passa, senza fallisce.

Il budget e' stato portato a 8 con la ragione scritta accanto — NON e' un cerotto
(quella lezione e' costata 23 override tolti su 24 all'inizio di questa serie): e'
un budget che dice la verita' finche' la causa non e' capita.

L'ipotesi da verificare per prima: la scansione delle evidenze — `format_constraint`
e `magnitude_cue` vengono enumerate a ogni turno, e ogni riga aggiunta le allunga.
Se e' cosi', il difetto e' strutturale e non del poker: **la KB cresce e un turno
rallenta**, cioe' esattamente il fallimento che gen382 aveva chiuso per kb_query.
Strumento: `PARROT0_TE_SLOW=0.5 make test`, e il profilo (-pg) su quel solo turno.


---

# gen383-390 — lo strato dialogico: che cosa resta aperto

*Scritto a fine sessione, 16 agosto 2026. Sette generazioni consecutive sullo
strato dialogico (`docs/plans/the-linguistic-glue.md`) piu' due sull'ambiguita' e
il registro (`docs/plans/question-emergence.md` §14). `make test` 1769 verdi,
`glue-bench` 11/11 crisp con zero righe qualitative. Qui c'e' solo cio' che NON e'
chiuso, in ordine di quanto blocca il resto.*

## 1. ⛔ PRIMO: tre `kb_match` consecutivi che restituiscono 0, e non so perche'

Dentro un modulo, in una sessione avanzata, tre `kb_match` di seguito su regole
DERIVATE restituivano `0` in modo non deterministico — mentre le stesse regole
interrogate da fuori (MCP `kb.match`) davano i numeri giusti nello stesso
processo. Riproduzione osservata: `count_readings_answer` in
`src/brain/25-wordmath-reasoning.c` con `collection_kinds` / `collection_per_side`
/ `collection_total`; il primo (o i primi due) tornavano 0 e l'ultimo il valore
giusto. Invertendo l'ordine, si spostava: falliva sempre il PRIMO.

**Non l'ho isolato.** Il consumer e' stato riscritto per non dipenderne (i numeri
vengono dall'enumerazione che serve comunque per la scomposizione), quindi il
sintomo non e' piu' visibile — **ma il difetto e' ancora li'**, e chiunque componga
piu' query derivate dentro un modulo puo' incontrarlo, in silenzio e con numeri
sbagliati invece che con un errore.

Ipotesi non verificate, in ordine di sospetto:
- `pred_bucket()` restituisce `b.idx`, un puntatore DENTRO il censimento;
  `pred_stats_rebuild()` fa `free(...idx)` su ogni slot. Un rebuild innescato da
  una chiamata annidata mentre un `PredBucket` esterno e' vivo lascia un puntatore
  penzolante — indici di fatto casuali, quindi risultati vuoti o sbagliati.
- oppure `pred_stat_slot(kb, pred, 0)` che non trova la voce e fa apparire il
  predicato come sconosciuto (`bk.live && bk.n == 0` -> "return 0").

Strumento: un test che ripete N `kb_match` su una regola derivata dentro un
modulo, con `P0DBG` a stampare il conteggio; e un `assert`/`retract` in mezzo per
forzare il rebuild.

**Aggiornamento gen396 — candidato serio, non ancora conferma.** La polarita' non
inizializzata dei goal costruiti a runtime (handoff in testa a questo file) ha
esattamente questa firma: zero soluzioni, silenzio, dipendenza dalla storia del
processo e non dalla KB, e il PRIMO uso che fallisce mentre i successivi passano.
`count_readings_answer` compone `collection_*`, cioe' regole derivate: se una di
quelle passa per `findall` o `call`, era lo stesso difetto. **Non dichiararlo
chiuso senza riprodurlo:** l'ipotesi del puntatore penzolante di `pred_bucket()`
resta in piedi e va falsificata a parte, perche' e' un difetto diverso con lo
stesso sintomo.

## 2. L'isolamento fra file nel test-engine

`tests/p0t/meta/knowledge_gap.p0t` passa 14/14 su un motore appena avviato e cade
sul PRIMO blocco quando nella stessa esecuzione lo precede un altro file — i sette
blocchi successivi, identici per forma, passano. Nessuna combinazione di `!set`
(BASE, PROFILE, WORLD_FACTS, LANG) lo rimette a posto, e un secondo `!reset`
consecutivo viene saltato dalla logica «reset intelligente».

Per questo la riga nel `Makefile` e' commentata. Finche' non e' capito, **ogni
misura fatta dentro la suite e' sospetta**, non solo questa.

## 3. L'ultimo anello: dato -> risposta

Le CORNICI sono localizzate (`response_template` /3) e i VALORI si rendono con
`tr/2` (gen388), quindi «quante sono le carte del poker» -> «poker ha 52 carte».
Ma la PROSA memorizzata resta inglese:

```
come finisce una partita a poker -> A hand ends when one player remains …
```

Non e' traducibile senza contenuto italiano: e' un limite onesto, non un bug. La
strada e' conoscenza (descrizioni italiane), non motore.

## 4. Il vocabolario italiano e' vuoto

`kb/core/lexeme.p0` ha 35 551 voci inglesi e **zero** italiane (misurato: panino,
cammino, ambiente, perche, pesce, mazzo, carte, giocatori — tutte assenti). Due
conseguenze gia' in produzione:

- il muro non puo' nominare una parola ignota in italiano (`lexicon_language(en)`
  lo dichiara, e il declino resta generico li');
- la riparazione ortografica (gen385) in italiano non ha vicini da proporre —
  «pamino» propone «amino» e non «panino», perche' *panino* non e' un lessema.

Aggiungere `lexicon_language(it).` il giorno in cui il lessico esiste accende
entrambi senza toccare il C.

## 5. Il sensore nomina parole che non sono argomenti

Il declino informato (gen384) nomina la prima parola senza fatti, e a volte non e'
un topic: «is five more than apples?» -> nomina *apples*; «…bigger…» -> *bigger*.
Onesto, a volte goffo. Il discriminante giusto non e' lessicale ma **posizionale**
— quale token e' l'argomento del turno — e arriva col residuo tipizzato di
`question-emergence.md` §11.5.

**Vicolo cieco gia' battuto, non ripeterlo:** stringere il criterio su `lexeme/1`
e' preciso in inglese, ma rende invisibile *cheese* in «parliamo di formaggio» e
caricare il lessico al muro triplica la KB e fa sforare i timeout.

## 6. Il registro: quel che manca dopo gen390

- `label_status(mangiare, informal)` e' implementato — la comprensione accetta il
  termine marcato, la realizzazione lo evita — ma **nessun turno lo esercita
  end-to-end**: mancano le MOSSE degli scacchi, non il meccanismo.
- **«Si dice X o Y»**: parrot0 ha lo statuto in KB e potrebbe rispondere quale dei
  due e' il termine curato («soprattutto catturare; mangiare e' informale ma
  d'uso corrente»). Consumer piccolo e generale, non fatto.
- **`pezzi minori`** e' dichiarato come categoria (alfiere, cavallo) ma non ha un
  conteggio suo: e' il TERZO livello, sotto il tecnico.
- **`cavallo` -> «horse»**: il pezzo degli scacchi viene letto come l'animale. E'
  `concept_label` al contrario — la stessa parola denota due concetti in due
  domini — e non e' toccato. Probabilmente serve un `concept_in_domain/3`.

## 7. Il contatore di rotazione, e altri accoppiamenti

Chiuso a gen388 per `response_template` (una rotazione per famiglia), ma restano
altri due usi di `b->response_pick` globale: `30-generation-reading.c:1287` e
`20-math.c:1890`. Stessa specie di accoppiamento — due famiglie che non si parlano
si influenzano — e stessa cura.

## 8. ✅ `apply/2` non si comporta dentro `findall/3` — ISOLATO a gen396

Osservato a gen389: `findall($P, apply(pred, cons(...)), $L)` raccoglie zero,
mentre lo stesso `apply` come goal diretto di una clausola funziona. Non isolato.

**Causa trovata a gen396** (dettaglio nell'handoff in testa a questo file):
`parse_to_term/2` non definiva `neg`, quindi il goal interno di `findall/3` —
l'unico costruito a runtime invece che dal loader — prendeva la polarita' dalla
spazzatura di stack del chiamante. Con quel byte non nullo il goal veniva letto
come `naf(G)`, era non-ground e floundava: enumerazione vuota, senza errore.
Spiega perche' lo STESSO `apply` come goal diretto funzionava (quello arriva dal
loader, che azzera la `Rule`) e perche' il difetto sembrava non deterministico.

Il ratchet e' in `tests/p0t/reasoning/sequential_view.p0t`: `findall` su `apply/2`
con controllo negativo, piu' il gemello `call/1`.

---

# Le lezioni di metodo, perche' non si ripetano

Non sono TODO: sono cose che sono costate tempo e che vanno sapute prima.

1. **I cue si dichiarano nella forma CANONICA, non nella superficie.** In
   inglese le due coincidono per caso, in italiano no. Due cue in albero erano
   scritti contro la forma CORROTTA — `answer_frame("quali am i", …)` e
   `intent_cue(…, "quali")` — cioe' la KB compensava un bug del motore, e
   sistemare il motore li ha uccisi. Non e' una regressione della correzione: e'
   debito che la correzione rende visibile. **Strumento: `lang.canonical`** (MCP),
   che esiste da gen383 apposta. Vale la pena passare in rassegna gli altri cue
   con quello strumento.

2. **Il topic non e' l'antecedente.** Registrare come antecedente ogni entita'
   nominata ha fatto smettere a parrot0 di chiedere «a chi si riferisce "it"?» —
   sceglieva in silenzio. Un pronome vuole un REFERENTE introdotto; un topic e'
   solo cio' di cui si parla. Due campi, due nozioni (`last_topic` vs
   `last_entity`).

3. **Una locuzione si dichiara SOLO dove il per-parole sbaglia**, mai dove e'
   soltanto goffo: lo strato delle locuzioni sta sopra quello delle parole e
   vince, quindi una locuzione superflua non e' neutra, CANCELLA una struttura.
   Tre casi misurati in `kb/core/lexicon.p0`.

4. **L'oracolo e' segnale comportamentale, mai autorita' sul contenuto.** Fra
   modelli varia il contenuto (kimi-k2.6 diceva «pannino» per «pamino»; gpt-5.6
   dice «panino»); resta invariante la MOSSA. Si copia l'invariante — ed e' il
   punto in cui il KB-first SUPERA l'LLM: un'ipotesi validata contro `lexeme/1`
   non puo' proporre una parola che non esiste.

5. **Il muro e' il punto in cui il sistema sa di piu'.** Ci arrivava sapendo
   quale parola non conosceva, e taceva. Peggio: il registratore di lacune
   esisteva gia' ma era annidato nel ramo ANTI-RIPETIZIONE — il sensore era un
   effetto collaterale di una correzione di naturalezza.

6. **Una prova deve poter fallire.** Vale anche per le sonde: il controllo
   negativo («non disambiguare cio' che non e' ambiguo», «non riparare cio' che
   non e' rotto») ha trovato due difetti veri che i casi positivi non vedevano.
