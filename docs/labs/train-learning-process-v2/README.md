# Verifica del cancello della prosa e revisione dell'indicatore

**Archivio della revisione precedente.** Il metodo vigente di
`docs/plans/train-the-learning-process.md` procede per **iterazioni di
riferimento** ed esclude il banco della prosa. Le istruzioni sul cancello qui
sotto documentano uno strumento separato; non sono prerequisiti di quel ciclo.

Riferimento: revisione del piano dopo `f50cdfe5`. Il timestamp effettivo della
misura e gli hash sono in [r300-baseline.json](r300-baseline.json).

Misura reale sulla KB agi completa, nessuna modifica al motore o alla KB:

| specie | esito |
|---|---|
| merito | 45/62 |
| meta | 2/2 |
| struttura | 5/5 |
| domande registrate | 69/69 |
| parole delle domande nel merito risolte | 282, su 299 parole del testo: il cancello assoluto resta non superato |

[Il log](r300-baseline.log) conserva il referto e gli output completi dei due
processi. Gli esiti delle 69 righe coincidono con il precedente referto
`../prose-ladder/referti/r300-2026-09-20-2153.txt`. Questo confronto storico è
diagnostico: il vecchio formato non contiene il contratto necessario al nuovo
cancello. Non è una coppia prima/dopo di una migrazione C di questo giro.

Verifiche effettuate:

- `make test-prose-gate`: 8 test della meccanica, incluso crollo simulato
  45→6, perdita compensata da un guadagno, nuovi errori non-muro, attribuzione
  a freddo mutata, risposta errata cambiata, input incomparabili/incompleti.
- `bash -n scripts/prose-probe.sh`: sintassi valida.
- Registrazione reale con `python3 scripts/prose-gate.py record ...`:
  file completo; confronto con se stesso verde (controllo della meccanica).
- Registrazione reale con `--timeout 1`: uscita 2, nessun JSON valido prodotto.
- Durante lo sviluppo il controllo del flusso ha rifiutato il prompt vuoto
  finale di `/quit`; ora viene eliminato solo quel terminatore. Nessun esito
  del banco è stato cambiato per far passare la verifica.

Per la prossima migrazione acquisire una baseline fresca prima di modificare
il consumatore e usare `make prose-gate` come indicato nell'handoff. Questa
baseline è utilizzabile solo con il medesimo contratto (KB, corpus, giudice e
impostazioni): il confronto rifiuta differenze. Il binario può cambiare.

Il nuovo indice `LC_v` è **specificato nel piano, non misurato qui**: manca il
manifest delle famiglie. r300 non dimostra generalità dell'apprendimento e il
giudice a regex resta una limitazione da affrontare separatamente.
