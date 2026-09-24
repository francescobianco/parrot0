# RI-021 — una domanda indiretta si insegna, e la condotta la ascolta

**Lotto:** `2026-09-24` · **Stato:** completa · **Classificazione:** meta-capability-only
(W=0: la capacità è linguistica e di condotta; le applicazioni sono certificate
su conoscenza vera già in KB — golfare M10, stagno, acetone, fusibile, USB, Parigi.)

## Famiglia e contesto umano
Chi legge un datasheet non chiede sempre «What is …?»: dice «Tell me the rated
current of a plug fuse.» o «I would like to know the working load of an M10 eye
bolt.». È una **domanda nascosta** in una richiesta o in un desiderio.

## Stato iniziale
Commit `f4d2a132` (RI-020), profilo `agi`. `stato.txt`.

## Limite osservato — `prima-dialogo.txt`
- «I would like to know the working load of an M10 eye bolt.» → *«On know the
  working load of an m10 eye bolt: Let us start with one observable result…»* —
  un consiglio di progetto: il turno letto come un OBIETTIVO;
- «Tell me the operating voltage of a USB port.» → «I don't know about operating
  voltage yet», mentre «What is …?» risponde 5 V.
- (reperto a margine) «I'd like to know the melting point of tin.» → *«Learned:
  i'd like to know the melting point of tin.»*, un fatto falso in sessione
  (`know(i'd_like_to, melting_point_of_tin)`): RI-022.

## Diagnosi con il trace unico — e dove il trace taceva
Il trace diceva solo `plan turn_priority_response answers «On know …»`: **quale**
delle regole di `turn_priority_response/2` avesse parlato non si vedeva. Ora la
risposta prioritaria del piano ha la sua prova come `turn_response`
(`turn_priority_support/2`, derivation.p0): `depends on dialogue_candidate …
dialogue_task … dialogue_initial … absent(dialogue_usable_first(planning))` — cioè
l'apertura d'obiettivo `dialogue_opening("i would like to", goal)`, la più lunga
che combacia. (Calcolata solo col trace profondo: costava un secondo di
soft-test.)

Poi, lezione per lezione (`certificazione-dialogo.txt`):
1. `tell me the x means what is the x?` — forma esistente (gen511): «Tell me the
   operating voltage…» → 5 V. Il turno «I would like to know …» resta un obiettivo.
2. `"i would like to know" is another way to say "tell me"` — la parafrasi entra
   (`phrase_canon`), ma l'iniziativa legge i token della IR, non il canone: resta
   un obiettivo.
3. `a turn that contains "i would like to know" is a question` — la forza entra, e
   **la condotta non la ascoltava**: l'iniziativa di dialogo non consultava la
   forza insegnata (mantra #17). Cura, una riga di KB in dialogue-initiative.p0:
   `dialogue_excluded($T) :- turn_pattern_force($F, question),
   turn_pattern_match($T, $F).` Vale solo la forza **insegnata** come forma, non
   ogni indizio di domanda.

## Curriculum (tre lezioni, nessun fatto di dominio)
1. `tell me the x means what is the x?` (L)
2. `"i would like to know" is another way to say "tell me"` (L)
3. `a turn that contains "i would like to know" is a question` (L)

## Certificazione sul meccanismo fermo — `certificazione-dialogo.txt`
- stimolo: «I would like to know the working load of an M10 eye bolt.» → 230 kilograms
- transfer 1 (altre relazioni e composizioni, mai nelle lezioni): stagno 232 °C,
  acetone 56 °C (la costruzione di RI-020), Parigi (relazione nativa)
- transfer 2 (altro verbo d'apertura, altra relazione): «Tell me the rated current
  of a plug fuse.» → 13 amperes
- contrasti: «I would like to learn welding.» e «I want to build a shed.» restano
  obiettivi con il loro piano; «What is the flash point of Jet A fuel?» invariato
- ablazione: `forget that a turn that contains "i would like to know" is a
  question` → torna il consiglio di progetto; il controllo indipendente («Tell me
  the operating voltage…» → 5 V) resta.

## Salvataggio e processo nuovo
`/save` 23 clausole: la forma `taught_form_16` («tell me the x» → «what is the
{x}»), la parafrasi `phrase_canon("i would like to know", "tell me")`, la forma
di forza `turn_pattern/turn_pattern_force` (spostata dalla ricaduta accanto alla
sua `turn_pattern` in intents.p0) — **L=3**, W=0, C=0, provenienza (P), **X=0**.
Processo nuovo: stimolo, flash point via richiesta indiretta, «Tell me …»,
contrasto welding; replay RI-016..020 verde.

## Verifiche
`make soft-test` verde (15 s e 14 s; col calcolo della prova sempre acceso era 16).
Banco L2 60/60.

## Limiti e prossimo problema
- **La contrazione**: «I'd like to know …» non è «I would like to know …», e la
  lezione di sigla esistente la guasta — `"i'd like" is short for "i would like"`
  → *«Learned: "i'd like" is a short for i would like»*, e senza virgolette
  *«Learned: d like short.»*: due fatti falsi da una lezione vera. → RI-022.
- Ci vogliono tre lezioni dove una persona ne darebbe una: la parafrasi verso
  «tell me» non trasferisce la forza della sua àncora, e l'iniziativa legge i
  token grezzi e non il canone (due letture che non si accordano).
- `intent_cue(30_generation_reading_chain1108, …)`: la lezione «another way to
  say» scrive anche in una classe dal nome seriale (mantra #19a), come prima.
