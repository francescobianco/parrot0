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
static int mod_tool(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Fire only on an explicit word-count request (EN + IT). */
    int want_words = cue(low, "how many words") || cue(low, "count the words") ||
                     cue(low, "quante parole")  || cue(low, "conta le parole");
    if (!want_words) return 0;

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
        put("I can only count plain words for now (letters and digits, no punctuation).",
            out, out_size);
        return 1;
    }
    size_t resl = strlen(result);
    while (resl > 0 && (result[resl-1]=='\n' || result[resl-1]==' '))
        result[--resl] = '\0';

    char msg[640];
    snprintf(msg, sizeof msg, "%s. (I ran the tool: %s.)", result, cmd);
    put(msg, out, out_size);

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
 *   - UNDERSTANDING CODE goes through the AST-as-KB engine and the SWE-bench
 *     localizers, never a blind grep: "read FILE" parses the file with code_ingest
 *     and asserts `code_function`/`code_calls` facts into the LIVE KB (so a later
 *     "where is X" is a KB query); "where is X" / "what calls X" call code_locate /
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

/* A path/dir/glob token is SAFE iff every char is alnum or one of . _ / - and the
 * glob star. Anything
 * else (space is impossible — tokens are whitespace-split — but ';', '$', '`',
 * '|', quotes, etc.) means we refuse to build a command from it. This, plus
 * single-quoting the grep pattern, is what keeps a hostile prompt from injecting a
 * second command into the shell parrot0 runs. */
static int safe_pathish(const char *t) {
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
static int detect_glob(const char *low, char *glob, size_t cap) {
    static const char *const exts[] = {
        ".c",".h",".py",".js",".ts",".md",".txt",".json",".sh",
        ".cpp",".cc",".go",".rs",".java",".rb",".html",".css", NULL };
    const char *star = strstr(low, "*.");
    if (star) {
        size_t i = 0; glob[i++] = '*'; const char *p = star + 1;   /* keep ".ext" */
        while (*p && i + 1 < cap && (isalnum((unsigned char)*p) || *p=='.')) glob[i++] = *p++;
        glob[i] = '\0';
        return 1;
    }
    for (int e = 0; exts[e]; e++) {
        if (strstr(low, exts[e])) { snprintf(glob, cap, "*%s", exts[e]); return 1; }
    }
    static const struct { const char *word, *glob; } words[] = {
        {"python","*.py"}, {"header","*.h"}, {"javascript","*.js"},
        {"typescript","*.ts"}, {"markdown","*.md"}, {"shell","*.sh"},
        {"rust","*.rs"}, {"golang","*.go"}, {"java","*.java"}, {NULL,NULL} };
    for (int w = 0; words[w].word; w++)
        if (strstr(low, words[w].word)) { snprintf(glob, cap, "%s", words[w].glob); return 1; }
    return 0;
}

/* A command is allowed for the "run" intent only if its head is a known build /
 * test / inspection tool. parrot0 will execute arbitrary read-only listings on its
 * own authority, but running a command the user dictated is gated to a whitelist so
 * a prompt cannot turn parrot0 into a general shell. */
static int run_whitelisted(const char *cmd) {
    static const char *const ok[] = {
        "make ", "make\0", "npm test", "npm run", "pnpm ", "yarn ",
        "pytest", "python -m pytest", "python3 -m pytest", "cargo test",
        "cargo build", "go test", "go build", "ctest", "./tests/", "bash tests/",
        "sh tests/", "git status", "git diff", "git log", NULL };
    for (int i = 0; ok[i]; i++)
        if (ci_prefix(cmd, ok[i])) return 1;
    return strcmp(cmd, "make") == 0;
}

/* Find the first token after one of the locator words ("in", "inside", ...) that
 * is a safe path, else the first bare safe path-ish token. Returns it or NULL. */
static const char *find_dir(char **w, size_t nw) {
    for (size_t i = 0; i + 1 < nw; i++) {
        if (ci_eq(w[i],"in")||ci_eq(w[i],"inside")||ci_eq(w[i],"under")||
            ci_eq(w[i],"within")||ci_eq(w[i],"from")||ci_eq(w[i],"directory")||
            ci_eq(w[i],"folder")||ci_eq(w[i],"dir")) {
            rstrip_punct(w[i+1]);
            if (safe_pathish(w[i+1]) && strchr(w[i+1], '/')) return w[i+1];
            if (safe_pathish(w[i+1]) && strcmp(w[i+1],"the") != 0) return w[i+1];
        }
    }
    /* a bare token that contains a slash is very likely the directory. */
    for (size_t i = 0; i < nw; i++) {
        rstrip_punct(w[i]);
        if (strchr(w[i], '/') && safe_pathish(w[i])) return w[i];
    }
    return NULL;
}

/* Find a file path token: one containing '/', else one ending in a known-looking
 * .ext (e.g. main.c) — used by the "read" intent. */
static const char *find_file(char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) { rstrip_punct(w[i]); if (strchr(w[i],'/') && safe_pathish(w[i])) return w[i]; }
    for (size_t i = 0; i < nw; i++) {
        char *dot = strrchr(w[i], '.');
        if (dot && dot != w[i] && isalpha((unsigned char)dot[1]) && safe_pathish(w[i]))
            return w[i];
    }
    return NULL;
}

/* Run `cmd` locally, fold its real output into `out` as one line tagged by the
 * human-facing `label`, record the proof, and claim the turn. */
static int piact_run(Brain *b, const char *cmd, const char *label,
                     char *out, size_t out_size) {
    char raw_out[8192];
    if (!run_shell(cmd, raw_out, sizeof raw_out)) {
        char m[320]; snprintf(m, sizeof m, "I tried to run `%s` but the tool would not start.", cmd);
        put(m, out, out_size);
        return 1;
    }
    char flat[4000];
    collapse_ws(raw_out, flat, sizeof flat);
    char msg[4400];
    if (flat[0] == '\0')
        snprintf(msg, sizeof msg, "%s: nothing. (ran `%s`)", label, cmd);
    else
        snprintf(msg, sizeof msg, "%s: %s (ran `%s`)", label, flat, cmd);
    put(msg, out, out_size);
    if (b) {
        snprintf(b->last_tool_cmd, sizeof b->last_tool_cmd, "%s", cmd);
        b->has_last_tool_cmd = 1;
        char proof[320];
        snprintf(proof, sizeof proof, "ran local tool: %s", cmd);
        store_proof(b, proof);
    }
    return 1;
}

static int mod_piact(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;
    const char *en = getenv("PARROT0_TOOLS");
    if (!en || strcmp(en, "1") != 0) return 0;

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
    int want_grep = ci_prefix(low,"grep ") || kb_cue_match(b, "piact_grep", low);
    int want_find = (cue(low,"find") && (cue(low,"named") || cue(low,"called"))) ||
                    kb_cue_match(b, "piact_find", low);
    int want_list = (cue(low,"list") && (cue(low,"file") || strstr(low,"*."))) ||
                    ci_prefix(low,"ls ") || ci_eq(low,"ls") ||
                    kb_cue_match(b, "piact_list", low);
    int want_read = ci_prefix(low,"read ") || ci_prefix(low,"cat ") ||
                    kb_cue_match(b, "piact_read", low);
    int want_run  = ci_prefix(low,"run ") || kb_cue_match(b, "piact_run", low);
    if (!(want_grep || want_find || want_list || want_read || want_run))
        return 0;

    char rawcopy[512]; snprintf(rawcopy, sizeof rawcopy, "%s", raw);
    char *w[96]; size_t nw = split_words(rawcopy, w, 96);

    /* ---- locate / search for a symbol ---- */
    if (want_grep) {
        /* the pattern is the token after the cue word; sanitize for single-quotes. */
        const char *pat = NULL;
        for (size_t i = 0; i + 1 < nw; i++) {
            if (ci_eq(w[i],"for")||ci_eq(w[i],"is")||ci_eq(w[i],"to")||
                ci_eq(w[i],"of")||ci_eq(w[i],"grep")||ci_eq(w[i],"does")||
                ci_eq(w[i],"calls")||ci_eq(w[i],"call")) { pat = w[i+1]; }
        }
        if (!pat && nw >= 2) pat = w[nw-1];
        if (!pat) return 0;
        char patbuf[128], clean[128]; size_t o = 0;
        rstrip_punct((char*)pat);
        for (const char *p = pat; *p && o + 1 < sizeof clean; p++)
            if (*p != '\'' && *p != '`' && *p != '\\') clean[o++] = *p;
        clean[o] = '\0';
        if (clean[0] == '\0') return 0;
        snprintf(patbuf, sizeof patbuf, "%s", clean);
        const char *dir = find_dir(w, nw);
        char dirbuf[256]; snprintf(dirbuf, sizeof dirbuf, "%s", (dir && safe_pathish(dir)) ? dir : ".");

        /* KB-FIRST: when the target is a bare code identifier, answer by STRUCTURE,
         * not by text. code_locate / code_find_callers are the very localizers that
         * drive `make swe-solve` (CODE-MASTERY §4) — so "where is X" / "what calls X"
         * inside pi reuse the SWE-bench spine, grounded in a real parse of real files
         * rather than a line match that a comment or a string could fake. Free-text
         * patterns (with spaces or punctuation) still fall through to grep. */
        int is_ident = isalpha((unsigned char)patbuf[0]) || patbuf[0] == '_';
        for (const char *p = patbuf; *p && is_ident; p++)
            if (!(isalnum((unsigned char)*p) || *p == '_')) is_ident = 0;
        int want_callers = cue(low,"call") || cue(low,"caller") ||
                           cue(low,"uses of") || cue(low,"used by");
        if (is_ident) {
            char file[256] = ""; int located = code_locate(dirbuf, patbuf, file, sizeof file);
            char callers[16][KB_TERM_LEN]; size_t nc = code_find_callers(dirbuf, patbuf, callers, 16);
            if (located || nc) {
                char msg[1200]; size_t off = 0;
                if (located)
                    off += (size_t)snprintf(msg+off, sizeof msg-off,
                                            "`%s` is defined in %s", patbuf, file);
                else
                    off += (size_t)snprintf(msg+off, sizeof msg-off,
                                            "`%s` is not defined under %s", patbuf, dirbuf);
                if (nc && (want_callers || located)) {
                    off += (size_t)snprintf(msg+off, sizeof msg-off, "; called by ");
                    for (size_t i = 0; i < nc && off < sizeof msg; i++)
                        off += (size_t)snprintf(msg+off, sizeof msg-off, "%s%s",
                                                i ? ", " : "", callers[i]);
                } else if (located && !nc && want_callers) {
                    off += (size_t)snprintf(msg+off, sizeof msg-off, "; nothing calls it");
                }
                if (off < sizeof msg) snprintf(msg+off, sizeof msg-off, ".");
                put(msg, out, out_size);
                if (b) {
                    char proof[320];
                    snprintf(proof, sizeof proof,
                             "structural locate of %s under %s (code_locate/code_find_callers)",
                             patbuf, dirbuf);
                    store_proof(b, proof);
                }
                return 1;
            }
            /* identifier but no structural hit — fall through to a text grep. */
        }

        char cmd[640];
        snprintf(cmd, sizeof cmd,
                 "grep -rn '%s' %s 2>/dev/null | head -n 40", patbuf, dirbuf);
        char label[160]; snprintf(label, sizeof label, "Matches for `%s`", patbuf);
        return piact_run(b, cmd, label, out, out_size);
    }

    /* ---- find a file by name ---- */
    if (want_find) {
        const char *name = NULL;
        for (size_t i = 0; i + 1 < nw; i++)
            if (ci_eq(w[i],"named")||ci_eq(w[i],"called")||ci_eq(w[i],"file")) name = w[i+1];
        if (!name && nw >= 2) name = w[nw-1];
        if (!name) return 0;
        rstrip_punct((char*)name);
        if (!safe_pathish(name)) return 0;
        const char *dir = find_dir(w, nw);
        char dirbuf[256]; snprintf(dirbuf, sizeof dirbuf, "%s", (dir && safe_pathish(dir)) ? dir : ".");
        char cmd[640];
        snprintf(cmd, sizeof cmd, "find %s -name '%s' 2>/dev/null | head -n 40", dirbuf, name);
        char label[200]; snprintf(label, sizeof label, "Found `%s`", name);
        return piact_run(b, cmd, label, out, out_size);
    }

    /* ---- list files (optionally filtered by a glob) ---- */
    if (want_list) {
        const char *dir = find_dir(w, nw);
        char dirbuf[256]; snprintf(dirbuf, sizeof dirbuf, "%s", (dir && safe_pathish(dir)) ? dir : ".");
        char glob[64]; int has_glob = detect_glob(low, glob, sizeof glob);
        char cmd[640], label[200];
        if (has_glob) {
            snprintf(cmd, sizeof cmd,
                     "find %s -maxdepth 1 -name '%s' -type f 2>/dev/null | sort | head -n 60",
                     dirbuf, glob);
            snprintf(label, sizeof label, "The `%s` files in %s", glob, dirbuf);
        } else {
            snprintf(cmd, sizeof cmd,
                     "find %s -maxdepth 1 -type f 2>/dev/null | sort | head -n 60", dirbuf);
            snprintf(label, sizeof label, "The files in %s", dirbuf);
        }
        return piact_run(b, cmd, label, out, out_size);
    }

    /* ---- read a file ---- */
    if (want_read) {
        const char *file = find_file(w, nw);
        if (!file || !safe_pathish(file)) return 0;

        /* KB-FIRST read. A remote LLM "reads" a file by pulling its bytes into its
         * context; parrot0 reads it into STRUCTURE. It parses the file with the
         * AST-as-KB front-end (the same code_ingest the codebase uses, gen173+),
         * asserting `code_function`/`code_calls` facts into the LIVE KB — so the
         * answer is the functions it really defines AND a later "where is X" / "what
         * calls X" is now a KB query, not another disk scan. Only when the file is
         * not ingestable source (a header with no defs, plain data) does it fall back
         * to an honest textual head. */
        if (b->kb) {
            static char code[262144];
            if (code_read_file(file, code, sizeof code)) {
                code_strip(code);
                char names[32][KB_TERM_LEN];
                int clang = identify_code_lang(code, b);
                size_t k = (clang == 2) ? code_ingest_py(b->kb, code, names, 32)
                                        : code_ingest(b->kb, code, names, 32);
                if (k > 0) {
                    size_t shown = k < 32 ? k : 32;
                    size_t off = (size_t)snprintf(out, out_size,
                                 "I read %s into structure: it defines ", file);
                    for (size_t i = 0; i < shown && off < out_size; i++) {
                        const char *sep = (i==0) ? "" : (i==shown-1) ? " and " : ", ";
                        off += (size_t)snprintf(out+off, out_size-off, "%s%s", sep, names[i]);
                    }
                    if (off < out_size)
                        snprintf(out+off, out_size-off,
                                 ". (%zu function%s now in the KB.)", k, k==1?"":"s");
                    if (b) {
                        char proof[320];
                        snprintf(proof, sizeof proof,
                                 "read+ingested %s: %zu code_function fact(s) asserted", file, k);
                        store_proof(b, proof);
                        snprintf(b->last_tool_cmd, sizeof b->last_tool_cmd,
                                 "code_ingest(%s)", file);
                        b->has_last_tool_cmd = 1;
                    }
                    return 1;
                }
            }
        }
        /* not ingestable source — an honest bounded textual look. */
        char cmd[640];
        snprintf(cmd, sizeof cmd, "head -n 40 %s 2>/dev/null", file);
        char label[256]; snprintf(label, sizeof label, "%s", file);
        return piact_run(b, cmd, label, out, out_size);
    }

    /* ---- run a build/test command (whitelisted) ---- */
    if (want_run) {
        const char *after = NULL;
        if (ci_prefix(low, "run ")) after = raw + 4;
        else { const char *p = strstr(raw, "compile"); if (p) after = p; }
        if (!after) after = raw;
        while (*after == ' ') after++;
        char cmd[300]; snprintf(cmd, sizeof cmd, "%s", after);
        rstrip_punct(cmd);
        if (!run_whitelisted(cmd)) {
            put("I only run build/test commands I recognize (make, pytest, cargo, go, the project's tests).",
                out, out_size);
            return 1;
        }
        char full[360]; snprintf(full, sizeof full, "%s 2>&1 | tail -n 20", cmd);
        char label[320]; snprintf(label, sizeof label, "`%s`", cmd);
        return piact_run(b, full, label, out, out_size);
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
            if (strcmp(nameo, "f") == 0) snprintf(nameo, nsz, "%s", aname);
            char comparator = '>';           /* algo_io says ascending */
            if (code_synth_from_shape(shape, nameo, comparator, src, srcsz)) {
                char err[256];
                int v = code_check_sort(src, nameo, err, sizeof err);
                if (v == 1) {
                    snprintf(note, notesz,
                             "verified by execution: sorted ascending AND a permutation "
                             "of the input on 8 vectors (run via the code_check_sort oracle)");
                    return 1;
                }
                if (v == 0) { snprintf(note, notesz, "the judge ran it but it did not sort every vector"); return -1; }
                snprintf(note, notesz, "the synthesized sort would not build/run, so I will not report it");
                return -1;
            }
        }
    }

    char opword[KB_TERM_LEN];
    char op = compose_op_from_kb(b, low, opword, sizeof opword);
    if (!op) {
        snprintf(note, notesz,
                 "I can only synthesize and VERIFY the sum, product, or difference of "
                 "two integers so far — I will not emit code I cannot check.");
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
        snprintf(note, notesz,
                 "verified by symbolic execution: %s(%ld,%ld)=%ld, %s(%ld,%ld)=%ld",
                 nameo,a1,b1,g1, nameo,a2,b2,g2);
        return 1;
    }
    snprintf(note, notesz, "the oracle did not confirm it");
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

/* gen207: execute an ARTICULATED, multi-step coding request. Splits `raw` into
 * ordered clauses on STRONG sequencers (",and then", "after that", "also", "then",
 * "e poi", ...) — never the weak " and " that lives inside "a and b" — then runs each
 * clause through compose/verify or eval, THREADING the functions composed earlier so
 * a later "use it to compute NAME(x,y)" evaluates the real artifact. Every step is
 * oracle-checked; the reply is a numbered, grounded transcript. */
static int compose_plan(Brain *b, const char *raw, char *out, size_t out_size) {
    static const char *const seq[] = {
        " and then ", " and also ", " after that ", " afterwards ",
        " e poi ", " e infine ", " e inoltre ",
        " then ", " also ", " next ", " finally ", " plus ",
        " poi ", " dopo ", " infine ", " inoltre ", "; ", NULL };
    size_t n = strlen(raw);
    static char lowall[2048];
    size_t lim = n < sizeof lowall - 1 ? n : sizeof lowall - 1;
    for (size_t i = 0; i < lim; i++) lowall[i] = (char)tolower((unsigned char)raw[i]);
    lowall[lim] = '\0';

    struct { size_t a, b; } cr[10]; size_t ncl = 0, from = 0;
    for (size_t i = 0; i < lim && ncl < 9; ) {
        size_t mlen = 0;
        for (int s = 0; seq[s]; s++) {
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
            static const char *const fill[] = {"and","also","then","next","finally",
                "first","please","use","it","to","poi","dopo","e","inoltre","infine",
                "la","lo","then,", NULL};
            int cut = 0;
            for (int f = 0; fill[f]; f++) {
                size_t fl = strlen(fill[f]);
                if (ci_prefix(clause, fill[f]) && (clause[fl]==' ' || clause[fl]=='\0'))
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
        int is_compose = (cue(cllow,"write")||cue(cllow,"generate")||cue(cllow,"create")||
                          cue(cllow,"implement")||cue(cllow,"scrivi")) &&
                         (cue(cllow,"function")||cue(cllow,"funzione")||
                          cue(cllow,"variant")||cue(cllow,"variante"));
        int is_eval = cue(cllow,"compute")||cue(cllow,"evaluate")||cue(cllow,"calcola")||
                      cue(cllow,"valuta")||cue(cllow,"call ");

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
                snprintf(piece, sizeof piece, "%zu) (no concrete call to evaluate here)", step);
            }
        } else {
            /* gen208: not a compose/eval clause — dispatch it through the registry
             * (skipping compose, no re-entry) so e.g. a "tell me about X" recall is
             * answered for real by mod_research, rather than stubbed as "(not yet)". */
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

static int mod_compose(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
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

    int want = (cue(low,"write") || cue(low,"generate") || cue(low,"create") ||
                cue(low,"scrivi") || cue(low,"implement")) &&
               (cue(low,"function") || cue(low,"funzione"));
    if (!want) return 0;

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

    /* gen207: if the request is ARTICULATED (a strong sequencer chains several
     * sub-goals), run the multi-step planner; otherwise compose the single function. */
    static const char *const seq[] = {
        " and then ", " and also ", " after that ", " afterwards ", " then ",
        " also ", " next ", " finally ", " e poi ", " poi ", " dopo ",
        " infine ", " inoltre ", "; ", NULL };
    for (int s = 0; seq[s]; s++)
        if (strstr(low, seq[s])) return compose_plan(b, raw, out, out_size);

    /* single compose: Knowledge (KB) -> Synthesis (structure) -> Oracle (code_eval). */
    char name[64], src[256], note[200];
    int r = compose_one(b, raw, low, name, sizeof name, src, sizeof src, note, sizeof note);
    char msg[700];
    if (r == 1)        snprintf(msg, sizeof msg, "%s  /* %s */", src, note);
    else if (r == -1)  snprintf(msg, sizeof msg, "I composed `%s` but %s.", src, note);
    else               snprintf(msg, sizeof msg, "%s", note);
    put(msg, out, out_size);
    char proof[320];
    snprintf(proof, sizeof proof, "composed %s via code_eval oracle (%s)", name, r==1?"verified":"unverified/refused");
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

static size_t parse_branch_ops(const char *clause, AgentOp *ops, size_t max) {
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
        if (!strcmp(t, "double") || !strcmp(t, "raddoppia"))     { ops[n].op='*'; ops[n].k=2; n++; }
        else if (!strcmp(t, "triple") || !strcmp(t, "triplica")) { ops[n].op='*'; ops[n].k=3; n++; }
        else if (!strcmp(t, "halve")  || !strcmp(t, "dimezza"))  { ops[n].op='/'; ops[n].k=2; n++; }
        else if (!strcmp(t,"add")||!strcmp(t,"plus")||!strcmp(t,"aggiungi")||!strcmp(t,"somma")) pend='+';
        else if (!strcmp(t,"subtract")||!strcmp(t,"minus")||!strcmp(t,"sottrai")||!strcmp(t,"togli")) pend='-';
        else if (!strcmp(t,"multiply")||!strcmp(t,"times")||!strcmp(t,"moltiplica")) pend='*';
        else if (!strcmp(t,"divide")||!strcmp(t,"dividi")||!strcmp(t,"diviso")) pend='/';
    }
    return n;
}

/* Copy [from, end) into out, NUL-terminated and length-clamped. */
static void agent_slice(const char *from, const char *end, char *out, size_t out_size) {
    size_t len = (size_t)(end - from);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, from, len); out[len] = '\0';
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
    int has_start = cue(low, "start") || cue(low, "begin") || cue(low, "parti") ||
                    cue(low, "inizia") || cue(low, "comincia");
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
    const char *ce = strstr(low, "even"); if (!ce) ce = strstr(low, "pari");
    const char *co = strstr(low, "odd");  if (!co) co = strstr(low, "dispari");
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
            size_t ne = parse_branch_ops(ec, even_ops, 4);
            size_t no = parse_branch_ops(oc, odd_ops, 4);

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
                        double nx = apply_arith_op(o, cur, seq[s].k, &ok);
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
                    snprintf(msg, sizeof msg, "Reached %s after %ld step%s: %s.",
                             nb, steps, steps == 1 ? "" : "s", traj);
                } else {
                    snprintf(msg, sizeof msg,
                             "It didn't settle within %ld steps.", CAP);
                }
                put(msg, out, out_size);
                char proof[512];
                snprintf(proof, sizeof proof,
                         "branching act-loop: %ld observed steps, %s", steps, traj);
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
    if (cue(left, "double") || cue(left, "raddoppi"))      { op = "*"; k = 2; }
    else if (cue(left, "triple") || cue(left, "triplic"))  { op = "*"; k = 3; }
    else if (cue(left, "halve")  || cue(left, "dimezz"))   { op = "/"; k = 2; }
    else if (ln >= 2) {
        k = lnums[1];
        if (cue(left, "add") || cue(left, "plus") || cue(left, "aggiung") ||
            cue(left, "somma") || cue(left, "più"))            op = "+";
        else if (cue(left, "subtract") || cue(left, "minus") ||
                 cue(left, "sottra") || cue(left, "togli"))    op = "-";
        else if (cue(left, "multiply") || cue(left, "times") ||
                 cue(left, "moltiplic"))                       op = "*";
        else if (cue(left, "divide") || cue(left, "dividi") ||
                 cue(left, "diviso"))                          op = "/";
    }
    if (!op) return 0;

    /* Read the goal comparator from the right clause; if unstated, infer it from
     * whether the operation grows or shrinks the value. */
    enum agent_cmp cmp;
    int have_cmp = 1;
    if (cue(right, "exceed") || cue(right, "pass") || cue(right, "above") ||
        cue(right, "over") || cue(right, "more than") || cue(right, "surpass") ||
        cue(right, "super") || cue(right, "oltre"))            cmp = AGT_GT;
    else if (cue(right, "reach") || cue(right, "at least") ||
             cue(right, "almeno") || cue(right, "raggiung"))   cmp = AGT_GE;
    else if (cue(right, "at most") || cue(right, "al massimo")) cmp = AGT_LE;
    else if (cue(right, "below") || cue(right, "under") ||
             cue(right, "less than") || cue(right, "beneath") ||
             cue(right, "sotto") || cue(right, "minore"))      cmp = AGT_LT;
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
        double next = apply_arith_op(op, cur, k, &ok);
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
        snprintf(msg, sizeof msg, "Reached %s after %ld step%s: %s.",
                 nb, steps, steps == 1 ? "" : "s", traj);
    } else {
        char sb[64], tb[64];
        format_num(start, sb, sizeof sb);
        format_num(target, tb, sizeof tb);
        snprintf(msg, sizeof msg,
                 "Starting from %s, that step never reaches %s — the goal can't "
                 "be met this way.", sb, tb);
    }
    put(msg, out, out_size);

    char proof[512];
    snprintf(proof, sizeof proof, "act-loop: %ld oracle call%s, %s",
             steps, steps == 1 ? "" : "s", traj);
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

static double apply_rule(const InducedRule *r, double cur) {
    const AgentOp *seq; size_t n;
    if (r->conditional) {
        long long iv = (long long)cur;
        int even = ((double)iv == cur) && (iv % 2 == 0);
        seq = even ? r->even_ops : r->odd_ops;
        n   = even ? r->ne       : r->no;
    } else { seq = r->even_ops; n = r->ne; }
    for (size_t s = 0; s < n; s++) {
        int ok; char o[2] = { seq[s].op, 0 };
        double nx = apply_arith_op(o, cur, seq[s].k, &ok);
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

