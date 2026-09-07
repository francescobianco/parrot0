# La rete come memoria profonda — l'accesso a Wikipedia è un atto cognitivo, non un circuito

*Aperto il 2026-09-07 su indicazione di F. Il sistema «Wikipedia» non era mai
stato dichiarato in un punto unico: è nato come fetch (gen171/240), è stato
riformato (gen436: «di Wikipedia non si conserva niente»), è stato attraversato
dal sogno (gen382/405) e da nove siti d'iniziativa — e il 7 settembre si è
scoperto che dal gen436 nessun fetch arrivava più al lettore, senza che nessuno
se ne accorgesse. Un pezzo che può rompersi per un mese senza che nessuno lo
senta non è una facoltà: è un tubo. Questo documento lo ridefinisce come
facoltà, e dice che cosa, dell'esistente, è mal implementato.*

---

## 0. La tesi (F.)

> *parrot0, tramite inferenza, deve accorgersi di avere delle lacune a runtime
> durante un prompt; questo non attraverso piani cablati ma attraverso piani di
> correzione e azione che esistono come KB. Deve capire che l'azione
> necessaria è leggere il topic da Wikipedia, elaborarlo leggendo la prosa —
> sempre come processo di inferenza — e ricavare nuova conoscenza che metterà
> in sessione e userà per interloquire. Questa è l'abilità d'uso della rete,
> quella che chiamiamo* network. *Il flusso `WIKI_FETCH` non serve, non c'è.
> Anche il poter chiedere all'utente se vuole un approfondimento — legato allo
> stato di rete attiva — sarà inferito dalla KB. Come abbiamo visto per i
> processi intelligenti emersi dall'interazione fra le parti della KB, per
> effetto della scala e della colla linguistica, l'accesso al topic Wikipedia
> non deve essere un circuito di alimentazione fisso ma un meccanismo cognitivo
> con una sorta di memoria profonda da cui ricava i topic e le loro
> definizioni. Tutto ciò che non rientra in questa definizione è mal
> implementato. Anche `--dream` deve ripercorrere questa idea: nel sogno non si
> sogna in maniera programmatica come uno script di fetch, ma si elabora la
> ricorsione, parola per parola, del topic in entrata, sempre in maniera
> cognitiva, dall'inferenza KB.*

In una riga: **la rete è una memoria profonda di parrot0, e leggerla è un
atto che parrot0 decide, esegue e ricorda come ogni altro atto — per
inferenza, con conoscenza, con provenienza.**

## 1. Perché ciò che c'è è mal implementato — misurato

La lettura del codice il 7 settembre (`99-registry.c` ~5040-5200,
`50-self-research-loop.c` 266-330, `learn.c`, `dream.c`) dà questo quadro:

| oggi | perché non rientra nella definizione |
|---|---|
| `PARROT0_WIKI_FETCH=1` e `PARROT0_TOOLS=1` letti con `getenv` decidono se la rete esiste | lo **stato di rete** è una variabile d'ambiente, non un fatto: nessuna regola KB può chiedere «posso leggere?», nessuna lezione può cambiarlo, `/debug` non lo vede |
| l'offerta «Want me to look it up?» → `pending_gap/1` + `pending_gap_question/1` → «yes» riconosciuto da `p0_is_confirmation` → fetch → ri-dispatch: **tutto in una funzione C** | è la mossa d'iniziativa #3 di `initiative.md`, l'unica completa nei cinque stadi — ma i cinque stadi sono cablati, non sono `initiative_trigger/ground/move/uptake` |
| `acquire_knowledge`: RAM → corpus locale → (fetch) in **quest'ordine, nel C** | la strategia d'indirizzamento è conoscenza (`autocrescita-v3` §2: «l'unificatore non possiede la strategia»); e il corpus locale non esiste più dal gen436, quindi il secondo gradino era morto |
| `wiki_fetch_bilingual` scaricava e **scartava** la prosa; `extract_page_facts` leggeva `kb/learning/pages/<key>.md`, cartella abolita | il fetch e il lettore erano due tubi che non si toccavano: la definizione del gen436 («la prosa passa dal lettore che legge un testo incollato») era scritta e non realizzata. Riparato il 7 settembre (`32c3660`) — ma riparare un tubo non lo rende una facoltà |
| `mod_learn` (registro #75) è una **facoltà terminale** che rivendica il turno per emettere l'offerta | l'offerta è una coda alla risposta (`initiative.md` §4: manca lo strato post-dispatch); per dirla, `learn` deve rubare il turno, e infatti oggi ruba anche «tell me about zorb» quando `means/2` è in KB |
| `--dream`: profondità e nodi sono policy KB (`dream_max_depth`), ma **quali parole sognare** e come espandere la frontiera lo decide `dream.c` | il sogno è «uno script di fetch» esattamente nel senso di F.: la ricorsione è del driver, non dell'inferenza |
| la definizione scaricata sopravvive come `wiki_concept/2` e `wiki_alias/2`, senza revisione né span | non è una memoria profonda: è un appunto senza indirizzo, non riproducibile (`autocrescita-v3` §2: `wiki_address(Language, Title, Revision, Section)`) |

Il sintomo che li riassume: **il ciclo si è rotto al gen436 e nessun test, nessuna
sonda e nessuna regola l'ha visto per un mese**, perché nessuna parte della KB
sapeva che quel ciclo esisteva.

## 2. La facoltà, definita

La facoltà ha **un nome** — la chiameremo *network*, come la chiama F. — ed è
l'integrazione di sei cose che i piani hanno già, prese ciascuna dal suo
proprietario:

```text
1. ACCORGERSI      il turno non attraversa il grafo: la lacuna ha una specie e
                   un termine mancante          (universal-comprehension §10,
                   frontier §2.1, gap-kinds.p0, information_need/4)
2. INDIRIZZARE     fra i rimedi conosciuti per quella specie c'è «leggere il
                   topic dalla memoria profonda»  (autocrescita-v3 §4:
                   address_candidate, gap_remedy)
3. DECIDERE        agire, chiedere prima, o fermarsi: una MOSSA, inferita dallo
                   stato — rete attiva, costo, politica del contesto,
                   iniziativa fondata           (frontier K3, initiative.md §7)
4. LEGGERE         la prosa passa dal lettore, frase per frase, come un testo
                   incollato; ogni proposizione porta la sua provenienza
                   (apprendimento-assistito §0.4, autocrescita-v3 §3)
5. RICORDARE       topic, definizione, indirizzo e proposizioni entrano in
                   sessione come memoria profonda interrogabile (frontier K7)
6. RIPRENDERE      la questione aperta si chiude nello stesso turno con ciò
                   che si è appena letto, o resta nominata (K3 resume/D49,
                   la colla: continuità fra prima e dopo)
```

Nessuno dei sei è nuovo. **Nuovo è che siano lo stesso ciclo, e che ogni
passo sia un fatto interrogabile.** È la stessa mossa della «KB viva»
(`apprendimento-assistito` §0.1): *un arresto → bisogno nominato →
acquisizione candidata → prova → uso*, con la rete come una delle fonti.

### 2.1 La memoria profonda

Wikipedia non è «un sito da cui scaricare»: è per parrot0 ciò che la memoria a
lungo termine è per una persona — **un deposito di topic e definizioni che non
sta nella memoria di lavoro, ma a cui si può accedere quando la conversazione
lo richiede, con un costo, e di cui si ricorda ciò che si è letto**. Da qui la
forma della memoria:

```prolog
% l'indirizzo di ciò che si è letto: riproducibile, con edizione e revisione
topic_read(Topic, wiki_address(Lang, Title, Revision, Section)).
% la definizione: la prima proposizione della pagina, con la sua fonte
topic_definition(Topic, Proposition).
% ogni proposizione letta ne conserva lo span
supported_by(Proposition, wiki_address(...), Span).
% che cosa il topic ha aperto e non chiuso: i termini ancora ignoti
topic_open_term(Topic, Term).
```

`wiki_concept/2` e `wiki_alias/2` sono la forma povera di questo: restano come
strutture secondarie finché la forma piena non le copre, poi diventano viste.

### 2.2 Lo stato di rete è un fatto

```prolog
% dichiarato dal profilo che avvia parrot0 (make chat), non da una variabile
capability(network, on).
network_available :- capability(network, on).
% e la politica su QUANDO usarla, per contesto, insegnabile e ritrattabile
acquisition_policy(Context, ask).      % proponi, e aspetta il si'
acquisition_policy(Context, act).      % leggi, e dillo
acquisition_policy(Context, never).    % dichiara la lacuna e fermati
```

`PARROT0_WIKI_FETCH` sparisce. I test dichiarano `capability(network, off)`
oppure un **provider locale** (`topic_provider(fixture, "tests/fixtures/wiki")`)
che è un'edizione come un'altra: la KB decide l'ordine dei provider, il C sa
solo aprire un indirizzo. Così il determinismo dei test resta, e resta anche la
regola del gen436: **non si archivia**, si legge.

### 2.3 Chiedere prima è una mossa, non un `if`

«Want me to look it up?» è oggi una stringa emessa da `mod_learn`. Nella
facoltà è la mossa `propose_acquisition`, che vale solo se:

- la lacuna è di specie `knowledge` o `reach` con un termine nominabile
  (fondamento: `initiative.md` §6.1, «nessuna iniziativa senza fondamento»);
- `network_available` è provabile (altrimenti la mossa è `decline_named`:
  «non posso leggere adesso», e il perché);
- `acquisition_policy(Ctx, ask)` — con `act` si legge e si dice «ho letto»;
- l'accettazione è `initiative_uptake(propose_acquisition, acquire(Topic))`, e
  l'azione parte davvero (§6.2: «nessuna offerta che non si sappia onorare»).

È il sito #3 di `initiative.md` riscritto nei suoi cinque stadi come fatti; la
macchina a stati della raccolta (stadi 4-5) diventa la **primitiva generale**
che quel piano chiede di estrarre, e serve a ogni altra offerta.

### 2.4 Il sogno è la stessa facoltà senza interlocutore

`--dream TOPIC` oggi: il driver C mette il topic in coda, scarica, chiama il
lettore frase per frase, e decide lui quali parole aggiungere alla frontiera.
Nella facoltà:

```text
sognare = ripetere il ciclo 1→6 senza un turno umano:
  la lacuna iniziale è il topic dato (o le lacune aperte della sessione);
  ogni proposizione letta che nomina un termine ignoto APRE una lacuna nuova
  (topic_open_term) — è la ricorsione «parola per parola» di F.;
  quale lacuna espandere dopo, e quando fermarsi, lo dicono le regole
  (frontiera, ordine, budget: autocrescita-v3 §2 — «ricerca e limite sono
  conoscenza eseguibile»);
  il driver C fa una cosa sola: «dammi la prossima lacuna da chiudere» → agisce
  → «ho letto» → di nuovo.
```

Il sogno smette così di essere uno script e diventa **l'inferenza che
continua da sola**: la stessa che, in conversazione, si fermerebbe alla prima
lacuna per chiedere. E il suo esito non è «N fatti estratti» ma **quante
lacune aperte ha chiuso** — che è già ciò che `dream.c` (gen405) prova a
contare, per conto suo.

## 3. Il flusso, come inferenza

```text
turno
  -> frame + tentativo di risposta
  -> nessuna prova: information_need(T, knowledge, malta_location, Goal)
  -> gap_remedy(knowledge, read_topic)             ← rimedio conosciuto
  -> action_schema(read_topic) precondizioni:
        network_available, topic_address(Need, Title)  ← inferite
  -> dialogue_move(T, propose_acquisition | acquire | decline_named)
        ← acquisition_policy + iniziativa fondata
  -> [se acquire] primitiva C: leggi(wiki_address) -> prosa in memoria
  -> il lettore di «read: …», frase per frase -> proposizioni + supported_by
  -> topic_read, topic_definition, topic_open_term          ← memoria profonda
  -> la questione aperta si riprova: resume(Issue)
        -> risposta, con la fonte; oppure lacuna ancora nominata
```

Le primitive C sono tre e passive (`autocrescita-v3` §2): aprire un indirizzo
(HTTP su edizione + revisione), spezzare in frasi, leggere una frase. Tutto ciò
che *decide* — quando, se chiedere, quale edizione, quante frasi, che cosa
ricordare, quando fermarsi — è KB.

## 4. La mappa con i sei piani

| piano | che cosa porta alla facoltà | che cosa la facoltà chiude di quel piano |
|---|---|---|
| **apprendimento-assistito** (la missione) | il ciclo della KB viva (§0.1), il contratto epistemico sulle fonti (§0.4), i quattro stati di una capacità | la cella «Piano → azione → osservazione» ha finalmente un'azione non-tool: leggere |
| **universal-comprehension** | le tre specie di lacuna (§10), l'acquisizione come passo del planner (§7) | §7 va riscritto su questo ciclo: sparisce `pages/<key>.md`, sparisce «UX a due turni» come limite (con `act` è un turno solo) |
| **initiative** | i cinque stadi della mossa, le regole d'onestà, il sito #3 come modello | il sito #3 diventa il primo `initiative_*` reale; lo strato post-dispatch nasce da qui |
| **frontier** | K3 (issue, mossa, obbligo), K7 (memoria), K11 (azioni con precondizioni ed effetti), §2.1 (i cinque residui) | K3 ottiene il **consumatore** che non ha (`armonizzazione-piani.md` §2.1): `resume` dopo la lettura è la sua istanza minima |
| **autocrescita-v3** | il confine C/KB/Wikipedia (§2), l'estrattore come moltiplicatore (§3), gli oggetti minimi (§4: arresto, indirizzo, provenienza, compensazione) | i suoi predicati smettono di essere «ontologia obiettivo»: `address_candidate`, `external_address`, `supported_by` hanno un produttore |
| **the-linguistic-glue** | la continuità fra prima e dopo la lettura: il referente, l'issue, «più precisamente» | la colla ha un caso in cui *deve* attraversare un'azione: la domanda fatta prima della lettura è la stessa dopo |

E `armonizzazione-piani.md` §3 resta l'ordine: **il consumatore di K3 per
primo**, perché è il passo 6 di questo ciclo e senza di esso la lettura non
torna alla conversazione.

## 5. Le regole d'onestà, ereditate e specifiche

1. **Una fonte sola, con indirizzo.** Solo Wikipedia, solo prosa, revisione
   e span conservati; niente motori di ricerca, niente Wikidata (autocrescita
   §2). Ciò che entra è *dato*, il ragionamento resta in casa.
2. **Verità di stato.** «Sto leggendo» solo se la lettura parte; «ho letto» solo
   con `topic_read` asserito; «non posso» con il perché (`network_available`
   falso, o `never`).
3. **Attribuzione.** Una proposizione letta è `supported_by` la pagina, non un
   fatto del mondo: la promozione passa dal gate di verità del protocollo
   (`apprendimento-assistito` §0.4). Wikipedia può sbagliare, e parrot0 deve
   poterlo dire citandola.
4. **Nessuna lettura mascherata da risposta.** Se dopo la lettura la questione
   non si chiude, si dice che cosa si è letto e che cosa manca ancora
   (`initiative.md` §6.4).
5. **La memoria profonda non è un archivio.** Restano proposizioni con
   indirizzo; non resta il testo. È la regola del gen436, e vale anche nel
   sogno.
6. **Si può tacere.** Non ogni lacuna merita una lettura: `acquisition_policy`
   e il fondamento decidono, e «non ora» è una mossa legittima.

## 6. Gate: quando la facoltà esiste

Una catena che regge vale più di dieci punti sparsi. I gate sono catene:

| # | catena | che cosa prova |
|---|---|---|
| G1 | «sai dove si trova malta» → Europe → «più precisamente» → *«nel Mediterraneo centrale (Wikipedia, Malta)»* | accorgersi + leggere + riprendere nello stesso filo (passi 1-6) |
| G2 | «cosa sai della birra» con `acquisition_policy(act)` → risposta con fonte **nello stesso turno** | la lettura è un atto interno al turno, non un secondo turno |
| G3 | stessa domanda con `capability(network, off)` → declino che nomina la rete | lo stato di rete è un fatto, e la mossa lo legge |
| G4 | `acquisition_policy(ask)` → offerta → «no» → nessuna lettura, nessuna insistenza; → «sì» → lettura | i cinque stadi dell'iniziativa come fatti |
| G5 | `--dream birra` chiude almeno una lacuna aperta della sessione e la sua espansione segue `topic_open_term` | il sogno è la stessa facoltà senza interlocutore |
| G6 | ablazione: `!forget gap_remedy(knowledge, read_topic)` → la lacuna resta nominata e nessuna lettura parte | la facoltà è conoscenza: togliendola, sparisce senza ricompilare |
| G7 | «riassunto sui problemi tipici nella produzione della birra» → lettura delle **sezioni** pertinenti → sintesi proposizionale con fonte | la memoria profonda ha sezioni (K7, D8); è il gate più lontano |

Tutti offline in `make test` con il provider locale; G1-G4 anche dal vivo in
`make chat`.

## 7. Gli incrementi, in ordine — uno per sessione, poi si massimizza

1. **Lo stato di rete e la politica come fatti** (`capability(network, on)`,
   `acquisition_policy/2`, `topic_provider/2`); l'`if (getenv(...))` diventa
   una query. Il flusso `WIKI_FETCH` non c'è più. Zero comportamento nuovo.
2. **Il rimedio come conoscenza**: `gap_remedy(knowledge, read_topic)` +
   `action_schema(read_topic, …)` con precondizioni ed effetti (K11); il sito
   C di `99-registry.c` diventa l'**esecutore** di quell'azione, non chi decide.
3. **La memoria profonda**: `topic_read`, `topic_definition`, `supported_by`
   con `wiki_address`; `wiki_concept/wiki_alias` come viste.
4. **Il consumatore di K3**: dopo la lettura, `resume` della questione aperta
   nello stesso turno; «più precisamente» come `qualify` sulla stessa issue
   (G1, G2). È il passo che fa convergere i piani.
5. **L'offerta come mossa**: i cinque stadi del sito #3 come `initiative_*`,
   la raccolta come primitiva generale (G4); `mod_learn` smette di rubare
   turni e diventa coda post-dispatch (`initiative.md` §8.1-8.4).
6. **Il sogno sull'inferenza**: `dream.c` chiede alla KB la prossima lacuna e
   il criterio d'arresto; `topic_open_term` è la frontiera (G5).
7. **Le sezioni**: `wiki_address` con `Section`, selezione per aspetto,
   sintesi proposizionale (G7).

Ogni incremento porta un cricchetto con ablazione, e ognuno lascia dietro di sé
la sonda `/debug` che l'avrebbe trovato: `information_need`, `dialogue_move`,
`topic_read`. Il difetto di partenza — un ciclo rotto per un mese senza che
nessuno lo veda — è esattamente ciò che una facoltà fatta di fatti non può più
avere: se `topic_read` non compare, `/debug` lo dice.

## 8. Che cosa NON è questa facoltà

- Non è un LLM esterno né un motore di ricerca: la rete è una **memoria**, non
  una mente in prestito (PRINCIPLES).
- Non è un modo per rispondere sempre: chi non trova conserva la lacuna
  nominata; la lettura non converte un muro in una risposta fluente.
- Non è un secondo lettore: la prosa passa da **il** lettore, quello di
  `read: …` e della prosa detta; un lettore «per Wikipedia» sarebbe D37
  («due percorsi che devono accordarsi e non condividono l'oggetto»).
- Non è un archivio: niente `pages/`, niente cache; solo proposizioni con il
  loro indirizzo.
