# Che forma ha una riparazione — lo studio dei fix del gen427-432

> **Stato:** scritto a gen433 (20 agosto 2026), su richiesta di F.: *«lo studio
> dei fix della KB può far emergere dei pattern di alto livello per
> l'autocorrezione».*
> **Metodo:** non introspezione. Sono state contate le righe realmente scritte
> nelle sei generazioni in cui i cento fallimenti sono passati da **32/100 a
> 99/100** — 959 righe di KB su 14 file, 80 predicati distinti, 479 righe di C —
> e ogni riparazione è stata classificata per **forma**, non per argomento.
> **Perché conta:** se le riparazioni hanno poche forme ricorrenti, allora
> l'autocorrezione non deve *inventare* rimedi: deve **riconoscere quale forma
> serve** e generare il candidato di quella forma. È la differenza fra un
> problema aperto e un problema di ingegneria.

## 0. La risposta breve alla domanda di F.

Sì, e i pattern sono **pochi e netti**. Su 507 righe di conoscenza aggiunte
(escludendo i commenti), **sette forme** coprono tutto, e tre di esse sono già
oggi **meccanizzabili senza giudizio umano**.

La cosa che non mi aspettavo, e che cambia il piano: **la classe più numerosa
non è la conoscenza mancante — è la SUPERFICIE mancante.** Cioè non *«non lo
so»*, ma *«non ho riconosciuto che me lo stavi chiedendo»*. Questo sposta il
baricentro dell'autocrescita: la Wikipedia chiude la classe delle *conoscenze*,
che è la terza per numerosità; la classe più grande si chiude **parlando**, o
leggendo prosa per raccogliere **come le cose si dicono** invece di cosa dicono.

## 1. Le sette forme, contate

| # | forma della riparazione | righe | predicati tipici | il fallimento che chiude |
|---|---|---:|---|---|
| **A** | **superficie mancante** — la facoltà c'è, il turno non ci arriva | ~100 | `own_method_cue`, `intent_cue`, `numeric_cue`, `time_cue`, `concept_surface_cue`, `phrase_canon`, `contrast_lead/sep` | *«I don't know about X yet»* su una cosa che sa fare |
| **B** | **fatto mancante** — il consumatore gira e non trova niente | ~45 | `wiki_concept`, `difference_between`, `tr_es`, `unit_symbol`, `grammar_error_correction`, `roman_digit` | risposta parziale o dichiarazione di lacuna |
| **C** | **classe lessicale mancante** — un motore consulta una classe e il token non c'è | ~80 | `content_kind`, `determiner_word`, `clause_copula`, `currency_sign`, `genre_name`, `vowel_letter`, `markable_role` | il motore declina in silenzio |
| **D** | **frase mancante** — la mossa è giusta, manca cosa dire | ~66 | `response_template`, `own_method` | muro, o template fuori bersaglio |
| **E** | **regola/procedura mancante** — un'inferenza nuova | ~50 | `lone_literal_kind`, `relation_check`, `relation_say`, `extract_frame`, `infix_relation`, `relation_noun` | il turno non è calcolabile con quello che c'è |
| **F** | **difetto del motore** — conoscenza dichiarata che non può funzionare | (C) | — | il fatto c'è, e non combacia mai |
| **G** | **guardia di pertinenza** — un modulo prendeva un turno che non era suo | (C) | — | risposta plausibile e sbagliata (*misclaim*) |

**Il rapporto che conta:** 507 righe di conoscenza contro 479 di C — ma il C non
è distribuito come la conoscenza. Quasi tutto sta in **F** e **G** (difetti e
guardie) e in **quattro motori generici** scritti una volta sola
(`p0_unattached_kind`, `mod_lone`, `mod_claim`, `tr_payload_kb`). Nessuna riga di
C è stata scritta per chiudere un singolo prompt: quando è successo, era un
motore che poi ne ha chiusi dieci.

## 2. Il pattern che rende l'autocorrezione possibile

Le forme A, B, C hanno tre proprietà che le altre non hanno, e sono esattamente
le tre che servono a un ciclo automatico:

1. **hanno una firma al momento del fallimento.** Non «il turno è fallito», ma
   *quale interrogazione è tornata vuota*:
   - **A**: nessuna cue ha combaciato, e una facoltà che avrebbe potuto servire il
     turno esiste (il registro delle cue è dichiarato, la relazione è dichiarata);
   - **B**: la cue ha combaciato, il consumatore è partito, e la relazione che
     doveva portare il valore non aveva righe per quell'argomento;
   - **C**: un motore ha consultato una classe unaria e il token del turno non ne
     era membro.
2. **hanno un generatore di candidati.** Per A il candidato è **una sottostringa
   del turno** (è ciò che l'unico ramo di autocorrezione esistente già sa fare);
   per C è **il token stesso**; per B è **un valore da una fonte** — ed è qui che
   la Wikipedia entra, non altrove.
3. **hanno un oracolo interno.** Si aggiunge la riga, **si ripone lo stesso
   turno**, e o passa o non passa. Non serve il substrato della pertinenza (S3)
   che tiene spento l'interruttore oggi: serve solo la ripetizione.

Le altre quattro non ce le hanno, e vanno trattate diversamente:

- **D è la classe pericolosa.** 66 righe di *cosa dire*: è esattamente ciò che un
  generatore libero produrrebbe bene e falsamente. Una frase inventata che suona
  giusta è il *misclaim* — la classe peggiore dei cento, quella che sembra una
  risposta. **Regola operativa: parrot0 può proporsi righe A, B, C da solo; per
  D deve comporre da template già in KB o chiedere.**
- **E richiede astrazione** (vedere che «un verbo transitivo è già un pattern»).
  È il posto dove serve ancora una testa, ed è anche il posto dove il guadagno è
  massimo: una riga di E ha chiuso undici prompt.
- **F e G sono lavoro di manutenzione**, non di crescita — ma vedi §4: F è
  **rilevabile da solo**, ed è la scoperta più utile di questo studio.

## 3. Dove entra la Wikipedia, e dove no

F. è convinto che parrot0 possa crescere da solo *«perché attraverso la wiki può
coprire gli archi mancanti»*. Lo studio dice che ha ragione **per una forma su
tre**, e indica la seconda strada per un'altra.

- **B (fatti): sì, direttamente.** Le 45 righe di questa forma —
  `wiki_concept(epistemic_injustice, …)`, `tr_es(runs, corre)`,
  `unit_symbol(cm, centimetres)` — sono *esattamente* ciò che un corpus statico
  contiene. Qui l'autocrescita è pulita: la lacuna è nominata (manca il valore di
  `R` per l'argomento `X`), la fonte è indicizzabile per nome, e l'oracolo è il
  turno riposto.
- **A (superfici): sì, ma indirettamente — e questa è la parte non ovvia.** Un
  corpus non contiene «come si chiede una cosa», ma contiene **come le cose si
  dicono**, e il gen382 l'ha già dimostrato una volta: le forme
  `extract_frame("@S is known as @O", also_known_as)` sono state raccolte
  **misurando la prosa** («known as» ricorre 15 volte in 49 pagine), non
  immaginandole. Quello è il precedente vivo della raccolta di superfici da
  corpus, ed è la strategia più promettente per la classe più numerosa.
- **C (classi lessicali): parzialmente.** Che «cm» sia un simbolo di unità o che
  «hello» sia un saluto un corpus lo dice; che «trace» sia un *genere di
  contenuto allegabile* no — quello è un fatto sull'**interazione**, non sul
  mondo, e si impara parlando.
- **D (frasi): no, e non deve.** Copiare frasi da un corpus è il frasario nella
  sua forma peggiore.

> **Tre sorgenti per tre lacune.** La prosa dà i fatti (B) e le forme espressive
> (A); il dialogo dà le superfici dell'interazione (A, C) e le mosse; la misura
> dà i difetti (F). Un piano di autocrescita che ne usa una sola copre un terzo
> del problema — ed è probabilmente il motivo per cui finora la crescita è
> sembrata possibile solo supervisionata.

## 4. La scoperta: sette difetti su sette erano CONOSCENZA MORTA

Le sette riparazioni di forma **F** di questa settimana, messe in fila, sono la
stessa cosa sette volte:

| difetto | la conoscenza c'era | e non poteva funzionare perché |
|---|---|---|
| la sterlina | `currency_char("£")` | il confronto era su **un** carattere, «£» ne occupa due |
| i frame al passato | `extract_frame("@S was born in @O")` | la copula veniva riscritta in «is» **prima** che i frame la vedessero |
| i registri delle cue | 18 `turn_cue_registry` dichiarati | il lettore ne prendeva **16** |
| l'induzione | fino a 40 regole indotte | il buffer ne conteneva **16**, e il conteggio non lo diceva |
| il corpo lungo | una regola a 9 goal | il massimo è **8**, e oltre non fallisce: non trova soluzioni |
| `chars/2` sul dollaro | `currency_sign("$")` | il `$` è il marcatore di variabile, quindi la stringa risultava non-ground |
| i frame contro il cablato | i frame KB dichiarati «corrono per primi» | la chiamata stava **sotto** l'estrattore cablato |

> **Un fatto che non combacia non si lamenta.** È la frase che ho scritto tre
> volte in tre commit diversi senza accorgermi che era un *pattern*.

E qui c'è la proposta operativa più concreta di questo studio, perché la
macchineria **esiste già ed è a metà**:

- il lato **statico** c'è: `kb_dead_rules/4` pubblica `dead_rule/2` e
  `inert_rule/1` — le regole il cui corpo nomina predicati che nessuno produce;
- il lato **dinamico** manca, ed è quello che avrebbe preso tutti e sette:
  l'**impronta di inferenza** (gen422) registra a ogni turno quali predicati sono
  stati toccati. Aggregata su un corpus — i cento, le classi misurate, la suite —
  dà l'insieme dei predicati **dichiarati e mai toccati**, e per i predicati
  toccati si può scendere alla **riga**: quale fatto ha effettivamente unificato.

**La domanda autoriflessa che ne esce è precisa e verificabile:**

> *«Quali cose che dico di sapere non hanno mai fatto niente?»*

Non è una metafora: è una `findall` sopra l'aggregato delle impronte. E ogni
risposta è o un difetto del motore (i sette qui sopra), o conoscenza da
ritirare — nei due casi, una lacuna che parrot0 ha trovato **da solo, senza che
nessuno gli ponga un prompt**.

## 5. Il secondo segnale che nessuno stava leggendo: la misura come sensore

I cento e le classi misurate sono nati come *voti*. Lo studio dice che sono
**sensori**, e che il loro output è già nella forma che l'autocorrezione vuole:

- `make hundred -v` produce, per ogni prompt fallito, la terna **(turno, attesa,
  risposta di oggi)**. È precisamente un `gap` tipato, con l'oracolo incluso;
- `make measure` produce, per ogni classe, la **firma** dell'inferenza: due turni
  con la stessa firma e risposte diverse sono un difetto di *distinzione*, due
  turni diversi con la stessa firma **e la stessa risposta** sono una facoltà
  che non guarda ciò che le viene dato. Il gen426 ha chiuso sei prompt partendo
  esattamente da questo segnale, e nessuno l'aveva chiesto.

> Un fallimento con un'attesa accanto è già una lacuna nominata. Il corpus dei
> cento non è la prova finale dell'autocorrezione: è il suo **primo campo**.

## 6. Le strategie da mettere in parallelo (e come si riconosce quella che vince)

Dallo studio escono quattro cicli indipendenti, tutti alimentabili subito, tutti
con lo stesso oracolo (**riponi il turno**) e lo stesso gate di promozione
(sessione → quarantena → KB ufficiale, con provenienza e revoca):

1. **Ciclo-superficie (A).** Segnale: nessuna cue combacia ma una facoltà
   dichiarata potrebbe servire il turno. Candidato: una sottostringa del turno.
   *È l'unico che già esiste* — e chiude la classe più numerosa. Estensione
   naturale: raccogliere superfici **dalla prosa** come il gen382.
2. **Ciclo-fatto (B).** Segnale: consumatore partito, relazione vuota per quel
   soggetto. Candidato: dal corpus statico, indicizzato per nome. È il ciclo che
   F. ha in mente, ed è il più sicuro perché il candidato è *citabile*.
3. **Ciclo-classe (C).** Segnale: un motore ha chiesto a una classe unaria e il
   token non c'era. Candidato: il token. Il più economico dei tre, e quello con
   il rischio più alto di falsi positivi: vuole l'ablazione (togli la riga, il
   turno torna a fallire? allora serviva).
4. **Ciclo-audit (F).** Segnale: predicati e righe mai toccati dall'impronta su
   un corpus. Nessun candidato da generare: è una **domanda su di sé** che
   produce o una riparazione del motore o una ritrattazione.

**Come si riconosce quella che vince**: non dal numero di righe che aggiunge, ma
dal **saldo sul corpus**. Ogni ciclo propone, la promozione passa dal riporre il
turno *e* dal rigirare i cento e le classi misurate: una riga che chiude un
prompt e ne rompe un altro non entra. È il cricchetto che il progetto ha già —
`make test`, `make hundred`, `make measure` — usato come giudice invece che come
pagella.

## 7. Il limite onesto di questo studio

Le forme sono state estratte da **una** campagna di riparazione, fatta da **un**
riparatore, su **un** corpus di cento prompt scelti nel 2026. È il campione che
c'è, e va detto che è di parte: se le riparazioni hanno poche forme, può essere
perché il problema le ha, o perché chi riparava ne conosceva poche. Il modo di
smentirlo è ripetere il conteggio sulla prossima campagna — la `rl-suite`, o i
cento rifatti su prompt nuovi — e vedere se la distribuzione regge.

E un secondo limite, che vale come avvertimento per il piano: **la forma D esiste
e pesa 66 righe.** Un ciclo automatico che non sa dire *«questa lacuna non è mia
da chiudere»* comincerà, prima o poi, a scrivere frasi plausibili. La misura di
sicurezza non è un filtro sulle parole: è che **D non abbia un generatore**.

## Riferimenti

- `docs/plans/autocrescita.md` — il piano che usa questo studio: la mappa
  sorgente → forma → fonte di §7 esce da qui
- `docs/autocorrezione.md` — la teoria, e §13 l'autocorrezione fatta in due
- `docs/plans/parrot0-100-failures.md` — il campo di prova, e `make hundred`
- `docs/measured-classes.md` — la firma dell'inferenza come sensore
- `docs/plans/teach-comprehension-via-prompt.md` — l'inventario delle superfici
- `docs/plans/question-emergence.md` — il ciclo esistente (forma A)
- `docs/plans/the-linguistic-glue.md` — la colla che tiene insieme A e C
