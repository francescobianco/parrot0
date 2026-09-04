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

/* ⛔ `matches_any` NON ESISTE PIU', ed e' un risultato, non una pulizia.
 *
 * La sua firma era `(stringa, elenco di letterali in C)`: la sua unica ragione
 * d'essere era confrontare una parola con un frasario compilato. Finche' e'
 * esistita, aggiungere una lista cablata costava zero attrito — e infatti
 * l'ultima superstite erano i ventidue riempitivi d'equazione in due lingue.
 *
 * Portata quella classe in `equation_filler/1`, la funzione e' rimasta senza
 * chiamanti. Toglierla chiude la porta al pattern: chi vorra' rifarlo dovra'
 * riscriverla, e a quel punto la domanda «perche' non e' un fatto?» si pone da
 * sola. Le classi lessicali si chiedono a `lex_class_member` (KB). */

/* gen211 (cardinal KB-first principle): true if the normalized input `norm` exactly
 * matches any surface form registered for `intent` as intent_phrase(intent, "form")
 * in the KB. The phrase forms are KNOWLEDGE, not a C array — so the class grows at
 * runtime: teach a new form, assert another intent_phrase/2, and this same matcher
 * fires with no code change (the KB-migration law of gen193, lifted from closed-class
 * words to multi-word idioms). The stored atom keeps its surrounding quotes (kb.c
 * parse_term), so we strip them before comparing. */
static char *kb_dequote(char *s);   /* definito piu' avanti */

/* ── SC32/D27 — UNA CUE NON GUARDA DENTRO UNA MENZIONE ────────────────────
 *
 * Una cue `substring` si accendeva dentro la frase che provava a RITRATTARLA:
 * «forget "helyla friend" as a casual opener» contiene la locuzione, quindi il
 * modulo sociale prendeva il turno e il retract non arrivava mai a destinazione.
 * Il contratto di `AGENTS.md` — ritrattare una cue deve togliere il
 * riconoscimento — era inesprimibile per un'intera classe.
 *
 * La cura non e' una guardia in un modulo: e' il principio uso/menzione
 * applicato al matching. Qui c'e' solo il mascheramento dei byte; quali
 * superfici aprano una menzione (`mention_delimiter/2`) e quali relazioni
 * debbano ignorarne il contenuto (`cue_scope/2`) sono fatti.
 *
 * Il costo per un turno senza citazioni e' una `strpbrk`: senza delimitatore
 * presente si restituisce l'originale e non si legge la KB. */
static const char *cue_visible_text(Brain *b, const char *relation,
                                    const char *norm, char *buf, size_t bufsz) {
    if (!b || !b->kb || !norm || !buf || bufsz == 0) return norm;
    char opens[8][KB_TERM_LEN];
    const char *dq[2] = { NULL, NULL };
    size_t nd = kb_match(b->kb, "mention_delimiter", dq, 2, opens, 8);
    if (nd == 0) return norm;
    char marks[16]; size_t nm = 0;
    for (size_t i = 0; i < nd && nm + 1 < sizeof marks; i++) {
        char o[KB_TERM_LEN]; snprintf(o, sizeof o, "%s", opens[i]);
        const char *d = kb_dequote(o);
        /* gen432: la sequenza di fuga si scioglie quando il testo ESCE dalla
         * KB, non dentro `kb_dequote`. Una virgoletta e' scritta `\"` nel
         * fatto, quindi qui si legge il byte vero. */
        if (d[0] == '\\' && d[1]) marks[nm++] = d[1];
        else if (d[0]) marks[nm++] = d[0];
    }
    marks[nm] = '\0';
    if (nm == 0 || !strpbrk(norm, marks)) return norm;

    const char *sq[2] = { relation, "outside_role(mention)" };
    if (!kb_query(b->kb, "cue_scope", sq, 2)) return norm;

    size_t n = strlen(norm);
    if (n + 1 > bufsz) return norm;
    char quote = 0;
    for (size_t i = 0; i < n; i++) {
        char c = norm[i];
        if (!quote && strchr(marks, c)) { quote = c; buf[i] = ' '; continue; }
        if (quote && c == quote) { quote = 0; buf[i] = ' '; continue; }
        buf[i] = quote ? ' ' : c;
    }
    buf[n] = '\0';
    /* Una citazione aperta e mai chiusa non e' una menzione: e' testo. */
    if (quote) return norm;
    return buf;
}

static int kb_intent_match(Brain *b, const char *intent, const char *norm) {
    if (!b || !b->kb || !intent || !norm) return 0;
    char masked[512];
    norm = cue_visible_text(b, "intent_phrase", norm, masked, sizeof masked);
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
static int lex_class_member(Brain *b, const char *cls, const char *word);

static int lex_class_member(Brain *b, const char *cls, const char *word);
static int lex_prefix_member(Brain *b, const char *cls, const char *word);

static int kb_cue_match_plain(Brain *b, const char *intent, const char *norm) {
    if (!b || !b->kb || !intent || !norm) return 0;
    char masked[512];
    norm = cue_visible_text(b, "intent_cue", norm, masked, sizeof masked);
    const char *candidate[] = { intent };
    char winner[KB_TERM_LEN], proof[KB_EVIDENCE_PROOF_LEN]; int score = 0;
    return kb_hypothesis_best(b->kb, "intent_cue", norm,
                              candidate, 1, winner, sizeof winner,
                              &score, proof, sizeof proof) == 1;
}

/* ── gen489 (F.) — LA CONGIUNZIONE E' CONOSCENZA, NON UN `if` ────────────────
 *
 * F., 2026-09-03, davanti a questa forma:
 *
 *     if (kb_cue_match(b, "…_cue1550",   q) &&
 *         kb_cue_match(b, "…_cue1550_2", q) &&
 *         kb_cue_match(b, "…_chain1554", q)) { … }
 *
 *     «se ha questa catena di && a runtime volessi aggiungere un nuovo elemento
 *      tramite addestramento tu non puoi farlo, perche' e' la catena di && che
 *      deve diventare essa stessa una regola nella KB»
 *
 * E' un piano piu' su del mantra #2. Con le classi al posto giusto si insegna un
 * MEMBRO di un ruolo che esiste gia'; non si insegna una FORMA nuova, perche' la
 * congiunzione — quali condizioni, quante, con quale polarita' — resta compilata.
 * Finche' e' li', l'insieme delle forme che parrot0 puo' riconoscere e' CHIUSO, e
 * nessuna lezione lo apre. Misurato al gen489: 213 istruzioni con due o piu'
 * `kb_cue_match` in `&&`, fino a quindici congiunti in una sola.
 *
 * Qui la congiunzione diventa un insieme di fatti:
 *
 *     turn_pattern(Forma, cue,     Classe)   il turno porta una cue di Classe
 *     turn_pattern(Forma, not_cue, Classe)   e non ne porta una di Classe
 *     turn_pattern(Forma, word,    Classe)   un token del turno e' membro di Classe
 *     turn_pattern(Forma, text,    "…")      il turno contiene questa superficie
 *     turn_pattern_intent(Forma, Intento)    che cosa vale il turno se la forma tiene
 *
 * La CONGIUNZIONE e' l'insieme dei fatti che condividono il nome della forma:
 * valgono tutti. Il motore non sa quante siano ne' quali: le chiede. Percio' una
 * forma nuova — con quante condizioni si vuole — e' un gruppo di `kb.assert` a
 * runtime, e vale dal turno dopo senza ricompilare. Il test del mantra #2, che
 * per la catena falliva, ora si supera: `tests/p0t/language/taught_turn_form.p0t`.
 *
 * L'aggancio e' dentro `kb_cue_match`, che e' la strozzatura da cui passano tutti
 * i 1052 siti che chiedono «questo turno e' di questa classe?» — la lezione delle
 * tre volte in `universal-comprehension.md`: si cerca il punto che tutte le vie
 * attraversano, non si enumerano i chiamanti. Costo per chi non ha forme
 * dichiarate: una query su un predicato assente. */

static int p0_turn_has_word(Brain *b, const char *norm, const char *cls) {
    const char *p = norm;
    while (*p) {
        while (*p && !isalnum((unsigned char)*p) && *p != '\'') p++;
        const char *s = p;
        while (*p && (isalnum((unsigned char)*p) || *p == '\'')) p++;
        if (p > s) {
            char tok[KB_TERM_LEN];
            size_t n = (size_t)(p - s);
            if (n >= sizeof tok) n = sizeof tok - 1;
            memcpy(tok, s, n); tok[n] = '\0';
            for (char *c = tok; *c; c++) *c = (char)tolower((unsigned char)*c);
            if (lex_class_member(b, cls, tok)) return 1;
        }
    }
    return 0;
}

/* Tutte le condizioni di `pat` valgono sul turno? `*seen` riceve quante ne sono
 * state trovate, cosi' il chiamante distingue «forma soddisfatta» da «forma che
 * non esiste»: una forma senza condizioni non e' vera, e' vuota. */
static int p0_turn_pattern_holds(Brain *b, const char *pat, const char *norm,
                                 size_t *seen) {
    static const char *KIND[] = { "cue", "not_cue", "word", "text" };
    size_t total = 0;
    for (size_t k = 0; k < sizeof KIND / sizeof KIND[0]; k++) {
        char args[16][KB_TERM_LEN];
        const char *q[3] = { pat, KIND[k], NULL };
        size_t n = kb_match(b->kb, "turn_pattern", q, 3, args, 16);
        total += n;
        for (size_t i = 0; i < n; i++) {
            char ab[KB_TERM_LEN]; snprintf(ab, sizeof ab, "%s", args[i]);
            const char *a = kb_dequote(ab);
            int ok;
            switch (k) {
                case 0: ok =  kb_cue_match_plain(b, a, norm); break;
                case 1: ok = !kb_cue_match_plain(b, a, norm); break;
                case 2: ok =  p0_turn_has_word(b, norm, a);   break;
                default: ok = (*a && strstr(norm, a) != NULL); break;
            }
            if (!ok) { if (seen) *seen = total; return 0; }
        }
    }
    if (seen) *seen = total;
    return total > 0;
}

static int kb_cue_match(Brain *b, const char *intent, const char *norm) {
    if (kb_cue_match_plain(b, intent, norm)) return 1;
    /* La forma DICHIARATA vale quanto la cue: se qualcuno ha insegnato una
     * congiunzione per questo intento, il turno la puo' soddisfare. */
    if (!b || !b->kb || !intent || !norm) return 0;
    if (!kb_knows_pred(b->kb, "turn_pattern_intent")) return 0;
    char pats[16][KB_TERM_LEN];
    const char *q[3] = { NULL, intent, NULL };
    size_t n = kb_match(b->kb, "turn_pattern_intent", q, 2, pats, 16);
    for (size_t i = 0; i < n; i++) {
        size_t seen = 0;
        if (p0_turn_pattern_holds(b, pats[i], norm, &seen)) return 1;
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
    if (!lex_class_member(b, "00_lex_lex146", lang)) {
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
    if (!lex_class_member(b, "00_lex_lex198", lang) && nslots && nslots <= 8) {
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

/* IL DEFAULT NON E' UNA FRASE (gen443).
 *
 * F.: «i default previsti nel caso in cui la KB non sia addestrata non devono
 * essere formali e umanizzati ma meccanicisti — una versione funzionale del
 * messaggio. Non devono esistere testi umanizzati di nessun tipo nel C.»
 *
 * E' piu' netto di quello che il mantra #16 chiedeva, ed e' giusto: finche' nel
 * C resta una frase «bella», quella frase e' la voce vera e la riga KB e' un
 * ornamento. Qui il motore non sa parlare — sa solo dire QUALE messaggio
 * starebbe dando e con quali valori. La forma e' quella di un termine, che e'
 * la lingua in cui questo progetto scrive tutto il resto:
 *
 *     conjunction_taught(blen)
 *
 * Se si legge un termine in chat, non e' un errore: e' una famiglia che nessuno
 * ha ancora insegnato, e si vede subito quale. */
static int kb_term_say(Brain *b, const char *key,
                       const KbResponseSlot *slots, size_t n,
                       char *out, size_t outsz) {
    if (b && kb_response_slots(b, key, slots, n, out, outsz) && out[0]) return 1;
    size_t o = (size_t)snprintf(out, outsz, "%s", key ? key : "?");
    if (n && slots) {
        if (o < outsz) o += (size_t)snprintf(out + o, outsz - o, "(");
        for (size_t i = 0; i < n && o < outsz; i++)
            o += (size_t)snprintf(out + o, outsz - o, "%s%s", i ? ", " : "",
                                  slots[i].value ? slots[i].value : "");
        if (o < outsz) snprintf(out + o, outsz - o, ")");
    }
    return 1;
}

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

static int mod_answer_frame(Brain *b, const char *norm, const char *raw,
                            char *out, size_t out_size);

/* gen490 — QUALE DOMANDA CONTIENE UN TESTO. Una volta sola.
 *
 * `answer_frame(Superficie, Relazione)` dice che una domanda che contiene
 * Superficie interroga Relazione. La risoluzione — enumerare le superfici
 * dichiarate, provare la PIU' SPECIFICA per prima, tenere quelle che il testo
 * contiene — viveva dentro `mod_answer_frame`, cioe' dentro il consumer che
 * risponde. Appena e' servita a un secondo lettore (l'atto didattico che ancora
 * una formulazione nuova a una che gia' funziona) la strada facile era
 * riscriverla: e' il mantra #5, e il duplicato sarebbe divergiuto al primo
 * cambiamento di uno dei due.
 *
 * Qui invece e' un motore solo, e non conosce nessuna cue, lingua o relazione:
 * la specificita' e' una proprieta' della SPAN di evidenza, non una precedenza
 * cablata fra predicati. Nessun tetto sul numero di superfici — la KB le fa
 * crescere, e un tetto fisso su una lista che cresce e' la bomba a tempo del
 * gen432.
 *
 * Restituisce quante superfici combaciano, dalla piu' specifica alla piu'
 * generica, nella forma CITATA con cui la KB le tiene (la chiave per rileggere
 * la relazione). Il chiamante libera `*hits`. */
static size_t answer_frame_surfaces(Brain *b, const char *text,
                                    char (**hits)[KB_TERM_LEN]) {
    *hits = NULL;
    if (!b || !b->kb || !text) return 0;
    char (*cues)[KB_TERM_LEN] = NULL;
    size_t nf = 0;
    const char *fq[2] = { NULL, NULL };
    if (!kb_match_all(b->kb, "answer_frame", fq, 2, &cues, &nf) || nf == 0) {
        free(cues);
        return 0;
    }
    /* Piu' superfici della KB possono sovrapporsi in un testo: si prova la piu'
     * specifica per prima. A parita' di lunghezza l'ordine resta quello di
     * inserimento, cosi' il contratto additivo fra righe con la stessa cue non
     * cambia. */
    for (size_t i = 1; i < nf; i++) {
        char selected[KB_TERM_LEN], probe[KB_TERM_LEN];
        snprintf(selected, sizeof selected, "%s", cues[i]);
        snprintf(probe, sizeof probe, "%s", selected);
        size_t selected_len = strlen(kb_dequote(probe));
        size_t j = i;
        while (j > 0) {
            snprintf(probe, sizeof probe, "%s", cues[j - 1]);
            if (strlen(kb_dequote(probe)) >= selected_len) break;
            memcpy(cues[j], cues[j - 1], sizeof cues[j]);
            j--;
        }
        if (j != i) memcpy(cues[j], selected, sizeof cues[j]);
    }
    size_t n = 0;
    for (size_t i = 0; i < nf; i++) {
        char probe[KB_TERM_LEN];
        snprintf(probe, sizeof probe, "%s", cues[i]);
        const char *cd = kb_dequote(probe);
        if (!*cd || !cue(text, cd)) continue;
        if (n != i) memcpy(cues[n], cues[i], sizeof cues[n]);
        n++;
    }
    if (n == 0) { free(cues); return 0; }
    *hits = cues;
    return n;
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
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%s", b->last_goal_arg);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%s", b->last_goal_pred);
  const KbResponseSlot _rs[] = { { "last_goal_arg", _v0 }, { "last_goal_pred", _v1 } };
          kb_term_say(b, "now_x_is_a_x_after_all", _rs, 2, note, sizeof note); }
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
int p0_turn_opens_as_question(Brain *b, const char *first_word);  /* definito in 10 */
static int is_stopword(Brain *b, const char *w);
static int is_conjunction(Brain *b, const char *w);
static char *strip_edge_punct(char *t);
static int is_internal_pred(const KB *kb, const char *pred);
static int domain_query(Brain *b, const char *role, const char *const *args, size_t argc);
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
/* ── SC32/D28 — LA META' MANCANTE DELL'ATTO DIDATTICO ─────────────────────
 *
 * `try_teach_form` insegna un membro di una classe leggendo `learnable/3`.
 * Ritrattarlo non aveva un gemello, e per un'intera classe di lezioni il
 * contratto di `AGENTS.md` — «retracting the cue must remove that
 * recognition» — era inesprimibile: ogni frase che nomina la locuzione la
 * contiene, e l'unica che sfuggiva alla cue insegnava un fatto sul verbo di
 * retract (`casual_opener(forget)`, la specie che SC2-A aveva gia' guardato a
 * mano su un altro percorso).
 *
 * Questa non e' una guardia in un modulo: e' lo STESSO registro letto nell'altro
 * verso. Una classe nuova in `learnable/3` diventa insegnabile e ritrattabile
 * insieme, e nessun modulo cambia. Il verbo che apre la mossa e' un fatto
 * (`state_move_cue(_, retract)`), come per `mod_forget`.
 *
 * Corre PRIMA di `try_teach_form` di proposito: «unlearn» contiene «learn», e
 * il mantra #8 vale anche qui. */
static int try_forget_faculty_force(Brain *b, const char *norm,
                                    char *out, size_t outsz) {
    if (!b || !b->kb || !norm) return 0;
    char moves[16][KB_TERM_LEN];
    const char *mq[2] = { NULL, "retract" };
    size_t nm = kb_match(b->kb, "state_move_cue", mq, 2, moves, 16);
    int retract = 0;
    for (size_t i = 0; i < nm && !retract; i++) {
        char move[KB_TERM_LEN]; snprintf(move, sizeof move, "%s", moves[i]);
        if (cue(norm, kb_dequote(move))) retract = 1;
    }
    if (!retract) return 0;
    char (*faculties)[KB_TERM_LEN] = NULL;
    size_t nf = 0;
    const char *fq0[3] = { NULL, NULL, NULL };
    if (!kb_match_all(b->kb, "faculty_surface", fq0, 3,
                      &faculties, &nf)) return 0;
    char faculty[KB_TERM_LEN] = "";
    char faculty_name[KB_TERM_LEN] = "";
    for (size_t fi = 0; fi < nf && !faculty[0]; fi++) {
        const char *fq[3] = { faculties[fi], NULL, NULL };
        char langs[8][KB_TERM_LEN];
        size_t nl = kb_match(b->kb, "faculty_surface", fq, 3, langs, 8);
        for (size_t li = 0; li < nl && !faculty[0]; li++) {
            const char *sq[3] = { faculties[fi], langs[li], NULL };
            char surfaces[8][KB_TERM_LEN];
            size_t ns = kb_match(b->kb, "faculty_surface", sq, 3, surfaces, 8);
            for (size_t si = 0; si < ns; si++) {
                char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", surfaces[si]);
                /* `kb_dequote` NON e' idempotente: toglie la virgoletta finale
                 * in place e restituisce s+1. Chiamarlo due volte sullo stesso
                 * buffer restituisce la stringa con la virgoletta APERTA ancora
                 * attaccata — ed e' cosi' che «"the narrator» finiva dentro una
                 * resa. Si dequota UNA volta e si usa quel puntatore. */
                const char *surface = kb_dequote(sb);
                if (cue(norm, surface)) {
                    snprintf(faculty, sizeof faculty, "%s", faculties[fi]);
                    snprintf(faculty_name, sizeof faculty_name, "%s", surface);
                    break;
                }
            }
        }
    }
    free(faculties);
    if (!faculty[0]) return 0;

    char (*forces)[KB_TERM_LEN] = NULL;
    size_t nforces = 0;
    const char *lq[2] = { NULL, NULL };
    if (!kb_match_all(b->kb, "faculty_force_lesson", lq, 2,
                      &forces, &nforces)) return 0;
    char force[KB_TERM_LEN] = "";
    for (size_t fi = 0; fi < nforces && !force[0]; fi++) {
        const char *cq[2] = { forces[fi], NULL };
        char forms[32][KB_TERM_LEN];
        size_t nforms = kb_match(b->kb, "faculty_force_lesson", cq, 2,
                                 forms, 32);
        for (size_t i = 0; i < nforms; i++) {
            char form[KB_TERM_LEN]; snprintf(form, sizeof form, "%s", forms[i]);
            if (cue(norm, kb_dequote(form))) {
                snprintf(force, sizeof force, "%s", forces[fi]);
                break;
            }
        }
    }
    free(forces);
    if (!force[0]) return 0;

    const char *args[2] = { faculty, force };
    int gone = kb_retract(b->kb, "faculty_force", args, 2);
    char msg[320];
    const KbResponseSlot slots[] = { { "faculty", faculty_name } };
    if (!kb_term_say(b, gone ? "faculty_force_forgotten"
                             : "faculty_force_not_held",
                     slots, 1, msg, sizeof msg)) return 0;
    put(msg, out, outsz);
    return 1;
}

int try_forget_form(Brain *b, const char *norm, const char *raw,
                     char *out, size_t outsz) {
    if (!b || !b->kb || !raw || !norm) return 0;
    if (try_forget_faculty_force(b, norm, out, outsz)) return 1;
    char low[512];
    size_t ln = 0;
    for (const char *c = raw; *c && ln + 1 < sizeof low; c++)
        low[ln++] = (char)tolower((unsigned char)*c);
    low[ln] = '\0';

    char mv[8][KB_TERM_LEN];
    const char *mq[2] = { NULL, "retract" };
    size_t nm = kb_match(b->kb, "state_move_cue", mq, 2, mv, 8);
    int is_retract = 0;
    for (size_t i = 0; i < nm && !is_retract; i++) {
        char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", mv[i]);
        if (cue(low, kb_dequote(cb))) is_retract = 1;
    }
    if (!is_retract) return 0;

    const char *rq1 = strchr(raw, '"'), *rq2 = rq1 ? strchr(rq1 + 1, '"') : NULL;
    if (!rq2 || rq2 <= rq1 + 1) return 0;

    char labels[96][KB_TERM_LEN];
    const char *qa[3] = { NULL, NULL, NULL };
    size_t nl = kb_match(b->kb, "learnable", qa, 3, labels, 96);
    for (size_t i = 0; i < nl; i++) {
        char lab[KB_TERM_LEN]; snprintf(lab, sizeof lab, "%s", labels[i]);
        char *ls = lab; size_t ll = strlen(ls);
        if (ll >= 2 && ls[0] == '"' && ls[ll - 1] == '"') { ls[ll - 1] = '\0'; ls++; }
        if (!*ls || !strstr(low, ls)) continue;

        char intent[1][KB_TERM_LEN], mode[1][KB_TERM_LEN];
        const char *qi[3] = { labels[i], NULL, NULL };
        if (kb_match(b->kb, "learnable", qi, 3, intent, 1) != 1) continue;
        const char *qm[3] = { labels[i], intent[0], NULL };
        if (kb_match(b->kb, "learnable", qm, 3, mode, 1) != 1) continue;

        /* Soltanto le modalita' che scrivono una SUPERFICIE riconosciuta hanno
         * un retract simmetrico ovvio. Le altre (fill, define, le maniglie
         * generiche) restano fuori e falliscono onestamente invece di
         * indovinare quale fatto togliere. */
        const char *pred = NULL;
        if      (lex_class_member(b, "00_lex_lex587", mode[0])) pred = "intent_phrase";
        else if (lex_class_member(b, "00_lex_lex588", mode[0])) pred = "intent_cue";
        else if (lex_class_member(b, "00_lex_lex590", mode[0])) pred = intent[0];
        else continue;

        /* La superficie memorizzata viene dalla forma canonicalizzata, come
         * all'atto dell'insegnamento: se le due letture divergessero, il
         * retract cercherebbe un fatto che non e' mai stato scritto. */
        const char *s1 = rq1, *s2 = rq2;
        const char *n1 = strchr(norm, '"'), *n2 = n1 ? strchr(n1 + 1, '"') : NULL;
        if (n2 && n2 > n1 + 1) { s1 = n1; s2 = n2; }
        size_t pl = (size_t)(s2 - (s1 + 1));
        if (pl == 0 || pl >= KB_TERM_LEN - 2) return 0;
        char phrase[KB_TERM_LEN]; memcpy(phrase, s1 + 1, pl); phrase[pl] = '\0';
        char quoted[KB_TERM_LEN]; snprintf(quoted, sizeof quoted, "\"%s\"", phrase);

        int gone;
        if (pred == intent[0]) {
            const char *ar1[1] = { phrase };
            gone = kb_retract(b->kb, pred, ar1, 1);
        } else {
            const char *ar2[2] = { intent[0], quoted };
            gone = kb_retract(b->kb, pred, ar2, 2);
        }
        char msg[256];
        kb_term_say(b, gone ? "taught_form_forgotten" : "taught_form_not_held",
                    (const KbResponseSlot[]){ { "phrase", phrase },
                                              { "label", ls } }, 2,
                    msg, sizeof msg);
        put(msg, out, outsz);
        return 1;
    }
    return 0;
}

/* ── gen491: PUO' QUESTA FACOLTA' PRENDERE IL TURNO? ────────────────────────
 *
 * Mantra #17: la condotta e' conoscenza quanto il lessico, e un turno rubato e'
 * un bug di CONOSCENZA, non di codice. Il motore non decide: legge la FORZA del
 * turno e chiede alla KB se questa facolta' la accetta.
 *
 * ⚠ Una facolta' senza `faculty_force/2` si comporta esattamente come prima.
 * Additivo per costruzione: nessun cancello implicito su condotte che nessuno
 * ha esaminato.
 *
 * La forza e' UNA lettura condivisa — non una lista di cue per facolta', che
 * sarebbe il mantra #2 al contrario (ed e' l'errore che questa funzione ha fatto
 * nella sua prima versione, vedi il blocco in kb/core/intents.p0). Migliorare la
 * lettura migliora ogni facolta' insieme. */
static int p0_turn_is(Brain *b, const char *force, const char *turn) {
    if (b && b->kb) {
        /* Una domanda BOOLEANA si fa con `kb_query`. Con `kb_match` non c'era
         * nessuno slot variabile da raccogliere, e una forza pubblicata nel
         * frame del turno risultava assente: la lettura c'era e nessuno la
         * vedeva. */
        const char *q[2] = { "current_turn", force };
        if (kb_query(b->kb, "turn_illocution", q, 2)) return 1;
    }
    char (*cues)[KB_TERM_LEN] = NULL; size_t nc = 0;
    const char *cq[2] = { force, NULL };
    if (!kb_match_all(b->kb, "illocution_cue", cq, 2, &cues, &nc)) return 0;
    int hit = 0;
    for (size_t i = 0; i < nc && !hit; i++) {
        char cb[KB_TERM_LEN];
        snprintf(cb, sizeof cb, "%s", cues[i]);
        const char *t = kb_dequote(cb);
        if (*t && cue(turn, t)) hit = 1;
    }
    free(cues);
    return hit;
}

static int p0_move_allowed(Brain *b, const char *faculty, const char *turn) {
    if (!b || !b->kb || !faculty || !turn) return 1;
    char (*forces)[KB_TERM_LEN] = NULL; size_t nf = 0;
    const char *fq[2] = { faculty, NULL };
    if (!kb_match_all(b->kb, "faculty_force", fq, 2, &forces, &nf) || nf == 0) {
        free(forces);
        return 1;                    /* nessuna condotta dichiarata: come prima */
    }
    int ok = 0;
    for (size_t i = 0; i < nf && !ok; i++) {
        char fb[KB_TERM_LEN];
        snprintf(fb, sizeof fb, "%s", forces[i]);
        const char *f = kb_dequote(fb);
        if (*f && p0_turn_is(b, f, turn)) ok = 1;
    }
    free(forces);
    return ok;
}

/* ── QUANDO UNA FACOLTA' CEDE IL TURNO ─────────────────────────────────────
 *
 * `p0_move_allowed` sopra dice quando una facolta' PUO' parlare, leggendo la
 * forza del turno. Questo e' il verso opposto e mancava: quando una facolta'
 * DEVE tacere perche' il turno appartiene a un'altra lettura.
 *
 * Era scritto come catena di `if (kb_cue_match(...)) return 0;` dentro quattro
 * moduli diversi. Il difetto e' quello del §1 di generation-kb-first.md, ed e'
 * di condotta, non di lessico: la politica NON HA NOME (esiste solo il fatto
 * che una riga viene prima di un'altra), non e' interrogabile (`who answered?`
 * dice chi ha risposto, mai perche' un altro non aveva diritto) e non e'
 * correggibile parlando — cioe' viola il mantra #17.
 *
 * `faculty_yield(Facolta', Stadio, Classe)` la nomina. Lo STADIO dice a che
 * punto del turno la cessione vale: cedere all'apertura e cedere dopo che la
 * facolta' ha gia' fatto competere i propri candidati sono due condotte
 * diverse, e confonderle perde artefatti legittimi. Da qui:
 *   - si interroga: `/debug faculty_yield` mostra l'intera condotta di cessione;
 *   - si estende senza ricompilare: una facolta' nuova che deve cedere a una
 *     lettura nuova e' UNA riga di KB;
 *   - si corregge parlando, perche' la facolta' ha gia' un nome pubblico in
 *     `faculty_surface/3`.
 *
 * `faculty_yield_both/4` e' la cessione CONGIUNTA: si cede solo se il turno
 * porta entrambe le prove. Serve dove una prova sola sarebbe troppo avida.
 *
 * Il turno si legge normalizzato E grezzo: cedere e' l'atto prudente, e una
 * facolta' che cede deve VEDERE DI PIU', non di meno. */
static int p0_faculty_yields(Brain *b, const char *faculty, const char *stage,
                             const char *norm, const char *raw) {
    if (!b || !b->kb || !faculty || !stage) return 0;
    int yield = 0;

    char (*classes)[KB_TERM_LEN] = NULL;
    size_t n = 0;
    const char *q[3] = { faculty, stage, NULL };
    if (kb_match_all(b->kb, "faculty_yield", q, 3, &classes, &n)) {
        for (size_t i = 0; i < n && !yield; i++) {
            char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", classes[i]);
            const char *cls = kb_dequote(cb);
            if ((norm && kb_cue_match(b, cls, norm)) ||
                (raw  && kb_cue_match(b, cls, raw))) yield = 1;
        }
    }
    free(classes);
    if (yield) return 1;

    /* ⛔ gen502 — LA CESSIONE CONGIUNTA LEGGEVA MEZZO TURNO.
     *
     * `faculty_yield/3` qui sopra prova `norm` E `raw`, perche' — dice il
     * commento in testa — «cedere e' l'atto prudente, e una facolta' che cede
     * deve VEDERE DI PIU', non di meno». La forma congiunta provava solo `norm`,
     * cioe' il turno canonicalizzato, che il dispatch tronca a 256 caratteri
     * (`canon[256]`, TODO gia' scritto in 99-registry.c).
     *
     * Misurato spostando la seconda cue dentro lo stesso testo: la cessione
     * scatta fino a colonna 217 e smette a colonna 262. Il precipizio E' la
     * finestra. Ne segue che la condotta piu' precisa che abbiamo era cieca
     * proprio sugli input per cui serve — una specifica di coding e' lunga per
     * natura: i prompt del banco sono 864, 1485 e 1839 byte.
     *
     * La cue si cerca nel turno intero, come fa gia' la forma singola. */
    char (*firsts)[KB_TERM_LEN] = NULL;
    size_t nf = 0;
    const char *bq[4] = { faculty, stage, NULL, NULL };
    if (kb_match_all(b->kb, "faculty_yield_both", bq, 4, &firsts, &nf)) {
        for (size_t i = 0; i < nf && !yield; i++) {
            char ab[KB_TERM_LEN]; snprintf(ab, sizeof ab, "%s", firsts[i]);
            const char *first = kb_dequote(ab);
            if (!((norm && kb_cue_match(b, first, norm)) ||
                  (raw  && kb_cue_match(b, first, raw)))) continue;
            const char *sq[4] = { faculty, stage, firsts[i], NULL };
            char seconds[8][KB_TERM_LEN];
            size_t ns = kb_match(b->kb, "faculty_yield_both", sq, 4, seconds, 8);
            for (size_t j = 0; j < ns && !yield; j++) {
                char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", seconds[j]);
                const char *second = kb_dequote(sb);
                if ((norm && kb_cue_match(b, second, norm)) ||
                    (raw  && kb_cue_match(b, second, raw))) yield = 1;
            }
        }
    }
    free(firsts);
    if (yield) return 1;

    /* La cessione per FORZA del turno, gemella di quella per cue. Serve dove il
     * motivo per tacere non e' una parola ma la LETTURA: «He reviewed the draft»
     * non contiene nessuna cue che dica «non sono una domanda» — lo dice il fatto
     * che lega un frame dichiarativo completo. `p0_turn_is` legge la forza
     * pubblicata nel frame del turno, quindi qui il C non nomina nessuna forza. */
    char (*forces)[KB_TERM_LEN] = NULL;
    size_t nfo = 0;
    const char *foq[3] = { faculty, stage, NULL };
    if (kb_match_all(b->kb, "faculty_yield_force", foq, 3, &forces, &nfo)) {
        for (size_t i = 0; i < nfo && !yield; i++) {
            char fb[KB_TERM_LEN]; snprintf(fb, sizeof fb, "%s", forces[i]);
            const char *force = kb_dequote(fb);
            if (*force && norm && p0_turn_is(b, force, norm)) yield = 1;
        }
    }
    free(forces);
    return yield;
}

/* Return the remainder after a KB-declared turn prefix. The comparison is
 * mechanical; the surface itself is learned data. */
static const char *kb_prefix_remainder(Brain *b, const char *class_name,
                                       const char *text) {
    if (!b || !b->kb || !class_name || !text) return NULL;
    char prefixes[32][KB_TERM_LEN];
    const char *q[1] = { NULL };
    size_t n = kb_match(b->kb, class_name, q, 1, prefixes, 32);
    for (size_t i = 0; i < n; i++) {
        char pbuf[KB_TERM_LEN]; snprintf(pbuf, sizeof pbuf, "%s", prefixes[i]);
        const char *prefix = kb_dequote(pbuf);
        size_t len = strlen(prefix);
        if (len && strncmp(text, prefix, len) == 0) return text + len;
    }
    return NULL;
}

/* #17: a faculty's conduct is teachable knowledge, not a guard hidden in C.
 * The faculty and the rule are both surfaces declared in the KB; this adapter
 * only joins the two readings and records the resulting policy. */
static int try_teach_faculty_force(Brain *b, const char *norm,
                                   char *out, size_t outsz) {
    if (!b || !b->kb || !norm) return 0;

    char (*lessons)[KB_TERM_LEN] = NULL;
    size_t nl = 0;
    const char *lq[2] = { NULL, NULL };
    if (!kb_match_all(b->kb, "faculty_force_lesson", lq, 2,
                      &lessons, &nl) || nl == 0) {
        free(lessons);
        return 0;
    }

    char faculty[KB_TERM_LEN] = "";
    char faculty_name[KB_TERM_LEN] = "";
    char (*faculties)[KB_TERM_LEN] = NULL;
    size_t nf = 0;
    const char *sq[3] = { NULL, NULL, NULL };
    if (kb_match_all(b->kb, "faculty_surface", sq, 3, &faculties, &nf)) {
        for (size_t fi = 0; fi < nf && !faculty[0]; fi++) {
            const char *fq[3] = { faculties[fi], NULL, NULL };
            char languages[8][KB_TERM_LEN];
            size_t nlanguages = kb_match(b->kb, "faculty_surface", fq, 3,
                                         languages, 8);
            for (size_t li = 0; li < nlanguages && !faculty[0]; li++) {
                const char *pq[3] = { faculties[fi], languages[li], NULL };
                char surfaces[8][KB_TERM_LEN];
                size_t ns = kb_match(b->kb, "faculty_surface", pq, 3,
                                     surfaces, 8);
                for (size_t si = 0; si < ns; si++) {
                    char s[KB_TERM_LEN]; snprintf(s, sizeof s, "%s", surfaces[si]);
                    char *surface = kb_dequote(s);
                    if (*surface && cue(norm, surface)) {
                        snprintf(faculty, sizeof faculty, "%s", faculties[fi]);
                        snprintf(faculty_name, sizeof faculty_name, "%s", surface);
                        break;
                    }
                }
            }
        }
    }
    free(faculties);
    if (!faculty[0]) { free(lessons); return 0; }

    char force[KB_TERM_LEN] = "";
    char lesson[KB_TERM_LEN] = "";
    for (size_t i = 0; i < nl && !force[0]; i++) {
        char force_text[KB_TERM_LEN]; snprintf(force_text, sizeof force_text, "%s", lessons[i]);
        char (*forms)[KB_TERM_LEN] = NULL;
        size_t nforms = 0;
        const char *fq[2] = { lessons[i], NULL };
        if (!kb_match_all(b->kb, "faculty_force_lesson", fq, 2,
                          &forms, &nforms)) continue;
        for (size_t j = 0; j < nforms && !force[0]; j++) {
            char *form = kb_dequote(forms[j]);
            if (!*form || !cue(norm, form)) continue;
            snprintf(lesson, sizeof lesson, "%s", form);
            snprintf(force, sizeof force, "%s", force_text);
        }
        free(forms);
    }
    free(lessons);
    if (!force[0]) return 0;

    kb_set_origin(b->kb, KB_SESSION);
    const char *args[2] = { faculty, force };
    kb_assert(b->kb, "faculty_force", args, 2);

    char msg[320];
    const KbResponseSlot slots[] = {
        { "faculty", faculty_name }, { "lesson", lesson }
    };
    if (!kb_term_say(b, "faculty_force_taught", slots, 2,
                     msg, sizeof msg)) return 0;
    put(msg, out, outsz);
    return 1;
}

int try_teach_form(Brain *b, const char *norm, const char *raw,
                           char *out, size_t outsz) {
    if (!b || !b->kb || !raw) return 0;
    if (try_teach_faculty_force(b, norm, out, outsz)) return 1;
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
        if (!strcmp(mode[0], "cue_for") || !strcmp(mode[0], "reply_for") ||
            !strcmp(mode[0], "frame_for") || !strcmp(mode[0], "role_for")) {
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
            if (!family[0] && strcmp(mode[0], "role_for")) continue;
            /* Una famiglia si insegna, non si inventa: deve gia' esistere,
             * altrimenti la lezione scriverebbe in un cassetto che nessuno
             * apre — il caso `greeting(ahoy)` di conoscenza morta. */
            /* E il controllo e' quello GIUSTO per la lezione: una cue ha senso
             * solo dove qualcuno legge cue, una risposta solo dove qualcuno
             * rende risposte. Attaccare una cue a una famiglia che nessuno
             * interroga per cue produrrebbe il fatto morto di `greeting(ahoy)`:
             * vero in KB, invisibile al comportamento. */
            /* gen457 — M15: LE FORME DELLA DOMANDA SI INSEGNANO.
             *
             * `answer_frame/2` lega una superficie interrogativa alla relazione
             * che deve interrogare, ed era l'unica conoscenza dichiarativa che
             * nessun atto didattico raggiungeva: quattro modi in `learnable/3`
             * e nessuno ci arrivava. Misurato: 270 formulazioni scritte a mano
             * per 136 relazioni, quasi tutte inglesi, al punto che
             *
             *   «which colors are used in chess»  rispondeva
             *   «which color  is  used in chess»  no
             *
             * e colmare quella differenza voleva dire aprire un file. E' il
             * limite che rendeva invisibili i risultati di ogni altro giro: si
             * possono insegnare fatti tutto il giorno, ma se non si insegna
             * COME SI CHIEDONO quei fatti non rispondono a nessuno.
             *
             * La guardia e' la stessa idea del M11 con il criterio giusto per
             * QUESTA lezione: una cue ha senso dove qualcuno legge cue, una
             * forma di domanda ha senso dove c'e' una relazione da interrogare.
             * Quindi la relazione deve gia' avere fatti, altrimenti la lezione
             * scriverebbe una porta davanti a una stanza vuota. */
            if (!strcmp(mode[0], "role_for")) {
                /* M1 assisted learning: teach a new input introducer by
                 * pointing at one that already works, without asking the
                 * teacher to know `segment_role`, its role atom, or a faculty
                 * name.  The model surface is interpreted by the SAME universal
                 * segmenter used at runtime; a live faculty_for/2 consumer is
                 * the proof that copying the role will not create dead KB. */
                char anchor[KB_TERM_LEN] = "";
                const char *ap = strstr(low, ls);
                if (ap) {
                    ap += strlen(ls);
                    while (*ap == ' ' || *ap == '\t') ap++;
                    if (*ap == '"') {
                        const char *ae = strchr(ap + 1, '"');
                        size_t al = ae ? (size_t)(ae - (ap + 1)) : 0;
                        if (al && al < sizeof anchor) {
                            memcpy(anchor, ap + 1, al);
                            anchor[al] = '\0';
                        }
                    }
                }

                char role[KB_TERM_LEN] = "";
                int model_ok = 0, model_ambiguous = 0;
                if (*anchor) {
                    InputSpan spans[16]; int ambiguous = 0;
                    size_t ns = input_segment(b->kb, anchor, spans, 16,
                                              &ambiguous);
                    if (!ambiguous) {
                        for (size_t si = 0; si < ns; si++) {
                            if (spans[si].cue_len == 0) continue;
                            char type[KB_TERM_LEN];
                            input_span_type(&spans[si], type, sizeof type);
                            char consumer[1][KB_TERM_LEN];
                            const char *cq[2] = { type, NULL };
                            if (kb_match(b->kb, "faculty_for", cq, 2,
                                         consumer, 1) != 1)
                                continue;
                            if (!role[0])
                                snprintf(role, sizeof role, "%s", spans[si].role);
                            else if (strcmp(role, spans[si].role) != 0)
                                model_ambiguous = 1;
                            model_ok = 1;
                        }
                    }
                }
                if (!model_ok || model_ambiguous) {
                    char msg[320];
                    kb_term_say(b, "no_segment_by_that_wording",
                                (const KbResponseSlot[]){ { "anchor", anchor } },
                                1, msg, sizeof msg);
                    put(msg, out, outsz);
                    return 1;
                }

                const char *n1 = strchr(norm, '"');
                const char *n2 = n1 ? strchr(n1 + 1, '"') : NULL;
                const char *f1 = n2 && n2 > n1 + 1 ? n1 : rq1;
                const char *f2 = n2 && n2 > n1 + 1 ? n2 : rq2;
                size_t fpl = (size_t)(f2 - (f1 + 1));
                if (fpl == 0 || fpl >= KB_TERM_LEN - 2) continue;
                char fphrase[KB_TERM_LEN], fq2[KB_TERM_LEN];
                memcpy(fphrase, f1 + 1, fpl); fphrase[fpl] = '\0';
                snprintf(fq2, sizeof fq2, "\"%s\"", fphrase);
                kb_set_origin(b->kb, KB_SESSION);
                const char *ra[2] = { role, fq2 };
                kb_assert(b->kb, "segment_role", ra, 2);

                char msg[320];
                kb_term_say(b, "teach_segment_like_ack",
                            (const KbResponseSlot[]){
                                { "phrase", fphrase }, { "anchor", anchor }
                            }, 2, msg, sizeof msg);
                put(msg, out, outsz);
                return 1;
            }
            if (!strcmp(mode[0], "frame_for")) {
                /* gen490 — M15, IL RESIDUO: LA LEZIONE NON DEVE NOMINARE LA
                 * RELAZIONE.
                 *
                 * Il gen457 ha aperto `answer_frame` all'insegnamento, ma solo
                 * nella forma «learn "…" as a way to ask side_color»: il
                 * teacher deve conoscere il NOME INTERNO della relazione. E'
                 * precisamente cio' che il vincolo zero di
                 * `docs/plans/apprendimento-assistito.md` vieta — «un esperto
                 * del dominio che ignora lo schema interno saprebbe formulare
                 * la lezione?». Con il nome del predicato la risposta e' no, e
                 * l'atto didattico resta fuori dal protocollo anche se
                 * funziona.
                 *
                 * La forma senza schema esiste e la sa dire chiunque: si
                 * ancora la formulazione nuova a una formulazione che parrot0
                 * gia' capisce.
                 *
                 *   learn "quali colori si usano in" as another way to ask
                 *         "which colors are used in"
                 *
                 * La relazione non viene nominata: viene DEDOTTA dalla domanda
                 * che il teacher ha usato come modello, insieme al verso. Il
                 * teacher non deve sapere ne' il predicato ne' quale argomento
                 * lega l'entita'; deve solo saper fare la stessa domanda in un
                 * modo che gia' funziona — e per definizione lo sa, perche' e'
                 * quella la domanda a cui vuole poter rispondere altrimenti.
                 *
                 * La guardia e' la stessa del gen457 spostata di un livello:
                 * il modello deve essere una domanda che parrot0 SA gia'
                 * rispondere, altrimenti non c'e' niente da cui copiare e la
                 * lezione aprirebbe una porta davanti a una stanza vuota. */
                char anchor[KB_TERM_LEN] = "";
                {
                    const char *ap = strstr(low, ls);
                    if (ap) {
                        ap += strlen(ls);
                        while (*ap == ' ' || *ap == '\t') ap++;
                        if (*ap == '"') {
                            const char *ae = strchr(ap + 1, '"');
                            size_t al = ae ? (size_t)(ae - (ap + 1)) : 0;
                            if (al && al < sizeof anchor) {
                                memcpy(anchor, ap + 1, al);
                                anchor[al] = '\0';
                            }
                        }
                    }
                }
                if (*anchor) {
                    /* Le virgolette proteggono il testo interno mentre viene
                     * canonicalizzato l'atto didattico.  Quel testo, pero', al
                     * replay sara' un turno normale e attraversera' il
                     * canonizzatore: usare qui la forma protetta puo' quindi
                     * creare una chiave che il lettore non produrra' mai
                     * (`come` -> `how` e' il controesempio misurato da UC1).
                     * Modello e replay devono condividere la stessa funzione. */
                    char anchor_canon[KB_TERM_LEN];
                    brain_canonical(b, anchor, anchor_canon,
                                    sizeof anchor_canon);
                    const char *anchor_query = *anchor_canon
                                             ? anchor_canon : anchor;
                    /* Il modello si legge come si legge un TURNO — e con lo
                     * STESSO motore: `answer_frame_surfaces` e' la risoluzione
                     * che `mod_answer_frame` gia' usava per capire quale
                     * domanda un turno contiene. Riscriverla qui sarebbe stata
                     * la duplicazione del mantra #5: la superficie piu'
                     * specifica vince, e chi insegna scrive la domanda intera
                     * che gia' gli funziona, non la cue con cui la KB la
                     * indicizza (saperla sarebbe di nuovo il vincolo zero). */
                    /* La guardia e' CONCLUSIVA, non permissiva, e questa non
                     * e' una duplicazione: sono due domande diverse. Il
                     * consumer chiede «quali candidate potrebbero valere» e
                     * lascia decidere l'evidenza; chi insegna deve sapere se il
                     * modello e' una domanda a cui parrot0 RISPONDE davvero —
                     * altrimenti eredita una relazione presa per sbaglio da una
                     * cue-sottostringa (mantra #8) e scrive un frame falso, che
                     * e' peggio di un muro. Misurato: senza questa prova,
                     * «what is the flurb of france» veniva accettato come
                     * modello. La prova la fa il motore vero, non una seconda
                     * regola di combaciamento. */
                    char probe_ans[512];
                    if (!mod_answer_frame(b, anchor_query, anchor_query,
                                          probe_ans, sizeof probe_ans)) {
                        char msg[256];
                        kb_term_say(b, "no_question_by_that_wording",
                                    (const KbResponseSlot[]){ { "anchor", anchor } },
                                    1, msg, sizeof msg);
                        put(msg, out, outsz);
                        return 1;
                    }
                    char (*hits)[KB_TERM_LEN] = NULL;
                    size_t nh = answer_frame_surfaces(b, anchor_query, &hits);
                    char rel[1][KB_TERM_LEN];
                    const char *arq[2] = { nh ? hits[0] : NULL, NULL };
                    int found = nh &&
                        kb_match(b->kb, "answer_frame", arq, 2, rel, 1) == 1;
                    free(hits);
                    if (!found) {
                        char msg[256];
                        kb_term_say(b, "no_question_by_that_wording",
                                    (const KbResponseSlot[]){ { "anchor", anchor } },
                                    1, msg, sizeof msg);
                        put(msg, out, outsz);
                        return 1;
                    }
                    snprintf(family, sizeof family, "%s", rel[0]);
                }
                if (!kb_knows_pred(b->kb, family)) {
                    char msg[256];
                    kb_term_say(b, "no_relation_by_that_name", (const KbResponseSlot[]){
                                    { "family", family } }, 1, msg, sizeof msg);
                    put(msg, out, outsz);
                    return 1;
                }
                /* La superficie insegnata viene estratta dalle virgolette, che
                 * giustamente l'hanno protetta durante la lettura dell'atto.
                 * Ora la trasformiamo come un turno autonomo: e' quella la
                 * rappresentazione che il riconoscitore vedra' al replay. */
                const char *n1 = strchr(norm, '"'), *n2 = n1 ? strchr(n1 + 1, '"') : NULL;
                const char *f1 = n2 && n2 > n1 + 1 ? n1 : rq1;
                const char *f2 = n2 && n2 > n1 + 1 ? n2 : rq2;
                size_t fpl = (size_t)(f2 - (f1 + 1));
                if (fpl == 0 || fpl >= KB_TERM_LEN - 2) continue;
                char fphrase[KB_TERM_LEN];
                memcpy(fphrase, f1 + 1, fpl); fphrase[fpl] = '\0';
                char stored_phrase[KB_TERM_LEN];
                brain_canonical(b, fphrase, stored_phrase,
                                sizeof stored_phrase);
                if (!*stored_phrase) continue;
                char fq2[KB_TERM_LEN];
                snprintf(fq2, sizeof fq2, "\"%s\"", stored_phrase);
                kb_set_origin(b->kb, KB_SESSION);
                const char *fa[2] = { fq2, family };
                kb_assert(b->kb, "answer_frame", fa, 2);
                /* La DIREZIONE si eredita da una formulazione gia' esistente
                 * della stessa relazione: chi insegna una parafrasi non deve
                 * sapere quale argomento lega l'entita' della domanda. Se
                 * nessuna la dichiara, resta il comportamento permissivo
                 * storico e non si inventa un vincolo. */
                {
                    char anyc[8][KB_TERM_LEN];
                    const char *acq[2] = { NULL, family };
                    size_t nac = kb_match(b->kb, "answer_frame", acq, 2, anyc, 8);
                    for (size_t ai = 0; ai < nac; ai++) {
                        char slot[4][KB_TERM_LEN];
                        const char *siq[3] = { anyc[ai], family, NULL };
                        size_t ns = kb_match(b->kb, "answer_frame_input_arg",
                                             siq, 3, slot, 4);
                        if (ns == 0) continue;
                        for (size_t si = 0; si < ns; si++) {
                            const char *ia[3] = { fq2, family, slot[si] };
                            kb_assert(b->kb, "answer_frame_input_arg", ia, 3);
                        }
                        break;
                    }
                }
                char msg[256];
                /* Chi ha insegnato ancorando a una domanda non ha nominato la
                 * relazione: la conferma non gliela mostra. E' il rovescio del
                 * vincolo zero — chi insegna senza leggere lo schema deve poter
                 * leggere anche la verifica senza schema (M20). */
                if (*anchor)
                    kb_term_say(b, "teach_frame_like_ack", (const KbResponseSlot[]){
                                    { "phrase", fphrase }, { "anchor", anchor } },
                                2, msg, sizeof msg);
                else
                    kb_term_say(b, "teach_family_ack", (const KbResponseSlot[]){
                                    { "phrase", fphrase }, { "label", ls },
                                    { "family", family } }, 3, msg, sizeof msg);
                put(msg, out, outsz);
                return 1;
            }
            char row[1][KB_TERM_LEN];
            const char *wanted = !strcmp(mode[0], "cue_for") ? "intent_cue"
                                                             : "response_template";
            const char *famq[2] = { family, NULL };
            if (kb_match(b->kb, wanted, famq, 2, row, 1) == 0) {
                char msg[256];
                const char *other = !strcmp(mode[0], "cue_for") ? "response_template"
                                                                : "intent_cue";
                if (kb_match(b->kb, other, famq, 2, row, 1) > 0)
                    {   const KbResponseSlot _rs[] = { { "family", family }, { "wanted", wanted } };
                      kb_term_say(b, "i_know_x_but_nothing_reads_its_x_so_that_les", _rs, 2, msg, sizeof msg); }
                else
                    {   const KbResponseSlot _rs[] = { { "family", family } };
                      kb_term_say(b, "i_don_t_have_a_family_called_x_so_i_can_t_at", _rs, 1, msg, sizeof msg); }
                put(msg, out, outsz);
                return 1;
            }
            generic = 1;
        }
        if      (generic && !strcmp(mode[0], "cue_for"))
                                                { pred = "intent_cue";        from_raw = 0; }
        else if (generic)                       { pred = "response_template"; from_raw = 1; }
        else if (lex_class_member(b, "00_lex_lex587", mode[0]))     { pred = "intent_phrase";     from_raw = 0; }
        else if (lex_class_member(b, "00_lex_lex588", mode[0])) { pred = "intent_cue";        from_raw = 0; }
        else if (lex_class_member(b, "00_lex_lex589", mode[0]))      { pred = "response_template"; from_raw = 1; }
        else if (lex_class_member(b, "00_lex_lex590", mode[0]))     { pred = intent[0];           from_raw = 0; unary = 1; }
        else if (lex_class_member(b, "00_lex_lex591", mode[0]))    { pred = intent[0];           from_raw = 1; define = 1; }
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
            { 
              char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", pl);
              char _v1[48]; snprintf(_v1, sizeof _v1, "%d", KB_TERM_LEN - 3);
  const KbResponseSlot _rs[] = { { "pl", _v0 }, { "KB_TERM_LEN", _v1 } };
              kb_term_say(b, "that_quoted_span_is_too_long_for_one_fact_x", _rs, 2, lim, sizeof lim); }
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
            if (!lex_class_member(b, "00_lex_lex660", clang)) {
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
        /* gen456: il ramo generico componeva `msg` e NON lo scriveva mai in
         * `out` — chi insegnava si vedeva restituire la risposta del turno
         * precedente. La lezione andava a segno, ma era invisibile: il modo
         * peggiore di fallire, perche' sembra che non abbia funzionato niente.
         * Ed era anche l'ultima frase umanizzata rimasta in questa funzione. */
        char msg[256];
        if (generic)
            kb_term_say(b, "teach_family_ack", (const KbResponseSlot[]){
                            { "phrase", phrase }, { "label", ls },
                            { "family", family } }, 3, msg, sizeof msg);
        else
            kb_term_say(b, "teach_form_ack", (const KbResponseSlot[]){
                            { "phrase", phrase }, { "ls", ls } },
                        2, msg, sizeof msg);
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
            if (domain_query(b, "content", kq, 1)) {
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
                if (*sing && domain_query(b, "content", sq, 1)) {
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
