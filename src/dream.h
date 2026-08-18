#ifndef PARROT0_DREAM_H
#define PARROT0_DREAM_H

#include <stdio.h>
#include "brain.h"

/* --dream — esplorazione ricorsiva di un topic attraverso la sua prosa (gen382).
 *
 * `parrot0 --dream scacchi` legge la pagina del topic, ne estrae i fatti con lo
 * STESSO percorso di comprensione della conversazione ("read the page on X"), e
 * poi prende la prosa PAROLA PER PAROLA: la prima parola diventa a sua volta un
 * topic da sognare, poi la seconda, e cosi' via in profondita'.
 *
 * Le stopword non sono escluse. Sono anzi il caso interessante: se sognare "of"
 * non produce nulla di utile, e' un dato sul confine della comprensione, non
 * rumore da nascondere.
 *
 * Il sogno NON e' un test: e' uno strumento di osservazione. Il suo output e'
 * fatto per essere letto — da una persona o da un assistente — per accorgersi
 * se parrot0 stia imparando le cose giuste o stia PERDENDO PEZZI. Per questo il
 * trace nomina esplicitamente cio' che e' andato perso (pagine assenti, frasi
 * che non hanno prodotto fatti, atomi malformati), non solo cio' che e' entrato.
 *
 * Quello che impara non va perso: i fatti vengono asseriti nella KB viva con la
 * loro provenienza e, a fine sogno, persistiti attraverso il routing dei file
 * gia' esistente (`dream_persist`).
 *
 * Nessun servizio intelligente: le pagine vengono dal corpus statico locale e,
 * se PARROT0_WIKI_FETCH lo consente, dall'API di Wikipedia — le stesse due
 * sorgenti dell'apprendimento normale.
 *
 * gen405 — VEDI `docs/plans/dream.md`. Due cose che questo file non diceva e
 * che cambiano cosa il sogno E':
 *
 * (1) LA PROSA DA SOLA NON E' UN ATTO. Misurato con
 *     `tests/dream_intent_probe.py`: davanti alla stessa pagina, senza
 *     intenzione dichiarata un ragionatore non impara — chiede quale atto
 *     compiere («summarize, simplify, paraphrase, or fact-check?»). Con
 *     «acquisisci» trattiene; con «acquisisci per rispondere a X» risponde
 *     direttamente alla domanda. Qui si chiede sempre «read the page on X», a
 *     qualunque pagina e per qualunque motivo: e' la cornice NUDA senza saperlo.
 *
 * (2) IL SOGNO DOVREBBE ESSERE UN'ATTIVITA' DI PARROT0, NON UNA MODALITA' DEL
 *     PROGRAMMA. L'agenda ora viene dalle sue lacune e la verifica e' sua (si
 *     ri-pone il turno che murava), ma l'INTENZIONE non e' dichiarata e la
 *     politica — profondita', ordine, arresto — e' ancora qui in C invece che
 *     in KB. Questo file deve restare l'adattatore che porta le pagine e conta
 *     il budget.
 *
 * Ritorna il numero di nodi sognati. */
typedef struct {
    int   max_depth;      /* quanto in profondita' (default: dream_max_depth/1)  */
    int   max_nodes;      /* tetto di lavoro       (default: dream_max_nodes/1)  */
    int   fetch;          /* 1 = puo' scaricare le pagine mancanti               */
    int   persist;        /* 1 = scrive cio' che ha imparato nei file della KB   */
    FILE *out;            /* dove va il trace                                    */
} DreamOpts;

int dream_run(Brain *b, const char *topic, const DreamOpts *opts);

#endif /* PARROT0_DREAM_H */
