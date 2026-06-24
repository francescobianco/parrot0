/* ----------------------------------------------------------------------------
 * small text utilities shared by modules
 * ------------------------------------------------------------------------- */

/* Copy `in` into `out` lowercased and with surrounding blanks trimmed, so
 * modules can match on intent without caring about case or stray spaces. */
static void normalize(const char *in, char *out, size_t out_size) {
    if (out_size == 0) return;
    while (*in && isspace((unsigned char)*in)) in++;       /* skip leading */
    size_t n = 0;
    for (; in[n] && n + 1 < out_size; n++) {
        out[n] = (char)tolower((unsigned char)in[n]);
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

