# L'iniziativa

*parrot0 non deve solo **rispondere**: deve **aprire il turno successivo**. Questo
piano sostiene che l'iniziativa non è un modulo fra i moduli, ma una **facoltà
generale** di cui molte cose già esistenti — lo smalltalk per primo — sono
**effetti particolari**, oggi implementati separatamente e senza saperlo.*

> **Stato (2026-08-11):** censimento fatto, tesi formulata, nessuna riga di
> motore ancora scritta. Il documento serve a nominare la facoltà prima di
> costruirla, perché il costo di non averla nominata è già visibile (§5).

---

## 0. La tesi (F.)

> *Lo smalltalk è di fatto un effetto dell'iniziativa presa in termini sociali.*

È l'osservazione che genera questo piano, ed è verificabile guardando cosa fa
davvero `mod_smalltalk`. Non è un modulo che "gestisce le chiacchiere": è un
modulo che, **non potendo rispondere**, prende la conduzione della conversazione
e la restituisce all'interlocutore con una domanda aperta. Ogni suo template
finisce così:

```
smalltalk_deflect:   "...I'm parrot0, an AI -- but I'd love to hear about yours."
smalltalk_continue:  "Interesting -- what got you into it?"
                     "Nice -- how did that go?"
```

La parte informativa di quelle risposte è vuota, o è una negazione. **Il
contenuto è l'iniziativa.** Togli la domanda finale e non resta nulla.

Una volta vista in un posto, la si vede ovunque. Il resto del piano parte da lì.

## 1. Censimento — dove parrot0 prende già iniziativa

Nove siti indipendenti, nessuno dei quali si chiama iniziativa tranne il primo.
Tutti verificati nel codice o in esecuzione.

| # | Mossa | Dove | Innesco |
|---|---|---|---|
| 1 | **Semina un fatto e rilancia** | `70-social-pragma.c:357`, `kb/core/initiative.p0` | eco esatta di self-play |
| 2 | **Devia e ridà il turno** | `70-social-pragma.c:402`, `responses.p0:225-252` | domanda in seconda persona non rivendicata |
| 3 | **Offre di documentarsi** | `responses.p0:351,376` | gap di conoscenza su un topic |
| 4 | **Propone come essere istruito** | `10-memory-knowledge.c:8947` | relazione ignota |
| 5 | **Chiede chiarimento** | `responses.p0:334`, `99-registry.c:740` | input non compreso |
| 6 | **Apre la conversazione** | `70-social-pragma.c:96` | saluto |
| 7 | **Propone il passo seguente** | `70-social-pragma.c:289,293` | accordo/chiusura di un compito |
| 8 | **Chiede aiuto al mondo** | `kb/experts/codebase/actions.p0:95-111` | piano bloccato (`needhelp.p0`) |
| 9 | **Genera un goal di acquisizione** | `universal-comprehension.md` §7 | precondizione `know(X)` mancante |

Il #3 merita una nota, perché è l'unico anello **completo** e va usato come
modello per tutti gli altri. L'ho verificato in esecuzione:

```
> what is a florbnax?
< I don't know much about florbnax yet. Want me to look it up?
> yes
< Looking up florbnax... I couldn't find information. I still don't know much about florbnax.
```

L'offerta viene **onorata**: l'accettazione è raccolta, l'azione parte davvero, e
l'esito — anche negativo — è riportato senza abbellirlo. Questo è il contratto
che l'iniziativa deve rispettare ovunque, ed è già dimostrato che il progetto sa
implementarlo.

## 2. L'anatomia della mossa

I nove siti, tolto il vestito, hanno tutti la stessa forma in cinque stadi:

```
1. INNESCO     uno stato riconosciuto: stallo, gap, incomprensione,
               blocco, disclosure sociale, compito concluso
2. FONDAMENTO  qualcosa di VERO da cui partire: un fatto noto, la relazione
               che manca, l'azione disponibile.  Senza fondamento non si apre.
3. PROPOSTA    la mossa offerta all'interlocutore: una domanda, un'offerta
               d'azione, una richiesta di chiarimento, un modo di istruire
4. RACCOLTA    l'accettazione (o il rifiuto) va riconosciuta al turno dopo
5. ONORARE     se accettata, l'azione si esegue e l'esito si riporta com'è
```

Oggi ogni sito reimplementa gli stadi che gli servono e salta gli altri. Solo il
#3 li ha tutti e cinque. Il #1 si ferma a 3. Il #2 si ferma a 3 e non ha nemmeno
il fondamento: propone una domanda senza avere niente da cui partire, ed è per
questo che può chiedere *"what got you into it?"* a chi ha appena detto che gli è
morto il gatto.

## 3. I tre registri

La stessa facoltà, tre materie diverse. Nominarli serve perché oggi solo il primo
è coperto, e in modo accidentale.

- **Sociale** — condurre la conversazione: rilanciare, cambiare argomento,
  restituire il turno. Siti #1, #2, #6, #7.
- **Epistemico** — condurre la *conoscenza*: accorgersi di non sapere e proporre
  come colmare. Siti #3, #4, #9.
- **Operativo** — condurre l'*azione*: proporre il passo successivo di un piano,
  chiedere aiuto quando si è bloccati. Siti #7, #8.

Il registro epistemico è quello che serve al ragionamento investigativo di
`tests/p0t/reasoning/investigation.p0t`. Là l'indagine avanza perché
**l'investigatore esterno** segue la risposta e chiede della causa trovata. Con
iniziativa epistemica sarebbe parrot0 a dire: *"la causa prossima è thaw — vuoi
che risalga a cosa ha causato thaw?"* È lo stesso arco, condotto da lui.

## 4. La diagnosi strutturale — manca lo strato post-dispatch

Questo è il punto tecnico centrale, e spiega perché l'iniziativa si è sparpagliata
in nove posti invece di esistere una volta sola.

L'iniziativa **non è una risposta: è una coda alla risposta.** Concettualmente il
turno è `rispondi(X) ; poi decidi se e come aprire il turno dopo`. Ma
`brain_reply` non ha un posto dove mettere quel "poi":

- **Sopra** il dispatch c'è una pila di quattro strati "lead" (`99-registry.c:1343-1379`,
  gen359-366): `reasoning_task_lead`, `structured_analysis_lead` stretto,
  `semantic_lead`, `structured_analysis_lead` di famiglia. Ognuno può reclamare il
  turno prima che i moduli vengano consultati.
- **Sotto** il dispatch non c'è **niente**. Dopo il ciclo `for` su `registry`
  (`99-registry.c:1568-1581`) restano solo contabilità: il trace per "why did you
  answer that way?", il buffer dei topic, il log conversazionale. **Nessuno strato
  può integrare o completare la risposta prodotta.**

L'architettura è asimmetrica. E la conseguenza è meccanica: se l'unico modo di
emettere una mossa d'iniziativa è **essere il modulo che vince il turno**, allora
ogni mossa d'iniziativa deve **divorare un turno intero**. Da qui i nove siti: non
sono nove decisioni di design, sono nove punti in cui qualcuno aveva bisogno di
aprire il turno dopo e l'unico modo era scrivere un modulo terminale.

**La proposta:** un `initiative_lead` **post-dispatch**, simmetrico alla pila
lead, che gira dopo che una risposta esiste e decide se aggiungerle una coda. Non
sostituisce nulla (§7 di `universal-comprehension.md`: cambi additivi), e i nove
siti restano dove sono finché non si dimostrano ridondanti.

## 5. Cosa spiega — i difetti osservati non sono indipendenti

Il valore di una buona astrazione si misura da quanti sintomi separati riunisce.
Questi erano stati trovati come bug distinti, prima di avere la parola
"iniziativa":

- **`mod_smalltalk` cattura troppo** (posizione 66/68, si prende "who created
  you?" e "what did I ask you first?"). Non è un gate scritto male: è il gate
  scritto *per forza* troppo largo, perché smalltalk è l'unico posto da cui esce
  una mossa d'iniziativa e quindi deve reclamare turni che non gli competono. Con
  uno strato post-dispatch non avrebbe **alcun** bisogno di reclamare quei turni:
  `mod_self` risponderebbe "chi mi ha creato", e la coda d'iniziativa si
  aggiungerebbe dopo.
- **La cecità alla valenza** ("I am feeling sad today" → *"That sounds nice --
  tell me more about it."*) è lo stadio 2 mancante: una proposta emessa **senza
  fondamento**. L'iniziativa costruita bene è obbligata a chiedersi *da cosa*
  sto aprendo, e "una disclosure negativa" è un fondamento diverso da "una
  disclosure positiva".
- **Il muro cieco sul ramo sociale** ("my cat died last week" → *"I don't
  understand that yet."*) è iniziativa **assente**: nessuno dei nove siti copre
  quell'innesco, e non c'è un default che dica "non ho capito il contenuto, ma
  posso comunque condurre".

Tre sintomi, una causa. È il segno che la facoltà è quella giusta.

## 6. Le regole d'onestà (non negoziabili)

L'iniziativa è la superficie da cui l'impostore entrerebbe più facilmente: una
domanda aperta *sembra* sempre intelligente, anche quando dietro non c'è nulla.
I PRINCIPLES vanno applicati qui in forma specifica.

1. **Nessuna iniziativa senza fondamento.** Si apre a partire da qualcosa che si
   possiede davvero — un fatto, una relazione mancante nominabile, un'azione
   disponibile. Mai una domanda aperta usata per riempire un vuoto. (È la regola
   che `conversation_seed` già rispetta: fatti veri, non curiosità simulata.)
2. **Nessuna offerta che non si sappia onorare.** Se si propone un'azione, la
   raccolta e l'esecuzione devono esistere (stadi 4-5). Il sito #3 dimostra che
   si può; proporre senza poter eseguire sarebbe un bluff.
3. **Nessuna esperienza inventata.** Vale già per `smalltalk_deflect` e va tenuta:
   si conduce **senza** fingere di aver vissuto qualcosa.
4. **L'iniziativa non maschera un fallimento.** Aggiungere una domanda aperta a
   un declino non lo trasforma in una risposta. Il declino resta visibile, la
   coda è in più — mai al posto.
5. **Si può tacere.** Un sistema che rilancia a ogni turno è invadente quanto uno
   che non rilancia mai. Non aprire deve restare una scelta legittima.

## 7. La decomposizione KB-first

Motore fisso, conoscenza nella KB — il template di `composition-is-kb-first`.

**Nella KB (fatti, si aggiungono senza toccare il C):**

```
initiative_trigger(Stato, Registro)        quali stati aprono, in che registro
initiative_ground(Registro, Fonte)         da cosa si parte per quel registro
initiative_move(Registro, Forma)           che forma ha la proposta
initiative_frame(Forma, Lang, Template)    la resa linguistica (già esiste per #1)
initiative_uptake(Forma, Azione)           cosa significa un "sì"
initiative_suppress(Contesto)              dove NON si apre (regola 5)
```

`conversation_seed/1` diventa un caso particolare di `initiative_ground`, non una
struttura a sé. I template di `smalltalk_continue`/`deflect` diventano
`initiative_move` del registro sociale. Nessuno dei due va cancellato: si
**reinterpretano** sotto la facoltà generale, e restano attivi finché la facoltà
non li copre meglio (keep-secondary-structures).

**La decisione è una PROCEDURA, non codice.** Il mantra #2 (estensione procedure)
è vincolante qui: la mossa d'iniziativa introduce una *trasformazione di classe*
— da stato del turno a proposta — e quindi va insegnata in `kb/core/procedures.p0`
sopra i primitivi, non scritta in C. La catena dei cinque stadi è una clausola:

```prolog
% da uno stato riconosciuto alla forma da emettere: tutta inferenza, zero C
initiative($Stato, $Lang, $Forma, $Ground) :-
    initiative_trigger($Stato, $Reg),
    initiative_ground($Reg, $Ground),
    initiative_move($Reg, $Forma),
    initiative_frame($Forma, $Lang, $_).
```

Scritta così, un registro nuovo o un innesco nuovo è **un fatto**, e la regola di
composizione resta una sola. Test del mantra #2: *"parrot0 può imparare domani un
nuovo tipo di iniziativa senza ricompilare?"* — con la clausola sopra, sì.

**Nel C solo l'adattatore**, ridotto al minimo indispensabile:
- il gancio post-dispatch in `brain_reply`, che chiama il goal e rende la forma;
- la macchina a stati della raccolta (stadi 4-5) come **primitiva generale**, non
  come consumer di un registro: il sito #3 la possiede già in forma
  specializzata e va estratta, non riscritta (mantra #5).

**Niente liste di parole in C** (`no-word-lists-in-c`): gli inneschi sono fatti.

### Il precedente che obbliga a questa forma

Non è cautela teorica: il debito è già nell'albero, ed è stato trovato scrivendo
`investigation.p0t`. `kb/core/procedures.p0` contiene la chiusura transitiva

```prolog
causes_t($X, $Z) :- causes($X, $Y), causes($Y, $Z).
causes_t($X, $Z) :- causes($X, $Y), causes_t($Y, $Z).
```

ed è caricata a ogni boot (`99-registry.c:524`), ma **nessun consumer C la
interroga**: il modulo causale rifà per conto proprio una camminata a due passi
cablata, e per questo `what is the cause of spoilage?` non risale alla radice.
La procedura giusta esisteva già e il C le è passato accanto.

È esattamente l'errore che l'iniziativa rischia di ripetere su scala maggiore —
nove siti che rifanno a mano ciò che una clausola direbbe una volta. Il §8.1 (il
gancio inerte) serve anche a questo: prima il posto, poi la procedura, e il C non
cresce.

## 8. Gli incrementi

Ordinati per rapporto valore/rischio, ciascuno completo e verificabile da solo.

1. **Il gancio.** `initiative_lead` post-dispatch che gira e non fa nulla:
   ritorna sempre 0. Serve solo a creare il posto dove l'iniziativa può vivere.
   Zero cambi di comportamento, quindi zero rischio di regressione.
2. **Il fondamento sociale, con valenza.** Il primo trigger reale:
   `initiative_ground` distingue disclosure positiva e negativa. Chiude il difetto
   più grave osservato ("sad" → "that sounds nice") senza toccare `mod_smalltalk`.
3. **L'iniziativa epistemica.** Dopo una risposta che ha lasciato un anello
   scoperto, proporre il passo: *"la causa prossima è thaw — risalgo?"*. È il
   registro che serve all'indagine (§3) ed è misurabile con un `.p0t`.
4. **Estrarre la raccolta.** Generalizzare gli stadi 4-5 del sito #3 in modo che
   ogni forma d'iniziativa possa essere accettata e onorata.
5. **Riassorbire.** Solo ora, e solo se i test lo dimostrano ridondante, ridurre
   il gate di `mod_smalltalk` — che a quel punto non deve più divorare turni per
   consegnare la sua coda.

Il punto 5 è **ultimo di proposito**. Finché l'iniziativa non è dimostrata, lo
smalltalk resta l'unica rete prima del muro e toglierlo sarebbe una perdita netta.

## 9. Come si misura

Un `.p0t` non basta: l'iniziativa è multi-turno per definizione, e va verificata
sull'**arco**, non sul singolo assert.

- **Test di condotta** (estensione di `investigation.p0t`): l'arco investigativo
  ripercorso con parrot0 che propone il passo successivo. Oggi lo conduce
  l'esterno; il test dirà quando lo conduce lui.
- **Test di raccolta**: proposta → `yes` → l'azione parte davvero; proposta →
  `no` → non parte e non si insiste. Il sito #3 lo passerebbe già.
- **Test di fondamento**: a parità di forma, valenza diversa deve produrre coda
  diversa. È il test che oggi fallirebbe su "I am feeling sad today".
- **Test di silenzio**: contesti in cui non aprire, per verificare la regola 5.
  Serve a impedire che l'iniziativa diventi un tic.

## 10. Verifica contro i mantra operativi

Checklist di `motorize-the-class.md` §"Mantra operativi", passata su questo piano
**prima** di scrivere una riga. Le voci che hanno cambiato il piano sono marcate.

| # | Mantra | Esito |
|---|---|---|
| 1 | Generalizzabile KB-first? | **Sì** — è il piano stesso: motorizzare la classe "iniziativa", non fixare i nove siti |
| 2 | Niente liste di parole nel C; **procedura insegnabile** prima del C | **HA CAMBIATO IL PIANO** — §7 riscritto: la catena dei cinque stadi è una clausola in `procedures.p0`, il C resta adattatore |
| 3 | Astrai fino al punto fisso | `conversation_seed` non è una struttura a sé: è `initiative_ground` del registro sociale. Una relazione, non nove |
| 4 | Il sostantivo del prompt è sospetto | Verificato: sopra "iniziativa" c'è la scala stato → registro → fondamento → forma. Il motore prende il nome dalla procedura, non dallo smalltalk che l'ha suggerita |
| 5 | Cerca il motore esistente prima di scriverne uno | **HA CAMBIATO IL PIANO** — trovato `causes_t` inutilizzato (§7); la raccolta del sito #3 va **estratta**, non riscritta |
| 6 | Non estendere per analogia col codice | Vietato aggiungere un decimo modulo terminale per analogia coi nove: è precisamente la patologia da chiudere |
| 7 | Uccidi il muro, mai con una risposta sbagliata | Regola §6.4: la coda non trasforma un declino in risposta. Il muro sociale si chiude con iniziativa fondata o resta muro |
| 10 | Nessuna risposta prima di aver soddisfatto il piano | L'iniziativa è **coda**, mai sostituzione: se la risposta non c'è, non la si copre con una domanda aperta |
| 15 | Un failure vale come seed di fuzzing | §9 va esteso a fascio: stessa forma con valenza, lingua e registro diversi |

Il mantra #2 è quello che ha morso più a fondo, e valeva il richiamo: la prima
stesura di questo piano metteva in C la valutazione trigger → registro →
fondamento → forma. Sarebbe stato un consumer di dominio scritto in C, cioè il
decimo sito invece della facoltà.

## 11. Verdetto

**La facoltà è reale e vale la pena nominarla.** L'indizio più forte non è
teorico: sono nove implementazioni indipendenti della stessa mossa in cinque
stadi, nate in generazioni diverse, nessuna delle quali sapeva delle altre. È il
profilo tipico di un'astrazione mancante — e l'osservazione di F. sullo smalltalk
è il punto da cui la si vede.

La correzione che tiene il piano onesto, simmetrica a quella di
`universal-comprehension.md` §6: **iniziativa non significa parlare di più.**
Significa che, quando c'è un fondamento vero, il turno può continuare — e quando
non c'è, si tace. Senza la regola del fondamento (§6.1) questa facoltà diventa
esattamente la macchina da chiacchiere che i PRINCIPLES rifiutano, e in più
sarebbe difficile da smascherare, perché una domanda aperta ben formata somiglia
molto all'intelligenza.

Il pezzo strutturalmente più importante è il §4: **lo strato post-dispatch non
esiste**. Finché non esiste, ogni nuova forma d'iniziativa continuerà a nascere
come un decimo modulo terminale che deve rubare un turno per parlare.
