static int mod_repair(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    if (!b) return 0;

    /* -- RESUME: a clarification is open; this turn should fill the slot. ---- */
    if (b->pending_repair) {
        b->pending_repair = 0;               /* single-turn window: consume it */
        char saved_canon[256], saved_slot[16];
        snprintf(saved_canon, sizeof saved_canon, "%s", b->pending_canon);
        snprintf(saved_slot,  sizeof saved_slot,  "%s", b->pending_slot);

        char tb[256]; snprintf(tb, sizeof tb, "%s", norm);
        char *tw[64]; size_t tnw = split_words(tb, tw, 64);
        if (tnw == 0) return 0;

        /* If the user changed the subject instead of answering, expire and let
         * normal dispatch handle this turn fresh (acceptance: repair state
         * expires when the topic clearly changes). */
        if (is_question_opener(tw[0]) || is_intent_starter(tw[0]) ||
            strstr(norm, "refer to"))
            return 0;

        /* Let a teachable answer act first: "rex is a dog" asserts the fact and
         * sets the discourse entity, so the stored pronoun resolves by coref. */
        int had = b->has_last_entity;
        char prev[KB_TERM_LEN];
        snprintf(prev, sizeof prev, "%s", b->last_entity);
        char ans[512] = "";
        repair_dispatch(b, norm, raw, ans, sizeof ans);
        int entity_set = b->has_last_entity &&
                         (!had || strcmp(prev, b->last_entity) != 0);

        char resumed[256];
        if (entity_set) {
            snprintf(resumed, sizeof resumed, "%s", saved_canon);
        } else {
            /* substitute the slot with a concrete referent token from the answer
             * (the last content word/number: "the dog rex" -> rex, "21" -> 21). */
            const char *ref = NULL;
            for (size_t i = tnw; i-- > 0; ) {
                char *t = strip_edge_punct(tw[i]);
                if (!*t || is_entity_pronoun(t) || is_stopword(b, t)) continue;
                if (isalpha((unsigned char)t[0]) || isdigit((unsigned char)t[0])) {
                    ref = t; break;
                }
            }
            if (!ref) return 0;              /* no usable answer -> expire */
            char cb[256]; snprintf(cb, sizeof cb, "%s", saved_canon);
            char *cw[64]; size_t cnw = split_words(cb, cw, 64);
            size_t pos = 0; int replaced = 0; resumed[0] = '\0';
            for (size_t i = 0; i < cnw && pos < sizeof resumed; i++) {
                const char *piece = cw[i];
                char bare[64]; snprintf(bare, sizeof bare, "%s", cw[i]);
                if (!replaced && strcmp(strip_edge_punct(bare), saved_slot) == 0) {
                    piece = ref; replaced = 1;
                }
                pos += (size_t)snprintf(resumed + pos, sizeof resumed - pos,
                                        "%s%s", i ? " " : "", piece);
            }
            if (!replaced) snprintf(resumed, sizeof resumed, "%s", saved_canon);
        }

        repair_dispatch(b, resumed, raw, out, out_size);
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "repair");
        return 1;
    }

    /* -- OPEN: detect a referential gap and ask a narrow clarification. ------ */
    char buf[256]; snprintf(buf, sizeof buf, "%s", norm);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;
    if (!is_question_opener(w[0])) return 0;     /* only questions/commands */
    if (strstr(norm, "refer to")) return 0;      /* WSC coref judgement (mod_same) */
    if (b->has_last_entity) return 0;            /* a referent is available */
    /* gen250: in contrast questions, canonicalized "it's" becomes the grammatical
     * phrase "it is". That is an object of comparison ("its" vs "it is"), not a
     * discourse pronoun asking for an antecedent, so let mod_knowledge parse it. */
    if (strstr(norm, "difference between") && strstr(norm, " it is"))
        return 0;
    /* gen146 (E5): "what year/time/date/day is it" is not a referential
     * gap. There is no hidden entity for "it"; the honest gap is that parrot0
     * has no clock/calendar fact or tool, so let the knowledge humility path
     * name that instead of opening a repair loop. */
    if (nw == 4 && strcmp(w[0], "what") == 0 && strcmp(w[2], "is") == 0 &&
        strcmp(strip_edge_punct(w[3]), "it") == 0 &&
        (strcmp(w[1], "year") == 0 || strcmp(w[1], "date") == 0 ||
         strcmp(w[1], "day") == 0 || strcmp(w[1], "time") == 0))
        return 0;

    /* gen311 (F., 2026-07-11): "what does it mean when someone says 'X'?" is an
     * idiom-MEANING query, not a referential gap — that "it" is an anticipatory
     * dummy, not an anaphor. Route it to mod_knowledge's idiom_meaning consumer:
     * if a stored idiom_meaning phrase occurs verbatim in a "mean" turn, pass so
     * first-match-wins reaches the consumer that answers it. */
    if (b->kb && strstr(norm, "mean")) {
        char ph[64][KB_TERM_LEN];
        const char *anyq[] = { NULL, NULL };
        size_t pn = kb_match(b->kb, "idiom_meaning", anyq, 2, ph, 64);
        for (size_t i = 0; i < pn; i++) {
            char *key = ph[i]; size_t kl = strlen(key);
            if (kl >= 2 && key[0] == '"' && key[kl - 1] == '"') { key[kl - 1] = '\0'; key++; }
            if (*key && strstr(norm, key)) return 0;   /* idiom_meaning will answer */
        }
    }

    /* gen311 (F.): a RIDDLE turn is not a coref gap — "what has to be broken before
     * you can use IT" binds "it" to the riddle's own answer, not a discourse entity.
     * If every cue of some riddle_sig id occurs in the turn, pass so the riddle
     * consumer (mod_knowledge) can answer instead of opening a clarification. */
    if (b->kb) {
        char rids[64][KB_TERM_LEN];
        const char *rq[] = { NULL, NULL };
        size_t nr = kb_match(b->kb, "riddle_sig", rq, 2, rids, 64);
        char seen[64][KB_TERM_LEN]; size_t nseen = 0;
        for (size_t i = 0; i < nr; i++) {
            int dup = 0;
            for (size_t j = 0; j < nseen; j++) if (!strcmp(seen[j], rids[i])) dup = 1;
            if (dup || nseen >= 64) continue;
            snprintf(seen[nseen++], KB_TERM_LEN, "%s", rids[i]);
            const char *cq[] = { rids[i], NULL };
            char cues[8][KB_TERM_LEN];
            size_t nc = kb_match(b->kb, "riddle_sig", cq, 2, cues, 8);
            if (nc < 2) continue;
            int all = 1;
            for (size_t c = 0; c < nc && all; c++) {
                char *cu = cues[c]; size_t cl = strlen(cu);
                if (cl >= 2 && cu[0] == '"' && cu[cl - 1] == '"') { cu[cl - 1] = '\0'; cu++; }
                if (!*cu || !strstr(norm, cu)) all = 0;
            }
            if (all) return 0;   /* the riddle consumer will answer */
        }
    }

    const char *pron = NULL;
    for (size_t i = 1; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (is_entity_pronoun(t)) { pron = t; break; }
    }
    if (!pron) return 0;

    /* gen239 (kb-first manifesto, same-sentence coref): if the pronoun is
     * "it" and a token EARLIER in the same turn is a known substrate entity
     * (e.g. category_member(country, _)), DERIVE the antecedent from KB facts
     * instead of opening a clarification window. This is the deduci move for
     * same-clause anaphora: the referent is structurally recoverable, so
     * asking the user to repeat it is the wrong move. We substitute the slot
     * and re-dispatch the rest of the registry (skipping repair itself). The
     * window is NOT opened — repair stays out of the exchange. */
    if (b->kb && strcmp(pron, "it") == 0) {
        const char *resolved = NULL;
        for (size_t i = 0; i + 1 < nw && !resolved; i++) {
            char *t = strip_edge_punct(w[i]);
            if (!*t || !isalpha((unsigned char)t[0])) continue;
            const char *cq[] = { "country", t };
            if (kb_query(b->kb, "category_member", cq, 2)) resolved = t;
        }
        if (resolved) {
            char rebuilt[256]; size_t o = 0; int rep = 0;
            for (size_t i = 0; i < nw; i++) {
                char bare[64]; snprintf(bare, sizeof bare, "%s", w[i]);
                char *t = strip_edge_punct(bare);
                const char *piece = w[i];
                if (!rep && strcmp(t, pron) == 0) { piece = resolved; rep = 1; }
                o += (size_t)snprintf(rebuilt + o, sizeof rebuilt - o,
                                      "%s%s", i ? " " : "", piece);
                if (o + 1 >= sizeof rebuilt) break;
            }
            if (rep) {
                repair_dispatch(b, rebuilt, raw, out, out_size);
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "repair");
                return 1;
            }
        }
    }

    snprintf(b->pending_canon, sizeof b->pending_canon, "%s", norm);
    snprintf(b->pending_slot, sizeof b->pending_slot, "%s", pron);
    b->pending_repair = 1;

    char msg[160];
    if (has_arith_cue(b, w, nw))
        snprintf(msg, sizeof msg, "What number should I use for \"%s\"?", pron);
    else
        snprintf(msg, sizeof msg, "Who or what does \"%s\" refer to?", pron);
    put(msg, out, out_size);
    return 1;
}

/* --- module: whatifnot (gen129, L20-deep / L16) -------------------------
 * The EPISTEMIC sibling of mod_counterfactual. gen128 asked "what would you say
 * without module X?" (counterfactual over the CONTROL FLOW). This asks "what
 * would you conclude if you didn't know that <fact>?" — a counterfactual over
 * the KNOWLEDGE. It HYPOTHETICALLY retracts a believed fact from the live KB,
 * RE-DERIVES the last stated conclusion, reports whether it still stands, then
 * RESTORES the fact — footprint-free, exactly the gen128 discipline one level
 * down (belief instead of module). This is how the agent learns which of its
 * conclusions DEPEND on which of its beliefs: dependency made explicit by
 * removal, not asserted. Builds on gen103's re-derivation (note_consequence)
 * but driven by a hypothetical un-knowing rather than a real correction.
 */

/* Parse a simple ground unary fact clause ("socrates is a man" / "socrates is
 * mortal") into pred + single arg. Returns 1 on success. */
static int parse_ground_unary(const char *clause, char *pred, size_t ps,
                              char *arg, size_t as) {
    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    size_t n = strlen(buf);
    while (n > 0 && (buf[n-1]=='?'||buf[n-1]=='.'||buf[n-1]==','||buf[n-1]==' '))
        buf[--n] = '\0';
    char *w[12];
    size_t nw = split_words(buf, w, 12);
    if (nw >= 4 && strcmp(w[1], "is") == 0 && is_article(w[2])) {
        snprintf(pred, ps, "%s", w[3]); snprintf(arg, as, "%s", w[0]); return 1;
    }
    if (nw == 3 && strcmp(w[1], "is") == 0) {
        snprintf(pred, ps, "%s", w[2]); snprintf(arg, as, "%s", w[0]); return 1;
    }
    return 0;
}

static int mod_whatifnot(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* locate the un-known fact after the trigger phrase (read from raw so the
     * fact clause keeps its original words for parsing). */
    const char *fact = NULL;
    static const char *const markers[] = {
        "didn't know that ", "didnt know that ",
        "did not know that ", "didn't know ", "didnt know ", "did not know ",
        "non sapessi che ", "non sapessi ",
        NULL,
    };
    for (const char *const *m = markers; *m; m++) {
        const char *p = strstr(low, *m);
        if (p) { fact = p + strlen(*m); break; }
    }
    /* alternative shape: "if <subject> weren't / were not a <class>" */
    char alt[128] = "";
    if (!fact) {
        const char *p = strstr(low, "if ");
        const char *neg = NULL;
        if (p) {
            const char *cands[] = {" weren't ", " werent ", " were not ",
                                   " wasn't ", " wasnt ", " was not ",
                                   " non fosse ", NULL};
            for (size_t i = 0; cands[i]; i++) {
                const char *q = strstr(p, cands[i]);
                if (q) { neg = q;
                    /* subject = words between "if " and the negation */
                    size_t slen = (size_t)(neg - (p + 3));
                    char subj[64]; if (slen >= sizeof subj) slen = sizeof subj - 1;
                    memcpy(subj, p + 3, slen); subj[slen] = '\0';
                    const char *cls = neg + strlen(cands[i]);
                    /* class = up to comma/question */
                    char clsbuf[64]; size_t k = 0;
                    while (cls[k] && cls[k] != ',' && cls[k] != '?' && k+1 < sizeof clsbuf)
                        { clsbuf[k] = cls[k]; k++; }
                    clsbuf[k] = '\0';
                    snprintf(alt, sizeof alt, "%s is %s", subj, clsbuf);
                    fact = alt;
                    break;
                }
            }
        }
    }
    if (!fact) return 0;

    /* must be a genuine "what would you conclude / would X still" question */
    int q = cue(low, "conclude") || cue(low, "still") || cue(low, "concluderesti") ||
            cue(low, "ancora") || strstr(low, "what would") || strstr(low, "cosa");
    if (!q) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can tell you what that "
            "conclusion rests on.", out, out_size);
        return 1;
    }

    /* parse the fact (canonicalized so Italian clauses go through one path) */
    char fc[256];
    { char n1[256]; normalize(fact, n1, sizeof n1); canonicalize_lang(n1, fc, sizeof fc); }
    char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
    if (!parse_ground_unary(fc, pred, sizeof pred, arg, sizeof arg)) {
        put("I can only reconsider a simple 'X is a Y' fact for now.", out, out_size);
        return 1;
    }
    const char *fargs[] = {arg};
    if (!kb_query(b->kb, pred, fargs, 1)) {
        char msg[200];
        snprintf(msg, sizeof msg,
                 "But I don't know that %s is a %s in the first place.", arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    /* hypothetically un-know the fact, re-derive, then restore it */
    int before = goal_truth(b);
    int removed = kb_retract(b->kb, pred, fargs, 1);
    int after = goal_truth(b);
    if (removed) { kb_set_origin(b->kb, KB_SESSION); kb_assert(b->kb, pred, fargs, 1); }

    char msg[400];
    if (before == 1 && after == 0) {
        snprintf(msg, sizeof msg,
                 "If I didn't know that %s is a %s, I could no longer conclude that "
                 "%s is a %s — that conclusion rests on it.",
                 arg, pred, b->last_goal_arg, b->last_goal_pred);
    } else if (before == 1 && after == 1) {
        snprintf(msg, sizeof msg,
                 "Even without knowing that %s is a %s, %s would still be a %s — I "
                 "can reach that another way.",
                 arg, pred, b->last_goal_arg, b->last_goal_pred);
    } else {
        snprintf(msg, sizeof msg,
                 "That wouldn't change my conclusion about %s either way.",
                 b->last_goal_arg);
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: robust (gen130, L20-deep) ----------------------------------
 * Composes gen129's single-belief ablation into a SWEEP: it stress-tests the
 * last stated conclusion by hypothetically removing EACH ground unary fact in
 * turn (retract -> re-derive -> restore) and collecting the ones whose removal
 * overturns it. The result is dependency structure the agent discovers about
 * ITSELF by systematic self-perturbation: a FRAGILE conclusion hangs on one
 * load-bearing fact; a ROBUST one survives the loss of any single belief. This
 * is knowledge gen76's proof trace cannot give — a proof shows one derivation;
 * ablation reveals whether OTHER derivations exist (redundancy = robustness).
 */
static int mod_robust(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);
    int trig = cue(low, "robust") || cue(low, "fragile") ||
               cue(low, "load-bearing") || cue(low, "load bearing") ||
               cue(low, "stress-test") || cue(low, "stress test") ||
               cue(low, "depend on") || cue(low, "rest on") ||
               cue(low, "solida") || cue(low, "dipende") || cue(low, "si regge");
    if (!trig) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can stress-test it.",
            out, out_size);
        return 1;
    }
    if (goal_truth(b) != 1) {
        put("That isn't something I currently conclude, so there's nothing to "
            "stress-test.", out, out_size);
        return 1;
    }

    /* sweep every ground unary fact: remove it, re-derive, restore, record the
     * load-bearing ones. */
    char preds[64][KB_TERM_LEN];
    size_t np = kb_unary_predicates(b->kb, preds, 64);
    char crit[16][96];
    size_t ncrit = 0;
    for (size_t p = 0; p < np; p++) {
        char consts[128][KB_TERM_LEN];
        const char *pat[] = {NULL};
        size_t nc = kb_match(b->kb, preds[p], pat, 1, consts, 128);
        for (size_t c = 0; c < nc; c++) {
            const char *a[] = {consts[c]};
            if (!kb_retract(b->kb, preds[p], a, 1)) continue;
            int after = goal_truth(b);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, preds[p], a, 1); /* restore — footprint-free */
            if (after == 0 && ncrit < 16)
                snprintf(crit[ncrit++], sizeof crit[0], "%s(%s)", preds[p], consts[c]);
        }
    }

    char msg[640];
    if (ncrit == 0) {
        snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s is robust — there's no single fact "
                 "I could forget that would overturn it.",
                 b->last_goal_arg, b->last_goal_pred);
    } else if (ncrit == 1) {
        snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s is fragile: it rests entirely on one "
                 "fact — %s. Forget that and it falls.",
                 b->last_goal_arg, b->last_goal_pred, crit[0]);
    } else {
        size_t o = (size_t)snprintf(msg, sizeof msg,
                 "My conclusion that %s is a %s rests on %zu load-bearing facts: ",
                 b->last_goal_arg, b->last_goal_pred, ncrit);
        for (size_t i = 0; i < ncrit && o < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                  i ? ", " : "", crit[i]);
        if (o < sizeof msg)
            snprintf(msg + o, sizeof msg - o, ". Removing any one would overturn it.");
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: calibrate (gen142, E8) -------------------------------------
 * Metacognitive calibration: the agent reports HOW it knows the last
 * conclusion, with confidence language COMPUTED from the real proof state, never
 * a canned adjective. It distinguishes FIVE epistemic states of the last stated
 * goal and answers two follow-ups from each:
 *   KNOWN        — a directly stored fact (proof has no "because").
 *   INFERRED     — derived through a rule/chain (proof contains "because").
 *   CONFLICTED   — the user has asserted both polarities about this atom; the
 *                  KB's last-write-wins override hid one claim, but the
 *                  contradiction was recorded, so both are named.
 *   HYPOTHETICAL — the conclusion rests on a standing "suppose X." assumption
 *                  (detected by ablation: removing the assumed fact flips it).
 *   UNKNOWN      — nothing supports it: predicate unknown, or no proof.
 * "Why do you think that?" reports the support + the state's confidence; "what
 * would make you change your mind?" reports the real lever — for KNOWN/INFERRED
 * the load-bearing facts found by the same ablation sweep mod_robust uses, for
 * HYPOTHETICAL the assumption to drop, for CONFLICTED the source to adjudicate,
 * for UNKNOWN the fact that would settle it. All grounded in state.
 *
 * It also owns the standing supposition: a bare "suppose X." (no "then", which
 * mod_knowledge's one-shot form needs) asserts X as a SESSION fact and records
 * it as an assumption, so subsequent ordinary answers can be graded hypothetical.
 */
enum epi_state { EPI_UNKNOWN, EPI_KNOWN, EPI_INFERRED, EPI_CONFLICTED, EPI_HYPO };

/* Which assumed fact (if any) the last goal's truth depends on: remove each
 * assumed fact, re-derive, restore; return the index whose removal flips the
 * goal from true to not-provable, else -1. Footprint-free (restores). */
static int calib_hypo_support(Brain *b) {
    if (!b || !b->kb || !b->has_last_goal || b->assumed_n == 0) return -1;
    if (goal_truth(b) != 1) return -1;
    for (size_t i = 0; i < b->assumed_n; i++) {
        const char *a[] = {b->assumed_arg[i]};
        if (!kb_query(b->kb, b->assumed_pred[i], a, 1)) continue;
        if (!kb_retract(b->kb, b->assumed_pred[i], a, 1)) continue;
        int after = goal_truth(b);
        kb_set_origin(b->kb, KB_SESSION);
        kb_assert(b->kb, b->assumed_pred[i], a, 1); /* restore */
        if (after == 0) return (int)i;
    }
    return -1;
}

/* Classify the epistemic state of the last stated goal from real KB/proof
 * state. `hypo_idx` (out) receives the assumed-fact index if HYPOTHETICAL. */
static enum epi_state calib_classify(Brain *b, int *hypo_idx) {
    *hypo_idx = -1;
    if (!b || !b->kb || !b->has_last_goal) return EPI_UNKNOWN;
    const char *a[] = {b->last_goal_arg};
    /* conflict the user actually produced about this exact atom */
    if (b->has_conflict &&
        strcmp(b->conflict_pred, b->last_goal_pred) == 0 &&
        strcmp(b->conflict_arg, b->last_goal_arg) == 0)
        return EPI_CONFLICTED;
    /* a direct fact-vs-fact conflict still standing in the KB */
    if (kb_is_conflicted(b->kb, b->last_goal_pred, a, 1)) return EPI_CONFLICTED;
    if (!kb_knows_pred(b->kb, b->last_goal_pred)) return EPI_UNKNOWN;
    if (goal_truth(b) != 1) return EPI_UNKNOWN; /* not currently concluded */
    int hi = calib_hypo_support(b);
    if (hi >= 0) { *hypo_idx = hi; return EPI_HYPO; }
    /* known vs inferred from the proof shape */
    if (b->has_last_proof && strstr(b->last_proof, " because ")) return EPI_INFERRED;
    return EPI_KNOWN;
}

/* Append the load-bearing facts of the last goal (the ones whose removal
 * overturns it) into `buf`; returns how many were found. Reuses the same
 * retract/re-derive/restore sweep as mod_robust — the genuine "what would change
 * my mind" lever for a derived or fact-backed conclusion. */
static size_t calib_load_bearing(Brain *b, char buf[][96], size_t max) {
    size_t n = 0;
    char preds[64][KB_TERM_LEN];
    size_t np = kb_unary_predicates(b->kb, preds, 64);
    for (size_t p = 0; p < np && n < max; p++) {
        char consts[128][KB_TERM_LEN];
        const char *pat[] = {NULL};
        size_t nc = kb_match(b->kb, preds[p], pat, 1, consts, 128);
        for (size_t c = 0; c < nc && n < max; c++) {
            const char *a[] = {consts[c]};
            if (!kb_retract(b->kb, preds[p], a, 1)) continue;
            int after = goal_truth(b);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, preds[p], a, 1); /* restore */
            if (after == 0)
                snprintf(buf[n++], 96, "%s is a %s", consts[c], preds[p]);
        }
    }
    return n;
}

static int mod_calibrate(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* (1) standing supposition: "suppose X." / "assume X." / "supponi X." with
     * NO "then" (the one-shot "suppose X then Y" stays with mod_knowledge). It
     * asserts X as a session fact AND records it as an assumption so later
     * answers that rest on it grade HYPOTHETICAL. */
    {
        const char *rest = NULL;
        static const char *const sup[] = {
            "suppose that ", "suppose ", "assume that ", "assume ",
            "let's say ", "lets say ", "supponi che ", "supponi ",
            "assumi che ", "assumi ", NULL };
        for (const char *const *m = sup; *m; m++)
            if (strncmp(norm, *m, strlen(*m)) == 0) { rest = norm + strlen(*m); break; }
        if (rest && !strstr(rest, " then ") && !strstr(rest, " allora ")) {
            char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
            if (parse_ground_unary(rest, pred, sizeof pred, arg, sizeof arg)) {
                const char *a[] = {arg};
                kb_set_origin(b->kb, KB_SESSION);
                kb_assert(b->kb, pred, a, 1);
                if (b->assumed_n < 8) {
                    snprintf(b->assumed_pred[b->assumed_n], KB_TERM_LEN, "%s", pred);
                    snprintf(b->assumed_arg[b->assumed_n], KB_TERM_LEN, "%s", arg);
                    b->assumed_n++;
                }
                char msg[200];
                snprintf(msg, sizeof msg,
                         "All right — assuming %s is a %s. Anything I conclude from "
                         "it will be conditional on that.", arg, pred);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* (2) metacognitive follow-ups. Two intents, recognized by communicative
     * cue, both answered from the last goal's epistemic state. */
    int why_think = cue(low, "why do you think") || cue(low, "why do you believe") ||
                    cue(low, "what makes you think") || cue(low, "what makes you say") ||
                    cue(low, "how certain") || cue(low, "how do you rate") ||
                    cue(low, "perché lo pensi") || cue(low, "perche lo pensi") ||
                    cue(low, "perché lo credi") || cue(low, "perche lo credi") ||
                    cue(low, "quanto sei certo");
    int change_mind = cue(low, "change your mind") || cue(low, "change my mind") ||
                      cue(low, "make you reconsider") || cue(low, "would you reconsider") ||
                      cue(low, "prove you wrong") || cue(low, "what would change") ||
                      cue(low, "cambiare idea") || cue(low, "farti ricredere");
    if (!why_think && !change_mind) return 0;

    if (!b->has_last_goal) {
        put("Ask me whether something holds first — then I can tell you how I know "
            "it and what would change my mind.", out, out_size);
        return 1;
    }

    int hypo_idx = -1;
    enum epi_state st = calib_classify(b, &hypo_idx);
    char msg[640];

    if (why_think) {
        switch (st) {
        case EPI_KNOWN:
            snprintf(msg, sizeof msg,
                     "I'm certain: %s is a %s is a fact you stated directly, so I "
                     "hold it as known, not inferred.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        case EPI_INFERRED:
            snprintf(msg, sizeof msg,
                     "I'm confident but it's derived, not given: %s. I concluded it "
                     "by applying a rule, so it's only as sound as those premises.",
                     b->has_last_proof ? b->last_proof : "a rule chain");
            break;
        case EPI_CONFLICTED:
            snprintf(msg, sizeof msg,
                     "I'm genuinely unsure — I hold conflicting claims about %s: you "
                     "told me it is a %s and also that it is not. Until that's "
                     "resolved I can't commit either way.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        case EPI_HYPO:
            snprintf(msg, sizeof msg,
                     "Only conditionally: %s is a %s holds because I'm ASSUMING %s is "
                     "a %s. Drop that assumption and I'd have no ground for it.",
                     b->last_goal_arg, b->last_goal_pred,
                     b->assumed_arg[hypo_idx], b->assumed_pred[hypo_idx]);
            break;
        default: /* EPI_UNKNOWN */
            snprintf(msg, sizeof msg,
                     "I don't actually think that — I have no support for %s being a "
                     "%s, so I can't claim it either way.",
                     b->last_goal_arg, b->last_goal_pred);
            break;
        }
        put(msg, out, out_size);
        return 1;
    }

    /* change_mind */
    switch (st) {
    case EPI_KNOWN:
        snprintf(msg, sizeof msg,
                 "Learning that %s is NOT a %s — that's the single fact it rests on, "
                 "so contradicting it would overturn my belief.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    case EPI_INFERRED: {
        char crit[8][96];
        size_t nc = calib_load_bearing(b, crit, 8);
        if (nc == 0)
            snprintf(msg, sizeof msg,
                     "Nothing simple — I can reach that another way, so no single "
                     "fact I drop would change it.");
        else if (nc == 1)
            snprintf(msg, sizeof msg,
                     "It hangs on one premise: if I learned that %s were false, I'd "
                     "stop concluding %s is a %s.",
                     crit[0], b->last_goal_arg, b->last_goal_pred);
        else {
            size_t o = (size_t)snprintf(msg, sizeof msg,
                     "It hangs on %zu premises — overturning any one would change it: ",
                     nc);
            for (size_t i = 0; i < nc && o < sizeof msg; i++)
                o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                      i ? ", " : "", crit[i]);
            if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
        }
        break;
    }
    case EPI_CONFLICTED:
        snprintf(msg, sizeof msg,
                 "Telling me which to trust — you've claimed both that %s is a %s and "
                 "that it is not. Retract one and I'd settle on the other.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    case EPI_HYPO:
        snprintf(msg, sizeof msg,
                 "Dropping the assumption — I only conclude %s is a %s because I'm "
                 "supposing %s is a %s. Withdraw that and the conclusion goes with it.",
                 b->last_goal_arg, b->last_goal_pred,
                 b->assumed_arg[hypo_idx], b->assumed_pred[hypo_idx]);
        break;
    default: /* EPI_UNKNOWN */
        snprintf(msg, sizeof msg,
                 "Learning whether %s is a %s at all — right now I have no support "
                 "either way, so any fact about it would be new ground.",
                 b->last_goal_arg, b->last_goal_pred);
        break;
    }
    put(msg, out, out_size);
    (void)norm;
    return 1;
}

/* --- module: abduce (gen131, L16/L19) -----------------------------------
 * The INVERSE of deduction and the mirror of gen129's subtraction. gen129 asked
 * "remove this belief — does the conclusion fall?" abduction asks the opposite:
 * the conclusion does NOT hold — what belief WOULD make it hold? It runs the
 * rule engine BACKWARDS: find a rule whose head is the goal, and name the body
 * premise(s) that, if believed, would entail it ("If you told me socrates is a
 * man, then socrates would be a mortal — that's the rule man -> mortal"). This
 * is inference to the missing premise, computed from the real rules, not
 * guessed. Honest when nothing could entail it, and defers to deduction when the
 * goal already holds.
 */

/* Loose parse of a goal clause into (arg, pred): drop the copula/article/
 * infinitive filler and take the first surviving token as the subject and the
 * last as the class. Handles "socrates a mortal", "socrates be a mortal",
 * "socrates to be a mortal". Returns 1 on a clean two-token reading. */
static int parse_goal_loose(const char *clause, char *pred, size_t ps,
                            char *arg, size_t as) {
    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    size_t n = strlen(buf);
    while (n > 0 && (buf[n-1]=='?'||buf[n-1]=='.'||buf[n-1]=='!'||buf[n-1]==','))
        buf[--n] = '\0';
    char *w[16];
    size_t nw = split_words(buf, w, 16);
    char kept[16][KB_TERM_LEN]; size_t nk = 0;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strcmp(t,"is")==0 || strcmp(t,"be")==0 || strcmp(t,"a")==0 ||
            strcmp(t,"an")==0 || strcmp(t,"to")==0 || strcmp(t,"the")==0 ||
            strcmp(t,"would")==0 || strcmp(t,"still")==0 ||
            strcmp(t,"not")==0)
            continue;
        if (nk < 16) snprintf(kept[nk++], KB_TERM_LEN, "%s", t);
    }
    if (nk < 2) return 0;
    snprintf(arg, as, "%s", kept[0]);
    snprintf(pred, ps, "%s", kept[nk - 1]);
    return 1;
}

/* gen132: backward chaining for abduction. Collect the ROOT premises that would
 * make goal `pred(arg)` hold — the unsatisfied predicates with no rule of their
 * own, reached by following rule bodies down. A body already true is satisfied
 * (skipped); a body with its own rule recurses; a body that is neither is a
 * root to acquire. Dedups; depth-guarded. Returns the new root count. */
static size_t abduce_roots(KB *kb, const char *pred, const char *arg, int depth,
                           char roots[][KB_TERM_LEN], size_t maxr, size_t nr) {
    if (depth > 16) return nr;
    char bodies[8][KB_TERM_LEN];
    size_t nb = kb_rule_body_preds(kb, pred, 1, bodies, 8);
    if (nb == 0) { /* no rule defines pred -> it is itself a root premise */
        for (size_t i = 0; i < nr; i++)
            if (strcmp(roots[i], pred) == 0) return nr; /* dedup */
        if (nr < maxr) snprintf(roots[nr++], KB_TERM_LEN, "%s", pred);
        return nr;
    }
    for (size_t i = 0; i < nb; i++) {
        const char *a[] = {arg};
        if (kb_query(kb, bodies[i], a, 1)) continue; /* already satisfied */
        nr = abduce_roots(kb, bodies[i], arg, depth + 1, roots, maxr, nr);
    }
    return nr;
}

/* gen132: build the linear rule spine "root -> … -> goal" by following the FIRST
 * body predicate at each level until a predicate with no rule. */
static void abduce_spine(KB *kb, const char *pred, char *out, size_t out_size) {
    char chain[16][KB_TERM_LEN]; size_t n = 0;
    char cur[KB_TERM_LEN]; snprintf(cur, sizeof cur, "%s", pred);
    for (int d = 0; d < 16 && n < 16; d++) {
        snprintf(chain[n++], KB_TERM_LEN, "%s", cur);
        char bodies[8][KB_TERM_LEN];
        if (kb_rule_body_preds(kb, cur, 1, bodies, 8) == 0) break;
        snprintf(cur, sizeof cur, "%s", bodies[0]);
    }
    size_t o = 0;
    for (size_t i = n; i-- > 0;) /* reverse: root first */
        o += (size_t)snprintf(out + o, o < out_size ? out_size - o : 0,
                              "%s%s", (i + 1 < n) ? " -> " : "", chain[i]);
}

static size_t add_missing_root(char roots[][KB_TERM_LEN], size_t nr,
                               size_t maxr, const char *pred) {
    for (size_t i = 0; i < nr; i++)
        if (strcmp(roots[i], pred) == 0) return nr;
    if (nr < maxr) snprintf(roots[nr++], KB_TERM_LEN, "%.*s", KB_TERM_LEN - 1, pred);
    return nr;
}

/* gen138: cost one alternative by the ROOT facts still needed to make it true.
 * Already-satisfied body predicates cost zero; derived missing predicates are
 * pushed backward to their roots, so the score is over acquirable facts. */
static size_t abduce_rule_missing_roots(KB *kb, const char *head, size_t rule_idx,
                                        const char *arg,
                                        char roots[][KB_TERM_LEN], size_t maxr,
                                        char *rule, size_t rule_size) {
    roots[0][0] = 0;
    char rb[8][KB_TERM_LEN];
    size_t nrb = kb_nth_rule_body_preds(kb, head, 1, rule_idx, rb, 8);
    size_t nr = 0, ro = 0;
    if (rule_size) rule[0] = 0;
    for (size_t i = 0; i < nrb; i++) {
        ro += (size_t)snprintf(rule + ro, ro < rule_size ? rule_size - ro : 0,
                               "%s%s", i ? " and " : "", rb[i]);
        const char *pa[] = {arg};
        if (kb_query(kb, rb[i], pa, 1)) continue;
        char sub[8][KB_TERM_LEN];
        if (kb_rule_body_preds(kb, rb[i], 1, sub, 8) > 0)
            nr = abduce_roots(kb, rb[i], arg, 0, roots, maxr, nr);
        else
            nr = add_missing_root(roots, nr, maxr, rb[i]);
    }
    return nr;
}

static int mod_abduce(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    /* trigger + clause extraction from the canonicalized surface */
    static const char *const pre[] = {
        "what would make ", "what makes ", "what would let ",
        "why might ", "why would ", "why could ", "how could ", "how can ",
        "what would you need to know for ", "what do you need to know for ",
        "cosa renderebbe ", "cosa servirebbe per ", "che cosa renderebbe ",
        NULL,
    };
    const char *clause = NULL;
    int contrastive = 0;
    int optimal = 0;
    int simulate = 0;

    static const char *const sim_pre[] = {
        "try the easiest way to make ", "simulate the easiest way to make ",
        "prova il modo piu facile per rendere ", "prova il modo più facile per rendere ", NULL
    };
    for (const char *const *p = sim_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; optimal = 1; simulate = 1; break; }
    }

    static const char *const opt_pre[] = {
        "what is the easiest way to make ", "what's the easiest way to make ",
        "easiest way to make ", "least i need to know for ",
        "modo piu facile per rendere ", "modo più facile per rendere ", NULL
    };
    for (const char *const *p = opt_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; optimal = 1; break; }
    }

    static const char *const neg_pre[] = {
        "why is not ", "why is ", "why ", "perché ", "perche ", NULL
    };
    for (const char *const *p = neg_pre; *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) != 0) continue;
        const char *cand = norm + L;
        if (strstr(cand, " not ") || strncmp(cand, "not ", 4) == 0) {
            clause = cand;
            contrastive = 1;
            break;
        }
    }

    for (const char *const *p = pre; !clause && *p; p++) {
        size_t L = strlen(*p);
        if (strncmp(norm, *p, L) == 0) { clause = norm + L; break; }
    }
    if (!clause) return 0;

    char pred[KB_TERM_LEN], arg[KB_TERM_LEN];
    if (!parse_goal_loose(clause, pred, sizeof pred, arg, sizeof arg)) {
        put("Tell me what you'd like me to account for, as 'X is a Y'.",
            out, out_size);
        return 1;
    }

    const char *gargs[] = {arg};
    if (kb_query(b->kb, pred, gargs, 1)) {
        char ex[400], msg[480];
        if (kb_explain(b->kb, pred, gargs, 1, ex, sizeof ex) && strstr(ex, " because "))
            snprintf(msg, sizeof msg, "I already conclude that: %s.", ex);
        else
            snprintf(msg, sizeof msg, "I already know that %s is a %s.", arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    char bodies[8][KB_TERM_LEN];
    size_t nb = kb_rule_body_preds(b->kb, pred, 1, bodies, 8);
    if (nb == 0) {
        char msg[200];
        snprintf(msg, sizeof msg,
                 "I have no rule that would make anything a %s, so I can't say "
                 "what would.", pred);
        put(msg, out, out_size);
        return 1;
    }

    if (optimal) {
        size_t nrules = kb_rules_for_head(b->kb, pred, 1);
        size_t best_n = 999;
        char best_roots[16][KB_TERM_LEN];
        char best_rule[200]; best_rule[0] = '\0';
        for (size_t r = 0; r < nrules; r++) {
            char roots[16][KB_TERM_LEN];
            char rule[200];
            size_t nr = abduce_rule_missing_roots(b->kb, pred, r, arg,
                                                  roots, 16, rule, sizeof rule);
            if (nr > 0 && nr < best_n) {
                best_n = nr;
                snprintf(best_rule, sizeof best_rule, "%s", rule);
                for (size_t i = 0; i < nr; i++)
                    snprintf(best_roots[i], KB_TERM_LEN, "%s", roots[i]);
            }
        }

        char msg[1400];
        if (best_n == 999) {
            snprintf(msg, sizeof msg,
                     "I can see rules for %s, but no missing premise plan stands out.",
                     pred);
        } else {
            char need[500]; size_t no = 0;
            for (size_t i = 0; i < best_n; i++)
                no += (size_t)snprintf(need + no,
                                       no < sizeof need ? sizeof need - no : 0,
                                       "%s%s is a %s", i ? " and " : "",
                                       arg, best_roots[i]);
            if (simulate) {
                for (size_t i = 0; i < best_n; i++) {
                    const char *ra[] = {arg};
                    kb_set_origin(b->kb, KB_SESSION);
                    kb_assert(b->kb, best_roots[i], ra, 1);
                }
                int ok = kb_query(b->kb, pred, gargs, 1);
                char ex[512] = "";
                if (ok) kb_explain(b->kb, pred, gargs, 1, ex, sizeof ex);
                for (size_t i = 0; i < best_n; i++) {
                    const char *ra[] = {arg};
                    kb_retract(b->kb, best_roots[i], ra, 1);
                }
                if (ok)
                    snprintf(msg, sizeof msg,
                             "Hypothetically adding %s makes %s a %s: %s. I restored the KB after the simulation.",
                             need, arg, pred, ex);
                else
                    snprintf(msg, sizeof msg,
                             "I tried the cheapest plan (%s), but it still did not derive %s(%s). I restored the KB.",
                             need, pred, arg);
            } else {
                snprintf(msg, sizeof msg,
                         "The easiest way is to tell me that %s — %zu missing premise%s via %s -> %s.",
                         need, best_n, best_n == 1 ? "" : "s", best_rule, pred);
            }
        }
        put(msg, out, out_size);
        return 1;
    }

    /* gen135-137: contrastive why-not questions reuse the abductive machinery.
     * A failed goal may have one rule, a chain, or several alternative rules;
     * explain the blocked proof shape that is actually present in the KB. */
    if (contrastive) {
        size_t crules = kb_rules_for_head(b->kb, pred, 1);
        char msg[1400];

        if (crules > 1) {
            char alts[1000]; alts[0] = '\0'; size_t ao = 0; size_t shown = 0;
            for (size_t r = 0; r < crules; r++) {
                char rb[8][KB_TERM_LEN];
                size_t nrb = kb_nth_rule_body_preds(b->kb, pred, 1, r, rb, 8);
                char rule[200]; size_t ro = 0;
                char need[300]; need[0] = '\0'; size_t no = 0; size_t nn = 0;
                for (size_t i = 0; i < nrb; i++) {
                    const char *pa[] = {arg};
                    ro += (size_t)snprintf(rule + ro,
                                           ro < sizeof rule ? sizeof rule - ro : 0,
                                           "%s%s", i ? " and " : "", rb[i]);
                    if (kb_query(b->kb, rb[i], pa, 1)) continue;
                    char sub[8][KB_TERM_LEN];
                    if (kb_rule_body_preds(b->kb, rb[i], 1, sub, 8) > 0) {
                        char roots[16][KB_TERM_LEN];
                        size_t nr = abduce_roots(b->kb, rb[i], arg, 0, roots, 16, 0);
                        for (size_t j = 0; j < nr; j++)
                            no += (size_t)snprintf(need + no,
                                                   no < sizeof need ? sizeof need - no : 0,
                                                   "%s%s is a %s", nn ? " and " : "",
                                                   arg, roots[j]), nn++;
                    } else {
                        no += (size_t)snprintf(need + no,
                                               no < sizeof need ? sizeof need - no : 0,
                                               "%s%s is a %s", nn ? " and " : "",
                                               arg, rb[i]);
                        nn++;
                    }
                }
                if (nn == 0) continue;
                ao += (size_t)snprintf(alts + ao,
                                       ao < sizeof alts ? sizeof alts - ao : 0,
                                       "%s%s -> %s needs %s", shown ? "; " : "",
                                       rule, pred, need);
                shown++;
            }
            snprintf(msg, sizeof msg,
                     "%s is not a %s because every alternative is blocked: %s.",
                     arg, pred, alts);
        } else {
            int deeper_missing = 0;
            for (size_t i = 0; i < nb; i++) {
                const char *pa[] = {arg};
                char sub[8][KB_TERM_LEN];
                if (!kb_query(b->kb, bodies[i], pa, 1) &&
                    kb_rule_body_preds(b->kb, bodies[i], 1, sub, 8) > 0) {
                    deeper_missing = 1;
                    break;
                }
            }

            if (deeper_missing) {
                char roots[16][KB_TERM_LEN];
                size_t nr = abduce_roots(b->kb, pred, arg, 0, roots, 16, 0);
                char missing[400]; size_t mo = 0;
                for (size_t i = 0; i < nr; i++)
                    mo += (size_t)snprintf(missing + mo,
                                           mo < sizeof missing ? sizeof missing - mo : 0,
                                           "%s%s is a %s", i ? " and " : "",
                                           arg, roots[i]);
                char spine[256]; abduce_spine(b->kb, pred, spine, sizeof spine);
                snprintf(msg, sizeof msg,
                         "%s is not a %s because I am missing the root premise: %s. By %s.",
                         arg, pred, missing, spine);
            } else {
                char missing[400]; size_t mo = 0; int nm = 0;
                char rule[200]; size_t ro = 0;
                for (size_t i = 0; i < nb; i++) {
                    const char *pa[] = {arg};
                    if (!kb_query(b->kb, bodies[i], pa, 1)) {
                        mo += (size_t)snprintf(missing + mo,
                                               mo < sizeof missing ? sizeof missing - mo : 0,
                                               "%s%s is a %s", nm ? " and " : "",
                                               arg, bodies[i]);
                        nm++;
                    }
                    ro += (size_t)snprintf(rule + ro,
                                           ro < sizeof rule ? sizeof rule - ro : 0,
                                           "%s%s", i ? " and " : "", bodies[i]);
                }
                if (nm > 0)
                    snprintf(msg, sizeof msg,
                             "%s is not a %s because I am missing: %s. The rule is %s -> %s.",
                             arg, pred, missing, rule, pred);
                else
                    snprintf(msg, sizeof msg,
                             "I have the premises for %s to be a %s, but I still cannot derive it.",
                             arg, pred);
            }
        }
        put(msg, out, out_size);
        return 1;
    }

    size_t nrules = kb_rules_for_head(b->kb, pred, 1);
    if (nrules > 1) {
        char alts[400]; size_t ao = 0; size_t shown = 0;
        for (size_t r = 0; r < nrules; r++) {
            char rb[8][KB_TERM_LEN];
            size_t nrb = kb_nth_rule_body_preds(b->kb, pred, 1, r, rb, 8);
            char one[200]; size_t oo = 0; int miss = 0;
            for (size_t i = 0; i < nrb; i++) {
                const char *pa[] = {arg};
                if (kb_query(b->kb, rb[i], pa, 1)) continue; /* satisfied conjunct */
                oo += (size_t)snprintf(one + oo, oo < sizeof one ? sizeof one - oo : 0,
                                       "%s%s is a %s", miss ? " and " : "", arg, rb[i]);
                miss++;
            }
            if (!miss) continue; /* this rule is already fully satisfied */
            ao += (size_t)snprintf(alts + ao, ao < sizeof alts ? sizeof alts - ao : 0,
                                   "%s%s", shown ? ", or " : "", one);
            shown++;
        }
        char msg[600];
        snprintf(msg, sizeof msg,
                 "There's more than one way: either %s — any one would make %s a %s.",
                 alts, arg, pred);
        put(msg, out, out_size);
        return 1;
    }

    /* gen132: is any immediate premise itself DERIVABLE (has its own rule) and
     * unsatisfied? Then a single step doesn't reach acquirable facts — chain
     * backwards to the ROOT premises and show the rule spine. Otherwise keep the
     * gen131 one-step message (the immediate body IS the root). */
    int deeper = 0;
    for (size_t i = 0; i < nb; i++) {
        const char *pa[] = {arg};
        char sub[8][KB_TERM_LEN];
        if (!kb_query(b->kb, bodies[i], pa, 1) &&
            kb_rule_body_preds(b->kb, bodies[i], 1, sub, 8) > 0) { deeper = 1; break; }
    }

    char msg[1024];
    if (deeper) {
        char roots[16][KB_TERM_LEN];
        size_t nr = abduce_roots(b->kb, pred, arg, 0, roots, 16, 0);
        char prem[400]; size_t po = 0;
        for (size_t i = 0; i < nr; i++)
            po += (size_t)snprintf(prem + po, po < sizeof prem ? sizeof prem - po : 0,
                                   "%s%s is a %s", i ? " and " : "", arg, roots[i]);
        char spine[256]; abduce_spine(b->kb, pred, spine, sizeof spine);
        snprintf(msg, sizeof msg,
                 "If you told me that %s, then %s would be a %s — by %s.",
                 prem, arg, pred, spine);
    } else {
        /* name only the MISSING conjuncts as the premise to supply, but show the
         * full rule body so the rule itself is reported faithfully. */
        char prem[400]; size_t po = 0; int np_missing = 0;
        char rule[200]; size_t ro = 0;
        for (size_t i = 0; i < nb; i++) {
            const char *pa[] = {arg};
            if (!kb_query(b->kb, bodies[i], pa, 1)) {
                po += (size_t)snprintf(prem + po, po < sizeof prem ? sizeof prem - po : 0,
                                       "%s%s is a %s", np_missing ? " and " : "",
                                       arg, bodies[i]);
                np_missing++;
            }
            ro += (size_t)snprintf(rule + ro, ro < sizeof rule ? sizeof rule - ro : 0,
                                   "%s%s", i ? " and " : "", bodies[i]);
        }
        snprintf(msg, sizeof msg,
                 "If you told me that %s, then %s would be a %s — that's the rule %s -> %s.",
                 prem, arg, pred, rule, pred);
    }
    put(msg, out, out_size);
    return 1;
}
