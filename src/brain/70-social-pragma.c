static int mod_social(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len-1]=='?'||buf[len-1]=='!'||buf[len-1]=='.'||buf[len-1]==' '))
        buf[--len] = '\0';
    if (len == 0) return 0;

    char tmp[256];
    memcpy(tmp, buf, len + 1);
    char *w[64];
    size_t nw = split_words(tmp, w, 64);
    if (nw == 0) return 0;

    /* gen73: all markers now come from kb/core/social.p0 via KB queries. */
    int has_opening   = tok_is_marker(b, "opening", w, nw) ||
                        has_social_pattern(b, "opening", buf);
    int has_closing   = tok_is_marker(b, "closing", w, nw) ||
                        has_social_pattern(b, "closing", buf);
    int has_thanks    = tok_is_marker(b, "thanks", w, nw) ||
                        has_social_pattern(b, "thanks", buf);
    int has_apology   = tok_is_marker(b, "apology", w, nw) ||
                        has_social_pattern(b, "apology", buf);
    int has_ambiguous = tok_is_marker(b, "ambiguous", w, nw);

    /* gen56/gen63/gen71: if the turn is mixed, let content modules handle the substance. */
    if (is_mixed_turn(b, buf, w, nw, has_opening, has_closing, has_thanks,
                      has_apology, has_ambiguous))
        return 0;

    /* gratitude */
    if (has_thanks) { tput(b, "You're welcome!", "Prego!", out, out_size); return 1; }

    /* apology — "scusa", "sorry", "mi dispiace" etc. */
    if (has_apology) { tput(b, "No problem.", "Nessun problema.", out, out_size); return 1; }

    /* wellbeing check-in — gen73: patterns from kb/core/social.p0 */
    if (has_social_pattern(b, "wellbeing", buf))
        { tput(b, "I'm well, thanks. How can I help?", "Sto bene, grazie. Come posso aiutarti?", out, out_size); return 1; }

    /* gen225 (basic-chat cat.1): phatic speech acts beyond hello/bye/thanks —
     * valediction, felicitation, well-wishing, condolence, blessing, politeness.
     * Recognized phrases are social_pattern(type, …) and replies are
     * response_template(type, …), both KB knowledge (EN+IT). The C knows only
     * the structural list of act TYPES, not the vocabulary; kb_response falls
     * back to a literal only if the KB file is absent, so the agent is engaged
     * but never mute. */
    {
        char phatic[16][KB_TERM_LEN];
        const char *pq[] = { NULL };
        size_t nphatic = kb_match(b->kb, "social_act_type", pq, 1, phatic, 16);
        for (size_t i = 0; i < nphatic; i++) {
            const char *type = kb_dequote(phatic[i]);
            if (has_social_pattern(b, type, buf)) {
                if (!kb_response(b, type, NULL, out, out_size))
                    kb_term_say(b, "that_s_kind_of_you_thank_you", NULL, 0, out, out_size);
                return 1;
            }
        }
    }

    /* position-disambiguated ambiguous marker: "ciao" opens early, closes late */
    if (has_ambiguous) {
        tput(b, b->turns <= 2 ? "Hi there!" : "Goodbye!", b->turns <= 2 ? "Ciao!" : "Arrivederci!", out, out_size);
        return 1;
    }

    /* explicit opening / closing markers */
    if (has_opening) { tput(b, "Hi there!", "Ciao!", out, out_size); return 1; }
    if (has_closing) { tput(b, "Goodbye!", "Arrivederci!", out, out_size); return 1; }

    /* gen72/gen73: laughter and conversational reactions — from kb/core/social.p0 */
    int has_reaction = 0;
    for (size_t i = 0; i < nw && !has_reaction; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && *t && b && b->kb) {
            const char *args[] = {t};
            if (kb_query(b->kb, "reaction_word", args, 1)) has_reaction = 1;
        }
    }
    if (has_reaction && nw <= 3) {
        kb_term_say(b, "glad_you_re_enjoying_the_conversation", NULL, 0, out, out_size);
        return 1;
    }

    /* the elimination move: a single contentless word as the opener is, by
     * exclusion, phatic contact — greet and invite content, without listing it.
     * Pure numbers are not contact, so require an alphabetic token. */
    if (nw == 1 && b->turns <= 1 && isalpha((unsigned char)w[0][0])) {
        tput(b, "Hi there! What would you like to talk about?", "Ciao! Di cosa ti va di parlare?", out, out_size);
        return 1;
    }

    return 0;
}

/* gen125 (chatsim-pulled, the outgrowing of the gen0 wall): the AFFECTIVE /
 * phatic register. The sim transcripts are full of casual social moves parrot0
 * could only meet with the bare "I don't understand" wall — laughter and emoji,
 * apologies tangled with content, encouragement ("you'll learn!"), frustration
 * at its own repetition, banter, and offers to switch language. These are not
 * information requests; they are TONE. mod_chitchat answers the tone in register
 * — honestly admitting parrot0 is small and not pretending to have parsed the
 * content — so a social move gets a social reply instead of a wall. It runs LAST,
 * after every substantive module, and fires ONLY on a real affective cue: a plain
 * unparseable statement with no such cue still gets the honest wall (so e.g. "is
 * the sky blue?" is untouched). This is phatic competence, not the gen0 parrot:
 * it reads the register from real signals and never claims understanding. */
static int has_emoji(const char *s) {
    for (const unsigned char *p = (const unsigned char *)s; *p; p++)
        if (*p >= 0xF0) return 1;          /* 4-byte UTF-8 = emoji plane */
    return 0;
}

/* gen228 (basic-chat cat.86): true when the WHOLE turn is a bare two-option
 * choice "A or B" — a single 'or' splitting two short (1-2 word) sides, with no
 * leading question word or verb that would make it a real question another module
 * owns ("is it morning or evening"). Recognized by SHAPE, not by a phrase list,
 * so it generalizes to any pair. */
static int is_binary_choice(Brain *b, const char *norm) {
    char buf[128];
    if (strlen(norm) >= sizeof buf) return 0;
    strcpy(buf, norm);
    char *w[16];
    size_t nw = split_words(buf, w, 16);
    if (nw < 3 || nw > 5) return 0;
    size_t orpos = nw, orcount = 0;
    for (size_t i = 0; i < nw; i++)
        if (lex_class_member(b, "70_social_pragma_lex135", w[i])) { orpos = i; orcount++; }
    if (orcount != 1 || orpos == 0 || orpos == nw - 1) return 0;
    size_t left = orpos, right = nw - orpos - 1;
    if (left < 1 || left > 2 || right < 1 || right > 2) return 0;
    /* gen382d: erano 26 letterali, inglesi e italiani, per dire "questa non e'
     * una scelta binaria ma una domanda". Non erano una classe nuova: erano
     * l'UNIONE di due che la KB ha gia' — question_word/1 e auxiliary/1 — copiate
     * a mano e percio' gia' incomplete in entrambe le direzioni (mancavano "am",
     * "qual", "quale"; e la copia non sapeva di "was", "were", "might"...).
     * Chiederlo alle classi le tiene allineate per costruzione, e una lingua
     * nuova entra una volta sola (mantra #3 e #5). */
    const char *q0[] = { w[0] };
    if (kb_query(b->kb, "question_word", q0, 1)) return 0;
    if (kb_query(b->kb, "auxiliary", q0, 1)) return 0;
    return 1;
}

static int mod_chitchat(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    if (!b) return 0;

    /* gen338 (L13 pragmatics, abstraction-ceiling beyond L12): speech acts
     * read between the lines. The act TYPES are a KB registry (pragma_act/1,
     * kb/core/pragmatics.p0); each act's surface forms are intent_cue facts
     * and its engaged reply a response_template (EN+IT). A NEW act is three
     * facts, zero C — this loop only walks the registry. Fires before the
     * smalltalk deflect ever sees the turn, so «can you pass me the salt?»
     * is read as the request it is, not as a question about parrot0's day. */
    {
        char acts[16][KB_TERM_LEN];
        const char *aq[1] = { NULL };
        size_t na = kb_match(b->kb, "pragma_act", aq, 1, acts, 16);
        for (size_t i = 0; i < na; i++) {
            /* raw too: Italian surface forms are rewritten by canonicalization
             * before norm reaches us (the experiential_move precedent). */
            if (!kb_cue_match(b, acts[i], norm) &&
                !(raw && kb_cue_match(b, acts[i], raw))) continue;
            if (kb_response(b, acts[i], NULL, out, out_size)) return 1;
        }
    }

    /* gen228 (basic-chat cat.86): a bare binary choice answered honestly —
     * parrot0 has no genuine preference. "would you rather" is its own honest
     * opener. Cues/replies are KB-first (intent_cue/response_template, EN+IT). */
    if (kb_cue_match(b, "would_rather", norm)) {
        if (!kb_response(b, "would_rather", NULL, out, out_size))
            kb_term_say(b, "i_ll_play_along_what_are_my_two_option", NULL, 0, out, out_size);
        return 1;
    }
    if (is_binary_choice(b, norm)) {
        if (!kb_response(b, "binary_choice", NULL, out, out_size))
            kb_term_say(b, "between_the_two_i_don_t_have_a_real_pr", NULL, 0, out, out_size);
        return 1;
    }

    /* gen403: qui stavano tredici bandiere, ognuna una catena di `cue(norm,
     * "…")` — ottantotto stringhe letterali nel C, con le risposte accanto e
     * l'ordine in cui provarle sepolto nella cascata di `if`. Meta' erano
     * italiane e meta' inglesi dentro lo stesso `||`: la lingua era diventata
     * una proprieta' del codice.
     *
     * Ora sono `chitchat_reaction(Kind, Priorita')` in kb/core/reactions.p0,
     * con le loro `intent_cue` e i loro `response_template` — la stessa forma
     * che il registro `pragma_act` qui sopra usa dal gen338. L'ordine e' un
     * dato perche' le reazioni si coprono a vicenda: «non preoccuparti,
     * imparerai» e' incoraggiamento e nomina l'imparare, e senza una priorita'
     * dichiarata quale delle due vinca dipenderebbe da come e' scritto il C.
     *
     * Il ciclo e' generico: una reazione nuova, in una lingua nuova, e' un
     * pugno di fatti e zero righe qui. */
    {
        char kinds[32][KB_TERM_LEN];
        char prios[32][KB_TERM_LEN];
        const char *rq[2] = { NULL, NULL };
        char rows[32][KB_TERM_LEN];
        size_t nk = kb_match(b->kb, "chitchat_reaction", rq, 2, kinds, 32);
        for (size_t i = 0; i < nk; i++) {
            const char *pq[2] = { kinds[i], NULL };
            if (kb_match(b->kb, "chitchat_reaction", pq, 2, rows, 1) >= 1)
                snprintf(prios[i], sizeof prios[i], "%s", rows[0]);
            else
                snprintf(prios[i], sizeof prios[i], "999");
        }
        /* ordine dichiarato, non ordine di scrittura */
        for (size_t i = 0; i + 1 < nk; i++)
            for (size_t j = i + 1; j < nk; j++)
                if (atoi(prios[j]) < atoi(prios[i])) {
                    char t[KB_TERM_LEN];
                    snprintf(t, sizeof t, "%s", kinds[i]);
                    snprintf(kinds[i], sizeof kinds[i], "%s", kinds[j]);
                    snprintf(kinds[j], sizeof kinds[j], "%s", t);
                    snprintf(t, sizeof t, "%s", prios[i]);
                    snprintf(prios[i], sizeof prios[i], "%s", prios[j]);
                    snprintf(prios[j], sizeof prios[j], "%s", t);
                }
        for (size_t i = 0; i < nk; i++) {
            if (!kb_cue_match(b, kinds[i], norm) &&
                !(raw && kb_cue_match(b, kinds[i], raw))) continue;
            /* Cio' che il turno LASCIA nella memoria, se lascia qualcosa. Era
             * `b->user_mood`, un campo C: la stessa conoscenza-che-nessuno-
             * puo'-interrogare che era il nome. Ora e' un fatto sull'utente. */
            const char *eq[3] = { kinds[i], NULL, NULL };
            char slotrow[1][KB_TERM_LEN], valrow[1][KB_TERM_LEN];
            if (kb_match(b->kb, "reaction_effect", eq, 3, slotrow, 1) >= 1) {
                /* kb_match riporta solo la PRIMA variabile: il valore vuole la
                 * seconda domanda, con lo slot ormai legato. */
                const char *vq[3] = { kinds[i], slotrow[0], NULL };
                if (kb_match(b->kb, "reaction_effect", vq, 3, valrow, 1) >= 1)
                    user_value_write(b, kb_dequote(slotrow[0]), kb_dequote(valrow[0]));
            }
            if (kb_response(b, kinds[i], NULL, out, out_size)) return 1;
        }
    }

    /* Struttura, non parole: una classe di codepoint e un asterisco iniziale.
     * Le risate e i vezzeggiativi sono parole e stanno nella KB, sotto
     * `playful`. */
    int emoji = has_emoji(raw);
    const char *t = raw; while (*t && isspace((unsigned char)*t)) t++;
    int emote = (*t == '*');     /* "*sighs heavily*", "*rolls eyes*" */

    /* Generic affective contact (emoji/emoticon/laughter/emote/endearment and
     * nothing more specific): engage in register, rotating so it never becomes a
     * broken record. Still honest — it names no understanding of the content. */
    if (emoji || emote || kb_cue_match(b, "playful", norm)) {
        if (kb_response(b, "playful", NULL, out, out_size)) return 1;
        kb_term_say(b, "i_m_a_simple_bot_but_i_m_here_for_it_go_on", NULL, 0, out, out_size);
        return 1;
    }
    return 0;
}

/* --- module: smalltalk (gen240, universal-comprehension social branch) ----
 * A conversational/experiential turn ("I just tried the oat-milk latte… have you
 * been there?") is COMPREHENSIBLE even with no fact to fetch and no specific cue
 * mod_chitchat already owns. The honest move is to keep the thread alive — a
 * continuity reply (acknowledge + ask back) from KB templates — instead of the
 * blind "I don't understand that yet." wall. It is ADDITIVE and LAST-RESORT
 * (registered after every content module, including mod_learn), so it never
 * pre-empts a real answer; it only catches turns that carry a social marker
 * (intent_cue(smalltalk_continue, …)) and that nothing else claimed. It fabricates
 * no experience or opinion — the templates are content-neutral continuity moves. */
/* gen335 (long-conversation): INITIATIVE — parrot0 LEADS instead of mirroring. The
 * limit of parrot0-vs-parrot0 is that two responders echo each other. When parrot0's
 * OWN previous reply comes back as the input (a self-play echo / a parroting loop), it
 * introduces a fresh topic it genuinely knows (conversation_seed/1, EN+IT — real facts,
 * no invented curiosity) and hands the turn back with an open question. Deterministic
 * over real state; the seed rotates via seed_shown/1 (REFLECTIVE), resetting when
 * exhausted. Gated on the loop signal, so it never steals a normal turn. */
static int mod_initiative(Brain *b, const char *norm, const char *raw,
                          char *out, size_t out_size) {
    (void)norm;
    if (!b || !b->kb || !raw || !b->last_reply[0]) return 0;
    size_t alen = strlen(raw), clen = strlen(b->last_reply);
    if (alen < 12 || alen != clen) return 0;
    for (size_t i = 0; i < alen; i++)
        if (tolower((unsigned char)raw[i]) !=
            tolower((unsigned char)b->last_reply[i]))
            return 0;                    /* not an exact self-play echo */

    char lang[8]; current_lang(b, lang, sizeof lang);
    int it = (lex_class_member(b, "entity_pronoun", lang));
    char seeds[64][KB_TERM_LEN]; size_t ns;
    if (it) { const char *q[2] = { "it", NULL }; ns = kb_match(b->kb, "conversation_seed", q, 2, seeds, 64); }
    else    { const char *q[1] = { NULL };       ns = kb_match(b->kb, "conversation_seed", q, 1, seeds, 64); }
    if (ns == 0) return 0;

    const char *chosen = NULL;
    for (int pass = 0; pass < 2 && !chosen; pass++) {
        for (size_t k = 0; k < ns; k++) {
            const char *sd = kb_dequote(seeds[k]);
            const char *sq[1] = { seeds[k] };
            if (pass == 0 && kb_query(b->kb, "seed_shown", sq, 1)) continue;   /* skip used */
            chosen = sd;
            kb_set_origin(b->kb, KB_REFLECTIVE);
            kb_assert(b->kb, "seed_shown", sq, 1);
            break;
        }
        if (pass == 0 && !chosen) {          /* all shown -> reset and reuse */
            for (size_t k = 0; k < ns; k++) { const char *sq[1] = { seeds[k] };
                kb_retract(b->kb, "seed_shown", sq, 1); }
        }
    }
    if (!chosen) return 0;

    char frame[256];
    if (!lang_template(b, "initiative_frame", frame, sizeof frame))
        snprintf(frame, sizeof frame, "%s",
                 "Here's something I know: %s. What do you make of it?");
    char msg[512]; snprintf(msg, sizeof msg, frame, chosen);
    put(msg, out, out_size);
    return 1;
}


/* gen426 — UN TOKEN SOLO NON E' UNA CHIACCHIERA.
 *
 * Le classi misurate hanno mostrato che «what», «help», «true», «qzxv», «dog» e
 * «yes» ricevevano tutti la STESSA risposta — «Hi there! What would you like to
 * talk about?» — e con la stessa firma di ragionamento: sei turni diversi,
 * indistinguibili da dentro e da fuori. Non era un difetto di una risposta: era
 * che nessuno guardava CHE COSA fosse quel token prima che lo smalltalk lo
 * prendesse.
 *
 * Qui il turno di una parola sola viene classificato prima del ripiego sociale,
 * e ogni classe ha la sua risposta:
 *
 *   una parola interrogativa   -> e' l'inizio di una domanda, non una domanda
 *   una cosa che la KB conosce -> si nomina e si chiede che cosa se ne voglia sapere
 *   un token che non dice nulla-> lo si cita e si chiede che cosa significhi
 *
 * Le classi sono conoscenza (`question_word/1`, e per il resto la KB stessa
 * risponde se conosce il termine) e le frasi sono template: aggiungere una
 * classe non costa C.
 *
 * Corre DOPO tutti i moduli di contenuto — se qualcuno sa rispondere davvero,
 * risponde lui — e PRIMA dello smalltalk, che altrimenti li acchiappa tutti. */
static int mod_lone(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    if (!b || !b->kb || !norm || !*norm) return 0;
    char buf[128];
    if (strlen(norm) >= sizeof buf) return 0;
    snprintf(buf, sizeof buf, "%s", norm);
    char *w[4];
    size_t nw = split_words(buf, w, 4);
    /* gen427 — UN DETERMINANTE NON FA DUE PAROLE. «a dog» chiede di «dog»
     * esattamente come «dog», e il modulo lo lasciava passare perche' contava i
     * token invece di guardarli. Quali parole siano determinanti sta in KB. */
    int had_determiner = 0;
    if (nw == 2) {
        char *d = strip_edge_punct(w[0]);
        const char *dq[1] = { d };
        if (!*d || !kb_query(b->kb, "determiner_word", dq, 1)) return 0;
        w[0] = w[1];
        nw = 1;
        had_determiner = 1;
    }
    if (nw != 1) return 0;
    char *tok = strip_edge_punct(w[0]);
    if (!*tok) return 0;

    /* gen427 — UNA PAROLA SOLA SI GUARDA COM'E' STATA SCRITTA.
     *
     * La canonicalizzazione espande le abbreviazioni — «u» diventa «you», «r»
     * diventa «are» — ed e' la lettura giusta dentro una frase («r u ok?»).
     * Da sola quella parola non ha contesto che la disambigui, e l'espansione
     * faceva rispondere «I have nothing on YOU» a chi aveva scritto «u», cioe'
     * nominava una parola che l'utente non ha scritto. Se anche il turno grezzo
     * e' un token solo, e' quello il token: sotto ci sono lookup di KB, e
     * cercare una parola diversa da quella ricevuta e' un errore di merito. */
    char rbuf[128];
    if (!had_determiner && raw && *raw && strlen(raw) < sizeof rbuf) {
        snprintf(rbuf, sizeof rbuf, "%s", raw);
        for (char *p = rbuf; *p; p++) *p = (char)tolower((unsigned char)*p);
        char *rw[4];
        if (split_words(rbuf, rw, 4) == 1) {
            char *rtok = strip_edge_punct(rw[0]);
            if (*rtok) tok = rtok;
        }
    }
    /* alfanumerico, ma non tutto cifre: «a1» e' un token opaco quanto «qz», e
     * lasciarlo fuori per un carattere era una distinzione senza differenza. I
     * numeri nudi hanno gia' il loro riconoscitore. */
    int has_alpha = 0;
    for (const char *p = tok; *p; p++) {
        if (isalpha((unsigned char)*p)) has_alpha = 1;
        else if (!isdigit((unsigned char)*p)) return 0;
    }
    if (!has_alpha) return 0;
    char msg[400];
    /* LA PAROLA INTERROGATIVA VIENE PRIMA DI OGNI GUARDIA. «what» e «when» sono
     * anche stopword, e mettere il filtro sociale davanti al controllo le
     * lasciava al saluto generico — cioe' proprio il caso che questo modulo
     * esiste per separare. */
    /* gen427 — UNA PAROLA PUO' AVERE UNA MOSSA SUA. «help» non chiede che cosa
     * si voglia sapere sulla parola «help»: chiede aiuto, e la risposta e'
     * offrirlo. Quali parole abbiano una mossa dedicata, e quale sia, e' un
     * fatto: una parola nuova costa una riga e nessuna ricompilazione. */
    {
        const char *dsq[2] = { tok, NULL };
        char say[1][KB_TERM_LEN];
        if (kb_match(b->kb, "lone_word_say", dsq, 2, say, 1) == 1) {
            char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", say[0]);
            if (kb_response_slots(b, kb_dequote(sb), NULL, 0, msg, sizeof msg)) {
                put(msg, out, out_size);
                return 1;
            }
        }
    }
    const char *qq[1] = { tok };
    if (kb_query(b->kb, "question_word", qq, 1)) {
        /* gen491 — LA FRASE COMPOSTA E MAI EMESSA, terza volta (M18).
         *
         * `kb_response_slots` riempie `msg`, e quando RIUSCIVA nessuno scriveva
         * `msg` in `out`: solo il ramo di ricaduta emetteva. Da quando la frase
         * e' passata in KB (`response_template(lone_question_word, …)`) il ramo
         * che riesce e' quello NORMALE, quindi `where`, `which`, `ok` e `no`
         * rispondevano con una stringa VUOTA — un muro che non si vede nemmeno.
         *
         * E' esattamente il P4.5 di LEARN_TODO: la lista passa alla conoscenza e
         * la chiave di lettura resta indietro. La stessa forma era gia' stata
         * trovata al gen452 (`mod_role`) e al gen456 (`try_teach_form`): vale
         * come CLASSE di difetto da cercare, non come incidente. */
        if (!kb_response_slots(b, "lone_question_word", NULL, 0, msg, sizeof msg))
            kb_term_say(b, "that_s_the_start_of_a_question_what_would_yo", NULL, 0, msg, sizeof msg);
        put(msg, out, out_size);
        return 1;
    }
    /* L'ASSENSO E IL DINIEGO sono una mossa, non un topic: «ok» non chiede di
     * sapere qualcosa su «ok», accusa ricezione. Le due classi sono conoscenza,
     * quindi una parola nuova — in qualunque lingua — costa una riga. */
    {
        const char *aq[1] = { tok };
        if (kb_query(b->kb, "assent_word", aq, 1) ||
            kb_query(b->kb, "dissent_word", aq, 1)) {
            if (!kb_response_slots(b, "lone_assent", NULL, 0, msg, sizeof msg))
                kb_term_say(b, "got_it_what_would_you_like_to_do", NULL, 0, msg, sizeof msg);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* I SALUTI E I MARCATORI SOCIALI RESTANO SOCIALI. «hi» da solo e' contatto,
     * non un topic da interrogare: rubarglielo produrrebbe «che cosa vorresti
     * sapere su hi?», che e' peggio del saluto generico che si sta sostituendo. */
    {
        const char *smq[2] = { NULL, tok };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "social_marker", smq, 2, hit, 1) > 0) return 0;
        const char *rq[1] = { tok };
        if (kb_query(b->kb, "reaction_word", rq, 1)) return 0;
        /* SC30 — E CEDE ANCHE A CIO' CHE E' STATO INSEGNATO PARLANDO.
         *
         * Misurato: insegnata «helyla» come attacco informale, la cue entrava,
         * era viva, e il turno non cambiava mai comportamento — questo modulo
         * classificava il token prima che il sociale potesse leggerla. La
         * lezione era corretta e senza TITOLO, che e' peggio di un muro:
         * rispondere «non ho niente su helyla» dopo averlo imparato e' falso.
         *
         * Il titolo non e' una posizione nel registry — spostare questo modulo
         * rimanderebbe il problema alla parola dopo. E' una proprieta' del
         * turno, e qui e' la stessa cessione delle due righe sopra. */
        if (kb_query(b->kb, "declared_surface", rq, 1)) return 0;
    }

    /* gen427 — LA GUARDIA SULLE STOPWORD E' STATA TOLTA, e le classi misurate
     * dicevano da tre lunghezze che era di troppo: «a», «e», «i», «o», «u»,
     * «r» e «if» ricevevano il saluto generico perche' sono stopword, cioe'
     * perche' sono parole-funzione. Ma una parola-funzione DA SOLA non e'
     * contatto fatico: e' un frammento opaco quanto «qzxv», e la mossa giusta
     * e' dirlo. Chi merita davvero il sociale e' gia' protetto sopra
     * (`social_marker`, `reaction_word`), e le interrogative sono prese prima
     * di ogni guardia: quello che restava qui era solo il caso da separare. */

    /* ── LA RIVENDICAZIONE GENERICA E' SOSPESA, E LA SOSPENSIONE E' UN FATTO ──
     *
     * F., 2026-08-30: «mod_lone dovrebbe rivendicare alcuni casi speciali, non
     * tutti i turni con una sola parola».
     *
     * Ha ragione, e la misura lo mostra: insegnata «helyla» come attacco
     * informale, la cue entrava, era viva, e il turno non cambiava mai
     * comportamento perche' questo modulo classificava il token prima che il
     * sociale potesse leggerla. Una lezione corretta e senza TITOLO e' peggio di
     * un muro — «non ho niente su helyla» dopo averlo imparato e' falso.
     *
     * Da qui in giu' ci sono le due rivendicazioni GENERICHE: «che cosa vuoi
     * sapere su X» per un token noto, «non ho niente su X» per uno ignoto.
     * Nessuna delle due poggia su conoscenza dichiarata su QUEL token — a
     * differenza di tutto cio' che sta sopra (`lone_word_say`, `question_word`,
     * `assent_word`, e le cessioni a `social_marker`/`reaction_word`/
     * `declared_surface`), che resta intatto ed e' il vero valore del modulo.
     *
     * Non si cancella: si DICHIARA, come il gen491 ha gia' fatto qui sotto per
     * la scommessa fonotattica. Senza `move_policy(lone_bare_token, claim)` il
     * modulo cede e il turno prosegue lungo il registry — dove trova un muro che
     * PROPONE il rimedio invece di limitarsi a dire che non sa (§18.27). Con
     * quel fatto asserito, il comportamento storico torna nello stesso turno.
     *
     * TODO(universal-comprehension): la sospensione e' temporanea e serve a non
     * far pagare alla comprensione universale il prezzo di una rivendicazione
     * per eliminazione. Il rientro passa da SC30/SC31 — quando il RUOLO del
     * turno decidera' chi ha titolo, questo modulo potra' rivendicare i propri
     * casi speciali senza dover arrivare per primo. Vedi LEARN_TODO.md SC34.
     *
     * MISURATO mentre si sospendeva: sospendere anche il ramo NOTO fa regredire
     * il gen491 — «milano» tornava a «non capisco» benche' parrot0 sappia tre
     * cose su Milano. Quel ramo non e' una rivendicazione cieca: poggia su stato
     * KB reale su QUEL token, ed e' quindi uno dei casi speciali che F. chiede
     * di conservare. La sospensione riguarda solo il token IGNOTO. */

    /* UNA LETTERA SOLA NON E' UN TOPIC, anche quando la KB ha per caso un
     * predicato che si chiama cosi'. «What would you like to know about b?» e'
     * una domanda che nessuno puo' raccogliere: e' un token opaco e va detto. */
    char desc[256];
    /* gen491 — IL TERMINE SOTTO CUI LA KB TIENE I FATTI PUO' NON ESSERE LA
     * PAROLA DETTA. Misurato sul turno segnalato da F.:
     *
     *     you> milano        ->  «Ciao! Di cosa ti va di parlare?»
     *
     * e parrot0 su Milano sa tre cose (`capital_of_region(lombardy, milan)`,
     * `demonym`, `artwork_place`). Non era ignoranza: era che il lookup cercava
     * `milano` mentre i fatti stanno sotto `milan`, quindi il topic risultava
     * sconosciuto e cadeva nel saluto per eliminazione qui sotto.
     *
     * `canonical_value/2` e' la procedura che il resto del motore gia' usa per
     * questo (turn-frames.p0 §3). Si cerca sotto il termine canonico e si NOMINA
     * la parola detta: cercare una parola diversa da quella ricevuta sarebbe un
     * errore di merito, dirla diversa da come e' stata detta anche. */
    char canon_tok[KB_TERM_LEN];
    snprintf(canon_tok, sizeof canon_tok, "%s", tok);
    {
        const char *cvq[2] = { tok, NULL };
        char cv[1][KB_TERM_LEN];
        if (kb_match(b->kb, "canonical_value", cvq, 2, cv, 1) == 1) {
            char cb[KB_TERM_LEN];
            snprintf(cb, sizeof cb, "%s", cv[0]);
            const char *cd = kb_dequote(cb);
            if (*cd) snprintf(canon_tok, sizeof canon_tok, "%s", cd);
        }
    }
    int known = strlen(tok) >= 2 &&
                (kb_knows_pred(b->kb, tok) ||
                 kb_describe_entity(b->kb, tok, desc, sizeof desc) ||
                 kb_knows_pred(b->kb, canon_tok) ||
                 kb_describe_entity(b->kb, canon_tok, desc, sizeof desc));
    const KbResponseSlot sl[] = { { "token", tok } };
    if (known) {
        if (!kb_response_slots(b, "lone_known_topic", sl, 1, msg, sizeof msg))
            { const KbResponseSlot _rs[] = { { "tok", tok } };
      kb_term_say(b, "what_would_you_like_to_know_about_x", _rs, 1, msg, sizeof msg); }
    } else {
        /* La rivendicazione su un token IGNOTO e' quella senza base: parrot0 non
         * sa niente su questo token e nessuna conoscenza dichiarata lo nomina.
         * Senza `move_policy(lone_bare_token, claim)` il modulo cede qui, e il
         * turno prosegue fino a un muro che PROPONE il rimedio invece di
         * limitarsi a dire che non sa (§18.27). Il fatto riporta il
         * comportamento storico nello stesso turno. */
        const char *bare[2] = { "lone_bare_token", "claim" };
        if (!kb_query(b->kb, "move_policy", bare, 2)) return 0;

        /* gen427 — UNA PAROLA PLAUSIBILE CHE NON CONOSCO PUO' ESSERE UN SALUTO.
         *
         * La mossa per eliminazione del gen52 — una parola sola e senza
         * contenuto, al primo turno, e' contatto fatico — vale ancora per
         * «ahoy», che nessuna lista contiene e che un umano ricambierebbe. Non
         * vale per «qzxvb», e la differenza non e' nella posizione nel discorso:
         * e' nella FORMA della parola. «ahoy» si puo' pronunciare, «qzxvb» no.
         *
         * Quindi si cede al sociale solo quando il token e' pronunciabile E la
         * KB non lo conosce come parola-funzione: «if» e «the» sono parole vere
         * e non sono saluti, e a quelle la risposta giusta e' dire che non c'e'
         * niente. Le vocali sono un fatto (`vowel_letter/1`), non una stringa
         * nel C: un alfabeto nuovo costa righe di KB. */
        int pronounceable = 0, all_alpha = 1;
        for (const char *p = tok; *p; p++) {
            if (!isalpha((unsigned char)*p)) { all_alpha = 0; continue; }
            char c[2] = { (char)tolower((unsigned char)*p), '\0' };
            const char *vq[1] = { c };
            if (kb_query(b->kb, "vowel_letter", vq, 1)) pronounceable = 1;
        }
        /* «a1» ha una vocale e non e' una parola: una cifra in mezzo esclude che
         * lo sia, e il saluto per eliminazione vale solo per le parole. */
        pronounceable = pronounceable && all_alpha;
        /* gen491 — LA MOSSA DAVANTI A UNA PAROLA SCONOSCIUTA E' UNA POLICY,
         * NON UNA PROPRIETA' DELLE LETTERE.
         *
         * Il gen427 decideva per FONOTATTICA che una parola pronunciabile e
         * sconosciuta fosse un saluto («ahoy»). La sonda `bare_topic_probe`
         * mostra che quella scommessa perde quasi sempre: `milano`, `mercurio`,
         * `python`, `rosso`, `zorblat` sono tutte pronunciabili e nessuna e' un
         * saluto — e il modello di frontiera, davanti a `qzxvb`, non saluta mai:
         * NOMINA la parola («Could you clarify what you mean by "qzxvb"?»,
         * tests/sym/measure-len5-full.md).
         *
         * Indovinare un atto sociale dalla forma delle lettere e' ridurre cio'
         * che parrot0 vede per avere ragione per costruzione. Chi merita davvero
         * il registro sociale e' gia' protetto sopra da conoscenza dichiarata
         * (`social_marker`, `reaction_word`, `assent_word`), e un saluto nuovo e'
         * una riga di KB.
         *
         * La scommessa non viene cancellata, viene DICHIARATA: resta raggiungibile
         * asserendo `move_policy(lone_unknown_word, social_contact)` e sparisce
         * ritraendola, nello stesso turno (keep-secondary-structures). */
        const char *mpq[2] = { "lone_unknown_word", "social_contact" };
        if (kb_query(b->kb, "move_policy", mpq, 2) &&
            pronounceable && strlen(tok) >= 2 && !is_stopword(b, tok)) return 0;
        if (!kb_response_slots(b, "lone_unknown_token", sl, 1, msg, sizeof msg))
            { const KbResponseSlot _rs[] = { { "tok", tok } };
      kb_term_say(b, "i_have_nothing_on_x_could_you_say_what_you_m", _rs, 1, msg, sizeof msg); }
    }
    put(msg, out, out_size);
    return 1;
}

static int mod_smalltalk(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
    char *w[64]; size_t nw = split_words(tmp, w, 64);
    if (nw < 3) return 0;                       /* a fragment is not a conversation */
    /* Conversational register, recognized by KB smalltalk cues (multi-word social
     * phrases, EN+IT). Last-resort: only turns nothing else claimed reach here, and
     * an impersonal factual question carries no such cue, so it falls through to the
     * honest informed decline rather than getting a "tell me more". */
    if (kb_cue_match(b, "smalltalk_continue", norm))
        return kb_response(b, "smalltalk_continue", NULL, out, out_size);

    /* gen335 (long-conversation, social branch): an experiential QUESTION addressed to
     * parrot0 ("do you...", "have you got...", "what are your plans?") that reached the
     * last resort — nothing factual claimed it, so it is a smalltalk move. Recognise the
     * STRUCTURE (a second-person question), not a phrase list: a question opener + "you"/
     * "your". Deflect honestly (no invented experience) and hand the turn back. */
    {
        int addressed = 0;
        for (size_t i = 0; i < nw; i++) {
            char *t = strip_edge_punct(w[i]);
            if (lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)||    /* EN */
                lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)||        /* IT */
                lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)||lex_class_member(b, "second_person_form", t)) { addressed = 1; break; }
        }
        /* an experiential marker ("for fun", "hobby", "tempo libero") is itself a
         * smalltalk signal — covers Italian PRO-DROP questions with no explicit "tu". */
        if (!addressed && (kb_cue_match(b, "experiential_move", norm) ||
                           kb_cue_match(b, "experiential_move", raw))) addressed = 1;
        const char *o = w[0];
        int question = (strchr(norm, '?') != NULL) ||
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||
            lex_class_member(b, "question_opener", o)||                                              /* EN openers */
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||          /* IT openers */
            lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o)||lex_class_member(b, "question_opener", o);
        /* gen453: una domanda COPULARE su un attributo di parrot0 non e' una
         * mossa sociale. Sviarla con «non ne ho di miei» e' una risposta
         * plausibile e non pertinente — peggio di un muro (PRINCIPLES.md). Quali
         * frasi chiedano un attributo sta in `self_attribute_request`, quindi la
         * distinzione si insegna senza ricompilare. */
        if (addressed && question &&
            !kb_cue_match(b, "self_attribute_request", norm))
            return kb_response(b, "smalltalk_deflect", NULL, out, out_size);
    }
    return 0;
}

/* --- module: pragma (gen142, E3) -----------------------------------------
 * Pragmatic intent from turn SHAPE, not from a phrase list. mod_chitchat /
 * mod_social already cover the AFFECTIVE register, but they do it with growing
 * cue lists: a held-out phrasing of the same speech act ("give me something to
 * think about", "could we chat about cheese", "i have no clue what to talk
 * about", "anyway, is socrates a man") still hits the wall. This module infers
 * the SPEECH ACT from structural FEATURES of the turn and routes each act to a
 * DIFFERENT conversational move, so unseen phrasings transfer.
 *
 * The features (computed once, below), never a memorized string:
 *   - opener class of the first content token: a DISCOURSE marker (anyway, so,
 *     well, ok, "by the way") vs a SOFT-REQUEST verb (tell/give/say) vs a MODAL
 *     invitation (can/could/shall + we);
 *   - a TOPIC-INTRO frame ("about/discuss/talk about/switch to <X>") + its object;
 *   - HEDGE markers (maybe/guess/suppose/dunno/"not sure"/perhaps) = hesitation;
 *   - NEGATION (not/no/never) and a CONTRASTIVE connective (but/however/though);
 *   - a STANCE object (you/that/this/right/agree/sense) = the thing disagreed with;
 *   - presence of a CONTENT PREDICATE: a token a content module could act on — a
 *     digit/arith operator, a known KB predicate or entity, or an assertion shape;
 *   - word count / question form.
 *
 * Routing (each a distinct MOVE):
 *   1. discourse-marker opener + residual content  -> STRIP the opener and
 *      re-dispatch the residue, so the content task SURVIVES the social wrapper
 *      ("anyway, is socrates a man" -> answers the question; "by the way, what is
 *      2 plus 2" -> 4). Re-entrancy-guarded.
 *   2. soft request with no content object ("tell me something", "give me
 *      something to think about", "say anything") -> invite a topic.
 *   3. topic-intro with an object ("can we talk about cheese", "let us switch to
 *      football") -> acknowledge and steer onto X, naming X (pulled from the turn).
 *   4. hesitation (hedge markers, no content) -> reassure, lower the stakes.
 *   5. disagreement (negation about a stance object, no content) -> accept the
 *      pushback, invite the correction.
 * Pure content/questions never reach here (the module runs late); a marker-only
 * turn that matches none of these is declined so social/chitchat still answer. */

/* first ALPHABETIC content token (strips edge punctuation) into dst; returns
 * its index in w[] or nw if none. */
static size_t first_word_tok(char **w, size_t nw, char *dst, size_t dstn) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && isalpha((unsigned char)t[0])) {
            snprintf(dst, dstn, "%s", t);
            return i;
        }
    }
    if (dstn) dst[0] = '\0';
    return nw;
}

/* a leading DISCOURSE marker: a connective whose only job is to manage the
 * channel/turn, carrying no propositional content. Single-token forms plus the
 * two-token "by the way". Detected by position (the opener), so it can be peeled
 * off without touching the content that follows. */
static int is_discourse_opener(Brain *b, char **w, size_t nw, size_t *skip) {
    if (nw == 0) return 0;
    char tmp[64];
    snprintf(tmp, sizeof tmp, "%s", w[0]);
    const char *t = strip_edge_punct(tmp);
    /* "by the way" -> skip 3 tokens */
    if (nw >= 3 && lex_class_member(b, "70_social_pragma_lex616", t)) {
        char a[64], c[64];
        snprintf(a, sizeof a, "%s", w[1]); snprintf(c, sizeof c, "%s", w[2]);
        if (lex_class_member(b, "english_determiner", strip_edge_punct(a)) &&
            lex_class_member(b, "70_social_pragma_lex620", strip_edge_punct(c))) { *skip = 3; return 1; }
    }
    /* gen335 round-3: KB-first migration — query discourse_opener/1 from KB
     * instead of a hardcoded C array. PRINCIPLES.md: engine fixed, lexicon learns. */
    if (b && b->kb) {
        char atom[64];
        size_t j = 0;
        for (const char *c = t; *c && j + 1 < sizeof atom; c++)
            atom[j++] = (char)tolower((unsigned char)*c);
        atom[j] = 0;
        const char *args[] = {atom};
        if (kb_query(b->kb, "discourse_opener", args, 1)) { *skip = 1; return 1; }
    }
    return 0;
}

/* a hedge / hesitation marker anywhere in the turn. */
static int has_hedge(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        const char *q[] = { t };
        if (b && b->kb && kb_query(b->kb, "hedge_word", q, 1)) return 1;
    }
    return 0;
}

/* a contrastive connective anywhere in the turn. */
static int has_contrastive(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        const char *q[] = { t };
        if (b && b->kb && kb_query(b->kb, "contrastive_connector", q, 1)) return 1;
    }
    return 0;
}

/* a negation marker anywhere in the turn. */
static int has_negation(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        const char *q[] = { t };
        if (b && b->kb && kb_query(b->kb, "negation_marker", q, 1)) return 1;
    }
    return 0;
}

/* True if token `t` is a STANCE PREDICATE: a word that, when negated, expresses
 * a disagreement about the prior claim — "(don't) agree", "(not) right/sure/
 * true/correct/convinced", "(doesn't) make sense". These are PREDICATES, not
 * mere objects: "that"/"you" alone are not stance ("dont say that" is an order,
 * not a disagreement), so the move keys on the predicate. */
static int is_stance_pred(Brain *b, const char *t) {
    const char *q[] = { t };
    return b && b->kb && kb_query(b->kb, "stance_predicate", q, 1);
}

/* A disagreement is a NEGATED stance predicate ("i don't agree", "that is not
 * right", "not so sure", "non sono d'accordo"), or the standalone verb
 * "disagree"/"dissento". Shape, not phrase: any negation co-occurring with a
 * stance predicate in a short turn reads as pushback. */
static int is_disagreement(Brain *b, char **w, size_t nw) {
    int neg = 0, stance = 0, plain = 0;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        const char *q[] = { t };
        if (b && b->kb && kb_query(b->kb, "negation_marker", q, 1)) neg = 1;
        if (is_stance_pred(b, t)) stance = 1;
        if (b && b->kb && kb_query(b->kb, "standalone_disagreement", q, 1)) plain = 1;
    }
    return plain || (neg && stance);
}

/* True if token is an OPEN QUANTIFIER object — the placeholder a content-free
 * soft request reaches for ("something", "anything", "qualcosa", "qualunque
 * cosa"). A CONCRETE object ("a story", "about C") is not open, so "tell me a
 * story" stays a real (unfulfillable) request and hits the honest wall instead
 * of this fill-the-silence move. */
static int has_open_quantifier(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        const char *q[] = { t };
        if (b && b->kb && kb_query(b->kb, "open_quantifier", q, 1)) return 1;
    }
    return 0;
}

/* True if the turn carries a CONTENT PREDICATE — something a content module
 * could actually act on. This is the gate that keeps the pragmatic moves from
 * swallowing real tasks: a digit or arithmetic operator, an assertion shape
 * (" is a "/" is an "), or a token that is a known KB predicate or entity. */
static int has_content_predicate(Brain *b, const char *canon, char **w, size_t nw) {
    if (kb_cue_match(b, "70_social_pragma_chain756", canon)) return 1;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (!t || !*t) continue;
        if (isdigit((unsigned char)t[0])) return 1;
        if (b && b->kb && strlen(t) >= 3) {
            if (kb_knows_pred(b->kb, t) && !is_internal_pred(b->kb, t)) return 1;
        }
    }
    return 0;
}

/* Pull the TOPIC object out of a topic-intro frame: the head noun after
 * "about/discuss/discutere/switch to/change to/parlare di/parliamo di". Returns
 * 1 and writes the object (first substantive token after the cue) into dst. */
static int topic_object(Brain *b, char **w, size_t nw, char *dst, size_t dstn) {
    for (size_t i = 0; i + 1 < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (!lex_class_member(b, "topic_preposition", t)) continue;
        for (size_t j = i + 1; j < nw; j++) {
            char o[64];
            snprintf(o, sizeof o, "%s", w[j]);
            const char *ot = strip_edge_punct(o);
            /* the head noun: first alphabetic token that is not a bare article.
             * We deliberately do NOT filter on the general stopword lexicon — a
             * topic word like "formaggio" happens to be listed there for a
             * chitchat test, but after "di"/"about" it is the genuine topic. */
            /* TODO(kb-first): GLI ARTICOLI SONO GIA' NELLA KB.
             * `definite_article/1` e `indefinite_article/1` esistono in
             * lexicon.p0 e sono interrogati altrove (vedi la lista `classes`
             * in 10-memory-knowledge.c). Questa copia nel C e' il caso
             * peggiore dell'audit: non una conoscenza che manca alla KB, ma
             * una che c'e' e che il codice ignora. */
            int art = lex_class_member(b, "determiner_word", ot);
            if (ot && isalpha((unsigned char)ot[0]) && strlen(ot) >= 3 && !art) {
                snprintf(dst, dstn, "%s", ot);
                return 1;
            }
        }
    }
    if (dstn) dst[0] = '\0';
    return 0;
}

static int mod_pragma(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    /* work on a trimmed copy of the canonicalized surface (`norm` is the
     * canonicalized input; `raw` is the original). */
    char buf[256];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len-1]=='?'||buf[len-1]=='!'||buf[len-1]=='.'||buf[len-1]==' '||buf[len-1]==','))
        buf[--len] = '\0';
    if (len == 0) return 0;

    char tmp[256];
    memcpy(tmp, buf, len + 1);
    char *w[64];
    size_t nw = split_words(tmp, w, 64);
    if (nw == 0) return 0;

    char first[64];
    first_word_tok(w, nw, first, sizeof first);

    int content = has_content_predicate(b, buf, w, nw);

    /* The pragmatic moves claim ONLY contentless turns, so a real task is never
     * swallowed. (A discourse-marker opener wrapping content — "anyway, is
     * socrates a man" — was already peeled and re-dispatched by pragma_peel in
     * brain_respond BEFORE the registry ran, so the content was handled there;
     * what reaches here is a turn with no actionable content predicate.) */
    if (content) return 0;

    int hedge       = has_hedge(b, w, nw);
    int contrastive = has_contrastive(b, w, nw);
    int negation    = has_negation(b, w, nw);
    int disagree    = is_disagreement(b, w, nw);

    /* ---- MOVE 5: disagreement. A NEGATED stance predicate ("i don't agree",
     * "that's not right", "non sono d'accordo"), or a contrastive pushback that
     * also negates ("nice try, but no") — with no content to act on. Keys on the
     * stance predicate, so an imperative like "dont say that" is NOT disagreement. */
    if (disagree || (contrastive && negation && nw <= 8)) {
        kb_term_say(b, "fair_enough_tell_me_where_i_went_wrong_and_w", NULL, 0, out, out_size);
        return 1;
    }

    /* ---- MOVE 4: hesitation. A hedge with nothing concrete to chew on. */
    if (hedge && nw <= 9) {
        kb_term_say(b, "no_pressure_we_can_take_it_slowly_what_s_on", NULL, 0, out, out_size);
        return 1;
    }

    /* ---- MOVE 3: topic introduction / change. A modal/imperative invitation
     * ("can we talk about X", "let us switch to X", "let's discuss X") naming an
     * object X — steer onto X by name. */
    {
        /* A topic-CHANGE invitation: either a MODAL opener proposing it ("can we
         * …", "could we …", "shall we …", "let us …", "possiamo/potremmo …"
         * canonicalize to can/could) OR an explicit switch/change verb. A bare
         * "talk about X" / "parliamo di X" with no modal is left to the filler
         * register (chitchat), so we don't override the gen140 decision that a
         * casual "parliamo di formaggio" is just filler — the proposal SHAPE (a
         * modal asking permission to change topic) is the discriminating cue. */
        int modal_open = lex_class_member(b, "request_auxiliary", first) || lex_class_member(b, "request_auxiliary", first) ||
                         lex_class_member(b, "request_auxiliary", first) || lex_class_member(b, "request_auxiliary", first) ||
                         lex_class_member(b, "request_auxiliary", first);
        int switch_verb = kb_cue_match(b, "70_social_pragma_cue887", buf) || kb_cue_match(b, "70_social_pragma_cue887_2", buf) ||
                          kb_cue_match(b, "70_social_pragma_cue888", buf) || kb_cue_match(b, "70_social_pragma_cue888_2", buf);
        int frame = kb_cue_match(b, "70_social_pragma_cue889", buf) || kb_cue_match(b, "70_social_pragma_cue889_2", buf) ||
                    kb_cue_match(b, "70_social_pragma_cue890", buf) || kb_cue_match(b, "70_social_pragma_cue890_2", buf) ||
                    kb_cue_match(b, "70_social_pragma_cue891", buf) || kb_cue_match(b, "70_social_pragma_cue891_2", buf) ||
                    kb_cue_match(b, "70_social_pragma_cue892", buf) || kb_cue_match(b, "70_social_pragma_cue892_2", buf);
        int invite = switch_verb || (modal_open && frame);
        char topic[40];
        if (invite && topic_object(b, w, nw, topic, sizeof topic)) {
            user_value_write(b, "current_topic", topic);
            char msg[160];
            { const KbResponseSlot _rs[] = { { "topic", topic }, { "topic2", topic } };
      kb_term_say(b, "sure_let_s_talk_about_x_what_about_x_is_on_y", _rs, 2, msg, sizeof msg);
              put(msg, out, out_size); }
            return 1;
        }
    }

    /* ---- MOVE 2: soft request. An imperative directed at me ("tell/give/show
     * me", "say") with NO content object — an open request to fill the silence. */
    {
        int soft = lex_class_member(b, "imperative_opener", first) || lex_class_member(b, "imperative_opener", first) ||
                   lex_class_member(b, "imperative_opener", first) || lex_class_member(b, "imperative_opener", first) ||
                   lex_class_member(b, "imperative_opener", first) || lex_class_member(b, "imperative_opener", first) ||
                   lex_class_member(b, "imperative_opener", first) || lex_class_member(b, "imperative_opener", first);
        /* OPEN-ended only: the object must be a quantifier placeholder
         * ("something/anything/qualcosa"), which is what distinguishes a
         * fill-the-silence request from a real (often unfulfillable) one — "tell
         * me a story about dragons" / "tell me about C" name a CONCRETE object, so
         * they fall through to the honest wall, not this move. The bare 3-token
         * "tell me something" family stays chitchat's established no-topic
         * register, so we require >= 4 tokens here and leave those to chitchat. */
        if (soft && has_open_quantifier(b, w, nw) && nw >= 4 && nw <= 8) {
            kb_term_say(b, "happy_to_pick_a_thread_your_day_a_small_fact", NULL, 0, out, out_size);
            return 1;
        }
    }

    return 0;
}

/* --- module: symbolic ----------------------------------------------------
 * Register recognition over symbolic FORM (gen65, sym-bench driven). The
 * cryptic-stimulus challenge (`make sym-bench`) showed the LLM's first move on
 * a non-prose stimulus is to NAME its register — "a palindrome", "Morse", "a
 * code snippet", "solfège" — then engage, whereas parrot0 walled (even saying
 * "I don't know about abccba yet" of a palindrome it could see from form). This
 * module recovers that move: it CLASSIFIES the stimulus by cheap structural
 * features and names it. It decodes nothing (recognition before decoding) and
 * hardcodes no oracle wording — classifying form is honest reasoning. It works
 * on `raw` because the symbols are the signal; it is deliberately conservative
 * so plain prose is never hijacked, and sits late in the registry so genuine
 * content modules (arith, shell, knowledge, …) get the turn first. */

/* True if `s` (already lowercased) is non-trivially symmetric: equals its own
 * reverse once spaces are dropped, length >= 3, not all-identical, and — for a
 * pure-letter run — length >= 5, so 3-letter interjections ("wow", "mom") are
 * left to the phatic layer rather than called palindromes. */
static int looks_palindrome(const char *s) {
    char c[256]; size_t n = 0; int has_nonletter = 0;
    for (size_t i = 0; s[i] && n + 1 < sizeof c; i++) {
        if (isspace((unsigned char)s[i])) continue;
        if (!isalpha((unsigned char)s[i])) has_nonletter = 1;
        c[n++] = s[i];
    }
    c[n] = '\0';
    if (n < 3) return 0;
    if (!has_nonletter && n < 5) return 0;
    int all_same = 1;
    for (size_t i = 1; i < n; i++) if (c[i] != c[0]) { all_same = 0; break; }
    if (all_same) return 0;
    for (size_t i = 0, j = n - 1; i < j; i++, j--)
        if (c[i] != c[j]) return 0;
    /* gen72: short palindromes with only 2 distinct letters (e.g. "ahaha",
     * "ohoho") are conversational interjections, not deliberate palindromes.
     * Let the social module handle them instead of hijacking with "That looks
     * like a palindrome." */
    if (!has_nonletter && n < 7) {
        int distinct = 0;
        char seen[26] = {0};
        for (size_t i = 0; i < n; i++)
            seen[c[i] - 'a'] = 1;
        for (size_t i = 0; i < 26; i++)
            if (seen[i]) distinct++;
        if (distinct <= 2) return 0;
    }
    return 1;
}

/* Morse iff the trimmed input is only '.', '-', spaces, with >= 3 dot/dash. */
static int looks_morse(const char *s) {
    size_t marks = 0;
    for (size_t i = 0; s[i]; i++) {
        if (s[i] == '.' || s[i] == '-') marks++;
        else if (!isspace((unsigned char)s[i])) return 0;
    }
    return marks >= 3;
}

/* Solfège iff >= 3 space-separated tokens and every one is a note name. The
 * >= 3 floor keeps lone "do"/"la"/"mi" (English/Italian words) out. Copies its
 * argument because split_words mutates the buffer. */
static int looks_solfege(Brain *b, const char *s) {
    char buf[256]; copy_trim(buf, sizeof buf, s);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw < 3) return 0;
    for (size_t i = 0; i < nw; i++) {
        const char *q[] = { w[i] };
        if (!b || !b->kb || !kb_query(b->kb, "solfege_note", q, 1)) return 0;
    }
    return 1;
}

/* Leetspeak iff a single token (no spaces), len >= 3, mixing ascii letters with
 * leet digits (0,1,3,4,5,7), e.g. "h3ll0", "n00b". */
static int looks_leet(const char *s) {
    int letter = 0, leetdigit = 0;
    for (size_t i = 0; s[i]; i++) {
        unsigned char ch = (unsigned char)s[i];
        if (isspace(ch)) return 0;
        if (isalpha(ch)) letter = 1;
        else if (strchr("013457", (char)ch)) leetdigit = 1;
        else if (!isdigit(ch)) return 0;  /* punctuation -> not a plain leet word */
    }
    return letter && leetdigit && strlen(s) >= 3;
}

/* Code fragment iff it carries a structural code signal: a bracket/operator
 * rare in chat prose, or a code keyword opening a block ("while True:"). */
static int looks_code(Brain *b, const char *s, char **w, size_t nw) {
    /* Strong, unambiguous code markers. NOTE: a bare '(' is NOT one of these —
     * a natural-language sentence with a parenthetical aside ("Chicago (800
     * miles away)", "scatters (spreads out)") is prose, not code (gen240). */
    if (kb_cue_match(b, "70_social_pragma_cue1026", s) || kb_cue_match(b, "70_social_pragma_cue1026_2", s) || kb_cue_match(b, "70_social_pragma_cue1026_3", s) ||
        kb_cue_match(b, "70_social_pragma_cue1027", s) || kb_cue_match(b, "70_social_pragma_cue1027_2", s) || kb_cue_match(b, "70_social_pragma_cue1027_3", s))
        return 1;
    /* A '(' counts only as a function-call: an identifier char immediately
     * before it ("printf(", "foo(x)"). A space before '(' is a prose aside. */
    for (const char *p = s; (p = strchr(p, '(')) != NULL; p++) {
        if (p != s) {
            char c = p[-1];
            if (isalnum((unsigned char)c) || c == '_') return 1;
        }
    }
    /* keyword + trailing ':' (e.g. "while true:", "for x in y:") */
    size_t len = strlen(s);
    if (nw >= 1 && len > 0 && s[len - 1] == ':') {
        const char *q[] = { "python", w[0] };
        if (b && b->kb && kb_query(b->kb, "code_keyword", q, 2)) return 1;
    }
    return 0;
}

/* Name the register, but if that exact line was our previous reply (two
 * same-register stimuli in a row), use the alternate phrasing instead — the
 * same non-repetition discipline the fallback uses, so a run of cryptic inputs
 * does not feel canned. The canonical phrasing `a` is the default, so a single
 * occurrence is stable (and testable). */
static int name_register(Brain *b, const char *a, const char *alt,
                         char *out, size_t out_size) {
    const char *pick = (b && strcmp(a, b->last_reply) == 0) ? alt : a;
    put(pick, out, out_size);
    return 1;
}

/* --- module: code (gen149) -----------------------------------------------
 * Basic inline-code assistant. Handles queries about small code snippets
 * passed directly in the prompt: "what is wrong with this code", "debug this",
 * "fix this", "what language is this", "is this valid C", "explain this code".
 * Extracts the code portion, identifies the language (C vs Python), runs simple
 * syntactic checks (missing semicolons, type mismatches, unclosed strings,
 * unbalanced braces/parens, undefined functions), and reports findings.
 * Grounded in kb/experts/programming/coding.p0 — the KB is the source of truth for
 * keywords, stdlib functions, error patterns and fix suggestions. */

static int find_code_section(Brain *b, const char *input,
                             char *code, size_t code_size, InputSpan *span) {
    /* Universal path first. A tie/unclosed region is terminal (-1): the legacy
     * colon adapter must never become a hidden tiebreak. */
    if (b && b->kb) {
        int ex = code_extract_source_span(b->kb, input, code, code_size, span);
        if (ex != 0) return ex;
    }

    /* Additive fallback for shapes the KB segmenter has no supported register
     * for yet. It preserves the old code faculties while evidence coverage
     * grows, but runs only after a genuine Gap (never after ambiguity). */
    const char *p = input;
    while (*p && *p != ':') p++;
    if (*p == ':') {
        p++;
        while (*p && isspace((unsigned char)*p)) p++;
        size_t n = 0;
        while (*p && n + 1 < code_size) { code[n++] = *p; p++; }
        while (n > 0 && isspace((unsigned char)code[n - 1])) n--;
        code[n] = '\0';
        return n > 1 ? 1 : 0;
    }
    return 0;
}

/* Universal register comparison (universal-input U3/U8).  The same hypothesis
 * engine used by kb_cue_match(intent_cue, ...) now ranks register_evidence.
 * There is no first-match and no surface tiebreak: a tie is returned as -1 with
 * its proof, prose(default) wins only when no specific evidence exists. */
static int identify_register(const char *text, Brain *b,
                             char *reg, size_t regsz,
                             char *proof, size_t proofsz, int *score) {
    if (reg && regsz) reg[0] = '\0';
    if (proof && proofsz) proof[0] = '\0';
    if (score) *score = 0;
    if (!text || !*text || !b || !b->kb) return 0;
    int r = kb_hypothesis_best(b->kb, "register_evidence", text, NULL, 0,
                               reg, regsz, score, proof, proofsz);
    if (r == 1 && reg && lex_class_member(b, "70_social_pragma_lex1109", reg)) {
        if (regsz) reg[0] = '\0';
        return 0;
    }
    return r;
}

/* Legacy front-end adapter: downstream AST/checker code still uses 1/2 while
 * register perception itself is open and KB-driven. */
static int identify_code_lang(const char *code, Brain *b) {
    char reg[KB_TERM_LEN], proof[KB_EVIDENCE_PROOF_LEN]; int score = 0;
    if (identify_register(code, b, reg, sizeof reg,
                          proof, sizeof proof, &score) != 1) return 0;
    if (lex_class_member(b, "70_social_pragma_lex1122", reg)) return 1;
    if (lex_class_member(b, "70_social_pragma_lex1123", reg)) return 2;
    return 0;
}

static int check_missing_semicolons(Brain *b, const char *code, char *findings,
                                     size_t findings_size) {
    int issues = 0;
    /* Check each physical line: if it looks like a C statement but doesn't
     * end with ; or {, flag it. Also check statements before closing braces. */
    char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
    char *lines[64]; int nl = 0;
    char *save = NULL;
    char *tok = strtok_r(buf, "\n", &save);
    while (tok && nl < 64) { lines[nl++] = tok; tok = strtok_r(NULL, "\n", &save); }
    if (nl == 0) {
        char cpy[1024]; snprintf(cpy, sizeof cpy, "%s", code);
        lines[0] = cpy; nl = 1;
    }
    for (int i = 0; i < nl; i++) {
        char *l = lines[i]; while (*l && isspace((unsigned char)*l)) l++;
        if (!*l || l[0] == '#') continue;
        size_t len = strlen(l);
        while (len > 0 && isspace((unsigned char)l[len-1])) l[--len] = '\0';
        if (len == 0) continue;
        if (l[len-1] == ';' || l[len-1] == '{') continue;
        /* Extract first word */
        char fw[64] = {0};
        { const char *p = l; while (*p && isspace((unsigned char)*p)) p++;
          size_t fwl = 0;
          while (*p && !isspace((unsigned char)*p) && *p != '(' && fwl < 63)
              fw[fwl++] = (char)tolower((unsigned char)*p++);
          fw[fwl] = '\0'; }
        const char *qkw[] = { "c", fw };
        int is_kw = b && b->kb && kb_query(b->kb, "code_keyword", qkw, 2);
        if (is_kw && l[len-1] != '}') issues++;
        /* Check for statement before closing brace: scan for "keyword ... }"
         * e.g. "int main() { return 0 }" — "return 0 }" has no semicolon.
         *
         * gen322: this fabricated. It compared the character IMMEDIATELY before
         * the '}' against ';' — but in a one-line function (the only shape the
         * line-based chat surface can receive) that character is a SPACE:
         *
         *   "int f(void) { return 0; }"  -> char before '}' is ' ' -> "missing ;"
         *   "int f(void) { return 0;}"   -> char before '}' is ';' -> silent
         *
         * So every correct one-line function was reported broken, and the truly
         * broken one passed only by accident. Step back over the whitespace and
         * test the last REAL character of the statement. */
        if (l[len-1] == '}' && strchr(l, '{')) {
            const char *p = l;
            char return_pattern[KB_TERM_LEN];
            const char *rpq[] = { "c", "return_statement", NULL };
            char rprow[1][KB_TERM_LEN];
            size_t rpn = b && b->kb ? kb_match(b->kb, "code_pattern", rpq, 3,
                                                rprow, 1) : 0;
            if (rpn == 1) snprintf(return_pattern, sizeof return_pattern, "%s",
                                   kb_dequote(rprow[0]));
            else return_pattern[0] = '\0';
            size_t return_pattern_len = strlen(return_pattern);
            while (return_pattern_len > 0 &&
                   (p = strstr(p, return_pattern)) != NULL) {
                const char *q = p + return_pattern_len;
                while (*q && isspace((unsigned char)*q)) q++;
                const char *end = strchr(q, '}');      /* next } or end of line */
                if (!end) end = q + strlen(q);
                while (end > q && isspace((unsigned char)end[-1])) end--;
                if (end > q && end[-1] != ';') { issues++; break; }
                p++;
            }
        }
    }
    if (issues == 1)
        {   const KbResponseSlot _rs[] = { { "x", "" } };
          kb_term_say(b, "missing_semicolon_at_the_end_of_a_statement", _rs, 0, findings, findings_size); }
    else if (issues > 1)
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%d", issues);
  const KbResponseSlot _rs[] = { { "issues", _v0 } };
          kb_term_say(b, "missing_semicolons_at_the_end_of_x_statement", _rs, 1, findings, findings_size); }
    return issues;
}

static int check_type_mismatch(Brain *b, const char *code, char *findings,
                                size_t findings_size) {
    /* Simple patterns: "int x = \"...\"" (string assigned to int)
     *                 "char y = 42" (number assigned to char pointer) */
    char int_pattern[KB_TERM_LEN], char_pointer_pattern[KB_TERM_LEN];
    const char *ipq[] = { "c", "int_assignment", NULL };
    const char *cpq[] = { "c", "char_pointer_assignment", NULL };
    char iprow[1][KB_TERM_LEN], cprow[1][KB_TERM_LEN];
    size_t nip = b && b->kb ? kb_match(b->kb, "code_pattern", ipq, 3,
                                        iprow, 1) : 0;
    size_t ncp = b && b->kb ? kb_match(b->kb, "code_pattern", cpq, 3,
                                        cprow, 1) : 0;
    if (nip == 1) snprintf(int_pattern, sizeof int_pattern, "%s", kb_dequote(iprow[0]));
    else int_pattern[0] = '\0';
    if (ncp == 1) snprintf(char_pointer_pattern, sizeof char_pointer_pattern, "%s",
                           kb_dequote(cprow[0]));
    else char_pointer_pattern[0] = '\0';
    if (int_pattern[0] && strstr(code, int_pattern) && strstr(code, "= \"") && strstr(code, "\"")) {
        {   const KbResponseSlot _rs[] = { { "x", "" } };
          kb_term_say(b, "type_mismatch_a_string_is_assigned_to_an_int", _rs, 0, findings, findings_size); }
        return 1;
    }
    /* char *x = number (without quotes) */
    { const char *cp = code;
      while (char_pointer_pattern[0] &&
             (cp = strstr(cp, char_pointer_pattern)) != NULL) {
          const char *eq = strstr(cp, "=");
          if (eq) {
              const char *v = eq + 1;
              while (*v && isspace((unsigned char)*v)) v++;
              if (*v == '\"' || isalpha((unsigned char)*v)) {
                  int has_digit = 0;
                  for (const char *d = v; *d && *d != ';' && *d != '\n'; d++)
                      if (isdigit((unsigned char)*d)) { has_digit = 1; break; }
                  if (has_digit && !strstr(v, "\"")) {
                      {   const KbResponseSlot _rs[] = { { "x", "" } };
                        kb_term_say(b, "suspicious_assignment_a_number_assigned_to_a", _rs, 0, findings, findings_size); }
                      return 1;
                  }
              }
          }
          cp++;
      }
    }
    return 0;
}

static int check_unclosed_string(Brain *b, const char *code, char *findings,
                                  size_t findings_size) {
    int quotes = 0;
    for (const char *p = code; *p; p++)
        if (*p == '\"' && (p == code || *(p-1) != '\\')) quotes++;
    if (quotes % 2 != 0) {
        {   const KbResponseSlot _rs[] = { { "x", "" } };
          kb_term_say(b, "unclosed_string_literal_there_is_an_odd_numb", _rs, 0, findings, findings_size); }
        return 1;
    }
    return 0;
}

static int check_balanced_braces(Brain *b, const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '{') open++;
        if (*p == '}') open--;
    }
    if (open > 0) {
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%d", open);
  const KbResponseSlot _rs[] = { { "open", _v0 } };
          kb_term_say(b, "unbalanced_braces_x_more_opening_brace_s_tha", _rs, 1, findings, findings_size); }
        return 1;
    }
    if (open < 0) {
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%d", -open);
  const KbResponseSlot _rs[] = { { "open", _v0 } };
          kb_term_say(b, "unbalanced_braces_x_more_closing_brace_s_tha", _rs, 1, findings, findings_size); }
        return 1;
    }
    return 0;
}

static int check_balanced_parens(Brain *b, const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '(') open++;
        if (*p == ')') open--;
    }
    if (open != 0) {
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%d", open > 0 ? open : -open);
          char _v1[48]; snprintf(_v1, sizeof _v1, "%s", open > 0 ? "opening" : "closing");
          char _v2[48]; snprintf(_v2, sizeof _v2, "%s", open > 0 ? "closing" : "opening");
  const KbResponseSlot _rs[] = { { "open", _v0 }, { "closing", _v1 }, { "opening", _v2 } };
          kb_term_say(b, "unbalanced_parentheses_x_more_x_than_x", _rs, 3, findings, findings_size); }
        return 1;
    }
    return 0;
}

static int check_unknown_function(const char *code, Brain *b, char *findings,
                                   size_t findings_size) {
    if (!b || !b->kb) return 0;
    char buf[1024]; snprintf(buf, sizeof buf, "%s", code);
    char *w[128]; size_t nw = split_words(buf, w, 128);
    for (size_t i = 0; i < nw; i++) {
        char *t = w[i]; size_t tl = strlen(t);
        /* Look for word followed by ( */
        if (i + 1 < nw && w[i+1][0] == '(' && tl > 1 && isalpha((unsigned char)t[0])) {
            char fname[64]; size_t fn = 0;
            for (size_t j = 0; j < tl && fn < sizeof(fname)-1; j++)
                fname[fn++] = (char)tolower((unsigned char)t[j]);
            fname[fn] = '\0';
            /* Skip known keywords */
            if (lex_class_member(b, "c_keyword", fname) || lex_class_member(b, "c_keyword", fname) ||
                lex_class_member(b, "preposition", fname) || lex_class_member(b, "c_keyword", fname) ||
                lex_class_member(b, "c_keyword", fname) || lex_class_member(b, "c_keyword", fname))
                continue;
            /* Check against KB */
            { const char *fa[] = { fname };
            if (!kb_query(b->kb, "c_stdlib", fa, 1)) {
                size_t fl = strlen(t);
                if (fl < sizeof(fname)) {
                    { char _t1[512];
                    char _t1_v0[96]; snprintf(_t1_v0, sizeof _t1_v0, "%.*s", (int)fl, t);
                    const KbResponseSlot _r1[] = { { "t", _t1_v0 } };
                    kb_term_say(b, "unknown_function_x_is_not_a_standard_c_libra", _r1, 1, _t1, sizeof _t1);
                    snprintf(findings, findings_size, "%s", _t1);
                    }
                    return 1;
                }
            }
            }
        }
    }
    return 0;
}

static void register_name(Brain *b, const char *reg, char *out, size_t out_size) {
    if (!out || out_size == 0) return;
    if (!reg || !*reg) { snprintf(out, out_size, "unknown"); return; }
    if (b && b->kb) {
        char labels[1][KB_TERM_LEN]; const char *q[2] = { reg, NULL };
        if (kb_match(b->kb, "register_label", q, 2, labels, 1) == 1) {
            char *p = labels[0]; size_t n = strlen(p);
            if (n >= 2 && p[0] == '"' && p[n - 1] == '"') {
                p[n - 1] = '\0'; p++;
            }
            snprintf(out, out_size, "%s", p);
            return;
        }
    }
    snprintf(out, out_size, "%s", reg);
}
