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
    canonicalize_lang(pre, buf, sizeof buf);
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    int self_ref = cue(buf, "yourself") || cue(buf, "about yourself") ||
                   cue(buf, "about you") || cue(buf, "your own") ||
                   cue(buf, "you fail") || cue(buf, "your implementation") ||
                   cue(buf, "your loop") || cue(buf, "your module") ||
                   cue(buf, "your subsystems") || cue(buf, "your parts") ||
                   cue(buf, "your modules") || cue(buf, "your capabilities") ||
                   cue(buf, "your abilities");

    /* gen164: a COMPOSITION self-challenge — "prove your subsystems compose",
     * "test composition over three subsystems", "do your parts cooperate?" —
     * is a distinct kind of self-challenge the gap branches below do not cover.
     * It asks parrot0 to reason about composing its OWN parts. EN+IT cues; the
     * parts/composition words carry the meaning, so it transfers to unseen
     * phrasings rather than a fixed trigger list. */
    int parts_ref = self_ref || cue(buf, "subsystems") || cue(buf, "parts") ||
                    cue(buf, "modules") || cue(buf, "capabilities") ||
                    cue(buf, "abilities") || cue(buf, "sottosistemi") ||
                    cue(buf, "moduli") || cue(buf, "parti") ||
                    cue(buf, "capacita") || cue(buf, "capacità");
    int compose_ref = cue(buf, "compose") || cue(buf, "composition") ||
                      cue(buf, "composing") || cue(buf, "cooperate") ||
                      cue(buf, "work together") || cue(buf, "compongono") ||
                      cue(buf, "comporre") || cue(buf, "composizione") ||
                      cue(buf, "cooperano") || cue(buf, "insieme");
    int compose_challenge = compose_ref && parts_ref;

    /* gen166: a request to SEE the dialogue, not just the method — "show me the
     * dialogue you would run", "write the example", "dimostralo con un dialogo".
     * It closes proposal to a runnable skeleton over the derived parts. */
    int want_skeleton = (cue(buf, "dialogue") || cue(buf, "dialog") ||
                         cue(buf, "transcript") || cue(buf, "dialogo") ||
                         cue(buf, "esempio") || cue(buf, "example")) &&
                        (compose_ref || parts_ref || cue(buf, "you would run") ||
                         cue(buf, "would you run") || cue(buf, "you would use") ||
                         cue(buf, "to prove it") || cue(buf, "show me"));

    /* gen167: a request to actually RUN the composition and report — "prove your
     * parts compose by running it yourself", "run the composition test on
     * yourself", IT "esegui tu il test di composizione". This does not describe
     * or display; it EXECUTES the derived dialogue on a fresh copy of parrot0 and
     * reports, computed from real output, not a canned string. Still no file
     * edit, no commit — the strongest reflexive claim inside the loop's boundary. */
    int run_ref = cue(buf, "run it yourself") || cue(buf, "run them yourself") ||
                  cue(buf, "running it yourself") || cue(buf, "by running") ||
                  cue(buf, "run the composition") || cue(buf, "run your") ||
                  cue(buf, "test yourself") || cue(buf, "self-test") ||
                  cue(buf, "selftest") || cue(buf, "execute") ||
                  cue(buf, "esegui") || cue(buf, "eseguilo") ||
                  cue(buf, "mettiti alla prova") || cue(buf, "verificati") ||
                  cue(buf, "provalo tu") || cue(buf, "run a self");
    int want_selftest = run_ref && (compose_ref || parts_ref);

    /* gen169: an AUDIT — run several different triples of my parts and report a
     * real cooperation MAP (the compose-bench matrix turned inward). "audit your
     * composition", "map which of your parts compose", IT "verifica quali tuoi
     * moduli si compongono". */
    int want_audit = (cue(buf, "audit") || cue(buf, "map") || cue(buf, "mappa") ||
                      cue(buf, "matrix") || cue(buf, "matrice") ||
                      cue(buf, "which combinations") || cue(buf, "quali") ||
                      cue(buf, "which of your")) &&
                     (compose_ref || parts_ref);

    int trigger = compose_challenge || want_skeleton || want_selftest ||
                  want_audit ||
                  cue(buf, "self-challenge") || cue(buf, "self challenge") ||
                  (cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "solve") && cue(buf, "challenge") && self_ref) ||
                  (cue(buf, "improve") && self_ref) ||
                  (cue(buf, "review this implementation") &&
                   (self_ref || cue(buf, "loop"))) ||
                  (cue(buf, "problem") && self_ref &&
                   (cue(buf, "solve") || cue(buf, "what should") ||
                    cue(buf, "what change")));
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
    int fallback_gap = cue(buf, "fallback") || cue(buf, "wall") ||
                       cue(buf, "do not understand") ||
                       cue(buf, "not understand") || cue(buf, "dont understand") ||
                       cue(buf, "repeat");
    int strong_implementation_gap = cue(buf, "implementation") || cue(buf, "code") ||
                                    cue(buf, "patch") || cue(buf, "dispatch") ||
                                    cue(buf, "loop");
    int implementation_gap = strong_implementation_gap ||
                             cue(buf, "module") || cue(buf, "modulo");

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

static int mod_learn(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    char buf[256];
    canonicalize_lang(norm, buf, sizeof buf);

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
    static const char *const strong_heads[] = {
        "what is a ", "what is an ", "tell me about ",
        "what do you know about ", "who is ", "who was ",
        /* gen240: explicit IMPERATIVE to acquire — a first-class command (or
         * planner step) to run the discovery plan, not just a question. */
        "learn about ", "research ", "look up ", "study ", "read up on ",
        "find out about ", "go learn about ",
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
    for (const char *const *h = strong_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; break; }
    }
    if (!x) for (const char *const *h = weak_heads; *h; h++) {
        size_t hl = strlen(*h);
        if (strncmp(work, *h, hl) == 0) { x = work + hl; weak = 1; break; }
    }
    if (!x || !*x) return 0;

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
    if (weak && strlen(tok[start]) < 4) return 0;

    char def[KB_TERM_LEN];
    char msg[320];

    /* gen240 (universal-comprehension §7): pursue the precondition know(X) via the
     * acquire-knowledge action — already in RAM, learned from the local certified
     * corpus, or fetched on demand from Wikipedia (all in C). On a miss, give the
     * INFORMED decline (§2): name what was understood and be honest it can learn —
     * never a blind "I don't understand". */
    int st = acquire_knowledge(b, key, def, sizeof def);
    char lang[8]; current_lang(b, lang, sizeof lang);
    int it = (strcmp(lang, "it") == 0);
    if (st == 2)
        snprintf(msg, sizeof msg,
                 it ? "Mi ero già documentato su %s: %s."
                    : "I already read up on %s: %s.", disp, def);
    else if (st == 1)
        snprintf(msg, sizeof msg,
                 it ? "Non conoscevo %s, così mi sono documentato: %s."
                    : "I didn't know about %s, so I just read it up: %s.", disp, def);
    else
        /* gen242: the INFORMED decline must announce, in the conversation's
         * language, that parrot0 CAN document itself — never a blind wall. */
        snprintf(msg, sizeof msg,
                 it ? "Ho capito che mi chiedi di %s, ma non lo conosco ancora: mi "
                      "documento da fonti statiche, indicamene una e lo imparo."
                    : "I understood you're asking about %s, but I don't know it yet and "
                      "have no source on it -- point me at one and I'll learn it.", disp);
    put(msg, out, out_size);
    return 1;
}

/* --- module: self --------------------------------------------------------
 * Identity & self-reflection (PRINCIPLES.md, "I know that I am"). The agent's
 * self-model lives in the very same KB it uses for the world: `i_am(parrot0).`
 * and one `module(<name>)` per registered part (asserted at birth — see
 * brain_create). This module answers introspective questions by *querying that
 * model*, so the answers are derived from real state, never hard-coded.
 */
static int mod_self(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    const char *var[] = {NULL};

    /* gen51 (C1): recognize intent by KEYWORD CUES, not one rigid template, so
     * many phrasings of the same question land. The answer is still derived from
     * the real self-model (i_am / module facts), never canned — robustness comes
     * from cue matching + the KB, not from a list of accepted sentences. */
    int identity = cue(buf, "your name") || cue(buf, "who are you") ||
                   cue(buf, "who re you") || cue(buf, "what are you") ||
                   cue(buf, "what exactly are you") ||
                   cue(buf, "call you") || cue(buf, "about yourself") ||
                   cue(buf, "who r u") || cue(buf, "your identity") ||
                   /* Italian cues (the bilingual ratchet). NB: input is already
                    * canonicalized, so "chi" has become "who" -> "who sei". */
                   cue(buf, "who sei") || cue(buf, "come ti chiami") ||
                   cue(buf, "il tuo nome");
    int exists = cue(buf, "you exist") || cue(buf, "are you real") ||
                 cue(buf, "esisti");
    int capability = cue(buf, "can you do") || cue(buf, "what do you do") ||
                     cue(buf, "capabilit") || cue(buf, "you able to") ||
                     cue(buf, "you help with") || cue(buf, "can you help") ||
                     /* Italian cues */
                     cue(buf, "cosa sai fare") || cue(buf, "puoi fare") ||
                     cue(buf, "sai fare") || cue(buf, "che cosa sai fare");
    /* gen240: "if you're happy and you know it, what do you do?" is a song, not a
     * capability query — a leading conditional means it isn't asking about me. */
    if (capability && (cue(buf, "if you") || cue(buf, "when you")))
        capability = 0;

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

