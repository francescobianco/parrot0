# RI-018 — un valore nominale si chiede alla lettura, non al frasario

**Lotto:** `2026-09-23` · **Stato:** completa · **Classificazione:** trained

## Famiglia e contesto umano
Schede tecniche: «the operating voltage of X is V», «the rated current of X is I».
Chi legge un datasheet chiede poi il valore. Prima della lezione il nome di
relazione di due parole non si leggeva; dopo la lezione il valore si leggeva, ma
la domanda riceveva la **definizione del sistema operativo** (il frasario prende
«operating»): una risposta sbagliata, peggiore del muro.

## Stato iniziale
Commit `afcb4ed8` (dopo RI-017), profilo `agi`. `stato.txt`.

## Limite osservato — `prima-dialogo.txt`
Muro sull'affermazione, «An operating system manages hardware resources…» sulla
domanda; «I can't show that.» per il fusibile.

## Diagnosi (trace unico) e supporti generali
- Lo schema della domanda era legato bene («@O is the operating voltage of @S» →
  [what][usb_port]), ma `answerframe` prendeva il turno prima. Le letture
  interrogative non venivano pubblicate. Ora una **prova a secco** con la stessa
  via che risponde (`p0_try_extract_frames_only` in solo-domanda) pubblica
  `turn_reading(T, answerable_question, frame)` quando la KB ha il valore; in KB
  `turn_frame_answer/2` + `faculty_yield_when(answerframe|answer_frame, open,
  turn_frame_answer)`: il frasario cede **solo** quando c'è una risposta che legge
  (pannello di 8 domande comuni identico alla base).
- Il ritiro «forget that X is a Y» accettava solo soggetti di una parola; ora il
  soggetto è ciò che precede la copula (e la conferma lo nomina con gli spazi).
- Il lettore di classe: la parola menzionata subito prima della copula è il
  soggetto anche se è un chiusore o non è un concetto («up is an adverbial
  particle» dopo RI-016 cadeva in «subject starts at a boundary»); tutte le sue
  uscite ora hanno un nome nel trace (niente più «gate: 8010»).

## Curriculum
1. `operating voltage is a relation` (L)
2. `The operating voltage of a USB port is 5 volts.` (W)
3. trasferimento (istanza nuova): `The operating voltage of a car battery is 12 volts.` (W)
4. trasferimento (relazione nuova, stessa via): `rated current is a relation` (L),
   `The rated current of a plug fuse is 13 amperes.` (W) — fusibile BS 1362.
   (Scartato «boiling point»: «point» è anche un verbo e chiude il sintagma —
   la classe di RI-014, non questa.)

## Certificazione — `certificazione-dialogo.txt`
«5 volts.» · «12 volts.» · «13 amperes.» · contrasti: «Paris.», «what is a dog»
invariato · ablazione `forget that operating voltage is a relation` → torna la
risposta sbagliata (la dipendenza è la lezione).

## Salvataggio e processo nuovo — `processo-nuovo-dialogo.txt`
`/save` 32 clausole: `relation(operating_voltage)`, `relation(rated_current)` (L=2)
accanto alle sorelle; `operating_voltage_of(usb_port, 5_volts)`,
`operating_voltage_of(car_battery, 12_volts)`, `rated_current_of(plug_fuse,
13_amperes)` (W=3) dalla ricaduta a engineering.p0; provenienza (P). Processo
nuovo: tutto risponde; replay RI-016/017 verde. W=3, L=2, C=0, X=0.

## Verifiche
Banco L2 60/60 (130 s: macchina lenta stanotte). soft-test: 16 s e poi 15 s
(verde); la base `015ce009` nelle stesse condizioni fallisce su turni oltre 1 s.
Per turno, l'albero attuale è più veloce della base sui 14 turni del soft-test
(7,7 s contro 8,8 s): il bordo del budget è del boot/reset, non dei turni.

## Limiti e prossimo problema
- «boiling point is a relation»: «point» verbo chiude il sintagma (RI-014 / L2).
- «Scartato: … non e' un concetto.» è un messaggio italiano scritto nel C (mantra #16).
