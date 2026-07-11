static int fr_lookup(Brain *b, const char *en, char *out, size_t out_sz) {
    const char *q[] = { en, NULL };
    char hit[1][KB_TERM_LEN];
    if (!b || !b->kb || kb_match(b->kb, "tr_fr", q, 2, hit, 1) != 1) return 0;
    snprintf(out, out_sz, "%s", kb_dequote(hit[0]));
    return 1;
}

static int fr_gender_for_en(Brain *b, const char *en, char *gender) {
    char fr[KB_TERM_LEN];
    if (!fr_lookup(b, en, fr, sizeof fr)) return 0;
    const char *q[] = { fr, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "gender_fr", q, 2, hit, 1) != 1) {
        char qfr[KB_TERM_LEN + 2];
        snprintf(qfr, sizeof qfr, "\"%s\"", fr);
        const char *qq[] = { qfr, NULL };
        if (kb_match(b->kb, "gender_fr", qq, 2, hit, 1) != 1) return 0;
    }
    *gender = hit[0][0];
    return 1;
}

/* U5 (gen289): the English present progressive "is V-ing" -> drop the auxiliary.
 * Migrated from a hardcoded strcmp(next,"sleeping") to knowledge: the auxiliary
 * is a fact (aux_progressive/1) and the progressive is a morphology RULE (the
 * "-ing" suffix over chars/2) in grammar.p0 — so it generalizes to any -ing verb.
 * Backstop to the historical hardcoded case if grammar.p0 is absent. */
static int is_progressive_aux(Brain *b, const char *aux, const char *verb) {
    if (b && b->kb && kb_knows_pred(b->kb, "aux_progressive")) {
        const char *aq[] = { aux };
        const char *pq[] = { verb };
        return kb_query(b->kb, "aux_progressive", aq, 1) &&
               kb_query(b->kb, "progressive", pq, 1);
    }
    return strcmp(aux, "is") == 0 && strcmp(verb, "sleeping") == 0;   /* backstop */
}

/* gen310: extract the phrase to translate from any of the request forms —
 * "translate ... : X", 'translate ... "X"', or "how do you say X in <lang>" —
 * so a fixed multi-word phrase can be a translation UNIT. Priority: a quoted span
 * (double or single) -> the span between "say " and " in " -> after a colon ->
 * after the language word. Fills buf (trimmed of edge quotes/space/punct). */
static int tr_payload(const char *low, const char *langword,
                      char *buf, size_t bufsz) {
    const char *start = NULL, *end = NULL;
    for (int qc = 0; qc < 2 && !start; qc++) {
        char qch = qc ? '\'' : '"';
        const char *q1 = strchr(low, qch);
        if (q1) { const char *q2 = strchr(q1 + 1, qch);
                  if (q2 && q2 > q1 + 1) { start = q1 + 1; end = q2; } }
    }
    if (!start) {
        const char *say = strstr(low, "say ");
        if (say) { const char *in = strstr(say + 4, " in ");
                   if (in && in > say + 4) { start = say + 4; end = in; } }
    }
    if (!start) { const char *c = strchr(low, ':'); if (c) start = c + 1; }
    if (!start) { const char *lw = strstr(low, langword); if (lw) start = lw + strlen(langword); }
    if (!start) return 0;
    while (*start && (isspace((unsigned char)*start) || *start == '"' || *start == '\'')) start++;
    if (!end || end < start) end = start + strlen(start);
    size_t n = (size_t)(end - start);
    if (n >= bufsz) n = bufsz - 1;
    memcpy(buf, start, n); buf[n] = '\0';
    size_t l = strlen(buf);
    while (l > 0 && (buf[l-1] == '.' || buf[l-1] == '?' || buf[l-1] == '!' ||
                     buf[l-1] == '"' || buf[l-1] == '\'' || isspace((unsigned char)buf[l-1])))
        buf[--l] = '\0';
    return l > 0;
}

/* gen310: longest-match a fixed phrase starting at token `start`. Tries spans from
 * the longest down to 2 words against `phrase_pred`(key, gloss); on a hit fills
 * gloss and returns the token count consumed, else 0. This is the multi-word UNIT
 * of the translation plan: "thank you"/"nice to meet you" translate as one chunk
 * (idiomatic, non-compositional) before the word-by-word + grammar fallback. */
static size_t tr_phrase_chunk(Brain *b, const char *phrase_pred, char **tok,
                              size_t ntok, size_t start, char *gloss, size_t gsz) {
    if (!b || !b->kb || start >= ntok) return 0;
    for (size_t len = ntok - start; len >= 2; len--) {
        char key[256]; size_t ko = 0; int overflow = 0;
        for (size_t j = 0; j < len; j++) {
            char *t = strip_edge_punct(tok[start + j]);
            int w = snprintf(key + ko, sizeof key - ko, "%s%s", j ? " " : "", t);
            if (w < 0 || (size_t)w >= sizeof key - ko) { overflow = 1; break; }
            ko += (size_t)w;
        }
        if (overflow || ko == 0) continue;
        char g[1][KB_TERM_LEN];
        /* gen310: try unquoted key first (matches autolearn-taught facts stored
         * via kb.assert), then quoted (matches .p0-loaded facts which keep quotes
         * from parse_term). */
        static const char *tq[2][2] = {{NULL, NULL}, {NULL, NULL}};
        char qraw[256];
        char qquoted[260];
        snprintf(qraw, sizeof qraw, "%s", key);
        snprintf(qquoted, sizeof qquoted, "\"%s\"", key);
        tq[0][0] = qraw; tq[1][0] = qquoted;
        int found = 0;
        for (int qi = 0; qi < 2 && !found; qi++) {
            if (kb_match(b->kb, phrase_pred, tq[qi], 2, g, 1) == 1) {
                snprintf(gloss, gsz, "%s", kb_dequote(g[0]));
                found = 1;
            }
        }
        if (found) return len;
    }
    return 0;
}

/* gen311 (teachable-procedures.md P0): a generic token-REWRITE interpreter. A
 * grammatical PROCEDURE is a TAUGHT fact rewrite_<lang>(LHS, RHS) whose two args are
 * space-joined token patterns with single-token variables ("$s"). If LHS unifies
 * with the current token list (literals equal, $vars bind), the list is replaced by
 * RHS with the vars substituted. So interrogative fronting/de-inversion is a taught
 * RULE, not C — a new construct is a new rule, no recompile. (Single-token vars for
 * P0; span vars are P1, real computation P2.) `scratch` owns the rewritten tokens.
 * Returns 1 if a rule fired. Kept alongside the older C pre-passes (keep-and-select). */
static int apply_token_rewrite(Brain *b, const char *pred, char **sw, size_t *snp,
                               char scratch[][KB_TERM_LEN], size_t scap) {
    if (!b || !b->kb) return 0;
    char lhss[64][KB_TERM_LEN];
    const char *anyq[] = { NULL, NULL };
    size_t nl = kb_match(b->kb, pred, anyq, 2, lhss, 64);
    for (size_t r = 0; r < nl; r++) {
        char lbuf[256]; snprintf(lbuf, sizeof lbuf, "%s", kb_dequote(lhss[r]));
        char *lp[32]; size_t ln = split_words(lbuf, lp, 32);
        if (ln == 0 || ln != *snp) continue;                 /* P0: equal length */
        char vn[16][KB_TERM_LEN], vv[16][KB_TERM_LEN]; size_t nv = 0; int ok = 1;
        for (size_t i = 0; i < ln && ok; i++) {
            char *pt = strip_edge_punct(lp[i]);
            char *st = strip_edge_punct(sw[i]);
            if (pt[0] == '$') {
                if (nv < 16) { snprintf(vn[nv], KB_TERM_LEN, "%s", pt);
                               snprintf(vv[nv], KB_TERM_LEN, "%s", st); nv++; }
            } else if (strcmp(pt, st) != 0) ok = 0;
        }
        if (!ok) continue;
        const char *rq[] = { lhss[r], NULL };
        char rhss[1][KB_TERM_LEN];
        if (kb_match(b->kb, pred, rq, 2, rhss, 1) != 1) continue;
        char rbuf[256]; snprintf(rbuf, sizeof rbuf, "%s", kb_dequote(rhss[0]));
        char *rp[32]; size_t rn = split_words(rbuf, rp, 32);
        if (rn == 0 || rn > scap) continue;
        for (size_t i = 0; i < rn; i++) {
            char *ot = strip_edge_punct(rp[i]);
            const char *val = ot;
            if (ot[0] == '$')
                for (size_t v = 0; v < nv; v++)
                    if (!strcmp(vn[v], ot)) { val = vv[v]; break; }
            snprintf(scratch[i], KB_TERM_LEN, "%s", val);
            sw[i] = scratch[i];
        }
        *snp = rn;
        return 1;
    }
    return 0;
}

static int mod_translate(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    /* A translator must read the ORIGINAL source words, not the canonicalized
     * surface (which maps Italian function words to English and would wreck an
     * IT->EN request). So we work off `raw`, lowercased here. */
    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* gen252: minimal compositional EN->FR. The lexicon is tr_fr/2 + gender_fr/2;
     * C supplies only grammar glue: article agreement, "is sleeping" -> finite
     * verb, and English pre-noun adjective -> French post-noun adjective. */
    if (strstr(low, "french")) {
        char fbuf[256];
        if (tr_payload(low, "french", fbuf, sizeof fbuf)) {
            /* gen310: a fixed multi-word phrase translates as ONE idiomatic unit
             * (non-compositional): try the whole payload as a phrase first. */
            char pg[1][KB_TERM_LEN];
            char qraw_fr[256]; snprintf(qraw_fr, sizeof qraw_fr, "%s", fbuf);
            char qquoted_fr[260]; snprintf(qquoted_fr, sizeof qquoted_fr, "\"%s\"", fbuf);
            const char *pqb[2][2] = {{qraw_fr, NULL}, {qquoted_fr, NULL}};
            for (int qit = 0; qit < 2; qit++) {
                if (kb_match(b->kb, "tr_fr_phrase", pqb[qit], 2, pg, 1) == 1) {
                    char sent[256]; snprintf(sent, sizeof sent, "%s", kb_dequote(pg[0]));
                    if (sent[0]) sent[0] = (char)toupper((unsigned char)sent[0]);
                    size_t sl = strlen(sent);
                    if (sl + 1 < sizeof sent) { sent[sl] = '.'; sent[sl + 1] = '\0'; }
                    put(sent, out, out_size);
                    return 1;
                }
            }
            char *fw[32]; size_t fn = split_words(fbuf, fw, 32);
            char pieces[32][KB_TERM_LEN]; size_t np = 0;  /* gen307: collect, then reorder clitics */
            char cur_subj[KB_TERM_LEN] = "";  /* gen311: last subject pronoun, for conj_fr */
            for (size_t i = 0; i < fn; i++) {
                char *tok = strip_edge_punct(fw[i]);
                if (!*tok) continue;
                char piece[KB_TERM_LEN] = "";

                /* gen311 (KB-first morphology): a word used as the SUBJECT arg of
                 * some conj_fr fact is a subject pronoun — remember it so a later
                 * verb conjugates for it. Fully data-driven: no pronoun list. */
                {
                    const char *subq[] = { NULL, tok, NULL };
                    char st[1][KB_TERM_LEN];
                    /* Only the FIRST subject-capable pronoun is the sentence subject;
                     * a later "you" ("I love you") is an object, left to the clitic
                     * path — so guard on cur_subj still being empty. */
                    if (cur_subj[0] == '\0' &&
                        kb_match(b->kb, "conj_fr", subq, 3, st, 1) >= 1) {
                        snprintf(cur_subj, sizeof cur_subj, "%s", tok);
                        /* gen311: French keeps the subject, but "you" as a SUBJECT is
                         * "vous", not the object clitic "te" (tr_fr(you, te)). Emit the
                         * taught subject form so it isn't mangled by the clitic path. */
                        const char *spq[] = { tok, NULL };
                        char sf[1][KB_TERM_LEN];
                        if (kb_match(b->kb, "subject_pron_fr", spq, 2, sf, 1) == 1 && np < 32) {
                            snprintf(pieces[np++], KB_TERM_LEN, "%s", sf[0]);
                            continue;
                        }
                    }
                }

                /* gen311: drop a QUESTION auxiliary ("do you speak" -> "parlez-vous",
                 * here the verb conjugates and the aux is dropped). aux_question/1 fact. */
                {
                    const char *qaq[] = { tok };
                    if (kb_query(b->kb, "aux_question", qaq, 1)) continue;
                }

                if (!strcmp(tok, "the")) {
                    char gender = 'm';
                    if (i + 1 < fn) {
                        char *n1 = strip_edge_punct(fw[i + 1]);
                        if (!fr_gender_for_en(b, n1, &gender) && i + 2 < fn)
                            (void)fr_gender_for_en(b, strip_edge_punct(fw[i + 2]), &gender);
                    }
                    /* U5 (gen288): the French definite article is a KB fact,
                     * article_fr(Gender, Form) in grammar.p0, not a C ternary. */
                    const char *aq[] = { gender == 'f' ? "f" : "m", NULL };
                    char af[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "article_fr", aq, 2, af, 1) == 1)
                        snprintf(piece, sizeof piece, "%s", af[0]);
                    else
                        snprintf(piece, sizeof piece, "%s", gender == 'f' ? "la" : "le");
                } else if (i + 1 < fn &&
                           is_progressive_aux(b, tok, strip_edge_punct(fw[i + 1]))) {
                    continue;   /* drop the progressive auxiliary; the -ing verb glosses finite */
                } else {
                    /* gen311: conjugate a verb for the tracked subject via
                     * conj_fr(Verb, Subject, Form); fall back to the plain gloss. */
                    if (*cur_subj && strcmp(tok, cur_subj) != 0) {
                        const char *cvq[] = { tok, cur_subj, NULL };
                        char cf[1][KB_TERM_LEN];
                        if (kb_match(b->kb, "conj_fr", cvq, 3, cf, 1) == 1)
                            snprintf(piece, sizeof piece, "%s", cf[0]);
                    }
                    char gnext;
                    if (!piece[0] && i + 1 < fn &&
                        fr_gender_for_en(b, strip_edge_punct(fw[i + 1]), &gnext) &&
                        !fr_gender_for_en(b, tok, &gnext)) {
                        char adj[KB_TERM_LEN];
                        if (fr_lookup(b, tok, adj, sizeof adj)) continue; /* render after noun */
                    }
                    if (!piece[0] && !fr_lookup(b, tok, piece, sizeof piece)) {
                        char msg[160];
                        snprintf(msg, sizeof msg,
                                 "I can translate most of it, but I don't know the French for \"%s\".",
                                 tok);
                        put(msg, out, out_size);
                        return 1;
                    }
                    char gnoun;
                    if (fr_gender_for_en(b, tok, &gnoun) && i > 0) {
                        char *prev = strip_edge_punct(fw[i - 1]);
                        char adj[KB_TERM_LEN], gdummy;
                        if (strcmp(prev, "the") != 0 &&
                            !fr_gender_for_en(b, prev, &gdummy) &&
                            fr_lookup(b, prev, adj, sizeof adj)) {
                            size_t pl = strlen(piece), al = strlen(adj);
                            if (pl + al + 2 < sizeof piece) {
                                piece[pl++] = ' ';
                                memcpy(piece + pl, adj, al + 1);
                            }
                        }
                    }
                }

                if (piece[0] && np < 32)
                    snprintf(pieces[np++], KB_TERM_LEN, "%s", piece);
            }
            /* gen307: French object-clitic placement. A clitic pronoun the
             * word-by-word gloss left at the END of the clause moves before its
             * verb, eliding pre-vowel (te+aime -> t'aime). Which words are
             * clitics and how they elide is grammar.p0 (clitic_obj_fr, elide_fr,
             * clitic_join); C only detects the sentence-final clitic and swaps.
             * Scoped to a TRAILING clitic so the article le/la (same surface) in
             * mid-sentence is never disturbed. clitic_join answers the elision
             * case; the non-eliding case falls back to a plain space join. */
            if (np >= 2) {
                char cl[16][KB_TERM_LEN];
                const char *cq[] = { NULL };   /* enumerate: a boolean query returns no count */
                size_t ncl = kb_match(b->kb, "clitic_obj_fr", cq, 1, cl, 16);
                int is_clitic = 0;
                for (size_t c = 0; c < ncl; c++)
                    if (!strcmp(cl[c], pieces[np - 1])) { is_clitic = 1; break; }
                if (is_clitic) {
                    const char *jq[] = { pieces[np - 1], pieces[np - 2], NULL };
                    char jr[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "clitic_join", jq, 3, jr, 1) == 1)
                        snprintf(pieces[np - 2], KB_TERM_LEN, "%s", jr[0]);       /* t'aime */
                    else {
                        char sp[KB_TERM_LEN];
                        snprintf(sp, sizeof sp, "%s %s", pieces[np - 1], pieces[np - 2]);
                        snprintf(pieces[np - 2], KB_TERM_LEN, "%s", sp);          /* te parle */
                    }
                    np--;
                }
            }
            /* gen311: French subject/determiner elision at each junction
             * ("je" + "apprends" -> "j'apprends"). elide_join glues the pair with
             * no space; chained by staying at i. KB-driven (elide_fr + vowel rule). */
            for (size_t i = 0; i + 1 < np; ) {
                const char *ejq[] = { pieces[i], pieces[i + 1], NULL };
                char ejr[1][KB_TERM_LEN];
                if (kb_match(b->kb, "elide_join", ejq, 3, ejr, 1) == 1) {
                    snprintf(pieces[i], KB_TERM_LEN, "%s", ejr[0]);
                    for (size_t k = i + 1; k + 1 < np; k++)
                        snprintf(pieces[k], KB_TERM_LEN, "%s", pieces[k + 1]);
                    np--;
                } else i++;
            }
            char result[512] = ""; size_t ro = 0;
            for (size_t i = 0; i < np; i++) {
                size_t pl = strlen(pieces[i]);
                if (ro && ro + 1 < sizeof result) result[ro++] = ' ';
                if (ro + pl + 1 < sizeof result) { memcpy(result + ro, pieces[i], pl + 1); ro += pl; }
            }
            if (ro) {
                if (result[0]) result[0] = (char)toupper((unsigned char)result[0]);
                if (ro + 1 < sizeof result) { result[ro++] = '.'; result[ro] = '\0'; }
                put(result, out, out_size);
                return 1;
            }
        }
    }

    /* gen236 (LLMSCORE): minimal EN->ES word-by-word translation for short
     * benchmark prompts. Words and gender live in KB as tr_es/2 + gender_es/2. */
    if (strstr(low, "spanish")) {
        char sbuf[256];
        if (tr_payload(low, "spanish", sbuf, sizeof sbuf)) {
            char *sw[32]; size_t sn = split_words(sbuf, sw, 32);
            /* gen311 (teachable-procedures P0): FIRST try a TAUGHT rewrite rule
             * rewrite_es(LHS, RHS) — the interrogative restructuring as DATA, not C
             * ("where are $s from" -> "from where $s are" -> de dónde eres). If no
             * rule matches, fall back to the older hardcoded pre-passes below
             * (keep-and-select), which do the same two moves in C. */
            char rw_scratch[32][KB_TERM_LEN];
            if (!apply_token_rewrite(b, "rewrite_es", sw, &sn, rw_scratch, 32)) {
                /* (a) FRONT a stranded preposition wh_front_es(prep, _); (b) DE-INVERT
                 * an inverted verb+subject conj_es(Verb, Subj, _). Net: "de dónde eres". */
                for (size_t j = 0; j < sn; j++) {
                    char *t = strip_edge_punct(sw[j]);
                    const char *wq[] = { t, NULL };
                    char wf[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "wh_front_es", wq, 2, wf, 1) == 1) {
                        char *moved = sw[j];
                        for (size_t k = j; k > 0; k--) sw[k] = sw[k - 1];
                        sw[0] = moved;
                        break;
                    }
                }
                for (size_t j = 0; j + 1 < sn; j++) {
                    char *vb = strip_edge_punct(sw[j]);
                    char *sj = strip_edge_punct(sw[j + 1]);
                    const char *cq[] = { vb, sj, NULL };
                    char cf[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "conj_es", cq, 3, cf, 1) == 1) {  /* verb before its subject */
                        char *tmp = sw[j]; sw[j] = sw[j + 1]; sw[j + 1] = tmp;
                        j++;
                    }
                }
            }
            char result[512] = ""; size_t ro = 0;
            char cur_subj_es[KB_TERM_LEN] = "";  /* gen311: tracked subject, for conj_es */
            for (size_t i = 0; i < sn; ) {
                char piece[KB_TERM_LEN] = "";
                /* gen310: longest-match a fixed multi-word phrase FIRST (the
                 * idiomatic unit: "nice to meet you" -> "mucho gusto"); fall back
                 * to word-by-word + article grammar for the rest. */
                char gloss[KB_TERM_LEN];
                size_t used = tr_phrase_chunk(b, "tr_es_phrase", sw, sn, i, gloss, sizeof gloss);
                if (used) { snprintf(piece, sizeof piece, "%s", gloss); i += used; }
                else {
                    char *tok = strip_edge_punct(sw[i]);
                    if (!*tok) { i++; continue; }
                    /* gen311 (F., KB-first): drop a QUESTION auxiliary. "do you
                     * speak English?" -> the "do" is dropped and the verb conjugates
                     * for the subject: "¿hablas inglés?". aux_question(word) is a
                     * fact — the same shape as the progressive-aux drop. */
                    {
                        const char *qaq[] = { tok };
                        if (kb_query(b->kb, "aux_question", qaq, 1)) { i++; continue; }
                    }
                    /* gen311 (F., KB-first morphology): subject-tracking conjugation.
                     * A word used as the SUBJECT arg of some conj_es is a subject
                     * pronoun (no pronoun list hardcoded); Spanish drops it when
                     * pro_drop(es) holds. A negation marker negation_es(word, form)
                     * glosses to "no". A later verb conjugates for the tracked subject
                     * via conj_es(Verb, Subject, Form). So "I don't understand" ->
                     * (drop I) + no + entiendo = "No entiendo", and "I need help" ->
                     * "necesito ayuda", all from taught facts, no C conjugator. */
                    {
                        const char *subq[] = { NULL, tok, NULL };
                        char st[1][KB_TERM_LEN];
                        /* first subject-capable pronoun only (a later "you" is an object). */
                        if (cur_subj_es[0] == '\0' &&
                            kb_match(b->kb, "conj_es", subq, 3, st, 1) >= 1) {
                            snprintf(cur_subj_es, sizeof cur_subj_es, "%s", tok);
                            const char *dq[] = { "es", NULL };
                            if (kb_query(b->kb, "pro_drop", dq, 1)) { i++; continue; } /* drop subject */
                            char es[1][KB_TERM_LEN];
                            const char *sq[] = { tok, NULL };
                            if (kb_match(b->kb, "tr_es", sq, 2, es, 1) == 1)
                                snprintf(piece, sizeof piece, "%s", es[0]);
                        }
                    }
                    if (!piece[0]) {
                        const char *ngq[] = { tok, NULL };
                        char nf[1][KB_TERM_LEN];
                        if (kb_match(b->kb, "negation_es", ngq, 2, nf, 1) == 1)
                            snprintf(piece, sizeof piece, "%s", nf[0]);
                    }
                    if (!piece[0] && *cur_subj_es && strcmp(tok, cur_subj_es) != 0) {
                        const char *cvq[] = { tok, cur_subj_es, NULL };
                        char cf[1][KB_TERM_LEN];
                        if (kb_match(b->kb, "conj_es", cvq, 3, cf, 1) == 1)
                            snprintf(piece, sizeof piece, "%s", cf[0]);
                    }
                    if (!piece[0] && strcmp(tok, "the") == 0 && i + 1 < sn) {
                        char *nx = strip_edge_punct(sw[i + 1]);
                        char esn[1][KB_TERM_LEN];
                        const char *nq[] = { nx, NULL };
                        if (kb_match(b->kb, "tr_es", nq, 2, esn, 1) == 1) {
                            char gen[1][KB_TERM_LEN];
                            const char *gq[] = { esn[0], NULL };
                            int fem_es = (kb_match(b->kb, "gender_es", gq, 2, gen, 1) == 1 &&
                                          strcmp(gen[0], "f") == 0);
                            /* U5 (gen288): the Spanish definite article is a KB fact,
                             * article_es(Gender, Form) in grammar.p0, not a C ternary. */
                            const char *aq[] = { fem_es ? "f" : "m", NULL };
                            char aes[1][KB_TERM_LEN];
                            if (kb_match(b->kb, "article_es", aq, 2, aes, 1) == 1)
                                snprintf(piece, sizeof piece, "%s", aes[0]);
                            else
                                snprintf(piece, sizeof piece, "%s", fem_es ? "la" : "el");
                        }
                    }
                    if (!piece[0]) {
                        char es[1][KB_TERM_LEN];
                        const char *q[] = { tok, NULL };
                        if (kb_match(b->kb, "tr_es", q, 2, es, 1) != 1) {
                            /* gen240: honest decline that NAMES the gap (like the IT path)
                             * and claims the turn, so the request isn't met with a worse
                             * generic deflection downstream. */
                            char msg[160];
                            snprintf(msg, sizeof msg,
                                     "I can translate most of it, but I don't know the "
                                     "Spanish for \"%s\".", tok);
                            put(msg, out, out_size);
                            return 1;
                        }
                        snprintf(piece, sizeof piece, "%s", es[0]);
                    }
                    i++;
                }
                if (!piece[0]) continue;
                size_t pl = strlen(piece);
                if (ro && ro + 1 < sizeof result) result[ro++] = ' ';
                if (ro + pl + 1 < sizeof result) {
                    memcpy(result + ro, piece, pl + 1);
                    ro += pl;
                }
            }
            if (ro) {
                /* gen239 (kb-first): cosmetic sentence glue. The lexicon is
                 * KB knowledge (tr_es/2); only the SENTENCE formatting
                 * (initial capital + terminating period) is added here, the
                 * same way mod_gen caps "Suddenly, ….". Pure surface glue,
                 * never knowledge. */
                char sent[512];
                snprintf(sent, sizeof sent, "%s", result);
                size_t sl = strlen(sent);
                if (sl > 0) {
                    if (sl + 1 < sizeof sent) { sent[sl] = '.'; sent[sl + 1] = '\0'; }
                    if (sent[0]) sent[0] = (char)toupper((unsigned char)sent[0]);
                }
                put(sent, out, out_size);
                return 1;
            }
        }
    }

    int to_it;
    const char *clause = NULL;
    static const struct { const char *p; int to_it; } pats[] = {
        {"translate to italian:", 1}, {"translate into italian:", 1},
        {"translate to english:", 0}, {"translate into english:", 0},
        {"traduci in italiano:", 1},  {"traduci in inglese:", 0},
    };
    for (size_t i = 0; i < sizeof pats / sizeof pats[0]; i++) {
        size_t L = strlen(pats[i].p);
        if (strncmp(low, pats[i].p, L) == 0) {
            to_it = pats[i].to_it;
            clause = low + L;
            goto found;
        }
    }
    return 0;
found:
    while (*clause && isspace((unsigned char)*clause)) clause++;
    if (!*clause) { put("Translate what?", out, out_size); return 1; }

    char buf[256];
    copy_trim(buf, sizeof buf, clause);
    /* strip a trailing terminal punctuation so it doesn't ride on the verb */
    size_t bl = strlen(buf);
    while (bl > 0 && (buf[bl-1] == '.' || buf[bl-1] == '?' || buf[bl-1] == '!'))
        buf[--bl] = '\0';
    char *w[32];
    size_t nw = split_words(buf, w, 32);
    if (nw == 0) { put("Translate what?", out, out_size); return 1; }

    char result[512] = "";
    size_t rn = 0;

    /* First pass (EN->IT): find the clause's head-noun gender so the article
     * AND any adjective (which precedes the noun in English) agree with it. */
    char clause_gender = 'm';
    if (to_it) {
        for (size_t i = 0; i < nw; i++) {
            char *tok = strip_edge_punct(w[i]);
            char it_word[KB_TERM_LEN];
            if (gloss_lookup(b, tok, 1, it_word, sizeof it_word)) {
                char hits[1][KB_TERM_LEN];
                const char *ga[] = {it_word, NULL};
                if (kb_match(b->kb, "gender", ga, 2, hits, 1) == 1) {
                    clause_gender = (hits[0][0] == 'f') ? 'f' : 'm';
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < nw; i++) {
        char *tok = strip_edge_punct(w[i]);
        char piece[KB_TERM_LEN] = "";

        if (to_it && is_en_det(tok)) {
            /* U5 (gen286, teach-comprehension §5.5): the Italian article FORM is a
             * KB fact table, article(Def, Gender, VowelInitial, Form) in
             * grammar.p0 — not a C ternary. The engine still classifies
             * definiteness, gender and vowel-initial noun (the fixed substrate);
             * it now SELECTS the form from knowledge, so the grammar (including the
             * elided l'/un') is inspectable via kb.match and teachable via MCP. */
            int indef = (strcmp(tok, "a") == 0 || strcmp(tok, "an") == 0);
            int vowel_next = 0;
            if (i + 1 < nw) {
                char *nx = strip_edge_punct(w[i + 1]);
                char nt[KB_TERM_LEN];
                if (gloss_lookup(b, nx, 1, nt, sizeof nt))
                    vowel_next = strchr("aeiou", nt[0]) != NULL;
            }
            const char *aq[] = { indef ? "indef" : "def",
                                 clause_gender == 'f' ? "f" : "m",
                                 vowel_next ? "yes" : "no", NULL };
            char hit[1][KB_TERM_LEN];
            if (kb_match(b->kb, "article", aq, 4, hit, 1) == 1)
                snprintf(piece, sizeof piece, "%s", hit[0]);
            else
                /* backstop if grammar.p0 is absent (never mute); the table is the
                 * source of truth, so the elided forms live only there. */
                snprintf(piece, sizeof piece, "%s",
                         indef ? (clause_gender == 'f' ? "una" : "un")
                               : (clause_gender == 'f' ? "la" : "il"));
        } else if (!to_it && is_it_det(tok)) {
            int indef = (strcmp(tok, "un") == 0 || strcmp(tok, "una") == 0 ||
                         strcmp(tok, "uno") == 0);
            snprintf(piece, sizeof piece, "%s", indef ? "a" : "the");
        } else {
            if (!gloss_lookup(b, tok, to_it, piece, sizeof piece)) {
                char msg[160];
                snprintf(msg, sizeof msg,
                         "I can't translate \"%s\" yet.", tok);
                put(msg, out, out_size);
                return 1;
            }
            if (to_it) {
                /* a noun carries a gender fact; anything else translatable in a
                 * noun phrase is an adjective and agrees with the head noun.
                 * U5 (gen287): the feminine agreement is a KB morphology RULE now
                 * — agree_f/2 in grammar.p0 (swap the last char via the fem/2
                 * ending map, over chars/2). An invariant adjective yields no
                 * match, so it is left unchanged; masculine never changes. The C
                 * agree_adj is the backstop if grammar.p0 is absent. */
                char hits[1][KB_TERM_LEN];
                const char *ga[] = {piece, NULL};
                if (kb_match(b->kb, "gender", ga, 2, hits, 1) == 0) {
                    if (clause_gender == 'f') {
                        char af[1][KB_TERM_LEN];
                        const char *aq[] = {piece, NULL};
                        if (kb_match(b->kb, "agree_f", aq, 2, af, 1) == 1)
                            snprintf(piece, sizeof piece, "%s", af[0]);
                        else if (!kb_knows_pred(b->kb, "agree_f"))
                            agree_adj(piece, clause_gender);  /* backstop */
                    }
                }
            }
        }

        size_t pl = strlen(piece);
        if (rn + pl + 2 < sizeof result) {
            /* no space after an elided article (l'uomo, un'amica) */
            if (rn && result[rn - 1] != '\'') result[rn++] = ' ';
            memcpy(result + rn, piece, pl + 1);
            rn += pl;
        }
    }

    put(result, out, out_size);
    return 1;
}

/* --- module: synth (gen127, L12) ----------------------------------------
 * The INVERSE of mod_shell: synthesize a one-line shell command from a natural
 * spec ("count the lines in a file" -> "wc -l <file>"), grounded in the SAME
 * cmd/flag knowledge the interpreter reads (kb/experts/programming/bash.p0). The command is
 * SELECTED by matching the spec's action against command descriptions, and its
 * FLAGS by matching the spec's object nouns against flag descriptions — never a
 * stored spec->command pair, so held-out specs over known commands transfer.
 *
 * The flag-disambiguation trick: a command description like wc's
 * "counts_lines_words_and_bytes" already contains the words (lines/words/bytes)
 * that distinguish its flags. So we drop any spec word that stem-matches the
 * command's VERB (the first description token) before choosing flags — the verb
 * picks the command, the surviving object noun picks the flag.
 */

/* Light stemmer: true if the shorter of a,b is a prefix of the longer (count ~
 * counts ~ counting, line ~ lines, remove ~ removes). Requires >=3 shared chars
 * to avoid spurious hits; otherwise demands exact equality. */
static int stem_match(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    size_t m = la < lb ? la : lb;
    if (m < 3) return strcmp(a, b) == 0;
    return strncmp(a, b, m) == 0;
}

/* Does spec word `sw` stem-match any '_'-separated token of snake_case `atom`?
 * If `verb_out` is non-NULL it receives the atom's FIRST token (the verb). */
static int spec_hits_atom(const char *sw, const char *atom, char *verb_out,
                          size_t verb_size) {
    char buf[KB_TERM_LEN];
    snprintf(buf, sizeof buf, "%s", atom);
    int hit = 0, first = 1;
    for (char *tok = strtok(buf, "_"); tok; tok = strtok(NULL, "_")) {
        if (first && verb_out) { snprintf(verb_out, verb_size, "%s", tok); first = 0; }
        else first = 0;
        if (strlen(tok) >= 3 && stem_match(sw, tok)) hit = 1;
    }
    return hit;
}

static int mod_synth(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb) return 0;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    static const char *const triggers[] = {
        "what command ", "which command ",
        "write a command to ", "write a command that ",
        "give me a command to ", "give me a command that ",
        "synthesize a command to ", "synthesize a command that ",
        "quale comando ", "che comando ", "scrivi un comando per ",
        NULL,
    };
    const char *spec = NULL;
    for (const char *const *t = triggers; *t; t++) {
        size_t L = strlen(*t);
        if (strncmp(low, *t, L) == 0) { spec = low + L; break; }
    }
    if (!spec) return 0;
    /* strip a leading relative ("that counts" already handled; "counts ..." is
     * fine) and trailing punctuation */
    char spec_buf[256];
    copy_trim(spec_buf, sizeof spec_buf, spec);
    size_t sl = strlen(spec_buf);
    while (sl > 0 && (spec_buf[sl-1]=='?'||spec_buf[sl-1]=='.'||spec_buf[sl-1]=='!'))
        spec_buf[--sl] = '\0';

    /* spec content words */
    char wb[256]; snprintf(wb, sizeof wb, "%s", spec_buf);
    char *sw[32]; size_t nsw = split_words(wb, sw, 32);
    char words[32][KB_TERM_LEN]; size_t nw = 0;
    int wants_file = 0, wants_pattern = 0;
    for (size_t i = 0; i < nsw && nw < 32; i++) {
        char *t = strip_edge_punct(sw[i]);
        if (strcmp(t, "file") == 0 || strcmp(t, "files") == 0) wants_file = 1;
        if (strcmp(t, "pattern") == 0 || strcmp(t, "string") == 0) wants_pattern = 1;
        if (strlen(t) >= 3 && isalpha((unsigned char)t[0]) && !is_stopword(b, t))
            snprintf(words[nw++], KB_TERM_LEN, "%s", t);
    }
    if (nw == 0) { put("Specify what the command should do.", out, out_size); return 1; }

    /* enumerate known commands; score each by spec-word matches on its desc */
    char cmds[64][KB_TERM_LEN];
    const char *cpat[] = {NULL, NULL};
    size_t ncmd = kb_match(b->kb, "cmd", cpat, 2, cmds, 64);
    const char *best = NULL; int best_score = 0; char best_verb[KB_TERM_LEN] = "";
    for (size_t i = 0; i < ncmd; i++) {
        char desc[1][KB_TERM_LEN];
        const char *dq[] = {cmds[i], NULL};
        if (kb_match(b->kb, "cmd", dq, 2, desc, 1) != 1) continue;
        int score = 0; char verb[KB_TERM_LEN] = "";
        for (size_t j = 0; j < nw; j++) {
            char v[KB_TERM_LEN];
            if (spec_hits_atom(words[j], desc[0], v, sizeof v)) score++;
            if (!verb[0]) snprintf(verb, sizeof verb, "%s", v);
        }
        if (score > best_score) {
            best_score = score; best = cmds[i];
            snprintf(best_verb, sizeof best_verb, "%s", verb);
        }
    }
    if (!best) {
        put("I don't know a command for that yet.", out, out_size);
        return 1;
    }

    /* choose flags: object nouns (spec words NOT stem-matching the verb) that
     * hit a flag's description. Preserve KB order, dedup. */
    char flags[16]; size_t nf = 0;
    char fletters[32][KB_TERM_LEN];
    const char *fpat[] = {best, NULL, NULL};
    size_t nfl = kb_match(b->kb, "flag", fpat, 3, fletters, 32);
    for (size_t i = 0; i < nfl && nf < sizeof flags - 1; i++) {
        char fdesc[1][KB_TERM_LEN];
        const char *fq[] = {best, fletters[i], NULL};
        if (kb_match(b->kb, "flag", fq, 3, fdesc, 1) != 1) continue;
        /* single-letter flags only (the synthesizable short options) */
        if (strlen(fletters[i]) != 1) continue;
        int selected = 0;
        for (size_t j = 0; j < nw; j++) {
            if (best_verb[0] && stem_match(words[j], best_verb)) continue; /* the verb */
            if (spec_hits_atom(words[j], fdesc[0], NULL, 0)) { selected = 1; break; }
        }
        if (selected) {
            int dup = 0;
            for (size_t k = 0; k < nf; k++) if (flags[k] == fletters[i][0]) dup = 1;
            if (!dup) flags[nf++] = fletters[i][0];
        }
    }
    flags[nf] = '\0';

    char cmd[128];
    size_t o = (size_t)snprintf(cmd, sizeof cmd, "%s", best);
    if (nf) o += (size_t)snprintf(cmd + o, sizeof cmd - o, " -%s", flags);
    if (wants_pattern) o += (size_t)snprintf(cmd + o, sizeof cmd - o, " <pattern>");
    if (wants_file)    o += (size_t)snprintf(cmd + o, sizeof cmd - o, " <file>");

    char msg[160];
    snprintf(msg, sizeof msg, "%s", cmd);
    put(msg, out, out_size);
    return 1;
}

/* --- module: counterfactual (gen128, L20-deep) --------------------------
 * The reflexive closure of mod_strategy. gen105 reported the REAL trace ("why
 * did you answer that way?"). This module answers the COUNTERFACTUAL: "what
 * would you have said WITHOUT the <X> module?" / "what else could have answered?"
 * — and it does not confabulate. It RE-RUNS its own dispatch over the previous
 * turn's input with one module suppressed, and reports whatever the alternative
 * self actually computes. A deterministic C program simulating itself with a
 * part removed: the method (introspection driving the loop) becomes a feature
 * (introspection inside the agent), exactly the fractal move PRINCIPLES.md asks.
 *
 * Defined here (above the registry it walks) via a forward-declared replay helper
 * that is implemented after the registry table. */
static int replay_dispatch(Brain *b, const char *canon, const char *raw,
                           const char *suppress, char *who, size_t who_size,
                           char *out, size_t out_size);
static int is_registry_module(const char *name); /* defined after the table */
static void not_understood(Brain *b, const char *canon, char *out, size_t out_size);
static int repair_dispatch(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size); /* gen141, after the table */
static int is_intent_starter(const char *w);           /* gen80, after the table */

static int mod_counterfactual(Brain *b, const char *norm, const char *raw,
                              char *out, size_t out_size) {
    if (!b) return 0;
    (void)norm;

    char low[256];
    lowercase_copy(low, sizeof low, raw);

    /* "what else matched / could have answered / would have answered" — suppress
     * the actual winner and report the runner-up. */
    /* gen273: both counterfactual cue chains migrated to KB by the derived
     * kbfirst_migration plan (Track 5.4) — the phrasings are intent_cue facts
     * in kb/core/intents.p0, teachable at runtime like any vocabulary. */
    int else_form =
        kb_cue_match(b, "85_translate_synth_world_chain494", low);

    /* "without <X>" / "senza <X>" — suppress a named module. */
    const char *wp = strstr(low, "without ");
    if (!wp) wp = strstr(low, "senza ");
    int without_form = (wp != NULL);

    /* Only engage on a genuine counterfactual question shape. */
    int question_shape =
        kb_cue_match(b, "85_translate_synth_world_chain506", low) || else_form;
    if (!without_form && !else_form) return 0;
    if (without_form && !question_shape) return 0;

    if (!b->has_last_input || !b->has_trace) {
        put("I don't have a previous turn to reconsider yet.", out, out_size);
        return 1;
    }

    char suppress[32] = "";
    if (without_form) {
        const char *p = wp + (low[wp - low] == 'w' ? 8 : 6); /* past "without "/"senza " */
        while (*p && isspace((unsigned char)*p)) p++;
        /* skip a leading article: "without the X module" / "senza il modulo X" */
        char tok[32]; size_t n = 0;
        const char *q = p;
        /* pull up to two words; if first is article/"module", take the next */
        for (int grabbed = 0; grabbed < 3 && *q; grabbed++) {
            n = 0;
            while (*q && !isspace((unsigned char)*q) && n + 1 < sizeof tok) {
                char c = *q++;
                if (c == '?' || c == '.' || c == '!') break;
                tok[n++] = c;
            }
            tok[n] = '\0';
            while (*q && isspace((unsigned char)*q)) q++;
            if (n == 0) continue;
            if (strcmp(tok, "the") == 0 || strcmp(tok, "il") == 0 ||
                strcmp(tok, "module") == 0 || strcmp(tok, "modulo") == 0 ||
                strcmp(tok, "a") == 0)
                continue; /* skip filler, take the real module name next */
            snprintf(suppress, sizeof suppress, "%s", tok);
            break;
        }
        if (!suppress[0] || !is_registry_module(suppress)) {
            char msg[128];
            snprintf(msg, sizeof msg,
                     "I don't have a module called \"%s\" to set aside.",
                     suppress[0] ? suppress : "that");
            put(msg, out, out_size);
            return 1;
        }
    } else {
        /* else-form: suppress the recorded winner. */
        snprintf(suppress, sizeof suppress, "%s", b->trace_winner);
        if (strcmp(suppress, "fallback") == 0) {
            put("Nothing claimed the last turn — there was no winner to set aside.",
                out, out_size);
            return 1;
        }
    }

    char who[32] = "", ans[512] = "";
    int claimed = replay_dispatch(b, b->last_input_canon, b->last_input_raw,
                                  suppress, who, sizeof who, ans, sizeof ans);

    char msg[768];
    if (!claimed) {
        snprintf(msg, sizeof msg,
                 "Without '%s', nothing else matches — I'd fall back to \"%s\"",
                 suppress, ans);
    } else if (strcmp(who, b->trace_winner) == 0) {
        snprintf(msg, sizeof msg,
                 "Setting '%s' aside changes nothing — '%s' ran first anyway and "
                 "still answers \"%s\"", suppress, who, ans);
    } else {
        snprintf(msg, sizeof msg,
                 "Without '%s', the next module that matches is '%s', and it would "
                 "answer \"%s\"", suppress, who, ans);
    }
    put(msg, out, out_size);
    return 1;
}

/* --- module: world (gen142, E7) — local-world working memory ----------------
 * A real interlocutor can hold a TEMPORARY fictional or task-local world across
 * several turns — "in this story Rex is a dragon" — use those facts later, run a
 * SECOND world where the same name means something else, answer "what is
 * assumed?", and TEAR a world down so nothing leaks into what it permanently
 * believes. parrot0 had only one flat belief store: "rex is a dragon" wrote a
 * permanent fact. This module gives it scoped working memory.
 *
 * Mechanism (all in Brain state, never the persisted KB):
 *   ENTER   "in the <name> world/story[:] ..." or "start a world called <name>"
 *           or "in this story/world ..." (an anonymous scratch world) makes a
 *           named world the ACTIVE scope. An intro phrase with a trailing clause
 *           ("in this story rex is a dragon") enters the world AND asserts.
 *   ASSERT  while a world is active, "X is a/an Y" (or "X is not a Y") lands in
 *           the overlay tagged with the world id — NOT in the KB.
 *   QUERY   while a world is active, "is X a Y?" / "who is X?" / "what is X?" read
 *           the overlay first; the same X can answer differently in two worlds.
 *   INSPECT "what is assumed?" / "what is true in this world?" lists the active
 *           overlay — grounded in real state, not a canned line.
 *   SWITCH  "in the <name> world" with no clause re-activates an existing world.
 *   EXIT    "forget/end/close the <name> world", "forget this world", "leave the
 *           world" tears down (or deactivates) the scope; its facts vanish.
 * Registered right after repair, before the KB modules, so a world assertion
 * pre-empts the permanent learner exactly when a scope is open.
 */

/* find an existing live world by name, or -1. */
static int world_find(Brain *b, const char *name) {
    for (size_t i = 0; i < b->world_count; i++)
        if (b->world_live[i] && strcmp(b->worlds[i], name) == 0) return (int)i;
    return -1;
}

/* enter (creating if needed) the world `name`; returns its id, or -1 if full. */
static int world_enter(Brain *b, const char *name) {
    int id = world_find(b, name);
    if (id >= 0) { b->active_world = id; return id; }
    if (b->world_count >= BRAIN_WORLDS_MAX) return -1;
    id = (int)b->world_count++;
    snprintf(b->worlds[id], sizeof b->worlds[0], "%s", name);
    b->world_live[id] = 1;
    b->active_world = id;
    return id;
}

/* tear a world down: drop every overlay fact tagged with it, mark it dead, and
 * clear the active scope if it was the one removed. Returns 1 if it existed. */
static int world_teardown(Brain *b, int id) {
    if (id < 0 || (size_t)id >= b->world_count || !b->world_live[id]) return 0;
    size_t w = 0;
    for (size_t r = 0; r < b->wfact_count; r++)
        if (b->wfacts[r].world != id)
            b->wfacts[w++] = b->wfacts[r];
    b->wfact_count = w;
    b->world_live[id] = 0;
    if (b->active_world == id) b->active_world = -1;
    return 1;
}

/* record subj/pred (possibly negated) in the active world, replacing any prior
 * claim about the same subj/pred in THIS world. Returns 1 on success. */
static int world_assert(Brain *b, const char *subj, const char *pred, int neg) {
    int id = b->active_world;
    if (id < 0) return 0;
    for (size_t i = 0; i < b->wfact_count; i++)
        if (b->wfacts[i].world == id &&
            strcmp(b->wfacts[i].subj, subj) == 0 &&
            strcmp(b->wfacts[i].pred, pred) == 0) {
            b->wfacts[i].neg = neg;
            return 1;
        }
    if (b->wfact_count >= BRAIN_WFACTS_MAX) return 0;
    size_t s = b->wfact_count++;
    snprintf(b->wfacts[s].subj, sizeof b->wfacts[s].subj, "%s", subj);
    snprintf(b->wfacts[s].pred, sizeof b->wfacts[s].pred, "%s", pred);
    b->wfacts[s].world = id;
    b->wfacts[s].neg = neg;
    return 1;
}

/* query the active world for subj being a pred: +1 yes, -1 explicit no, 0 silent. */
static int world_query(Brain *b, const char *subj, const char *pred) {
    int id = b->active_world;
    if (id < 0) return 0;
    for (size_t i = 0; i < b->wfact_count; i++)
        if (b->wfacts[i].world == id &&
            strcmp(b->wfacts[i].subj, subj) == 0 &&
            strcmp(b->wfacts[i].pred, pred) == 0)
            return b->wfacts[i].neg ? -1 : 1;
    return 0;
}

/* "a" or "an" for the class word `p`, for natural replies ("is a dragon"). */
static const char *world_art(const char *p) {
    char c = (char)tolower((unsigned char)p[0]);
    return (c=='a'||c=='e'||c=='i'||c=='o'||c=='u') ? "an" : "a";
}

/* strip a leading article from a multi-word remainder pointer-style: returns the
 * pointer just past "a "/"an " if present, else the original. */
static const char *skip_article(const char *s) {
    if (strncmp(s, "a ", 2) == 0) return s + 2;
    if (strncmp(s, "an ", 3) == 0) return s + 3;
    return s;
}

/* True if `w` is a noun naming a local scope ("world"/"story"/"scenario"). */
static int is_world_noun(const char *w) {
    return strcmp(w, "world") == 0 || strcmp(w, "story") == 0 ||
           strcmp(w, "scenario") == 0 || strcmp(w, "puzzle") == 0;
}

/* Try to ASSERT/QUERY a single "X is [a] Y" clause inside the active world.
 * Returns 1 if it claimed the clause (writing `out`), 0 if the shape didn't fit.
 * `interrogative` marks a trailing '?' so subject-first questions are queries. */
static int world_clause(Brain *b, const char *clause, int interrogative,
                        char *out, size_t out_size) {
    char cb[256];
    copy_trim(cb, sizeof cb, clause);
    size_t n = strlen(cb);
    while (n > 0 && (cb[n-1]=='?'||cb[n-1]=='.'||cb[n-1]==','||cb[n-1]==' '))
        cb[--n] = '\0';
    char *w[8];
    size_t nw = split_words(cb, w, 8);

    /* QUERY: "is X [a] Y" */
    if (nw >= 3 && strcmp(w[0], "is") == 0) {
        const char *subj = w[1];
        size_t pi = 2;
        if (is_article(w[2]) && nw >= 4) pi = 3;
        if (pi != nw - 1) return 0;            /* one-word class only, for now */
        const char *pred = w[pi];
        int r = world_query(b, subj, pred);
        if (r > 0)      put("Yes.", out, out_size);
        else if (r < 0) put("No.", out, out_size);
        else {
            char m[160];
            snprintf(m, sizeof m, "Not assumed in the %s world.",
                     b->worlds[b->active_world]);
            put(m, out, out_size);
        }
        return 1;
    }

    /* QUERY: subject-first interrogative "X is [a] Y?" (the Italian shape
     * "rex è un drago?" canonicalizes here; same rule serves both languages). */
    if (interrogative && nw >= 3 && strcmp(w[1], "is") == 0 &&
        strcmp(w[0], "who") != 0 && strcmp(w[0], "what") != 0) {
        const char *subj = w[0];
        size_t pi = 2;
        if (strcmp(w[2], "not") == 0 && nw >= 4) pi = 3;
        if (pi < nw && is_article(w[pi]) && pi + 1 < nw) pi++;
        if (pi != nw - 1) return 0;
        const char *pred = w[pi];
        int r = world_query(b, subj, pred);
        if (r > 0)      put("Yes.", out, out_size);
        else if (r < 0) put("No.", out, out_size);
        else {
            char m[160];
            snprintf(m, sizeof m, "Not assumed in the %s world.",
                     b->worlds[b->active_world]);
            put(m, out, out_size);
        }
        return 1;
    }

    /* QUERY: "who is X" / "what is X" -> list X's classes in this world. */
    if (nw == 3 && (strcmp(w[0], "who") == 0 || strcmp(w[0], "what") == 0) &&
        strcmp(w[1], "is") == 0) {
        const char *subj = w[2];
        char list[400]; size_t off = 0, hits = 0;
        for (size_t i = 0; i < b->wfact_count; i++)
            if (b->wfacts[i].world == b->active_world &&
                !b->wfacts[i].neg &&
                strcmp(b->wfacts[i].subj, subj) == 0) {
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s %s", hits ? ", " : "",
                                        world_art(b->wfacts[i].pred),
                                        b->wfacts[i].pred);
                hits++;
            }
        if (hits == 0) {
            char m[160];
            snprintf(m, sizeof m, "I have no assumptions about %s in the %s world.",
                     subj, b->worlds[b->active_world]);
            put(m, out, out_size);
        } else {
            char m[480];
            snprintf(m, sizeof m, "In the %s world, %s is %s.",
                     b->worlds[b->active_world], subj, list);
            put(m, out, out_size);
        }
        return 1;
    }

    /* ASSERT: "X is [a] Y" or "X is not [a] Y" (subject-first, no trailing '?'). */
    if (!interrogative && nw >= 3 && strcmp(w[1], "is") == 0) {
        const char *subj = w[0];
        size_t pi = 2; int neg = 0;
        if (strcmp(w[2], "not") == 0 && nw >= 4) { neg = 1; pi = 3; }
        if (pi < nw && is_article(w[pi]) && pi + 1 < nw) pi++;
        if (pi != nw - 1) return 0;            /* one-word class only, for now */
        const char *pred = w[pi];
        if (!world_assert(b, subj, pred, neg)) {
            put("This world is full; I can't assume more here.", out, out_size);
            return 1;
        }
        char m[300];
        snprintf(m, sizeof m, "In the %s world, %s is%s %s %s.",
                 b->worlds[b->active_world], subj, neg ? " not" : "",
                 world_art(pred), pred);
        put(m, out, out_size);
        return 1;
    }
    return 0;
}

static int mod_world(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[512];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    int interrogative = (len > 0 && buf[len-1] == '?');

    /* -- INSPECT: "what is assumed?" / "what is true in this/the <name> world?" -
     * Gated on an active world, so the "is assumed"/"assumed" cue (which also
     * catches the Italian "cosa è assunto?" -> "...is assumed?") cannot fire in
     * ordinary prose where no scope is open. */
    if (b->active_world >= 0 &&
        (strstr(norm, "is assumed") || strstr(norm, "what do you assume") ||
         strstr(norm, "what is true in this") ||
         strstr(norm, "what is true in the") ||
         strstr(norm, "what holds in this"))) {
        int id = b->active_world;
        char list[480]; size_t off = 0, hits = 0;
        for (size_t i = 0; i < b->wfact_count; i++)
            if (b->wfacts[i].world == id) {
                off += (size_t)snprintf(list + off, sizeof list - off,
                                        "%s%s is%s %s %s", hits ? "; " : "",
                                        b->wfacts[i].subj,
                                        b->wfacts[i].neg ? " not" : "",
                                        world_art(b->wfacts[i].pred),
                                        b->wfacts[i].pred);
                hits++;
            }
        char m[600];
        if (hits == 0)
            snprintf(m, sizeof m, "Nothing is assumed yet in the %s world.",
                     b->worlds[id]);
        else
            snprintf(m, sizeof m, "In the %s world I am assuming: %s.",
                     b->worlds[id], list);
        put(m, out, out_size);
        return 1;
    }

    /* -- EXIT: tear down or leave a world. -------------------------------------- */
    {
        char *w[12]; char tb[512];
        snprintf(tb, sizeof tb, "%s", buf);
        size_t nw = split_words(tb, w, 12);
        /* "forget/end/close/leave [this|the] [<name>] world/story" */
        int teardown = 0, leave_only = 0;
        if (nw >= 2 && (strcmp(w[0], "forget") == 0 || strcmp(w[0], "end") == 0 ||
                        strcmp(w[0], "close") == 0 || strcmp(w[0], "drop") == 0))
            teardown = 1;
        else if (nw >= 2 && strcmp(w[0], "leave") == 0)
            { teardown = 1; leave_only = 1; }
        if (teardown) {
            /* find the world noun and an optional name token before it */
            int has_world_noun = 0; const char *named = NULL;
            for (size_t i = 1; i < nw; i++) {
                char *t = strip_edge_punct(w[i]);
                if (is_world_noun(t)) {
                    has_world_noun = 1;
                    if (i >= 1) {
                        char *prev = strip_edge_punct(w[i-1]);
                        if (!is_article(prev) && strcmp(prev, "this") != 0 &&
                            strcmp(prev, "the") != 0 && strcmp(prev, "that") != 0 &&
                            !is_world_noun(prev) && strcmp(prev, "forget") != 0 &&
                            strcmp(prev, "end") != 0 && strcmp(prev, "close") != 0 &&
                            strcmp(prev, "leave") != 0 && strcmp(prev, "drop") != 0)
                            named = prev;
                    }
                    break;
                }
            }
            if (has_world_noun) {
                int id = named ? world_find(b, named) : b->active_world;
                if (id < 0) {
                    put("There is no such world open.", out, out_size);
                    return 1;
                }
                char nm[48]; snprintf(nm, sizeof nm, "%s", b->worlds[id]);
                if (leave_only) {
                    /* leave: deactivate but keep the world and its facts alive */
                    if (b->active_world == id) b->active_world = -1;
                    char m[120];
                    snprintf(m, sizeof m, "Left the %s world.", nm);
                    put(m, out, out_size);
                } else {
                    world_teardown(b, id);
                    char m[140];
                    snprintf(m, sizeof m,
                             "Forgotten the %s world; none of it reached my memory.",
                             nm);
                    put(m, out, out_size);
                }
                return 1;
            }
        }
    }

    /* -- ENTER (+ optional inline clause): an intro phrase opens a scope. ------- */
    {
        const char *rest = NULL; char wname[48] = "";
        /* "in the <name> world[:] ..." / "in the <name> story ..." */
        const char *p = NULL;
        if (strncmp(buf, "in the ", 7) == 0)      p = buf + 7;
        else if (strncmp(buf, "in this ", 8) == 0) {
            /* anonymous scratch scope: name it after the noun (world/story). */
            const char *q = buf + 8;
            char first[32]; size_t fi = 0;
            while (q[fi] && !isspace((unsigned char)q[fi]) && fi+1 < sizeof first)
                { first[fi] = q[fi]; fi++; }
            first[fi] = '\0';
            char bare[32]; snprintf(bare, sizeof bare, "%s", first);
            if (is_world_noun(strip_edge_punct(bare))) {
                snprintf(wname, sizeof wname, "%s", strip_edge_punct(bare));
                rest = q + fi;
            }
        }
        if (p && !*wname) {
            /* read tokens until the world noun. The NAME is the adjacent content
             * token: before the noun in English ("saga world"), or after it in
             * Italian ("mondo saga" -> "world saga"). The clause begins after the
             * name token, wherever it sits. */
            char nb[512]; snprintf(nb, sizeof nb, "%s", p);
            char *nw_[12]; size_t nn = split_words(nb, nw_, 12);
            for (size_t i = 0; i < nn; i++) {
                char *t = strip_edge_punct(nw_[i]);
                if (is_world_noun(t)) {
                    size_t name_idx = i; /* default: no name -> use the noun word */
                    char *before = (i >= 1) ? strip_edge_punct(nw_[i-1]) : NULL;
                    if (before && !is_article(before) &&
                        strcmp(before, "the") != 0 && strcmp(before, "this") != 0 &&
                        strcmp(before, "that") != 0) {
                        snprintf(wname, sizeof wname, "%s", before);
                        name_idx = i; /* clause starts after the noun */
                    } else if (i + 1 < nn) {
                        char *after_tok = strip_edge_punct(nw_[i+1]);
                        /* the next token names the world only if it isn't itself
                         * the start of a clause ("X is …"); else stay anonymous. */
                        int looks_clause = (i + 2 < nn) &&
                            strcmp(strip_edge_punct(nw_[i+2]), "is") == 0;
                        if (!looks_clause && *after_tok) {
                            snprintf(wname, sizeof wname, "%s", after_tok);
                            name_idx = i + 1; /* clause starts after the name */
                        } else {
                            snprintf(wname, sizeof wname, "%s", t);
                            name_idx = i;
                        }
                    } else {
                        snprintf(wname, sizeof wname, "%s", t);
                        name_idx = i;
                    }
                    /* rest = original text after token `name_idx` in `p` */
                    const char *after = p;
                    for (size_t k = 0; k <= name_idx; k++) {
                        after = strchr(after, ' ');
                        if (!after) break;
                        after++;
                    }
                    rest = after ? after : "";
                    break;
                }
            }
        }
        if (*wname) {
            int id = world_enter(b, wname);
            if (id < 0) { put("I can't open another world right now.", out, out_size);
                          return 1; }
            /* an inline clause after the noun (skip a leading colon / "where") */
            const char *clause = rest ? rest : "";
            while (*clause == ':' || *clause == ',' || isspace((unsigned char)*clause))
                clause++;
            if (strncmp(clause, "where ", 6) == 0) clause += 6;
            if (*clause) {
                char ans[300];
                if (world_clause(b, clause, interrogative, ans, sizeof ans)) {
                    put(ans, out, out_size);
                    return 1;
                }
            }
            char m[160];
            snprintf(m, sizeof m,
                     "Opened the %s world. Tell me what is true in it.", wname);
            put(m, out, out_size);
            return 1;
        }
        /* "start/open a world called <name>" / "new world <name>" */
        if (strncmp(buf, "start a ", 8) == 0 || strncmp(buf, "open a ", 7) == 0 ||
            strncmp(buf, "new ", 4) == 0) {
            char *w2[12]; char sb[256]; snprintf(sb, sizeof sb, "%s", buf);
            size_t nn = split_words(sb, w2, 12);
            int wn_idx = -1;
            for (size_t i = 0; i < nn; i++)
                if (is_world_noun(strip_edge_punct(w2[i]))) { wn_idx = (int)i; break; }
            if (wn_idx >= 0) {
                const char *name = NULL;
                /* prefer the token after "called"/"named", else the one after noun */
                for (size_t i = 0; i + 1 < nn; i++) {
                    char *t = strip_edge_punct(w2[i]);
                    if (strcmp(t, "called") == 0 || strcmp(t, "named") == 0)
                        { name = strip_edge_punct(w2[i+1]); break; }
                }
                if (!name && (size_t)wn_idx + 1 < nn)
                    name = strip_edge_punct(w2[wn_idx + 1]);
                if (name && *name) {
                    int id = world_enter(b, name);
                    if (id < 0) { put("I can't open another world right now.",
                                      out, out_size); return 1; }
                    char m[160];
                    snprintf(m, sizeof m,
                             "Opened the %s world. Tell me what is true in it.", name);
                    put(m, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* -- IN-SCOPE assertion/query while a world is active. ---------------------- */
    if (b->active_world >= 0) {
        const char *clause = (const char *)skip_article(buf);
        if (world_clause(b, clause, interrogative, out, out_size))
            return 1;
    }

    return 0;
}

/* --- module: repair (gen141, E2) — the conversational repair loop -----------
 * The weakest signal in the emergence audit (E-series) is the human surface:
 * vague turns hit the wall. A real interlocutor, asked something it cannot pin
 * down — "is it a mammal?" with no antecedent, "what is it plus 10" with no
 * number — does not collapse into "I don't understand". It asks a NARROW
 * clarification, holds the unfinished thought, and RESUMES it once answered.
 *
 * This module is that loop, as a stateful bridge across two turns:
 *   OPEN   a question whose referential slot (an entity pronoun) cannot be
 *          resolved -> store the turn, ask specifically what the slot means.
 *   RESUME the next turn fills the slot. If it is itself a teachable assertion
 *          ("rex is a dog") it runs first, so coreference now resolves the
 *          stored pronoun; otherwise a concrete referent token is substituted
 *          into the stored turn. Either way the ORIGINAL intent is re-dispatched
 *          through the normal registry and its real answer returned.
 *   EXPIRE if the next turn is plainly a new intent (a fresh question/command),
 *          the pending state is dropped and that turn handled normally.
 * The window is a single turn: clarification is consumed immediately, never
 * lingering to hijack later conversation. It is registered FIRST so it can both
 * pre-empt the wall and catch the answer before any other module.
 *
 * Defined above the registry it resumes through, via a forward-declared helper
 * implemented after the registry table (same shape as mod_counterfactual). */

/* first word is an interrogative/auxiliary that opens a question */
static int is_question_opener(const char *w) {
    static const char *const q[] = {
        "is", "are", "was", "were", "does", "do", "did", "can", "could",
        "will", "would", "should", "what", "who", "where", "when", "why",
        "how", NULL };
    for (const char *const *p = q; *p; p++)
        if (strcmp(w, *p) == 0) return 1;
    return 0;
}

/* an arithmetic operator cue, so the clarification can ask for a NUMBER rather
 * than a referent when the gap is an operand. */
static int has_arith_cue(Brain *b, char **w, size_t nw) {
    static const char *const ops[] = {
        "plus", "minus", "times", "divided", "multiplied", "double", "triple",
        "half", "square", "sum", "product", NULL };
    (void)b;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strpbrk(t, "+-*/")) return 1;
        for (const char *const *p = ops; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}
