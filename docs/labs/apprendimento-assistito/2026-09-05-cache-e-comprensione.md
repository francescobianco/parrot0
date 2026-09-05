# La cache che nascondeva una lezione

Stato: `meta-capability-only`, `W=0`. Partenza `ba45d82`, checkpoint gia'
pubblicato; worktree pulito salvo la strumentazione aggiunta per questa diagnosi.
Profilo sempre `PARROT0_PROFILE=kb/profiles/agi.p0`, KB completa:
**38627 fatti, 2793 regole** al boot. Nessun file KB modificato, nessun `/save`.

## Causa dimostrata, non dedotta dal nome del predicato

La segnalazione in testa a `LEARN_TODO.md` contrapponeva due clausole identiche:

```prolog
probe_two($Pat,$Pred) :- relation($N), concat_atoms($N,"_of",$Pred),
                       concat_atoms("the ",$N,$A),
                       concat_atoms($A," of @S is @O",$Pat).
extract_frame($Pat,$Pred) :- relation($N), concat_atoms($N,"_of",$Pred),
                           concat_atoms("the ",$N,$A),
                           concat_atoms($A," of @S is @O",$Pat).
```

Per riprodurre senza modificare i file, entrambe sono state aggiunte a runtime
con `kb.assert_clause` via MCP. Si scalda poi la vista **prima** di asserire
`relation(zzz)`. La sonda e' diagnostica, non una lezione in lingua naturale.

| Operazione, nello stesso processo | Prima del fix | Dopo |
|---|---|---|
| `extract_frame(?,zzz_of)`, prima della fonte | nessun binding | nessun binding |
| assert `relation(zzz)` | memorizzato | memorizzato |
| `probe_two(?,zzz_of)` | `the zzz of @S is @O` | stesso pattern |
| `extract_frame(?,zzz_of)` | **nessun binding** | **stesso pattern** |
| assert `relation_verb(weaves)`, poi rilettura | il pattern compare | gia' presente |

La traccia interna temporanea in `kb_view_ensure`, rimossa dal diff finale,
ha mostrato questo passaggio prima del fix:

```text
prima della lezione:             signature=16093100353029513759
dopo relation(zzz):              signature=16093100353029513759
dopo relation_verb(weaves):       signature=6059209771833989446
```

`extract_frame` e' dichiarata `materialized_view/2`. Il solver saltava le
regole perche' la vista risultava viva. La sua firma consultava solo un piccolo
elenco manuale di `view_depends`, non tutte le premesse delle regole. Non era
un problema di variabili, posizione delle clausole, secondo argomento legato
o bucket pieno. Nemmeno aggiungere solo `view_depends(extract_frame,relation)`
avrebbe riparato la classe del difetto: domani un'altra fonte sarebbe rimasta
invisibile. Inoltre i conteggi non distinguono una sostituzione a pari dimensione.

## Correzione del contratto della vista

- Chiusura transitiva delle dipendenze strutturali delle clausole, inclusi
  predicati senza fatti e goal negati. Gli archi `view_depends` possono
  integrare il grafo; non sono piu' l'unica autorita'.
- Le mutazioni di fatti e regole invalidano le viste interessate. La lettura
  resta economica: una modifica estranea al grafo non spegne la cache.
- Una riga scaduta e' invisibile anche durante una risoluzione annidata.
  La rimozione fisica aspetta un ingresso sicuro, fuori da entrambi i solver;
  prima di derivare si eliminano anche le viste sorelle scadute.
- Il salto delle regole riguarda il goal corrente e l'arieta' materializzata.
  Una continuazione non puo' sovrascrivere quella decisione per il chiamante.
- Dipendenze e registro delle viste crescono dinamicamente. L'incompletezza
  dell'enumerazione viene propagata da `kb_match_all`; una vista parziale non
  viene mai marcata viva. In caso di fallimento si usa l'inferenza ordinaria.
- I contatori registrano gli inserimenti reali: `kb_assert` ritorna successo
  anche se un fatto esiste gia', quindi successo non significa nuova riga.
  Il riscaldamento sul boot attuale inserisce **zero** righe: i risultati sono
  gia' presenti, ma la vecchia cache si attribuiva 359 inserimenti per
  `extract_frame`. La revisione non deve cambiare per un riscaldamento.
- `KB_DERIVED` aveva lo stesso bit di `KB_INDUCED`. Ora possiede un bit distinto:
  i candidati induttivi non diventano righe di cache e una pulizia della cache
  non li possiede. Un fatto cache insegnato esplicitamente viene promosso alla
  provenienza della lezione e sopravvive al ritiro della sua vecchia premessa.

Nessuna parola di lingua naturale, forma di relazione o risposta aggiunta nel C.
I nomi confrontati dal nuovo codice sono identificatori della meccanica KB.

## Sonde interne sulla KB completa

Sonda locale in-process: `brain_create` + `brain_boot`, stessi oggetti del
programma e nessuna amputazione del mondo. Si e' ispezionato lo stato delle
viste per escludere che il risultato corretto provenisse dal semplice
spegnimento della cache:

```text
FULL KB facts=38627 rules=2793 views=2
VIEW extract_frame live=1 dependencies=27 broad=0
VIEW verb_stem live=1 dependencies=6 broad=0
DERIVED 0
```

Esiti verificati nella stessa sessione:

- nuova fonte runtime, retract e sostituzione `warp`/`weft` senza lettura
  intermedia, usando la regola `relation_noun` gia' esistente;
- vista nuova senza `view_depends`, fonte transitiva e rimpiazzo della regola
  intermediaria; domanda attraverso un wrapper dopo il ritiro della fonte;
- dipendenza negata, ritiro per predicato, per pattern e per provenienza
  ipotetica; mutazione durante il solver seguita dalla lettura della vista;
- insegnamento esplicito di un fatto gia' derivato, rimozione a runtime della
  dichiarazione di vista e assenza delle vecchie righe cache;
- traffico estraneo che lascia la cache viva; regole binarie non oscurate da
  una vista unaria omonima; registro cresciuto oltre sedici viste;
- meta-chiamata con fallback ordinario, proposta indotta ancora visibile,
  enumerazione interrotta non promossa a vista e contatori finali esatti.

La sonda e' stata eseguita anche con AddressSanitizer e UndefinedBehaviorSanitizer
sul codice KB, collegato agli altri oggetti ordinari: conclusa senza diagnostiche.
Il primo tentativo nel sandbox era fermato da LeakSanitizer sotto ptrace;
rieseguito fuori sandbox dopo approvazione. Non e' una verifica sanitizzata
dell'intero programma. Nessuna suite del progetto eseguita.

## Verifica parlata, distinto tipo di evidenza

Sessione diagnostica, non conoscenza del mondo da consolidare:

```text
> alice weaves linen
Non capisco ancora.
> the word weaves is a relation verb
Learned: weaves is a relation_verb.
> alice weaves linen
Learned: alice weaves linen.
> what does alice weaves?
Linen.
> forget the word weaves is a relation verb
I no longer treat «weaves» as a relation verb.
> bob weaves cotton
[muro: propone di insegnare il verbo]
> the word weaves is a relation verb
Learned: weaves is a relation_verb.
> bob weaves cotton
Learned: bob weaves cotton.
> what does bob weaves?
Cotton.
```

Non ripulire la grammatica del transcript: la domanda naturale «what does alice
weave?» ha invece dato `I don't understand that yet.` Questo e' un limite del
lato domanda/morfologia, non un successo da attribuire alla cache. Anche la
prima risposta italiana e' un comportamento osservato, non un'aggiunta qui.

Conoscenza vera gia' consolidata, riletta con lo stesso binario:

```text
> why is azurite a copper mineral?
azurite is a copper mineral because azurite is a copper carbonate mineral.
> what are the copper minerals?
chalcopyrite, chalcocite, covellite, bornite, azurite, malachite.
```

## Confine del risultato e ripresa

Il difetto di invalidazione e' chiuso. Non e' ancora chiuso il ciclo naturale
«insegnare il nome di una relazione → affermare → interrogare → spiegare →
correggere → salvare → richiamare in processo fresco». Una `relation_noun`
asserita da una API prova la meccanica, non la comprensione della lezione.

Le viste con meta-chiamate, riflessione o effetti non analizzabili staticamente
non vengono materializzate: restano sull'inferenza ordinaria. L'ottimizzazione
di quelle dipendenze e delle arita' oltre due e' lavoro successivo.
`kb_revision` resta un indicatore storico basato sulla dimensione: gli altri
consumer che lo scambiano per un contatore di mutazioni vanno censiti.

La nuova coda in testa a `LEARN_TODO.md` parte dal ciclo parlato delle relazioni
e prosegue con binding condiviso, arbitrato, lingue, ambiti/provenienza,
revisione, domande utili e connessioni fra conoscenze vere. Non dichiara raggiunta
la comprensione universale: rende prendibili gli incrementi che la avvicinano.

Clausole KB persistite `W/L/C/P/O/X = 0/0/0/0/0/0`; nessuna nuova conoscenza
del mondo promossa. Le entita' della sonda sono diagnostica effimera. Build
riuscita; nessun `/save`, nessun corpus di prova scritto nella KB ufficiale.
