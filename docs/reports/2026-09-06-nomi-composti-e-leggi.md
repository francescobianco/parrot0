# Nomi composti: la legge era nota, il nome no

6 settembre 2026. Base: `7d09192`. Interlocuzioni sulla KB completa
(`PARROT0_PROFILE=kb/profiles/agi.p0`), senza suite come richiesto da F.

## Difetto e riparazione

Il reperto dell'utente era:

> parlami della legge di ohm
>
> Su read di ohm non so ancora molto. Vuoi che cerchi?

`lang.canonical` mostrava `parlami of the read of ohm`: `tr(read, legge)`
veniva applicato senza riconoscere il sintagma. La KB aveva gia' sia
`law_prose(ohms_law, ...)` sia l'albero `product(current, resistance)`.
Non mancava una definizione della legge; mancavano i collegamenti linguistici.

Inoltre `mod_learn`, riconosciuta la testa italiana sul testo originale,
traduceva nuovamente i token del tema con `tr/2`. Questo secondo traduttore
ignorava qualsiasi nome composto appreso e sostituiva una parola solo quando
la traduzione entrava nello spazio occupato dall'originale.

La correzione usa meccanismi condivisi:

1. `law_surface` alimenta `entity_alias`; `also_known_as` appreso fa lo stesso.
2. `phrase_canon` legge gli alias, prima della traduzione dei singoli token.
   Quando consuma un nome composto conserva la punteggiatura finale del nome:
   il punto interrogativo non deve sparire insieme all'ultimo token.
3. Il nome oggetto di «is also called» / «is known as» e' una menzione; il
   valore di `also_known_as` e' testuale, quindi non si ferma alla preposizione.
4. `mod_learn` conserva il tema originale per mostrarlo e per il fallback;
   per il lookup canonico chiama il canonizzatore comune sull'intero tema.
5. La lezione di definizione gia' esistente e' aperta alle spiegazioni italiane
   mediante `learnable`; una vista KB le rende disponibili a `concept_gloss`.

Non e' stata aggiunta al C alcuna parola italiana, nome di legge o formula.

## Apprendimento e ablazione nella stessa sessione

Prima della persistenza, su un processo con la KB completa:

| Interazione | Risultato osservato |
|---|---|
| `parlami della legge di ohm` | lacuna sul nome originale, senza trasformarlo in «read» nella risposta |
| `ohms law is also called legge di ohm` | appreso il nome intero |
| canonicalizzazione della domanda | `parlami of the ohms_law` |
| stessa domanda | prosa inglese della legge gia' presente |
| `what is the expression of legge di ohm?` | `(current * resistance).` |
| retrazione MCP del solo alias | `removed: true` |
| canonicalizzazione della stessa domanda | ritorna `parlami of the read of ohm` |
| stessa domanda | torna la lacuna sul nome, senza cancellare la legge |
| `lui legge un libro` | `he read a book` |

La lezione e' lingua naturale. La retrazione MCP misura soltanto la dipendenza
dal fatto insegnato: non viene presentata come capacita' di dimenticare parlando.

## Lezioni persistite

Questi turni sono stati pronunciati attraverso la chat, poi salvati con `/save`:

```text
ohms law is also called legge di ohm
newtons second law is also called seconda legge di newton
kinetic energy law is also called legge dell energia cinetica
density law is also called formula della densità
learn italian definition of ohms law: "La legge di Ohm lega tensione corrente e resistenza nei conduttori ohmici a temperatura costante: V = I * R. La tensione si misura in volt, la corrente in ampere e la resistenza in ohm"
learn italian definition of newtons second law: "La seconda legge di Newton lega la forza risultante alla massa e alla accelerazione: F = m * a, per massa costante in un sistema inerziale"
learn italian definition of kinetic energy law: "La energia cinetica di un corpo di massa m e velocita v vale E = 0.5 * m * v * v nel regime non relativistico"
learn italian definition of density law: "La densita media di un corpo si calcola dividendo la massa per il volume: rho = m / V. Il volume deve essere positivo"
```

La spiegazione di Ohm e' una formulazione nostra verificata su
[OpenStax, University Physics Volume 2, §9.4](https://openstax.org/books/university-physics-volume-2/pages/9-4-ohms-law).
Le altre localizzano le formule gia' presenti, specificandone le ipotesi usuali.

Le quattro `italian_description` uscite in `learned.p0` sono state ricollocate,
senza modificarne il contenuto, in `kb/experts/physics/laws.p0`. Alias,
provenienza e trascrizioni sono quelli prodotti dalla sessione. Sette nuovi
fatti effimeri (`discourse_referent`, `turn_span*`) sono stati rimossi dalla
ricaduta: non sono conoscenza del mondo da ricaricare al prossimo boot.

## Riavvio con le opzioni di make chat

Con strumenti e rete abilitati, senza ripetere le lezioni:

- `parlami della legge di ohm`: restituisce la spiegazione italiana insegnata.
- `parlami della seconda legge di newton`: restituisce forza risultante, massa,
  accelerazione e sistema inerziale.
- `parlami della formula della densità`: restituisce massa divisa per volume.
- `what is the expression of kinetic energy law?`:
  `(0.5 * (mass * (velocity * velocity))).`
- `what is the expression of legge di ohm?`: `(current * resistance).`

Le espressioni provengono dal fold sull'albero preesistente; le glosse non
contengono queste risposte e non vengono usate come programmi.

## Limiti aperti, da non nascondere

- Gli omografi fuori da un nome conosciuto non sono risolti in generale.
- L'alias della densita' viene memorizzato come `formula della densit`:
  `strip_edge_punct` tratta i byte finali accentati come punteggiatura.
  La domanda accentata funziona perche' attraversa la stessa potatura;
  questo NON dimostra conservazione corretta della superficie Unicode.
- «forget that ohms law is also called legge di ohm» non ritira l'alias:
  viene respinto come soggetto non concettuale. Nessuna falsa conferma di oblio.
- Una porta sperimentale «is described in italian as ...» ha mostrato
  rivendicazioni indebite da altri moduli sulle descrizioni lunghe. Non e'
  stata mantenuta: la lezione di definizione gia' esistente conserva il testo
  originale e funziona. La copertura del turno didattico resta lavoro aperto.
- Il nome di una legge e la sua descrizione si insegnano; il relativo albero
  ancora no. Generare una funzione completa e verificare dimensionalmente
  i suoi ingressi non sono risultati di questo intervento.
- Nessuna suite, benchmark o harness eseguito. Compilazione, diagnostica
  mirata e conversazioni reali non equivalgono a una regressione completa.
