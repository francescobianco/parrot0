# C_TODO — che cosa deve ancora uscire dal C

> Compagno di `KB_TODO.md`, che elenca i residui della conoscenza. Questo
> elenca i residui del **motore**: tutto ciò che oggi vive in `src/brain/*.c` e
> che, per i mantra #2 e #16, dovrebbe essere conoscenza.
>
> Aperto durante la campagna gen442-443. Le misure sono sonde a espressioni
> regolari su 40.918 righe di moduli: ordini di grandezza, non un censimento.

## Stato al 2026-08-27

| | inizio campagna | ora |
|---|---:|---:|
| punti della voce con chiave KB | 160 | **656** |
| `snprintf` di frase senza chiave | 297 | **11** |
| `put("…")` letterali | 77 | **7** |
| `cue(x, "letterale")` | 1393 | **1** |
| parole in `strcmp`/`strstr` | 1629 | **292** |
| predicati di dominio nominati nel C | 250 | **250** |
| template in `kb/core/messages.p0` | 49 | **524** |

La regola che governa tutto, ed è più netta del mantra #16
(vedi `docs/plans/messages-are-knowledge.md`):

> **Nel C non deve esistere nessun testo umanizzato.** Il default, quando la KB
> non è addestrata, è la forma funzionale del messaggio — `famiglia(valore)` —
> non una frase più povera. `kb_term_say/6` lo fa già: se in chat compare un
> termine, è una famiglia che nessuno ha ancora insegnato, e si vede quale.

---

## 1. Gli undici `snprintf` rimasti — e nessuno è un messaggio

Vanno chiusi **decidendo che cosa sono**, non estraendoli a macchina.

### 1a. Frammenti di composizione (5)

Costruiscono una frase pezzo per pezzo, quindi la famiglia giusta è la **frase
intera**, che va disegnata.

- `10-memory-knowledge.c:1058` — `"Learned rule: %s("`
- `10-memory-knowledge.c:12319` — `"Learned rule: %s(X) :- "`
- `20-math.c:2431` — `"Near-rhymes for \"%s\": "`
- `60-agent-tools.c:566` — `` "Matches for `%s`" ``
- `10-memory-knowledge.c:579,628` — `"%s your name is %s"`, `"%s current topic is %s"`
  (il primo `%s` è un prefisso variabile: «I remember: » / «You told me: »)

**Come chiuderli:** una famiglia per la frase completa, con gli slot che oggi
sono pezzi separati — `learned_rule(head, body)`, `near_rhymes(word, list)`.
Serve guardare il ciclo che li compone, non solo la riga.

### 1b. Dati travestiti da testo (4)

Non sono la voce di parrot0: sono **superfici di concetti**. La loro casa non è
`messages.p0`, è il lessico.

- `10-memory-knowledge.c:11491-11492` — `"the United States"`, `"the United Kingdom"`
  (dovrebbero essere `concept_label/4` o equivalente: la forma con articolo di
  un nome di paese)
- `30-generation-reading.c:1022` — `"quiet street"` (materiale di scena)

**Come chiuderli:** una riga di lessico e un `kb_match`, non un
`response_template`. Metterli fra i template li archivierebbe nel posto
sbagliato solo per far scendere un contatore.

### 1c. Diagnostica di piano (2)

- `25-wordmath-reasoning.c:784,798` — `"%s stopped at step %zu %s because no
  action_impl fact named…"`, `"%s stopped at step %zu %s via %s: %s."`

Sono messaggi veri e convertibili; hanno quattro-cinque slot e meritano nomi di
slot scelti a mano.

## 2. I sette `put("…")` letterali

Stanno in funzioni senza `Brain` in portata, o contengono virgolette interne.
Il threading è la stessa operazione già fatta per `is_truth_probe`,
`is_wellbeing_content`, `looks_code`, `build_turn`, `idk`, `piact_dir`.

## 3. Le 292 parole ancora confrontate nel C

È il residuo del mantra #2, e la parte più grande che resta. Concentrate in:

| file | conta |
|---|---:|
| `25-wordmath-reasoning.c` | 60 |
| `10-memory-knowledge.c` | 43 |
| `20-math.c` | 43 |
| `60-agent-tools.c` | 30 |
| `90-repair-robust-abduce.c` | 29 |
| `70-social-pragma.c` | 15 |

Quasi tutte stanno in **funzioni ausiliarie che non ricevono un `Brain`**
(`is_entity_pronoun`, `wp_length_unit`, `arith_op_char`, `eval_operand`,
`parse_branch_ops`, `parse_goal_loose`, …). Circa 35 funzioni.

**Come chiuderle:** aggiungere `Brain *b` come primo parametro e aggiornare i
chiamanti, poi far girare la conversione. **Non in blocco**: un tentativo
automatico su tutte e 35 ha rotto la build in dieci punti, perché il threading
va a cascata e alcuni chiamanti lavorano su un sub-brain (`run_composition` usa
`sub`, non `b`). Poche per volta, con il compilatore a ogni passo.

Non tutti i letterali sono uguali: `strcmp(tok, "@S")` confronta un simbolo del
protocollo e va benissimo. Il numero vero è più basso di 292, ma resta nelle
centinaia.

## 4. I 250 predicati di dominio nominati nel C

**La voce più grave rispetto alla tesi del progetto**, e la sola che non si può
affrontare a macchina.

Dei 465 nomi di predicato scritti nei moduli, 215 sono di **protocollo** —
`input_frame_commit`, `turn_teaching_offer`, `turn_gap_middle`: il contratto
aperto fra motore e conoscenza, documentato e legittimo. Gli altri **250 sono
di dominio**: `borders`, `capital_of_country`, `planet_superlative`,
`magnitude`, `category_member`, `riddle_sig`.

Finché il motore nomina `borders`, non è un adattatore: è un'enciclopedia con
un parser davanti. Ogni caso richiede di trovare (o costruire) la relazione più
generale che lo comprende — mantra #3, astrai fino al punto fisso — e non si
chiude con una sostituzione.

## 5. I nomi di famiglia generati per slug

Le famiglie create dalla campagna hanno nomi derivati dal testo inglese:
`got_it_i_ll_treat_x_as_a_conjunction_now_lik`. Andava bene quando la chiave era
invisibile; **ora la chiave è la voce di riserva**, quindi è ciò che
l'interlocutore legge quando la KB tace.

Vanno rinominate le famiglie che si vedono spesso — `conjunction_taught`,
`name_recall`, `punctuation_only` — lasciando lo slug alle altre. Una rinomina
tocca due punti: la riga in `kb/core/messages.p0` e la chiave nel sito C.

## 6. Il debito lasciato dalla campagna

- **Test rossi.** La campagna ha cambiato la forma di molti messaggi (le
  virgolette dritte sono diventate « »), e i commit di conoscenza appresa hanno
  aggiunto template che entrano nella rotazione: `teachverb`, `retract`,
  `glue`, `reflexive_*`, `prosefact.it` e altri si aspettano formulazioni
  precedenti. La bisezione è già fatta e sta nel messaggio di `b81b52b`.
- **`messages.p0` è cresciuto per accumulo**, non per disegno: 524 template in
  un file solo, in ordine di conversione. Va diviso per famiglia quando
  diventerà scomodo — non prima (mantra sulle strutture secondarie).
- **`cue(norm, "?")`** in `30-generation-reading.c:82` resta, ed è corretto: la
  punteggiatura è una meccanica, non vocabolario.

---

## Le guardie imparate sbagliando

Chi riprende questa campagna eviti di ripetere questi quattro errori, tutti
commessi e corretti durante gen442-443:

1. **Un frammento non è un messaggio.** Se il testo ha parentesi o virgolette
   spaiate, o finisce con un separatore, sta componendo qualcosa: la famiglia
   giusta è la frase intera.
2. **`snprintf` come espressione va lasciato stare.** `off = (size_t)snprintf(…)`
   usa il valore di ritorno; sostituirlo lascia un'assegnazione senza destra.
3. **Le dichiarazioni issate non stanno nel corpo nudo di un `if`.** Servono le
   graffe, tranne quando il prefisso dichiara una variabile usata dopo — e in
   quel caso, se è anche un corpo nudo, si lascia stare.
4. **Il rilevamento della guardia KB dev'essere stretto.** Guardare indietro
   senza fermarsi al confine di istruzione fa saltare i siti *vicini* a una
   conversione precedente, che non c'entrano nulla.

E una regola di metodo che ha retto per tutta la campagna: **convertire a lotti
piccoli, con `make build` e una prova a mano dopo ognuno, e committare ogni
lotto.** Un lotto grande che rompe la build costa più di dieci lotti piccoli.
