# L'emersione delle domande

> **La domanda fondamentale.** Esiste un meccanismo — KB-based, e KB-first anche
> nel modo in cui è costruito — che faccia **emergere dalla KB stessa le domande
> a cui parrot0 non saprebbe rispondere ma a cui dovrebbe saper rispondere**?
>
> Documento aperto a gen382m. Serve a far ragionare in parallelo più agenti sullo
> stesso problema: qui c'è il contesto completo, il perché la strada ovvia non
> funziona, e i vincoli che una proposta deve rispettare per essere accettabile in
> questo progetto.

---

## 0. Il contesto minimo, per chi arriva adesso

parrot0 è un agente conversazionale in C puro con una base di conoscenza in un
dialetto Prolog (`kb/**/*.p0`). La regola fondativa del progetto non è "scrivi
codice che funziona", è **"non scrivere codice se la conoscenza può farlo"**
(`MANTRA.md`): il motore in C deve restare un adattatore fisso, e tutto ciò che
può crescere — vocabolario, classi, procedure, formati, risposte — vive nella KB
come fatti e clausole. Il test operativo di ogni proposta è:

> *parrot0 può impararne un nuovo membro domani, parlando, senza ricompilare?*

Chi propone una soluzione a questo documento deve passare quel test. Una
soluzione che elenca in C le domande da porre è, in questo progetto, una
non-soluzione — anche se funziona.

---

## 1. Il fatto che ha generato la domanda

Una conversazione reale, quattro turni:

```
you> come tichiami
    Non capisco ancora.
you> quante carte ci sono nel poker
    Non ho afferrato bene. Cosa vorresti sapere?
you> parlami del poker
    So già qualcosa su poker: a family of card games where players wager on
    hands whose strength may be shown or represented by betting.
you> e piu forte il full o il poker come punteggio
    Non capisco ancora.
```

Il terzo turno dimostra che parrot0 **ha** un esperto di poker: il file
`kb/experts/games/poker.p0` contiene la descrizione, le regole, i consigli, i
ranghi delle mani e persino una clausola `poker_hand_beats/2`. Eppure due domande
elementari su quel dominio vanno a muro.

## 2. Perché la strada ovvia non trova questi buchi

La reazione naturale è misurare: `tests/expertbench.py` (scritto in questa
sessione) legge dalla KB che cosa parrot0 **dichiara** di sapere — `expert/1`,
`category_surface/2`, `game_play/2`, `means/2` … — e per ogni dominio pone le
forme di domanda corrispondenti. È un buon strumento e ha già trovato difetti di
raggiungibilità.

**Ma sui domini di gioco dà 0% di muri.** Non perché parrot0 sia bravo: perché
quel bench può interrogare **solo lo spazio dichiarato**. Le domande che avevano
deluso — *quante carte ha il poker*, *è più forte il full o il poker* — non sono
irraggiungibili: sono **non dichiarate**. Il conteggio delle carte non esiste in
nessun fatto; il rango delle mani esisteva solo in un dialetto privato
(`poker_hand_rank/2`) che il motore generale di confronto non legge.

> Un bench derivato da ciò che la KB afferma **non può, per costruzione, trovare
> ciò che la KB tace.** È il complemento — lo spazio negativo — che serve.

## 3. Che cosa si chiede, detto con precisione

Non "elenca le domande possibili": sono infinite. Si chiede di far **emergere**,
dalla struttura della KB stessa, le domande che soddisfano *entrambe* queste
condizioni:

1. **parrot0 non saprebbe rispondere** — nessun fatto, nessuna regola, nessun
   frame le copre;
2. **dovrebbe saper rispondere** — c'è qualcosa nella KB che le rende *legittime
   e attese*: un fratello che quella cosa la sa, una forma di domanda dichiarata
   e mai soddisfatta, un'entità nominata e mai descritta.

La seconda condizione è tutto il problema. Senza di essa si ottiene rumore
infinito ("qual è il colore preferito del poker?"). Con essa si ottiene un
**debito di conoscenza misurabile**, e — questa è l'intuizione da cui il documento
nasce — *la stessa struttura che fa emergere la domanda dice anche come colmarla*.

## 4. Cinque sorgenti di spazio negativo, tutte calcolabili dalla KB

Ipotesi di lavoro, da valutare, estendere o smontare.

### 4a. Asimmetria fra fratelli

Se quindici esperti di gioco dichiarano `game_players/2` e il poker no, allora
*"quanti giocatori servono a poker"* è una domanda che parrot0 dovrebbe saper
reggere e non regge. La KB dice da sé che cosa sia un esperto di gioco
"completo": lo dice **per maggioranza**, non per definizione — nessuno deve
scrivere uno schema.

Generalizzato: per ogni insieme di entità che condividono un tipo
(`expert_domain(X, games)`, `is_a(X, mammal)`, `category_member(C, X)`), l'unione
dei predicati usati dai membri definisce il profilo atteso; ogni membro a cui ne
manca uno è un buco tipizzato.

### 4b. Frame dichiarati senza dati

`answer_frame(Cue, Pred)` dichiara una forma di domanda. Il prodotto
*frame × entità note* è l'insieme delle domande **poste in modo comprensibile**;
sottratte quelle per cui `Pred` ha fatti, restano le domande che parrot0 capisce
e non sa.

Questa sorgente ha una proprietà preziosa: il buco è già espresso nella forma
esatta del fatto che lo colmerebbe.

### 4c. Entità opache

Un termine che compare **solo come argomento** e mai come soggetto di un fatto:
`straight_flush` è membro di `poker_hand`, ma non ha `means/2`, non ha
descrizione, non ha magnitudine. parrot0 sa nominarlo e non sa dirne niente. Ogni
entità opaca è una domanda che qualunque interlocutore porrebbe.

### 4d. Regole morte e dialetti privati

Due varianti dello stesso difetto:

- una clausola il cui corpo non può essere soddisfatto (predicato del corpo senza
  fatti) — deduce zero, e nessuno se ne accorge;
- una relazione espressa in un **dialetto privato** invece che nella relazione
  generale che il motore legge. `poker_hand_rank(full_house, 7)` diceva
  esattamente ciò che `magnitude(hand_rank, full_house, 7)` dice, ma sotto un nome
  che solo quel file capiva: il motore generale di confronto — quello che risponde
  a *"quale fiume è più lungo"* — non poteva vederlo. **Non mancava una capacità:
  mancava che il dato fosse detto nella lingua comune** (mantra #3, *astrai fino
  al punto fisso*).

Questa quarta sorgente è la più insidiosa perché la conoscenza **c'è**, e nessuna
misura di copertura la segnala come mancante.

### 4e. La varianza della SUPERFICIE: resilienza linguistica

La prima riga della conversazione di §1 non e' un buco di conoscenza:

```
you> come tichiami
    Non capisco ancora.
```

"come ti chiami" scritto con uno spazio in meno. Un LLM lo capisce senza
accorgersene; parrot0 va a muro. E questa varianza **deve emergere dallo stesso
meccanismo**, perche' e' la stessa specie di debito: c'e' una domanda a cui
parrot0 *dovrebbe* saper rispondere — sa gia' rispondere alla sua forma pulita —
e non risponde.

E' la sorgente piu' feconda delle cinque, perche' e' calcolabile in modo
completamente meccanico e non richiede conoscenza nuova: **si prende una domanda
che oggi FUNZIONA e la si deforma.** Ogni deformazione che rompe la risposta e'
un buco di resilienza, e lo spazio delle deformazioni e' enumerabile:

- **segmentazione**: spazio mancante ("tichiami"), spazio di troppo ("po ker"),
  parole unite dalla fretta;
- **ortografia**: lettera scambiata, doppia mancata, accento assente ("piu",
  "perche", "e" per "è") — l'italiano scritto in fretta perde gli accenti sempre;
- **ordine**: "il full o il poker, quale e' piu' forte";
- **cortesia e riempitivi**: "scusa, sapresti dirmi …", "senti, ma …";
- **abbreviazioni e registro**: "qnt", "cmq", minuscole ovunque, nessuna
  punteggiatura;
- **codice misto**: una parola inglese dentro una frase italiana.

Il metro e' immediato e non richiede oracolo: **la risposta alla forma deformata
deve essere la stessa della forma pulita.** Non serve giudicare la verita' — si
confronta con sé stessi.

Due note che cambiano il modo di affrontarla, e sono il motivo per cui questa
sorgente sta in questo documento e non in un TODO qualunque:

1. **Il rimedio non e' un correttore ortografico in C.** Sarebbe vocabolario nel
   motore, e per giunta monolingue. La strada KB-first e' che le CLASSI DI
   DEFORMAZIONE siano conoscenza (`surface_variation(missing_space, …)`,
   `surface_variation(missing_accent, …)`) e che il motore le applichi come
   ipotesi di riparazione — parrot0 ha gia' `mod_robust` e una nozione di
   riparazione, ma non e' guidata da nulla di dichiarato.

2. **La riparazione ha bisogno del lessico che gia' esiste.** "tichiami" si
   risolve senza sapere nulla di ortografia: si prova a spezzarlo in due token
   che siano *entrambi parole note* (`ti` + `chiami`), e la KB il lessico ce
   l'ha. E' lo stesso principio con cui `np_closer/1` ha sbloccato l'estrazione
   dalla prosa — non una lista nuova, una lettura nuova di cio' che c'e'.

Questa sorgente, a differenza delle altre quattro, produce buchi **misurabili
oggi**: basta prendere le domande che il sistema gia' supera e deformarle. Un
prototipo di emersione che comincia da qui ha il vantaggio di poter mostrare
subito una lista vera, e di non dover attendere la parte piu' difficile
(l'asimmetria fra fratelli, §4a).

## 5. Il vincolo che rende il problema interessante

Il meccanismo di emersione deve essere **esso stesso KB-first**. Cioè:

- le sorgenti di §4 non devono essere cinque funzioni C, ma **regole nella KB**
  sopra una rappresentazione della KB stessa (parrot0 possiede già un modello di
  sé: `machinery/1`, `file_attribute/1`, `expert/1`, `capability/2`);
- il **tipo** di un buco e il suo **rimedio** devono essere fatti, non `switch`:

  ```prolog
  gap(missing_sibling_attribute, poker, game_players).
  gap(opaque_entity, straight_flush).
  gap(private_dialect, poker_hand_rank, magnitude).
  gap(surface_fragility, "come ti chiami", missing_space).

  gap_remedy(opaque_entity, dream).          % vai a leggere la pagina
  gap_remedy(missing_sibling_attribute, ask_user).
  gap_remedy(private_dialect, restate).      % ridillo nella relazione generale
  gap_remedy(surface_fragility, repair_hypothesis).
  ```

- un rimedio nuovo, o una sorgente nuova, deve costare **un fatto**.

## 6. Perché questo chiude un cerchio già aperto

parrot0 ha `--dream`: un'esplorazione ricorsiva che parte da un topic, ne legge la
prosa (corpus statico o Wikipedia) ed estrae fatti e regole, scendendo parola per
parola. Oggi il sogno esplora **ciò che incontra**. È già emerso, discutendone,
che servirebbe una guida in stile alfa-beta — una funzione di valutazione che
decida quali rami valga la pena approfondire.

**I buchi sono quella funzione di valutazione.** Un nodo vale la pena se colma un
buco dichiarato. Il sogno smetterebbe di esplorare a caso e comincerebbe a
*cercare ciò che gli manca* — e siccome ciò che impara viene instradato nell'albero
curato e committato, il ciclo si chiude: **emersione → sogno guidato → conoscenza
persistita → nuovi buchi di livello più fine**.

## 7. Che cosa renderebbe accettabile una proposta

- **Deriva le domande, non le elenca.** Nessuna lista di domande in C né in KB:
  devono nascere dalla struttura.
- **Nessun falso allarme.** Il primo prototipo di `expertbench.py` chiedeva "come
  si gioca a algebra" e dava 100% di muri: misurava sé stesso. Una proposta deve
  spiegare *perché* le domande che genera sono legittime.
- **Il buco è tipizzato e il rimedio è dichiarato**, così l'output è azionabile e
  non un elenco di lamentele.
- **Il meccanismo cresce come conoscenza**: una sorgente nuova = fatti.
- **Una prova che può fallire.** Questo progetto ha appena pagato caro il suo
  contrario (`KB_TODO.md`, sezione sul punto 1): una dimostrazione compatibile
  anche con la NON avvenuta implementazione non dimostra niente. Per ogni
  proposta: *quale osservazione sarebbe diversa se il meccanismo non ci fosse?*

## 8. La domanda di review

> Dato uno stato qualunque della KB, parrot0 sa **elencare da sé** le domande che
> un interlocutore ragionevole gli porrebbe e a cui non saprebbe rispondere — e
> sa dire, per ciascuna, **da dove prenderebbe la risposta**?
>
> E sa dire quali domande **sa già reggere solo se scritte bene**?

Oggi: no. Non ha nemmeno il vocabolario per nominare un proprio buco.

---

*File aperto. Le proposte possono essere aggiunte in coda come sezioni datate,
oppure discusse in `KB_TODO.md`. Chi propone: leggere prima `MANTRA.md` — non è
cerimoniale, è il criterio con cui la proposta verrà giudicata.*
