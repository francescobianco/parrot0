# piano: parrot0 genera codice C da richieste in linguaggio naturale

## 1. analisi della richiesta

Input: `mi generi un piccolo hello world in c`

Cosa serve per gestirlo:

| strato          | da fare                                       | complessità |
|-----------------|-----------------------------------------------|-------------|
| parsing intent  | riconoscere "generi/mi fai/scrivi [cosa] in C" | bassa       |
| composizione    | assemblare un programma C da parti             | media       |
| output multi-riga | emettere codice su più righe                 | media       |
| anti-impostor   | generare varianti held-out (non template fissi) | alta        |

## 2. cosa c'è già

| capacità esistente   | modulo     | come si riusa                                            |
|----------------------|------------|----------------------------------------------------------|
| sintesi da specifica | mod_synth  | "cosa X in Y" → comando shell. Pattern identico: action→target, noun→language. |
| KB con fatti C       | coding.p0  | keyword, c_stdlib, ctype già pronti                      |
| KB con conoscenza    | bash.p0    | cmd/flag → composizione. Stesso pattern: code_pattern/param |
| parsing NL           | canonicalize_lang | function words EN↔IT già mappati                   |
| identificazione C    | mod_code   | già riconosce C vs Python                               |
| generative model     | mod_gen    | completa sequenze statistiche (non serve qui)            |

## 3. cosa manca — stima di impatto

### A) conoscenza di dominio: `kb/patterns.p0` (basso impatto, ~0 gen)

Fatti che descrivono pattern di codice composable:

```
% pattern C: hello world
c_skeleton(intro).
c_skeleton_body(intro, "printf(\"hello world\\n\"); ").

% pattern C: read input
c_skeleton(echo).
c_skeleton_body(echo, "char buf[256]; fgets(buf, sizeof buf, stdin); printf(\"%s\", buf); ").

% template C: funzione main
c_template(main, "int main(void) {\n    BODY\n    return 0;\n}").

% verbo → azione
code_action(generate, "genera").
code_action(create, "crea").
code_action(write, "scrivi").
code_action(scrivi, "scrivi").

% azione → pattern
code_target("hello world", intro).
code_target("ciao mondo", intro).
code_target("echo", echo).
```

**Solo file dati, zero modifiche a brain.c.** La KB si arricchisce senza toccare codice.

### B) `mod_gencode` in brain.c (impatto medio, ~3–5 gen)

Nuovo modulo che:
1. Riconosce richieste del tipo "generami/scrivi/fammi [X] in [L]" via cue
2. Estrae target ("hello world") e linguaggio ("c")
3. Cerca `code_target(Target, Pattern)` nel KB
4. Cerca `c_template(Pattern, Template)` nel KB
5. Sostituisce `BODY` nel template con `c_skeleton_body(Pattern, Body)`
6. Emette il codice

**Struttura simile a `mod_synth` ma per codice invece che comandi.**

### C) output multi-riga (impatto medio-alto, ~2–4 gen)

Oggi `brain_respond` produce una singola riga. Il codice è multi-riga.

Opzioni:
- **(c1) escape newline:** `printf("int main(void) {\n    printf(\"hello\");\n    return 0;\n}")`
  - Pro: nessuna modifica al formato output, immediato
  - Contro: output criptico, non testabile in modo leggibile
- **(c2) delimitatore speciale:** `[[[CODE]]]int main...[[[END]]]`
  - Pro: singola riga stdout, retrocompatibile
  - Contro: i test case `.chat` vanno adattati al delimitatore
- **(c3) output multi-linea nativo** — `brain_respond` supporta `\n` nell'output
  - Pro: output user-friendly
  - Contro: cambia il contratto del protocollo chat (una riga in → una riga out). I test case `.chat` confrontano riga per riga; un output multi-riga romperebbe la corrispondenza 1:1.

**Raccomandazione: (c2)** — delimitatore `[[[CODE]]]...[[[END]]]` in una singola riga stdout. I test case verificano la presenza di sottostringhe. Il test runner (`run.sh`) già confronta l'intera linea, quindi il delimitatore non lo rompe.

### D) anti-impostor (impatto alto, ~3–5 gen)

Il rischio è che il generatore diventi un phrasebook: "hello world" → template pre-scritto. Per evitarlo:

1. **Pattern parametrici:** non `printf("hello world")` ma `printf("TEXT")` con TEXT sostituito
2. **Test held-out:** generare "goodbye world", "saluti in C", "programma che dice buongiorno" con target mai visti nel training
3. **Composizionalità:** il generatore non sceglie un template intero — sceglie uno skeleton (intro), uno skeleton body, e li compone con un template main
4. **Varianti linguistiche:** stesso target in italiano e inglese produce lo stesso codice (ratchet bilingue)

## 4. architettura finale

```
input: "mi generi un piccolo hello world in c"
   │
   ▼
canonicalize_lang: "mi generi un piccolo hello world in c"
   │
   ▼
mod_gencode (nuovo, nel registry PRIMA di mod_code):
   1. cue "generi" / "scrivi" / "fammi" / "crea" → code_action match
   2. extract target: dopo il verbo, prima di "in C/Python"
   3. extract language: "c" / "python" dopo "in"
   4. kb_query → code_target(target, pattern)
   5. kb_query → c_skeleton_body(pattern, body)
   6. kb_query → c_template(template_name, template_str)
   7. replace BODY in template_str with body
   8. emit [[[CODE]]]template[[[END]]]
   │
   ▼
stdout: "[[[CODE]]]int main(void) {\n    printf(\"hello world\\n\");\n    return 0;\n}[[[END]]]"
```

## 5. stima effort

| step | cosa                                    | gen | complessità | note                                    |
|------|----------------------------------------|-----|-------------|-----------------------------------------|
| A1   | `kb/patterns.p0` con fatti skeleton    | 0   | bassa       | solo dati, niente codice                |
| A2   | `kb/patterns.p0` con code_action/verb  | 0   | bassa       | verbi IT+EN                             |
| B1   | `mod_gencode` — intent detection       | 1   | bassa       | riconoscere "generami/scrivi X in C"    |
| B2   | `mod_gencode` — KB lookup + compose    | 1   | media       | query code_target, compose con template |
| B3   | `mod_gencode` — ratchet bilingue       | 1   | bassa       | test IT + EN                            |
| C1   | output [[[CODE]]] delimitatore         | 1   | bassa       | snprintf con delimitatore               |
| D1   | held-out test: "goodbye world" in C    | 1   | media       | nuovo target nel KB, genera codice      |
| D2   | held-out test: varianti non viste      | 2   | alta        | "scrivi un contatore fino a 5 in C"     |
|      | **totale**                             | ~7  |             |                                         |

## 6. cosa NON serve (per ora)

- **Parsing AST**: per hello world basta template, non serve parser
- **Modifica dell'output di brain_respond**: il delimitatore `[[[CODE]]]` è una stringa, non cambia la signature
- **Esecuzione/compilazione reale**: non in questa fase, il codice è solo generato
- **Generazione Python/altri linguaggi**: estensione futura, il design lo supporta (basta aggiungere fatti `py_template(...)`)

## 7. giudizio sintetico

La complessità è **media (7 generazioni)**. Il pattern è già rodato da `mod_synth`: specifica NL → lookup KB → composizione → output. La differenza è che invece di `cmd + flag` si compone `skeleton + template`, e l'output è multi-riga (gestito con delimitatore).

Il rischio principale è il **phrasebook**: se il generatore si riduce a memorizzare "hello world" → `printf("hello world")`, l'anti-impostor fallisce. La difesa è la composizionalità: target e template sono separati nel KB, e il generatore non può funzionare senza entrambi. Un target held-out come "goodbye world" deve trasferire perché usa lo stesso skeleton con un parametro diverso.
