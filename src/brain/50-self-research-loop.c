/* --- module: loop --------------------------------------------------------
 * Self-challenge parity for the external LOOP.md driver. This is deliberately
 * not self-management: parrot0 does not edit files, run tests, commit, or choose
 * its own next task. It answers a narrower conversational challenge: when the
 * external agent poses a problem about parrot0 itself, propose a comparable
 * engineering move in the same discipline the loop uses - smallest behavioral
 * change, executable ratchet, version bump, and journaled observation. */
static int mod_loop(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    char pre[256];
    normalize((raw && *raw) ? raw : norm, pre, sizeof pre);
    for (size_t i = 0; pre[i]; i++)
        if (ispunct((unsigned char)pre[i]) && pre[i] != '-') pre[i] = ' ';

    char buf[256];
    canonicalize_lang(b, pre, buf, sizeof buf);
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    /* gen275: this module's cue chains were migrated to the KB by parrot0's
     * own kbfirst_migration plan (Track 5.4) — the phrasings are intent_cue
     * facts in kb/core/intents.p0, teachable at runtime like any vocabulary. */
    int self_ref = kb_cue_match(b, "50_self_research_loop_chain21", buf);

    /* gen164: a COMPOSITION self-challenge — "prove your subsystems compose",
     * "test composition over three subsystems", "do your parts cooperate?" —
     * is a distinct kind of self-challenge the gap branches below do not cover.
     * It asks parrot0 to reason about composing its OWN parts. EN+IT cues; the
     * parts/composition words carry the meaning, so it transfers to unseen
     * phrasings rather than a fixed trigger list. */
    int parts_ref = self_ref || kb_cue_match(b, "50_self_research_loop_chain35", buf);
    int compose_ref = kb_cue_match(b, "50_self_research_loop_chain40", buf);
    int compose_challenge = compose_ref && parts_ref;

    /* gen166: a request to SEE the dialogue, not just the method — "show me the
     * dialogue you would run", "write the example", "dimostralo con un dialogo".
     * It closes proposal to a runnable skeleton over the derived parts. */
    int want_skeleton = (kb_cue_match(b, "50_self_research_loop_chain50", buf)) &&
                        (compose_ref || parts_ref || kb_cue_match(b, "50_self_research_loop_chain53", buf));

    /* gen167: a request to actually RUN the composition and report — "prove your
     * parts compose by running it yourself", "run the composition test on
     * yourself", IT "esegui tu il test di composizione". This does not describe
     * or display; it EXECUTES the derived dialogue on a fresh copy of parrot0 and
     * reports, computed from real output, not a canned string. Still no file
     * edit, no commit — the strongest reflexive claim inside the loop's boundary. */
    int run_ref = kb_cue_match(b, "50_self_research_loop_chain63", buf);
    int want_selftest = run_ref && (compose_ref || parts_ref);

    /* gen169: an AUDIT — run several different triples of my parts and report a
     * real cooperation MAP (the compose-bench matrix turned inward). "audit your
     * composition", "map which of your parts compose", IT "verifica quali tuoi
     * moduli si compongono". */
    int want_audit = (kb_cue_match(b, "50_self_research_loop_chain77", buf)) &&
                     (compose_ref || parts_ref);

    int trigger = compose_challenge || want_skeleton || want_selftest ||
                  want_audit ||
                  kb_cue_match(b, "50_self_research_loop_chain85", buf) ||
                  (cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "solve") && cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "improve") && self_ref) ||
                  (cue(buf, "review this implementation") &&
                   (self_ref || cue(buf, "loop"))) ||
                  (cue(buf, "problem") && self_ref &&
                   (kb_cue_match(b, "50_self_research_loop_chain92", buf)));
    if (!trigger) return 0;

    /* The composition self-challenge answers over real parts — exactly the
     * compose-bench discipline — and stays anti-self-management: it proposes, an
     * external agent acts. gen165: the three parts are DERIVED from the live
     * self-model — walk a composable-core list and keep only modules that hold as
     * module(X) (the fact base "who is a module?" reads), so retracting a module
     * shifts the named parts. gen166: each part also carries a TURN fragment, so
     * the proposal can become a runnable held-out dialogue skeleton. The default
     * triple — knowledge, abduce, robust — is exactly the one
     * tests/compose/analytical_en.dlg proves cooperates. */
    if (compose_challenge || want_skeleton || want_selftest || want_audit) {
        static const struct { const char *key, *gloss, *sig; } core[] = {
            {"knowledge", "knowledge (facts and rules)",        "Learned rule"},
            {"abduce",    "abduction (the missing premise)",    "missing"},
            {"robust",    "robustness (which facts are load-bearing)", "load-bearing"},
            {"calibrate", "calibration (how sure I am)",        "confident"},
            {"memory",    "personal memory",                    "name is"},
            {"coref",     "discourse reference",                "Yes"},
            {"cause",     "cause and effect",                   "flood"},
            {"compare",   "comparison",                         "5"},
        };
        size_t pick[3], picked = 0;
        for (size_t i = 0; i < sizeof core / sizeof core[0] && picked < 3; i++) {
            const char *a[] = {core[i].key};
            if (b->kb && kb_query(b->kb, "module", a, 1)) pick[picked++] = i;
        }
        char msg[900];
        if (want_audit) {
            /* gen169: run several DIFFERENT triples of my parts, each on a fresh
             * copy of myself with held-out vocab, and report a real cooperation
             * MAP — the compose-bench matrix turned inward. A triple "composes"
             * iff every part is one I still believe I have (module(X) holds) AND
             * each fired when actually run; so retracting a module changes the
             * map, and the verdict is computed, never tabulated. */
            static const size_t triples[][3] = {{0,1,2},{0,1,3},{0,2,3}};
            size_t nt = sizeof triples / sizeof triples[0];
            char rep[700]; size_t ro = 0, pass_count = 0;
            for (size_t t = 0; t < nt; t++) {
                const char *keys[3], *sigs[3];
                int available = 1;
                for (size_t j = 0; j < 3; j++) {
                    keys[j] = core[triples[t][j]].key;
                    sigs[j] = core[triples[t][j]].sig;
                    const char *a[] = {keys[j]};
                    if (!b->kb || !kb_query(b->kb, "module", a, 1)) available = 0;
                }
                const char *const *v = compose_vocab[t % COMPOSE_VOCAB_N];
                size_t fired = available
                    ? run_composition(keys, sigs, 3, v, NULL, 0) : 0;
                int ok = available && fired == 3;
                if (ok) pass_count++;
                ro += (size_t)snprintf(rep + ro, sizeof rep - ro, "%s%s+%s+%s %s",
                                       t ? "; " : "", keys[0], keys[1], keys[2],
                                       ok ? "compose" : "seam");
            }
            /* gen240 (KB-first): the report text is KB knowledge, localized to the
             * current language (response_template(audit_report, Lang, …)), with
             * {map}/{n}/{total} filled here — not a hardcoded C phrasebook string. */
            char tpl[400];
            if (!lang_template(b, "audit_report", tpl, sizeof tpl))
                snprintf(tpl, sizeof tpl,
                    "I audited my own composition on fresh copies of myself: {map}. "
                    "{n} of {total} triples hold. No file was touched; an external "
                    "agent owns edits and commits.");
            char ns[16], ts[16];
            snprintf(ns, sizeof ns, "%zu", pass_count);
            snprintf(ts, sizeof ts, "%zu", nt);
            size_t mo = 0;
            for (const char *c = tpl; *c && mo + 1 < sizeof msg; ) {
                if (!strncmp(c, "{map}", 5))   { mo += (size_t)snprintf(msg+mo, sizeof msg-mo, "%s", rep); c += 5; }
                else if (!strncmp(c, "{n}", 3)) { mo += (size_t)snprintf(msg+mo, sizeof msg-mo, "%s", ns);  c += 3; }
                else if (!strncmp(c, "{total}", 7)) { mo += (size_t)snprintf(msg+mo, sizeof msg-mo, "%s", ts); c += 7; }
                else msg[mo++] = *c++;
            }
            msg[mo < sizeof msg ? mo : sizeof msg - 1] = '\0';
            put(msg, out, out_size);
            store_proof(b, "loop composition audit: run several triples of my parts on fresh sub-brains and report which compose; computed from real output, edits external.");
            return 1;
        }
        if (picked < 3) {
            put("I would treat it as a composition self-challenge, not self-management: pick three parts I already have and write ONE held-out dialogue, with fresh names so it cannot be memorized, that needs all three at once; it passes only if they cooperate with no new special-case module. I would ratchet it in English and Italian, bump my version, and journal whether composition held or a seam appeared. I can propose this; an external agent edits, runs the tests, and commits.",
                out, out_size);
        } else if (want_selftest) {
            /* gen167/168: actually RUN the derived composition on a fresh copy of
             * myself and report from real output. gen168: a FRESH vocab tuple is
             * chosen per run (so two self-tests use different names — proof the run
             * is executed, not memorized), and the PASS report cites the
             * cooperation actually OBSERVED in the sub-run. Each part's turns are
             * fed through brain_respond on the sub-brain (no static state, so this
             * is reentrancy-safe and footprint-free on the live brain); a part
             * "fired" iff its signature appears. The verdict is computed. */
            const char *const *v = compose_vocab[b->selftest_runs % COMPOSE_VOCAB_N];
            b->selftest_runs++;
            const char *keys[3], *sigs[3];
            char names[256]; size_t no = 0;
            for (size_t k = 0; k < picked; k++) {
                keys[k] = core[pick[k]].key;
                sigs[k] = core[pick[k]].sig;
                no += (size_t)snprintf(names + no, sizeof names - no, "%s%s",
                                       k ? (k == picked - 1 ? " and " : ", ") : "",
                                       core[pick[k]].key);
            }
            char observed[512] = "";
            size_t fired = run_composition(keys, sigs, picked, v,
                                           observed, sizeof observed);
            if (fired == picked)
                snprintf(msg, sizeof msg,
                    "I ran it on a fresh copy of myself with held-out names (a %s %s named %s): %s — %zu of %zu parts fired and cooperated. Observed: %s Composition holds (PASS). No file was touched; an external agent still owns edits and commits.",
                    v[0], v[1], v[3], names, fired, picked, observed);
            else
                snprintf(msg, sizeof msg,
                    "I ran it on a fresh copy of myself with held-out names (a %s %s named %s): %s — only %zu of %zu parts fired, so a seam appeared (FAIL). That gap is the next task for the external loop.",
                    v[0], v[1], v[3], names, fired, picked);
            put(msg, out, out_size);
            store_proof(b, "loop composition self-test: generate fresh vocab, run the derived dialogue on a fresh sub-brain, report pass/seam + observed cooperation from real output; footprint-free, edits external.");
            return 1;
        } else if (want_skeleton) {
            /* gen166: emit a runnable, single-line `>`-turn skeleton over the
             * derived parts (a fixed example tuple). An external agent fills the
             * names and drops it into tests/compose/; parrot0 does not run it. */
            size_t o = (size_t)snprintf(msg, sizeof msg,
                "Here is a held-out dialogue I would run (fresh names; an external agent fills and runs it, I do not):");
            for (size_t k = 0; k < picked; k++) {
                char trn[256];
                build_turn(core[pick[k]].key, compose_vocab[0], trn, sizeof trn);
                o += (size_t)snprintf(msg + o, sizeof msg - o, " > %s", trn);
            }
            put(msg, out, out_size);
            store_proof(b, "loop composition skeleton: emit a runnable >-turn dialogue over the parts derived from module/X; external agent fills, runs, commits.");
        } else {
            snprintf(msg, sizeof msg,
                "I would treat it as a composition self-challenge, not self-management: from my own module set I would pick three parts I actually have — %s, %s, and %s — and write ONE held-out dialogue, with fresh names so it cannot be memorized, that needs all three at once; it passes only if they cooperate with no new special-case module. I would ratchet it in English and Italian, bump my version, and journal whether composition held or a seam appeared. I can propose this; an external agent edits, runs the tests, and commits.",
                core[pick[0]].gloss, core[pick[1]].gloss, core[pick[2]].gloss);
            put(msg, out, out_size);
            store_proof(b, "loop composition self-challenge: compose >=3 existing parts (derived from module/X) in one held-out dialogue, fresh names, ratchet EN+IT, no new module, edits external.");
        }
        return 1;
    }
    int fallback_gap = kb_cue_match(b, "50_self_research_loop_chain233", buf);
    int strong_implementation_gap = kb_cue_match(b, "50_self_research_loop_chain237", buf);
    int implementation_gap = strong_implementation_gap ||
                             kb_cue_match(b, "50_self_research_loop_chain241", buf);

    if (implementation_gap && (!fallback_gap || strong_implementation_gap)) {
        put("I would solve it by parity with the external loop: name the missing behavior, locate the owning module or registry point, add the smallest deterministic change, add English and Italian regression tests, bump my version, and journal the observed support quality. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    } else if (fallback_gap) {
        put("I would treat it as a fallback gap: make the fallback or owning module handle that turn without pretending to understand, ratchet it with held-out English and Italian chats, bump my version, and record whether the wall got smaller. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    } else {
        put("I would treat it as a self-challenge, not self-management: identify the missing behavior, choose the smallest module or dispatch change, ratchet it with English and Italian tests, bump my version, and record what changed. I can propose the change; an external agent still edits and verifies the files.",
            out, out_size);
    }
    store_proof(b, "loop self-challenge parity: classify the gap, name the smallest behavior change, require tests, version, and journal, and keep file edits external.");
    return 1;
}

/* --- module: learn (gen171, dynamic knowledge; renamed from research gen240) --
 * The "inexhaustible interlocutor" seam. Asked to DEFINE a topic it does not
 * know, parrot0 neither silently walls nor pretends. It reads only STATIC local
 * markdown (no intelligence API), asserts the learned wiki_concept straight into
 * RAM through learn_topic(), and answers honestly that it just learned the
 * definition. If no local source exists, it says so. Registered LAST, so it only
 * catches a definitional gap that every other module already declined — never
 * social/arith/memory turns.
 * gen172: the learning STICKS. The def is asserted quoted (the .p0 atom
 * convention), so a re-ask is no longer a gap — mod_knowledge's exact-key path
 * (now compound-aware) speaks it as a known concept, and this module's own
 * RAM-recall guard is the honest fallback if reached. */
/* gen240 (universal-comprehension §7): the ACQUIRE-KNOWLEDGE action, factored out
 * so it is a reusable planner step, not just mod_learn's tail. Pursues the missing
 * precondition know(key): already in RAM -> 2 (def from memory); learned now from
 * the local certified corpus or an on-demand Wikipedia fetch -> 1; no source -> 0.
 * The discovery plan (local corpus, then HTTPS fetch via wiki_fetch_topic) lives
 * entirely here, so any goal that needs a concept can call it as its precondition. */
static int acquire_knowledge(Brain *b, const char *key, char *def, size_t def_sz) {
    if (!b || !b->kb || !key || !*key) return 0;
    if (kb_concept_def(b->kb, key, def, def_sz)) return 2;        /* already known */
    if (learn_topic(b->kb, key, key, def, def_sz)) return 1;      /* local corpus */
    if (wiki_fetch_topic(key) && learn_topic(b->kb, key, key, def, def_sz)) {
        /* the fetch wrote a page file — record it as a session artifact */
        const char *dir = getenv("PARROT0_WIKI_DIR");
        if (!dir || !*dir) dir = "kb/learning/pages";
        char path[256]; snprintf(path, sizeof path, "%s/%s.md", dir, key);
        note_artifact(b, "file", path);
        return 1;                                                 /* on-demand fetch */
    }
    return 0;
}

/* deep-reasoning M2 (docs/plans/deep-reasoning.md §8): the PROSE extractor. Read a
 * corpus page's `## Extract` lead paragraph, split it into sentences, and run each
 * through the SAME comprehension parser M0 extended (extract_class_statement) — so
 * what parrot0 can UNDERSTAND, it EXTRACTS, each fact carrying its source fragment
 * (M1). Broad by design (§4.4): over-extraction is tolerated; the deep-reasoning
 * loop re-checks facts against their source (M4). Returns the number of facts
 * asserted and appends a readable list to `out`. */
static int extract_page_facts(Brain *b, const char *key, char *out, size_t out_sz) {
    if (!b || !b->kb || !key || !*key) return 0;
    const char *dir = getenv("PARROT0_WIKI_DIR");
    if (!dir || !*dir) dir = "kb/learning/pages";
    char path[512];
    snprintf(path, sizeof path, "%s/%s.md", dir, key);
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char extract[4096]; size_t eo = 0; extract[0] = '\0';
    char line[1024]; int in_ex = 0;
    while (fgets(line, sizeof line, f)) {
        if (strncmp(line, "## Extract", 10) == 0) { in_ex = 1; continue; }
        if (in_ex) {
            if (strncmp(line, "##", 2) == 0) break;
            for (char *c = line; *c && eo + 1 < sizeof extract; c++)
                extract[eo++] = (*c == '\n') ? ' ' : *c;
        }
    }
    fclose(f);
    extract[eo] = '\0';
    if (eo == 0) return 0;

    int nfacts = 0; size_t mo = 0;
    if (out_sz) out[0] = '\0';
    char *p = extract;
    while (*p) {
        char *q = p;
        while (*q && *q != '.' && *q != '!' && *q != '?') q++;
        size_t slen = (size_t)(q - p);
        if (slen > 4 && slen < 380) {
            char sent[400], nrm[400], canon[400], msg[256];
            memcpy(sent, p, slen); sent[slen] = '\0';
            normalize(sent, nrm, sizeof nrm);
            canonicalize_lang(b, nrm, canon, sizeof canon);
            msg[0] = '\0';
            if (extract_class_statement(b, canon, msg, sizeof msg, 1)) {
                const char *fact = msg;
                if (!strncmp(fact, "Learned: ", 9)) fact += 9;
                int flen = (int)strcspn(fact, ".");   /* drop the trailing period */
                if (mo + (size_t)flen + 4 < out_sz) {
                    mo += (size_t)snprintf(out + mo, out_sz - mo, "%s%.*s",
                                           nfacts ? ", " : "", flen, fact);
                    nfacts++;
                }
            }
        }
        if (!*q) break;
        p = q + 1;
    }
    return nfacts;
}

/* deep-reasoning M3: is `to` reachable from `from` over the binary relation `rel`?
 * A breadth-first walk of the rel edges — O(V+E), no explosion. A recursive
 * transitivity CLAUSE on the solver blows up exponentially on a query that FAILS
 * (the negative control) as SLD explores every path to KB_MAX_DEPTH; the C closure
 * is the same choice gen292 (equality) and gen233 (qchain) made for transitivity. */
static int deep_reachable(Brain *b, const char *rel, const char *from, const char *to) {
    char queue[64][64]; size_t qh = 0, qt = 0;
    char seen[64][64]; size_t ns = 0;
    snprintf(queue[qt++], 64, "%s", from);
    while (qh < qt) {
        char cur[64]; snprintf(cur, sizeof cur, "%s", queue[qh++]);
        if (!strcmp(cur, to)) return 1;
        int dup = 0;
        for (size_t i = 0; i < ns; i++) if (!strcmp(seen[i], cur)) { dup = 1; break; }
        if (dup) continue;
        if (ns < 64) snprintf(seen[ns++], 64, "%s", cur);
        char succ[32][KB_TERM_LEN];
        const char *qy[] = { cur, NULL };
        size_t k = kb_match(b->kb, rel, qy, 2, succ, 32);
        for (size_t i = 0; i < k && qt < 64; i++)
            snprintf(queue[qt++], 64, "%s", kb_dequote(succ[i]));
    }
    return 0;
}

/* deep-reasoning M3: from the facts extract_page_facts just learned about `concept`
 * (a "pred(a), pred(a, b), …" string), grow the frontier and, in class mode,
 * materialize is_a. located_in mode: the OBJECT of each located_in edge is a new
 * concept to explore. class mode: each unary class P(concept) becomes is_a(concept,
 * P) and P joins the frontier (to find P's own superclasses). */
static void deep_expand(Brain *b, const char *facts, int class_mode,
                        char frontier[][64], size_t *ftail, size_t fmax) {
    const char *p = facts;
    while (*p) {
        while (*p == ' ' || *p == ',') p++;
        const char *lp = p; while (*lp && *lp != '(') lp++;
        if (*lp != '(') break;
        char pred[KB_TERM_LEN]; size_t pl = (size_t)(lp - p);
        if (pl == 0 || pl >= sizeof pred) break;
        memcpy(pred, p, pl); pred[pl] = '\0';
        const char *ap = lp + 1; const char *rp = ap; int d = 1;
        while (*rp && d) { if (*rp == '(') d++; else if (*rp == ')') d--; if (d) rp++; }
        char inner[KB_TERM_LEN]; size_t il = (size_t)(rp - ap);
        if (il >= sizeof inner) il = sizeof inner - 1;
        memcpy(inner, ap, il); inner[il] = '\0';
        /* split inner on the top-level comma */
        char a0[KB_TERM_LEN] = "", a1[KB_TERM_LEN] = "";
        char *comma = strchr(inner, ',');
        if (comma) {
            *comma = '\0';
            snprintf(a0, sizeof a0, "%s", inner);
            const char *s = comma + 1; while (*s == ' ') s++;
            snprintf(a1, sizeof a1, "%s", s);
        } else {
            snprintf(a0, sizeof a0, "%s", inner);
        }
        if (!class_mode && a1[0] && !strcmp(pred, "located_in")) {
            if (*ftail < fmax) snprintf(frontier[(*ftail)++], 64, "%s", a1);
        } else if (class_mode && !a1[0] && a0[0]) {
            const char *ia[] = { a0, pred };            /* is_a(concept, class) */
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, "is_a", ia, 2);
            if (*ftail < fmax) snprintf(frontier[(*ftail)++], 64, "%s", pred);
        }
        p = (*rp == ')') ? rp + 1 : rp;
    }
}

/* deep-reasoning M3 (docs/plans/deep-reasoning.md §3): the budgeted inference LOOP.
 * "think deeply: is Paris in Europe?" -> parse the target, seed a frontier, then
 * repeatedly ACQUIRE facts from a concept's page (extract_page_facts, M2 — each with
 * its source, M1), apply the relation's transitivity (gen291), and expand the
 * frontier — until the target is provable (the conclusion EMERGED, multi-hop from
 * separate sources), the frontier empties (convergence), or the wall-clock budget
 * expires. Reports the conclusion PLUS the readable derivation trace; declines
 * honestly if it cannot derive it. Deterministic multi-hop, not generative CoT. */
static int mod_deep_reason(Brain *b, const char *norm, const char *raw,
                           char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;
    if (!kb_cue_match(b, "deep_reason_fresh", norm)) return 0;
    int it = kb_cue_match(b, "deep_reason_it", norm);

    char q[256]; snprintf(q, sizeof q, "%s", norm);
    char *w[40]; size_t n = split_words(q, w, 40);
    size_t ip = n;
    for (size_t i = 0; i < n; i++)
        if (!strcmp(strip_edge_punct(w[i]), "is")) { ip = i; break; }

    char subj[KB_TERM_LEN] = "", obj[KB_TERM_LEN] = "";
    const char *rel = NULL; int class_mode = 0;
    if (ip < n && ip + 2 < n) {
        size_t inp = n;
        for (size_t i = ip + 1; i < n; i++)
            if (!strcmp(strip_edge_punct(w[i]), "in")) { inp = i; break; }
        if (inp != n && inp > ip + 1 && inp + 1 < n &&
            p0_join(w, ip + 1, inp, subj, sizeof subj) &&
            p0_join(w, inp + 1, n, obj, sizeof obj)) {
            rel = "located_in"; class_mode = 0;
        } else {
            size_t s = ip + 1; if (s < n && is_article(w[s])) s++;
            size_t ap = n;
            for (size_t i = s + 1; i < n; i++)
                if (is_article(strip_edge_punct(w[i]))) { ap = i; break; }
            if (ap != n && ap > s && ap + 1 < n &&
                p0_join(w, s, ap, subj, sizeof subj) &&
                p0_join(w, ap + 1, n, obj, sizeof obj)) {
                rel = "is_a"; class_mode = 1;
            }
        }
    }
    if (!rel) {
        put(it ? "Dammi una domanda sì/no su cui ragionare -- es. \"pensaci a fondo: "
                 "parigi è in europa?\"."
               : "Give me a yes/no question to reason about -- e.g. \"think deeply: "
                 "is Paris in Europe?\".", out, out_size);
        return 1;
    }

    char frontier[24][64]; size_t fhead = 0, ftail = 0;
    char seen[24][64]; size_t nseen = 0;
    snprintf(frontier[ftail++], 64, "%s", subj);
    snprintf(frontier[ftail++], 64, "%s", obj);

    char trace[900]; size_t to = 0; trace[0] = '\0';
    int hops = 0, found = 0;
    time_t start = time(NULL);
    const char *bud = getenv("PARROT0_DEEP_BUDGET");
    long budget = bud && *bud ? atol(bud) : 60;      /* seconds; §3 default 60 */
    for (int iter = 0; iter < 40; iter++) {
        if (deep_reachable(b, rel, subj, obj)) { found = 1; break; }
        if (time(NULL) - start >= budget) break;
        if (fhead >= ftail) break;                   /* frontier empty: convergence */
        char c[64]; snprintf(c, sizeof c, "%s", frontier[fhead++]);
        int dup = 0;
        for (size_t i = 0; i < nseen; i++) if (!strcmp(seen[i], c)) { dup = 1; break; }
        if (dup) continue;
        if (nseen < 24) snprintf(seen[nseen++], 64, "%s", c);
        char facts[512];
        int nf = extract_page_facts(b, c, facts, sizeof facts);
        if (nf == 0) continue;
        if (to + strlen(facts) + 32 < sizeof trace)
            to += (size_t)snprintf(trace + to, sizeof trace - to,
                                   "%slearned %s (source: %s)", hops ? "; " : "",
                                   facts, c);
        hops++;
        deep_expand(b, facts, class_mode, frontier, &ftail, 24);
    }

    /* M4 (§4bis): self-correction. "located in" is a strict containment order, so a
     * CYCLE is impossible — a sign the broad extraction (§4.4) learned a wrong edge
     * from an ambiguous fragment. On a 2-cycle A<->B the loop RETURNS TO THE SOURCE
     * (fact_source, M1): it keeps the edge that serves the target derivation and
     * retracts the other (the reversed/back edge), then re-derives. Resilience, not
     * failure — the contradiction is the SIGNAL. (Located-in only for now.) */
    char correction[420]; correction[0] = '\0';
    if (!class_mode) {
        for (size_t i = 0; i < nseen && !correction[0]; i++) {
            char succ[32][KB_TERM_LEN];
            const char *sq0[] = { seen[i], NULL };
            size_t k = kb_match(b->kb, rel, sq0, 2, succ, 32);
            for (size_t j = 0; j < k && !correction[0]; j++) {
                char Y[64]; snprintf(Y, sizeof Y, "%s", kb_dequote(succ[j]));
                if (!strcmp(Y, seen[i])) continue;
                const char *back[] = { Y, seen[i] };
                if (!kb_query(b->kb, rel, back, 2)) continue;   /* no back edge */
                /* 2-cycle seen[i] <-> Y. Adjudicate: retract the edge whose removal
                 * keeps the target reachable; that is the wrong (back) edge. */
                const char *e1[] = { Y, seen[i] };              /* Y -> seen[i] */
                kb_retract(b->kb, rel, e1, 2);
                int t1 = deep_reachable(b, rel, subj, obj);
                kb_assert(b->kb, rel, e1, 2);                   /* restore to decide */
                const char *sa = t1 ? Y : seen[i];
                const char *sb = t1 ? seen[i] : Y;
                char fr[KB_TERM_LEN];
                snprintf(fr, sizeof fr, "%s(%s, %s)", rel, sa, sb);
                char src[1][KB_TERM_LEN], srctxt[220] = "the source";
                const char *fq[] = { fr, sa, NULL };
                if (kb_match(b->kb, "fact_source", fq, 3, src, 1) == 1) {
                    char raw[KB_TERM_LEN]; snprintf(raw, sizeof raw, "%s", src[0]);
                    snprintf(srctxt, sizeof srctxt, "%s", kb_dequote(raw));
                    const char *fsr[] = { fr, sa, src[0] };     /* drop its provenance too */
                    kb_retract(b->kb, "fact_source", fsr, 3);
                }
                const char *susp[] = { sa, sb };
                kb_retract(b->kb, rel, susp, 2);                /* drop the wrong edge */
                snprintf(correction, sizeof correction, it
                    ? " Strada facendo ho trovato una contraddizione: \"si trova in\" "
                      "non può fare cicli (%s non può stare dentro %s ed insieme "
                      "contenerlo), quindi sono tornato alla fonte \"%s\" e ho "
                      "ritirato l'arco sbagliato %s."
                    : " Along the way I hit a contradiction: \"located in\" can't "
                      "cycle (%s can't both be inside and contain %s), so I went back "
                      "to the source \"%s\" and retracted the wrong edge %s.",
                    sa, sb, srctxt, fr);
            }
        }
    }

    if (!found) found = deep_reachable(b, rel, subj, obj);

    char tgt[200];
    if (class_mode) snprintf(tgt, sizeof tgt, it ? "%s è un %s" : "a %s is a %s", subj, obj);
    else            snprintf(tgt, sizeof tgt, it ? "%s è in %s" : "%s is in %s", subj, obj);

    char msg[1200];
    if (found)
        snprintf(msg, sizeof msg, it
                 ? "Sì -- %s. L'ho derivato in %d pass%s: %s; poi per transitività "
                   "di \"%s\" ne segue che %s."
                 : "Yes -- %s. I derived it across %d hop%s: %s; then by transitivity "
                   "of \"%s\" it follows that %s.",
                 tgt, hops, it ? (hops == 1 ? "o" : "i") : (hops == 1 ? "" : "s"),
                 trace, class_mode ? (it ? "è un" : "is a") : (it ? "si trova in" : "located in"),
                 tgt);
    else if (hops > 0)
        snprintf(msg, sizeof msg, it
                 ? "Ho letto %d font%s ma non ho potuto derivare se %s. Ho imparato: %s."
                 : "I read %d source%s but couldn't derive whether %s. I learned: %s.",
                 hops, it ? (hops == 1 ? "e" : "i") : (hops == 1 ? "" : "s"), tgt, trace);
    else
        snprintf(msg, sizeof msg, it
                 ? "Non ho trovato una fonte da cui ragionare su %s."
                 : "I couldn't find a source to reason about %s from.", tgt);
    if (correction[0]) strncat(msg, correction, sizeof msg - strlen(msg) - 1);
    put(msg, out, out_size);
    return 1;
}

static int mod_learn(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    if (!b) return 0;
    char buf[256];
    canonicalize_lang(b, norm, buf, sizeof buf);

    /* gen335e: Italian heads containing "di"/"a"/"in" get broken by
     * canonicalization ("di"→"of"). Also try matching against the raw
     * normalized form so "parlami di X" matches "parlami di " even when
     * the canon form is "parlami of X". */
    char rawbuf[256] = "";
    normalize(raw && *raw ? raw : norm, rawbuf, sizeof rawbuf);

    /* Definitional gap: extract the topic X. STRONG heads (article / "about" /
     * "who" / Italian) signal definitional intent and allow a multi-word topic;
     * the WEAK bare "what is <X>" is ambiguous with arithmetic ("what is 2 + 2",
     * "what is gold plus silver"), so it is accepted only for a single concept
     * word. Arithmetic operators and digits are always rejected, so compute-style
     * questions keep walling as before. */
    char work[256]; snprintf(work, sizeof work, "%s", buf);
    size_t wl = strlen(work);
    while (wl > 0 && (work[wl-1] == '?' || work[wl-1] == ' ')) work[--wl] = '\0';
    const char *x = NULL;
    int weak = 0;
    int use_raw = 0;  /* gen335e: flag: extract topic from raw form */
    static const char *const strong_heads[] = {
        "what is a ", "what is an ", "tell me about ",
        "what do you know about ", "who is ", "who was ",
        /* gen240: explicit IMPERATIVE to acquire — a first-class command (or
         * planner step) to run the discovery plan, not just a question. */
        "learn about ", "research ", "look up ", "study ", "read up on ",
        "find out about ", "go learn about ",
        /* deep-reasoning M2: the DEEP read — extract every fact from the page's
         * prose, not just the lead concept. Anchored heads so it is unambiguous. */
        "read the page on ", "read the page about ", "extract facts from ",
        "read everything about ", "leggi la pagina su ", "estrai i fatti da ",
        "cos'è un ", "cos'è una ", "cos'è uno ", "che cos'è ", "cos'è ",
        /* gen242: "cosa è X" / "che cosa è X" — the spelled-out variant of cos'è.
         * canonicalize_lang maps "è" -> "is" (but leaves "cosa"), so the head we
         * must match is the post-canonical "cosa is " / "che cosa is ". */
        "che cosa is ", "cosa is ",
        "parlami di ", "cosa sai di ", "chi è ", "impara ", "studia ",
        "informati su ", "documentati su ", NULL,
    };
    static const char *const weak_heads[] = {
        "what is ", "what are ", "what was ", NULL,
    };
    /* gen243: answer in the language of the QUESTION (which head matched), not the
     * flaky session-language detector — an English "tell me about foo" must reply in
     * English even if marker counting guessed Italian. The Italian heads are the
     * ones carrying an Italian-only token; flag the match. */
    const char *matched = NULL;
    for (const char *const *h = strong_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; matched = *h; break; }
    }
    /* gen335e: if canonical form didn't match, try raw normalized form.
     * Italian heads like "parlami di " are broken by "di"→"of" canonicalization.
     * The raw form preserves "di" and matches the original head. */
    if (!x && rawbuf[0]) {
        for (const char *const *h = strong_heads; *h; h++) {
            size_t hl = strlen(*h);
            if (strncmp(rawbuf, *h, hl) == 0) {
                x = rawbuf + hl; matched = *h; use_raw = 1; break;
            }
        }
    }
    if (!x) for (const char *const *h = weak_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; weak = 1; matched = *h; break; }
    }
    if (!x || !*x) return 0;
    int it = matched && (strstr(matched, "cos") || strstr(matched, "cosa") ||
                         strstr(matched, "parlami") || strstr(matched, "chi ") ||
                         strstr(matched, "impara") || strstr(matched, "studia") ||
                         strstr(matched, "informati") || strstr(matched, "documentati") ||
                         strstr(matched, "leggi") || strstr(matched, "estrai"));

    /* Build the concept key (drop a leading article, join words with '_') and a
     * display form; guard pronouns and too-short topics. */
    char xbuf[160]; snprintf(xbuf, sizeof xbuf, "%s", x);
    char *tok[16]; size_t nt = split_words(xbuf, tok, 16);
    size_t start = 0;
    if (nt > 0 && (is_article(tok[0]) ||
                   !strcmp(tok[0],"the") || !strcmp(tok[0],"il") ||
                   !strcmp(tok[0],"la")  || !strcmp(tok[0],"lo") ||
                   !strcmp(tok[0],"un")  || !strcmp(tok[0],"una") ||
                   !strcmp(tok[0],"uno") || !strcmp(tok[0],"i") ||
                   !strcmp(tok[0],"gli") || !strcmp(tok[0],"le")))
        start = 1;
    if (start >= nt) return 0;
    if (is_entity_pronoun(tok[start])) return 0;
    /* A bare "what is <X>" is only definitional for a single concept word. */
    if (weak && nt - start != 1) return 0;
    /* Reject arithmetic / numeric expressions in any head (they should wall). */
    for (size_t i = start; i < nt; i++) {
        char *t = strip_edge_punct(tok[i]);
        for (char *c = t; *c; c++) if (isdigit((unsigned char)*c)) return 0;
        if (!strcmp(t,"plus")||!strcmp(t,"minus")||!strcmp(t,"times")||
            !strcmp(t,"divided")||!strcmp(t,"over")||!strcmp(t,"equals")||
            !strcmp(t,"più")||!strcmp(t,"piu")||!strcmp(t,"meno")||
            !strcmp(t,"per")||!strcmp(t,"diviso"))
            return 0;
    }

    /* gen335e: when head matched against raw Italian form, canonicalize each
     * topic token to English so the concept key is canonical (chess, not scacchi). */
    if (use_raw) {
        for (size_t i = start; i < nt; i++) {
            char *t = strip_edge_punct(tok[i]);
            char canon_tok[KB_TERM_LEN];
            if (kb_tr_it_en(b, t, canon_tok, sizeof canon_tok)) {
                /* tok[i] points into xbuf — overwrite with canonical form */
                size_t cl = strlen(canon_tok);
                if (cl >= strlen(tok[i])) cl = strlen(tok[i]) - 1;
                memcpy(tok[i], canon_tok, cl);
                tok[i][cl] = '\0';
            }
        }
    }
    char key[80]; size_t ko = 0;
    char disp[80]; size_t dpo = 0;
    for (size_t i = start; i < nt; i++) {
        char *t = strip_edge_punct(tok[i]);
        if (!*t) continue;
        ko  += (size_t)snprintf(key  + ko,  sizeof key  - ko,  "%s%s", ko ? "_" : "", t);
        dpo += (size_t)snprintf(disp + dpo, sizeof disp - dpo, "%s%s", dpo ? " " : "", t);
        if (ko >= sizeof key - 8) break;
    }
    if (ko < 3) return 0;
    if (weak && strlen(tok[start]) < 3) return 0;  /* gen335e: allow 3-char topics (DNA, ACL) */

    /* deep-reasoning M2: a DEEP read extracts every fact from the page's prose
     * (extract_page_facts), each with its source (M1) — distinct from the shallow
     * concept-learn below. Honest miss if the page has no page or no facts. */
    int deep = matched && (strstr(matched, "read the page") ||
                           strstr(matched, "extract facts") ||
                           strstr(matched, "read everything") ||
                           strstr(matched, "leggi la pagina") ||
                           strstr(matched, "estrai i fatti"));
    if (deep) {
        char facts[512];
        int nf = extract_page_facts(b, key, facts, sizeof facts);
        char dmsg[700];
        if (nf > 0)
            snprintf(dmsg, sizeof dmsg,
                     it ? "Dalla pagina su %s ho estratto %d fatti: %s."
                        : "From the %s page I extracted %d facts: %s.",
                     disp, nf, facts);
        else
            snprintf(dmsg, sizeof dmsg,
                     it ? "Non ho una pagina con fatti estraibili su %s."
                        : "I don't have a page with extractable facts on %s.",
                     disp);
        put(dmsg, out, out_size);
        return 1;
    }

    char def[KB_TERM_LEN];
    char msg[320];

    /* gen240 (universal-comprehension §7): pursue the precondition know(X) via the
     * acquire-knowledge action — already in RAM, learned from the local certified
     * corpus, or fetched on demand from Wikipedia (all in C). On a miss, give the
     * INFORMED decline (§2): name what was understood and be honest it can learn —
     * never a blind "I don't understand". */
    int st = acquire_knowledge(b, key, def, sizeof def);
    if (st == 2)
        snprintf(msg, sizeof msg,
                 it ? "Mi ero già documentato su %s: %s."
                    : "I already read up on %s: %s.", disp, def);
    else if (st == 1)
        snprintf(msg, sizeof msg,
                 it ? "Non conoscevo %s, così mi sono documentato: %s."
                    : "I didn't know about %s, so I just read it up: %s.", disp, def);
    else {
        /* gen335d (linguistic glue, KB-first): the informed decline now offers
         * to learn. gen335e: skip if this topic was already tried and failed. */
        kb_set_origin(b->kb, KB_REFLECTIVE);
        const char *gq[] = { NULL };
        char gcheck[1][KB_TERM_LEN];
        int already_gap = kb_match(b->kb, "pending_gap", gq, 1, gcheck, 1) > 0;
        const char *fq[] = { key, NULL };
        int already_failed = kb_query(b->kb, "pending_gap_failed", fq, 1);
        if (!already_gap && !already_failed) {
            const char *ga[] = { key };
            kb_assert(b->kb, "pending_gap", ga, 1);
            char qq[KB_TERM_LEN];
            /* gen335e: store the RAW question so dispatch_one can re-canonicalize
             * with full language context. The canonical form loses Italian prepositions
             * ("di"→"of") that strong heads depend on. */
            snprintf(qq, sizeof qq, "\"%s\"", raw && *raw ? raw : norm);
            const char *qa[] = { qq };
            kb_assert(b->kb, "pending_gap_question", qa, 1);
            snprintf(msg, sizeof msg,
                     it ? "Ho capito che mi chiedi di %s: ho provato a documentarmi, "
                           "ma non ho ancora trovato una fonte. Vuoi che impari "
                           "dalle pagine disponibili?"
                        : "I understood you're asking about %s: I tried to look it up, "
                          "but I don't have a source yet. Want me to learn about it "
                          "from available pages?", disp);
        } else {
            snprintf(msg, sizeof msg,
                     it ? "Ho capito che mi chiedi di %s: ho provato a documentarmi, ma "
                           "non ho ancora trovato una fonte su cui impararlo."
                        : "I understood you're asking about %s: I tried to look it up, "
                          "but I don't have a source to learn it from yet.", disp);
        }
        kb_set_origin(b->kb, KB_SESSION);
    }
    put(msg, out, out_size);
    return 1;
}

/* gen325: a faculty id from the ledger (multi_file_editing) as English
 * (multi file editing). The C knows the SHAPE of an identifier, never the list
 * of faculties — those are facts. */
static void self_readable(char *dst, size_t dsz, const char *id) {
    size_t i = 0;
    for (; id[i] && i + 1 < dsz; i++)
        dst[i] = (id[i] == '_') ? ' ' : id[i];
    dst[i] = '\0';
}

/* gen325: the wall that blocks a faculty's next level, read from the KB. The
 * .p0 stores it quoted; strip the quotes as kb_cue_match does. */
static int self_capability_wall(Brain *b, const char *id, char *dst, size_t dsz) {
    if (!b || !b->kb) return 0;
    char walls[1][KB_TERM_LEN];
    const char *q[2] = { id, NULL };
    if (kb_match(b->kb, "capability_wall", q, 2, walls, 1) == 0) return 0;
    char *p = walls[0];
    size_t l = strlen(p);
    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
    if (!*p) return 0;
    snprintf(dst, dsz, "%s", p);
    return 1;
}

/* --- module: self --------------------------------------------------------
 * Identity & self-reflection (PRINCIPLES.md, "I know that I am"). The agent's
 * self-model lives in the very same KB it uses for the world: `i_am(parrot0).`
 * and one `module(<name>)` per registered part (asserted at birth — see
 * brain_create). This module answers introspective questions by *querying that
 * model*, so the answers are derived from real state, never hard-coded.
 *
 * gen325 (forge §18): it also knows where it ENDS — capability/2 and
 * capability_wall/2, projected from the ledger that the gates verify.
 */
static int mod_self(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    /* gen325: `raw` is now READ (the self_limits branch below): normalization
     * erases the negation that distinguishes "what can you do" from "what can
     * you NOT do", so the negative reading must see the user's own words. */
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    /* gen334: identity cues in the KB may be in the user's original language
     * (e.g. "come ti chiami") — canonicalization would have folded function
     * words, so match against the raw normalized input too. Non-canonicalized
     * raw forms are never modified, so Italian intent_cue entries stay intact. */
    char ribuf[256];
    normalize(raw, ribuf, sizeof ribuf);
    len = strlen(ribuf);
    if (len > 0 && ribuf[len - 1] == '?') ribuf[--len] = '\0';
    while (len > 0 && ribuf[len - 1] == ' ') ribuf[--len] = '\0';

    const char *var[] = {NULL};

    /* gen51 (C1): recognize intent by KEYWORD CUES, not one rigid template, so
     * many phrasings of the same question land. The answer is still derived from
     * the real self-model (i_am / module facts), never canned — robustness comes
     * from cue matching + the KB, not from a list of accepted sentences. */
    int identity = kb_cue_match(b, "50_self_research_loop_chain439", buf) ||
                   kb_cue_match(b, "50_self_research_loop_chain439", ribuf);
    int exists = kb_cue_match(b, "50_self_research_loop_chain448", buf) ||
                 kb_cue_match(b, "50_self_research_loop_chain448", ribuf);
    int capability = kb_cue_match(b, "50_self_research_loop_chain450", buf) ||
                     kb_cue_match(b, "50_self_research_loop_chain450", ribuf);
    /* gen240: "if you're happy and you know it, what do you do?" is a song, not a
     * capability query — a leading conditional means it isn't asking about me. */
    if (capability && (kb_cue_match(b, "50_self_research_loop_chain458", buf)))
        capability = 0;

    /* gen325 (TODO.md P6, forge plan §18): WHERE I END.
     *
     * Asked what it could not do, parrot0 answered "I am parrot0." — the identity
     * module took the turn — and in Italian ("cosa non sai fare?") it listed what
     * it CAN do, which is worse than a wall: the question was inverted and the
     * answer was still a brochure.
     *
     * It had no notion of its own envelope, because the self-model derived only
     * from module(name): a module either exists or it does not, and that says
     * nothing about how FAR it reaches. The capability ledger already knew —
     * maturity plus the wall that blocks the next level, verified by
     * `make capability-report` against real gate results. gen325 projects it into
     * knowledge (kb/core/capabilities.p0, generated) and answers from THOSE facts.
     *
     * So this is derived, never written: no faculty name appears in this C. If a
     * capability regresses, the report downgrades it and parrot0's answer shrinks
     * with it. A self-description that cannot shrink is a brochure.
     *
     * Tested BEFORE capability/identity: "what are you unable to do" contains the
     * cues of both, and the negative reading is the specific one.
     *
     * Matched on the RAW turn, not on `norm`: normalization ERASES the negation
     * (it canonicalizes "cosa non sai fare" and "cosa sai fare" to the same
     * thing, which is exactly why the Italian question got the capability list
     * and the apostrophe in "can't" was lost). The negation IS the signal here,
     * so this reads the words the user actually typed. */
    char rawlow[256];
    { size_t i = 0;
      for (; raw && raw[i] && i + 1 < sizeof rawlow; i++)
          rawlow[i] = (char)tolower((unsigned char)raw[i]);
      rawlow[i] = '\0';
      while (i > 0 && (rawlow[i-1] == '?' || rawlow[i-1] == ' ')) rawlow[--i] = '\0'; }

    if (kb_cue_match(b, "self_limits", rawlow)) {
        char absent[12][KB_TERM_LEN], seed[12][KB_TERM_LEN];
        const char *qa[2] = { NULL, "absent" };
        const char *qs[2] = { NULL, "seed" };
        size_t na = kb_match(b->kb, "capability", qa, 2, absent, 12);
        size_t ns = kb_match(b->kb, "capability", qs, 2, seed, 12);
        if (na == 0 && ns == 0) return 0;   /* no ledger loaded -> claim nothing */

        char body[900];
        size_t off = (size_t)snprintf(body, sizeof body,
            "From my capability ledger, which is generated from real test results "
            "and not a description I wrote about myself: ");

        if (na > 0 && off < sizeof body) {
            off += (size_t)snprintf(body + off, sizeof body - off, "I cannot do ");
            for (size_t i = 0; i < na && off < sizeof body; i++) {
                char nm[KB_TERM_LEN]; self_readable(nm, sizeof nm, absent[i]);
                char wall[KB_TERM_LEN];
                int haswall = self_capability_wall(b, absent[i], wall, sizeof wall);
                off += (size_t)snprintf(body + off, sizeof body - off,
                    "%s%s%s%s%s", (i == 0) ? "" : (i + 1 == na) ? " or " : ", ",
                    nm, haswall ? " (it would need " : "",
                    haswall ? wall : "", haswall ? ")" : "");
            }
            if (off < sizeof body)
                off += (size_t)snprintf(body + off, sizeof body - off, ". ");
        }
        if (ns > 0 && off < sizeof body) {
            off += (size_t)snprintf(body + off, sizeof body - off,
                "These I can only do as a seed — one demonstrated case, not "
                "reliably: ");
            for (size_t i = 0; i < ns && off < sizeof body; i++) {
                char nm[KB_TERM_LEN]; self_readable(nm, sizeof nm, seed[i]);
                off += (size_t)snprintf(body + off, sizeof body - off, "%s%s",
                    (i == 0) ? "" : (i + 1 == ns) ? " and " : ", ", nm);
            }
            if (off < sizeof body)
                snprintf(body + off, sizeof body - off, ".");
        }
        put(body, out, out_size);
        store_proof(b, "Derived from capability/2 and capability_wall/2 in the KB "
                       "(kb/core/capabilities.p0, generated from gate results).");
        return 1;
    }

    /* capability is the more specific intent ("what are you ABLE TO DO" also
     * contains the identity cue "what are you"), so test it first. Describe what
     * the registered modules let me do, in plain language (not the module(...)
     * jargon) — grounded in the real module set, a module absent contributes
     * nothing. */
    if (capability) {
        static const struct { const char *mod, *say; } cap[] = {
            {"knowledge", "answer questions about facts and rules"},
            {"arith",     "do simple arithmetic"},
            {"cause",     "reason about cause and effect"},
            {"compare",   "compare quantities"},
            {"same",      "tell whether two things are the same"},
            {"memory",    "remember things you tell me"},
            {"reader",    "read a short passage and pull out facts"},
            {"coref",     "track what a pronoun refers to"},
            {"gen",       "continue a sequence you teach me"},
            {"discourse", "remember what we talked about"},
        };
        char list[600];
        size_t off = 0, n = 0;
        for (size_t i = 0; i < sizeof cap / sizeof cap[0]; i++) {
            const char *m[] = {cap[i].mod};
            if (!kb_query(b->kb, "module", m, 1)) continue;
            const char *sep = (n == 0) ? "" : ", ";
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", sep, cap[i].say);
            n++;
        }
        char msg[700];
        if (n == 0) snprintf(msg, sizeof msg, "Not much yet.");
        else snprintf(msg, sizeof msg, "I can %s.", list);
        put(msg, out, out_size);
        store_proof(b, "This is derived from my registered module list.");
        return 1;
    }

    if (identity || exists) {
        char id[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        char proof[160];
        if (k == 0) snprintf(msg, sizeof msg, "I don't know what I am.");
        else if (exists) {
            snprintf(msg, sizeof msg, "Yes, I am %s.", id[0]);
            snprintf(proof, sizeof proof, "i_am(%s) is a reflective fact in my knowledge base.", id[0]);
        }
        else {
            snprintf(msg, sizeof msg, "I am %s.", id[0]);
            snprintf(proof, sizeof proof, "i_am(%s) is a reflective fact in my knowledge base.", id[0]);
        }
        put(msg, out, out_size);
        if (k > 0) store_proof(b, proof);
        return 1;
    }

    /* gen77: self-model introspection — the architecture made queryable.
     * Each cue is guarded by word count so full queries (e.g. "what do you
     * know about X?") fall through to mod_knowledge unchanged. */

    /* Quick word-count helper (buf is stripped of trailing punctuation). */
    size_t wn = 0, inw = 0;
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == ' ') inw = 0;
        else if (!inw) { wn++; inw = 1; }
    }

    /* "how many facts do you know?" → kb_user_facts */
    int fact_count = (cue(buf, "how many facts") && wn <= 6) ||
                     (cue(buf, "quanti fatti") && wn <= 5);
    if (fact_count) {
        char msg[128];
        snprintf(msg, sizeof msg, "I know %zu fact(s).", kb_user_facts(b->kb));
        put(msg, out, out_size);
        return 1;
    }

    /* "what predicates do you know?" / "what topics do you know about?" */
    int pred_list = (cue(buf, "what predicates") && wn <= 5) ||
                    (cue(buf, "what topics") && wn <= 6) ||
                    (cue(buf, "quali predicati") && wn <= 4);
    if (pred_list) {
        char preds[128][KB_TERM_LEN];
        size_t np = kb_user_predicates(b->kb, preds, 128);
        if (np == 0) { put("I don't know any predicates yet.", out, out_size); return 1; }
        char list[1024];
        size_t off = 0;
        for (size_t i = 0; i < np && off < sizeof list; i++)
            off += (size_t)snprintf(list + off, sizeof list - off,
                                     "%s%s", i ? ", " : "", preds[i]);
        char msg[1100];
        if (off < sizeof list)
            snprintf(msg, sizeof msg, "I know these predicates: %s.", list);
        else
            snprintf(msg, sizeof msg, "I know %zu distinct predicate(s).", np);
        put(msg, out, out_size);
        return 1;
    }

    /* "show me your knowledge" / "dump everything" */
    int show_knowledge = (cue(buf, "show me your knowledge") && wn <= 5) ||
                         (cue(buf, "show me what you know") && wn <= 7) ||
                         (cue(buf, "dump everything") && wn <= 3) ||
                         (cue(buf, "dump what you know") && wn <= 6) ||
                         (cue(buf, "mostrami la conoscenza") && wn <= 4);
    if (show_knowledge) {
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            snprintf(msg, sizeof msg, "Here is everything I know: %s", dump);
            put(msg, out, out_size);
        } else {
            char msg[128];
            snprintf(msg, sizeof msg, "I know %zu fact(s) total.", kb_user_facts(b->kb));
            put(msg, out, out_size);
        }
        return 1;
    }

    /* "what do you know?" → stats overview. Only claims short forms;
     * "what do you know about X?" has more words and reaches mod_knowledge. */
    int what_know = (cue(buf, "what do you know") && wn <= 4) ||
                    (cue(buf, "cosa sai") && wn <= 3);
    if (what_know) {
        size_t nfacts = kb_user_facts(b->kb);
        char preds[128][KB_TERM_LEN];
        size_t np = kb_user_predicates(b->kb, preds, 128);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I know %zu fact(s) across %zu predicate(s). Ask me about a specific topic.",
                 nfacts, np);
        put(msg, out, out_size);
        return 1;
    }

    /* gen78: "which part of you answered that?" / "what module handled that?"
     * Reads from the last_module stored by the dispatch loop. */
    int which_module = (cue(buf, "which part of you") && wn <= 6) ||
                       (cue(buf, "what module") && wn <= 4) ||
                       (cue(buf, "who answered") && wn <= 4) ||
                       (cue(buf, "quale parte di te") && wn <= 6) ||
                       (cue(buf, "quale modulo") && wn <= 4);
    if (which_module) {
        if (b->last_module[0]) {
            char msg[160];
            if (strcmp(b->last_module, "fallback") == 0)
                snprintf(msg, sizeof msg,
                         "No module could handle that — it fell through to "
                         "the not-understood fallback.");
            else
                snprintf(msg, sizeof msg,
                         "The '%s' module answered your last question.",
                         b->last_module);
            put(msg, out, out_size);
        } else {
            put("I haven't answered anything yet.", out, out_size);
        }
        return 1;
    }

    /* gen83 entities_q ... (above) */

    /* gen86 mod_cap ... (above) */

    /* gen89: "what have I taught you?" — show only session facts. */
    int taught_q = (cue(buf, "what have i taught you") && wn <= 6) ||
                   (cue(buf, "what did i teach you") && wn <= 6) ||
                   (cue(buf, "cosa ti ho insegnato") && wn <= 5);
    if (taught_q) {
        /* Dump only user-facing predicates, regardless of origin.
         * kb_user_facts already filters internal predicates. */
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            snprintf(msg, sizeof msg, "You taught me: %s", dump);
            put(msg, out, out_size);
        } else {
            put("You haven't taught me any facts yet.", out, out_size);
        }
        return 1;
    }
    int mod_cap = (cue(buf, "what can the") && wn <= 7) ||
                  (cue(buf, "what does the") && wn <= 7) ||
                  (cue(buf, "cosa può fare il modulo") && wn <= 7);
    if (mod_cap) {
        if (cue(buf, "what does the word") || cue(buf, "what does word"))
            return 0;
        static const struct { const char *mod, *say; } cmap[] = {
            {"knowledge", "answer questions about facts and logical rules"},
            {"arith",     "compute arithmetic (plus, minus, times, divisible by)"},
            {"cause",     "reason about cause and effect relations"},
            {"compare",   "compare magnitudes (more/less than)"},
            {"memory",    "remember your name, possessions, and personal facts"},
            {"reader",    "read a passage and extract known fact patterns"},
            {"shell",     "explain shell commands and predict their output"},
            {"gen",       "generate text continuations from learned sequences"},
            {"self",      "answer questions about my own identity and capabilities"},
            {"meta",      "handle meta-conversation (attention, understanding)"},
            {"discourse", "remember what topics we discussed"},
            {"social",    "handle greetings, thanks, and social conventions"},
            {"symbolic",  "recognize symbolic patterns (Morse, leet, palindromes)"},
        };
        for (size_t i = 0; i < sizeof cmap / sizeof cmap[0]; i++) {
            if (cue(buf, cmap[i].mod)) {
                char msg[256];
                snprintf(msg, sizeof msg, "The %s module can %s.",
                         cmap[i].mod, cmap[i].say);
                put(msg, out, out_size);
                return 1;
            }
        }
        put("I don't have a module by that name. Ask 'what can you do?' for a list.",
            out, out_size);
        return 1;
    }
    int entities_q = (cue(buf, "what names have i") && wn <= 6) ||
                     (cue(buf, "who have i mentioned") && wn <= 5) ||
                     (cue(buf, "who have i talked about") && wn <= 7) ||
                     (cue(buf, "quali nomi ho menzionato") && wn <= 5);
    if (entities_q) {
        if (b->entity_count == 0) {
            put("You haven't mentioned any names I recognized.", out, out_size);
        } else {
            char list[512]; size_t off = 0;
            for (size_t i = 0; i < b->entity_count && off < sizeof list; i++)
                off += (size_t)snprintf(list + off, sizeof list - off,
                                         "%s%s", i ? ", " : "", b->entities[i]);
            char msg[600];
            snprintf(msg, sizeof msg, "You mentioned: %s.", list);
            put(msg, out, out_size);
        }
        return 1;
    }

    return 0;
}

/* --- module: shell -------------------------------------------------------
 * POSIX/shell knowledge — Mission M1, step 1 (gen53) + step 2 (gen61).
 * Answers "what does <cmd> do?" / "explain <cmd>" by PARSING the command line
 * into (command, flags, args) and COMPOSING the answer from learned `cmd`/`flag`
 * facts (kb/experts/programming/bash.p0, carried in the commits) — so "ls -la" is explained
 * by composing ls + l + a even though that combination is not stored.
 *
 * gen61 extends this to simple PIPELINES: "cmd1 | cmd2" is explained by
 * describing each segment and joining them with "then". This is still shell
 * *structure*, not a dictionary. Reads `raw`, not `norm`: the shell is
 * case-sensitive (-r != -R), so flag case must survive normalization. */
static void de_underscore(const char *in, char *out, size_t n) {
    size_t i = 0;
    for (; in[i] && i + 1 < n; i++) out[i] = (in[i] == '_') ? ' ' : in[i];
    out[i] = '\0';
}

/* Describe a single shell command line (no pipeline) into `desc`.
 * Returns 1 if it wrote a description or a clear "unknown command" admission,
 * 0 if the input is not clearly shell syntax and should be declined. */
static int describe_command(Brain *b, const char *cmdline,
                            char *desc, size_t desc_size) {
    char clbuf[256];
    snprintf(clbuf, sizeof clbuf, "%s", cmdline);
    char *w[64];
    size_t nw = split_words(clbuf, w, 64);
    if (nw == 0) return 0;

    const char *command = NULL;
    int has_flag = 0;
    for (size_t i = 0; i < nw; i++) {
        if (w[i][0] == '-') has_flag = 1;
        else if (!command) command = w[i];
    }
    if (!command) return 0;

    /* command name is matched lowercased (commands are lowercase); look it up */
    char lc[KB_TERM_LEN];
    size_t ci = 0;
    for (; command[ci] && ci + 1 < sizeof lc; ci++)
        lc[ci] = (char)tolower((unsigned char)command[ci]);
    lc[ci] = '\0';

    const char *cpat[] = {lc, NULL};
    char eff[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cmd", cpat, 2, eff, 4);
    if (k == 0) {
        /* unknown command: only claim the turn when it is clearly shell syntax
         * (a flag is present), so we don't hijack "what does a bird do?". */
        if (!has_flag) return 0;
        snprintf(desc, desc_size, "I don't know the command %s.", lc);
        return 1;
    }

    char base[256];
    de_underscore(eff[0], base, sizeof base);

    char known[512]; size_t ko = 0, kn = 0;
    char unknown[160]; size_t uo = 0, un = 0;
    for (size_t i = 0; i < nw; i++) {
        if (w[i][0] != '-') continue;
        if (w[i][1] == '-') { /* long option: keep whole, do not split chars */
            uo += (size_t)snprintf(unknown + uo, sizeof unknown - uo,
                                   "%s%s", un ? ", " : "", w[i]);
            un++;
            continue;
        }
        for (const char *f = w[i] + 1; *f; f++) {
            /* the resolver reads an uppercase-initial atom as a variable, so an
             * uppercase flag (-R) is looked up case-tagged as "u_r". */
            char fs[8];
            if (isupper((unsigned char)*f))
                snprintf(fs, sizeof fs, "u_%c", (char)tolower((unsigned char)*f));
            else
                snprintf(fs, sizeof fs, "%c", *f);
            const char *fpat[] = {lc, fs, NULL};
            char fe[4][KB_TERM_LEN];
            if (kb_match(b->kb, "flag", fpat, 3, fe, 4) > 0) {
                char ph[160]; de_underscore(fe[0], ph, sizeof ph);
                ko += (size_t)snprintf(known + ko, sizeof known - ko,
                                       "%s%s", kn ? ", " : "", ph);
                kn++;
            } else {
                uo += (size_t)snprintf(unknown + uo, sizeof unknown - uo,
                                       "%s-%c", un ? ", " : "", *f);
                un++;
            }
        }
    }

    size_t o = (size_t)snprintf(desc, desc_size, "%s %s", lc, base);
    if (kn) o += (size_t)snprintf(desc + o, desc_size - o, ", %s", known);
    if (o < desc_size) o += (size_t)snprintf(desc + o, desc_size - o, ".");
    if (un && o < desc_size)
        snprintf(desc + o, desc_size - o,
                 " I don't know the option %s.", unknown);
    return 1;
}

/* gen62: oracle-grounded output prediction for a small allow-list of PURE
 * shell commands. Only active when PARROT0_ORACLE=1, so the default build never
 * executes arbitrary shell code. */

/* True if s contains only safe shell token characters (alphanumerics, -, _, .). */
static int safe_token(const char *s) {
    for (size_t i = 0; s[i]; i++) {
        if (isalnum((unsigned char)s[i])) continue;
        if (strchr("-_.", s[i])) continue;
        return 0;
    }
    return *s != '\0';
}

/* True if every command in the pipeline is in the pure allow-list. */
static int pipeline_is_pure(char **segs, size_t nseg) {
    static const char *pure[] = {"echo", "wc", "cat", "pwd", NULL};
    for (size_t i = 0; i < nseg; i++) {
        char buf[256];
        snprintf(buf, sizeof buf, "%s", segs[i]);
        char *w[64];
        size_t nw = split_words(buf, w, 64);
        if (nw == 0) return 0;
        int ok = 0;
        for (size_t j = 0; pure[j] && !ok; j++)
            if (strcmp(w[0], pure[j]) == 0) ok = 1;
        if (!ok) return 0;
        for (size_t j = 0; j < nw; j++)
            if (!safe_token(w[j])) return 0;
    }
    return 1;
}

/* Simulate one pure command segment. `input` is its stdin; output is written
 * to `out` (with the trailing newline the real command would produce). */
static int simulate_pure(const char *input, const char *cmdline,
                         char *out, size_t out_size) {
    char buf[256];
    snprintf(buf, sizeof buf, "%s", cmdline);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw == 0) return 0;

    if (strcmp(w[0], "echo") == 0) {
        size_t o = 0;
        for (size_t i = 1; i < nw && o + 1 < out_size; i++) {
            if (i > 1) out[o++] = ' ';
            size_t l = strlen(w[i]);
            if (l > out_size - o - 1) l = out_size - o - 1;
            memcpy(out + o, w[i], l);
            o += l;
        }
        if (o + 1 < out_size) out[o++] = '\n';
        out[o] = '\0';
        return 1;
    }

    if (strcmp(w[0], "pwd") == 0) {
        if (getcwd(out, (int)out_size)) {
            size_t o = strlen(out);
            if (o + 1 < out_size) out[o++] = '\n';
            out[o] = '\0';
        } else {
            snprintf(out, out_size, "\n");
        }
        return 1;
    }

    if (strcmp(w[0], "cat") == 0) {
        /* cat with no file arguments copies stdin to stdout */
        if (nw == 1) {
            snprintf(out, out_size, "%s", input);
            return 1;
        }
        return 0;
    }

    if (strcmp(w[0], "wc") == 0) {
        int count_words = 0;
        for (size_t i = 1; i < nw; i++)
            if (strcmp(w[i], "-w") == 0) count_words = 1;
        if (!count_words) return 0;
        size_t n = 0;
        const char *p = input;
        while (*p) {
            while (*p && isspace((unsigned char)*p)) p++;
            if (*p) n++;
            while (*p && !isspace((unsigned char)*p)) p++;
        }
        snprintf(out, out_size, "%zu\n", n);
        return 1;
    }

    return 0;
}

/* Simulate a pure pipeline by threading stdin/stdout through each segment. */
static int simulate_pipeline(const char *pipeline,
                             char *out, size_t out_size) {
    char pipebuf[512];
    size_t len = strlen(pipeline);
    if (len >= sizeof pipebuf) return 0;
    memcpy(pipebuf, pipeline, len + 1);

    char *segs[8];
    size_t nseg = 0;
    char *p = pipebuf;
    while (p && *p && nseg < 8) {
        char *next = strchr(p, '|');
        if (next) *next++ = '\0';
        while (*p && isspace((unsigned char)*p)) p++;
        segs[nseg++] = p;
        p = next;
    }
    if (!pipeline_is_pure(segs, nseg)) return 0;

    char buf[4096];
    buf[0] = '\0';
    for (size_t i = 0; i < nseg; i++) {
        char next[4096];
        if (!simulate_pure(buf, segs[i], next, sizeof next)) return 0;
        snprintf(buf, sizeof buf, "%s", next);
    }
    snprintf(out, out_size, "%s", buf);
    return 1;
}
