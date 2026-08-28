# docs/llmscores — la collezione dei prompt LLMSCORE


**531 prompt** estratti da **48 revisioni storiche** di `LLMSCORE.md` più le sette
sonde tematiche di `tests/llmscore-probes/`. Di questi, **315 sono muri** — prompt
su cui parrot0 ha preso 0 da un giudice esterno — e 146 sono già stati vinti.

## Perché esiste

I report LLMSCORE vivevano uno alla volta: ogni run riscriveva `LLMSCORE.md` e le
venti domande precedenti restavano solo nella storia di git. Erano il materiale
più prezioso che il progetto produce — *fallimenti reali davanti a un giudice
esterno* — e non erano consultabili. Questa cartella li rimette insieme.

## Come si usa

È il **serbatoio da cui pescare i temi** di [`LEARN_TODO.md`](../../LEARN_TODO.md).
Il ciclo:

1. si apre il file del tema su cui si vuole lavorare;
2. si prendono alcuni **muri** della stessa famiglia — non uno solo: un prompt
   chiuso da solo è frasario, una famiglia chiusa è una capacità;
3. si controlla la baseline in chat, perché un muro di sei mesi fa può essere
   già caduto;
4. si esegue [`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md) sul tema, non sul
   singolo prompt;
5. i «già vinti» servono come **controllo di non regressione**: se uno di quelli
   torna a murare, è un tema da riapprendere (sezione P4 di LEARN_TODO).

## Avvertenze

- **Un prompt non è un test.** Chiudere l'istanza senza chiudere la classe non
  conta come progresso (mantra #1). Questi file sono un serbatoio di *temi*, non
  una suite.
- **I voti sono storici.** Portano la data del report da cui vengono; il
  comportamento di oggi va verificato, non dedotto.
- **Non tutti i muri sono uguali.** Alcuni chiedono conoscenza che parrot0
  dovrebbe banalmente avere («Who was the first President of the United
  States?»), altri chiedono capacità che non ha ancora («Write a short poem…»).
  I primi sono P0/P2 di LEARN_TODO, i secondi P1/P3.
- **Le domande di identità sono fuori gioco**, come dice `LLMSCORE.md`: parrot0
  non è un LLM e la regola anti-inganno gli vieta di fingersi tale.

## I file

| # | Tema | Prompt | Muri | Vinti |
|---|---|---:|---:|---:|
| 01 | [generale-e-misto](01-generale-e-misto.md) | 151 | 92 | 39 |
| 02 | [linguaggio-e-lessico](02-linguaggio-e-lessico.md) | 75 | 45 | 19 |
| 03 | [aritmetica-e-conteggio](03-aritmetica-e-conteggio.md) | 67 | 25 | 33 |
| 04 | [creativita-e-scrittura](04-creativita-e-scrittura.md) | 55 | 33 | 17 |
| 05 | [geografia](05-geografia.md) | 45 | 26 | 15 |
| 06 | [spiegazione-causale](06-spiegazione-causale.md) | 24 | 13 | 2 |
| 07 | [arti-e-cultura](07-arti-e-cultura.md) | 22 | 16 | 5 |
| 08 | [logica-e-deduzione](08-logica-e-deduzione.md) | 18 | 12 | 4 |
| 09 | [design-e-progetto](09-design-e-progetto.md) | 14 | 14 | 0 |
| 10 | [preferenze-e-ipotetiche](10-preferenze-e-ipotetiche.md) | 11 | 4 | 6 |
| 11 | [storia](11-storia.md) | 11 | 5 | 3 |
| 12 | [scienza-e-natura](12-scienza-e-natura.md) | 10 | 8 | 0 |
| 13 | [meta-e-limiti](13-meta-e-limiti.md) | 6 | 3 | 2 |
| 14 | [procedure-e-istruzioni](14-procedure-e-istruzioni.md) | 5 | 5 | 0 |
| 15 | [sociale-e-dialogo](15-sociale-e-dialogo.md) | 5 | 3 | 0 |
| 16 | [affari-e-pratica](16-affari-e-pratica.md) | 4 | 4 | 0 |
| 17 | [enigmi-e-indovinelli](17-enigmi-e-indovinelli.md) | 4 | 3 | 1 |
| 18 | [codice](18-codice.md) | 2 | 2 | 0 |
| 19 | [etica-e-giudizio](19-etica-e-giudizio.md) | 1 | 1 | 0 |
| 20 | [relazioni-di-famiglia](20-relazioni-di-famiglia.md) | 1 | 1 | 0 |
| | **totale** | **531** | **315** | **146** |

## Da dove vengono

- `LLMSCORE.md`, 48 revisioni in git — ogni run ne conteneva 20, con la risposta
  di parrot0 e la motivazione del giudice. Qui è conservata la sola domanda: le
  risposte sono storiche e invecchiano, la domanda no.
- `tests/llmscore-probes/*.txt` — le sette sonde tematiche già organizzate nel
  repository (aritmetica, causale, creativa, deduzione, fattuale, lessicale,
  pragmatica), incluse come «sonde non votate».

L'estrazione è riproducibile: le domande stanno nelle tabelle Markdown dei
report, e ogni revisione di `LLMSCORE.md` è ancora in `git log --all`.
