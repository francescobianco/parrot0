/* gen115 (L15): the first deliberate mid-turn TOOL CALL — the honest seam
 * between "reason" and "act". Until now every module answered by lookup,
 * inference, or inline computation. This one ACTS: it recognizes a question it
 * cannot resolve by knowing (how many words are in this text?), compiles it to a
 * real command, and INVOKES a deterministic oracle — the pure POSIX pipeline
 * simulator (no subprocess, no network) — then folds the computed result back
 * into the reply, naming the exact command it ran so the call is observable, not
 * a stubbed constant. Held-out text transfers because the count is produced by
 * the oracle, not stored. The tool call is also recorded as the proof trace.
 * This is the structural precondition for agency (rung 19): a brain that can
 * reach outside its own deduction to a tool and bring the answer back. */
static const char *tool_surface_find(Brain *b, const char *text,
                                      const char *kind, size_t *span) {
    if (span) *span = 0;
    if (!b || !b->kb || !text || !kind) return NULL;
    const char *q[] = { kind, NULL };
    char surfaces[32][KB_TERM_LEN];
    size_t n = kb_match(b->kb, "tool_surface", q, 2, surfaces, 32);
    const char *best = NULL; size_t best_len = 0;
    for (size_t i = 0; i < n; i++) {
        const char *surface = kb_dequote(surfaces[i]);
        const char *hit = surface ? strstr(text, surface) : NULL;
        size_t len = surface ? strlen(surface) : 0;
        if (hit && (!best || hit < best || (hit == best && len > best_len))) {
            best = hit; best_len = len;
        }
    }
    if (span) *span = best_len;
    return best;
}

static int mod_tool(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Fire only on an explicit word-count request (EN + IT).
     * gen254: letter counts ride the same seam — "how many letters are in the
     * word strawberry?" is a deterministic count over the named token, not
     * knowledge; the count is computed, never stored. */
    /* gen254: alphabet-coverage check — "what does 'The quick brown fox...'
     * contain that most sentences do not?" is COMPUTED over the quoted text
     * (26 distinct letters -> pangram), never a stored trivia answer. */
    if ((kb_cue_match(b, "60_agent_tools_chain29", low)) && strchr(low, '"')) {
        const char *q1 = strchr(low, '"');
        const char *q2 = q1 ? strchr(q1 + 1, '"') : NULL;
        if (q1 && q2 && q2 - q1 > 15) {
            int seen[26] = {0}; int distinct = 0;
            for (const char *c = q1 + 1; c < q2; c++)
                if (isalpha((unsigned char)*c) && !seen[*c - 'a']) {
                    seen[*c - 'a'] = 1; distinct++;
                }
            if (distinct == 26) {
                kb_term_say(b, "it_contains_every_letter_of_the_alphabet_it", NULL, 0, out, out_size);
                store_proof(b, "Counted 26 distinct letters in the quoted text.");
                return 1;
            }
        }
    }

    int want_letters = kb_cue_match(b, "60_agent_tools_chain48", low);
    int want_words = !want_letters &&
                     (kb_cue_match(b, "60_agent_tools_chain52", low));
    if (!want_words && !want_letters) return 0;
    if (want_letters) {
        const char *p = strchr(low, ':');
        if (p) p++;
        else { p = strstr(low, " in "); if (p) p += 4; }
        if (!p) return 0;
        while (*p && isspace((unsigned char)*p)) p++;
        if (lex_prefix_member(b, "60_agent_tools_lex57", p)) p += 9;
        else if (lex_prefix_member(b, "60_agent_tools_lex58", p)) p += 9;
        char word[128]; snprintf(word, sizeof word, "%s", p);
        size_t wl = strlen(word);
        while (wl > 0 && !isalpha((unsigned char)word[wl - 1])) word[--wl] = '\0';
        char *ws = word;
        while (*ws && !isalpha((unsigned char)*ws)) ws++;
        size_t nl = 0;
        for (const char *c = ws; *c; c++)
            if (isalpha((unsigned char)*c)) nl++;
        if (nl == 0) return 0;
        char msg[220];
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", nl);
  const KbResponseSlot _rs[] = { { "nl", _v0 }, { "ws", ws } };
          kb_term_say(b, "there_are_x_letters_in_x", _rs, 2, msg, sizeof msg); }
        put(msg, out, out_size);
        store_proof(b, "Counted the alphabetic characters of the named word.");
        return 1;
    }

    /* The text to count: prefer everything after a ':'; else after " in ". */
    const char *p = strchr(low, ':');
    if (p) p++;
    else { p = strstr(low, " in "); if (p) p += 4; }
    if (!p) return 0;
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return 0;

    char text[256];
    snprintf(text, sizeof text, "%s", p);
    size_t tl = strlen(text);
    while (tl > 0 && (text[tl-1]=='?' || text[tl-1]=='.' || text[tl-1]=='!' ||
                      isspace((unsigned char)text[tl-1])))
        text[--tl] = '\0';
    if (tl == 0) return 0;

    /* Compile the tool call and run the REAL deterministic oracle. */
    char cmd[320];
    snprintf(cmd, sizeof cmd, "echo %s | wc -w", text);
    char result[64];
    if (!simulate_pipeline(cmd, result, sizeof result)) {
        kb_term_say(b, "i_can_only_count_plain_words_for_now_letters", NULL, 0, out, out_size);
        return 1;
    }
    size_t resl = strlen(result);
    while (resl > 0 && (result[resl-1]=='\n' || result[resl-1]==' '))
        result[--resl] = '\0';

    char msg[640];
    { const KbResponseSlot _rs[] = { { "result", result }, { "cmd", cmd } };
      kb_term_say(b, "x_i_ran_the_tool_x", _rs, 2, msg, sizeof msg);
      put(msg, out, out_size); }

    char proof[640];
    snprintf(proof, sizeof proof, "tool call %s = %s", cmd, result);
    store_proof(b, proof);
    return 1;
}

/* gen205 (pi-agent): LOCAL coding-agent tools — the step from "an agent that can
 * compute" (gen115's in-process oracle) to "an agent that can operate a codebase".
 * See docs/use-on-pi-agent.md. A REMOTE LLM cannot read the disk; it must emit a
 * tool_call, surrender the turn, and wait for the runtime to feed the observation
 * back into its context — the two-step OpenAI tool protocol. parrot0 runs on the
 * machine, so it skips that whole dance and answers in the SAME turn.
 *
 * Crucially this module is a ROUTER INTO THE EXISTING SUBSTRATE, not a parallel
 * shell-out island. It separates two kinds of act:
 *   - PERCEPTION of the filesystem (list / find files) has no KB primitive, so it
 *     genuinely shells out (find), grounded by naming the command it ran.
 *   - UNDERSTANDING CODE goes through the code-IR engine and the SWE-bench
 *     localizers, never a blind grep: "read FILE" publishes source-qualified,
 *     revisioned observations from which the KB derives `code_function` and
 *     `code_calls`; "where is X" / "what calls X" call code_locate /
 *     code_find_callers — the exact primitives that drive `make swe-solve`
 *     (CODE-MASTERY §4) — and answer from a real parse, which a comment or a string
 *     literal cannot fake. A text grep is the FALLBACK, only for free-text patterns.
 * So the anti-impostor rule (PRINCIPLES.md) holds at the structural level: parrot0
 * reports a definition/caller only when the parser actually found it. Gated by
 * PARROT0_TOOLS=1 so the hermetic test harness never shells out; the pi wrapper sets
 * it. This is KB-first agency: the same KB and code engine the loop already grew,
 * reached from the agent's read-only verbs. */

/* case-insensitive prefix test (no <strings.h> in this translation unit). */
static int ci_prefix(const char *s, const char *prefix) {
    for (size_t i = 0; prefix[i]; i++)
        if (tolower((unsigned char)s[i]) != tolower((unsigned char)prefix[i]))
            return 0;
    return 1;
}

/* True if `tok` equals `lit` case-insensitively. */
static int ci_eq(const char *tok, const char *lit) {
    return ci_prefix(tok, lit) && tok[strlen(lit)] == '\0';
}

/* Collapse every run of whitespace/newlines in `src` to a single space and trim
 * the ends, into `dst` (truncated to cap-1). Keeps a multi-line command result on
 * the single line parrot0's protocol promises. */
static void collapse_ws(const char *src, char *dst, size_t cap) {
    size_t o = 0; int sp = 1;                 /* sp: suppress a leading space */
    for (size_t i = 0; src[i] && o + 1 < cap; i++) {
        if (isspace((unsigned char)src[i])) { if (!sp) { dst[o++] = ' '; sp = 1; } }
        else { dst[o++] = src[i]; sp = 0; }
    }
    while (o > 0 && dst[o - 1] == ' ') o--;
    dst[o] = '\0';
}

/* gen329 (TODO.md P0/05): a path token is plausible iff its characters could
 * belong to a path. That is ALL this predicate ever knew, and mistaking it for a
 * containment check is what let `read /etc/os-release` walk out of the repository:
 * "/etc/os-release" is made of perfectly innocent characters and points perfectly
 * outside the workspace. Containment is not a character class — it is a resolution
 * discipline, and it now lives in p0_open_in_root/p0_safe_rel (openat + O_NOFOLLOW
 * from a dirfd on the root, one component at a time). This function survives only
 * as what it always was: a cheap TOKENIZER filter that tells a path-shaped word
 * apart from prose, before the real gate ever sees it. It grants nothing. */
static int pathish_token(const char *t) {
    if (!t || !*t) return 0;
    for (const char *p = t; *p; p++) {
        char c = *p;
        if (!(isalnum((unsigned char)c) || c=='.' || c=='_' || c=='/' ||
              c=='-' || c=='*'))
            return 0;
    }
    return 1;
}

/* Strip trailing sentence punctuation from a token in place. */
static void rstrip_punct(char *t) {
    size_t n = strlen(t);
    while (n > 0 && (t[n-1]=='?'||t[n-1]=='.'||t[n-1]=='!'||t[n-1]==','||
                     t[n-1]==';'||t[n-1]==':')) t[--n] = '\0';
}

/* Map an English file-kind cue in `low` to a shell glob written into `glob`.
 * Returns 1 if a glob was found. Recognizes an explicit "*.ext" and ".ext" as
 * well as a handful of language words. */
static int detect_glob(Brain *b, const char *low, char *glob, size_t cap) {
    /* TODO(kb-first): le estensioni di file sono conoscenza sul mondo del
     * codice, non struttura del motore. Un linguaggio nuovo deve entrare come
     * fatto — `file_extension(rust, ".rs")` — insieme al resto di cio' che
     * parrot0 sa di quel linguaggio. */
    const char *star = strstr(low, "*.");
    if (star) {
        size_t i = 0; glob[i++] = '*'; const char *p = star + 1;   /* keep ".ext" */
        while (*p && i + 1 < cap && (isalnum((unsigned char)*p) || *p=='.')) glob[i++] = *p++;
        glob[i] = '\0';
        return 1;
    }
    if (b && b->kb) {
        char exts[64][KB_TERM_LEN];
        const char *eq[1] = { NULL };
        size_t ne = kb_match(b->kb, "file_extension", eq, 1, exts, 64);
        for (size_t e = 0; e < ne; e++) {
            const char *needle = kb_dequote(exts[e]);
            if (needle && strstr(low, needle)) {
                snprintf(glob, cap, "*%s", needle);
                return 1;
            }
        }
        char words[64][KB_TERM_LEN];
        const char *wq[2] = { NULL, NULL };
        size_t nw = kb_match(b->kb, "file_kind_word", wq, 2, words, 64);
        for (size_t i = 0; i < nw; i++) {
            char word[KB_TERM_LEN];
            snprintf(word, sizeof word, "%s", kb_dequote(words[i]));
            char pattern[1][KB_TERM_LEN];
            const char *pq[2] = { words[i], NULL };
            if (kb_match(b->kb, "file_kind_word", pq, 2, pattern, 1) == 1 &&
                strstr(low, word)) {
                snprintf(glob, cap, "%s", kb_dequote(pattern[0]));
                return 1;
            }
        }
    }
    return 0;
}

/* gen329 (TODO.md P0/04): AUTHORIZATION, now that the shell is gone.
 *
 * The old gate matched the PREFIX of a command string that was then handed whole
 * to popen(3) — so `git status; printf P0_CHAINED` passed the gate on its head and
 * ran its tail. The string is gone (p0_exec execve's an argv vector, in which a
 * ';' is an ordinary character), and what is left to decide is not "does this text
 * look safe" but "is parrot0 authorized to run this PROGRAM" — a question of
 * knowledge, answered by the KB:
 *
 *   tool_for(Purpose, Argv0)   parrot0 may execute Argv0 (and why)
 *   tool_subcmd(Argv0, Sub)    if any exist for Argv0, argv[1] must be one of them
 *
 * so `git` means status/diff/log and never `git push`, and authorizing a new tool
 * is a fact — teachable at runtime, never a recompile, and unable to widen into
 * "run anything". Returns 1 iff argv is authorized; on refusal `why` says which
 * clause was missing (the Gap, not a scolding). */
static int tool_authorized(Brain *b, char *const *argv, size_t argc,
                           char *why, size_t whysz) {
    if (why && whysz) why[0] = '\0';
    if (!b || !b->kb || argc == 0) return 0;

    const char *q[2] = {NULL, argv[0]};
    char purpose[4][KB_TERM_LEN];
    if (kb_match(b->kb, "tool_for", q, 2, purpose, 4) == 0) {
        if (why) { const KbResponseSlot _rs[] = { { "tool", argv[0] } };
                   kb_term_say(b, "no_contract_authorizing_x", _rs, 1, why, whysz); }
        return 0;
    }
    /* If the KB constrains this tool's subcommands, argv[1] must be one of them. */
    const char *qs[2] = {argv[0], NULL};
    char subs[16][KB_TERM_LEN];
    size_t ns = kb_match(b->kb, "tool_subcmd", qs, 2, subs, 16);
    if (ns > 0) {
        if (argc < 2) {
            if (why) { const KbResponseSlot _rs[] = { { "tool", argv[0] } };
                       kb_term_say(b, "x_needs_one_of_its_authorized_subcommands", _rs, 1, why, whysz); }
            return 0;
        }
        for (size_t i = 0; i < ns; i++)
            if (strcmp(subs[i], argv[1]) == 0) return 1;
        if (why) snprintf(why, whysz, "no tool_subcmd(%s, %s)", argv[0], argv[1]);
        return 0;
    }
    return 1;
}

/* Find the first token after one of the locator words ("in", "inside", ...) that
 * is a safe path, else the first bare safe path-ish token. Returns it or NULL. */
/* An article to skip after a preposition ("in THE current dir", "in QUESTA cartella"). */
static int dir_article(const char *s) {
    return ci_eq(s,"the")||ci_eq(s,"a")||ci_eq(s,"an")||ci_eq(s,"il")||ci_eq(s,"lo")||
           ci_eq(s,"la")||ci_eq(s,"l")||ci_eq(s,"i")||ci_eq(s,"gli")||ci_eq(s,"le");
}
/* gen223: words that DEICTICALLY mean "the current directory" (EN+IT) — "this/current
 * folder", "questa cartella", "here"/"qui". The bug they fix: "lista i file in questa
 * cartella" used to run `find questa` because "questa" looked path-ish. Now it -> ".". */
static int dir_here_word(const char *s) {
    return ci_eq(s,"here")||ci_eq(s,"qui")||ci_eq(s,"cwd")||ci_eq(s,".");
}
static int dir_deictic_det(const char *s) {
    return ci_eq(s,"this")||ci_eq(s,"that")||ci_eq(s,"current")||ci_eq(s,"present")||
           ci_eq(s,"questa")||ci_eq(s,"questo")||ci_eq(s,"quella")||ci_eq(s,"quello")||
           ci_eq(s,"corrente")||ci_eq(s,"attuale");
}
static int dir_noun(const char *s) {
    return ci_eq(s,"folder")||ci_eq(s,"directory")||ci_eq(s,"dir")||ci_eq(s,"cartella");
}

/* gen494 — QUALE PAROLA INTRODUCE UNO SLOT E' CONOSCENZA (`tool_slot_cue/2`).
 * Qui c'era una lista di otto parole inglesi nel C: mantra #2, nel modulo che
 * O1 chiama «il vocabolario dei tool dell'agente stesso». Ora il C chiede alla
 * classe, e l'italiano entra senza ricompilare. */
static int tool_slot_word(Brain *b, const char *slot, const char *word) {
    if (!b || !b->kb || !slot || !word || !*word) return 0;
    char rows[32][KB_TERM_LEN];
    const char *q[2] = { slot, NULL };
    size_t n = kb_match(b->kb, "tool_slot_cue", q, 2, rows, 32);
    for (size_t i = 0; i < n; i++) {
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rows[i]);
        const char *cue = kb_dequote(rb);
        if (cue && *cue && ci_eq(word, cue)) return 1;
    }
    return 0;
}

static const char *find_dir_kb(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i + 1 < nw; i++) {
        if (!tool_slot_word(b, "dir", w[i])) continue;
        size_t j = i + 1;
        while (j < nw && dir_article(w[j])) j++;       /* skip "the"/"il"/… */
        if (j >= nw) continue;
        rstrip_punct(w[j]);
        /* "in this folder" / "in questa cartella" / "here"/"qui" -> current directory. */
        if (dir_here_word(w[j]) || dir_deictic_det(w[j])) return ".";
        if (pathish_token(w[j]) && strchr(w[j], '/')) return w[j];
        /* gen494 — UNA CARTELLA DEV'ESSERE UNA CARTELLA.
         *
         * «in» introduce davvero una directory, ma compare anche in «in formato
         * markdown»: il ramo prendeva `formato` come cartella e il turno moriva
         * su `unsafe_path` — un rifiuto giusto per una domanda che nessuno aveva
         * fatto. Non e' un problema di vocabolario ma di VERIFICA: un candidato
         * che non esiste sul disco non e' una cartella, e chiederlo costa una
         * stat. Se non esiste si continua a cercare; in mancanza d'altro resta la
         * cartella corrente, che e' cio' che «listami i file» vuol dire. */
        if (pathish_token(w[j]) && !dir_noun(w[j])) {
            struct stat dst;
            if (stat(w[j], &dst) == 0 && S_ISDIR(dst.st_mode)) return w[j];
            continue;
        }
    }
    /* a bare token that contains a slash is very likely the directory. */
    for (size_t i = 0; i < nw; i++) {
        rstrip_punct(w[i]);
        if (strchr(w[i], '/') && pathish_token(w[i])) return w[i];
    }
    return NULL;
}

/* Find a file path token: one containing '/', else one ending in a known-looking
 * .ext (e.g. main.c) — used by the "read" intent. */
static const char *find_file(char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) { rstrip_punct(w[i]); if (strchr(w[i],'/') && pathish_token(w[i])) return w[i]; }
    for (size_t i = 0; i < nw; i++) {
        char *dot = strrchr(w[i], '.');
        if (dot && dot != w[i] && isalpha((unsigned char)dot[1]) && pathish_token(w[i]))
            return w[i];
    }
    return NULL;
}

/* gen329 (TODO.md P0/07): an act RETURNS AN OBSERVATION, and the answer is
 * RENDERED FROM IT.
 *
 * What stood here before took a command string, ran it through popen(3), threw
 * away the exit status — pclose's return value was not even looked at — and
 * printed whatever bytes came back under a cheerful human label. So at gen328
 * `run make bogus-target-xyz` answered "`make bogus-target-xyz`: make: *** No rule
 * to make target. Stop." in exactly the register it used for a successful build:
 * a failure, narrated as a result. That is the impostor shape PRINCIPLES.md
 * forbids, sitting in the one place where parrot0 touches the real world.
 *
 * Now every act goes through p0_exec and comes back as a P0Obs — argv, cwd, exit
 * code, terminating signal, stdout and stderr SEPARATELY, truncation, duration,
 * digest. The renderer below is a total function over the verdict: each verdict
 * gets its own sentence, and only P0_OK may be spoken as a result. A failure
 * cannot be dressed as an answer, because the sentence for a failure is a
 * different sentence. The Observation is also what the proof cites, so
 * "how do you know?" quotes the RUN — exit status and digest — not the claim.
 *
 * gen342: the command line is now SECOND-LEVEL information. A normal successful
 * answer says the observed content only; the exact argv/exit/digest remain in
 * last_tool_cmd + last_proof for "come lo sai?" / audit. */
/* ── gen494 — L'ESPANSORE DI TEMPLATE: il C non sa che cosa sia `find` ───────
 *
 * F.: «tutti questi comandi cablati nel C devono essere tolti». Prima l'argv era
 * un array letterale — `{"find", dir, "-maxdepth", "1", "-type", "f"}` — quindi
 * cambiare COME si elencano i file richiedeva una ricompilazione e nessuna
 * lezione poteva toccarlo.
 *
 * Ora il comando arriva da `tool_argv(Strumento, "find {dir} -type f")` e questa
 * funzione fa una cosa sola: sostituisce i segnaposto con i valori che il
 * chiamante ha gia' calcolato e spezza in argv. Non conosce nessun comando,
 * nessun flag, nessuna cartella. Fra piu' template dello stesso strumento vince
 * quello che ha PIU' segnaposto soddisfatti, cosi' «i file .c» sceglie da se' la
 * variante col filtro senza un `if`.
 *
 * Torna 0 se nessun template e' utilizzabile: allora il chiamante non agisce,
 * invece di agire con un comando inventato. */
typedef struct { const char *key; const char *val; } ToolSlot;

static int tool_argv_build(Brain *b, const char *tool,
                           const ToolSlot *slots, size_t nslots,
                           char *store, size_t store_sz,
                           char **argv, size_t argv_max) {
    if (!b || !b->kb || !tool) return 0;
    char rows[8][KB_TERM_LEN];
    const char *tq[2] = { tool, NULL };
    size_t n = kb_match(b->kb, "tool_argv", tq, 2, rows, 8);
    if (!n) return 0;

    const char *best = NULL; int best_score = -1;
    for (size_t i = 0; i < n; i++) {
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rows[i]);
        const char *tpl = kb_dequote(rb);
        int score = 0, usable = 1;
        for (size_t k = 0; k < nslots; k++) {
            char needle[64]; snprintf(needle, sizeof needle, "{%s}", slots[k].key);
            if (!strstr(tpl, needle)) continue;
            if (slots[k].val && *slots[k].val) score++;
            else usable = 0;                 /* il template chiede cio' che non c'e' */
        }
        if (usable && score > best_score) {
            best_score = score;
            static char keep[KB_TERM_LEN];
            snprintf(keep, sizeof keep, "%s", tpl);
            best = keep;
        }
    }
    if (!best) return 0;

    size_t o = 0;
    for (const char *p = best; *p && o + 1 < store_sz; ) {
        if (*p == '{') {
            const char *close = strchr(p, '}');
            if (close) {
                size_t klen = (size_t)(close - p - 1);
                int filled = 0;
                for (size_t k = 0; k < nslots && !filled; k++)
                    if (strlen(slots[k].key) == klen &&
                        !strncmp(slots[k].key, p + 1, klen)) {
                        o += (size_t)snprintf(store + o, store_sz - o, "%s",
                                              slots[k].val ? slots[k].val : "");
                        filled = 1;
                    }
                p = close + 1;
                if (filled) continue;
                continue;
            }
        }
        store[o++] = *p++;
        store[o] = '\0';
    }
    store[o] = '\0';

    size_t na = 0;
    char *tok = strtok(store, " ");
    while (tok && na + 1 < argv_max) { argv[na++] = tok; tok = strtok(NULL, " "); }
    argv[na] = NULL;
    return na > 0;
}

/* ── gen494 — IL RISULTATO DI UNO STRUMENTO PASSA DAL FORMATO CHIESTO ───────
 *
 * F.: «poi ti processa il risultato, e se gli chiedi la lista dei file in
 * formato markdown lui lo sa fare per inferenza con la conoscenza».
 *
 * Lo strato dei formati esisteva ma serviva solo i piani di analisi: l'uscita di
 * uno strumento tornava sempre nella stessa forma. Qui la stessa conoscenza la
 * governa. Il motore sa spezzare un elenco e mettere un prefisso davanti a ogni
 * elemento; **non sa che «-» sia markdown**, e non deve: quale vincolo il turno
 * esprime lo dice `format_constraint/2`, e con che cosa si apre un elemento lo
 * dice `format_line_prefix/2`. Un formato nuovo e' due fatti, zero C.
 *
 * Torna 0 se il turno non chiede nessun formato: allora l'uscita resta quella
 * di prima, e nessuno paga niente. */
static int tool_format_prefix(Brain *b, const char *turn,
                              char *prefix, size_t psz) {
    if (!b || !b->kb || !turn || !*turn) return 0;
    char constraint[KB_TERM_LEN], proof[KB_EVIDENCE_PROOF_LEN];
    int score = 0;
    if (kb_hypothesis_best(b->kb, "format_constraint", turn, NULL, 0,
                           constraint, sizeof constraint, &score,
                           proof, sizeof proof) != 1) return 0;
    char rows[1][KB_TERM_LEN];
    const char *pq[2] = { constraint, NULL };
    if (kb_match(b->kb, "format_line_prefix", pq, 2, rows, 1) != 1) return 0;
    char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rows[0]);
    snprintf(prefix, psz, "%s", kb_dequote(rb));
    return prefix[0] != '\0';
}

/* Riscrive un elenco piatto — «a b c» — come una riga per elemento, ciascuna
 * aperta dal prefisso dichiarato. Meccanica pura: non nomina nessun formato. */
static void tool_apply_prefix(const char *flat, const char *prefix,
                              char *out, size_t out_size) {
    char work[4000]; snprintf(work, sizeof work, "%s", flat);
    size_t o = 0; out[0] = '\0';
    for (char *tok = strtok(work, " "); tok; tok = strtok(NULL, " ")) {
        if (!*tok) continue;
        int w = snprintf(out + o, out_size - o, "%s%s%s",
                         o ? "\n" : "", prefix, tok);
        if (w < 0 || (size_t)w >= out_size - o) break;
        o += (size_t)w;
    }
}

/* gen494: il turno arriva fin qui perche' il FORMATO e' parte della richiesta,
 * non una decorazione dell'uscita (mantra #11). Chi non lo passa ha l'uscita di
 * prima: la variabile e' letta solo se c'e'. */
static const char *g_tool_turn = NULL;
/* gen503: il rientro di un passo-prosa e' a profondita' uno. Un piano che si
 * richiama non e' un piano, e' un ciclo. */
static int g_plan_reentry = 0;

/* ── gen503 — IL RISULTATO DI UN'AZIONE DIVENTA CONOSCENZA ──────────────────
 *
 * F.: «parrot0 costruisce il comando, lo esegue come tool, E POI TI PROCESSA IL
 * RISULTATO». Fino a qui il risultato era testo: si stampava e svaniva. Il
 * banco lo mostrava nel modo piu' costoso possibile — `run make` diceva gia'
 * «No rule to make target 'strjoin.c'», cioe' nominava esattamente il file che
 * il compito chiedeva di scrivere, e un turno dopo parrot0 non lo sapeva piu'.
 *
 * COME si legge l'uscita di uno strumento e' conoscenza: la FRASE che introduce
 * un valore sta in `tool_result_cue(Predicato, "frase")` ed e' insegnabile;
 * qui resta solo l'estrazione — prendi il token che segue, fra apici o nudo.
 * E' la stessa divisione di `tool_slot_cue` (gen494) applicata all'uscita
 * invece che all'ingresso.
 *
 * I fatti sono KB_REFLECTIVE: un'osservazione del mondo di adesso, che non
 * finisce nella KB curata e non sopravvive al processo. */
static void tool_result_read(Brain *b, const char *text) {
    if (!b || !b->kb || !text || !*text) return;
    char low[8192];
    size_t tl = strlen(text);
    if (tl >= sizeof low) tl = sizeof low - 1;
    for (size_t i = 0; i < tl; i++) low[i] = (char)tolower((unsigned char)text[i]);
    low[tl] = '\0';

    char (*preds)[KB_TERM_LEN] = NULL;
    size_t npred = 0;
    const char *pq[2] = { NULL, NULL };
    if (!kb_match_all(b->kb, "tool_result_cue", pq, 2, &preds, &npred)) return;

    int saved = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    for (size_t i = 0; i < npred; i++) {
        /* gen503: cio' che si rilegge si ritratta prima. Un verdetto vecchio
         * che sopravvive a un'osservazione nuova fa dire «devi produrre X»
         * dopo che X e' stato prodotto — mentire su se stessi e' il modo piu'
         * facile, e la regola sta QUI e non nel chiamante perche' vale per
         * ogni lettura, non per il build. */
        char pb0[KB_TERM_LEN]; snprintf(pb0, sizeof pb0, "%s", preds[i]);
        kb_retract_pred(b->kb, kb_dequote(pb0));
        char cues[16][KB_TERM_LEN];
        const char *cq[2] = { preds[i], NULL };
        size_t nc = kb_match(b->kb, "tool_result_cue", cq, 2, cues, 16);
        for (size_t c = 0; c < nc; c++) {
            char phrase[KB_TERM_LEN];
            snprintf(phrase, sizeof phrase, "%s", cues[c]);
            char *ph = phrase;
            size_t pl = strlen(ph);
            if (pl >= 2 && ph[0] == '"' && ph[pl-1] == '"') { ph[pl-1] = '\0'; ph++; }
            for (char *q = ph; *q; q++) *q = (char)tolower((unsigned char)*q);
            const char *hit = strstr(low, ph);
            if (!hit) continue;
            const char *v = text + (hit - low) + strlen(ph);
            while (*v && (isspace((unsigned char)*v) || *v == ':')) v++;
            char open = 0;
            if (*v == '\'' || *v == '"' || *v == '`') { open = *v; v++; }
            char val[KB_TERM_LEN];
            size_t n = 0;
            while (*v && n + 1 < sizeof val) {
                if (open ? (*v == open || (open == '`' && *v == '\'')) 
                         : (isspace((unsigned char)*v) || *v == ',' || *v == ';'))
                    break;
                val[n++] = *v++;
            }
            val[n] = '\0';
            if (!n) continue;
            const char *aa[1] = { val };
            kb_assert(b->kb, preds[i], aa, 1);
        }
    }
    kb_set_origin(b->kb, saved);
    free(preds);
}

static int piact_obs(Brain *b, char *const *argv, const char *label,
                     char *out, size_t out_size) {
    P0Obs obs;
    p0_exec(argv, ".", 20000, NULL, &obs);

    char flat[4000];
    collapse_ws(obs.out, flat, sizeof flat);
    char eflat[600];
    collapse_ws(obs.err, eflat, sizeof eflat);
    tool_result_read(b, flat);
    tool_result_read(b, eflat);

    char msg[5200];
    char lang[8]; current_lang(b, lang, sizeof lang);
    int it = lex_class_member(b, "entity_pronoun", lang);
    switch (obs.verdict) {
    case P0_OK:
        if (flat[0] == '\0') {
            const KbResponseSlot slots[] = {{ "label", label },
                                             { "result", it ? "nessun risultato" : "nothing" }};
            if (!kb_response_slots(b, "tool_result_empty", slots, 2,
                                   msg, sizeof msg))
                snprintf(msg, sizeof msg, "%s: %s.", label,
                         it ? "nessun risultato" : "nothing");
        } else {
            char result[4200];
            /* gen494 — il formato chiesto governa anche l'uscita di uno
             * strumento: il motore spezza e prefissa, la KB dice con che cosa. */
            char prefix[32];
            if (g_tool_turn &&
                tool_format_prefix(b, g_tool_turn, prefix, sizeof prefix)) {
                char shaped[4200];
                tool_apply_prefix(flat, prefix, shaped, sizeof shaped);
                snprintf(result, sizeof result, "%s%s", shaped,
                         obs.out_truncated ? " …[truncated]" : "");
            } else {
                snprintf(result, sizeof result, "%s%s", flat,
                         obs.out_truncated ? " …[truncated]" : "");
            }
            const KbResponseSlot slots[] = {{ "label", label }, { "result", result }};
            if (!kb_response_slots(b, "tool_result", slots, 2, msg, sizeof msg))
                snprintf(msg, sizeof msg, "%s: %s", label, result);
        }
        break;
    case P0_EXIT_NONZERO:
        snprintf(msg, sizeof msg, "`%s` FAILED (exit %d).%s%s", obs.cmd, obs.exit_code,
                 eflat[0] ? " " : "", eflat[0] ? eflat : (flat[0] ? flat : ""));
        break;
    case P0_TIMEOUT:
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", obs.cmd);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%ld", obs.duration_ms);
  const KbResponseSlot _rs[] = { { "cmd", _v0 }, { "duration_ms", _v1 } };
          kb_term_say(b, "x_timed_out_after_x_ms_i_killed_it_i_do_not", _rs, 2, msg, sizeof msg); }
        break;
    case P0_SIGNALED:
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", obs.cmd);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%d", obs.term_signal);
  const KbResponseSlot _rs[] = { { "cmd", _v0 }, { "term_signal", _v1 } };
          kb_term_say(b, "x_was_killed_by_signal_x_it_did_not_complete", _rs, 2, msg, sizeof msg); }
        break;
    case P0_SPAWN_FAILED:
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", obs.cmd);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%s", obs.detail);
  const KbResponseSlot _rs[] = { { "cmd", _v0 }, { "detail", _v1 } };
          kb_term_say(b, "x_never_started_x", _rs, 2, msg, sizeof msg); }
        break;
    case P0_UNSAFE_PATH:
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", p0_root());
  const KbResponseSlot _rs[] = { { "p0_root", _v0 } };
          kb_term_say(b, "unsafe_path_that_is_outside_my_workspace_x_i", _rs, 1, msg, sizeof msg); }
        break;
    default:
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", obs.cmd);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%s", obs.detail[0] ? obs.detail : p0_verdict_name(obs.verdict));
  const KbResponseSlot _rs[] = { { "cmd", _v0 }, { "verdict", _v1 } };
          kb_term_say(b, "i_did_not_run_x_x", _rs, 2, msg, sizeof msg); }
        break;
    }
    put(msg, out, out_size);

    if (b) {
        snprintf(b->last_tool_cmd, sizeof b->last_tool_cmd, "%s", obs.cmd);
        b->has_last_tool_cmd = 1;
        char proof[512];
        p0_obs_proof(&obs, proof, sizeof proof);
        store_proof(b, proof);
    }
    return 1;
}

/* gen329 (TODO.md P0/05): resolve a user-named directory INSIDE the workspace,
 * through the real gate (openat/O_NOFOLLOW from the root dirfd), and hand back the
 * relative form that the resolution actually produced — never the user's spelling.
 * If it escapes, parrot0 declines by NAME (`unsafe_path`) and claims the turn: a
 * precise decline is green, a quiet look outside the repository is not. */
static int piact_dir(Brain *b, const char *dir, char *rel, size_t cap,
                     char *out, size_t out_size, int *claimed) {
    *claimed = 0;
    if (!dir || !pathish_token(dir)) { snprintf(rel, cap, "."); return 1; }
    if (p0_safe_rel(dir, 1, rel, cap) != P0_OK) {
        char m[600];
        { 
          char _v1[48]; snprintf(_v1, sizeof _v1, "%s", p0_root());
  const KbResponseSlot _rs[] = { { "dir", dir }, { "p0_root", _v1 } };
          kb_term_say(b, "unsafe_path_x_is_not_a_directory_inside_my_w", _rs, 2, m, sizeof m); }
        put(m, out, out_size);
        *claimed = 1;
        return 0;
    }
    return 1;
}

/* ══ gen495 — UN PIANO SU STRUMENTI ══════════════════════════════════════════
 *
 * F.: «analizza tutti i file sorgenti che trovi» — e parrot0 murava. Non era una
 * superficie mancante: era una richiesta che **nessuna azione sola soddisfa**.
 * Trova → leggi ciascuno → riferisci. Finche' un turno vale al massimo
 * un'azione, parrot0 risponde a domande; non agisce.
 *
 * La forma e' quella di `p0_compensate` (gen442), ed e' deliberato: la KB dice
 * QUALI passi esistono e in che ordine (`turn_plan/2`, `plan_step/3`), il motore
 * possiede soltanto le PRIMITIVE. Qui sono due:
 *
 *     list_sources   elenca i file che hanno un'estensione dichiarata sorgente
 *     read_each      ripete la lettura su ogni risultato del passo precedente
 *
 * Il motore non sa che cosa sia «un sorgente»: lo dice `source_extension/1`, e
 * un linguaggio nuovo e' una riga di KB. Non sa nemmeno che i passi siano due:
 * li enumera.
 *
 * ⚠ Un piano NON e' un'autorizzazione. Ogni passo passa dallo stesso gate degli
 * strumenti singoli — `p0_read_in_root` per la lettura, `brain_policy_on` per il
 * permesso — quindi con i tool spenti il piano declina come tutto il resto. Un
 * piano che aggirasse i gate sarebbe un modo elegante di bucare il workspace. */
static int plan_list_sources(Brain *b, const char *dir,
                             char found[][KB_TERM_LEN], size_t max) {
    char exts[16][KB_TERM_LEN];
    const char *eq[1] = { NULL };
    size_t ne = kb_match(b->kb, "source_extension", eq, 1, exts, 16);
    size_t n = 0;
    for (size_t i = 0; i < ne && n < max; i++) {
        char eb[KB_TERM_LEN]; snprintf(eb, sizeof eb, "%s", exts[i]);
        const char *glob = kb_dequote(eb);
        if (!glob || !*glob) continue;
        char store[512]; char *argv[24];
        const ToolSlot slots[] = { { "dir", dir }, { "glob", glob } };
        if (!tool_argv_build(b, "list", slots, 2, store, sizeof store, argv, 24))
            continue;
        P0Obs obs;
        p0_exec(argv, ".", 20000, NULL, &obs);
        if (obs.verdict != P0_OK) continue;
        char flat[4000];
        collapse_ws(obs.out, flat, sizeof flat);
        for (char *tok = strtok(flat, " "); tok && n < max; tok = strtok(NULL, " "))
            if (*tok) snprintf(found[n++], KB_TERM_LEN, "%s", tok);
    }
    return (int)n;
}

static int mod_toolplan(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    if (!b || !b->kb || !raw) return 0;
    /* gen503: era 1024, e il testo di un compito vero ne fa 1485. Un piano che
     * si spegne SOPRA una certa lunghezza si spegne esattamente sui turni per
     * cui esiste — e in silenzio. Misurato sul banco: il prompt di match0 non
     * arrivava mai qui. */
    char low[8192];
    size_t rl = strlen(raw);
    if (rl == 0 || rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* quale piano chiede questo turno: lo dice la KB, non una catena */
    char plan[KB_TERM_LEN] = "";
    {
        char plans[8][KB_TERM_LEN];
        const char *pq[2] = { NULL, NULL };
        size_t np = kb_match(b->kb, "turn_plan", pq, 2, plans, 8);
        for (size_t i = 0; i < np && !plan[0]; i++) {
            const char *cq[2] = { plans[i], NULL };
            char cls[1][KB_TERM_LEN];
            if (kb_match(b->kb, "turn_plan", cq, 2, cls, 1) == 1 &&
                kb_cue_match(b, cls[0], low))
                snprintf(plan, sizeof plan, "%s", plans[i]);
        }
    }
    if (!plan[0]) return 0;
    (void)norm;

    /* un piano non aggira il permesso: e' il primo controllo, non l'ultimo */
    if (!brain_policy_on(b, "tools")) return 0;   /* lo dira' mod_toolpolicy */

    char found[64][KB_TERM_LEN]; int nfound = 0;
    size_t nread = 0, nfun = 0, nacted = 0;
    char detail[2400]; size_t dof = 0; detail[0] = '\0';

    char steps[8][KB_TERM_LEN];
    const char *sq[3] = { plan, NULL, NULL };
    size_t ns = kb_match(b->kb, "plan_step", sq, 3, steps, 8);
    for (size_t si = 0; si < ns; si++) {
        const char *aq[3] = { plan, steps[si], NULL };
        char acts[1][KB_TERM_LEN];
        if (kb_match(b->kb, "plan_step", aq, 3, acts, 1) != 1) continue;
        char ab[KB_TERM_LEN]; snprintf(ab, sizeof ab, "%s", acts[0]);
        const char *action = kb_dequote(ab);

        /* ── gen503 — UN PASSO PUO' ESSERE PROSA, E RIENTRARE ───────────────
         *
         * F.: «capire se il processo basato su thinking potrebbe aiutarci nel
         * costruire dei metaprompt che indirizzano, o addirittura della prosa
         * operativa derivata da conoscenza ingegneristica, e una volta
         * autodigerita dall'inferenza si trasformi nella connessione fra i vari
         * moduli».
         *
         * E' il punto fisso dell'astrazione applicato a questo file. Lo schema
         * era gia' conoscenza, ma OGNI OPERATORE era un ramo qui sotto: il
         * vocabolario delle azioni restava chiuso, e un passo nuovo costava una
         * ricompilazione — il mantra #19 un piano piu' su.
         *
         * Un passo dichiarato come `plan_utterance/3` non ha nessun ramo: e'
         * una frase che parrot0 DICE A SE STESSO, rientrando dalla stessa porta
         * da cui entrano i turni dell'interlocutore. Cio' che il turno lascia
         * in KB — un verdetto letto, una struttura ingerita — e' la
         * connessione fra i moduli, e non passa da qui.
         *
         * I segnaposto `{predicato}` si riempiono dal primo valore di quel
         * predicato unario: cosi' «run {build_command}» resta generale come lo
         * era il ramo che sostituisce.
         *
         * ⚠ La prosa si prova PRIMA dei rami: un passo che si puo' dire non ha
         * bisogno di essere compilato. E il rientro e' a profondita' uno — un
         * piano che si richiama non e' un piano. */
        {
            char utt[512];
            const char *uq[3] = { plan, steps[si], NULL };
            char uv[1][KB_TERM_LEN];
            if (!g_plan_reentry &&
                kb_match(b->kb, "plan_utterance", uq, 3, uv, 1) == 1) {
                char ub[KB_TERM_LEN]; snprintf(ub, sizeof ub, "%s", uv[0]);
                snprintf(utt, sizeof utt, "%s", kb_dequote(ub));
                /* i segnaposto: {pred} -> primo valore di pred/1 */
                for (int guard = 0; guard < 8; guard++) {
                    char *lb = strchr(utt, '{');
                    if (!lb) break;
                    char *rb = strchr(lb, '}');
                    if (!rb) break;
                    char key[64];
                    size_t kl = (size_t)(rb - lb - 1);
                    if (kl == 0 || kl >= sizeof key) break;
                    memcpy(key, lb + 1, kl); key[kl] = '\0';
                    char vv[1][KB_TERM_LEN];
                    const char *vq[1] = { NULL };
                    const char *val = "";
                    char vb[KB_TERM_LEN] = "";
                    if (kb_match(b->kb, key, vq, 1, vv, 1) == 1) {
                        snprintf(vb, sizeof vb, "%s", vv[0]);
                        val = kb_dequote(vb);
                    }
                    char rebuilt[512];
                    snprintf(rebuilt, sizeof rebuilt, "%.*s%s%s",
                             (int)(lb - utt), utt, val, rb + 1);
                    snprintf(utt, sizeof utt, "%s", rebuilt);
                }
                char sub[4096];
                g_plan_reentry = 1;
                brain_respond(b, utt, sub, sizeof sub);
                g_plan_reentry = 0;
                nacted++;
                if (dof + 1 < sizeof detail)
                    dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                                            "%s%s", dof ? ", " : "", sub);
                continue;
            }
        }

        if (!strcmp(action, "list_sources")) {
            nfound = plan_list_sources(b, ".", found, 64);
        } else if (!strcmp(action, "state_obligation")) {
            /* Che cosa il turno deve PRODURRE: non lo decide questo passo, lo
             * legge. `must_produce/2` e' derivato da cio' che il build ha
             * nominato, quindi senza il passo precedente qui non c'e' niente —
             * ed e' giusto che non ci sia. */
            char arts[16][KB_TERM_LEN];
            const char *mq[2] = { NULL, "build" };
            size_t na = kb_match(b->kb, "must_produce", mq, 2, arts, 16);
            for (size_t ai = 0; ai < na; ai++) {
                char ab2[KB_TERM_LEN]; snprintf(ab2, sizeof ab2, "%s", arts[ai]);
                const char *art = kb_dequote(ab2);
                nacted++;
                if (dof + 1 < sizeof detail)
                    dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                                            "%smust produce %s", dof ? ", " : "",
                                            art);
                /* e il contratto, se l'abbiamo gia' letto da qualche header */
                /* `kb_match` con il primo argomento libero restituisce i
                 * valori di QUEL argomento — cioe' le funzioni, non le
                 * clausole. Contare quelle sarebbe stato dire «1 clausola»
                 * dove ce ne sono cinque: si conta per funzione e si somma. */
                char fns[16][KB_TERM_LEN];
                const char *fq2[2] = { NULL, NULL };
                size_t nfn = kb_match(b->kb, "contract_condition", fq2, 2, fns, 16);
                size_t nc2 = 0;
                for (size_t fi = 0; fi < nfn; fi++) {
                    char conds[32][KB_TERM_LEN];
                    const char *cq2[2] = { fns[fi], NULL };
                    nc2 += kb_match(b->kb, "contract_condition", cq2, 2, conds, 32);
                }
                if (nc2 && dof + 1 < sizeof detail)
                    dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                                            " (%zu contract clause%s known)",
                                            nc2, nc2 == 1 ? "" : "s");
            }
        } else if (!strcmp(action, "produce_artifact")) {
            /* ── gen503 — IL PASSO CHE CHIUDE IL CICLO ──────────────────────
             *
             * Qui si incontrano tre organi che fino a ieri esistevano da soli:
             * cio' che il build dice di dover produrre (`must_produce/2`), cio'
             * che i sorgenti letti dicono di essere dichiarato e mai definito
             * (`undefined_declaration/1`), e la forma che il CONTRATTO letto
             * dalla prosa dell'header seleziona (`shape_satisfies/2`).
             *
             * Nessuna delle tre decisioni sta qui: questo ramo le legge, emette
             * e scrive passando dal gate del workspace come ogni altro
             * strumento. Se nessuna forma dichiarata soddisfa il contratto non
             * si scrive niente e lo si DICE — che e' la risposta giusta, non
             * un fallimento da nascondere. */
            char arts[8][KB_TERM_LEN];
            const char *mq2[2] = { NULL, "build" };
            size_t nart = kb_match(b->kb, "must_produce", mq2, 2, arts, 8);
            char fns[16][KB_TERM_LEN];
            const char *uq[1] = { NULL };
            size_t nfn = kb_match(b->kb, "undefined_declaration", uq, 1, fns, 16);
            for (size_t ai = 0; ai < nart && nfn > 0; ai++) {
                char ab3[KB_TERM_LEN]; snprintf(ab3, sizeof ab3, "%s", arts[ai]);
                const char *art = kb_dequote(ab3);
                for (size_t fi = 0; fi < nfn; fi++) {
                    char fb[KB_TERM_LEN]; snprintf(fb, sizeof fb, "%s", fns[fi]);
                    const char *fn = kb_dequote(fb);
                    char shapes[4][KB_TERM_LEN];
                    const char *ssq[2] = { NULL, fns[fi] };
                    size_t nsh = kb_match(b->kb, "shape_satisfies", ssq, 2, shapes, 4);
                    if (nsh == 0) {
                        nacted++;
                        if (dof + 1 < sizeof detail)
                            dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                                "%sno declared shape satisfies the contract of %s",
                                dof ? ", " : "", fn);
                        continue;
                    }
                    char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", shapes[0]);
                    const char *shape = kb_dequote(sb);
                    char src2[8192];
                    if (!code_synth_from_shape(b->kb, shape, fn, '>', src2, sizeof src2))
                        continue;
                    size_t written = 0;
                    int wr = code_write_file(art, src2, &written);
                    nacted++;
                    if (dof + 1 < sizeof detail)
                        dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                            "%s%s %s from shape %s",
                            dof ? ", " : "",
                            (wr == 1 || wr == 2) ? "wrote" : "could not write",
                            art, shape);
                    break;
                }
            }
        } else if (!strcmp(action, "read_each")) {
            for (int i = 0; i < nfound; i++) {
                char code[65536]; int trunc = 0;
                if (p0_read_in_root(found[i], code, sizeof code, &trunc) != P0_OK)
                    continue;
                /* il linguaggio si riconosce, non si assume: e' lo stesso
                 * rilevatore che usa la lettura di un file singolo, cosi' le
                 * due vie non possono divergere. */
                char names[64][KB_TERM_LEN];
                int lang_id = identify_code_lang(code, b);
                size_t k = code_ingest_source(b->kb, code, found[i],
                                              lang_id == 2 ? "python" : "c",
                                              names, 64, NULL);
                nread++; nfun += k;
                if (dof + 1 < sizeof detail)
                    dof += (size_t)snprintf(detail + dof, sizeof detail - dof,
                                            "%s%s (%zu)", dof ? ", " : "",
                                            found[i], k);
            }
        }
    }
    if (nread == 0 && nacted == 0) return 0;

    char nr[24], nf[24];
    snprintf(nr, sizeof nr, "%zu", nread);
    snprintf(nf, sizeof nf, "%zu", nfun);
    /* gen503: COME si riferisce di un piano lo dice il piano. Prima il rapporto
     * era uno solo — «ho letto N sorgenti, M funzioni» — e un piano che non
     * legge sorgenti lo diceva lo stesso, con due numeri a zero: una risposta
     * che descrive un altro lavoro. Il termine e' un fatto per schema, e il
     * vecchio resta il default per chi non ne dichiara uno. */
    char term[KB_TERM_LEN] = "plan_sources_read";
    {
        const char *tq[2] = { plan, NULL };
        char tv[1][KB_TERM_LEN];
        if (kb_match(b->kb, "plan_report_template", tq, 2, tv, 1) == 1) {
            char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", tv[0]);
            snprintf(term, sizeof term, "%s", kb_dequote(tb));
        }
    }
    char msg[3000];
    { const KbResponseSlot rs[] = { { "count", nr }, { "functions", nf },
                                    { "detail", detail } };
      if (!kb_response_slots(b, term, rs, 3, msg, sizeof msg))
          snprintf(msg, sizeof msg, "%s.", detail); }
    put(msg, out, out_size);
    return 1;
}

static int mod_piact(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    size_t rl = strlen(raw);
    if (rl == 0 || rl >= 480) return 0;

    char low[512];
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Detect the intent BEFORE tokenizing (split_words mutates its buffer).
     * gen223 (docs/use-on-pi-agent.md experiment + F.'s KB-first steer): the natural
     * phrasings that name each read-only verb are NOT C string literals — that chained
     * `cue("...")` was a phrasebook, exactly the impostor shape PRINCIPLES.md rejects.
     * The fragments now live as intent_cue(piact_*, "…") in kb/core/intents.p0 and are
     * matched by kb_cue_match (the same substring engine as the brevity cues), so a new
     * phrasing is DATA — teachable at runtime ("learn … as a cue for …"), no recompile.
     * Only the *structural* anchors (a leading "read "/"grep "/"ls" prefix) and the AND
     * logic ("find" + "named") stay in C, since those are shape, not vocabulary. */
    /* ── gen494 — QUALE STRUMENTO SERVE LO DICE LA KB, NON UNA CATENA ────────
     *
     * Qui c'erano cinque `int want_…` calcolati da una catena di `||` con i
     * nomi degli strumenti e le loro ancore scritti nel C. Due difetti in uno,
     * gia' segnati da O1 di `coding-agent-todo.md`: **il vocabolario dei tool
     * dell'agente stesso non era insegnabile a runtime**, e aggiungere uno
     * strumento voleva dire ricompilare invece di dichiarare.
     *
     * Ora l'inventario e' `local_tool(Nome, Intento, Glossa)` e le ancore sono
     * `tool_anchor(Nome, "prefisso")`. La FORMA — «il turno comincia con» —
     * resta meccanica nel C; il TESTO dell'ancora e' conoscenza, ed e' per
     * questo che «leggi src/report.c» ora funziona come «read src/report.c».
     *
     * Uno strumento nuovo e' due righe di KB e zero righe qui. */
    char tool[KB_TERM_LEN] = "";
    {
        char tools[16][KB_TERM_LEN];
        const char *tq[3] = { NULL, NULL, NULL };
        size_t nt = kb_match(b->kb, "local_tool", tq, 3, tools, 16);
        for (size_t i = 0; i < nt && !tool[0]; i++) {
            /* l'intento dichiarato per questo strumento */
            const char *iq[3] = { tools[i], NULL, NULL };
            char intents[1][KB_TERM_LEN];
            if (kb_match(b->kb, "local_tool", iq, 3, intents, 1) == 1 &&
                kb_cue_match(b, intents[0], low)) {
                snprintf(tool, sizeof tool, "%s", tools[i]);
                break;
            }
            /* le sue ancore strutturali */
            const char *aq[2] = { tools[i], NULL };
            char anchors[8][KB_TERM_LEN];
            size_t na = kb_match(b->kb, "tool_anchor", aq, 2, anchors, 8);
            for (size_t k = 0; k < na && !tool[0]; k++) {
                char ab[KB_TERM_LEN]; snprintf(ab, sizeof ab, "%s", anchors[k]);
                const char *anc = kb_dequote(ab);
                if (anc && *anc &&
                    (ci_prefix(low, anc) || (anc[strlen(anc)-1] != ' ' && ci_eq(low, anc))))
                    snprintf(tool, sizeof tool, "%s", tools[i]);
            }
        }
    }
    /* la sola forma che resta nel C, perche' e' struttura e non vocabolario:
     * «trova» richiede anche di dire COME si chiama cio' che si cerca. */
    /* ── gen503 — IL CANCELLO NON ENUMERA PIU' GLI STRUMENTI ────────────────
     *
     * Qui c'era `if (!(want_grep || want_find || want_list || want_read ||
     * want_run)) return 0;` — i cinque strumenti di sola lettura scritti a mano
     * nel C, subito sotto l'inventario che li legge dalla KB.
     *
     * Misurato il 2026-09-04: `write` era DICHIARATO da gen502
     * (`local_tool(write, …)`, la classe `write_file_request`, i suoi
     * `tool_slot_cue`) e il suo ramo, ottanta righe piu' sotto, non veniva MAI
     * raggiunto — perche' il nome non compariva in nessuno di quei cinque
     * `strcmp`. Una capacita' costruita, documentata e fenced-out da una catena
     * compilata: e' il mantra #19 preso sul fatto, e il costo era l'intera
     * differenza fra «parrot0 non sa scrivere un file» e «sa scriverlo».
     *
     * `tool` viene gia' dalla KB. Se un turno ha selezionato uno strumento
     * dichiarato, questo modulo e' quello giusto; uno strumento nuovo resta due
     * righe di KB e zero righe qui, che era la promessa di gen494.
     *
     * ⚠ Il residuo che resta, e va detto: piu' sotto ogni ramo si riconosce
     * confrontando `tool` con il proprio nome dichiarato. Quello NON e' la
     * catena di prima — e' il legame fra un'implementazione e la riga di KB che
     * la dichiara, e ogni nome compare una volta sola, nel ramo che lo serve.
     * Uno strumento nuovo con un'azione nuova richiede comunque il suo C: e'
     * una primitiva, non un membro di una classe. Cio' che NON deve piu'
     * succedere e' che un nome dichiarato in KB venga scartato da un elenco
     * scritto altrove — che e' esattamente quello che era successo a `write`. */
    if (!tool[0])
        return 0;

    /* gen331 (TODO.md P1/09): with tools off this module simply steps aside, as it
     * always did — because several faculties BELOW it (codeast reading a snippet
     * into structure, prosepage extracting facts from a page) legitimately serve
     * these same phrasings without needing tool mode. Claiming the turn here in
     * order to announce the policy would steal it from them, which is what my first
     * attempt did: it broke eleven passing contracts to deliver an honest sentence
     * nobody had asked for. The policy is announced instead by mod_toolpolicy, at
     * the END of the registry — where it speaks only if nobody else could. */
    if (!brain_policy_on(b, "tools")) return 0;

    /* ── gen494 — IL FORMATO SI CONSUMA PRIMA CHE GLI SLOT LEGGANO ──────────
     *
     * «listami i file in formato markdown» chiede DUE cose: un'azione e una
     * forma. Lasciando la richiesta di forma dentro il turno, gli estrattori di
     * slot ci inciampavano — `in` prendeva «formato» come cartella, e «markdown»
     * diventava il filtro `*.md`, cosi' l'elenco tornava con il solo README.
     * Non erano difetti degli estrattori: era che nessuno aveva tolto dal turno
     * la parte gia' interpretata.
     *
     * Quale porzione sia una richiesta di forma e' conoscenza
     * (`format_constraint/2`); toglierla e' meccanica. Il turno che arriva agli
     * slot e' quindi la sola richiesta d'azione, e il formato viaggia a parte. */
    /* il turno INTERO resta per il formato; agli slot arriva quello ripulito */
    static char fmt_turn[2048];
    snprintf(fmt_turn, sizeof fmt_turn, "%s", low);
    g_tool_turn = fmt_turn;
    {
        char (*rows)[KB_TERM_LEN] = NULL; size_t nrows = 0;
        const char *fq[2] = { NULL, NULL };
        if (kb_match_all(b->kb, "format_constraint", fq, 2, &rows, &nrows)) {
            for (size_t i = 0; i < nrows; i++) {
                const char *cq[2] = { rows[i], NULL };
                char evs[8][KB_TERM_LEN];
                size_t ne = kb_match(b->kb, "format_constraint", cq, 2, evs, 8);
                for (size_t k = 0; k < ne; k++) {
                    char eb[KB_TERM_LEN]; snprintf(eb, sizeof eb, "%s", evs[k]);
                    const char *ev = kb_dequote(eb);
                    if (!ev || !*ev) continue;
                    char *at = strstr(low, ev);
                    while (at) {                 /* si cancella, non si accorcia */
                        for (size_t z = 0; ev[z]; z++) at[z] = ' ';
                        at = strstr(low, ev);
                    }
                }
            }
        }
        free(rows);
    }
    char rawcopy[512]; snprintf(rawcopy, sizeof rawcopy, "%s", raw);
    char *w[96]; size_t nw = split_words(rawcopy, w, 96);

    /* ---- gen502: SCRIVERE CONTENUTO IN UN FILE, e perche' mancava ----------
     *
     * Misurato su `tests/challenge/tasks/match0/seed`, la codebase su cui
     * freebuff ha fatto 100/100: parrot0 elenca (`run ls`), legge in struttura
     * (`read main.c`), risponde con span e provenienza, ed ESEGUE IL BUILD
     * leggendone il verdetto (`run make` -> «No rule to make target
     * 'strjoin.c'»). Sa creare un file — VUOTO. Fra lui e un compito chiuso
     * c'era un VERBO MANCANTE, non una difficolta' del compito.
     *
     * L'inventario `local_tool` aveva search/find/list/read/run e nessuno che
     * scrivesse, e `run` rifiuta le redirezioni di shell per progetto — una
     * buona proprieta' di sicurezza, che non va aggirata ma completata.
     *
     * KB-first come gli altri: la CLASSE della richiesta e' `write_file_request`
     * (teachable, EN+IT), il nome del file si riconosce per FORMA (un token con
     * estensione — struttura, non vocabolario), e il contenuto lo estrae
     * `code_extract_source`, cioe' la stessa percezione universale degli span
     * che gia' serve mod_gen. Il C qui non nomina nessun linguaggio e nessuna
     * parola di dominio.
     *
     * ⚠ Il gate del workspace non si sposta: `code_write_file` rifiuta i
     * percorsi, i dotfile e i nomi non basename, e RILEGGE il file prima di
     * dire che l'ha scritto — «ho scritto» senza rilettura e' una promessa, non
     * un fatto. */
    if (kb_cue_match(b, "write_file_request", low)) {
        char body[262144];
        if (code_extract_source(b->kb, raw, body, sizeof body) == 1 && body[0]) {
            /* ⛔ QUI AVEVO NASCOSTO CONOSCENZA NEL C, e F. lo ha visto.
             *
             * La prima stesura riconosceva il nome del file per FORMA — «un
             * token con un punto e un'estensione, mai un percorso» — piu' una
             * politica non scritta («vince l'ultimo che combacia»). Sembrava
             * struttura; erano due decisioni compilate, e nessuna delle due si
             * poteva correggere parlando. Il test del bilancio del mantra #18 lo
             * diceva in un numero: C +131 contro KB +30.
             *
             * Il meccanismo giusto esisteva gia' ed e' di gen494: QUALE PAROLA
             * INTRODUCE UNO SLOT e' `tool_slot_cue(name, …)`, la stessa classe
             * che serve `find … named X`. Aggiungere «into» o «dentro» e' una
             * riga di KB, non una ricompilazione — ed e' esattamente cio' che
             * qui prima non si poteva fare. */
            const char *name = NULL;
            for (size_t i = 0; i + 1 < nw; i++)
                if (tool_slot_word(b, "name", w[i])) name = w[i + 1];
            if (name) {
                char nb[80]; snprintf(nb, sizeof nb, "%s", name);
                rstrip_punct(nb);
                size_t written = 0;
                int wr = code_write_file(nb, body, &written);
                if (wr == 1 || wr == 2) {
                    char nbytes[32];
                    snprintf(nbytes, sizeof nbytes, "%zu", written);
                    const KbResponseSlot _rs[] = { { "name", nb }, { "bytes", nbytes } };
                    kb_term_say(b, wr == 2 ? "rewrote_file_x_with_y_bytes_verified"
                                           : "wrote_file_x_with_y_bytes_verified",
                                _rs, 2, out, out_size);
                    note_artifact(b, "file", nb);
                    store_proof(b, out);
                    return 1;
                }
                {   const KbResponseSlot _rs[] = { { "name", nb } };
                    kb_term_say(b, "i_could_not_write_x_and_i_do_not_say_i_did",
                                _rs, 1, out, out_size); }
                store_proof(b, out);
                return 1;
            }
        }
    }

    /* ---- locate / search for a symbol ---- */
    if (!strcmp(tool, "search")) {
        /* the pattern is the token after the cue word; sanitize for single-quotes. */
        const char *pat = NULL;
        /* gen494: quali parole introducono il pattern e' `tool_slot_cue(pattern, …)`. */
        for (size_t i = 0; i + 1 < nw; i++)
            if (tool_slot_word(b, "pattern", w[i])) pat = w[i+1];
        if (!pat && nw >= 2) pat = w[nw-1];
        if (!pat) return 0;
        /* gen329: the pattern is no longer scrubbed of quotes and backticks. It used
         * to be, because it was about to be pasted between single quotes into a
         * SHELL LINE, where a quote would break out of the quoting and a backtick
         * would start a command substitution. There is no shell line any more: the
         * pattern is argv[3] of an execve'd grep, and a backtick there is a backtick.
         * Removing the sanitizer is not a relaxation — it is the proof that the
         * injection surface it was defending is gone. */
        char patbuf[128];
        rstrip_punct((char*)pat);
        if (!*pat) return 0;
        snprintf(patbuf, sizeof patbuf, "%s", pat);
        const char *dir = find_dir_kb(b, w, nw);
        char dirbuf[256]; int claimed;
        if (!piact_dir(b, dir, dirbuf, sizeof dirbuf, out, out_size, &claimed)) return claimed;

        /* KB-FIRST: when the target is a bare code identifier, answer by STRUCTURE,
         * not by text. code_locate / code_find_callers are the very localizers that
         * drive `make swe-solve` (CODE-MASTERY §4) — so "where is X" / "what calls X"
         * inside pi reuse the SWE-bench spine, grounded in a real parse of real files
         * rather than a line match that a comment or a string could fake. Free-text
         * patterns (with spaces or punctuation) still fall through to grep. */
        int is_ident = isalpha((unsigned char)patbuf[0]) || patbuf[0] == '_';
        for (const char *p = patbuf; *p && is_ident; p++)
            if (!(isalnum((unsigned char)*p) || *p == '_')) is_ident = 0;
        int want_callers = kb_cue_match(b, "60_agent_tools_chain500", low);
        if (is_ident) {
            char file[256] = ""; int located = code_locate(dirbuf, patbuf, file, sizeof file);
            char callers[16][KB_TERM_LEN]; size_t nc = code_find_callers(dirbuf, patbuf, callers, 16);
            if (located || nc) {
                char msg[1200]; size_t off = 0;
                if (located)
                    off += (size_t)snprintf(msg+off, sizeof msg-off,
                                            "`%s` is defined in %s", patbuf, file);
                else
                    { char _t1[512];
                    const KbResponseSlot _r1[] = { { "patbuf", patbuf }, { "dirbuf", dirbuf } };
                    kb_term_say(b, "x_is_not_defined_under_x", _r1, 2, _t1, sizeof _t1);
                    off += (size_t)snprintf(msg+off, sizeof msg-off, "%s", _t1);
                    }
                if (nc && (want_callers || located)) {
                    off += (size_t)snprintf(msg+off, sizeof msg-off, "; called by ");
                    for (size_t i = 0; i < nc && off < sizeof msg; i++)
                        off += (size_t)snprintf(msg+off, sizeof msg-off, "%s%s",
                                                i ? ", " : "", callers[i]);
                } else if (located && !nc && want_callers) {
                    { char _t2[512];
                    const KbResponseSlot _r2[] = { { "x", "" } };
                    kb_term_say(b, "nothing_calls_it", _r2, 0, _t2, sizeof _t2);
                    off += (size_t)snprintf(msg+off, sizeof msg-off, "%s", _t2);
                    }
                }
                if (off < sizeof msg) snprintf(msg+off, sizeof msg-off, ".");
                put(msg, out, out_size);
                if (b) {
                    char proof[320];
                    { const KbResponseSlot _rs[] = { { "patbuf", patbuf }, { "dirbuf", dirbuf } };
      kb_term_say(b, "structural_locate_of_x_under_x_code_locate_c", _rs, 2, proof, sizeof proof); }
                    store_proof(b, proof);
                }
                return 1;
            }
            /* identifier but no structural hit — fall through to a text grep. */
        }

        /* `--` so a pattern that begins with '-' stays a pattern and never becomes
         * an option: with the shell gone, argv-level confusion is the only injection
         * left, and it is closed by construction rather than by scrubbing. */
        /* gen494: il comando viene da `tool_argv(search, …)`, non da qui. */
        char gstore[512]; char *gargv[24];
        const ToolSlot gs[] = { { "pattern", patbuf }, { "dir", dirbuf } };
        if (!tool_argv_build(b, "search", gs, 2, gstore, sizeof gstore,
                             gargv, 24)) return 0;
         char label[160];
         kb_term_say(b, "matches_for_x", (const KbResponseSlot[]){
                         { "pattern", patbuf } }, 1, label, sizeof label);
         return piact_obs(b, gargv, label, out, out_size);
    }

    /* ---- find a file by name ---- */
    if (!strcmp(tool, "find")) {
        const char *name = NULL;
        for (size_t i = 0; i + 1 < nw; i++)
            if (tool_slot_word(b, "name", w[i])||ci_eq(w[i],"file")) name = w[i+1];
        if (!name && nw >= 2) name = w[nw-1];
        if (!name) return 0;
        rstrip_punct((char*)name);
        if (!pathish_token(name)) return 0;
        const char *dir = find_dir_kb(b, w, nw);
        char dirbuf[256]; int claimed;
        if (!piact_dir(b, dir, dirbuf, sizeof dirbuf, out, out_size, &claimed)) return claimed;
        /* gen494: idem — `tool_argv(find, …)`. */
        char fstore[512]; char *fargv[24];
        const ToolSlot fs[] = { { "dir", dirbuf }, { "name", name } };
        if (!tool_argv_build(b, "find", fs, 2, fstore, sizeof fstore,
                             fargv, 24)) return 0;
        char label[200];
        kb_term_say(b, "found_file", (const KbResponseSlot[]){
                        { "name", name } }, 1, label, sizeof label);
        return piact_obs(b, fargv, label, out, out_size);
    }

    /* ---- list files (optionally filtered by a glob) ---- */
    if (!strcmp(tool, "list")) {
        const char *dir = find_dir_kb(b, w, nw);
        char dirbuf[256]; int claimed;
        if (!piact_dir(b, dir, dirbuf, sizeof dirbuf, out, out_size, &claimed)) return claimed;
        char glob[64]; int has_glob = detect_glob(b, low, glob, sizeof glob);
        char label[200];
        /* ⛔ gen494 — «NIENTE» ERA FALSO, E DETTO CON SICUREZZA.
         *
         * La profondita' era `-maxdepth 1` cablata. In una cartella di progetto
         * — dove i sorgenti stanno sotto `src/` — «listami i file .c» rispondeva
         * «nothing», che non e' una risposta incompleta: e' una risposta FALSA,
         * ed e' il mantra #7 (un errore detto con sicurezza e' peggio di un
         * muro). Nessun utente puo' sapere che l'elenco si fermava al primo
         * livello, perche' l'elenco non lo diceva.
         *
         * Quanto in fondo guardare e' CONOSCENZA — `listing_max_depth/1` — e non
         * una costante: un progetto profondo la alza dicendolo. Senza il fatto
         * si guarda ovunque, perche' «quali file ci sono» significa quello. */
        /* gen494 — IL COMANDO DELL'ELENCO VIENE DA KB.
         *
         * Qui c'erano due argv letterali con `find`, `-maxdepth 1` e `-type f`
         * dentro. Quel `-maxdepth 1` faceva rispondere «nothing» a «listami i
         * file .c» in una cartella di progetto — falso, perche' i sorgenti
         * stavano sotto `src/`. Ora il comando e' `tool_argv(list, …)`: fra le
         * varianti vince quella con piu' segnaposto soddisfatti, quindi la
         * richiesta con un filtro sceglie da se' il template col filtro, senza
         * un `if` che lo decida. */
        char lstore[512]; char *largv[24];
        const ToolSlot ls_slots[] = { { "dir", dirbuf },
                                      { "glob", has_glob ? glob : NULL } };
        if (!tool_argv_build(b, "list", ls_slots, 2, lstore, sizeof lstore,
                             largv, 24)) return 0;
        char lang[8]; current_lang(b, lang, sizeof lang);
        int it = lex_class_member(b, "entity_pronoun", lang);
        if (has_glob)
            snprintf(label, sizeof label, it ? "I file `%s` in %s" : "The `%s` files in %s",
                     glob, dirbuf);
        else
            snprintf(label, sizeof label, it ? "I file in %s" : "The files in %s", dirbuf);
        return piact_obs(b, largv, label, out, out_size);
    }

    /* ---- read a file ---- */
    if (!strcmp(tool, "read")) {
        const char *file = find_file(w, nw);
        if (!file || !pathish_token(file)) return 0;

        /* gen329 (TODO.md P0/05): the bytes come through the WORKSPACE GATE, before
         * anything else looks at them. This is the exact line where `read
         * /etc/os-release` used to leave the repository: the old code called
         * code_read_file() — an unguarded fopen — and, failing that, shelled out to
         * `head -n 40 /etc/os-release`. Neither ever asked WHERE the file was.
         * p0_read_in_root answers that question by construction, and an escape is
         * declined by name (`unsafe_path`) instead of being quietly served.
         *
         * Reading is also no longer a subprocess at all: no `head`, no shell, no
         * pipe — just the fd that the containment walk itself opened and validated. */
        static char code[262144];
        int trunc = 0;
        P0Verdict rv = p0_read_in_root(file, code, sizeof code, &trunc);
        if (rv != P0_OK) {
            char m[600];
            { 
              char _v1[48]; snprintf(_v1, sizeof _v1, "%s", p0_root());
  const KbResponseSlot _rs[] = { { "file", file }, { "p0_root", _v1 } };
              kb_term_say(b, "unsafe_path_x_is_not_a_regular_file_inside_m", _rs, 2, m, sizeof m); }
            put(m, out, out_size);
            if (b) {
                char proof[320];
                snprintf(proof, sizeof proof, "refused to read %s: unsafe_path (outside %s)",
                         file, p0_root());
                store_proof(b, proof);
            }
            return 1;
        }

        /* KB-FIRST read. A remote LLM "reads" a file by pulling its bytes into its
         * context; parrot0 reads it into a source-qualified, revisioned IR.  The
         * compatibility predicates `code_function` and `code_calls` are KB views
         * over that observation, not facts asserted by this tool.  Only when the
         * file is not ingestable source (a header with no defs, plain data) does it
         * fall back to an honest textual look. */
        if (b->kb) {
            char names[32][KB_TERM_LEN];
            int clang = identify_code_lang(code, b);
            CodeIngestReport rep;
            size_t k = code_ingest_source(b->kb, code, file,
                                          clang == 2 ? "python" : "c",
                                          names, 32, &rep);
            if (k > 0) {
                size_t shown = k < 32 ? k : 32;
                char _t3[512];
                const KbResponseSlot _r3[] = { { "file", file } };
                /* gen503: un header non definisce niente. Il ruolo osservato
                 * sceglie la frase — «declares» quando tutto cio' che si e'
                 * visto sono prototipi — invece di chiamare definizione un
                 * prototipo per comodita' di una sola stringa. */
                kb_term_say(b, rep.declarations == k
                                   ? "i_read_x_into_structure_it_declares"
                                   : "i_read_x_into_structure_it_defines",
                            _r3, 1, _t3, sizeof _t3);
                size_t off = (size_t)snprintf(out, out_size, "%s", _t3);
                for (size_t i = 0; i < shown && off < out_size; i++) {
                    const char *sep = (i==0) ? "" : (i==shown-1) ? " and " : ", ";
                    off += (size_t)snprintf(out+off, out_size-off, "%s%s", sep, names[i]);
                }
                if (off < out_size)
                    { char _t4[512];
                    char _t4_v0[48]; snprintf(_t4_v0, sizeof _t4_v0, "%zu", k);
                    char _t4_v1[48]; snprintf(_t4_v1, sizeof _t4_v1, "%s", k==1?"":"s");
                    const KbResponseSlot _r4[] = { { "k", _t4_v0 }, { "s", _t4_v1 } };
                    kb_term_say(b, "x_functionx_now_in_the_kb", _r4, 2, _t4, sizeof _t4);
                    snprintf(out+off, out_size-off, "%s", _t4);
                    }
                char proof[320];
                { 
                  char _v1[48]; snprintf(_v1, sizeof _v1, "%zu", k);
  const KbResponseSlot _rs[] = { { "file", file }, { "k", _v1 } };
                  kb_term_say(b, "read_ingested_x_x_code_ir_functions_observed", _rs, 2, proof, sizeof proof); }
                store_proof(b, proof);
                snprintf(b->last_tool_cmd, sizeof b->last_tool_cmd, "code_ingest(%s)", file);
                b->has_last_tool_cmd = 1;
                return 1;
            }
        }
        /* not ingestable source — an honest bounded textual look at the bytes we
         * already hold. Truncation is DECLARED, not silently swallowed. */
        char flat[4000];
        collapse_ws(code, flat, sizeof flat);
        char msg[4400];
        if (flat[0] == '\0')
            snprintf(msg, sizeof msg, "%s is empty.", file);
        else
            snprintf(msg, sizeof msg, "%s: %s%s", file, flat,
                     trunc ? " …[truncated]" : "");
        put(msg, out, out_size);
        if (b) {
            char proof[320];
            char _n[48]; snprintf(_n, sizeof _n, "%zu", strlen(code));
            { const KbResponseSlot _rs[] = { { "file", file }, { "bytes", _n },
                                             { "trunc", trunc ? ", truncated" : "" } };
              kb_term_say(b, "read_x_inside_the_workspace", _rs, 3, proof, sizeof proof); }
            store_proof(b, proof);
            snprintf(b->last_tool_cmd, sizeof b->last_tool_cmd, "read(%s)", file);
            b->has_last_tool_cmd = 1;
        }
        return 1;
    }

    /* ---- run a build/test command (authorized by a KB tool contract) ---- */
    if (!strcmp(tool, "run")) {
        /* gen329 (TODO.md P0/04): THE injection site. What used to happen here:
         * the text after "run " was copied into a string, its PREFIX was compared
         * against a whitelist, and the whole string — tail included — was handed to
         * popen(3). `run git status; printf P0_CHAINED` passed on its head and ran
         * its tail. The bug was never the whitelist's contents; it was that a shell
         * string cannot be validated by looking at its beginning.
         *
         * Now the prompt does not build a command at all. It is TOKENIZED into an
         * argv vector, the vector is authorized against tool_for/tool_subcmd facts
         * in the KB, and p0_exec execve's it. No shell is involved, so ';' has no
         * meaning left to exploit — it is simply part of argv[1], where `git` will
         * reject it as an unknown subcommand. We check for shell metacharacters
         * anyway, not because they are dangerous now, but because their presence
         * means the user thinks they are talking to a shell, and the honest answer
         * is to say that they are not. */
        const char *after = NULL;
        size_t run_len = 0;
        const char *run = tool_surface_find(b, low, "run_prefix", &run_len);
        if (run == low) after = raw + run_len;
        else {
            size_t compile_len = 0;
            const char *p = tool_surface_find(b, low, "compile_marker", &compile_len);
            if (p) after = raw + (p - low);
        }
        if (!after) after = raw;
        while (*after == ' ') after++;

        char cmdline[512]; snprintf(cmdline, sizeof cmdline, "%s", after);
        rstrip_punct(cmdline);
        if (strpbrk(cmdline, ";|&$`><\n")) {
            char m[600];
            {   const KbResponseSlot _rs[] = { { "x", "" } };
              kb_term_say(b, "unsafe_command_i_do_not_run_shell_syntax_i_e", _rs, 0, m, sizeof m); }
            put(m, out, out_size);
            if (b) store_proof(b, "refused: unsafe_command (shell metacharacters, no shell exists)");
            return 1;
        }

        /* the label must be taken BEFORE split_words, which writes NULs into the
         * buffer in place — after it, `cmdline` is only its first word. */
        char label[320]; snprintf(label, sizeof label, "`%s`", cmdline);

        char *cargv[32]; size_t cargc = split_words(cmdline, cargv, 31);
        if (cargc == 0) return 0;
        cargv[cargc] = NULL;

        char why[256];
        if (!tool_authorized(b, cargv, cargc, why, sizeof why)) {
            char m[700];
            {   const KbResponseSlot _rs[] = { { "cargv", cargv[0] }, { "why", why } };
              kb_term_say(b, "unsafe_command_i_am_not_authorized_to_run_x", _rs, 2, m, sizeof m); }
            put(m, out, out_size);
            if (b) {
                char proof[400];
                snprintf(proof, sizeof proof, "refused to run %s: unsafe_command (%s)",
                         cargv[0], why);
                store_proof(b, proof);
            }
            return 1;
        }
        return piact_obs(b, cargv, label, out, out_size);
    }

    return 0;
}

/* gen206 (composer, docs/plans/generative.md): verified code GENERATION — the honest
 * instance of "generate from the KNOWN, not from the probable". A remote LLM writes
 * code by sampling tokens and hoping; the artifact is plausible, not proven. parrot0
 * inverts it: it COMPOSES a structure (a function whose body is a binary arithmetic
 * relation) from knowledge of the operator, then SUBMITS that structure to the
 * deterministic oracle (code_eval) on sample inputs. The artifact is reported as
 * correct ONLY because it actually computes the intended relation — generator
 * proposes, oracle disposes. gen207 lifts this to ARTICULATED tasks: a long,
 * multi-step prompt ("write add, then use it to compute add(3,4), and also write the
 * variant mul") is split into ORDERED sub-goals, each run through the same compose/
 * verify/eval faculties, THREADING the artifacts (step 2 uses the function step 1
 * built) — a real plan executed and checked step by step, never a single guess. */

/* Query the KB (code_operator/2 in compose.p0) for the C operator the text cues.
 * Returns the operator char (+,-,*) and the matched cue word, or 0 if none. */
static char compose_op_from_kb(Brain *b, const char *low, char *opword, size_t opsz) {
    if (opword && opsz) opword[0] = '\0';
    if (!b || !b->kb) return 0;
    char cues[24][KB_TERM_LEN];
    const char *qall[2] = {NULL, NULL};
    size_t ncue = kb_match(b->kb, "code_operator", qall, 2, cues, 24);
    for (size_t i = 0; i < ncue; i++) {
        if (!strstr(low, cues[i])) continue;
        char sym[1][KB_TERM_LEN];
        const char *qsym[2] = {cues[i], NULL};
        if (kb_match(b->kb, "code_operator", qsym, 2, sym, 1) == 1)
            for (const char *p = sym[0]; *p; p++)
                if (*p=='+' || *p=='-' || *p=='*') {
                    if (opword) snprintf(opword, opsz, "%s", cues[i]);
                    return *p;
                }
    }
    return 0;
}

/* The function name: the identifier after function/called/named/variant, else "f". */
static void compose_name(const char *raw, char *name, size_t nsz) {
    snprintf(name, nsz, "f");
    char rc[512]; snprintf(rc, sizeof rc, "%s", raw);
    char *w[96]; size_t nw = split_words(rc, w, 96);
    for (size_t i = 0; i + 1 < nw; i++)
        if (ci_eq(w[i],"function") || ci_eq(w[i],"funzione") || ci_eq(w[i],"called") ||
            ci_eq(w[i],"named") || ci_eq(w[i],"variant") || ci_eq(w[i],"variante")) {
            rstrip_punct(w[i+1]);
            const char *t = w[i+1];
            int ok = (isalpha((unsigned char)t[0]) || t[0]=='_');
            for (const char *p = t; *p && ok; p++)
                if (!(isalnum((unsigned char)*p) || *p=='_')) ok = 0;
            if (ok && !ci_eq(t,"that") && !ci_eq(t,"named") && !ci_eq(t,"called") &&
                !ci_eq(t,"which") && !ci_eq(t,"to") && !ci_eq(t,"for"))
                { snprintf(name, nsz, "%s", t); return; }
        }
}

/* Compose+verify ONE binary-arith function from a clause. Returns 1 (verified, src
 * and name written), 0 (no operator cued — note holds the honest refusal), or -1
 * (composed but the oracle rejected it). The note is a human-facing fragment. */
/* gen209 (Track B/B3): query algo_shape(Name, Shape) from the KB and, if the request
 * `low` NAMES one of the known algorithms, return its name and schema. Matched
 * space/underscore-insensitively so the learned "bubble sort" and "bubble_sort" both
 * hit algo_shape(bubblesort, …). Returns 1 with name+shape filled, else 0. */
static int sort_shape_from_kb(Brain *b, const char *low,
                              char *name, size_t nsz, char *shape, size_t shsz) {
    if (!b || !b->kb) return 0;
    char names[16][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t nn = kb_match(b->kb, "algo_shape", q, 2, names, 16);
    if (nn == 0) return 0;

    char dl[2048]; size_t k = 0;             /* `low` with spaces/underscores removed */
    for (const char *p = low; *p && k < sizeof dl - 1; p++)
        if (*p != ' ' && *p != '_') dl[k++] = *p;
    dl[k] = '\0';

    for (size_t i = 0; i < nn; i++) {
        char dn[KB_TERM_LEN]; size_t m = 0;  /* the algo name, same despacing */
        for (const char *p = names[i]; *p && m < sizeof dn - 1; p++)
            if (*p != ' ' && *p != '_') dn[m++] = *p;
        dn[m] = '\0';
        if (m == 0 || !strstr(dl, dn)) continue;
        const char *q2[2] = { names[i], NULL };
        char sh[1][KB_TERM_LEN];
        if (kb_match(b->kb, "algo_shape", q2, 2, sh, 1) == 1) {
            snprintf(name, nsz, "%s", names[i]);
            snprintf(shape, shsz, "%s", sh[0]);
            return 1;
        }
    }
    return 0;
}

/* gen328 (TODO.md P3, forge C2/W6): INDUCE the schema from a page.
 *
 * algo_shape(bubblesort, nested_loop_compare_swap) was CURATED — a human wrote
 * it. That is the linear growth LOOP.md calls the impostor: every new algorithm
 * costs a generation, so generation n buys capability n and nothing compounds.
 *
 * This reads a page of STEPS and derives the schema from what the steps DO. The
 * page is reduced to the SET of step kinds it describes (step_cue facts say which
 * words mean which kind), and a shape is induced only when EVERY kind its
 * signature requires is present (shape_signature facts). The algorithm's NAME is
 * never consulted — a page could call it anything, and a page that describes a
 * different structure (insertion sort shifts and inserts; it never sweeps
 * adjacent pairs across repeated passes) covers no signature and is DECLINED.
 * Inventing a schema to have one would be the fabrication class all over again.
 *
 * Writes the induced shape into `shape` and returns 1; 0 if the page covers no
 * signature (with `missing` naming the first requirement it could not find).
 */
static int induce_shape_from_steps(Brain *b, const char *steps,
                                   char *shape, size_t shsz,
                                   char *missing, size_t msz) {
    if (!b || !b->kb || !steps) return 0;
    if (missing && msz) missing[0] = '\0';

    char low[1024];
    { size_t i = 0;
      for (; steps[i] && i + 1 < sizeof low; i++)
          low[i] = (char)tolower((unsigned char)steps[i]);
      low[i] = '\0'; }

    /* Every shape the synthesizer could instantiate. */
    char shapes[8][KB_TERM_LEN];
    const char *qs[2] = { NULL, NULL };
    size_t ns = kb_match(b->kb, "shape_signature", qs, 2, shapes, 8);

    for (size_t i = 0; i < ns; i++) {
        /* the kinds THIS shape requires */
        char kinds[8][KB_TERM_LEN];
        const char *qk[2] = { shapes[i], NULL };
        size_t nk = kb_match(b->kb, "shape_signature", qk, 2, kinds, 8);
        if (nk == 0) continue;

        int covered = 1;
        for (size_t k = 0; k < nk && covered; k++) {
            /* is this kind actually described in the page? */
            char cues[16][KB_TERM_LEN];
            const char *qc[2] = { kinds[k], NULL };
            size_t nc = kb_match(b->kb, "step_cue", qc, 2, cues, 16);
            int found = 0;
            for (size_t c = 0; c < nc && !found; c++) {
                char *cue_s = cues[c];
                size_t l = strlen(cue_s);
                if (l >= 2 && cue_s[0] == '"' && cue_s[l - 1] == '"') {
                    cue_s[l - 1] = '\0'; cue_s++;
                }
                if (*cue_s && strstr(low, cue_s)) found = 1;
            }
            if (!found) {
                covered = 0;
                if (missing && msz && !*missing)
                    snprintf(missing, msz, "%s", kinds[k]);
            }
        }
        if (covered) {
            snprintf(shape, shsz, "%s", shapes[i]);
            if (missing && msz) missing[0] = '\0';
            return 1;
        }
    }
    return 0;
}

static int compose_one(Brain *b, const char *raw, const char *low,
                       char *nameo, size_t nsz, char *src, size_t srcsz,
                       char *note, size_t notesz) {
    /* gen209 (Track B/B3): a SORT request whose algorithm is named in the KB is
     * SYNTHESIZED from its schema and DISPOSED by the run-grounded judge — knowledge
     * (algo_shape) -> synthesis (code_synth_from_shape) -> oracle (code_check_sort on
     * real unsorted vectors). Reported only if every vector comes back sorted AND a
     * permutation of the input. This is the honest BUILD half: no hardcoded body. */
    {
        char aname[64], shape[64];
        if (sort_shape_from_kb(b, low, aname, sizeof aname, shape, sizeof shape)) {
            compose_name(raw, nameo, nsz);
            if (lex_class_member(b, "60_agent_tools_lex919", nameo)) snprintf(nameo, nsz, "%s", aname);
            char comparator = '>';           /* algo_io says ascending */
            if (code_synth_from_shape(b->kb, shape, nameo, comparator, src, srcsz)) {
                char err[256];
                int v = code_check_sort(src, nameo, err, sizeof err);
                if (v == 1) {
                    {   const KbResponseSlot _rs[] = { { "x", "" } };
                      kb_term_say(b, "verified_by_execution_sorted_ascending_and_a", _rs, 0, note, notesz); }
                    return 1;
                }
                if (v == 0) { kb_term_say(b, "the_judge_ran_it_but_it_did_not_sort_every_ve", NULL, 0, note, notesz); return -1; }
                kb_term_say(b, "the_synthesized_sort_would_not_build_run_so", NULL, 0, note, notesz);
                return -1;
            }
        }
    }

    char opword[KB_TERM_LEN];
    char op = compose_op_from_kb(b, low, opword, sizeof opword);
    if (!op) {
        {   const KbResponseSlot _rs[] = { { "x", "" } };
          kb_term_say(b, "i_can_only_synthesize_and_verify_the_sum_pro", _rs, 0, note, notesz); }
        return 0;
    }
    compose_name(raw, nameo, nsz);
    snprintf(src, srcsz, "int %s(int a, int b) { return a %c b; }", nameo, op);
    long a1=6,b1=4,a2=9,b2=2;
    long e1 = (op=='+')?a1+b1:(op=='*')?a1*b1:a1-b1;
    long e2 = (op=='+')?a2+b2:(op=='*')?a2*b2:a2-b2;
    long g1=0,g2=0, A1[2]={a1,b1}, A2[2]={a2,b2};
    int ok = code_eval(src,nameo,A1,2,&g1) && g1==e1 &&
             code_eval(src,nameo,A2,2,&g2) && g2==e2;
    if (ok) {
        { 
          char _v1[48]; snprintf(_v1, sizeof _v1, "%ld", a1);
          char _v2[48]; snprintf(_v2, sizeof _v2, "%ld", b1);
          char _v3[48]; snprintf(_v3, sizeof _v3, "%ld", g1);
          char _v5[48]; snprintf(_v5, sizeof _v5, "%ld", a2);
          char _v6[48]; snprintf(_v6, sizeof _v6, "%ld", b2);
          char _v7[48]; snprintf(_v7, sizeof _v7, "%ld", g2);
  const KbResponseSlot _rs[] = { { "nameo", nameo }, { "a1", _v1 }, { "b1", _v2 }, { "g1", _v3 }, { "nameo2", nameo }, { "a2", _v5 }, { "b2", _v6 }, { "g2", _v7 } };
          kb_term_say(b, "verified_by_symbolic_execution_x_x_x_x_x_x_x", _rs, 8, note, notesz); }
        return 1;
    }
    kb_term_say(b, "the_oracle_did_not_confirm_it", NULL, 0, note, notesz);
    return -1;
}

/* Parse a concrete call NAME(int, int, ...) anywhere in `s`. Writes name + args. */
static int parse_int_call(const char *s, char *name, size_t nsz,
                          long *argv, size_t *argc, size_t maxa) {
    for (const char *p = s; *p; ) {
        if (!(isalpha((unsigned char)*p) || *p=='_')) { p++; continue; }
        const char *id = p;
        while (isalnum((unsigned char)*p) || *p=='_') p++;
        size_t l = (size_t)(p - id);
        const char *q = p; while (*q==' ') q++;
        if (*q != '(') continue;
        const char *a = q + 1; long tmp[8]; size_t n=0; int ok=1, any=0;
        while (*a && *a != ')') {
            while (*a==' ' || *a==',') a++;
            if (*a==')' || !*a) break;
            int sign=1; if (*a=='-'){sign=-1;a++;} else if (*a=='+') a++;
            if (!isdigit((unsigned char)*a)) { ok=0; break; }
            long v=0; while (isdigit((unsigned char)*a)) { v=v*10+(*a-'0'); a++; }
            any=1; if (n<8) tmp[n++]=sign*v; else { ok=0; break; }
            while (*a==' ') a++;
            if (*a!=',' && *a!=')') { ok=0; break; }
        }
        if (ok && any && l < nsz) {
            memcpy(name, id, l); name[l]='\0';
            size_t k = n<maxa ? n : maxa;
            for (size_t i=0;i<k;i++) argv[i]=tmp[i];
            *argc = k;
            return 1;
        }
    }
    return 0;
}

/* gen208: dispatch ONE planner clause that is neither compose nor eval (e.g. "tell
 * me about bubble sort") through the normal registry, skipping `compose` itself so the
 * planner never re-enters. First-match-wins, like brain_respond's main loop, but
 * bounded: it returns the claiming module's reply in `out` and 1, or 0 if nothing
 * claimed. Forward-declared here, defined after the registry table (which is below). */
static int dispatch_one(Brain *b, const char *clause, char *out, size_t out_size);

/* The strong sequencers are KB knowledge (`strong_sequencer/1` in
 * kb/core/lexicon.p0), read once and shared by every consumer in this file.
 * The engine keeps only the MECHANICS: a strong sequencer separates sub-goals
 * when it stands between spaces, and the ";" separator is punctuation, not
 * vocabulary, so it stays here. This replaced two divergent C copies. */
#define SEQ_MAX 64

static size_t load_strong_sequencers(Brain *b, char out[][KB_TERM_LEN + 4]) {
    char raw[SEQ_MAX][KB_TERM_LEN];
    const char *var[] = {NULL};
    size_t n = b && b->kb ? kb_match(b->kb, "strong_sequencer", var, 1, raw, SEQ_MAX) : 0;
    size_t k = 0;
    for (size_t i = 0; i < n && k < SEQ_MAX - 1; i++) {
        const char *w = kb_dequote(raw[i]);
        if (!w || !*w) continue;
        snprintf(out[k++], KB_TERM_LEN + 4, " %s ", w);
    }
    snprintf(out[k++], KB_TERM_LEN + 4, "; ");   /* punctuation: mechanics */
    return k;
}

/* gen207: execute an ARTICULATED, multi-step coding request. Splits `raw` into
 * ordered clauses on STRONG sequencers (",and then", "after that", "also", "then",
 * "e poi", ...) — never the weak " and " that lives inside "a and b" — then runs each
 * clause through compose/verify or eval, THREADING the functions composed earlier so
 * a later "use it to compute NAME(x,y)" evaluates the real artifact. Every step is
 * oracle-checked; the reply is a numbered, grounded transcript. */
static int compose_plan(Brain *b, const char *raw, char *out, size_t out_size) {
    char seq[SEQ_MAX][KB_TERM_LEN + 4];
    size_t nseq = load_strong_sequencers(b, seq);
    char fill[SEQ_MAX][KB_TERM_LEN];
    const char *fillvar[] = {NULL};
    size_t nfill = b->kb ? kb_match(b->kb, "clause_filler", fillvar, 1, fill, SEQ_MAX) : 0;
    size_t n = strlen(raw);
    static char lowall[2048];
    size_t lim = n < sizeof lowall - 1 ? n : sizeof lowall - 1;
    for (size_t i = 0; i < lim; i++) lowall[i] = (char)tolower((unsigned char)raw[i]);
    lowall[lim] = '\0';

    struct { size_t a, b; } cr[10]; size_t ncl = 0, from = 0;
    for (size_t i = 0; i < lim && ncl < 9; ) {
        size_t mlen = 0;
        for (size_t s = 0; s < nseq; s++) {
            size_t sl = strlen(seq[s]);
            if (i + sl <= lim && strncmp(lowall + i, seq[s], sl) == 0) { mlen = sl; break; }
        }
        if (mlen) { cr[ncl].a = from; cr[ncl].b = i; ncl++; i += mlen; from = i; }
        else i++;
    }
    cr[ncl].a = from; cr[ncl].b = n; ncl++;

    struct { char name[64]; char src[256]; } fns[10]; size_t nf = 0;
    char acc[4096]; size_t off = 0, step = 0;

    for (size_t c = 0; c < ncl; c++) {
        char cl[512];
        size_t l = cr[c].b - cr[c].a; if (l >= sizeof cl) l = sizeof cl - 1;
        memcpy(cl, raw + cr[c].a, l); cl[l] = '\0';

        /* drop leading filler ("and", "also", "use", "it", "to", "please", ...). */
        char *clause = cl;
        for (;;) {
            while (*clause==' ' || *clause==',' || *clause=='.') clause++;
            /* Filler words are `clause_filler/1` in lexicon.p0; the trailing
             * comma variant is punctuation and stays mechanics here. */
            int cut = 0;
            for (size_t f = 0; f < nfill; f++) {
                const char *fw = kb_dequote(fill[f]);
                size_t fl = strlen(fw);
                if (ci_prefix(clause, fw) &&
                    (clause[fl]==' ' || clause[fl]==',' || clause[fl]=='\0'))
                    { clause += fl; cut = 1; break; }
            }
            if (!cut) break;
        }
        if (!*clause) continue;

        char cllow[512];
        size_t cl2 = strlen(clause); if (cl2 >= sizeof cllow) cl2 = sizeof cllow - 1;
        for (size_t i = 0; i < cl2; i++) cllow[i] = (char)tolower((unsigned char)clause[i]);
        cllow[cl2] = '\0';

        step++;
        char piece[760] = {0};
        int is_compose = (kb_cue_match(b, "60_agent_tools_chain1051", cllow)) &&
                         (kb_cue_match(b, "60_agent_tools_chain1053", cllow));
        int is_eval = kb_cue_match(b, "60_agent_tools_chain1055", cllow);

        if (is_compose) {
            char nm[64], src[256], note[200];
            int r = compose_one(b, clause, cllow, nm, sizeof nm, src, sizeof src, note, sizeof note);
            if (r == 1) {
                if (nf < 10) { snprintf(fns[nf].name,64,"%s",nm); snprintf(fns[nf].src,256,"%s",src); nf++; }
                snprintf(piece, sizeof piece, "%zu) %s  /* %s */", step, src, note);
            } else if (r == -1) {
                snprintf(piece, sizeof piece, "%zu) %s  /* %s */", step, src, note);
            } else {
                snprintf(piece, sizeof piece, "%zu) %s", step, note);
            }
        } else if (is_eval) {
            char nm[64] = ""; long av[8]; size_t ac = 0;
            if (parse_int_call(clause, nm, sizeof nm, av, &ac, 8)) {
                const char *src = NULL;
                for (size_t i = 0; i < nf; i++) if (strcmp(fns[i].name, nm)==0) { src = fns[i].src; break; }
                if (!src && nf > 0) src = fns[nf-1].src;   /* "use it" -> last artifact */
                long val;
                if (src && code_eval(src, nm, av, ac, &val)) {
                    size_t o = (size_t)snprintf(piece, sizeof piece, "%zu) %s(", step, nm);
                    for (size_t i = 0; i < ac && o < sizeof piece; i++)
                        o += (size_t)snprintf(piece+o, sizeof piece-o, "%s%ld", i?",":"", av[i]);
                    if (o < sizeof piece) snprintf(piece+o, sizeof piece-o, ") = %ld", val);
                } else {
                    snprintf(piece, sizeof piece, "%zu) (can't evaluate %s — compose it first)", step, nm);
                }
            } else {
                { 
                  char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", step);
  const KbResponseSlot _rs[] = { { "step", _v0 } };
                  kb_term_say(b, "x_no_concrete_call_to_evaluate_here", _rs, 1, piece, sizeof piece); }
            }
        } else {
            /* gen208: not a compose/eval clause — dispatch it through the registry
             * (skipping compose, no re-entry) so e.g. a "tell me about X" recall is
             * answered for real by mod_learn, rather than stubbed as "(not yet)". */
            char rep[640] = {0};
            if (dispatch_one(b, clause, rep, sizeof rep) && rep[0])
                snprintf(piece, sizeof piece, "%zu) %s", step, rep);
            else
                snprintf(piece, sizeof piece, "%zu) (not yet: %.50s)", step, clause);
        }

        int wrote = snprintf(acc+off, off < sizeof acc ? sizeof acc - off : 0,
                             "%s%s", off ? "  " : "", piece);
        if (wrote < 0 || (size_t)wrote >= sizeof acc - off) break;
        off += (size_t)wrote;
    }

    put(acc, out, out_size);
    store_proof(b, "articulated code plan: split into ordered sub-goals, each composed/evaluated and oracle-checked");
    return 1;
}

/* ── MODULE REVIEW — compose ──────────────────── rivista: gen502, 2026-09-04
 *   maturita'   transitional          diritto   FALLBACK (retrocesso)
 *
 *   L1 addestrabile   parziale: le sue soglie di segmentazione non si ritirano
 *                     parlando, quindi una pretesa sbagliata non si corregge
 *   L2 kb_first       si': 0 parole compilate, 7 letture dalla KB
 *   L3 universale     NO: 0 usi del frame del turno
 *
 *   PERCHE' E' RETROCESSO. Su un prompt di 864 byte che chiedeva di scrivere
 *   un file C (tests/challenge match0, run gen502-m0b) ha rivendicato il turno,
 *   l'ha spezzato in segmenti indipendenti e li ha dati a chi li prendeva: una
 *   storia sul `Makefile` personificato piu' l'imperativo finale del compito
 *   asserito come fatto. La pretesa non e' fondata sulla LETTURA del turno ma
 *   sulla sua SEGMENTABILITA' — e ogni prosa lunga e' segmentabile.
 *
 *   Comporre risposte altrui e' legittimo quando nessun altro sa rispondere per
 *   intero: e' esattamente cio' che significa ultima risorsa. Per risalire a
 *   `primary` serve L3 — pretendere sul frame, non sulla lunghezza.
 *
 *   Mantra #21; proiezione in kb/core/module-review.p0. */
static int mod_compose(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    /* gen431: se la richiesta nomina un testo che non e' stato allegato, non si
     * compone niente — si chiede il testo. Comporre su un referente assente
     * produceva sei paragrafi che non nominano mai la cosa richiesta, che e' la
     * classe peggiore dei cento fallimenti: sembra una risposta e non lo e'. */
    {
        char kind_[64];
        if (p0_unattached_kind(b, norm, raw, kind_, sizeof kind_)) return 0;
    }

    (void)norm;
    if (!b) return 0;
    const char *en = getenv("PARROT0_TOOLS");
    if (!en || strcmp(en, "1") != 0) return 0;
    size_t rl = strlen(raw);
    if (rl == 0 || rl >= 2000) return 0;
    if (!b->kb) return 0;

    char low[2048];
    size_t lim = rl < sizeof low - 1 ? rl : sizeof low - 1;
    for (size_t i = 0; i < lim; i++) low[i] = (char)tolower((unsigned char)raw[i]);
    low[lim] = '\0';

    if (kb_cue_match(b, "handshake_problem", low)) return 0;
    /* TODO(kb-first, gen489) — ⛔ CATENA COMPILATA: QUESTA CONGIUNZIONE NON E' CONOSCENZA.
     * Le condizioni qui sotto sono legate da `&&`/`||` nel C. Anche quando ogni
     * singola condizione legge la KB, l'INSIEME — quali condizioni, quante, in
     * che ordine, con quale polarita' — resta compilato: a runtime si puo'
     * insegnare un MEMBRO di una classe che esiste, mai una FORMA nuova. Finche'
     * la catena e' qui, l'insieme delle forme che parrot0 riconosce e' CHIUSO e
     * nessuna lezione lo apre — F., 2026-09-03: «e' la catena di && che deve
     * diventare essa stessa una regola nella KB».
     * Forma di arrivo: `turn_pattern(Forma, cue|not_cue|word|text, Arg)` piu'
     * `turn_pattern_intent(Forma, Intento)` — il motore generico e la spiegazione
     * stanno in `src/brain/00-lex.c` sopra `p0_turn_pattern_holds`, l'esempio
     * lavorato in `tests/p0t/language/taught_turn_form.p0t`. Vedi mantra #19. */
    if (kb_cue_match(b, "probability_draw", low) &&
        kb_cue_match(b, "probability_at_least", low)) return 0;

    /* gen328 (TODO.md P3): TEACH an algorithm from a page of steps —
     *   "learn sinking sort from these steps: repeat passes over the list;
     *    compare each adjacent pair; swap them if they are out of order"
     * The schema is INDUCED from what the steps do (never from the name), so a
     * page for an algorithm nobody curated becomes buildable. */
    int teach = (kb_cue_match(b, "60_agent_tools_chain1129", low)) &&
                (kb_cue_match(b, "60_agent_tools_chain1130", low)) && strchr(raw, ':');

    int want = (kb_cue_match(b, "60_agent_tools_chain1132", low)) &&
               (kb_cue_match(b, "60_agent_tools_chain1134", low));
    if (!want && !teach) return 0;

    /* Lazily load the generative substrate into the REFLECTIVE layer (never
     * persisted, filtered from introspection) the first time the composer runs, so
     * the boot KB and ordinary conversation stay untouched while the knowledge is
     * still KB-first (a file, queried) rather than a C table. */
    if (!b->compose_kb_loaded) {
        kb_set_origin(b->kb, KB_REFLECTIVE);
        kb_load(b->kb, "kb/experts/programming/compose.p0");
        kb_load(b->kb, "kb/experts/programming/algo_steps.p0"); /* gen209 (B1) */
        kb_set_origin(b->kb, KB_SESSION);
        b->compose_kb_loaded = 1;
    }

    if (teach) {
        /* the name sits between "learn" and "from"; the steps after the colon */
        size_t teach_len = 0;
        const char *ln = tool_surface_find(b, low, "teach_prefix", &teach_len);
        if (!ln) return 0;
        const char *nstart = ln + teach_len;
        while (*nstart == ' ') nstart++;
        size_t delimiter_len = 0;
        const char *nend = tool_surface_find(b, nstart, "teach_source_delimiter",
                                              &delimiter_len);
        const char *colon = strchr(raw, ':');
        if (!nend || !colon || nend <= nstart) return 0;

        char key[KB_TERM_LEN]; size_t k = 0;
        for (const char *p = nstart; p < nend && k + 1 < sizeof key; p++)
            key[k++] = (*p == ' ') ? '_' : *p;
        key[k] = '\0';
        if (k == 0) return 0;

        char shape[64] = "", missing[64] = "";
        if (!induce_shape_from_steps(b, colon + 1, shape, sizeof shape,
                                     missing, sizeof missing)) {
            /* No signature is covered. Say what was missing and refuse to invent
             * a schema — a schema nobody can build from is worse than none. */
            { 
              char _v0[48]; snprintf(_v0, sizeof _v0, "%s", *missing ? ": I found no step that means " : "");
              char _v1[48]; snprintf(_v1, sizeof _v1, "%s", *missing ? missing : "");
  const KbResponseSlot _rs[] = { { "means", _v0 }, { "missing", _v1 } };
              kb_term_say(b, "i_read_those_steps_but_they_do_not_describe", _rs, 2, out, out_size); }
            return 1;
        }
        const char *fa[2] = { key, shape };
        kb_set_origin(b->kb, KB_SESSION);
        kb_assert(b->kb, "algo_shape", fa, 2);
        /* provenance: this schema was LEARNED, not curated. */
        const char *fp[2] = { key, "taught_from_steps" };
        kb_assert(b->kb, "algo_source", fp, 2);
        {   const KbResponseSlot _rs[] = { { "shape", shape }, { "key", key } };
          kb_term_say(b, "steps_shape_learned", _rs, 2, out, out_size); }
        return 1;
    }

    /* gen207: if the request is ARTICULATED (a strong sequencer chains several
     * sub-goals), run the multi-step planner; otherwise compose the single function. */
    char seq[SEQ_MAX][KB_TERM_LEN + 4];
    size_t nseq = load_strong_sequencers(b, seq);
    for (size_t s = 0; s < nseq; s++)
        if (strstr(low, seq[s])) return compose_plan(b, raw, out, out_size);

    /* single compose: Knowledge (KB) -> Synthesis (structure) -> Oracle (code_eval). */
    char name[64], src[256], note[200];
    int r = compose_one(b, raw, low, name, sizeof name, src, sizeof src, note, sizeof note);
    char msg[700];
    if (r == 1)        snprintf(msg, sizeof msg, "%s  /* %s */", src, note);
    else if (r == -1)
        kb_term_say(b, "composed_with_caveat", (const KbResponseSlot[]){
                        { "name", src }, { "note", note } }, 2, msg, sizeof msg);
    else               snprintf(msg, sizeof msg, "%s", note);
    put(msg, out, out_size);
    char proof[320];
    { 
      char _v1[48]; snprintf(_v1, sizeof _v1, "%s", r==1?"verified":"unverified/refused");
  const KbResponseSlot _rs[] = { { "name", name }, { "refused", _v1 } };
      kb_term_say(b, "composed_x_via_code_eval_oracle_x", _rs, 2, proof, sizeof proof); }
    store_proof(b, proof);
    return 1;
}

/* gen116 (rung 19): the autonomous ACT-LOOP — parrot0's first perceive ->
 * decide -> act -> observe cycle, and the seed of agency. gen115 made a single
 * tool call; this REPEATS one to pursue a goal. Given a start value, an
 * operation, and a stop condition ("start at 3, double until you reach 50") the
 * agent loops: it OBSERVES the current value, DECIDES whether the goal is met,
 * and if not it ACTS by applying the operation through the arithmetic oracle,
 * then observes the new value — and repeats until the goal holds. The trajectory
 * and the step count are PRODUCED by running the loop, never a closed-form
 * formula, so held-out start/op/target transfer. A bounded step cap (and a
 * no-progress guard) keep an unreachable goal honest ("can't reach ...") instead
 * of spinning forever. This stands on gen115's reason/act seam and turns it into
 * the structural precondition for an autonomous agent (ladder rung 19). */
enum agent_cmp { AGT_GE, AGT_GT, AGT_LE, AGT_LT };

static int agent_goal_met(double cur, double t, enum agent_cmp cmp) {
    switch (cmp) {
        case AGT_GE: return cur >= t - 1e-9;
        case AGT_GT: return cur >  t + 1e-9;
        case AGT_LE: return cur <= t + 1e-9;
        case AGT_LT: return cur <  t - 1e-9;
    }
    return 0;
}

/* gen117 (rung 19, deeper): one step of an act-loop is no longer a FIXED
 * operation — it is CHOSEN by observing the state. A single branch clause ("if
 * it is even, halve it" / "triple it and add 1") is parsed into a short sequence
 * of operations the agent applies in order when that branch is taken. */
typedef struct { char op; double k; } AgentOp;

static size_t parse_branch_ops(Brain *b, const char *clause, AgentOp *ops, size_t max) {
    if (!b || !b->kb || !clause || !ops || max == 0) return 0;
    char buf[256];
    snprintf(buf, sizeof buf, "%s", clause);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    size_t n = 0;
    char pend = 0;                       /* an operator still awaiting its operand */
    for (size_t i = 0; i < nw && n < max; i++) {
        char *t = strip_edge_punct(w[i]);
        double v;
        if (pend && parse_value(t, &v)) { ops[n].op = pend; ops[n].k = v; n++; pend = 0; continue; }
        char quoted[KB_TERM_LEN];
        snprintf(quoted, sizeof quoted, "\"%s\"", t);
        const char *q[3] = { quoted, NULL, NULL };
        char op[1][KB_TERM_LEN];
        if (kb_match(b->kb, "agent_branch_step", q, 3, op, 1) != 1)
            continue;
        char opquoted[KB_TERM_LEN];
        snprintf(opquoted, sizeof opquoted, "%s", op[0]);
        const char *fq[3] = { quoted, opquoted, NULL };
        char factor[1][KB_TERM_LEN];
        if (kb_match(b->kb, "agent_branch_step", fq, 3, factor, 1) != 1)
            continue;
        char opterm[KB_TERM_LEN];
        snprintf(opterm, sizeof opterm, "%s", kb_dequote(op[0]));
        double k = 0;
        parse_value(kb_dequote(factor[0]), &k);
        if (k == 0 && strlen(opterm) == 1) pend = opterm[0];
        else if (n < max && strlen(opterm) == 1) {
            ops[n].op = opterm[0]; ops[n].k = k; n++;
        }
    }
    return n;
}

/* Copy [from, end) into out, NUL-terminated and length-clamped. */
static void agent_slice(const char *from, const char *end, char *out, size_t out_size) {
    size_t len = (size_t)(end - from);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, from, len); out[len] = '\0';
}

static const char *agent_parity_surface(Brain *b, const char *text,
                                         const char *kind) {
    if (!b || !b->kb || !text || !kind) return NULL;
    const char *q[] = { kind, NULL };
    char surfaces[8][KB_TERM_LEN];
    size_t n = kb_match(b->kb, "agent_parity_marker", q, 2, surfaces, 8);
    const char *best = NULL;
    for (size_t i = 0; i < n; i++) {
        const char *surface = kb_dequote(surfaces[i]);
        const char *hit = surface ? strstr(text, surface) : NULL;
        if (hit && (!best || hit < best)) best = hit;
    }
    return best;
}

static int mod_agent(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Must look like an iterate-until-goal instruction: a start anchor and an
     * "until" boundary. Without both, this is not our turn. */
    int has_start = kb_cue_match(b, "60_agent_tools_chain1286", low);
    char *until = strstr(low, "until");
    if (!until) { char *u2 = strstr(low, "finch"); if (u2) until = u2; }
    if (!until) { char *u3 = strstr(low, "fino a"); if (u3) until = u3; }
    if (!has_start || !until) return 0;

    /* gen117: the BRANCHING act-loop. If the instruction carries a parity
     * condition ("if it is even ... if it is odd ..."), the action each step is
     * not fixed but DECIDED by observing the current value's parity, then the
     * chosen branch's operation sequence is applied. The classic witness is the
     * Collatz process — halve when even, triple-and-add-one when odd, until it
     * reaches 1 — whose step count is famously unpredictable, so the answer can
     * only come from actually running the loop, never a formula. */
    const char *ce = agent_parity_surface(b, low, "even");
    const char *co = agent_parity_surface(b, low, "odd");
    if (ce && co) {
        {
            /* Each branch clause runs from its parity marker to the next marker
             * (or to the goal), so one branch never bleeds into the next — works
             * the same for "if even ... if odd" and "se pari ... se dispari". */
            const char *end_e = until, *end_o = until;
            if (co > ce && co < end_e) end_e = co;
            if (ce > co && ce < end_o) end_o = ce;
            char ec[256], oc[256];
            agent_slice(ce, end_e, ec, sizeof ec);
            agent_slice(co, end_o, oc, sizeof oc);
            AgentOp even_ops[4], odd_ops[4];
            size_t ne = parse_branch_ops(b, ec, even_ops, 4);
            size_t no = parse_branch_ops(b, oc, odd_ops, 4);

            /* start value: the first number before the first branch marker. */
            const char *first = (ce < co) ? ce : co;
            char head[256];
            size_t hl = (size_t)(first - low);
            if (hl >= sizeof head) hl = sizeof head - 1;
            memcpy(head, low, hl); head[hl] = '\0';
            char hb[256]; snprintf(hb, sizeof hb, "%s", head);
            char *hw[64]; size_t hnw = split_words(hb, hw, 64);
            double snums[8]; size_t sn = collect_numbers(hw, hnw, snums, 8);

            /* target: the first number in the goal clause (after "until"). */
            char gb[256]; snprintf(gb, sizeof gb, "%s", until);
            char *gw[64]; size_t gnw = split_words(gb, gw, 64);
            double gnums[8]; size_t gn = collect_numbers(gw, gnw, gnums, 8);

            if (ne > 0 && no > 0 && sn > 0 && gn > 0) {
                double cur = snums[0], target = gnums[0];
                long steps = 0; const long CAP = 1000000;
                char traj[480], nb[64];
                format_num(cur, nb, sizeof nb);
                size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
                int truncated = 0;
                while (!(cur >= target - 1e-9 && cur <= target + 1e-9) &&
                       steps < CAP) {
                    long long iv = (long long)cur;
                    int even = ((double)iv == cur) && (iv % 2 == 0);
                    const AgentOp *seq = even ? even_ops : odd_ops;
                    size_t nseq = even ? ne : no;
                    double before = cur;
                    for (size_t s = 0; s < nseq; s++) {
                        int ok; char o[2] = { seq[s].op, 0 };
                        double nx = apply_arith_op(b, o, cur, seq[s].k, &ok);
                        if (ok) cur = nx;
                    }
                    if (cur == before) break;       /* no-progress guard */
                    steps++;
                    format_num(cur, nb, sizeof nb);
                    if (steps <= 8)
                        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
                    else if (!truncated) {
                        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> ...");
                        truncated = 1;
                    }
                }
                if (truncated)
                    snprintf(traj + to, sizeof traj - to, " -> %s", nb);

                char msg[640];
                if (cur >= target - 1e-9 && cur <= target + 1e-9) {
                    format_num(target, nb, sizeof nb);
                    { 
                      char _v1[48]; snprintf(_v1, sizeof _v1, "%ld", steps);
                      char _v2[48]; snprintf(_v2, sizeof _v2, "%s", steps == 1 ? "" : "s");
  const KbResponseSlot _rs[] = { { "nb", nb }, { "steps", _v1 }, { "s", _v2 }, { "traj", traj } };
                      kb_term_say(b, "reached_x_after_x_stepx_x", _rs, 4, msg, sizeof msg); }
                } else {
                    { 
                      char _v0[48]; snprintf(_v0, sizeof _v0, "%ld", CAP);
  const KbResponseSlot _rs[] = { { "CAP", _v0 } };
                      kb_term_say(b, "it_didn_t_settle_within_x_steps", _rs, 1, msg, sizeof msg); }
                }
                put(msg, out, out_size);
                char proof[512];
                { 
                  char _v0[48]; snprintf(_v0, sizeof _v0, "%ld", steps);
  const KbResponseSlot _rs[] = { { "steps", _v0 }, { "traj", traj } };
                  kb_term_say(b, "branching_act_loop_x_observed_steps_x", _rs, 2, proof, sizeof proof); }
                store_proof(b, proof);
                return 1;
            }
        }
    }

    /* Split into the left clause (start + operation) and the right clause
     * (the goal), then read the numbers out of each. */
    size_t cut = (size_t)(until - low);
    char left[256], right[256];
    snprintf(left, sizeof left, "%.*s", (int)cut, low);
    snprintf(right, sizeof right, "%s", until);

    double lnums[8], rnums[8];
    char lb[256], rb[256];
    snprintf(lb, sizeof lb, "%s", left);
    snprintf(rb, sizeof rb, "%s", right);
    char *lw[64], *rw[64];
    size_t lnw = split_words(lb, lw, 64);
    size_t rnw = split_words(rb, rw, 64);
    size_t ln = collect_numbers(lw, lnw, lnums, 8);
    size_t rn = collect_numbers(rw, rnw, rnums, 8);
    if (ln == 0 || rn == 0) return 0;

    double start = lnums[0];
    double target = rnums[0];

    /* Read the operation from the left clause. Word-named operators (double,
     * halve, triple) carry their operand; the rest take the second number. */
    const char *op = NULL; double k = 0;
    if (kb_cue_match(b, "60_agent_tools_chain1407", left))      { op = "*"; k = 2; }
    else if (kb_cue_match(b, "60_agent_tools_chain1408", left))  { op = "*"; k = 3; }
    else if (kb_cue_match(b, "60_agent_tools_chain1409", left))   { op = "/"; k = 2; }
    else if (ln >= 2) {
        k = lnums[1];
        if (kb_cue_match(b, "60_agent_tools_chain1412", left))            op = "+";
        else if (kb_cue_match(b, "60_agent_tools_chain1414", left))    op = "-";
        else if (kb_cue_match(b, "60_agent_tools_chain1416", left))                       op = "*";
        else if (kb_cue_match(b, "60_agent_tools_chain1418", left))                          op = "/";
    }
    if (!op) return 0;

    /* Read the goal comparator from the right clause; if unstated, infer it from
     * whether the operation grows or shrinks the value. */
    enum agent_cmp cmp;
    int have_cmp = 1;
    if (kb_cue_match(b, "60_agent_tools_chain1427", right))            cmp = AGT_GT;
    else if (kb_cue_match(b, "60_agent_tools_chain1430", right))   cmp = AGT_GE;
    else if (kb_cue_match(b, "60_agent_tools_chain1432", right)) cmp = AGT_LE;
    else if (kb_cue_match(b, "60_agent_tools_chain1433", right))      cmp = AGT_LT;
    else have_cmp = 0;
    if (!have_cmp) {
        int grows = (strcmp(op, "+") == 0) ||
                    (strcmp(op, "*") == 0 && k > 1);
        cmp = grows ? AGT_GE : AGT_LE;
    }

    /* Run the loop. Build a trajectory string as we go (head, then an ellipsis
     * for long runs, then the tail). The step count is the number of real
     * oracle calls — the agent's actions. */
    const long CAP = 100000;
    double cur = start;
    long steps = 0;
    char traj[480];
    char nb[64];
    format_num(cur, nb, sizeof nb);
    size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
    int truncated = 0;

    while (!agent_goal_met(cur, target, cmp) && steps < CAP) {
        int ok;
        double next = apply_arith_op(b, op, cur, k, &ok);
        if (!ok) break;
        if (next == cur) break;             /* no-progress guard */
        cur = next;
        steps++;
        format_num(cur, nb, sizeof nb);
        if (steps <= 10) {
            to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
        } else if (!truncated) {
            to += (size_t)snprintf(traj + to, sizeof traj - to, " -> ...");
            truncated = 1;
        }
    }
    if (truncated) /* always show where it ended up */
        snprintf(traj + to, sizeof traj - to, " -> %s", nb);

    char msg[640];
    if (agent_goal_met(cur, target, cmp)) {
        format_num(cur, nb, sizeof nb);
        { 
          char _v1[48]; snprintf(_v1, sizeof _v1, "%ld", steps);
          char _v2[48]; snprintf(_v2, sizeof _v2, "%s", steps == 1 ? "" : "s");
  const KbResponseSlot _rs[] = { { "nb", nb }, { "steps", _v1 }, { "s", _v2 }, { "traj", traj } };
          kb_term_say(b, "reached_x_after_x_stepx_x", _rs, 4, msg, sizeof msg); }
    } else {
        char sb[64], tb[64];
        format_num(start, sb, sizeof sb);
        format_num(target, tb, sizeof tb);
        {   const KbResponseSlot _rs[] = { { "sb", sb }, { "tb", tb } };
          kb_term_say(b, "starting_from_x_that_step_never_reaches_x_th", _rs, 2, msg, sizeof msg); }
    }
    put(msg, out, out_size);

    char proof[512];
    { 
      char _v0[48]; snprintf(_v0, sizeof _v0, "%ld", steps);
      char _v1[48]; snprintf(_v1, sizeof _v1, "%s", steps == 1 ? "" : "s");
  const KbResponseSlot _rs[] = { { "steps", _v0 }, { "s", _v1 }, { "traj", traj } };
      kb_term_say(b, "act_loop_x_oracle_callx_x", _rs, 3, proof, sizeof proof); }
    store_proof(b, proof);
    return 1;
}

/* gen118 (rung 19 + L10, deeper): rule INDUCTION from observed transitions. In
 * gen117 the agent was TOLD the law ("if even halve, if odd triple+1"); here it
 * DISCOVERS it. Shown a handful of integer transitions ("6 -> 3, 3 -> 10,
 * 10 -> 5, 5 -> 16"), it fits a hypothesis — first a single global operation,
 * else a PARITY-SPLIT rule — and then applies it: continue a sequence, give the
 * next value, or state the rule. The striking witness is that the SAME data that
 * defines the Collatz step lets parrot0 re-derive Collatz with nobody telling it,
 * then run it. This is program induction: recovering a generative function from
 * examples, the core of in-context learning, in deterministic C. */
typedef struct {
    int    conditional;          /* 0 = one global op sequence; 1 = parity split */
    AgentOp even_ops[3]; size_t ne;
    AgentOp odd_ops[3];  size_t no;
} InducedRule;

/* Fit one class of (in -> out) integer pairs to a short operation sequence.
 * Tries, in order: constant, add/subtract, multiply, divide, then affine a*n+b.
 * Returns 1 and writes ops on success. The identity map is rejected as
 * uninformative (it would make a sequence never progress). */
static int fit_class(const long *in, const long *out, size_t n,
                     AgentOp *ops, size_t *nops) {
    if (n == 0) return 0;
    /* constant: every output identical (independent of input). */
    int all_const = 1;
    for (size_t i = 1; i < n; i++) if (out[i] != out[0]) { all_const = 0; break; }
    if (all_const && n >= 2) {
        ops[0].op='*'; ops[0].k=0; ops[1].op='+'; ops[1].k=(double)out[0]; *nops=2; return 1;
    }
    /* add/subtract a constant. */
    long d = out[0] - in[0]; int add_ok = (d != 0);
    for (size_t i = 1; i < n && add_ok; i++) if (out[i]-in[i] != d) add_ok = 0;
    if (add_ok) { ops[0].op='+'; ops[0].k=(double)d; *nops=1; return 1; }
    /* multiply by a constant. */
    if (in[0] != 0 && out[0] % in[0] == 0) {
        long r = out[0]/in[0]; int mul_ok = (r != 1);
        for (size_t i = 0; i < n && mul_ok; i++)
            if (in[i]==0 || out[i] != r*in[i]) mul_ok = 0;
        if (mul_ok) { ops[0].op='*'; ops[0].k=(double)r; *nops=1; return 1; }
    }
    /* divide by a constant. */
    if (out[0] != 0 && in[0] % out[0] == 0) {
        long dv = in[0]/out[0]; int div_ok = (dv != 1);
        for (size_t i = 0; i < n && div_ok; i++)
            if (out[i]==0 || in[i] != dv*out[i]) div_ok = 0;
        if (div_ok) { ops[0].op='/'; ops[0].k=(double)dv; *nops=1; return 1; }
    }
    /* affine a*n+b, needs two distinct inputs; a integer. */
    for (size_t j = 1; j < n; j++) {
        if (in[j] == in[0]) continue;
        long num = out[j]-out[0], den = in[j]-in[0];
        if (num % den != 0) break;
        long a = num/den, bb = out[0] - a*in[0];
        int ok = 1;
        for (size_t i = 0; i < n && ok; i++) if (out[i] != a*in[i]+bb) ok = 0;
        if (ok && !(a==1 && bb==0)) {
            size_t k = 0;
            if (a != 1) { ops[k].op='*'; ops[k].k=(double)a; k++; }
            if (bb != 0 || k == 0) { ops[k].op='+'; ops[k].k=(double)bb; k++; }
            *nops = k; return 1;
        }
        break;
    }
    return 0;
}

static double apply_rule(Brain *b, const InducedRule *r, double cur) {
    const AgentOp *seq; size_t n;
    if (r->conditional) {
        long long iv = (long long)cur;
        int even = ((double)iv == cur) && (iv % 2 == 0);
        seq = even ? r->even_ops : r->odd_ops;
        n   = even ? r->ne       : r->no;
    } else { seq = r->even_ops; n = r->ne; }
    for (size_t s = 0; s < n; s++) {
        int ok; char o[2] = { seq[s].op, 0 };
        double nx = apply_arith_op(b, o, cur, seq[s].k, &ok);
        if (ok) cur = nx;
    }
    return cur;
}

/* Render an op sequence as a compact algebraic expression in n. */
static void describe_ops(const AgentOp *ops, size_t n, char *buf, size_t sz) {
    char ka[64], kb[64];
    if (n == 2 && ops[0].op=='*' && ops[0].k==0 && ops[1].op=='+') {
        format_num(ops[1].k, ka, sizeof ka); snprintf(buf, sz, "%s", ka); return;
    }
    if (n == 1 && ops[0].op=='/') {
        format_num(ops[0].k, ka, sizeof ka); snprintf(buf, sz, "n / %s", ka); return;
    }
    if (n == 1 && ops[0].op=='*') {
        format_num(ops[0].k, ka, sizeof ka); snprintf(buf, sz, "%sn", ka); return;
    }
    if (n == 1 && ops[0].op=='+') {
        double k = ops[0].k; format_num(k<0?-k:k, ka, sizeof ka);
        snprintf(buf, sz, "n %s %s", k<0?"-":"+", ka); return;
    }
    if (n == 2 && ops[0].op=='*' && ops[1].op=='+') {
        double k = ops[1].k; format_num(ops[0].k, ka, sizeof ka);
        format_num(k<0?-k:k, kb, sizeof kb);
        snprintf(buf, sz, "%sn %s %s", ka, k<0?"-":"+", kb); return;
    }
    snprintf(buf, sz, "n"); /* identity / unknown shape */
}

/* Fit a rule to (in -> out) pairs and describe it. Returns 1 on success, 0 if
 * nothing in the hypothesis space fits. Shared by gen118 (use a rule) and gen120
 * (test a rule). A conditional rule needs >= 2 examples per parity class so a
 * stray pair can't overfit a spurious branch. */
static int induce_rule(const long *in, const long *out, size_t npair,
                       InducedRule *r, char *rule, size_t rulesz) {
    memset(r, 0, sizeof *r);
    int fitted = 0;
    if (fit_class(in, out, npair, r->even_ops, &r->ne)) {
        r->conditional = 0; fitted = 1;
    } else {
        long ein[16], eout[16], oin[16], oout[16]; size_t en=0, on=0;
        for (size_t i = 0; i < npair && en < 16 && on < 16; i++) {
            if (in[i] % 2 == 0) { ein[en]=in[i]; eout[en]=out[i]; en++; }
            else                { oin[on]=in[i]; oout[on]=out[i]; on++; }
        }
        if (en >= 2 && on >= 2 &&
            fit_class(ein, eout, en, r->even_ops, &r->ne) &&
            fit_class(oin, oout, on, r->odd_ops, &r->no)) {
            r->conditional = 1; fitted = 1;
        }
    }
    if (!fitted) return 0;
    if (r->conditional) {
        char e[64], o[64];
        describe_ops(r->even_ops, r->ne, e, sizeof e);
        describe_ops(r->odd_ops,  r->no, o, sizeof o);
        snprintf(rule, rulesz, "if even: %s; if odd: %s", e, o);
    } else {
        char g[64]; describe_ops(r->even_ops, r->ne, g, sizeof g);
        snprintf(rule, rulesz, "f(n) = %s", g);
    }
    return 1;
}


/* --- module: reqgen (gen267, universal-comprehension §2/§4.2 first slice) ---
 * The request/generate FORM read structurally: an imperative make-verb in one
 * of the first positions plus a non-empty object noun-phrase, optionally
 * closed by a language constraint "in <lang>". The verb class and the
 * language names are intent_cue KB facts (whole-token match, teachable at
 * runtime); only the FORM is C. Registered LATE — right before mod_learn —
 * so it can never steal a turn from a competent module: it catches exactly
 * the turns that used to fall through to the blind wall, and turns them into
 * the doc's INFORMED decline: proof the request was understood (the parsed
 * object quoted back, the language named) plus the honest gap ("no verified
 * schema"), never a fabricated attempt. */

/* defined later in the translation unit (80-code.c) — one TU, forward decls */
static long rulespec_cue_at(Brain *b, const char *intent, const char *seg);
static int rulespec_word_after(Brain *b, const char *seg, long from, int skip_stop,
                               char *w, size_t wsz, long *wstart);

static int reqgen_in_class(Brain *b, const char *intent, const char *w) {
    if (!b || !b->kb || !w || !*w) return 0;
    char cues[64][KB_TERM_LEN];
    const char *q[2] = { intent, NULL };
    size_t n = kb_match(b->kb, "intent_cue", q, 2, cues, 64);
    for (size_t i = 0; i < n; i++) {
        char *p = cues[i];
        size_t l = strlen(p);
        if (l >= 2 && p[0] == '"' && p[l-1] == '"') { p[l-1] = '\0'; p++; }
        if (*p && strcmp(p, w) == 0) return 1;
    }
    return 0;
}

static int mod_reqgen(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb || !raw) return 0;
    /* Read the plainly-normalized RAW (like mod_plan): canonicalize_lang
     * rewrites Italian function words, which would garble the quoted object. */
    char praw[512];
    normalize(raw, praw, sizeof praw);
    char buf[512];
    snprintf(buf, sizeof buf, "%s", praw);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;

    /* A make-verb inside a question is not an imperative.  The old positional
     * shortcut accepted any make_verb among the first three tokens, so the
     * Italian question "cosa fai se ..." became a request to manufacture the
     * words after `fai`.  Which words open questions is KB knowledge; C only
     * enforces the speech-act boundary.  Adding/retracting question_word/1 now
     * changes this guard in the running process. */
    {
        const char *first = strip_edge_punct(w[0]);
        const char *q[] = { first };
        if (*first && kb_query(b->kb, "question_word", q, 1)) return 0;
    }

    /* gen335 (long-conversation): "cosa fai nel tempo libero" / "what do you do for fun"
     * is an experiential smalltalk move, not a produce-this request — an experiential_move
     * marker defers it to mod_smalltalk's honest deflect. Evidence, not a phrase list. */
    /* TODO(kb-first, gen489) — ⛔ CATENA COMPILATA: QUESTA CONGIUNZIONE NON E' CONOSCENZA.
     * Le condizioni qui sotto sono legate da `&&`/`||` nel C. Anche quando ogni
     * singola condizione legge la KB, l'INSIEME — quali condizioni, quante, in
     * che ordine, con quale polarita' — resta compilato: a runtime si puo'
     * insegnare un MEMBRO di una classe che esiste, mai una FORMA nuova. Finche'
     * la catena e' qui, l'insieme delle forme che parrot0 riconosce e' CHIUSO e
     * nessuna lezione lo apre — F., 2026-09-03: «e' la catena di && che deve
     * diventare essa stessa una regola nella KB».
     * Forma di arrivo: `turn_pattern(Forma, cue|not_cue|word|text, Arg)` piu'
     * `turn_pattern_intent(Forma, Intento)` — il motore generico e la spiegazione
     * stanno in `src/brain/00-lex.c` sopra `p0_turn_pattern_holds`, l'esempio
     * lavorato in `tests/p0t/language/taught_turn_form.p0t`. Vedi mantra #19. */
    if (kb_cue_match(b, "experiential_move", buf) ||
        kb_cue_match(b, "experiential_move", raw))
        return 0;

    /* TODO(kb-first, gen489) — ⛔ CATENA COMPILATA: QUESTA CONGIUNZIONE NON E' CONOSCENZA.
     * Le condizioni qui sotto sono legate da `&&`/`||` nel C. Anche quando ogni
     * singola condizione legge la KB, l'INSIEME — quali condizioni, quante, in
     * che ordine, con quale polarita' — resta compilato: a runtime si puo'
     * insegnare un MEMBRO di una classe che esiste, mai una FORMA nuova. Finche'
     * la catena e' qui, l'insieme delle forme che parrot0 riconosce e' CHIUSO e
     * nessuna lezione lo apre — F., 2026-09-03: «e' la catena di && che deve
     * diventare essa stessa una regola nella KB».
     * Forma di arrivo: `turn_pattern(Forma, cue|not_cue|word|text, Arg)` piu'
     * `turn_pattern_intent(Forma, Intento)` — il motore generico e la spiegazione
     * stanno in `src/brain/00-lex.c` sopra `p0_turn_pattern_holds`, l'esempio
     * lavorato in `tests/p0t/language/taught_turn_form.p0t`. Vedi mantra #19. */
    if (kb_cue_match(b, "creative_text_request", buf) ||
        kb_cue_match(b, "creative_text_request", raw))
        return 0;

    /* TODO(kb-first, gen489) — ⛔ CATENA COMPILATA: QUESTA CONGIUNZIONE NON E' CONOSCENZA.
     * Le condizioni qui sotto sono legate da `&&`/`||` nel C. Anche quando ogni
     * singola condizione legge la KB, l'INSIEME — quali condizioni, quante, in
     * che ordine, con quale polarita' — resta compilato: a runtime si puo'
     * insegnare un MEMBRO di una classe che esiste, mai una FORMA nuova. Finche'
     * la catena e' qui, l'insieme delle forme che parrot0 riconosce e' CHIUSO e
     * nessuna lezione lo apre — F., 2026-09-03: «e' la catena di && che deve
     * diventare essa stessa una regola nella KB».
     * Forma di arrivo: `turn_pattern(Forma, cue|not_cue|word|text, Arg)` piu'
     * `turn_pattern_intent(Forma, Intento)` — il motore generico e la spiegazione
     * stanno in `src/brain/00-lex.c` sopra `p0_turn_pattern_holds`, l'esempio
     * lavorato in `tests/p0t/language/taught_turn_form.p0t`. Vedi mantra #19. */
    if (kb_cue_match(b, "translation_request", buf) ||
        kb_cue_match(b, "translation_request", raw))
        return 0;

    /* Uno snippet richiesto per nome: la coppia cue -> template e' interamente
     * conoscenza. `snippet_request(Cue, Template)` dice quale richiesta serve
     * quale modello, quindi uno snippet nuovo domani — in un'altra lingua o per
     * un altro linguaggio di programmazione — e' due righe di KB e nessuna
     * ricompilazione. Il caso del numero primo era il primo membro, cablato per
     * nome; ora e' un membro come gli altri. */
    {
        char pairs[32][KB_TERM_LEN];
        const char *pq[2] = { NULL, NULL };
        size_t npair = kb_match(b->kb, "snippet_request", pq, 2, pairs, 32);
        for (size_t i = 0; i < npair; i++) {
            char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", pairs[i]);
            const char *cue_name = kb_dequote(cb);
            if (!*cue_name) continue;
            if (!kb_cue_match(b, cue_name, buf) && !kb_cue_match(b, cue_name, raw))
                continue;
            char tpl[1][KB_TERM_LEN];
            const char *tq[2] = { pairs[i], NULL };
            if (kb_match(b->kb, "snippet_request", tq, 2, tpl, 1) != 1) continue;
            char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", tpl[0]);
            const char *key = kb_dequote(tb);
            const char *q[] = { key, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "code_template", q, 2, hit, 1) > 0) {
                /* Il codice ha delle RIGHE. Un template lo scrive «\n» perche'
                 * una clausola sta su una riga sola; renderlo alla lettera
                 * stampava «\n» dentro il sorgente e dava uno snippet che non
                 * compila. L'espansione e' meccanica fissa, non vocabolario. */
                char raw_tpl[KB_TERM_LEN], shown[KB_TERM_LEN];
                snprintf(raw_tpl, sizeof raw_tpl, "%s", hit[0]);
                const char *src = kb_dequote(raw_tpl);
                size_t o = 0;
                for (const char *c = src; *c && o + 1 < sizeof shown; c++) {
                    if (c[0] == '\\' && c[1]) {
                        char e = c[1];
                        if (e == 'n')      { shown[o++] = '\n'; c++; continue; }
                        if (e == 't')      { shown[o++] = '\t'; c++; continue; }
                        if (e == '\\')    { shown[o++] = '\\'; c++; continue; }
                    }
                    shown[o++] = *c;
                }
                shown[o] = '\0';
                put(shown, out, out_size);
                store_proof(b, "Rendered a code_template selected by KB request cues.");
                return 1;
            }
        }
    }

    size_t vi = (size_t)-1;                 /* the make-verb, imperative slot */
    for (size_t i = 0; i < nw && i < 3; i++)
        if (reqgen_in_class(b, "make_verb", strip_edge_punct(w[i]))) { vi = i; break; }
    if (vi == (size_t)-1 || vi + 1 >= nw) return 0;

    char lang[16] = "";                     /* trailing "in <lang_name>" */
    size_t end = nw;
    char *lastw = strip_edge_punct(w[nw-1]);
    if (nw >= vi + 3 && lex_class_member(b, "60_agent_tools_lex1746", strip_edge_punct(w[nw-2])) &&
        reqgen_in_class(b, "lang_name", lastw)) {
        snprintf(lang, sizeof lang, "%s", lastw);
        end = nw - 2;
    }

    size_t oi = vi + 1;                     /* the object NP, articles skipped */
    while (oi < end && is_stopword(b, strip_edge_punct(w[oi]))) oi++;
    if (oi >= end) return 0;
    char obj[256] = "";
    size_t o = 0;
    for (size_t i = oi; i < end && o + 1 < sizeof obj; i++)
        o += (size_t)snprintf(obj + o, sizeof obj - o, "%s%s",
                              i > oi ? " " : "", strip_edge_punct(w[i]));
    if (!obj[0]) return 0;

    /* gen270 (universal-comprehension §8, F.'s pupo.txt): the simplest
     * produce-artifact — "create a file named X" / "crea un file chiamato X"
     * really CREATES the (empty) file, under hard guards: plain basename in
     * the working directory, never overwrite (O_EXCL), and only while the KB
     * declares artifact_shape(empty_file, …). The name is taken after a KB
     * name-marker token, or as the object's only dotted token; its case comes
     * from the RAW turn. Grounded: success is re-verified by stat, recorded
     * as session knowledge (artifact/2), and an existing file is an honest
     * refusal, never a silent clobber. */
    {
        int has_file = 0;
        char nlow[80] = "";
        {
            char obuf[256];
            snprintf(obuf, sizeof obuf, "%s", obj);
            char *ow[48];
            size_t on = split_words(obuf, ow, 48);
            size_t mark = (size_t)-1;
            for (size_t i = 0; i < on; i++) {
                if (reqgen_in_class(b, "file_word", ow[i])) has_file = 1;
                if (mark == (size_t)-1 &&
                    reqgen_in_class(b, "name_marker", ow[i])) mark = i;
            }
            if (has_file) {
                if (mark != (size_t)-1 && mark + 1 < on)
                    snprintf(nlow, sizeof nlow, "%s", ow[mark + 1]);
                else
                    for (size_t i = 0; i < on && !nlow[0]; i++)
                        if (strchr(ow[i], '.')) snprintf(nlow, sizeof nlow, "%s", ow[i]);
            }
        }
        if (has_file && nlow[0]) {
            if (!b->compose_kb_loaded) {
                kb_set_origin(b->kb, KB_REFLECTIVE);
                kb_load(b->kb, "kb/experts/programming/compose.p0");
                kb_load(b->kb, "kb/experts/programming/algo_steps.p0");
                kb_set_origin(b->kb, KB_SESSION);
                b->compose_kb_loaded = 1;
            }
            const char *qa[2] = { "empty_file", NULL };
            char ash[1][KB_TERM_LEN];
            if (kb_match(b->kb, "artifact_shape", qa, 2, ash, 1) == 1) {
                char name[80];                    /* original case, from RAW */
                snprintf(name, sizeof name, "%s", nlow);
                int pathy = 0;   /* the user really asked for a PATH, not a name */
                {
                    char rbuf[512];
                    snprintf(rbuf, sizeof rbuf, "%s", raw);
                    char *rw[64];
                    size_t rn = split_words(rbuf, rw, 64);
                    for (size_t i = 0; i < rn; i++) {
                        const char *orig = rw[i];
                        char *t = strip_edge_punct(rw[i]);
                        char tl[80];
                        size_t k = 0;
                        for (const char *c = t; *c && k + 1 < sizeof tl; c++)
                            tl[k++] = (char)tolower((unsigned char)*c);
                        tl[k] = '\0';
                        if (!strcmp(tl, nlow)) {
                            /* the guard must see what was ASKED: a token that
                             * was a path before edge-stripping is refused, not
                             * silently flattened into the working directory */
                            if (strchr(orig, '/') || orig[0] == '.') pathy = 1;
                            snprintf(name, sizeof name, "%s", t);
                            break;
                        }
                    }
                }
                int cr = pathy ? -1 : code_create_empty_file(name);
                if (cr == 1) {
                    struct stat fst;
                    if (stat(name, &fst) == 0 && fst.st_size == 0) {
                        {   const KbResponseSlot _rs[] = { { "name", name } };
                          kb_term_say(b, "created_the_empty_file_x_in_the_working_dire", _rs, 1, out, out_size); }
                        note_artifact(b, "file", name);
                        store_proof(b, out);
                        return 1;
                    }
                }
                if (cr == 0) {
                    {   const KbResponseSlot _rs[] = { { "name", name } };
                      kb_term_say(b, "i_understood_create_the_file_x_but_a_file_wi", _rs, 1, out, out_size); }
                    store_proof(b, out);
                    return 1;
                }
                /* invalid name (paths, dotfiles, odd characters): fall through
                 * to the informed decline below — honest, no side effect. */
            }
        }
    }

    /* gen268 (universal-comprehension §8): before declining, try the ONE
     * program schema the KB declares (program_shape print_message). The
     * message parameter comes either from WORLD KNOWLEDGE — a program_output/2
     * fact naming a conventional artifact ("hello world" carries its own
     * canonical text) — or from the request itself ("… that prints <text>",
     * the rule_print cue class). Only C is emittable; the candidate is
     * DISPOSED by really running it (stdout must be exactly the message).
     * Any missing piece falls through to the informed decline below. */
    if (!lang[0] || lex_class_member(b, "60_agent_tools_lex1865", lang)) {
        if (!b->compose_kb_loaded) {
            kb_set_origin(b->kb, KB_REFLECTIVE);
            kb_load(b->kb, "kb/experts/programming/compose.p0");
            kb_load(b->kb, "kb/experts/programming/algo_steps.p0");
            kb_set_origin(b->kb, KB_SESSION);
            b->compose_kb_loaded = 1;
        }
        const char *qs[2] = { "print_message", NULL };
        char shp[1][KB_TERM_LEN];
        if (kb_match(b->kb, "program_shape", qs, 2, shp, 1) == 1) {
            char msg[128] = "";
            char keys[8][KB_TERM_LEN];
            const char *qk[2] = { NULL, NULL };
            size_t nk = kb_match(b->kb, "program_output", qk, 2, keys, 8);
            for (size_t i = 0; i < nk && !msg[0]; i++) {
                char spaced[KB_TERM_LEN];
                size_t m = 0;
                for (const char *c = keys[i]; *c && m + 1 < sizeof spaced; c++)
                    spaced[m++] = (*c == '_') ? ' ' : *c;
                spaced[m] = '\0';
                if (m && strstr(obj, spaced)) {
                    const char *qv[2] = { keys[i], NULL };
                    char val[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "program_output", qv, 2, val, 1) == 1) {
                        plan_unquote(val[0]);
                        snprintf(msg, sizeof msg, "%s", val[0]);
                    }
                }
            }
            if (!msg[0]) {
                long pc = rulespec_cue_at(b, "rule_print", obj);
                if (pc >= 0) {
                    char fw[64];
                    long ws = -1;
                    if (rulespec_word_after(b, obj, pc, 1, fw, sizeof fw, &ws) &&
                        ws >= 0)
                        snprintf(msg, sizeof msg, "%s", obj + ws);
                }
            }
            if (msg[0]) {
                char src[512], err[256];
                if (code_synth_print_program(msg, src, sizeof src) &&
                    code_check_print_program(src, msg, err, sizeof err) == 1) {
                    /* gen269: multi-line reply, code in a markdown fence, so
                     * clients (opencode, pi) render it as real code. */
                    {   const KbResponseSlot _rs[] = { { "msg", msg }, { "src", src } };
                      kb_term_say(b, "verified_by_execution_it_prints_x_and_exits", _rs, 2, out, out_size); }
                    store_proof(b, out);
                    return 1;
                }
            }
        }
    }

    char _t1[512];
    const KbResponseSlot _r1[] = { { "obj", obj } };
    kb_term_say(b, "i_understood_the_request_produce_x", _r1, 1, _t1, sizeof _t1);
    size_t oo = (size_t)snprintf(out, out_size, "%s", _t1);
    if (lang[0]) {
        /* Even the two-word glue is a family: an Italian turn must not get an
         * English " in " welded on by the engine. */
        char _tg[64];
        kb_term_say(b, "artifact_target_language", (const KbResponseSlot[]){
                        { "lang", lang } }, 1, _tg, sizeof _tg);
        oo += (size_t)snprintf(out + oo, out_size - oo, "%s", _tg);
    }
    { char _t2[512];
    const KbResponseSlot _r2[] = { { "x", "" } };
    kb_term_say(b, "but_i_don_t_have_a_verified_schema_for_that", _r2, 0, _t2, sizeof _t2);
    snprintf(out + oo, out_size - oo, "%s", _t2);
    }
    store_proof(b, out);
    return 1;
}

/* gen331 (TODO.md P1/09): the LAST-RESORT policy decline.
 *
 * Counterexample: `make chat` loaded the AGI profile without PARROT0_TOOLS, so
 *
 *   you> list the files in src
 *   parrot0> I don't understand that yet.
 *
 * It understood perfectly. It was not permitted. Answering a permission with a
 * comprehension failure is a false claim about its own capability — the same
 * species of lie as reporting a failed build as a result (gen329) or inventing a
 * semicolon that is not there (gen330). parrot0 is allowed to be unable; it is not
 * allowed to misdescribe why.
 *
 * Why HERE, at the end of the registry, rather than inside mod_piact where the
 * intent is recognized: several faculties below piact — codeast reading a snippet
 * into structure, prosepage extracting facts from a page — legitimately answer
 * these same phrasings with no tool mode at all. Declining the turn early, to
 * announce the policy, stole it from them and broke eleven passing contracts. A
 * policy is not an intent handler: it may only speak when the intent handlers have
 * all passed. So this module sits last, and says the true thing only when the
 * alternative is the fallback saying a false one. */
static int mod_toolpolicy(Brain *b, const char *norm, const char *raw,
                          char *out, size_t out_size) {
    if (!b || !raw) return 0;
    if (brain_policy_on(b, "tools")) return 0;   /* enabled: piact already ran */

    /* Read the PER-CLAUSE canonical surface, not the raw turn. The "and"-compound
     * handler dispatches each half with the clause in `norm` and the WHOLE input
     * still in `raw` — so a module that reads `raw` sees the run verb again while
     * handling the trailing half, and answers it twice. That is exactly what
     * happened: "run tests/code/buildable/exit7.c and tell me its exit code" came
     * back as "it ran and exited with code 7. tools_disabled: …", the second clause
     * having re-fired this decline over an answer another faculty had already given
     * correctly. (tests/cases/run_execute.it.chat left a note about this trap in
     * gen-past; I walked into it anyway.) */
    const char *src = (norm && *norm) ? norm : raw;
    size_t rl = strlen(src);
    if (rl == 0 || rl >= 480) return 0;
    char low[512];
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)src[i]);

    /* gen494 — L'INVENTARIO DEGLI STRUMENTI E' CONOSCENZA (F.).
     *
     * Qui c'era una catena di `else if` con i nomi degli strumenti scritti nel C
     * — «search», «find», «list», «read», «run» — cioe' due difetti in uno: una
     * lista di parole (mantra #2) e una congiunzione compilata (mantra #19), con
     * il TODO gia' scritto sopra. Ne seguiva che uno strumento nuovo non poteva
     * essere DICHIARATO: poteva solo essere compilato, e finche' non lo era,
     * spegnerlo produceva un «non capisco» invece di «non posso».
     *
     * Ora si legge `local_tool(Nome, Intento, Glossa)`: il modulo chiede alla KB
     * quali strumenti esistono e quale intento serve ciascuno, e nomina quello
     * che manca. Uno strumento nuovo e' una riga, e lo spegnimento lo racconta
     * da solo. Le ancore STRUTTURALI (un prefisso «grep »/«read »/«ls») restano
     * nel C perche' sono forma, non vocabolario. */
    char kindbuf[KB_TERM_LEN] = "";
    const char *kind = NULL;
    {
        char tools[16][KB_TERM_LEN];
        const char *tq[3] = { NULL, NULL, NULL };
        size_t nt = kb_match(b->kb, "local_tool", tq, 3, tools, 16);
        for (size_t i = 0; i < nt && !kind; i++) {
            const char *iq[3] = { tools[i], NULL, NULL };
            char intents[1][KB_TERM_LEN];
            if (kb_match(b->kb, "local_tool", iq, 3, intents, 1) != 1) continue;
            if (!kb_cue_match(b, intents[0], low)) continue;
            snprintf(kindbuf, sizeof kindbuf, "%s", tools[i]);
            kind = kindbuf;
        }
    }
    /* le ancore strutturali, che non sono vocabolario */
    if (!kind && (ci_prefix(low,"grep "))) kind = "search";
    if (!kind && (ci_prefix(low,"ls ") || ci_eq(low,"ls"))) kind = "list";
    if (!kind && (ci_prefix(low,"read ") || ci_prefix(low,"cat "))) kind = "read";
    if (!kind && ci_prefix(low,"run ")) kind = "run";
    if (!kind) {
        /* gen495 — anche un PIANO e' una capacita' che puo' essere spenta, e
         * deve dirlo. Senza questo, «analizza tutti i sorgenti» con i tool
         * spenti rispondeva «non capisco»: la stessa bugia sulla propria
         * capacita' che questo modulo esiste per non dire. */
        char plans[8][KB_TERM_LEN];
        const char *pq[2] = { NULL, NULL };
        size_t np = kb_match(b->kb, "turn_plan", pq, 2, plans, 8);
        for (size_t i = 0; i < np && !kind; i++) {
            const char *cq[2] = { plans[i], NULL };
            char cls[1][KB_TERM_LEN];
            /* si prova sia il turno canonicalizzato sia quello DETTO: la
             * canonicalizzazione riscrive le forme italiane, e una capacita'
             * spenta deve dirlo anche a chi non parla inglese. */
            char rawlow[512];
            size_t rl2 = raw ? strlen(raw) : 0;
            if (rl2 >= sizeof rawlow) rl2 = sizeof rawlow - 1;
            for (size_t z = 0; z < rl2; z++)
                rawlow[z] = (char)tolower((unsigned char)raw[z]);
            rawlow[rl2] = '\0';
            if (kb_match(b->kb, "turn_plan", cq, 2, cls, 1) == 1 &&
                (kb_cue_match(b, cls[0], low) ||
                 kb_cue_match(b, cls[0], rawlow))) {
                snprintf(kindbuf, sizeof kindbuf, "%s", plans[i]);
                kind = kindbuf;
            }
        }
    }
    if (!kind) return 0;

    char mode[32]; brain_mode(b, mode, sizeof mode);
    char m[440];
    {   const KbResponseSlot _rs[] = { { "kind", kind }, { "mode", mode } };
      kb_term_say(b, "tools_disabled_i_understood_that_it_is_a_x_r", _rs, 2, m, sizeof m); }
    put(m, out, out_size);
    store_proof(b, "declined: tools_disabled (policy(tools, off))");
    return 1;
}
