# Prosa, 13 settembre 2026 (gen514) — la direzione della domanda

Checkpoint di sviluppo sulla KB viva completa, a partire da `6fb2c343`. Banco e
attese invariati: `tests/fixtures/prose/ladder/r300.{txt,q}`. Nessun `/save`,
nessun fatto del brano persistito: **W=0, L=0, meta-capability-only**.

Punto di partenza: il laboratorio `2026-09-13-prosa-esplorazione/` (cinque
sonde). Il reperto piu' largo era che una domanda sul **soggetto** non arrivava
mai al fatto: «Hard carbonate exoskeletons support the coral» si legge, e «what
supports the coral?» andava a muro. Sotto c'era una bugia gia' esistente
nell'altro verso.

## Le distinzioni

**La forma finita porta la direzione.** In inglese la domanda sull'oggetto
chiede l'ausiliare e lascia il verbo nudo («what does coral secrete?»); quando
l'interrogativo e' il soggetto il verbo finito lo segue subito («what supports
coral?», «which animal eats fish?»). Il lettore scrive la radice (`support`), la
domanda portava solo la cue flessa `supports`, predicato vuoto.
`subject_question_verb/2` e `object_question_verb/2` (grammar.p0) leggono la
struttura del turno sui token pubblicati; `turn_question_verb/3` ne ricava il
ponte forma→radice e la direzione per `answer_frame_input_arg/3`.

**La bugia chiusa.** «what does coral support?» provava la relazione nei due
versi e rispondeva «Hard carbonate exoskeletons.» — cio' che sostiene il
corallo. Stesso difetto con un verbo insegnato a voce: «what do coral polyps
cement?» → «calcium carbonate.». Ora l'entita' fra ausiliare e verbo nudo e' il
soggetto.

**Non ogni ausiliare regge il verbo nudo.** Con `auxiliary/1` intero «what IS a
compass USED for?» diventava una domanda sull'oggetto di `used`. Il verbo nudo
lo reggono il «do» di supporto e i modali: `bare_verb_auxiliary/1`, che riusa
`modal_force/2`.

**Il frame universale deve tenere il tipo chiesto.** Con la direzione
dichiarata, la domanda entra anche nel produttore universale
(`turn_response`), che non applicava `answer_type_ok/2`: il banco ha mostrato
«what phylum does coral belong to?» → «Class anthozoa.», bugia che prima
murava. Il filtro ora sta in `turn_reading/4`, dove le letture nascono.

## Tre trappole pagate

1. **Una superficie con direzione e' anche un indizio di domanda**
   (`turn_declared_act`, turn-frames.p0). Dichiarare `belongs` senza condizioni
   trasformava «Coral belongs to the class Anthozoa» in una domanda che nessuno
   leggeva. La direzione si legge sulla struttura, mai sulla sola superficie.
2. **Il costo delle superfici sempre presenti.** Ottocento forme flesse fisse in
   `answer_frame/2` portavano una domanda qualsiasi da 650 a 770 ms; goal
   ordinati dai verbi invece che dai token raddoppiavano il turno. Una
   `materialized_view` sulla forma del turno non ha cambiato un passo (non
   viene materializzata) ed e' stata tolta invece di restare come acceleratore
   dichiarato che non accelera.
3. **`answering_surface/2` partiva dalla cue sola**, enumerando tutte le regole
   a testa variabile di `answer_frame/2`: +350 ms su ogni domanda relazionale
   con direzione. Riordinato (prima la direzione, che lega la relazione): la
   stessa congiunzione, 915 → 710 ms.

## Il motore: lo stack C uccideva il processo

Dentro il turno «what supports the coral?», con il fatto presente, il processo
moriva di SIGSEGV. Non ricorsione infinita: `solve_frame` pesa ~39 KB di stack
per goal e il solver a continuazioni li tiene tutti; con 8 MB il tetto e' ~200
goal. `main.c` alza il limite soft a 96 MB; `solve()` taglia oltre il limite
come ricerca incompleta (`budget_hit`), non come crash — verificato con
`ulimit -Hs 8192`. Il peso del frame resta aperto in `C_TODO.md`.

## Verifica

`tests/p0t/language/question_direction.p0t`: **23 verdi**; con le regole KB
tolte **tutti e quattro i blocchi sono rossi**. Il blocco del verbo insegnato
(«cement is a relation verb») prova crescita, ritrattazione e reinsegnamento, e
usa la domanda sull'oggetto perche' la domanda sul soggetto di un verbo nuovo la
prendeva gia' `mod_knowledge` per un'altra strada.
`prose_relation_scope.p0t` 31 verdi, `facts.p0t` 9 verdi. `make soft-test`:
invariato rispetto alla baseline, fermo su `basics.p0t` (tigre 1,11 s,
antonimo 1,24 s, limite 1 s).

Costo A/B per turno: domande non relazionali invariate (capitale 630/637 ms,
bussola 657–675 → 649 ms); domanda relazionale che risponde gia' 582 → 710 ms,
il prezzo del cammino universale che ora vede relazione e direzione.

Bilancio: KB grammar.p0 **+71**, turn-frames.p0 **+8/−3**; C **+53**, tutto
robustezza del solver (nessun vocabolario, nessuna migrazione rivendicata).

## Banco

Merito **17/50** (invariato), cancello **97/299**, meta 2/2, struttura 5/5.
Nessuna risposta corretta persa. La prima misura ([iterazione 1](2026-09-13-direzione-della-domanda/bench-iterazione1.txt))
aveva una bugia nuova — «what phylum does coral belong to?» → «Class anthozoa.»
— chiusa col filtro del tipo in `turn_reading/4`; la [misura finale](2026-09-13-direzione-della-domanda/bench-finale.txt)
torna al muro. Il circuito non sposta il numero da solo: le domande del banco
sulla direzione («what supports/protects/threatens…») aspettano ancora la
lettura delle loro frasi (relativa con «that», light verbs).

## Che cosa resta, in ordine

1. **La relativa con «that» senza virgola e i predicati coordinati** —
   «exoskeletons that support and protect the coral», «ocean waters that provide
   few nutrients». Le domande «what supports/protects the corals?» ora trovano
   il fatto se c'e': manca che la lettura lo scriva. `relative_opener/1` chiede
   la virgola; «that» e' anche complementatore («indicate that DART…») e
   dimostrativo, quindi la condizione e' strutturale (il token dopo e' una
   forma di verbo di relazione). La coordinazione ha gia' viste nel frame
   universale (`input_binary_object_coordination`): riusarle, non riscriverle.
2. **L'alternanza di voce**: «what holds coral polyps together?» arriva a
   `hold`, il fatto letto dal passivo sta sotto la relazione con particella.
   E' la declinazione di questo circuito.
3. **La coordinazione degli oggetti perde il secondo membro**: «Cnidaria includes
   sea anemones and jellyfish» → solo `sea anemones`; idem «nitrogen and
   phosphorus».
4. **«which animal secretes X?»** → elenco degli animali della KB (modulo
   `knowledge`), preesistente: la classe nominata non restringe le risposte.
5. Il plurale nella chiave: «what shades the path?» dopo «…shade the paths»
   mura, in entrambe le versioni.
6. «Shallow tropical coral reefs have declined by 50% since 1950» → «975.»
   (laboratorio di esplorazione): l'aritmetica rivendica una frase
   dichiarativa. Bugia, da mettere in testa al prossimo giro.
