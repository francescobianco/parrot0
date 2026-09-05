# Tre solfuri di rame appresi e richiamati

Stato: `trained`, limitatamente a tre fatti ground; nessuna nuova costruzione
generale rivendicata. Checkpoint successivo a `6276cb2`.

## Parametri e fonti

Dominio: minerali solfuri di rame. Obiettivo: tre nuove appartenenze accessibili
con domanda, prova ed enumerazione. Budget: tre lezioni, con un retract/reteach.
Target: `W=3`, `C=0`. Stop condition: primo misclaim non spiegato o candidato
invalido nel dump; chiusura anticipata richiesta dall'operatore dopo le prove.

Fonti aperte e verificate il 2026-09-05, *Handbook of Mineralogy*, Mineral Data
Publishing, versione 1 (2001–2005):

| Lezione | Fonte e sostegno |
|---|---|
| bornite is a copper sulfide mineral | [Bornite](https://www.handbookofmineralogy.org/pdfs/bornite.pdf): composizione Cu5FeS4 e descrizione mineralogica. |
| chalcocite is a copper sulfide mineral | [Chalcocite](https://www.handbookofmineralogy.org/pdfs/chalcocite.pdf): composizione Cu2S e descrizione mineralogica. |
| covellite is a copper sulfide mineral | [Covellite](https://www.handbookofmineralogy.org/pdfs/covellite.pdf): composizione CuS e descrizione mineralogica. |

La classificazione insegnata e' la sintesi delle composizioni e delle schede;
non e' una citazione verbatim. Non si insegna che tutti questi minerali siano
economicamente estraibili in qualunque deposito, ne' che l'elenco sia completo.

## Sessione naturale

Profilo AGI, KB completa. `B0/R0=38528/2781`. Il binario includeva la correzione
di `6276cb2`; il timbro mostrava ancora `f5f66be` perche' il commit era avvenuto
dopo la build. Prima della verifica fresca e' stato rigenerato con `make build`.

Baseline prima di ciascuna lezione, due formulazioni per minerale: domanda di
appartenenza e richiesta di prova. Bornite: «I don't know about copper sulfide
mineral.»; per gli altri, con la classe gia' popolata, ignoranza esplicita e
distinzione fra assenza di prova e falsita'. Tutte le prove: «I can't show that.».

Le tre lezioni della tabella sono state dette una alla volta, nella stessa
sessione delle verifiche. Conferme: «Learned: bornite/chalcocite/covellite is a
copper sulfide mineral.», ciascuna con il proprio soggetto.

Turni successivi rilevanti, verbatim:

```text
> why is bornite a copper sulfide mineral?
I hold that bornite is a copper sulfide mineral.
> is bornite a copper sulfide mineral?
Yes.
> how do you know chalcocite is a copper sulfide mineral?
Directly: chalcocite is a copper sulfide mineral — no reasoning needed.
> why is covellite a copper sulfide mineral?
I hold that covellite is a copper sulfide mineral.
> what are the copper sulfide minerals?
bornite, chalcocite, covellite.
> what do you know about copper sulfide mineral?
What I hold as copper sulfide mineral: bornite, chalcocite, covellite.
> is hematite a copper sulfide mineral?
I don't know: nothing I hold says hematite is a copper sulfide mineral, and nothing says it isn't. Knowing some copper sulfide minerals does not tell me they are all of them, so not proved is not the same as false. Tell me either way and I will hold it.
> forget bornite is a copper sulfide mineral
I no longer treat «bornite» as a copper sulfide mineral.
> why is bornite a copper sulfide mineral?
I can't show that.
> what are the copper sulfide minerals?
chalcocite, covellite.
> bornite is a copper sulfide mineral
Learned: bornite is a copper sulfide mineral.
> how do you know bornite is a copper sulfide mineral?
Directly: bornite is a copper sulfide mineral — no reasoning needed.
```

Il contrasto misura l'assenza di una certezza inventata; non rivendica una nuova
capacita' di dedurre il no. L'enumerazione dopo retract conserva i due sostegni
indipendenti. Bornite e' stata reinsegnata prima di salvare.

## Persistenza e riconciliazione

Dump ispezionato interamente: `/tmp/parrot0-session-2.p0`, 66 righe.
Conteneva i tre fatti, la superficie della classe, i sostegni, i turni e lo
stato conversazionale. Nessuna regola indotta o fatto artificiale.

```text
> /save
parrot0: routed 55 clause(s) into the KB tree
```

Diff letto riga per riga, solo aggiunte:

| Categoria | Numero | Contenuto |
|---|---:|---|
| W | 3 | Tre appartenenze `copper_sulfide_mineral`. |
| L | 1 | `class_surface`, derivata dalla lezione, non un quarto fatto del mondo. |
| C | 0 | Nessuna nuova regola/procedura/costruzione. |
| P | 6 | Tre `fact_source` e tre `reading_fact`. |
| O | 45 | 41 `utterance`, tre `discourse_referent`, un `turn_goal`. |
| X | 0 | Nessuna clausola falsa, artificiale o inspiegata. |

Nuovi fatti veri del mondo salvati in KB: **3**.
Nuove clausole totali salvate e classificate: **55**. `S=55`.
I 42 turni hanno 41 nuove clausole di transcript: il «Yes.» al turno 10 era
gia' presente. I token e il contatore transitori del dump non aggiungono
clausole al diff. Nessun filtro manuale applicato.

File KB: `kb/learning/learned.p0` (+8),
`kb/machinery/fact-provenance.p0` (+6), `kb/machinery/transcripts.p0` (+41).

Processo nuovo dopo `make build`, senza reinsegnamento:
`B1/R1=38583/2781`, timbro `6276cb2`. Le cinque domande sono quelle di prova
su bornite, provenienza su chalcocite, appartenenza di covellite, enumerazione
plurale e resoconto della classe. Risposte corrette **5/5**, con **3/3** fatti
richiamati. L'elenco fresco e' «chalcocite, covellite, bornite.»: l'ordine
riflette il retract/reteach, l'insieme resta lo stesso.

## Metriche, limiti e handoff

LessonYield `3/3`; replay e uso dei tre fatti `3/3`; due forme di enumerazione
`2/2`; contrasto epistemico `1/1`; ablation/reteach `1/1`; retention dopo piu'
di cinque turni osservata prima del retract. FreshProcessRecall `5/5`.
FalseUnderstandingRate `0/4` conferme, contando il reteach.
Transfer@3 di una nuova costruzione: **non applicabile**, `C=0`. Non si
scambiano tre esempi della stessa appartenenza per apprendimento di una regola.

Restano i soggetti multi-parola nelle domande di prova, la derivazione della
classe piu' generale da quella composta e la fonte esterna raggiungibile in
dialogo: `fact_source` conserva la lezione, le URL verificate stanno in questo
report. `class_surface` si salva ancora senza virgolette e `turn_goal` corrente
finisce nella ricaduta; sono debiti noti di rappresentazione/routing, non
ragioni per eliminare tracce. Nessuna suite eseguita.
