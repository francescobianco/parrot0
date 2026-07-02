/* --- module: codeast (gen173, code as KB) -------------------------------
 * The first step of docs/CODE-MASTERY.md: a code snippet is just another corpus.
 * parrot0 parses it DETERMINISTICALLY (code_ingest, in code.c), asserts its
 * structure into the SAME live KB (code_function/1), and answers a structural
 * question from that — honestly, never from the surface substring matching that
 * mod_code does. No statistics: the C grammar is formal, so primitives-first
 * wins. Registered BEFORE mod_code so a structural question reaches the real
 * parser, not the pattern matcher. */
static int mod_codeast(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    if (!b || !b->kb || !raw || !norm) return 0;
    char s[512]; copy_trim(s, sizeof s, raw);
    if (!*s) return 0;

    /* The question text (before the ':' that introduces the code section). */
    char qpart[256]; snprintf(qpart, sizeof qpart, "%s", s);
    char *colon = strchr(qpart, ':'); if (colon) *colon = '\0';

    /* gen182: F4 localization — "which file in <dir> defines <X>?". Scan a
     * directory (sandboxed) for the file that defines the named function. Handled
     * before the snippet questions because it takes a directory, not code. */
    if ((cue(qpart, "which file") || cue(qpart, "what file") || cue(qpart, "quale file")) &&
        (cue(qpart, "define") || cue(qpart, "defines") || cue(qpart, "definisce") ||
         cue(qpart, "contains") || cue(qpart, "contiene"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char fnname[KB_TERM_LEN] = ""; char dir[256] = "";
        for (size_t i = 0; i + 1 < nw; i++) {
            if (!strcmp(w[i], "defines") || !strcmp(w[i], "define") ||
                !strcmp(w[i], "definisce") || !strcmp(w[i], "contains") ||
                !strcmp(w[i], "contiene"))
                snprintf(fnname, sizeof fnname, "%s", strip_edge_punct(w[i+1]));
            if (!strcmp(w[i], "in") || !strcmp(w[i], "under") || !strcmp(w[i], "inside") ||
                !strcmp(w[i], "within") || !strcmp(w[i], "nella") || !strcmp(w[i], "nel") ||
                !strcmp(w[i], "dentro"))
                snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i+1]));
        }
        if (!dir[0]) for (size_t i = 0; i < nw; i++)   /* fallback: a path-like token */
            if (strchr(w[i], '/')) { snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i])); break; }
        if (!fnname[0] || !dir[0]) return 0;
        char file[256];
        if (code_locate(dir, fnname, file, sizeof file))
            snprintf(out, out_size, "I looked through %s: %s is defined in %s.", dir, fnname, file);
        else
            snprintf(out, out_size, "I looked through %s but found no file that defines %s.", dir, fnname);
        store_proof(b, out);
        return 1;
    }

    /* gen191: F5 edit — "delete/remove the function <X> under <dir|file>". The
     * SAME micro-loop as rename (locate -> edit -> compile-verify -> report,
     * temp-only) with a different transformation rule, proving the edit loop is
     * rule-shaped rather than a single hardcoded operation. Deleting a function
     * that is still called yields an honest "no longer compiles" from the real
     * compiler — the grounded oracle, not a guess. */
    if ((cue(qpart, "delete") || cue(qpart, "remove") || cue(qpart, "elimina") ||
         cue(qpart, "rimuovi")) && (cue(qpart, "function") || cue(qpart, "funzione")) &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".h"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char fnname[KB_TERM_LEN] = "", path[256] = "";
        for (size_t i = 0; i < nw; i++) {
            if ((!strcmp(w[i], "function") || !strcmp(w[i], "funzione")) && i + 1 < nw)
                snprintf(fnname, sizeof fnname, "%s", strip_edge_punct(w[i+1]));
            if (strchr(w[i], '/') ||
                (strlen(w[i]) >= 2 && (strstr(w[i], ".c") || strstr(w[i], ".h"))))
                snprintf(path, sizeof path, "%s", strip_edge_punct(w[i]));
        }
        if (!fnname[0] || !path[0]) return 0;

        char fullpath[512]; char where[280] = "";
        size_t pl = strlen(path);
        int is_file = (pl >= 2 && path[pl-2] == '.' && (path[pl-1] == 'c' || path[pl-1] == 'h'));
        if (is_file) {
            snprintf(fullpath, sizeof fullpath, "%s", path);
        } else {
            char rel[256];
            if (!code_locate(path, fnname, rel, sizeof rel)) {
                snprintf(out, out_size,
                         "I looked through %s but found no file that defines %s.", path, fnname);
                store_proof(b, out);
                return 1;
            }
            snprintf(fullpath, sizeof fullpath, "%s/%s", path, rel);
            snprintf(where, sizeof where, " in %s", rel);
        }

        const char *tmp = ".p0_edit_tmp.c";
        int n = code_delete_function(fullpath, fnname, tmp);
        if (n < 0) return 0;                       /* bad name / unreadable — not ours */
        if (n == 0) {
            remove(tmp);
            snprintf(out, out_size, "I did not find a definition of %s in %s, so nothing was deleted.",
                     fnname, fullpath);
            store_proof(b, out);
            return 1;
        }
        char err[512];

        /* gen192: F5 verification by BUILDING. A "link"/"build"/"run" cue asks
         * the stronger question — does it still LINK? Syntax-only misses a call to
         * the now-missing function (an implicit-declaration warning); a real
         * compile+link makes it an undefined reference. When it fails, the reverse
         * call graph (code_find_callers) names WHO still calls it — cause derived
         * from KB, verdict grounded in the real linker. */
        int want_link = cue(qpart, "link") || cue(qpart, "build") ||
                        cue(qpart, "run") || cue(qpart, "linka") ||
                        cue(qpart, "esegui") || cue(qpart, "compila e linka");
        if (want_link) {
            int rc = code_build(tmp, err, sizeof err);
            remove(tmp);
            if (rc == 1) {
                snprintf(out, out_size, "Deleted %s%s; the result still links.", fnname, where);
            } else if (rc == 0) {
                /* who still calls it? scan the directory the file lives in. */
                char cdir[256];
                if (is_file) {
                    snprintf(cdir, sizeof cdir, "%s", fullpath);
                    char *slash = strrchr(cdir, '/');
                    if (slash) *slash = '\0'; else snprintf(cdir, sizeof cdir, ".");
                } else {
                    snprintf(cdir, sizeof cdir, "%s", path);
                }
                char callers[16][KB_TERM_LEN];
                size_t nc = code_find_callers(cdir, fnname, callers, 16);
                if (nc > 0) {
                    char who_calls[256] = "";
                    size_t off = 0;
                    for (size_t i = 0; i < nc && off + 1 < sizeof who_calls; i++)
                        off += (size_t)snprintf(who_calls + off, sizeof who_calls - off,
                                                "%s%s", i ? (i + 1 == nc ? " and " : ", ") : "",
                                                callers[i]);
                    snprintf(out, out_size,
                             "Deleted %s%s, but %s still %s %s, so the program no longer links.",
                             fnname, where, who_calls, nc > 1 ? "call" : "calls", fnname);
                } else {
                    snprintf(out, out_size,
                             "Deleted %s%s, but the result no longer links.", fnname, where);
                }
            } else {
                snprintf(out, out_size,
                         "Deleted %s%s in a temp copy; the original is unchanged.", fnname, where);
            }
            store_proof(b, out);
            return 1;
        }

        int rc = code_compile(tmp, err, sizeof err);
        remove(tmp);
        if (rc == 1)
            snprintf(out, out_size,
                     "Deleted %s%s; the result still compiles.", fnname, where);
        else if (rc == 0)
            snprintf(out, out_size,
                     "Deleted %s%s, but the result no longer compiles (something still uses it).",
                     fnname, where);
        else
            snprintf(out, out_size,
                     "Deleted %s%s in a temp copy; the original is unchanged.", fnname, where);
        store_proof(b, out);
        return 1;
    }

    /* gen187: F5 edit — "rename <old> to <new> in <file>". The full micro-loop:
     * read the file, rename the identifier (non-destructively, to a temp), compile
     * the result to VERIFY, report, and delete the temp. The original is untouched. */
    if (cue(qpart, "rename") && cue(qpart, " to ") &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".h"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char oldn[KB_TERM_LEN] = "", newn[KB_TERM_LEN] = "", path[256] = "";
        for (size_t i = 0; i < nw; i++) {
            if (!strcmp(w[i], "to") && i > 0 && i + 1 < nw) {
                snprintf(oldn, sizeof oldn, "%s", strip_edge_punct(w[i-1]));
                snprintf(newn, sizeof newn, "%s", strip_edge_punct(w[i+1]));
            }
            if (strchr(w[i], '/') ||
                (strlen(w[i]) >= 2 && (strstr(w[i], ".c") || strstr(w[i], ".h"))))
                snprintf(path, sizeof path, "%s", strip_edge_punct(w[i]));
        }
        if (!oldn[0] || !newn[0] || !path[0]) return 0;

        /* gen188: if the path is a DIRECTORY (no .c/.h suffix), first locate the
         * file that defines the function, then edit it — chaining F4 into F5. */
        char fullpath[512]; char where[280] = "";
        size_t pl = strlen(path);
        int is_file = (pl >= 2 && path[pl-2] == '.' && (path[pl-1] == 'c' || path[pl-1] == 'h'));
        if (is_file) {
            snprintf(fullpath, sizeof fullpath, "%s", path);
        } else {
            char rel[256];
            if (!code_locate(path, oldn, rel, sizeof rel)) {
                snprintf(out, out_size,
                         "I looked through %s but found no file that defines %s.", path, oldn);
                store_proof(b, out);
                return 1;
            }
            snprintf(fullpath, sizeof fullpath, "%s/%s", path, rel);
            snprintf(where, sizeof where, " in %s", rel);
        }

        const char *tmp = ".p0_edit_tmp.c";
        int n = code_rename(fullpath, oldn, newn, tmp);
        if (n < 0) return 0;                       /* bad names / unreadable — not ours */
        if (n == 0) {
            remove(tmp);
            snprintf(out, out_size, "I did not find %s in %s, so nothing was renamed.",
                     oldn, fullpath);
            store_proof(b, out);
            return 1;
        }
        char err[512];
        int rc = code_compile(tmp, err, sizeof err);
        remove(tmp);
        if (rc == 1)
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences); the result still compiles.",
                     oldn, newn, where, n);
        else if (rc == 0)
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences), but the result no longer compiles.",
                     oldn, newn, where, n);
        else
            snprintf(out, out_size,
                     "Renamed %s to %s%s (%d occurrences) in a temp copy; the original is unchanged.",
                     oldn, newn, where, n);
        store_proof(b, out);
        return 1;
    }

     /* gen257 (Track 5.1, outer-circle codebase work): OR-chain perception —
     * "how many or-chains of calls to <fn> in <path>?". A run of calls to the
     * same function joined by `||` is the structural signature of a word list
     * encoded as code (a C phrasebook) — the thing the KB-first law says should
     * be knowledge. This branch only PERCEIVES (counts and locates; changes
     * nothing); the knowledge-directed refactor is a later pull (Track 5.2+).
     * Coherence from day one: the trigger vocabulary is KB knowledge
     * (intent_cue(orchain_query, …) in kb/core/intents.p0, matched by
     * kb_cue_match), and the function name is a PARAMETER from the question —
     * the detector knows nothing of "cue" or of this codebase (it must work on
     * a foreign one; that is the anti-impostor ratchet). */
    if (kb_cue_match(b, "orchain_query", norm)) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char fn[64] = "", path[256] = "";
        for (size_t i = 0; i < nw; i++)
            if (strchr(w[i], '/')) {
                snprintf(path, sizeof path, "%s", strip_edge_punct(w[i]));
                break;
            }
        for (size_t i = 0; i + 1 < nw && !fn[0]; i++) {
            if (strcmp(w[i], "to") && strcmp(w[i], "a") && strcmp(w[i], "su")) continue;
            char *c = strip_edge_punct(w[i + 1]);
            int ok = (*c != '\0');
            for (const char *t = c; *t; t++)
                if (!(isalnum((unsigned char)*t) || *t == '_')) { ok = 0; break; }
            if (ok && strcmp(c, "in") != 0) snprintf(fn, sizeof fn, "%s", c);
        }
        if (!fn[0] || !path[0]) return 0;
        struct stat ost;
        if (stat(path, &ost) == 0 && S_ISDIR(ost.st_mode)) {
            int files = 0, calls = 0, topn = 0; char top[256] = "";
            int n = code_orchain_tree(path, fn, &files, &calls, top, sizeof top, &topn);
            if (n < 0) return 0;
            if (n == 0)
                snprintf(out, out_size,
                         "Under %s, I found no OR-chains of calls to `%s`.", path, fn);
            else
                snprintf(out, out_size,
                         "Under %s, I found %d OR-chains of calls to `%s` in %d files "
                         "(%d calls in chains); the densest is %s (%d chains).",
                         path, n, fn, files, calls, top, topn);
            store_proof(b, out);
            return 1;
        }
        int lines[1] = {0}, calls = 0;
        int n = code_find_or_chains(path, fn, lines, 1, &calls);
        if (n < 0) return 0;
        if (n == 0)
            snprintf(out, out_size,
                     "In %s, I found no OR-chains of calls to `%s`.", path, fn);
        else
            snprintf(out, out_size,
                     "In %s, I found %d OR-chains of calls to `%s` (%d calls in chains); "
                     "the first starts at line %d.",
                     path, n, fn, calls, lines[0]);
        store_proof(b, out);
        return 1;
    }

    /* gen200: F5 repair (X6 localization by structure + X7 patch) — "fix the
     * symmetry bug in <path>". parrot0 reads the file, finds a structural symmetry
     * break (a sibling branch that assigns a literal where the analogous variable
     * belongs — code_symmetry_fix, naming nothing in advance), and writes a patched
     * copy with the literal replaced by that variable (code_replace_expr). It does
     * NOT decide the fix is correct — it proposes a grounded candidate; the real
     * test suite is the oracle that confirms it (tests/swebench/oracle.sh). This is
     * the non-deceptive repair loop CODE-MASTERY §4/§6 calls for. */
    int sym_write = cue(qpart, "fix") || cue(qpart, "repair") ||
                    cue(qpart, "correggi") || cue(qpart, "ripara");
    int sym_find  = cue(qpart, "find") || cue(qpart, "locate") || cue(qpart, "where") ||
                    cue(qpart, "trova") || cue(qpart, "dove") || cue(qpart, "identify");
    if ((sym_write || sym_find) &&
        (cue(qpart, "symmetry") || cue(qpart, "simmetria") || cue(qpart, "bug")) &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".py"))) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p; while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (l >= 3 && t[l-3] == '.' && t[l-2] == 'p' && t[l-1] == 'y') looks_path = 1;
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0]) return 0;
        /* gen256: X6 SELF-localization. When the path is a DIRECTORY (a repo
         * tree, not a pre-localized file), parrot0 walks it and lets the smell
         * chain itself pick the buggy file — the harness no longer names the
         * file (parrot_solve.sh used to `find | head -1`; that was the
         * harness's intelligence). The located file then flows through the
         * SAME per-file analysis below, so the answer names file, statement
         * and fix, all derived, nothing given in advance. */
        struct stat pst;
        if (stat(path, &pst) == 0 && S_ISDIR(pst.st_mode)) {
            char hit[256];
            int tr = code_smell_tree(path, hit, sizeof hit);
            if (tr < 0) return 0;              /* sandbox/unreadable — not ours */
            if (tr == 0) {
                snprintf(out, out_size,
                         "I swept the sources under %s but found no structural bug to fix.",
                         path);
                store_proof(b, out);
                return 1;
            }
            snprintf(path, sizeof path, "%s", hit);
        }
        /* Try each structural bug smell in turn; the first to fire wins. Each
         * names nothing in advance and the real test suite is the judge. */
        char olds[256], news[256]; const char *reason = NULL;
        int r = code_symmetry_fix(path, NULL, olds, sizeof olds, news, sizeof news);
        if (r > 0) reason = "breaks the symmetry with its sibling branch";
        else if (r == 0) {
            r = code_find_discarded_result(path, NULL, olds, sizeof olds, news, sizeof news);
            if (r > 0) reason = "discards the result of a value-returning call";
        }
        if (r == 0) {
            r = code_find_cond_asymmetry(path, NULL, olds, sizeof olds, news, sizeof news);
            if (r > 0) reason = "is an asymmetric None-check that should test the attribute its sibling branches use";
        }
        if (r == 0) {
            /* gen210: the case-folding smell yields MULTIPLE coupled edits, so it is
             * applied as a small batch rather than one expr swap. It fires only when a
             * file matches all-caps keywords case-sensitively in TWO mechanisms at
             * once (flagless re.compile + an uppercase-literal ==). The real test suite
             * is still the judge; parrot0 only proposes the coupled fix. */
            char colds[4][256], cnews[4][256];
            int ne = code_find_case_folding(path, colds, cnews, 4);
            if (ne < 0) return 0;
            if (ne > 0) {
                const char *creason = "matches all-caps keywords case-sensitively "
                    "(a flagless re.compile and an uppercase-literal ==) where case should not matter";
                if (!sym_write) {                  /* report-only: localize, touch nothing */
                    size_t o = (size_t)snprintf(out, out_size,
                                 "In %s, the code %s; the coupled fix is:", path, creason);
                    for (int i = 0; i < ne && o < out_size; i++)
                        o += (size_t)snprintf(out + o, out_size - o,
                                              " [%d] `%s` -> `%s`", i + 1, colds[i], cnews[i]);
                    store_proof(b, out);
                    return 1;
                }
                char outpath[300]; snprintf(outpath, sizeof outpath, "%s.p0fix", path);
                int all = 1;
                for (int i = 0; i < ne; i++) {     /* edit 0 from original, rest in place */
                    const char *srcp = (i == 0) ? path : outpath;
                    if (code_replace_expr(srcp, colds[i], cnews[i], outpath) <= 0) { all = 0; break; }
                }
                if (!all) {                        /* don't emit a half-applied patch */
                    remove(outpath);
                    snprintf(out, out_size,
                             "In %s, the code %s, but I could not apply the coupled fix cleanly.",
                             path, creason);
                    store_proof(b, out);
                    return 1;
                }
                size_t o = (size_t)snprintf(out, out_size,
                             "In %s, the code %s. Patched copy written to %s; %d edits:",
                             path, creason, outpath, ne);
                for (int i = 0; i < ne && o < out_size; i++)
                    o += (size_t)snprintf(out + o, out_size - o,
                                          " [%d] `%s` -> `%s`", i + 1, colds[i], cnews[i]);
                store_proof(b, out);
                return 1;
            }
        }
        if (r < 0) return 0;                       /* unreadable / unsafe — not ours */
        if (r == 0 || !reason) {
            snprintf(out, out_size,
                     "I read %s but found no structural bug to fix.", path);
            store_proof(b, out);
            return 1;
        }
        if (!sym_write) {          /* report-only: localize, do not touch any file */
            snprintf(out, out_size,
                     "In %s, `%s` %s; the fix is `%s`.", path, olds, reason, news);
            store_proof(b, out);
            return 1;
        }
        char outpath[300]; snprintf(outpath, sizeof outpath, "%s.p0fix", path);
        int n = code_replace_expr(path, olds, news, outpath);
        if (n <= 0) {
            snprintf(out, out_size,
                     "In %s, `%s` %s; the fix is `%s`.", path, olds, reason, news);
            store_proof(b, out);
            return 1;
        }
        snprintf(out, out_size,
                 "In %s, `%s` %s; the fix is `%s`. Patched copy written to %s.",
                 path, olds, reason, news, outpath);
        store_proof(b, out);
        return 1;
    }

    /* gen198: F5 verification by RUNNING — "build and run <path> ... exit code".
     * The rung past code_build: compile+link, then EXECUTE and report the real
     * exit status from the process (the grounded oracle), not a guess. Handled
     * before "compile" so a turn that asks to RUN is not answered as a mere
     * compile check. The "run"/"esegui" cue plus a real path-like token is the
     * trigger; mod_tool/mod_shell run earlier and ignore a .c/.py path, so this
     * is the part that owns "run this source file".
     *
     * The verb cue is read from `norm` (the per-clause canonical surface), not
     * `raw`: when the compound splitter dispatches "run X and tell me its exit
     * code" it hands each sub-clause but passes the WHOLE raw input, so reading
     * the verb from raw would make the trailing "tell me ..." clause re-fire and
     * double the answer. The path is still taken from raw to preserve case. */
    if ((cue(norm, "run") || cue(norm, "execute") || cue(norm, "esegui") ||
         cue(norm, "esegu")) &&
        (cue(qpart, "/") || cue(qpart, ".c") || cue(qpart, ".py"))) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (l >= 3 && t[l-3] == '.' && t[l-2] == 'p' && t[l-1] == 'y') looks_path = 1;
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0]) return 0;
        char err[512]; int ec = 0;
        int rc = code_run(path, &ec, err, sizeof err);
        if (rc < 0 && err[0]) {
            /* a build failure: quote the first diagnostic line */
            char first[200] = ""; size_t fo = 0;
            for (const char *c = err; *c && *c != '\n' && fo + 1 < sizeof first; c++) first[fo++] = *c;
            first[fo] = '\0';
            snprintf(out, out_size, "It would not build, so it never ran: %s", first);
        } else if (rc < 0) {
            return 0;                          /* unsafe/unrunnable path — not ours */
        } else if (rc == 0) {
            snprintf(out, out_size, "It built, but the program did not exit normally (it was killed before finishing).");
        } else {
            snprintf(out, out_size, "it ran and exited with code %d.", ec);
        }
        store_proof(b, out);
        return 1;
    }

    /* gen186: F5 verification — "does <file> compile?". Run the compiler (a
     * deterministic tool) on a sandboxed path and report. Handled before the
     * snippet questions because it takes a file path, not inline code. */
    if (cue(qpart, "compile") || cue(qpart, "compiles") || cue(qpart, "compila")) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (l >= 3 && t[l-3] == '.' && t[l-2] == 'p' && t[l-1] == 'y') looks_path = 1; /* gen196: Python */
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0]) return 0;
        char err[512];
        int rc = code_compile(path, err, sizeof err);
        if (rc < 0) return 0;                       /* unsafe/unrunnable — not ours */
        if (rc == 1) {
            snprintf(out, out_size, "Yes, it compiles: %s has no errors.", path);
        } else {
            /* quote the first diagnostic line, trimmed */
            char first[200] = ""; size_t fo = 0;
            for (const char *c = err; *c && *c != '\n' && fo + 1 < sizeof first; c++) first[fo++] = *c;
            first[fo] = '\0';
            if (first[0])
                snprintf(out, out_size, "No, %s does not compile: %s", path, first);
            else
                snprintf(out, out_size, "No, %s does not compile.", path);
        }
        store_proof(b, out);
        return 1;
    }

    /* gen185: reverse call graph — "what/who calls <X> in <dir>?". Scan the
     * directory (sandboxed, recursive) for the functions whose body calls X — the
     * blast radius before an edit. Distinct phrasing from "what does X call". */
    if ((cue(qpart, "what calls") || cue(qpart, "who calls") ||
         cue(qpart, "chi chiama")) &&
        (cue(qpart, " in ") || cue(qpart, " under ") || cue(qpart, "/"))) {
        char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", qpart);
        char *w[48]; size_t nw = split_words(qbuf, w, 48);
        char target[KB_TERM_LEN] = ""; char dir[256] = "";
        for (size_t i = 0; i + 1 < nw; i++) {
            if (!strcmp(w[i], "calls") || !strcmp(w[i], "chiama"))
                snprintf(target, sizeof target, "%s", strip_edge_punct(w[i+1]));
            if (!strcmp(w[i], "in") || !strcmp(w[i], "under") || !strcmp(w[i], "inside") ||
                !strcmp(w[i], "within") || !strcmp(w[i], "nella") || !strcmp(w[i], "nel"))
                snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i+1]));
        }
        if (!dir[0]) for (size_t i = 0; i < nw; i++)
            if (strchr(w[i], '/')) { snprintf(dir, sizeof dir, "%s", strip_edge_punct(w[i])); break; }
        if (!target[0] || !dir[0]) return 0;
        char callers[64][KB_TERM_LEN];
        size_t nc = code_find_callers(dir, target, callers, 64);
        if (nc == 0) {
            snprintf(out, out_size, "I looked through %s but nothing calls %s.", dir, target);
            store_proof(b, out);
            return 1;
        }
        size_t off = (size_t)snprintf(out, out_size, "%s is called by ", target);
        for (size_t i = 0; i < nc && off < out_size; i++) {
            const char *sep = (i == 0) ? "" : (i == nc - 1) ? " and " : ", ";
            off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, callers[i]);
        }
        if (off < out_size) snprintf(out + off, out_size - off, ".");
        store_proof(b, out);
        return 1;
    }

    /* Three structural questions, all EN+IT, all requiring a code section so they
     * never fire on prose:
     *   - "what does <X>(args) return?"            -> code_eval       (B5)
     *   - "what/which functions does this define?" -> code_function/1 (F2)
     *   - "what does <X> call?"                    -> code_calls/2    (F3)
     * The eval cue is checked on the QUESTION only, so a snippet's own "return"
     * never reroutes a define/call question. */
    int wants_eval =
        cue(qpart, "return") || cue(qpart, "returns") || cue(qpart, "evaluate") ||
        cue(qpart, "restituisce") || cue(qpart, "ritorna") || cue(qpart, "valuta");
    int wants_funcs = !wants_eval &&
        (cue(s, "function") || cue(s, "funzioni") || cue(s, "funzione")) &&
        (cue(s, "define") || cue(s, "defined") || cue(s, "definisce") ||
         cue(s, "definite") || cue(s, "definisci"));
    int wants_calls = !wants_eval && !wants_funcs &&
        (cue(s, "call") || cue(s, "calls") || cue(s, "invoke") ||
         cue(s, "chiama") || cue(s, "invoca"));
    if (!wants_eval && !wants_funcs && !wants_calls) return 0;

    /* The code comes either inline (after a ':') or, gen181, from a real file on
     * disk named in the question (a token with a '/' or a .c/.h suffix). The file
     * read is sandboxed to the working directory (see code_read_file). */
    static char code[262144];
    code[0] = '\0';
    if (!find_code_section(s, code, sizeof code)) {
        char path[256] = "";
        for (const char *p = qpart; *p; ) {
            while (*p == ' ' || *p == '\t') p++;
            const char *t = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            size_t l = (size_t)(p - t);
            if (l > 0 && l < sizeof path) {
                int looks_path = 0;
                for (size_t i = 0; i < l; i++) if (t[i] == '/') looks_path = 1;
                if (l >= 2 && t[l-2] == '.' && (t[l-1] == 'c' || t[l-1] == 'h')) looks_path = 1;
                if (l >= 3 && t[l-3] == '.' && t[l-2] == 'p' && t[l-1] == 'y') looks_path = 1; /* gen196: Python */
                if (looks_path) { memcpy(path, t, l); path[l] = '\0'; break; }
            }
        }
        if (!path[0] || !code_read_file(path, code, sizeof code)) return 0;
    }

    /* gen184: blank comments and string/char literals so the scanners see only
     * real code (a '(' or '{' inside a comment/string no longer derails parsing).
     * Applied once here for the snippet questions; code_locate strips per file. */
    code_strip(code);

    /* B5: symbolic execution. Parse a concrete call NAME(int, int, ...) from the
     * question and COMPUTE its result from the function body — nothing is run. */
    if (wants_eval) {
        char fname[KB_TERM_LEN] = ""; long argv[8]; size_t nargs = 0; int have = 0;
        for (const char *p = qpart; *p && !have; ) {
            if (!(isalpha((unsigned char)*p) || *p == '_')) { p++; continue; }
            const char *id = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            size_t l = (size_t)(p - id);
            const char *q = p; while (*q == ' ') q++;
            if (*q != '(') continue;
            const char *a = q + 1; long tmp[8]; size_t n = 0; int ok = 1, any = 0;
            while (*a && *a != ')') {
                while (*a == ' ' || *a == ',') a++;
                if (*a == ')' || !*a) break;
                int sign = 1;
                if (*a == '-') { sign = -1; a++; } else if (*a == '+') a++;
                if (!isdigit((unsigned char)*a)) { ok = 0; break; }
                long v = 0; while (isdigit((unsigned char)*a)) { v = v * 10 + (*a - '0'); a++; }
                any = 1;
                if (n < 8) tmp[n++] = sign * v; else { ok = 0; break; }
                while (*a == ' ') a++;
                if (*a != ',' && *a != ')') { ok = 0; break; }
            }
            if (ok && any && l < sizeof fname) {
                memcpy(fname, id, l); fname[l] = '\0';
                for (size_t i = 0; i < n; i++) argv[i] = tmp[i];
                nargs = n; have = 1;
            }
        }
        if (!have) return 0;   /* eval phrasing but no concrete call — not ours */

        char callstr[128]; size_t co = (size_t)snprintf(callstr, sizeof callstr, "%s(", fname);
        for (size_t i = 0; i < nargs && co < sizeof callstr; i++)
            co += (size_t)snprintf(callstr + co, sizeof callstr - co,
                                   "%s%ld", i ? ", " : "", argv[i]);
        if (co < sizeof callstr) snprintf(callstr + co, sizeof callstr - co, ")");

        long res;
        if (code_eval(code, fname, argv, nargs, &res))
            snprintf(out, out_size,
                     "I read it as code and worked it out: %s returns %ld.", callstr, res);
        else
            snprintf(out, out_size,
                     "I read it as code, but I cannot compute %s yet — its body is "
                     "beyond my arithmetic evaluator.", callstr);
        store_proof(b, out);
        return 1;
    }

    /* gen196 (language-as-delta): pick the front-end by language, but feed the
     * SAME downstream analyzers — Python emits the same code_function/code_calls
     * facts as C, so the answer code below is unchanged. */
    char names[32][KB_TERM_LEN];
    int clang = identify_code_lang(code, b);
    size_t k = (clang == 2) ? code_ingest_py(b->kb, code, names, 32)
                            : code_ingest(b->kb, code, names, 32);
    size_t shown = k < 32 ? k : 32;

    if (wants_funcs) {
        if (k == 0) {
            put("I read that as code, but I do not see any function definitions in it.",
                out, out_size);
            return 1;
        }
        size_t off = (size_t)snprintf(out, out_size,
                                      "I read it as code: it defines ");
        for (size_t i = 0; i < shown && off < out_size; i++) {
            const char *sep = (i == 0) ? "" : (i == shown - 1) ? " and " : ", ";
            off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, names[i]);
        }
        if (off < out_size) snprintf(out + off, out_size - off, ".");
        store_proof(b, out);
        return 1;
    }

    /* wants_calls: the question names a defined function (or there is only one).
     * Look at the question text (qpart, computed above) to pick which one. */
    const char *xfn = NULL;
    for (size_t i = 0; i < shown; i++)
        if (strstr(qpart, names[i])) { xfn = names[i]; break; }
    if (!xfn && k == 1) xfn = names[0];
    if (!xfn) {
        put("I read that as code, but I am not sure which function you mean.",
            out, out_size);
        return 1;
    }

    /* Answer from the derived call graph (code_calls/2), not from a re-scan. */
    char callees[32][KB_TERM_LEN];
    const char *pat[] = { xfn, NULL };
    size_t c = kb_match(b->kb, "code_calls", pat, 2, callees, 32);
    if (c == 0) {
        snprintf(out, out_size,
                 "I read it as code: %s does not call any function.", xfn);
        store_proof(b, out);
        return 1;
    }
    size_t off = (size_t)snprintf(out, out_size, "I read it as code: %s calls ", xfn);
    size_t cshown = c < 32 ? c : 32;
    for (size_t i = 0; i < cshown && off < out_size; i++) {
        const char *sep = (i == 0) ? "" : (i == cshown - 1) ? " and " : ", ";
        off += (size_t)snprintf(out + off, out_size - off, "%s%s", sep, callees[i]);
    }
    if (off < out_size) snprintf(out + off, out_size - off, ".");
    store_proof(b, out);
    return 1;
}

static int mod_code(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)norm;
    if (!raw) return 0;

    char s[512]; copy_trim(s, sizeof s, raw);
    if (!*s) return 0;

    int qtype = 0; /* 0=none 1=wrong/debug 2=fix 3=what-lang 4=valid 5=explain */
    if (cue(s, "what is wrong") || cue(s, "what s wrong") ||
        cue(s, "debug") || cue(s, "find the bug") || cue(s, "find the error") ||
        cue(s, "cosa non va") || cue(s, "trova l errore") || cue(s, "debugga")) {
        qtype = 1;
    } else if (cue(s, "fix this") || cue(s, "correct this") ||
               cue(s, "correggi questo") || cue(s, "aggiusta")) {
        qtype = 2;
    } else if (cue(s, "what language is this") ||
               cue(s, "che linguaggio")) {
        qtype = 3;
    } else if (cue(s, "is this valid") || cue(s, "e valido") ||
               cue(s, "is this correct")) {
        qtype = 4;
    } else if (cue(s, "explain this code") || cue(s, "spiega questo codice") ||
               cue(s, "what does this code do") || cue(s, "cosa fa questo codice")) {
        qtype = 5;
    } else if (cue(s, "what is a") && !strstr(s, "what is a ") &&
               (strstr(s, "in C") || strstr(s, "in Python") || strstr(s, "in programming"))) {
        /* Concept query handled by mod_knowledge via KB concept() facts */
        return 0;
    }
    if (!qtype) return 0;

    char code[512] = {0};
    if (!find_code_section(s, code, sizeof code)) {
        /* Try without colon: if the query is short and followed by code-like content */
        const char *c = s;
        while (*c && *c != ':') c++;
        if (!*c) return 0; /* no colon, can't extract code */
        return 0;
    }

    /* Identify language */
    int lang = identify_code_lang(code, b);

    /* For language-only queries */
    if (qtype == 3) {
        char ln[32]; lang_name(lang, ln, sizeof ln);
        if (lang == 0) {
            snprintf(out, out_size, "I can not identify the language. "
                     "It does not look like C or Python to me.");
        } else {
            snprintf(out, out_size, "This looks like %s code.", ln);
        }
        return 1;
    }

    /* For explain queries */
    if (qtype == 5) {
        char ln[32]; lang_name(lang, ln, sizeof ln);
        snprintf(out, out_size, "This is a %s code snippet.", ln);
        if (strstr(code, "printf")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It prints output using printf.");
        }
        if (strstr(code, "return")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It returns a value.");
        }
        if (strstr(code, "for ") || strstr(code, "while ")) {
            size_t l = strlen(out);
            snprintf(out + l, out_size - l, " It contains a loop.");
        }
        return 1;
    }

    /* Run checks (focus on C; Python checks are minimal) */
    char findings[512] = {0};
    int errors = 0;

    if (lang == 1) {  /* C */
        char r[256];
        if (check_missing_semicolons(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_type_mismatch(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_unclosed_string(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_balanced_braces(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_balanced_parens(code, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
        if (check_unknown_function(code, b, r, sizeof r)) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol, "%s. ", r);
            errors++;
        }
    } else if (lang == 2) {  /* Python */
        if (strstr(code, "def ") || strstr(code, "if ") || strstr(code, "for ") ||
            strstr(code, "while ") || strstr(code, "class ") || strstr(code, "elif ")) {
            /* Check for missing colon: the line ends without : */
            char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
            char *lines[64]; int nl = 0;
            char *save = NULL;
            char *tok = strtok_r(buf, "\n", &save);
            while (tok && nl < 64) { lines[nl++] = tok; tok = strtok_r(NULL, "\n", &save); }
            for (int i = 0; i < nl; i++) {
                char *l = lines[i]; while (*l && isspace((unsigned char)*l)) l++;
                char tmp[256]; snprintf(tmp, sizeof tmp, "%s", l);
                char *tw[64]; size_t tnw = split_words(tmp, tw, 64);
                if (tnw > 0 && (strcmp(tw[0], "def") == 0 || strcmp(tw[0], "if") == 0 ||
                    strcmp(tw[0], "for") == 0 || strcmp(tw[0], "while") == 0 ||
                    strcmp(tw[0], "class") == 0 || strcmp(tw[0], "elif") == 0 ||
                    strcmp(tw[0], "else") == 0 || strcmp(tw[0], "try") == 0 ||
                    strcmp(tw[0], "except") == 0)) {
                    size_t len = strlen(l);
                    if (len > 0 && l[len - 1] != ':') {
                        snprintf(findings, sizeof findings,
                            "Missing colon: the line starting with \"%s\" needs a colon at the end.", tw[0]);
                        errors++;
                        break;
                    }
                }
            }
        }
        if (strstr(code, "print") && !strstr(code, "print(")) {
            size_t ol = strlen(findings);
            snprintf(findings + ol, sizeof findings - ol,
                "Python 3 requires parentheses for print: use print(...) not print ...");
            errors++;
        }
    }

    if (errors == 0 && (qtype == 1 || qtype == 2)) {
        snprintf(out, out_size, "I did not find obvious errors in this code snippet. "
                 "(I only check basic syntax: semicolons, braces, parentheses, "
                 "string quotes, and known function names.)");
        return 1;
    }

    if (qtype == 1) {
        /* Strip trailing ". " */
        size_t fl = strlen(findings);
        while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
        snprintf(out, out_size, "%s found: %s.", errors == 1 ? "Issue" : "Issues", findings);
    } else if (qtype == 2) {
        size_t fl = strlen(findings);
        while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
        if (strstr(findings, "semicolon"))
            snprintf(out, out_size, "Fix: add a semicolon at the end of each statement.");
        else if (strstr(findings, "Type mismatch"))
            snprintf(out, out_size, "Fix: change the variable type or the value to make them compatible.");
        else if (strstr(findings, "string"))
            snprintf(out, out_size, "Fix: add the closing double-quote.");
        else if (strstr(findings, "braces"))
            snprintf(out, out_size, "Fix: add or remove braces to balance them.");
        else if (strstr(findings, "parentheses"))
            snprintf(out, out_size, "Fix: add or remove parentheses to balance them.");
        else if (strstr(findings, "function"))
            snprintf(out, out_size, "Fix: check the function name spelling or include the right header. Did you mean printf instead of print?");
        else if (strstr(findings, "colon"))
            snprintf(out, out_size, "Fix: add a colon at the end of the block-introducing line.");
        else
            snprintf(out, out_size, "I did not find a clear fix. Can you describe what behavior you expect?");
    } else if (qtype == 4) {
        if (errors == 0)
            snprintf(out, out_size, "This code looks valid at first glance (basic syntax checks pass).");
        else {
            size_t fl = strlen(findings);
            while (fl > 0 && (findings[fl-1] == ' ' || findings[fl-1] == '.')) findings[--fl] = '\0';
            snprintf(out, out_size, "Not valid: %s.", findings);
        }
    }

    return 1;
}

static int mod_symbolic(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)norm;
    if (!raw) return 0;

    char s[256];
    copy_trim(s, sizeof s, raw);          /* keep symbols; trim edges only */
    if (s[0] == '\0') return 0;
    char lc[256]; size_t i = 0;
    for (; s[i] && i + 1 < sizeof lc; i++) lc[i] = (char)tolower((unsigned char)s[i]);
    lc[i] = '\0';

    /* Morse is the strictest charset, so test it first. */
    if (looks_morse(lc))
        return name_register(b, "That looks like Morse code.",
                             "Dots and dashes — that's Morse.", out, out_size);

    /* tokenize a throwaway copy for the code check (split_words mutates it);
     * the other detectors read `lc`, which stays intact. */
    char tok[256]; copy_trim(tok, sizeof tok, lc);
    char *w[64]; size_t nw = split_words(tok, w, 64);

    if (looks_solfege(lc))
        return name_register(b, "Those are musical notes (solfège).",
                             "Sounds like solfège — do re mi.", out, out_size);

    if (looks_palindrome(lc))
        return name_register(b, "That looks like a palindrome.",
                             "Neat — it reads the same backwards (a palindrome).",
                             out, out_size);

    if (looks_leet(lc))
        return name_register(b, "That looks like leetspeak.",
                             "Letters as numbers — that's leetspeak.", out, out_size);

    if (looks_code(lc, w, nw))
        return name_register(b, "That looks like a snippet of code.",
                             "Looks like a fragment of code.", out, out_size);

    return 0;
}

/* --- module: translate (gen126, L5) -------------------------------------
 * Grounded translation of a short clause, EN<->IT, COMPOSED from per-word
 * glosses (tr/2 in kb/core/gloss.p0) plus a structural article rule, never
 * a stored sentence. So any noun/verb in the lexicon transfers to a held-out
 * clause: this is translation, not a phrasebook. Honest decline on an unknown
 * content word (it names the word it cannot translate).
 *
 * Shapes handled: [det] noun [adj] verb, and [det] noun. Italian gives the
 * noun's gendered article (il/la, un/una) and post-posed agreeing adjective.
 */

/* Look up the Italian gloss of an English word, or vice-versa. dir = 1 means
 * en->it (match arg0, return arg1); dir = 0 means it->en (match arg1, return
 * arg0). Returns 1 and fills `out` on success. */
static int gloss_lookup(Brain *b, const char *word, int dir,
                        char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char hits[1][KB_TERM_LEN];
    const char *args_en2it[] = {word, NULL};   /* tr(word, ?) */
    const char *args_it2en[] = {NULL, word};   /* tr(?, word) */
    size_t n = kb_match(b->kb, "tr", dir ? args_en2it : args_it2en, 2, hits, 1);
    if (n == 0) return 0;
    put(hits[0], out, out_size);
    return 1;
}

/* Agree an Italian masculine-singular adjective with the noun's gender:
 * -o -> -a for feminine (piccolo -> piccola); invariant adjectives (grande)
 * and others are left unchanged. */
static void agree_adj(char *adj, char gender) {
    size_t n = strlen(adj);
    if (gender == 'f' && n > 0 && adj[n - 1] == 'o') adj[n - 1] = 'a';
}

static int is_en_det(const char *w) {
    return strcmp(w, "the") == 0 || strcmp(w, "a") == 0 || strcmp(w, "an") == 0;
}
static int is_it_det(const char *w) {
    return strcmp(w, "il") == 0 || strcmp(w, "la") == 0 ||
           strcmp(w, "lo") == 0 || strcmp(w, "un") == 0 ||
           strcmp(w, "una") == 0 || strcmp(w, "uno") == 0;
}

