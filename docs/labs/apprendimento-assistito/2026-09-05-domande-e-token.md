# Domande di definizione, prove e token completi

Stato: `meta-capability-only`, checkpoint di sviluppo dalla prima voce di
`LEARN_TODO.md`. Base `f5f66be`; worktree inizialmente pulito.
Tutte le conversazioni usano la KB completa con
`PARROT0_PROFILE=kb/profiles/agi.p0`: 38528 fatti, 2781 regole al boot.
Nessuna suite eseguita, come richiesto dall'handoff. `make build` riuscito.

## Il primo arresto

```text
> why is hematite an iron oxide mineral?
hematite is an iron oxide mineral; hematite is an iron ore mineral.
> why is socrates a mortal?
socrates is a man; socrates is a philosopher; socrates is a humanities_topic.
```

Il blocco gen151 delle definizioni chiedeva `question_word` e una copula.
Accettava dunque anche una richiesta di prova, prima che il ramo della prova
potesse vederla. Correzione: consultare `definition_interrogative`, gia' in KB.
Nessun nuovo elenco di parole, guardia di cessione o riordino del dispatch.

Un secondo difetto era strutturale: `char *w[8]`. `split_words` lascia tutta la
coda nell'ultimo token quando raggiunge la capacita'. Il risultato puo' contenere
spazi dentro un presunto token, rendendo incoerenti le chiavi costruite da code
multi-parola. Il vettore e' ora dimensionato sull'intero buffer: parole di un
byte separate da uno spazio rappresentano il massimo possibile. Nessun testo
gia' accettato da quel buffer puo' saturare il vettore.

## Risposte dopo la correzione

```text
> why is hematite an iron oxide mineral?
I hold that hematite is an iron oxide mineral.
> how do you know hematite is an iron oxide mineral?
Directly: hematite is an iron oxide mineral — no reasoning needed.
> why is socrates a mortal?
mortal(socrates) because man(socrates).
> what is hematite?
hematite is an iron oxide mineral; hematite is an iron ore mineral.
```

Composizione, nello stesso processo della lezione diagnostica:

```text
> every iron oxide mineral is an oxide mineral
Learned rule: oxide_mineral(X) :- iron_oxide_mineral(X).
> why is hematite an oxide mineral?
hematite is an oxide mineral because hematite is an iron oxide mineral.
> why is magnetite an oxide mineral?
magnetite is an oxide mineral because magnetite is an iron oxide mineral.
> what are the iron oxide minerals?
hematite, magnetite.
> why is quartz an oxide mineral?
I can't show that.
> what do you know about copper mineral?
What I hold as copper mineral: chalcopyrite.
```

La conferma della regola era seguita dall'annuncio preesistente di sette regole
indotte, anche su domini estranei: `chess_rank` da `is_prime`, `philosopher` da
`man`, `humanities_topic` da `man`, `philosopher` e `anthropologist`,
`go_edge_intersection` da `go_corner_intersection`, `iron_ore_mineral` da
`iron_oxide_mineral`. Non si promuovono queste induzioni come verita' verificate.

## Il ruolo resta insegnabile e ritrattabile

Prova di sviluppo della meccanica lessicale, non addestramento di una lingua:

```text
> que is the hematite?
I don't understand that yet.
> the word que is a definition interrogative
Learned: que is a definition_interrogative.
> que is the hematite?
hematite is an iron oxide mineral; hematite is an iron ore mineral.
> forget the word que is a definition interrogative
I no longer treat «que» as a definition interrogative.
> que is the hematite?
I don't understand that yet.
> the word que is a definition interrogative
Learned: que is a definition_interrogative.
> que is the magnetite?
magnetite is an iron oxide mineral; magnetite is an iron ore mineral.
```

Il binario non cambia tra prima, lezione, retract e reteach. L'atto didattico
usa menzione e nome grammaticale naturale; nessuna API o sintassi P0 in chat.

## Limiti e conteggi

Il checkpoint ripara l'accesso a capacita' esistenti; non chiude la famiglia dei
conteggi fissi. Restano il soggetto a posizione fissa nelle domande di prova,
le rese interne di classi prive di superficie e l'accesso al no da «why is …»:
su siderite/iron oxide mineral risponde ancora «I can't show that.».

Nuovi fatti veri del mondo salvati: `W=0`. Clausole salvate e classificate: `0`.
`/save` non eseguito; `C=0`, `X=0` nel diff promosso. Nessun file KB modificato.
LessonYield e FreshProcessRecall di nuove lezioni: non applicabili. La prova
di ruolo ha add/retract/reteach riusciti; il trasferimento della spiegazione e'
osservato su due minerali e sul dominio logico di Socrate. Questo non equivale
al gate generale Transfer@3 del protocollo di training.
