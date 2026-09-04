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

#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "brain.h"
#include "kb.h"
#include "serve.h"
#include "mcp.h"
#include "testeng.h"
#include "dream.h"
#include "env.h"

#include <ctype.h>
#include <errno.h>
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
            "  --coverage PATH             Che cosa un corpus non mette mai alla prova (NON e' un audit della KB)\n"
            "  --footprint                 Firma del ragionamento, un prompt per riga da stdin\n"
            "                              (no TOPIC: dream its own open gaps)\n"
            "    --depth=N                 Limit dream traversal depth\n"
            "    --nodes=N                 Limit dream traversal nodes\n"
            "    --debug                   Show dream prompt interpretation\n"
            "                              (Wikipedia is fetched automatically in memory)\n"
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
/* ── COLORE: MECCANICA DI TERMINALE, NON TESTO ────────────────────────────
 *
 * Il mantra #16 vincola cio' che parrot0 DICE — le frasi — a stare in KB. Una
 * sequenza ANSI non e' una frase: e' la stessa famiglia di `\x1b[?2004h` che
 * questo file gia' scrive per il bracketed paste. Il testo del banner e del
 * prompt qui sotto non cambia di un carattere; cambia solo come viene dipinto.
 *
 * E degrada in silenzio: senza tty, con `NO_COLOR` o su `TERM=dumb` l'uscita
 * torna byte per byte quella storica, cosi' le pipe, i bench che leggono il
 * prompt e le suite non vedono nulla di diverso. */
static int use_color(void) {
    static int cached = -1;
    if (cached >= 0) return cached;
    const char *no = getenv("NO_COLOR");
    const char *term = getenv("TERM");
    cached = isatty(STDERR_FILENO) && !(no && *no) &&
             !(term && strcmp(term, "dumb") == 0);
    return cached;
}
#define C(code) (use_color() ? "\x1b[" code "m" : "")
#define C_OFF   (use_color() ? "\x1b[0m" : "")

/* Il prompt: tre chevron in gradiente, dal piu' scuro al piu' chiaro. Senza
 * colore resta la forma storica, che i bench sanno gia' leggere. */
static const char *prompt_str(void) {
    return use_color()
        ? "\x1b[38;5;25m>\x1b[38;5;39m>\x1b[38;5;81m>\x1b[0m "
        : "you> ";
}

static void tty_write(const char *s) { ssize_t n = write(STDERR_FILENO, s, strlen(s)); (void)n; }

/* gen500: a coding-agent terminal must expose liveness while the KB or a turn
 * is being evaluated.  This pulse is terminal mechanics, like the cursor and
 * bracketed-paste escapes above: it says no natural-language sentence and
 * contains no domain decision.  Pipe/API/test surfaces remain byte-identical.
 * A tiny child is used instead of a thread so the reasoning engine acquires no
 * concurrent access to Brain/KB state. */
static pid_t activity_pulse_start(void) {
    if (!isatty(STDIN_FILENO) || !isatty(STDERR_FILENO)) return (pid_t)-1;
    pid_t pid = fork();
    if (pid != 0) return pid;
    static const char frame[] = "|/-\\";
    struct timespec pause = {0, 200 * 1000 * 1000};
    size_t i = 0;
    for (;;) {
        char out[8];
        int n = snprintf(out, sizeof out, "\r[%c]", frame[i++ % 4]);
        if (n > 0) {
            ssize_t written = write(STDERR_FILENO, out, (size_t)n);
            (void)written;
        }
        nanosleep(&pause, NULL);
    }
}

static void activity_pulse_stop(pid_t pid) {
    if (pid <= 0) return;
    (void)kill(pid, SIGTERM);
    while (waitpid(pid, NULL, 0) < 0 && errno == EINTR) { }
    tty_write("\r   \r");
}

/* ── LA STORIA DEI TURNI, CON LE FRECCE ───────────────────────────────────
 *
 * Il lettore canonico (`fgets`) lascia l'editing al terminale, che senza
 * readline non offre ne' storia ne' movimento del cursore: freccia su usciva
 * come `^[[A` dentro il testo del turno. Qui c'e' un editor minimo in raw mode,
 * attivo SOLTANTO quando stdin e' un tty interattivo — le pipe, i bench e le
 * suite restano sul percorso `fgets` byte per byte.
 *
 * Sceglie deliberatamente di NON accendere i protocolli kitty/bracketed-paste
 * che il lettore multilinea usa: sono la ragione per cui quel lettore e' rimasto
 * opt-in (un `CSI < u` non capito sporca il prompt su quasi tutti i terminali).
 * Qui si usa solo termios, che funziona ovunque.
 *
 * Il movimento e' consapevole di UTF-8: una lettera accentata e' piu' byte e
 * cancellarne uno solo lascerebbe mezzo carattere sullo schermo. */
#define HIST_MAX 200
static char *hist_buf[HIST_MAX];
static size_t hist_n = 0;

static void hist_push(const char *s) {
    if (!s || !*s) return;
    if (hist_n && strcmp(hist_buf[hist_n - 1], s) == 0) return;   /* no duplicati adiacenti */
    char *copy = strdup(s);
    if (!copy) return;
    if (hist_n == HIST_MAX) {
        free(hist_buf[0]);
        memmove(hist_buf, hist_buf + 1, (HIST_MAX - 1) * sizeof *hist_buf);
        hist_n--;
    }
    hist_buf[hist_n++] = copy;
}

/* Confine di carattere UTF-8: i byte di continuazione stanno in 0x80..0xBF. */
static size_t utf8_prev(const char *b, size_t i) {
    if (i == 0) return 0;
    do { i--; } while (i > 0 && ((unsigned char)b[i] & 0xC0) == 0x80);
    return i;
}
static size_t utf8_next(const char *b, size_t i, size_t len) {
    if (i >= len) return len;
    do { i++; } while (i < len && ((unsigned char)b[i] & 0xC0) == 0x80);
    return i;
}
static size_t utf8_count(const char *b, size_t len) {
    size_t n = 0;
    for (size_t i = 0; i < len; i++)
        if (((unsigned char)b[i] & 0xC0) != 0x80) n++;
    return n;
}

/* Ridisegna la riga: prompt, testo, cancella la coda, riporta il cursore.
 * Le sequenze del prompt sono a larghezza zero, quindi la colonna si conta
 * sui caratteri del buffer, non sui byte. */
static void line_redraw(const char *buf, size_t len, size_t cur) {
    tty_write("\r");
    tty_write(prompt_str());
    ssize_t w = write(STDERR_FILENO, buf, len); (void)w;
    tty_write("\x1b[K");
    size_t back = utf8_count(buf + cur, len - cur);
    if (back) {
        char mv[32];
        snprintf(mv, sizeof mv, "\x1b[%zuD", back);
        tty_write(mv);
    }
}

static int read_line_history(char *buf, size_t cap) {
    struct termios old, raw;
    if (tcgetattr(STDIN_FILENO, &old) != 0) return -1;   /* nessun tty: ricadi */
    raw = old;
    raw.c_lflag &= ~(unsigned)(ICANON | ECHO);
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    size_t len = 0, cur = 0;
    /* `hist_at == hist_n` significa «sto scrivendo un turno nuovo»; la bozza in
     * corso viene messa da parte quando si sale, e riappare tornando in fondo. */
    size_t hist_at = hist_n;
    char draft[LINE_MAX_LEN]; size_t draft_len = 0;
    int eof = 0, done = 0;
    buf[0] = '\0';
    line_redraw(buf, len, cur);

    while (!done) {
        unsigned char c;
        if (read(STDIN_FILENO, &c, 1) != 1) { eof = (len == 0); done = 1; break; }

        if (c == '\r' || c == '\n') { done = 1; continue; }
        if (c == 3) {                                   /* Ctrl-C: azzera */
            len = cur = 0; buf[0] = '\0';
            tty_write("^C\r\n");
            line_redraw(buf, len, cur);
            continue;
        }
        if (c == 4) {                                   /* Ctrl-D */
            if (len == 0) { eof = 1; done = 1; continue; }
            if (cur < len) {                            /* cancella avanti */
                size_t nx = utf8_next(buf, cur, len);
                memmove(buf + cur, buf + nx, len - nx);
                len -= (nx - cur); buf[len] = '\0';
                line_redraw(buf, len, cur);
            }
            continue;
        }
        if (c == 127 || c == 8) {                       /* backspace */
            if (cur > 0) {
                int at_end = (cur == len);
                size_t pv = utf8_prev(buf, cur);
                memmove(buf + pv, buf + cur, len - cur);
                len -= (cur - pv); cur = pv; buf[len] = '\0';
                /* Una colonna sola, anche se il carattere cancellato occupava
                 * piu' byte: e' la larghezza sullo schermo che conta. */
                if (at_end) tty_write("\b \b");
                else line_redraw(buf, len, cur);
            }
            continue;
        }
        if (c == 1)  { cur = 0; line_redraw(buf, len, cur); continue; }        /* Ctrl-A */
        if (c == 5)  { cur = len; line_redraw(buf, len, cur); continue; }      /* Ctrl-E */
        if (c == 21) { memmove(buf, buf + cur, len - cur); len -= cur; cur = 0; /* Ctrl-U */
                       buf[len] = '\0'; line_redraw(buf, len, cur); continue; }
        if (c == 11) { len = cur; buf[len] = '\0';                              /* Ctrl-K */
                       line_redraw(buf, len, cur); continue; }
        if (c == 23) {                                                          /* Ctrl-W */
            size_t e = cur;
            while (e > 0 && buf[e - 1] == ' ') e--;
            while (e > 0 && buf[e - 1] != ' ') e--;
            memmove(buf + e, buf + cur, len - cur);
            len -= (cur - e); cur = e; buf[len] = '\0';
            line_redraw(buf, len, cur);
            continue;
        }

        if (c == 27) {                                  /* sequenza di escape */
            unsigned char a, b2;
            if (read(STDIN_FILENO, &a, 1) != 1) continue;
            if (a != '[' && a != 'O') continue;
            if (read(STDIN_FILENO, &b2, 1) != 1) continue;
            if (b2 >= '0' && b2 <= '9') {               /* forma CSI n ~ */
                unsigned char t;
                if (read(STDIN_FILENO, &t, 1) != 1) continue;
                if (t == '~' && b2 == '3' && cur < len) {           /* Canc */
                    size_t nx = utf8_next(buf, cur, len);
                    memmove(buf + cur, buf + nx, len - nx);
                    len -= (nx - cur); buf[len] = '\0';
                    line_redraw(buf, len, cur);
                } else if (t == '~' && (b2 == '1' || b2 == '7')) {
                    cur = 0; line_redraw(buf, len, cur);
                } else if (t == '~' && (b2 == '4' || b2 == '8')) {
                    cur = len; line_redraw(buf, len, cur);
                }
                continue;
            }
            if (b2 == 'D') { if (cur > 0) { cur = utf8_prev(buf, cur);
                                            line_redraw(buf, len, cur); } continue; }
            if (b2 == 'C') { if (cur < len) { cur = utf8_next(buf, cur, len);
                                              line_redraw(buf, len, cur); } continue; }
            if (b2 == 'H') { cur = 0; line_redraw(buf, len, cur); continue; }
            if (b2 == 'F') { cur = len; line_redraw(buf, len, cur); continue; }
            if (b2 == 'A') {                            /* freccia su */
                if (hist_at == 0) continue;
                if (hist_at == hist_n) {                /* metti da parte la bozza */
                    memcpy(draft, buf, len); draft_len = len;
                }
                hist_at--;
                snprintf(buf, cap, "%s", hist_buf[hist_at]);
                len = strlen(buf); cur = len;
                line_redraw(buf, len, cur);
                continue;
            }
            if (b2 == 'B') {                            /* freccia giu' */
                if (hist_at >= hist_n) continue;
                hist_at++;
                if (hist_at == hist_n) {                /* torna alla bozza */
                    memcpy(buf, draft, draft_len); len = draft_len;
                } else {
                    snprintf(buf, cap, "%s", hist_buf[hist_at]);
                    len = strlen(buf);
                }
                buf[len] = '\0'; cur = len;
                line_redraw(buf, len, cur);
                continue;
            }
            continue;
        }

        if (c >= 32) {                                  /* inserisci al cursore */
            if (len + 1 >= cap) continue;
            int at_end = (cur == len);
            memmove(buf + cur + 1, buf + cur, len - cur);
            buf[cur] = (char)c;
            len++; cur++; buf[len] = '\0';
            /* Scrivere in coda — il caso normale — costa un byte invece del
             * ridisegno dell'intera riga: su una riga lunga o su una
             * connessione lenta la differenza si vede. */
            if (at_end) { char e[2] = { (char)c, 0 }; tty_write(e); }
            else line_redraw(buf, len, cur);
        }
    }

    buf[len < cap ? len : cap - 1] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    tty_write("\r\n");
    if (!eof) hist_push(buf);
    return eof ? 0 : 1;
}

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
        if (c == 3) { len = 0; tty_write("^C\r\n"); tty_write(prompt_str()); continue; }     /* Ctrl-C: clear */
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
 * La stazza e' la somma dei prompt risolti. «Oggi parrot0 e' a stazza 103» vuol
 * dire che di quelli curati ne risolve centotre — un numero che sale solo
 * lavorando, e che nessuna riscrittura del corpus puo' gonfiare senza che si
 * veda nel diff.
 *
 * Vedi docs/measured-classes.md. */
static int measure_line_ok(Brain *brain, const char *query, const char *want,
                           unsigned long *out_fp, unsigned long *out_rh) {
    char reply[2048]; reply[0] = '\0';
    brain_respond(brain, query, reply, sizeof reply);
    if (out_fp) *out_fp = brain_footprint(brain);
    if (out_rh) {   /* la RISPOSTA, ridotta a un numero per confrontarla */
        unsigned long h = 5381;
        for (const char *p = reply; *p; p++) h = h * 33u ^ (unsigned char)*p;
        *out_rh = h;
    }
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

/* gen435 — `parrot0 --coverage PATH`: CHE COSA UN CORPUS NON METTE MAI ALLA PROVA.
 *
 * ⛔ NON e' un audit della KB, e nasceva come tale: era «la conoscenza che non ha
 * mai fatto niente», cioe' una scansione a freddo per trovare lacune e colmarle.
 * F. l'ha respinto e ha ragione: quello e' un DEFRAG della KB. Una lacuna non e'
 * un'assenza nella KB — e' un ARRESTO nell'inferenza, e si scopre DENTRO un
 * turno, perche' la scoperta della lacuna E' parte dell'inferenza
 * (docs/autocorrezione.md §0, docs/plans/autocrescita.md §0a «il perimetro»).
 *
 * Quel che resta, e che e' onesto, misura I BANCHI e non la KB.
 *
 * I sette difetti del gen427-432 erano tutti della stessa specie — conoscenza
 * dichiarata che NON POTEVA funzionare: la riga della sterlina confrontata su un
 * carattere solo, i frame al passato uccisi dalla normalizzazione della copula,
 * diciotto registri letti a sedici. Nessuno di loro si e' mai lamentato, e sono
 * emersi per caso. Un fatto che non combacia non si lamenta: qui comincia a
 * farlo.
 *
 * Il metodo e' semplice e per questo affidabile: si riproduce un corpus con
 * l'audit acceso, ogni fatto che unifica si segna, e alla fine si guarda chi non
 * si e' mai segnato. Il motore MISURA; il giudizio su che cosa quel silenzio
 * significhi sta in KB (`dormant_by_design/1`), perche' distinguere una
 * conoscenza che tace per disegno da una che tace per un difetto richiede di
 * sapere a che cosa serve — e quello il motore non lo sa. */
static int coverage_run(const char *const *roots, size_t nroots) {
    Brain *b = setup_brain(NULL);
    if (!b) return 1;
    KB *kb = brain_kb(b);
    kb_audit_set(kb, 1);

    /* PIU' CORPORA, e non e' un dettaglio: l'audit e' relativo a cio' che ha
     * VISTO. Misurato: `stopword` risulta muto sui cento — che sono prompt
     * analitici lunghi — e si accende con tre turni ordinari. Un corpus stretto
     * fa sembrare morta della conoscenza sanissima, quindi l'unico uso onesto e'
     * l'unione dei banchi che ci sono. */
    char paths[64][512];
    size_t np = 0;
    for (size_t r = 0; r < nroots && np < 64; r++) {
        const char *path = roots[r];
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            DIR *d = opendir(path);
            struct dirent *e;
            while (d && (e = readdir(d)) && np < 64) {
                const char *dot = strrchr(e->d_name, '.');
                if (!dot || strcmp(dot, ".qa") != 0) continue;
                snprintf(paths[np++], sizeof paths[0], "%s/%s", path, e->d_name);
            }
            if (d) closedir(d);
        } else {
            snprintf(paths[np++], sizeof paths[0], "%s", path);
        }
    }
    if (np == 0) { fprintf(stderr, "audit: nessun corpus\n"); return 1; }

    size_t turns = 0;
    char reply[4096];
    for (size_t i = 0; i < np; i++) {
        FILE *f = fopen(paths[i], "r");
        if (!f) continue;
        char line[1024];
        while (fgets(line, sizeof line, f)) {
            size_t l = strlen(line);
            while (l && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
            if (!l) continue;
            char *bar = strstr(line, " | ");     /* i .qa portano l'attesa accanto */
            if (bar) *bar = '\0';
            brain_respond(b, line, reply, sizeof reply);
            turns++;
        }
        fclose(f);
    }

    /* IL RAPPORTO. Ordinato per quanto silenzio: un predicato con cento fatti e
     * cento silenzi e' una cosa diversa da uno con cento fatti e un silenzio. */
    static char preds[4096][KB_TERM_LEN];
    static size_t unused[4096], total[4096];
    size_t n = kb_unused_by_pred(kb, preds, unused, total, 4096);
    size_t tot_facts = 0, tot_unused = 0, silent_preds = 0;
    for (size_t i = 0; i < n; i++) {
        tot_facts += total[i];
        tot_unused += unused[i];
        if (unused[i] == total[i]) silent_preds++;
    }
    /* gen435b — NON SI CENSISCE L'ASSENZA (F.).
     *
     * La prima stesura apriva con «13.935 fatti mai usati, 85%». E' il numero
     * sbagliato: su una KB grande e' illimitato e quasi tutto sano — i fatti di
     * mondo SONO lo spazio delle risposte, non buchi da tappare — e leggerlo
     * spinge a «usarli tutti», che e' completezza, non vita. La KB non si
     * completa: si anima.
     *
     * Quello che vale e' un'altra cosa, ed e' piccola: la conoscenza che si
     * dichiara MECCANICA — cioe' che qualcuno ha promesso di consumare — e che
     * non fira mai. Non e' un buco: e' un ARCO DICHIARATO E NON PERCORRIBILE, la
     * specie della sterlina confrontata su un carattere solo. Quella e' una
     * promessa che il motore non puo' mantenere, ed e' sempre un difetto. */
    (void)tot_facts; (void)tot_unused; (void)silent_preds;
    printf("coverage  %zu corpora · %zu turni — CHE COSA QUESTI BANCHI NON METTONO MAI ALLA PROVA:\n\n",
           np, turns);

    /* I PREDICATI COMPLETAMENTE MUTI, che sono i sospetti veri: nessuna delle
     * loro righe ha mai fatto niente. Chi si dichiara dormiente per disegno esce
     * dall'elenco — e quella dichiarazione e' conoscenza, non un ramo. */
    /* Solo la MECCANICA, cioe' cio' che ha un consumatore dichiarato. Un fatto di
     * mondo che nessuno ha chiesto non e' morto: e' semplicemente non chiesto. */
    size_t shown = 0, skipped = 0, arcs = 0;
    for (size_t i = 0; i < n; i++) {
        if (unused[i] == 0) continue;
        const char *mq[1] = { preds[i] };
        if (!kb_query(kb, "machinery", mq, 1)) continue;   /* solo cio' che promette */
        if (kb_query(kb, "dormant_by_design", mq, 1)) { skipped++; continue; }
        arcs++;
        if (shown >= 30) continue;
        char row[256] = "";
        if (!kb_first_unused_row(kb, preds[i], row, sizeof row)) continue;
        printf("  %-26s %3zu/%-4zu   es. %s\n", preds[i], unused[i], total[i], row);
        shown++;
    }
    if (arcs > shown) printf("  … e altri %zu\n", arcs - shown);
    if (arcs == 0) printf("  (nessuno: ogni meccanica dichiarata ha percorso almeno una riga)\n");
    printf("\n%zu meccaniche con righe mai percorse", arcs);
    if (skipped) printf(" · %zu dichiarate dormienti per disegno", skipped);
    printf("\n\nCHE COSA QUESTO NUMERO E' E CHE COSA NON E' (gen435b, F.).\n"
           "NON e' un elenco di lacune da riempire, e la KB non si completa: si anima.\n"
           "E non prova nemmeno che una riga sia morta — «mai chiesta» e «non puo'\n"
           "combaciare» sono indistinguibili da un conteggio su un corpus, e infatti\n"
           "in questo elenco le due specie stanno mescolate.\n"
           "E' una misura DEI BANCHI, non della KB: dice quali meccaniche dichiarate\n"
           "questi corpora non esercitano mai — cioe' dove le prove sono strette.\n");
    brain_destroy(b);
    return 0;
}

/* gen422 — `parrot0 --footprint`: legge un prompt per riga da stdin e stampa la
 * firma del ragionamento che ha prodotto la risposta.
 *
 * Serve a due cose: a riempire la colonna centrale dei file .qa, e a verificare
 * l'invariante che da' senso a tutta la firma — prompt DIVERSI con VALORI
 * diversi ma risolti dalla stessa inferenza devono portare la stessa firma. */
static int footprint_run(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        size_t l = strlen(line);
        while (l && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
        if (!l) continue;
        Brain *b = setup_brain(NULL);
        if (!b) return 1;
        char reply[2048]; reply[0] = '\0';
        brain_respond(b, line, reply, sizeof reply);
        printf("%08lx\t%3zu\t%s\n", brain_footprint(b) & 0xfffffffful, brain_footprint_width(b), line);
        brain_destroy(b);
    }
    return 0;
}

static int measure_run(const char *dir) {
    if (!dir || !*dir) return 1;
    /* i file si prendono in ordine di CLASSE, cioe' numerico: 2.qa viene dopo
     * 1.qa e prima di 10.qa, che l'ordine alfabetico sbaglierebbe. */
    long total_ok = 0, total_n = 0, max_len = 0;
    for (long len_class = 1; len_class <= 512; len_class++) {
        char path[512];
        if ((size_t)snprintf(path, sizeof path, "%s/%ld.qa", dir, len_class) >= sizeof path) continue;
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
         * stretta la stazza si muove per capacita' INTERE — oggi la lunghezza 1
         * vale zero, e vale uno il giorno in cui anche le cifre e i tre segni ci
         * arrivano. E' un numero piu' duro e molto piu' utile. */
        /* gen422c — SI CONTA PER FIRMA, non per risposta attesa (F.).
         *
         * Una risposta diversa prodotta dalla STESSA STRADA non e' un'abilita'
         * nuova: e' lo stesso ragionamento con altri valori. Contare le firme
         * chiude anche la crepa che la lunghezza 3 aveva aperto — «1+1|2»,
         * «9-4|5», «2*3|6» sono tre risposte diverse e una firma sola, quindi
         * valgono uno, e aggiungerne cento con un ciclo `for` non regala piu'
         * niente.
         *
         * Cambia anche cosa misura la stazza, ed e' bene dirlo: non piu' quante
         * cose il corpus CHIEDE, ma quante strade diverse parrot0 PERCORRE su
         * quel corpus. E' la varieta' comportamentale, nel bene e nel male. */
        /* gen424 — UN PUNTO E' UNA COPPIA NUOVA (F.).
         *
         * Non basta che sia nuova la firma, e non basta che sia nuova la
         * risposta: devono esserlo ENTRAMBE. Una strada nuova che produce
         * un'uscita gia' vista non e' comportamento nuovo visto da fuori;
         * un'uscita nuova prodotta da una strada gia' vista non e' comportamento
         * nuovo visto da dentro. Il punto e' la coppia (come, cosa).
         *
         * E' l'INTERSEZIONE delle due regole precedenti, quindi piu' stretta di
         * entrambe: tiene chiusa la crepa delle famiglie parametriche (cento
         * addizioni condividono la firma) e in piu' non regala niente a strade
         * diverse che finiscono per dire la stessa frase. */
        unsigned long paths[256], replies[256]; int solved[256]; size_t nans = 0;
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
            /* TRE COLONNE: `domanda | firma | risposta attesa` (F., gen422).
             * La firma sta in mezzo perche' e' una proprieta' del PERCORSO, e il
             * percorso viene prima della risposta. */
            char *sep = strstr(line, " | ");
            if (!sep) {
                fprintf(stderr, "measure: %s: riga senza separatore \" | \": %s\n",
                        path, line);
                continue;
            }
            char *sep2 = strstr(sep + 3, " | ");
            if (!sep2) {
                fprintf(stderr, "measure: %s: riga senza la colonna della firma: %s\n",
                        path, line);
                continue;
            }
            if ((size_t)(sep - line) != (size_t)len_class) {
                fprintf(stderr, "measure: %s: la domanda non e' lunga %ld byte: %s\n",
                        path, len_class, line);
                continue;
            }
            *sep = '\0'; *sep2 = '\0';
            const char *query = line, *fp_want = sep + 3, *want = sep2 + 3;
            /* UN CERVELLO NUOVO PER OGNI PROMPT. La misura dev'essere la stessa
             * a ogni giro: parrot0 varia la frase per non ripetersi e la lingua
             * segue il turno precedente, quindi due prompt di fila si
             * influenzano. Costa, ed e' il prezzo di un numero che significa
             * qualcosa. */
            Brain *b = setup_brain(NULL);
            if (!b) { fclose(f); return 1; }
            unsigned long fp = 0, rh = 0;
            int good = measure_line_ok(b, query, want, &fp, &rh);
            fp &= 0xfffffffful;   /* la firma si scrive a 32 bit, come --footprint */
            brain_destroy(b);
            /* la firma si segnala quando e' cambiata: vuol dire che il turno ha
             * preso una strada diversa da quella registrata, e va guardato anche
             * se la risposta e' ancora giusta. */
            unsigned long fp_rec = strtoul(fp_want, NULL, 16);
            if (fp_rec && fp_rec != fp)
                fprintf(stderr, "measure: %s: [%s] firma %08lx, attesa %08lx\n",
                        path, query, fp, fp_rec);
            n++;
            if (good) ok++;
            else if (nfail < 64) snprintf(failed[nfail++], sizeof failed[0], "%s", query);
            /* nuova solo se NE' la firma NE' la risposta si sono gia' viste */
            int fp_seen = 0, rh_seen = 0;
            size_t a = nans;
            for (size_t k = 0; k < nans; k++) {
                if (paths[k] == fp)   { fp_seen = 1; a = k; }
                if (replies[k] == rh) { rh_seen = 1; if (a == nans) a = k; }
            }
            if (!fp_seen && !rh_seen && nans < 256) {
                paths[nans] = fp; replies[nans] = rh;
                solved[nans] = 1;          /* si presume dimostrata... */
                a = nans; nans++;
            }
            if (a < nans && !good) solved[a] = 0;   /* ...finche' un membro non cade */
        }
        fclose(f);
        long dist_ok = 0;
        for (size_t a = 0; a < nans; a++) if (solved[a]) dist_ok++;
        total_ok += dist_ok; total_n += (long)nans; max_len = len_class;
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
     * QUALI membri cadono — serve a curare il corpus, non a leggerlo, e
     * ingombrava il titolo.
     *
     * gen427 — MA I DUE NUMERI VANNO DETTI TUTT'E DUE, e la ragione e' stata
     * misurata, non temuta: chiudendo sette muri (i decimali, gli orari, il
     * denaro) la mole e' SCESA da 45 a 43. E' corretto e va capito — un muro
     * detto in modo suo E' un comportamento, e un muro che diventa una risposta
     * giusta spesso si UNISCE a una classe che esiste gia'. La mole misura la
     * varieta'; da sola premia anche il rumore.
     *
     * `solved` conta le classi DIMOSTRATE: nuove come coppia, e con tutti i
     * membri che incontrano l'attesa. Quel numero non si puo' alzare rompendo
     * niente — sale solo insegnando. E' il numero da massimizzare; la mole
     * resta accanto perche' dice quanto grande e' la misura che lo produce. */
    printf("tonnage %ld   solved %ld   max length %ld\n",
           total_n, total_ok, max_len);
    return 0;
}

/* Le stringhe della KB arrivano fra virgolette; qui si tolgono per stamparle.
 * Il brain ha il suo `kb_dequote`, ma e' statico nella sua TU. */
static const char *kb_dequote_pub(char *s) {
    size_t l = strlen(s);
    if (l >= 2 && s[0] == '"' && s[l - 1] == '"') { s[l - 1] = '\0'; return s + 1; }
    return s;
}

/* gen433 — L'ISPETTORE DEL SUPERVISORE.
 *
 * `/debug` non e' piu' solo un profiler. Finche' la KB non e' fertile
 * (docs/plans/autocrescita.md §0a) ci saranno turni in cui l'autocorrezione non
 * si innesca — o perche' manca qualcosa di piu' fondamentale, o perche' le
 * quattro condizioni non valgono tutte insieme — e in quei turni chi addestra
 * deve poter vedere DOVE piantare il seme che manca.
 *
 * Quello che l'ispettore dice sta tutto in `kb/core/debug.p0`: la nota, le
 * sonde, le condizioni, i semi. Il C stampa e non decide, quindi un'ispezione
 * nuova costa UNA RIGA DI KB — che e' il modo in cui questo strumento dovra'
 * crescere, perche' crescera' (F.: gli strumenti di debug vanno migliorati di
 * continuo, in relazione a necessita' che oggi non conosciamo). */
static void debug_lines(KB *kb, const char *pred, size_t argc, const char *tag) {
    char rows[32][KB_TERM_LEN];
    const char *q2[2] = { NULL, NULL };
    size_t n = kb_match(kb, pred, q2, argc, rows, 32);
    for (size_t i = 0; i < n; i++) {
        char b[KB_TERM_LEN]; snprintf(b, sizeof b, "%s", rows[i]);
        const char *first = kb_dequote_pub(b);
        if (argc == 1) { fprintf(stderr, "%s%s\n", tag, first); continue; }
        const char *q3[3] = { rows[i], NULL, NULL };
        char snd[1][KB_TERM_LEN];
        if (kb_match(kb, pred, q3, argc, snd, 1) == 1) {
            char c[KB_TERM_LEN]; snprintf(c, sizeof c, "%s", snd[0]);
            fprintf(stderr, "%s%s\n", tag, kb_dequote_pub(c));
        }
    }
}

/* gen459 — `/debug <predicato>`: QUANTE CLAUSOLE LO DEFINISCONO, E QUANTE
 * PRODUCONO DAVVERO QUALCOSA.
 *
 * Nasce da un difetto che mi e' costato dieci turni di congetture, e la lezione
 * non e' il difetto: e' che lo strumento non lo mostrava. `extract_frame/2` e'
 * definito da fatti E da regole; aggiungendone una seconda — «un verbo di
 * relazione puo' essere preceduto da una copula» — il comportamento non
 * cambiava, e non c'era modo di vedere perche'. Provando a mano si scopriva
 * che, invertendo l'ordine delle due regole, funzionava l'una O l'altra ma mai
 * entrambe: di un predicato con piu' regole ne rende solo la prima.
 *
 * Quel confronto — «clausole che lo definiscono» contro «binding che ne
 * escono» — e' l'informazione che mancava, e ora si legge in un comando. Se le
 * regole sono due e i binding raccontano una regola sola, il difetto e' li' e
 * si vede senza inventarsi esperimenti.
 *
 * Regola di crescita di questo strumento (F.): va migliorato ogni volta che un
 * problema e' stato scoperto in un altro modo. Se per capire qualcosa ho dovuto
 * fare un esperimento a mano, quell'esperimento appartiene a `/debug`. */
static void debug_predicate(KB *kb, const char *pred) {
    long amax = 4;
    {
        const char *cq[1] = { NULL };
        char cv[1][KB_TERM_LEN];
        if (kb_match(kb, "debug_pred_arity_max", cq, 1, cv, 1) > 0) {
            char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", cv[0]);
            long v = strtol(kb_dequote_pub(cb), NULL, 10);
            if (v > 0 && v <= 8) amax = v;
        }
    }
    fprintf(stderr, "\n  PREDICATO  %s\n", pred);
    if (!kb_knows_pred(kb, pred)) {
        fprintf(stderr, "  (nessuna clausola: ne' fatti ne' regole)\n\n");
        return;
    }
    fprintf(stderr, "  fatti ground            %zu\n", kb_pred_fact_count(kb, pred));
    for (long a = 1; a <= amax; a++) {
        size_t nr = kb_rules_for_head(kb, pred, (size_t)a);
        const char *q[8] = {0};
        enum { DBG_ROWS = 512 };
        static char rows[DBG_ROWS][KB_TERM_LEN];
        size_t nb = kb_match(kb, pred, q, (size_t)a, rows, DBG_ROWS);
        if (!nr && !nb) continue;
        fprintf(stderr, "  arita' %ld: regole %zu, binding resi %zu", a, nr, nb);
        if (nb == DBG_ROWS)
            fprintf(stderr, "   <-- SATURO: il tetto di questa ispezione e' %d,"
                            " il vero numero puo' essere piu' alto", DBG_ROWS);
        if (nr > 1 && nb <= 1)
            fprintf(stderr, "   <-- %zu regole, %zu binding: le clausole oltre la"
                            " prima non stanno rendendo niente", nr, nb);
        fprintf(stderr, "\n");
        for (size_t i = 0; i < nb && i < 6; i++) {
            char bb[KB_TERM_LEN]; snprintf(bb, sizeof bb, "%s", rows[i]);
            fprintf(stderr, "      %s\n", kb_dequote_pub(bb));
        }
        if (nb > 6) fprintf(stderr, "      … e altri %zu\n", nb - 6);
    }
    fprintf(stderr, "\n");
}

static void debug_inspect(Brain *brain, const char *last_line) {
    KB *kb = brain_kb(brain);
    fprintf(stderr, "\n");
    /* (1) la nota, che si legge PRIMA di qualunque numero */
    {
        char idx[32][KB_TERM_LEN];
        const char *q[2] = { NULL, NULL };
        size_t n = kb_match(kb, "debug_charter_line", q, 2, idx, 32);
        for (long k = 1; k <= (long)n; k++) {
            char key[16]; snprintf(key, sizeof key, "%ld", k);
            const char *q2[2] = { key, NULL };
            char row[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_charter_line", q2, 2, row, 1) != 1) continue;
            char b[KB_TERM_LEN]; snprintf(b, sizeof b, "%s", row[0]);
            fprintf(stderr, "  %s\n", kb_dequote_pub(b));
        }
    }
    if (!last_line || !*last_line) {
        fprintf(stderr, "\n  (nessun turno da ispezionare: dimmi qualcosa e richiama /debug)\n");
        return;
    }
    /* (2) l'anatomia del turno */
    fprintf(stderr, "\n  TURNO   %s\n", last_line);
    fprintf(stderr, "  modulo  %s\n", brain_last_module(brain));
    fprintf(stderr, "  firma   %08lx  (%zu predicati distinti)\n",
            kb_footprint(kb) & 0xfffffffful, kb_footprint_width(kb));
    {
        /* Quanti nomi mostrare e' un fatto: un turno ne tocca anche cento e
         * l'elenco intero smette di essere leggibile. */
        long cap = 24;
        {
            const char *cq[1] = { NULL };
            char cv[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_road_max", cq, 1, cv, 1) > 0) {
                char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", cv[0]);
                long v = strtol(kb_dequote_pub(cb), NULL, 10);
                if (v > 0) cap = v;
            }
        }
        size_t shown = 0, total = 0;
        for (size_t i = 0; i < kb_footprint_width(kb); i++)
            if (kb_footprint_pred(kb, i)) total++;
        for (size_t i = 0; i < kb_footprint_width(kb) && (long)shown < cap; i++) {
            const char *p = kb_footprint_pred(kb, i);
            if (!p) continue;
            if (shown == 0) fprintf(stderr, "  strada  ");
            else if (shown % 6 == 0) fprintf(stderr, ",\n          ");
            else fprintf(stderr, ", ");
            fprintf(stderr, "%s", p);
            shown++;
        }
        if (shown) {
            if ((long)total > cap)
                fprintf(stderr, " … e altri %zu\n", total - shown);
            else fprintf(stderr, "\n");
        } else {
            fprintf(stderr, "  strada  (i nomi si raccolgono solo col debug gia' acceso: riponi il turno)\n");
        }
    }
    /* (3) le sonde dichiarate in KB */
    {
        char canon[512];
        brain_canonical(brain, last_line, canon, sizeof canon);
        char quoted[KB_TERM_LEN];
        snprintf(quoted, sizeof quoted, "\"%s\"", canon);
        char ord[32][KB_TERM_LEN];
        const char *q[4] = { NULL, NULL, NULL, NULL };
        size_t n = kb_match(kb, "debug_probe", q, 4, ord, 32);
        fprintf(stderr, "\n  SONDE\n");
        for (size_t i = 0; i < n; i++) {
            const char *q2[4] = { ord[i], NULL, NULL, NULL };
            char pred[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_probe", q2, 4, pred, 1) != 1) continue;
            const char *q3[4] = { ord[i], pred[0], NULL, NULL };
            char key[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_probe", q3, 4, key, 1) != 1) continue;
            const char *q4[4] = { ord[i], pred[0], key[0], NULL };
            char lab[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_probe", q4, 4, lab, 1) != 1) continue;
            char pb[KB_TERM_LEN]; snprintf(pb, sizeof pb, "%s", pred[0]);
            char lb[KB_TERM_LEN]; snprintf(lb, sizeof lb, "%s", lab[0]);
            const char *pname = kb_dequote_pub(pb), *label = kb_dequote_pub(lb);
            char kb2[KB_TERM_LEN]; snprintf(kb2, sizeof kb2, "%s", key[0]);
            const char *keyname = kb_dequote_pub(kb2);
            int by_turn = strcmp(keyname, "turn") == 0;
            /* gen434: chiave `now` = i fatti di QUESTO turno, indicizzati su
             * `current_turn` invece che sul testo canonicalizzato. */
            int by_now = strcmp(keyname, "now") == 0;
            char found[16][KB_TERM_LEN];
            size_t m = 0;
            if (by_now) {
                const char *nq[2] = { "current_turn", NULL };
                m = kb_match(kb, pname, nq, 2, found, 16);
            } else if (by_turn) {
                const char *tq[2] = { quoted, NULL };
                m = kb_match(kb, pname, tq, 2, found, 16);
                if (m == 0 && kb_query(kb, pname, (const char *[]){ quoted }, 1))
                    { snprintf(found[0], KB_TERM_LEN, "si'"); m = 1; }
            } else {
                const char *aq[1] = { NULL };
                m = kb_match(kb, pname, aq, 1, found, 16);
            }
            fprintf(stderr, "    %-22s %s", pname, label);
            if (m == 0) { fprintf(stderr, " — niente\n"); continue; }
            fprintf(stderr, " —");
            for (size_t k = 0; k < m && k < 6; k++) {
                char fb[KB_TERM_LEN]; snprintf(fb, sizeof fb, "%s", found[k]);
                fprintf(stderr, " %s", kb_dequote_pub(fb));
            }
            fprintf(stderr, "%s\n", m > 6 ? " …" : "");
        }
    }
    /* (4) le quattro condizioni, con il segno di quelle che si possono
     * DECIDERE guardando lo stato — un elenco senza segni e' un promemoria,
     * non una diagnosi. */
    fprintf(stderr, "\n  PERCHE' L'AUTOCORREZIONE POTREBBE NON ESSERSI INNESCATA\n");
    {
        char canon[512];
        brain_canonical(brain, last_line, canon, sizeof canon);
        char quoted[KB_TERM_LEN];
        snprintf(quoted, sizeof quoted, "\"%s\"", canon);
        const char *gq[1] = { quoted };
        int c1 = kb_query(kb, "machinery_gap", gq, 1);
        char pg[4][KB_TERM_LEN];
        const char *pq[1] = { NULL };
        int c2 = kb_match(kb, "pending_gap", pq, 1, pg, 4) == 0;
        const char *sq[1] = { "off" };
        int c4 = !kb_query(kb, "self_correct_on_wall", sq, 1);
        int mark[5] = { 0, c1, c2, 1, c4 };
        char idx[16][KB_TERM_LEN];
        const char *q[2] = { NULL, NULL };
        size_t n = kb_match(kb, "debug_condition", q, 2, idx, 16);
        for (long k = 1; k <= (long)n && k <= 4; k++) {
            char key[8]; snprintf(key, sizeof key, "%ld", k);
            const char *q2[2] = { key, NULL };
            char row[1][KB_TERM_LEN];
            if (kb_match(kb, "debug_condition", q2, 2, row, 1) != 1) continue;
            char b[KB_TERM_LEN]; snprintf(b, sizeof b, "%s", row[0]);
            fprintf(stderr, "    %s %s\n", mark[k] ? "OK  " : "NO  ",
                    kb_dequote_pub(b));
        }
        /* IL CASO SILENZIOSO, che e' il piu' importante da nominare: nessuna
         * lacuna registrata NON vuol dire che il turno sia andato bene. */
        if (!c1)
            debug_lines(kb, "debug_note_silent", 2, "    ! ");
    }
    /* (5) i semi, in ordine di leva */
    fprintf(stderr, "\n  SEMI CHE PUOI PIANTARE, dal piu' generale al piu' stretto\n");
    {
        char ord[16][KB_TERM_LEN];
        const char *q[3] = { NULL, NULL, NULL };
        size_t n = kb_match(kb, "supervisor_seed", q, 3, ord, 16);
        for (size_t i = 0; i < n; i++) {
            const char *q2[3] = { ord[i], NULL, NULL };
            char kind[1][KB_TERM_LEN];
            if (kb_match(kb, "supervisor_seed", q2, 3, kind, 1) != 1) continue;
            const char *q3[3] = { ord[i], kind[0], NULL };
            char text[1][KB_TERM_LEN];
            if (kb_match(kb, "supervisor_seed", q3, 3, text, 1) != 1) continue;
            char kbf[KB_TERM_LEN]; snprintf(kbf, sizeof kbf, "%s", kind[0]);
            char tbf[KB_TERM_LEN]; snprintf(tbf, sizeof tbf, "%s", text[0]);
            fprintf(stderr, "    %-11s %s\n", kb_dequote_pub(kbf), kb_dequote_pub(tbf));
        }
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

/* ── gen497 — IL PENSIERO SI VEDE MENTRE ACCADE ────────────────────────────
 *
 * F., 2026-09-04: «questi step di thinking si devono vedere in grigio nella ui
 * di parrot0 […] si deve vedere il processo completo, metaprompt piu' output,
 * cosi' li possiamo debuggare».
 *
 * Percio' si stampano ENTRAMBI, e non solo il risultato: un passo di cui si
 * vede solo l'uscita non e' ispezionabile — non si puo' sapere che cosa gli sia
 * stato chiesto, e quindi nemmeno se la risposta c'entri. Il meta-prompt e' la
 * meta' che permette di dare la colpa a chi ce l'ha.
 *
 * Il grigio e' meccanica di terminale, non testo (stesso ragionamento del
 * banner): senza tty, con NO_COLOR o su TERM=dumb l'uscita resta leggibile e i
 * bench non vedono sequenze. Va su STDERR perche' il pensiero non e' la
 * risposta: chi mette parrot0 in una pipe deve ricevere solo la risposta. */
static void think_step_print(void *ud, int index, const char *prompt,
                             const char *answer) {
    (void)ud;
    fprintf(stderr, "%s  · pensiero %d ─ %s%s\n", C("38;5;244"), index,
            prompt ? prompt : "", C_OFF);
    fprintf(stderr, "%s    ↳ %s%s\n", C("38;5;244"),
            answer ? answer : "", C_OFF);
    fflush(stderr);
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
    int dream_depth = 0, dream_nodes = 0, dream_fetch = 1, dream_persist = 0;
    int dream_debug = 0;
    const char *host = "127.0.0.1";
    const char *sockpath = TEST_ENGINE_SOCK_DEFAULT;
    const char *send_file = NULL;
    const char *bench_file = NULL;
    const char *bench_health_file = NULL;
    const char *bench_stats = BENCH_STATS_DEFAULT;
    const char *profile = NULL;
    const char *measure_dir = NULL;   /* gen421: la stazza */
    const char *audit_paths[8]; size_t n_audit = 0;  /* gen435: la conoscenza muta */
    int footprint_mode = 0;           /* gen422: la firma dell'inferenza */
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
        else if (strcmp(argv[i], "--footprint") == 0) { footprint_mode = 1; }
        else if (strcmp(argv[i], "--coverage") == 0 && i + 1 < argc) {
            if (n_audit < 8) audit_paths[n_audit++] = argv[++i]; else i++;
        }
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
        else if (strcmp(argv[i], "--debug") == 0) dream_debug = 1;
        else if (strcmp(argv[i], "--fetch") == 0) dream_fetch = 1; /* obsolete alias */
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
    pid_t boot_pulse = activity_pulse_start();
    Brain *brain = setup_brain(&sess);
    activity_pulse_stop(boot_pulse);
    if (!brain) {
        fprintf(stderr, "parrot0: out of memory\n");
        return 1;
    }

    /* gen382: il sogno gira sul cervello COMPLETO (e' esplorazione, non un test
     * ermetico), stampa il suo trace su stdout ed esce. */
    if (footprint_mode) { brain_destroy(brain); return footprint_run(); }
    if (n_audit) {
        return coverage_run(audit_paths, n_audit);
    }
    if (measure_dir) {
        brain_destroy(brain);            /* la misura crea il proprio, uno per prompt */
        return measure_run(measure_dir);
    }
    if (dream_topic) {   /* "" = sogna le lacune aperte, vedi dream.c */
        DreamOpts dopts = { dream_depth, dream_nodes, dream_fetch, dream_persist,
                            dream_debug, stdout };
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
      /* Una riga sola. Le quattro di prima dicevano le stesse quattro cose e
       * spingevano fuori schermo l'inizio della conversazione ogni volta. */
      int tools_on = brain_policy_on(brain, "tools");
      int net_on   = brain_policy_on(brain, "network");
      /* Tre righe piu' una vuota: l'identita', lo stato, il suggerimento, e poi
       * il respiro prima del primo prompt. Il testo non cambia — cambia dove
       * va a capo, e sparisce il trattino che separava identita' e stato
       * perche' adesso li separa la riga. */
      fprintf(stderr,
              "%s%sparrot0%s %s[%s]%s\n"
              "mode: %s%s%s "
              "%s(%stools %s%s%s%s,%s network %s%s%s%s)%s %s-%s "
              "%s%zu%s facts, %s%zu%s rules %s(%.2f%%)%s\n"
              "%ssay something ('/quit' to exit, '/save' to persist, "
              "'/restore' to reload the KB from disk)%s\n"
              "\n",
              C("1"), C("38;5;79"), C_OFF,
              C("38;5;245"), brain_version(), C_OFF,
              C("38;5;215"), mode, C_OFF,
              C("38;5;240"), C_OFF,
              tools_on ? C("38;5;114") : C("38;5;244"),
              tools_on ? "on" : "off", C_OFF, C("38;5;240"), C_OFF,
              net_on ? C("38;5;114") : C("38;5;244"),
              net_on ? "on" : "off", C_OFF, C("38;5;240"), C_OFF,
              C("38;5;240"), C_OFF,
              C("38;5;111"), facts, C_OFF,
              C("38;5;111"), rules, C_OFF,
              C("38;5;244"), rule_fact_ratio, C_OFF,
              C("38;5;244"), C_OFF); }

    char line[LINE_MAX_LEN];
    char resp[RESP_MAX_LEN];
    char last_line[1024] = "";   /* gen433: l'ispettore guarda l'ultimo turno */

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
        /* L'editor con storia ridisegna il prompt da se' a ogni tasto: se lo
         * stampasse anche il ciclo, ne resterebbe uno di troppo sulla riga. */
        if (!(interactive && !multiline)) {
            fprintf(stderr, "%s", prompt_str());
            fflush(stderr);
        }

        if (multiline) {
            /* gen197: multi-line capable reader (Shift+Enter / paste / '\'). */
            if (!read_turn_tty(line, sizeof line)) break;   /* EOF */
        } else if (interactive) {
            /* Editor di riga con storia (frecce su/giu'). Se il terminale non
             * si lascia mettere in raw mode ritorna -1 e si ricade sul lettore
             * canonico, che e' sempre stato il comportamento di base. */
            int r = read_line_history(line, sizeof line);
            if (r == 0) break;                             /* EOF / Ctrl-D */
            if (r < 0) {
                if (!fgets(line, sizeof line, stdin)) break;
                chomp(line);
            }
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
        if (strncmp(line, "/debug ", 7) == 0 &&
            strcmp(line, "/debug off") != 0) {
            debug_predicate(brain_kb(brain), line + 7);
            continue;
        }
        if (strcmp(line, "/debug") == 0 || strcmp(line, "/debug off") == 0) {
            KB *kb = brain_kb(brain);
            if (strcmp(line, "/debug off") == 0) {
                kb_profile_set(kb, 0);
                fprintf(stderr, "parrot0: debug OFF\n");
                continue;
            }
            debug_inspect(brain, last_line);
            if (!kb_profile_on(kb)) {
                kb_profile_set(kb, 1);
                fprintf(stderr, "  (profilo acceso: i turni successivi riportano anche"
                                " tempi, passi e la strada per nome — /debug off per"
                                " spegnerlo)\n\n");
            }
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
        snprintf(last_line, sizeof last_line, "%s", line);
        /* gen497: con il thinking acceso il turno non finisce alla prima
         * risposta — rientra nella pipeline finche' lo schema non si chiude, e
         * ogni passo si vede. Spento, e' esattamente la chiamata di prima. */
        pid_t turn_pulse = activity_pulse_start();
        if (brain_policy_on(brain, "thinking"))
            brain_think(brain, line, resp, sizeof resp, think_step_print, NULL);
        else
            brain_respond(brain, line, resp, sizeof resp);
        activity_pulse_stop(turn_pulse);
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
