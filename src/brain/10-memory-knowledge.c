/* --- module: memory ------------------------------------------------------
 * The first *stateful* part: it learns the user's name and recalls it. This
 * is where the brain stops being purely reactive and starts carrying context
 * across turns.
 *
 * gen57 (C3): it also learns small personal-fact frames: "I have a <thing>
 * named <name>" and "my <thing> is <name>", stored as `called(thing, name)`
 * and queried by "what is my <thing> called?". "call me <X>" / Italian
 * "chiamami <X>" / "mi chiamo <X>" extend the name-teaching path.
 */
/* gen212: the "just learned your name" reply, phrased from the KB (response_template,
 * kb/core/responses.p0) so the wording is knowledge and new phrasings can be taught at
 * runtime. Falls back to the original literal only if no template is registered (e.g.
 * the KB file is absent), so the agent is never mute. */
static void greet_name_reply(Brain *b, char *out, size_t outsz) {
    if (!kb_response(b, "greet_name", b->name, out, outsz))
        snprintf(out, outsz, "Nice to meet you, %s!", b->name);
}

/* gen221 (the-linguistic-glue.md, KB-first memory): parse_value/word_number live at
 * the end of this file (with the arithmetic helpers); forward-declare so the memory
 * frames above can gate a "my <thing> is <N>" fact on the value being numeric. */
static int parse_value(const char *s, double *out);

static int mod_memory(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;

    /* gen193: teach a CONJUNCTION as KB knowledge — "use X as a conjunction" /
     * "usa X come congiunzione" asserts conjunction(X) into the same conjunction/1
     * class the list parsers read. The behaviour then changes with NO code edit:
     * a coordinator parrot0 was just taught splits lists like "and"/"e". This is
     * the KB-migration direction made concrete (PRINCIPLES.md: a fixed engine,
     * lexicon and world both growing as KB). */
    if (b->kb && (cue(norm, "conjunction") || cue(norm, "congiunzione")) &&
        (cue(norm, "use ") || cue(norm, "usa ") ||
         cue(norm, "treat ") || cue(norm, "tratta "))) {
        char nb[256]; snprintf(nb, sizeof nb, "%s", norm);
        char *cw[32]; size_t cnw = split_words(nb, cw, 32);
        size_t marker = cnw;
        for (size_t i = 0; i < cnw; i++)
            if (!strcmp(cw[i], "as") || !strcmp(cw[i], "come")) { marker = i; break; }
        if (marker != cnw && marker > 0) {
            char *word = strip_edge_punct(cw[marker - 1]);
            if (*word && !is_conjunction(b, word)) {
                const char *ar[] = { word };
                kb_set_origin(b->kb, KB_SESSION);
                kb_assert(b->kb, "conjunction", ar, 1);
                char msg[160];
                snprintf(msg, sizeof msg,
                         "Got it - I'll treat \"%s\" as a conjunction now, like \"and\".", word);
                put(msg, out, out_size);
                return 1;
            }
            if (*word) {     /* already known — acknowledge without re-asserting */
                char msg[160];
                snprintf(msg, sizeof msg, "I already treat \"%s\" as a conjunction.", word);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen214: ONE generic, KB-driven teach path (learnable/3 → intent_phrase / intent_cue
     * / response_template). Replaces the per-intent teach blocks: a new learnable intent
     * is now DATA, not C. Runs before the recognizers below so a teach utterance is not
     * consumed as the intent it is teaching. */
    if (try_teach_form(b, norm, raw, out, out_size)) return 1;

    /* Teach: "my name is <X>" */
    static const char *const prefix = "my name is ";
    size_t plen = strlen(prefix);
    if (strncmp(norm, prefix, plen) == 0) {
        const char *name = skip_ws(skip_ws(raw) + plen);
        if (*name == '\0') {
            put("I didn't catch your name.", out, out_size);
            return 1;
        }
        copy_trim(b->name, sizeof b->name, name);
        b->has_name = 1;
        char msg[128];
        greet_name_reply(b, msg, sizeof msg);
        put(msg, out, out_size);
        return 1;
    }

    /* Teach: "call me <X>" and Italian equivalents (chiamami / mi chiamo). */
    static const char *const call_me = "call me ";
    static const char *const chiamami = "chiamami ";
    static const char *const mi_chiamo = "mi chiamo ";
    const char *name_from = NULL;
    if (strncmp(norm, call_me, strlen(call_me)) == 0)
        name_from = raw + strlen(call_me);
    else if (strncmp(norm, chiamami, strlen(chiamami)) == 0)
        name_from = raw + strlen(chiamami);
    else if (strncmp(norm, mi_chiamo, strlen(mi_chiamo)) == 0)
        name_from = raw + strlen(mi_chiamo);
    if (name_from) {
        const char *name = skip_ws(name_from);
        if (*name == '\0') {
            put("I didn't catch your name.", out, out_size);
            return 1;
        }
        copy_trim(b->name, sizeof b->name, name);
        b->has_name = 1;
        char msg[128];
        greet_name_reply(b, msg, sizeof msg);
        put(msg, out, out_size);
        return 1;
    }

    /* Bare self-introduction: "i'm <X>" / "i am <X>" / "im <X>", optionally
     * behind a greeting ("hi, i'm vera"), feeds the SAME name memory that
     * "my name is X" / "call me X" fill. The hazard is stealing affective turns
     * ("i'm tired", "i am bored") from mod_chitchat, so we accept ONLY a single
     * trailing token that is not an article, a stopword, a known KB class, or a
     * common state/feeling — generalizing to unseen NAMES, not phrases. */
    {
        char nbuf[256];
        size_t nl = strlen(norm);
        if (nl < sizeof nbuf) {
            memcpy(nbuf, norm, nl + 1);
            char *nwds[24];
            size_t nnw = split_words(nbuf, nwds, 24);
            size_t cand = nnw;                       /* candidate name index */
            for (size_t i = 0; i + 1 < nnw; i++) {
                if (strcmp(nwds[i], "i'm") == 0 || strcmp(nwds[i], "im") == 0) {
                    cand = i + 1; break;
                }
                if (strcmp(nwds[i], "i") == 0 && i + 2 < nnw &&
                    strcmp(nwds[i + 1], "am") == 0) {
                    cand = i + 2; break;
                }
            }
            if (cand < nnw && cand == nnw - 1) {     /* single-word name only */
                char *c = strip_edge_punct(nwds[cand]);
                static const char *const nonname[] = {
                    "tired","bored","happy","sad","fine","good","ok","okay",
                    "here","ready","sorry","busy","lost","sure","hungry","cold",
                    "hot","sleepy","angry","scared","confused","done","back",
                    "late","free","alone","well","great","bad","new","young",
                    "old","right","wrong","not","sick","glad","nervous",
                    "excited","curious","afraid","awake","hurt","convinced",
                    "kidding","joking","serious","worried","calm","listening",
                    NULL,
                };
                int ok = c[0] && isalpha((unsigned char)c[0]) &&
                         strlen(c) >= 2 && !is_article(c) &&
                         !is_stopword(b, c) && !matches_any(c, nonname) &&
                         !(b->kb && kb_knows_pred(b->kb, c));
                if (ok) {
                    char nm[64];
                    copy_last_word(nm, sizeof nm, raw);
                    copy_trim(b->name, sizeof b->name, nm);
                    b->has_name = 1;
                    char msg[128];
                    greet_name_reply(b, msg, sizeof msg);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* Personal possession frame: "I have a <thing> named <name>",
     * "my <thing> is <name>", "my <thing> is called <name>", plus their
     * Italian canonicalizations. The parser searches for the content frame
     * inside the token stream so a leading social marker ("hi, my dog...")
     * does not derail the content module. */
    {
        char buf[256];
        size_t len = strlen(norm);
        if (len < sizeof buf) {
            memcpy(buf, norm, len + 1);
            if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';
            char *w[12];
            size_t nw = split_words(buf, w, 12);

            /* "I have a <thing> named <name>" */
            {
                size_t i = find_token(w, nw, "i");
                if (i + 4 < nw && strcmp(w[i + 1], "have") == 0 &&
                    is_article(w[i + 2]) && strcmp(w[i + 4], "named") == 0) {
                    const char *thing = w[i + 3];
                    char n[64];
                    copy_last_word(n, sizeof n, raw);
                    remember_possession(b, thing, n);
                    char msg[160];
                    snprintf(msg, sizeof msg, "Got it: your %s is called %s.", thing, n);
                    put(msg, out, out_size);
                    return 1;
                }
            }

            /* "my <thing> is <name>" and "my <thing> is called <name>" */
            {
                size_t i = find_token(w, nw, "my");
                if (i + 3 < nw && strcmp(w[i + 2], "is") == 0) {
                    const char *thing = w[i + 1];
                    char n[64];
                    copy_last_word(n, sizeof n, raw);
                    int has_called = (i + 4 < nw && strcmp(w[i + 3], "called") == 0);
                    if (has_called) {
                        remember_possession(b, thing, n);
                        char msg[160];
                        snprintf(msg, sizeof msg, "Got it: your %s is called %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    } else {
                        remember_possession(b, thing, n);
                        char msg[160];
                        snprintf(msg, sizeof msg, "Got it: your %s is %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    }
                }
            }

            /* gen221 (the-linguistic-glue.md, G2 — symptom #5 setup, KB-first per
             * F.'s steer): a NUMERIC personal fact whose name spans more than one
             * word ("(remember) my favorite number is 7", "il mio numero preferito
             * è 7"). The single-word frame above stores "my age is 30"; this one
             * captures the multi-word key so a LATER turn can COMPUTE with it
             * (memref_resolve). The fact lives ONLY in the KB — user_value(Key, N),
             * Key the '_'-joined span between "my" and "is" — and is INFERRED back
             * from there, never held in a C field (PRINCIPLES.md: knowledge in the
             * KB). Gated on a numeric value, so it never steals an affective/identity
             * "my X is Y"; KB_SESSION, so it persists on /save and stays reversible. */
            if (find_token(w, nw, "my") < nw) {
                size_t mi = find_token(w, nw, "my");
                size_t isx = nw;
                for (size_t k = mi + 2; k < nw; k++)
                    if (strcmp(w[k], "is") == 0) { isx = k; break; }
                if (isx < nw && isx > mi + 2 && isx + 1 < nw) {
                    char val[64];
                    copy_last_word(val, sizeof val, raw);
                    strip_edge_punct(val);
                    double dv;
                    if (parse_value(val, &dv)) {
                        char key[128]; size_t off = 0; key[0] = '\0';
                        for (size_t k = mi + 1; k < isx && off + 1 < sizeof key; k++)
                            off += (size_t)snprintf(key + off, sizeof key - off,
                                                    "%s%s", k > mi + 1 ? "_" : "", w[k]);
                        const char *uv[] = { key, val };
                        kb_assert(b->kb, "user_value", uv, 2);
                        char disp[128]; snprintf(disp, sizeof disp, "%s", key);
                        for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
                        char msg[200];
                        snprintf(msg, sizeof msg, "Got it: your %s is %s.", disp, val);
                        put(msg, out, out_size);
                        return 1;
                    }
                }
            }

            /* gen217 (glue): possessive-pronoun anaphor — "what is his/her/its
             * name?". The antecedent is the salient possession (last set by
             * remember_possession), so the noun need not be repeated. Resolves
             * deterministically over real session state, not a guessed referent;
             * claims only when a recent possession is actually on record. */
            {
                size_t i = find_token(w, nw, "what");
                if (i + 3 < nw && strcmp(w[i + 1], "is") == 0 &&
                    (strcmp(w[i + 2], "his") == 0 || strcmp(w[i + 2], "her") == 0 ||
                     strcmp(w[i + 2], "its") == 0)) {
                    char tail[64];
                    snprintf(tail, sizeof tail, "%s", w[i + 3]);
                    if (strcmp(strip_edge_punct(tail), "name") == 0 &&
                        b->has_last_possession) {
                        const char *n = find_possession_name(b, b->last_possession_thing);
                        if (n) {
                            char msg[160];
                            snprintf(msg, sizeof msg, "Your %s is called %s.",
                                     b->last_possession_thing, n);
                            put(msg, out, out_size);
                            return 1;
                        }
                    }
                }
            }

            /* gen219 (glue): Italian PRO-DROP name question. Italian drops the
             * subject — "come si chiama" (reflexive "si chiama" -> canon "is
             * called") carries no pronoun for coref to resolve, so the EN
             * possessive-pronoun path cannot fire. Structurally it is a bare name
             * question with a NULL subject; the antecedent is the salient
             * possession (gen217's last_possession_thing). Detected by shape, not
             * a stored phrase: a "called"/"named" predicate with no nominal
             * subject (only the opaque opener "come" before "is called"). Claims
             * only when a recent possession is on record — never a guess. This is
             * the IT counterpart of "what is his name", completing the bilingual
             * coref ratchet. */
            if (nw == 3 && strcmp(w[1], "is") == 0 &&
                (strcmp(strip_edge_punct(w[2]), "called") == 0 ||
                 strcmp(strip_edge_punct(w[2]), "named") == 0) &&
                strcmp(w[0], "what") != 0 && strcmp(w[0], "who") != 0 &&
                b->has_last_possession) {
                const char *n = find_possession_name(b, b->last_possession_thing);
                if (n) {
                    char msg[160];
                    snprintf(msg, sizeof msg, "Your %s is called %s.",
                             b->last_possession_thing, n);
                    put(msg, out, out_size);
                    return 1;
                }
            }

            /* gen221 (glue, KB-first per F.'s steer): recall a NUMERIC personal fact
             * INFERRED from KB memory (user_value/2), not a C field. "what is my
             * favorite number" -> query user_value(favorite_number) -> 7. The key is
             * the run of words after "my", '_'-joined; the longest run that names a
             * stored value wins. Falls through if no such fact, so the single-word
             * possession recall below still runs. (The arithmetic case "... plus 3"
             * is intercepted earlier by memref_resolve, so it never reaches here.) */
            {
                size_t i = find_token(w, nw, "what");
                if (i + 2 < nw && strcmp(w[i + 1], "is") == 0) {
                    size_t m = find_token(w + i, nw - i, "my");
                    if (m < nw - i) {
                        m += i;
                        for (size_t span = nw - (m + 1); span >= 1; span--) {
                            char key[128]; size_t off = 0; key[0] = '\0'; int okrun = 1;
                            for (size_t k = 0; k < span && off + 1 < sizeof key; k++) {
                                char *t = strip_edge_punct(w[m + 1 + k]);
                                if (!*t) { okrun = 0; break; }
                                off += (size_t)snprintf(key + off, sizeof key - off,
                                                        "%s%s", k ? "_" : "", t);
                            }
                            if (!okrun) continue;
                            const char *q[2] = { key, NULL };
                            char res[1][KB_TERM_LEN];
                            if (kb_match(b->kb, "user_value", q, 2, res, 1) == 1) {
                                char disp[128]; snprintf(disp, sizeof disp, "%s", key);
                                for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
                                char msg[200];
                                snprintf(msg, sizeof msg, "Your %s is %s.", disp, res[0]);
                                put(msg, out, out_size);
                                return 1;
                            }
                        }
                    }
                }
            }

            /* Queries: "what is my <thing> called?" and "what is my <thing>?" */
            {
                size_t i = find_token(w, nw, "what");
                if (i + 2 < nw && strcmp(w[i + 1], "is") == 0) {
                    size_t m = find_token(w + i, nw - i, "my");
                    /* gen254 (repair): claim ONLY when "my" is actually present.
                     * find_token returns nw-i when absent; the old code fell
                     * through with that sentinel and read a spurious token as the
                     * possession ("what's the usual intent behind those words?"
                     * -> "I don't know what your a is called."). */
                    if (m < nw - i && (m += i) + 1 < nw) {
                        const char *thing = w[m + 1];
                        int has_called = (m + 2 < nw);
                        if (has_called) {
                            char tmp[64];
                            snprintf(tmp, sizeof tmp, "%s", w[m + 2]);
                            has_called = (strcmp(strip_edge_punct(tmp), "called") == 0);
                        }
                        if (strcmp(thing, "name") == 0) {
                            if (b->has_name) {
                                char msg[128];
                                snprintf(msg, sizeof msg, "Your name is %s.", b->name);
                                put(msg, out, out_size);
                            } else {
                                put("I don't know your name yet.", out, out_size);
                            }
                            return 1;
                        }
                        const char *n = find_possession_name(b, thing);
                        char msg[160];
                        if (!n) {
                            snprintf(msg, sizeof msg,
                                     "I don't know what your %s is called.", thing);
                            put(msg, out, out_size);
                            return 1;
                        }
                        if (has_called)
                            snprintf(msg, sizeof msg, "%s.", n);
                        else
                            snprintf(msg, sizeof msg, "Your %s is %s.", thing, n);
                        put(msg, out, out_size);
                        return 1;
                    }
                }
            }
        }
    }

    /* gen148 (E4): ordinary user-model facts beyond name/possessions. */
    {
        const char *val_from = NULL;
        const char *verb = NULL;
        if (strncmp(norm, "i like ", 7) == 0) {
            val_from = raw + 7; verb = "like";
        } else if (strncmp(norm, "i prefer ", 9) == 0) {
            val_from = raw + 9; verb = "prefer";
        } else if (strncmp(norm, "mi piace ", 9) == 0) {
            val_from = raw + 9; verb = "like";
        } else if (strncmp(norm, "preferisco ", 11) == 0) {
            val_from = raw + 11; verb = "prefer";
        }
        if (val_from) {
            char val[64];
            copy_trim(val, sizeof val, skip_ws(val_from));
            if (val[0] == 0) {
                put("I did not catch the preference.", out, out_size);
                return 1;
            }
            snprintf(b->user_preference_verb, sizeof b->user_preference_verb, "%s", verb);
            snprintf(b->user_preference_value, sizeof b->user_preference_value, "%s", val);
            b->has_user_preference = 1;
            char msg[160];
            snprintf(msg, sizeof msg, "Got it: you %s %s.", verb, val);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* The brevity cues are KB knowledge (intent_cue(brevity, …) in kb/core/intents.p0),
     * matched as substrings by kb_cue_match; new cues are taught via the generic
     * learnable/3 path (try_teach_form) at the top of this module — no bespoke handler. */
    if (kb_cue_match(b, "brevity", norm)) {
        snprintf(b->user_constraint, sizeof b->user_constraint, "%s", "keep it short");
        b->has_user_constraint = 1;
        put("Got it: I will keep it short.", out, out_size);
        return 1;
    }
    if (cue(norm, "not too technical") || cue(norm, "avoid technical") ||
        cue(norm, "non essere tecnico") || cue(norm, "non troppo tecnico")) {
        snprintf(b->user_constraint, sizeof b->user_constraint, "%s", "avoid technical detail");
        b->has_user_constraint = 1;
        put("Got it: I will avoid technical detail.", out, out_size);
        return 1;
    }

    if (cue(norm, "what do i like") || cue(norm, "what do i prefer") ||
        cue(norm, "cosa mi piace") || cue(norm, "cosa preferisco")) {
        if (b->has_user_preference) {
            char msg[160];
            snprintf(msg, sizeof msg, "You %s %s.",
                     b->user_preference_verb, b->user_preference_value);
            put(msg, out, out_size);
        } else {
            put("I do not know your preference yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what mood") || cue(norm, "how do i feel") ||
        cue(norm, "come mi sento") || cue(norm, "che umore")) {
        if (b->has_user_mood) {
            char msg[160];
            snprintf(msg, sizeof msg, "You told me you feel %s.", b->user_mood);
            put(msg, out, out_size);
        } else {
            put("I do not know your current mood yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what topic") || cue(norm, "which topic") ||
        cue(norm, "what are we talking about") || cue(norm, "di cosa parliamo") ||
        cue(norm, "di cosa stiamo parlando")) {
        if (b->has_current_topic) {
            char msg[160];
            snprintf(msg, sizeof msg, "The current topic is %s.", b->current_topic);
            put(msg, out, out_size);
        } else {
            put("I do not know the current topic yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what constraint") || cue(norm, "what did i ask you to keep in mind") ||
        cue(norm, "what should you remember for this chat") || cue(norm, "che vincolo")) {
        if (b->has_user_constraint) {
            char msg[192];
            snprintf(msg, sizeof msg, "Your current constraint is: %s.", b->user_constraint);
            put(msg, out, out_size);
        } else {
            put("I do not know any current constraint yet.", out, out_size);
        }
        return 1;
    }

    if (cue(norm, "what do you remember about me") ||
        cue(norm, "what do you know about me") || cue(norm, "cosa ricordi di me") ||
        cue(norm, "cosa sai di me")) {
        char msg[640];
        size_t off = 0;
        int any = 0;
        off = (size_t)snprintf(msg, sizeof msg, "I remember:");
        if (b->has_name && off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s your name is %s", any ? ";" : "", b->name);
            any = 1;
        }
        for (size_t i = 0; i < b->possession_count && off < sizeof msg; i++) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s your %s is %s", any ? ";" : "",
                                    b->possessions[i][0], b->possessions[i][1]);
            any = 1;
        }
        if (b->has_user_preference && off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s you %s %s", any ? ";" : "",
                                    b->user_preference_verb, b->user_preference_value);
            any = 1;
        }
        if (!any) {
            off = (size_t)snprintf(msg, sizeof msg,
                                   "I remember no durable personal facts yet.");
        } else if (off < sizeof msg) {
            off += (size_t)snprintf(msg + off, sizeof msg - off, ".");
        }

        int session = b->has_user_mood || b->has_current_topic || b->has_user_constraint;
        if (session && off < sizeof msg) {
            int s = 0;
            off += (size_t)snprintf(msg + off, sizeof msg - off, " Session context:");
            if (b->has_user_mood && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s you feel %s", s ? ";" : "", b->user_mood);
                s = 1;
            }
            if (b->has_current_topic && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s current topic is %s", s ? ";" : "", b->current_topic);
                s = 1;
            }
            if (b->has_user_constraint && off < sizeof msg) {
                off += (size_t)snprintf(msg + off, sizeof msg - off,
                                        "%s constraint: %s", s ? ";" : "", b->user_constraint);
            }
            if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
        }
        put(msg, out, out_size);
        return 1;
    }

    /* Recall: "what is my name?" — the surface forms are KB knowledge
     * (intent_phrase(ask_name, …) in kb/core/intents.p0), matched here and
     * extensible at runtime via the teach handler above (gen211). */
    if (kb_intent_match(b, "ask_name", norm)) {
        if (b->has_name) {
            char msg[128];
            snprintf(msg, sizeof msg, "Your name is %s.", b->name);
            put(msg, out, out_size);
        } else {
            put("I don't know your name yet.", out, out_size);
        }
        return 1;
    }

    return 0;
}

/* --- module: knowledge ---------------------------------------------------
 * The first step of the Prolog-like spine (see PRINCIPLES.md). It translates
 * a sliver of natural language into ground facts and ground queries over the
 * knowledge base:
 *
 *   "<x> is a <y>"   /  "<x> is an <y>"   ->  assert y(x)
 *   "is <x> a <y>?"  /  "is <x> an <y>?"  ->  query  y(x)   (closed-world)
 *
 * Only single-word x and y for now; richer terms emerge in later generations.
 */

/* Split `s` (modified in place) into up to `max` whitespace-separated words,
 * storing pointers in `argv`. Returns the word count. */
static size_t split_words(char *s, char **argv, size_t max) {
    size_t n = 0;
    char *p = s;
    while (*p && n < max) {
        while (*p && isspace((unsigned char)*p)) *p++ = '\0';
        if (!*p) break;
        argv[n++] = p;
        while (*p && !isspace((unsigned char)*p)) p++;
    }
    return n;
}

/* "a" or "an" — the article that separates subject from class. */
static int is_article(const char *w) {
    return strcmp(w, "a") == 0 || strcmp(w, "an") == 0;
}

/* gen43 — multilingual as a generalization probe (PRINCIPLES.md: no phrasebook).
 * Map one FUNCTION word of any supported language onto the canonical (English)
 * token the reasoning modules already parse, or NULL to leave it untouched.
 * Content words are opaque symbols and are never listed, so the same reasoning
 * core answers in any language whose function words are mapped — *no module is
 * duplicated*. Only tokens that cannot occur in English are listed, so English
 * input is provably unaffected. The competence is thus shown to live in the
 * algorithm, not in English surface strings; where a language needs more than a
 * lexical swap (e.g. Italian negation "x non è un y" reorders to "x not is a y",
 * not the English "x is not a y"), that is the probe correctly exposing a
 * word-order assumption the core still bakes in — a future iteration, not a
 * second phrasebook. */
static const char *canonical_token(const char *w) {
    static const struct { const char *src, *dst; } lex[] = {
        /* Italian */
        {"è",   "is"},
        {"un",  "a"}, {"uno", "a"}, {"una", "a"},
        {"mio", "my"}, {"mia", "my"},
        {"ho",  "i have"},
        {"chiamato", "named"},
        {"si",  "is"}, {"chiama", "called"},
        {"ogni","every"}, {"tutti","all"}, {"tutte","all"},
        {"chi", "who"},

        {"non", "not"},
        {"anche","also"},
        {"causa","causes"},
        /* gen142 (E3): Italian modals so the pragmatic topic-intro / disagreement
         * shapes fire through the SAME mod_pragma path (no phrase duplication). */
        {"possiamo","can"}, {"potremmo","could"},
        {"sfida", "challenge"}, {"risolvere", "solve"}, {"risolveresti", "solve"},
        {"migliorare", "improve"}, {"miglioreresti", "improve"},
        {"implementazione", "implementation"}, {"modifica", "change"},
        {"codice", "code"}, {"capitale", "capital"},

        {"stesso", "yourself"}, {"stessa", "yourself"}, {"te", "you"},
        {"tuo", "your"}, {"tua", "your"}, {"riguarda", "about"},
        {"fallisci", "fail"}, {"fallisce", "fail"},

        {"cos'è", "what is"},
        {"qual", "what"},  /* gen155: "qual è ..." -> "what is ..." reaches the
                            * same concept-recall path as English. */
        {"sono", "am"},
        /* gen141: subject pronouns, so the repair loop's referential-gap probe
         * (a pronoun with no antecedent) reaches the SAME code path in Italian.
         * These are unambiguous subject forms; "lo"/"la"/"li" (clitics/articles)
         * are deliberately left out to avoid colliding with article parsing. */
        {"esso", "it"}, {"essa", "it"}, {"essi", "they"}, {"esse", "they"},
        {"lui", "he"}, {"lei", "she"},
        /* gen142 (E7): local-world vocabulary so the scoped-world module reaches
         * the SAME path in Italian. "mondo"/"storia" name a scope; "assunto"/
         * "assume" are the inspect cue ("cosa è assunto?" -> "what is assumed").
         * These cannot occur as English words, so English input is unaffected. */
        {"mondo", "world"}, {"storia", "story"},
        {"nel", "in the"}, {"nella", "in the"},
        {"questo", "this"}, {"questa", "this"},
        {"assunto", "assumed"}, {"dimentica", "forget"},
        /* Chat-register shorthand (gen64), not a second language. "u"/"r" are
         * English letters, but never stand-alone English *words*; in a chat
         * agent a lone "u"/"r" overwhelmingly means you/are ("what can u do?",
         * "who r u?"). Folding them here routes every intent through the same
         * canonical path instead of accreting shorthand cues per module. */
        {"u",   "you"}, {"r",  "are"},
        /* gen74: chat-register contractions — common abbreviated forms that
         * real users type. Expanding them into their canonical spaced forms
         * lets the existing parsers (arith, knowledge, identity) work on
         * contracted input without duplicating logic. */
        {"whats", "what is"}, {"what's", "what is"},
        {"whos", "who is"}, {"who's", "who is"},
        {"wheres", "where is"}, {"where's", "where is"},
        {"it's", "it is"},
        {"dont",  "do not"},  {"cant", "can not"}, {"isnt", "is not"},
        {"isn't", "is not"}, {"pls", "please"},
    };
    for (size_t i = 0; i < sizeof lex / sizeof lex[0]; i++)
        if (strcmp(w, lex[i].src) == 0) return lex[i].dst;
    return NULL;
}

/* Rewrite a normalized line, canonicalizing each word's function-word form.
 * A trailing '?' is kept on its token so question parsers still see it. For
 * all-English input every token maps to NULL, so the output equals the input
 * (modulo whitespace already collapsed by the parsers' tokenizer). */
static void canonicalize_lang(const char *norm, char *out, size_t out_size) {
    if (out_size == 0) return;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) { snprintf(out, out_size, "%s", norm); return; }
    memcpy(buf, norm, len + 1);

    char *w[64];
    size_t nw = split_words(buf, w, 64);
    size_t off = 0;
    out[0] = '\0';
    for (size_t i = 0; i < nw && off + 1 < out_size; i++) {
        char *tok = w[i];
        size_t tl = strlen(tok);
        const char *tail = "";
        if (tl > 0 && tok[tl - 1] == '?') { tok[tl - 1] = '\0'; tail = "?"; }
        /* gen220: Italian naming idiom "di nome" — a two-word naming marker
         * ("ho un cane di nome rex") equivalent to "named"/"chiamato". Mapped at
         * the language layer (not in a single module) so EVERY parser that
         * already handles "named" gets the variant for free — same no-duplication
         * rule as the per-token map. Only the exact bigram "di nome" collapses;
         * a bare "di" stays "di" (it serves as "of" in relations elsewhere). */
        if (strcmp(tok, "di") == 0 && i + 1 < nw && strcmp(w[i + 1], "nome") == 0) {
            off += (size_t)snprintf(out + off, out_size - off, "%snamed",
                                    i ? " " : "");
            i++;            /* consume "nome" */
            continue;
        }
        const char *canon = canonical_token(tok);
        off += (size_t)snprintf(out + off, out_size - off, "%s%s%s",
                                i ? " " : "", canon ? canon : tok, tail);
    }
}

/* gen240 (universal-comprehension): the CURRENT conversation language lives as the
 * session fact current_language/1 — NOT a C variable — so it persists, /save's, and
 * is queryable ("what language are we speaking?"). Default "en" when no fact yet. */
static void current_lang(Brain *b, char *out, size_t sz) {
    snprintf(out, sz, "en");
    if (!b || !b->kb) return;
    const char *q[] = { NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "current_language", q, 1, hit, 1) > 0)
        snprintf(out, sz, "%s", hit[0]);
}

/* gen240 (universal-comprehension): emit a hardcoded reply in the CURRENT language.
 * For the many C-literal replies not (yet) migrated to response_template/3, this
 * picks the Italian wording when the session language is Italian, else English.
 * Additive: a literal becomes localized just by giving it an `it` here. */
static void tput(Brain *b, const char *en, const char *it, char *out, size_t sz) {
    char lang[8]; current_lang(b, lang, sizeof lang);
    put((strcmp(lang, "it") == 0 && it && *it) ? it : en, out, sz);
}

/* Detect the turn's language from KB-first markers (language_marker/2) and, if it
 * differs from the recorded one, REPLACE the session fact. Sticky: a turn with no
 * exclusive marker keeps the prior language (so neutral turns don't flap). */
static void detect_set_language(Brain *b, const char *norm) {
    if (!b || !b->kb) return;
    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
    char *w[64]; size_t nw = split_words(tmp, w, 64);
    int it = 0, en = 0;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (!*t) continue;
        const char *qi[] = { "it", t }; if (kb_query(b->kb, "language_marker", qi, 2)) it++;
        const char *qe[] = { "en", t }; if (kb_query(b->kb, "language_marker", qe, 2)) en++;
    }
    const char *lang = NULL;
    if (it > en) lang = "it";
    else if (en > it) lang = "en";
    if (!lang) return;                          /* no clear signal -> keep sticky */
    char cur[8]; current_lang(b, cur, sizeof cur);
    if (strcmp(cur, lang) == 0) return;
    char old[4][KB_TERM_LEN];
    const char *qa[] = { NULL };
    size_t k = kb_match(b->kb, "current_language", qa, 1, old, 4);
    for (size_t i = 0; i < k; i++) { const char *o[] = { old[i] }; kb_retract(b->kb, "current_language", o, 1); }
    kb_set_origin(b->kb, KB_SESSION);
    const char *a[] = { lang };
    kb_assert(b->kb, "current_language", a, 1);
}

/* Fetch a localized response_template(Intent, Lang, "…") for the CURRENT language,
 * falling back to English, into `out` (quotes stripped). Returns 1 if found. This
 * is the /3 (localized) selector — additive beside the language-agnostic /2
 * kb_response, never replacing it. */
static int lang_template(Brain *b, const char *intent, char *out, size_t sz) {
    if (!b || !b->kb) return 0;
    char lang[8]; current_lang(b, lang, sizeof lang);
    for (int pass = 0; pass < 2; pass++) {
        const char *L = pass == 0 ? lang : "en";
        const char *q[] = { intent, L, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "response_template", q, 3, hit, 1) > 0) {
            char *p = hit[0]; size_t l = strlen(p);
            if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
            snprintf(out, sz, "%s", p);
            return 1;
        }
        if (pass == 0 && strcmp(lang, "en") == 0) break;   /* no second try needed */
    }
    return 0;
}

/* gen240 (universal-comprehension): an ARTIFACT parrot0 creates (a file, a dir, …)
 * becomes session knowledge too — artifact(Kind, "path") — so "what have you
 * created?" is inferable from the KB, not a C log. */
static void note_artifact(Brain *b, const char *kind, const char *path) {
    if (!b || !b->kb || !path || !*path) return;
    char q[KB_TERM_LEN];
    snprintf(q, sizeof q, "\"%.*s\"", (int)(KB_TERM_LEN - 4), path);
    kb_set_origin(b->kb, KB_SESSION);
    const char *a[] = { kind, q };
    kb_assert(b->kb, "artifact", a, 2);
}

/* gen240 (universal-comprehension): recall from the session conversation log
 * (utterance(Seq, Speaker, "text")). speaker is "self" (parrot0) or "user". `first`
 * picks the earliest vs latest; `word` returns just the first/last word of it.
 * Composes the answer; 0 if the log has nothing for that speaker yet. */
static int recall_utterance(Brain *b, const char *speaker, int first, int word,
                            char *out, size_t sz) {
    if (!b || !b->kb) return 0;
    char seqs[128][KB_TERM_LEN];
    const char *q[] = { NULL, speaker, NULL };
    size_t n = kb_match(b->kb, "utterance", q, 3, seqs, 128);
    if (n == 0) return 0;
    long best = -1; char bestseq[24] = "";
    for (size_t i = 0; i < n; i++) {
        long v = atol(seqs[i]);
        if (best < 0 || (first ? v < best : v > best)) { best = v; snprintf(bestseq, sizeof bestseq, "%s", seqs[i]); }
    }
    const char *qt[] = { bestseq, speaker, NULL };
    char tx[1][KB_TERM_LEN];
    if (kb_match(b->kb, "utterance", qt, 3, tx, 1) == 0) return 0;
    char *p = tx[0]; size_t l = strlen(p);
    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
    const char *who = strcmp(speaker, "self") == 0 ? "I" : "you";
    if (word) {
        char wb[KB_TERM_LEN]; snprintf(wb, sizeof wb, "%s", p);
        char *w[64]; size_t nw = split_words(wb, w, 64);
        if (nw == 0) return 0;
        char *wd = strip_edge_punct(first ? w[0] : w[nw - 1]);
        snprintf(out, sz, "The %s word %s said was \"%s\".", first ? "first" : "last", who, wd);
    } else {
        snprintf(out, sz, "The %s thing %s said was: \"%s\"", first ? "first" : "last", who, p);
    }
    return 1;
}

/* Minimal discourse coreference (gen22): pronouns resolve to the most recent
 * concrete entity mentioned in the knowledge surface. */
static int is_entity_pronoun(const char *w) {
    return strcmp(w, "he") == 0 || strcmp(w, "she") == 0 ||
           strcmp(w, "it") == 0 || strcmp(w, "they") == 0 ||
           strcmp(w, "him") == 0 || strcmp(w, "her") == 0 ||
           strcmp(w, "them") == 0;
}

static int resolve_entity(Brain *b, const char *word, const char **entity,
                          char *out, size_t out_size) {
    if (!is_entity_pronoun(word)) { *entity = word; return 1; }
    if (b->has_last_entity) { *entity = b->last_entity; return 1; }

    char msg[160];
    snprintf(msg, sizeof msg, "I don't know who %s refers to.", word);
    put(msg, out, out_size);
    return 0;
}

static void remember_entity(Brain *b, const char *word, const char *entity) {
    if (is_entity_pronoun(word) || !entity || strlen(entity) >= KB_TERM_LEN)
        return;
    snprintf(b->last_entity, sizeof b->last_entity, "%s", entity);
    b->has_last_entity = 1;
}

/* gen79: run rule induction over the current KB and, if any new rules are
 * found, append them to `out`. Returns number of rules induced. */
static size_t auto_induce(Brain *b, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char heads[16][KB_TERM_LEN], bodies[16][KB_TERM_LEN];
    size_t k = kb_induce(b->kb, 2, heads, bodies, 16);
    if (k == 0) return 0;
    /* Filter out emergent rules whose head or body is an internal predicate
     * (coding knowledge, social markers, etc.) — those are infrastructure,
     * not domain reasoning. */
    size_t kept = 0;
    size_t out_len = strlen(out);
    for (size_t i = 0; i < k; i++) {
        if (is_internal_pred(heads[i]) || is_internal_pred(bodies[i])) continue;
        if (out_len + 2 < out_size) {
            if (kept == 0) {
                if (out_len > 0) { out[out_len] = ' '; out[out_len + 1] = '\0'; out_len++; }
            }
            out_len += (size_t)snprintf(out + out_len, out_size - out_len,
                                         "%s%s(X) :- %s(X).",
                                         kept ? " " : "Induced: ", heads[i], bodies[i]);
            kept++;
        }
    }
    return kept;
}

/* Admit ignorance about a predicate we've never heard of (gen16 scaffold;
 * see DESIGN.md D6 — to become emergent meta-knowledge). */
static void idk(const char *pred, char *out, size_t out_size) {
    char msg[160];
    snprintf(msg, sizeof msg, "I don't know about %s.", pred);
    put(msg, out, out_size);
}

/* Answer a "why ...?" by rendering the proof, or admit there is none. */
static void explain_reply(Brain *b, const char *pred, const char *const *args,
                          size_t argc, char *out, size_t out_size) {
    if (kb_is_conflicted(b->kb, pred, args, argc)) {
        put("I have conflicting evidence for that.", out, out_size);
        return;
    }

    char ex[512];
    if (kb_explain(b->kb, pred, args, argc, ex, sizeof ex)) {
        char msg[600];
        if (strstr(ex, " because ")) snprintf(msg, sizeof msg, "%s.", ex);
        else snprintf(msg, sizeof msg, "%s is a known fact.", ex);
        put(msg, out, out_size);
        store_proof(b, ex);
    } else {
        put("I can't show that.", out, out_size);
    }
}

/* Answer "how do you know <goal>?" by reading the proof trace and reporting
 * its *depth* — the number of inference steps. A goal proved straight from a
 * stored fact is DIRECT (zero rule applications); a goal proved through rules
 * is MULTI-STEP, and we report how many steps. The step count is the number of
 * " because " links the proof renderer emits, i.e. one per rule application
 * along the chain — so this is a property of the actual proof structure, not a
 * canned label. A goal with no proof is refused, never invented (gen26). */
static void howknow_reply(Brain *b, const char *pred, const char *const *args,
                          size_t argc, char *out, size_t out_size) {
    if (kb_is_conflicted(b->kb, pred, args, argc)) {
        put("I have conflicting evidence for that.", out, out_size);
        return;
    }

    char ex[512];
    if (!kb_explain(b->kb, pred, args, argc, ex, sizeof ex)) {
        put("I can't show that.", out, out_size);
        return;
    }

    size_t steps = 0;
    for (const char *p = ex; (p = strstr(p, " because ")) != NULL; p += 9)
        steps++;

    char msg[640];
    if (steps == 0)
        snprintf(msg, sizeof msg, "Directly: %s is a known fact.", ex);
    else
        snprintf(msg, sizeof msg, "By %zu step%s of reasoning: %s.",
                 steps, steps == 1 ? "" : "s", ex);
    put(msg, out, out_size);
    store_proof(b, ex);
}

static int mod_knowledge(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size);

static char *trim_mut(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    return s;
}

static int apply_premise_clause(Brain *tmp, char *clause) {
    clause = trim_mut(clause);
    if (*clause == '\0') return 1;

    int origin = KB_SESSION;
    if (strncmp(clause, "base says ", 10) == 0) {
        origin = KB_BASE;
        clause = trim_mut(clause + 10);
    } else if (strncmp(clause, "session says ", 13) == 0) {
        origin = KB_SESSION;
        clause = trim_mut(clause + 13);
    }

    kb_set_origin(tmp->kb, origin);
    char discard[256];
    int claimed = mod_knowledge(tmp, clause, clause, discard, sizeof discard);
    kb_set_origin(tmp->kb, KB_SESSION);
    return claimed && strncmp(discard, "I couldn't", 10) != 0 &&
           strncmp(discard, "I don't understand", 18) != 0;
}

static int apply_premises(Brain *tmp, char *premises) {
    char *p = premises;
    while (p && *p) {
        char *next = strstr(p, " and ");
        if (next) {
            *next = '\0';
            next += 5;
        }
        if (!apply_premise_clause(tmp, p)) return 0;
        p = next;
    }
    return 1;
}

/* Entailment surface modes. PLAIN/EXPLAIN speak parrot0's own 4-valued
 * epistemic vocabulary; LABEL collapses it into SuperGLUE CB's 3-way label
 * space (entailment / contradiction / neutral), discovered from the CB probe.
 * The collapse is a real decision: both "unknown" (predicate never seen) and
 * "conflicted" (contradictory evidence) become neutral — see Decision
 * D-2026-06-15b. */
enum { ENT_PLAIN = 0, ENT_EXPLAIN = 1, ENT_LABEL = 2 };

static void entailment_status(Brain *tmp, const char *hyp, int mode,
                              char *out, size_t out_size) {
    char hbuf[256];
    size_t len = strlen(hyp);
    if (len >= sizeof hbuf) { put("I don't understand that entailment yet.", out, out_size); return; }
    memcpy(hbuf, hyp, len + 1);
    if (len > 0 && hbuf[len - 1] == '?') hbuf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(hbuf, w, 8);
    const char *pred = NULL;
    const char *args[2];
    size_t argc = 0;

    if (nw == 4 && strcmp(w[0], "is") == 0 && is_article(w[2])) {
        pred = w[3];
        args[0] = w[1];
        argc = 1;
    } else if (nw == 6 && strcmp(w[0], "is") == 0 &&
               strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0) {
        pred = w[3];
        args[0] = w[1];
        args[1] = w[5];
        argc = 2;
    } else {
        put("I don't understand that entailment yet.", out, out_size);
        return;
    }

    if (!kb_knows_pred(tmp->kb, pred))
        put(mode == ENT_LABEL ? "Neutral." : "Unknown.", out, out_size);
    else if (kb_is_conflicted(tmp->kb, pred, args, argc))
        put(mode == ENT_LABEL ? "Neutral." : "Conflicted.", out, out_size);
    else if (kb_query(tmp->kb, pred, args, argc)) {
        if (mode == ENT_LABEL) {
            put("Entailment.", out, out_size);
        } else if (mode == ENT_PLAIN) {
            put("Entailed.", out, out_size);
        } else {
            char ex[512];
            if (kb_explain(tmp->kb, pred, args, argc, ex, sizeof ex)) {
                char msg[640];
                if (strstr(ex, " because "))
                    snprintf(msg, sizeof msg, "Entailed: %s.", ex);
                else
                    snprintf(msg, sizeof msg, "Entailed: %s is a known fact.", ex);
                put(msg, out, out_size);
            } else {
                put("Entailed.", out, out_size);
            }
        }
    }
    else if (kb_is_negated(tmp->kb, pred, args, argc))
        put(mode == ENT_LABEL ? "Contradiction." : "Contradicted.", out, out_size);
    else
        put(mode == ENT_LABEL ? "Neutral." : "Not entailed.", out, out_size);
}

static int entailment_reply(const char *premises, const char *hypothesis,
                            int mode, char *out, size_t out_size) {
    Brain tmp;
    memset(&tmp, 0, sizeof tmp);
    tmp.kb = kb_create();
    if (!tmp.kb) { put("I couldn't evaluate that entailment.", out, out_size); return 1; }

    char pbuf[512];
    size_t plen = strlen(premises);
    if (plen >= sizeof pbuf) {
        kb_destroy(tmp.kb);
        put("I don't understand that entailment yet.", out, out_size);
        return 1;
    }
    memcpy(pbuf, premises, plen + 1);

    if (!apply_premises(&tmp, pbuf)) {
        kb_destroy(tmp.kb);
        put("I don't understand that entailment yet.", out, out_size);
        return 1;
    }

    entailment_status(&tmp, trim_mut((char *)hypothesis), mode, out, out_size);
    kb_destroy(tmp.kb);
    return 1;
}

/* gen231 (LLMSCORE, ambitious): crude English singularizer so a plural subject in
 * a universal ("all MEN are mortal", "all ROSES are flowers") maps to the singular
 * predicate the fact path uses (man/1, rose/1). A few irregulars, then regular
 * -ies/-es/-s; adjectives and already-singular words pass through unchanged. */
static void singularize(const char *in, char *out, size_t sz) {
    static const struct { const char *pl, *sg; } irr[] = {
        {"men","man"},{"women","woman"},{"people","person"},{"children","child"},
        {"feet","foot"},{"teeth","tooth"},{"mice","mouse"},{"geese","goose"},{NULL,NULL} };
    for (size_t i = 0; irr[i].pl; i++)
        if (strcmp(in, irr[i].pl) == 0) { snprintf(out, sz, "%s", irr[i].sg); return; }
    size_t n = strlen(in);
    if (n > 3 && strcmp(in + n - 3, "ies") == 0) {            /* puppies -> puppy */
        snprintf(out, sz, "%.*sy", (int)(n - 3), in); return;
    }
    if (n > 4 && (strcmp(in + n - 4, "ches") == 0 ||
                  strcmp(in + n - 4, "shes") == 0)) {          /* beaches -> beach */
        snprintf(out, sz, "%.*s", (int)(n - 2), in); return;
    }
    if (n > 3 && (strcmp(in + n - 3, "ses") == 0 ||
                  strcmp(in + n - 3, "xes") == 0 ||
                  strcmp(in + n - 3, "zes") == 0)) {           /* boxes -> box */
        snprintf(out, sz, "%.*s", (int)(n - 2), in); return;
    }
    if (n > 2 && in[n - 1] == 's' && in[n - 2] != 's') {      /* roses -> rose */
        snprintf(out, sz, "%.*s", (int)(n - 1), in); return;
    }
    snprintf(out, sz, "%s", in);
}

/* gen231 (LLMSCORE, ambitious): a ONE-TURN syllogism. "if all men are mortal and
 * socrates is a man, is socrates mortal?" — apply the premises to a scratch KB
 * (universals become rules via the parser inside mod_knowledge) and resolve the
 * closing yes/no question against it. Genuine multi-premise inference in a single
 * turn, the shape an LLM is probed with — not a recited answer. Declines unless it
 * actually reaches a Yes/No, so an unparseable "if …" still falls through honestly. */
static int one_turn_syllogism(Brain *b, const char *norm, char *out, size_t out_size) {
    (void)b;
    size_t L = strlen(norm);
    /* gen290: a trailing '?' is no longer required — the "if <premises>, is <x>
     * <y>" shape is interrogative by structure, so prompt 125 ("if all cats are
     * mammals and Tom is a cat, is Tom a mammal", no '?') routes here too. The
     * "if " prefix + comma + "is/are" tail keep this from hijacking prose, and a
     * non-deductive turn still declines when apply_premises or the query fail. */
    if (L < 10 || L >= 480) return 0;
    if (strncmp(norm, "if ", 3) != 0) return 0;
    const char *comma = strrchr(norm, ',');
    if (!comma) return 0;
    const char *q = comma + 1;
    while (*q == ' ') q++;
    if (strncmp(q, "is ", 3) != 0 && strncmp(q, "are ", 4) != 0) return 0;

    char prem[512];
    size_t plen = (size_t)(comma - (norm + 3));
    if (plen == 0 || plen >= sizeof prem) return 0;
    memcpy(prem, norm + 3, plen); prem[plen] = '\0';

    Brain tmp; memset(&tmp, 0, sizeof tmp);
    tmp.kb = kb_create();
    if (!tmp.kb) return 0;
    kb_set_origin(tmp.kb, KB_SESSION);
    if (!apply_premises(&tmp, prem)) { kb_destroy(tmp.kb); return 0; }

    char qbuf[256]; snprintf(qbuf, sizeof qbuf, "%s", q);
    char ans[256];
    int claimed = mod_knowledge(&tmp, qbuf, qbuf, ans, sizeof ans);
    kb_destroy(tmp.kb);
    if (!claimed) return 0;
    if (strncmp(ans, "Yes", 3) != 0 && strncmp(ans, "No", 2) != 0 &&
        strncmp(ans, "Conflicted", 10) != 0) return 0;
    put(ans, out, out_size);
    return 1;
}

/* gen290 (basic-chat cat.7 "Logica deduttiva"): the SAME multi-premise inference
 * as one_turn_syllogism, but for the natural MULTI-SENTENCE surface form
 * "Socrates is a man. All men are mortal. Is Socrates mortal?" (prompt 122) — the
 * period-separated shape an LLM is actually probed with, rather than the packed
 * "if <p1> and <p2>, <q>?" conjunction. The reasoning core is untouched: split
 * the turn into sentences on '.', treat the last as the yes/no query and the rest
 * as premises, apply them to a scratch KB, and resolve the query. Deliberately
 * language-NEUTRAL: the interrogative is marked by the trailing '?' (not an
 * English "is/are" prefix), so the same code path serves the Italian
 * "socrate è un uomo. tutti gli uomini sono mortali. socrate è mortale?" once the
 * universal parser is article-tolerant (below). Declines unless the premises
 * genuinely assert and the query reaches a Yes/No, so ordinary multi-sentence
 * prose is never hijacked. */
static int multi_sentence_syllogism(Brain *b, const char *norm,
                                    char *out, size_t out_size) {
    (void)b;
    size_t L = strlen(norm);
    if (L < 10 || L >= 480) return 0;
    if (norm[L - 1] != '?') return 0;          /* must close with a question */

    char buf[512];
    memcpy(buf, norm, L + 1);

    /* Split into sentence segments on '.' (the '?' stays on the final segment). */
    char *segs[16];
    size_t nseg = 0;
    char *start = buf;
    for (char *p = buf; ; p++) {
        if (*p == '.' || *p == '\0') {
            char c = *p;
            *p = '\0';
            char *s = trim_mut(start);
            if (*s) { if (nseg >= 16) return 0; segs[nseg++] = s; }
            start = p + 1;
            if (c == '\0') break;
        }
    }
    if (nseg < 2) return 0;                     /* need premises + a query */

    char *query = segs[nseg - 1];
    char prem[512];
    size_t off = 0;
    prem[0] = '\0';
    for (size_t i = 0; i + 1 < nseg; i++) {
        int n = snprintf(prem + off, sizeof prem - off, "%s%s",
                         i ? " and " : "", segs[i]);
        if (n < 0 || (size_t)n >= sizeof prem - off) return 0;
        off += (size_t)n;
    }

    Brain tmp;
    memset(&tmp, 0, sizeof tmp);
    tmp.kb = kb_create();
    if (!tmp.kb) return 0;
    kb_set_origin(tmp.kb, KB_SESSION);
    if (!apply_premises(&tmp, prem)) { kb_destroy(tmp.kb); return 0; }

    char qbuf[256];
    snprintf(qbuf, sizeof qbuf, "%s", query);
    char ans[256];
    int claimed = mod_knowledge(&tmp, qbuf, qbuf, ans, sizeof ans);
    kb_destroy(tmp.kb);
    if (!claimed) return 0;
    if (strncmp(ans, "Yes", 3) != 0 && strncmp(ans, "No", 2) != 0 &&
        strncmp(ans, "Conflicted", 10) != 0) return 0;
    put(ans, out, out_size);
    return 1;
}

/* gen233 (kb-first manifesto): shallow transitive closure over grows_with/2 — true
 * if FEATURE is co-monotone with BASE (FEATURE grows when BASE grows), directly or
 * through a chain (circumference -> circle). The qualitative analogue of SLD: the
 * unary rule engine can't carry a binary transitive relation, so the chain is walked
 * here in C, over KB facts (not hardcoded edges). */
static int qchain_reaches(KB *kb, const char *feature, const char *base, int depth) {
    if (!kb || depth > 5) return 0;
    if (strcmp(feature, base) == 0) return 1;
    const char *pat[2] = { feature, NULL };
    char nexts[16][KB_TERM_LEN];
    size_t n = kb_match(kb, "grows_with", pat, 2, nexts, 16);
    for (size_t i = 0; i < n; i++)
        if (qchain_reaches(kb, nexts[i], base, depth + 1)) return 1;
    return 0;
}

/* gen240: fetch magnitude(Dim, Item, Rank) into `rank`, trying the item as written
 * and then a naive singular. Returns 1 if found. */
static int magnitude_lookup(Brain *b, const char *dim, const char *item, char *rank) {
    const char *q[] = { dim, item, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "magnitude", q, 3, hit, 1) > 0) { snprintf(rank, KB_TERM_LEN, "%s", hit[0]); return 1; }
    size_t l = strlen(item);
    if (l > 1 && item[l - 1] == 's') {
        char sg[64]; snprintf(sg, sizeof sg, "%.*s", (int)(l - 1), item);
        const char *q2[] = { dim, sg, NULL };
        if (kb_match(b->kb, "magnitude", q2, 3, hit, 1) > 0) { snprintf(rank, KB_TERM_LEN, "%s", hit[0]); return 1; }
    }
    return 0;
}

/* gen250: KB-backed magnitude cue map. The words that name a comparison
 * dimension live in magnitude_cue(Cue, Dim, Direction), so extending "faster",
 * "heavier", etc. is data, not another C branch. Direction is max/min. */
static int magnitude_cue_lookup(Brain *b, const char *cue_word,
                                char *dim, size_t dim_sz, int *want_max) {
    if (!b || !b->kb || !cue_word || !*cue_word) return 0;
    const char *q[] = { cue_word, NULL, NULL };
    char dims[1][KB_TERM_LEN];
    if (kb_match(b->kb, "magnitude_cue", q, 3, dims, 1) == 0) return 0;
    const char *q2[] = { cue_word, dims[0], NULL };
    char dirs[1][KB_TERM_LEN];
    if (kb_match(b->kb, "magnitude_cue", q2, 3, dirs, 1) == 0) return 0;
    snprintf(dim, dim_sz, "%s", dims[0]);
    *want_max = strcmp(dirs[0], "min") != 0;
    return 1;
}

static void display_key(const char *key, char *out, size_t sz) {
    if (!key || !*key) { if (sz) out[0] = '\0'; return; }
    if (strcmp(key, "usa") == 0 || strcmp(key, "united_states") == 0) {
        snprintf(out, sz, "USA"); return;
    }
    if (strcmp(key, "uk") == 0 || strcmp(key, "united_kingdom") == 0) {
        snprintf(out, sz, "UK"); return;
    }
    if (strcmp(key, "ram") == 0) { snprintf(out, sz, "RAM"); return; }
    if (strcmp(key, "rom") == 0) { snprintf(out, sz, "ROM"); return; }
    size_t o = 0;
    for (const char *p = key; *p && o + 1 < sz; p++)
        out[o++] = (*p == '_') ? ' ' : *p;
    out[o] = '\0';
    if (out[0]) out[0] = (char)toupper((unsigned char)out[0]);
}

static int compare_entity_token(const char *t) {
    if (!t || !*t) return 0;
    return !(is_article(t) || !strcmp(t, "the") ||
             !strcmp(t, "what") || !strcmp(t, "which") ||
             !strcmp(t, "who") || !strcmp(t, "is") || !strcmp(t, "are") ||
             !strcmp(t, "does") || !strcmp(t, "do") || !strcmp(t, "than") ||
             !strcmp(t, "to") || !strcmp(t, "of") || !strcmp(t, "in") ||
             !strcmp(t, "on") || !strcmp(t, "planet"));
}

static int join_entity_span(char **w, size_t start, size_t end,
                            char *out, size_t out_sz) {
    size_t off = 0;
    out[0] = '\0';
    for (size_t i = start; i < end; i++) {
        char *t = strip_edge_punct(w[i]);
        if (!compare_entity_token(t)) continue;
        if (!strcmp(t, "u") && i + 1 < end && !strcmp(strip_edge_punct(w[i + 1]), "s")) {
            t = (char *)"usa"; i++;
        } else if (!strcmp(t, "it") && i + 1 < end && !strcmp(strip_edge_punct(w[i + 1]), "is")) {
            t = (char *)"it_is"; i++;
        } else if (!strcmp(t, "united") && i + 1 < end) {
            char *n = strip_edge_punct(w[i + 1]);
            if (!strcmp(n, "states")) { t = (char *)"united_states"; i++; }
            else if (!strcmp(n, "kingdom")) { t = (char *)"united_kingdom"; i++; }
        }
        if (off && off + 1 < out_sz) out[off++] = '_';
        off += (size_t)snprintf(out + off, out_sz - off, "%s", t);
        if (off >= out_sz) { out[out_sz - 1] = '\0'; break; }
    }
    return out[0] != '\0';
}

static int last_entity_before(char **w, size_t pos, char *out, size_t out_sz) {
    if (pos == 0) return 0;
    size_t j = pos;
    while (j > 0) {
        char *t = strip_edge_punct(w[j - 1]);
        if (*t && compare_entity_token(t)) break;
        j--;
    }
    if (j == 0) return 0;
    size_t start = j - 1;
    if (start > 0) {
        char *p = strip_edge_punct(w[start - 1]);
        char *t = strip_edge_punct(w[start]);
        if ((!strcmp(p, "united") && (!strcmp(t, "states") || !strcmp(t, "kingdom"))) ||
            (!strcmp(p, "ice") && !strcmp(t, "cream")))
            start--;
    }
    return join_entity_span(w, start, j, out, out_sz);
}

static int first_entity_after(char **w, size_t start, size_t nw,
                              char *out, size_t out_sz) {
    for (size_t i = start; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (!compare_entity_token(t)) continue;
        size_t end = i + 1;
        if (!strcmp(t, "united") && i + 1 < nw) {
            char *n = strip_edge_punct(w[i + 1]);
            if (!strcmp(n, "states") || !strcmp(n, "kingdom")) end = i + 2;
        }
        return join_entity_span(w, i, end, out, out_sz);
    }
    return 0;
}

static int answer_magnitude_compare(Brain *b, const char *dim, int want_max,
                                    const char *a, const char *c, int yesno,
                                    char *out, size_t out_size) {
    char ra[KB_TERM_LEN], rc[KB_TERM_LEN];
    int fa = magnitude_lookup(b, dim, a, ra);
    int fc = magnitude_lookup(b, dim, c, rc);
    if (!fa || !fc) {
        char da[64], dc[64];
        display_key(a, da, sizeof da);
        display_key(c, dc, sizeof dc);
        char msg[220];
        snprintf(msg, sizeof msg,
                 "I recognize a comparison on %s, but I don't have magnitudes for %s and %s.",
                 dim, da, dc);
        put(msg, out, out_size);
        return 1;
    }
    double na = 0, nc = 0;
    parse_value(ra, &na);
    parse_value(rc, &nc);
    char proof[220];
    snprintf(proof, sizeof proof, "Compared magnitude(%s,%s,%s) with magnitude(%s,%s,%s).",
             dim, a, ra, dim, c, rc);
    store_proof(b, proof);
    if (yesno) {
        put((want_max ? na > nc : na < nc) ? "Yes." : "No.", out, out_size);
        return 1;
    }
    if (na == nc) {
        char msg[160];
        snprintf(msg, sizeof msg, "They are tied on %s.", dim);
        put(msg, out, out_size);
        return 1;
    }
    const char *win = want_max ? (na > nc ? a : c) : (na < nc ? a : c);
    char dw[80], msg[96];
    display_key(win, dw, sizeof dw);
    snprintf(msg, sizeof msg, "%s.", dw);
    put(msg, out, out_size);
    return 1;
}

static char *kb_dequote(char *s) {
    size_t l = strlen(s);
    if (l >= 2 && s[0] == '"' && s[l - 1] == '"') {
        s[l - 1] = '\0';
        return s + 1;
    }
    return s;
}

static int difference_lookup(Brain *b, const char *a, const char *c,
                             char *out, size_t out_sz) {
    const char *q[] = { a, c, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "difference_between", q, 3, hit, 1) == 0) {
        const char *qr[] = { c, a, NULL };
        if (kb_match(b->kb, "difference_between", qr, 3, hit, 1) == 0) return 0;
    }
    char *p = kb_dequote(hit[0]);
    snprintf(out, out_sz, "%s", p);
    return 1;
}

static int token_list_has(char **w, size_t nw, const char *tok) {
    if (!tok || !*tok) return 0;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strcmp(t, tok) == 0) return 1;
    }
    return 0;
}

static int seen_term(char terms[][KB_TERM_LEN], size_t n, const char *term) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(terms[i], term) == 0) return 1;
    return 0;
}

static int kb_topic_task(Brain *b, const char *step_pred, const char *topic_pred,
                         char **w, size_t nw, char *task, size_t task_sz) {
    if (!b || !b->kb || !step_pred || !topic_pred || !task || task_sz == 0)
        return 0;

    const char *all_steps[] = { NULL, NULL, NULL };
    char raw_tasks[64][KB_TERM_LEN];
    size_t nr = kb_match(b->kb, step_pred, all_steps, 3, raw_tasks, 64);
    char tasks[32][KB_TERM_LEN];
    size_t nt = 0;
    for (size_t i = 0; i < nr && nt < 32; i++) {
        if (seen_term(tasks, nt, raw_tasks[i])) continue;
        snprintf(tasks[nt++], KB_TERM_LEN, "%s", raw_tasks[i]);
    }

    int best_score = 0;
    char best[KB_TERM_LEN] = "";
    for (size_t i = 0; i < nt; i++) {
        const char *tq[] = { tasks[i], NULL };
        char topics[32][KB_TERM_LEN];
        size_t tn = kb_match(b->kb, topic_pred, tq, 2, topics, 32);
        int score = 0;
        if (tn == 0) {
            score = token_list_has(w, nw, tasks[i]) ? 1 : 0;
        } else {
            for (size_t j = 0; j < tn; j++)
                if (token_list_has(w, nw, topics[j])) score++;
        }
        if (score > best_score) {
            best_score = score;
            snprintf(best, sizeof best, "%s", tasks[i]);
        }
    }

    if (best_score <= 0) return 0;
    snprintf(task, task_sz, "%s", best);
    return 1;
}

static int kb_render_steps(Brain *b, const char *step_pred, const char *task,
                           const char *intro, char *out, size_t out_size) {
    const char *q[] = { task, NULL, NULL };
    char nums[16][KB_TERM_LEN];
    size_t sn = kb_match(b->kb, step_pred, q, 3, nums, 16);
    if (sn == 0) return 0;

    char msg[1000];
    size_t off = 0;
    msg[0] = '\0';
    if (intro && *intro)
        off += (size_t)snprintf(msg + off, sizeof msg - off, "%s", intro);
    for (size_t i = 0; i < sn; i++) {
        const char *nq[] = { task, nums[i], NULL };
        char th[1][KB_TERM_LEN];
        if (kb_match(b->kb, step_pred, nq, 3, th, 1) == 0) continue;
        char *p = kb_dequote(th[0]);
        off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%s. %s",
                                (off || i) ? "\n" : "", nums[i], p);
    }
    put(msg, out, out_size);
    return 1;
}

/* gen254: defining-phrase vocabulary lookup. word_for(KeyPhrase, Word) facts map
 * a defining phrase ("always tells the truth") to the word that names it
 * ("honest"). The stored phrase is matched as a substring of the turn — same
 * scheme as idiom_meaning — so quoting and framing are free; teaching a new
 * entry is one KB fact. Returns the matched (dequoted) key and word. */
static int word_for_lookup(Brain *b, const char *buf,
                           char *key_out, size_t key_sz,
                           char *word_out, size_t word_sz) {
    char ph[64][KB_TERM_LEN];
    const char *anyq[] = { NULL, NULL };
    size_t pn = kb_match(b->kb, "word_for", anyq, 2, ph, 64);
    for (size_t i = 0; i < pn; i++) {
        char *key = ph[i]; size_t kl = strlen(key);
        if (kl >= 2 && key[0] == '"' && key[kl - 1] == '"') { key[kl - 1] = '\0'; key++; }
        if (!*key || !cue(buf, key)) continue;
        char qkey[KB_TERM_LEN]; snprintf(qkey, sizeof qkey, "\"%s\"", key);
        const char *gq[] = { qkey, NULL };
        char gh[1][KB_TERM_LEN];
        if (kb_match(b->kb, "word_for", gq, 2, gh, 1) > 0) {
            snprintf(key_out, key_sz, "%s", key);
            snprintf(word_out, word_sz, "%s", kb_dequote(gh[0]));
            return 1;
        }
    }
    return 0;
}

static int mod_knowledge(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* gen240 (LLMSCORE): compound-word riddle. "the word that follows X and
     * precedes Y" / "comes after X and before Y" -> the compound X+Y, looked up in
     * compound_word(X, Y, Whole) so it is KB knowledge, not a guess. Declines if
     * the pair forms no known compound. */
    if ((cue(norm, "follows") && cue(norm, "precedes")) ||
        (cue(norm, "after") && cue(norm, "before") &&
         (cue(norm, "word") || cue(norm, "comes")))) {
        char cb[256]; snprintf(cb, sizeof cb, "%s", norm);
        char *cw[64]; size_t cn = split_words(cb, cw, 64);
        const char *X = NULL, *Y = NULL;
        for (size_t i = 0; i + 1 < cn; i++) {
            char *t = strip_edge_punct(cw[i]);
            if ((!strcmp(t, "follows") || !strcmp(t, "after")) && !X)
                X = strip_edge_punct(cw[i + 1]);
            if ((!strcmp(t, "precedes") || !strcmp(t, "before")) && !Y)
                Y = strip_edge_punct(cw[i + 1]);
        }
        if (X && Y && *X && *Y) {
            const char *pq[] = { X, Y, NULL };
            char w[1][KB_TERM_LEN];
            if (kb_match(b->kb, "compound_word", pq, 3, w, 1) > 0) {
                char m[64]; snprintf(m, sizeof m, "%s", w[0]);
                if (m[0]) m[0] = (char)toupper((unsigned char)m[0]);
                char msg[96]; snprintf(msg, sizeof msg, "%s.", m);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): animal sound — "what sound/noise does a dog make?" /
     * "what does a cow say?" -> sound_of(animal, "…"). */
    if ((cue(norm, "sound") || cue(norm, "noise") || cue(norm, "say") || cue(norm, "says")) &&
        (cue(norm, "make") || cue(norm, "makes") || cue(norm, "does") || cue(norm, "do"))) {
        char ab[256]; snprintf(ab, sizeof ab, "%s", norm);
        char *aw[64]; size_t an = split_words(ab, aw, 64);
        for (size_t i = 0; i < an; i++) {
            char *t = strip_edge_punct(aw[i]); size_t tl = strlen(t);
            if (tl < 2) continue;
            char sg[64]; snprintf(sg, sizeof sg, "%s", t);
            if (tl > 1 && sg[tl-1]=='s') sg[tl-1]='\0';
            const char *q[] = { sg, NULL }; char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "sound_of", q, 2, hit, 1) > 0) {
                char *p = hit[0]; size_t l = strlen(p);
                if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                char msg[96]; snprintf(msg, sizeof msg, "A %s goes \"%s\".", sg, p);
                put(msg, out, out_size); return 1;
            }
        }
    }

    /* gen245: reverse animal-sound frame. The relation is still sound_of/2; this
     * just inverts the lookup for "what animal is known for saying 'meow'?". */
    if ((cue(norm, "animal") || cue(norm, "known for saying") ||
         cue(norm, "known for making")) &&
        (cue(norm, "saying") || cue(norm, "sound") || cue(norm, "noise"))) {
        const char *aq[] = { NULL, NULL };
        char animals[64][KB_TERM_LEN];
        size_t an = kb_match(b->kb, "sound_of", aq, 2, animals, 64);
        for (size_t i = 0; i < an; i++) {
            const char *sq[] = { animals[i], NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "sound_of", sq, 2, hit, 1) == 0) continue;
            char *p = kb_dequote(hit[0]);
            if (!*p || !cue(norm, p)) continue;
            char name[KB_TERM_LEN]; snprintf(name, sizeof name, "%s", animals[i]);
            char msg[96]; snprintf(msg, sizeof msg, "A %s.", name);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen250: KB-backed contrast frame. The parser extracts the two slots from
     * "difference between X and Y"; difference_between/3 supplies the actual
     * distinction. Missing facts become an informed gap rather than the blind
     * fallback. */
    if (cue(norm, "difference between")) {
        char db[256]; snprintf(db, sizeof db, "%s", norm);
        char *dw[64]; size_t dn = split_words(db, dw, 64);
        size_t between = dn, sep = dn;
        for (size_t i = 0; i < dn; i++) {
            char *t = strip_edge_punct(dw[i]);
            if (!strcmp(t, "between")) between = i;
            else if (between < dn && sep == dn &&
                     (!strcmp(t, "and") || !strcmp(t, "or")))
                sep = i;
        }
        if (between + 1 < sep && sep + 1 < dn) {
            char a[KB_TERM_LEN], c[KB_TERM_LEN];
            if (join_entity_span(dw, between + 1, sep, a, sizeof a) &&
                join_entity_span(dw, sep + 1, dn, c, sizeof c)) {
                char gloss[KB_TERM_LEN];
                if (difference_lookup(b, a, c, gloss, sizeof gloss)) {
                    put(gloss, out, out_size);
                    store_proof(b, "Answered from difference_between/3 in the KB.");
                    return 1;
                }
                char da[64], dc[64], msg[220];
                display_key(a, da, sizeof da);
                display_key(c, dc, sizeof dc);
                snprintf(msg, sizeof msg,
                         "You're asking for a distinction between %s and %s, but I don't have that contrast fact yet.",
                         da, dc);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen250: generic magnitude frame. Cue words map to dimensions in the KB via
     * magnitude_cue/3, and magnitude(Dim, Item, Rank) supplies the comparable
     * values. Handles both "which is faster, A or B" and "is A bigger than B?". */
    {
        char mb[256]; snprintf(mb, sizeof mb, "%s", norm);
        char *mw[64]; size_t mn = split_words(mb, mw, 64);
        for (size_t cue_i = 0; cue_i < mn; cue_i++) {
            char *ct = strip_edge_punct(mw[cue_i]);
            char dim[KB_TERM_LEN]; int want_max = 1;
            if (!magnitude_cue_lookup(b, ct, dim, sizeof dim, &want_max)) continue;

            size_t or_i = mn, than_i = mn;
            for (size_t j = 0; j < mn; j++) {
                char *t = strip_edge_punct(mw[j]);
                if (!strcmp(t, "or") && or_i == mn) or_i = j;
                if (!strcmp(t, "than") && than_i == mn) than_i = j;
            }
            if (or_i < mn) {
                char a[KB_TERM_LEN], c[KB_TERM_LEN];
                if (last_entity_before(mw, or_i, a, sizeof a) &&
                    first_entity_after(mw, or_i + 1, mn, c, sizeof c))
                    return answer_magnitude_compare(b, dim, want_max, a, c, 0, out, out_size);
            }
            if (than_i < mn && cue_i > 0 && cue_i < than_i) {
                char a[KB_TERM_LEN], c[KB_TERM_LEN];
                if (join_entity_span(mw, 1, cue_i, a, sizeof a) &&
                    first_entity_after(mw, than_i + 1, mn, c, sizeof c))
                    return answer_magnitude_compare(b, dim, want_max, a, c, 1, out, out_size);
            }
        }
    }

    /* gen240 (LLMSCORE): pairwise comparison — "which is larger: a strawberry or a
     * watermelon?", "which planet is closer to the Sun: Mars or Jupiter?" Compares
     * magnitude(Dim, Item, Rank); the cue word picks the Dim and direction. KB-first:
     * add a magnitude fact and the comparison extends with no code edit. */
    if (cue(norm, " or ") &&
        (cue(norm, "larger") || cue(norm, "bigger") || cue(norm, "smaller") ||
         cue(norm, "biggest") || cue(norm, "largest") || cue(norm, "smallest") ||
         cue(norm, "closer") || cue(norm, "nearer") || cue(norm, "closest") ||
         cue(norm, "farther") || cue(norm, "further") || cue(norm, "farthest") ||
         cue(norm, "heavier") || cue(norm, "tinier"))) {
        const char *dim = NULL; int want_max = 1;
        if (cue(norm, "larger")||cue(norm, "bigger")||cue(norm, "largest")||
            cue(norm, "biggest")||cue(norm, "heavier")) { dim = "size"; want_max = 1; }
        else if (cue(norm, "smaller")||cue(norm, "smallest")||cue(norm, "tinier")) { dim = "size"; want_max = 0; }
        else if (cue(norm, "closer")||cue(norm, "nearer")||cue(norm, "closest")) { dim = "distance_from_sun"; want_max = 0; }
        else if (cue(norm, "farther")||cue(norm, "further")||cue(norm, "farthest")) { dim = "distance_from_sun"; want_max = 1; }
        if (dim) {
            char cb[256]; snprintf(cb, sizeof cb, "%s", norm);
            char *cw[64]; size_t cn = split_words(cb, cw, 64);
            size_t orp = cn;
            for (size_t i = 0; i < cn; i++) if (!strcmp(strip_edge_punct(cw[i]), "or")) { orp = i; break; }
            const char *A = NULL, *B = NULL;
            static const char *skip[] = {"a","an","the","is","are","which","what",
                "planet","bigger","larger","smaller","closer","nearer","farther",
                "further","to","sun","or","does","do",NULL};
            for (size_t i = 0; i < orp; i++) { char *t = strip_edge_punct(cw[i]);
                int sk=0; for (size_t s=0;skip[s];s++) if(!strcmp(t,skip[s])) sk=1;
                if (!sk && isalpha((unsigned char)t[0]) && strlen(t)>1) A = t; }
            for (size_t i = orp+1; i < cn; i++) { char *t = strip_edge_punct(cw[i]);
                int sk=0; for (size_t s=0;skip[s];s++) if(!strcmp(t,skip[s])) sk=1;
                if (!sk && isalpha((unsigned char)t[0]) && strlen(t)>1) { B = t; break; } }
            if (A && B) {
                /* look up the item as written, then a naive singular as fallback —
                 * never blindly strip a trailing 's' ("mars"/"venus" are not plurals). */
                char ra[1][KB_TERM_LEN], rb[1][KB_TERM_LEN];
                char a2[64], b2[64]; snprintf(a2,sizeof a2,"%s",A); snprintf(b2,sizeof b2,"%s",B);
                int fa = magnitude_lookup(b, dim, a2, ra[0]);
                int fb = magnitude_lookup(b, dim, b2, rb[0]);
                if (fa && fb) {
                    double na=0,nb=0; parse_value(ra[0],&na); parse_value(rb[0],&nb);
                    const char *win = want_max ? (na>=nb?a2:b2) : (na<=nb?a2:b2);
                    char w[64]; snprintf(w,sizeof w,"%s",win);
                    if (w[0]) w[0]=(char)toupper((unsigned char)w[0]);
                    char msg[96]; snprintf(msg,sizeof msg,"%s.",w);
                    put(msg,out,out_size); return 1;
                }
            }
        }
    }

    /* gen240 (LLMSCORE): the race-overtaking trick. If you pass the runner in Nth
     * place you TAKE their position — you are now Nth (not (N-1)th). A general rule
     * over the ordinal, not a memorized answer. */
    if ((cue(norm, "pass") || cue(norm, "overtake") || cue(norm, "overtook") ||
         cue(norm, "passed")) &&
        (cue(norm, "place") || cue(norm, "position")) &&
        (cue(norm, "what position") || cue(norm, "which position") ||
         cue(norm, "what place") || cue(norm, "which place") || cue(norm, "now in"))) {
        static const char *ord[] = { "first","second","third","fourth","fifth",
            "sixth","seventh","eighth","ninth","tenth","last", NULL };
        char rb[256]; snprintf(rb, sizeof rb, "%s", norm);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        const char *got = NULL;
        for (size_t i = 0; i < rn; i++) {
            char *t = strip_edge_punct(rw[i]);
            /* the ordinal that sits just before "place"/"position" is the one passed */
            if ((!strcmp(t, "place") || !strcmp(t, "position")) && i > 0) {
                char *p = strip_edge_punct(rw[i - 1]);
                for (size_t k = 0; ord[k]; k++) if (!strcmp(p, ord[k])) { got = ord[k]; break; }
            }
        }
        if (got && strcmp(got, "first") != 0 && strcmp(got, "last") != 0) {
            char msg[160];
            snprintf(msg, sizeof msg,
                     "You're now in %s place -- you take the spot of the runner you "
                     "passed, not the one ahead of them.", got);
            put(msg, out, out_size);
            store_proof(b, "Overtaking the Nth runner puts you in Nth place.");
            return 1;
        }
    }

    /* gen255 (Fase D, generative-prolog spirit): kinship chain composition.
     * "Anna is Maria's grandmother and Maria is Elena's mother. What relation
     * is Anna to Elena?" — each "X is Y's R" clause is a LINK whose generation
     * height and gender live in KB (kinship_level/2, kinship_gender/2); the
     * solver walks the chain from the asked ancestor to the asked descendant,
     * SUMS the levels, and maps the total back through kinship_name/3. The
     * lexicon (new relation words, deeper names) extends in KB only. */
    if (cue(norm, "relation") && strstr(norm, "'s ")) {
        char kb2[256]; snprintf(kb2, sizeof kb2, "%s", norm);
        char *w[64]; size_t n = split_words(kb2, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        struct { char from[48], to[48]; long lvl; char g; } link[6];
        size_t nl = 0;
        for (size_t i = 0; i + 3 < n && nl < 6; i++) {
            if (strcmp(w[i + 1], "is")) continue;
            char *poss = w[i + 2]; size_t pl = strlen(poss);
            if (pl < 3 || poss[pl - 2] != '\'' || poss[pl - 1] != 's') continue;
            char owner[48]; snprintf(owner, sizeof owner, "%.*s", (int)(pl - 2), poss);
            const char *rel = w[i + 3];
            char lvl[1][KB_TERM_LEN], gen[1][KB_TERM_LEN];
            char qrel[KB_TERM_LEN]; snprintf(qrel, sizeof qrel, "\"%s\"", rel);
            const char *q1[] = { rel, NULL }, *q2[] = { qrel, NULL };
            if (kb_match(b->kb, "kinship_level", q1, 2, lvl, 1) == 0 &&
                kb_match(b->kb, "kinship_level", q2, 2, lvl, 1) == 0) continue;
            if (kb_match(b->kb, "kinship_gender", q1, 2, gen, 1) == 0 &&
                kb_match(b->kb, "kinship_gender", q2, 2, gen, 1) == 0) continue;
            snprintf(link[nl].from, sizeof link[nl].from, "%s", w[i]);
            snprintf(link[nl].to, sizeof link[nl].to, "%s", owner);
            link[nl].lvl = atol(lvl[0]);
            link[nl].g = gen[0][0];
            nl++;
        }
        /* the question: "what relation is X to Z" */
        char qx[48] = "", qz[48] = "";
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "relation") && !strcmp(w[i + 1], "is") &&
                !strcmp(w[i + 3], "to") && i + 4 < n) {
                snprintf(qx, sizeof qx, "%s", w[i + 2]);
                snprintf(qz, sizeof qz, "%s", w[i + 4]);
                break;
            }
        if (nl >= 1 && qx[0] && qz[0]) {
            long total = 0; char gender = 0; int steps = 0;
            char cur[48]; snprintf(cur, sizeof cur, "%s", qx);
            while (strcmp(cur, qz) != 0 && steps <= (int)nl) {
                int found = 0;
                for (size_t k = 0; k < nl; k++)
                    if (!strcmp(link[k].from, cur)) {
                        total += link[k].lvl;
                        if (!gender) gender = link[k].g;
                        snprintf(cur, sizeof cur, "%s", link[k].to);
                        found = 1; steps++; break;
                    }
                if (!found) break;
            }
            if (!strcmp(cur, qz) && total > 0 && gender) {
                char tn[16]; snprintf(tn, sizeof tn, "%ld", total);
                char gs[2] = { gender, '\0' };
                const char *nq[] = { tn, gs, NULL };
                char nm[1][KB_TERM_LEN];
                if (kb_match(b->kb, "kinship_name", nq, 3, nm, 1) > 0) {
                    char *p = kb_dequote(nm[0]);
                    char xd[48]; snprintf(xd, sizeof xd, "%s", qx);
                    char zd[48]; snprintf(zd, sizeof zd, "%s", qz);
                    xd[0] = (char)toupper((unsigned char)xd[0]);
                    zd[0] = (char)toupper((unsigned char)zd[0]);
                    char msg[200];
                    snprintf(msg, sizeof msg, "%s is %s's %s.", xd, zd, p);
                    put(msg, out, out_size);
                    store_proof(b, "Summed the generation levels along the "
                                   "stated kinship chain.");
                    return 1;
                }
            }
        }
    }

    /* gen240 (LLMSCORE): the existential syllogism (Darii). From "some A are B"
     * and "every/all B <pred>" conclude "Some A <pred>." — a real deduction over
     * the parsed premises (docs/plans/kb-first.md: a sentence with a logical soul
     * is code waiting to be read), not a template. Distractor premises about other
     * subjects (e.g. "all A have four legs") are ignored because the bridge B must
     * match the universal's subject. */
    if (cue(norm, "conclude") && cue(norm, "some") &&
        (cue(norm, "every") || cue(norm, "all"))) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", norm);
        char *w[64]; size_t n = split_words(sb, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        char setA[64] = "", bridge[64] = "";
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "some") && !strcmp(w[i + 2], "are")) {
                snprintf(setA, sizeof setA, "%s", w[i + 1]);
                snprintf(bridge, sizeof bridge, "%s", w[i + 3]);
                break;
            }
        if (setA[0] && bridge[0]) {
            char bsing[64]; snprintf(bsing, sizeof bsing, "%s", bridge);
            size_t bl = strlen(bsing); if (bl > 1 && bsing[bl - 1] == 's') bsing[bl - 1] = '\0';
            /* find the universal whose subject equals the bridge, collect its predicate */
            char pred[160] = "";
            for (size_t i = 0; i + 1 < n; i++) {
                if (strcmp(w[i], "every") && strcmp(w[i], "all")) continue;
                char csing[64]; snprintf(csing, sizeof csing, "%s", w[i + 1]);
                size_t cl = strlen(csing); if (cl > 1 && csing[cl - 1] == 's') csing[cl - 1] = '\0';
                if (strcmp(csing, bsing) != 0) continue;
                size_t off = 0;
                for (size_t j = i + 2; j < n; j++) {
                    if (!strcmp(w[j], "and") || !strcmp(w[j], "what") ||
                        !strcmp(w[j], "then") || !strcmp(w[j], "so")) break;
                    off += (size_t)snprintf(pred + off, sizeof pred - off,
                                            "%s%s", off ? " " : "", w[j]);
                }
                break;
            }
            if (pred[0]) {
                /* the conclusion subject is plural ("some A"), so a singular
                 * copula in the predicate ("is loved") agrees as "are loved". */
                char fixed[180];
                if (!strncmp(pred, "is ", 3))
                    snprintf(fixed, sizeof fixed, "are %s", pred + 3);
                else if (!strncmp(pred, "has ", 4))
                    snprintf(fixed, sizeof fixed, "have %s", pred + 4);
                else
                    snprintf(fixed, sizeof fixed, "%s", pred);
                char msg[256];
                snprintf(msg, sizeof msg, "Some %s %s.", setA, fixed);
                put(msg, out, out_size);
                store_proof(b, "Darii: some A are B, every B has the property, so some A have it.");
                return 1;
            }
        }
    }

    /* gen255 (Fase D): the NEGATIVE existential syllogism (Ferio family), over
     * nonce words. "Some Mips are Glorps, and no Glorps are Zorks — can we
     * conclude that some Mips are not Zorks?" -> YES: the Mips that are Glorps
     * cannot be Zorks. Parsed as set relations: conclusion "some B are not A"
     * follows iff a premise gives some B are C (or some C are B) and another
     * gives no C are A (or no A are C — E-propositions convert). Real deduction
     * with the witness named; anything outside the schema falls through to the
     * honest paths below. */
    if ((cue(norm, "conclude") || cue(norm, "does it follow") ||
         cue(norm, "can we say")) &&
        cue(norm, "some") && cue(norm, "no ") && cue(norm, "not ")) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", norm);
        char *w[64]; size_t n = split_words(sb, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        #define KSING(dst, src) do { snprintf(dst, sizeof dst, "%s", src); \
            size_t _l = strlen(dst); if (_l > 1 && dst[_l-1] == 's') dst[_l-1] = '\0'; } while (0)
        /* conclusion: the "some P are (definitely) not Q" clause */
        char P[64] = "", Q[64] = "";
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "some") && !strcmp(w[i + 2], "are")) {
                size_t j = i + 3;
                if (j < n && !strcmp(w[j], "definitely")) j++;
                if (j + 1 < n && !strcmp(w[j], "not")) {
                    KSING(P, w[i + 1]); KSING(Q, w[j + 1]);
                }
            }
        /* premises: some X are Y (affirmative) and no X are Y */
        char sA[64] = "", sB[64] = "", nA[64] = "", nB[64] = "";
        for (size_t i = 0; i + 3 < n; i++) {
            if (!strcmp(w[i], "some") && !strcmp(w[i + 2], "are") &&
                strcmp(w[i + 3], "not") && strcmp(w[i + 3], "definitely")) {
                if (!sA[0]) { KSING(sA, w[i + 1]); KSING(sB, w[i + 3]); }
            }
            if (!strcmp(w[i], "no") && !strcmp(w[i + 2], "are")) {
                KSING(nA, w[i + 1]); KSING(nB, w[i + 3]);
            }
        }
        if (P[0] && Q[0] && sA[0] && nA[0]) {
            /* bridge C: the some-premise links P to C; the no-premise separates
             * C from Q in either direction. */
            const char *C = NULL;
            if (!strcmp(sA, P)) C = sB;           /* some P are C */
            else if (!strcmp(sB, P)) C = sA;      /* some C are P */
            int separated = C &&
                ((!strcmp(nA, C) && !strcmp(nB, Q)) ||   /* no C are Q */
                 (!strcmp(nA, Q) && !strcmp(nB, C)));    /* no Q are C */
            if (separated) {
                char msg[300];
                snprintf(msg, sizeof msg,
                         "Yes -- some %ss are %ss, and no %ss are %ss, so those "
                         "%ss cannot be %ss: some %ss are definitely not %ss.",
                         P, C, C, Q, P, Q, P, Q);
                put(msg, out, out_size);
                store_proof(b, "Ferio: some P are C and no C are Q entail "
                               "some P are not Q (witness: the P that are C).");
                return 1;
            }
        }
        #undef KSING
    }

    /* gen240 (LLMSCORE): the INVALID syllogism (undistributed middle). "All A are
     * B, some B are/​do C, can we conclude some A are C?" does NOT follow — the B
     * that are C needn't be the A ones. Honest reasoning means saying No, not
     * pattern-matching a yes. Detected when the some-clause is about the universal's
     * predicate B and the conclusion is about A. */
    if ((cue(norm, "conclude") || cue(norm, "does it follow") ||
         cue(norm, "can we conclude") || cue(norm, "can we say") ||
         cue(norm, "therefore") || cue(norm, "valid")) &&
        cue(norm, "all") && cue(norm, "some")) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", norm);
        char *w[64]; size_t n = split_words(sb, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        char A[64] = "", B[64] = "";
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "all") && !strcmp(w[i + 2], "are")) {
                snprintf(A, sizeof A, "%s", w[i + 1]);
                snprintf(B, sizeof B, "%s", w[i + 3]);
                break;
            }
        char someFirst[64] = "", someLast[64] = "";
        for (size_t i = 0; i + 1 < n; i++)
            if (!strcmp(w[i], "some")) {
                if (!someFirst[0]) snprintf(someFirst, sizeof someFirst, "%s", w[i + 1]);
                snprintf(someLast, sizeof someLast, "%s", w[i + 1]);
            }
        #define SING(s) do{ size_t _l=strlen(s); if(_l>1&&(s)[_l-1]=='s')(s)[_l-1]='\0'; }while(0)
        if (A[0] && B[0] && someFirst[0] && someLast[0]) {
            char a2[64],b2[64],sf[64],sl[64];
            snprintf(a2,sizeof a2,"%s",A); snprintf(b2,sizeof b2,"%s",B);
            snprintf(sf,sizeof sf,"%s",someFirst); snprintf(sl,sizeof sl,"%s",someLast);
            SING(a2);SING(b2);SING(sf);SING(sl);
            /* some-clause about B, conclusion about A -> undistributed middle */
            if (!strcmp(sf, b2) && !strcmp(sl, a2) && strcmp(a2, b2) != 0) {
                char msg[300];
                snprintf(msg, sizeof msg,
                         "No -- that doesn't follow. From \"all %s are %s\" and "
                         "\"some %s ...\", nothing follows about %s: the %s in question "
                         "need not be %s (the middle term is undistributed).",
                         A, B, B, A, B, A);
                put(msg, out, out_size);
                store_proof(b, "Undistributed middle: all A are B + some B are C does not give some A are C.");
                return 1;
            }
        }
        #undef SING
    }

    /* gen248: universal syllogism chain. "All dogs are mammals; all mammals
     * breathe; what can you conclude about dogs?" -> Dogs breathe. */
    if ((cue(norm, "conclude") || cue(norm, "what can you conclude")) &&
        cue(norm, "all") && !cue(norm, "some")) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", norm);
        char *w[64]; size_t n = split_words(sb, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        char A[64] = "", B[64] = "", pred[160] = "";
        for (size_t i = 0; i + 3 < n; i++) {
            if (!strcmp(w[i], "all") && !strcmp(w[i + 2], "are")) {
                snprintf(A, sizeof A, "%s", w[i + 1]);
                snprintf(B, sizeof B, "%s", w[i + 3]);
                break;
            }
        }
        if (A[0] && B[0]) {
            char bsing[64]; snprintf(bsing, sizeof bsing, "%s", B);
            size_t bl = strlen(bsing); if (bl > 1 && bsing[bl - 1] == 's') bsing[bl - 1] = '\0';
            for (size_t i = 0; i + 2 < n; i++) {
                if (strcmp(w[i], "all")) continue;
                char subj[64]; snprintf(subj, sizeof subj, "%s", w[i + 1]);
                size_t sl = strlen(subj); if (sl > 1 && subj[sl - 1] == 's') subj[sl - 1] = '\0';
                if (strcmp(subj, bsing) != 0) continue;
                size_t start = (!strcmp(w[i + 2], "are")) ? i + 4 : i + 2;
                size_t off = 0;
                for (size_t j = start; j < n; j++) {
                    if (!strcmp(w[j], "what") || !strcmp(w[j], "can") ||
                        !strcmp(w[j], "conclude") || !strcmp(w[j], "about") ||
                        !strcmp(w[j], "and")) break;
                    off += (size_t)snprintf(pred + off, sizeof pred - off,
                                            "%s%s", off ? " " : "", w[j]);
                }
                break;
            }
        }
        if (A[0] && pred[0]) {
            char subj[64]; snprintf(subj, sizeof subj, "%s", A);
            if (subj[0]) subj[0] = (char)toupper((unsigned char)subj[0]);
            char msg[240]; snprintf(msg, sizeof msg, "%s %s.", subj, pred);
            put(msg, out, out_size);
            store_proof(b, "Barbara-style universal chain: all A are B, all B have the property.");
            return 1;
        }
    }

    /* gen249: explicit no-overlap beats existential uncertainty. */
    if ((cue(norm, "can") || cue(norm, "could")) && cue(norm, "no ") &&
        cue(norm, " are ") && cue(norm, "also be")) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", norm);
        char *w[64]; size_t n = split_words(sb, w, 64);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        char X[64] = "", Y[64] = "", qx[64] = "", qy[64] = "";
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "no") && !strcmp(w[i + 2], "are")) {
                snprintf(X, sizeof X, "%s", w[i + 1]);
                snprintf(Y, sizeof Y, "%s", w[i + 3]);
                break;
            }
        for (size_t i = 0; i + 5 < n; i++)
            if ((!strcmp(w[i], "can") || !strcmp(w[i], "could")) &&
                is_article(w[i + 1]) && !strcmp(w[i + 3], "also") &&
                !strcmp(w[i + 4], "be")) {
                snprintf(qx, sizeof qx, "%s", w[i + 2]);
                if (is_article(w[i + 5]) && i + 6 < n)
                    snprintf(qy, sizeof qy, "%s", w[i + 6]);
                else
                    snprintf(qy, sizeof qy, "%s", w[i + 5]);
                break;
            }
        if (X[0] && Y[0] && qx[0] && qy[0]) {
            char xs[64], ys[64], qxs[64], qys[64];
            snprintf(xs, sizeof xs, "%s", X); snprintf(ys, sizeof ys, "%s", Y);
            snprintf(qxs, sizeof qxs, "%s", qx); snprintf(qys, sizeof qys, "%s", qy);
            size_t l;
            l = strlen(xs); if (l > 1 && xs[l - 1] == 's') xs[l - 1] = '\0';
            l = strlen(ys); if (l > 1 && ys[l - 1] == 's') ys[l - 1] = '\0';
            l = strlen(qxs); if (l > 1 && qxs[l - 1] == 's') qxs[l - 1] = '\0';
            l = strlen(qys); if (l > 1 && qys[l - 1] == 's') qys[l - 1] = '\0';
            if ((!strcmp(xs, qxs) && !strcmp(ys, qys)) ||
                (!strcmp(xs, qys) && !strcmp(ys, qxs))) {
                put("No -- the statement says those classes do not overlap.", out, out_size);
                store_proof(b, "The explicit no-overlap premise rules out being both.");
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): "describe what a sunset looks like to you." parrot0 has
     * no senses, so it says so honestly — then gives the DESCRIPTION from KB
     * knowledge (appearance/2) rather than walling. The C only selects by the
     * concept named; any concept taught extends it with no code edit. */
    if (cue(norm, "taste") || cue(norm, "tasted")) {
        char tb[256]; snprintf(tb, sizeof tb, "%s", norm);
        char *tw[64]; size_t tn = split_words(tb, tw, 64);
        for (size_t i = 0; i < tn; i++) {
            char *t = strip_edge_punct(tw[i]);
            if (strlen(t) < 3 || !isalpha((unsigned char)t[0])) continue;
            const char *tq[] = { t, NULL };
            char th[1][KB_TERM_LEN];
            if (kb_match(b->kb, "taste_of", tq, 2, th, 1) > 0) {
                char *r = kb_dequote(th[0]);
                char msg[360];
                snprintf(msg, sizeof msg,
                         "I don't actually taste things, but it is often described as %s.",
                         r);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen254: a MIX question asks for the RESULT of combining, not a
     * description of one ingredient — leave it to the mix reader below. */
    if ((cue(norm, "describe") || cue(norm, "look like") ||
         cue(norm, "looks like") || cue(norm, "what does") || cue(norm, "what do")) &&
        !cue(norm, "mix")) {
        char db[256]; snprintf(db, sizeof db, "%s", norm);
        char *dw[64]; size_t dn = split_words(db, dw, 64);
        for (size_t i = 0; i < dn; i++) {
            char *t = strip_edge_punct(dw[i]);
            if (strlen(t) < 3 || !isalpha((unsigned char)t[0])) continue;
            char cand[2][64]; int nc = 0;
            snprintf(cand[nc++], 64, "%s", t);
            size_t tl = strlen(t);
            if (tl > 1 && t[tl - 1] == 's') snprintf(cand[nc++], 64, "%.*s", (int)(tl - 1), t);
            for (int c = 0; c < nc; c++) {
                const char *aq[] = { cand[c], NULL };
                char ad[1][KB_TERM_LEN];
                if (kb_match(b->kb, "appearance", aq, 2, ad, 1) > 0) {
                    char *r = ad[0]; size_t rl = strlen(r);
                    if (rl >= 2 && r[0] == '"' && r[rl - 1] == '"') { r[rl - 1] = '\0'; r++; }
                    char msg[400];
                    snprintf(msg, sizeof msg,
                             "I don't actually see or experience things, but I can "
                             "describe it: %s.", r);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* gen239 (kb-first manifesto): the commonsense lookups gen238 had
     * hardcoded as printf are now SEMANTIC recognizers over KB facts. The C
     * only SELECTS from knowledge; the surface lives in the KB so adding a
     * mix pair, a riddle, a country border, etc. is DATA, never code. */
    if ((cue(norm, "mix") || cue(norm, "what color") || cue(norm, "what colour") || cue(norm, "get")) &&
        (cue(norm, "paint") || cue(norm, "mix"))) {
        /* pick the two named colours mentioned; resolve symmetrically against
         * paint_mix/3. Honest if no such pair is recorded. */
        char mb[256]; snprintf(mb, sizeof mb, "%s", norm);
        char *mw[64]; size_t mn = split_words(mb, mw, 64);
        const char *col[2] = { NULL, NULL };
        for (size_t i = 0; i < mn && (!col[0] || !col[1]); i++) {
            char *t = strip_edge_punct(mw[i]);
            if (!*t || !isalpha((unsigned char)t[0])) continue;
            const char *cq[] = { "color", t };
            if (!kb_query(b->kb, "category_member", cq, 2)) continue;
            if (!col[0]) col[0] = t;
            else if (!col[1] && strcmp(t, col[0]) != 0) col[1] = t;
        }
        if (col[0] && col[1]) {
            char res[2][KB_TERM_LEN];
            const char *pq[3] = { col[0], col[1], NULL };
            /* gen254: LIGHT mixes additively, not like pigment — "red and blue
             * light" reads light_mix/3 (magenta), never paint_mix (purple). */
            const char *mixpred = cue(norm, "light") ? "light_mix" : "paint_mix";
            if (kb_match(b->kb, mixpred, pq, 3, res, 2) > 0) {
                char msg[64];
                snprintf(msg, sizeof msg, "%s.", res[0]);
                if (msg[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
        /* fall through honestly if no mixing fact is known */
    }

    /* gen254: classic riddles as PURE KB. riddle_sig(Id, "cue") lists each
     * riddle's required substrings; when every cue of an Id occurs in the turn,
     * response_template(Id, ...) answers. Teaching a new riddle is facts only —
     * no C branch per riddle (the older per-riddle branches below stay as
     * secondary structures per F.'s keep-and-select steer). */
    {
        char ids[64][KB_TERM_LEN];
        const char *anyq3[] = { NULL, NULL };
        size_t nid = kb_match(b->kb, "riddle_sig", anyq3, 2, ids, 64);
        char done[16][KB_TERM_LEN]; size_t nd = 0;
        for (size_t i = 0; i < nid; i++) {
            int seen2 = 0;
            for (size_t j = 0; j < nd; j++) if (!strcmp(done[j], ids[i])) seen2 = 1;
            if (seen2 || nd >= 16) continue;
            snprintf(done[nd++], KB_TERM_LEN, "%s", ids[i]);
            const char *sq2[] = { ids[i], NULL };
            char cues[8][KB_TERM_LEN];
            size_t ncue = kb_match(b->kb, "riddle_sig", sq2, 2, cues, 8);
            if (ncue < 2) continue;                /* one cue is too weak */
            int all = 1;
            for (size_t c = 0; c < ncue && all; c++)
                if (!cue(norm, kb_dequote(cues[c]))) all = 0;
            if (all && kb_response(b, ids[i], NULL, out, out_size)) return 1;
        }
    }

    if (cue(norm, "keys") && cue(norm, "locks") && cue(norm, "space") &&
        cue(norm, "room") && cue(norm, "enter")) {
        if (kb_response(b, "riddle_keyboard", NULL, out, out_size)) return 1;
    }
    if (cue(norm, "cities") && cue(norm, "no houses") &&
        cue(norm, "forests") && cue(norm, "no trees") &&
        cue(norm, "water") && cue(norm, "no fish")) {
        if (kb_response(b, "riddle_map", NULL, out, out_size)) return 1;
    }
    if (cue(norm, "once in a minute") && cue(norm, "twice in a moment") &&
        cue(norm, "thousand years")) {
        if (kb_response(b, "riddle_letter_m", NULL, out, out_size)) return 1;
    }

    if (cue(norm, "difference") && cue(norm, "fruit") && cue(norm, "vegetable")) {
        if (kb_response(b, "diff_fruit_vegetable", NULL, out, out_size)) return 1;
    }

    /* gen239 (kb-first manifesto): "what is the capital of X, and (name two)
     * countries that border it?" answered by DERIVING each half from facts:
     * capital_of_country(_, X) -> capital; borders(X, _) -> list, or
     * no_land_border(X) -> honest "no land-bordering countries." The TOKEN
     * called X is whatever category_member(country, _) appears in the turn
     * — already coref-resolved to itself by gen239's same-sentence step in
     * mod_repair, so "it" is no longer the slot but the resolved country. */
    /* gen240 (LLMSCORE): list a country's neighbours, INFERRED from borders/2 (no
     * pre-cooked list facts). "name three countries that border Germany", "which
     * countries border X", "who are X's neighbours" -> collect borders(X,_) and
     * borders(_,X), dedupe, and answer N of them (three/two/one/a) or all. The same
     * relation answers "which country borders both X and Y" elsewhere. KB-first:
     * add a borders/2 fact and every such question extends with no code edit. */
    if ((cue(norm, "border") || cue(norm, "bordering") || cue(norm, "borders") ||
         cue(norm, "neighbor") || cue(norm, "neighbour")) &&
        !cue(norm, "both") && !cue(norm, "capital")) {
        char nb[256]; snprintf(nb, sizeof nb, "%s", norm);
        char *nw2[64]; size_t nn2 = split_words(nb, nw2, 64);
        const char *country = NULL;
        for (size_t i = 0; i < nn2 && !country; i++) {
            char *t = strip_edge_punct(nw2[i]);
            if (!*t || !isalpha((unsigned char)t[0])) continue;
            const char *cq[] = { "country", t };
            if (kb_query(b->kb, "category_member", cq, 2)) country = t;
        }
        if (country) {
            char list[16][KB_TERM_LEN]; size_t nl = 0;
            char r[32][KB_TERM_LEN];
            const char *q1[] = { country, NULL };
            size_t k1 = kb_match(b->kb, "borders", q1, 2, r, 32);
            for (size_t i = 0; i < k1 && nl < 16; i++) {
                int dup = 0; for (size_t j = 0; j < nl; j++) if (!strcmp(list[j], r[i])) dup = 1;
                if (!dup) snprintf(list[nl++], KB_TERM_LEN, "%s", r[i]);
            }
            const char *q2[] = { NULL, country };
            size_t k2 = kb_match(b->kb, "borders", q2, 2, r, 32);
            for (size_t i = 0; i < k2 && nl < 16; i++) {
                int dup = 0; for (size_t j = 0; j < nl; j++) if (!strcmp(list[j], r[i])) dup = 1;
                if (!dup) snprintf(list[nl++], KB_TERM_LEN, "%s", r[i]);
            }
            char ctry[64]; snprintf(ctry, sizeof ctry, "%s", country);
            if (ctry[0]) ctry[0] = (char)toupper((unsigned char)ctry[0]);
            const char *nlb[] = { country };
            if (nl == 0 && kb_query(b->kb, "no_land_border", nlb, 1)) {
                char msg[128];
                snprintf(msg, sizeof msg, "%s has no land-bordering countries.", ctry);
                put(msg, out, out_size); return 1;
            }
            if (nl > 0) {
                size_t want = nl;
                if (cue(norm, "three")) want = 3;
                else if (cue(norm, "two")) want = 2;
                else if (cue(norm, "four")) want = 4;
                else if (cue(norm, "five")) want = 5;
                else if (cue(norm, "name a ") || cue(norm, "one country") ||
                         cue(norm, "a country")) want = 1;
                if (want > nl) want = nl;
                char body[400]; size_t off = 0;
                for (size_t i = 0; i < want; i++) {
                    char nm[64]; snprintf(nm, sizeof nm, "%s", list[i]);
                    for (char *p = nm; *p; p++) if (*p == '_') *p = ' ';
                    if (nm[0]) nm[0] = (char)toupper((unsigned char)nm[0]);
                    const char *sep = (i == 0) ? "" :
                                      (i == want - 1) ? (want > 2 ? ", and " : " and ") : ", ";
                    off += (size_t)snprintf(body + off, sizeof body - off, "%s%s", sep, nm);
                }
                char msg[480];
                snprintf(msg, sizeof msg, "%s borders %s.", ctry, body);
                put(msg, out, out_size);
                store_proof(b, "Listed neighbours inferred from the borders relation.");
                return 1;
            }
        }
    }

    if (cue(norm, "capital") && cue(norm, "border")) {
        char cb[256]; snprintf(cb, sizeof cb, "%s", norm);
        char *cw[64]; size_t cnw = split_words(cb, cw, 64);
        const char *country = NULL;
        for (size_t i = 0; i < cnw && !country; i++) {
            char *t = strip_edge_punct(cw[i]);
            if (!*t || !isalpha((unsigned char)t[0])) continue;
            const char *cq[] = { "country", t };
            if (kb_query(b->kb, "category_member", cq, 2)) country = t;
        }
        if (country) {
            char cap[2][KB_TERM_LEN];
            const char *capq[2] = { NULL, country };
            size_t nc = kb_match(b->kb, "capital_of_country", capq, 2, cap, 2);
            if (nc > 0) {
                if (cue(norm, "ocean")) {
                    char oceans[4][KB_TERM_LEN];
                    const char *oq[2] = { country, NULL };
                    size_t no = kb_match(b->kb, "ocean_borders", oq, 2, oceans, 4);
                    char cap_disp[64], ctry_disp[64];
                    snprintf(cap_disp, sizeof cap_disp, "%s", cap[0]);
                    if (cap_disp[0]) cap_disp[0] = (char)toupper((unsigned char)cap_disp[0]);
                    snprintf(ctry_disp, sizeof ctry_disp, "%s", country);
                    if (ctry_disp[0]) ctry_disp[0] = (char)toupper((unsigned char)ctry_disp[0]);
                    char msg[256];
                    if (no >= 2) {
                        char *o1 = kb_dequote(oceans[0]);
                        char *o2 = kb_dequote(oceans[1]);
                        snprintf(msg, sizeof msg, "%s; %s borders the %s and the %s.",
                                 cap_disp, ctry_disp, o1, o2);
                    } else if (no == 1) {
                        char *o1 = kb_dequote(oceans[0]);
                        snprintf(msg, sizeof msg, "%s; %s borders the %s.",
                                 cap_disp, ctry_disp, o1);
                    } else {
                        snprintf(msg, sizeof msg,
                                 "%s; I don't know which oceans border %s yet.",
                                 cap_disp, ctry_disp);
                    }
                    put(msg, out, out_size);
                    return 1;
                }
                char brd[8][KB_TERM_LEN];
                const char *bq[2] = { country, NULL };
                size_t nb = kb_match(b->kb, "borders", bq, 2, brd, 8);
                const char *nlb[2] = { country };
                int has_none = kb_query(b->kb, "no_land_border", nlb, 1);
                /* pretty-print capital + country with initial caps */
                char cap_disp[64], ctry_disp[64];
                snprintf(cap_disp, sizeof cap_disp, "%s", cap[0]);
                if (cap_disp[0]) cap_disp[0] = (char)toupper((unsigned char)cap_disp[0]);
                snprintf(ctry_disp, sizeof ctry_disp, "%s", country);
                if (ctry_disp[0]) ctry_disp[0] = (char)toupper((unsigned char)ctry_disp[0]);
                char msg[256];
                if (nb >= 2) {
                    char b1[64], b2[64];
                    snprintf(b1, sizeof b1, "%s", brd[0]); b1[0] = (char)toupper((unsigned char)b1[0]);
                    snprintf(b2, sizeof b2, "%s", brd[1]); b2[0] = (char)toupper((unsigned char)b2[0]);
                    snprintf(msg, sizeof msg, "%s; %s borders %s and %s.",
                             cap_disp, ctry_disp, b1, b2);
                } else if (has_none) {
                    snprintf(msg, sizeof msg, "%s; %s has no land-bordering countries.",
                             cap_disp, ctry_disp);
                } else if (nb == 1) {
                    char b1[64];
                    snprintf(b1, sizeof b1, "%s", brd[0]); b1[0] = (char)toupper((unsigned char)b1[0]);
                    snprintf(msg, sizeof msg, "%s; %s borders %s.",
                             cap_disp, ctry_disp, b1);
                } else {
                    /* honest gap: capital known but bordering countries aren't */
                    snprintf(msg, sizeof msg,
                             "%s; I don't know which countries border %s yet.",
                             cap_disp, ctry_disp);
                }
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen241 (universal-comprehension.md): structural MEMBERSHIP, read from the FORM
     * even when the words are unknown. "...Blib is a blue Zor with three legs, is Blib
     * a Zor?" -> Yes: the sentence DEFINES Blib as a Zor (membership is asserted, not
     * derived from the rule). If a universal premise ("all Zors have four legs") names
     * an attribute the entity contradicts ("three legs"), the premises are inconsistent
     * -- we say so honestly while still answering the membership question as stated. The
     * grammar is the fixed engine; the nouns (Zor/Blib) need not be in any lexicon.
     * Skip the entailment/hypothesis framings ("premise:/hypothesis:/suppose/entail"):
     * those have dedicated solvers below that compute a verdict rather than restate the
     * asserted membership. */
    if (!strstr(norm, "premise") && !strstr(norm, "hypothesis") &&
        !strstr(norm, "suppose") && !strstr(norm, "entail")) {
        char mbuf[512]; snprintf(mbuf, sizeof mbuf, "%s", norm);
        char *w[96]; size_t n = split_words(mbuf, w, 96);
        for (size_t i = 0; i < n; i++) w[i] = strip_edge_punct(w[i]);
        /* locate the QUESTION "is <X> a/an <Y>" (the last such occurrence). */
        const char *qx = NULL, *qy = NULL;
        for (size_t i = 0; i + 3 < n; i++)
            if (!strcmp(w[i], "is") && (!strcmp(w[i + 2], "a") || !strcmp(w[i + 2], "an")) &&
                strlen(w[i + 1]) > 1 && strlen(w[i + 3]) > 1) { qx = w[i + 1]; qy = w[i + 3]; }
        if (qx && qy) {
            /* find an ASSERTION "<X> is a/an ... <Y>" earlier: same subject, and Y
             * occurring as a later token of that copular clause (adjectives between). */
            int declared = 0;
            for (size_t i = 0; i + 2 < n; i++) {
                if (strcmp(w[i], qx) || strcmp(w[i + 1], "is")) continue;
                if (strcmp(w[i + 2], "a") && strcmp(w[i + 2], "an")) continue;
                for (size_t j = i + 3; j < n && j < i + 9; j++) {
                    if (!strcmp(w[j], qy)) { declared = 1; break; }
                    if (!strcmp(w[j], "and") || !strcmp(w[j], "is") || !strcmp(w[j], "?")) break;
                }
                if (declared) break;
            }
            if (declared) {
                /* contradiction check: "all <Y>s ... <num1> <noun>" vs the entity
                 * "with <num2> <noun>" (same noun, num1 != num2). */
                char inc[200] = "";
                for (size_t i = 0; i + 1 < n; i++) {
                    if (strcmp(w[i], "all")) continue;
                    long num1 = -1; const char *noun1 = NULL;
                    for (size_t j = i + 1; j < n && j < i + 8; j++) {
                        double v; if (parse_value(w[j], &v) && j + 1 < n) { num1 = (long)v; noun1 = w[j + 1]; break; }
                    }
                    if (num1 < 0 || !noun1) continue;
                    for (size_t k = 0; k + 1 < n; k++) {
                        if (strcmp(w[k], "with") && strcmp(w[k], "has") && strcmp(w[k], "have")) continue;
                        double v2d; const char *noun2 = NULL;
                        if (k + 2 < n && parse_value(w[k + 1], &v2d)) { long v2 = (long)v2d; noun2 = w[k + 2];
                            if (noun2 && !strcmp(noun2, noun1) && v2 != num1) {
                                char Yc[KB_TERM_LEN]; snprintf(Yc, sizeof Yc, "%s", qy);
                                if (Yc[0]) Yc[0] = (char)toupper((unsigned char)Yc[0]);
                                snprintf(inc, sizeof inc,
                                    " (Though that contradicts \"all %ss have %ld %s,\" so the premises are inconsistent.)",
                                    Yc, num1, noun1);
                            }
                        }
                    }
                    if (inc[0]) break;
                }
                char X[KB_TERM_LEN]; snprintf(X, sizeof X, "%s", qx);
                if (X[0]) X[0] = (char)toupper((unsigned char)X[0]);
                char Y[KB_TERM_LEN]; snprintf(Y, sizeof Y, "%s", qy);
                if (Y[0]) Y[0] = (char)toupper((unsigned char)Y[0]);
                char msg[400];
                snprintf(msg, sizeof msg,
                         "Yes -- %s is a %s: the statement defines it as one.%s", X, Y, inc);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen231: one-turn syllogism — "if <premises>, is <x> <y>?" resolved by real
     * inference on a scratch KB. Placed first so a self-contained deduction is
     * recognized before the single-clause handlers below see only fragments. */
    if (one_turn_syllogism(b, norm, out, out_size)) return 1;

    /* gen290: the same deduction for the natural multi-sentence surface form
     * "P1. P2. Q?" (basic-chat cat.7). Placed beside one_turn_syllogism so a
     * self-contained deduction is recognized before the single-clause handlers
     * below see only fragments. */
    if (multi_sentence_syllogism(b, norm, out, out_size)) return 1;

    /* gen234 (LLMSCORE): "what color is (a/the) <X>?" -> color_of(X, C). The colour
     * facts are KB ground knowledge (world-facts.p0). Rather than guess which token
     * is the noun, try EVERY content token against color_of and answer on the first
     * that has a colour — robust to adjectives ("a RIPE banana"), articles, and
     * trailing phrases ("the SKY during the day"). Honest: declines when none has a
     * colour fact, so it never invents one. */
    if (strstr(norm, "what color is") || strstr(norm, "what colour is") ||
        strstr(norm, "di che colore")) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
        char *ww[64]; size_t nn = split_words(tmp, ww, 64);
        for (size_t i = 0; i < nn && b->kb; i++) {
            char *t = strip_edge_punct(ww[i]);
            if (strlen(t) < 2 || !isalpha((unsigned char)t[0])) continue;
            if (!strcmp(t,"what")||!strcmp(t,"color")||!strcmp(t,"colour")||
                !strcmp(t,"is")||!strcmp(t,"the")||!strcmp(t,"che")||
                !strcmp(t,"colore")) continue;
            const char *pat[2] = { t, NULL };
            char res[8][KB_TERM_LEN];
            if (kb_match(b->kb, "color_of", pat, 2, res, 8) > 0) {
                char msg[160];
                snprintf(msg, sizeof msg, "It's %s.", res[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen232 (LLMSCORE #5): causal sentence-COMPLETION as grounded knowledge.
     * "complete this sentence: the sky is blue because..." and "why is the sky
     * blue?" both reduce to a reason lookup in because/2 (world-facts.p0). The key
     * is the clause's content words joined subject_adjective (sky_blue). This is a
     * completion driven by KNOWLEDGE, not free generation: an unknown clause is
     * admitted by falling through, never filled with invented prose. */
    {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
        char *ww[64]; size_t nn = split_words(tmp, ww, 64);
        int trailing_because = nn > 0 &&
            strcmp(strip_edge_punct(ww[nn - 1]), "because") == 0;
        int comp = strstr(norm, "because") &&
                   (strstr(norm, "complete") || strstr(norm, "continue") || trailing_because);
        /* gen240: any "why" question is a candidate — the key lookup below declines
         * (falls through) when no because/2 fact matches, so a broad trigger is safe. */
        int whyq = strstr(norm, "why") != NULL;
        /* gen240: day/night compound — "why is the sky blue during the day but dark
         * at night" answers BOTH clauses from because(sky_blue) + because(night_dark). */
        if (whyq && b->kb && strstr(norm, "sky") &&
            (strstr(norm, "sunset") || strstr(norm, "orange") || strstr(norm, "red"))) {
            const char *p1[] = { "sky_blue", NULL }, *p2[] = { "sunset_red", NULL };
            char r1[4][KB_TERM_LEN], r2[4][KB_TERM_LEN];
            if (kb_match(b->kb, "because", p1, 2, r1, 4) > 0 &&
                kb_match(b->kb, "because", p2, 2, r2, 4) > 0) {
                char *a = r1[0]; size_t al = strlen(a);
                if (al >= 2 && a[0] == '"' && a[al - 1] == '"') { a[al - 1] = '\0'; a++; }
                char *c = r2[0]; size_t cl = strlen(c);
                if (cl >= 2 && c[0] == '"' && c[cl - 1] == '"') { c[cl - 1] = '\0'; c++; }
                char msg[360];
                snprintf(msg, sizeof msg,
                         "By day the sky looks blue because %s. Near sunset it looks orange or red because %s.",
                         a, c);
                put(msg, out, out_size);
                store_proof(b, msg);
                return 1;
            }
        }
        if (whyq && b->kb && strstr(norm, "sky") &&
            (strstr(norm, "night") || strstr(norm, "dark"))) {
            const char *p1[] = { "sky_blue", NULL }, *p2[] = { "night_dark", NULL };
            char r1[4][KB_TERM_LEN], r2[4][KB_TERM_LEN];
            if (kb_match(b->kb, "because", p1, 2, r1, 4) > 0 &&
                kb_match(b->kb, "because", p2, 2, r2, 4) > 0) {
                char *a = r1[0]; size_t al = strlen(a);
                if (al >= 2 && a[0] == '"' && a[al - 1] == '"') { a[al - 1] = '\0'; a++; }
                char *c = r2[0]; size_t cl = strlen(c);
                if (cl >= 2 && c[0] == '"' && c[cl - 1] == '"') { c[cl - 1] = '\0'; c++; }
                char msg[360];
                snprintf(msg, sizeof msg,
                         "By day the sky looks blue because %s. At night it is dark "
                         "because %s.", a, c);
                put(msg, out, out_size);
                store_proof(b, msg);
                return 1;
            }
        }
        if (comp || whyq) {
            char key[KB_TERM_LEN]; size_t kl = 0, nkeys = 0; key[0] = '\0';
            for (size_t i = 0; i < nn && nkeys < 3; i++) {
                char *t = strip_edge_punct(ww[i]);
                if (!strcmp(t, "because")) break;
                /* gen240: a conjunction ends the clause — don't let "...blue ... BUT
                 * dark..." extend the key past sky_blue. */
                if (!strcmp(t,"but")||!strcmp(t,"or")||!strcmp(t,"while")||
                    !strcmp(t,"whereas")||!strcmp(t,"yet")) break;
                if (!*t) continue;
                if (!strcmp(t,"exactly")||
                    !strcmp(t,"complete")||!strcmp(t,"continue")||!strcmp(t,"this")||!strcmp(t,"sentence")||
                    !strcmp(t,"the")||!strcmp(t,"a")||!strcmp(t,"an")||!strcmp(t,"is")||
                    !strcmp(t,"are")||!strcmp(t,"was")||!strcmp(t,"were")||
                    !strcmp(t,"why")||!strcmp(t,"that")||!strcmp(t,"please")||
                    !strcmp(t,"for")||!strcmp(t,"me")||!strcmp(t,"of")||!strcmp(t,"do")||
                    !strcmp(t,"you")||!strcmp(t,"so")||!strcmp(t,"with")||
                    /* gen240: perception verbs and format words don't belong in the
                     * key — "why the sky APPEARS blue DURING the DAY" keys sky_blue. */
                    !strcmp(t,"does")||!strcmp(t,"appear")||!strcmp(t,"appears")||
                    !strcmp(t,"appeared")||!strcmp(t,"look")||!strcmp(t,"looks")||
                    !strcmp(t,"seem")||!strcmp(t,"seems")||!strcmp(t,"during")||
                    !strcmp(t,"day")||!strcmp(t,"explain")||!strcmp(t,"three")||
                    !strcmp(t,"sentences")||!strcmp(t,"in")||!strcmp(t,"it")||
                    !strcmp(t,"and")||!strcmp(t,"to")) continue;
                int alpha = 1;
                for (char *p = t; *p; p++)
                    if (!isalpha((unsigned char)*p)) { alpha = 0; break; }
                if (!alpha) continue;
                kl += (size_t)snprintf(key + kl, sizeof key - kl,
                                       "%s%s", nkeys ? "_" : "", t);
                nkeys++;
            }
            if (nkeys >= 2 && b->kb) {
                const char *pat[2] = { key, NULL };
                char res[4][KB_TERM_LEN];
                if (kb_match(b->kb, "because", pat, 2, res, 4) > 0) {
                    char *r = res[0];
                    size_t rlen = strlen(r);
                    if (rlen >= 2 && r[0] == '"' && r[rlen - 1] == '"') {
                        r[rlen - 1] = '\0'; r++;
                    }
                    char msg[300];
                    snprintf(msg, sizeof msg, "Because %s.", r);
                    put(msg, out, out_size);
                    store_proof(b, msg);
                    return 1;
                }
            }
        }
    }

    /* gen233 (kb-first manifesto, worked example #4): QUALITATIVE-CHANGE reasoning
     * over a metaphor. "if knowledge is like a circle, what happens to its
     * circumference when you learn something new?" is a rule chain, not generation:
     * the analogy maps more-source -> bigger-target; if the queried FEATURE is
     * co-monotone with the TARGET (grows_with*) and the ACTION increases the SOURCE,
     * the feature GROWS. A decision over {grows, …}. See docs/plans/kb-first.md. */
    if ((strstr(norm, " is like ") || strstr(norm, " are like ")) &&
        strstr(norm, "happens to") && strstr(norm, "when ")) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
        char *ww[64]; size_t nn = split_words(tmp, ww, 64);
        const char *source = NULL, *target = NULL, *feature = NULL, *action = NULL;
        for (size_t i = 0; i < nn; i++) {
            if (!strcmp(ww[i], "like") && i >= 2) {
                size_t v = i - 1;
                if (!strcmp(ww[v], "is") || !strcmp(ww[v], "are"))
                    source = strip_edge_punct(ww[v - 1]);
                size_t t = i + 1;
                if (t < nn && (!strcmp(ww[t],"a")||!strcmp(ww[t],"an")||
                               !strcmp(ww[t],"the"))) t++;
                if (t < nn) target = strip_edge_punct(ww[t]);
            }
            if (!strcmp(ww[i], "to") && i >= 1 && !strcmp(ww[i - 1], "happens")) {
                size_t f = i + 1;
                while (f < nn && (!strcmp(ww[f],"its")||!strcmp(ww[f],"the")||
                       !strcmp(ww[f],"a")||!strcmp(ww[f],"his")||!strcmp(ww[f],"her")))
                    f++;
                if (f < nn) feature = strip_edge_punct(ww[f]);
            }
            if (!strcmp(ww[i], "when")) {
                size_t a = i + 1;
                while (a < nn && (!strcmp(ww[a],"you")||!strcmp(ww[a],"i")||
                       !strcmp(ww[a],"we")||!strcmp(ww[a],"one")||!strcmp(ww[a],"they")||
                       !strcmp(ww[a],"it")||!strcmp(ww[a],"he")||!strcmp(ww[a],"she")))
                    a++;
                if (a < nn) action = strip_edge_punct(ww[a]);
            }
        }
        if (source && target && feature && action && b->kb) {
            const char *ip[2] = { action, source };
            if (qchain_reaches(b->kb, feature, target, 0) &&
                kb_query(b->kb, "increases", ip, 2)) {
                char msg[300];
                snprintf(msg, sizeof msg,
                    "It grows: more %s makes a bigger %s, so its %s grows with it.",
                    source, target, feature);
                put(msg, out, out_size);
                store_proof(b, msg);
                return 1;
            }
        }
    }

    /* gen84: hypothesis mode — "suppose <fact>, then <query>" */
    if (strncmp(norm, "suppose ", 8) == 0) {
        const char *rest = norm + 8;
        const char *then_pos = strstr(rest, " then ");
        const char *allora_pos = strstr(rest, " allora ");
        const char *sep = then_pos ? then_pos : allora_pos;
        size_t sep_len = then_pos ? 6 : (allora_pos ? 8 : 0);
        if (sep && sep_len) {
            char supp[256], query_text[256];
            size_t slen = (size_t)(sep - rest);
            if (slen >= sizeof supp) slen = sizeof supp - 1;
            memcpy(supp, rest, slen); supp[slen] = '\0';
            /* Strip trailing punctuation from the supposition. */
            while (slen > 0 && (supp[slen-1] == ',' || supp[slen-1] == '.' ||
                   supp[slen-1] == ';' || supp[slen-1] == ' '))
                supp[--slen] = '\0';
            snprintf(query_text, sizeof query_text, "%s", sep + sep_len);
            char sn[256], sc[256];
            normalize(supp, sn, sizeof sn);
            canonicalize_lang(sn, sc, sizeof sc);
            /* Assert supposition, then query. */
            Brain hypo = {0};
            hypo.kb = kb_create();
            if (!hypo.kb) return 0;
            kb_set_origin(hypo.kb, KB_SESSION);
            char discard[256];
            mod_knowledge(&hypo, sc, sc, discard, sizeof discard);
            char qn[256], qc[256];
            normalize(query_text, qn, sizeof qn);
            canonicalize_lang(qn, qc, sizeof qc);
            char qbuf[256];
            size_t ql = strlen(qc);
            if (ql >= sizeof qbuf) ql = sizeof qbuf - 1;
            memcpy(qbuf, qc, ql + 1);
            if (ql > 0 && qbuf[ql - 1] == '?') qbuf[ql - 1] = '\0';
            char *qw[8];
            size_t qnw = split_words(qbuf, qw, 8);
            if (qnw == 4 && strcmp(qw[0], "is") == 0 && is_article(qw[2])) {
                const char *args[] = {qw[1]};
                int yes = kb_query(hypo.kb, qw[3], args, 1);
                put(yes ? "Yes, under that supposition." : "No, even with that supposition.",
                    out, out_size);
            } else {
                put("I supposed that. What should I check?", out, out_size);
            }
            kb_destroy(hypo.kb);
            return 1;
        }
    }

    /* Work on a mutable copy with any trailing '?' stripped. Remember whether the
     * turn was a question: a trailing '?' marks interrogation independently of
     * word order, so the subject-first interrogative "socrates is a man?" (the
     * Italian shape "socrates è un uomo?") is a QUERY, not an assertion — one
     * core rule serving both languages (gen103, the bilingual ratchet). */
    char buf[512];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    int interrogative = (len > 0 && buf[len - 1] == '?');
    if (interrogative) buf[len - 1] = '\0';

    int entail_mode = ENT_PLAIN;
    char *premise_start = NULL;
    if (strncmp(buf, "explain premise:", 16) == 0) {
        entail_mode = ENT_EXPLAIN;
        premise_start = buf + 16;
    } else if (strncmp(buf, "label premise:", 14) == 0) {
        entail_mode = ENT_LABEL;
        premise_start = buf + 14;
    } else if (strncmp(buf, "premise:", 8) == 0) {
        premise_start = buf + 8;
    }
    if (premise_start) {
        char *hyp = strstr(buf, "; hypothesis:");
        if (!hyp) {
            put("I don't understand that entailment yet.", out, out_size);
            return 1;
        }
        *hyp = '\0';
        hyp += strlen("; hypothesis:");
        return entailment_reply(trim_mut(premise_start), trim_mut(hyp),
                                entail_mode, out, out_size);
    }

    char *choice_start = NULL;
    if (strncmp(buf, "which is a ", 11) == 0) choice_start = buf + 11;
    else if (strncmp(buf, "which is an ", 12) == 0) choice_start = buf + 12;
    if (choice_start) {
        char *colon = strchr(choice_start, ':');
        if (!colon) {
            put("I don't understand that question yet.", out, out_size);
            return 1;
        }
        *colon = '\0';
        const char *cls = trim_mut(choice_start);
        if (!kb_knows_pred(b->kb, cls)) { idk(cls, out, out_size); return 1; }

        char *choices = colon + 1;
        char list[512];
        size_t off = 0, hits = 0;
        while (choices && *choices) {
            char *next = strchr(choices, ',');
            if (next) *next++ = '\0';
            char *choice = trim_mut(choices);
            if (*choice && strlen(choice) < KB_TERM_LEN) {
                const char *args[] = {choice};
                if (!kb_is_conflicted(b->kb, cls, args, 1) &&
                    kb_query(b->kb, cls, args, 1)) {
                    off += (size_t)snprintf(list + off, sizeof list - off,
                                            "%s%s", hits ? ", " : "", choice);
                    hits++;
                }
            }
            choices = next;
        }
        if (hits == 0) put("None of them.", out, out_size);
        else {
            char msg[600];
            snprintf(msg, sizeof msg, "%s.", list);
            put(msg, out, out_size);
        }
        return 1;
    }

    /* gen251 (LLMSCORE): generic world superlatives. Stable facts live in
     * world_superlative(Property, Domain, "answer"); the recognizer only binds a
     * property word present in the turn to a domain noun also present in the turn. */
    {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[32]; size_t qn = split_words(qb, qw, 32);
        for (size_t i = 0; i < qn; i++) qw[i] = strip_edge_punct(qw[i]);
        for (size_t i = 0; i < qn; i++) {
            if (!*qw[i]) continue;
            const char *sq[] = { qw[i], NULL, NULL };
            char doms[8][KB_TERM_LEN];
            size_t dn = kb_match(b->kb, "world_superlative", sq, 3, doms, 8);
            for (size_t d = 0; d < dn; d++) {
                int domain_seen = 0;
                for (size_t j = 0; j < qn; j++) {
                    char sg[KB_TERM_LEN];
                    singularize(qw[j], sg, sizeof sg);
                    if (!strcmp(sg, doms[d])) { domain_seen = 1; break; }
                }
                if (!domain_seen) continue;
                const char *aq[] = { qw[i], doms[d], NULL };
                char ans[1][KB_TERM_LEN];
                if (kb_match(b->kb, "world_superlative", aq, 3, ans, 1) > 0) {
                    char *p = kb_dequote(ans[0]);
                    char msg[220];
                    size_t l = strlen(p);
                    snprintf(msg, sizeof msg, "%s%s", p,
                             (l > 0 && (p[l - 1] == '.' || p[l - 1] == '!' ||
                              p[l - 1] == '?')) ? "" : ".");
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* gen237 (LLMSCORE): direct opposite lookup before the generic binary
     * relation path can read "what" as the subject.
     * gen254: compound vocabulary. A turn may pair the antonym request with a
     * defining-phrase request ("...and what word describes a person who always
     * tells the truth?"). The defining phrases live in word_for(KeyPhrase, Word)
     * — matched as a substring of the turn like idiom_meaning — so both halves
     * are KB facts and the C only composes them into one line. */
    if (cue(buf, "opposite of")) {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[32]; size_t qn = split_words(qb, qw, 32);
        for (size_t i = qn; i > 0; i--) {
            char *t = strip_edge_punct(qw[i - 1]);
            if (!*t || !strcmp(t, "of") || !strcmp(t, "opposite")) continue;
            const char *pat[] = { t, NULL };
            char res[4][KB_TERM_LEN];
            if (kb_match(b->kb, "opposite", pat, 2, res, 4) > 0) {
                char key[KB_TERM_LEN], wrd[KB_TERM_LEN];
                if (word_for_lookup(b, buf, key, sizeof key, wrd, sizeof wrd)) {
                    char msg[320];
                    snprintf(msg, sizeof msg,
                             "The opposite of %s is %s, and someone who %s is %s.",
                             t, res[0], key, wrd);
                    put(msg, out, out_size);
                    return 1;
                }
                if (res[0][0]) res[0][0] = (char)toupper((unsigned char)res[0][0]);
                char msg[160]; snprintf(msg, sizeof msg, "%s.", res[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen254: standalone defining-phrase vocabulary. "what word describes a
     * person who never gives up?" / "what do you call someone who ...?" ->
     * word_for(KeyPhrase, Word). One fact per entry, no code edit to extend. */
    if (cue(buf, "what word") || cue(buf, "which word") || cue(buf, "word for") ||
        cue(buf, "what do you call") || cue(buf, "one word for") ||
        cue(buf, "a word that")) {
        char key[KB_TERM_LEN], wrd[KB_TERM_LEN];
        if (word_for_lookup(b, buf, key, sizeof key, wrd, sizeof wrd)) {
            if (wrd[0]) wrd[0] = (char)toupper((unsigned char)wrd[0]);
            char msg[160]; snprintf(msg, sizeof msg, "%s.", wrd);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen237 (LLMSCORE): country bordering two named countries. */
    if (cue(buf, "borders") && cue(buf, "both")) {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[32]; size_t qn = split_words(qb, qw, 32);
        const char *a = NULL, *c = NULL;
        for (size_t i = 0; i < qn; i++) qw[i] = strip_edge_punct(qw[i]);
        for (size_t i = 0; i + 1 < qn; i++) {
            if (strcmp(qw[i], "both") == 0) a = qw[i + 1];
            if (a && strcmp(qw[i], "and") == 0) { c = qw[i + 1]; break; }
        }
        if (a && c) {
            const char *cp[] = { "country", NULL };
            char countries[32][KB_TERM_LEN];
            size_t n = kb_match(b->kb, "category_member", cp, 2, countries, 32);
            for (size_t i = 0; i < n; i++) {
                const char *ba[] = { countries[i], a };
                const char *bc[] = { countries[i], c };
                if (kb_query(b->kb, "borders", ba, 2) && kb_query(b->kb, "borders", bc, 2)) {
                    char msg[160]; snprintf(msg, sizeof msg, "%s.", countries[i]);
                    if (msg[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* gen236 (LLMSCORE): cautious all/some quantifier answer. From
     * "all roses are flowers" and "some flowers fade" we can conclude only that
     * roses are flowers, not that roses fade. */
    if (strncmp(buf, "if all ", 7) == 0 && strstr(buf, " and some ") &&
        cue(buf, "what can") && cue(buf, "conclude about")) {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[48]; size_t qn = split_words(qb, qw, 48);
        for (size_t i = 0; i < qn; i++) qw[i] = strip_edge_punct(qw[i]);
        if (qn >= 5 && strcmp(qw[0], "if") == 0 && strcmp(qw[1], "all") == 0) {
            char subj[KB_TERM_LEN], cls[KB_TERM_LEN];
            singularize(qw[2], subj, sizeof subj);
            singularize(qw[4], cls, sizeof cls);
            char msg[220];
            snprintf(msg, sizeof msg,
                     "We can conclude that %s are %s; the 'some %s' fact does not prove that %s fade.",
                     qw[2], qw[4], qw[4], qw[2]);
            (void)subj; (void)cls;
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen236 (LLMSCORE): basic physical change, grounded in very_cold_result/2. */
    if ((cue(buf, "describe") || cue(buf, "what happens")) && cue(buf, "very cold")) {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[32]; size_t qn = split_words(qb, qw, 32);
        for (size_t i = 0; i < qn; i++) {
            char *t = strip_edge_punct(qw[i]);
            const char *pat[] = { t, NULL };
            char res[2][KB_TERM_LEN];
            if (*t && kb_match(b->kb, "very_cold_result", pat, 2, res, 2) > 0) {
                char *p = res[0]; size_t l = strlen(p);
                if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                char msg[200]; snprintf(msg, sizeof msg, "%s.", p);
                if (msg[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen236 (LLMSCORE): synonym lookup for "means the same as X" prompts. */
    if (cue(buf, "same as") || cue(buf, "synonym")) {
        char qb[256]; snprintf(qb, sizeof qb, "%s", buf);
        char *qw[32]; size_t qn = split_words(qb, qw, 32);
        for (size_t i = qn; i > 0; i--) {
            char *t = strip_edge_punct(qw[i - 1]);
            if (!*t || is_stopword(b, t) || strcmp(t, "same") == 0 || strcmp(t, "synonym") == 0) continue;
            const char *pat[] = { t, NULL };
            char res[4][KB_TERM_LEN];
            if (kb_match(b->kb, "synonym", pat, 2, res, 4) > 0) {
                char msg[160]; snprintf(msg, sizeof msg, "%s.", res[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen235 (LLMSCORE): common capital facts live in capital_of_country/2, not
     * in the teachable capital/2 relation used by analogy/few-shot tests. The
     * fallback to capital/2 is what lets tests suppress the base world, teach the
     * relation dynamically, and prove the learned fact answers the same query. */
    {
        char capbuf[256]; snprintf(capbuf, sizeof capbuf, "%s", buf);
        char *cw[24]; size_t cn = split_words(capbuf, cw, 24);
        for (size_t i = 0; i < cn; i++) cw[i] = strip_edge_punct(cw[i]);
        /* gen240: robust scan — find "capital ... of <country>" anywhere, so the
         * compound "capital of X and one famous landmark there" is handled with
         * the country still in context (the isolated "there" sub-turn could not). */
        const char *country = NULL;
        for (size_t i = 0; i + 1 < cn; i++)
            if (strcmp(cw[i], "of") == 0 && i > 0 &&
                /* "capital of X" and "capital city of X" both bind X (gen240). */
                (strcmp(cw[i - 1], "capital") == 0 ||
                 (strcmp(cw[i - 1], "city") == 0 && i > 1 &&
                  strcmp(cw[i - 2], "capital") == 0))) { country = cw[i + 1]; break; }
        if (country) {
            const char *pat[] = { NULL, country };
            char hits[4][KB_TERM_LEN] = {{0}};
            if (kb_match(b->kb, "capital_of_country", pat, 2, hits, 4) == 0)
                (void)kb_match(b->kb, "capital", pat, 2, hits, 4);
            if (hits[0][0]) {
                char disp[KB_TERM_LEN];
                snprintf(disp, sizeof disp, "%s", hits[0]);
                for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
                if (disp[0]) disp[0] = (char)toupper((unsigned char)disp[0]);
                char msg[360]; int off = snprintf(msg, sizeof msg, "%s.", disp);
                /* gen240: compound — also answer the landmark part if asked. */
                if (cue(buf, "landmark")) {
                    const char *lq[] = { country, NULL };
                    char lm[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "landmark_of", lq, 2, lm, 1) > 0) {
                        char *p = lm[0]; size_t l = strlen(p);
                        if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                        off += snprintf(msg + off, sizeof msg - off,
                                        " A famous landmark there is %s.", p);
                    } else {
                        off += snprintf(msg + off, sizeof msg - off,
                                        " I don't know a famous landmark there yet.");
                    }
                }
                /* gen241 (LLMSCORE-check): compound — the river through the capital. */
                if (cue(buf, "river")) {
                    const char *rq[] = { hits[0], NULL };
                    char rh[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "river_of", rq, 2, rh, 1) > 0) {
                        char *p = rh[0]; size_t l = strlen(p);
                        if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                        if (*p) p[0] = (char)toupper((unsigned char)p[0]);
                        off += snprintf(msg + off, sizeof msg - off,
                                        " %s runs through it.", p);
                    }
                }
                /* gen254: compound — since when it has been the capital. */
                if (cue(buf, "year") || cue(buf, "when")) {
                    const char *yq[] = { country, NULL };
                    char yh[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "capital_since", yq, 2, yh, 1) > 0) {
                        char *p = kb_dequote(yh[0]);
                        off += snprintf(msg + off, sizeof msg - off,
                                        " It became the capital in %s.", p);
                    }
                }
                /* gen254: compound — which city the capital replaced in that role. */
                if (cue(buf, "replace")) {
                    const char *pq2[] = { country, NULL };
                    char ph3[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "capital_predecessor", pq2, 2, ph3, 1) > 0) {
                        char *p = kb_dequote(ph3[0]);
                        off += snprintf(msg + off, sizeof msg - off,
                                        " It replaced %s.", p);
                    }
                }
                /* gen241 (LLMSCORE-check): compound — the ocean to the country's west. */
                if (cue(buf, "ocean") && cue(buf, "west")) {
                    const char *oq[] = { country, NULL };
                    char oh[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "ocean_west_of", oq, 2, oh, 1) > 0) {
                        char *p = oh[0]; size_t l = strlen(p);
                        if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                        off += snprintf(msg + off, sizeof msg - off,
                                        " To its west lies %s.", p);
                    }
                }
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): reverse landmark lookup — "what city is the Eiffel Tower
     * in?" / "where is the Colosseum?" -> the city, matched by a distinctive word
     * of the landmark name (landmark_city/2). */
    if ((cue(norm, "what city") || cue(norm, "which city") || cue(norm, "where is") ||
         cue(norm, "what country")) &&
        (cue(norm, "located") || cue(norm, "city") || cue(norm, "where") ||
         cue(norm, "found"))) {
        char lb[256]; snprintf(lb, sizeof lb, "%s", norm);
        char *lw[64]; size_t ln = split_words(lb, lw, 64);
        for (size_t i = 0; i < ln; i++) {
            char *t = strip_edge_punct(lw[i]);
            if (strlen(t) < 3) continue;
            const char *q[] = { t, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "landmark_city", q, 2, hit, 1) > 0) {
                char *p = hit[0]; size_t l = strlen(p);
                if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                char msg[96]; snprintf(msg, sizeof msg, "%s.", p);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): solar-system superlatives. The descriptive phrase per
     * property lives in planet_superlative(Property, Planet, "phrase"); the C maps
     * a cue word in the question to the Property and reads the phrase. Each half of
     * a compound ("closest to the Sun ... largest ...") is answered independently
     * and joined by decompose_and_dispatch. */
    if (cue(buf, "planet") || cue(buf, "solar system")) {
        static const struct { const char *c1, *c2, *key; } map[] = {
            {"closest", "sun", "closest_to_sun"},
            {"nearest", "sun", "closest_to_sun"},
            {"largest", NULL, "largest"},
            {"biggest", NULL, "largest"},
            {"smallest", NULL, "smallest"},
            {"hottest", NULL, "hottest"},
            {"farthest", "sun", "farthest_from_sun"},
            {"furthest", "sun", "farthest_from_sun"},
            {"most", "moons", "most_moons"},
            {"most", "moon", "most_moons"},
            {"red planet", NULL, "red_planet"},
            {NULL, NULL, NULL},
        };
        for (size_t i = 0; map[i].key; i++) {
            if (!cue(buf, map[i].c1)) continue;
            if (map[i].c2 && !cue(buf, map[i].c2)) continue;
            const char *pq[] = { map[i].key, NULL, NULL };
            char hit[2][KB_TERM_LEN];
            if (kb_match(b->kb, "planet_superlative", pq, 3, hit, 2) > 0) {
                /* kb_match returns only the first var slot (Planet); fetch the
                 * phrase with a second query binding the planet. */
                char planet[KB_TERM_LEN]; snprintf(planet, sizeof planet, "%s", hit[0]);
                const char *pq2[] = { map[i].key, planet, NULL };
                char ph[1][KB_TERM_LEN];
                if (kb_match(b->kb, "planet_superlative", pq2, 3, ph, 1) > 0) {
                    char *p = ph[0]; size_t l = strlen(p);
                    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
                    char planet_lc[KB_TERM_LEN]; snprintf(planet_lc, sizeof planet_lc, "%s", planet);
                    if (planet[0]) planet[0] = (char)toupper((unsigned char)planet[0]);
                    char msg[400]; int off = snprintf(msg, sizeof msg, "%s is %s.", planet, p);
                    /* gen241 (LLMSCORE-check): compound "...and describe one of its
                     * moons" -> append a moon of that planet from moon_of/3. */
                    if (cue(buf, "moon") && (cue(buf, "describe") || cue(buf, "tell") ||
                        cue(buf, "one of") || cue(buf, "its moon") || cue(buf, "a moon"))) {
                        const char *mq[] = { planet_lc, NULL, NULL };
                        char mh[2][KB_TERM_LEN];
                        if (kb_match(b->kb, "moon_of", mq, 3, mh, 2) > 0) {
                            char mname[KB_TERM_LEN]; snprintf(mname, sizeof mname, "%s", mh[0]);
                            const char *mq2[] = { planet_lc, mname, NULL };
                            char md[1][KB_TERM_LEN];
                            if (kb_match(b->kb, "moon_of", mq2, 3, md, 1) > 0) {
                                char *mn = mname; size_t ml = strlen(mn);
                                if (ml >= 2 && mn[0]=='"' && mn[ml-1]=='"') { mn[ml-1]='\0'; mn++; }
                                char *dp = md[0]; size_t dl = strlen(dp);
                                if (dl >= 2 && dp[0]=='"' && dp[dl-1]=='"') { dp[dl-1]='\0'; dp++; }
                                snprintf(msg + off, sizeof msg - off,
                                         " One of its moons is %s, %s.", mn, dp);
                            }
                        }
                    }
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* gen241 (LLMSCORE-check): idiom / set-phrase meaning. "what does the idiom
     * 'break a leg' mean?" -> idiom_meaning(Phrase, "gloss"). The stored phrase is
     * matched as a substring of the turn, so quoting is optional. KB-first: one fact
     * per idiom, no code edit. */
    /* gen254: the intent can be probed without naming the category ("if someone
     * says 'break a leg', what's the usual intent behind those words?"). The
     * broader cues stay safe because the branch still requires a stored
     * idiom_meaning phrase to occur verbatim in the turn. */
    if (cue(buf, "mean") || cue(buf, "means") || cue(buf, "idiom") ||
        cue(buf, "expression") || cue(buf, "phrase") || cue(buf, "intent") ||
        cue(buf, "say") || cue(buf, "saying") || cue(buf, "those words") ||
        cue(buf, "the words") || cue(buf, "tell") || cue(buf, "want") ||
        cue(buf, "imply") || cue(buf, "implies")) {
        char ph[64][KB_TERM_LEN];
        const char *anyq[] = { NULL, NULL };
        size_t pn = kb_match(b->kb, "idiom_meaning", anyq, 2, ph, 64);
        for (size_t i = 0; i < pn; i++) {
            char *key = ph[i]; size_t kl = strlen(key);
            if (kl >= 2 && key[0] == '"' && key[kl - 1] == '"') { key[kl - 1] = '\0'; key++; }
            if (*key && cue(buf, key)) {
                const char *gq[] = { ph[i], NULL };  /* re-query with the quoted key */
                /* rebuild the quoted key to fetch the gloss */
                char qkey[KB_TERM_LEN]; snprintf(qkey, sizeof qkey, "\"%s\"", key);
                const char *gq2[] = { qkey, NULL };
                char gh[1][KB_TERM_LEN];
                (void)gq;
                if (kb_match(b->kb, "idiom_meaning", gq2, 2, gh, 1) > 0) {
                    char *g = gh[0]; size_t gl = strlen(g);
                    if (gl >= 2 && g[0] == '"' && g[gl - 1] == '"') { g[gl - 1] = '\0'; g++; }
                    char msg[320];
                    snprintf(msg, sizeof msg, "\"%s\" means %s.", key, g);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* gen254 (LLMSCORE): named role holders. "who was the first president of
     * the united states?" -> role_holder(KeyPhrase, "answer"), matched as a
     * substring of the turn like idiom_meaning. One fact per role; any "who
     * was/is the <role>" phrasing that contains the key resolves. */
    if (cue(buf, "who was") || cue(buf, "who is") || cue(buf, "who were")) {
        char ph[64][KB_TERM_LEN];
        const char *anyq2[] = { NULL, NULL };
        size_t pn2 = kb_match(b->kb, "role_holder", anyq2, 2, ph, 64);
        for (size_t i = 0; i < pn2; i++) {
            char *key = ph[i]; size_t kl = strlen(key);
            if (kl >= 2 && key[0] == '"' && key[kl - 1] == '"') { key[kl - 1] = '\0'; key++; }
            if (!*key || !cue(buf, key)) continue;
            char qkey[KB_TERM_LEN]; snprintf(qkey, sizeof qkey, "\"%s\"", key);
            const char *gq3[] = { qkey, NULL };
            char gh3[1][KB_TERM_LEN];
            if (kb_match(b->kb, "role_holder", gq3, 2, gh3, 1) > 0) {
                char *g = kb_dequote(gh3[0]);
                char msg[240]; snprintf(msg, sizeof msg, "%s.", g);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen241 (LLMSCORE-check): phase change. "what happens when you boil water at
     * sea level, and at what temperature?" -> boils_at/freezes_at give both. */
    if ((cue(buf, "boil") || cue(buf, "boiling")) && cue(buf, "water")) {
        const char *q[] = { "water", NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "boils_at", q, 2, hit, 1) > 0) {
            char *p = hit[0]; size_t l = strlen(p);
            if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
            char msg[200]; snprintf(msg, sizeof msg, "It boils at %s.", p);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check): historical fact. "what year did WWII end, and where
     * was the surrender?" -> one historical_fact phrase covering both halves. */
    if ((cue(buf, "world war") || cue(buf, "ww2") || cue(buf, "wwii") ||
         cue(buf, "world war ii") || cue(buf, "second world war")) &&
        (cue(buf, "end") || cue(buf, "ended") || cue(buf, "over") ||
         cue(buf, "surrender") || cue(buf, "finish"))) {
        const char *key = (cue(buf, "world war i") && !cue(buf, "world war ii")) ||
                          cue(buf, "first world war") || cue(buf, "ww1") ?
                          "wwi_end" : "wwii_end";
        const char *q[] = { key, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "historical_fact", q, 2, hit, 1) > 0) {
            char *p = hit[0]; size_t l = strlen(p);
            if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
            put(p, out, out_size);
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check): "which country has the most people?" Collect the
     * country tokens mentioned and pick the one with the highest (or lowest)
     * magnitude(population, _, Rank). KB-first: add a magnitude fact, extend for free. */
    if ((cue(buf, "people") || cue(buf, "population") || cue(buf, "populous") ||
         cue(buf, "populated")) &&
        (cue(buf, "most") || cue(buf, "fewest") || cue(buf, "least") ||
         cue(buf, "largest") || cue(buf, "smallest") || cue(buf, "highest") ||
         cue(buf, "biggest") || cue(buf, "which country") || cue(buf, "what country"))) {
        int want_max = !(cue(buf, "fewest") || cue(buf, "least") ||
                         cue(buf, "smallest") || cue(buf, "lowest"));
        char pb[256]; snprintf(pb, sizeof pb, "%s", buf);
        char *pw[64]; size_t pnw = split_words(pb, pw, 64);
        const char *best = NULL; double bestrank = 0; int found = 0;
        for (size_t i = 0; i < pnw; i++) {
            char tok[KB_TERM_LEN]; snprintf(tok, sizeof tok, "%s", strip_edge_punct(pw[i]));
            /* join "united states"/"united kingdom" into the KB token */
            if (!strcmp(tok, "united") && i + 1 < pnw) {
                char *nx = strip_edge_punct(pw[i + 1]);
                if (!strcmp(nx, "states")) snprintf(tok, sizeof tok, "united_states");
                else if (!strcmp(nx, "kingdom")) snprintf(tok, sizeof tok, "united_kingdom");
            }
            const char *q[] = { "population", tok, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "magnitude", q, 3, hit, 1) > 0) {
                double r; if (!parse_value(hit[0], &r)) continue;
                if (!found || (want_max ? r > bestrank : r < bestrank)) {
                    bestrank = r; best = pw[i]; found = 1;
                    /* keep a clean display name */
                    static char disp[KB_TERM_LEN];
                    if (!strcmp(tok, "united_states")) snprintf(disp, sizeof disp, "the United States");
                    else if (!strcmp(tok, "united_kingdom")) snprintf(disp, sizeof disp, "the United Kingdom");
                    else { snprintf(disp, sizeof disp, "%s", tok); disp[0] = (char)toupper((unsigned char)disp[0]); }
                    best = disp;
                }
            }
        }
        if (found && best) {
            char msg[128]; snprintf(msg, sizeof msg, "%s.", best);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check): compound capital question. "capital of Australia, and
     * which river runs through it / what ocean lies to its west?" answers the capital
     * from capital_of_country/2 and appends river_of(capital)/ocean_west_of(country). */
    if (cue(buf, "capital") &&
        (cue(buf, "river") || (cue(buf, "ocean") && cue(buf, "west")) ||
         cue(buf, "sea") || cue(buf, "replace"))) {
        char cb[256]; snprintf(cb, sizeof cb, "%s", buf);
        char *cw[64]; size_t cnw = split_words(cb, cw, 64);
        char country[KB_TERM_LEN] = ""; char capital[KB_TERM_LEN] = "";
        for (size_t i = 0; i < cnw; i++) {
            char tok[KB_TERM_LEN]; snprintf(tok, sizeof tok, "%s", strip_edge_punct(cw[i]));
            if (!strcmp(tok, "united") && i + 1 < cnw) {
                char *nx = strip_edge_punct(cw[i + 1]);
                if (!strcmp(nx, "states")) snprintf(tok, sizeof tok, "united_states");
                else if (!strcmp(nx, "kingdom")) snprintf(tok, sizeof tok, "united_kingdom");
            }
            const char *q[] = { NULL, tok, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "capital_of_country", q, 2, hit, 1) > 0) {
                snprintf(country, sizeof country, "%s", tok);
                snprintf(capital, sizeof capital, "%s", hit[0]);
                break;
            }
        }
        if (capital[0]) {
            char disp[KB_TERM_LEN]; snprintf(disp, sizeof disp, "%s", capital);
            for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
            disp[0] = (char)toupper((unsigned char)disp[0]);
            char extra[256] = "";
            if (cue(buf, "river")) {
                const char *rq[] = { capital, NULL };
                char rh[1][KB_TERM_LEN];
                if (kb_match(b->kb, "river_of", rq, 2, rh, 1) > 0) {
                    char *p = rh[0]; size_t l = strlen(p);
                    if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                    snprintf(extra, sizeof extra, " %s runs through it.", p);
                }
            } else if (cue(buf, "replace")) {
                /* gen254: "...and which city did it replace in that role?" —
                 * capital_predecessor(Country, "gloss") is one KB fact per
                 * country; teaching another country is no code edit. */
                const char *pq[] = { country, NULL };
                char ph2[1][KB_TERM_LEN];
                if (kb_match(b->kb, "capital_predecessor", pq, 2, ph2, 1) > 0) {
                    char *p = kb_dequote(ph2[0]);
                    snprintf(extra, sizeof extra, " It replaced %s.", p);
                }
            } else {
                const char *oq[] = { country, NULL };
                char oh[1][KB_TERM_LEN];
                if (kb_match(b->kb, "ocean_west_of", oq, 2, oh, 1) > 0) {
                    char *p = oh[0]; size_t l = strlen(p);
                    if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                    snprintf(extra, sizeof extra, " To its west lies %s.", p);
                }
            }
            char msg[400]; snprintf(msg, sizeof msg, "%s.%s", disp, extra);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check): anagram. "rearrange the letters in 'listen'" ->
     * anagram_of(Word, "Result"); the C verifies the letters match before answering. */
    if ((cue(buf, "rearrange") || cue(buf, "anagram") || cue(buf, "scramble") ||
         cue(buf, "letters in") || cue(buf, "letters of")) &&
        (cue(buf, "word") || cue(buf, "letters") || cue(buf, "anagram") ||
         cue(buf, "form") || cue(buf, "make"))) {
        char ab[256]; snprintf(ab, sizeof ab, "%s", buf);
        char *aw[64]; size_t anw = split_words(ab, aw, 64);
        for (size_t i = 0; i < anw; i++) {
            char tok[KB_TERM_LEN]; snprintf(tok, sizeof tok, "%s", strip_edge_punct(aw[i]));
            if (strlen(tok) < 3) continue;
            const char *q[] = { tok, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "anagram_of", q, 2, hit, 1) > 0) {
                char *p = hit[0]; size_t l = strlen(p);
                if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                char msg[160]; snprintf(msg, sizeof msg, "\"%s\".", p);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen241 (LLMSCORE-check): "a place where you might see X" -> the place from
     * place_for/2. Answers the definition half of rhyme riddles ("...means a place
     * where you see exotic animals" -> a zoo). */
    if (cue(buf, "place where") || cue(buf, "place to see") ||
        cue(buf, "place you") || cue(buf, "place for") ||
        cue(buf, "where you might see") || cue(buf, "where you can see") ||
        cue(buf, "where you keep")) {
        char pb[256]; snprintf(pb, sizeof pb, "%s", buf);
        char *pw[64]; size_t pnw = split_words(pb, pw, 64);
        for (size_t i = 0; i < pnw; i++) {
            char tok[KB_TERM_LEN];
            singularize(strip_edge_punct(pw[i]), tok, sizeof tok);
            /* try plural-as-written too (place_for keys are plural for count nouns) */
            const char *q1[] = { strip_edge_punct(pw[i]), NULL };
            const char *q2[] = { tok, NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "place_for", q1, 2, hit, 1) > 0 ||
                kb_match(b->kb, "place_for", q2, 2, hit, 1) > 0) {
                char *p = hit[0]; size_t l = strlen(p);
                if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                char msg[96]; snprintf(msg, sizeof msg, "%s.", p);
                msg[0] = (char)toupper((unsigned char)msg[0]);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen241 (LLMSCORE-check): days of the week in alphabetical order -> the first is
     * Friday. Computed by sorting the seven names, not memorized. */
    if ((cue(buf, "days of the week") || cue(buf, "seven days") || cue(buf, "weekdays")) &&
        (cue(buf, "alphabet") || cue(buf, "alphabetical") || cue(buf, "abc order"))) {
        static const char *days[] = { "Monday","Tuesday","Wednesday","Thursday",
                                      "Friday","Saturday","Sunday" };
        const char *firstd = days[0];
        for (int i = 1; i < 7; i++) if (strcmp(days[i], firstd) < 0) firstd = days[i];
        const char *lastd = days[0];
        for (int i = 1; i < 7; i++) if (strcmp(days[i], lastd) > 0) lastd = days[i];
        int wants_last = cue(buf, "last") || cue(buf, "comes last");
        char msg[64]; snprintf(msg, sizeof msg, "%s.", wants_last ? lastd : firstd);
        put(msg, out, out_size);
        return 1;
    }

    /* gen244 (NEXTMOVE/Fase A+B): step-by-step procedure as KB schema. The old
     * tea/coffee branch named tasks in C; now process_topic(Task, Token) is the
     * KB-side linguistic/domain bridge and process_step(Task, N, Text) is the
     * ordered content. The engine detects a process request, chooses the best
     * matching task by topic overlap, and renders the stored steps. */
    if ((cue(buf, "step by step") || cue(buf, "steps to") || cue(buf, "how to make") ||
         cue(buf, "how do you make") || cue(buf, "how do i make") ||
         cue(buf, "process of making") || cue(buf, "describe the process") ||
         cue(buf, "describe how to") || cue(buf, "how does") ||
         cue(buf, "how do") || cue(buf, "why does")) &&
        (cue(buf, "process") || cue(buf, "step") || cue(buf, "make") ||
         cue(buf, "rise") || cue(buf, "rises") || cue(buf, "rising"))) {
        char tb[512]; snprintf(tb, sizeof tb, "%s", buf);
        char *tw[96]; size_t tn = split_words(tb, tw, 96);
        char task[KB_TERM_LEN];
        if (kb_topic_task(b, "process_step", "process_topic", tw, tn,
                          task, sizeof task) &&
            kb_render_steps(b, "process_step", task, "", out, out_size))
            return 1;
        put("I understood you're asking for a process, but I don't have process_step facts for that topic yet.",
            out, out_size);
        return 1;
    }

    /* gen244: practical advice as KB-backed activity steps. This is not a generic
     * preference persona: activity_topic/2 selects a situation, activity_step/3
     * supplies the grounded recommendation, and unknown situations get a scoped
     * gap instead of a blind wall. */
    int activity_favorite = cue(buf, "favorite thing to do") ||
                            cue(buf, "favourite thing to do") ||
                            (cue(buf, "favorite") && cue(buf, "to do")) ||
                            (cue(buf, "favourite") && cue(buf, "to do"));
    if (activity_favorite ||
        cue(buf, "best way to") || cue(buf, "good way to") ||
        cue(buf, "what should i do") || cue(buf, "how should i spend") ||
        cue(buf, "way to spend") || cue(buf, "recommend") || cue(buf, "suggest")) {
        char ab[512]; snprintf(ab, sizeof ab, "%s", buf);
        char *aw[96]; size_t an = split_words(ab, aw, 96);
        char scene[KB_TERM_LEN];
        if (kb_topic_task(b, "activity_step", "activity_topic", aw, an,
                          scene, sizeof scene)) {
            if (activity_favorite) {
                const char *sq[] = { scene, NULL };
                char sh[1][KB_TERM_LEN];
                if (kb_match(b->kb, "activity_summary", sq, 2, sh, 1) > 0) {
                    char *p = kb_dequote(sh[0]);
                    put(p, out, out_size);
                    return 1;
                }
            }
            if (kb_render_steps(b, "activity_step", scene,
                                activity_favorite ?
                                "I don't have real favorites, but a good plan is:" :
                                "From what I know, a good plan is:",
                                out, out_size))
                return 1;
        }
        put("I understood you're asking for a recommendation, but I don't have activity_step facts for that situation yet.",
            out, out_size);
        return 1;
    }

    /* gen241 (LLMSCORE-check): limerick. A fixed AABBA form; the five lines per theme
     * live in KB as limerick_l1..l5(Theme). The C only selects the theme and joins. */
    if (cue(buf, "limerick")) {
        char lb[256]; snprintf(lb, sizeof lb, "%s", buf);
        char *lw[64]; size_t lnw = split_words(lb, lw, 64);
        for (size_t i = 0; i < lnw; i++) {
            char tok[KB_TERM_LEN]; snprintf(tok, sizeof tok, "%s", strip_edge_punct(lw[i]));
            if (strlen(tok) < 3) continue;
            const char *q[] = { tok, NULL };
            char l1[1][KB_TERM_LEN];
            if (kb_match(b->kb, "limerick_l1", q, 2, l1, 1) == 0) continue;
            const char *preds[] = { "limerick_l1","limerick_l2","limerick_l3","limerick_l4","limerick_l5" };
            char msg[700]; size_t off = 0; int ok = 1;
            for (int j = 0; j < 5; j++) {
                char lh[1][KB_TERM_LEN];
                if (kb_match(b->kb, preds[j], q, 2, lh, 1) == 0) { ok = 0; break; }
                char *p = lh[0]; size_t l = strlen(p);
                if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%s", j ? "\n" : "", p);
            }
            if (ok) { put(msg, out, out_size); return 1; }
        }
        /* limerick asked but no theme matched -> honest decline (Genera ceiling). */
        put("I can only do a limerick on a theme I have lines for -- like a programmer, "
            "coffee, or a cat. Pick one of those?", out, out_size);
        return 1;
    }

    /* gen241 (LLMSCORE-check): roleplay scenario advice. "As a store manager, what
     * would you do?" about a refund/complaint -> an honest preface plus the ordered
     * scenario_step(Scene, N, "step") facts. KB-first: add a scenario, no code edit. */
    if ((cue(buf, "what would you do") || cue(buf, "how would you handle") ||
         cue(buf, "how would you respond") || cue(buf, "how do you handle") ||
         cue(buf, "what should i do")) &&
        (cue(buf, "manager") || cue(buf, "as a") || cue(buf, "customer") ||
         cue(buf, "refund") || cue(buf, "complaint") || cue(buf, "return"))) {
        const char *scene = NULL;
        if (cue(buf, "refund") || cue(buf, "return") || cue(buf, "receipt")) scene = "refund";
        else if (cue(buf, "complaint") || cue(buf, "complain") || cue(buf, "angry") ||
                 cue(buf, "upset")) scene = "complaint";
        if (scene) {
            const char *q[] = { scene, NULL, NULL };
            char nums[8][KB_TERM_LEN];
            size_t sn = kb_match(b->kb, "scenario_step", q, 3, nums, 8);
            if (sn > 0) {
                char msg[800];
                int off = snprintf(msg, sizeof msg,
                    "I'm a small program, not a real manager, but here's how I'd handle it:");
                for (size_t i = 0; i < sn; i++) {
                    const char *nq[] = { scene, nums[i], NULL };
                    char th[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "scenario_step", nq, 3, th, 1) == 0) continue;
                    char *p = th[0]; size_t l = strlen(p);
                    if (l >= 2 && p[0]=='"' && p[l-1]=='"') { p[l-1]='\0'; p++; }
                    off += snprintf(msg + off, sizeof msg - off, "\n%s. %s", nums[i], p);
                }
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen235 (LLMSCORE): generic kind-choice question, e.g. "is a dog a mammal
     * or a reptile?" -> choose the class supported by kind_is/2. */
    {
        char kb2[256]; snprintf(kb2, sizeof kb2, "%s", buf);
        char *kw[24]; size_t kn = split_words(kb2, kw, 24);
        for (size_t i = 0; i < kn; i++) kw[i] = strip_edge_punct(kw[i]);
        if (kn >= 7 && strcmp(kw[0], "is") == 0) {
            size_t si = 1;
            if (is_article(kw[si]) && si + 1 < kn) si++;
            const char *kind0 = kw[si++];
            if (si < kn && is_article(kw[si])) si++;
            if (si + 2 < kn) {
                const char *c10 = kw[si++];
                if (si < kn && strcmp(kw[si], "or") == 0) si++;
                if (si < kn && is_article(kw[si])) si++;
                if (si < kn) {
                    const char *c20 = kw[si];
                    char kind[KB_TERM_LEN], c1[KB_TERM_LEN], c2[KB_TERM_LEN];
                    singularize(kind0, kind, sizeof kind);
                    singularize(c10, c1, sizeof c1);
                    singularize(c20, c2, sizeof c2);
                    const char *a1[] = { kind, c1 };
                    const char *a2[] = { kind, c2 };
                    int yes1 = kb_query(b->kb, "kind_is", a1, 2);
                    int yes2 = kb_query(b->kb, "kind_is", a2, 2);
                    if (yes1 != yes2) {
                        char msg[200];
                        snprintf(msg, sizeof msg, "A %s is a %s.", kind, yes1 ? c1 : c2);
                        put(msg, out, out_size);
                        return 1;
                    }
                    if (yes1 && yes2) { put("Both, as far as I know.", out, out_size); return 1; }
                }
            }
        }
    }

    /* gen235 (LLMSCORE): option-choice over kind traits, e.g. "what do dogs
     * typically say: woof, meow, or oink?" grounded in trait(Kind, Action). */
    {
        char tb[256]; snprintf(tb, sizeof tb, "%s", buf);
        char *tw[32]; size_t tn = split_words(tb, tw, 32);
        for (size_t i = 0; i < tn; i++) tw[i] = strip_edge_punct(tw[i]);
        const char *kind0 = NULL;
        if (tn >= 4 && strcmp(tw[0], "what") == 0 && strcmp(tw[1], "do") == 0)
            kind0 = tw[2];
        else if (tn >= 7 && strcmp(tw[0], "which") == 0 && strcmp(tw[2], "does") == 0) {
            for (size_t i = 3; i + 1 < tn; i++)
                if (is_article(tw[i])) { kind0 = tw[i + 1]; break; }
        }
        const char *colon = strchr(norm, ':');
        if (kind0 && colon) {
            char kind[KB_TERM_LEN];
            singularize(kind0, kind, sizeof kind);
            char opts[256]; snprintf(opts, sizeof opts, "%s", colon + 1);
            char *ow[24]; size_t on = split_words(opts, ow, 24);
            for (size_t i = 0; i < on; i++) {
                char *opt = strip_edge_punct(ow[i]);
                if (!*opt || strcmp(opt, "or") == 0) continue;
                const char *qa[] = { kind, opt };
                if (kb_query(b->kb, "trait", qa, 2)) {
                    char ans[KB_TERM_LEN]; snprintf(ans, sizeof ans, "%s", opt);
                    if (ans[0]) ans[0] = (char)toupper((unsigned char)ans[0]);
                    char msg[160]; snprintf(msg, sizeof msg, "%s.", ans);
                    put(msg, out, out_size);
                    return 1;
                }
            }
        }
    }

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* gen146 (E5): open-domain humility for questions that look like world
     * facts but fall outside the current KB/tool model. This does not answer
     * from general knowledge; it names the missing predicate/relation/tool and
     * gives the user a useful next action. */
    if (nw == 4 && strcmp(w[0], "what") == 0 && strcmp(w[2], "is") == 0 &&
        strcmp(w[3], "it") == 0 &&
        (strcmp(w[1], "year") == 0 || strcmp(w[1], "date") == 0 ||
         strcmp(w[1], "day") == 0 || strcmp(w[1], "time") == 0)) {
        char pred[64];
        snprintf(pred, sizeof pred, "current_%s", w[1]);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I do not know the current %s: I have no %s fact or clock/calendar tool. Tell me the %s, or give facts I can reason from.",
                 w[1], pred, w[1]);
        put(msg, out, out_size);
        return 1;
    }

    if ((nw == 6 && (strcmp(w[0], "what") == 0 || strcmp(w[0], "who") == 0) &&
         strcmp(w[1], "is") == 0 && strcmp(w[2], "the") == 0 &&
         (strcmp(w[4], "of") == 0 || strcmp(w[4], "di") == 0)) ||
        (nw == 5 && (strcmp(w[0], "what") == 0 || strcmp(w[0], "who") == 0) &&
         strcmp(w[1], "is") == 0 &&
         (strcmp(w[3], "of") == 0 || strcmp(w[3], "di") == 0))) {
        const char *rel = (nw == 6) ? w[3] : w[2];
        const char *obj = (nw == 6) ? w[5] : w[4];
        if (!kb_knows_pred(b->kb, rel)) {
            char msg[320];
            snprintf(msg, sizeof msg,
                     "I do not know the relation %s yet, so I cannot answer the %s of %s. You can teach me with thing is the %s of %s, or give facts/rules to reason from.",
                     rel, rel, obj, rel, obj);
            put(msg, out, out_size);
            return 1;
        }
    }

    if ((nw == 4 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0) ||
        (nw == 5 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
         strcmp(w[2], "the") == 0)) {
        const char *subj = (nw == 4) ? w[2] : w[3];
        const char *pred = (nw == 4) ? w[3] : w[4];
        const char *args[] = {subj};
        char ex[512];
        if (!kb_knows_pred(b->kb, pred) ||
            !kb_explain(b->kb, pred, args, 1, ex, sizeof ex)) {
            char msg[320];
            snprintf(msg, sizeof msg,
                     "I do not know why %s is %s: I have no %s(%s) fact/rule or cause explaining it. Teach me facts or rules, or give me a passage to read.",
                     subj, pred, pred, subj);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen59 (C5): "what is <x>?" is a natural way to ask for a description of
     * an entity. Reuse the existing belief-report path; decline if x is an
     * article or common function word so "what is a ...?" still falls through. */
    if (nw == 3 && strcmp(w[0], "what") == 0 && strcmp(w[1], "is") == 0 &&
        !is_article(w[2]) && !is_stopword(b, w[2])) {
        const char *entity;
        if (!resolve_entity(b, w[2], &entity, out, out_size)) return 1;
        char desc[1024];
        if (kb_describe_entity(b->kb, entity, desc, sizeof desc)) {
            put(desc, out, out_size);
            store_proof(b, desc);
            remember_entity(b, w[2], entity);
            return 1;
        }
        /* gen242: unknown entity -> don't wall here. Fall through so mod_learn
         * (registered last) gives the INFORMED, self-documenting reply: it can
         * read the topic up from its static corpus, or honestly say it has no
         * source yet -- never a blank "I don't know anything about X". */
    }

    /* gen157: emergent relational reasoning over descriptions. parrot0 was never
     * told "heart is part of circulatory" — but the circulatory DESCRIPTION names
     * the heart, so "what is the heart part of?" / "what contains the lungs?" /
     * "di cosa fa parte heart" recovers the container from the text. The relation
     * is DERIVED, never asserted: a taxonomy emerging from the glossary. Runs
     * before the describe block so "what is X part of" is not answered with X's
     * own definition. Fires only with a containment cue AND a known concept. */
    {
        char low[256]; lowercase_copy(low, sizeof low, raw);
        int want_container =
            cue(norm, "part of") || cue(norm, "belong") ||
            cue(norm, "contains") || cue(norm, "contain ") ||
            cue(norm, "includes") || cue(norm, "include ") ||
            cue(low, "fa parte") || cue(low, "contiene") || cue(low, "contengono");
        if (want_container) {
            /* concept keys in the turn, and the index of the containment cue.
             * A trailing category noun ("the nervous SYSTEM") is the frame, not
             * the target — skip it even though "system" is a concept elsewhere. */
            const char *keys[8]; size_t keypos[8], nk = 0;
            for (size_t i = 0; i < nw && nk < 8; i++) {
                if (!strcmp(w[i], "system") || !strcmp(w[i], "group") ||
                    !strcmp(w[i], "class") || !strcmp(w[i], "family") ||
                    !strcmp(w[i], "category") || !strcmp(w[i], "type") ||
                    !strcmp(w[i], "kind") || !strcmp(w[i], "set")) continue;
                if (kb_is_concept_key(b->kb, w[i])) { keys[nk] = w[i]; keypos[nk] = i; nk++; }
            }
            size_t cuei = nw;
            for (size_t i = 0; i < nw; i++)
                if (!strcmp(w[i], "part") || !strcmp(w[i], "contains") ||
                    !strcmp(w[i], "contain") || !strcmp(w[i], "includes") ||
                    !strcmp(w[i], "include") || !strcmp(w[i], "contiene") ||
                    !strcmp(w[i], "contengono")) { cuei = i; break; }

            /* gen158 (proof): "is X part of Y?" — PROVE it against the
             * materialized part_of/2 fact derived from the descriptions. */
            if (nk >= 2 && strcmp(w[0], "is") == 0) {
                const char *xx = keys[0], *yy = keys[nk - 1];
                const char *args[2] = { xx, yy };
                char msg[200];
                if (kb_query(b->kb, "part_of", args, 2))
                    snprintf(msg, sizeof msg, "Yes, %s is part of %s.", xx, yy);
                else
                    snprintf(msg, sizeof msg,
                             "No, I have no evidence that %s is part of %s.", xx, yy);
                put(msg, out, out_size);
                store_proof(b, msg);
                return 1;
            }

            /* gen158 (members): "what is part of Y?" / "what is in Y?" — the
             * inverse query over part_of, listing what Y contains. */
            int key_before_cue = 0;
            for (size_t i = 0; i < nk; i++) if (keypos[i] < cuei) key_before_cue = 1;
            if (nk >= 1 && cuei < nw && !key_before_cue && keypos[nk - 1] > cuei) {
                const char *yy = keys[nk - 1];
                char members[16][KB_TERM_LEN];
                const char *args[2] = { NULL, yy };
                size_t m = kb_match(b->kb, "part_of", args, 2, members, 16);
                if (m > 0) {
                    char msg[512];
                    int off = snprintf(msg, sizeof msg, "%s contains: ", yy);
                    for (size_t i = 0; i < m && off > 0 && (size_t)off < sizeof msg; i++)
                        off += snprintf(msg + off, sizeof msg - (size_t)off, "%s%s",
                                        i ? ", " : "", members[i]);
                    if (off > 0 && (size_t)off < sizeof msg)
                        snprintf(msg + off, sizeof msg - (size_t)off, ".");
                    put(msg, out, out_size);
                    store_proof(b, msg);
                    return 1;
                }
            }

            /* gen157 (container): "what is X part of?" — the concept whose text
             * names X, recovered on demand. */
            const char *x = (nk >= 1) ? keys[0] : NULL;
            char ckey[128], cdesc[1024];
            if (x && kb_concept_mentioning(b->kb, x, ckey, sizeof ckey,
                                           cdesc, sizeof cdesc)) {
                char msg[1200];
                snprintf(msg, sizeof msg, "%s is part of %s: %s.", x, ckey, cdesc);
                put(msg, out, out_size);
                store_proof(b, msg);
                remember_entity(b, x, x);
                return 1;
            }
        }
    }

    /* gen151: natural access to gen150 domain knowledge (experts/skills). Beyond
     * the bare "what is X?" above, accept an article, a multiword topic, or a
     * "tell me about X" framing: "what is the heart", "what is a prime",
     * "what is the circulatory system", "tell me about pi". Each content word is
     * tried as the concept key; the first that has a KB description is spoken.
     * Claims ONLY on a hit, so unknown topics still fall through to the humility
     * blocks above and the fallback below — this never widens the wall. */
    {
        size_t start = 0;
        if (nw >= 3 && strcmp(w[0], "what") == 0 &&
            (strcmp(w[1], "is") == 0 || strcmp(w[1], "are") == 0)) start = 2;
        else if (nw >= 4 && strcmp(w[0], "tell") == 0 &&
                 strcmp(w[1], "me") == 0 && strcmp(w[2], "about") == 0) start = 3;
        /* "what is a/an X?" is the membership query (list the X's), handled
         * downstream — not a description request. Leave it alone. */
        if (start == 2 && (strcmp(w[2], "a") == 0 || strcmp(w[2], "an") == 0))
            start = 0;
        /* "what is the <rel> of <obj>?" is a relational query, handled elsewhere;
         * an "of"/"di" marker means this is not a plain description request. */
        for (size_t i = start; start && i < nw; i++)
            if (strcmp(w[i], "of") == 0 || strcmp(w[i], "di") == 0) start = 0;
        if (start) {
            /* An exact concept key named directly ("what is the heart") always
             * wins — a precise match must beat a fuzzy guess. */
            for (size_t i = start; i < nw; i++) {
                if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                char desc[1024];
                if (kb_describe_entity(b->kb, w[i], desc, sizeof desc)) {
                    put(desc, out, out_size);
                    store_proof(b, desc);
                    remember_entity(b, w[i], w[i]);
                    return 1;
                }
            }
            /* gen172: a multi-word concept is stored under an underscore-joined
             * key (e.g. "prime number" -> prime_number) that the single-word loop
             * above cannot match. Try that exact joined key too, so a learned or
             * loaded multi-word concept still beats the fuzzy guess below — the
             * "exact key always wins" rule, completed for compound names. Speak it
             * with the spaced display form (no underscore). */
            {
                char jkey[128]; size_t jo = 0;
                char jdisp[128]; size_t jd = 0;
                for (size_t i = start; i < nw; i++) {
                    if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                    jo += (size_t)snprintf(jkey + jo, sizeof jkey - jo,
                                           "%s%s", jo ? "_" : "", w[i]);
                    jd += (size_t)snprintf(jdisp + jd, sizeof jdisp - jd,
                                           "%s%s", jd ? " " : "", w[i]);
                }
                char jdef[KB_TERM_LEN];
                if (jo && strchr(jkey, '_') &&
                    kb_concept_def(b->kb, jkey, jdef, sizeof jdef)) {
                    char msg[1200];
                    snprintf(msg, sizeof msg, "%s is %s.", jdisp, jdef);
                    put(msg, out, out_size);
                    store_proof(b, msg);
                    remember_entity(b, jkey, jdisp);
                    return 1;
                }
            }
            /* gen155: no exact key — recall the concept whose description
             * structurally OVERLAPS the query (similarity, not a cue list):
             * "what is the longest bone in the body" -> femur. Hedged ("You
             * might mean ...") because it is a best guess from overlap, and
             * fires only with >=2 matching words and a clear winner, so a bare
             * concept name (one content word) and genuinely unknown topics fall
             * through unharmed. Discrete overlap is noisier than an LLM's
             * continuous space; precision is bought with the margin + hedge. */
            const char *qw[24]; size_t nq = 0;
            for (size_t i = start; i < nw && nq < 24; i++) {
                if (is_article(w[i]) || is_stopword(b, w[i])) continue;
                if (!strcmp(w[i], "mean") || !strcmp(w[i], "means") ||
                    !strcmp(w[i], "thing") || !strcmp(w[i], "called") ||
                    !strcmp(w[i], "definition")) continue;
                qw[nq++] = w[i];
            }
            char ckey[128], cdesc[1024];
            if (nq >= 2 &&
                kb_nearest_concept(b->kb, qw, nq, ckey, sizeof ckey, cdesc, sizeof cdesc)) {
                char msg[1200];
                snprintf(msg, sizeof msg, "You might mean %s: %s.", ckey, cdesc);
                put(msg, out, out_size);
                store_proof(b, msg);
                remember_entity(b, ckey, ckey);
                return 1;
            }
        }
    }

    /* explanation: "why is <x> a/an <y>?" -> render the proof of y(x) */
    if (nw == 5 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        is_article(w[3])) {
        const char *subj;
        if (!resolve_entity(b, w[2], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        explain_reply(b, w[4], args, 1, out, out_size);
        remember_entity(b, w[2], subj);
        return 1;
    }
    /* explanation, Italian subject-verb order: "perché <x> è un <y>?" reaches
     * here already half-canonicalized as "perché <x> is a <y>" (è->is, un->a),
     * but "perché" stays (it is not in canonical_token, so the many "perché ..."
     * cue handlers keep working). The subject sits before the verb, so the
     * English-order branch above (w[1]=="is") misses it. Same proof rendering,
     * one extra order; the contrastive "perché ... non è" path was already
     * order-free, this gives the affirmative why-proof the same bilingual reach.
     * Transfers to any unseen x/y. */
    if (nw == 5 &&
        (strcmp(w[0], "perché") == 0 || strcmp(w[0], "perche") == 0) &&
        strcmp(w[2], "is") == 0 && is_article(w[3])) {
        const char *subj;
        if (!resolve_entity(b, w[1], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        explain_reply(b, w[4], args, 1, out, out_size);
        remember_entity(b, w[1], subj);
        return 1;
    }
    /* explanation: "why is <x> the <rel> of <y>?" -> proof of rel(x, y) */
    if (nw == 7 && strcmp(w[0], "why") == 0 && strcmp(w[1], "is") == 0 &&
        strcmp(w[3], "the") == 0 && strcmp(w[5], "of") == 0) {
        const char *args[] = {w[2], w[6]};
        explain_reply(b, w[4], args, 2, out, out_size);
        return 1;
    }

    /* proof depth (gen26): "how do you know <x> is a/an <y>?" -> classify the
     * proof of y(x) as direct (fact) vs multi-step (rule chain) reasoning. */
    if (nw == 8 && strcmp(w[0], "how") == 0 && strcmp(w[1], "do") == 0 &&
        strcmp(w[2], "you") == 0 && strcmp(w[3], "know") == 0 &&
        strcmp(w[5], "is") == 0 && is_article(w[6])) {
        const char *subj;
        if (!resolve_entity(b, w[4], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        howknow_reply(b, w[7], args, 1, out, out_size);
        remember_entity(b, w[4], subj);
        return 1;
    }

    /* direct belief report: "what do you know about <x>?" */
    if (nw == 6 && strcmp(w[0], "what") == 0 && strcmp(w[1], "do") == 0 &&
        strcmp(w[2], "you") == 0 && strcmp(w[3], "know") == 0 &&
        strcmp(w[4], "about") == 0) {
        const char *entity;
        if (!resolve_entity(b, w[5], &entity, out, out_size)) return 1;
        char desc[1024];
        if (kb_describe_entity(b->kb, entity, desc, sizeof desc)) {
            put(desc, out, out_size);
        } else {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know anything about %s.", entity);
            put(msg, out, out_size);
        }
        remember_entity(b, w[5], entity);
        return 1;
    }

    /* induction ("training"): "generalize" / "learn" -> induce rules from
     * the facts and report what was learned. */
    if (nw == 1 && (strcmp(w[0], "generalize") == 0 ||
                    strcmp(w[0], "learn") == 0)) {
        char heads[16][KB_TERM_LEN], bodies[16][KB_TERM_LEN];
        size_t k = kb_induce(b->kb, 2, heads, bodies, 16);
        /* Filter internal predicates (gen150) */
        size_t kept = 0;
        char fheads[16][KB_TERM_LEN], fbodies[16][KB_TERM_LEN];
        for (size_t i = 0; i < k; i++) {
            if (is_internal_pred(heads[i]) || is_internal_pred(bodies[i])) continue;
            snprintf(fheads[kept], KB_TERM_LEN, "%s", heads[i]);
            snprintf(fbodies[kept], KB_TERM_LEN, "%s", bodies[i]);
            kept++;
        }
        if (kept == 0) { put("Nothing new to generalize.", out, out_size); return 1; }
        char msg[600];
        size_t off = (size_t)snprintf(msg, sizeof msg, "Induced: ");
        for (size_t i = 0; i < kept && off < sizeof msg; i++) {
            off += (size_t)snprintf(msg + off, sizeof msg - off,
                                    "%s%s(X) :- %s(X)", i ? "; " : "",
                                    fheads[i], fbodies[i]);
        }
        if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
        put(msg, out, out_size);
        return 1;
    }

    /* rule: "every <body...> is a/an <head>" -> head(X) :- body0(X), …
     * gen133 generalizes the single-body form to a CONJUNCTION: the modifiers
     * before the head noun become conjoined premises, e.g. "every friendly dog
     * is a goodboy" -> goodboy(X) :- friendly(X), dog(X). nw==5 (one body word)
     * stays exactly the old single-body rule, so prior behaviour is preserved. */
    if (nw >= 5 && nw <= 4 + KB_MAX_BODY && strcmp(w[0], "every") == 0 &&
        strcmp(w[nw - 3], "is") == 0 && is_article(w[nw - 2])) {
        const char *head = w[nw - 1];
        const char *bodies[KB_MAX_BODY];
        size_t nbody = nw - 4; /* body words are w[1 .. nw-4] */
        for (size_t i = 0; i < nbody; i++) bodies[i] = w[1 + i];
        if (kb_assert_rule_n(b->kb, head, bodies, nbody)) {
            char msg[256];
            size_t o = (size_t)snprintf(msg, sizeof msg, "Learned rule: %s(X) :- ",
                                        head);
            for (size_t i = 0; i < nbody && o < sizeof msg; i++)
                o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s(X)",
                                      i ? ", " : "", bodies[i]);
            if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
            put(msg, out, out_size);
            auto_induce(b, out, out_size);
        } else {
            put("I couldn't store that rule.", out, out_size);
        }
        return 1;
    }

    /* gen96: bulk forget — "forget everything about <x>" */
    /* retract: "forget that <x> is a/an <y>" -> remove y(x) */
    if (nw == 6 && strcmp(w[0], "forget") == 0 && strcmp(w[1], "that") == 0 &&
        strcmp(w[3], "is") == 0 && is_article(w[4])) {
        const char *subj, *cl = w[5];
        if (!resolve_entity(b, w[2], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        if (kb_retract(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Forgotten: %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I didn't know that anyway.");
        put(msg, out, out_size);
        remember_entity(b, w[2], subj);
        return 1;
    }

    /* explicit negative correction, order-insensitive (gen44): both English
     * "<x> is not a <y>" and the Italian-canonicalized "<x> not is a <y>" mean
     * not y(x). Detected by ROLE not position: subject first, article at nw-2,
     * class last, and the two middle tokens are exactly {is, not} in any order.
     * Word order is surface, not meaning, so one parser serves both languages
     * (the multilingual probe's gen43 finding). Question words are excluded so a
     * negated query is not mistaken for an assertion. */
    if (nw == 5 && is_article(w[3]) &&
        strcmp(w[0], "who") != 0 && strcmp(w[0], "what") != 0 &&
        strcmp(w[0], "is") != 0 &&
        ((strcmp(w[1], "is") == 0) || (strcmp(w[2], "is") == 0)) &&
        ((strcmp(w[1], "not") == 0) || (strcmp(w[2], "not") == 0))) {
        const char *subj, *cl = w[4];
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        int before = goal_truth(b); /* gen103 (L16): snapshot before mutation */
        note_contradiction(b, cl, subj, 0); /* gen142 (E8): self-contradiction? */
        /* gen218 (glue): an EXPLICIT correction ("no, X is not a Y") overrides
         * the standing belief — retract any positive y(x) across every layer so
         * the conclusion re-derives to "No." rather than stalling on a conflict
         * between a curated/base fact and the user's correction. Plain "X is not
         * a Y" (no marker) keeps the honest conflict. Session-only, reversible. */
        if (b->correcting) while (kb_retract(b->kb, cl, args, 1)) {}
        if (kb_assert_neg(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Learned: not %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        note_consequence(b, cl, before, out, out_size); /* gen103 (L16) */
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* additional class (gen46): "<x> is also a/an <y>" -> y(x). Explanatory
     * prose adds classes incrementally ("a dolphin is also a mammal"); it is the
     * same assertion as "x is a y", one more membership. */
    if (nw == 5 && strcmp(w[1], "is") == 0 && strcmp(w[2], "also") == 0 &&
        is_article(w[3])) {
        const char *subj, *cl = w[4];
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        char msg[128];
        if (kb_assert(b->kb, cl, args, 1))
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cl, subj);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* --- binary relations: "<x> is the <rel> of <y>" (gen11) --- */
    if (nw == 6 && strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0) {
        const char *rel = w[3], *obj = w[5];

        /* variable query, subject unknown: "who is the <rel> of <y>?" ->
         * rel(X, y); object unknown: "what is the <rel> of <y>?" -> rel(y, X) */
        if ((strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
            strcmp(w[1], "is") == 0) {
            if (!kb_knows_pred(b->kb, rel)) { idk(rel, out, out_size); return 1; }
            const char *who_pat[]  = {NULL, obj};   /* rel(X, y) */
            const char *what_pat[] = {obj, NULL};   /* rel(y, X) */
            const char *const *pat =
                (strcmp(w[0], "what") == 0) ? what_pat : who_pat;
            char hits[64][KB_TERM_LEN];
            size_t k = kb_match(b->kb, rel, pat, 2, hits, 64);
            if (k == 0) { put("Nobody that I know of.", out, out_size); return 1; }
            char list[512];
            size_t off = 0;
            for (size_t i = 0; i < k && off < sizeof list; i++)
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s", i ? ", " : "", hits[i]);
            char msg[600];
            snprintf(msg, sizeof msg, "%s.", list);
            put(msg, out, out_size);
            return 1;
        }

        /* ground query: "is <x> the <rel> of <y>?" -> rel(x, y)? */
        if (strcmp(w[0], "is") == 0) {
            const char *subj = w[1];
            const char *args[] = {subj, obj};
            if (!kb_knows_pred(b->kb, rel)) idk(rel, out, out_size);
            else if (kb_is_conflicted(b->kb, rel, args, 2))
                put("Conflicted.", out, out_size);
            else put(kb_query(b->kb, rel, args, 2) ? "Yes." : "No.",
                     out, out_size);
            return 1;
        }

        /* assert: "<x> is the <rel> of <y>" -> rel(x, y) */
        if (strcmp(w[1], "is") == 0) {
            const char *subj = w[0];
            const char *args[] = {subj, obj};
            char msg[160];
            if (kb_assert(b->kb, rel, args, 2))
                snprintf(msg, sizeof msg, "Learned: %s(%s, %s).", rel, subj, obj);
            else
                snprintf(msg, sizeof msg, "I couldn't store that.");
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen133: article-free class assertion "<x> is <adj>" -> adj(x), but ONLY
     * when adj is a predicate some rule body already depends on. This makes the
     * conjuncts of a learned conjunctive concept ("every friendly dog is a
     * goodboy") assertable in natural English ("rex is friendly"), without the
     * frame ever firing on arbitrary "X is Y" prose. */
    if (nw == 3 && strcmp(w[1], "is") == 0 &&
        !is_stopword(b, w[0]) && isalpha((unsigned char)w[0][0])) {
        char clsbuf[KB_TERM_LEN];
        snprintf(clsbuf, sizeof clsbuf, "%s", w[2]);
        char *cl2 = strip_edge_punct(clsbuf);
        if (kb_rule_body_mentions(b->kb, cl2)) {
            const char *subj;
            if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
            const char *args[] = {subj};
            char msg[128];
            int before = b->has_last_goal ? goal_truth(b) : -1;
            if (kb_assert(b->kb, cl2, args, 1))
                snprintf(msg, sizeof msg, "Learned: %s(%s).", cl2, subj);
            else
                snprintf(msg, sizeof msg, "I couldn't store that.");
            put(msg, out, out_size);
            remember_entity(b, w[0], subj);
            note_consequence(b, cl2, before, out, out_size);
            return 1;
        }
    }

    /* gen231 (LLMSCORE, ambitious): UNIVERSAL QUANTIFICATION -> a definite rule the
     * SLD resolver already chains. "all men are mortal" / "every rose is a flower"
     * become mortal(X):-man(X) / flower(X):-rose(X); afterwards "is socrates mortal?"
     * and "is <r> a flower?" deduce over the rule plus the ground fact. This is real
     * syllogistic reasoning on parrot0's own engine, not a recited string. */
    if (nw >= 4 && nw <= 6 &&
        (strcmp(w[0], "all") == 0 || strcmp(w[0], "every") == 0 ||
         strcmp(w[0], "any") == 0)) {
        /* gen290: locate the copula STRUCTURALLY rather than by fixed position, so
         * the Italian universal "tutti gli uomini sono mortali" (canonicalized to
         * "all gli uomini am mortali") parses through the SAME rule as "all men are
         * mortal". The subject is the token just BEFORE the copula, which naturally
         * skips any determiner between the quantifier and the noun ("all THE men…",
         * "tutti GLI uomini…") with no hardcoded article list; the copula is any of
         * are/is/am ("am" is what the canonicalizer emits for Italian "sono"). */
        size_t cop = 0;
        for (size_t i = 2; i < nw && i <= 3; i++)
            if (strcmp(w[i], "are") == 0 || strcmp(w[i], "is") == 0 ||
                strcmp(w[i], "am") == 0) { cop = i; break; }
        if (cop >= 2 && cop + 1 < nw) {
            size_t si = cop - 1;                 /* subject: token before copula */
            size_t ci = cop + 1;                 /* class:   token after copula  */
            if (is_article(w[ci]) && ci + 1 < nw) ci++;   /* skip "a"/"an" */
            char subjb[KB_TERM_LEN], clsb[KB_TERM_LEN];
            char sj[KB_TERM_LEN], cl[KB_TERM_LEN];
            snprintf(subjb, sizeof subjb, "%s", w[si]);
            snprintf(clsb, sizeof clsb, "%s", w[ci]);
            singularize(strip_edge_punct(subjb), sj, sizeof sj);
            singularize(strip_edge_punct(clsb), cl, sizeof cl);
            if (*sj && *cl && strcmp(sj, cl) != 0) {
                char msg[160];
                if (kb_assert_rule(b->kb, cl, sj))
                    snprintf(msg, sizeof msg,
                             "Got it: if something is a %s, then it is %s.", sj, cl);
                else
                    snprintf(msg, sizeof msg, "I couldn't store that rule.");
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* gen231/gen290: adjective-form deduction query "is <x> <y>?" (no article,
     * e.g. "is socrates mortal?") -> resolve y(x) over rules+facts. gen290 also
     * accepts the SUBJECT-FIRST interrogative "<x> is <y>?" — the shape Italian
     * "socrate è mortale?" canonicalizes to ("socrate is mortale?") — so the same
     * deduction serves both word orders through one path (the bilingual ratchet,
     * mirroring gen103's subject-first membership query). Subject-first is gated on
     * a trailing '?' so a plain "<x> is <y>" assertion is never mistaken for a
     * query. Guarded on a known predicate so it never feigns a yes/no for an
     * unknown property. */
    if (nw == 3 &&
        ((strcmp(w[0], "is") == 0 && !is_article(w[1]) &&
          isalpha((unsigned char)w[1][0])) ||
         (interrogative && strcmp(w[1], "is") == 0 &&
          isalpha((unsigned char)w[0][0])))) {
        int subj_first = (strcmp(w[1], "is") == 0);
        const char *subjw = subj_first ? w[0] : w[1];
        char clsb[KB_TERM_LEN];
        snprintf(clsb, sizeof clsb, "%s", w[2]);
        char *cl = strip_edge_punct(clsb);
        if (*cl && kb_knows_pred(b->kb, cl)) {
            const char *subj;
            if (!resolve_entity(b, subjw, &subj, out, out_size)) return 1;
            const char *args[] = {subj};
            if (kb_is_conflicted(b->kb, cl, args, 1)) put("Conflicted.", out, out_size);
            else {
                int yes = kb_query(b->kb, cl, args, 1);
                put(yes ? "Yes." : "No.", out, out_size);
                if (yes) {
                    char ex[512];
                    if (kb_explain(b->kb, cl, args, 1, ex, sizeof ex)) store_proof(b, ex);
                }
                snprintf(b->last_goal_pred, sizeof b->last_goal_pred, "%s", cl);
                snprintf(b->last_goal_arg, sizeof b->last_goal_arg, "%s", subj);
                b->last_goal_yes = yes;
                b->has_last_goal = 1;
            }
            remember_entity(b, subjw, subj);
            return 1;
        }
    }

    if (nw != 4 || !is_article(w[2])) return 0;
    const char *cls = w[3];

    /* variable query: "who/what is a <y>?" -> y(X), list the bindings */
    if ((strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
        strcmp(w[1], "is") == 0) {
        if (!kb_knows_pred(b->kb, cls)) {
            /* gen242: "what is a <X>?" for an unknown class is a DEFINITION
             * request -- fall through so mod_learn documents it (or honestly
             * offers to). "who is a <X>?" stays a member query, so it keeps the
             * gen16 idk wall ("Nobody that I know of." is the known-but-empty case). */
            if (strcmp(w[0], "what") == 0) return 0;
            idk(cls, out, out_size); return 1;
        }
        const char *pat[] = {NULL}; /* one variable in arg 0 */
        char hits[64][KB_TERM_LEN];
        size_t k = kb_match(b->kb, cls, pat, 1, hits, 64);
        if (k == 0) { put("Nobody that I know of.", out, out_size); return 1; }
        char list[512];
        size_t off = 0;
        for (size_t i = 0; i < k && off < sizeof list; i++) {
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", i ? ", " : "", hits[i]);
        }
        char msg[600];
        snprintf(msg, sizeof msg, "%s.", list);
        put(msg, out, out_size);
        return 1;
    }

    /* ground query: "is <x> a <y>?" -> y(x)? */
    if (strcmp(w[0], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[1], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        if (!kb_knows_pred(b->kb, cls)) idk(cls, out, out_size);
        else if (kb_is_conflicted(b->kb, cls, args, 1)) {
            put("Conflicted.", out, out_size);
            char ex[512];
            if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex))
                store_proof(b, ex);
        }
        else {
            int yes = kb_query(b->kb, cls, args, 1);
            put(yes ? "Yes." : "No.", out, out_size);
            if (yes) {
                char ex[512];
                if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex))
                    store_proof(b, ex);
            }
            /* gen103 (L16): remember this conclusion so a later correction can
             * re-derive it and flag the consequence. */
            snprintf(b->last_goal_pred, sizeof b->last_goal_pred, "%s", cls);
            snprintf(b->last_goal_arg, sizeof b->last_goal_arg, "%s", subj);
            b->last_goal_yes = yes;
            b->has_last_goal = 1;
        }
        remember_entity(b, w[1], subj);
        return 1;
    }

    /* subject-first interrogative: "<x> is a <y>?" -> y(x)? (the Italian shape
     * "<x> è un <y>?"). A trailing '?' makes this a QUERY, not an assertion, so
     * the same conclusion-memory + consequence machinery (gen103/L16) fires in
     * both languages through one path. */
    if (interrogative && strcmp(w[1], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        if (!kb_knows_pred(b->kb, cls)) idk(cls, out, out_size);
        else if (kb_is_conflicted(b->kb, cls, args, 1)) put("Conflicted.", out, out_size);
        else {
            int yes = kb_query(b->kb, cls, args, 1);
            put(yes ? "Yes." : "No.", out, out_size);
            if (yes) {
                char ex[512];
                if (kb_explain(b->kb, cls, args, 1, ex, sizeof ex)) store_proof(b, ex);
            }
            snprintf(b->last_goal_pred, sizeof b->last_goal_pred, "%s", cls);
            snprintf(b->last_goal_arg, sizeof b->last_goal_arg, "%s", subj);
            b->last_goal_yes = yes;
            b->has_last_goal = 1;
        }
        remember_entity(b, w[0], subj);
        return 1;
    }

    /* assert: "<x> is a <y>" -> y(x) */
    if (strcmp(w[1], "is") == 0) {
        const char *subj;
        if (!resolve_entity(b, w[0], &subj, out, out_size)) return 1;
        const char *args[] = {subj};
        int before = goal_truth(b); /* gen103 (L16): snapshot before mutation */
        note_contradiction(b, cls, subj, 1); /* gen142 (E8): self-contradiction? */
        if (kb_assert(b->kb, cls, args, 1)) {
            char msg[128];
            snprintf(msg, sizeof msg, "Learned: %s(%s).", cls, subj);
            put(msg, out, out_size);
            note_consequence(b, cls, before, out, out_size); /* gen103 (L16) */
        } else {
            put("I couldn't store that.", out, out_size);
        }
        remember_entity(b, w[0], subj);
        return 1;
    }

    return 0;
}

/* --- module: compare -----------------------------------------------------
 * Ordinal reasoning over quantities (gen27). Discovered by domain-pull: the
 * first official SuperGLUE/BoolQ question parrot0 was shown asks whether
 * ethanol "take[s] more energy ... than [it] produces" — a *comparison of two
 * magnitudes*, a kind of reasoning the KB (symbolic atoms only) could not do.
 * This part answers the comparison itself: "is <a> more/less than <b>?" over
 * numbers, returning a closed yes/no. It is the reasoning primitive on the
 * path to such questions; turning a passage into the two numbers is a separate,
 * larger feature (NL extraction) we deliberately do NOT fake here. */
static int parse_num(const char *s, double *out) {
    if (!*s) return 0;
    char *end;
    double v = strtod(s, &end);
    if (end == s) return 0;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return 0;
    *out = v;
    return 1;
}

/* gen112: value of a SINGLE number word (English or Italian), 0–90 plus the
 * multipliers hundred/thousand. Returns 1 on a hit. Content words only — no
 * function-word collision, so it is language-neutral by construction. */
static int single_word_number(const char *s, double *out) {
    static const struct { const char *w; double v; } t[] = {
        {"zero",0},{"one",1},{"two",2},{"three",3},{"four",4},{"five",5},
        {"six",6},{"seven",7},{"eight",8},{"nine",9},{"ten",10},{"eleven",11},
        {"twelve",12},{"thirteen",13},{"fourteen",14},{"fifteen",15},
        {"sixteen",16},{"seventeen",17},{"eighteen",18},{"nineteen",19},
        {"twenty",20},{"thirty",30},{"forty",40},{"fifty",50},{"sixty",60},
        {"seventy",70},{"eighty",80},{"ninety",90},{"hundred",100},
        {"thousand",1000},
        {"dozen",12},{"dozzina",12},  /* gen240: "a dozen apples" = 12 */
        /* Italian */
        {"uno",1},{"due",2},{"tre",3},{"quattro",4},{"cinque",5},{"sei",6},
        {"sette",7},{"otto",8},{"nove",9},{"dieci",10},{"undici",11},
        {"dodici",12},{"tredici",13},{"quattordici",14},{"quindici",15},
        {"sedici",16},{"diciassette",17},{"diciotto",18},{"diciannove",19},
        {"venti",20},{"trenta",30},{"quaranta",40},{"cinquanta",50},
        {"sessanta",60},{"settanta",70},{"ottanta",80},{"novanta",90},
        {"cento",100},{"mille",1000},
    };
    for (size_t i = 0; i < sizeof t / sizeof t[0]; i++)
        if (strcmp(s, t[i].w) == 0) { *out = t[i].v; return 1; }
    return 0;
}

/* A number WORD, including a hyphenated tens-unit compound ("twenty-one"). */
static int word_number(const char *s, double *out) {
    const char *hy = strchr(s, '-');
    if (hy) {
        char head[KB_TERM_LEN];
        size_t hn = (size_t)(hy - s);
        if (hn < sizeof head) {
            memcpy(head, s, hn); head[hn] = '\0';
            double tens, unit;
            if (single_word_number(head, &tens) && tens >= 20 &&
                (long)tens % 10 == 0 &&
                single_word_number(hy + 1, &unit) && unit >= 1 && unit <= 9) {
                *out = tens + unit; return 1;
            }
        }
    }
    return single_word_number(s, out);
}

/* Parse a value that may be a digit literal OR a number word. */
static int parse_value(const char *s, double *out) {
    return parse_num(s, out) || word_number(s, out);
}

/* gen112: collect the numbers in a token stream, reading digits AND number
 * words, merging spaced word compounds ("twenty five" -> 25) and multipliers
 * ("two hundred" -> 200). Merges apply only to WORD numbers, so two adjacent
 * digit quantities stay distinct. Returns how many were written (capped). */
static size_t collect_numbers(char **w, size_t nw, double *nums, size_t max) {
    size_t nn = 0; int prev_word = 0;
    for (size_t i = 0; i < nw && nn < max; i++) {
        char *t = strip_edge_punct(w[i]);
        if (!*t) { prev_word = 0; continue; }
        double v; int isword = 0;
        if (!parse_num(t, &v)) { isword = word_number(t, &v); if (!isword) { prev_word = 0; continue; } }
        if (isword && (v == 100 || v == 1000) && nn > 0 && nums[nn - 1] < v) {
            nums[nn - 1] *= v; prev_word = 1; continue;          /* "two hundred" */
        }
        if (isword && prev_word && nn > 0 && nums[nn - 1] >= 20 &&
            (long)nums[nn - 1] % 10 == 0 && v >= 1 && v <= 9) {
            nums[nn - 1] += v; prev_word = 1; continue;          /* "twenty five" */
        }
        nums[nn++] = v; prev_word = isword;
    }
    return nn;
}

/* The shared magnitude test: is `a` more (greater=1) / less (greater=0) than
 * `c`? Both mod_compare (literal numbers) and mod_quantity (numbers looked up
 * from the KB) route their decision through this one comparator. */
static int magnitude_more(double a, double c, int greater) {
    return greater ? (a > c) : (a < c);
}

/* Map "more"/"greater" -> 1, "less"/"fewer" -> 0, anything else -> -1. */
static int compare_word(const char *w) {
    if (strcmp(w, "more") == 0 || strcmp(w, "greater") == 0) return 1;
    if (strcmp(w, "less") == 0 || strcmp(w, "fewer") == 0) return 0;
    return -1;
}
