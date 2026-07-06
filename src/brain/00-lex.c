/* ----------------------------------------------------------------------------
 * small text utilities shared by modules
 * ------------------------------------------------------------------------- */

/* Copy `in` into `out` lowercased and with surrounding blanks trimmed, so
 * modules can match on intent without caring about case or stray spaces. */
static void normalize(const char *in, char *out, size_t out_size) {
    if (out_size == 0) return;
    while (*in && isspace((unsigned char)*in)) in++;       /* skip leading */
    size_t n = 0;
    for (size_t i = 0; in[i] && n + 1 < out_size; i++) {
        /* gen240: spell out a PERCENTAGE '%' as the word "percent" so a later
         * punctuation-stripping canon pass doesn't drop the signal ("15%" -> "15
         * percent"). Only when it follows a digit — a lone '%' stays punctuation so
         * the "that's just punctuation" detector still fires. */
        if (in[i] == '%' && n > 0 && isdigit((unsigned char)out[n - 1])) {
            const char *p = " percent ";
            for (size_t k = 0; p[k] && n + 1 < out_size; k++) out[n++] = p[k];
            continue;
        }
        out[n++] = (char)tolower((unsigned char)in[i]);
    }
    while (n > 0 && isspace((unsigned char)out[n - 1])) n--; /* trim trailing */
    out[n] = '\0';
}

/* True if `s` equals any of the NULL-terminated list of words. */
static int matches_any(const char *s, const char *const *words) {
    for (; *words; words++) {
        if (strcmp(s, *words) == 0) return 1;
    }
    return 0;
}

/* gen211 (cardinal KB-first principle): true if the normalized input `norm` exactly
 * matches any surface form registered for `intent` as intent_phrase(intent, "form")
 * in the KB. The phrase forms are KNOWLEDGE, not a C array — so the class grows at
 * runtime: teach a new form, assert another intent_phrase/2, and this same matcher
 * fires with no code change (the KB-migration law of gen193, lifted from closed-class
 * words to multi-word idioms). The stored atom keeps its surrounding quotes (kb.c
 * parse_term), so we strip them before comparing. */
static int kb_intent_match(Brain *b, const char *intent, const char *norm) {
    if (!b || !b->kb || !intent || !norm) return 0;
    char forms[64][KB_TERM_LEN];
    const char *q[2] = { intent, NULL };
    size_t n = kb_match(b->kb, "intent_phrase", q, 2, forms, 64);
    for (size_t i = 0; i < n; i++) {
        char *p = forms[i];
        size_t l = strlen(p);
        if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
        if (strcmp(p, norm) == 0) return 1;
    }
    return 0;
}

/* gen213 (same cardinal principle, SUBSTRING flavour): true if any cue registered for
 * `intent` as intent_cue(intent, "fragment") in the KB OCCURS ANYWHERE in `norm` — the
 * KB-backed form of a `cue(norm, "…") || cue(norm, "…")` chain. Like kb_intent_match but
 * with substring containment instead of whole-turn equality, so the cue set grows at
 * runtime with no code edit. */
static int kb_cue_match(Brain *b, const char *intent, const char *norm) {
    if (!b || !b->kb || !intent || !norm) return 0;
    char cues[64][KB_TERM_LEN];
    const char *q[2] = { intent, NULL };
    size_t n = kb_match(b->kb, "intent_cue", q, 2, cues, 64);
    for (size_t i = 0; i < n; i++) {
        char *p = cues[i];
        size_t l = strlen(p);
        if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }
        if (*p && strstr(norm, p)) return 1;
    }
    return 0;
}

/* gen212 (cardinal KB-first principle, OUTPUT side): build a reply for `intent` from a
 * response_template(intent, "…{name}…") fact, substituting "{name}" with `slot`. The
 * phrasings are KNOWLEDGE, not C literals: the class grows at runtime (teach a phrasing,
 * assert another fact, it joins the rotation, no code edit). Rotates across the
 * registered templates by b->response_pick so taught forms are actually used and
 * repetition is avoided (the gen55 anti-repeat instinct). Writes the filled reply into
 * `out` and returns 1, or 0 if `intent` has no template (caller keeps a literal
 * fallback so the agent is never mute even if the KB file is absent). */
static void current_lang(Brain *b, char *out, size_t sz);   /* gen240, defined in 10 */

static int kb_response(Brain *b, const char *intent, const char *slot,
                       char *out, size_t outsz) {
    if (!b || !b->kb || !intent || !out || outsz == 0) return 0;
    char tpl[16][KB_TERM_LEN];
    /* gen240 (universal-comprehension): prefer a LOCALIZED template for the current
     * conversation language — response_template(intent, Lang, "…") — falling back to
     * the language-agnostic /2 form. So any reply is localized just by adding a /3
     * fact (KB-first, additive); English stays the /2 default. */
    size_t n = 0;
    char lang[8]; current_lang(b, lang, sizeof lang);
    if (strcmp(lang, "en") != 0) {
        const char *q3[3] = { intent, lang, NULL };
        n = kb_match(b->kb, "response_template", q3, 3, tpl, 16);
    }
    if (n == 0) {
        const char *q[2] = { intent, NULL };
        n = kb_match(b->kb, "response_template", q, 2, tpl, 16);
    }
    if (n == 0) return 0;
    /* gen226 (mimic-llm, primo giro): a loaded STYLE profile may set the selection
     * temperature — the nitidezza of choosing among interchangeable FORMS. t==0 is
     * argmax (always the canonical first phrasing: a decided, terse persona); when
     * no profile is loaded (no fact) or t!=0, the gen55 anti-repeat rotation holds.
     * This biases only HOW a reply is phrased, never WHAT is said (see
     * docs/plans/mimic-llm.md). */
    size_t idx = b->response_pick % n;
    {
        char tv[1][KB_TERM_LEN];
        const char *tq[1] = { NULL };
        if (kb_match(b->kb, "style_temperature", tq, 1, tv, 1) == 1 &&
            atoi(tv[0]) == 0)
            idx = 0;
    }
    char *p = tpl[idx];                        /* selected phrasing */
    b->response_pick++;
    size_t l = strlen(p);
    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }  /* strip quotes */

    /* substitute every "{name}" with `slot` (or "" if no slot was given) */
    const char *s = slot ? slot : "";
    size_t o = 0;
    for (const char *c = p; *c && o + 1 < outsz; ) {
        if (strncmp(c, "{name}", 6) == 0) {
            o += (size_t)snprintf(out + o, outsz - o, "%s", s);
            c += 6;
        } else {
            out[o++] = *c++;
        }
    }
    out[o < outsz ? o : outsz - 1] = '\0';
    return 1;
}

/* Return the index of token `t` in `w[0..nw)`, or `nw` if absent. */
static size_t find_token(char **w, size_t nw, const char *t) {
    for (size_t i = 0; i < nw; i++)
        if (strcmp(w[i], t) == 0) return i;
    return nw;
}

/* True if needle occurs anywhere in haystack — the keyword-cue test behind
 * paraphrase-robust intent (gen51): one intent reached from many phrasings. */
static int cue(const char *haystack, const char *needle) {
    return strstr(haystack, needle) != NULL;
}

/* Return a pointer past leading whitespace in `s`. */
static const char *skip_ws(const char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

/* Copy `src` into `dst` (size `dst_size`), trimming trailing whitespace. */
static void copy_trim(char *dst, size_t dst_size, const char *src) {
    if (dst_size == 0) return;
    size_t n = 0;
    for (; src[n] && n + 1 < dst_size; n++) dst[n] = src[n];
    while (n > 0 && isspace((unsigned char)dst[n - 1])) n--;
    dst[n] = '\0';
}

/* Write a fixed reply into out, returning its length. */
static size_t put(const char *text, char *out, size_t out_size) {
    size_t n = strlen(text);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, text, n);
    out[n] = '\0';
    return n;
}

/* gen76: store a proof trace so a follow-up "how do you know?" can cite it. */
static void store_proof(Brain *b, const char *proof) {
    if (!b || !proof || !*proof) return;
    snprintf(b->last_proof, sizeof b->last_proof, "%s", proof);
    b->has_last_proof = 1;
}

/* gen103 (L16): current truth of the last stated class-conclusion, or -1 if none
 * is remembered. Used to snapshot the goal right before a correction. */
static int goal_truth(Brain *b) {
    if (!b || !b->kb || !b->has_last_goal) return -1;
    const char *args[] = { b->last_goal_arg };
    return kb_query(b->kb, b->last_goal_pred, args, 1);
}

/* gen103 (L16): after a correction changes the KB, re-derive the last stated
 * class-conclusion. If THIS correction flipped its truth (compared to `before`,
 * the snapshot taken just before the mutation), append a sentence announcing the
 * consequence to `out` — a correction's downstream effect is volunteered, not
 * waited for. `just_asserted` is the predicate the correction touched: we only
 * speak up when the affected conclusion is a *different*, downstream predicate —
 * the genuinely joined-up case, not a restatement of what was just asserted. The
 * `before` gate means a flip that already happened on an earlier turn (e.g. via
 * "forget") is never announced belatedly on an unrelated later assertion. */
static void note_consequence(Brain *b, const char *just_asserted, int before,
                             char *out, size_t out_size) {
    if (!b || !b->kb || !b->has_last_goal || before < 0) return;
    if (just_asserted && strcmp(just_asserted, b->last_goal_pred) == 0) return;
    int now = goal_truth(b);
    if (now == before) return; /* this correction changed nothing downstream */
    char note[200];
    if (before && !now)
        snprintf(note, sizeof note, " Then %s is no longer a %s.",
                 b->last_goal_arg, b->last_goal_pred);
    else
        snprintf(note, sizeof note, " Now %s is a %s after all.",
                 b->last_goal_arg, b->last_goal_pred);
    size_t cur = strlen(out);
    if (cur + strlen(note) + 1 < out_size)
        memcpy(out + cur, note, strlen(note) + 1);
    b->last_goal_yes = now; /* keep the memory consistent for further turns */
}

/* ----------------------------------------------------------------------------
 * module protocol
 *
 * A module looks at one turn and returns 1 if it claims it (having written a
 * response into out), or 0 to decline and pass the turn to the next module.
 * `norm` is the normalized input; `raw` is the original; `b` is brain state.
 * ------------------------------------------------------------------------- */

typedef int (*Handler)(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size);

typedef struct {
    const char *name;
    Handler     handle;
} Module;

/* Greetings & farewells (gen1) were the first intents. gen52 generalizes them
 * into the dialogue-act layer `mod_social` (defined near the registry, where
 * split_words is in scope) — the phatic register as a structure, not a list. */

/* Forward declarations: the possession frame in mod_memory needs the same
 * tokenizer and article test used later by the knowledge modules; discourse
 * memory needs the stoplist and edge-punctuation stripper from the bench
 * baseline helpers. */
static size_t split_words(char *s, char **argv, size_t max);
static int is_article(const char *w);
static int is_stopword(Brain *b, const char *w);
static int is_conjunction(Brain *b, const char *w);
static char *strip_edge_punct(char *t);
static int is_internal_pred(const char *pred);
static int run_shell(const char *cmd, char *out, size_t out_size);
static int identify_code_lang(const char *code, Brain *b);

/* Copy the last whitespace-separated word of `raw` into `dst`, preserving its
 * original casing and trimming trailing punctuation/whitespace. Used to keep
 * proper names (Rex, Luna) intact while matching on the normalized surface. */
static void copy_last_word(char *dst, size_t dst_size, const char *raw) {
    size_t n = strlen(raw);
    while (n > 0 && isspace((unsigned char)raw[n - 1])) n--;
    while (n > 0 && ispunct((unsigned char)raw[n - 1])) n--;
    size_t end = n;
    while (n > 0 && !isspace((unsigned char)raw[n - 1])) n--;
    size_t len = end - n;
    if (len >= dst_size) len = dst_size - 1;
    memcpy(dst, raw + n, len);
    dst[len] = '\0';
}

/* Personal-possession display helpers (gen57). The KB key is lowercased because
 * uppercase-initial atoms are read as variables; the original casing lives here
 * so replies feel natural. */
static void lowercase_copy(char *dst, size_t dst_size, const char *src) {
    size_t i = 0;
    for (; src[i] && i + 1 < dst_size; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

static void remember_possession(Brain *b, const char *thing, const char *name) {
    if (b->possession_count >= 8) return;
    size_t slot = b->possession_count;
    for (size_t i = 0; i < b->possession_count; i++) {
        if (strcmp(b->possessions[i][0], thing) == 0) { slot = i; break; }
    }
    copy_trim(b->possessions[slot][0], sizeof b->possessions[slot][0], thing);
    copy_trim(b->possessions[slot][1], sizeof b->possessions[slot][1], name);
    if (slot == b->possession_count) b->possession_count++;

    /* gen217 (glue): mark this thing as the salient possession so a later
     * possessive-pronoun anaphor ("what is his name") resolves to it. */
    copy_trim(b->last_possession_thing, sizeof b->last_possession_thing, thing);
    b->has_last_possession = 1;

    char thing_key[64], name_key[64];
    lowercase_copy(thing_key, sizeof thing_key, thing);
    lowercase_copy(name_key, sizeof name_key, name);
    const char *args[] = {thing_key, name_key};
    kb_assert(b->kb, "called", args, 2);

    /* gen163: the named pet becomes the salient discourse entity, so a later
     * unbound "she/he/it" composes possession memory with discourse reference
     * ("i have a cat named smoke" then "is she a robot?" resolves to smoke).
     * A real KB-fact antecedent mentioned afterwards still overrides this. */
    if (strlen(name_key) < KB_TERM_LEN) {
        snprintf(b->last_entity, sizeof b->last_entity, "%s", name_key);
        b->has_last_entity = 1;
    }
}

static const char *find_possession_name(Brain *b, const char *thing) {
    for (size_t i = 0; i < b->possession_count; i++)
        if (strcmp(b->possessions[i][0], thing) == 0)
            return b->possessions[i][1];
    return NULL;
}

/* gen214: ONE generic teach handler, driven by the learnable/3 KB registry, replacing
 * the per-intent teach blocks (the duplication that didn't scale). On a turn carrying a
 * teach verb and a quoted span, find the learnable Label that occurs in the (raw,
 * non-canonical) turn and assert the right fact for its Intent, by Mode:
 *   exact     -> intent_phrase(Intent, "<normalized span>")    (whole-turn match)
 *   substring -> intent_cue(Intent, "<normalized span>")       (substring match)
 *   fill      -> response_template(Intent, "<raw span>")       (output; keeps casing)
 * So a new learnable intent is DATA (a learnable/3 row), never new C. KB_SESSION, so it
 * persists on /save. Returns 1 if it claimed the turn. Defined here, after cue()/put(),
 * since it uses them. */
static int try_teach_form(Brain *b, const char *norm, const char *raw,
                          char *out, size_t outsz) {
    if (!b || !b->kb || !raw) return 0;
    char low[512];                                  /* raw, lowercased, NOT canonicalized */
    size_t ln = 0;
    for (const char *c = raw; *c && ln + 1 < sizeof low; c++)
        low[ln++] = (char)tolower((unsigned char)*c);
    low[ln] = '\0';
    /* gen271: this guard's verb list was the first REAL cue chain migrated to
     * KB by parrot0's own derived plan (Track 5.4) — the vocabulary lives as
     * intent_cue(00_lex_chain332, …) facts in kb/core/intents.p0, data not code. */
    if (!(kb_cue_match(b, "00_lex_chain332", low)))
        return 0;
    const char *rq1 = strchr(raw, '"'), *rq2 = rq1 ? strchr(rq1 + 1, '"') : NULL;
    if (!rq2 || rq2 <= rq1 + 1) return 0;

    char labels[32][KB_TERM_LEN];
    const char *qa[3] = { NULL, NULL, NULL };
    size_t nl = kb_match(b->kb, "learnable", qa, 3, labels, 32);
    for (size_t i = 0; i < nl; i++) {
        char lab[KB_TERM_LEN]; snprintf(lab, sizeof lab, "%s", labels[i]);  /* quoted */
        char *ls = lab; size_t ll = strlen(ls);
        if (ll >= 2 && ls[0] == '"' && ls[ll - 1] == '"') { ls[ll - 1] = '\0'; ls++; }
        if (!*ls || !strstr(low, ls)) continue;     /* this intent's label not named here */

        char intent[1][KB_TERM_LEN], mode[1][KB_TERM_LEN];
        const char *qi[3] = { labels[i], NULL, NULL };
        if (kb_match(b->kb, "learnable", qi, 3, intent, 1) != 1) continue;
        const char *qm[3] = { labels[i], intent[0], NULL };
        if (kb_match(b->kb, "learnable", qm, 3, mode, 1) != 1) continue;

        const char *pred; int from_raw;
        if      (!strcmp(mode[0], "exact"))     { pred = "intent_phrase";     from_raw = 0; }
        else if (!strcmp(mode[0], "substring")) { pred = "intent_cue";        from_raw = 0; }
        else if (!strcmp(mode[0], "fill"))      { pred = "response_template"; from_raw = 1; }
        else continue;

        /* span: raw (keep casing) for fill; the canonicalized `norm` for exact/substring
         * so the stored form matches what the recognizer sees on a later turn. */
        const char *s1 = rq1, *s2 = rq2;
        if (!from_raw) {
            const char *n1 = strchr(norm, '"'), *n2 = n1 ? strchr(n1 + 1, '"') : NULL;
            if (n2 && n2 > n1 + 1) { s1 = n1; s2 = n2; }
        }
        size_t pl = (size_t)(s2 - (s1 + 1));
        if (pl >= KB_TERM_LEN - 2) return 0;
        char phrase[KB_TERM_LEN]; memcpy(phrase, s1 + 1, pl); phrase[pl] = '\0';

        char quoted[KB_TERM_LEN]; snprintf(quoted, sizeof quoted, "\"%s\"", phrase);
        const char *ar[2] = { intent[0], quoted };
        kb_set_origin(b->kb, KB_SESSION);
        kb_assert(b->kb, pred, ar, 2);
        char msg[256];
        snprintf(msg, sizeof msg, "Got it - I'll take \"%s\" as a way to %s now.", phrase, ls);
        put(msg, out, outsz);
        return 1;
    }
    return 0;
}

