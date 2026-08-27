# Sessione 01 — il metalinguaggio verificato da un guadagno di conoscenza

**Data:** 2026-08-27
**Natura:** sessione di apprendimento, non un test end-to-end
**Esito:** cinque costruzioni e sette fatti veri entrati nella KB del repository

La demo precedente (`demo-05min.md`) usava entità inventate. Serviva a misurare
la meccanica, ma non produceva nulla: `torvo` non è conoscenza. Questa sessione
cambia il criterio — **si insegnano cose vere, e il metalinguaggio si giudica da
ciò che la KB guadagna**.

## 1. Che cosa è stato insegnato

Parafrasi inglesi reali, ciascuna ancorata a un frame che parrot0 già capiva:

| Lezione | Frame bersaglio | Nota |
|---|---|---|
| `x lies in y means x is located in y` | `located_in` | diretta |
| `x is home to y means y is located in x` | `located_in` | **inverte i ruoli** |
| `x is bordered by y means y borders x` | `borders` | **inverte i ruoli** |
| `x revolves around y means x orbits y` | `orbits` | diretta |
| `x is the birthplace of y means y was born in x` | `born_in` | **inverte i ruoli** |

E una menzione, su una parola-funzione vera: `the word whereas is a
conjunction` — classe che il parser della coordinazione consuma davvero.

## 2. Che cosa la KB ha guadagnato

Fatti veri prodotti *dalle lezioni*, instradati da `/save` nell'albero curato:

```prolog
located_in(bergamo, lombardy).
orbits(europa, jupiter).      orbits(callisto, jupiter).
orbits(titan, saturn).
born_in(christopher_columbus, genoa).
born_in(galileo_galilei, pisa).
conjunction(whereas).
```

Le costruzioni hanno una casa nuova, `kb/learning/constructions.p0`.
`kb/learning/learned.p0` dice che quando la ricaduta cresce la risposta non è
ordinarla ma creare la categoria mancante: questa è quella categoria, e il
save-map ora vi instrada ogni `construction_frame/3` appreso.

**La prova che conta** è un processo nuovo, che legge soltanto la KB del
repository e non ha visto la sessione:

```text
> Naples lies in Campania
< Learned: located_in(naples, campania).

> where is bergamo?
< Lombardy.
```

Nessuno ha ripetuto la lezione. La costruzione è conoscenza, non stato di
sessione.

## 3. Il difetto che solo la conoscenza vera poteva mostrare

Il primo giro della sessione è fallito, e il modo in cui è fallito vale più
delle lezioni riuscite:

```text
> x lies in y means x is located in y
< I learned a candidate construction: @S lies in @O maps to @S is located in @O.

> Siena lies in Tuscany
< Italy.

> Europa revolves around Jupiter
< Sun.
```

La lezione risultava appresa e non produceva **niente**. Su `Siena` e su
`Europa` un consumer che sa già qualcosa prende il turno e risponde con quello
che sa; la costruzione appena insegnata non viene mai applicata.

Con entità inventate il difetto è invisibile — nessun altro modulo ha qualcosa
da dire su `torvo`, quindi la lezione vince sempre. **Il metalinguaggio
sembrava funzionare perché veniva provato dove non c'era concorrenza.** È il
limite L2 della prima sessione di profondità, e si vede solo insegnando cose
vere.

La correzione è M1: ciò che una lezione ha reso leggibile si legge prima che un
modulo generico risponda, e solo per i pattern nati da una lezione. Dopo:

```text
> Siena lies in Tuscany
< Learned: located_in(siena, tuscany).
> Europa revolves around Jupiter
< Learned: orbits(europa, jupiter).
```

Una regressione trovata subito dopo, e conservata perché insegna: su un `read:`
multi-frase lo stesso modulo faceva combaciare il pattern attraverso l'intero
testo e produceva `orbits(read_tari, luma_nova_shimmers_brightly)` — un fatto
falso al posto di due veri. Legge una frase sola.

## 4. Quello che ancora non funziona

**La regola relazionale insegnata è un misclaim.** Vero e utile da imparare:
«se x è la capitale di y allora x si trova in y». Il risultato:

```text
> if x is the capital of y then x is located in y
< Learned rule: located($V1) :- capital($V1, $V2).
```

Le relazioni `capital_of/2` e `located_in/2` diventano `capital/2` e
`located/1`: la regola è inutile, e viene annunciata come appresa. È M8 —
rappresentazioni equivalenti che non si ricongiungono — dentro l'atto didattico
delle regole.

**Il numero non passa il cancello dei concetti.**

```text
> The University of Bologna was established in 1088
< Scartato: founded_in(university_of_bologna, 1088) non e' fatto di concetti.
```

Il rifiuto è onesto e visibile, ma blocca una classe intera di fatti veri —
fondazioni, date, quantità — che la costruzione insegnata saprebbe estrarre.

**La domanda sull'altro slot sbaglia lettura.** `what revolves around saturn?`
risponde `Sun`: la superficie interrogabile aperta dalla lezione non regge la
domanda inversa. E `where was galileo galilei born?` mura su `galileo`, perché
l'entità è stata memorizzata come `galileo_galilei` e la domanda non la
ricompone.

**`/save` instrada nell'albero curato anche fatti di un altro ordine.** Sono
finiti nei file curati `turn_counter(18)` — un contatore di runtime — e un
`gap_source/2` del registro di lavoro, accanto ai `gap_source/3` semantici.
Tolti a mano.

Ma chiamarlo «rumore» sarebbe una conclusione affrettata, e per ora è vietato
automatizzarne la rimozione: quello che oggi sembra scarto può essere conoscenza
di ordine superiore — il registro della conversazione, la provenienza delle
letture e i gap aperti sono già dichiarati conoscenza altrove, e M14 si
costruisce proprio su di essi. L'ipotesi di lavoro è che manchi la **casa**, non
che avanzi il fatto: è la stessa storia di `construction_frame/3`, che finiva
nella ricaduta finché non ha avuto `kb/learning/constructions.p0`. La questione
è aperta in
[`session-and-provenance.md` §6](../../session-and-provenance.md#6-il-rumore-di-sessione--questione-aperta-da-non-chiudere-con-un-filtro).

## 5. Bilancio

Il metalinguaggio regge su conoscenza vera: cinque costruzioni, tre delle quali
invertono i ruoli, producono fatti veri che sopravvivono alla sessione e
funzionano in un processo nuovo. Il guadagno è piccolo e reale.

Quello che la sessione ha mostrato di più importante non è il guadagno: è che
**provare il metalinguaggio su entità inventate lo faceva sembrare più chiuso di
quanto fosse.** Le prossime sessioni si fanno così — su cose vere, con `/save`,
e con il diff della KB letto riga per riga prima di committare.
