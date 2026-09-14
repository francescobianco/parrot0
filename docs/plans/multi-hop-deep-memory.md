# Missione Multi-Hop Deep Memory

> **In una riga.** parrot0 risolve un compito di ragionamento la cui risposta non sta
> né nel prompt né in una sola pagina: scopre che cosa gli manca, lo legge dalla sua
> memoria profonda (Wikipedia), lo integra, e da lì capisce che cosa leggere dopo.
> Nessun LLM, nessun motore di ricerca esterno, nessun aiuto nel prompt: soltanto la
> KB viva, la lettura e l'inferenza. Ogni passo è ispezionabile e porta la sua fonte.

Indice: [0. Stato](#0-stato-misurato-14-settembre-2026) ·
[1. Premessa](#1-premessa-che-cosè-la-memoria-profonda) ·
[2. Missione](#2-la-missione) · [3. Regole anti-inganno](#3-regole-anti-inganno) ·
[4. Architettura](#4-architettura-kb-first-che-cosa-cè-che-cosa-manca) ·
[5. Progressione](#5-progressione-dm-1--dm-10) · [6. Esperimenti](#6-esperimenti) ·
[7. Traccia](#7-che-cosa-si-registra-per-ogni-hop) · [8. Metriche](#8-metriche) ·
[9. La demo](#9-la-demo) · [10. Criterio finale](#10-criterio-finale) ·
[11. Registro](#11-registro-delle-sessioni)

---

## 0. Stato misurato (14 settembre 2026)

Misurato in `make chat` con politica «look things up yourself» e sulle pagine vere in
edizione locale (`tests/fixtures/wiki/`, lead di Wikipedia con revisione).

| Prova | Risposta di oggi | Diagnosi |
|---|---|---|
| Prompt a passi di §6.2 (Karamazov → … → religione) | «A word that starts with "t": tab. … a close reading turns on the premise…» | **misclaim**: il compito a passi arriva a facoltà che non lo capiscono |
| Prompt della demo §6.1 (Zarathustra → … → divinità) | «Thus was a mysterious Thus. Then one day…» | **misclaim**: il generatore di storie prende il turno |
| «who wrote the brothers karamazov?» | «fyodor dostoevsky.» | hop 0 nella KB: sa |
| «what is thus spoke zarathustra?» | legge la pagina e dice il lead | la lettura c'è |
| «who is the protagonist of thus spoke zarathustra?» | «I don't know about protagonist.» | la relazione non si estrae dal testo letto |
| «what did zoroaster found?» | ripete la definizione letta | idem: «founder of Zoroastrianism» è nel lead e non diventa `founded/2` |
| «which deity do zoroastrians worship?» | lacuna su «zoroastrians» | nome del gruppo non collegato al topic |
| «who wrote thus spoke zarathustra?» | lacuna su «zarathustra» | la lacuna nomina una parola, non il titolo |

**Dove sono scritti gli anelli** (lead veri, revisioni del 14 settembre):

| Anello | Nel lead? | Frase |
|---|---|---|
| The Brothers Karamazov → Dostoevsky | sì | «…final novel by Russian author Fyodor Dostoevsky» |
| Dostoevsky → Nietzsche | **no** | solo nel corpo di *Friedrich Nietzsche*, e nel verso opposto: «Nietzsche called Dostoevsky "the only psychologist from whom I have anything to learn"» |
| Nietzsche → Thus Spoke Zarathustra | no (nel lead del libro, verso opposto) | «…written by German philosopher Friedrich Nietzsche» |
| Thus Spoke Zarathustra → Zoroaster | sì | «The protagonist is nominally the historical Zarathustra, more commonly called Zoroaster» |
| Zoroaster → Zoroastrianism | sì, **con distrattori** | «…the spiritual founder of Zoroastrianism» — nello stesso lead: Ancient Iranian religion, Judaism, Christianity, Islam |
| Zoroastrianism → Ahura Mazda | sì | «…deity known as Ahura Mazda, who is hailed as the supreme being» |

Conclusione operativa: la catena di §6.1 è percorribile **con i soli lead** e ha una
biforcazione vera (quale religione); la catena di §6.2 richiede due capacità in più:
leggere oltre il lead e cercare all'indietro («chi cita Dostoevskij?»).

---

## 1. Premessa: che cos'è la memoria profonda

parrot0 dispone di una **memoria profonda**: esterna, persistente, non parametrica, il cui
corpus è Wikipedia. Non è un information retrieval messo accanto al sistema: fornisce la
conoscenza che manca **nel momento in cui serve per proseguire un ragionamento**
(`kb/core/network.p0`, [la-rete-come-memoria-profonda](la-rete-come-memoria-profonda.md)).

Il ciclo fondamentale:

**reason → knowledge gap → recall → evidence → integrate → reason**

Tre spazi, distinti:

| Spazio | Che cosa | Dove vive oggi |
|---|---|---|
| **KB** | ciò che parrot0 conosce operativamente | `kb/core`, `kb/facts`, `kb/wiki`, `kb/learning` |
| **Working memory** | ciò che tiene durante il problema corrente | fatti di scratch del turno e del problema (`turn_scratch/1`, i fatti numerati di `problem-texts.p0`) |
| **Memoria profonda** | ciò che può leggere da Wikipedia quando serve | `topic_read/2`, `topic_definition/2` (`kb/wiki/deep-memory.p0`) |

E cinque statuti epistemici, che ogni informazione della catena deve portare:

| Statuto | Significato | Come si dice oggi |
|---|---|---|
| **known** | nella KB prima del problema | fatto della base curata |
| **given** | detto dal problema | premessa (`problem_premises`, non si impara) |
| **inferred** | ottenuto per inferenza | «The conclusion is mine» (`derivation_answer/3`) |
| **recalled** | letto dalla memoria profonda | «I read it on Wikipedia: the article …, revision …» (`source_answer/3`) |
| **unknown** | non ancora disponibile | lacuna nominata (`turn_gap_phrase/2`, `pending_gap`) |

---

## 2. La missione

Dimostrare che parrot0 risolve problemi la cui soluzione **non è disponibile né nel prompt
né attraverso un singolo accesso a Wikipedia**, ma emerge da una sequenza di accessi
intercalati da ragionamento.

Non vogliamo dimostrare che parrot0 sa fare dieci ricerche. Vogliamo dimostrare che:

> **parrot0 risolve autonomamente un problema che richiede una catena di N accessi alla
> memoria profonda, individuando progressivamente che cosa deve sapere, recuperando
> l'evidenza, integrandola nella propria rappresentazione e usandola per decidere il
> passo successivo.**

Il comportamento desiderato:

```
Problema → ragionamento → lacuna₁ → lettura₁ → evidenza₁ → aggiornamento della IR
        → ragionamento → lacuna₂ → lettura₂ → evidenza₂ → … → inferenza finale → risposta
```

Ogni risultato intermedio deve poter diventare parte della rappresentazione con cui si
formula il bisogno successivo: l'entità trovata al passo *k* è il soggetto della lettura
al passo *k+1*.

---

## 3. Regole anti-inganno

Valgono i MANTRA, e in più per questa missione:

1. **Il prompt non dice come.** Niente «use your Deep Memory», niente «this requires 5
   hops», niente titoli di pagina da aprire. Il numero di hop è una proprietà nascosta,
   osservata nella valutazione.
2. **Le pagine sono vere.** Le prove deterministiche usano i lead reali di Wikipedia in
   edizione locale (`tests/fixtures/wiki/<topic>.txt`), mai testo scritto per il test.
   La demo dal vivo usa la rete. Stesso motore, stessa KB: cambia solo il provider.
3. **Nessun anello nella KB prima del test.** Se un anello è già un fatto della base, quel
   hop non conta come lettura (si registra come *known*). Le prove partono dalla KB viva
   completa, mai amputata.
4. **Nessun lettore per il problema.** Una relazione che serve a un hop («founder of»,
   «named after», «supreme being») è conoscenza generale del lettore di prosa, valida per
   ogni pagina, e si insegna parlando. Un pattern che vale solo per Zoroastro vale zero.
5. **Una risposta giusta con un passo sbagliato è un fallimento.** Si valuta la catena,
   non la parola finale (HCR, §8).
6. **Nessun «Imparato» dal prompt.** Le frasi del problema sono premesse, non conoscenza
   (la lezione dell'[esperimento età e incontri](../sessions/2026-09-14-esperimento-ordine-eta.md)).

---

## 4. Architettura KB-first: che cosa c'è, che cosa manca

### 4.1 Che cosa c'è già (e va riusato, non riscritto)

| Capacità | Dove | Serve a |
|---|---|---|
| Decidere di leggere (ask/act), leggere, ricordare con indirizzo e revisione | `network.p0`, `50-self-research-loop.c`, `deep-memory.p0` | ogni hop |
| Non leggere mentre si legge (`reading_now`) | `network.p0` | evitare letture a cascata non pertinenti |
| La lacuna nomina il sintagma, non una parola | `turn_gap_phrase/2` (grammar.p0) | formulare la query giusta |
| Definizione letta come risposta; «where did you read that?» | `mod_knowledge`, `source-questions.p0` | statuto *recalled* |
| Catene e premesse con provenienza, attenuazione | `causal-questions.p0` | statuto *inferred*, traccia |
| Testo con compito: premesse non imparate, lettura a stadi, risposta in parti | `problem-texts.p0`, `turn_response_part/3` | tenere il problema nella working memory e rispondere in più righe |
| Conversa di una relazione con particella («result from» → `causes`) | `converse_particle_reading/3` | leggere relazioni nel verso giusto |
| Determinatezza (necessario / possibile / indeterminato) | `order-determinacy.p0` | tenere aperte le ipotesi nel branching |

### 4.2 Che cosa manca, in ordine di dipendenza

| # | Capacità | Perché serve | Forma KB-first |
|---|---|---|---|
| M1 | **Il compito a catena come atto**: riconoscere un prompt che chiede una risposta attraverso riferimenti in sequenza («that figure», «its followers», «named after a …») | oggi va a storie e letture ravvicinate | `turn_declared_act(_, chain_task)` sulle anafore e sui nomi di tipo, non su cue del prompt |
| M2 | **Il lead intero nella working memory**, con le maiuscole e le frasi | oggi si tiene solo la prima frase (`topic_definition`) e i fatti estratti | `topic_sentence(Topic, N, "…")` di scratch, dalla stessa lettura |
| M3 | **Menzioni come candidati**: le entità nominate in una frase del lead | è il ponte fra una pagina e la successiva | regola sulle frasi: sintagmi nominali propri e topic noti |
| M4 | **Relazione dell'hop dalla frase**: quale menzione sta nel ruolo chiesto («founder of X», «named after X», «known as X … supreme being») | la risposta di un hop | superfici di relazione come fatti (`relation_surface/2`), insegnabili parlando; riuso di `extract_frame/2` |
| M5 | **Verifica di tipo leggendo il candidato**: «a religious figure» → il lead del candidato dice «religious reformer» | scegliere fra candidati, e non accettare un nome solo perché compare | lettura del candidato + classe letta (`class_reading`) |
| M6 | **Branching**: tenere più candidati, scartarli con evidenza, non promuovere un candidato a fatto | il lead di Zoroaster nomina cinque religioni | ipotesi di scratch + `order-determinacy`-like per l'esclusione |
| M7 | **La traccia dell'hop** (§7) come fatti, e la risposta che la ricostruisce | «reconstruct every step» | `hop_trace/3` numerato per problema + `turn_response_part/3` |
| M8 | **Oltre il lead**: sezioni della pagina | Dostoevsky → Nietzsche sta nel corpo | provider: sezioni; lettura a finestre come in `problem-texts.p0` |
| M9 | **Ricerca all'indietro**: «quale pagina cita X in questo ruolo?» | «a philosopher who treated this author as a precursor» | provider di ricerca (esiste `.options.txt`), filtrato per tipo (M5) |

⚠ Lezioni dall'esperimento del 14 settembre che valgono qui: le letture composte in KB
si fanno **a stadi** (limite dei 384 legami), nessun `retract` a metà risoluzione, i fatti
del problema si **numerano**, `findall/3` raccoglie solo variabili, liste ≤ 512 byte
(`C_TODO.md`, «Limiti silenziosi del risolutore»).

---

## 5. Progressione DM-1 → DM-10

Il numero è la profondità **minima** della catena di letture necessarie.

| Livello | Che cosa dimostra | Capacità richieste |
|---|---|---|
| **DM-1** | una lacuna, una lettura, una relazione estratta dal lead | M2, M4 |
| **DM-3** | catena lineare su lead, relazioni eterogenee, una biforcazione | M1–M7 |
| **DM-5** | un anello fuori dal lead e uno all'indietro | + M8, M9 |
| **DM-7** | domini diversi (letteratura, filosofia, religione, storia) e più biforcazioni | stabilità di M6 |
| **DM-10** | catena lunga con relazioni tutte diverse, traccia completa | tutto, a costo del turno controllato |

Gli hop devono rappresentare **relazioni semanticamente diverse**, non
`persona → città → regione → stato → capitale`. Per esempio:
opera → autore → influenza intellettuale → filosofo → opera → personaggio → figura storica →
dottrina → testo → concetto.

### Lineare e ramificata

- **Lineare** — ogni hop ha un solo successore: `A → B → C → D`. Misura la profondità.
- **Ramificata** — un hop produce candidati: `A → B → {C₁, C₂}`, e un'evidenza successiva
  ne elimina alcuni: `{C₁, C₂} → evidenza → C₂`. Misura la capacità di **tenere
  l'incertezza senza trasformare un candidato in un fatto**: è il punto in cui parrot0 si
  distingue da un sistema che indovina.

---

## 6. Esperimenti

### 6.1 E1 — DM-3, la demo: il nome del profeta di Nietzsche  *(primo test, in corso)*

**Prompt** (unico, niente istruzioni sul come):

> The protagonist of Thus Spoke Zarathustra is named after a historical religious figure.
> Which religion is traditionally associated with that figure, and which deity do its
> followers exalt as the supreme being? Reconstruct every step.

**Catena attesa** (non data a parrot0):

| Hop | Lacuna | Lettura | Evidenza (frase vera) | Relazione | Risultato |
|---|---|---|---|---|---|
| 1 | di chi porta il nome il protagonista? | *Thus Spoke Zarathustra* | «The protagonist is nominally the historical Zarathustra, more commonly called Zoroaster» | named_after | Zoroaster (candidati: Zarathustra = Zoroaster, West) |
| 1b | è una figura religiosa? | *Zoroaster* | «…was an Iranian religious reformer…» | verifica di tipo | sì |
| 2 | quale religione è associata a lui? | *Zoroaster* | «…becoming the spiritual founder of Zoroastrianism» | founder_of | Zoroastrianism — **scartate**: Ancient Iranian religion (la sfidò), Judaism, Christianity, Islam (influenza «with much scholarly controversy») |
| 3 | quale divinità esaltano i suoi fedeli? | *Zoroastrianism* | «…its adherents exalt an uncreated … deity known as Ahura Mazda, who is hailed as the supreme being» | supreme_deity | **Ahura Mazda** |

**Risposta attesa**: Zoroastrianism e Ahura Mazda, con i tre articoli letti e le loro
revisioni, lo statuto di ogni passo (*given* il nome del libro, *recalled* le tre frasi,
*inferred* la scelta fra i candidati) e i candidati scartati con il perché.

**Perché lascia il segno**: tre pagine lette in sequenza, ciascuna scelta da ciò che la
precedente ha detto; una biforcazione vera risolta con l'evidenza e dichiarata; nessun
modello linguistico; ogni frase citata con la revisione. E si può chiedere a ogni passo
«where did you read that?».

**Prova**: `tests/p0t/crossing/dm3_zarathustra_chain.p0t` — pagine vere in edizione
locale. **Oggi è rossa**, e deve esserlo: rende visibili M1–M7.

### 6.2 E2 — DM-5: da Karamazov alla religione

> Begin with the author of *The Brothers Karamazov*. Identify a philosopher who treated
> this author as an important precursor to his own thought. Determine which major
> philosophical work by that philosopher introduces the figure of a prophet who announces
> the death of God. Identify the historical religious figure from whom the prophet takes
> his name. Finally, determine the ancient religion traditionally associated with that
> figure. Return the religion and reconstruct every step that led you to it.

Catena: The Brothers Karamazov → Fyodor Dostoevsky *(known)* → Friedrich Nietzsche *(M8+M9:
corpo della voce, verso opposto)* → Thus Spoke Zarathustra *(verso opposto: il libro nomina
l'autore)* → Zoroaster → Zoroastrianism. Hop semanticamente diversi: work → author, author →
influenza intellettuale, philosopher → work, figura letteraria → figura storica, figura →
tradizione. La relazione Dostoevskij–Nietzsche non è un attributo: è evidenza testuale da
interpretare («the only psychologist from whom I have anything to learn»).

### 6.3 E3 e oltre — DM-7, DM-10

Da costruire con lo stesso metodo: prima si verificano **dove** sono scritti gli anelli
(tabella di §0), poi si scrive il prompt. Candidati di dominio diverso: storia della
scienza (strumento → inventore → città → università → allievo), musica (opera → libretto →
fonte letteraria → autore → movimento). Nessun prompt si scrive prima di aver letto le
pagine vere.

---

## 7. Che cosa si registra per ogni hop

Per ogni esecuzione si deve poter ricostruire, come fatti ispezionabili del problema
(`hop_trace(Problema, K, …)`, M7):

1. **Lacuna** — quale informazione parrot0 ritiene mancante.
2. **Query** — che cosa decide di leggere, e perché quella pagina.
3. **Evidenza** — la frase letta, con articolo, edizione e revisione.
4. **Aggiornamento della IR** — come quella frase diventa rappresentazione (relazione,
   candidati).
5. **Inferenza** — quale conclusione diventa possibile; quali candidati cadono e perché.
6. **Prossimo obiettivo** — perché quella conclusione genera il bisogno successivo.
7. **Risposta** — la conclusione e la catena di evidenze che la sostiene.

Una risposta corretta ottenuta per caso non completa la missione.

---

## 8. Metriche

| Metrica | Definizione | Che cosa punisce |
|---|---|---|
| **DMHR(n)** — Deep Memory Hop Rate | problemi risolti con almeno *n* hop necessari / problemi di profondità *n* | la profondità che non regge |
| **HCR** — Hop Correctness Rate | hop corretti e giustificati / hop eseguiti | la risposta giusta per la strada sbagliata |
| **ARR** — Autonomous Recall Rate | letture attivate da una lacuna reale / letture totali, senza suggerimenti nel prompt | leggere a tappeto, o leggere perché il prompt lo dice |
| **BRR** — Branch Resolution Rate | biforcazioni risolte con evidenza citata / biforcazioni incontrate | promuovere un candidato a fatto senza prova |
| **Costo** | tempo del turno e letture per problema | una demo che non si può fare dal vivo |

Il banco (`tests/comprehension-probe/`-like, da creare in `tests/dm/`) tiene un file per
livello con prompt, pagine locali, catena attesa e punteggio per hop.

---

## 9. La demo

**Obiettivo**: far vedere in pochi minuti che parrot0 **non sa la risposta, capisce che cosa
gli manca, lo va a leggere, e ragiona su ciò che ha letto**, dicendo da dove viene ogni
passo.

Copione (dal vivo, `make chat`, rete accesa):

1. «what is Ahura Mazda?» → non lo sa (e lo dice): la risposta non è nella KB.
2. Il prompt di E1 → la risposta con la catena, le tre letture, i candidati scartati.
3. «where did you read that?» → articolo e revisione dell'ultimo anello.
4. «why not Christianity?» → la frase che la cita e perché non è l'associazione chiesta.
5. Lo stesso prompt con un'altra opera e un altro personaggio (variante del banco) → per
   mostrare che non è un copione.

**Piano B senza rete**: la stessa sessione con `topic_provider(fixture, …)` sulle pagine
vere salvate con la revisione; lo si dice al pubblico.

---

## 10. Criterio finale

Il traguardo **DM-10** non è avere fatto dieci letture. È che parrot0, partendo soltanto
dal problema, produca da solo

**reason → discover missing knowledge → recall from Wikipedia → validate evidence → update
representation → infer → discover next missing knowledge → … → final solution**

per una catena di almeno **10 hop necessari**, con la provenienza di ogni informazione
tracciabile.

> **parrot0 can autonomously traverse a ten-hop chain of heterogeneous knowledge
> dependencies using Wikipedia as Deep Memory, while preserving an explicit and inspectable
> reasoning state between successive recalls.**

A quel punto la memoria profonda non è «Wikipedia accessibile a parrot0»: è parte del
ciclo cognitivo. **parrot0 non deve possedere tutta la conoscenza prima di ragionare: deve
scoprire che cosa gli manca mentre ragiona, recuperarlo e continuare.**

---

## 11. Registro delle sessioni

- **14 settembre 2026** — piano riordinato ed espanso; stato misurato (§0); pagine vere dei
  sei anelli salvate in `tests/fixtures/wiki/`; primo test E1 scritto e rosso
  (`tests/p0t/crossing/dm3_zarathustra_chain.p0t`). Prossima mossa: M1 + M2 (il compito a
  catena non va più a storie; il lead intero nella working memory), poi M4 sull'hop 2, che
  è il primo anello leggibile con una relazione già a portata del lettore di prosa.
