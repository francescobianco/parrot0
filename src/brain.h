/*
 * brain.h - the part of parrot0 that EVOLVES.
 *
 * Everything in this header/implementation pair is fair game for the
 * self-improvement loop. The I/O shell (main.c) should stay stable; the
 * intelligence of the agent emerges here, as pure C algorithms.
 *
 * Contract (keep this stable so the loop can always compile & test):
 *   - brain_respond() takes a single line of user input and writes a
 *     single line of response into `out` (NUL-terminated, never overflowing).
 *   - It must be deterministic given the same input + same brain state,
 *     so the test harness can check behaviour.
 */
#ifndef PARROT0_BRAIN_H
#define PARROT0_BRAIN_H

#include <stddef.h>
#include <stdint.h>

/* Opaque, persistent state the brain may accumulate during a session.
 * v0 keeps almost nothing here, but evolution will grow it. */
typedef struct Brain Brain;

/* Forward-declared so a host can reach the brain's KB (brain_kb) without
 * pulling in kb.h; identical to kb.h's own typedef (C11 allows the repeat). */
typedef struct KB KB;

/* gen277: the brain's knowledge base, so a host (the MCP engine) can call the
 * kb.h primitives directly on it — expose the engine, don't wrap every call. */
KB *brain_kb(Brain *b);

/* gen400: quale modulo ha chiuso l'ultimo turno. Lo stato c'era gia' (la chat
 * sa rispondere «which part of you answered that?»); questa e' la porta per il
 * profiler, che deve poter dire dove il tempo e' stato speso E chi ha parlato. */
const char *brain_last_module(Brain *b);

/* Atomically change one typed agent record's state. The in-memory P0AStore and
 * its live rec(Id,Kind,Parent,State) projection either both move or both remain
 * at the old state. Returns 1 on success, 0 for an unknown id/invalid input or
 * a projection failure. This is the sole state-transition seam for agent loops. */
int brain_agent_set_state(Brain *b, uint64_t id, const char *state);

/* Create / destroy brain state. Returns NULL on allocation failure. */
Brain *brain_create(void);
void   brain_destroy(Brain *b);

/* Produce a response to `input`, writing at most `out_size-1` chars into
 * `out` plus a NUL terminator. Returns the number of chars written. */
size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size);

/* A short, human-readable name/version of the current brain generation.
 * Bump this whenever the algorithm meaningfully changes. */
/* gen382s — LA FORMA CANONICA E' ISPEZIONABILE.
 *
 * I moduli non vedono cio' che l'utente ha scritto: vedono la superficie
 * normalizzata e canonicalizzata (verbi lemmatizzati, riempitivi interrogativi
 * caduti, parole tradotte nella lingua comune). Chi insegna una porta —
 * `answer_frame(Cue, Pred)` — deve scrivere il cue in QUELLA forma, e finora non
 * aveva modo di vederla: una porta italiana si dichiarava e non scattava, senza
 * spiegazione. Vedi docs/plans/question-emergence.md §11.3.
 *
 * Scrive la forma canonica di `input` in `out`. Nessuna conoscenza nuova: espone
 * una trasformazione che il motore gia' esegue a ogni turno. */
size_t brain_canonical(Brain *b, const char *input, char *out, size_t out_size);

/* gen411 — IL CICLO DI AUTORIPARAZIONE, come atto invocabile.
 *
 * Per ogni lacuna di macchineria aperta propone un ponte (una cue tratta dal
 * turno stesso), lo asserisce in ipotetico, RIPONE il turno che murava e lo
 * tiene solo se il turno ora risponde e se il candidato assomiglia alla classe
 * in cui entrerebbe. Scrive in `out` i ponti tenuti; ritorna quanti sono.
 *
 * E' esposto qui perche' il sogno — che e' il comando di run del processo
 * autonomo — deve poterlo scegliere come RIMEDIO: una lacuna con parole ignote
 * si colma leggendo, una lacuna in cui tutte le parole erano note non si colma
 * leggendo per definizione, e li' l'unica mossa e' proporre. */
int brain_self_repair(Brain *b, char *out, size_t out_size);

/* gen411: il registro di lavoro del processo autonomo — i turni che hanno
 * murato e aspettano un ponte. Vive in un file proprio (`PARROT0_GAPS`, di
 * default kb/learning/gaps.p0) e NON nell'albero curato: non e' qualcosa che
 * parrot0 sa, e' qualcosa che deve fare. Si ricarica alla nascita, ed e' cio'
 * che rende il processo un processo e non un episodio. */
const char *brain_gaps_path(void);
int brain_gaps_save(Brain *b);

/* gen411: i ponti che parrot0 si e' insegnato da solo. Indotti, non curati,
 * in un file proprio: senza persistenza il bilancio del sogno era una misura
 * falsa — le lacune si azzeravano dentro il giro e tornavano tutte al giro
 * dopo. Un processo il cui effetto non sopravvive al processo non e' un
 * processo. */
const char *brain_bridges_path(void);
int brain_bridges_save(Brain *b);

/* gen422 — la FIRMA del flusso di inferenza dell'ultimo turno, in esadecimale
 * quando la si stampa. Due turni che hanno percorso la stessa strada portano lo
 * stesso valore: e' un CRC del ragionamento, non della risposta. Vedi
 * kb_footprint in kb.h e docs/measured-classes.md. */
unsigned long brain_footprint(const Brain *b);

const char *brain_version(void);

/* Load a knowledge file into the brain's KB. `as_base` non-zero tags the
 * loaded clauses as base knowledge, otherwise as session knowledge. An
 * empty/NULL path or missing file is a no-op. Returns clauses loaded. */
int brain_load(Brain *b, const char *path, int as_base);

/* Persist the session delta (session + induced clauses, never the reflective
 * self-model) to `path`. Returns clauses written, or -1 on error. */
/* Salva cio' che si e' imparato INSTRADANDOLO nell'albero curato (save-map).
 * `path` e' la sola ricaduta per i fatti che il routing non colloca, e non e'
 * mai un file di sessione. */
int brain_save_session(Brain *b, const char *path);

/* gen382g — il DUMP della sessione: una fotografia Prolog di cio' che parrot0 ha
 * in memoria adesso, da leggere con `cat`. Non e' conoscenza da caricare e non
 * viene mai riletta; e' unica per processo, cosi' due parrot0 sulla stessa
 * macchina non si sovrascrivono. Percorso da PARROT0_SESSION_DUMP, altrimenti
 * <PARROT0_RUNTIME_DIR|/tmp>/parrot0-session-<pid>.p0 */
int brain_session_dump(Brain *b);
const char *brain_session_dump_path(void);

/* gen331 (TODO.md P1/09): the EFFECTIVE runtime policy, projected into the KB at
 * boot as policy(tools|network|mode, …) and read back through these. Every host
 * and every module must ask HERE rather than call getenv() and reach its own
 * conclusion — otherwise the banner can promise what a decline denies, which is
 * precisely what `make chat` did: it advertised the AGI profile, refused every
 * file request as "I don't understand", and silently enabled the network. */
int  brain_policy_on(Brain *b, const char *key);      /* "tools" / "network" */
void brain_mode(Brain *b, char *out, size_t cap);     /* conversational|agent|acquire */

/* gen276: load the outer KB layers on top of a freshly-created brain — the
 * curated base, the session delta, the coding expert, and (if named by
 * PARROT0_PROFILE) an expert/skill profile. brain_create() already loads the
 * kernel lexicon and the reflective self-model; brain_boot() adds what the CLI
 * shell used to load inline, so the same full boot is reachable from any host
 * (the chat REPL, the daemon, the MCP engine). The file paths come from the
 * environment (PARROT0_BASE / PARROT0_SESSION / PARROT0_PROFILE), each with the
 * historical default, an empty value disabling that layer. */
void brain_boot(Brain *b);

/* gen276: forget the UNSAVED current session and reload every knowledge file
 * from disk into a fresh KB, in place — so an agent that has written new
 * knowledge to a `.p0` file (e.g. via the MCP engine) makes parrot0 pick it up
 * WITHOUT restarting the process: just `/restore`. Anything asserted this
 * session but never persisted (via /save or session.save) is dropped; anything
 * on disk — including files changed since boot — is re-read. All conversational
 * state (name, topics, proof trace, open worlds, …) resets to a clean session.
 * The caller's `Brain *` stays valid (rebuilt in place). Returns the number of
 * clauses now loaded, or -1 on allocation failure (in which case `b` is
 * unchanged). */
int brain_reload(Brain *b);

#endif /* PARROT0_BRAIN_H */
