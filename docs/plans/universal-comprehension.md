# Comprensione universale

*Estensione operativa del manifesto [kb-first](kb-first.md): parrot0 non dovrebbe
mai murare una frase **ben formata** con un "non capisco" cieco. Anche quando le
parole sono ignote, la **struttura grammaticale** è estraibile, e da quella si
legge l'**intento**. La grammatica è motore fisso; le strutture e i ruoli vivono
nella KB.*

---

## 1. L'idea (gli appunti, sistemati)

In regime di comprensione universale vorremmo che **non esistano frasi su cui
parrot0 va a muro** per pura incomprensione. L'ipotesi:

- parrot0 sa **estrarre grammaticalmente la struttura di qualsiasi frase**;
- le **strutture** (schemi sintattici, ruoli, schemi d'intento) vivono **nella
  KB**, non cablate nel C;
- così una frase **ben formata ma con parole sconosciute** può comunque essere
  **interpretata nel suo intento**;
- è un *meccanismo analitico*: la lettura del significato si appoggia alla KB.

In una riga: **capire la forma è sempre possibile; la forma rivela l'intento;
l'intento dice cosa servirebbe sapere per rispondere.**

## 2. La distinzione che rende l'idea sana (e onesta)

L'idea così com'è rischia di collidere con la regola d'onestà dei PRINCIPLES
("declina quando la catena non regge"). La riconciliazione è una **distinzione
netta**, ed è il cuore della proposta:

> **Comprendere la FORMA ≠ saper RISPONDERE.**
> La comprensione strutturale è (quasi) sempre possibile. La risposta resta
> *gated* dalla conoscenza nella KB.

Quindi "non andare a muro" **non** significa "rispondere sempre" (sarebbe
l'impostore). Significa **eliminare il muro cieco** `"I don't understand that
yet."` e sostituirlo con un **declino informato** che *dimostra* di aver capito
la domanda e nomina esattamente l'anello mancante:

```
Q: What's the capital of Zembla?
muro cieco (oggi):   I don't understand that yet.
declino informato:   You're asking for the capital of a country (Zembla),
                     but I have no fact about Zembla yet.
```

Il declino informato è ancora un declino onesto — ma è *competente*: prova che la
struttura è stata letta. È lo stesso salto di qualità di gen237 (l'opposto), solo
generalizzato: il motore riconosce **la domanda** anche quando non possiede **il
dato**.

## 3. Perché è coerente col progetto

- **Engine fixed, lexicon learns.** La grammatica (estrazione struttura + ruoli)
  è un motore fisso; gli schemi sintattici, le parti del discorso e gli schemi
  d'intento sono **fatti KB** (come già `conjunction/1`, `stopword/1`,
  `intent_phrase/2`, `intent_cue/2`, `social_marker/2`). Aggiungere uno schema =
  un fatto, non codice.
- **È la chiusura del manifesto kb-first.** kb-first dice "prima di dire *serve un
  LLM*, decomponi e prova a derivare la struttura". La comprensione universale è
  quella decomposizione resa **default del parser**, non un'eccezione per-frase.
- **È colla linguistica** ([the-linguistic-glue](the-linguistic-glue.md)): leggere
  l'intento di una frase incompleta o con parole ignote è esattamente il "tessuto
  connettivo" descritto lì.

## 4. Il meccanismo (sketch a piccoli passi)

1. **Skeleton sintattico.** Tokenizza e assegna a ogni token una *parte del
   discorso* interrogando la KB (`pos(word, noun|verb|adj|…)`, più le classi
   chiuse già presenti). Le parole ignote restano `unknown` ma **tengono il loro
   slot posizionale**.
2. **Schema della frase.** Riconosci lo *schema d'intento* dalla forma, non dal
   lessico: `[WH] [is|are] the [N] of [N]?` → intento `attribute_of`; `[V-imp]
   [NP] …` → intento `request/generate`; `[if] [clause], [clause]` → condizionale;
   ecc. Gli schemi sono fatti KB (`intent_schema(Pattern, Intent)`).
3. **Assegnazione ruoli.** Lega gli slot ai ruoli dell'intento (soggetto,
   relazione, oggetto, vincolo). Un content-word ignoto **può** riempire un ruolo:
   "capital of Zembla" → `relation=capital, object=Zembla` anche se `Zembla` è
   ignota.
4. **Risoluzione KB.** Prova a soddisfare l'intento dai fatti. Se la catena
   regge → rispondi. Se manca un anello → **declino informato** che nomina
   l'anello (passo 2 di onestà).
5. **Abduzione (costo/limite).** Per parole ignote in posizione di *predicato/
   relazione* nuova, la mappa va **abdotta** (ipotizzata + oracolo di
   plausibilità). Questo è il pezzo realmente difficile e va trattato come la
   classe "abduci" del manifesto, non come magia.

## 5. Cosa esiste già e cosa manca

- **Esiste:** classi chiuse in KB (`conjunction`, `stopword`), `intent_phrase`/
  `intent_cue`, dispatch first-match, coref/pragma/repair (la colla), il declino
  onesto. La materia prima c'è.
- **Manca:** (a) un vero `pos/2` lessicale in KB e un assegnatore di ruoli
  generico; (b) `intent_schema/2` come fatti (oggi l'intento si riconosce per
  cue/frase, non per *schema strutturale*); (c) il **declino informato** come
  output di default al posto del muro cieco; (d) l'abduzione della mappa per
  predicati ignoti.

## 6. Verdetto — ha senso?

**Sì, con una correzione.** Il nucleo è giusto e profondamente in linea col
progetto: **separare struttura (grammatica) da contenuto (lessico)**, mettere la
struttura nella KB, e leggere l'intento dalla forma. È la generalizzazione
naturale di kb-first.

La correzione necessaria: "nessuna frase va a muro" va inteso come **"nessun muro
cieco"**, non "rispondi sempre". L'asticella onesta si sposta da *"capisci la
domanda?"* a *"hai il dato per rispondere?"*. Senza questa distinzione l'idea
diventerebbe l'impostore che i PRINCIPLES rifiutano.

Il valore concreto è alto e misurabile: oggi moltissimi 0 di LLMSCORE sono muri
ciechi su frasi *che parrot0 ha capito benissimo* ma per cui mancava un fatto.
Trasformarli in declini informati è onesto **e** alza la qualità percepita.

Rischi/limiti (onestà): il passo 5 (abduzione della mappa per predicati inediti)
resta il soffitto difficile; e un `pos/2` esteso è "substrato" da marcare per non
inquinare "quante cose sai?". Da costruire a piccoli incrementi, ciascuno con un
bench oracolare — la stessa disciplina del resto del progetto.

### Primo incremento consigliato

Il **declino informato** (§2, §4.4) prima di tutto: è il pezzo a più alto
rapporto valore/rischio e non richiede ancora `pos/2` né abduzione. Basta che,
quando un riconoscitore d'intento identifica lo schema ma la KB non ha il dato,
si emetta "ho capito che chiedi X su Y, ma non conosco Y" invece di
`"I don't understand that yet."`. Da lì si sale verso schemi e ruoli generici.

---

## Appendice — appunti originali

> in regime di comprensione universale (kb-first) sarebbe interessante pensare che
> non esistano frasi di cui parrot0 non capisce / va a muro. ma immaginavo che lui
> grammaticalmente sappia estrarre la struttura di qualsiasi frase e le strutture
> siano dentro la KB. in questo modo frasi ben formate ma con parole che non
> conosce possono essere interpretate nell'intento — quel meccanismo analitico, la
> lettura del significato appoggiata alla KB.