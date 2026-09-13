# Autoaddestramento dalla prosa — il testo come supervisore

> **F., 13 settembre 2026:** «l'addestramento supervisionato come stai facendo è
> molto utile ma scala lentamente: le forme sono sempre puntuali e le verifiche
> richiedono tempo. Interroghiamoci su come poter rendere questo processo più
> dinamico, più veloce e più automatico senza un supervisore intelligente.»

## 0. Da che cosa nasce

Sessione gen514 (13 settembre): piolo r300 da 17/50 a 49/50 con il cancello
passato, piolo r320 da 4/13 a 12/13. Il supervisore (l'agente) ha fatto quattro
lavori, e solo uno è davvero intelligente:

| lavoro | come si è fatto | quanto costava |
|---|---|---|
| 1. trovare il difetto | leggere il referto del banco scritto a mano | 8–10 min per banco |
| 2. diagnosticarlo | `P0_READ_TRACE`, bisezione, `!query` sul demone | la maggior parte del tempo |
| 3. scegliere la cura | un repertorio che si è ripetuto (§3) | il lavoro intelligente |
| 4. verificare | cricchetti + due banchi (r300 contro le regressioni) | 10–15 min per gradino |

Il repertorio del punto 3, ricostruito dai commit della sessione, è di circa otto
mosse: unire due nomi della stessa relazione (`include`/`includes`); aggiungere un
membro a una classe di confine (`np_closer`, `np_opener`); aggiungere una
particella (`verb_particle`); dare la porta interrogativa a una costruzione;
dichiarare un verso (tutto-parte, soggetto/oggetto); rendere trasparente una parola
(avverbio, aggettivo valutativo); dividere una clausola a un segno dichiarato;
far cedere un lettore legacy. Quasi ogni cura è stata una **dimensione
descrittiva** della KB (mantra #23).

## 1. Le ipotesi

**H1 — Il testo come supervisore.** Per ogni fatto letto `R(a, b)`, con la sua
frase sorgente (`fact_source/3`), si generano le domande nelle forme che la lingua
conosce (sull'oggetto, sul soggetto). La risposta attesa è nota perché il fatto
viene dal testo: nessun oracolo esterno. Un fallimento è una **strada interrotta**:
il fatto c'è, la domanda non ci arriva.

**H2 — La domanda al contrario rivela le bugie.** Per `made_of(reefs, colonies)`,
«what are colonies made of?» deve murare. Se risponde con `reefs`, è una bugia di
direzione — trovata senza nessuno che la riconosca. È ciò che rende H1 sicuro:
senza supervisore il rischio vero sono le risposte false.

**H3 — La perdita di lettura come lacuna tipata.** `P0FrameReading` misura già
`consumed`/`total`. Le parti non consumate («for tourism», «since 1950», le
parentetiche, «including …») sono state quasi tutte le cause dei muri di gen514.
Pubblicate in KB con la specie letta dalla prima parola scoperta, sostituiscono la
diagnosi a mano.

**H4 — Il catalogo delle cure, generato e provato.** Ogni lacuna tipata (H3) o
fallimento di H1 indica 1–3 mosse candidate (`remedy_for(Specie, Mossa)` in KB, così
anche le mosse restano insegnabili). Un ciclo le prova sulla sola frase coinvolta
(secondi) e tiene quella che chiude la lacuna senza aprire bugie (H2). È il gradino
3 della gerarchia di crescita di `MANTRA.md`.

**H5 — Verifica veloce e mirata.** Letture in cache per frase; si rifanno solo le
domande i cui predicati sono toccati; frammenti in parallelo. La regressione è
l'insieme delle domande di H1 già chiuse su tutti i pioli: un cricchetto che cresce
da solo.

**H6 — La frequenza al posto del giudizio.** H1+H3 su tutta la scala (una trentina
di testi) danno un istogramma delle specie di lacuna; si cura la specie più
frequente su più testi, e una mossa si promuove solo se chiude lacune su più pioli
(contro l'adattamento a un testo solo).

## 2. Rischi, da misurare fin dal primo esperimento

- **Cure che passano i test e degradano altro.** In gen514 ogni gradino ne ha
  prodotta almeno una (verso, salienza, avverbi, `answer_frame` da 9 400 a 151 000
  passi). Il criterio d'accettazione automatico deve contenere H2 e il costo.
- **Crescita della KB senza potatura.**
- **Circolarità.** Le domande di H1 sono generate dalla stessa lingua che legge:
  misurano la *raggiungibilità*, non la comprensione. Il banco scritto a mano per
  piolo resta la verifica finale, non il motore.
- **I fatti sbagliati sono sorgente di domande «giuste».** Un fatto letto male
  (`compost of ingredients used as…`) genera domande che «passano». H1 va
  affiancato da un controllo sulla lettura (il fatto è compatibile con la frase?).

## 3. L'esperimento E1 — H1+H2, retroattivo

**Domanda:** il rilevatore automatico trova da solo i difetti che gen514 ha curato
a mano?

**Strumento:** `scripts/self-questions.py` (strumento di progetto, come
`prose-probe.sh`; la generazione delle domande migrerà in KB come forme
interrogative se l'ipotesi regge). Per un testo:
1. avvia `parrot0 --mcp-engine` con la KB viva, legge il testo con `gen.respond`;
2. estrae i fatti binari letti da `fact_source/3` la cui frase sorgente sta nel
   testo;
3. per ogni fatto genera: **T1** domanda sull'oggetto (attesa: `b`), **T2** domanda
   sul soggetto (attesa: `a`), **R1** la domanda sull'oggetto con i ruoli invertiti
   (attesa: muro, cioè niente di `a`);
4. classifica ogni risposta: `ok` (contiene l'atteso), `muro`, `altro` (risponde
   con qualcosa di diverso), `bugia` (R1 risponde con l'altro argomento).

**Confronto:** lo stesso testo sul commit di inizio sessione (`6fb2c343`) e sul
commit corrente; r300 e r320.

**Esito atteso, se H1+H2 reggono:**
- la chiusura di T1/T2 cresce fra i due commit in modo coerente con il banco
  scritto a mano (17 → 49 e 4 → 12);
- R1 sul commit iniziale trova almeno le bugie di direzione curate a mano
  (supports, made of, belong, cement);
- i fallimenti residui sul commit corrente indicano specie di difetto che il banco
  a mano non vede.

**Se non reggono:** lo si sa senza aver costruito H3–H6.

## 4. Risultati di E1 (13 settembre 2026)

Strumento: `scripts/self-questions.py` (per farlo servire, `kb.match` MCP ha
ricevuto `offset`/`limit` e il totale: con il tetto fisso a 64 i fatti di sessione
non si vedevano mai). Stesso strumento sui due commit: il worktree di `6fb2c343`
ha ricevuto solo il blocco `kb.match` di `src/mcp.c`. Referti JSON in
[`docs/labs/apprendimento-assistito/2026-09-13-e1-domande-dal-testo/`](../labs/apprendimento-assistito/2026-09-13-e1-domande-dal-testo/).

| testo | commit | fatti letti | T1 ok | T2 ok | **R1 bugie di verso** | tempo |
|---|---|---:|---:|---:|---:|---:|
| r300 | inizio (`6fb2c343`) | 12 | 11 | 9 | **11** | 79 s |
| r300 | fine (`7932ee90`) | 29 | 25 | 23 | **8** | 137 s |
| r320 | inizio | 6 | 5 | 5 | **5** | 46 s |
| r320 | fine | 18 | 18 | 18 | **2** | 113 s |

### 4.1 H1 e H2 reggono: sul commit iniziale trovano i difetti curati a mano

Senza nessun supervisore, in poco piu' di un minuto per testo, il rilevatore
segnala sul commit di inizio sessione **le stesse classi** che gen514 ha curato a
mano in molte ore:

- **bugie di verso** su quasi ogni fatto (11/12 e 5/6): «what are colonies made
  of?» → «Reefs.», «what does class anthozoa belong to?» → «coral.», «what does
  soil fertility improve?» → «compost.» — il circuito della direzione;
- **domande sul soggetto che murano**: «what secretes hard carbonate exoskeletons?»
  — `subject_question_form`;
- **due nomi della stessa relazione**: `include`/`includes`, muro in T1 e T2;
- **letture storte** visibili nei fatti stessi: `used(compost_of_ingredients,
  as_plant_fertilizer)`, `break(materials, down)`, `monitor(closely, process)`.

### 4.2 Sul commit finale segnala difetti che il banco scritto a mano non vede

- **bugie di verso rimaste, una classe sola**: la cue con particella non ha verso
  («what do ocean waters flourish IN?» → «coral reefs.», «what do groups cluster
  in?», «what does animal phylum cnidaria belong to?», «what does organic material
  break up?», «what is calcium carbonate held together by?»). Il circuito di gen514
  copre il verbo nudo e le superfici tutto-parte insegnate, non le superfici
  `verb_particle_surface`;
- **morfologia della terza persona**: «what occupies…?», «what flourishes…?» murano
  perche' `subject_question_form` fa radice+«s» («occupys», «flourishs»);
- **strade interrotte**: «what are coral reefs located in?», «also known as» —
  il fatto c'e';
- **misclaim**: «what are sensitive to water conditions?» → la definizione
  dell'acqua;
- **lettura**: `break(materials, down)` — la particella «down» letta come oggetto.

### 4.3 Che cosa E1 non misura

- **La copertura**: 12 fatti letti su r300 al commit iniziale contro 29 alla fine.
  H1 misura la raggiungibilita' dei fatti letti; cio' che non si legge non genera
  domande. Il numero di fatti per frase va tenuto accanto (H3 lo rende tipato).
- **La correttezza della lettura**: `break(materials, down)` genera domande «ok».
  Un controllo sulla frase sorgente resta da scrivere (§2).
- **Il rumore dello strumento** non e' zero: domande malformate su superfici della
  KB (`also_known_as` → «what do X also known as?», «under threat from» con «is»),
  e la grammatica e' inglese e minima. Le forme migrano in KB come passo seguente.

### 4.4 Conseguenze per il piano

1. **E1 diventa la regressione automatica** di ogni gradino (≈2 minuti per testo,
   in parallelo), accanto al banco scritto a mano: ogni cura deve non aumentare le
   R1 bugie su nessun piolo gia' misurato.
2. **Il prossimo circuito lo sceglie E1, non il supervisore** (H6): la classe con
   piu' occorrenze e' il verso delle cue con particella (R1), poi la morfologia
   della terza persona (T2).
3. **H3 prima di H4**: la perdita di lettura tipata serve a coprire cio' che E1 non
   vede (4.3), ed e' la condizione perche' il ciclo delle cure (H4) abbia lacune
   da chiudere oltre ai muri.
