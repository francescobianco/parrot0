# Missione Multi-Hop Deep Memory

> **In una riga.** parrot0 risolve un compito di ragionamento la cui risposta non sta
> né nel prompt né in una sola pagina: scopre che cosa gli manca, lo legge dalla sua
> memoria profonda (Wikipedia), lo integra, e da lì capisce che cosa leggere dopo.
> Nessun LLM, nessun motore di ricerca esterno, nessun aiuto nel prompt: soltanto la
> KB viva, la lettura e l'inferenza. Ogni passo è ispezionabile e porta la sua fonte.
>
> **Il luogo in cui tutto questo accade è la IR.** Il problema, ogni pagina letta, ogni
> lacuna, ogni candidato e ogni conclusione sono nodi e frame della stessa
> rappresentazione intermedia (`kb/core/input-structure.p0`), in scope diversi. Un hop non
> è «una ricerca»: è **un ruolo mancante in un frame della IR che una lettura riempie**.

Indice: [0. Stato](#0-stato-misurato-14-settembre-2026) ·
[1. Premessa](#1-premessa-che-cosè-la-memoria-profonda) ·
[2. Missione](#2-la-missione) · [3. Regole anti-inganno](#3-regole-anti-inganno) ·
[4. La IR è il ciclo](#4-la-ir-è-il-ciclo) ·
[5. Architettura](#5-architettura-kb-first-che-cosa-cè-che-cosa-manca) ·
[6. Progressione](#6-progressione-dm-1--dm-10) · [7. Esperimenti](#7-esperimenti) ·
[8. Traccia](#8-che-cosa-si-registra-per-ogni-hop) · [9. Metriche](#9-metriche) ·
[10. La demo](#10-la-demo) · [11. Criterio finale](#11-criterio-finale) ·
[12. Registro](#12-registro-delle-sessioni)

---

## 0. Stato misurato (14 settembre 2026)

Misurato in `make chat` con politica «look things up yourself» e sulle pagine vere in
edizione locale (`tests/fixtures/wiki/`, lead di Wikipedia con revisione).

| Prova | Risposta di oggi | Diagnosi |
|---|---|---|
| Prompt a passi di §7.2 (Karamazov → … → religione) | «A word that starts with "t": tab. … a close reading turns on the premise…» | **misclaim**: il compito a passi arriva a facoltà che non lo capiscono |
| Prompt della demo §7.1 (Zarathustra → … → divinità) | «Thus was a mysterious Thus. Then one day…» | **misclaim**: il generatore di storie prende il turno |
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

Conclusione operativa: la catena di §7.1 è percorribile **con i soli lead** e ha una
biforcazione vera (quale religione); la catena di §7.2 richiede due capacità in più:
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
| **Working memory** | ciò che tiene durante il problema corrente | **la IR**: lo scope del problema e uno scope per ogni pagina letta, con i legami fra scope (§4) |
| **Memoria profonda** | ciò che può leggere da Wikipedia quando serve | la pagina, finché non è letta; letta, diventa uno scope della IR. In KB resta ciò che si conserva: `topic_read/2`, `topic_definition/2` (`kb/wiki/deep-memory.p0`) |

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
   non la parola finale (HCR, §9).
6. **Nessun «Imparato» dal prompt.** Le frasi del problema sono premesse, non conoscenza
   (la lezione dell'[esperimento età e incontri](../sessions/2026-09-14-esperimento-ordine-eta.md)).

---

## 4. La IR è il ciclo

### 4.1 Perché la IR non è un dettaglio

La tesi di F. del 12 settembre ([lettura-della-prosa](lettura-della-prosa.md) §0) vale qui
più che altrove: **una prosa si incamera una volta, in una rappresentazione che non sa a
che cosa servirà, e chi la lavora ne è un consumatore, non un secondo lettore della
stringa.** Un problema multi-hop è fatto di prose — il prompt e N pagine — che devono
**parlarsi**: l'entità trovata in una pagina è il soggetto della domanda alla pagina dopo.
Se ogni hop avesse il suo lettore, la catena sarebbe una sequenza di stringhe passate di
mano; con la IR è **una sola rappresentazione che cresce**.

Due prove dal 14 settembre che senza la IR si sbaglia:

- la base della demo («Thus was a mysterious Thus. Then one day…») è la stessa malattia
  M4 di `lettura-della-prosa.md` («Quipu was a mysterious Quipu…»), la cui diagnosi era:
  *nessun lettore ha consultato l'IR*;
- l'[esperimento età e incontri](../sessions/2026-09-14-esperimento-ordine-eta.md) ha
  funzionato con un lettore per posizioni di token parallelo alla IR, e §8 del rapporto
  misura il prezzo: tre varianti minime (un verbo, una dimensione, tre nomi) falliscono.

Una prima stesura di questo piano ripeteva l'errore — predicati ad hoc per le frasi del
lead, per le menzioni e per la traccia. Questa sezione lo corregge: **ogni capacità di §5 è
un consumatore o un produttore della IR, e nient'altro.**

### 4.2 Il ciclo, detto in IR

| Momento del ciclo | Che cosa è nella IR | Pezzi che esistono già |
|---|---|---|
| **Problema** | lo scope `problem` con i suoi nodi: entità nominate, descrizioni («a historical religious figure»), anafore («that figure», «its followers») | `input_node/4`, `input_entity_node`, `input_class_node`, `input_node_unresolved` |
| **Lacuna** | un frame con un ruolo **mancante**: `question, binary(named_after), roles(subject(protagonist_of(tsz)), object(missing))` | `input_semantic_frame(…, object(missing))`, `input_frame_gap`, `input_gap_node` |
| **Query** | la lettura si sceglie dal frame: il ruolo **legato** nomina la pagina (il soggetto noto), il ruolo **mancante** dice che cosa cercare in essa | `turn_gap_phrase/2`, `acquisition_move/1` (network.p0) |
| **Lettura** | la pagina diventa uno **scope** della IR (`page(zoroaster)`), segmentata in frasi e nodi con la stessa gerarchia del turno | `input_structure_publish(…, "last_text", …)` usato oggi per le frasi del turno composto |
| **Evidenza** | un frame **assertivo** nello scope della pagina che ha la stessa relazione e un soggetto coreferente: `assertion, binary(founder_of), roles(subject(zoroaster), object(zoroastrianism))` | `input_semantic_frame(…, assertion, …)`, `input_binary_assertion` |
| **Integrazione** | il ruolo mancante del frame del problema si **lega** al nodo della pagina: un legame fra scope, con la provenienza del nodo (articolo, revisione, frase, range) | `input_node_range`, `fact_source/3` — manca il legame cross-scope |
| **Candidati** | più nodi dello scope pagina compatibili col ruolo: restano **alternative** del legame, non fatti | `input_frame_set` / `input_frame_unique`, ambiguità dicibile (universal-input, «Quale? …») |
| **Prossima lacuna** | l'anafora del problema («that figure») ora si risolve al nodo legato: il frame successivo ha il soggetto e diventa interrogabile | spazio del discorso (`discourse_referent/2`), coreferenza |
| **Risposta e traccia** | il cammino dei legami, dal problema all'ultimo scope, reso in parti | `turn_response_part/3`, `source_answer/3`, `derivation_answer/3` |

**Conseguenza**: i sette campi di §8 non sono un registro da scrivere accanto al
ragionamento. **Sono una vista della IR.** Se il ragionamento è avvenuto nella IR, la
traccia si interroga; se bisogna scriverla a mano, il ragionamento è avvenuto altrove.

### 4.3 Che cosa manca alla IR per reggere il ciclo

| # | Mancanza nella IR | Perché blocca | Direzione |
|---|---|---|---|
| IR1 | **Scope di pagina**: una pagina letta non è pubblicata nella IR, se ne tengono la prima frase e i fatti estratti | l'evidenza non ha nodi, range, frasi a cui legarsi | la lettura pubblica il lead (poi le sezioni) in `page(Topic)` con lo stesso `input_structure_publish` delle frasi del turno |
| IR2 | **Legami fra scope**: un nodo del problema e un nodo di una pagina che sono la stessa cosa | senza, l'integrazione è una copia di stringa e la provenienza si perde | `input_bound(ScopeA, NodeA, ScopeB, NodeB, Evidence)` (arietà da impacchettare) come vista, prodotta dalla coreferenza |
| IR3 | **Frame con descrizioni come ruoli**: «the protagonist of X», «a historical religious figure», «its followers» | la lacuna del problema non è un'entità nominata ma una descrizione | le descrizioni come termini (la lezione di `problem-texts.p0`: «chiunque Bruno abbia incontrato…» come referente), dentro la IR |
| IR4 | **Scope persistente per il problema**: la IR di oggi è di turno (`turn_scoped/2`) | un problema multi-hop dura più letture, e forse più turni | scope numerati per problema, senza `retract` a metà risoluzione (limite noto) |
| IR5 | **Frame assertivi dalla prosa vera**: le relazioni del lettore (`extract_frame/2`) non tornano come frame della IR con i nodi | «spiritual founder of Zoroastrianism» diventa (quando diventa) un fatto, non un frame con range | unificare `P0FrameReading` e `input_semantic_frame`: una lettura, due usi (commit in KB, o frame nello scope) |
| IR6 | **Alternative**: più legami candidati per un ruolo, con evidenza per ciascuno | il branching senza promuovere un candidato a fatto | `input_frame_set` esteso ai legami; esclusione per evidenza (come `order-determinacy.p0`, ma sui legami) |

Il test di generalità di questa sezione è quello di `lettura-della-prosa.md`: **una forma
nuova di hop deve costare un consumatore della IR, mai uno scanner nuovo.**

---

## 5. Architettura KB-first: che cosa c'è, che cosa manca

### 5.1 Che cosa c'è già (e va riusato, non riscritto)

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

### 5.2 Che cosa manca, in ordine di dipendenza

Ogni riga è un produttore o un consumatore della IR (§4.3 dice che cosa manca alla IR stessa).

| # | Capacità | Perché serve | Nella IR |
|---|---|---|---|
| M1 | **Il compito a catena come atto** | oggi il prompt va a storie e letture ravvicinate | una forza del turno letta dallo scope `problem`: frame con ruoli `missing` collegati da anafore (IR3), non cue del prompt |
| M2 | **La pagina letta come scope** | oggi si tiene la prima frase e i fatti estratti | **IR1**: il lead pubblicato in `page(Topic)`, frasi e nodi con range |
| M3 | **Menzioni come candidati** | è il ponte fra una pagina e la successiva | nodi entità dello scope pagina (`input_entity_node`), non una regola sulle stringhe |
| M4 | **Relazione dell'hop dalla frase** | la risposta di un hop | **IR5**: frame assertivi dello scope pagina con la relazione del frame lacuna; superfici di relazione insegnabili parlando (`extract_frame/2`, `converse_particle_reading/3`) |
| M5 | **Verifica di tipo leggendo il candidato** | scegliere fra candidati, e non accettare un nome solo perché compare | la descrizione del ruolo (IR3) contro il frame di classe dello scope pagina del candidato |
| M6 | **Branching** | il lead di Zoroaster nomina cinque religioni | **IR6**: legami alternativi con evidenza, esclusione per evidenza |
| M7 | **La traccia e la risposta** | «reconstruct every step» | **vista** sui legami fra scope (IR2), resa con `turn_response_part/3` — nessun registro scritto a parte |
| M8 | **Oltre il lead** | Dostoevsky → Nietzsche sta nel corpo | più scope per pagina (sezioni), stessa pubblicazione |
| M9 | **Ricerca all'indietro** | «a philosopher who treated this author as a precursor» | la query è il frame con il **soggetto** mancante; il provider di ricerca restituisce pagine candidate, che diventano scope da verificare (M5) |

⚠ Lezioni dall'esperimento del 14 settembre che valgono qui — sulla meccanica, **non** sul
lettore, che era proprio ciò da non ripetere: le letture composte in KB si fanno **a stadi** (limite dei 384 legami), nessun `retract` a metà risoluzione, i fatti
del problema si **numerano**, `findall/3` raccoglie solo variabili, liste ≤ 512 byte
(`C_TODO.md`, «Limiti silenziosi del risolutore»).

---

## 6. Progressione DM-1 → DM-10

Il numero è la profondità **minima** della catena di letture necessarie.

| Livello | Che cosa dimostra | Capacità richieste |
|---|---|---|
| **DM-1** | una lacuna, una lettura, una relazione estratta dal lead | IR1, IR5; M2, M4 |
| **DM-3** | catena lineare su lead, relazioni eterogenee, una biforcazione | IR1–IR6; M1–M7 |
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

## 7. Esperimenti

### 7.1 E1 — DM-3, la demo: il nome del profeta di Nietzsche  *(primo test, in corso)*

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

**La stessa catena, nella IR** (ciò che la prova dovrà poter interrogare, non solo leggere
nella risposta):

```text
scope problem:  F1 = question named_after(subject: protagonist_of(tsz), object: missing)
                F2 = question associated_religion(subject: ↑F1.object, object: missing)
                F3 = question supreme_deity(subject: ↑F2.object, object: missing)
lettura 1 → scope page(thus_spoke_zarathustra):  A1 = assertion named_after(protagonist, zoroaster)
            legame F1.object ← A1.object   [evidenza: frase 2, rev. 1373299517]
lettura 2 → scope page(zoroaster):  A2 = assertion founder_of(zoroaster, zoroastrianism)
            candidati F2.object: {zoroastrianism ✓ founder_of, ancient_iranian_religion ✗ challenged,
                                  judaism|christianity|islam ✗ influence "with much scholarly controversy"}
lettura 3 → scope page(zoroastrianism):  A3 = assertion supreme_deity(zoroastrianism, ahura_mazda)
            legame F3.object ← A3.object
risposta = cammino F1 → A1 → F2 → A2 → F3 → A3
```

**Prova**: `tests/p0t/crossing/dm3_zarathustra_chain.p0t` — pagine vere in edizione
locale. **Oggi è rossa**, e deve esserlo: rende visibili M1–M7. Le verifiche sulla IR
(scope di pagina, legami, alternative) si aggiungono con IR1–IR6.

### 7.2 E2 — DM-5: da Karamazov alla religione

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

### 7.3 E3 e oltre — DM-7, DM-10

Da costruire con lo stesso metodo: prima si verificano **dove** sono scritti gli anelli
(tabella di §0), poi si scrive il prompt. Candidati di dominio diverso: storia della
scienza (strumento → inventore → città → università → allievo), musica (opera → libretto →
fonte letteraria → autore → movimento). Nessun prompt si scrive prima di aver letto le
pagine vere.

---

## 8. Che cosa si registra per ogni hop

Per ogni esecuzione si deve poter ricostruire — **come vista interrogabile della IR**, non
come registro scritto a parte (§4.2):

1. **Lacuna** — quale informazione parrot0 ritiene mancante.
2. **Query** — che cosa decide di leggere, e perché quella pagina.
3. **Evidenza** — la frase letta, con articolo, edizione e revisione.
4. **Aggiornamento della IR** — il frame assertivo trovato nello scope della pagina, il
   legame che riempie il ruolo mancante del frame del problema, e i legami alternativi
   rimasti aperti. È il campo **centrale**: gli altri sei sono letture di questo.
5. **Inferenza** — quale conclusione diventa possibile; quali candidati cadono e perché.
6. **Prossimo obiettivo** — perché quella conclusione genera il bisogno successivo.
7. **Risposta** — la conclusione e la catena di evidenze che la sostiene.

Una risposta corretta ottenuta per caso non completa la missione.

---

## 9. Metriche

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

## 10. La demo

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

## 11. Criterio finale

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

## 12. Registro delle sessioni

- **14 settembre 2026** — piano riordinato ed espanso; stato misurato (§0); pagine vere dei
  sei anelli salvate in `tests/fixtures/wiki/`; primo test E1 scritto e rosso
  (`tests/p0t/crossing/dm3_zarathustra_chain.p0t`). Prossima mossa: M1 + M2 (il compito a
  catena non va più a storie; il lead intero nella working memory), poi M4 sull'hop 2, che
  è il primo anello leggibile con una relazione già a portata del lettore di prosa.
- **14 settembre 2026, dopo** — osservazione di F.: la IR era citata pochissimo. Aveva
  ragione: il piano proponeva predicati ad hoc per frasi, menzioni e traccia, cioè un
  lettore parallelo. Aggiunta §4 (la IR è il ciclo: lacuna = ruolo mancante di un frame,
  lettura = scope di pagina, integrazione = legame fra scope, traccia = vista), le mancanze
  della IR IR1–IR6, e M1–M9 riscritte come produttori e consumatori della IR. **La prossima
  mossa cambia**: prima IR1 (la pagina letta pubblicata come scope) e IR5 (i frame
  assertivi della prosa dentro la IR), perché senza di esse M1–M7 si costruirebbero di
  nuovo fuori dalla IR.
