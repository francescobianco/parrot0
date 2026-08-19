# HANDOFF — dove riprendere (gen431, 19 agosto 2026)

Stato dell'albero al momento dello stacco: **`make test` 2569 verdi**,
**`make hundred` 54/100**, **`make measure` tonnage 48 / solved 48 / max length 5**.
Tutto committato e pushato.

## 1. La cosa da riprendere per prima: i cento

`make hundred` (nuovo) misura `docs/plans/parrot0-100-failures.md` per davvero:
`tests/hundred/hundred.qa` ha una riga per prompt — `numero | prompt | attesa` —
e l'attesa e' curata **perche' nessun ripiego possa passarla**. `make hundred-v`
elenca i mancanti con la risposta di oggi.

Da 32 a 54 in una sessione, chiudendo **classi**. I 46 che restano, per forma
(il dettaglio e' in fondo a `parrot0-100-failures.md`):

1. **meta-domande sul metodo** (~14) — «come tieni un obiettivo per dieci
   turni?», «come previeni la contaminazione fra sessioni?». parrot0 **ha** quei
   meccanismi: la risposta giusta e' il suo modello di se'. E' la classe piu'
   numerosa e la piu' promettente, e non chiede facolta' nuove;
2. **logica** (5) — contrapposizione, affermazione del conseguente, sillogismo
   motivato, non-contraddizione;
3. **artefatti verificabili** (6) — JSON, CSV, YAML, regex, matrice di rischio;
4. **salienza** (2) — quale riga conta di piu' in un log: ordine di gravita' in KB;
5. **generativi** (5) — inventare nomi, regole, metafore.

## 2. TODO esplicito di F. (gen431)

> «anche la risoluzione del plurale e del singolare deve essere fatta via KB»

Segnato come `TODO(kb-first)` in `src/brain/00-lex.c` dentro
`p0_unattached_kind`: la chiamata a `singularize_kb` e' C cablato. La forma
giusta e' una **relazione interrogabile** (morfologia gia' in
`kb/core/morphology.p0`), cosi' una lingua nuova non passa da quel file.

## 3. Due debiti lasciati aperti, con la misura accanto

- **le copule italiane nell'estrazione.** Aggiungere `is` a `generic_copula`
  cambia l'estrazione italiana: «Marte e' rosso» diventa un fatto (4 invece di
  3+1 in `summary.it.p0t`). Probabilmente e' **meglio** cosi', ma va fatto con
  la sua prova. Per questo il gen431 ha creato una classe separata
  (`clause_copula/1`) invece di allargare quella esistente;
- **`forget my name` non chiude il ricovero.** Dopo «my name is luca» →
  «forget my name» risponde «Done — I've let go of your name», ma il turno dopo
  «what is my name» risponde ancora «Your name is luca». La supersessione e'
  scritta e **non viene letta** da quel percorso: e' `docs/issues/04` che torna,
  ed e' un misclaim (mantra #7).

## 4. Il filo lungo di questa sessione (gen427-431)

Tutto insegnabile **parlando**, senza ricompilare e senza moduli nuovi:

- `docs/plans/teach-comprehension-via-prompt.md` — il piano del canale-dialogo,
  gemello di quello MCP: il debito si sposta dalla tipizzazione dei tool
  all'**inventario delle superfici**, che e' un debito migliore perche' una
  forma e' un fatto;
- `docs/autocorrezione.md` §13 — **l'autocorrezione fatta in due**: il pezzo
  mancante non va indovinato, va **chiesto**, e la risposta dell'altro e' gia'
  l'atto di insegnamento. Il muro consegna la frase gia' scritta;
- `tests/p0t/knowledge/literal_forms.p0t` (49 assert) — il ratchet: se una forma
  tornasse nel C, cade.

Prossimo passo naturale di quel filo: **l'offerta di insegnamento va scelta
dalla posizione in cui il lettore si e' fermato**, non dal fatto che si e'
fermato (oggi il menu e' fisso e propone anche forme che non c'entrano). E' lo
stesso `P0/S4b` — l'inferenza che riporta dove si e' arrestata — che serve ai
quattro casi di schema dei `docs/issues/`.
