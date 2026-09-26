# Live teaching, debug di applicazioni PHP — 26 settembre 2026

Transcript: [live/2026-09-26-php-debug.log](live/2026-09-26-php-debug.log).
Un processo, KB viva, chiusa con `/save`; la ricaduta curata è in
`kb/experts/programming/debug.p0` (sezione «IL DEBUG DI APPLICAZIONI PHP»).
Indirizzi di F.: annotare ogni forma che non funziona in
[train-the-learning-process.md](../plans/train-the-learning-process.md) e andare
avanti; insegnare anche per contatto ([l3-upgrade.md](../plans/l3-upgrade.md));
alla fine il coefficiente imparate / con problemi.

**Esito: 19 lezioni imparate, 17 con problemi, coefficiente 1,12.** Per contatto
1 su 5. Il dettaglio (P1–P14) e il conteggio sono nella tabella del piano.

## Che cosa regge dopo il riavvio

Verificato in un processo nuovo: «Is Xdebug an extension?» Yes (per catena di
classi) · «What causes a parse error?» Unclosed string and missing semicolon ·
«Does output before the header function trigger a headers already sent
warning?» Yes (il verbo imparato per contatto, sul fatto tenuto fuori) · «What
does var_dump print?» Value and type · «What manages dependencies?» Composer ·
default di upload_max_filesize 2 megabytes · porta di Xdebug 9003 · «Does a
warning stop the script?» No · PHPUnit è un testing framework · OPcache
memorizza il bytecode compilato.

## Tolto dalla ricaduta, a freddo

`write(error_log_function, message)` e `name(error_log)` (P2),
`quantity(so_its_default, seconds, 30)` (un contatto letto male),
`class_surface(name, name)`, due `pending_gap_failed` superati. Non entrati, e
quindi niente da togliere: P4, P7, P10, P11.

## I quattro da chiudere per primi (risposte sbagliate, non muri)

P5 «the symptom of X» con X lungo → la definizione di «function»; P7 fatto
rifiutato ma usato («Readable.»); P10 «What stops the script?» → «Fatal error
and warning does not.»; P13 «Where does OPcache store…?» → l'oggetto.
