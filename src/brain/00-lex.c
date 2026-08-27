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
 * KB-backed form of a `kb_cue_match(b, "00_lex_cue58", norm) || kb_cue_match(b, "00_lex_cue58", norm)` chain. Like kb_intent_match but
 * with substring containment instead of whole-turn equality, so the cue set grows at
 * runtime with no code edit. */
static int kb_cue_match(Brain *b, const char *intent, const char *norm) {
    if (!b || !b->kb || !intent || !norm) return 0;
    const char *candidate[] = { intent };
    char winner[KB_TERM_LEN], proof[KB_EVIDENCE_PROOF_LEN]; int score = 0;
    return kb_hypothesis_best(b->kb, "intent_cue", norm,
                              candidate, 1, winner, sizeof winner,
                              &score, proof, sizeof proof) == 1;
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

typedef struct {
    const char *name;
    const char *value;
} KbResponseSlot;

/* The ONE slot-substitution engine (gen362: extracted from kb_response_slots so
 * every consumer of a KB-authored phrasing shares it — a taught template and a
 * reusable semantic_atom must fill "{name}" by the same rule).
 *
 * `strict` selects what an unbound placeholder means:
 *   0  leave it literal — a malformed or half-taught template must not silently
 *      lose information (the historical response_template behaviour);
 *   1  fail — the caller is proving a plan, and a slot it cannot bind means the
 *      claim is incomplete, so it must decline rather than speak.
 * A placeholder whose name begins with a capital asks for the value with its
 * first letter capitalized, so an atom can open a sentence with a slot. */
static int kb_fill_slots(const char *tpl, const KbResponseSlot *slots,
                         size_t nslots, int strict, char *out, size_t outsz) {
    if (!tpl || !out || outsz == 0) return 0;
    size_t o = 0;
    for (const char *c = tpl; *c && o + 1 < outsz; ) {
        int filled = 0;
        if (*c == '{') {
            const char *r = strchr(c + 1, '}');
            if (r) {
                size_t nl = (size_t)(r - (c + 1));
                int capital = nl && isupper((unsigned char)c[1]);
                for (size_t i = 0; i < nslots; i++) {
                    if (!slots[i].name || strlen(slots[i].name) != nl ||
                        strncasecmp(c + 1, slots[i].name, nl) != 0)
                        continue;
                    const char *v = slots[i].value ? slots[i].value : "";
                    if (strict && !*v) return 0;
                    size_t vl = strlen(v);
                    if (vl >= outsz - o) vl = outsz - o - 1;
                    memcpy(out + o, v, vl);
                    if (capital && vl)
                        out[o] = (char)toupper((unsigned char)out[o]);
                    o += vl;
                    c = r + 1; filled = 1;
                    break;
                }
                if (!filled && strict) return 0;
            }
        }
        if (!filled) out[o++] = *c++;
    }
    out[o < outsz ? o : outsz - 1] = '\0';
    return out[0] != '\0';
}

/* Fixed renderer for KB response templates with named slots. The wording and
 * placeholder layout remain knowledge; C only substitutes values. */
static int kb_response_slots(Brain *b, const char *intent,
                             const KbResponseSlot *slots, size_t nslots,
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
    /* gen388: il contatore di rotazione e' quello di QUESTA famiglia. */
    unsigned *pick = &b->response_pick;
    for (size_t i = 0; i < b->n_pick_keys; i++)
        if (strcmp(b->pick_by_key[i].key, intent) == 0) { pick = &b->pick_by_key[i].n; break; }
    if (pick == &b->response_pick &&
        b->n_pick_keys < sizeof b->pick_by_key / sizeof b->pick_by_key[0] &&
        strlen(intent) < sizeof b->pick_by_key[0].key) {
        size_t k = b->n_pick_keys++;
        snprintf(b->pick_by_key[k].key, sizeof b->pick_by_key[k].key, "%s", intent);
        b->pick_by_key[k].n = 0;
        pick = &b->pick_by_key[k].n;
    }
    size_t idx = *pick % n;
    {
        char tv[1][KB_TERM_LEN];
        const char *tq[1] = { NULL };
        if (kb_match(b->kb, "style_temperature", tq, 1, tv, 1) == 1 &&
            atoi(tv[0]) == 0)
            idx = 0;
    }
    char *p = tpl[idx];                        /* selected phrasing */
    (*pick)++;
    size_t l = strlen(p);
    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') { p[l - 1] = '\0'; p++; }  /* strip quotes */
    /* ── gen388: ANCHE I VALORI PARLANO LA LINGUA DEL TURNO ─────────────────
     *
     * Localizzare la cornice non basta: «{entity} ha {count} {unit}» riempito con
     * atomi inglesi da «poker ha 52 cards», che e' un ibrido peggiore di una
     * risposta interamente inglese, perche' sembra una svista invece che un
     * limite. La tabella per tradurli e' la STESSA che canonicalizza l'ingresso —
     * `tr/2` — quindi la realizzazione e' la canonicalizzazione al contrario, e
     * non serve una risorsa nuova.
     *
     * Guardie: solo atomi di una parola (una descrizione in prosa non si traduce
     * a pezzi), mai un `proper_name/1` (Rex resta Rex), mai un numero. Cio' che
     * `tr/2` non copre resta com'e': un'isola inglese e' onesta, un'invenzione no. */
    KbResponseSlot local[8];
    if (strcmp(lang, "en") != 0 && nslots && nslots <= 8) {
        for (size_t i = 0; i < nslots; i++) {
            local[i] = slots[i];
            const char *v = slots[i].value;
            if (!v || !*v || strchr(v, ' ') || strlen(v) >= KB_TERM_LEN) continue;
            if (isdigit((unsigned char)v[0])) continue;
            const char *pn[] = { v };
            if (kb_query(b->kb, "proper_name", pn, 1)) continue;
            char hit[1][KB_TERM_LEN];
            const char *q[2] = { v, NULL };
            if (kb_match(b->kb, "tr", q, 2, hit, 1) == 1) {
                static char loc[8][KB_TERM_LEN];
                snprintf(loc[i], KB_TERM_LEN, "%s", hit[0]);
                local[i].value = loc[i];
            }
        }
        slots = local;
    }
    if (!kb_fill_slots(p, slots, nslots, 0, out, outsz)) return 0;
    /* gen363: record WHICH frame spoke, so a rule over knowledge can later ask
     * what kind of reply this was without inspecting its words. */
    snprintf(b->turn_frame, sizeof b->turn_frame, "%s", intent);
    return 1;
}

static int kb_response(Brain *b, const char *intent, const char *slot,
                       char *out, size_t outsz) {
    const KbResponseSlot named[] = { { "name", slot ? slot : "" } };
    return kb_response_slots(b, intent, named, 1, out, outsz);
}

/* Un messaggio con un DEFAULT nel codice e la parola finale alla KB (gen382d).
 *
 * I moduli avevano ~140 risposte scritte come letterali in C: corrette, e mute.
 * Non erano interrogabili, non erano localizzabili, e soprattutto non erano
 * INSEGNABILI — dire a parrot0 "al posto di rispondere cosi' rispondi cosi'" non
 * poteva funzionare su una stringa che il motore porta dentro di se'.
 *
 * Migrarle tutte in una volta spostando il testo sarebbe stato rischioso (una
 * riga di KB dimenticata = una risposta che sparisce). Questo helper le rende
 * KB-first senza quel rischio: la KB decide se ha qualcosa da dire, altrimenti
 * vale il letterale. Il letterale non e' piu' LA risposta, e' il suo default —
 * e da quel momento ogni messaggio e' sovrascrivibile a runtime, in qualunque
 * lingua, senza ricompilare.
 *
 * La chiave e' il nome del messaggio; il testo in C resta come documentazione di
 * cosa dice di solito. */
static size_t put(const char *s, char *out, size_t out_size);   /* fwd */

static int kb_say(Brain *b, const char *key, const char *fallback,
                  char *out, size_t outsz) {
    char buf[1024];
    if (b && kb_response_slots(b, key, NULL, 0, buf, sizeof buf) && buf[0]) {
        put(buf, out, outsz);
        return 1;
    }
    put(fallback, out, outsz);
    return 1;
}


/* gen363 (motorize-the-class) — a reply carries the FRAME that produced it.
 *
 * A consumer that matched a request but had no facts for it used to write its
 * surrender as a literal sentence in C. That is a phrasebook (mantra #2) and,
 * worse, it is indistinguishable from an answer: the turn counts as handled, so
 * the last-resort planner never runs and the judge sees a guaranteed zero.
 *
 * gen362 tried to recognize those sentences with `wall_marker/1`. Matching the
 * text parrot0 itself produced is the wrong layer: a consumer that wrote "for
 * that situation" escaped a marker written "for that topic". So the engine keeps
 * only PROVENANCE — which frame spoke — and the meaning of a frame stays in the
 * KB (`gap_frame/1`). Teaching that a frame is a surrender, in any wording or
 * language, is one fact and zero C. The recording happens in kb_response_slots
 * above, so EVERY frame carries provenance, not just the ones a module thought
 * to mark. */

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
/* gen432 — LA SEQUENZA DI FUGA SI SCIOGLIE QUANDO IL TESTO ESCE.
 *
 * Una stringa della KB non poteva contenere una VIRGOLETTA: le virgolette
 * delimitano, e chi scriveva la sequenza di fuga se la ritrovava stampata con la
 * barra davanti — quindi un documento JSON, che di virgolette e' fatto, non si
 * poteva scrivere come conoscenza.
 *
 * Il primo tentativo fu scioglierla in `kb_dequote`, ed era il posto SBAGLIATO:
 * quella funzione lavora sul posto e certi chiamanti le passano la memoria della
 * KB, quindi lo spostamento dei byte corrompeva i fatti — l'induzione cominciava
 * a produrre predicati fatti di byte a caso (misurato: abduce.p0t). Qui si tocca
 * solo la COPIA che esce, e la barra da sola resta se' stessa, cosi' il punto
 * protetto di un'espressione regolare non viene alterato. */
static size_t put(const char *text, char *out, size_t out_size) {
    size_t n = strlen(text);
    if (n >= out_size) n = out_size - 1;
    memcpy(out, text, n);
    out[n] = '\0';
    char *r = out, *w = out;
    while (*r) {
        if (r[0] == '\\' && (r[1] == '"' || r[1] == '\\')) { *w++ = r[1]; r += 2; }
        else *w++ = *r++;
    }
    *w = '\0';
    return (size_t)(w - out);
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
static int is_article(Brain *b, const char *w);
static int is_stopword(Brain *b, const char *w);
static int is_conjunction(Brain *b, const char *w);
static char *strip_edge_punct(char *t);
static int is_internal_pred(const KB *kb, const char *pred);
int brain_policy_on(Brain *b, const char *key);       /* gen331: the effective policy */
/* gen371: the reasoning-sandbox seam and its substrate lookups (99-registry.c). */
int brain_scratch_init(Brain *scratch, Brain *parent);
int brain_fresh_token(Brain *b, const char *base, char *out, size_t n);
int brain_substrate_query(Brain *b, const char *pred,
                          const char *const *args, size_t argc);
int brain_substrate_knows(Brain *b, const char *pred);
size_t brain_substrate_match(Brain *b, const char *pred,
                             const char *const *args, size_t argc,
                             char out[][KB_TERM_LEN], size_t max);
void brain_mode(Brain *b, char *out, size_t cap);
static int run_shell(const char *cmd, char *out, size_t out_size);
static int identify_code_lang(const char *code, Brain *b);
static int mod_toolpolicy(Brain *b, const char *norm, const char *raw, char *out, size_t out_size);

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
int try_teach_form(Brain *b, const char *norm, const char *raw,
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

    /* gen337: the registry outgrew the gen214 bound (48 learnable rows > 32);
     * rows past the cap were silently unteachable. 96 leaves headroom. */
    char labels[96][KB_TERM_LEN];
    const char *qa[3] = { NULL, NULL, NULL };
    size_t nl = kb_match(b->kb, "learnable", qa, 3, labels, 96);
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

        const char *pred; int from_raw; int unary = 0; int define = 0;
        /* LA MANIGLIA GENERICA (M11).
         *
         * Misurato: 222 famiglie hanno una `intent_cue`, 301 hanno un
         * `response_template`, e soltanto una quarantina ha una riga
         * `learnable/3` che le nomini. Il 90% del comportamento di un agente
         * conversazionale — umore, cortesia, battute, deflessioni, registri —
         * era gia' interamente dichiarativo e restava comunque fuori dalla
         * portata di chi insegna parlando, per mancanza della sola maniglia.
         *
         * Scrivere un'etichetta per famiglia sarebbe un frasario, e non
         * starebbe nemmeno nel tetto di questa enumerazione. Qui invece la
         * lezione NOMINA la famiglia: «learn "…" as a cue for mood_tired».
         * Le famiglie esistenti sono conoscenza, quindi una famiglia aggiunta
         * domani e' insegnabile lo stesso giorno, senza righe nuove. */
        char family[KB_TERM_LEN] = "";
        int generic = 0;
        if (!strcmp(mode[0], "cue_for") || !strcmp(mode[0], "reply_for")) {
            const char *after = strstr(low, ls);
            if (!after) continue;
            after += strlen(ls);
            while (*after && (isspace((unsigned char)*after) || *after == '"'))
                after++;
            size_t fl = 0;
            while (after[fl] && (isalnum((unsigned char)after[fl]) ||
                                 after[fl] == '_') && fl + 1 < sizeof family) {
                family[fl] = after[fl]; fl++;
            }
            family[fl] = '\0';
            if (!family[0]) continue;
            /* Una famiglia si insegna, non si inventa: deve gia' esistere,
             * altrimenti la lezione scriverebbe in un cassetto che nessuno
             * apre — il caso `greeting(ahoy)` di conoscenza morta. */
            /* E il controllo e' quello GIUSTO per la lezione: una cue ha senso
             * solo dove qualcuno legge cue, una risposta solo dove qualcuno
             * rende risposte. Attaccare una cue a una famiglia che nessuno
             * interroga per cue produrrebbe il fatto morto di `greeting(ahoy)`:
             * vero in KB, invisibile al comportamento. */
            char row[1][KB_TERM_LEN];
            const char *wanted = !strcmp(mode[0], "cue_for") ? "intent_cue"
                                                             : "response_template";
            const char *famq[2] = { family, NULL };
            if (kb_match(b->kb, wanted, famq, 2, row, 1) == 0) {
                char msg[256];
                const char *other = !strcmp(mode[0], "cue_for") ? "response_template"
                                                                : "intent_cue";
                if (kb_match(b->kb, other, famq, 2, row, 1) > 0)
                    snprintf(msg, sizeof msg,
                             "I know %s, but nothing reads its %s, so that lesson "
                             "would not change what I do.", family, wanted);
                else
                    snprintf(msg, sizeof msg,
                             "I don't have a family called %s, so I can't attach "
                             "that to it.", family);
                put(msg, out, outsz);
                return 1;
            }
            generic = 1;
        }
        if      (generic && !strcmp(mode[0], "cue_for"))
                                                { pred = "intent_cue";        from_raw = 0; }
        else if (generic)                       { pred = "response_template"; from_raw = 1; }
        else if (!strcmp(mode[0], "exact"))     { pred = "intent_phrase";     from_raw = 0; }
        else if (!strcmp(mode[0], "substring")) { pred = "intent_cue";        from_raw = 0; }
        else if (!strcmp(mode[0], "fill"))      { pred = "response_template"; from_raw = 1; }
        else if (!strcmp(mode[0], "unary"))     { pred = intent[0];           from_raw = 0; unary = 1; }
        else if (!strcmp(mode[0], "define"))    { pred = intent[0];           from_raw = 1; define = 1; }
        else continue;
        if (generic) snprintf(intent[0], KB_TERM_LEN, "%s", family);

        /* span: raw (keep casing) for fill; the canonicalized `norm` for exact/substring
         * so the stored form matches what the recognizer sees on a later turn. */
        const char *s1 = rq1, *s2 = rq2;
        if (!from_raw) {
            const char *n1 = strchr(norm, '"'), *n2 = n1 ? strchr(n1 + 1, '"') : NULL;
            if (n2 && n2 > n1 + 1) { s1 = n1; s2 = n2; }
        }
        size_t pl = (size_t)(s2 - (s1 + 1));
        if (pl >= KB_TERM_LEN - 2) {
            /* gen337: a teach-shaped turn whose quoted span exceeds the atom
             * bound used to fall through SILENTLY — and a later module (the
             * story generator) would fabricate a reply to a teaching turn: a
             * misclaim, worse than a wall. Name the mechanical limit instead. */
            char lim[160];
            snprintf(lim, sizeof lim,
                     "That quoted span is too long for one fact (%zu chars, "
                     "limit %d) - can you shorten it?", pl, KB_TERM_LEN - 3);
            put(lim, out, outsz);
            return 1;
        }
        char phrase[KB_TERM_LEN]; memcpy(phrase, s1 + 1, pl); phrase[pl] = '\0';

        char quoted[KB_TERM_LEN]; snprintf(quoted, sizeof quoted, "\"%s\"", phrase);
        kb_set_origin(b->kb, KB_SESSION);
        if (define) {
            /* gen337: definitional teaching — the capture side of the concept
             * consumer that already existed (kb_concept_def reads any
             * pred(Key, "Description")). The TERM being defined is what sits
             * between the label ("definition of"/"definizione di" — KB data)
             * and the quoted description; spaces become '_' so a multi-word
             * term matches the what-is join key. The label names WHICH
             * predicate to assert (the Intent column) — knowledge, not C. */
            const char *lp = strstr(low, ls);
            if (!lp) continue;
            lp += strlen(ls);
            while (*lp == ' ') lp++;
            char term[KB_TERM_LEN]; size_t tl = 0;
            while (lp[tl] && lp[tl] != ':' && lp[tl] != '"' &&
                   tl + 1 < sizeof term) { term[tl] = lp[tl]; tl++; }
            while (tl && (term[tl - 1] == ' ' || term[tl - 1] == ':' ||
                          term[tl - 1] == ',')) tl--;
            term[tl] = '\0';
            for (size_t t = 0; t < tl; t++) if (term[t] == ' ') term[t] = '_';
            if (!tl) continue;
            const char *ard[2] = { term, quoted };
            kb_assert(b->kb, pred, ard, 2);
            char dmsg[320];
            snprintf(dmsg, sizeof dmsg, "Got it - %s: %s.", term, phrase);
            put(dmsg, out, outsz);
            return 1;
        }
        if (unary) {
            const char *ar1[1] = { phrase };
            kb_assert(b->kb, pred, ar1, 1);
        } else if (from_raw) {
            /* Fill mode: assert with the session language so the
             * taught phrasing is reachable. Query current_language/1
             * directly from the KB (current_lang() is in a later .c). */
            char clang[8] = "en";
            {
                const char *cq[] = { NULL };
                char ch[1][KB_TERM_LEN];
                if (kb_match(b->kb, "current_language", cq, 1, ch, 1) > 0)
                    snprintf(clang, sizeof clang, "%s", ch[0]);
            }
            if (strcmp(clang, "en") != 0) {
                const char *ar3[3] = { intent[0], clang, quoted };
                kb_assert(b->kb, pred, ar3, 3);
            } else {
                const char *ar2[2] = { intent[0], quoted };
                kb_assert(b->kb, pred, ar2, 2);
            }
        } else {
            const char *ar[2] = { intent[0], quoted };
            kb_assert(b->kb, pred, ar, 2);
        }
        char msg[256];
        if (generic)
            snprintf(msg, sizeof msg, "Got it - I'll take \"%s\" as %s %s now.",
                     phrase, ls, family);
        else
            snprintf(msg, sizeof msg, "Got it - I'll take \"%s\" as a way to %s now.", phrase, ls);
        put(msg, out, outsz);
        return 1;
    }
    return 0;
}

static void singularize_kb(Brain *b, const char *word, char *out, size_t sz);
static int lex_class_member(Brain *b, const char *cls, const char *word);

/* gen431 — DOPO IL SINTAGMA C'E' UNA FRASE? Allora il contenuto e' ALLEGATO.
 *
 * «in this story rex is a dragon» nomina «questa storia» e poi la RIEMPIE: e'
 * un turno che apre un mondo, non una richiesta incompleta, e chiedere di
 * incollare la storia sarebbe non aver letto la frase (misurato: world.p0t).
 * Il segno e' la copula — «rex E' un drago» — e quali parole siano copule e'
 * gia' conoscenza. Un modificatore («in its historical context», «from playful
 * to tragic») non ne ha nessuna, e infatti non allega niente. */
static int p0_clause_follows(Brain *b, char **w, size_t nw, size_t from) {
    for (size_t i = from + 1; i < nw; i++) {
        char t[64]; snprintf(t, sizeof t, "%s", w[i]);
        char *c = strip_edge_punct(t);
        if (!*c) continue;
        const char *cq[1] = { c };
        if (kb_query(b->kb, "clause_copula", cq, 1)) return 1;
    }
    return 0;
}


/* gen431 — «QUESTO» CHE COSA? Il referente che non e' stato allegato.
 *
 * Dodici dei cento fallimenti (docs/plans/parrot0-100-failures.md) chiedono di
 * lavorare su un testo che nessuno ha allegato: «explain this stack trace»,
 * «refactor this function», «translate this paragraph». Non sono richieste
 * difficili — sono richieste INCOMPLETE, e la risposta giusta e' dirlo e
 * chiedere il pezzo, invece di produrre un paragrafo generico.
 *
 * Il riconoscimento sta qui, in un posto solo, perche' serve a DUE moduli: chi
 * compone deve tacere (altrimenti si mette a comporre su un testo che non ha) e
 * chi ripara deve chiedere. Quali generi di contenuto e quali dimostrativi
 * esistano e' conoscenza (`content_kind/1`, `demonstrative_word/1`).
 *
 * Restituisce 1 e scrive il genere se il turno nomina «questo <genere>» e il
 * contenuto non c'e': turno corto e su una riga sola. Se il testo e' allegato —
 * piu' righe, o un turno lungo — questo non e' il caso e la funzione tace. */
static int p0_unattached_kind(Brain *b, const char *norm, const char *raw,
                              char *kind, size_t ksz) {
    if (!b || !b->kb || !norm) return 0;
    if (raw && strchr(raw, '\n')) return 0;
    /* gen431 — I DUE PUNTI ALLEGANO. «which line matters most in this log: INFO
     * ready, WARN retry, ERROR database refused?» nomina «questo log» e POI lo
     * porta: chiedere di incollarlo sarebbe non aver letto la frase. Il segno e'
     * strutturale — due punti seguiti da qualcosa di sostanzioso — e vale per
     * ogni genere senza sapere niente del genere. */
    {
        const char *colon = strchr(norm, ':');
        if (colon && strlen(colon) > 8) return 0;
    }
    char buf[512];
    if (strlen(norm) >= sizeof buf) return 0;
    snprintf(buf, sizeof buf, "%s", norm);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw < 2 || nw > 14) return 0;
    for (size_t i = 0; i + 1 < nw; i++) {
        char dbuf[64]; snprintf(dbuf, sizeof dbuf, "%s", w[i]);
        const char *dq[1] = { strip_edge_punct(dbuf) };
        /* gen431 — anche un PLURALE NUMERATO apre lo stesso vuoto: «compare two
         * graphs structurally» non porta i due grafi piu' di quanto «explain
         * this poem» porti la poesia. Quali parole contino come quantificatore
         * e' conoscenza (`countable_opener/1`); il singolare lo fa la
         * morfologia che c'e' gia'. */
        int counted = kb_query(b->kb, "countable_opener", dq, 1);
        /* gen432 — «why did A FUNCTION return early?» apre lo stesso vuoto di
         * «why did THIS function…»: l'articolo indeterminativo in una DOMANDA
         * indica una cosa precisa che chi chiede ha in mente e non ha allegato.
         * Solo in una domanda: «design a test for a race condition» e' un ordine
         * di produrre, non una richiesta su qualcosa che manca. */
        int indefinite = 0;
        if (!counted && kb_query(b->kb, "indefinite_opener", dq, 1)) {
            char q0[64]; snprintf(q0, sizeof q0, "%s", w[0]);
            const char *qq0[1] = { strip_edge_punct(q0) };
            if (kb_query(b->kb, "question_word", qq0, 1)) indefinite = 1;
            /* Ma non se prima dell'articolo c'e' gia' una COPULA: «why isn't zed
             * a trace?» non chiede una traccia che manca, chiede perche' zed non
             * appartenga a una classe — e li «trace» e' un predicato, non un
             * documento da allegare (misurato: abduce_chain.p0t). */
            for (size_t z = 0; z < i && indefinite; z++) {
                char cz[64]; snprintf(cz, sizeof cz, "%s", w[z]);
                const char *czq[1] = { strip_edge_punct(cz) };
                if (kb_query(b->kb, "clause_copula", czq, 1)) indefinite = 0;
            }
        }
        if (!counted && !indefinite && !kb_query(b->kb, "demonstrative_word", dq, 1)) continue;
        /* il genere e' la TESTA del sintagma, non la parola subito dopo: in
         * «this stack trace» il genere e' «trace», e fermarsi al primo token
         * perdeva meta' dei casi. Si guardano i tre token successivi. */
        for (size_t k = i + 1; k < nw && k <= i + 3; k++) {
            char kbuf[64]; snprintf(kbuf, sizeof kbuf, "%s", w[k]);
            char *h = strip_edge_punct(kbuf);
            const char *kq[1] = { h };
            if (kb_query(b->kb, "content_kind", kq, 1)) {
                if (p0_clause_follows(b, w, nw, k)) return 0;
                snprintf(kind, ksz, "%s", h);
                return 1;
            }
            if (counted) {
                /* TODO(kb-first) (F., gen431): il passaggio plurale→singolare
                 * deve essere fatto in KB, non da una funzione del C.
                 * `singularize_kb` legge gia' regole di morfologia, ma la
                 * chiamata e' cablata qui: la forma giusta e' una relazione
                 * interrogabile, cosi' una lingua nuova non passa da questo file. */
                char sing[KB_TERM_LEN];
                singularize_kb(b, h, sing, sizeof sing);
                const char *sq[1] = { sing };
                if (*sing && kb_query(b->kb, "content_kind", sq, 1)) {
                    if (p0_clause_follows(b, w, nw, k)) return 0;
                    snprintf(kind, ksz, "%s", h);
                    return 1;
                }
            }
            continue;
        }
    }
    return 0;
}
