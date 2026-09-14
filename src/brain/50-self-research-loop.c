/* --- module: loop --------------------------------------------------------
 * Self-challenge parity for the external LOOP.md driver. This is deliberately
 * not self-management: parrot0 does not edit files, run tests, commit, or choose
 * its own next task. It answers a narrower conversational challenge: when the
 * external agent poses a problem about parrot0 itself, propose a comparable
 * engineering move in the same discipline the loop uses - smallest behavioral
 * change, executable ratchet, version bump, and journaled observation. */

static void machinery_gap_record(Brain *b, const char *canon, const char *raw);  /* 99-registry.c */

/* ── dialogica L1 (gen506i) — IL TABELLONE E' UNO: kb/core/issues.p0 ───────
 *
 * Una questione aperta e' `open_issue(Issue, Kind)` con il suo contenuto
 * (`issue_topic`, `issue_turn`, `issue_question`, `issue_option`) e identita'
 * Kind_Topic. Qui il C fa due cose sole — aprire e chiudere — e non sa che
 * cosa un genere significhi, come si legga un turno sotto di esso, ne' quando
 * scada: e' KB (issues.p0, network.p0 §10-11). `pending_gap`,
 * `pending_gap_question`, `pending_disambiguation`, `disambiguation_option`
 * sono VISTE su questo tabellone: i lettori C che le nominano ancora vanno
 * migrati a `open_issue`, non riforniti. Stato del dialogo: memoria di
 * lavoro, mai persistita (KB_REFLECTIVE). */
static void board_issue_id(const char *kind, const char *topic, char *out, size_t sz) {
    snprintf(out, sz, "%s_%s", kind, topic ? topic : "");
}
static void board_open(Brain *b, const char *kind, const char *topic, const char *question_quoted) {
    if (!b || !b->kb || !kind || !topic || !*topic) return;
    char id[KB_TERM_LEN]; board_issue_id(kind, topic, id, sizeof id);
    char turn[24]; snprintf(turn, sizeof turn, "%lu", b->turns);
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_assert(b->kb, "open_issue",  (const char *[]){ id, kind },  2);
    kb_assert(b->kb, "issue_topic", (const char *[]){ id, topic }, 2);
    kb_assert(b->kb, "issue_turn",  (const char *[]){ id, turn },  2);
    if (question_quoted && *question_quoted)
        kb_assert(b->kb, "issue_question", (const char *[]){ id, question_quoted }, 2);
    kb_set_origin(b->kb, prev);
}
static void board_option(Brain *b, const char *kind, const char *topic, const char *n, const char *title_quoted) {
    if (!b || !b->kb || !kind || !topic || !n || !title_quoted) return;
    char id[KB_TERM_LEN]; board_issue_id(kind, topic, id, sizeof id);
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_assert(b->kb, "issue_option", (const char *[]){ id, n, title_quoted }, 3);
    kb_set_origin(b->kb, prev);
}
static void board_close_id(Brain *b, const char *id) {
    if (!b || !b->kb || !id || !*id) return;
    kb_retract_match(b->kb, "open_issue",     (const char *[]){ id, NULL }, 2);
    kb_retract_match(b->kb, "issue_topic",    (const char *[]){ id, NULL }, 2);
    kb_retract_match(b->kb, "issue_turn",     (const char *[]){ id, NULL }, 2);
    kb_retract_match(b->kb, "issue_question", (const char *[]){ id, NULL }, 2);
    kb_retract_match(b->kb, "issue_relation", (const char *[]){ id, NULL }, 2);
    kb_retract_match(b->kb, "issue_option",   (const char *[]){ id, NULL, NULL }, 3);
}
static void board_close(Brain *b, const char *kind, const char *topic) {
    if (!kind || !topic) return;
    char id[KB_TERM_LEN]; board_issue_id(kind, topic, id, sizeof id);
    board_close_id(b, id);
}
/* Tutte le questioni di un genere: la chiusura globale che i tre tabelloni
 * facevano con kb_retract_pred. */
static void board_close_kind(Brain *b, const char *kind) {
    if (!b || !b->kb || !kind) return;
    char (*ids)[KB_TERM_LEN] = NULL; size_t n = 0;
    const char *q[2] = { NULL, kind };
    if (kb_match_all(b->kb, "open_issue", q, 2, &ids, &n))
        for (size_t i = 0; i < n; i++) board_close_id(b, ids[i]);
    free(ids);
}
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
                  (kb_cue_match(b, "50_self_research_loop_cue63", buf) && self_ref) ||
                  (kb_cue_match(b, "50_self_research_loop_cue64", buf) && kb_cue_match(b, "50_self_research_loop_cue64_2", buf) && self_ref) ||
                  (kb_cue_match(b, "50_self_research_loop_cue65", buf) && self_ref) ||
                  (kb_cue_match(b, "50_self_research_loop_cue66", buf) &&
                   (self_ref || kb_cue_match(b, "50_self_research_loop_cue67", buf))) ||
                  (kb_cue_match(b, "50_self_research_loop_cue68", buf) && self_ref &&
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
                {   const KbResponseSlot _rs[] = { { "x", "" } };
                  kb_term_say(b, "i_audited_my_own_composition_on_fresh_copies", _rs, 0, tpl, sizeof tpl); }
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
            kb_term_say(b, "i_would_treat_it_as_a_composition_self_chall", NULL, 0, out, out_size);
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
                { 
                  char _v4[48]; snprintf(_v4, sizeof _v4, "%zu", fired);
                  char _v5[48]; snprintf(_v5, sizeof _v5, "%zu", picked);
  const KbResponseSlot _rs[] = { { "v", v[0] }, { "v2", v[1] }, { "v3", v[3] }, { "names", names }, { "fired", _v4 }, { "picked", _v5 }, { "observed", observed } };
                  kb_term_say(b, "i_ran_it_on_a_fresh_copy_of_myself_with_held", _rs, 7, msg, sizeof msg); }
            else
                { 
                  char _v4[48]; snprintf(_v4, sizeof _v4, "%zu", fired);
                  char _v5[48]; snprintf(_v5, sizeof _v5, "%zu", picked);
  const KbResponseSlot _rs[] = { { "v", v[0] }, { "v2", v[1] }, { "v3", v[3] }, { "names", names }, { "fired", _v4 }, { "picked", _v5 } };
                  kb_term_say(b, "i_ran_it_on_a_fresh_copy_of_myself_with_held_2", _rs, 6, msg, sizeof msg); }
            put(msg, out, out_size);
            store_proof(b, "loop composition self-test: generate fresh vocab, run the derived dialogue on a fresh sub-brain, report pass/seam + observed cooperation from real output; footprint-free, edits external.");
            return 1;
        } else if (want_skeleton) {
            /* gen166: emit a runnable, single-line `>`-turn skeleton over the
             * derived parts (a fixed example tuple). An external agent fills the
             * names and drops it into tests/compose/; parrot0 does not run it. */
            char _t1[512];
            const KbResponseSlot _r1[] = { { "x", "" } };
            kb_term_say(b, "here_is_a_held_out_dialogue_i_would_run_fres", _r1, 0, _t1, sizeof _t1);
            size_t o = (size_t)snprintf(msg, sizeof msg, "%s", _t1);
            for (size_t k = 0; k < picked; k++) {
                char trn[256];
                build_turn(b, core[pick[k]].key, compose_vocab[0], trn, sizeof trn);
                o += (size_t)snprintf(msg + o, sizeof msg - o, " > %s", trn);
            }
            put(msg, out, out_size);
            store_proof(b, "loop composition skeleton: emit a runnable >-turn dialogue over the parts derived from module/X; external agent fills, runs, commits.");
        } else {
            { 
              char _v0[48]; snprintf(_v0, sizeof _v0, "%s", core[pick[0]].gloss);
              char _v1[48]; snprintf(_v1, sizeof _v1, "%s", core[pick[1]].gloss);
              char _v2[48]; snprintf(_v2, sizeof _v2, "%s", core[pick[2]].gloss);
  const KbResponseSlot _rs[] = { { "gloss", _v0 }, { "gloss2", _v1 }, { "gloss3", _v2 } };
              kb_term_say(b, "i_would_treat_it_as_a_composition_self_chall_2", _rs, 3, msg, sizeof msg); }
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
        kb_term_say(b, "i_would_solve_it_by_parity_with_the_external", NULL, 0, out, out_size);
    } else if (fallback_gap) {
        kb_term_say(b, "i_would_treat_it_as_a_fallback_gap_make_the", NULL, 0, out, out_size);
    } else {
        kb_term_say(b, "i_would_treat_it_as_a_self_challenge_not_sel", NULL, 0, out, out_size);
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
/* gen396: drop one trailing sentence terminator from text that will fill a slot
 * inside a frame that supplies its own.
 *
 * A definition is a whole sentence and «{def}.» adds a second full stop, so a
 * confirmed lookup ended on «... Moldova..». Which character ends a sentence is a
 * property of the language and `sentence_terminator/2` already says it, so no
 * punctuation is written into the C. */
static void strip_sentence_end(Brain *b, const char *lang, char *text) {
    if (!b || !b->kb || !text || !*text) return;
    char mark[1][KB_TERM_LEN];
    const char *mq[] = { lang, NULL };
    if (kb_match(b->kb, "sentence_terminator", mq, 2, mark, 1) != 1) return;
    const char *m = kb_dequote(mark[0]);
    size_t tl = strlen(text), ml = strlen(m);
    if (ml && tl > ml && !strcmp(text + tl - ml, m)) text[tl - ml] = '\0';
}

/* gen240 (universal-comprehension §7): the ACQUIRE-KNOWLEDGE action, factored out
 * so it is a reusable planner step, not just mod_learn's tail. Pursues the missing
 * precondition know(key): already in RAM -> 2 (def from memory); learned now from
 * the local certified corpus or an on-demand Wikipedia fetch -> 1; no source -> 0.
 * The discovery plan (local corpus, then HTTPS fetch via wiki_fetch_topic) lives
 * entirely here, so any goal that needs a concept can call it as its precondition. */
static int acquire_knowledge(Brain *b, const char *key, char *def, size_t def_sz) {
    if (!b || !b->kb || !key || !*key) return 0;
    if (kb_concept_def(b->kb, key, def, def_sz)) return 2;        /* already known */
    /* gen505y — la MEMORIA PROFONDA e' conoscenza posseduta: se il topic e'
     * gia' stato letto, la sua definizione risponde senza rileggere. */
    {
        char d[1][KB_TERM_LEN];
        const char *dq[2] = { key, NULL };
        if (kb_match(b->kb, "topic_definition", dq, 2, d, 1) == 1) {
            char db[KB_TERM_LEN]; snprintf(db, sizeof db, "%s", d[0]);
            if (def && def_sz) snprintf(def, def_sz, "%s", kb_dequote(db));
            return 2;
        }
    }
    if (learn_topic(b->kb, key, key, def, def_sz)) return 1;      /* local corpus */
    return 0;
}

/* deep-reasoning M2 (docs/plans/deep-reasoning.md §8): the PROSE extractor. Read a
 * corpus page's `## Extract` lead paragraph, split it into sentences, and run each
 * through the SAME comprehension parser M0 extended (extract_class_statement) — so
 * what parrot0 can UNDERSTAND, it EXTRACTS, each fact carrying its source fragment
 * (M1). Broad by design (§4.4): over-extraction is tolerated; the deep-reasoning
 * loop re-checks facts against their source (M4). Returns the number of facts
 * asserted and appends a readable list to `out`. */
static int learn_from_prose(Brain *b, char *extract, char *out, size_t out_sz);

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
    return learn_from_prose(b, extract, out, out_sz);
}

/* gen407 — UN SOLO ATTO DI APPRENDIMENTO.
 *
 * Questa funzione era il corpo di `extract_page_facts`, cioe' una cosa che
 * parrot0 sapeva fare SOLO leggendo una pagina. Misurato con la stessa prosa
 * per le due strade — la pagina di photosynthesis letta, e la sua identica
 * prosa incollata in conversazione:
 *
 *     letta   -> 8 fatti
 *     detta   -> 0 fatti, e al loro posto duecento parole di analisi generica
 *
 * Non e' una sfumatura: e' la stessa conoscenza che entra o non entra a seconda
 * di CHI l'ha portata. E il motivo per cui era passato inosservato e' che la
 * strada detta non produceva un muro visibile — murava davvero, e un modulo
 * generativo di ultima istanza copriva il muro con un saggio.
 *
 * La tesi di F. (question-emergence.md): il sogno non e' una facolta'
 * superiore, e' lo stesso atto che avviene quando si incolla della prosa con un
 * prompt che chiede di acquisirla. Se le due strade divergono, una delle due e'
 * un ramo morto che nessuno manutiene. Da qui in poi c'e' una funzione sola, e
 * la pagina e' solo un testo piu' lungo. */
/* gen408: una frase RIGUARDA cio' che si cerca se ne condivide una parola
 * piena. E' grezzo e falsificabile, e va bene che lo sia: il criterio non deve
 * essere intelligente, deve essere lo stesso che userebbe una persona che
 * scorre una pagina cercando una parola. */
static int prose_touches_purpose(Brain *b, const char *sentence,
                                 const char *purpose) {
    char pb[KB_TERM_LEN]; snprintf(pb, sizeof pb, "%s", purpose);
    char low[400];
    size_t i = 0;
    for (; sentence[i] && i + 1 < sizeof low; i++)
        low[i] = (char)tolower((unsigned char)sentence[i]);
    low[i] = '\0';
    char *pw[40];
    size_t np = split_words(pb, pw, 40);
    for (size_t k = 0; k < np; k++) {
        char *t = strip_edge_punct(pw[k]);
        if (strlen(t) < 4 || is_stopword(b, t)) continue;
        if (strstr(low, t)) return 1;
    }
    return 0;
}

/* ── gen513 — UN PUNTO FRA DUE CIFRE NON CHIUDE UNA FRASE ──────────────────
 *
 * Reperto della scala della prosa, e la perdita era totale e muta:
 *
 *   «Coral reefs occupy 3.5 percent of the ocean area.»
 *     -> Learned 1 facts: coral reefs occupy 3.
 *
 * Il lettore spezzava a OGNI `.`, quindi «3.5» diventava due frasi e tutto cio'
 * che seguiva il numero spariva. In una prosa d'enciclopedia i decimali sono
 * ovunque — «0.1%», «1.6 billion tons», «US$30-375 billion» — e ogni volta si
 * perdeva il resto della frase senza un muro e senza una traccia.
 *
 * Il tokenizzatore questa regola ce l'ha dal gen399. Qui era duplicata male,
 * cioe' non c'era: e' il caso peggiore dell'audit KB-first, la conoscenza
 * scritta due volte di cui una sbagliata. Quali segni siano interessati lo dice
 * la KB (`boundary_not_between_digits/1`), il confronto di byte resta qui. */
static int p0_boundary_inside_number(Brain *b, const char *start,
                                     const char *at) {
    if (!b || !b->kb || !start || !at || at == start || !at[1]) return 0;
    if (!isdigit((unsigned char)at[-1]) || !isdigit((unsigned char)at[1])) return 0;
    char rows[8][KB_TERM_LEN];
    const char *q[1] = { NULL };
    size_t n = kb_match(b->kb, "boundary_not_between_digits", q, 1, rows, 8);
    for (size_t i = 0; i < n; i++) {
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rows[i]);
        const char *m = kb_dequote(rb);
        if (*m && *m == *at) return 1;
    }
    return 0;
}

static int learn_from_prose(Brain *b, char *extract, char *out, size_t out_sz) {
    size_t eo = strlen(extract);

    (void)eo;
    /* gen382: una pagina che DICHIARA di non contenere conoscenza non va letta.
     * "X may refer to ..." apre una disambiguazione: elenca cose diverse che
     * portano lo stesso nome. E' la trappola degli omonimi che --dream ha
     * mostrato — sognare il modale "may" scaricava il mese di maggio — e
     * imparare da li' non e' imparare poco, e' imparare il falso. Quali frasi
     * lo dichiarino e' conoscenza: disambiguation_marker/1 in grammar.p0. */
    {
        char marks[16][KB_TERM_LEN];
        const char *mq[] = { NULL };
        size_t nm = kb_match(b->kb, "disambiguation_marker", mq, 1, marks, 16);
        char low[512];
        snprintf(low, sizeof low, "%.*s", (int)sizeof low - 1, extract);
        for (char *c = low; *c; c++) *c = (char)tolower((unsigned char)*c);
        for (size_t i = 0; i < nm; i++) {
            if (strstr(low, kb_dequote(marks[i]))) {
                kb_response(b, "page_disambiguates", NULL, out, out_sz);
                return 0;
            }
        }
    }

    int nfacts = 0, nrules = 0, nrejected = 0; size_t mo = 0;
    if (out_sz) out[0] = '\0';

    /* ── gen408: L'INTENZIONE CON CUI SI LEGGE ─────────────────────────────
     *
     * `tests/dream_intent_probe.py` misura che davanti alla stessa prosa la
     * cornice decide l'ATTO: senza intenzione un ragionatore chiede cosa
     * dovrebbe farne, con «acquisisci» trattiene, con «acquisisci per
     * rispondere a X» va dritto alla risposta e non riassume nemmeno.
     *
     * Qui parrot0 aveva un solo modo di leggere. Ora la KB puo' dichiarare
     * `reading_intent(bridge, "<turno che murava>")`, e cambiano due cose reali:
     *
     *   l'ORDINE  — le frasi che nominano le parole del turno rimasto senza
     *               risposta si leggono per prime;
     *   la FINE   — appena quel turno risponde, si smette. L'intenzione era
     *               quella, ed e' soddisfatta.
     *
     * Non e' «leggere meglio»: e' leggere PER qualcosa, che e' l'unica cosa che
     * la sonda mostra fare la differenza. E su un budget limitato — cioe' nel
     * sogno, dove il budget e' il punto — l'ordine E' il risultato. */
    char purpose[KB_TERM_LEN]; purpose[0] = '\0';
    {
        const char *iq[2] = { "bridge", NULL };
        char row[1][KB_TERM_LEN];
        if (kb_match(b->kb, "reading_intent", iq, 2, row, 1) > 0)
            snprintf(purpose, sizeof purpose, "%s", kb_dequote(row[0]));
    }

    /* Con un'intenzione mirata si legge SOLO cio' che la riguarda. E' quello che
     * fa una persona che cerca una risposta in una pagina, ed e' l'unica cosa
     * che rende l'intenzione misurabile invece che decorativa: se leggessimo
     * tutto lo stesso, l'ordine non cambierebbe nessun risultato finale.
     *
     * Se pero' nessuna frase nomina cio' che si cerca, l'intenzione non ha
     * presa: si legge tutto, come senza. Meglio imparare qualcosa d'altro che
     * non imparare niente per aver cercato male. */
    int selective = 0;
    if (purpose[0]) {
        for (char *r = extract; *r && !selective; ) {
            char *e = r;
            while (*e && !((*e == '.' || *e == '!' || *e == '?') &&
                           !p0_boundary_inside_number(b, extract, e))) e++;
            size_t l = (size_t)(e - r);
            if (l > 4 && l < 380) {
                char probe[400];
                memcpy(probe, r, l); probe[l] = '\0';
                if (prose_touches_purpose(b, probe, purpose)) selective = 1;
            }
            if (!*e) break;
            r = e + 1;
        }
    }

    /* ── gen505z — DOVE FINISCE UNA FRASE E' CONOSCENZA (piano §3.1) ────────
     *
     * Erano `. ! ?` scritti qui. `passage_boundary_mark/1` li dichiara e riusa
     * `sentence_terminator/2`: ritirarlo lascia il passo in una frase sola, che
     * e' l'ablazione con cui il cricchetto misura questa riga. */
    char bmarks[16][KB_TERM_LEN];
    const char *bmq[1] = { NULL };
    size_t nbm = kb_match(b->kb, "passage_boundary_mark", bmq, 1, bmarks, 16);
    char bchars[32]; size_t nbc = 0;
    for (size_t i = 0; i < nbm && nbc + 1 < sizeof bchars; i++) {
        char mb[KB_TERM_LEN];
        snprintf(mb, sizeof mb, "%s", bmarks[i]);
        const char *m = kb_dequote(mb);
        if (*m) bchars[nbc++] = m[0];
    }
    bchars[nbc] = '\0';

    /* ── E IL FOCUS SCORRE DENTRO IL PASSO ──────────────────────────────────
     *
     * «Its capital is Velk», letta da sola, non ha soggetto e non rende niente.
     * Il soggetto della primaria e' il focus della secondaria — la stessa cosa
     * che «what is it part of» fa da un turno all'altro. Il determinante che
     * punta indietro (`referring_possessive/1`, conoscenza) viene sostituito dal
     * focus, e la frase riscritta va allo STESSO frame che gia' legge «the
     * capital of X is Y» da un turno normale: non un secondo estrattore. */
    char focus[KB_TERM_LEN] = "";

    char *p = extract;
    while (*p) {
        char *q = p;
        while (*q && !(nbc && strchr(bchars, *q) &&
                       !p0_boundary_inside_number(b, extract, q))) q++;
        size_t slen = (size_t)(q - p);
        if (selective && slen > 4 && slen < 380) {
            char probe[400];
            memcpy(probe, p, slen); probe[slen] = '\0';
            if (!prose_touches_purpose(b, probe, purpose)) {
                if (!*q) break;
                p = q + 1;
                continue;
            }
        }
        if (slen > 4 && slen < 380) {
            char sent[400], nrm[400], canon[400], msg[256];
            memcpy(sent, p, slen); sent[slen] = '\0';
            /* MISURATO E SCARTATO (gen405): togliere le parentetiche di glossa
             * («ribonucleic acid (RNA)») non ha cambiato nessun conto sulle
             * cinque pagine provate, perche' quelle frasi cadono per un altro
             * motivo. Resta il fatto che la glossa E' conoscenza:
             * TODO(kb-first): `alias(rna, ribonucleic_acid)` dalla parentetica —
             * ma va fatto perche' rende un fatto, non per ripulire l'ingresso. */
            normalize(sent, nrm, sizeof nrm);
            /* gen506e — IL FOCUS SCORRE SULLA FORMA CANONICA. «la sua capitale
             * e' velk», «il paese e' stato fondato nel 1845»: il possessivo, la
             * classe («country») e la copula che la regola del focus cerca
             * stanno nell'interlingua, non nella superficie italiana. Prima si
             * canonicalizza, poi il focus riscrive (lo stesso lettore di
             * read_passage); per l'inglese e' la stessa stringa di prima. */
            canonicalize_lang(b, nrm, canon, sizeof canon);
            {
                char rw[400];
                if (reader_focus_rewrite(b, canon, focus, sizeof focus, rw, sizeof rw))
                    snprintf(canon, sizeof canon, "%s", rw);
            }
            if (getenv("P0_READ_TRACE")) fprintf(stderr, "[prose] focus=«%s» norm=«%s» canon=«%s»\n", focus, nrm, canon);
            msg[0] = '\0';
            /* gen382: NIENTE `continue` qui — il ciclo sulle frasi avanza `p` in
             * fondo al corpo, quindi saltare il fondo e' un loop infinito (lo
             * era: due pagine su dodici non tornavano piu'). Il rifiuto si
             * esprime come condizione, non come salto. */
            /* gen405: la SECONDA forma della prosa, provata per prima perche'
             * e' piu' specifica — una frase che elenca («organismi come le
             * piante, le alghe e i cianobatteri») e' anche una frase «X e' un
             * Y», e letta come tale rende un fatto vuoto al posto di tre veri. */
            /* TODO(kb-first): l'enumerazione e' agganciata SOLO a questo
             * percorso — la lettura profonda di una pagina. Chi DICE «metals
             * such as copper, tin and lead» in conversazione riceve ancora un
             * muro, mentre chi la fa leggere viene capito. E' la stessa
             * conoscenza per due strade diverse, e l'asimmetria non ha ragione
             * di esistere: va chiamata anche dal percorso della frase detta. */
            char emsg[512]; emsg[0] = '\0';
            int ne = extract_enumeration(b, canon, emsg, sizeof emsg);
            if (ne && emsg[0]) {
                if (mo + strlen(emsg) + 4 < out_sz) {
                    mo += (size_t)snprintf(out + mo, out_sz - mo, "%s%s",
                                           (nfacts || nrules) ? ", " : "", emsg);
                    /* `ne` e' il numero di MEMBRI entrati, non di frasi: il
                     * resoconto deve dire quanti fatti, non quante letture. */
                    nfacts += ne;
                }
                if (!*q) break;
                p = q + 1;
                continue;
            }
            /* MISURATO E SCARTATO (gen405). Sognando cinque pagine, le frasi
             * che battono l'estrattore sembravano bloccate dal soggetto
             * coordinato — «dna and ribonucleic acid are nucleic acids». Ho
             * spezzato il soggetto e non e' cambiato niente: le due meta' non
             * si leggono lo stesso. Il bloccante vero e' un altro, e ora e'
             * misurato invece che immaginato.
             * TODO(kb-first): la forma PLURALE SENZA ARTICOLO. «entropy is a
             * thermodynamic state variable» entra (classe multiparola, con
             * articolo); «dna are nucleic acids» no. E' la forma con cui
             * un'enciclopedia dice l'appartenenza a una categoria, e oggi cade
             * tutta. */
            /* ── gen505z — IL LETTORE NON USA I FRAME DELLA CONVERSAZIONE ───
             *
             * Questo percorso conosce DUE forme — l'enumerazione e
             * l'appartenenza a una classe — mentre un turno normale ne legge
             * centotrentasei: «the capital of zorbium is velk» detto in chat
             * diventa un fatto, letto da una pagina sparisce. E' il «cassetto
             * senza maniglia» applicato alla lettura, ed e' il punto 1 della
             * coda del gen505y. Le due meta' che lo precedono FUNZIONANO: i
             * confini vengono dalla KB e il focus scorre (misurato: «its capital
             * is velk» arriva qui come «the capital of zorbium is velk»).
             *
             *   verdetto    non chiuso (2026-09-07, gen505z)
             *   ragione     chiamare `p0_try_extract_frames_only` da qui non
             *               rende il fatto, ne' prima ne' dopo la classe. E il
             *               frame ESISTE ed e' derivato:
             *                 relation_noun(capital_of, "capital")
             *                   -> extract_frame("the capital of @S is @O", …)
             *               quindi il legatore lo trova e qualcosa A VALLE lo
             *               respinge — il sospetto misurabile e' il cancello
             *               `p0_fact_is_clean`, che chiede che gli argomenti
             *               siano concetti noti: «velk» non lo e'.
             *   condizione  il gate dei fatti puliti applicato a un'entita'
             *               appena incontrata in lettura
             *   specie      prematuro — e' probabile che la cura sia far entrare
             *               l'entita' letta PRIMA di leggere il fatto che la
             *               nomina, non aggirare il cancello
             *
             * ⚠ Sei forme tentate; fermato per la regola d'arresto 2 e tolto
             * tutto il codice che non scatta. La prossima da provare e' la
             * verifica del cancello, con `p0_fact_is_clean` messo a stampa su
             * questa frase — NON un settimo punto di chiamata.
             *
             * ⚠ Nota misurata: `extract_class_statement` CONSUMA la frase
             * leggendola come «X is Y» e producendo `velk(the_capital_of_
             * zorbium)`, un'entita' inventata dalla forma. Quel fatto entra
             * anche oggi, ed e' un secondo difetto da chiudere insieme. */
            int r = extract_class_statement(b, canon, msg, sizeof msg, 1);
            if (r == 2) nrejected++;                 /* cancello: respinto */
            else if (!r) {
                /* gen405 (F.): UNA FORMA DI PROSA CHE NON SO LEGGERE E' UNA
                 * LACUNA, e va nello stesso registro dei turni senza ponte.
                 *
                 * Prima la frase spariva: la pagina diceva «rilascia ossigeno
                 * come sottoprodotto della scissione dell'acqua» e parrot0
                 * andava avanti come se non ci fosse. Non e' una pagina povera,
                 * e' una forma che nessuno gli ha mostrato — la stessa cosa che
                 * al gen404 abbiamo smesso di lasciar cadere in conversazione.
                 *
                 * Registrarla ha due effetti, e il secondo e' quello che conta:
                 * il sogno smette di sembrare produttivo quando non lo e', e
                 * l'elenco delle forme che lo battono diventa l'agenda di cosa
                 * insegnargli a leggere. Il cancello (`r == 2`) resta fuori: un
                 * rifiuto e' una decisione presa, non una lacuna subita. */
                machinery_gap_record(b, canon, sent);
            }
            if (r == 1) {
                /* gen382: la prosa produce anche REGOLE (il generico plurale
                 * "whales are mammals"), e vanno contate come tali. Prima solo
                 * "Learned: " veniva tolto, quindi il testo del messaggio di una
                 * regola finiva DENTRO l'elenco dei fatti — la KB si approfondiva
                 * davvero ma il resoconto lo nascondeva, che e' il modo peggiore
                 * di crescere: senza poterlo vedere. */
                const char *fact = msg;
                int is_rule = 0;
                if (!strncmp(fact, "Learned rule: ", 14)) { fact += 14; is_rule = 1; }
                else if (!strncmp(fact, "Learned: ", 9)) fact += 9;
                int flen = (int)strcspn(fact, ".");   /* drop the trailing period */
                if (mo + (size_t)flen + 4 < out_sz) {
                    mo += (size_t)snprintf(out + mo, out_sz - mo, "%s%.*s",
                                           (nfacts || nrules) ? ", " : "", flen, fact);
                    if (is_rule) nrules++; else nfacts++;
                }
            }
        }
        if (!*q) break;
        p = q + 1;
    }
    /* Il chiamante riceve il TOTALE di cio' che e' entrato in KB — fatti piu'
     * regole — perche' e' quello il conto della crescita. L'elenco distingue le
     * due forme perche' le regole si leggono come clausole. */
    /* Lo scarto viaggia col risultato: una pagina da cui si e' respinto molto non
     * e' una pagina povera, e' una pagina letta male — e chi guarda deve poterlo
     * distinguere. */
    if (nrejected && mo + 40 < out_sz)
        { char _t2[512];
        char _t2_v0[48]; snprintf(_t2_v0, sizeof _t2_v0, "%d", nrejected);
        const KbResponseSlot _r2[] = { { "nrejected", _t2_v0 } };
        kb_term_say(b, "x_scartati_dal_cancello", _r2, 1, _t2, sizeof _t2);
        snprintf(out + mo, out_sz - mo, "%s", _t2);
        }
    return nfacts + nrules;
}

/* ── gen505y — L'AZIONE «LEGGERE IL TOPIC DALLA MEMORIA PROFONDA» ─────────────
 *
 * docs/plans/la-rete-come-memoria-profonda.md. Questa funzione e' l'ESECUTORE
 * di `action_schema(read_topic, network)`: non decide se leggere (lo dice
 * `acquisition_move/1`), non decide da dove (lo dice `topic_provider_order/2`),
 * non decide che cosa tenere (lo dice il lettore). Fa tre cose passive: apre
 * l'indirizzo che la KB ha scelto, passa la prosa al lettore di «read: …», e
 * scrive nella memoria profonda DOVE ha letto (`topic_read/2`, con edizione,
 * titolo risolto e revisione) e la definizione (`topic_definition/2`).
 * Niente testo archiviato: e' la regola del gen436. */
static int network_acquire_passage(Brain *b, const char *topic, char *def,
                                   size_t def_sz, int *nfacts);

/* ── 13 settembre 2026 — MENTRE SI LEGGE, NON SI VA A LEGGERE ALTRO ─────────
 *
 * Con `acquisition_policy(act)`, «what is a fire blanket?» leggeva la pagina e
 * poi anche «Design» sotto la chiave `designed`, e «water hammer» leggeva
 * «Causality» sotto `caused`: il resto di una frase del passo arrivava al muro
 * come un turno, e il muro, trovando la politica «agisci», andava a leggere il
 * participio. Chiavi sbagliate e pagine non pertinenti (mix 04×23: una lacuna non
 * pertinente non genera una ricerca). Che si stia leggendo e' un fatto
 * (`reading_in_progress/1`), e la mossa lo legge in network.p0; i termini ancora
 * ignoti del passo restano lacune nominate per il sogno (`topic_open_term`). */
static int network_acquire(Brain *b, const char *topic, char *def, size_t def_sz,
                           int *nfacts) {
    if (!b || !b->kb || !topic || !*topic) return 0;
    char quoted[KB_TERM_LEN];
    snprintf(quoted, sizeof quoted, "\"%.*s\"", KB_TERM_LEN - 3, topic);
    const char *ra[] = { quoted };
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_assert(b->kb, "reading_in_progress", ra, 1);
    kb_set_origin(b->kb, prev);
    int r = network_acquire_passage(b, topic, def, def_sz, nfacts);
    kb_retract(b->kb, "reading_in_progress", ra, 1);
    return r;
}

/* La pagina di un topic, dal primo provider che la ha (network.p0 §7): il testo del
 * lead, l'edizione, il titolo risolto, la revisione e, per l'edizione locale, la
 * cartella. Estratta dal lettore della memoria profonda perche' la usano due lettori:
 * quello che impara la pagina e quello che la legge per rispondere (guided-reading.p0).
 * `edition` 32, `title` 160, `revision` 64, `fixdir` KB_TERM_LEN byte. */
static int network_fetch_page(Brain *b, const char *topic, char *prose, size_t prose_sz,
                              char *edition, char *title, char *revision, char *fixdir) {
    if (!b || !b->kb || !topic || !*topic || !prose || prose_sz < 16) return 0;
    prose[0] = '\0'; edition[0] = title[0] = revision[0] = fixdir[0] = '\0';
    int got = 0;
    for (int rank = 1; rank <= 8 && !got; rank++) {
        char rs[8]; snprintf(rs, sizeof rs, "%d", rank);
        char pv[1][KB_TERM_LEN];
        const char *oq[2] = { NULL, rs };
        if (kb_match(b->kb, "topic_provider_order", oq, 2, pv, 1) != 1) continue;
        char pb[KB_TERM_LEN]; snprintf(pb, sizeof pb, "%s", pv[0]);
        const char *prov = kb_dequote(pb);
        if (strcmp(prov, "fixture") == 0) {
            char dirs[1][KB_TERM_LEN];
            const char *fq[2] = { "fixture", NULL };
            if (kb_match(b->kb, "topic_provider", fq, 2, dirs, 1) != 1) continue;
            char db[KB_TERM_LEN]; snprintf(db, sizeof db, "%s", dirs[0]);
            snprintf(fixdir, KB_TERM_LEN, "%s", kb_dequote(db));
            char path[512];
            snprintf(path, sizeof path, "%s/%s.txt", fixdir, topic);
            FILE *f = fopen(path, "r");
            if (!f) continue;
            size_t n = fread(prose, 1, prose_sz - 1, f);
            fclose(f);
            prose[n] = '\0';
            while (n && (prose[n - 1] == '\n' || prose[n - 1] == ' ')) prose[--n] = '\0';
            if (n < 10) continue;
            snprintf(edition, 32, "fixture");
            snprintf(title, 160, "%s", topic);
            snprintf(revision, 64, "local");
            /* l'edizione locale e' una copia di pagine vere: se la cartella dice da
             * quale titolo e revisione viene (`SOURCES.tsv`: chiave, titolo,
             * revisione), la lettura lo riporta come la rete */
            {
                char spath[600]; snprintf(spath, sizeof spath, "%s/SOURCES.tsv", fixdir);
                FILE *sf = fopen(spath, "r");
                if (sf) {
                    char line[512];
                    while (fgets(line, sizeof line, sf)) {
                        char *t1 = strchr(line, '\t'); if (!t1) continue;
                        *t1 = '\0';
                        if (strcmp(line, topic)) continue;
                        char *t2 = strchr(t1 + 1, '\t'); if (!t2) break;
                        *t2 = '\0';
                        char *nl = strchr(t2 + 1, '\n'); if (nl) *nl = '\0';
                        snprintf(title, 160, "%s", t1 + 1);
                        snprintf(revision, 64, "%s", t2 + 1);
                        break;
                    }
                    fclose(sf);
                }
            }
            got = 1;
        } else if (strcmp(prov, "wikipedia") == 0) {
            if (!kb_query(b->kb, "network_available", NULL, 0)) continue;
            /* gen506e: l'edizione segue la lingua del turno (network.p0,
             * `edition_for_language/2`); se non ha la pagina, si ripiega su en. */
            char lang[16] = "en";
            {
                char cl[1][KB_TERM_LEN], ed[1][KB_TERM_LEN];
                const char *lq[1] = { NULL };
                if (kb_match(b->kb, "current_language", lq, 1, cl, 1) > 0) {
                    const char *eq[2] = { cl[0], NULL };
                    if (kb_match(b->kb, "edition_for_language", eq, 2, ed, 1) == 1)
                        snprintf(lang, sizeof lang, "%.15s", kb_dequote(ed[0]));
                }
            }
            /* Il tema arriva gia' canonicalizzato in inglese («fotosintesi» ->
             * «photosynthesis», via tr/2): un'edizione che non e' quella inglese
             * vuole il titolo nella SUA lingua, e la stessa conoscenza che ha
             * tradotto in avanti — `tr(En, Nativo)` — traduce indietro. */
            char native[KB_TERM_LEN] = "";
            if (strcmp(lang, "en") != 0) {
                char nv[1][KB_TERM_LEN];
                const char *tq[2] = { topic, NULL };
                if (kb_match(b->kb, "tr", tq, 2, nv, 1) == 1)
                    snprintf(native, sizeof native, "%s", kb_dequote(nv[0]));
            }
            if (getenv("P0_READ_TRACE")) fprintf(stderr, "[acquire] topic=«%s» edition=%s native=«%s»\n", topic, lang, native);
            int fetched = (native[0] && wiki_fetch_topic_lang_prose(native, lang, prose, prose_sz)) ||
                          wiki_fetch_topic_lang_prose(topic, lang, prose, prose_sz);
            if (!fetched) {
                if (!strcmp(lang, "en") || !wiki_fetch_topic_lang_prose(topic, "en", prose, prose_sz)) continue;
                snprintf(lang, sizeof lang, "en");
            }
            snprintf(edition, 32, "%s", lang);
            snprintf(title, 160, "%s", wiki_last_title());
            snprintf(revision, 64, "%s",
                     wiki_last_revision()[0] ? wiki_last_revision() : "unknown");
            got = 1;
        }
    }
    return got;
}

static int network_acquire_passage(Brain *b, const char *topic, char *def,
                                   size_t def_sz, int *nfacts) {
    if (!b || !b->kb || !topic || !*topic) return 0;
    if (nfacts) *nfacts = 0;
    if (def && def_sz) def[0] = '\0';
    char prose[4096] = "";
    char edition[32] = "", title[160] = "", revision[64] = "", fixdir[KB_TERM_LEN] = "";
    int got = network_fetch_page(b, topic, prose, sizeof prose, edition, title, revision, fixdir);
    if (!got) return 0;

    /* ── gen506d — LA PAGINA CHE DISAMBIGUA NON SI LEGGE: SI CHIEDE ──────────
     *
     * «parlami dei plc» -> «si» -> «Vediamo cosa trovo su plc... PLC or plc may
     * refer to:.» — la prima frase di una pagina di disambiguazione incollata
     * come se fosse una definizione. La KB sapeva gia' riconoscerla
     * (`disambiguation_marker/1`, gen382) ma solo per non impararne; qui la
     * si tratta per quello che e': un bivio. I significati fra cui scegliere
     * sono i titoli che la ricerca dell'edizione propone (Wikipedia: la
     * search API; fixture: `<dir>/<topic>.options.txt`) e finiscono in KB —
     * `pending_disambiguation/1`, `disambiguation_option/3` — dove la
     * conoscenza decide COME mostrarli e come si sceglie (network.p0 §9).
     * Il C non conosce ne' lo stile ne' le parole: torna 3, «disambigua». */
    {
        char marks[16][KB_TERM_LEN];
        const char *mq[] = { NULL };
        size_t nm = kb_match(b->kb, "disambiguation_marker", mq, 1, marks, 16);
        char low[600];
        snprintf(low, sizeof low, "%.*s", (int)sizeof low - 1, prose);
        for (char *c = low; *c; c++) *c = (char)tolower((unsigned char)*c);
        int dis = 0;
        for (size_t i = 0; i < nm && !dis; i++)
            if (strstr(low, kb_dequote(marks[i]))) dis = 1;
        if (dis) {
            char titles[2048] = "";
            if (fixdir[0]) {
                char path[512];
                snprintf(path, sizeof path, "%s/%s.options.txt", fixdir, topic);
                FILE *f = fopen(path, "r");
                if (f) { size_t n = fread(titles, 1, sizeof titles - 1, f); fclose(f); titles[n] = '\0'; }
            } else {
                wiki_search_titles(topic, edition[0] ? edition : "en", titles, sizeof titles);
            }
            int prev_o = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_SESSION);
            /* gen506i: il bivio e' una questione del tabellone unico (issues.p0) */
            board_close_kind(b, "choice");
            kb_retract_pred(b->kb, "option_word");
            int n = 0;
            char *save = NULL;
            for (char *ln = strtok_r(titles, "\n", &save); ln && n < 8; ln = strtok_r(NULL, "\n", &save)) {
                while (*ln == ' ') ln++;
                size_t l = strlen(ln);
                while (l && (ln[l - 1] == ' ' || ln[l - 1] == '\r')) ln[--l] = '\0';
                if (!l) continue;
                if (strcasecmp(ln, title) == 0 || strcasecmp(ln, topic) == 0) continue;  /* la pagina stessa */
                char nstr[8]; snprintf(nstr, sizeof nstr, "%d", ++n);
                char qt[192];
                for (char *c = ln; *c; c++) if (*c == '"') *c = '\'';
                snprintf(qt, sizeof qt, "\"%s\"", ln);
                board_option(b, "choice", topic, nstr, qt);
                /* gen506h: le parole dell'opzione come cue del frame
                 * (`option_word(Parola, Tema, N)`, network.p0 §11) — tranne il
                 * tema stesso, che sta in ogni opzione. */
                {
                    char wb[192]; snprintf(wb, sizeof wb, "%s", ln);
                    for (char *c = wb; *c; c++) *c = (char)tolower((unsigned char)*c);
                    char *ww[16]; size_t nww = split_words(wb, ww, 16);
                    for (size_t k = 0; k < nww; k++) {
                        if (!strcmp(ww[k], topic) || strlen(ww[k]) < 2) continue;
                        char qw[192]; snprintf(qw, sizeof qw, "\"%s\"", ww[k]);
                        const char *wa[3] = { qw, topic, nstr };
                        kb_assert(b->kb, "option_word", wa, 3);
                    }
                }
            }
            if (n > 0) board_open(b, "choice", topic, NULL);
            kb_set_origin(b->kb, prev_o);
            if (def && def_sz) def[0] = '\0';
            if (nfacts) *nfacts = 0;
            return n > 0 ? 3 : 0;
        }
    }

    /* la prima frase e' la definizione, prima che il lettore la consumi */
    if (def && def_sz) {
        size_t cut = 0;
        for (size_t i = 0; prose[i]; i++)
            if ((prose[i] == '.' || prose[i] == '!' || prose[i] == '?') &&
                (!prose[i + 1] || prose[i + 1] == ' ')) { cut = i + 1; break; }
        if (!cut) cut = strlen(prose);
        if (cut >= def_sz) cut = def_sz - 1;
        memcpy(def, prose, cut); def[cut] = '\0';
    }
    char lmsg[512] = "";
    int nf = learn_from_prose(b, prose, lmsg, sizeof lmsg);
    if (nfacts) *nfacts = nf;

    /* la memoria profonda: dove, e che cosa e' */
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_SESSION);
    {
        char qt[192]; snprintf(qt, sizeof qt, "\"%s\"", title);
        char addr[KB_TERM_LEN];
        snprintf(addr, sizeof addr, "wiki_address(%s, %s, %s, lead)", edition, qt, revision);
        const char *ra[2] = { topic, addr };
        kb_assert(b->kb, "topic_read", ra, 2);
        if (def && def[0]) {
            char dcopy[KB_TERM_LEN];
            snprintf(dcopy, sizeof dcopy, "%.*s", (int)(sizeof dcopy - 4), def);
            for (char *c = dcopy; *c; c++) if (*c == '"') *c = '\'';
            char qd[KB_TERM_LEN + 4];
            snprintf(qd, sizeof qd, "\"%s\"", dcopy);
            const char *da[2] = { topic, qd };
            kb_assert(b->kb, "topic_definition", da, 2);
        }
    }
    kb_set_origin(b->kb, prev);
    return 1;
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
            domain_assert(b, "isa", ia, 2);
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
        if (lex_class_member(b, "50_self_research_loop_lex628", strip_edge_punct(w[i]))) { ip = i; break; }

    char subj[KB_TERM_LEN] = "", obj[KB_TERM_LEN] = "";
    const char *rel = NULL; int class_mode = 0;
    if (ip < n && ip + 2 < n) {
        size_t inp = n;
        for (size_t i = ip + 1; i < n; i++)
            if (lex_class_member(b, "50_self_research_loop_lex635", strip_edge_punct(w[i]))) { inp = i; break; }
        if (inp != n && inp > ip + 1 && inp + 1 < n &&
            p0_join(w, ip + 1, inp, subj, sizeof subj) &&
            p0_join(w, inp + 1, n, obj, sizeof obj)) {
            rel = "located_in"; class_mode = 0;
        } else {
            size_t s = ip + 1; if (s < n && is_article(b, w[s])) s++;
            size_t ap = n;
            for (size_t i = s + 1; i < n; i++)
                if (is_article(b, strip_edge_punct(w[i]))) { ap = i; break; }
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

/* gen395: un CANDIDATO di lemma diventa la chiave solo se la KB lo conosce.
 *
 * `singular/2` e' una tabella curata, cioe' una decisione; `lemma_candidate/2`
 * aggiunge le regole di flessione, che per costruzione sovragenerano — «florble»
 * propone «florbla». Prendere il primo candidato, com'era scritto qui, trasforma
 * un candidato in una decisione: misurato, la ricerca partiva per «florbla» e
 * mancava la parola che l'utente aveva scritto. La sostituzione va percio'
 * MERITATA — vale solo se qualche concetto della KB porta gia' quel nome — e
 * quale forma sia un lemma resta interamente conoscenza. */
static void research_lemma_key(Brain *b, char *key, size_t sz) {
    if (!b || !b->kb || !key || !*key) return;
    char cand[8][KB_TERM_LEN];
    const char *q[] = { key, NULL };
    size_t n = kb_match(b->kb, "lemma_candidate", q, 2, cand, 8);
    for (size_t i = 0; i < n; i++) {
        if (strcmp(cand[i], key) == 0) continue;
        if (kb_is_concept_key(b->kb, cand[i])) {
            snprintf(key, sz, "%s", cand[i]);
            return;
        }
    }
}

static int mod_learn_turn(Brain *b, const char *norm, const char *raw,
                          char *out, size_t out_size);

/* 14 settembre 2026 — LA TESTA DEL TURNO E' UN FATTO.
 *
 * «definisci "X"» e «tell me about X» aprono la stessa facolta', ma non
 * chiedono la stessa cosa: la prima e' gia' il permesso di leggere, la seconda
 * no. Quale testa porti il permesso e' conoscenza (`read_request_head/1`,
 * network.p0), e la mossa la decide `acquisition_move/1`. Il C pubblica
 * soltanto quale testa ha riconosciuto — `turn_knowledge_head/1` — per la
 * durata del turno, e la ritira qualunque cosa la facolta' risponda. */
static int mod_learn(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    int r = mod_learn_turn(b, norm, raw, out, out_size);
    if (b && b->kb) kb_retract_pred(b->kb, "turn_knowledge_head");
    return r;
}

static int mod_learn_turn(Brain *b, const char *norm, const char *raw,
                          char *out, size_t out_size) {
    if (!b) return 0;

    /* gen408: LO SCOPO SI STACCA PRIMA DI TUTTO IL RESTO.
     *
     * «read the page on X to answer: Y» ha due parti, e la seconda non e' parte
     * del titolo. La prima versione la staccava tardi, dopo che la chiave della
     * pagina era gia' stata costruita: la chiave diventava
     * `photosynthesis_to_answer_which_carbohydrates…`, nessuna pagina la
     * corrispondeva, e il turno finiva nel registro sociale. Uno scopo
     * dichiarato dev'essere tolto dal turno prima che qualcuno legga il turno. */
    char scoped_norm[512];
    char scoped_raw[512];
    char purpose[KB_TERM_LEN]; purpose[0] = '\0';
    if (b->kb) {
        char cues[8][KB_TERM_LEN];
        const char *cq[1] = { NULL };
        size_t nc = kb_match(b->kb, "reading_purpose_cue", cq, 1, cues, 8);
        for (size_t i = 0; i < nc && !purpose[0]; i++) {
            const char *cue_text = kb_dequote(cues[i]);
            const char *at = strstr(norm, cue_text);
            if (!at || at == norm) continue;
            const char *tail = at + strlen(cue_text);
            while (*tail == ' ' || *tail == ':') tail++;
            if (strlen(tail) < 4) continue;
            snprintf(purpose, sizeof purpose, "%s", tail);
            snprintf(scoped_norm, sizeof scoped_norm, "%.*s",
                     (int)(at - norm), norm);
            const char *rat = raw ? strstr(raw, cue_text) : NULL;
            snprintf(scoped_raw, sizeof scoped_raw, "%.*s",
                     rat ? (int)(rat - raw) : (int)strlen(raw ? raw : ""),
                     raw ? raw : "");
            norm = scoped_norm;
            raw = scoped_raw;
        }
    }
    if (purpose[0]) {
        char q[KB_TERM_LEN];
        snprintf(q, sizeof q, "\"%s\"", purpose);
        const char *ia[2] = { "bridge", q };
        kb_set_origin(b->kb, KB_REFLECTIVE);
        kb_assert(b->kb, "reading_intent", ia, 2);
        kb_set_origin(b->kb, KB_SESSION);
    }

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
    /* TODO(kb-first): le TESTE di domanda che chiedono di documentarsi, con la
     * distinzione forte/debole scritta come due array. E' `intent_cue` piu'
     * una priorita' dichiarata — la stessa forma di `chitchat_reaction/2`
     * (gen403), che questa distinzione la esprime come dato. */
    char heads[128][KB_TERM_LEN], kinds[128][KB_TERM_LEN];
    size_t nheads = 0;
    const char *hq[2] = { NULL, NULL };
    char rawheads[128][KB_TERM_LEN];
    size_t nr = kb_match(b->kb, "knowledge_head", hq, 2, rawheads, 128);
    for (size_t i = 0; i < nr && nheads < 128; i++) {
        const char *kq[2] = { rawheads[i], NULL };
        char kind[1][KB_TERM_LEN];
        if (kb_match(b->kb, "knowledge_head", kq, 2, kind, 1) != 1) continue;
        snprintf(heads[nheads], KB_TERM_LEN, "%s", kb_dequote(rawheads[i]));
        snprintf(kinds[nheads], KB_TERM_LEN, "%s", kb_dequote(kind[0]));
        nheads++;
    }
    /* gen243: answer in the language of the QUESTION (which head matched), not the
     * flaky session-language detector — an English "tell me about foo" must reply in
     * English even if marker counting guessed Italian. The Italian heads are the
     * ones carrying an Italian-only token; flag the match. */
    const char *matched = NULL;
    char learned_head[KB_TERM_LEN] = "";
    for (size_t hi = 0; hi < nheads; hi++) {
        if (strcmp(kinds[hi], "strong") != 0) continue;
        size_t hl = strlen(heads[hi]);
        if (strncmp(work, heads[hi], hl) == 0) { x = work + hl; matched = heads[hi]; break; }
    }
    /* gen335e: if canonical form didn't match, try raw normalized form.
     * Italian heads like "parlami di " are broken by "di"→"of" canonicalization.
     * The raw form preserves "di" and matches the original head. */
    if (!x && rawbuf[0]) {
        for (size_t hi = 0; hi < nheads; hi++) {
            if (strcmp(kinds[hi], "strong") != 0) continue;
            size_t hl = strlen(heads[hi]);
            if (strncmp(rawbuf, heads[hi], hl) == 0) {
                x = rawbuf + hl; matched = heads[hi]; use_raw = 1; break;
            }
        }
    }
    /* Articulated heads are an open linguistic class.  The KB supplies them
     * through knowledge_head/2; this bridge keeps the topic extractor generic
     * and lets a runtime-taught head participate without a C edit. */
    if (!x && rawbuf[0]) {
        char (*heads)[KB_TERM_LEN] = NULL;
        const char *hq[2] = { NULL, NULL };
        size_t nh = 0;
        if (kb_match_all(b->kb, "knowledge_head", hq, 2, &heads, &nh)) {
            for (size_t hi = 0; hi < nh; hi++) {
                char probe[KB_TERM_LEN];
                snprintf(probe, sizeof probe, "%s", heads[hi]);
                const char *head = kb_dequote(probe);
                size_t hl = strlen(head);
                if (hl && strncmp(rawbuf, head, hl) == 0) {
                    x = rawbuf + hl;
                    use_raw = 1;
                    snprintf(learned_head, sizeof learned_head, "%s", head);
                    matched = learned_head;
                    break;
                }
            }
        }
        free(heads);
    }
    if (!x) for (size_t hi = 0; hi < nheads; hi++) {
        if (strcmp(kinds[hi], "weak") != 0) continue;
        size_t hl = strlen(heads[hi]);
        if (strncmp(work, heads[hi], hl) == 0) { x = work + hl; weak = 1; matched = heads[hi]; break; }
    }
    if (!x || !*x) return 0;
    if (matched && b->kb) {
        char hq2[KB_TERM_LEN];
        snprintf(hq2, sizeof hq2, "\"%s\"", matched);
        const char *ha[1] = { hq2 };
        int po = kb_origin(b->kb);
        kb_set_origin(b->kb, KB_REFLECTIVE);
        kb_assert(b->kb, "turn_knowledge_head", ha, 1);
        kb_set_origin(b->kb, po);
    }
    int it = matched && (kb_cue_match(b, "50_self_research_loop_lex943", matched) ||kb_cue_match(b, "50_self_research_loop_lex943_2", matched) ||kb_cue_match(b, "50_self_research_loop_lex943_3", matched) ||kb_cue_match(b, "50_self_research_loop_lex944", matched) ||kb_cue_match(b, "50_self_research_loop_lex944_2", matched) ||kb_cue_match(b, "50_self_research_loop_lex945", matched) ||kb_cue_match(b, "50_self_research_loop_lex945_2", matched) ||kb_cue_match(b, "50_self_research_loop_lex946", matched) ||kb_cue_match(b, "50_self_research_loop_lex946_2", matched) ||kb_cue_match(b, "50_self_research_loop_lex947", matched));

    /* ── gen505v — IL DECIMO CONSUMATORE, E QUELLO A CUI SERVE DI PIU' ────────
     *
     * Il mestiere di questa facolta' e' proprio «di che cosa si sta chiedendo»,
     * e finora la coda del turno la prendeva intera. Con un ambito dentro,
     * due righe piu' sotto la scartavano — `nt - start != 1` per la forma
     * debole, e il rifiuto delle preposizioni — quindi:
     *
     *     cosa e' lo zugzwang                 -> «Su zugzwang non so ancora
     *                                             molto. Vuoi che cerchi?»
     *     cosa e' lo zugzwang negli scacchi   -> «Non capisco ancora.»
     *
     * Stessa domanda, un ambito in piu', e il muro perdeva il termine: un
     * `blind_wall` al posto di un muro che nomina. Il template italiano c'era
     * gia' — non mancava una frase, mancava di arrivarci.
     *
     * Il fuoco del turno lo dice, ed e' gia' pubblicato: si legge quello invece
     * della coda grezza. Se non c'e' fuoco, o non restringe, tutto si comporta
     * come prima. */
    char xfocus[160];
    if (p0_current_question_focus(b, norm, xfocus, sizeof xfocus) &&
        *xfocus && strlen(xfocus) < strlen(x))
        x = xfocus;

    /* Build the concept key (drop a leading article, join words with '_') and a
     * display form; guard pronouns and too-short topics. */
    char xbuf[160]; snprintf(xbuf, sizeof xbuf, "%s", x);
    char *tok[16]; size_t nt = split_words(xbuf, tok, 16);
    size_t start = 0;
    if (nt > 0 && (is_article(b, tok[0]) ||
                   lex_class_member(b, "english_determiner", tok[0]) || lex_class_member(b, "italian_determiner", tok[0]) ||
                   lex_class_member(b, "italian_determiner", tok[0])  || lex_class_member(b, "italian_determiner", tok[0]) ||
                   lex_class_member(b, "italian_determiner", tok[0])  || lex_class_member(b, "italian_determiner", tok[0]) ||
                   lex_class_member(b, "italian_determiner", tok[0]) || lex_class_member(b, "italian_determiner", tok[0]) ||
                   lex_class_member(b, "italian_determiner", tok[0]) || lex_class_member(b, "italian_determiner", tok[0])))
        start = 1;
    if (start >= nt) return 0;
    if (is_entity_pronoun(b, tok[start])) return 0;
    /* A bare "what is <X>" is only definitional for a single concept word. */
    if (weak && nt - start != 1) return 0;
    /* Reject arithmetic / numeric expressions in any head (they should wall). */
    for (size_t i = start; i < nt; i++) {
        char *t = strip_edge_punct(tok[i]);
        for (char *c = t; *c; c++) if (isdigit((unsigned char)*c)) return 0;
        if (lex_class_member(b, "conjunction", t)||lex_class_member(b, "arithmetic_operator_word", t)||lex_class_member(b, "arithmetic_operator_word", t)||
            lex_class_member(b, "arithmetic_operator_word", t)||lex_class_member(b, "preposition", t)||lex_class_member(b, "arithmetic_operator_word", t)||
            !strcmp(t,"più")||lex_class_member(b, "arithmetic_operator_word", t)||lex_class_member(b, "arithmetic_operator_word", t)||
            lex_class_member(b, "arithmetic_operator_word", t)||lex_class_member(b, "arithmetic_operator_word", t))
            return 0;
    }

    /* Keep the user's topic intact for display and fallback. Translation is
     * performed below on the WHOLE phrase by the shared canonicalizer: a
     * learned multi-word name must not be destroyed by a second tokenwise
     * translator, nor depend on whether its English spelling fits in-place. */
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

    /* gen335 (KB-first morphology): normalize plural key to singular for lookup.
     * "cavalli" → "cavallo" so Wikipedia finds the animal, not the surname. */
    research_lemma_key(b, key, sizeof key);

    /* ── gen506 — NON SI VA A LEGGERE CIO' CHE SI SA GIA' (glm-test §3.5) ────
     *
     * Reperto di Buffy, ed e' la «malattia radice» che il report mette per
     * prima: la stessa entita' sotto forme di domanda diverse.
     *
     *     A wombat is a marsupial      ->  «Learned: wombat is a marsupial.»
     *     what do you know about wombat ->  «wombat is a marsupial.»     ✅
     *     what is a wombat?            ->  «I don't know much about wombat»
     *
     * La conoscenza c'era e una delle due porte non ci arrivava. Ma non era il
     * lettore definitorio a essere cieco — `kb_define_entity` legge benissimo i
     * fatti unari: era QUESTA facolta' a prendersi il turno prima, per andare a
     * cercare fuori cio' che parrot0 aveva gia' dentro.
     *
     * E' il mantra #21: rivendicare un turno e' un titolo, non un diritto. Il
     * mestiere dell'acquisizione e' colmare una LACUNA; dove la lacuna non c'e'
     * non ha titolo, e cede a chi sa gia' rispondere. Nessuna parola nuova nel
     * C: si chiede se la definizione esiste, con la stessa funzione che la
     * risposta userebbe. */
    {
        char known[1024];
        if (kb_define_entity(b->kb, key, known, sizeof known)) return 0;
    }

    /* deep-reasoning M2: a DEEP read extracts every fact from the page's prose
     * (extract_page_facts), each with its source (M1) — distinct from the shallow
     * concept-learn below. Honest miss if the page has no page or no facts. */
    int deep = matched && (kb_cue_match(b, "50_self_research_loop_lex1016", matched) ||kb_cue_match(b, "50_self_research_loop_lex1016_2", matched) ||kb_cue_match(b, "50_self_research_loop_lex1017", matched) ||kb_cue_match(b, "50_self_research_loop_lex1018", matched) ||kb_cue_match(b, "50_self_research_loop_lex1019", matched));
    if (deep) {
        char facts[512];
        facts[0] = '\0';   /* l'estrattore puo' uscire prima di scrivere qui */
        int nf = extract_page_facts(b, key, facts, sizeof facts);
        /* L'intenzione vive solo per la durata della lettura: e' come si legge
         * QUESTA pagina, non una proprieta' di parrot0. */
        if (purpose[0]) {
            const char *iq[2] = { "bridge", NULL };
            char row[1][KB_TERM_LEN];
            while (kb_match(b->kb, "reading_intent", iq, 2, row, 1) > 0) {
                const char *ra[2] = { "bridge", row[0] };
                if (!kb_retract(b->kb, "reading_intent", ra, 2)) break;
            }
        }
        char dmsg[700];
        if (nf > 0)
            snprintf(dmsg, sizeof dmsg,
                     it ? "Dalla pagina su %s ho estratto %d fatti: %s."
                        : "From the %s page I extracted %d facts: %s.",
                     disp, nf, facts);
        else if (facts[0])
            /* gen382: l'estrattore ha un MOTIVO (es. la pagina disambigua piu'
             * significati). Dirlo e' diverso dal dire "non ho una pagina": il
             * primo e' un declino informato, il secondo nasconde che la pagina
             * c'era ed e' stata rifiutata apposta. */
            snprintf(dmsg, sizeof dmsg,
                     it ? "Ho una pagina su %s ma %s."
                        : "I have a page on %s but %s.", disp, facts);
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

    /* A raw-language head retains its original topic for display, but lookup
     * uses the SAME phrase-aware canonicalization as the rest of the brain.
     * Otherwise an alias learned at runtime works in an English question and
     * is torn back into unrelated words in a raw-language question. */
    char key_en[80] = "";
    if (use_raw) {
        char topic_canon[256];
        canonicalize_lang(b, x, topic_canon, sizeof topic_canon);
        char *ctok[64];
        size_t cn = split_words(topic_canon, ctok, 64);
        size_t cs = 0;
        if (cn && (is_article(b, ctok[0]) ||
                   lex_class_member(b, "english_determiner", ctok[0]) ||
                   lex_class_member(b, "italian_determiner", ctok[0]))) cs = 1;
        size_t keo = 0;
        for (size_t i = cs; i < cn; i++) {
            char *t = strip_edge_punct(ctok[i]);
            if (!*t) continue;
            keo += (size_t)snprintf(key_en + keo, sizeof key_en - keo,
                                    "%s%s", keo ? "_" : "", t);
            if (keo >= sizeof key_en - 8) break;
        }
    }

    /* gen335 (KB-first morphology): also normalize the English-fallback key,
     * so a plural key_en ("cavalli") doesn't override a singular key ("cavallo")
     * and re-find the wrong concept. */
    if (key_en[0]) research_lemma_key(b, key_en, sizeof key_en);

    /* gen240 (universal-comprehension §7): pursue the precondition know(X) via the
     * acquire-knowledge action — already in RAM, learned from the local certified
     * corpus, or fetched on demand from Wikipedia (all in C). On a miss, give the
     * INFORMED decline (§2): name what was understood and be honest it can learn —
     * never a blind "I don't understand". */
    /* gen396: when the KB knows what the word DENOTES, that concept leads.
     *
     * gen335h tried the raw surface first and fell back to the canonical key
     * only on a miss, which is the right order for a word parrot0 cannot
     * translate and the wrong one for a word it can. «cosa è una pompa» went to
     * the English page named *Pompa* and came back with a commune in Moldova:
     * the surface matched a title, so the fallback never ran, and a sense the KB
     * could name lost to a homograph it could not. A question is about a concept,
     * not about a string — so once `tr/2` has resolved the surface, the resolved
     * concept is what is looked up, and the raw surface stays as the fallback for
     * everything still untranslated. */
    const char *eff_key = key;
    int st = 0;
    if (key_en[0] && strcmp(key_en, key) != 0) {
        st = acquire_knowledge(b, key_en, def, sizeof def);
        if (st) eff_key = key_en;
    }
    if (st == 0) { st = acquire_knowledge(b, key, def, sizeof def); eff_key = key; }

    /* gen335 (KB-first disambiguation): if the definition matches a disambig_flag
     * (e.g. "is a surname", "può riferirsi a"), warn the user that the result may
     * not match their intended meaning. Flags are KB facts, not C strings. */
    int disambig = 0;
    if (st && def[0]) {
        char lang[8]; current_lang(b, lang, sizeof lang);
        for (int pass = 0; pass < 2 && !disambig; pass++) {
            const char *L = pass == 0 ? lang : "en";
            const char *dq[] = { L, NULL };
            char dh[8][KB_TERM_LEN];
            size_t dn = kb_match(b->kb, "disambig_flag", dq, 2, dh, 8);
            for (size_t di = 0; di < dn; di++)
                if (strstr(def, dh[di])) { disambig = 1; break; }
            if (lex_class_member(b, "50_self_research_loop_lex1123", lang)) break;
        }
    }
    if (st == 2) {
        {
            /* gen396: say the definition in the ASKER's language when the KB
             * holds it there. The acquire path claims a turn about a concept
             * learned at runtime before the localized definitional path can, so
             * a `concept_gloss/3` taught alongside the concept stayed
             * unreachable and the reply carried an English sentence inside an
             * Italian frame. Same knowledge, chosen realization; with no gloss
             * the English one stands, which is an honest limit and not a
             * translation the engine invents. */
            char lang[8]; current_lang(b, lang, sizeof lang);
            char body[1024];
            if (lex_class_member(b, "50_self_research_loop_lex1138", lang) ||
                !kb_concept_gloss(b->kb, eff_key, lang, body, sizeof body))
                snprintf(body, sizeof body, "%s", def);
            strip_sentence_end(b, lang, body);
            const KbResponseSlot slots[] = { {"topic", disp}, {"def", body} };
            kb_response_slots(b, "learn_already_know", slots, 2, msg, sizeof msg);
        }
        if (disambig) {
            char note[100];
            kb_response_slots(b, "learn_disambig_note", NULL, 0, note, sizeof note);
            size_t ml = strlen(msg);
            snprintf(msg + ml, sizeof msg - ml, "%s", note);
        }
    }
    else if (st == 1) {
        {
            char lang[8]; current_lang(b, lang, sizeof lang);
            char body[1024];
            snprintf(body, sizeof body, "%s", def);
            strip_sentence_end(b, lang, body);
            const KbResponseSlot slots[] = { {"topic", disp}, {"def", body} };
            kb_response_slots(b, "learn_found", slots, 2, msg, sizeof msg);
        }
        if (disambig) {
            char note[100];
            kb_response_slots(b, "learn_disambig_note", NULL, 0, note, sizeof note);
            size_t ml = strlen(msg);
            snprintf(msg + ml, sizeof msg - ml, "%s", note);
        }
        /* gen335g: after a successful acquire, also extract structured facts
         * from the page prose. */
        char facts[512] = "";
        int nf = extract_page_facts(b, eff_key, facts, sizeof facts);
        if (nf > 0) {
            char fstr[16]; snprintf(fstr, sizeof fstr, "%d", nf);
            char tail[80];
            const KbResponseSlot fslots[] = { {"count", fstr} };
            if (kb_response_slots(b, "learn_extracted", fslots, 1, tail, sizeof tail)) {
                size_t ml = strlen(msg);
                snprintf(msg + ml, sizeof msg - ml, "%s", tail);
            }
        }
    }
    else if (kb_query(b->kb, "acquisition_move", (const char *[]){ "acquire" }, 1)) {
        /* gen505y — la KB ha deciso di LEGGERE, senza chiedere: la lacuna ha un
         * rimedio, la rete e' disponibile, la politica e' `act`. */
        char d2[512] = ""; int nf = 0;
        if (network_acquire(b, eff_key, d2, sizeof d2, &nf)) {
            char lang[8]; current_lang(b, lang, sizeof lang);
            char body[1024]; snprintf(body, sizeof body, "%s", d2);
            strip_sentence_end(b, lang, body);
            const KbResponseSlot slots[] = { {"topic", disp}, {"def", body} };
            kb_response_slots(b, "acquisition_read", slots, 2, msg, sizeof msg);
            if (nf > 0) {
                char fstr[16]; snprintf(fstr, sizeof fstr, "%d", nf);
                char tail[80];
                const KbResponseSlot fslots[] = { {"count", fstr} };
                if (kb_response_slots(b, "learn_extracted", fslots, 1, tail, sizeof tail)) {
                    size_t ml = strlen(msg);
                    snprintf(msg + ml, sizeof msg - ml, "%s", tail);
                }
            }
        } else {
            const KbResponseSlot slots[] = { {"topic", disp} };
            kb_response_slots(b, "learn_still_gap", slots, 1, msg, sizeof msg);
        }
    }
    else if (kb_query(b->kb, "acquisition_move", (const char *[]){ "decline_named" }, 1)) {
        /* la rete non c'e': si dice, invece di offrire cio' che non si puo' onorare */
        const KbResponseSlot slots[] = { {"topic", disp} };
        kb_response_slots(b, "acquisition_declined_network", slots, 1, msg, sizeof msg);
    }
    else if (kb_query(b->kb, "acquisition_move", (const char *[]){ "silent" }, 1)) {
        const KbResponseSlot slots[] = { {"topic", disp} };
        kb_response_slots(b, "learn_still_gap", slots, 1, msg, sizeof msg);
    }
    else if (!kb_query(b->kb, "acquisition_move", (const char *[]){ "propose" }, 1)) {
        /* nessuna mossa: la lacuna resta nominata, e nessuna offerta che non
         * poggi su un rimedio conosciuto (ablazione: senza gap_remedy_action
         * l'offerta sparisce) */
        const KbResponseSlot slots[] = { {"topic", disp} };
        kb_response_slots(b, "learn_still_gap", slots, 1, msg, sizeof msg);
    }
    else {
        /* gen335d (linguistic glue, KB-first): the informed decline now offers
         * to learn. gen335e: skip if this topic was already tried and failed.
         * gen505y: e' la mossa `propose` di kb/core/network.p0; la raccolta del
         * si' e l'azione stanno in 99-registry.c (network_acquire). */
        kb_set_origin(b->kb, KB_REFLECTIVE);
        /* gen506i: la guardia e' per TEMA, come al sito gemello di 99-registry.c
         * (gen384): un'offerta aperta su un altro tema non impedisce questa —
         * sul tabellone unico stanno insieme, e «si'» va alla piu' recente. */
        const char *gq[] = { key_en[0] ? key_en : eff_key };
        int already_gap = kb_query(b->kb, "pending_gap", gq, 1);
        const char *fq[] = { eff_key, NULL };
        int already_failed = kb_query(b->kb, "pending_gap_failed", fq, 1);
        if (!already_gap && !already_failed) {
            char qq[KB_TERM_LEN];
            /* gen335e: store the RAW question so dispatch_one can re-canonicalize
             * with full language context. The canonical form loses Italian prepositions
             * ("di"→"of") that strong heads depend on. */
            snprintf(qq, sizeof qq, "\"%s\"", raw && *raw ? raw : norm);
            board_open(b, "gap_offer", key_en[0] ? key_en : eff_key, qq);
            {
                const KbResponseSlot slots[] = { {"topic", disp} };
                kb_response_slots(b, "learn_gap_offer", slots, 1, msg, sizeof msg);
            }
        } else {
            {
                const KbResponseSlot slots[] = { {"topic", disp} };
                kb_response_slots(b, "learn_still_gap", slots, 1, msg, sizeof msg);
            }
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

/* gen342: human-facing capability names are OUTPUT wording, hence KB knowledge.
 * The C only enumerates capability(Id, Maturity), filters by maturity, and fills
 * a localized label from capability_label(Id, Lang, Text). If a capability is
 * downgraded to absent it disappears; if a label is taught/changed, this branch
 * changes without recompilation. */
static int self_capability_label(Brain *b, const char *id, char *dst, size_t dsz) {
    if (!b || !b->kb || !id || !dst || dsz == 0) return 0;
    char lang[8]; current_lang(b, lang, sizeof lang);
    for (int pass = 0; pass < 2; pass++) {
        const char *L = pass == 0 ? lang : "en";
        const char *q[3] = { id, L, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "capability_label", q, 3, hit, 1) > 0) {
            char *p = hit[0];
            size_t l = strlen(p);
            if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
            if (!*p) break;
            snprintf(dst, dsz, "%s", p);
            return 1;
        }
    }
    self_readable(dst, dsz, id);
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
    if (capability &&
        (kb_cue_match(b, "conditional_context", buf) ||
         kb_cue_match(b, "conditional_context", ribuf)))
        capability = 0;

    /* gen335 (long-conversation): "what do you do FOR FUN / in your free time / for a
     * living" is an experiential smalltalk move, not a capability query — an
     * experiential_move marker disqualifies the capability reading so mod_smalltalk's
     * honest deflect answers it. Evidence (a KB marker present), not a phrase list. */
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
    if (capability && (kb_cue_match(b, "experiential_move", buf) ||
                       kb_cue_match(b, "experiential_move", ribuf)))
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
        char _t3[512];
        const KbResponseSlot _r3[] = { { "x", "" } };
        kb_term_say(b, "from_my_capability_ledger_which_is_generated", _r3, 0, _t3, sizeof _t3);
        size_t off = (size_t)snprintf(body, sizeof body, "%s", _t3);

        if (na > 0 && off < sizeof body) {
            char absent_list[700];
            size_t absent_off = 0;
            for (size_t i = 0; i < na && absent_off < sizeof absent_list; i++) {
                char nm[KB_TERM_LEN]; self_readable(nm, sizeof nm, absent[i]);
                char wall[KB_TERM_LEN];
                int haswall = self_capability_wall(b, absent[i], wall, sizeof wall);
                absent_off += (size_t)snprintf(absent_list + absent_off,
                    sizeof absent_list - absent_off,
                    "%s%s%s%s%s", (i == 0) ? "" : (i + 1 == na) ? " or " : ", ",
                    nm, haswall ? " (it would need " : "",
                    haswall ? wall : "", haswall ? ")" : "");
            }
            char absent_msg[800];
            const KbResponseSlot slots[] = { { "items", absent_list } };
            if (!kb_response_slots(b, "capability_absent_list", slots, 1,
                                   absent_msg, sizeof absent_msg))
                kb_term_say(b, "capability_absent_list", slots, 1,
                            absent_msg, sizeof absent_msg);
            off += (size_t)snprintf(body + off, sizeof body - off, "%s", absent_msg);
        }
        if (ns > 0 && off < sizeof body) {
            { char _t4[512];
            const KbResponseSlot _r4[] = { { "x", "" } };
            kb_term_say(b, "these_i_can_only_do_as_a_seed_one_demonstrat", _r4, 0, _t4, sizeof _t4);
            off += (size_t)snprintf(body + off, sizeof body - off, "%s", _t4);
            }
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
     * contains the identity cue "what are you"), so test it first. Describe the
     * capability LEDGER, not a C-side brochure: capability/2 decides what exists
     * and capability_label/3 decides how to say it in the current language. */
    if (capability) {
        const char *levels[] = { "hardened", "field", "transfer", "seed", NULL };
        char list[900];
        size_t off = 0, n = 0;
        char lang[8]; current_lang(b, lang, sizeof lang);
        int it = lex_class_member(b, "entity_pronoun", lang);
        for (size_t lv = 0; levels[lv]; lv++) {
            char ids[24][KB_TERM_LEN];
            const char *q[2] = { NULL, levels[lv] };
            size_t ni = kb_match(b->kb, "capability", q, 2, ids, 24);
            for (size_t i = 0; i < ni; i++) {
                char label[KB_TERM_LEN];
                if (!self_capability_label(b, ids[i], label, sizeof label)) continue;
                const char *sep = "";
                if (n > 0) sep = it ? "; " : "; ";
                if (off + strlen(sep) + strlen(label) + 1 >= sizeof list) break;
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s", sep, label);
                n++;
            }
        }
        char msg[1100];
        if (n == 0) {
            if (!kb_response_slots(b, "self_capability_none", NULL, 0, msg, sizeof msg))
                kb_term_say(b, "not_much_yet", NULL, 0, msg, sizeof msg);
        } else {
            const KbResponseSlot slots[] = { {"items", list} };
            if (!kb_response_slots(b, "self_capability_from_ledger", slots, 1,
                                   msg, sizeof msg))
                { const KbResponseSlot _rs[] = { { "list", list } };
      kb_term_say(b, "from_my_capability_ledger_i_can_currently_x", _rs, 1, msg, sizeof msg); }
        }
        put(msg, out, out_size);
        store_proof(b, "Derived from capability/2 and capability_label/3 in the KB.");
        return 1;
    }

    if (identity || exists) {
        char id[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        char proof[160];
        if (k == 0)
            kb_response_slots(b, "self_identity_unknown", NULL, 0, msg, sizeof msg);
        else if (exists) {
            const KbResponseSlot slots[] = { {"name", id[0]} };
            kb_response_slots(b, "self_identity_exists", slots, 1, msg, sizeof msg);
            { const KbResponseSlot _rs[] = { { "id", id[0] } };
      kb_term_say(b, "i_am_x_is_a_reflective_fact_in_my_knowledge", _rs, 1, proof, sizeof proof); }
        }
        else {
            const KbResponseSlot slots[] = { {"name", id[0]} };
            kb_response_slots(b, "self_identity_name", slots, 1, msg, sizeof msg);
            { const KbResponseSlot _rs[] = { { "id", id[0] } };
      kb_term_say(b, "i_am_x_is_a_reflective_fact_in_my_knowledge", _rs, 1, proof, sizeof proof); }
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
    int fact_count = (kb_cue_match(b, "50_self_research_loop_cue1479", buf) && wn <= 6) ||
                     (kb_cue_match(b, "50_self_research_loop_cue1480", buf) && wn <= 5);
    if (fact_count) {
        char msg[128];
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", kb_user_facts(b->kb));
  const KbResponseSlot _rs[] = { { "kb", _v0 } };
          kb_term_say(b, "i_know_x_fact_s", _rs, 1, msg, sizeof msg); }
        put(msg, out, out_size);
        return 1;
    }

    /* "what predicates do you know?" / "what topics do you know about?" */
    int pred_list = (kb_cue_match(b, "50_self_research_loop_cue1489", buf) && wn <= 5) ||
                    (kb_cue_match(b, "50_self_research_loop_cue1490", buf) && wn <= 6) ||
                    (kb_cue_match(b, "50_self_research_loop_cue1491", buf) && wn <= 4);
    if (pred_list) {
        /* gen382e: anche qui il tetto di 128 tagliava, e con una KB cresciuta la
         * risposta degradava a "I know 128 distinct predicate(s)" — un numero
         * che era il TETTO, non la conoscenza. Dimensionato sui fatti reali. */
        size_t pcap = kb_size(b->kb) ? kb_size(b->kb) : 1;
        char (*preds)[KB_TERM_LEN] = malloc(pcap * KB_TERM_LEN);
        if (!preds) return 0;
        size_t np = kb_user_predicates(b->kb, preds, pcap);
        if (np == 0) { kb_term_say(b, "i_don_t_know_any_predicates_yet", NULL, 0, out, out_size); return 1; }
        char list[1024];
        size_t off = 0;
        for (size_t i = 0; i < np && off < sizeof list; i++)
            off += (size_t)snprintf(list + off, sizeof list - off,
                                     "%s%s", i ? ", " : "", preds[i]);
        char msg[1100];
        if (off < sizeof list)
            { const KbResponseSlot _rs[] = { { "list", list } };
      kb_term_say(b, "i_know_these_predicates_x", _rs, 1, msg, sizeof msg); }
        else
            { 
              char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", np);
  const KbResponseSlot _rs[] = { { "np", _v0 } };
              kb_term_say(b, "i_know_x_distinct_predicate_s", _rs, 1, msg, sizeof msg); }
        put(msg, out, out_size);
        return 1;
    }

    /* "show me your knowledge" / "dump everything" */
    int show_knowledge = (kb_cue_match(b, "50_self_research_loop_cue1516", buf) && wn <= 5) ||
                         (kb_cue_match(b, "50_self_research_loop_cue1517", buf) && wn <= 7) ||
                         (kb_cue_match(b, "50_self_research_loop_cue1518", buf) && wn <= 3) ||
                         (kb_cue_match(b, "50_self_research_loop_cue1519", buf) && wn <= 6) ||
                         (kb_cue_match(b, "50_self_research_loop_cue1520", buf) && wn <= 4);
    if (show_knowledge) {
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            { const KbResponseSlot _rs[] = { { "dump", dump } };
              kb_term_say(b, "here_is_everything_i_know_x", _rs, 1, msg, sizeof msg);
              put(msg, out, out_size); }
        } else {
            char msg[128];
            { 
              char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", kb_user_facts(b->kb));
  const KbResponseSlot _rs[] = { { "kb", _v0 } };
              kb_term_say(b, "i_know_x_fact_s_total", _rs, 1, msg, sizeof msg); }
            put(msg, out, out_size);
        }
        return 1;
    }

    /* "what do you know?" → stats overview. Only claims short forms;
     * "what do you know about X?" has more words and reaches mod_knowledge. */
    int what_know = (kb_cue_match(b, "50_self_research_loop_cue1539", buf) && wn <= 4) ||
                    (kb_cue_match(b, "50_self_research_loop_cue1540", buf) && wn <= 3);
    if (what_know) {
        size_t nfacts = kb_user_facts(b->kb);
        /* gen382e: anche qui il tetto di 128 tagliava, e con una KB cresciuta la
         * risposta degradava a "I know 128 distinct predicate(s)" — un numero
         * che era il TETTO, non la conoscenza. Dimensionato sui fatti reali. */
        size_t pcap = kb_size(b->kb) ? kb_size(b->kb) : 1;
        char (*preds)[KB_TERM_LEN] = malloc(pcap * KB_TERM_LEN);
        if (!preds) return 0;
        size_t np = kb_user_predicates(b->kb, preds, pcap);
        char msg[256];
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", nfacts);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%zu", np);
  const KbResponseSlot _rs[] = { { "nfacts", _v0 }, { "np", _v1 } };
          kb_term_say(b, "i_know_x_fact_s_across_x_predicate_s_ask_me", _rs, 2, msg, sizeof msg); }
        put(msg, out, out_size);
        return 1;
    }

    /* gen78: "which part of you answered that?" / "what module handled that?"
     * Reads from the last_module stored by the dispatch loop. */
    int which_module = (kb_cue_match(b, "50_self_research_loop_cue1560", buf) && wn <= 6) ||
                       (kb_cue_match(b, "50_self_research_loop_cue1561", buf) && wn <= 4) ||
                       (kb_cue_match(b, "50_self_research_loop_cue1562", buf) && wn <= 4) ||
                       (kb_cue_match(b, "50_self_research_loop_cue1563", buf) && wn <= 6) ||
                       (kb_cue_match(b, "50_self_research_loop_cue1564", buf) && wn <= 4);
    if (which_module) {
        if (b->last_module[0]) {
            char msg[160];
            if (strcmp(b->last_module, "fallback") == 0)
                {   const KbResponseSlot _rs[] = { { "x", "" } };
                  kb_term_say(b, "no_module_could_handle_that_it_fell_through", _rs, 0, msg, sizeof msg); }
            else
                { 
                  char _v0[48]; snprintf(_v0, sizeof _v0, "%s", b->last_module);
  const KbResponseSlot _rs[] = { { "last_module", _v0 } };
                  kb_term_say(b, "the_x_module_answered_your_last_question", _rs, 1, msg, sizeof msg); }
            put(msg, out, out_size);
        } else {
            kb_term_say(b, "i_haven_t_answered_anything_yet", NULL, 0, out, out_size);
        }
        return 1;
    }

    /* gen83 entities_q ... (above) */

    /* gen86 mod_cap ... (above) */

    /* gen89: "what have I taught you?" — show only session facts. */
    int taught_q = (kb_cue_match(b, "50_self_research_loop_cue1588", buf) && wn <= 6) ||
                   (kb_cue_match(b, "50_self_research_loop_cue1589", buf) && wn <= 6) ||
                   (kb_cue_match(b, "50_self_research_loop_cue1590", buf) && wn <= 5);
    if (taught_q) {
        /* Dump only user-facing predicates, regardless of origin.
         * kb_user_facts already filters internal predicates. */
        char dump[4096];
        if (kb_dump_user(b->kb, dump, sizeof dump)) {
            char msg[4200];
            { const KbResponseSlot _rs[] = { { "dump", dump } };
      kb_term_say(b, "you_taught_me_x", _rs, 1, msg, sizeof msg);
              put(msg, out, out_size); }
        } else {
            kb_term_say(b, "you_haven_t_taught_me_any_facts_yet", NULL, 0, out, out_size);
        }
        return 1;
    }
    int mod_cap = (kb_cue_match(b, "50_self_research_loop_cue1606", buf) && wn <= 7) ||
                  (kb_cue_match(b, "50_self_research_loop_cue1607", buf) && wn <= 7) ||
                  (kb_cue_match(b, "50_self_research_loop_cue1608", buf) && wn <= 7);
    if (mod_cap) {
        if (kb_cue_match(b, "50_self_research_loop_chain1610", buf))
            return 0;
        char cmap[24][2][KB_TERM_LEN];
        const char *cq[] = { NULL, NULL };
        char ckeys[24][KB_TERM_LEN];
        size_t ncmap = kb_match(b->kb, "module_capability", cq, 2, ckeys, 24);
        for (size_t i = 0; i < ncmap; i++) {
            const char *sq[] = {ckeys[i], NULL};
            if (kb_match(b->kb, "module_capability", sq, 2, cmap[i], 1) != 1)
                continue;
            snprintf(cmap[i][0], KB_TERM_LEN, "%s", ckeys[i]);
            if (cue(buf, kb_dequote(cmap[i][0]))) {
                char msg[256];
                { 
                  char _v0[48]; snprintf(_v0, sizeof _v0, "%s", kb_dequote(cmap[i][0]));
                  char _v1[48]; snprintf(_v1, sizeof _v1, "%s", kb_dequote(cmap[i][1]));
  const KbResponseSlot _rs[] = { { "mod", _v0 }, { "say", _v1 } };
                  kb_term_say(b, "the_x_module_can_x", _rs, 2, msg, sizeof msg); }
                put(msg, out, out_size);
                return 1;
            }
        }
        /* gen505f — UN MODULO NON RIVENDICA CIO' CHE NON SERVE.
         *
         * La cue che apre questo ramo e' «what does the», un prefisso di TRE
         * parole che cattura qualunque domanda cominci cosi': «what does the
         * senate govern?» finiva qui e riceveva «I don't have a module by that
         * name», cioe' una risposta fuori tema data con sicurezza a una domanda
         * di dominio. E il muro scattava anche quando nessun modulo era stato
         * nominato — una facolta' che non sa rispondere prendeva il turno lo
         * stesso.
         *
         * La cura non e' una cessione (`turn-arbitration.md` vieta la quarta
         * riga di `faculty_yield`): e' il gradino zero di quel documento, la
         * LEGITTIMITA'. Il muro resta giusto quando il turno nomina davvero un
         * modulo — il registro e' `module/1`, 78 fatti — o quando nomina il
         * frame («module», «modulo»: conoscenza, `module_frame_word/1`).
         * Altrimenti il turno prosegue verso chi sa leggerlo. */
        {
            char nb[300]; snprintf(nb, sizeof nb, "%s", buf);
            char *nw2[48]; size_t nn2 = split_words(nb, nw2, 48);
            int names_module = 0;
            for (size_t i = 0; i < nn2 && !names_module; i++) {
                char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", nw2[i]);
                const char *t = strip_edge_punct(tb);
                if (!*t) continue;
                const char *q[] = { t };
                if (kb_query(b->kb, "module", q, 1) ||
                    kb_query(b->kb, "module_frame_word", q, 1)) names_module = 1;
            }
            if (!names_module) return 0;
        }
        kb_term_say(b, "i_don_t_have_a_module_by_that_name_ask_what", NULL, 0, out, out_size);
        return 1;
    }
    int entities_q = (kb_cue_match(b, "50_self_research_loop_cue1640", buf) && wn <= 6) ||
                     (kb_cue_match(b, "50_self_research_loop_cue1641", buf) && wn <= 5) ||
                     (kb_cue_match(b, "50_self_research_loop_cue1642", buf) && wn <= 7) ||
                     (kb_cue_match(b, "50_self_research_loop_cue1643", buf) && wn <= 5);
    if (entities_q) {
        if (b->entity_count == 0) {
            kb_term_say(b, "you_haven_t_mentioned_any_names_i_reco", NULL, 0, out, out_size);
        } else {
            char list[512]; size_t off = 0;
            for (size_t i = 0; i < b->entity_count && off < sizeof list; i++)
                off += (size_t)snprintf(list + off, sizeof list - off,
                                         "%s%s", i ? ", " : "", b->entities[i]);
            char msg[600];
            { const KbResponseSlot _rs[] = { { "list", list } };
      kb_term_say(b, "you_mentioned_x", _rs, 1, msg, sizeof msg);
              put(msg, out, out_size); }
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
        { const KbResponseSlot _rs[] = { { "lc", lc } };
      kb_term_say(b, "i_don_t_know_the_command_x", _rs, 1, desc, desc_size); }
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
        { char _t5[512];
        const KbResponseSlot _r5[] = { { "unknown", unknown } };
        kb_term_say(b, "i_don_t_know_the_option_x", _r5, 1, _t5, sizeof _t5);
        snprintf(desc + o, desc_size - o, "%s", _t5);
        }
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

/* ══════════════════════════════════════════════════════════════════════════
 * LETTURA GUIDATA DAL FRAME APERTO — kb/core/guided-reading.p0
 * docs/plans/multi-hop-deep-memory.md §4.4 (strada 2), banco §7.0
 *
 * «In which country is the museum that holds The Starry Night?» Si parte da cio'
 * che la domanda nomina e che ha una pagina; nella pagina si cerca il tipo chiesto
 * (un museo); se c'e', la risposta diventa la pagina successiva e si cerca il tipo
 * dopo (un paese); se non c'e', la frase piu' pertinente sceglie la pagina ponte.
 *
 * Qui c'e' soltanto la meccanica: scaricare, dividere in frasi, trovare i nomi
 * propri dall'ortografia e i sintagmi con una testa data, contare, girare. Ogni
 * decisione — che cosa si chiede, quali parole sono indizi o prove, quando un
 * candidato e' del tipo giusto, come si dice la catena — e' una domanda alla KB.
 *
 * ⚠ Debito (MANTRA #24): frasi e menzioni sono strutture locali di questa funzione,
 * non ancora nodi della Document IR. Gemelli e migrazione: guided-reading.p0 in
 * testa, e il piano §4.3 (IR1, IR2, IR6).
 * ══════════════════════════════════════════════════════════════════════════ */

enum { GR_MAX_SENT = 16, GR_MAX_TOK = 96, GR_MAX_MENT = 24, GR_MAX_PAGES = 6, GR_MAX_TYPES = 4 };

typedef struct {
    char text[600];                          /* la frase, con le maiuscole */
    char tok[GR_MAX_TOK][KB_TERM_LEN];       /* token originali, senza punteggiatura ai bordi */
    char low[GR_MAX_TOK][KB_TERM_LEN];       /* gli stessi, in minuscolo */
    unsigned char stop[GR_MAX_TOK];          /* il token chiudeva con una punteggiatura */
    char orig[GR_MAX_TOK][KB_TERM_LEN];      /* il token com'era scritto, per citare */
    int head_first;                          /* la lingua del testo mette la testa prima («Mar Nero») */
    size_t ntok;
} GrSentence;

typedef struct {
    char topic[KB_TERM_LEN];
    char prose[4096];
    char edition[32], title[160], revision[64], fixdir[KB_TERM_LEN];
    GrSentence sent[GR_MAX_SENT];
    size_t nsent;
} GrPage;

typedef struct {
    char surface[KB_TERM_LEN];   /* «Museum of Modern Art» */
    char key[KB_TERM_LEN];       /* museum_of_modern_art */
    char head[KB_TERM_LEN];      /* museum: la parola prima di un connettore, o l'ultima */
    size_t start, end;           /* token [start, end) */
    int modifier;                /* «French composer»: maiuscola che modifica un nome comune */
} GrMention;

static void gr_key_of(const char *surface, char *key, size_t ksz) {
    size_t o = 0;
    for (const char *c = surface; *c && o + 1 < ksz; c++) {
        unsigned char ch = (unsigned char)*c;
        if (ch == ' ' || ch == '-') { if (o && key[o - 1] != '_') key[o++] = '_'; }
        else if (ch == '\'' || ch == '.' || ch == ',') continue;
        else key[o++] = (char)tolower(ch);
    }
    while (o && key[o - 1] == '_') o--;
    key[o] = '\0';
}

static int gr_kb1(Brain *b, const char *pred, const char *a) {
    const char *q[1] = { a };
    return kb_query(b->kb, pred, q, 1);
}
static int gr_kb2(Brain *b, const char *pred, const char *a, const char *c) {
    const char *q[2] = { a, c };
    return kb_query(b->kb, pred, q, 2);
}
static int gr_same(Brain *b, const char *w, const char *t) {
    if (!*w || !*t) return 0;
    if (!strcmp(w, t)) return 1;
    /* la stessa parola in un'altra lingua: un fatto, costa una ricerca */
    if (gr_kb2(b, "tr", w, t) || gr_kb2(b, "tr", t, w)) return 1;
    if (strncmp(w, t, 3)) return 0;          /* meccanica: la morfologia si chiede solo se puo' essere */
    return gr_kb2(b, "same_word_lemma", w, t);
}

static void gr_split_sentences(Brain *b, GrPage *pg) {
    pg->nsent = 0;
    const char *p = pg->prose;
    while (*p && pg->nsent < GR_MAX_SENT) {
        while (*p == ' ' || *p == '\n') p++;
        const char *q = p;
        while (*q && !(((*q == '.' || *q == '!' || *q == '?') &&
                        (!q[1] || q[1] == ' ' || q[1] == '\n') &&
                        !p0_boundary_inside_number(b, pg->prose, (char *)q)))) q++;
        size_t len = (size_t)(q - p);
        if (len > 3) {
            GrSentence *s = &pg->sent[pg->nsent++];
            snprintf(s->text, sizeof s->text, "%.*s", (int)(len < sizeof s->text ? len : sizeof s->text - 1), p);
            char buf[600]; snprintf(buf, sizeof buf, "%s", s->text);
            char *w[GR_MAX_TOK]; size_t n = split_words(buf, w, GR_MAX_TOK);
            s->ntok = 0;
            for (size_t i = 0; i < n && s->ntok < GR_MAX_TOK; i++) {
                char t[KB_TERM_LEN]; snprintf(t, sizeof t, "%s", w[i]);
                char orig_tok[KB_TERM_LEN]; snprintf(orig_tok, sizeof orig_tok, "%s", w[i]);
                /* il possessivo «'s» e la punteggiatura non sono parte della parola */
                size_t tl = strlen(t);
                unsigned char closed = 0;
                while (tl && strchr(",;:()\"«»", t[tl - 1])) { t[--tl] = '\0'; closed = 1; }
                if (tl > 2 && t[tl - 2] == '\'' && t[tl - 1] == 's') { t[tl - 2] = '\0'; tl -= 2; }
                char *st = t; while (*st && strchr("(\"«", *st)) st++;
                /* l'articolo eliso non fa parte del nome: «dell'Ungheria», «l'Italia» */
                {
                    char *ap = strchr(st, '\'');
                    if (!ap) ap = strstr(st, "\xE2\x80\x99");
                    if (ap && ap > st && ap - st <= 4 && islower((unsigned char)st[0])) {
                        char *after = ap + (*ap == '\'' ? 1 : 3);
                        if (isalpha((unsigned char)*after)) st = after;
                    }
                }
                if (!*st) continue;
                s->stop[s->ntok] = closed;
                snprintf(s->orig[s->ntok], KB_TERM_LEN, "%s", orig_tok);
                snprintf(s->tok[s->ntok], KB_TERM_LEN, "%s", st);
                for (size_t k = 0; ; k++) { s->low[s->ntok][k] = (char)tolower((unsigned char)st[k]); if (!st[k]) break; }
                s->ntok++;
            }
        }
        if (!*q) break;
        p = q + 1;
    }
}

/* Le menzioni: corse di parole con l'iniziale maiuscola, con dentro i connettori che
 * la KB dichiara (`name_connector/1`: «of», «da», «von»). La prima parola di una
 * frase conta solo se non e' una parola funzione. */
static size_t gr_mentions(Brain *b, const GrSentence *s, GrMention *m, size_t max) {
    size_t n = 0;
    for (size_t i = 0; i < s->ntok && n < max; ) {
        int cap = isupper((unsigned char)s->tok[i][0]);
        if (cap && i == 0 && (is_stopword(b, (char *)s->low[i]) || gr_kb1(b, "english_determiner", s->low[i]))) cap = 0;
        /* a inizio frase, una sola parola maiuscola che il lessico conosce come comune */
        if (cap && i == 0 && (i + 1 >= s->ntok || !isupper((unsigned char)s->tok[i + 1][0])) &&
            gr_kb1(b, "common_lexical_word", s->low[i])) cap = 0;
        if (!cap) { i++; continue; }
        size_t j = i + 1, last = i;
        while (j < s->ntok && !s->stop[j - 1]) {
            if (isupper((unsigned char)s->tok[j][0])) { last = j; j++; continue; }
            if (gr_kb1(b, "name_connector", s->low[j]) && j + 1 < s->ntok &&
                isupper((unsigned char)s->tok[j + 1][0])) { j++; continue; }
            break;
        }
        GrMention *g = &m[n++];
        size_t o = 0; g->surface[0] = '\0'; g->head[0] = '\0';
        int seen_connector = 0;
        for (size_t k = i; k <= last; k++) {
            o += (size_t)snprintf(g->surface + o, sizeof g->surface - o, "%s%s", k > i ? " " : "", s->tok[k]);
            if (!seen_connector && gr_kb1(b, "name_connector", s->low[k])) {
                seen_connector = 1;
                snprintf(g->head, sizeof g->head, "%s", s->low[k - 1]);
            }
        }
        if (!g->head[0]) {
            /* dove sta la testa lo dice la lingua (compound_head_side/2): «Black SEA», «MAR Nero» */
            snprintf(g->head, sizeof g->head, "%s", s->head_first ? s->low[i] : s->low[last]);
        }
        gr_key_of(g->surface, g->key, sizeof g->key);
        g->start = i; g->end = last + 1;
        /* seguita, senza punteggiatura, da un nome comune pieno: e' un modificatore di quel
         * nome («the French composer Georges Bizet», «Dutch Post-Impressionist painter»),
         * non un'entita' da leggere */
        g->modifier = (last + 1 < s->ntok && !s->stop[last] && islower((unsigned char)s->tok[last + 1][0]) &&
                       !is_stopword(b, (char *)s->low[last + 1]) && !gr_kb1(b, "name_connector", s->low[last + 1]) &&
                       !gr_kb1(b, "definition_copula", s->low[last + 1]) && !gr_kb1(b, "relative_clause_verb", s->low[last + 1]));
        i = last + 1;
    }
    return n;
}

static int gr_fetch(Brain *b, const char *topic, GrPage *pg) {
    memset(pg, 0, sizeof *pg);
    snprintf(pg->topic, sizeof pg->topic, "%s", topic);
    if (!network_fetch_page(b, topic, pg->prose, sizeof pg->prose, pg->edition, pg->title,
                            pg->revision, pg->fixdir)) return 0;
    gr_split_sentences(b, pg);
    {
        /* la lingua della pagina e' quella della sua edizione; l'edizione locale e' inglese */
        const char *hq[2] = { pg->edition, "first" };
        int hf = kb_query(b->kb, "compound_head_side", hq, 2);
        for (size_t i = 0; i < pg->nsent; i++) pg->sent[i].head_first = hf;
    }
    return pg->nsent > 0;
}

/* Il pezzo di frase attorno a un candidato: dieci parole prima, quattro dopo. La
 * citazione intera di una frase d'enciclopedia supera la lunghezza di un atomo. */
static void gr_excerpt(const GrSentence *s, size_t start, size_t end, char *out, size_t osz) {
    size_t from = start > 10 ? start - 10 : 0, to = end + 4 < s->ntok ? end + 4 : s->ntok;
    size_t o = 0; out[0] = '\0';
    if (from > 0) o += (size_t)snprintf(out + o, osz - o, "… ");
    for (size_t i = from; i < to && o + 1 < osz; i++)
        o += (size_t)snprintf(out + o, osz - o, "%s%s", i > from ? " " : "", s->orig[i]);
    if (to < s->ntok && o + 4 < osz) snprintf(out + o, osz - o, " …");
}

/* La pagina di una menzione: il nome intero, oppure senza la classe davanti
 * («River Danube» → Danube) quando la KB la dichiara (`name_class_prefix/1`). */
static int gr_fetch_mention(Brain *b, const GrMention *m, GrPage *pg) {
    if (gr_fetch(b, m->key, pg)) return 1;
    /* sulla rete il titolo ha le maiuscole: «Thus Spoke Zarathustra», non la chiave */
    if (m->surface[0]) {
        char tt[KB_TERM_LEN]; snprintf(tt, sizeof tt, "%s", m->surface);
        for (char *c = tt; *c; c++) if (*c == ' ') *c = '_';
        if (strcmp(tt, m->key) && gr_fetch(b, tt, pg)) {
            snprintf(pg->topic, sizeof pg->topic, "%s", m->key);   /* la chiave resta quella della KB */
            return 1;
        }
    }
    const char *us = strchr(m->key, '_');
    if (!us) return 0;
    char first[KB_TERM_LEN]; snprintf(first, sizeof first, "%.*s", (int)(us - m->key), m->key);
    if (!gr_kb1(b, "name_class_prefix", first)) return 0;
    return gr_fetch(b, us + 1, pg);
}

/* La pagina che la ricerca propone per una frase: risultati veri registrati per l'edizione
 * locale (`search.tsv`), o la ricerca di Wikipedia; il primo risultato leggibile che nomina
 * almeno due indizi del turno. */
static int gr_fetch(Brain *b, const char *topic, GrPage *pg);
static int gr_cue_hits_fwd(Brain *b, const GrSentence *s, char (*cues)[KB_TERM_LEN], size_t ncue);
static int gr_type_ok(Brain *b, const GrMention *m, const char *type, GrPage *scratch);
static int gr_search_page(Brain *b, const char *phrase, const char *type, char (*cues)[KB_TERM_LEN], size_t ncue, GrPage *pg) {
    char titles[2048] = "";
    char fixd[1][KB_TERM_LEN]; const char *fq[2] = { "fixture", NULL };
    if (kb_match(b->kb, "topic_provider", fq, 2, fixd, 1) == 1) {
        char spath[600]; snprintf(spath, sizeof spath, "%s/search.tsv", kb_dequote(fixd[0]));
        FILE *sf = fopen(spath, "r");
        if (sf) {
            char line[1024];
            while (fgets(line, sizeof line, sf)) {
                char *tab = strchr(line, '\t'); if (!tab) continue;
                *tab = '\0';
                if (strcasecmp(line, phrase)) continue;
                char *nl = strchr(tab + 1, '\n'); if (nl) *nl = '\0';
                snprintf(titles, sizeof titles, "%s", tab + 1);
                for (char *c = titles; *c; c++) if (*c == '|') *c = '\n';
                break;
            }
            fclose(sf);
        }
    }
    if (!titles[0] && kb_query(b->kb, "network_available", NULL, 0)) {
        /* la ricerca nell'edizione della lingua di chi chiede, come la lettura */
        char lang[16] = "en";
        char cl[1][KB_TERM_LEN], ed[1][KB_TERM_LEN]; const char *lq[1] = { NULL };
        if (kb_match(b->kb, "current_language", lq, 1, cl, 1) > 0) {
            const char *eq[2] = { cl[0], NULL };
            if (kb_match(b->kb, "edition_for_language", eq, 2, ed, 1) == 1) snprintf(lang, sizeof lang, "%.15s", kb_dequote(ed[0]));
        }
        if (!wiki_search_titles(phrase, lang, titles, sizeof titles) && strcmp(lang, "en"))
            wiki_search_titles(phrase, "en", titles, sizeof titles);
    }
    char *save = NULL;
    for (char *ln = strtok_r(titles, "\n", &save); ln; ln = strtok_r(NULL, "\n", &save)) {
        char tt[KB_TERM_LEN]; snprintf(tt, sizeof tt, "%s", ln);
        for (char *c = tt; *c; c++) if (*c == ' ') *c = '_';
        char key[KB_TERM_LEN]; gr_key_of(ln, key, sizeof key);
        if (!gr_fetch(b, key, pg) && !gr_fetch(b, tt, pg)) continue;
        snprintf(pg->topic, sizeof pg->topic, "%s", key);
        int hits = 0;
        for (size_t si = 0; si < pg->nsent; si++) hits += gr_cue_hits_fwd(b, &pg->sent[si], cues, ncue);
        if (hits < 2) continue;
        if (type && *type) {
            /* il risultato deve essere del tipo che la descrizione nomina: la ricerca di «il
             * fiume che attraversa…» propone anche la citta' di Fiume */
            static GrPage probe;
            GrMention sm; memset(&sm, 0, sizeof sm);
            snprintf(sm.surface, sizeof sm.surface, "%s", ln);
            snprintf(sm.key, sizeof sm.key, "%s", key);
            const char *us = strrchr(key, '_');
            snprintf(sm.head, sizeof sm.head, "%s", us ? us + 1 : key);
            if (!gr_type_ok(b, &sm, type, &probe)) continue;
        }
        return 1;
    }
    return 0;
}

/* Il tipo di un candidato: testa del nome, tipo noto in KB, o la prima frase della
 * sua pagina («X is an Iranian religion»). */
static int gr_type_ok(Brain *b, const GrMention *m, const char *type, GrPage *scratch) {
    if (gr_same(b, m->head, type)) return 1;
    /* «River Danube», «Mount Elbrus»: la classe dichiarata davanti al nome */
    {
        const char *us = strchr(m->key, '_');
        if (us) {
            char first[KB_TERM_LEN]; snprintf(first, sizeof first, "%.*s", (int)(us - m->key), m->key);
            if (gr_kb1(b, "name_class_prefix", first) && gr_same(b, first, type)) return 1;
        }
    }
    if (gr_kb2(b, "known_entity_type", m->key, type)) return 1;
    {   /* il tipo nella sua forma inglese: la KB sa che India e' un «country», non un «paese» */
        char etypes[4][KB_TERM_LEN]; const char *eq[2] = { type, NULL };
        size_t net = kb_match(b->kb, "guided_english_key", eq, 2, etypes, 4);
        for (size_t e = 0; e < net; e++)
            if (strcmp(kb_dequote(etypes[e]), type) && gr_kb2(b, "known_entity_type", m->key, kb_dequote(etypes[e]))) return 1;
    }
    if (!gr_fetch(b, m->key, scratch)) return 0;
    const GrSentence *s0 = &scratch->sent[0];
    size_t cop = s0->ntok;
    for (size_t i = 0; i < s0->ntok; i++)
        if (gr_kb1(b, "definition_copula", s0->low[i])) { cop = i; break; }
    for (size_t i = cop + 1; i < s0->ntok && i <= cop + 8; i++)
        if (gr_same(b, s0->low[i], type)) return 1;
    return 0;
}

/* Un passo avanti: la pagina di un candidato ponte, appena aperta, nomina gia' qualcosa
 * del tipo che si cerca? Si guardano le prime due frasi, senza aprire altre pagine: la
 * testa del nome, la classe davanti, o un tipo che la KB conosce. */
static size_t gr_mentions(Brain *b, const GrSentence *s, GrMention *m, size_t max);
static int gr_lookahead(Brain *b, const GrPage *pg, const char *type) {
    for (size_t si = 0; si < pg->nsent && si < 2; si++) {
        GrMention mm[GR_MAX_MENT]; size_t nm = gr_mentions(b, &pg->sent[si], mm, GR_MAX_MENT);
        for (size_t k = 0; k < nm; k++) {
            if (mm[k].modifier || !strcmp(mm[k].key, pg->topic)) continue;
            if (gr_same(b, mm[k].head, type) || gr_kb2(b, "known_entity_type", mm[k].key, type)) return 1;
        }
    }
    return 0;
}

static int gr_cue_hits_fwd(Brain *b, const GrSentence *s, char (*cues)[KB_TERM_LEN], size_t ncue);
static int gr_cue_hits(Brain *b, const GrSentence *s, char (*cues)[KB_TERM_LEN], size_t ncue) {
    int hits = 0;
    for (size_t c = 0; c < ncue; c++) {
        const char *cue = kb_dequote(cues[c]);
        for (size_t i = 0; i < s->ntok; i++)
            if (gr_same(b, s->low[i], cue)) { hits++; break; }
    }
    return hits;
}

/* Le prove prima del candidato, entro sei parole: a favore (+2) o contro (-3). */
static int gr_local_evidence(Brain *b, const GrSentence *s, const GrMention *m,
                             char (*ev)[KB_TERM_LEN], size_t nev) {
    int score = 0;
    size_t from = m->start > 6 ? m->start - 6 : 0;
    for (size_t i = from; i < m->start; i++) {
        for (size_t e = 0; e < nev; e++)
            if (!strcmp(s->low[i], kb_dequote(ev[e]))) score += 2;
        if (gr_kb1(b, "opposition_evidence", s->low[i])) score -= 3;
    }
    return score;
}

static void gr_quote(const char *text, char *out, size_t osz) {
    size_t o = 0;
    if (osz < 3) return;
    out[o++] = '"';
    for (const char *c = text; *c && o + 3 < osz; c++) {
        if (*c == '"' || *c == '\\') out[o++] = '\\';
        out[o++] = *c;
    }
    out[o++] = '"'; out[o] = '\0';
}

static void gr_assert(Brain *b, const char *pred, const char **args, size_t n) {
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_assert(b->kb, pred, args, n);
    kb_set_origin(b->kb, prev);
}

static int guided_reading_lead(Brain *b, const char *norm, const char *raw, char *out, size_t out_size) {
    if (!b || !b->kb || !raw || !*raw) return 0;
    const char *tq[1] = { "current_turn" };
    /* ⚠ tre domande separate e non una congiunzione: il riconoscimento del tipo e quello
     * della descrizione sono ricorsivi, e in un solo ramo di risoluzione esaurivano i 384
     * legami (la regola intera falliva con ogni pezzo vero). Le decisioni restano in
     * guided-reading.p0 (turn_guided_inquiry/1 ne e' la forma dichiarativa). */
    { const char *aq[1] = { "acquire" };
      const char *gq[3] = { "current_turn", NULL, NULL }; char r1[1][KB_TERM_LEN];
      if (!kb_query(b->kb, "acquisition_move", aq, 1)) return 0;
      if (kb_match(b->kb, "guided_asked_type", gq, 3, r1, 1) < 1) return 0;
      if (!kb_query(b->kb, "turn_guided_bridge", tq, 1)) return 0; }
    if (getenv("P0_READ_TRACE")) fprintf(stderr, "[guided] inquiry recognized\n");
    (void)norm;

    /* i tipi chiesti, nell'ordine del turno */
    char types[GR_MAX_TYPES][KB_TERM_LEN]; size_t ntypes = 0;
    long first_type_pos = -1;
    for (long idx = 0; idx < 64 && ntypes < GR_MAX_TYPES; idx++) {
        char is[16]; snprintf(is, sizeof is, "%ld", idx);
        const char *aq[3] = { "current_turn", is, NULL };
        char row[1][KB_TERM_LEN];
        if (kb_match(b->kb, "guided_asked_type", aq, 3, row, 1) == 1) {
            snprintf(types[ntypes++], KB_TERM_LEN, "%s", kb_dequote(row[0]));
            if (first_type_pos < 0) first_type_pos = idx;
        }
    }
    if (getenv("P0_READ_TRACE")) for (size_t i = 0; i < ntypes; i++) fprintf(stderr, "[guided] asked type %s\n", types[i]);
    if (!ntypes) return 0;
    /* le descrizioni di entita' intermedie sono sotto-domande, e si risolvono dalla piu'
     * interna: «the capital of [the country where [the highest mountain in Africa] stands]» */
    int type_desc[GR_MAX_TYPES + 4]; for (size_t i = 0; i < GR_MAX_TYPES + 4; i++) type_desc[i] = -1;
    char dmods[4][4][KB_TERM_LEN]; size_t ndmods[4] = {0}; size_t ndesc = 0;
    char dheads[4][KB_TERM_LEN]; long dpos[4];
    for (long idx = 63; idx >= 0 && ndesc < 3; idx--) {
        char is[16]; snprintf(is, sizeof is, "%ld", idx);
        const char *dq[3] = { "current_turn", is, NULL };
        char dh[1][KB_TERM_LEN];
        /* ⚠ la descrizione nuda, e il confronto con i tipi chiesti qui: la regola intera
         * (tipo chiesto + descrizione + due naf ricorsivi) esauriva i legami */
        if (kb_match(b->kb, "guided_description_at", dq, 3, dh, 1) != 1) continue;
        {
            const char *h = kb_dequote(dh[0]);
            int is_asked = 0;
            for (size_t i = 0; i < ntypes; i++) if (gr_same(b, h, types[i])) is_asked = 1;
            if (is_asked || idx <= first_type_pos) continue;
        }
        snprintf(dheads[ndesc], KB_TERM_LEN, "%s", kb_dequote(dh[0]));
        dpos[ndesc] = idx;
        char (*mods)[KB_TERM_LEN] = NULL; size_t nmods = 0;
        const char *mq[3] = { "current_turn", is, NULL };
        if (kb_match_all(b->kb, "guided_description_modifier", mq, 3, &mods, &nmods))
            for (size_t m = 0; m < nmods && ndmods[ndesc] < 4; m++)
                snprintf(dmods[ndesc][ndmods[ndesc]++], KB_TERM_LEN, "%s", kb_dequote(mods[m]));
        free(mods);
        ndesc++;
    }
    if (ndesc && ntypes + ndesc <= GR_MAX_TYPES) {
        memmove(types + ndesc, types, ntypes * sizeof types[0]);
        for (size_t d = 0; d < ndesc; d++) {           /* la piu' interna (posizione piu' alta) per prima */
            snprintf(types[d], KB_TERM_LEN, "%s", dheads[d]);
            type_desc[d] = (int)d;
        }
        ntypes += ndesc;
    }
    char desc_head[KB_TERM_LEN] = "";
    if (ndesc) snprintf(desc_head, sizeof desc_head, "%s", dheads[0]);
    char (*cues)[KB_TERM_LEN] = NULL; size_t ncue = 0;
    { const char *cq[2] = { "current_turn", NULL };
      if (!kb_match_all(b->kb, "guided_cue", cq, 2, &cues, &ncue)) ncue = 0; }
    char (*ev)[KB_TERM_LEN] = NULL; size_t nev = 0;
    { const char *eq[2] = { "current_turn", NULL };
      if (!kb_match_all(b->kb, "guided_evidence_word", eq, 2, &ev, &nev)) nev = 0; }

    static GrPage page, cand, bridge_page;
    char known_rel[KB_TERM_LEN] = "", known_from[KB_TERM_LEN] = "", known_to[KB_TERM_LEN] = "", searched_q[KB_TERM_LEN] = "";
    /* il punto di partenza: il primo nome del turno che ha una pagina */
    GrSentence ps; memset(&ps, 0, sizeof ps);
    { static GrPage tmp; memset(&tmp, 0, sizeof tmp); snprintf(tmp.prose, sizeof tmp.prose, "%s", raw);
      gr_split_sentences(b, &tmp);
      { int hf = !kb_query(b->kb, "guided_head_last", NULL, 0);
        for (size_t i = 0; i < tmp.nsent; i++) tmp.sent[i].head_first = hf; }
      int started = 0;
      /* la frase descrittiva, se c'e': un nome che sta dentro la descrizione di un'altra
       * entita' («the highest mountain in Africa») non e' il punto di partenza — lo e'
       * l'entita' descritta */
      char desc_phrase[KB_TERM_LEN] = "";
      { const char *sq[2] = { "current_turn", NULL }; char ph[1][KB_TERM_LEN];
        if (kb_match(b->kb, "guided_search_phrase", sq, 2, ph, 1) == 1)
            snprintf(desc_phrase, sizeof desc_phrase, "%s", kb_dequote(ph[0])); }
      for (size_t si = 0; si < tmp.nsent && !started; si++) {
          GrMention pm[GR_MAX_MENT]; size_t np = gr_mentions(b, &tmp.sent[si], pm, GR_MAX_MENT);
          for (size_t k = 0; k < np && !started; k++) {
              if (!gr_fetch_mention(b, &pm[k], &page)) continue;
              /* una pagina di partenza deve parlare del problema: «Start», «Begin» hanno
               * una pagina, ma non nominano nessun indizio del prompt */
              int hits = (pm[k].end - pm[k].start) > 1;   /* un nome di piu' parole che e' un titolo basta */
              for (size_t si2 = 0; si2 < page.nsent && !hits; si2++)
                  for (size_t c = 0; c < ncue && !hits; c++) {
                      const char *cw = kb_dequote(cues[c]);
                      int own = 0;
                      char tk[KB_TERM_LEN]; snprintf(tk, sizeof tk, "%s", pm[k].key);
                      for (char *tok = strtok(tk, "_"); tok && !own; tok = strtok(NULL, "_")) if (!strcmp(tok, cw)) own = 1;
                      if (own) continue;
                      for (size_t i = 0; i < page.sent[si2].ntok && !hits; i++)
                          if (gr_same(b, page.sent[si2].low[i], cw)) hits = 1;
                  }
              if (hits) started = 1;
          }
          /* nessun nome ha una pagina: la KB sa gia' un fatto che parte da uno di loro
           * e che il turno interroga («the capital of Hungary») */
          for (size_t k = 0; k < np && !started; k++) {
              /* il nome e l'indizio anche nella loro forma inglese (tr/2) */
              char ekeys[4][KB_TERM_LEN]; size_t nek = 0;
              { const char *eq[2] = { pm[k].key, NULL };
                nek = kb_match(b->kb, "guided_english_key", eq, 2, ekeys, 4); }
              for (size_t c = 0; c < ncue && !started; c++) {
                char ecues[4][KB_TERM_LEN]; size_t nec = 0;
                { const char *cq2[2] = { kb_dequote(cues[c]), NULL };
                  nec = kb_match(b->kb, "guided_english_key", cq2, 2, ecues, 4); }
                for (size_t ec = 0; ec < nec && !started; ec++)
                for (size_t ek = 0; ek < nek && !started; ek++) {
                  const char *fq[2] = { kb_dequote(ecues[ec]), NULL };
                  char preds[8][KB_TERM_LEN];
                  size_t npr = kb_match(b->kb, "answer_frame", fq, 2, preds, 8);
                  for (size_t x = 0; x < npr && !started; x++) {
                      char pr[KB_TERM_LEN]; snprintf(pr, sizeof pr, "%s", kb_dequote(preds[x]));
                      const char *vq[2] = { kb_dequote(ekeys[ek]), NULL };
                      char vals[1][KB_TERM_LEN];
                      if (kb_match(b->kb, pr, vq, 2, vals, 1) != 1) continue;
                      GrMention vm; memset(&vm, 0, sizeof vm);
                      snprintf(vm.key, sizeof vm.key, "%s", kb_dequote(vals[0]));
                      if (!gr_fetch_mention(b, &vm, &page)) continue;
                      started = 1;
                      snprintf(known_rel, sizeof known_rel, "%s", kb_dequote(cues[c]));
                      snprintf(known_from, sizeof known_from, "%s", pm[k].surface);
                      snprintf(known_to, sizeof known_to, "%s", page.title);
                  }
                }
              }
          }
      }
      /* ancora niente: si cerca la frase che segue il ponte, o la descrizione; se la frase
       * risolve una descrizione, il risultato dev'essere del suo tipo */
      if (!started) {
          const char *sq[2] = { "current_turn", NULL };
          char ph[1][KB_TERM_LEN];
          if (kb_match(b->kb, "guided_search_phrase", sq, 2, ph, 1) == 1) {
              char phrase[KB_TERM_LEN]; snprintf(phrase, sizeof phrase, "%s", kb_dequote(ph[0]));
              size_t pl = strlen(phrase);
              while (pl && strchr("?.!", phrase[pl - 1])) phrase[--pl] = '\0';
              const char *want = (ndesc && type_desc[0] >= 0) ? types[0] : NULL;
              if (gr_search_page(b, phrase, want, cues, ncue, &page)) {
                  started = 1;
                  snprintf(searched_q, sizeof searched_q, "%s", phrase);
              }
          }
      }
      if (getenv("P0_READ_TRACE")) fprintf(stderr, "[guided] start %s\n", started ? page.topic : "(none)");
      if (!started) { free(cues); free(ev); return 0; }
      /* le parole del nome da cui si parte non sono indizi: ogni frase della sua pagina
       * le contiene */
      for (size_t c = 0; c < ncue; ) {
          const char *cw = kb_dequote(cues[c]);
          int drop = 0;
          char tk[KB_TERM_LEN]; snprintf(tk, sizeof tk, "%s", page.topic);
          for (char *tok = strtok(tk, "_"); tok && !drop; tok = strtok(NULL, "_"))
              if (!strcmp(tok, cw)) drop = 1;
          if (drop) { memmove(cues + c, cues + c + 1, (ncue - c - 1) * sizeof *cues); ncue--; }
          else c++;
      }
    }

    kb_retract_pred(b->kb, "guided_turn"); kb_retract_pred(b->kb, "guided_read");
    kb_retract_pred(b->kb, "guided_found"); kb_retract_pred(b->kb, "guided_sentence");
    kb_retract_pred(b->kb, "guided_bridge"); kb_retract_pred(b->kb, "guided_missing");
    { char tc[1][KB_TERM_LEN]; const char *cq[1] = { NULL };
      if (kb_match(b->kb, "turn_counter", cq, 1, tc, 1) == 1) { const char *ga[1] = { tc[0] }; gr_assert(b, "guided_turn", ga, 1); } }

    kb_retract_pred(b->kb, "guided_known"); kb_retract_pred(b->kb, "guided_unmatched");
    kb_retract_pred(b->kb, "guided_rejected");
    kb_retract_pred(b->kb, "guided_searched"); kb_retract_pred(b->kb, "guided_found_known");
    kb_retract_pred(b->kb, "guided_found_key");
    if (searched_q[0]) {
        char qq[KB_TERM_LEN], qt3[KB_TERM_LEN];
        gr_quote(searched_q, qq, sizeof qq); gr_quote(page.title, qt3, sizeof qt3);
        const char *sa[2] = { qq, qt3 }; gr_assert(b, "guided_searched", sa, 2);
    }
    if (known_rel[0]) {
        char qf[KB_TERM_LEN], qt2[KB_TERM_LEN];
        gr_quote(known_from, qf, sizeof qf); gr_quote(known_to, qt2, sizeof qt2);
        const char *ka[3] = { known_rel, qf, qt2 }; gr_assert(b, "guided_known", ka, 3);
    }
    int cue_seen[64] = {0};
    char visited[GR_MAX_PAGES + 2][KB_TERM_LEN]; size_t nvis = 0;
    int blind_bridges = 0;
    size_t t = 0, pages = 0, found = 0;
    if (desc_head[0] && known_rel[0] && gr_same(b, known_rel, desc_head)) {
        /* «the capital of Hungary»: la descrizione l'ha gia' risolta la KB */
        char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
        char qv[KB_TERM_LEN], qfrom[KB_TERM_LEN];
        gr_quote(page.title, qv, sizeof qv); gr_quote(known_from, qfrom, sizeof qfrom);
        { const char *fa[4] = { fn, desc_head, qv, "1" }; gr_assert(b, "guided_found", fa, 4); }
        { const char *ka[3] = { fn, known_rel, qfrom }; gr_assert(b, "guided_found_known", ka, 3); }
        kb_retract_pred(b->kb, "guided_known");
        t = 1;
    }
    if (desc_head[0] && searched_q[0]) {
        /* la ricerca della descrizione ha gia' dato l'entita' descritta */
        char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
        char qv[KB_TERM_LEN]; gr_quote(page.title, qv, sizeof qv);
        { const char *fa[4] = { fn, desc_head, qv, "1" }; gr_assert(b, "guided_found", fa, 4); }
        t = 1;
    }
    while (t < ntypes && pages < GR_MAX_PAGES) {
        pages++;
        snprintf(visited[nvis++], KB_TERM_LEN, "%s", page.topic);
        /* la pagina letta: indirizzo e revisione, come ogni lettura della memoria profonda */
        char pn[16]; snprintf(pn, sizeof pn, "%zu", pages);
        char addr[KB_TERM_LEN], qt[200];
        gr_quote(page.title, qt, sizeof qt);
        snprintf(addr, sizeof addr, "wiki_address(%s, %s, %s, lead)", page.edition, qt,
                 page.revision[0] ? page.revision : "unknown");
        { const char *ra[3] = { pn, page.topic, addr }; gr_assert(b, "guided_read", ra, 3); }
        { const char *ta[2] = { page.topic, addr };
          int prev = kb_origin(b->kb); kb_set_origin(b->kb, KB_SESSION);
          if (!kb_query(b->kb, "topic_read", ta, 2)) kb_assert(b->kb, "topic_read", ta, 2);
          kb_set_origin(b->kb, prev); }

        for (size_t si = 0; si < page.nsent; si++)
            for (size_t c = 0; c < ncue && c < 64; c++)
                for (size_t i = 0; i < page.sent[si].ntok && !cue_seen[c]; i++)
                    if (gr_same(b, page.sent[si].low[i], kb_dequote(cues[c]))) cue_seen[c] = 1;
        /* il tipo chiesto, in questa pagina: la miglior candidatura su tutte le frasi */
        int progressed = 1;
        while (progressed && t < ntypes) {
            progressed = 0;
            const char *type = types[t];
            /* la KB sa gia' il tipo chiesto del nodo corrente? (answer_frame: la
             * superficie del tipo interroga una relazione) */
            {
                char preds[8][KB_TERM_LEN];
                size_t npr = 0;
                {
                    char etypes[4][KB_TERM_LEN];
                    const char *eq[2] = { type, NULL };
                    size_t net = kb_match(b->kb, "guided_english_key", eq, 2, etypes, 4);
                    for (size_t e = 0; e < net && npr == 0; e++) {
                        const char *fq[2] = { kb_dequote(etypes[e]), NULL };
                        npr = kb_match(b->kb, "answer_frame", fq, 2, preds, 8);
                    }
                }
                int known = 0;
                for (size_t x = 0; x < npr && !known; x++) {
                    char pr[KB_TERM_LEN]; snprintf(pr, sizeof pr, "%s", kb_dequote(preds[x]));
                    const char *vq[2] = { page.topic, NULL };
                    char vals[1][KB_TERM_LEN];
                    if (kb_match(b->kb, pr, vq, 2, vals, 1) != 1) continue;
                    char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
                    char val[KB_TERM_LEN], qv[KB_TERM_LEN], qfrom[KB_TERM_LEN];
                    present_atom(b, kb_dequote(vals[0]), val, sizeof val);
                    if (val[0]) val[0] = (char)toupper((unsigned char)val[0]);
                    gr_quote(val[0] ? val : kb_dequote(vals[0]), qv, sizeof qv);
                    gr_quote(page.title, qfrom, sizeof qfrom);
                    { const char *fa[4] = { fn, type, qv, pn }; gr_assert(b, "guided_found", fa, 4); }
                    { const char *ka[3] = { fn, pr, qfrom }; gr_assert(b, "guided_found_known", ka, 3); }
                    known = 1;
                }
                if (known) { t++; progressed = 1; continue; }
            }
            /* il nodo raggiunto e' gia' del tipo chiesto? («the capital of Hungary»
             * e' una citta') — vale per la pagina di partenza, dove non c'e' un ponte
             * che l'abbia scelto per un altro ruolo */
            if ((pages == 1 && !found) || (t > 0 && type_desc[t - 1] >= 0 && found)) {
                GrMention self; memset(&self, 0, sizeof self);
                snprintf(self.key, sizeof self.key, "%s", page.topic);
                const char *hs = strrchr(page.topic, '_');
                snprintf(self.head, sizeof self.head, "%s", hs ? hs + 1 : page.topic);
                if (gr_kb2(b, "known_entity_type", self.key, type) ||
                    (page.nsent && gr_type_ok(b, &self, type, &cand))) {
                    char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
                    char qv[KB_TERM_LEN], qs[700];
                    gr_quote(page.title, qv, sizeof qv);
                    gr_excerpt(&page.sent[0], 0, 0, qs + 1, sizeof qs - 2);
                    { char ex[600]; gr_excerpt(&page.sent[0], 0, 0, ex, sizeof ex); gr_quote(ex, qs, sizeof qs); }
                    { const char *fa[4] = { fn, type, qv, pn }; gr_assert(b, "guided_found", fa, 4); }
                    { const char *sa[2] = { fn, qs }; gr_assert(b, "guided_sentence", sa, 2); }
                    t++; progressed = 1; continue;
                }
            }
            int generic = gr_kb1(b, "generic_type", type);
            int best = -1000; char best_val[KB_TERM_LEN] = "", best_key[KB_TERM_LEN] = "", best_ex[600] = "";
            GrMention rej[8]; char rej_ex[8][300]; int rej_sc[8]; size_t nrej = 0;
            GrMention best_m; memset(&best_m, 0, sizeof best_m);
            const GrSentence *best_s = NULL;
            char shape[1][KB_TERM_LEN]; shape[0][0] = '\0';
            { const char *sq[2] = { type, NULL }; kb_match(b->kb, "answer_shape", sq, 2, shape, 1); }
            const char *sh = shape[0][0] ? kb_dequote(shape[0]) : "";
            for (size_t si = 0; si < page.nsent && !generic; si++) {
                const GrSentence *s = &page.sent[si];
                int cue = gr_cue_hits(b, s, cues, ncue);
                GrMention mm[GR_MAX_MENT]; size_t nm = gr_mentions(b, s, mm, GR_MAX_MENT);
                /* le forme della risposta dichiarate per il tipo */
                if (!strcmp(sh, "class_modifier") && si == 0) {
                    for (size_t i = 0; i + 1 < s->ntok; i++)
                        if (gr_kb1(b, "definition_copula", s->low[i]) && i + 2 < s->ntok &&
                            gr_kb1(b, "english_determiner", s->low[i + 1]) && isupper((unsigned char)s->tok[i + 2][0])) {
                            int sc = 5 + cue;
                            if (sc > best) { best = sc; snprintf(best_val, sizeof best_val, "%s", s->tok[i + 2]); best_key[0] = '\0'; best_s = s; gr_excerpt(s, i + 2, i + 3, best_ex, sizeof best_ex); }
                            break;
                        }
                }
                if (!strcmp(sh, "headed_phrase")) {
                    for (size_t i = 1; i < s->ntok; i++) {
                        if (!gr_same(b, s->low[i], type) || !cue) continue;
                        size_t k = i;
                        while (k > 0 && i - k < 2 && !is_stopword(b, (char *)s->low[k - 1]) &&
                               !isupper((unsigned char)s->tok[k - 1][0])) k--;
                        char val[KB_TERM_LEN]; size_t o = 0; val[0] = '\0';
                        for (size_t x = k; x <= i; x++) o += (size_t)snprintf(val + o, sizeof val - o, "%s%s", x > k ? " " : "", s->low[x]);
                        int sc = 4 + cue;
                        if (sc > best) { best = sc; snprintf(best_val, sizeof best_val, "%s", val); best_key[0] = '\0'; best_s = s; gr_excerpt(s, k, i + 1, best_ex, sizeof best_ex); }
                    }
                }
                for (size_t k = 0; k < nm; k++) {
                    GrMention *g = &mm[k];
                    /* «the Ancient Iranian religion»: il nome proprio seguito dal tipo
                     * chiesto e' un nome solo, con il tipo per testa */
                    if (g->end < s->ntok && !s->stop[g->end - 1] && !isupper((unsigned char)s->tok[g->end][0]) &&
                        gr_same(b, s->low[g->end], type)) {
                        GrMention ext = *g;
                        size_t o2 = strlen(ext.surface);
                        snprintf(ext.surface + o2, sizeof ext.surface - o2, " %s", s->tok[g->end]);
                        snprintf(ext.head, sizeof ext.head, "%s", s->low[g->end]);
                        gr_key_of(ext.surface, ext.key, sizeof ext.key);
                        ext.end++;
                        ext.modifier = 0;      /* col suo tipo e' un nome, non piu' un modificatore */
                        /* e' un nome solo se e' il nome di qualcosa: «Ancient Iranian religion»
                         * ha una pagina, «Russian philosopher» e' una classe */
                        if (gr_fetch_mention(b, &ext, &cand)) *g = ext;
                    }
                    int skip = g->modifier;
                    for (size_t v = 0; v < nvis; v++) if (!strcmp(visited[v], g->key)) skip = 1;
                    if (!strcmp(g->key, page.topic)) skip = 1;
                    /* il candidato presentato con un nome di ruolo che il prompt usa per un
                     * referente gia' dato («this author») e' quel referente */
                    if (!skip && g->start > 0) {
                        const char *aq[2] = { "current_turn", s->low[g->start - 1] };
                        if (kb_query(b->kb, "guided_anaphor_noun", aq, 2)) skip = 1;
                    }
                    if (skip) continue;
                    /* «a deity known as Ahura Mazda»: la frase nomina il tipo */
                    int named = 0;
                    for (size_t i = g->start >= 3 ? g->start - 3 : 0; i < g->start; i++)
                        if (gr_same(b, s->low[i], type)) {
                            for (size_t j = i + 1; j < g->start; j++)
                                if (gr_kb1(b, "naming_link", s->low[j])) named = 1;
                        }
                    if (type_desc[t] >= 0 && ndmods[type_desc[t]]) {
                        /* «the HIGHEST mountain»: il modificatore della descrizione deve stare
                         * nella frase del candidato */
                        int has_mod = 0;
                        for (size_t m = 0; m < ndmods[type_desc[t]] && !has_mod; m++)
                            for (size_t i = 0; i < s->ntok && !has_mod; i++)
                                if (gr_same(b, s->low[i], dmods[type_desc[t]][m])) has_mod = 1;
                        if (!has_mod) continue;
                    }
                    int ok = named || gr_type_ok(b, g, type, &cand);
                    if (!ok) continue;
                    int sc = 3 + cue + gr_local_evidence(b, s, g, ev, nev) + (named ? 2 : 0);
                    if (nrej < 8) {
                        snprintf(rej[nrej].surface, KB_TERM_LEN, "%s", g->surface);
                        gr_excerpt(s, g->start, g->end, rej_ex[nrej], sizeof rej_ex[nrej]);
                        rej_sc[nrej++] = sc;
                    }
                    if (sc > best) { best = sc; snprintf(best_val, sizeof best_val, "%s", g->surface); snprintf(best_key, sizeof best_key, "%s", g->key); best_s = s; best_m = *g; gr_excerpt(s, g->start, g->end, best_ex, sizeof best_ex); }
                }
            }
            if (best_s) {
                char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
                char qv[KB_TERM_LEN], qs[700];
                gr_quote(best_val, qv, sizeof qv); gr_quote(best_ex, qs, sizeof qs);
                { const char *fa[4] = { fn, type, qv, pn }; gr_assert(b, "guided_found", fa, 4); }
                { const char *sa[2] = { fn, qs }; gr_assert(b, "guided_sentence", sa, 2); }
                { char vk[KB_TERM_LEN]; gr_key_of(best_val, vk, sizeof vk);
                  const char *ka2[2] = { fn, vk }; gr_assert(b, "guided_found_key", ka2, 2); }
                /* gli altri candidati dello stesso tipo, con la frase che li nomina: la
                 * biforcazione si dice, non si nasconde */
                for (size_t r = 0; r < nrej; r++) {
                    if (!strcmp(rej[r].surface, best_val)) continue;
                    int dup = 0;
                    for (size_t r2 = 0; r2 < r; r2++) if (!strcmp(rej[r2].surface, rej[r].surface)) dup = 1;
                    if (dup) continue;
                    char qr[KB_TERM_LEN], qe[400], diff[16];
                    gr_quote(rej[r].surface, qr, sizeof qr); gr_quote(rej_ex[r], qe, sizeof qe);
                    snprintf(diff, sizeof diff, "%d", best - rej_sc[r]);
                    const char *ra2[4] = { fn, qr, qe, diff }; gr_assert(b, "guided_rejected", ra2, 4);
                }
                t++;
                if (best_key[0] && t < ntypes) {
                    /* la risposta e' il nodo da cui riparte il frame successivo */
                    if (gr_fetch_mention(b, &best_m, &cand)) { page = cand; progressed = 0; goto next_page; }
                }
                progressed = 1;
            }
        }
        if (t >= ntypes) break;
        if (getenv("P0_READ_TRACE")) fprintf(stderr, "[guided] page %s: %zu sentences, type %s not found\n", page.topic, page.nsent, types[t]);
        if (type_desc[t] >= 0) {
            /* una descrizione che la pagina d'appoggio non risolve si cerca, non si insegue:
             * «the highest mountain in Europe» */
            char dps[16]; snprintf(dps, sizeof dps, "%ld", dpos[type_desc[t]]);
            { const char *aq2[2] = { "current_turn", dps };
              if (kb_query(b->kb, "guided_description_anaphoric", aq2, 2)) break; }
            const char *pq[3] = { "current_turn", dps, NULL };
            char phs[1][KB_TERM_LEN];
            size_t nph = kb_match(b->kb, "guided_description_words_at", pq, 3, phs, 1);
            int resolved = 0;
            for (size_t x = 0; x < nph && !resolved; x++) {
                char phrase[KB_TERM_LEN]; snprintf(phrase, sizeof phrase, "%s", kb_dequote(phs[x]));
                size_t pl = strlen(phrase); while (pl && strchr("?.!", phrase[pl - 1])) phrase[--pl] = '\0';
                if (gr_search_page(b, phrase, types[t], cues, ncue, &cand)) {
                    char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
                    char qv[KB_TERM_LEN], qq[KB_TERM_LEN], qt4[KB_TERM_LEN];
                    gr_quote(cand.title, qv, sizeof qv);
                    { const char *fa[4] = { fn, types[t], qv, pn }; gr_assert(b, "guided_found", fa, 4); }
                    gr_quote(phrase, qq, sizeof qq); gr_quote(cand.title, qt4, sizeof qt4);
                    { const char *sa2[2] = { qq, qt4 }; gr_assert(b, "guided_searched", sa2, 2); }
                    page = cand; t++; resolved = 1;
                }
            }
            if (resolved) continue;
            break;
        }
        /* nessun candidato del tipo: la frase piu' pertinente sceglie il ponte */
        {
            int bestc = 0, have = 0; char nextk[KB_TERM_LEN] = "", nexts[KB_TERM_LEN] = "", bs[600] = "";
            for (size_t si = 0; si < page.nsent; si++) {
                const GrSentence *s = &page.sent[si];
                int cue = gr_cue_hits(b, s, cues, ncue);
                GrMention mm[GR_MAX_MENT]; size_t nm = gr_mentions(b, s, mm, GR_MAX_MENT);
                for (size_t k = 0; k < nm; k++) {
                    int skip = !strcmp(mm[k].key, page.topic) || mm[k].modifier;
                    for (size_t v = 0; v < nvis; v++) if (!strcmp(visited[v], mm[k].key)) skip = 1;
                    /* gli alias della pagina stanno prima della copula della prima frase */
                    if (si == 0) {
                        size_t cop = s->ntok;
                        for (size_t i = 0; i < s->ntok; i++) if (gr_kb1(b, "definition_copula", s->low[i])) { cop = i; break; }
                        if (mm[k].start < cop) skip = 1;
                    }
                    if (skip) continue;
                    int sc = cue * 10 + gr_local_evidence(b, s, &mm[k], ev, nev) - (int)k;
                    if (getenv("P0_READ_TRACE")) fprintf(stderr, "[guided] bridge candidate «%s» key=%s sc=%d\n", mm[k].surface, mm[k].key, sc);
                    if (have && sc + 20 <= bestc) continue;
                    if (!gr_fetch_mention(b, &mm[k], &cand)) continue;
                    if (t < ntypes && gr_lookahead(b, &cand, types[t])) sc += 20;
                    if (have && sc <= bestc) continue;
                    have = 1; bestc = sc; snprintf(nextk, sizeof nextk, "%s", cand.topic); bridge_page = cand;
                    snprintf(nexts, sizeof nexts, "%s", mm[k].surface);
                    gr_excerpt(s, mm[k].start, mm[k].end, bs, sizeof bs);
                }
            }
            if (!nextk[0]) break;
            /* due ponti di fila senza nessun indizio: non si sta cercando, si vaga */
            if (bestc <= 0) { if (++blind_bridges >= 2) break; } else blind_bridges = 0;
            char qn[KB_TERM_LEN], qs[700];
            gr_quote(nexts, qn, sizeof qn); gr_quote(bs, qs, sizeof qs);
            if (t < ntypes && gr_kb1(b, "generic_type", types[t])) {
                /* «the historical figure he is named after»: il tipo generico lo
                 * soddisfa l'entita' che la frase pertinente nomina */
                char fn[16]; snprintf(fn, sizeof fn, "%zu", ++found);
                { const char *fa[4] = { fn, types[t], qn, pn }; gr_assert(b, "guided_found", fa, 4); }
                { const char *sa[2] = { fn, qs }; gr_assert(b, "guided_sentence", sa, 2); }
                t++;
            } else {
                const char *ba[3] = { pn, qn, qs }; gr_assert(b, "guided_bridge", ba, 3);
            }
            page = bridge_page;
        }
        continue;
    next_page:;
    }
    for (size_t k = t; k < ntypes; k++) { const char *ma[1] = { types[k] }; gr_assert(b, "guided_missing", ma, 1); }
    /* i tipi delle descrizioni sono passi, non la risposta chiesta */
    kb_retract_pred(b->kb, "guided_intermediate_type");
    for (size_t d = 0; d < ndesc; d++) { const char *ia[1] = { dheads[d] }; gr_assert(b, "guided_intermediate_type", ia, 1); }
    if (t < ntypes)
        for (size_t c = 0; c < ncue && c < 64; c++)
            if (!cue_seen[c]) { const char *ua[1] = { kb_dequote(cues[c]) }; gr_assert(b, "guided_unmatched", ua, 1); }
    free(cues); free(ev);

    /* la lettura dice da dove viene l'ultimo anello, per «where did you read that?» */
    { char tc[1][KB_TERM_LEN]; const char *cq[1] = { NULL };
      if (kb_match(b->kb, "turn_counter", cq, 1, tc, 1) == 1) {
          const char *ra[2] = { tc[0], page.topic };
          int prev = kb_origin(b->kb); kb_set_origin(b->kb, KB_SESSION);
          kb_assert(b->kb, "read_topic_asked", ra, 2);
          kb_set_origin(b->kb, prev);
      } }

    /* la risposta: le parti che la KB compone (guided_part/2), in ordine */
    char (*orders)[KB_TERM_LEN] = NULL; size_t no = 0;
    const char *oq[2] = { NULL, NULL };
    if (!kb_match_all(b->kb, "guided_part", oq, 2, &orders, &no) || !no) { free(orders); return 0; }
    long ov[64]; size_t nov = 0;
    for (size_t i = 0; i < no && nov < 64; i++) {
        long v = strtol(kb_dequote(orders[i]), NULL, 10); int seen = 0;
        for (size_t k = 0; k < nov; k++) if (ov[k] == v) seen = 1;
        if (!seen) ov[nov++] = v;
    }
    free(orders);
    for (size_t i = 1; i < nov; i++) { long v = ov[i]; size_t k = i; while (k && ov[k - 1] > v) { ov[k] = ov[k - 1]; k--; } ov[k] = v; }
    size_t off = 0; out[0] = '\0';
    for (size_t i = 0; i < nov && off + 1 < out_size; i++) {
        char num[32]; snprintf(num, sizeof num, "%ld", ov[i]);
        const char *pq[2] = { num, NULL };
        char (*texts)[KB_TERM_LEN] = NULL; size_t nt = 0;
        if (!kb_match_all(b->kb, "guided_part", pq, 2, &texts, &nt)) nt = 0;
        for (size_t x = 0; x < nt && off + 1 < out_size; x++)
            off += (size_t)snprintf(out + off, out_size - off, "%s%s", off ? "\n" : "", kb_dequote(texts[x]));
        free(texts);
    }
    return out[0] != '\0';
}

static int gr_cue_hits_fwd(Brain *b, const GrSentence *s, char (*cues)[KB_TERM_LEN], size_t ncue) {
    return gr_cue_hits(b, s, cues, ncue);
}
