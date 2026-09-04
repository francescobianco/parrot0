# Demo guidata — parrot0 legge una codebase e la giudica

Una codebase finta ma **vera**: due file C che si comportano come codice reale,
non come fixture didattiche.

```
src/cache.c    una hash table con linear probing — tre funzioni sane
src/report.c   un builder che fa troppe cose — una funzione con 10 chiamate
```

## Come si avvia

Dalla radice del repository:

```sh
make demo
```

(Equivale a entrare in `tests/fixtures/demo-code-review/` e lanciare `parrot0`
con gli strumenti locali accesi. Dal gen493 parrot0 trova la propria KB da
qualunque cartella, quindi **si lancia dove sta il codice da analizzare**, non
dove sta parrot0.)

---

## I prompt, in ordine. Copiali uno per volta.

### 1. Leggere — non «cercare», leggere

```
what functions does src/cache.c define
what functions does src/report.c define
```

> I read src/cache.c into structure: it defines hash_table_slot, hash_table_get
> and hash_table_put. (3 functions now in the KB.)

Da qui in poi il file non viene più riaperto: quello che resta è **struttura in
KB** — nodi, span, hash della sorgente, provenienza. La differenza si vede al
prompt 6.

### 2. ⭐ Il momento che conta: conoscenza che attraversa il confine

```
domain knowledge about hash_table_get
```

> Key-value mapping -- O(1) average lookup/insert/delete.

**Quella frase non è mai stata scritta per parlare di questo codice.** Vive in
`kb/experts/programming/algo.p0` come conoscenza di dominio sulle strutture
dati. parrot0 l'ha raggiunta perché l'identificatore `hash_table_get` contiene
il concetto `hash_table`, e un **ponte fra rappresentazioni** lascia passare la
domanda dal lato codice al lato dominio.

Non c'è nessuna tabella `hash_table_get → …`. C'è un ponte dichiarato, e la
stessa conoscenza serve due rappresentazioni senza essere duplicata.

### 3. Un giudizio, con la sua evidenza

```
improvement note for build_report
improvement note for render_summary
```

> Wide fanout — code definition fanout = 10.
> No finding: nothing measured crosses a declared threshold.

Il primo è un finding **misurato** (dieci chiamate contate, non stimate) contro
una **soglia dichiarata in KB**. Il secondo è la cosa che quasi nessuno strumento
dice: *ho guardato e non ho trovato niente* — che è un risultato, non una lacuna.

Non c'è nessuna libreria di smell. C'è un fatto:

```prolog
quality_criterion(wide_fanout, coupling, threshold(code_definition_fanout, above, 8)).
```

dove `code_definition_fanout` è il **nome di una relazione**, non una funzione
compilata. Un criterio nuovo è una relazione più un fatto — zero C.

### 4. ⭐ L'onestà, che vale quanto le risposte

```
speed of build_report
cost candidate for build_report
```

> No profile on record -- from structure alone I can name a costly candidate,
> never a slow part.
> 10.

parrot0 **si rifiuta di indovinare**. Dalla sola sintassi si può nominare un
candidato costoso; dire «questa parte è lenta» richiede evidenza dinamica, e
senza profilo quella evidenza non c'è. Un errore detto con sicurezza è peggio di
un muro.

### 5. Dove sta, e come lo sa

```
definition source of build_report
source evidence for build_report
```

> Src/report.c.
> Evidence("src/report.c", fnv1a64 bbb42dafab4e8d1d, span(315, 12), builtin c scan v1).

File, hash della sorgente, span esatto, e quale frontend l'ha osservato. Ogni
affermazione porta la propria prova: se il file cambia, l'hash cambia e la vecchia
osservazione non è più dimostrabile.

### 6. ⭐ E adesso glielo insegni tu

```
should i refactor build_report
```

> Hmm, I don't know about refactor yet. …

Non conosce la forma. Insegnagliela **parlando**:

```
learn "should i refactor" as another way to ask "improvement note for build_report"
```

> Got it - «should i refactor» now asks the same thing as «improvement note for build_report».

E ora:

```
should i refactor build_report
should i refactor render_summary
```

> Wide fanout — code definition fanout = 10.
> No finding: nothing measured crosses a declared threshold.

**Guarda la seconda riga.** `render_summary` non compare da nessuna parte nella
lezione: la forma è stata insegnata su un esempio e vale su un soggetto mai
nominato. Se fosse un frasario, non trasferirebbe. Nessuna ricompilazione,
nessun riavvio.

---

## Che cosa hai appena visto, in una riga

Una codebase letta in una **IR interrogabile con provenienza**; una domanda che
**attraversa il confine di rappresentazione** e raggiunge conoscenza imparata
altrove; un giudizio qualitativo che è un **motore con soglie dichiarate**, non
un catalogo; un rifiuto onesto dove manca l'evidenza; e una forma nuova
**insegnata parlando**, che trasferisce.

## Che cosa NON hai visto, e va detto

- **Non modifica il codice.** Questa è la parte read-only: capire e giudicare.
  Patch e repair stanno in altri piani.
- **Le parafrasi sono quelle insegnate.** Non c'è comprensione libera del
  linguaggio sulle domande di codice: c'è un consumer universale a cui si
  aggiungono forme, come al prompt 6.
- **Due criteri, non venti.** Bastano a dimostrare il meccanismo; il terzo non
  aggiunge comprensione, aggiunge una riga.
- **Severità e ordinamento fra finding non ci sono ancora**, né l'azione che
  separa due ipotesi concorrenti. Sono la prossima tappa del piano.

Il piano completo: [`docs/plans/universal-code-comprehension.md`](../../../docs/plans/universal-code-comprehension.md).
