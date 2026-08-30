# Perché non cresceva — diagnosi, e la strategia che ne discende

Data: 2026-08-31 · Richiesta di F.: *«cerca di capire cosa ci ha impedito di
avere risultati importanti e di crescita sostanziale; appena abbiamo idea dei
limiti facciamo una strategia per superarli e ripetere con più successo.»*

## 1. I tre giri, e i loro numeri

| giro | che cosa ha insegnato | delta su 360 turni |
|---|---|---|
| GD2 | 193 forme colloquiali, 8 classi, it+en | muri 236 → 226, match 117 → **128** |
| GD6 | 83 superfici interrogative **italiane**, 19 relazioni | muri 226 → **226**, match 128 → **128** |

GD6 ha prodotto **zero**. E non perché non funzioni: in prova diretta

```text
> Il gatto è sul tetto.        -> Learned: located_in(gatto, tetto).
> Dov'è il gatto?              -> Tetto.        (prima: muro)
> qual è la capitale dell'Italia -> Roma.
```

Una lezione, una riga, e la domanda italiana si apre. Sul corpus: niente.

**Quel «funziona in prova, zero sul corpus» è il dato più importante della
giornata**, e inseguirlo ha prodotto la diagnosi.

## 2. La diagnosi: si rompono quattro anelli, non uno

Ho smesso di guardare gli aggregati e ho tracciato **un solo dialogo banale**,
turno per turno. In trenta secondi sono comparsi quattro guasti indipendenti:

```text
> Ho messo il libro rosso sul tavolo.      -> muro      (1)
> Il quaderno blu invece è nello zaino.    -> muro      (2)
> Il libro rosso è sul tavolo.             -> Learned: located_in(book_red, tavolo)
> dove si trova il libro rosso             -> muro      (3)  ← il fatto C'È
> Dov'è il primo?                          -> muro      (4)
```

1. **Transitivo con complemento**: «ho messo X sul Y» non ha lettura (tre ruoli,
   agente eliso in italiano).
2. **Connettivo e preposizione articolata**: «invece», «nello» disfano la lettura.
3. **Il fatto esiste e non è raggiungibile.**
4. **Riferimento cross-turn** («il primo»): nessuna risoluzione.

## 3. Il guasto numero 3, isolato — ed è il più grave

```text
> Il libro rosso è sul tavolo.     -> Learned: located_in(book_red, tavolo).
> dove si trova il libro rosso     -> muro
> dove si trova book_red           -> Tavolo.        ← funziona SOLO col nome interno
> dove si trova libro rosso        -> muro

> Il gatto è sul tetto.            -> Learned: located_in(gatto, tetto).
> dove si trova il gatto           -> Tetto.         ← una parola sola: funziona
```

Un'entità di **una** parola fa il giro completo. Un'entità di **più** parole no:
la lettura canonicalizza «il libro rosso» in `book_red` — tradotto *e*
riordinato — e il percorso della domanda non applica la stessa trasformazione.
L'unico modo per recuperare quel fatto è pronunciare `book_red`, un nome che
nessun essere umano digiterebbe.

> **parrot0 impara sotto un nome che non sa più pronunciare.**

Ogni fatto appreso da prosa italiana su qualcosa che si chiama con più di una
parola finisce in un cassetto **senza maniglia**.

## 4. Perché questo spiega tutti i numeri

- **GD2 +11**: le interiezioni non hanno bisogno del giro dell'entità, quindi il
  guadagno si è visto — piccolo ma reale, e concentrato dove doveva (F01, +29%).
- **GD6 +0**: ho aperto le porte interrogative *italiane*, ma le domande del
  corpus riguardano entità multi-parola introdotte nei turni precedenti. La
  porta si apre su una stanza il cui indirizzo non combacia.
- **F03 coref è la famiglia peggiore (24/30)**: la coreferenza richiede che
  l'entità sia *nominabile*, e non lo è.
- Il corpus è pieno di referenti multi-parola: «il libro rosso», «il quaderno
  blu», «il treno notturno». Sono la norma del parlato, non un caso limite.

## 5. Che cosa ci ha impedito la crescita, detto in una riga

> **Ho misurato aggregati e insegnato classi, senza mai tracciare un turno da
> capo a fondo.**

Il corpus misura **congiunzioni**: un turno riesce solo se tengono insieme
superficie, forma della domanda, nome dell'entità, riferimento, fatto e
realizzazione. Riparare un congiunto alla volta muove ~zero, perché il turno
continua a fallire su un altro. Due giri, 276 forme insegnate, +11 turni: non è
il metodo che è debole — è che **fissavo un anello di una catena**.

E l'aggregato lo nascondeva: 66% di muri sembra «serve più conoscenza», mentre
la traccia di un dialogo solo dice «servono quattro cose diverse, e una è un bug
di simmetria».

## 6. La strategia per ripetere con più successo

**Regola d'ingaggio, prima di qualunque giro di insegnamento:**

1. **Traccia una catena intera prima di insegnare.** Prendi *un* dialogo del
   corpus, eseguilo turno per turno, e scrivi dove si rompe. Un aggregato non
   dice mai dove.
2. **Ripara la catena più corta che chiude una famiglia**, non il difetto più
   evidente. Il criterio non è «quanti muri tocca» ma «quanti anelli restano
   dopo».
3. **Insegnare viene per ultimo.** Il lessico moltiplica una catena che chiude;
   su una catena rotta è rumore misurabile a +3%.

**Ordine dei lavori che ne discende** — dal collo verso l'esterno:

| # | lavoro | perché prima |
|---|---|---|
| **GD7** | **Round-trip del nome dell'entità**: ciò che si impara da una frase deve essere interrogabile *con la stessa frase* | sblocca il congiunto che oggi rompe ogni catena multi-parola. Senza, tutto il resto resta irraggiungibile |
| **GD4** | riferimento cross-turn | seconda per impatto (24 muri), e richiede GD7 per avere qualcosa da nominare |
| **GD8** | frase ordinaria a tre ruoli («ho messo X su Y») + preposizioni articolate | è la forma normale del parlato, non un caso limite |
| GD3 | famiglia di varianti ortografiche | moltiplicatore, ma solo dopo che una catena chiude |

**Come misurare che la strategia funziona.** Non con il totale: con una
**famiglia chiusa**. Il gate di GD7+GD4 è che il dialogo `gd1_011` — cinque turni,
due oggetti, un riferimento — passi **da capo a fondo**. Una famiglia che chiude
vale più di dieci punti percentuali sparsi, perché prova che la catena regge.

## 7. Che cosa resta acquisito

I due giri non si buttano: 193 forme colloquiali e 83 superfici interrogative
italiane sono conoscenza vera, verificata in processo nuovo, e diventano
**moltiplicatore** appena la catena chiude. GD6 in particolare è pronto ad
accendersi: le porte ci sono già, manca l'indirizzo.

E un risultato di metodo: parrot0 ha **rifiutato** cinque lezioni su
`source_of` — «non conosco nessuna relazione chiamata source_of, quindi un modo
per chiederla si aprirebbe su una stanza vuota». Ha detto no a una porta senza
stanza, che è esattamente la guardia giusta.
