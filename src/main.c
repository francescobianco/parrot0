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
#include "serve.h"
#include "mcp.h"
#include "testeng.h"

#include <stdio.h>
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
        const char *sess = getenv("PARROT0_SESSION");
        if (!sess) sess = "kb/core/session.p0";
        *out_sess = sess;
    }
    return brain;
}

int main(int argc, char **argv) {
    /* gen221: `parrot0 --daemon [--port N] [--host H]` serves the
     * OpenAI-compatible HTTP API directly (replacing scripts/pi_server.py). */
    int daemon_mode = 0, mcp_mode = 0, test_mode = 0, port = 9902;
    int send_mode = 0, report_mode = 0;
    const char *host = "127.0.0.1";
    const char *sockpath = TEST_ENGINE_SOCK_DEFAULT;
    const char *send_file = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--daemon") == 0) daemon_mode = 1;
        else if (strcmp(argv[i], "--mcp-engine") == 0) mcp_mode = 1;
        else if (strcmp(argv[i], "--test-engine") == 0) test_mode = 1;
        else if (strcmp(argv[i], "--test-send") == 0) {
            send_mode = 1;
            if (i + 1 < argc && strncmp(argv[i + 1], "--", 2) != 0) send_file = argv[++i];
        }
        else if (strcmp(argv[i], "--test-report") == 0) report_mode = 1;
        else if (strcmp(argv[i], "--sock") == 0 && i + 1 < argc) sockpath = argv[++i];
        else if (strncmp(argv[i], "--sock=", 7) == 0) sockpath = argv[i] + 7;
        else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) port = atoi(argv[++i]);
        else if (strncmp(argv[i], "--port=", 7) == 0) port = atoi(argv[i] + 7);
        else if (strcmp(argv[i], "--host") == 0 && i + 1 < argc) host = argv[++i];
        else if (strncmp(argv[i], "--host=", 7) == 0) host = argv[i] + 7;
        else {
            fprintf(stderr, "parrot0: unknown argument '%s'\n"
                    "usage: parrot0 [--daemon [--port N] [--host H]] [--mcp-engine]\n"
                    "               [--test-engine [--sock PATH]]\n"
                    "               [--test-send FILE [--sock PATH]] [--test-report [--sock PATH]]\n",
                    argv[i]);
            return 2;
        }
    }

    /* gen345: the test-engine CLIENTS run FIRST and load NOTHING — no brain, no
     * KB — so every `make test` line is a cheap socket relay to the one daemon.
     * --test-send FILE streams a `.p0t` file; --test-report asks for the summary
     * and stops the daemon, propagating its pass/fail as the exit code. */
    if (send_mode) {
        FILE *in = send_file ? fopen(send_file, "rb") : stdin;
        if (!in) { fprintf(stderr, "parrot0: cannot open '%s'\n", send_file); return 2; }
        int rc = test_engine_send(sockpath, in, send_file ? send_file : "stdin");
        if (in != stdin) fclose(in);
        return rc;
    }
    if (report_mode) return test_engine_send_str(sockpath, "!shutdown\n", NULL);

    /* --test-engine is the DAEMON: one brain, listening on the Unix socket. */
    if (test_mode) {
        Brain *brain = setup_brain(NULL);
        if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
        int rc = test_engine_serve(brain, sockpath);
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

    /* gen331 (TODO.md P1/09): the banner reports the EFFECTIVE policy, read from
     * the same KB facts the modules obey. Before this it said nothing about the
     * mode, and `make chat` — which had tools OFF and the network ON — looked
     * identical to the agent. A user cannot calibrate what they cannot see. */
    { char mode[32]; brain_mode(brain, mode, sizeof mode);
      fprintf(stderr, "parrot0 [%s] - mode: %s (tools %s, network %s)\n"
                      "say something ('/quit' to exit, '/save' to persist, "
                      "'/restore' to reload the KB from disk)\n",
              brain_version(), mode,
              brain_policy_on(brain, "tools")   ? "on" : "off",
              brain_policy_on(brain, "network") ? "on" : "off"); }

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
        if (strcmp(line, "/save") == 0) {
            int n = brain_save_session(brain, sess);
            if (n >= 0) fprintf(stderr, "parrot0: saved %d clause(s) to %s\n",
                                n, sess);
            else        fprintf(stderr, "parrot0: could not save to %s\n", sess);
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
        if (line[0] == '\0') {
            continue; /* ignore empty turns */
        }

        brain_respond(brain, line, resp, sizeof resp);
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