# Da parola sconosciuta a stato dell'utente

> **F., 14 settembre 2026:** dimostrare che parrot0 trasforma una parola
> semanticamente sconosciuta in conoscenza operativa, senza che sia stata
> programmata prima nella KB. Il turno di partenza è «mi sento stanco».

Questo piano è il **primo caso guida di M1** di
[`interlocutore-di-frontiera.md`](interlocutore-di-frontiera.md) — *un modello
della situazione costruito da ciò che l'utente dice* — ristretto a una specie di
fatto sull'utente: **il suo stato**. Si esegue come sessione **mista**
(comprensione, metacomprensione, KB viva) secondo
[`procedura-crescita-kb.md`](procedura-crescita-kb.md) §4, e ogni pezzo di
conoscenza entra con [`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md).

---

## ⛔ RINFORZO KB-FIRST — da rileggere prima di ogni passo

Questo esperimento è il più esposto del progetto alla scorciatoia, perché la
risposta «giusta» si ottiene in dieci righe di C o in una cue. Chi lo riprende
deve sapere che quelle dieci righe **lo annullano**: la tesi è che parrot0 lo
capisca, non che risponda.

**La domanda zero, a ogni modifica:** *questa cosa è generalizzabile KB-first?*
Se «sì ma è più lavoro», si fa il lavoro (`MANTRA.md`).

**Il test operativo, applicato qui:** *domani parrot0 può imparare «sfinito»,
«assonnato», «in ansia», «affamato» — e «Marco è preoccupato» — senza
ricompilare e senza che nessuno scriva una riga per quella parola?* Se no, la
conoscenza è nel posto sbagliato.

### Vietato in questo piano

| scorciatoia | perché annulla l'esperimento |
|---|---|
| `intent_cue(mood_tired, "mi sento spossato")` o qualunque cue per la parola di prova | è il frasario di `reactions.p0` §5: misura la cue, non il ciclo |
| estendere `mood_tired`/`reaction_effect` alle parole nuove | idem, un gradino più su |
| un elenco C di copule/verbi di stato («sento», «sono», «ero», «sto») | parole in C (mantra #2: niente elenchi di parole in C) |
| una `struct`/campo C per lo stato dell'utente | stato non interrogabile, non insegnabile (gen240: `b->user_mood` era già stato tolto) |
| un ramo `if (stato == fatigue)` o una risposta scritta per la stanchezza | la risposta deve derivare dal modello, non dal valore |
| scrivere a mano in `.p0` che cosa significa «spossato»/«stanchezza» | è ciò che parrot0 deve apprendere (LEARN_PROTOCOL §1.3) |
| fonti diverse da **Wikipedia ufficiale** (it/en) | vincolo di F.: nessun dizionario, corpus o servizio esterno |
| MCP (`gen.respond`, `kb.assert`, `kb.save`) per far crescere o misurare la KB | MCP è uno strato applicativo, non di crescita; e `kb.save` instrada nei file tracciati |
| fixture o entità inventate per «dimostrare» comprensione | MANTRA: un test non inventa la conoscenza che dichiara di scoprire |
| alzare i `!timeout` o togliere casi per far passare un gate | i timeout misurano il motore, non il test |

### Ammesso

- **Porte in C**: una primitiva che pubblica un fatto o chiede alla KB e basta
  (come `inner_turn` della mossa 1). Ogni porta ha il suo commento con il file
  KB che decide.
- **Forme, classi, regole di lettura e di stato in KB**, scritte a mano **solo**
  se aprono una meccanica generale di meta-comprensione (LEARN_PROTOCOL §1.3), e
  con la lezione parlata che le ricrea annotata come catena d'insegnabilità
  («Le radici dell'insegnabilità»). Una riga che nessuna superficie produce è una
  **riga a mano**: si registra ed è il lavoro successivo.
- **Conoscenza del mondo solo parlando o leggendo** Wikipedia, con `/save`, diff
  classificato (W/L/C/P/O/X) e rilettura in un processo nuovo (LEARN_PROTOCOL
  §8-10).

### Dove vive ogni pezzo

| pezzo | casa | chi lo può cambiare domani |
|---|---|---|
| quali verbi introducono uno stato di chi parla | KB, lessico/forme (`lexicon.p0`, `grammar.p0`) | una lezione «"sentirsi" introduce come sta qualcuno» |
| come si legge un'autodescrizione (ruoli, tempo, grado, causa) | KB, forme di turno (`turn_form/3` o forme IR) | lezione di forma (catalogo §6-bis, D/E) |
| che cos'è uno stato, le sue dimensioni | KB, ontologia | lezione di classe (catalogo A) |
| che cosa significa «spossatezza» | KB viva, **appreso** da it.wikipedia | nessuno a mano |
| «spossato» ↔ «spossatezza» | KB, forme delle parole (catalogo N) | lezione o lettura |
| lo stato corrente dell'utente | KB di sessione (`session_slot`/`turn_scratch`) | il dialogo stesso |
| che cosa fare di uno stato (chiedere, suggerire, non dare consigli medici) | KB, condotta (catalogo E/P) | lezione di condotta |
| che un turno interno non parli per l'utente | KB (`inner_turn`, `discourse.p0`) + tre porte C | una riga per una nuova ragione |

---

## 0. L'esperimento

Il ciclo da mostrare:

```
UNKNOWN → READ → UNDERSTAND → INTEGRATE → REREAD → STATE → REASON → RESPOND
```

1. **Stato iniziale.** La KB ha le strutture generali (parlante, autodescrizione,
   stato, intensità, causa, durata, bisogni, azioni) e le forme «mi sento X»,
   «sono X», «I feel X», «I am X». Non ha il significato della parola, né una
   regola «se l'utente dice "mi sento X" allora …».
2. **IR parziale.** Il fallimento semantico è **locale**: «capisco la struttura,
   non il valore» (§2).
3. **Buco epistemico.** `need_to_understand(X)`, distinto da «non so cosa
   rispondere».
4. **Memoria profonda.** La prosa di **it.wikipedia** (o en.wikipedia),
   recuperata.
5. **Comprensione.** La prosa passa dalla **stessa** comprensione dei turni,
   non da un estrattore speciale; il concetto si ancora a concetti vivi.
6. **Integrazione.** Relazioni, non una `definition/2`.
7. **Rilettura.** Lo stesso turno ora aggiorna lo **stato dell'utente** (§3),
   non `user_said(...)`.
8. **Ragionamento.** Procedure generali più salienza. Nessun piano inventato.
9. **Risposta.** Deriva dallo stato modellato. Niente informazioni mediche non
   sostenute da ciò che si è appreso (LEARN_PROTOCOL §4.1).
10. **Integrazione vera** su turni nuovi (§5).
11. **Trasferimento**: un secondo concetto definito *per mezzo* del primo.
12. **Ablazioni**, parlate e mirate, sulla KB viva.
13. **Misure**: Acquisition, Integration, Transfer, State grounding, Persistence.

---

## 1. Parametri di sessione (LEARN_PROTOCOL §2)

| parametro | valore |
|---|---|
| `DOMINIO` | stati della persona riferiti in prima e terza persona (stanchezza e affini) |
| `OBIETTIVO` | una parola di stato sconosciuta, detta dall'utente, diventa stato dell'utente con tempo, grado e causa, per mezzo della lettura di Wikipedia |
| `BUDGET` | una mossa per sessione (§6), passi brevi e stoppabili (regola di F.) |
| `FONTI` | **solo** it.wikipedia.org / en.wikipedia.org, con titolo risolto e revisione (`topic_read/2` le registra già) |
| `TARGET_WORLD_FACTS` | ≥ 3 fatti veri sul concetto letto (classe, effetto, rimedio), classificati `W` |
| `TARGET_CAPABILITIES` | 1 per mossa: la forma o la meccanica che la mossa apre |
| `STOP_CONDITION` | primo misclaim non spiegato; fonte che non sostiene la proposizione; gap non chiudibile in KB → §7 del protocollo |

**Parola di prova: «spossato».** Zero occorrenze in `kb/` (anche *esausto,
sfinito, affaticato, assonnato, stremato, intontito, frastornato, svogliato,
sonnolento*). **«stanco» è il controllo**: il frasario `mood_tired` esiste e
non si pota (regola di F.: strutture secondarie si tengono); il ciclo, una volta vero, deve
renderlo superfluo, e la differenza fra i due è la misura.

⚠ **Il taglio medico.** it.wikipedia porta *Spossatezza* → *Astenia* («un
sintomo…») e *Stanchezza* → *Fatica*. Sono pagine mediche: la lezione tiene il
livello **stato della persona** (riduzione di energia, alleviata dal riposo) e
la condotta non dà consigli clinici. Una proposizione che la pagina non sostiene
esattamente si scarta (§4.2 del protocollo).

---

## 2. La IR dell'autodescrizione — che cosa deve venire fuori da un turno

La IR è quella dei turni che esiste già (`input_node_atom/3`, `turn_span/4`,
`turn_illocution/2`, `input_frame_record`, il fuoco `turn_focus`); qui si
elenca **che cosa deve contenere** per un turno che riporta uno stato. Nessuna
di queste voci è una parola scritta in C: ognuna è una classe o una forma in KB
con i suoi membri.

### 2.1 Il turno come atto

| componente | valori | da dove, in KB |
|---|---|---|
| atto | `self_report` · `third_party_report` · `state_question` («sei stanco?», «come sta Marco?») · `state_change` («ora sto meglio») · `denial` («non sono stanco») | `turn_illocution` + forma |
| forza | `assertion` · `expressive` · `question` | `turn_illocution/2` (oggi: *expressive* su «mi sento stanco») |
| parlante | `user` | il turno |
| destinatario | `parrot0` (o nessuno, se **turno interno**: mossa 1) | `inner_turn` |

### 2.2 Il soggetto dello stato

| componente | valori | note |
|---|---|---|
| soggetto | `user` (io, mi, me) · `parrot0` (tu, ti — va a `self_embodiment`, onestà) · terza persona nominata («Marco») · gruppo («siamo stanchi») | deissi come classe di pronomi (`participant_pronoun/1` esiste già) |
| accordo | genere e numero dal complemento («stanca» → femminile, «stanchi» → plurale) | informazione sul soggetto, non rumore: può correggere o arricchire il profilo |
| coreferenza | «lui», «lei», «anche lui» → antecedente del discorso | M3 di frontiera |

### 2.3 Il predicato e il complemento

| componente | valori | esempi |
|---|---|---|
| costruzione | copulare (`essere`) · percettiva (`sentirsi`) · possessiva (`avere` + nome: «ho sonno», «ho fame») · di condizione (`stare`: «sto a pezzi», «sto male») · inglese (`be`, `feel`, `get`) | ciascuna una riga `state_verb(Verbo, Costruzione)` in KB |
| evidenzialità | percepito («mi sento») · asserito («sono») · attribuito («sembri stanco») · creduto («credo di essere») | cambia la **fonte** dello stato (§3.3) |
| categoria del complemento | aggettivo/participio («spossato») · nome di stato («spossatezza», «sonno») · locuzione («a pezzi», «uno straccio») | la locuzione è una forma, non una cue |
| **slot del valore** | lemma riconosciuto → concetto; lemma **non** riconosciuto → `unknown_state_value(Token)` | il **fallimento locale**: il resto dell'IR resta valido |
| polarità | affermativa · negata («non sono stanco», «per niente») | la negazione chiude o esclude uno stato, non lo apre |

### 2.4 Grado, tempo, durata

| componente | valori | forme |
|---|---|---|
| intensità | scala ordinata `slight` < `moderate` < `high` < `extreme` | modificatori come classe: «un po'», «abbastanza», «molto», «troppo», «da morire»; morfologia «-issimo» (catalogo N) |
| ancoraggio temporale | `now` · `today` · `yesterday` · `this_morning` · data | avverbi deittici come classe; si risolvono sull'orologio (M2 di frontiera) |
| tempo verbale / aspetto | presente → corrente · imperfetto («ero») → passato, non corrente · passato prossimo («sono stato») → concluso · futuro («sarò») → previsto | morfologia verbale in KB (`irregular_verb_form/2` e affini) |
| durata / esordio | «da tre giorni» → esordio + durata · «sempre» → abituale · «all'improvviso» → esordio brusco | forme temporali (catalogo L: oggi 🔴 18-20) |

### 2.5 Relazioni con il resto della frase

| componente | valori | forme |
|---|---|---|
| causa | «perché ho dormito poco» · «per il lavoro» · «dopo la corsa» | connettivi causali come classe; la causa è **un'altra IR** (ricorsiva) |
| contrasto | «ma voglio uscire» → `contrast(stato, scopo)` | connettivi avversativi |
| scopo / obbligo | «voglio», «devo», «ho bisogno di» | modalità come classe |
| conseguenza dichiarata | «quindi non vengo» | connettivi consecutivi |
| riferimento a un piano già detto | «stanco per la riunione di domani» | aggancio al modello della situazione (M1) |

### 2.6 La lacuna, tipizzata

Quando lo slot del valore non si risolve, l'IR pubblica **una** lacuna locale,
con la sua specie e il suo rimedio — mai «Non capisco ancora» sull'intero turno:

```
turn_gap(current_turn, state_value, "spossato")
gap_kind(..., knowledge)          % manca il VALORE, la strada c'è
gap_remedy_action(knowledge, read_topic)   % esiste già in network.p0
```

Oggi `pending_gap` resta vuoto e la specie è `reachability`/`blind` (E0, §4):
cioè il muro dice «manca la strada» quando manca il valore. È l'M13 di
`apprendimento-assistito.md` (tipizzazione del gap) e l'R7 di frontiera
(comprensione parziale **dichiarata**).

---

## 3. Lo stato e la condizione dell'utente — il modello

`user_value(mood, tired)` è **un caso** di questo modello, non il modello: resta
come vista derivata (non si toglie). Lo stato è una specie di **fatto sulla
situazione** (M1 di frontiera): la stessa casa deve poter ospitare «domani ho
una riunione alle 9», quindi le coordinate sono quelle di qualunque evento.

### 3.1 Il fatto di stato

```
holds(Stato, Soggetto, Intervallo)       % che cosa, di chi, quando
state_degree(Id, Grado)                  % quanto
state_source(Id, Fonte, Confidenza)      % come lo sappiamo
state_cause(Id, CausaId)                 % perché (un altro fatto)
state_onset(Id, Istante)                 % da quando
state_closed(Id, Istante)                % fino a quando («ora sto meglio»)
```

La forma esatta si decide alla mossa 3; conta che siano **fatti interrogabili**
(«come mi sento?», «da quando sono stanco?», «perché?»), non campi.

### 3.2 Le dimensioni di uno stato (ontologia, in KB, insegnabile)

| dimensione | valori | esempio con la stanchezza letta da Wikipedia |
|---|---|---|
| specie | fisico · mentale · emotivo · sociale · misto | la pagina distingue fatica **fisica** e **mentale** |
| effetto sulle capacità | riduce / aumenta una capacità | riduce energia, capacità di sostenere attività |
| valenza | spiacevole · piacevole · neutra | spiacevole |
| energia/attivazione | bassa · alta | bassa |
| decorso | transitorio · persistente · ricorrente | «insorgenza graduale»; la persistenza distingue *astenia* |
| rimedio noto | che cosa lo riduce | «alleviata da un periodo di riposo» |
| cause tipiche | che cosa lo produce | sforzo, cause fisiche o mentali |
| sensibilità | ordinario · salute · intimo | salute → condotta prudente (§3.5) |

Nessuna riga di questa tabella si scrive per la stanchezza: le **dimensioni**
sono ontologia generale (lezione di classe); i **valori** per la stanchezza si
leggono dalla pagina.

### 3.3 Fonte e fiducia

| fonte | quando | confidenza | note |
|---|---|---|---|
| `self_report` | «mi sento / sono X» | alta | chi parla è autorità sul proprio stato |
| `perceived_by_speaker` | «mi sento» vs «sono» | alta, ma soggettiva | l'evidenzialità è conservata |
| `third_party_report` | «Marco è stanco» detto dall'utente | media | si attribuisce: «secondo te Marco è stanco» |
| `inferred` | dedotto da una causa («ho dormito due ore») | bassa, **dichiarata** | mai detto come certo (M6 di frontiera, R2) |
| `stale` | stato «now» di turni fa, o di ieri | decrescente | un «now» invecchia (R8) |

### 3.4 Vita dello stato nel tempo

- **corrente** solo se ancorato a *now/oggi* e non chiuso; «ieri ero stanco» non
  tocca lo stato corrente;
- **chiusura**: «ora sto meglio», «mi è passata» chiude l'intervallo, non
  cancella la storia;
- **invecchiamento**: uno stato *now* senza conferma perde attualità dopo N
  turni o una data (`turn_counter` e l'orologio; N in KB);
- **contraddizione**: «sono stanco» dopo «sono pieno di energia» → revisione con
  fonte, oppure domanda di chiarimento (M8);
- **sessione vs profilo**: lo stato è di sessione (`session_slot`); diventa
  profilo (M11) solo se ricorrente **e** l'utente acconsente — un dato di salute
  non si persiste per inerzia.

### 3.5 Che cosa lo stato permette (condotta, in KB)

| situazione | mossa | vincolo |
|---|---|---|
| stato riportato, nessun piano aperto | riconoscere con il **contenuto** («poca energia») + domanda aperta | nessun piano inventato |
| stato + piano già detto (riunione alle 9, guida di tre ore) | segnalare la rilevanza (R3 salienza) | solo se la relazione è derivabile (capacità ridotta ↔ attività del piano) |
| stato + scopo in contrasto («ma voglio uscire») | riconoscere il contrasto, non decidere per l'utente | |
| stato di salute persistente / grave | nessun consiglio clinico; al più «se persiste, un medico» **solo** se la pagina letta lo sostiene | LEARN_PROTOCOL §4.1 |
| valore sconosciuto e lettura non consentita | chiedere il significato («che cosa intendi con spossato?»: M8) | la politica `ask/act/never` esiste già |
| domanda sullo stato di parrot0 | onestà (`self_embodiment`) | |

---

## 4. E0 — misura di partenza, senza cure (14 settembre 2026)

Commit `30b73fc4`, binario ricompilato, ambiente di `make chat` (rete accesa,
profilo `agi`), turni via stdin e `/debug` sui turni chiave.

> ⚠ Una parte di E0 (§4.3) era stata rilevata con una sonda MCP (`kb.save`).
> **Metodo ritirato**: MCP non è un layer di crescita né di misura della KB, e
> `kb.save` ha instradato 97 fatti nei file tracciati (ripristinati). Le cifre
> di §4.3 sono da **rimisurare** in `make chat` con `/debug` e `/save` + diff
> (LEARN_PROTOCOL §9).

### 4.1 La premessa non vale per «stanco»

`kb/core/reactions.p0` §5: ~45 `intent_cue(mood_tired, …)` («sono stanco»,
«sono stanca», «i am tired», «che stanchezza», «sono a pezzi»…) →
`reaction_effect(mood_tired, mood, tired)` → una frase fissa. È esattamente il
«se l'utente dice X allora …» che l'esperimento esclude.

### 4.2 Turni

| turno | risposta | modulo / esito |
|---|---|---|
| mi sento stanco | «Non capisco ancora.» | fallback, `blind_wall`, `pending_gap` vuoto |
| oggi sono molto stanco | «Non sono sicuro di aver seguito.» | fallback |
| ieri ero stanco | «Non capisco ancora.» | fallback |
| Marco è stanco | «Non ho afferrato bene.» | fallback |
| sono stanco perché ho dormito poco | «Sembra un momento un po' fiacco…» | chitchat, cue per sottostringa; causa persa |
| sono stanco ma voglio uscire | stessa frase fissa | chitchat; scopo e contrasto persi |
| mi sento spossato | «Non capisco ancora.» | fallback, `blind_wall`, **nessuna parola opaca** |
| definisci «spossatezza» | «Ho letto di «spossatezza»: L'astenia è un sintomo…» · 1 fatto | learn (it.wiki → *Astenia*) |
| definisci «stanchezza» | «Ho letto di «stanchezza»: La fatica o affaticamento…» · 2 fatti | learn (it.wiki → *Fatica*) |
| che cosa è la stanchezza? *(subito dopo)* | «Su stanchezza non so ancora molto. Vuoi che cerchi?» | la lettura non raggiunge la domanda |
| che cosa allevia la fatica? | «Non so ancora tradurre «allevia».» | |
| mi sento stanco *(dopo la lettura)* | «Ho cercato «insorgenza graduale» ma non ho trovato niente.» | **la lettura aveva parlato per l'utente** → mossa 1 |
| come mi sento? | «Non so ancora tradurre «sento».» | `user_value/2` vuoto |

### 4.3 Che cosa ha lasciato la lettura (da rimisurare senza MCP)

- fatti: `sensazione_soggettiva(fatica)`, `sintomo(astenia)`,
  `cause(affaticamento_può_avere, fisiche)` — **letto storto**;
- perse come `machinery_gap`, con *riposo, alleviata, riduzione* opache:
  «può essere alleviata da un periodo variabile di riposo», «riduzione di
  energia», «la fatica mentale è un transitorio declino delle funzioni
  cognitive», «può manifestarsi con sonnolenza, letargia…»;
- la prosa italiana letta **dopo** `canonicalize_lang` parola per parola: «the
  fatica o affaticamento is a sensazione soggettiva of stanchezza a insorgenza
  graduale»;
- la chiave conserva le virgolette basse: `topic_read(«stanchezza», …)`.

### 4.4 I muri, per stadio

| stadio | muro | strato (apprendimento-assistito) | frontiera |
|---|---|---|---|
| IR parziale | nessuna forma di autodescrizione; la parola sconosciuta non è marcata opaca | M1, M4, M13 | M1, R7 |
| stato | solo `user_value(mood, …)` dal frasario | M8 | M1, M11 |
| buco → lettura | nessuna strada aggettivo → nome dello stato | M5 | M1 |
| sorgente | Wikipedia dà l'inquadratura medica | — | R6 |
| comprensione | italiano ibridato; 3 fatti su ~7 frasi, uno storto | M9 | M9, R6 |
| integrazione | la lettura non rende rispondibile «che cosa è la stanchezza?» | M8 | M9 |
| rilettura | **la lettura parlava per l'utente** | M11 | M3 |

---

## 5. Banco dell'esperimento (held-out, LEARN_PROTOCOL §4.3)

Tutto in `make chat` o in `.p0t` **puntuali** sulla KB viva. Nessuna entità
inventata come conoscenza del mondo: «Marco» è stato di sessione riferito da un
terzo, non un fatto `W`, e non si salva.

| gruppo | turni | atteso nel modello (§3) |
|---|---|---|
| lezione / baseline | mi sento spossato | lacuna locale sul valore; lettura (con permesso) |
| replay (§6.3) | mi sento spossato | `holds(spossatezza, user, now)`, fonte `self_report` |
| transfer ×3 (§6.4) | «oggi sono molto spossata» · «da tre giorni mi sento spossato» · «sono spossato perché ho dormito poco» | grado alto + accordo femminile · esordio/durata · causa |
| parafrasi ×2 (§6.5) | «mi sento proprio senza energie» · «I feel exhausted» | stesso stato (inglese: via en.wikipedia se serve) |
| contrasto (§6.5) | «ieri ero spossato» · «non sono spossato» · «Marco è spossato» | non corrente · negato · soggetto Marco |
| composizione (§6.5) | «sono spossato ma voglio uscire» · con un piano già detto | contrasto stato/scopo · salienza sul piano |
| trasferimento concettuale (§0.11) | una pagina **reale** di it.wikipedia che definisca uno stato *per mezzo* di fatica/stanchezza (da scegliere e verificare al gate §4.2) | la definizione si legge usando il concetto appreso |
| ablazione (§6.6) | «dimentica che cosa vuol dire spossato» → replay → re-insegna | lo stato non si ricava più; poi torna |
| retention (§6.7) | dopo 5 turni diversi | come replay |
| persistenza (§10) | processo nuovo, rete **spenta** | «mi sento spossato» funziona senza rileggere |

**Controllo «stanco»:** stessi turni. Oggi il frasario risponde a due su sei; il
ciclo deve coprire tutti e sei per la stessa via di «spossato».

---

## 6. Le mosse, in ordine

Ogni mossa: che cosa, dove sta la conoscenza, quale porta C (se serve), come si
verifica **puntualmente** (un caso, confrontato con HEAD), che cosa conta nel
diff. Test: **solo casi puntuali**, niente suite né file interi (F.).

### Mossa 1 — la lettura non parla per l'utente ✅ (14 settembre 2026)

**Trovato con gdb** (backtrace su `conv_log_one`): `learn_from_prose` →
`extract_class_statement` → `brain_respond` sulla relativa ridotta («fatica of
stanchezza a insorgenza graduale»). Quel turno annidato passava da `turn_done`
come un turno di dialogo: entrava nel registro come `utterance(_, user, _)` e il
suo muro apriva `gap_offer` sul tabellone; al turno dopo
`pending_offer_fallthrough` leggeva «mi sento stanco» come un «sì».

**Cura KB-first:**

- `discourse.p0`: `inner_turn :- inner_turn_reason($Why)`, con le ragioni come
  righe — `reading` (da `reading_prose(1)`) e `repair` (da `repairing(1)`).
- `network.p0`: `unclaimed_turn_captured` aggiunge `naf(inner_turn)`.
- C, tre porte che chiedono o pubblicano: `conv_log_one` chiede `inner_turn` (al
  posto del solo `repairing`); `board_open` non apre questioni in un turno
  interno; `learn_from_prose` asserisce `reading_prose(1)` **solo** attorno al
  ciclo sulle frasi.

**Errore misurato e corretto:** la prima stesura legava la ragione a
`reading_now`, che copre tutta l'acquisizione. Chiudeva così anche il bivio
«Quale intendi?» dei PLC, che è una domanda vera all'utente (`offer_context.p0t`
riga 76: HEAD 4 passati, prima stesura 1). Con `reading_prose` il caso torna
identico a HEAD.

**Verifica (puntuale):**

| caso | HEAD | dopo |
|---|---|---|
| `definisci «stanchezza»` → `mi sento stanco` | «Ho cercato «insorgenza graduale»…» | «Non capisco ancora.» |
| tabellone dopo la lettura | `gap_offer` aperto | vuoto |
| `offer_context.p0t` blocco riga 76 | 4 ✓, 2 timeout | 4 ✓, 2 timeout |
| `gap_dialogue.p0t` blocco riga 57 | 2 ✓, 2 ✗ | identico |
| `dialogue_board.p0t` blocco riga 55 | 1 ✓, 6 ✗ | identico |
| `basics.p0t` antonym | timeout 1,17 s | timeout 1,19 s |

**Catena d'insegnabilità:** `inner_turn_reason/1` è una **riga a mano**, perché
nessuna superficie la produce. Resta aperto: la stessa perdita per le clausole
dei turni composti (`compound_turn_lead`), gli `exchange/3` e i `turn_topic`
scritti dal turno interno. Si registrano, non si curano qui.

### Mossa 2 — la forma dell'autodescrizione, con lo slot del valore

> **Stato al 14 settembre 2026 (sera): a metà.** Forma `state_self_report` in
> `kb/core/user-state.p0` e porta `turn_form_yield/3`. Misclaim aperto su «I feel
> exhausted». Handoff completo in `LEARN_TODO.md`.

- **Che cosa:** l'IR del §2.1-2.3 e 2.6 per costruzioni copulari e percettive,
  italiano e inglese; lo slot sconosciuto diventa lacuna `knowledge` locale.
- **Dove:** classi dei verbi di stato e delle costruzioni in KB; forma di turno
  (`turn_form/3`); la specie della lacuna in `gap-kinds.p0`.
- **Porta C:** nessuna prevista (la mossa prova prima in KB, procedura §4.3).
- **Frontiera:** M1 (dichiarazione dell'utente ≠ richiesta di vocabolario), R7.
- **Verifica puntuale:** «mi sento spossato», «Marco è spossato», «ieri ero
  spossato» → `/debug` mostra soggetto, tempo e lacuna sul solo valore.
- **Catena:** la lezione parlata che ricrea la forma («"sentirsi" dice come sta
  chi parla»); se non esiste, riga a mano registrata.

### Mossa 3 — lo stato come fatto con coordinate

- **Che cosa:** §3.1, §3.3, §3.4 per `self_report` e `third_party_report`;
  `user_value(mood, …)` diventa vista derivata.
- **Dove:** KB di sessione, `session_slot`/`turn_scratch`; ontologia delle
  dimensioni (§3.2) come classi.
- **Frontiera:** M1 (la stessa casa per «domani ho una riunione»), M11.
- **Verifica puntuale:** «sono stanco» (controllo) → «come mi sento?» →
  «perché?» · «ieri ero stanco» non cambia lo stato corrente.

### Mossa 4 — dal valore sconosciuto alla pagina, solo Wikipedia

- **Che cosa:** «spossato» → la voce di it.wikipedia. Due strade, entrambe
  dentro Wikipedia: la **ricerca dell'edizione** (`wiki_search_titles` esiste
  già) e la **derivazione** aggettivo → nome di stato come forma delle parole
  (catalogo N: *-ato → -atezza*, *-o → -ezza*), appresa parlando.
- **Muro noto:** i titoli di una lettera sola sono scartati in `learn.c`
  (`kn < 2`), e «a» è mangiata dai controlli su articoli e preposizioni in
  `mod_learn_turn` (sessione del 14 settembre, «definisci «a»»): la citazione è
  menzione, non uso (M2 di apprendimento-assistito).
- **Verifica puntuale:** «mi sento spossato» con politica `act` → `topic_read`
  con titolo e revisione di it.wikipedia.

### Mossa 5 — la prosa definitoria italiana, capita

- **Che cosa:** leggere la pagina *Fatica* senza ibridarla con l'inglese e
  ricavarne la classe (sensazione/stato), l'effetto (riduzione di energia), il
  rimedio (riposo), le specie (fisica, mentale). *riposo, riduzione, alleviata*
  smettono di essere opache.
- **Dove:** lessico italiano e forme di lettura in KB (M9 di
  apprendimento-assistito); **nessun** estrattore dedicato.
- **Verifica puntuale:** in `make chat`, dopo la lettura: «che cosa allevia la
  fatica?», «la fatica è uno stato?», «che cosa riduce la fatica?» — domande
  naturali (§6.3-6.5 del protocollo), non MCP.
- **Conteggio:** `/save`, diff, classificazione W/L/C/P/O/X = 0, processo nuovo.

### Mossa 6 — rilettura, ragionamento, risposta

- **Che cosa:** il turno che aveva la lacuna si rilegge dopo la lettura
  (`dispatch_one` dopo `network_acquire` esiste già nel muro con politica
  `act`); lo stato entra con i valori della pagina; la condotta del §3.5 decide
  la mossa.
- **Frontiera:** M4 (generazione sul contenuto: «poca energia», non «momento
  fiacco»), M6 (pertinenza), M8 (chiedere), R3 (salienza sul piano).

### Mossa 7 — banco completo, trasferimento, ablazione, persistenza

Il §5 intero, un caso alla volta, con i gate del protocollo (Transfer@3 = 3/3,
Paraphrase 2/2, Contrast 1/1, Composition 1/1, Ablation, Retention) e la prova
nel processo nuovo con la rete spenta.

---

## 7. Che cosa questo piano **non** chiude (residui di frontiera)

- **R1** — lo schema delle dimensioni (§3.2) è scritto da chi insegna; parrot0
  non se lo inventa ancora.
- **R2** — «un po'», «probabilmente», «credo»: il grado e la fiducia sono scale
  dichiarate, non una semantica della plausibilità.
- **R3** — la salienza sul piano funziona solo per relazioni derivabili.
- **R8** — invecchiamento e revisione dello stato sono regole semplici; la
  propagazione alle conseguenze già tratte no.
