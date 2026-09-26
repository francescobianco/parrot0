# Piano B — capire passando per l'inglese quando la lingua della sessione non basta

**Piano, 26 settembre 2026, sera.** Nasce dalla sessione live «presa
sull'interlocutore» in italiano ([train-the-learning-process.md](train-the-learning-process.md),
§0, PR8–PR12). F.:

> *«l'italiano è molto indietro rispetto all'inglese, ma attraverso il reasoning
> lui dovrebbe accorgersi se sa tradurre la frase e rispondere ragionandola in
> inglese. Questo dovrebbe essere un piano: c'è una sorta di piano B legato alla
> mancata comprensione direttamente nella lingua di riferimento della sessione.»*

> **In una frase.** Quando un turno detto nella lingua della sessione non viene
> capito, parrot0 si chiede **se sa tradurre la frase intera**. Se sì, la rilegge
> in inglese, dove la sua comprensione è più ampia, e risponde nella lingua
> della sessione, dicendo come ha ottenuto la lettura. Se no, nomina che cosa
> non sa tradurre e lo chiede, invece di un muro. È una **compensazione** di una
> specie di arresto, nel ciclo che il motore ha già; non è un secondo cervello
> né un traduttore parallelo.

---

## 1. Il caso misurato

Dalla sessione del 26 settembre (`docs/sessions/live/2026-09-26-presa-it.log`,
contrasti in inglese nello stesso processo):

| turno italiano | risposta | lo stesso in inglese | risposta | che cosa dice del piano B |
|---|---|---|---|---|
| «Mi aiuti?» | «Non capisco ancora.» | «Can you help me?» | elenco delle capacità | **bersaglio**: l'inglese capisce, l'italiano no |
| «Cosa devo fare?» | «Non so ancora tradurre «devo».» | «What should I do?» | «…I don't have a grounded plan…» | bersaglio parziale: manca una parola, e il passo 5 lo dice già; il resto va ragionato |
| «Qual è la temperatura interna sicura del pollo?» | definizione di temperatura | «What is the safe internal temperature of chicken?» | la stessa definizione | **non** è piano B: la traduzione ha funzionato, sbaglia la comprensione in entrambe le lingue (PR10) |
| «Per quanto tempo il riso cotto può stare fuori dal frigo?» | «va un po' oltre…» | «How long can cooked rice stay out?» | «don't know about cooked» | **non** è piano B: muro in entrambe |
| «Ciao, mi serve una mano con il tornio.» | (dopo lezioni inglesi) risposta **in inglese** | — | — | la lingua della risposta deve essere quella della sessione, non quella appiccicosa dei turni precedenti (PR8) |

La distinzione conta: il piano B cura solo i turni che **l'inglese capisce e
l'italiano no**. Quando la comprensione manca anche in inglese, il difetto è
altrove, e il piano B non deve mascherarlo con una traduzione riuscita.

## 2. Che cosa esiste già

| pezzo | dove | che cosa fa oggi | limite per il piano B |
|---|---|---|---|
| canonicalizzazione | C `canonicalize_lang`, tabelle KB `tr/2`, `phrase_canon/2` | traduce **parola per parola** le parole funzione e le glosse prima del dispatch | non è la traduzione di una frase: «dimmi qualcosa sui vulcani» diventava «tell me qualcosa on the vulcani»; soggetto sottinteso, clitici e ordine restano italiani |
| lo schema `translate_turn` | [gloss.p0](../../kb/core/gloss.p0), `thinking_scheme/step/after/stop` | 5 passi dichiarati: parole, flessione (`inflection/3`), domanda riproposta, qualifica («Leggo «X» come «Y»»), parola non tradotta nominata | lavora **dentro** il turno, non dopo un fallimento; non giudica se la frase intera è tradotta |
| le ricevute | `turn_translated/3`, `turn_kept/2` | quali parole del turno sono state tradotte e quali tenute | sono il materiale del giudizio di copertura (§3.1), oggi nessuno le somma |
| il passo 5 | `turn_untranslated/2` | in una domanda nomina la parola che manca e come insegnarla | è l'esito corretto quando la copertura è parziale; va agganciato al piano B, non duplicato |
| l'arresto tipizzato | [arrests.p0](../../kb/core/arrests.p0), `turn_arrest(Turn, language(In, Out), Kind, …)` | le coordinate di un turno fermo, **lingua d'ingresso e d'uscita comprese** | la specie «non capito nella lingua della sessione» non esiste |
| l'esecutore delle compensazioni | C `p0_compensate` (`99-registry.c`), KB `compensates(Azione, Specie)` | per una specie di arresto prova le azioni dichiarate, ripone il turno, si ferma alla prima che lo fa ripartire (precedente: `repair_surface`) | manca l'azione «rileggi attraverso l'inglese» |
| la resa italiana | `response_template(K, it, …)`, `current_language/1`, `tr/2` | molte risposte hanno la loro versione italiana | la risposta a una lettura inglese può uscire in inglese (PR8, e «A gatto eats Fish» in gloss.p0) |

## 3. La tesi: una compensazione in più, con un giudizio in più

### 3.1 «So tradurre questa frase?» è un giudizio KB

Prima di rileggere, parrot0 deve **accorgersi** se la traduzione è
affidabile. Il giudizio si deriva dalle ricevute che la lettura già scrive:

- **copertura lessicale**: ogni parola piena del turno è tradotta
  (`turn_translated`), oppure è parola funzione, nome proprio o tema noto.
  Altrimenti la copertura è parziale e porta le parole mancanti;
- **copertura strutturale**: le costruzioni che l'italiano non condivide con
  l'inglese sono state rese: soggetto sottinteso («devo» → «I must»), clitici
  («dimmelo»), negazione prima del verbo («non è» → «is not»), ordine. Sono
  riscritture KB con il precedente di `rewrite_es/2` e `phrase_canon/2`; il
  giudizio chiede se il turno ne contiene una che nessuna riscrittura copre.

Il risultato è interrogabile, per esempio `translation_coverage(Turn, full)` o
`translation_coverage(Turn, partial(Mancanti))`, e si **dice**: fa parte della
qualifica della risposta, come oggi «Leggo «X» come «Y»». Una lettura
ottenuta per traduzione ha una precisione diversa, e chi ascolta deve saperlo
(F., 11 settembre, già in gloss.p0).

### 3.2 La specie di arresto e la sua azione

```text
turno detto in L (≠ en) ──► esito insoddisfacente (muro, gap kind)
        │
        ▼
turn_arrest(T, language(L, L), unread_in_session_language, …)
        │
        ▼  compensates(reread_through_pivot, unread_in_session_language)
        │
   copertura piena? ──no──► nomina ciò che manca (passo 5) e chiedi: niente muro
        │ sì
        ▼
rilettura IPOTETICA della frase tradotta in inglese
        │
   riparte? ──no──► muro onesto: «anche tradotta in inglese non la so leggere»
        │ sì
        ▼
risposta resa in L + qualifica («l'ho capita traducendola: «…»»)
```

- **L'innesco** è KB: quali esiti contano come «non capito»
  (`unsatisfying_outcome/2`, `turn_gap_kind/2`), per quali lingue vale
  (`lexicon_language/1` dice già in quali lingue il vocabolario **non** è
  completo), e se la condotta è accesa.
- **L'azione** `reread_through_pivot` è una primitiva dell'esecutore, come
  `repair_surface`. Il motore sa **come** riproporre un testo come lettura
  ipotetica; **quale** testo, **quando** e **in che lingua rendere** lo dice la KB.
- **La lingua pivot** è un fatto (`pivot_language(en)`), non una costante del C:
  oggi l'inglese è la lingua in cui la comprensione è più ampia, domani potrebbe
  non esserlo per un dominio.

### 3.3 La condotta stessa è conoscenza (mantra #17)

Il piano B è uno schema di pensiero dichiarato, come `translate_turn`, con
passi, dipendenze e punti d'arresto. Ne segue che si può **discutere e
correggere parlando**: «when you don't understand Italian, try English»,
«non tradurre le domande su di te», e il suo ritiro. Se per spegnerlo o
restringerlo bisogna ricompilare, sta nel posto sbagliato.

## 4. Vincoli che il piano non può violare

1. **Nessun secondo cervello** (L4 §2.7). Riproporre un testo non è chiamare
   una funzione pura: una lettura può imparare fatti, aprire questioni,
   avanzare il dialogo. La rilettura tradotta è **ipotetica**: niente fatti
   scritti, niente contatti, niente tabellone avanzato due volte. L'impegno
   avviene una volta sola, sul turno originale, se la lettura tradotta regge.
   Il precedente è `action_repair_surface`; se la separazione analisi/impegno
   non basta, lo si dichiara come blocco e lo si costruisce prima.
2. **Niente logica per lingua nel C.** Tabelle di glosse, flessioni,
   riscritture e soglie stanno in KB; il C conosce solo la primitiva di
   rilettura.
3. **La risposta nella lingua della sessione** (PR8). La decide la lingua del
   turno, non quella appiccicosa dei turni precedenti. Se una risposta non ha
   resa italiana, lo si dice in italiano e si dà la parte inglese come
   citazione, dichiarata.
4. **Mai spacciare per capito ciò che è stato indovinato.** Una traduzione con
   flessione indovinata o una costruzione riscritta porta la sua qualifica.
   Una copertura parziale non si rilegge: si chiede.
5. **Non mascherare i difetti inglesi.** Se anche la lettura inglese mura o
   sbaglia (PR10, PR11), il piano B risponde «anche tradotta non la so
   leggere», e il turno entra nel repertorio della comprensione, non in quello
   della lingua.
6. **Costo.** Un turno in più solo quando il primo è fallito, dentro il budget
   del turno; una rilettura sola per turno, mai a catena.

## 5. Crescita per fasi

**B0 — lo specchio.** Un banco di coppie italiano/inglese dello stesso
contenuto, preso dalla KB viva e dalle sessioni (la regola dello specchio
multilingue: la stessa competenza nelle due lingue per la stessa strada).
Ogni coppia si classifica: (a) inglese sì, italiano no, **bersaglio**;
(b) tutte e due no, repertorio della comprensione; (c) italiano sì. Il numero
che conta è la quota di (a) che il piano B porta in (c), senza spostare niente
da (b) a una risposta sbagliata.

**B1 — il giudizio.** `translation_coverage/2` dalle ricevute, con le
riscritture strutturali più frequenti del banco (soggetto sottinteso,
negazione, clitici). Chiusura: parrot0 sa dire, per un turno, se l'ha tradotto
tutto e che cosa manca, anche senza rileggere.

**B2 — la compensazione.** La specie `unread_in_session_language`,
`compensates(reread_through_pivot, …)`, la primitiva di rilettura ipotetica.
Chiusura: su (a) la risposta arriva; su (b) il muro onesto nomina
l'inglese; nessun fatto scritto dalla rilettura; tempo entro il budget.

**B3 — la resa.** La risposta esce nella lingua della sessione, con la
qualifica della traduzione. Chiusura: nessuna risposta inglese a un turno
italiano senza che sia dichiarata come citazione.

**B4 — l'insegnabilità.** Ciò che il giudizio trova mancante (una parola, una
costruzione) si insegna parlando e vale dal turno dopo; la condotta del piano
B si discute e si ritira parlando. Chiusura: una lezione in italiano porta un
turno da (a) a (c) senza passare dal piano B, cioè la lingua cresce e il
ripiego serve sempre meno.

## 6. False piste

| scorciatoia | perché no |
|---|---|
| un traduttore statistico o un modello esterno | non è parrot0 e non si insegna parlando |
| duplicare le facoltà per l'italiano | è il fallimento che lo specchio multilingue esiste per scoprire |
| rispondere in inglese senza dirlo | l'interlocutore perde la presa proprio nel turno in cui l'avevamo ripresa |
| rileggere anche con copertura parziale | una frase tradotta a metà si legge male con fiducia: peggio del muro (mantra #7) |
| lasciare che la rilettura impari | due impegni per un turno, e fatti nati da una traduzione indovinata |
| contare la traduzione riuscita come comprensione | PR10: tradotta bene, capita male |

## 7. Relazione con gli altri piani

- [l4-upgrade.md](l4-upgrade.md): la lezione sulla lingua detta in italiano non
  è riconosciuta (residuo dell'incremento 2). Il piano B è la stessa idea sul
  lato della comprensione: ciò che l'inglese sa fare deve valere per chi parla
  italiano, e la differenza deve essere visibile e rivedibile.
- [train-the-learning-process.md](train-the-learning-process.md) §0: PR8
  (lingua della risposta), PR11–PR12 (muri su domande comuni) sono il primo
  banco di B0.
- [live-teaching.md](live-teaching.md): le sessioni in italiano misurano
  quanto il piano B serve meno man mano che la lingua cresce (B4).
