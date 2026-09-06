# Lo specchio italiano — che cos'è, e che cosa NON è

## Che cos'è

48 frasi (8 per banda CEFR) tradotte dall'inglese di **CEFR-SP**, campionate in
modo deterministico da `wikiauto_test` + `wikiauto_dev` (seme 7, le prime 8 per
banda dopo mescolamento). Servono a far girare il `cefr-bench` **anche in
italiano**, cioè a misurare se una capacità regge quando cambia la lingua.

## Chi ha tradotto, e perché va scritto

Tradotte **da un assistente LLM durante la sessione del 2026-09-06**, non da un
traduttore umano e non da un servizio verificato. Nessuna frase è stata
ricontrollata da un madrelingua. Questa riga vale più di una nota a piè di
pagina: chi legge un risultato di bench deve sapere che la variabile «qualità
della traduzione» esiste e non è stata controllata.

## ⛔ Le etichette sono EREDITATE, non annotate

Le due colonne di livello sono **copiate dall'originale inglese**. Non sono
un'annotazione CEFR dell'italiano, e **non vanno presentate come tale**.

Il motivo non è pigrizia: **il livello CEFR non sopravvive alla traduzione.**
Una frase C1 in inglese può essere B1 in italiano e viceversa — cambiano la
frequenza lessicale, la morfologia, la lunghezza media, le costruzioni
disponibili. Chi annota CEFR per l'italiano lo fa su criteri italiani.

Presentare queste etichette come «annotazione CEFR italiana» sarebbe:

- **sbagliato verso chi legge il bench**, perché la stratificazione misurerebbe
  qualcosa che non è il livello italiano;
- **scorretto verso gli autori di CEFR-SP**, perché attribuirebbe a loro un
  lavoro di annotazione che non hanno fatto e che non hanno validato.

Qui servono solo come **partizione stabile**: «le frasi che in inglese erano di
banda X». È abbastanza per la domanda a cui il bench risponde davvero — *la
capacità regge cambiando lingua, a parità di frase?* — e non è abbastanza per
nient'altro.

## Licenza

Opera **derivata** dalla porzione Wiki-Auto di CEFR-SP, che è **CC BY-SA 3.0**.
La traduzione eredita quindi BY-SA: attribuzione agli autori originali
(`../../ATTRIBUTION.md`) e licenza compatibile per chi ridistribuisce.

## Come si estende

Il campione è piccolo apposta: 48 frasi tradotte e dichiarate valgono più di
7.453 tradotte a macchina e non verificate. Per allargarlo, il criterio è che
ogni riga aggiunta porti con sé **come è nata** — traduttore umano, servizio, o
assistente — e che questo file lo registri.
