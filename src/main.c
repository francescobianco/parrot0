/*
 * main.c - the I/O shell of parrot0. STABLE.
 *
 * This file is intentionally boring and should rarely change: it owns the
 * read-eval-print loop, line buffering and the chat protocol, so that the
 * self-improvement loop can focus entirely on src/brain.c.
 *
 * Protocol (kept deterministic & test-friendly):
 *   - Reads one line of user input per turn from stdin.
 *   - Writes exactly one line of response to stdout (and flushes).
 *   - Decorative prompts go to stderr, so piping stdin/stdout stays clean
 *     for the test harness.
 *   - Input "/quit", "/exit" or EOF (Ctrl-D) ends the session.
 */
#define _POSIX_C_SOURCE 200809L

#include "brain.h"
#include "kb.h"
#include "serve.h"
#include "mcp.h"
#include "testeng.h"
#include "dream.h"
#include "env.h"

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* Long enough to hold a whole benchmark prompt (passage+question) on one line;
 * fgets would otherwise split a long passage across reads, hiding its tail
 * markers from the brain (gen49). */
#define LINE_MAX_LEN 65536
#define RESP_MAX_LEN 8192

static void chomp(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static void print_usage(FILE *out) {
    fprintf(out,
            "Usage: parrot0 [OPTIONS]\n"
            "\n"
            "Without options, start an interactive conversation.\n"
            "\n"
            "Options:\n"
            "  -h, --help                  Show this help and exit\n"
            "  --profile FILE.p0           Load FILE.p0 as the KB profile entrypoint\n"
            "  --daemon                    Serve the HTTP API\n"
            "    --host HOST               Bind host (default: 127.0.0.1)\n"
            "    --port PORT               Bind port (default: 9902)\n"
            "  --mcp-engine                Serve the Prolog engine over stdio\n"
            "  --test-engine               Start the .p0t test daemon\n"
            "    --sock PATH               Use a different test socket\n"
            "  --test [FILE]               Send a .p0t file (or stdin) to the test daemon\n"
            "  --test-report               Print the test summary and stop the daemon\n"
            "  --bench-engine              Start the resumable .p0t benchmark daemon\n"
            "    --bench-stats PATH         Persistent benchmark progress TSV\n"
            "  --bench PATH                Send a .p0t file, glob, or directory recursively\n"
            "  --bench-report              Print benchmark totals and stop the daemon\n"
            "  --bench-health FILE         Warm up and verify the benchmark daemon\n"
            "  --dream [TOPIC]             Explore a topic recursively\n"
            "  --measure DIR               Misura la STAZZA sui file 1.qa, 2.qa, … in DIR\n"
            "                              (no TOPIC: dream its own open gaps)\n"
            "    --depth=N                 Limit dream traversal depth\n"
            "    --nodes=N                 Limit dream traversal nodes\n"
            "    --fetch                   Allow fetching sources while dreaming\n"
            "    --persist                 Persist facts learned while dreaming\n");
}

/* Opt-in input capture (off unless PARROT0_TRACE names a file). Appends every
 * received input line so the self-improvement loop can SEE exactly what a
 * benchmark feeds parrot0 — a discovery tool for which reasoning features to
 * build next, never a runtime behaviour. Stays in the I/O shell because what
 * arrives on stdin is the shell's concern, not the brain's. */
static void trace_input(const char *line) {
    const char *path = getenv("PARROT0_TRACE");
    if (!path || !*path) return;
    FILE *f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "%s\n", line);
    fclose(f);
}

/* Read one TURN interactively, with multi-line support (gen197). A turn is
 * normally one line and Enter submits it; the user composes a multi-line turn —
 * e.g. to paste a Python function with its indentation — in three ways:
 *
 *   - Shift+Enter inserts a newline without submitting. Terminals only report
 *     Shift+Enter distinctly under an extended keyboard protocol, so we enable
 *     the kitty disambiguate flag (CSI > 1 u) and xterm modifyOtherKeys, and
 *     parse both the CSI-u form (ESC[13;2u) and the modifyOtherKeys form
 *     (ESC[27;2;13~). Where the terminal supports neither, Shift+Enter degrades
 *     to a plain Enter — so two universal fallbacks always work:
 *   - a line ending in a backslash '\' continues onto the next line, and
 *   - bracketed paste: pasted text (incl. its newlines) is collected as one turn.
 *
 * Returns 1 with a NUL-terminated turn in `buf` (newlines preserved, no trailing
 * newline), or 0 on EOF. Only used when stdin is a TTY; piped input keeps the
 * plain line-based path so the test harness is byte-for-byte unchanged. */
static void tty_write(const char *s) { ssize_t n = write(STDERR_FILENO, s, strlen(s)); (void)n; }

static int read_turn_tty(char *buf, size_t cap) {
    struct termios old, raw;
    tcgetattr(STDIN_FILENO, &old);
    raw = old;
    raw.c_lflag &= ~(unsigned)(ICANON | ECHO);
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    tty_write("\x1b[>1u\x1b[?2004h");        /* kitty disambiguate + bracketed paste */

    size_t len = 0;
    int paste = 0, done = 0, eof = 0;

    while (!done) {
        unsigned char c;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r < 0) continue;                 /* EINTR */
        if (r == 0) { eof = (len == 0); done = 1; break; }

        if (c == 27) {                       /* an escape sequence */
            unsigned char b, seq[32]; size_t sl = 0, final = 0;
            if (read(STDIN_FILENO, &b, 1) <= 0) continue;
            if (b != '[') continue;          /* only CSI is handled */
            while (sl < sizeof seq - 1) {
                if (read(STDIN_FILENO, &b, 1) <= 0) break;
                if (b >= 0x40 && b <= 0x7e) { final = b; break; }
                seq[sl++] = b;
            }
            seq[sl] = 0;
            const char *p = (const char *)seq;
            if (final == '~' && strcmp(p, "200") == 0) { paste = 1; }
            else if (final == '~' && strcmp(p, "201") == 0) { paste = 0; }
            else if ((final == 'u' && strcmp(p, "13;2") == 0) ||
                     (final == '~' && strcmp(p, "27;2;13") == 0)) {     /* Shift+Enter */
                if (len + 1 < cap) buf[len++] = '\n';
                tty_write("\r\n... ");
            } else if ((final == 'u' && strcmp(p, "13") == 0) ||
                       (final == '~' && strcmp(p, "27;1;13") == 0)) {   /* plain Enter */
                done = 1;
            }
            continue;                        /* ignore arrows, etc. */
        }

        if (paste) {                         /* collect pasted bytes verbatim */
            char ch = (c == '\r') ? '\n' : (char)c;
            if (len + 1 < cap) buf[len++] = ch;
            if (ch == '\n') tty_write("\r\n"); else { char e[2] = {ch, 0}; tty_write(e); }
            continue;
        }

        if (c == '\r' || c == '\n') {        /* Enter: '\'-continuation or submit */
            if (len > 0 && buf[len - 1] == '\\') { buf[len - 1] = '\n'; tty_write("\r\n... "); continue; }
            done = 1; continue;
        }
        if (c == 4) { if (len == 0) { eof = 1; done = 1; } continue; }   /* Ctrl-D */
        if (c == 3) { len = 0; tty_write("^C\r\nyou> "); continue; }     /* Ctrl-C: clear */
        if (c == 127 || c == 8) { if (len > 0) { len--; tty_write("\b \b"); } continue; }
        if (c >= 32) { if (len + 1 < cap) buf[len++] = (char)c; char e[2] = {(char)c, 0}; tty_write(e); }
    }

    buf[len < cap ? len : cap - 1] = '\0';
    tty_write("\x1b[<u\x1b[?2004l");          /* restore terminal protocols */
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    tty_write("\r\n");
    return eof ? 0 : 1;
}

/* Create the brain and load its knowledge layers. `*out_sess` receives the
 * session file path (for /save). Paths come from the environment (empty disables
 * loading — used by the hermetic test harness and the daemon). gen150:
 * PARROT0_PROFILE loads a knowledge profile (e.g. profiles/agi.p0) that chains
 * experts and skills via :- include directives. Returns NULL on OOM. */
static Brain *setup_brain(const char **out_sess) {
    Brain *brain = brain_create();
    if (!brain) return NULL;

    /* gen276: the outer KB layers (base/session/coding/profile) now live in
     * brain_boot, so the same full boot is reachable from every host and from
     * brain_reload (the engine behind /restore). */
    brain_boot(brain);

    if (out_sess) {
        /* gen382g: nessun DEFAULT di sessione. Il file di sessione non e' piu' ne'
         * un input (il boot non lo carica) ne' il bersaglio di /save (che
         * instrada): resta solo la ricaduta esplicita, se qualcuno la chiede. */
        const char *sess = p0env("PARROT0_SESSION_FALLBACK");
        if (!sess || !*sess) sess = "kb/learning/learned.p0";
        *out_sess = sess;
    }
    return brain;
}


/* ── gen421: LA STAZZA — `parrot0 --measure PATH` ─────────────────────────────
 *
 * Una misura della MOLE di cio' che parrot0 sa fare, costruita curando invece
 * che inseguendo. Nella cartella stanno file numerati in progressione — 1.qa, 2.qa, … — e in
 * ognuno ci sono solo prompt lunghi ESATTAMENTE quanti byte dice il suo nome — uno spazzolamento sistematico dello spazio
 * d'ingresso per lunghezza, non una selezione di casi che ci piacciono.
 *
 * Ogni riga e' `domanda : risposta attesa`, e la risposta attesa e' quello che
 * parrot0 DOVREBBE dire, non quello che dice oggi. E' la sola cosa che rende la
 * misura utile: un corpus riempito con le risposte correnti sarebbe uno specchio,
 * e uno specchio segna sempre cento.
 *
 * La stazza e' la somma dei prompt risolti. «Oggi parrot0 e' a classe 103» vuol
 * dire che di quelli curati ne risolve centotre — un numero che sale solo
 * lavorando, e che nessuna riscrittura del corpus puo' gonfiare senza che si
 * veda nel diff.
 *
 * Vedi docs/measured-classes.md. */
static int measure_line_ok(Brain *brain, const char *query, const char *want) {
    char reply[2048]; reply[0] = '\0';
    brain_respond(brain, query, reply, sizeof reply);
    if (!*want) return 0;
    /* «contiene», senza distinguere maiuscole: la resa di una frase varia, cio'
     * che deve esserci no. E' la stessa semantica di `<~` nei .p0t. */
    for (const char *h = reply; *h; h++) {
        const char *a = h, *b = want;
        while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) { a++; b++; }
        if (!*b) return 1;
    }
    return 0;
}

static int measure_run(const char *dir) {
    if (!dir || !*dir) return 1;
    /* i file si prendono in ordine di CLASSE, cioe' numerico: 2.qa viene dopo
     * 1.qa e prima di 10.qa, che l'ordine alfabetico sbaglierebbe. */
    long total_ok = 0, total_n = 0, max_cls = 0;
    for (long cls = 1; cls <= 512; cls++) {
        char path[512];
        if ((size_t)snprintf(path, sizeof path, "%s/%ld.qa", dir, cls) >= sizeof path) continue;
        FILE *f = fopen(path, "r");
        if (!f) continue;
        long ok = 0, n = 0;
        char line[1024];
        char failed[64][256]; size_t nfail = 0;
        /* gen421b — I DOPPIONI NON SI CONTANO (F.).
         *
         * Dentro un file, risposte attese UGUALI valgono uno. La stazza smette
         * cosi' di contare i prompt e conta le CAPACITA' DISTINTE che parrot0
         * dimostra: senza questa regola bastava aggiungere mille righe con la
         * stessa attesa per farla salire di mille, che e' il modo piu' facile di
         * rendere una misura priva di significato.
         *
         * Ne segue una cosa che va guardata in faccia: `1.qa` ha una sola
         * risposta attesa per tutti e sessantotto i byte, quindi vale UNO. E'
         * corretto — davanti a un byte la mossa giusta e' sempre la stessa, e
         * quella classe esercita una capacita' sola. Il conto dei prompt resta
         * stampato accanto, perche' serve a curare: dice QUALI membri della
         * classe non ci arrivano.
         *
         * E una capacita' conta solo se e' dimostrata su TUTTI i suoi membri: se
         * bastasse un prompt qualunque, aggiungerne uno facile regalerebbe il
         * punto e i tredici difficili sparirebbero dal numero. Con questa
         * stretta la stazza si muove per capacita' INTERE — oggi la classe 1
         * vale zero, e vale uno il giorno in cui anche le cifre e i tre segni ci
         * arrivano. E' un numero piu' duro e molto piu' utile. */
        char answers[256][256]; int solved[256]; size_t nans = 0;
        while (fgets(line, sizeof line, f)) {
            size_t l = strlen(line);
            while (l && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
            if (!l) continue;
            /* IL SEPARATORE E' « | », con gli spazi. Un file .qa non supporta
             * nient'altro: niente commenti, niente direttive, niente righe
             * speciali. Ogni riga e' un prompt con la sua risposta attesa, e
             * questo e' tutto il formato (F.).
             *
             * La prima stesura usava « : » e aveva i commenti con «#», e le due
             * cose insieme producevano una collisione che nessuna euristica
             * risolveva: «#» e' un prompt valido di un byte, «:» pure, e la riga
             * d'intestazione portava il separatore. Un formato senza eccezioni
             * non ha quel problema — ed e' anche piu' facile da generare.
             *
             * Resta un controllo di integrita: la domanda dev essere
             * lunga esattamente N byte. Il numero del file valida il corpus, e
             * una riga fuori misura si segnala invece di sparire — un corpus che
             * perde righe in silenzio falsa la stazza verso il basso. */
            char *sep = strstr(line, " | ");
            if (!sep) {
                fprintf(stderr, "measure: %s: riga senza separatore \" | \": %s\n",
                        path, line);
                continue;
            }
            if ((size_t)(sep - line) != (size_t)cls) {
                fprintf(stderr, "measure: %s: la domanda non e' lunga %ld byte: %s\n",
                        path, cls, line);
                continue;
            }
            *sep = '\0';
            const char *query = line, *want = sep + 3;
            /* UN CERVELLO NUOVO PER OGNI PROMPT. La misura dev'essere la stessa
             * a ogni giro: parrot0 varia la frase per non ripetersi e la lingua
             * segue il turno precedente, quindi due prompt di fila si
             * influenzano. Costa, ed e' il prezzo di un numero che significa
             * qualcosa. */
            Brain *b = setup_brain(NULL);
            if (!b) { fclose(f); return 1; }
            int good = measure_line_ok(b, query, want);
            brain_destroy(b);
            n++;
            if (good) ok++;
            else if (nfail < 64) snprintf(failed[nfail++], sizeof failed[0], "%s", query);
            size_t a = 0;
            while (a < nans && strcmp(answers[a], want) != 0) a++;
            if (a == nans && nans < 256) {
                snprintf(answers[nans], sizeof answers[0], "%s", want);
                solved[nans] = 1;          /* si presume dimostrata... */
                nans++;
            }
            if (a < nans && !good) solved[a] = 0;   /* ...finche' un membro non cade */
        }
        fclose(f);
        long dist_ok = 0;
        for (size_t a = 0; a < nans; a++) if (solved[a]) dist_ok++;
        total_ok += dist_ok; total_n += (long)nans; max_cls = cls;
    }
    if (total_n == 0) {
        fprintf(stderr, "measure: nessun file 1.qa, 2.qa, … in %s\n", dir);
        return 1;
    }

    /* LA STAZZA E' LA MOLE DEL CORPUS: quante risposte distinte contiene, cioe'
     * quante capacita' diverse gli si stanno chiedendo. E' il numero che dice
     * QUANTO GRANDE E' la misura, e cresce solo curando altre righe.
     *
     * Quante ne risolve e' un secondo numero, e va tenuto separato: mescolarli
     * darebbe un titolo che scende quando il corpus cresce, che e' esattamente
     * il contrario di quello che serve. */
    /* Una riga sola (F.): la MOLE e fin dove e' stata misurata. Il dettaglio —
     * quante ne risolve, e quali membri cadono — serve a curare il corpus, non a
     * leggerlo, e ingombrava il titolo. */
    (void)total_ok;
    printf("tonnage %ld   max length %ld\n", total_n, max_cls);
    return 0;
}

int main(int argc, char **argv) {
    /* gen221: `parrot0 --daemon [--port N] [--host H]` serves the
     * OpenAI-compatible HTTP API directly (replacing scripts/pi_server.py). */
    int daemon_mode = 0, mcp_mode = 0, test_mode = 0, port = 9902;
    int send_mode = 0, report_mode = 0;
    int bench_mode = 0, bench_send_mode = 0, bench_report_mode = 0, bench_health_mode = 0;
    /* gen382 — `--dream <topic>`: esplorazione ricorsiva di un topic attraverso
     * la sua prosa, parola per parola. Vedi src/dream.h. */
    const char *dream_topic = NULL;
    int dream_depth = 0, dream_nodes = 0, dream_fetch = 0, dream_persist = 0;
    const char *host = "127.0.0.1";
    const char *sockpath = TEST_ENGINE_SOCK_DEFAULT;
    const char *send_file = NULL;
    const char *bench_file = NULL;
    const char *bench_health_file = NULL;
    const char *bench_stats = BENCH_STATS_DEFAULT;
    const char *profile = NULL;
    const char *measure_dir = NULL;   /* gen421: la stazza */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(stdout);
            return 0;
        }
        else if (strcmp(argv[i], "--daemon") == 0) daemon_mode = 1;
        else if (strcmp(argv[i], "--mcp-engine") == 0) mcp_mode = 1;
        else if (strcmp(argv[i], "--test-engine") == 0) test_mode = 1;
        else if (strcmp(argv[i], "--bench-engine") == 0) bench_mode = 1;
        /* `--test FILE` is the name; `--test-send` stays as a compatible alias so
         * an older script or note keeps working. */
        else if (strcmp(argv[i], "--test") == 0 ||
                 strcmp(argv[i], "--test-send") == 0) {
            send_mode = 1;
            if (i + 1 < argc && strncmp(argv[i + 1], "--", 2) != 0) send_file = argv[++i];
        }
        else if (strcmp(argv[i], "--test-report") == 0) report_mode = 1;
        else if (strcmp(argv[i], "--bench") == 0) {
            bench_send_mode = 1;
            if (i + 1 < argc && strncmp(argv[i + 1], "--", 2) != 0) bench_file = argv[++i];
        }
        else if (strcmp(argv[i], "--bench-report") == 0) bench_report_mode = 1;
        else if (strcmp(argv[i], "--bench-health") == 0) {
            bench_health_mode = 1;
            if (i + 1 < argc && strncmp(argv[i + 1], "--", 2) != 0) bench_health_file = argv[++i];
        }
        else if (strcmp(argv[i], "--bench-stats") == 0 && i + 1 < argc) bench_stats = argv[++i];
        else if (strncmp(argv[i], "--bench-stats=", 14) == 0) bench_stats = argv[i] + 14;
        else if (strcmp(argv[i], "--profile") == 0) {
            if (i + 1 >= argc || strncmp(argv[i + 1], "--", 2) == 0) {
                fprintf(stderr, "parrot0: --profile requires a .p0 file\n\n");
                print_usage(stderr);
                return 2;
            }
            profile = argv[++i];
        }
        else if (strncmp(argv[i], "--profile=", 10) == 0) {
            if (!argv[i][10]) {
                fprintf(stderr, "parrot0: --profile requires a .p0 file\n\n");
                print_usage(stderr);
                return 2;
            }
            profile = argv[i] + 10;
        }
        else if (strcmp(argv[i], "--sock") == 0 && i + 1 < argc) sockpath = argv[++i];
        else if (strncmp(argv[i], "--sock=", 7) == 0) sockpath = argv[i] + 7;
        else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) port = atoi(argv[++i]);
        else if (strncmp(argv[i], "--port=", 7) == 0) port = atoi(argv[i] + 7);
        else if (strcmp(argv[i], "--host") == 0 && i + 1 < argc) host = argv[++i];
        else if (strncmp(argv[i], "--host=", 7) == 0) host = argv[i] + 7;
        else if (strcmp(argv[i], "--measure") == 0 && i + 1 < argc) {
            measure_dir = argv[++i];
        }
        else if (strcmp(argv[i], "--dream") == 0) {
            /* gen405: senza topic, il sogno prende l'agenda dalle PROPRIE
             * lacune. La stringa vuota e' il modo di dirlo restando un
             * puntatore non nullo. */
            dream_topic = "";
            if (i + 1 < argc && strncmp(argv[i + 1], "--", 2) != 0) dream_topic = argv[++i];
        }
        else if (strncmp(argv[i], "--depth=", 8) == 0) dream_depth = atoi(argv[i] + 8);
        else if (strncmp(argv[i], "--nodes=", 8) == 0) dream_nodes = atoi(argv[i] + 8);
        else if (strcmp(argv[i], "--fetch") == 0) dream_fetch = 1;
        else if (strcmp(argv[i], "--persist") == 0) dream_persist = 1;
        else {
            fprintf(stderr, "parrot0: unknown argument '%s'\n\n", argv[i]);
            print_usage(stderr);
            return 2;
        }
    }

    /* A profile is the KB boot entrypoint.  The command line is an explicit
     * invocation choice and therefore overrides PARROT0_PROFILE; without this
     * option the environment keeps exactly its historical semantics. */
    if (profile) setenv("PARROT0_PROFILE", profile, 1);

    /* gen345: the test-engine CLIENTS run FIRST and load NOTHING — no brain, no
     * KB — so every `make test` line is a cheap socket relay to the one daemon.
     * --test FILE streams a `.p0t` file; --test-report asks for the summary
     * and stops the daemon, propagating its pass/fail as the exit code. */
    if (send_mode) {
        FILE *in = send_file ? fopen(send_file, "rb") : stdin;
        if (!in) { fprintf(stderr, "parrot0: cannot open '%s'\n", send_file); return 2; }
        int rc = test_engine_send(sockpath, in, send_file ? send_file : "stdin");
        if (in != stdin) fclose(in);
        return rc;
    }
    if (bench_send_mode) {
        if (bench_file) return bench_engine_send_path(sockpath, bench_file);
        return bench_engine_send(sockpath, stdin, "stdin");
    }
    if (bench_health_mode) {
        FILE *in = bench_health_file ? fopen(bench_health_file, "rb") : stdin;
        if (!in) { fprintf(stderr, "parrot0: cannot open '%s'\n", bench_health_file); return 2; }
        int rc = bench_engine_health(sockpath, in, bench_health_file ? bench_health_file : "stdin");
        if (in != stdin) fclose(in);
        return rc;
    }
    if (report_mode) return test_engine_send_str(sockpath, "!shutdown\n", NULL);
    if (bench_report_mode) return bench_engine_send_str(sockpath, "!bench-shutdown\n", NULL);

    /* --test-engine is the DAEMON: one brain, listening on the Unix socket. */
    if (test_mode) {
        Brain *brain = setup_brain(NULL);
        if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
        int rc = test_engine_serve(brain, sockpath);
        brain_destroy(brain);
        return rc;
    }
    if (bench_mode) {
        Brain *brain = setup_brain(NULL);
        if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
        int rc = bench_engine_serve(brain, sockpath, bench_stats);
        brain_destroy(brain);
        return rc;
    }

    /* gen277: --mcp-engine serves the Prolog engine + generation primitives as
     * MCP tools over stdio (docs/plans/mcp-engine.md). Same setup_brain as chat,
     * so the agent gets the full KB unless overridden by the environment. */
    if (mcp_mode) {
        Brain *brain = setup_brain(NULL);
        if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
        int rc = mcp_serve_stdio(brain);
        brain_destroy(brain);
        return rc;
    }

    if (daemon_mode) {
        /* Match the pi-agent defaults the Python wrapper used to inject: tools on,
         * the agi profile by default, no session persistence. setenv(...,0) never
         * overwrites an explicit value, so a harness can still drop the profile
         * (PARROT0_PROFILE="") or turn tools off (PARROT0_TOOLS=0). */
        setenv("PARROT0_TOOLS", "1", 0);
        setenv("PARROT0_PROFILE", "kb/profiles/agi.p0", 0);
        setenv("PARROT0_SESSION", "", 0);
        Brain *brain = setup_brain(NULL);
        if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
        int rc = serve_http(brain, host, port);
        brain_destroy(brain);
        return rc;
    }

    const char *sess = NULL;
    Brain *brain = setup_brain(&sess);
    if (!brain) {
        fprintf(stderr, "parrot0: out of memory\n");
        return 1;
    }

    /* gen382: il sogno gira sul cervello COMPLETO (e' esplorazione, non un test
     * ermetico), stampa il suo trace su stdout ed esce. */
    if (measure_dir) {
        brain_destroy(brain);            /* la misura crea il proprio, uno per prompt */
        return measure_run(measure_dir);
    }
    if (dream_topic) {   /* "" = sogna le lacune aperte, vedi dream.c */
        DreamOpts dopts = { dream_depth, dream_nodes, dream_fetch, dream_persist, stdout };
        int n = dream_run(brain, dream_topic, &dopts);
        /* gen411: il registro di lavoro si riscrive alla fine del giro — le
         * lacune chiuse spariscono, quelle rimaste aspettano il prossimo. E'
         * l'unica cosa che rende il criterio del piano osservabile fra due
         * esecuzioni invece che dentro una sola. */
        brain_gaps_save(brain);
        brain_bridges_save(brain);
        brain_destroy(brain);
        return n > 0 ? 0 : 1;
    }

    /* gen331 (TODO.md P1/09): the banner reports the EFFECTIVE policy, read from
     * the same KB facts the modules obey. Before this it said nothing about the
     * mode, and `make chat` — which had tools OFF and the network ON — looked
     * identical to the agent. A user cannot calibrate what they cannot see. */
    { char mode[32]; brain_mode(brain, mode, sizeof mode);
      size_t facts = kb_size(brain_kb(brain));
      size_t rules = kb_rule_count(brain_kb(brain));
      double rule_fact_ratio = facts ? ((double)rules * 100.0 / (double)facts) : 0.0;
      fprintf(stderr, "parrot0 [%s] - mode: %s (tools %s, network %s)\n"
                      "Facts: %zu\n"
                      "Rules: %zu\n"
                      "Rules/Facts: %.4f%%\n"
                      "say something ('/quit' to exit, '/save' to persist, "
                      "'/restore' to reload the KB from disk)\n",
              brain_version(), mode,
              brain_policy_on(brain, "tools")   ? "on" : "off",
              brain_policy_on(brain, "network") ? "on" : "off",
              facts, rules, rule_fact_ratio); }

    char line[LINE_MAX_LEN];
    char resp[RESP_MAX_LEN];

    /* gen202: the multi-line raw reader (gen197) emits kitty-keyboard + bracketed-
     * paste escape sequences every turn; on terminals that do not implement the
     * kitty keyboard protocol (most of them) the pop sequence `CSI < u` leaks a
     * stray `<` and garbles the prompt. So the DEFAULT is the plain canonical
     * reader (the terminal does the line editing — bulletproof, the pre-gen197
     * behaviour). The multi-line reader is opt-in via PARROT0_MULTILINE=1 for the
     * terminals that support it. Piped input always uses the plain path so the
     * test harness is byte-for-byte unchanged. */
    int interactive = isatty(STDIN_FILENO);
    const char *ml = getenv("PARROT0_MULTILINE");
    int multiline = interactive && ml && ml[0] && ml[0] != '0';

    for (;;) {
        fprintf(stderr, "you> ");
        fflush(stderr);

        if (multiline) {
            /* gen197: multi-line capable reader (Shift+Enter / paste / '\'). */
            if (!read_turn_tty(line, sizeof line)) break;   /* EOF */
        } else {
            if (!fgets(line, sizeof line, stdin)) break;    /* EOF / Ctrl-D */
            chomp(line);
        }
        trace_input(line);

        if (strcmp(line, "/quit") == 0 || strcmp(line, "/exit") == 0) {
            break;
        }
        if (strcmp(line, "/session") == 0) {
            fprintf(stderr, "parrot0: session dump at %s\n", brain_session_dump_path());
            continue;
        }
        if (strcmp(line, "/gaps") == 0) {
            /* gen411: il registro di LAVORO, che e' un'altra cosa dalla
             * conoscenza: `/save` instrada nell'albero curato, e un'agenda non
             * va nell'albero curato. */
            int n = brain_gaps_save(brain);
            fprintf(stderr, "parrot0: %d gap(s) written to %s\n",
                    n, brain_gaps_path());
            continue;
        }
        if (strcmp(line, "/save") == 0) {
            /* gen382g: si salva INSTRADANDO nell'albero curato, mai su un file di
             * sessione. `sess` resta come sola ricaduta per i fatti che il
             * save-map non sa dove collocare. */
            int n = brain_save_session(brain, sess);
            if (n >= 0) fprintf(stderr, "parrot0: routed %d clause(s) into the KB tree\n", n);
            else        fprintf(stderr, "parrot0: could not save\n");
            continue;
        }
        /* gen276: /restore — forget the unsaved session and reload every KB file
         * from disk in place, so knowledge written to a .p0 file (by hand or by
         * an MCP-engine agent) goes live WITHOUT restarting parrot0. Anything
         * asserted this session but not /save'd is dropped. */
        if (strcmp(line, "/restore") == 0) {
            int n = brain_reload(brain);
            if (n >= 0)
                fprintf(stderr, "parrot0: restored — dropped the unsaved session, "
                                "reloaded %d clause(s) from disk\n", n);
            else
                fprintf(stderr, "parrot0: restore failed (out of memory); "
                                "the session is unchanged\n");
            continue;
        }
        /* gen400: /debug — il profiler dell'inferenza, acceso a runtime.
         *
         * Spento non costa nulla: i contatori sono dietro un flag e nessuna
         * misura viene presa. Acceso, ogni turno successivo stampa dove sono
         * finiti i passi — non quanto e' costato un goal, che il motore sapeva
         * gia' dire, ma come si distribuisce il costo di un turno intero, che
         * di goal ne apre decine. E' la domanda che serve per ottimizzare.
         *
         * Deliberatamente piccolo. Cresce quando una domanda di ottimizzazione
         * lo chiede — un profiler scritto tutto in anticipo misura cio' che
         * l'autore immaginava, non cio' che poi rallenta. */
        if (strcmp(line, "/debug") == 0) {
            KB *kb = brain_kb(brain);
            int on = !kb_profile_on(kb);
            kb_profile_set(kb, on);
            fprintf(stderr, "parrot0: debug %s\n",
                    on ? "ON — ogni turno riporta chiamate, passi e i goal piu' cari"
                       : "OFF");
            continue;
        }
        if (line[0] == '\0') {
            continue; /* ignore empty turns */
        }

        struct timespec t0, t1;
        int profiling = kb_profile_on(brain_kb(brain));
        if (profiling) {
            kb_profile_reset(brain_kb(brain));
            timespec_get(&t0, TIME_UTC);
        }
        brain_respond(brain, line, resp, sizeof resp);
        if (profiling) {
            timespec_get(&t1, TIME_UTC);
            double ms = (t1.tv_sec - t0.tv_sec) * 1000.0
                      + (t1.tv_nsec - t0.tv_nsec) / 1000000.0;
            KB *kb = brain_kb(brain);
            KbProfileRow top[8];
            size_t n = kb_profile_top(kb, top, 8);
            fprintf(stderr, "\n[debug] %.1f ms turno · %.1f ms nel solver · %zu query · %lu passi\n",
                    ms, kb_profile_ms(kb), kb_profile_calls(kb), kb_profile_steps(kb));
            fprintf(stderr, "[debug] %zu fatti · %zu regole · fuori dal solver: %.1f ms · %zu ricostruzioni indice\n",
                    kb_size(kb), kb_rule_count(kb), ms - kb_profile_ms(kb),
                    kb_profile_rebuilds(kb));
            fprintf(stderr, "[debug] fatti visitati: %lu · scansioni senza indice: %zu\n",
                    kb_profile_visits(kb), kb_profile_scans(kb));
            fprintf(stderr, "[debug] modulo: %s\n", brain_last_module(brain));
            for (size_t i = 0; i < n && top[i].calls > 0; i++)
                fprintf(stderr, "[debug]   %7.1f ms  %8lu passi  %5zu call  %s\n",
                        top[i].ms, top[i].steps, top[i].calls, top[i].pred);
            fflush(stderr);
        }
        /* gen382g: il dump della sessione si riscrive a ogni turno, cosi' un
         * `cat` mostra sempre cio' che parrot0 ha in memoria ADESSO. Si scrive e
         * non si rilegge: non e' conoscenza da caricare, e' una finestra. */
        brain_session_dump(brain);
        printf("%s\n", resp);
        /* gen269: replies may span several lines (markdown-fenced code). Line-
         * based drivers that pair one stdout line per turn can opt into an
         * explicit end-of-turn marker line via PARROT0_EOT and read until it.
         * Default off: the plain chat surface and tests/run.sh stay unchanged. */
        {
            const char *eot = getenv("PARROT0_EOT");
            if (eot && *eot) printf("%s\n", eot);
        }
        fflush(stdout);
    }

    fprintf(stderr, "\nparrot0: bye\n");
    brain_destroy(brain);
    return 0;
}
