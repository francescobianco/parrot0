/* gen189 (basic-chat cat.0): classify NON-LINGUISTIC input and answer at the
 * CHANNEL level instead of the topic-level "I don't understand that yet."
 *
 * The distinction is structural, taken before the reasoning core: "is this
 * language at all?" — not "do I know this topic?". Three shapes of noise:
 *   (1) punctuation/symbols only ("?", "!", "!@#$%^&*()")
 *   (2) a bare number with no operation ("1234567890")
 *   (3) keyboard-mash letters ("asdfghjkl", "aaaaaaaaaa")
 * This is NOT a phrasebook of the example strings: it keys purely on character
 * structure, so it generalizes to any such input. And because non-linguistic
 * input carries no language, the same path serves every language — the
 * bilingual ratchet holds by construction. It only ever REDIRECTS honestly; it
 * never feigns understanding (PRINCIPLES.md: no impostor printf of "knowledge").
 *
 * Placed right after mod_repair so a noise turn is recognized before any content
 * module can mis-claim it (mod_code used to read "!@#$%^&*()" as a code snippet;
 * mod_social used to greet "asdfghjkl"). It declines (returns 0) for anything
 * that looks like language, so normal turns reach the rest of the registry. */
static int mod_input(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    size_t len = strlen(norm);
    if (len == 0) return 0;          /* empty turns are dropped by the I/O shell */

    size_t nalpha = 0, ndigit = 0, npunct = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)norm[i];
        if (isalpha(c)) nalpha++;
        else if (isdigit(c)) ndigit++;
        else if (!isspace(c)) npunct++;
    }

    /* (1) Punctuation/symbols only — no letters, no digits, at least one mark.
     * Defer dot/dash/slash sequences: those are a *structured* code (Morse),
     * owned by mod_symbolic, not formless noise. The boundary is honest — this
     * module claims punctuation only when it is not a recognized symbolic form. */
    if (nalpha == 0 && ndigit == 0 && npunct > 0) {
        int morse_only = 1;
        for (size_t i = 0; i < len; i++) {
            char c = norm[i];
            if (c != '.' && c != '-' && c != '/' && !isspace((unsigned char)c)) {
                morse_only = 0; break;
            }
        }
        if (morse_only) return 0;    /* let the symbolic recognizer name it */
        put("That's just punctuation, not words — what would you like to ask?",
            out, out_size);
        return 1;
    }

    /* The remaining shapes are single tokens; multi-word input is linguistic. */
    char buf[256];
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw != 1) return 0;
    char *tok = strip_edge_punct(w[0]);
    size_t tlen = strlen(tok);
    if (tlen == 0) return 0;

    /* (2) A bare number (>=4 digits, no operator). mod_arith owns anything with
     * an operation; a lone long digit-run has nothing to compute. The >=4 gate
     * leaves short numbers (a game pick, "7", "100") for future modules. */
    int all_digit = 1;
    for (size_t i = 0; i < tlen; i++)
        if (!isdigit((unsigned char)tok[i])) { all_digit = 0; break; }
    if (all_digit && tlen >= 4) {
        char msg[160];
        snprintf(msg, sizeof msg,
                 "That's just the number %s with nothing to do — what would "
                 "you like me to do with it?", tok);
        put(msg, out, out_size);
        return 1;
    }

    /* (3) Keyboard-mash letters: a single all-alphabetic token with the shape of
     * noise — no vowel at all, one character repeated, a run of >=4 identical
     * letters, or an implausibly long consonant run (>=6) — that the KB does not
     * recognize. Thresholds are conservative so real words never trip it
     * ("strengths" peaks at 5 consonants; "rhythm" is too short here). */
    if (tlen >= 6) {
        int all_alpha = 1;
        for (size_t i = 0; i < tlen; i++)
            if (!isalpha((unsigned char)tok[i])) { all_alpha = 0; break; }
        if (all_alpha) {
            size_t vowels = 0, distinct = 0;
            size_t run = 1, max_run = 1, cons = 0, max_cons = 0;
            int seen[26] = {0};
            for (size_t i = 0; i < tlen; i++) {
                char c = tok[i];
                /* 'y' counts as a vowel here so consonant-only words that lean
                 * on it ("rhythm", "syzygy") are not mistaken for noise. */
                int v = (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||c=='y');
                if (v) vowels++;
                if (c >= 'a' && c <= 'z' && !seen[c-'a']) { seen[c-'a']=1; distinct++; }
                if (i > 0 && c == tok[i-1]) { if (++run > max_run) max_run = run; }
                else run = 1;
                if (!v) { if (++cons > max_cons) max_cons = cons; } else cons = 0;
            }
            int known = 0;
            if (b && b->kb) {
                char desc[256];
                known = kb_knows_pred(b->kb, tok) ||
                        kb_describe_entity(b->kb, tok, desc, sizeof desc);
            }
            int noise = (vowels == 0) || (distinct == 1) ||
                        (max_run >= 4) || (max_cons >= 6);
            if (noise && !known && !is_stopword(b, tok)) {
                put("That doesn't look like words to me — did a key get stuck? "
                    "I'm here when you'd like to ask something.",
                    out, out_size);
                return 1;
            }
        }
    }
    return 0;
}

/* The registry: an ordered list of cooperating parts. To add or remove a
 * behaviour, touch only this table — not brain_respond()'s control flow.
 * (This table is also reified into the KB as module(...) facts at birth, so
 * the agent's self-description cannot drift from its real structure.) */
static const Module registry[] = {
    {"piact",     mod_piact},
    {"compose",   mod_compose},
    {"repair",    mod_repair},
    {"input",     mod_input},
    {"world",     mod_world},
    {"translate", mod_translate},
    {"synth",     mod_synth},
    {"induce",    mod_induce},
    {"verify",    mod_verify},
    {"memory",    mod_memory},
    {"loop",      mod_loop},
    {"meta",      mod_meta},
    {"strategy",  mod_strategy},
    {"counterfactual", mod_counterfactual},
    {"whatifnot", mod_whatifnot},
    {"robust",    mod_robust},
    {"calibrate", mod_calibrate},
    {"abduce",    mod_abduce},
    {"role",      mod_role},
    {"self",      mod_self},
    {"fewshot",   mod_fewshot},
    {"compare",   mod_compare},
    {"algebra",   mod_algebra},
    {"arith",     mod_arith},
    {"plan",      mod_plan},
    {"wordproblem", mod_wordproblem},
    {"agent",     mod_agent},
    {"search",    mod_search},
    {"tool",      mod_tool},
    {"quantity",  mod_quantity},
    {"cause",     mod_cause},
    {"same",      mod_same},
    {"analogy",   mod_analogy},
    {"conj",      mod_conj},
    {"gen",       mod_gen},
    {"coref",     mod_coref},
    {"bench",     mod_bench},
    {"reader",    mod_reader},
    {"shell",     mod_shell},
    {"knowledge", mod_knowledge},
    {"codeast",   mod_codeast},
    {"code",      mod_code},
    {"symbolic",  mod_symbolic},
    {"summary",   mod_summary},
    {"discourse", mod_discourse},
    {"pragma",    mod_pragma},
    {"social",    mod_social},
    {"chitchat",  mod_chitchat},
    {"research",  mod_research},
};
static const size_t registry_len = sizeof registry / sizeof registry[0];

/* gen128: true if `name` is a real module in the registry. */
static int is_registry_module(const char *name) {
    for (size_t i = 0; i < registry_len; i++)
        if (strcmp(registry[i].name, name) == 0) return 1;
    return 0;
}

/* gen208: dispatch one planner clause through the registry (defn for the prototype
 * above compose_plan). Canonicalizes the clause exactly like brain_respond, then walks
 * the registry first-match-wins, SKIPPING compose so the planner cannot re-enter
 * itself, and also skipping repair (a sub-goal must not open a clarification). Returns
 * 1 with the claiming module's reply in `out`, else 0. Bounded: one pass, no recursion
 * into compose_plan. */
static int dispatch_one(Brain *b, const char *clause, char *out, size_t out_size) {
    if (!b || !clause || !*clause || out_size == 0) return 0;
    char norm[256];
    normalize(clause, norm, sizeof norm);
    char canon[256];
    canonicalize_lang(norm, canon, sizeof canon);
    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, "compose") == 0) continue; /* no re-entry */
        if (strcmp(registry[i].name, "repair") == 0) continue;  /* no clarification */
        if (registry[i].handle(b, canon, clause, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    return 0;
}

/* gen128: re-run the first-match-wins dispatch over a stored turn with module
 * `suppress` skipped. Writes the claiming module's name into `who` and its reply
 * into `out`; returns 1 if some module claimed it, 0 if it fell through to the
 * honest fallback (out then holds that fallback line). This is parrot0
 * simulating its own counterfactual self, so we protect the live conversation
 * state: the volatile last_reply/last_module are snapshotted and restored, and
 * the trace is left untouched (the candidate handlers write only to `out`; any
 * KB assertion they repeat for this same turn is idempotent). */
/* gen141: dispatch a (possibly reconstructed) turn through the registry, skipping
 * the repair module itself so a resumed turn cannot re-open a clarification. Unlike
 * replay_dispatch this is NOT footprint-free: a resumed assertion really learns and
 * a resumed query really runs — resuming the original intent is the whole point. */
static int repair_dispatch(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size) {
    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, "repair") == 0) continue;
        if (registry[i].handle(b, canon, raw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    not_understood(b, canon, out, out_size);
    return 0;
}

/* gen142 (E3): peel a leading DISCOURSE-MARKER opener off a turn and re-dispatch
 * the residue through the WHOLE registry, so a content task survives the social
 * wrapper ("anyway, is socrates a man" -> "Yes."; "by the way what is 2 plus 2"
 * -> "4."). This runs as a pre-dispatch normalization in brain_respond — BEFORE
 * the registry — because a content module earlier than pragma would otherwise
 * mis-parse the wrapped turn (e.g. the extra opener tokens shift memory's
 * "what is my X" window onto "plus"). A channel-management opener is not content;
 * normalizing it away is the same gesture as canonicalize_lang, one level up.
 *
 * The peel is conservative: it fires only when (a) a real discourse opener leads,
 * (b) there is residue after it, and (c) some module ACTUALLY claims the residue.
 * If the residue is unclaimed we return 0 and the original turn is dispatched
 * normally (so e.g. "anyway, i guess maybe" still reaches mod_pragma's hesitation
 * move on its own shape). The RAW residue is rebuilt by skipping the same leading
 * word-count from the original input, preserving proper-name casing ("well,
 * remember my dog is Rex" keeps "Rex"). */
static int pragma_peel(Brain *b, const char *canon, const char *raw,
                       char *out, size_t out_size) {
    char buf[256];
    size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw == 0) return 0;

    size_t skip = 0;
    if (!is_discourse_opener(w, nw, &skip) || skip >= nw) return 0;

    char residue[256]; size_t off = 0; residue[0] = '\0';
    for (size_t i = skip; i < nw && off + 1 < sizeof residue; i++)
        off += (size_t)snprintf(residue + off, sizeof residue - off,
                                "%s%s", i > skip ? " " : "", w[i]);
    if (!residue[0]) return 0;

    /* matching RAW residue: skip the same leading word count (and a trailing
     * comma the opener often carries), keep original casing. */
    char raw_res[256]; { const char *p = raw ? raw : ""; size_t s = 0;
        while (*p && s < skip) {
            while (*p && isspace((unsigned char)*p)) p++;
            while (*p && !isspace((unsigned char)*p)) p++;
            s++;
        }
        while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
        snprintf(raw_res, sizeof raw_res, "%s", p);
    }

    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, residue, raw_res, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    return 0;
}

/* gen218 (docs/plans/the-linguistic-glue.md, G2 — correction pull): integrate an
 * explicit CORRECTION. A turn opening with the marker "no" followed by a negative
 * claim ("no, socrates is not a man") is the user overriding a belief stated
 * earlier. Peel the marker, flag the turn as a correction, and re-dispatch the
 * residue so the existing negation parser stores not y(x); the flag makes that
 * store OVERRIDE any standing positive (even a curated/base fact), so a later
 * query re-derives cleanly to "No." instead of stalling on "Conflicted." — the
 * essay's "integrate a later correction" made into a deterministic state change.
 * Gated on BOTH the explicit marker AND a real negation marker in the residue, so
 * it never fires on a bare "no" answer or "no thanks". Mirror of pragma_peel. */
static int is_negation_marker(const char *w);

static int correction_peel(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size) {
    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    /* split_words keeps a trailing comma on the token ("no,"), so match on the
     * punctuation-stripped marker. */
    if (nw < 4 || strcmp(strip_edge_punct(w[0]), "no") != 0) return 0;
    int has_neg = 0;
    for (size_t i = 1; i < nw; i++)
        if (is_negation_marker(w[i])) { has_neg = 1; break; }
    if (!has_neg) return 0;                             /* correct a negation only */

    char residue[256]; size_t off = 0; residue[0] = '\0';
    for (size_t i = 1; i < nw && off + 1 < sizeof residue; i++)
        off += (size_t)snprintf(residue + off, sizeof residue - off,
                                "%s%s", i > 1 ? " " : "", w[i]);
    if (!residue[0]) return 0;

    /* matching RAW residue: drop the leading "no" (and a trailing comma it
     * usually carries), keeping original casing for the reader. */
    char raw_res[256]; { const char *p = raw ? raw : "";
        while (*p && isspace((unsigned char)*p)) p++;
        while (*p && !isspace((unsigned char)*p)) p++;  /* the "no"/"no," token */
        while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
        snprintf(raw_res, sizeof raw_res, "%s", p);
    }

    int saved = b->correcting; b->correcting = 1;
    int claimed = 0;
    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, residue, raw_res, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            claimed = 1; break;
        }
    }
    b->correcting = saved;
    return claimed;
}

static int replay_dispatch(Brain *b, const char *canon, const char *raw,
                           const char *suppress, char *who, size_t who_size,
                           char *out, size_t out_size) {
    char saved_reply[256], saved_module[32];
    unsigned long saved_fallbacks = b->fallbacks;
    snprintf(saved_reply, sizeof saved_reply, "%s", b->last_reply);
    snprintf(saved_module, sizeof saved_module, "%s", b->last_module);

    int claimed = 0;
    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, suppress) == 0) continue; /* the removed part */
        if (strcmp(registry[i].name, "counterfactual") == 0) continue; /* no self-recursion */
        if (registry[i].handle(b, canon, raw, out, out_size)) {
            snprintf(who, who_size, "%s", registry[i].name);
            claimed = 1;
            break;
        }
    }
    if (!claimed) {
        snprintf(who, who_size, "fallback");
        not_understood(b, canon, out, out_size);
    }

    /* restore the live state so the counterfactual probe leaves no footprint */
    snprintf(b->last_reply, sizeof b->last_reply, "%s", saved_reply);
    snprintf(b->last_module, sizeof b->last_module, "%s", saved_module);
    b->fallbacks = saved_fallbacks;
    return claimed;
}

/* ----------------------------------------------------------------------------
 * brain lifecycle + dispatch
 * ------------------------------------------------------------------------- */

Brain *brain_create(void) {
    Brain *b = calloc(1, sizeof *b);
    if (!b) return NULL;
    b->kb = kb_create();
    if (!b->kb) { free(b); return NULL; }
    b->start_time = time(NULL);
    b->active_world = -1; /* gen142 (E7): no local world is open at birth */

    /* Curated lexical knowledge used by the kernel itself. It lives in the
     * knowledge layer, not as C word arrays; loading it as base keeps it out of
     * session saves while tests stay independent of world knowledge files. */
    const char *lexicon = getenv("PARROT0_LEXICON");
    if (!lexicon) lexicon = "kb/core/lexicon.p0";
    if (*lexicon) {
        kb_set_origin(b->kb, KB_BASE);
        kb_load(b->kb, lexicon);
    }

    /* gen73 (PLAN.md Phase 3): social markers, question words and reaction words
     * live in kb/core/social.p0, not as hardcoded C arrays. The KB is the
     * single source of truth; the C code queries it at runtime. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/social.p0");

    /* gen101 (C15): role/character world-knowledge — what parrot0 knows about
     * the kinds and figures it may be asked to impersonate (see mod_role). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/roles.p0");

    /* gen126 (L5): bilingual content lexicon used by mod_translate to COMPOSE a
     * clause translation from word glosses + a structural article rule. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/gloss.p0");

    /* gen211 (cardinal KB-first principle): multi-word INTENT phrases — the exact
     * surface forms that mean "ask my name" etc. — live in kb/core/intents.p0, not in
     * C arrays. kb_intent_match() queries intent_phrase/2; a form taught at runtime
     * grows the class with no code edit (see kb/core/intents.p0, PRINCIPLES.md). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/intents.p0");

    /* gen212 (cardinal KB-first principle, OUTPUT side): the agent's own reply
     * phrasings — "Nice to meet you, {name}!" etc. — live in kb/core/responses.p0, not
     * as C literals. kb_response() fills response_template/2; a phrasing taught at
     * runtime grows the class with no code edit (see PRINCIPLES.md). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/responses.p0");

    /* gen215 (docs/plans/the-linguistic-glue.md, G0): curated glue_role/2 — which
     * faculties carry cross-turn coherence and what each contributes. Reified below into
     * glue_faculty/2 only for modules that really exist, so the self-model can't drift. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/glue.p0");

    /* Reflective self-model: the agent writes itself into its own KB, derived
     * from real structure (PRINCIPLES.md). Tagged KB_REFLECTIVE so it is
     * regenerated every boot and NEVER persisted (DESIGN.md D3). */
    kb_set_origin(b->kb, KB_REFLECTIVE);
    const char *me[] = {"parrot0"};
    kb_assert(b->kb, "i_am", me, 1);
    for (size_t i = 0; i < registry_len; i++) {
        const char *m[] = {registry[i].name};
        kb_assert(b->kb, "module", m, 1);
        /* gen215 (G0): if this real module has a curated glue_role, reify it as a
         * glue_faculty fact — the linguistic-glue family, derived from real structure. */
        char role[1][KB_TERM_LEN];
        const char *gq[2] = { registry[i].name, NULL };
        if (kb_match(b->kb, "glue_role", gq, 2, role, 1) == 1) {
            const char *gf[] = { registry[i].name, role[0] };
            kb_assert(b->kb, "glue_faculty", gf, 2);
        }
    }
    kb_set_origin(b->kb, KB_SESSION); /* conversation default */
    return b;
}

int brain_load(Brain *b, const char *path, int as_base) {
    if (!b || !b->kb) return 0;
    kb_set_origin(b->kb, as_base ? KB_BASE : KB_SESSION);
    int n = kb_load(b->kb, path);
    kb_set_origin(b->kb, KB_SESSION); /* back to conversation default */
    return n;
}

int brain_save_session(Brain *b, const char *path) {
    if (!b || !b->kb) return -1;
    return kb_save(b->kb, path, KB_SESSION | KB_INDUCED);
}

void brain_destroy(Brain *b) {
    if (!b) return;
    kb_destroy(b->kb);
    free(b);
}

const char *brain_version(void) {
    return "gen224-clean-build-deictic-dir";
}

/* gen55 (C5a): an honest, NON-repeating not-understood reply. The chatsim users
 * showed that repeating "I don't understand that yet." verbatim is the #1
 * naturalness killer (a broken record). So the classic line is kept for a LONE
 * occurrence (no test churn, still honest), but when it would repeat our previous
 * reply we vary — reflecting a salient word from the user so it feels heard, else
 * rotating honest redirects. It never feigns understanding. */
static void not_understood(Brain *b, const char *canon,
                           char *out, size_t out_size) {
    static const char *const v[] = {
        "I'm not sure I followed. Can you say it another way?",
        "I didn't quite catch that. What would you like to know?",
        "Hmm, that's a bit beyond me right now.",
        "I don't understand that yet.",
    };
    const size_t NV = sizeof v / sizeof v[0];
    const char *classic = "I don't understand that yet.";

    if (!b || strcmp(classic, b->last_reply) != 0) { /* fine to say it once */
        put(classic, out, out_size);
        if (b) b->fallbacks++;
        return;
    }

    /* it would repeat -> vary. Prefer reflecting a salient content word. */
    char buf[256];
    snprintf(buf, sizeof buf, "%s", canon);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    const char *sw = NULL;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        char desc[256];
        int known = b && b->kb &&
                    (kb_knows_pred(b->kb, t) ||
                     kb_describe_entity(b->kb, t, desc, sizeof desc));
        if (strlen(t) >= 6 && isalpha((unsigned char)t[0]) &&
            !is_stopword(b, t) && !known) {
            sw = t; break;
        }
    }
    char cand[256];
    unsigned long k = b->fallbacks;
    if (sw) snprintf(cand, sizeof cand, "Hmm, I don't know about %s yet.", sw);
    else    snprintf(cand, sizeof cand, "%s", v[k % NV]);
    for (size_t t = 0; t < NV && strcmp(cand, b->last_reply) == 0; t++)
        snprintf(cand, sizeof cand, "%s", v[(k + t) % NV]);
    put(cand, out, out_size);
    b->fallbacks++;
}

/* gen80: true if word `w` is a likely intent-marker that starts a sub-turn
 * after a discourse connector like "e"/"and" in a compound utterance. */
static int is_intent_starter(const char *w) {
    static const char *const starters[] = {
        "chi", "che", "cosa", "come", "dove", "quando", "perche", "perché",
        "ricordati", "dimmi", "spiegami", "chiamami", "insegnami", "parlami",
        "prima", "poi", "non",
        "what", "who", "where", "when", "why", "how",
        "remember", "tell", "explain", "call", "teach", "say",
        "is", "are", "does", "do", "can", "every", "forget",
        "learn", "describe", "read", "show", "first", "then", "dont",
        NULL
    };
    for (const char *const *s = starters; *s; s++)
        if (strcmp(w, *s) == 0) return 1;
    return 0;
}

/* gen88: true if word `w` is a negation marker that should cause the sub-turn
 * to be suppressed (e.g., "dont answer", "non rispondere"). */
static int is_negation_marker(const char *w) {
    return strcmp(w, "dont") == 0 || strcmp(w, "non") == 0 || strcmp(w, "not") == 0;
}

/* gen80: split `canon` on discourse connectors where the second half starts
 * with an intent marker, dispatch each sub-turn, and join responses. Returns
 * 1 if decomposition was applied, 0 to use normal dispatch. */
static int decompose_and_dispatch(Brain *b, const char *canon, const char *input,
                                   char *out, size_t out_size) {
    /* Don't decompose structured prompts — they have their own parser. */
    if (strncmp(canon, "premise:", 8) == 0 ||
        strncmp(canon, "label premise:", 14) == 0 ||
        strncmp(canon, "explain premise:", 16) == 0 ||
        strncmp(canon, "superglue", 9) == 0 ||
        strncmp(canon, "effect of ", 10) == 0 ||
        strncmp(canon, "cause of ", 9) == 0 ||
        strncmp(canon, "read:", 5) == 0 ||
        strncmp(canon, "learn sequence:", 15) == 0)
        return 0;

    const char *connectors[] = {" e ", " and ", " ed ", " ma ", " but ", NULL};
    const char *conn = NULL;
    size_t conn_len = 0;
    int is_but = 0;
    for (const char *const *c = connectors; *c; c++) {
        const char *pos = strstr(canon, *c);
        if (pos && (!conn || pos < conn)) {
            conn = pos; conn_len = strlen(*c);
            is_but = (strcmp(*c, " ma ") == 0 || strcmp(*c, " but ") == 0);
        }
    }
    if (!conn) return 0;

    const char *after = conn + conn_len;
    while (*after && isspace((unsigned char)*after)) after++;
    if (!*after) return 0;

    char first_word[64]; size_t fw = 0;
    while (after[fw] && !isspace((unsigned char)after[fw]) && fw + 1 < 64)
        { first_word[fw] = (char)tolower((unsigned char)after[fw]); fw++; }
    first_word[fw] = '\0';

    if (!is_intent_starter(first_word)) return 0;

    char sub1[256], sub2[256];
    size_t cpos = (size_t)(conn - canon);
    size_t len = strlen(canon);
    if (cpos >= sizeof sub1) cpos = sizeof sub1 - 1;
    memcpy(sub1, canon, cpos); sub1[cpos] = '\0';
    size_t s2len = len - cpos - conn_len;
    if (s2len >= sizeof sub2) s2len = sizeof sub2 - 1;
    memcpy(sub2, conn + conn_len, s2len); sub2[s2len] = '\0';

    char r1[1024] = "", r2[1024] = "";
    int h1 = 0, h1_disc = 0, h2 = 0;
    int negate1 = 0, negate2 = 0;

    /* gen88: check if either sub-turn starts with a negation marker. */
    {
        char *sw[8]; char b1[256], b2[256];
        snprintf(b1, sizeof b1, "%s", sub1);
        snprintf(b2, sizeof b2, "%s", sub2);
        size_t n1 = split_words(b1, sw, 8);
        if (n1 > 0 && is_negation_marker(sw[0])) negate1 = 1;
        size_t n2 = split_words(b2, sw, 8);
        if (n2 > 0 && is_negation_marker(sw[0])) negate2 = 1;
    }

    if (!is_but) {
        for (size_t i = 0; i < registry_len; i++) {
            if (negate1) break; /* gen88: skip negated sub-turn */
            if (registry[i].handle(b, sub1, input, r1, sizeof r1)) {
                h1 = 1;
                if (strcmp(registry[i].name, "discourse") == 0) h1_disc = 1;
                if (b) {
                    snprintf(b->last_reply, sizeof b->last_reply, "%s", r1);
                    snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
                }
                break;
            }
        }
        if (!h1 && !negate1) return 0;
    } /* first sub-turn unclaimed → fall through to normal dispatch */

    for (size_t i = 0; i < registry_len; i++) {
        if (negate2) break; /* gen88: skip negated sub-turn */
        if (registry[i].handle(b, sub2, input, r2, sizeof r2)) {
            h2 = 1; break;
        }
    }
    if (!h2 && !negate2) return 0;

    snprintf(out, out_size, "%s%s%s", r1,
             (r2[0] && r1[0]) ? " " : "", r2);
    if (!is_but && h1 && !h1_disc) update_topics(b, sub1);
    return 1;
}

/* gen216 (docs/plans/the-linguistic-glue.md, G2 — first pull from glue-bench's gap map):
 * resolve a standalone entity pronoun to the most recent entity and RE-DISPATCH the
 * rewritten turn, so a reference carries across turns ("tell me about the heart" then
 * "what is it part of" -> "what is heart part of" -> circulatory). This is linguistic
 * glue as a DETERMINISTIC operation over real session state (b->last_entity), not an
 * emergent coherence field (PRINCIPLES anti-impostor). Conservative: runs only as a
 * FALLBACK when the turn as-is was not understood, and claims only if a real module
 * answers the resolved turn — so it never hijacks a turn that already works. Skips coref
 * itself to avoid re-entry. Mirror of pragma_peel. */
static int coref_resolve(Brain *b, const char *canon, char *out, size_t out_size) {
    if (!b || !b->has_last_entity) return 0;
    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;                       /* a bare pronoun is not a query */
    size_t pidx = nw;
    for (size_t i = 0; i < nw; i++)
        if (!strcmp(w[i], "it") || !strcmp(w[i], "its")) { pidx = i; break; }
    if (pidx == nw) return 0;

    char rw[256]; size_t off = 0; rw[0] = '\0';
    for (size_t i = 0; i < nw && off + 1 < sizeof rw; i++) {
        const char *tok = (i == pidx) ? b->last_entity : w[i];
        off += (size_t)snprintf(rw + off, sizeof rw - off, "%s%s", i ? " " : "", tok);
    }
    if (!rw[0] || strcmp(rw, canon) == 0) return 0;

    for (size_t i = 0; i < registry_len; i++) {
        if (strcmp(registry[i].name, "coref") == 0) continue;     /* no re-entry */
        if (registry[i].handle(b, rw, rw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    return 0;
}

/* gen221 (the-linguistic-glue.md, G2 — symptom #5, "one interlocutor across
 * faculties"): bridge MEMORY into ARITHMETIC, the essay's deepest absence-symptom
 * (the sense of talking to several independent systems rather than one). When the
 * user has told us a numeric personal value ("remember my favorite number is 7",
 * stored KB-first as user_value/2) and a later turn computes with it ("what is my
 * favorite number plus 3"), resolve the "my <key>" reference to the value INFERRED
 * from KB memory, rewrite the turn, and re-dispatch so mod_arith computes it
 * ("what is 7 plus 3" -> "10."). The continuity is a DETERMINISTIC substitution
 * over real KB state, not an emergent coherence field (PRINCIPLES anti-impostor).
 * Conservative, mirroring correction_peel/coref_resolve: it fires only when (a) a
 * "my <key>" matches a remembered NUMERIC user_value, (b) an arithmetic operator
 * follows the key in the same turn (so a pure recall is left for mod_memory), and
 * (c) some module actually claims the rewritten turn. Runs as a PRE-dispatch
 * normalization so a content module cannot mis-claim the unresolved turn first. */
static int memref_resolve(Brain *b, const char *canon, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    size_t mi = nw;
    for (size_t i = 0; i < nw; i++) if (!strcmp(w[i], "my")) { mi = i; break; }
    if (mi == nw || mi + 1 >= nw) return 0;

    /* the key is the run of words after "my", stopping at the first operator or
     * number — a key names a thing, it is not part of the computation. */
    size_t run = 0;
    while (mi + 1 + run < nw) {
        const char *t = w[mi + 1 + run];
        double dv;
        if (arith_op_char(t) || parse_value(t, &dv)) break;
        run++;
    }
    if (run == 0) return 0;
    /* an operator must follow the key (else it is a recall, not a computation). */
    if (mi + 1 + run >= nw || !arith_op_char(w[mi + 1 + run])) return 0;

    /* longest-first: the longest key prefix that names a stored value wins. */
    char value[KB_TERM_LEN]; int found = 0; size_t span = 0;
    for (size_t s = run; s >= 1 && !found; s--) {
        char key[128]; size_t off = 0; key[0] = '\0';
        for (size_t k = 0; k < s && off + 1 < sizeof key; k++)
            off += (size_t)snprintf(key + off, sizeof key - off,
                                    "%s%s", k ? "_" : "", w[mi + 1 + k]);
        const char *q[2] = { key, NULL };
        char res[1][KB_TERM_LEN];
        if (kb_match(b->kb, "user_value", q, 2, res, 1) == 1) {
            snprintf(value, sizeof value, "%s", res[0]);
            span = s; found = 1;
        }
    }
    if (!found) return 0;

    /* rewrite "... my <key> <rest>" as "... <value> <rest>" and re-dispatch. */
    char rw[256]; size_t off = 0; rw[0] = '\0';
    for (size_t i = 0; i < nw && off + 1 < sizeof rw; i++) {
        if (i == mi) {
            off += (size_t)snprintf(rw + off, sizeof rw - off, "%s%s", i ? " " : "", value);
            i += span;                          /* skip the "my <key>" span */
            continue;
        }
        off += (size_t)snprintf(rw + off, sizeof rw - off, "%s%s", i ? " " : "", w[i]);
    }
    if (!rw[0] || !strcmp(rw, canon)) return 0;

    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, rw, rw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            return 1;
        }
    }
    return 0;
}

/* gen222 (the-linguistic-glue.md, G2 — symptom #3, "over-literal"): the result of a
 * computation is carried as a discourse antecedent so a PRECISATION can continue it.
 * Stored KB-first (per F.'s steer) as last_result/1 in the REFLECTIVE layer — inferred
 * back from the KB, never held in a C field, and never persisted (it is transient
 * working memory, regenerated by the next computation). Records ONLY a bare numeric
 * answer ("12."), never a sentence ("Yes.", "5. (...)"). Single fact: the previous
 * value is retracted before the new one is asserted. */
static void note_arith_result(Brain *b, const char *out) {
    if (!b || !b->kb || !out) return;
    size_t n = strlen(out);
    if (n < 2 || out[n - 1] != '.') return;           /* a value reply ends in '.' */
    char tmp[64];
    if (n - 1 >= sizeof tmp) return;
    memcpy(tmp, out, n - 1); tmp[n - 1] = '\0';        /* drop the trailing period */
    for (const char *p = tmp; *p; p++)                 /* purely numeric, nothing else */
        if (!isdigit((unsigned char)*p) && *p != '-' && *p != '.') return;
    double v;
    if (!parse_value(tmp, &v)) return;

    char prev[1][KB_TERM_LEN]; const char *q[1] = { NULL };
    if (kb_match(b->kb, "last_result", q, 1, prev, 1) == 1) {
        const char *pr[] = { prev[0] };
        kb_retract(b->kb, "last_result", pr, 1);
    }
    kb_set_origin(b->kb, KB_REFLECTIVE);               /* transient: never saved */
    const char *a[] = { tmp };
    kb_assert(b->kb, "last_result", a, 1);
    kb_set_origin(b->kb, KB_SESSION);                  /* restore conversation default */
}

/* gen222 (the-linguistic-glue.md, G2 — symptom #3, "over-literal"): a PRECISATION that
 * continues the previous computation. "what is 2 plus 2" then "and times 3" must mean
 * "(2 plus 2) times 3" = 12, not a literal fragment parrot0 cannot parse. The implicit
 * left operand is the last result, INFERRED from KB memory (last_result/1); the turn is
 * rewritten "what is <last_result> <tail>" and re-dispatched so the arithmetic core
 * computes it (EN+IT: "e per 3" -> 12). Continuity as a DETERMINISTIC substitution over
 * real KB state, not an emergent field (PRINCIPLES anti-impostor). Conservative: fires
 * only when (a) a last_result exists, (b) after an optional connector the turn is PURELY
 * an arithmetic tail (operator, then numbers/operators/"by", nothing else — so a normal
 * "and tell me about X" is never hijacked), and (c) some module claims the rewrite.
 * Pre-dispatch, mirroring memref_resolve. */
static int continue_resolve(Brain *b, const char *canon, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char res[1][KB_TERM_LEN]; const char *q[1] = { NULL };
    if (kb_match(b->kb, "last_result", q, 1, res, 1) != 1) return 0;

    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw == 0) return 0;

    size_t i = 0;                                       /* skip a leading connector */
    while (i < nw && (!strcmp(w[i], "and") || !strcmp(w[i], "e") ||
                      !strcmp(w[i], "then") || !strcmp(w[i], "poi") ||
                      !strcmp(w[i], "also") || !strcmp(w[i], "anche") ||
                      !strcmp(w[i], "inoltre"))) i++;
    if (i >= nw || !arith_op_char(w[i])) return 0;      /* must lead with an operator */

    int saw_num = 0;                                    /* rest must be a pure arith tail */
    for (size_t k = i; k < nw; k++) {
        double dv;
        if (arith_op_char(w[k]) || !strcmp(w[k], "by")) continue;
        if (parse_value(w[k], &dv)) { saw_num = 1; continue; }
        return 0;
    }
    if (!saw_num) return 0;

    char residue[256]; size_t off = 0; residue[0] = '\0';
    for (size_t k = i; k < nw && off + 1 < sizeof residue; k++)
        off += (size_t)snprintf(residue + off, sizeof residue - off,
                                "%s%s", k > i ? " " : "", w[k]);
    char rw[256];
    snprintf(rw, sizeof rw, "what is %s %s", res[0], residue);

    for (size_t r = 0; r < registry_len; r++) {
        if (registry[r].handle(b, rw, rw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[r].name);
            return 1;
        }
    }
    return 0;
}

size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size) {
    if (out_size == 0) return 0;
    if (b) b->turns++;

    /* gen158: once the whole KB (base + profile) is loaded, materialize the
     * part_of/2 relation latent in the concept descriptions, so the resolution
     * engine can prove and query it. Done on the first turn, when loading is
     * complete; KB_REFLECTIVE so it never persists. */
    if (b && b->kb && !b->relations_derived) {
        kb_derive_part_of(b->kb);
        b->relations_derived = 1;
    }

    char norm[256];
    normalize(input, norm, sizeof norm);

    /* gen43: canonicalize the parsing surface (function words -> English tokens)
     * before dispatch, so the reasoning core answers in any mapped language
     * without duplicating a module. `raw` (input) is left untouched, so the
     * reader still induces its generative model from the original prose. */
    char canon[256];
    canonicalize_lang(norm, canon, sizeof canon);

    /* gen80: try to decompose compound turns (e.g. "chi sei e ricordati X")
     * before the normal single-turn dispatch. */
    if (b && decompose_and_dispatch(b, canon, input, out, out_size))
        { note_arith_result(b, out); return strlen(out); }

    /* gen142 (E3): peel a leading discourse-marker opener and re-dispatch the
     * residue, so a content task wrapped in a channel-management opener survives
     * ("anyway, is socrates a man" -> "Yes."). Only claims when the residue is
     * actually owned by a module; otherwise the original turn dispatches normally
     * and its pragmatic shape is read by mod_pragma. */
    if (b && pragma_peel(b, canon, input, out, out_size))
        { note_arith_result(b, out); return strlen(out); }

    /* gen218: an explicit correction ("no, X is not a Y") peels its marker and
     * re-dispatches the negative claim with the correction flag set, so the
     * standing belief is overridden and the conclusion re-derives. */
    if (b && correction_peel(b, canon, input, out, out_size))
        { note_arith_result(b, out); return strlen(out); }

    /* gen221 (glue, symptom #5): a numeric personal fact remembered earlier feeds a
     * later computation — resolve "my <key>" to its KB value and re-dispatch so the
     * arithmetic core computes it ("what is my favorite number plus 3" -> "10.").
     * Pre-dispatch so mod_memory cannot mis-claim the unresolved reference first. */
    if (b && memref_resolve(b, canon, out, out_size))
        { note_arith_result(b, out); return strlen(out); }

    /* gen222 (glue, symptom #3): a precisation that continues the previous computation
     * ("and times 3" after "what is 2 plus 2") — prepend the last result, inferred from
     * the KB, and re-dispatch so the arithmetic core finishes it ("what is 4 times 3"
     * -> "12."). Pre-dispatch so the bare fragment cannot fall to not-understood. */
    if (b && continue_resolve(b, canon, out, out_size))
        { note_arith_result(b, out); return strlen(out); }

    /* Walk the registry; first module to claim the turn wins. */
    int handled = 0, handled_by_discourse = 0;

    /* gen83: extract capitalized words as named-entity candidates. */
    if (b) {
        char rbuf[256];
        snprintf(rbuf, sizeof rbuf, "%s", input);
        char *rw[64];
        size_t rnw = split_words(rbuf, rw, 64);
        for (size_t i = 0; i < rnw && b->entity_count < 8; i++) {
            if (isupper((unsigned char)rw[i][0]) && strlen(rw[i]) >= 2) {
                int dup = 0;
                for (size_t j = 0; j < b->entity_count; j++)
                    if (strcmp(b->entities[j], rw[i]) == 0) { dup = 1; break; }
                if (!dup) {
                    snprintf(b->entities[b->entity_count],
                             sizeof b->entities[0], "%s", rw[i]);
                    b->entity_count++;
                }
            }
        }
    }

    /* gen105 (L20): record the real control-flow trace of this turn — the
     * modules consulted that declined, then the one that claimed it. */
    char declined[64][24]; size_t ndecl = 0;
    const char *winner = "fallback";
    for (size_t i = 0; i < registry_len; i++) {
        if (registry[i].handle(b, canon, input, out, out_size)) {
            handled = 1;
            winner = registry[i].name;
            if (strcmp(registry[i].name, "discourse") == 0) handled_by_discourse = 1;
            if (b) {
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s", registry[i].name);
            }
            break;
        }
        if (ndecl < 64) snprintf(declined[ndecl++], sizeof declined[0], "%s",
                                 registry[i].name);
    }

    /* Commit the trace for "why did you answer that way?" and the verbatim input
     * for "what would you have said without X?" — but NOT when this turn was
     * itself one of those introspective questions, so each reports the decision
     * it is being asked about, not its own lookup. */
    if (b && strcmp(winner, "strategy") != 0 &&
        strcmp(winner, "counterfactual") != 0) {
        b->trace_declined_n = ndecl;
        for (size_t i = 0; i < ndecl; i++)
            snprintf(b->trace_declined[i], sizeof b->trace_declined[0], "%s",
                     declined[i]);
        snprintf(b->trace_winner, sizeof b->trace_winner, "%s", winner);
        b->has_trace = 1;
        snprintf(b->last_input_canon, sizeof b->last_input_canon, "%s", canon);
        snprintf(b->last_input_raw, sizeof b->last_input_raw, "%s", input);
        b->has_last_input = 1;
    }

    /* gen58: update the rolling discourse topic buffer from the current turn,
     * but a summary question should not add its own words to the buffer. */
    if (handled && !handled_by_discourse) update_topics(b, canon);

    /* If no module claimed the turn, fall back to the honest not-understood reply
     * (gen15 retired the gen0 parrot-echo; gen55 made it non-repeating).
     * Honest admission, never a mirror or a wrong "No.". */
    if (!handled) {
        /* gen216 (glue G2): before giving up, try resolving an entity pronoun to the
         * recent entity and re-dispatching — carries a reference across turns. */
        if (coref_resolve(b, canon, out, out_size)) {
            handled = 1;
            if (!handled_by_discourse) update_topics(b, canon);
        } else {
            not_understood(b, canon, out, out_size);
            if (b) {
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s", "fallback");
            }
        }
    }
    /* gen222 (glue, symptom #3): carry a bare numeric answer as the KB last_result so a
     * later precisation ("and times 3") can continue the computation. No-op for any
     * non-numeric reply. */
    note_arith_result(b, out);
    return strlen(out);
}

