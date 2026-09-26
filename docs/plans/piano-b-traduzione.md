# Piano B — capire passando per l'inglese quando la lingua della sessione non basta

**Piano, 26 settembre 2026, sera; rivisto lo stesso giorno perché sia un piano KB insegnabile (F.).** Nasce dalla sessione live «presa
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
> non sa tradurre e lo chiede, invece di un muro.

> **⛔ È un piano KB, insegnabile (F.).** Il piano B non è un comportamento del
> motore: è un **piano di situazione** come quelli di `user-situations.p0`, con
> una situazione («non capisco una frase detta in italiano»), delle mosse
> («chiedimi se so tradurla», «rileggila in inglese», «dimmi che parola manca»)
> e un ordine, che si **insegnano parlando** («quando non capisci una frase in
> italiano allora rileggila in inglese»), si correggono («move 2 for … is now
> …»), si raccontano («your plan when …?») e si ritirano. Senza una lezione, il
> piano B non scatta. Il motore mette a disposizione soltanto radici generiche
> che esistono già (`reread(…)`, le glosse, i template); nessuna azione, nessuna
> lingua e nessuna soglia del piano B sta nel C.

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
| i piani di situazione | [user-situations.p0](../../kb/core/user-situations.p0): `situation_cue/2`, `move_cue/2`, `plan_move/3`, `turn_situation_plan/2`, `answer_content(situation_plan(…))`; le forme `teach_plan`/`teach_plan_it` | una situazione riconosciuta esegue le mosse **insegnate a voce**, nell'ordine insegnato; senza piano la porta resta chiusa | è la **forma** del piano B: manca la situazione «non capito nella lingua della sessione» e mancano le mosse che la servono |
| la rilettura | l'atto `reread(Testo)` delle forme (C, radice generica) | rilegge un testo come turno annidato, con la lingua del discorso; ogni lettore esistente diventa bersaglio | è il modo di **eseguire** la mossa «rileggila in inglese», senza un'azione nuova nel motore |
| l'esecutore delle compensazioni | C `p0_compensate`, KB `compensates(Azione, Specie)` | per una specie di arresto prova le azioni dichiarate e ripone il turno | ⚠ sceglie le azioni con uno `strcmp` sul nome (`repair_surface`): è un residuo di condotta compilata, e il piano B **non** deve aggiungergli un ramo |
| la resa italiana | `response_template(K, it, …)`, `current_language/1`, `tr/2` | molte risposte hanno la loro versione italiana | la risposta a una lettura inglese può uscire in inglese (PR8, e «A gatto eats Fish» in gloss.p0) |

## 3. La tesi: un piano di situazione insegnabile, con un giudizio in più

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

### 3.2 Il piano B è un piano di situazione, eseguito con radici che esistono

```text
turno detto in L (≠ lingua pivot) ──► esito insoddisfacente (muro, gap kind)
        │
        ▼  situazione riconosciuta (KB): «non capisco una frase detta in L»
        │
        ▼  il piano INSEGNATO per quella situazione, mossa per mossa:
        │
   mossa «chiediti se sai tradurla»   ──► translation_coverage(T, …)   (§3.1)
        │ parziale ──► mossa «dimmi che parola manca» (il passo 5 che c'è)
        │ piena
        ▼
   mossa «rileggila in inglese»       ──► reread(<traduzione costruita in KB>)
        │ non riparte ──► «anche tradotta in inglese non la so leggere»
        │ riparte
        ▼
   mossa «rispondimi in italiano»     ──► resa in L + qualifica della traduzione
```

- **La situazione** è vocabolario KB come le altre: come si riconosce
  (l'esito insoddisfacente di un turno detto in una lingua il cui vocabolario
  non è completo, `lexicon_language/1`) e come si **nomina** per insegnarla
  (`situation_cue("qualcuno dice una frase in italiano che non capisci", …)`,
  in italiano e in inglese).
- **Le mosse** sono vocabolario KB (`move_cue/2`) e ciascuna ha il suo
  contenuto (`answer_content`/`situation_move_text`), come «Vediamolo insieme.».
  Una mossa che deve **fare** qualcosa, come rileggere, si scrive con l'atto
  generico `reread(…)` sul testo che la KB costruisce: la traduzione è un
  termine KB fatto di glosse (`tr/2`), flessioni (`inflection/3`) e riscritture
  (`phrase_canon/2`, `rewrite_es/2` come precedente), non una funzione C.
- **Il piano** (quali mosse, in che ordine) non è scritto in KB a mano: lo si
  insegna a voce e lo si salva con `/save`, come il piano di `user_problem`.
  Il `.p0` porta soltanto il vocabolario (situazione e mosse), la forma porta la
  lezione. È la gerarchia di crescita del MANTRA: la riga manuale apre un
  canale, non sostituisce la lezione.
- **La lingua pivot** è un fatto (`pivot_language(en)`), non una costante del C.

### 3.3 Il test KB-first del piano B

Il piano è nel posto giusto solo se tutte queste cose si fanno **parlando**,
dal turno dopo, senza ricompilare:

1. insegnarlo: «quando qualcuno dice una frase in italiano che non capisci
   allora rileggila in inglese»;
2. raccontarlo: «qual è il tuo piano quando non capisci una frase in italiano?»;
3. correggerne una mossa: «move 1 for … is now dimmi che parola manca»;
4. restringerlo o ritirarlo (mantra #17): «non rileggere in inglese le domande
   su di te», e il ritiro del piano;
5. allargarne il materiale: «the italian for could is potrebbe», una
   contrazione, una costruzione, e la copertura del §3.1 cambia.

Se uno di questi richiede di toccare il C, il piano B è condotta compilata
esattamente come il ramo `strcmp` dell'esecutore delle compensazioni, e va
fermato lì. Dove oggi una forma di lezione è rotta (le situazioni nuove non si
insegnano, PR4; la forma italiana del racconto del piano manca, PR13), quello
è un **blocco dichiarato** del piano B, da curare come tale.

## 4. Vincoli che il piano non può violare

1. **Nessun secondo cervello** (L4 §2.7). Riproporre un testo non è chiamare
   una funzione pura: una lettura può imparare fatti, aprire questioni,
   avanzare il dialogo. La rilettura tradotta è **ipotetica**: niente fatti
   scritti, niente contatti, niente tabellone avanzato due volte. L'impegno
   avviene una volta sola, sul turno originale, se la lettura tradotta regge.
   Il precedente è `action_repair_surface`; se la separazione analisi/impegno
   non basta, lo si dichiara come blocco e lo si costruisce prima.
2. **Niente nel C che sia del piano B.** Situazione, mosse, ordine, lingua
   pivot, soglie di copertura, glosse, flessioni e riscritture stanno in KB; il
   motore offre solo le radici generiche che esistono (`reread(…)`, i template).
   Nessun nome d'azione nuovo nell'esecutore delle compensazioni. Se una radice
   manca davvero (per esempio una rilettura **ipotetica**, che non impegni), si
   aggiunge nella forma generica, utile a ogni piano, e lo si dichiara.
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

**B2 — il piano come conoscenza.** Il vocabolario della situazione e delle
mosse in KB (italiano e inglese), la mossa «rileggila in inglese» scritta con
`reread(…)` sulla traduzione costruita in KB, e i blocchi delle forme di
lezione che servono (PR4, PR13). Chiusura: il piano si insegna, si racconta, si
corregge e si ritira in una sessione live (§3.3); su (a) la risposta arriva;
su (b) il muro onesto nomina l'inglese; nessun fatto scritto dalla rilettura.

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
| un'azione `reread_through_pivot` nell'esecutore delle compensazioni | un nome d'azione in C è condotta compilata: il piano non si insegna né si ritira parlando |
| scrivere il piano (mosse e ordine) a mano nel `.p0` | sostituisce la lezione invece di aprirle il canale; il `.p0` porta il vocabolario, la lezione porta il piano |
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
