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
        /* gen425 — SI CEDE AL MORSE SOLO SE IL MORSE LO PRENDE.
         *
         * «.», «-» e «/» sono l'alfabeto del morse, quindi questo ramo cedeva
         * loro il turno — ma il riconoscitore simbolico ne vuole almeno tre, e
         * uno o due simboli cadevano nel vuoto FRA I DUE, fino al muro cieco.
         * Erano esattamente i tre caratteri che la classe 1 delle classi
         * misurate segnalava da giorni, mentre gli altri ventinove ricevevano la
         * risposta giusta.
         *
         * Quanti simboli servano perche' un morse sia un morse e' conoscenza. */
        long morse_min = 3;
        if (b && b->kb) {
            const char *mq[1] = { NULL };
            char mv[1][KB_TERM_LEN];
            if (kb_match(b->kb, "morse_min_symbols", mq, 1, mv, 1) > 0) {
                char mb[KB_TERM_LEN]; snprintf(mb, sizeof mb, "%s", mv[0]);
                long v = strtol(kb_dequote(mb), NULL, 10);
                if (v > 0) morse_min = v;
            }
        }
        /* gen427 — E CON ALMENO DUE SIMBOLI DIVERSI. «.....» ha la lunghezza di
         * un morse ma non ne ha la forma: un morse e' un'alternanza, una fila
         * di cinque punti e' un'ellissi. Anche quanti simboli DISTINTI servano
         * e' conoscenza, come la loro quantita' minima. */
        long morse_distinct = 1;
        if (b && b->kb) {
            const char *dq[1] = { NULL };
            char dv[1][KB_TERM_LEN];
            if (kb_match(b->kb, "morse_min_distinct_symbols", dq, 1, dv, 1) > 0) {
                char db[KB_TERM_LEN]; snprintf(db, sizeof db, "%s", dv[0]);
                long v = strtol(kb_dequote(db), NULL, 10);
                if (v > 0) morse_distinct = v;
            }
        }
        long seen_dot = 0, seen_dash = 0, seen_slash = 0;
        for (size_t i = 0; i < len; i++) {
            if (norm[i] == '.') seen_dot = 1;
            else if (norm[i] == '-') seen_dash = 1;
            else if (norm[i] == '/') seen_slash = 1;
        }
        long ndistinct = seen_dot + seen_dash + seen_slash;
        if (morse_only && (long)len >= morse_min && ndistinct >= morse_distinct)
            return 0;                                    /* lo nomina il simbolico */
        kb_term_say(b, "punctuation_only", NULL, 0, out, out_size);
        return 1;
    }

    /* (0) UN LETTERALE E BASTA — e QUALI forme siano letterali e' conoscenza.
     *
     * gen427 — «-12.5», «100.5», «14:30», «9:15», «$1000» finivano tutti nel
     * muro cieco, e il ramo (2) qui sotto ne prendeva solo una parte: sapeva
     * leggere le cifre e il segno, e nient'altro. La differenza fra «100» e
     * «100.5» non e' una capacita' in piu': e' UNA RIGA DI FORMA in piu', e
     * stava in C.
     *
     * Ora la forma sta in `kb/core/literals.p0`, scritta sui CARATTERI
     * (`chars/2`), e il motore fa tre cose sole: passa la superficie alla KB,
     * riceve il GENERE, e dice la frase che il genere dichiara. Una notazione
     * nuova — un'altra valuta, un altro formato di orario, una percentuale —
     * costa una riga di KB e nessuna ricompilazione.
     *
     * Il ramo (2) resta sotto come struttura secondaria: se la KB dei letterali
     * non c'e', i numeri interi si nominano lo stesso. */
    if (b && b->kb) {
        /* La superficie si prova come e' stata SCRITTA e come e' stata
         * canonicalizzata: «XVIII» e' un numerale romano solo in maiuscolo (in
         * minuscolo «mix» e «did» sarebbero numerali anche loro), e la forma
         * canonica perde le maiuscole. Due tentativi, nessuna conoscenza in C. */
        /* Un letterale e' CORTO per natura, e la domanda non si fa su una frase:
         * guardare i caratteri di un turno lungo costa e non puo' concludere
         * niente. Quanto corto e' un fatto (`lone_literal_max_length/1`). */
        long lit_max = 24;
        {
            const char *mq[1] = { NULL };
            char mv[1][KB_TERM_LEN];
            if (kb_match(b->kb, "lone_literal_max_length", mq, 1, mv, 1) > 0) {
                char mb[KB_TERM_LEN]; snprintf(mb, sizeof mb, "%s", mv[0]);
                long v = strtol(kb_dequote(mb), NULL, 10);
                if (v > 0) lit_max = v;
            }
        }
        /* IL GREZZO SI GUARDA QUANDO E' DAVVERO QUESTO TURNO.
         *
         * La forma canonica perde informazione che alla FORMA serve: le
         * maiuscole («XVIII» diventa «xviii», e in minuscolo «mix» sarebbe un
         * numerale romano) e i simboli che la canonicalizzazione scioglie in
         * parole («50%» diventa «50 percent»). Un letterale va letto come e'
         * stato scritto.
         *
         * Ma il grezzo non e' sempre di questo turno: il ricovero della
         * riparazione ridispaccia «what is 21 plus 10» tenendo il grezzo «21»
         * del turno che ha riempito lo slot, e leggerlo faceva rispondere «e' il
         * numero 21» a una domanda che ormai era un'addizione (misurato:
         * repair.p0t). La prova che distingue i due casi e' esatta e non e' una
         * regola di pollice: SE IL GREZZO SI CANONICALIZZA IN QUESTO NORM,
         * allora e' questo turno scritto a modo suo; altrimenti e' un altro
         * turno e non va guardato. */
        const char *cased = NULL;
        if (raw && *raw && norm && (long)strlen(raw) <= lit_max) {
            if (strcasecmp(raw, norm) == 0) cased = raw;
            else {
                char cn[256];
                brain_canonical(b, raw, cn, sizeof cn);
                if (strcmp(cn, norm) == 0) cased = raw;
            }
        }
        const char *surf[2] = { cased, norm };
        for (size_t si = 0; si < 2; si++) {
            if (!surf[si] || !*surf[si]) continue;
            if ((long)strlen(surf[si]) > lit_max) continue;
            if (si == 1 && surf[0] && strcmp(surf[0], surf[1]) == 0) continue;
            char q[KB_TERM_LEN];
            if (snprintf(q, sizeof q, "\"%s\"", surf[si]) >= (int)sizeof q) continue;
            const char *lq[2] = { q, NULL };
            char kind[1][KB_TERM_LEN];
            if (kb_match(b->kb, "lone_literal_kind", lq, 2, kind, 1) != 1) continue;
            const char *sq[2] = { kind[0], NULL };
            char tmpl[1][KB_TERM_LEN];
            if (kb_match(b->kb, "lone_literal_say", sq, 2, tmpl, 1) != 1) continue;
            char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", tmpl[0]);
            /* Alcuni generi si NOMINANO e basta (un numero, un orario); altri si
             * TRADUCONO — un numerale romano vale un numero, una misura si dice
             * per esteso. Se la KB sa renderla, si dice la resa. */
            char shown[KB_TERM_LEN];
            snprintf(shown, sizeof shown, "%s", surf[si]);
            const char *rq[3] = { kind[0], q, NULL };
            char rendered[1][KB_TERM_LEN];
            if (kb_match(b->kb, "lone_literal_render", rq, 3, rendered, 1) == 1) {
                char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rendered[0]);
                snprintf(shown, sizeof shown, "%s", kb_dequote(rb));
            }
            /* La superficie che si RIDICE e' quella che l'utente ha scritto: se
             * ha scritto «50PERC» e la forma ha combaciato sulla canonica
             * «50perc», rispondergli «50perc» sarebbe rinominare quello che ha
             * detto — lo stesso difetto di «u» letto «you». */
            const char *say_surface = cased ? cased : surf[si];
            const KbResponseSlot sl[] = { { "shown", shown }, { "surface", say_surface } };
            char msg[400];
            if (kb_response_slots(b, kb_dequote(tb), sl, 2, msg, sizeof msg)) {
                put(msg, out, out_size);
                return 1;
            }
        }
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

    /* (2) UN NUMERO NUDO, senza operatori: non c'e' niente da calcolare, e la
     * risposta onesta e' dirlo nominandolo.
     *
     * gen425 — LA SOGLIA ERA CABLATA A QUATTRO CIFRE, con un commento che la
     * giustificava «per moduli futuri» che non sono mai arrivati. L'effetto,
     * misurato dalle classi misurate:
     *
     *     5     ->  I don't understand that yet.
     *     42    ->  I don't understand that yet.
     *     123   ->  I don't understand that yet.
     *     1234  ->  That's just the number 1234 with nothing to do — …
     *
     * Un solo numero cablato spiegava i fallimenti di tre classi: le dieci cifre
     * della classe 1, i numeri della 2 e quelli della 3. Ora la soglia e' un
     * fatto (`bare_number_min_digits/1`) e vale UNO: ogni numero nudo si nomina.
     *
     * La precedenza fra moduli e' il modo giusto di riservarsi i numeri corti —
     * un modulo che li vuole si registra prima — e una soglia di lunghezza non
     * lo era: negava a tutti per riservare a nessuno.
     *
     * Il segno fa parte del numero: «-100» e' un numero quanto «100», e
     * lasciarlo fuori era la stessa svista in piccolo. */
    /* IL SEGNO E' PARTE DEL NUMERO, e va anche RIDETTO: `strip_edge_punct` lo
     * toglie, e la prima stesura rispondeva «the number 100» a «-100», cioe'
     * rinominava il numero mentre lo nominava. Si guarda il token com'era. */
    const char *shown = tok;
    if ((w[0][0] == '-' || w[0][0] == '+') && isdigit((unsigned char)w[0][1]))
        shown = w[0];
    const char *tnum = tok;
    if ((*tnum == '-' || *tnum == '+') && tnum[1]) tnum++;
    int all_digit = *tnum != '\0';
    for (const char *p = tnum; *p; p++)
        if (!isdigit((unsigned char)*p)) { all_digit = 0; break; }
    size_t ndig = strlen(tnum);
    long need = 1;
    if (b && b->kb) {
        const char *mq[1] = { NULL };
        char mv[1][KB_TERM_LEN];
        if (kb_match(b->kb, "bare_number_min_digits", mq, 1, mv, 1) > 0) {
            char mb[KB_TERM_LEN]; snprintf(mb, sizeof mb, "%s", mv[0]);
            long v = strtol(kb_dequote(mb), NULL, 10);
            if (v > 0) need = v;
        }
    }
    if (all_digit && (long)ndig >= need) {
        char msg[160];
        {   const KbResponseSlot _rs[] = { { "shown", shown } };
          kb_term_say(b, "that_s_just_the_number_x_with_nothing_to_do", _rs, 1, msg, sizeof msg); }
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
                kb_term_say(b, "that_doesn_t_look_like_words_to_me_did_a_key", NULL, 0, out, out_size);
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
    /* gen335: FIRST — but gated strictly on the loop signal (input echoes parrot0's own
     * last reply), so it preempts a mirror loop with a fresh KB topic and passes every
     * normal turn straight through. */
    {"initiative", mod_initiative},
    {"piact",     mod_piact},
    {"compose",   mod_compose},
    {"repair",    mod_repair},
    /* M2: la menzione corre PRIMA del lettore universale. Misurato in italiano:
     * «il termine sebbene è un marcatore» veniva letto come appartenenza con
     * soggetto `termine_sebbene`, cioe' la parola di cui si parla finiva
     * inglobata nel sintagma che la nomina. Chi dichiara di menzionare una
     * parola deve poterlo fare prima che la frase venga interpretata. */
    {"mention",   mod_mention},
    {"input",     mod_input},
    /* gen420: la MOSSA prima del contenuto. «forget that my name is franco»
     * contiene un'asserzione che piu' moduli sanno leggere, e ognuno se la
     * prendeva a turno; registrarsi qui e' l'unico modo di non inseguirli. */
    {"forget",    mod_forget},
    /* gen335+: moved earlier so riddle/story handlers claim before coref (174). */
    /* gen382c: l'atto di INSEGNARE una risposta corre prima del motore che le
     * consuma — altrimenti "quando ti chiedo di X rispondi Y" verrebbe letto
     * come una domanda su X invece che come l'istruzione che e'. */
    /* gen382h: insegnare una REGOLA con variabili corre prima di chi legge le
     * frasi come domande — "if someone is a X then they are a Y" ha la forma di
     * un condizionale e il contenuto di una quantificazione. */
    {"teachconstruction", mod_teach_construction},
    /* M1: cio' che una LEZIONE ha reso leggibile si legge prima che un modulo
     * generico risponda. Vale solo per i pattern nati da una lezione. */
    {"taughtframe", mod_taught_frame},
    {"teachrule",  mod_teach_rule},
    {"teachreply", mod_teach_reply},
    /* SC2-B: una domanda su cio' che un DOCUMENTO riporta corre prima dei
     * motori generici, che la leggevano come una domanda sul mondo e finivano
     * a muro sul primo sostantivo. Il cancello e' stretto — servono claim
     * osservate e una evidenza derivata da una cue viva — quindi un turno
     * ordinario passa oltre. Dopo gli atti didattici, perche' insegnare o
     * ritrattare una locuzione contiene la locuzione stessa. */
    {"claimq",    mod_claim_question},
    {"qa",        mod_qa},
    {"gen",       mod_gen},
    /* deep-reasoning M3: the budgeted multi-hop inference loop. Fires only on the
     * "think deeply" trigger (deep_reason_fresh), so it claims a deep-reason turn
     * before mod_knowledge sees the embedded question, and passes everything else. */
    {"deepreason", mod_deep_reason},
    {"count",     mod_count},
    {"namestart", mod_namestart},
    {"wordquery", mod_wordquery},
    {"sequence",  mod_sequence},
    {"spell",     mod_spell},
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
    {"archetype", mod_archetype},
    {"fewshot",   mod_fewshot},
    {"compare",   mod_compare},
    /* gen265: BEFORE the numeric modules — a rules spec is full of numbers and
     * sums, and arith/wordproblem would claim it ("6.") ahead of recognition.
     * The gate is strict (>=2 ordered rule markers + a KB request cue + length),
     * so genuine arithmetic never reaches it. */
    {"rulespec",  mod_rulespec},
    {"algebra",   mod_algebra},
    /* gen423: l'operatore DICHIARATO corre prima del calcolo cablato. Non lo
     * sostituisce — mod_arith resta per tutto il resto — ma la forma semplice
     * «A op B» passa dalla KB, cosi' un operatore nuovo costa due righe di
     * conoscenza e zero C. */
    /* gen427: VERIFICARE prima di CALCOLARE. «2+2=4» contiene un «+», e chi
     * calcola e basta risponde «4» a chi ha gia' scritto 4 — cioe' non risponde
     * alla domanda posta, che era «e' vero?». */
    {"claim",     mod_claim},
    {"operator",  mod_operator},
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
    {"coref",     mod_coref},
    {"bench",     mod_bench},
    {"reader",    mod_reader},
    {"shell",     mod_shell},
    /* A closed code register plus a code question is stronger evidence than a
     * generic lexical answer frame. codeast has its own structural gate and
     * declines ordinary prose, so let it avoid an exhaustive relational scan
     * of mixed `statements; what is X` turns. */
    {"codeast",   mod_codeast},
    /* teach-comprehension: answer a question about a TAUGHT binary relation via a
     * teachable answer_frame(Cue, Pred) — before mod_knowledge, which would else
     * mis-claim the turn ("Hmm, I don't know about <first word>"). */
    {"answerframe", mod_answer_frame},
    /* gen309: superlative aggregation over a relation via a teachable
     * aggregate_frame(Cue, Pred, ReturnArg, Mode) — a fold (group+count+extremum),
     * before mod_knowledge which would else mis-claim "which river ...?". */
    {"aggregate", mod_aggregate},
    /* gen313: code modules must claim code queries before mod_knowledge's
     * describe_cue handler intercepts "define"/"defines" turns. All three
     * code entry points have tight gates (code-keyword + structure pattern)
     * so they never claim non-code queries. */
    {"code",      mod_code},
    {"knowledge", mod_knowledge},
    {"symbolic",  mod_symbolic},
    {"summary",   mod_summary},
    {"discourse", mod_discourse},
    {"pragma",    mod_pragma},
    {"family",    mod_family},
    {"personal",  mod_personal},   /* gen335: user self-facts; AFTER family so "my father
                                    * works in a bank" stays a family statement, not a job */
    /* gen426: un token solo si classifica PRIMA del sociale. La «mossa per
     * eliminazione» di mod_social — una parola sola al primo turno e' contatto
     * fatico — acchiappava «what», «dog» e «qzxv» insieme ai saluti veri, e le
     * classi misurate mostravano sei turni diversi con la stessa risposta E la
     * stessa firma. I saluti restano a lui: mod_lone li lascia passare. */
    {"lone",      mod_lone},
    {"social",    mod_social},
    {"chitchat",  mod_chitchat},
    {"reqgen",    mod_reqgen},
    {"learn",     mod_learn},
    /* Dopo `learn` di proposito: «learn "…" to ask what kind of gap» CONTIENE la
     * cue del referto, e messo prima il modulo si mangiava la frase che lo stava
     * insegnando. Resta comunque prima di `smalltalk`, che era chi rubava la
     * domanda quando il referto non esisteva. */
    {"gapreport", mod_gapreport},
    /* gen331: after every faculty that could genuinely serve the turn, and before
     * the conversational fallback — so a tool request in a tools-off mode hears
     * "I understood, I am not permitted" instead of "I don't understand". */
    {"toolpolicy", mod_toolpolicy},
    {"smalltalk", mod_smalltalk},   /* gen240: last-resort conversational continuity */
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
/* gen382s: la stessa coppia normalize+canonicalize_lang che ogni turno subisce
 * prima del dispatch, esposta perche' sia ispezionabile. Vedi brain.h. */
size_t brain_canonical(Brain *b, const char *input, char *out, size_t out_size) {
    if (!b || !input || !out || out_size == 0) return 0;
    char norm[256];
    normalize(input, norm, sizeof norm);
    /* La lingua del turno si rileva PRIMA di canonicalizzare, esattamente come in
     * brain_respond: le parole funzione sono scoped per lingua, quindi ispezionare
     * senza rilevare mostrerebbe una forma che il turno vero non produce mai. */
    detect_set_language(b, norm);
    canonicalize_lang(b, norm, out, out_size);
    return strlen(out);
}

static int dispatch_one(Brain *b, const char *clause, char *out, size_t out_size) {
    if (!b || !clause || !*clause || out_size == 0) return 0;
    char norm[256];
    normalize(clause, norm, sizeof norm);
    char canon[256];
    canonicalize_lang(b, norm, canon, sizeof canon);
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
    not_understood(b, canon, raw, out, out_size);
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
    if (!is_discourse_opener(b, w, nw, &skip) || skip >= nw) return 0;

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
static int is_negation_marker(Brain *b, const char *w);

static int correction_peel(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size) {
    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    /* split_words keeps a trailing comma on the token ("no,"), so match on the
     * punctuation-stripped marker. */
    if (nw < 4 || !lex_class_member(b, "99_registry_lex586", strip_edge_punct(w[0]))) return 0;
    int has_neg = 0;
    for (size_t i = 1; i < nw; i++)
        if (is_negation_marker(b, w[i])) { has_neg = 1; break; }
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

static void dead_rules_publish(Brain *b);   /* gen408: §4d, definita piu' avanti */

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
        not_understood(b, canon, raw, out, out_size);
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
    b->agent_store = p0a_new();
    if (!b->agent_store) { kb_destroy(b->kb); free(b); return NULL; }
    b->start_time = 0; /* gen251: conversation time starts on first user turn. */
    b->active_world = -1; /* gen142 (E7): no local world is open at birth */

    /* Curated lexical knowledge used by the kernel itself. It lives in the
     * knowledge layer, not as C word arrays; loading it as base keeps it out of
     * session saves while tests stay independent of world knowledge files. */
    const char *lexicon = p0env("PARROT0_LEXICON");
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

    /* gen286 (U5, teach-comprehension §5.5): the grammatical glue (Secchio B)
     * starts migrating from C to declarative facts. First rule: the Italian
     * article form is an article/4 fact table mod_translate now queries, so the
     * grammar is inspectable/teachable knowledge instead of a C ternary. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/grammar.p0");
    /* gen382d: le risposte dei moduli. Il letterale in C resta il default e
     * questo file ha l'ultima parola, quindi ogni messaggio e' sovrascrivibile
     * a runtime invece che murato nel motore. */
    kb_load(b->kb, "kb/core/messages.p0");

    /* gen211 (cardinal KB-first principle): multi-word INTENT phrases — the exact
     * surface forms that mean "ask my name" etc. — live in kb/core/intents.p0, not in
     * C arrays. kb_intent_match() queries intent_phrase/2; a form taught at runtime
     * grows the class with no code edit (see kb/core/intents.p0, PRINCIPLES.md). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/intents.p0");

    /* Universal input perception (docs/plans/universal-input.md): register
     * evidence, delimiter/indent closure, open segment roles, provenance and
     * faculty routing are knowledge.  The fixed C engine measures bytes and
     * compares hypotheses; teaching a new cue/register changes this live table
     * without rebuilding the binary. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/input.p0");

    /* gen212 (cardinal KB-first principle, OUTPUT side): the agent's own reply
     * phrasings — "Nice to meet you, {name}!" etc. — live in kb/core/responses.p0, not
     * as C literals. kb_response() fills response_template/2; a phrasing taught at
     * runtime grows the class with no code edit (see PRINCIPLES.md). */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/responses.p0");

    /* gen325 (TODO.md P6, forge plan §18): the capability LEDGER as knowledge —
     * capability(Id, Maturity) and capability_wall(Id, "…"), projected by
     * tests/capability_facts.py from tests/bench/benchmarks.json (whose claims
     * `make capability-report` verifies against real gate results). This is what
     * lets mod_self answer where the envelope ENDS instead of listing modules.
     * GENERATED: never hand-edit kb/core/capabilities.p0.
     *
     * Tagged KB_REFLECTIVE, like module(…) and i_am(…): this is the agent's model
     * OF ITSELF, not knowledge about the world. It is regenerated every boot and
     * never persisted (DESIGN.md D3). Loading it as KB_BASE made a hermetic brain
     * report "I know 24 fact(s)" when it should know none — the self-model would
     * have masqueraded as world knowledge, which is exactly the pollution gen275
     * fixed for dispatch vocabulary. */
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_load(b->kb, "kb/core/capabilities.p0");

    /* gen215 (docs/plans/the-linguistic-glue.md, G0): curated glue_role/2 — which
     * faculties carry cross-turn coherence and what each contributes. Reified below into
     * glue_faculty/2 only for modules that really exist, so the self-model can't drift. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/glue.p0");

    /* gen335 (KB-first morphology): plural→singular facts for the research
     * pipeline. The engine queries singular/2 before Wikipedia/file lookup so
     * "cavalli" resolves to "cavallo". A new plural form is a fact, not a C branch. */
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/morphology.p0");

    /* gen335 (F.): the PRESENTATION layer — how a datum is rendered in the output
     * surface is knowledge, not C. present_rule/1 + proper_name/1 drive present_atom
     * (10-memory-knowledge.c). Loaded after morphology because it reuses proper_name/1. */
    kb_load(b->kb, "kb/core/presentation.p0");

    /* gen335 (F.): computational PROCEDURES as knowledge — fold/filter/arithmetic
     * relations over the engine's is/2 + comparison primitives (kb.c). "How to sum /
     * ignore odds / solve a relation" is KB, not a C consumer. See teachable-procedures.
     * (The higher-order world knowledge from the mesh was scattered into world-facts.p0
     * and social.p0 so the save-map routes future growth next to its kin.) */
    kb_load(b->kb, "kb/core/procedures.p0");

    /* gen335 (long-conversation): personal-fact capture/recall knowledge — factored
     * slot_evidence/2 (scored by the shared hypothesis engine) + EN/IT reply templates.
     * Drives mod_personal (10-memory-knowledge.c). A new slot or language is facts. */
    kb_load(b->kb, "kb/core/personal.p0");

    /* gen335 (long-conversation): conversational INITIATIVE seeds — real facts parrot0
     * can bring up to lead a stalled/looping exchange (mod_initiative). */
    kb_load(b->kb, "kb/core/initiative.p0");

    /* gen230/gen235: curated world commons. Tests that must prove dynamic
     * learning from an empty world can set PARROT0_WORLD_FACTS=0; llmscore and
     * ordinary chat keep the layer loaded. */
    if (!p0env("PARROT0_WORLD_FACTS") || strcmp(p0env("PARROT0_WORLD_FACTS"), "0") != 0) {
        kb_set_origin(b->kb, KB_BASE);
        kb_load(b->kb, "kb/core/world-facts.p0");
    }

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
    /* gen240 (universal-comprehension): the running process itself is session
     * context. The PID and the OS user's language (from the launching ENV locale)
     * become session facts — so they are queryable, and the OS locale SEEDS the
     * default reply language: a lone ambiguous "ciao" greets in Italian on an
     * Italian machine, English elsewhere. Per-turn detection still overrides. */
    {
        /* gen411 (F.): RIFLESSIVO, non sessione. Il pid, la lingua del sistema e
         * la lingua corrente sono il modello del GIRO IN CORSO, e KB_REFLECTIVE
         * significa esattamente questo — «il modello di se', mai persistito».
         * Sotto KB_SESSION finivano nel file di ricaduta a ogni `/save`:
         * `process_pid(445600).` scritto nell'albero curato come se fosse
         * conoscenza. Non e' un filtro aggiunto al salvataggio, e' l'origine
         * giusta nel punto dove il fatto nasce. */
        kb_set_origin(b->kb, KB_REFLECTIVE);
        /* gen346: the PID is a system read too, so it is pilotable — PARROT0_PID
         * overrides getpid() (a test can freeze it for a deterministic reply). */
        char pid[24];
        const char *pid_over = p0env("PARROT0_PID");
        if (pid_over && *pid_over) snprintf(pid, sizeof pid, "%s", pid_over);
        else snprintf(pid, sizeof pid, "%ld", (long)getpid());
        const char *pa[] = { pid }; kb_assert(b->kb, "process_pid", pa, 1);
        const char *loc = p0env("PARROT0_LANG");
        if (!loc || !*loc) loc = p0env("LANG");
        if (!loc || !*loc) loc = p0env("LC_ALL");
        if (!loc || !*loc) loc = p0env("LC_MESSAGES");
        char prefix[KB_TERM_LEN] = "";
        if (loc) {
            size_t n = 0;
            while (loc[n] && loc[n] != '_' && loc[n] != '-' && loc[n] != '.' &&
                   n + 1 < sizeof prefix) {
                prefix[n] = (char)tolower((unsigned char)loc[n]);
                n++;
            }
            prefix[n] = '\0';
        }
        char osl[KB_TERM_LEN];
        default_lang(b, osl, sizeof osl);
        if (prefix[0]) {
            const char *lq[] = { NULL, prefix };
            char mapped[1][KB_TERM_LEN];
            if (kb_match(b->kb, "locale_language_prefix", lq, 2,
                         mapped, 1) > 0)
                snprintf(osl, sizeof osl, "%s", mapped[0]);
        }
        if (osl[0]) {
            const char *oa[] = { osl }; kb_assert(b->kb, "os_language", oa, 1);
            const char *ca[] = { osl }; kb_assert(b->kb, "current_language", ca, 1);
        }
        kb_set_origin(b->kb, KB_SESSION);
    }

    /* gen408 — LE REGOLE MORTE DIVENTANO FATTI, alla nascita.
     *
     * §4d di question-emergence.md e' la sorgente di spazio negativo piu'
     * economica di tutte: si calcola dalla sola KB, senza corpus, senza oracolo
     * e senza conversazione. Era elencata nel documento da gen382m e non era
     * mai stata calcolata.
     *
     * L'ha fatta nascere una domanda vera di F. — «il padre del padre è inteso
     * il?» — che ha rivelato molto piu' di un muro: l'intero strato del
     * ragionamento familiare (`ancestor_of`, `grandfather_of`, `sibling_of`,
     * `child_of`) legge `parent_of/2`, che nessuno scrive mai. La conversazione
     * produce `father/2` e `mother/2`. Due vocabolari che non si toccano, e uno
     * strato di ragionamento che poteva funzionare solo sui cinque fatti di
     * esempio scritti a mano accanto alle regole.
     *
     * Il calcolo va DOPO tutti i caricamenti, altrimenti dichiara morto cio'
     * che non e' ancora arrivato. */
    /* Il registro di lavoro si ricarica: e' cio' che rende il processo
     * autonomo un processo e non un episodio. */
    {
        const char *gp = brain_gaps_path();
        kb_set_origin(b->kb, KB_SESSION);
        kb_load(b->kb, gp);
        kb_set_origin(b->kb, KB_INDUCED);
        kb_load(b->kb, brain_bridges_path());
    }

    dead_rules_publish(b);

    kb_set_origin(b->kb, KB_SESSION); /* conversation default */
    return b;
}

unsigned long brain_footprint(const Brain *b) {
    return (b && b->kb) ? kb_footprint(b->kb) : 0;
}

size_t brain_footprint_width(const Brain *b) {
    return (b && b->kb) ? kb_footprint_width(b->kb) : 0;
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
    /* gen382g: salvare significa INSTRADARE. Ogni fatto nuovo va accanto ai suoi
     * simili nell'albero curato (il save-map); `path` e' solo la ricaduta per
     * cio' che il routing non sa dove mettere, e non e' mai un file di sessione.
     * La radice ha un default perche' l'instradamento e' il comportamento
     * normale, non un opt-in: senza, cio' che si impara finirebbe tutto in un
     * unico file indistinto. */
    const char *root = p0env("PARROT0_KB_ROOT");
    if (!root || !*root) root = "kb";
    return kb_save_routed(b->kb, path, root);
}

/* gen382g — il DUMP della sessione: una fotografia leggibile di cio' che parrot0
 * ha in memoria ADESSO.
 *
 * Non e' un file di conoscenza e non viene mai riletto: e' una rappresentazione
 * Prolog dello stato di runtime, da guardare con `cat`. Percio' e' UNICO PER
 * PROCESSO — due parrot0 sulla stessa macchina non si sovrascrivono — e non ha
 * un default condiviso: chi lo vuole lo chiede con PARROT0_SESSION_DUMP, e
 * altrimenti vive accanto agli altri artefatti di runtime col PID nel nome. */
const char *brain_session_dump_path(void) {
    static char path[512];
    const char *explicit_path = p0env("PARROT0_SESSION_DUMP");
    if (explicit_path && *explicit_path) return explicit_path;
    if (!path[0]) {
        const char *dir = p0env("PARROT0_RUNTIME_DIR");
        if (!dir || !*dir) dir = "/tmp";
        snprintf(path, sizeof path, "%s/parrot0-session-%ld.p0", dir, (long)getpid());
    }
    return path;
}

int brain_session_dump(Brain *b) {
    if (!b || !b->kb) return -1;
    const char *path = brain_session_dump_path();
    if (!path || !*path) return -1;
    return kb_save(b->kb, path, KB_SESSION | KB_INDUCED);
}

void brain_destroy(Brain *b) {
    if (!b) return;
    p0a_free(b->agent_store);
    kb_destroy(b->kb);
    free(b);
}

/* gen277: the brain's KB, for a host that drives the engine directly (MCP). */
KB *brain_kb(Brain *b) { return b ? b->kb : NULL; }

/* Prepare `scratch` as an isolated reasoning sandbox spawned from `parent`.
 *
 * The sandbox owns a fresh, EMPTY KB: premises asserted into it never touch the
 * real one, and a query over world facts still sees only what this turn stated —
 * the closed world that makes syllogism reasoning meaningful. What it no longer
 * loses is parrot0's MACHINERY: `substrate` links back to the parent's KB so
 * grammar and closed lexical classes stay reachable (brain_substrate_query).
 *
 * Returns 0 if the KB could not be created, leaving `scratch` zeroed. */
int brain_scratch_init(Brain *scratch, Brain *parent) {
    if (!scratch) return 0;
    memset(scratch, 0, sizeof *scratch);
    scratch->kb = kb_create();
    if (!scratch->kb) return 0;
    scratch->substrate = parent ? parent->kb : NULL;
    return 1;
}

/* Look up a MACHINERY fact: the sandbox's own KB first, then the substrate it
 * was spawned from. Only for parrot0's own engine knowledge (grammar, lexical
 * classes, routing) — never a path for world facts to leak into a sandbox. On a
 * real brain `substrate` is NULL, so this is exactly kb_query. */
int brain_substrate_query(Brain *b, const char *pred,
                          const char *const *args, size_t argc) {
    if (!b) return 0;
    if (b->kb && kb_query(b->kb, pred, args, argc)) return 1;
    return b->substrate ? kb_query(b->substrate, pred, args, argc) : 0;
}

/* gen382i — un TOKEN FRESCO, dimostrabilmente non intercettato.
 *
 * Il ragionamento su premesse ipotetiche ha sempre avuto bisogno di un mondo
 * chiuso: "se tutti i gatti sono mammiferi e tom e' un gatto, tom e' un
 * mammifero?" deve essere deciso DALLE PREMESSE, non da cio' che parrot0 sa gia'
 * dei gatti. La soluzione era amputare — una KB vuota — ma una KB vuota non e'
 * lo stesso soggetto con meno dati, e' un soggetto diverso: dentro quel sandbox
 * parrot0 non sapeva nemmeno riconoscere un articolo, e ogni lookup lessicale ha
 * dovuto tenersi una lista di ripiego in C (gen371 ha rimediato col substrato,
 * cioe' con una seconda via d'accesso alla stessa KB).
 *
 * La chiusura si ottiene senza togliere niente: si RINOMINANO i termini delle
 * premesse in token che la KB non menziona da nessuna parte. Nessun fatto
 * esistente puo' unificare con `gatto__h1`, quindi il mondo e' chiuso per
 * costruzione — e la KB resta intera, viva e interrogabile per tutto il resto.
 * Non e' un trucco: e' esattamente cio' che fa uno skolem, ed e' verificabile,
 * che l'amputazione non era.
 *
 * Ritorna 0 se non riesce a trovarne uno libero entro un tetto ragionevole. */
int brain_fresh_token(Brain *b, const char *base, char *out, size_t n) {
    if (!b || !b->kb || !base || !*base || !out || n == 0) return 0;
    for (int k = 1; k <= 64; k++) {
        /* Il suffisso NON contiene underscore, e la ragione e' istruttiva: gli
         * atomi composti si scrivono unendo le parole con "_", quindi un "__h1"
         * faceva contare `red__h1_cat__h1` come quattro parole e il cancello dei
         * concetti (gen382e) lo respingeva. Un token fresco deve essere fresco
         * senza cambiare la FORMA di cio' che rinomina. */
        snprintf(out, n, "%sh%dz", base, k);
        if (kb_knows_pred(b->kb, out)) continue;
        if (kb_mentions_term(b->kb, out)) continue;
        return 1;
    }
    out[0] = '\0';
    return 0;
}

/* Enumerate a MACHINERY relation: the sandbox's own KB first, then the substrate
 * it was spawned from (gen382). The query twin of brain_substrate_query — needed
 * as soon as a lexical table has more than a yes/no shape, e.g. the plural rules,
 * where the engine must READ the endings rather than test one. Same rule: only
 * parrot0's own machinery, never a path for world facts to reach a sandbox. */
size_t brain_substrate_match(Brain *b, const char *pred,
                             const char *const *args, size_t argc,
                             char out[][KB_TERM_LEN], size_t max) {
    if (!b) return 0;
    size_t n = b->kb ? kb_match(b->kb, pred, args, argc, out, max) : 0;
    if (n) return n;
    return b->substrate ? kb_match(b->substrate, pred, args, argc, out, max) : 0;
}

/* True if the class is DEFINED at all (own KB or substrate), so a caller can
 * tell "the class says no" from "there is no such class here". */
int brain_substrate_knows(Brain *b, const char *pred) {
    if (!b) return 0;
    if (b->kb && kb_knows_pred(b->kb, pred)) return 1;
    return b->substrate ? kb_knows_pred(b->substrate, pred) : 0;
}

/* gen276: the outer KB layers, factored out of main.c's setup_brain so the same
 * boot is reachable from every host (chat REPL, --daemon, --mcp-engine) and from
 * brain_reload. brain_create already loaded the kernel lexicon + reflective
 * self-model; this adds base/session/coding/profile. Paths come from the
 * environment with the historical defaults; an empty value skips that layer
 * (brain_load/kb_load treat "" as a no-op). */
/* gen331 (TODO.md P1/09): the runtime POLICY becomes knowledge.
 *
 * Counterexample: `make chat` loaded the AGI profile but did not set
 * PARROT0_TOOLS=1 — so the very prompts that are green in piagent-bench ("list the
 * files in src") came back "I don't understand that yet." parrot0 understood
 * perfectly; it simply was not permitted. Answering a permission with a
 * comprehension failure is a discordant claim about itself, and it is the same
 * species of lie as reporting a failed build as a result. Meanwhile the same target
 * quietly turned the Wikipedia fetch ON, so the mode that claimed the least
 * capability was the one reaching the network.
 *
 * The environment is therefore projected into the KB at boot, ONCE, and everything
 * downstream — the banner, the self-model, the declines — reads those facts instead
 * of calling getenv() on its own and drawing its own conclusion:
 *
 *   policy(tools, on|off)      may parrot0 touch the filesystem / run tools?
 *   policy(network, on|off)    may it fetch?
 *   policy(mode, conversational|agent|acquire)
 *
 * One source of truth, so a banner cannot promise what a decline denies. */
static void brain_policy(Brain *b) {
    if (!b || !b->kb) return;
    const char *t = p0env("PARROT0_TOOLS");
    const char *n = p0env("PARROT0_WIKI_FETCH");
    int tools = t && strcmp(t, "1") == 0;
    int net   = n && strcmp(n, "1") == 0;

    const char *pt[] = { "tools",   tools ? "on" : "off" };
    const char *pn[] = { "network", net   ? "on" : "off" };
    const char *pm[] = { "mode",    net ? "acquire" : (tools ? "agent" : "conversational") };
    /* gen411: riflessivo per la stessa ragione del pid — quale politica vale in
     * QUESTO giro non e' conoscenza da depositare nell'albero. */
    int prev_origin = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    kb_assert(b->kb, "policy", pt, 2);
    kb_assert(b->kb, "policy", pn, 2);
    kb_assert(b->kb, "policy", pm, 2);
    kb_set_origin(b->kb, prev_origin);

    /* policy/2 is machinery: knowing that parrot0 may run tools is knowing
     * something about parrot0, not about the world. (gen329's rule, applied at the
     * point where the fact is born rather than in a list somewhere else.) */
    const char *m[] = { "policy" };
    kb_assert(b->kb, "machinery", m, 1);
}

/* The effective policy, for the hosts (banner) and the modules (declines). */
int brain_policy_on(Brain *b, const char *key) {
    if (!b || !b->kb) return 0;
    const char *q[] = { key, "on" };
    return kb_query(b->kb, "policy", q, 2);
}

void brain_mode(Brain *b, char *out, size_t cap) {
    if (!out || !cap) return;
    snprintf(out, cap, "conversational");
    if (!b || !b->kb) return;
    const char *q[] = { "mode", NULL };
    char v[1][KB_TERM_LEN];
    if (kb_match(b->kb, "policy", q, 2, v, 1) == 1) snprintf(out, cap, "%s", v[0]);
}

void brain_boot(Brain *b) {
    if (!b) return;
    const char *base = p0env("PARROT0_BASE");
    const char *profile = p0env("PARROT0_PROFILE");
    if (!base) base = "kb/core/base.p0";
    brain_load(b, base, 1);
    /* gen382g — la SESSIONE NON E' UN INPUT.
     *
     * session.p0 veniva caricato qui come se fosse un file di conoscenza, ed era
     * anche il bersaglio di /save: un file che era insieme sorgente e
     * destinazione, con un default fisso condiviso da ogni parrot0 sulla stessa
     * macchina. Da qui tre difetti in uno — due istanze si sovrascrivevano a
     * vicenda, cio' che si salvava rientrava dalla porta del boot invece di
     * essere instradato nell'albero curato, e la sessione (che e' effimera per
     * definizione) si comportava come conoscenza permanente.
     *
     * Ora la sessione ha una sola natura: e' RUNTIME. Cio' che si impara e si
     * vuole tenere va nei file opportuni tramite il routing (brain_save_session);
     * cio' che sta in memoria e' leggibile dal DUMP (brain_session_dump), che si
     * scrive e non si rilegge mai. */
    brain_load(b, "kb/experts/programming/coding.p0", 1); /* gen149: coding domain */
    if (profile && *profile)
        brain_load(b, profile, 1);                        /* gen150: expert/skill profile */
    brain_policy(b);                                      /* gen331: the effective policy */
}

/* gen276: rebuild the brain's knowledge and session state in place from the
 * files on disk. Builds a fresh brain the SAME way boot did (brain_create +
 * brain_boot), then MOVES its guts into *b — a whole-struct copy so no session
 * field can be missed (the KB and typed-record store are owned pointers, handled
 * explicitly).
 * The unsaved session dies with the old KB; the fresh KB reflects the current
 * file contents, so newly-written knowledge goes live without a restart. */
int brain_reload(Brain *b) {
    if (!b) return -1;
    Brain *fresh = brain_create();
    if (!fresh) return -1;              /* *b left untouched on failure */
    brain_boot(fresh);
    p0a_free(b->agent_store);            /* /restore drops the in-memory task trail */
    kb_destroy(b->kb);                  /* drop the old (possibly unsaved) KB */
    *b = *fresh;                        /* move every field, including owned pointers */
    free(fresh);                        /* free shell; fresh's KB/store now belong to b */
    return (int)kb_size(b->kb);
}

/* gen317 (forge W0.4): the version is DERIVED at build time from the VERSION
 * file (generation label) and the git commit — never a hand-maintained string
 * that can drift from the repo (brain_version said gen300 while HEAD was at
 * gen312; that class of lie is now structurally impossible).
 *
 * gen321 (forge §15 row 7): the stamp is no longer INLINED here. Concatenating
 * the generated macros inside the brain TU made the whole engine depend on which
 * commit it was built at, so every commit rebuilt brain.c (12.4 s). We now call
 * across to src/version.c — the one small TU that sees the stamp. */
#include "../version.h"
const char *brain_version(void) {
    return parrot0_version();
}

/* gen404: IL MURO DIVENTA UN FATTO — e proprio nel caso che finora non lasciava
 * traccia.
 *
 * Il sensore delle lacune esiste dal gen335d, ma registra `pending_gap(parola)`
 * SOLO quando il turno contiene una parola che parrot0 non conosce. Cioe' copre
 * la classe W — manca conoscenza del mondo — e lascia muta la classe M: tutte
 * le parole erano note, e a mancare e' il PONTE. «is pine strong enough for
 * that?», «in three years how old will Bea be?», «why not?»: nessuna parola
 * ignota, nessun fatto scritto, nessun oggetto su cui tornare.
 *
 * E' la classe che conta di piu' (question-emergence.md §10.2-10.3): una lacuna
 * in M e' peggio di una in W perche' nessuna quantita' di lettura la colma, ed
 * e' anche l'unica DECIDIBILE — il rimedio si verifica da solo, perche' la prova
 * e' che il turno che murava ora risponde.
 *
 * Qui si scrive solo il fatto. Chi lo trasformera' in domanda, e la domanda in
 * rimedio, sono le frecce successive dell'anello: questa e' la prima, e senza
 * di essa nessuna delle altre ha un ingresso. */
/* gen416 — LO SCHEMA COMPOSIZIONALE: un'inferenza che dice dove si e' fermata.
 *
 * I lettori di parrot0 restituiscono 1 o 0. `nw == 3 && w[1] == "is"` combacia o
 * non combacia, e fallendo non porta nessuna informazione — da li' le due
 * patologie misurate in docs/autocorrezione.md: il messaggio indovina una parola
 * e la riparazione indovina una sottostringa.
 *
 * Uno schema e' una SEQUENZA DI RUOLI, non un conteggio di token, e quando non
 * si completa sa dire QUALE ruolo manca. E' la differenza fra «non ho capito» e
 * «riconosco una forma copulare, manca la copula»: la seconda e' una lacuna
 * nominabile, quindi riparabile — e il candidato si legge nel turno, come una
 * cue.
 *
 * Additivo fino in fondo: gira soltanto in fondo alla catena, dove tutti i
 * sessantotto moduli hanno gia' rifiutato, e riporta SOLO le corrispondenze
 * parziali. Una forma completa che nessuno ha saputo usare non e' affar suo.
 *
 * Gli schemi, i ruoli e le classi che li riempiono sono fatti (`schema_shape/2`,
 * `role_class/2`, `role_open/1`): uno schema nuovo non costa una riga di C. */
static int schema_probe(Brain *b, char **w, size_t nw,
                        char *out_schema, size_t ssz,
                        char *out_role, size_t rsz) {
    if (!b || !b->kb || nw == 0) return 0;
    char shapes[16][KB_TERM_LEN];
    const char *nq[2] = { NULL, NULL };
    size_t ns = kb_match(b->kb, "schema_shape", nq, 2, shapes, 16);
    size_t best_filled = 0;
    int found = 0;
    for (size_t k = 0; k < ns; k++) {
        char sch[KB_TERM_LEN];
        snprintf(sch, sizeof sch, "%s", shapes[k]);
        const char *shq[2] = { sch, NULL };
        char seq[1][KB_TERM_LEN];
        if (kb_match(b->kb, "schema_shape", shq, 2, seq, 1) != 1) continue;
        char sb[KB_TERM_LEN];
        snprintf(sb, sizeof sb, "%s", kb_dequote(seq[0]));
        char *roles[12];
        size_t nr = split_words(sb, roles, 12);
        size_t ti = 0, filled = 0, class_filled = 0;
        const char *missing = NULL;
        for (size_t r = 0; r < nr; r++) {
            const char *cq[2] = { roles[r], NULL };
            char cls[1][KB_TERM_LEN];
            if (kb_match(b->kb, "role_class", cq, 2, cls, 1) == 1) {
                /* ruolo di CLASSE: si cerca in avanti il primo token che le
                 * appartiene — cosi' un ruolo aperto che ha consumato poco non
                 * fa fallire lo schema intero. */
                char cb[KB_TERM_LEN];
                snprintf(cb, sizeof cb, "%s", cls[0]);
                const char *cname = kb_dequote(cb);
                size_t j = ti;
                for (; j < nw; j++) {
                    char t[KB_TERM_LEN]; snprintf(t, sizeof t, "%s", w[j]);
                    const char *tq[1] = { strip_edge_punct(t) };
                    if (kb_query(b->kb, cname, tq, 1)) break;
                }
                if (j >= nw) { missing = roles[r]; break; }
                ti = j + 1; filled++; class_filled++;
            } else {
                const char *oq[1] = { roles[r] };
                if (!kb_query(b->kb, "role_open", oq, 1)) { missing = roles[r]; break; }
                /* gen416b — IL RUOLO APERTO CONOSCE IL PROPRIO CONFINE.
                 *
                 * Prima consumava un token e basta, e cosi' non sapeva
                 * distinguere «qui il contenuto manca davvero» da «me lo sono
                 * mangiato io». Ora si ferma davanti al ruolo di CLASSE che
                 * segue: se il primo token e' gia' quel marcatore, lo slot di
                 * contenuto e' VUOTO — «if then the ground is wet» non ha
                 * antecedente, «is wet» non ha soggetto — ed e' l'unica assenza
                 * che valga la pena denunciare (criterio 2). */
                const char *stop = NULL;
                char nb[KB_TERM_LEN];
                if (r + 1 < nr) {
                    const char *ncq[2] = { roles[r + 1], NULL };
                    char ncls[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "role_class", ncq, 2, ncls, 1) == 1) {
                        snprintf(nb, sizeof nb, "%s", ncls[0]);
                        stop = kb_dequote(nb);
                    }
                }
                size_t took = 0;
                while (ti < nw) {
                    char t[KB_TERM_LEN]; snprintf(t, sizeof t, "%s", w[ti]);
                    const char *tq[1] = { strip_edge_punct(t) };
                    if (stop && kb_query(b->kb, stop, tq, 1)) break;
                    ti++; took++;
                }
                if (took == 0) { missing = roles[r]; break; }
                filled++;
            }
        }
        /* SERVE UN'EVIDENZA VERA. Un ruolo aperto accetta qualunque token,
         * quindi averne riempito uno non dimostra niente: «tell me about C»
         * riempiva `np` con «tell» e annunciava una forma copulare mancante di
         * copula (misurato su greet.p0t). Almeno un ruolo di CLASSE — un
         * marcatore dichiarato, un «if», una copula — dev'essere stato trovato
         * davvero, altrimenti lo schema non ha riconosciuto nulla e tace.
         *
         * Ne segue che `copular` oggi non si annuncia mai: il suo unico ruolo di
         * classe e' proprio la copula, cioe' il pezzo che manca. Resta dichiarato
         * ed e' corretto cosi' — diventera' utile il giorno in cui esistera' una
         * classe per il verbo, che gli darebbe una seconda ancora. */
        /* E IL PEZZO MANCANTE DEV'ESSERE OBBLIGATORIO.
         *
         * Misurato su transitivity.p0t: «if a is bigger than b and b is taller
         * than c, is a bigger than c?» e' inglese CORRETTO — il «then» e'
         * opzionale, la virgola fa lo stesso lavoro — e lo schema lo denunciava
         * come implicazione monca. Un marcatore assente non e' un difetto: la
         * lingua li omette di continuo.
         *
         * Quello che manca DAVVERO, quando manca, e' uno slot di contenuto. Quali
         * ruoli siano obbligatori e' un fatto (`role_required/1`), e oggi non ne
         * e' dichiarato nessuno: il meccanismo c'e', tace, e si accende con una
         * riga il giorno in cui uno schema sa distinguere un contenuto assente da
         * un contenuto che il ruolo aperto si e' mangiato. Serve che i ruoli
         * aperti conoscano il proprio confine (`np_closer`), ed e' il passo dopo. */
        const char *rq[1] = { missing ? missing : "" };
        if (missing && !kb_query(b->kb, "role_required", rq, 1)) missing = NULL;
        if (missing && class_filled > 0 && filled > best_filled) {
            best_filled = filled;
            snprintf(out_schema, ssz, "%s", sch);
            snprintf(out_role, rsz, "%s", missing);
            found = 1;
        }
    }
    return found && best_filled > 0;
}

/* gen415 — IL REGISTRO SI ANNUNCIA.
 *
 * `looks_code` riconosce il codice per INDIZI — `{`, `;`, `==`, un `(` preceduto
 * da un identificatore — senza nessuno schema da far combaciare, e quando
 * riconosce senza saper eseguire dice quale registro e'. Per la logica gli
 * indizi sono altrettanto robusti e SONO GIA' IN KB (`logic_connector/2`), ma
 * nessuno li usava per classificare: o il lettore di regole faceva combaciare lo
 * schema, o si finiva a nominare una parola a caso.
 *
 * Qui il fondo della catena, prima di nominare una parola, guarda se il turno
 * appartiene a un registro dichiarato. Gli indizi sono fatti
 * (`register_hint/2`), la soglia e' un fatto (`register_hint_min/1`): una lingua
 * nuova o un registro nuovo non costano C.
 *
 * La soglia esiste per una ragione precisa: un indizio solo ruberebbe il
 * messaggio storico a turni che non c'entrano — «all» e «some» stanno in mezzo
 * a mille frasi. Servono DUE indizi distinti perche' il registro si dichiari.
 * Il match e' a parola intera (MANTRA #8: «eat» sta dentro «f-EAT-hers»). */
static int register_of_turn(Brain *b, char **w, size_t nw, char *out, size_t osz) {
    if (!b || !b->kb || nw == 0) return 0;
    char mins[1][KB_TERM_LEN];
    const char *mq[1] = { NULL };
    long need = 2;
    if (kb_match(b->kb, "register_hint_min", mq, 1, mins, 1) > 0) {
        char mb[KB_TERM_LEN]; snprintf(mb, sizeof mb, "%s", mins[0]);
        long v = strtol(kb_dequote(mb), NULL, 10);
        if (v > 0) need = v;
    }
    char regs[16][KB_TERM_LEN];
    const char *rq[2] = { NULL, NULL };
    size_t nr = kb_match(b->kb, "register_hint", rq, 2, regs, 16);
    for (size_t r = 0; r < nr; r++) {
        char reg[KB_TERM_LEN]; snprintf(reg, sizeof reg, "%s", regs[r]);
        long hits = 0;
        char seen[16][KB_TERM_LEN]; size_t nseen = 0;
        for (size_t i = 0; i < nw; i++) {
            char t[KB_TERM_LEN]; snprintf(t, sizeof t, "%s", w[i]);
            char *tok = strip_edge_punct(t);
            if (!*tok) continue;
            const char *hq[2] = { reg, tok };
            if (!kb_query(b->kb, "register_hint", hq, 2)) continue;
            int dup = 0;
            for (size_t k = 0; k < nseen; k++) if (!strcmp(seen[k], tok)) dup = 1;
            if (dup) continue;
            if (nseen < 16) snprintf(seen[nseen++], KB_TERM_LEN, "%s", tok);
            hits++;
        }
        if (hits >= need) { snprintf(out, osz, "%s", reg); return 1; }
    }
    return 0;
}

/* gen414 — IL FALLIMENTO HA QUATTRO FORME, non una.
 *
 * §9.1 di question-emergence.md aveva gia' decomposto il problema — knowledge,
 * reachability, surface, wrong-answer — e poi gen406-411 hanno costruito per il
 * solo muro cieco. Misurato sui cento fallimenti di parrot0-100-failures.md:
 * 88 fallimenti, 85 invisibili al ciclo, perche' un declino informato («I don't
 * know about X yet») e' considerato un turno GESTITO e non lascia traccia.
 *
 * Qui l'esito dichiara la propria forma, e QUALI esiti siano fallimenti e' un
 * fatto (`unsatisfying_outcome/2`): togliere quella riga dalla KB rimette le
 * cose come stavano, senza ricompilare. `machinery_gap/1` resta esattamente
 * com'era, con tutti i suoi consumatori — `gap_kind/2` gli si affianca. */
static void gap_record_as(Brain *b, const char *canon, const char *raw,
                          const char *outcome) {
    if (!b || !b->kb || !canon || !*canon) return;
    const char *q[2] = { outcome, NULL };
    char kind[1][KB_TERM_LEN];
    if (kb_match(b->kb, "unsatisfying_outcome", q, 2, kind, 1) != 1) return;
    machinery_gap_record(b, canon, raw);
    char cq[KB_TERM_LEN];
    snprintf(cq, sizeof cq, "\"%s\"", canon);
    const char *ka[2] = { cq, kind[0] };
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_SESSION);
    kb_assert(b->kb, "gap_kind", ka, 2);
    /* gen434: e l'esito del turno, che e' la prima delle tre cose da cui la
     * specie si deriva. */
    kb_set_origin(b->kb, KB_REFLECTIVE);
    const char *oa[2] = { "current_turn", outcome };
    kb_assert(b->kb, "turn_outcome", oa, 2);
    kb_set_origin(b->kb, prev);
}

/* gen413: l'autocorrezione sul muro sta piu' su della sua implementazione. */
static int self_repair_target(Brain *b, const char *only, char *out, size_t out_size);

static void machinery_gap_record(Brain *b, const char *canon, const char *raw) {
    if (!b || !b->kb || !canon || !*canon) return;
    char q[KB_TERM_LEN];
    snprintf(q, sizeof q, "\"%s\"", canon);
    const char *ga[] = { q };
    if (kb_query(b->kb, "machinery_gap", ga, 1)) return;   /* gia' registrata */
    /* gen411: SESSIONE, non riflessivo. Il modello di se' non si persiste per
     * scelta — si ricalcola a ogni nascita — ma una lacuna non e' il modello di
     * se': e' un registro di LAVORO, e un processo autonomo che riparte da zero
     * a ogni avvio non ha nessuna agenda su cui lavorare. Era il blocco vero
     * fra «il ciclo funziona se glielo chiedi» e «il ciclo gira da solo». */
    kb_set_origin(b->kb, KB_SESSION);
    kb_assert(b->kb, "machinery_gap", ga, 1);

    /* gen410: LA LACUNA RICORDA ANCHE COME E' STATA DETTA.
     *
     * La lacuna e' indicizzata sul turno CANONICALIZZATO, che e' giusto — due
     * modi di dire la stessa cosa sono la stessa lacuna. Ma un ponte si prova
     * riponendo il turno VERO: la prima versione del ciclo di autoriparazione
     * proponeva `setting_cue("are to")`, cioe' una cue nello spazio
     * canonicalizzato, la verificava contro il canon stesso e la dichiarava
     * buona — e poi «siamo al mare» murava ancora. Una riparazione verificata
     * contro una traduzione non e' una riparazione. */
    if (raw && *raw) {
        char rq[KB_TERM_LEN];
        snprintf(rq, sizeof rq, "\"%s\"", raw);
        const char *sa[2] = { q, rq };
        kb_assert(b->kb, "gap_source", sa, 2);
    }

    /* gen406 — L'ANCORA: DOVE SI E' FERMATO.
     *
     * Fin qui la lacuna era il turno e nient'altro, e chi volesse rimediarla
     * aveva come spazio di ricerca l'intera KB. Ma un muro non e' opaco: il
     * turno si divide sempre in due, cio' di cui parrot0 SA qualcosa e cio' che
     * per lui non e' niente, e quella divisione e' gia' calcolabile — la stessa
     * domanda che il declino informato fa per scegliere quale parola nominare.
     *
     *   «in three years how old will Bea be»  ancora: years  opaco: bea
     *   «what is the total of 4, 5 and 6»     ancora: (nessuna)
     *
     * La seconda riga e' la piu' istruttiva: nessuna ancora vuol dire che non
     * manca conoscenza sul mondo — manca un PONTE, e nessuna lettura lo
     * portera'. E' la distinzione M/W di question-emergence.md §10 calcolata sul
     * singolo turno invece che ragionata a tavolino.
     *
     * Qui si scrive solo la divisione. Chi la usera' per restringere l'ipotesi
     * e' la gen409; senza, non ha un ingresso. */
    {
        char buf[KB_TERM_LEN];
        snprintf(buf, sizeof buf, "%s", canon);
        char *w[64];
        size_t nw = split_words(buf, w, 64);
        for (size_t i = 0; i < nw; i++) {
            char *t = strip_edge_punct(w[i]);
            if (strlen(t) < 3 || !isalpha((unsigned char)t[0])) continue;
            if (is_stopword(b, t)) continue;
            char desc[KB_TERM_LEN];
            int known = kb_knows_pred(b->kb, t) ||
                        kb_describe_entity(b->kb, t, desc, sizeof desc);
            const char *aa[] = { q, t };
            kb_assert(b->kb, known ? "gap_anchor" : "gap_opaque", aa, 2);
        }
    }
    kb_set_origin(b->kb, KB_SESSION);
}

/* La chiusura: vedi la nota alla chiamata, in fondo a brain_respond. `bridged/1`
 * resta come traccia — una lacuna colmata e' un'informazione diversa da una
 * lacuna mai avuta, ed e' quella che dice se l'anello sta girando. */
static void machinery_gap_close(Brain *b, const char *canon) {
    if (!b || !b->kb || !canon || !*canon) return;
    char q[KB_TERM_LEN];
    snprintf(q, sizeof q, "\"%s\"", canon);
    const char *ga[] = { q };
    if (!kb_query(b->kb, "machinery_gap", ga, 1)) {
        /* gen410: la lacuna e' indicizzata sul turno CANONICALIZZATO, ma su
         * alcuni percorsi la chiusura arriva con il turno normalizzato — un
         * limite che il gen404 aveva dichiarato e che ora morde: una lacuna
         * italiana restava aperta anche dopo essere stata colmata, e il ciclo
         * di autoriparazione concludeva che il suo ponte non aveva funzionato
         * mentre aveva funzionato benissimo.
         *
         * `gap_source` conserva come il turno era stato detto: si chiude anche
         * per quella chiave. E' la ragione per cui la sorgente e' un fatto e
         * non un dettaglio di comodo. */
        const char *sq[2] = { NULL, q };
        char key[1][KB_TERM_LEN];
        if (kb_match(b->kb, "gap_source", sq, 2, key, 1) < 1) return;
        snprintf(q, sizeof q, "%s", key[0]);
        if (!kb_query(b->kb, "machinery_gap", ga, 1)) return;
    }
    kb_retract(b->kb, "machinery_gap", ga, 1);
    /* Una lacuna colmata non lascia il proprio scavo: l'ancora serviva a
     * cercarne il rimedio, e il rimedio c'e'. Lasciarla renderebbe il registro
     * monotono crescente dal lato dei dettagli, che e' lo stesso difetto da cui
     * il gen404 ci ha tirati fuori dal lato delle lacune. */
    for (;;) {
        const char *aq[2] = { q, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "gap_anchor", aq, 2, hit, 1) < 1) break;
        const char *ra[2] = { q, hit[0] };
        if (!kb_retract(b->kb, "gap_anchor", ra, 2)) break;
    }
    for (;;) {
        const char *aq[2] = { q, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "gap_opaque", aq, 2, hit, 1) < 1) break;
        const char *ra[2] = { q, hit[0] };
        if (!kb_retract(b->kb, "gap_opaque", ra, 2)) break;
    }
    /* gen405 (F.): UN PONTE TROVATO DAL CICLO E UNO MESSO A MANO SONO DUE
     * EVENTI OPPOSTI, e finora lasciavano la stessa traccia.
     *
     * Il criterio del progetto non e' «nessuna lacuna chiusa a mano» — sarebbe
     * aspettare che il ciclo si sblocchi proprio dove e' bloccato. E' che le
     * lacune chiuse a mano siano le MINIME che fanno evincere che il processo
     * non progredisce. Un registro che non distingue i due casi non puo'
     * misurarlo: da qui in poi ogni chiusura dice da chi.
     *
     * `dreaming/1` e' asserito dal sogno per la sua durata. Oggi le uniche due
     * provenienze sono «durante un sogno» e «in conversazione»; quando il ciclo
     * sapra' PROPORRE (gen410) ne servira' una terza, e sara' quella che conta. */
    const char *dq[] = { NULL };
    char who[1][KB_TERM_LEN];
    const char *by = "dialogue";
    if (kb_match(b->kb, "repairing", dq, 1, who, 1) > 0) by = "proposal";
    else if (kb_match(b->kb, "dreaming", dq, 1, who, 1) > 0) by = "dream";
    kb_set_origin(b->kb, KB_SESSION);
    const char *ba[] = { q, by };
    kb_assert(b->kb, "bridged", ba, 2);
    kb_set_origin(b->kb, KB_SESSION);
}

/* gen55 (C5a): an honest, NON-repeating not-understood reply. The chatsim users
 * showed that repeating "I don't understand that yet." verbatim is the #1
 * naturalness killer (a broken record). So the classic line is kept for a LONE
 * occurrence (no test churn, still honest), but when it would repeat our previous
 * reply we vary — reflecting a salient word from the user so it feels heard, else
 * rotating honest redirects. It never feigns understanding. */
static void not_understood(Brain *b, const char *canon, const char *raw,
                           char *out, size_t out_size) {
    /* gen240 (universal-comprehension): the honest fallback in the CURRENT language. */
    /* LE FRASI DEL MURO SONO CONOSCENZA (gen450).
     *
     * Erano due array C, uno per lingua, con la rotazione anti-ripetizione
     * scritta a mano accanto. Ma la rotazione fra forme intercambiabili
     * `kb_response_slots` la fa GIA' da sola — sceglie fra le righe della
     * stessa famiglia con un contatore per chiave — e la scelta della lingua
     * pure, preferendo `response_template/3`. Il C ne teneva una seconda copia
     * divergente proprio per la frase che parrot0 dice piu' spesso, e che era
     * quindi l'unica non insegnabile.
     *
     * Ora: `wall_generic` porta le forme intercambiabili, `wall_classic` la
     * forma canonica per l'occorrenza isolata. Qui resta solo la MECCANICA —
     * non ripetere l'ultima risposta. */
    enum { WALL_TRIES = 8 };
    char classicbuf[512];
    kb_term_say(b, "wall_classic", NULL, 0, classicbuf, sizeof classicbuf);
    const char *classic = classicbuf;

    /* ── gen384: IL MURO DEVE DIRE DOVE SI E' FERMATO ───────────────────────
     *
     * Fino a qui il declino informato — «non conosco ancora <parola>» — era
     * annidato SOTTO il ramo anti-ripetizione: scattava solo quando la risposta
     * *stava per ripetersi*. Cioe' il sensore delle lacune era un effetto
     * collaterale di una correzione di naturalezza (gen55), e alla PRIMA
     * occorrenza non registrava e non diceva niente.
     *
     * Il costo, misurato: «quante sono le carte del pocker» si canonicalizza in
     * "how many are the cards of the pocker" — perfetto tranne una parola — e la
     * risposta era «Non capisco ancora». parrot0 sapeva esattamente dove si era
     * fermato e taceva. Un interlocutore che dice *dove* si e' perso e' cio' che
     * distingue una conversazione da un muro (the-linguistic-glue.md).
     *
     * Ora la ricerca della parola opaca viene PRIMA, e il declino informato e' il
     * caso normale: la frase generica resta per quando non c'e' niente da
     * nominare — quando tutte le parole erano note e a mancare e' il ponte. */
    char buf[256];
    snprintf(buf, sizeof buf, "%s", canon);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    const char *sw = NULL;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        char desc[256];
        /* CHE COSA si nomina. Il criterio resta quello storico — «non ho fatti su
         * questa cosa» — con una sola esclusione, e nessuna delle due e' un
         * elenco in C.
         *
         * gen384 ha provato a stringerlo su `lexeme/1` (35k voci) per non
         * nominare parole comuni come "bigger" o "capital". Due misure lo hanno
         * scartato: «parliamo di formaggio» smetteva di nominare "cheese" — il
         * caso che conta — e caricare il lessico al muro triplicava la KB e
         * faceva sforare i tempi dei turni successivi. La lezione e' registrata
         * perche' non la si ripeta: il discriminante che serve non e' lessicale,
         * e' POSIZIONALE (quale token e' l'argomento del turno), e arriva col
         * residuo tipizzato di question-emergence.md §11.5.
         *
         * L'esclusione: una parola che parrot0 usa come MARCATORE non e' un
         * argomento su cui offrirsi di imparare — "thanks, that was wrong" non e'
         * una domanda su "thanks". I marcatori sono gia' conoscenza
         * (`social_marker/2`), quindi la frontiera si estende con un fatto. */
        const char *smq[2] = { NULL, t };
        char smhit[1][KB_TERM_LEN];
        int is_marker = b && b->kb &&
                        kb_match(b->kb, "social_marker", smq, 2, smhit, 1) > 0;
        int known = b && b->kb &&
                    (is_marker ||
                     kb_knows_pred(b->kb, t) ||
                     kb_describe_entity(b->kb, t, desc, sizeof desc));
        /* IL SENSORE E LA RISPOSTA SONO DUE COSE DIVERSE.
         *
         * Il vocabolario di parrot0 copre UNA lingua (question-emergence.md
         * §13.4). In italiano, quindi, "non ho fatti su questa parola" non
         * distingue un argomento sconosciuto da una parola qualunque della
         * lingua: misurato sulla suite, il declino finiva per nominare
         * "facciamo", "chiamo", "allora", "parlare", "scritto" — parole funzione
         * e verbi comuni. Nominarle e' vero alla lettera e privo di senso per
         * chi legge.
         *
         * Quindi la NOMINA vale solo dove il vocabolario e' abbastanza completo
         * da rendere informativa un'assenza, e quali lingue lo siano e' un fatto
         * (`lexicon_language/1`). Il SENSORE resta attivo comunque — la lacuna
         * viene registrata in KB anche quando la risposta non la nomina, perche'
         * l'autodiscovery lavora sui fatti e non sulle frasi. Il giorno in cui il
         * vocabolario italiano esiste, un fatto accende la nomina senza toccare
         * il motore. */
        if (!known) {
            char tl[8]; current_lang(b, tl, sizeof tl);
            const char *llq[] = { tl };
            if (b && b->kb && !kb_query(b->kb, "lexicon_language", llq, 1))
                known = 1;
        }
        if (strlen(t) >= 6 && isalpha((unsigned char)t[0]) &&
            !is_stopword(b, t) && !known) {
            sw = t; break;
        }
    }
    /* gen416: prima ancora del registro, prova a nominare IL PEZZO CHE MANCA.
     * E' l'informazione piu' specifica che il fondo della catena possa dare, e
     * l'unica che indichi un rimedio invece di un'area. */
    {
        char sch[KB_TERM_LEN], role[KB_TERM_LEN];
        if (b && b->kb && schema_probe(b, w, nw, sch, sizeof sch, role, sizeof role)) {
            gap_record_as(b, canon, raw, "incomplete_schema");
            char cq2[KB_TERM_LEN];
            snprintf(cq2, sizeof cq2, "\"%s\"", canon);
            const char *ma[3] = { cq2, sch, role };
            int prev = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, "gap_missing", ma, 3);
            kb_set_origin(b->kb, prev);
            char smsg[400];
            const KbResponseSlot ss[] = { {"schema", sch}, {"role", role} };
            if (!kb_response_slots(b, "schema_incomplete", ss, 2, smsg, sizeof smsg))
                { const KbResponseSlot _rs[] = { { "sch", sch }, { "role", role } };
      kb_term_say(b, "i_can_see_a_x_shape_here_but_the_x_is_missin", _rs, 2, smsg, sizeof smsg);
                  put(smsg, out, out_size); }
            if (b) b->fallbacks++;
            return;
        }
    }
    /* gen415: PRIMA di nominare una parola, prova a nominare il REGISTRO.
     * Nominare una parola a caso e' una diagnosi falsa spacciata per
     * informazione (docs/autocorrezione.md §11); dire «questo e' un problema di
     * logica che non so ancora risolvere» e' vero, ed e' una lacuna nominabile
     * — cioe' riparabile. Se nessun registro si dichiara, tutto prosegue
     * esattamente come prima: lo strato si aggiunge, non sostituisce. */
    {
        char reg[KB_TERM_LEN];
        const char *slq[1] = { reg };
        /* gen417: un registro puo' servire come VINCOLO senza essere una cosa da
         * annunciare. `self` dice «questo turno parla di parrot0» e serve a
         * impedire che un ponte di auto-resoconto venga proposto per una domanda
         * sul mondo — ma annunciarlo ruberebbe il turno al ripiego che registra
         * la lacuna, e il ciclo resterebbe senza niente da riparare (misurato:
         * self_repair.p0t). Quali registri tacciano e' un fatto. */
        if (b && b->kb && register_of_turn(b, w, nw, reg, sizeof reg) &&
            !kb_query(b->kb, "register_silent", slq, 1)) {
            gap_record_as(b, canon, raw, "recognized_register");
            char rq2[KB_TERM_LEN];
            snprintf(rq2, sizeof rq2, "\"%s\"", canon);
            const char *ra2[2] = { rq2, reg };
            int prev = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, "gap_register", ra2, 2);
            /* gen434: e il registro di QUESTO turno, per la derivazione della
             * specie — `gap_register` e' indicizzato sul canon e sopravvive al
             * turno, `turn_register` no. Sono due cose diverse. */
            kb_set_origin(b->kb, KB_REFLECTIVE);
            const char *tr2[2] = { "current_turn", reg };
            kb_assert(b->kb, "turn_register", tr2, 2);
            kb_set_origin(b->kb, prev);
            char rmsg[400];
            const KbResponseSlot rs[] = { {"register", reg} };
            if (!kb_response_slots(b, "register_declined", rs, 1, rmsg, sizeof rmsg))
                { const KbResponseSlot _rs[] = { { "reg", reg } };
      kb_term_say(b, "that_looks_like_a_x_problem_and_i_cannot_sol", _rs, 1, rmsg, sizeof rmsg);
                  put(rmsg, out, out_size); }
            if (b) b->fallbacks++;
            return;
        }
    }
    /* gen404: IL MURO DIVENTA UN FATTO ANCHE QUANDO NON HA UNA PAROLA DA
     * NOMINARE — vedi machinery_gap_record poco sopra questa funzione. */
    /* Inizializzato alla frase generica: se un template KB manca o non rende,
     * il declino resta comunque una frase vera invece di memoria non scritta. */
    /* Il messaggio porta ora due frasi didattiche complete: 256 byte lo
     * troncavano a meta' dell'offerta, cioe' proprio sulla parte utile. */
    char cand[768];
    snprintf(cand, sizeof cand, "%s", classic);
    if (!b) { put(classic, out, out_size); return; }
    if (sw) {
        /* gen335d (linguistic glue, KB-first): store the knowledge gap as a KB
         * fact, not a C field. The fact of not-knowing IS knowledge.
         * gen335e: skip if this topic was already tried and failed. */
        /* gen384: la guardia era GLOBALE — un solo pending_gap alla volta, per
         * tutta la sessione — quindi la seconda parola ignota di una conversazione
         * non veniva mai registrata. Ora e' per TOPIC: chiedere di due cose
         * sconosciute lascia due tracce, che e' il minimo perche' l'autodiscovery
         * abbia qualcosa su cui lavorare. */
        const char *gq[] = { sw };
        int already_gap = kb_query(b->kb, "pending_gap", gq, 1);
        const char *fq[] = { sw, NULL };
        int already_failed = kb_query(b->kb, "pending_gap_failed", fq, 1);
        /* gen414: anche questo e' un fallimento. Il turno ha nominato una
         * parola e offerto di impararla, il che LO FA SEMBRARE gestito — ma il
         * lavoro non e' stato fatto, e sono quarantanove prompt su cento. */
        gap_record_as(b, canon, raw, "informed_decline");
        /* gen434: la parola nominata e' il TOPIC del turno, e serve a decidere
         * se manchi il valore o la strada che ci arriva. */
        {
            int prev_o = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_REFLECTIVE);
            const char *ta[2] = { "current_turn", sw };
            kb_assert(b->kb, "turn_topic", ta, 2);
            kb_set_origin(b->kb, prev_o);
        }
        if (!already_gap && !already_failed) {
            kb_set_origin(b->kb, KB_REFLECTIVE);
            const char *ga[] = { sw };
            kb_assert(b->kb, "pending_gap", ga, 1);
            char qq[KB_TERM_LEN];
            snprintf(qq, sizeof qq, "\"%s\"", canon);
            const char *qa[] = { qq };
            kb_assert(b->kb, "pending_gap_question", qa, 1);
            kb_set_origin(b->kb, KB_SESSION);
            {
                /* gen430 — IL MURO DIVENTA UNA RICHIESTA DI INSEGNAMENTO.
                 *
                 * Il ciclo di autocorrezione (docs/autocorrezione.md) deve
                 * INVENTARSI il pezzo mancante, e per questo arriva a tre casi
                 * su ottantotto: sa proporre solo cue. Ma l'interlocutore e' nella
                 * stanza, e dal gen427-429 una forma si insegna PARLANDO — quindi
                 * il pezzo non va indovinato, va CHIESTO.
                 *
                 * Qui il declino su una parola opaca smette di essere una
                 * domanda retorica («vuoi che lo impari?») e diventa una
                 * richiesta con la FRASE GIA' PRONTA da dire. Quali forme si
                 * possano offrire per una parola sola, e con che parole
                 * chiederle, sta in `word_teaching_offer/2`: una forma nuova si
                 * affaccia da sola nel messaggio, senza toccare questo file. */
                char teach[512]; teach[0] = '\0';
                size_t to = 0;
                /* La parola che sta al POSTO DELLA RELAZIONE, quando il turno
                 * ha la forma di una relazione binaria: il muro nomina la prima
                 * parola opaca, che nella forma «X ? Y» e' il soggetto, non la
                 * forma da insegnare. Chi la sceglie e' la KB. */
                char middle[KB_TERM_LEN] = "";
                {
                    const char *mq[2] = { "current_turn", NULL };
                    char mv[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "turn_gap_middle", mq, 2, mv, 1) == 1) {
                        char mb[KB_TERM_LEN]; snprintf(mb, sizeof mb, "%s", mv[0]);
                        snprintf(middle, sizeof middle, "%s", kb_dequote(mb));
                    }
                }
                long cap = 2;
                {
                    const char *cq2[1] = { NULL };
                    char cv[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "teaching_offer_max", cq2, 1, cv, 1) > 0) {
                        char cb2[KB_TERM_LEN]; snprintf(cb2, sizeof cb2, "%s", cv[0]);
                        long v = strtol(kb_dequote(cb2), NULL, 10);
                        if (v > 0) cap = v;
                    }
                }
                /* M13: QUALI forme offrire lo decide la KB guardando la forma
                 * del turno (`turn_teaching_offer/2`), non l'ordine in cui
                 * l'elenco e' scritto. Se la derivazione non produce nulla —
                 * perche' il turno non ha struttura pubblicata, o perche'
                 * nessuna regola combacia — resta l'elenco completo: una
                 * diagnosi assente non deve poter togliere l'offerta. */
                char offers[8][KB_TERM_LEN];
                const char *sq[2] = { "current_turn", NULL };
                size_t no = kb_match(b->kb, "turn_teaching_offer", sq, 2,
                                     offers, 8);
                if (no == 0) {
                    const char *oq[2] = { NULL, NULL };
                    no = kb_match(b->kb, "word_teaching_offer", oq, 2, offers, 8);
                }
                for (size_t oi = 0; oi < no && (long)oi < cap; oi++) {
                    const char *fq2[2] = { offers[oi], NULL };
                    char text[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "word_teaching_offer", fq2, 2, text, 1) != 1) continue;
                    char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", text[0]);
                    const char *phrase = kb_dequote(tb);
                    /* la parola entra nella frase dove la frase la chiede */
                    for (const char *p = phrase; *p && to + 1 < sizeof teach; ) {
                        if (!strncmp(p, "{topic}", 7)) {
                            to += (size_t)snprintf(teach + to, sizeof teach - to, "%s", sw);
                            p += 7;
                        } else if (!strncmp(p, "{middle}", 8)) {
                            to += (size_t)snprintf(teach + to, sizeof teach - to,
                                                   "%s", middle[0] ? middle : sw);
                            p += 8;
                        } else teach[to++] = *p++;
                        teach[to] = '\0';
                    }
                    if (oi + 1 < no && (long)(oi + 1) < cap)
                        to += (size_t)snprintf(teach + to, sizeof teach - to, "; ");
                }
                const KbResponseSlot slots[] = { {"topic", sw}, {"teach", teach} };
                kb_response_slots(b, "fallback_gap_offer", slots, 2, cand, sizeof cand);
            }
        } else {
            {
                const KbResponseSlot slots[] = { {"topic", sw} };
                kb_response_slots(b, "fallback_still_gap", slots, 1, cand, sizeof cand);
            }
        }
    }
    else if (strcmp(classic, b->last_reply) != 0) {
        /* Nessuna parola opaca da nominare: le parole c'erano tutte e a non
         * esserci e' il ponte. */
        machinery_gap_record(b, canon, raw);
        gap_record_as(b, canon, raw, "blind_wall");
        /* ── gen413: L'AUTOCORREZIONE AVVIENE QUI, NEL TURNO ─────────────────
         *
         * Questo ramo e' l'unico posto dove ha senso, e l'ancora del gen406 dice
         * perche': se nessuna parola era opaca, leggere non puo' colmare niente
         * — le parole c'erano tutte. L'unica mossa e' proporre, ed e' proprio
         * adesso che serve, non quando qualcuno lo chiede.
         *
         * Finora il ciclo funzionava ma andava INNESCATO: «prova a ripararti»
         * era un turno dell'utente. Un sistema che si ripara solo se glielo
         * chiedi non si sta riparando da solo. La porta e' sempre la stessa —
         * il prompt — quindi il tentativo va fatto sul muro, prima di
         * dichiararlo.
         *
         * E' sicuro per costruzione, non per fiducia: `repair_try` asserisce in
         * ipotetico, RIPONE il turno, e tiene solo cio' che lo fa rispondere.
         * Un tentativo fallito non lascia niente. Percio' il peggio che puo'
         * capitare e' lo stesso muro di prima, pagato un po' piu' caro.
         *
         * L'interruttore e' un fatto (`self_correct_on_wall/1`): spegnerlo non
         * richiede di ricompilare, e chi vuole il muro nudo lo puo' avere. */
        {
            const char *onq[] = { "on" };
            const char *busy[] = { "1" };
            if (kb_query(b->kb, "self_correct_on_wall", onq, 1) &&
                !kb_query(b->kb, "repairing", busy, 1) &&
                !kb_query(b->kb, "self_correcting", busy, 1)) {
                kb_set_origin(b->kb, KB_REFLECTIVE);
                kb_assert(b->kb, "self_correcting", busy, 1);
                kb_set_origin(b->kb, KB_SESSION);
                char learned[512]; learned[0] = '\0';
                const char *ra[1] = { "1" };
                kb_assert(b->kb, "repairing", ra, 1);
                int fixed = self_repair_target(b, canon, learned, sizeof learned);
                kb_retract(b->kb, "repairing", ra, 1);
                char again[900]; again[0] = '\0';
                if (fixed > 0) brain_respond(b, raw, again, sizeof again);
                kb_retract(b->kb, "self_correcting", busy, 1);
                /* Si tiene solo se il turno ORA risponde davvero: un ponte che
                 * sposta il muro da una frase all'altra non e' una riparazione. */
                if (fixed > 0 && again[0] && strcmp(again, classic) != 0) {
                    char msg[1100];
                    const KbResponseSlot sl[] = { {"bridge", learned}, {"answer", again} };
                    if (!kb_response_slots(b, "self_corrected_on_wall", sl, 2,
                                           msg, sizeof msg))
                        { const KbResponseSlot _rs[] = { { "again", again }, { "learned", learned } };
      kb_term_say(b, "x_i_was_missing_a_piece_and_taught_it_to_mys", _rs, 2, msg, sizeof msg);
                          put(msg, out, out_size); }
                    return;
                }
            }
        }
        put(classic, out, out_size);
        b->fallbacks++;
        return;
    }
    else {
        machinery_gap_record(b, canon, raw);
        gap_record_as(b, canon, raw, "blind_wall");
        kb_term_say(b, "wall_generic", NULL, 0, cand, sizeof cand);
    }
    /* Anti-ripetizione: ogni chiamata avanza il contatore della famiglia, quindi
     * richiedere la frase E' gia' il modo di chiederne un'altra. */
    for (size_t t = 0; t < WALL_TRIES && strcmp(cand, b->last_reply) == 0; t++)
        kb_term_say(b, "wall_generic", NULL, 0, cand, sizeof cand);
    put(cand, out, out_size);
    b->fallbacks++;
}

/* gen80: true if word `w` is a likely intent-marker that starts a sub-turn
 * after a discourse connector like "e"/"and" in a compound utterance. */
static int is_intent_starter(Brain *b, const char *w) {
    /* gen335 round-3: KB-first migration — query intent_starter/1 from KB
     * instead of a hardcoded C array. The engine is fixed; the lexicon learns. */
    if (b && b->kb && w && *w) {
        char atom[64]; size_t j = 0;
        for (const char *c = w; *c && j + 1 < sizeof atom; c++)
            atom[j++] = (char)tolower((unsigned char)*c);
        atom[j] = 0;
        const char *args[] = {atom};
        return kb_query(b->kb, "intent_starter", args, 1);
    }
    return 0;
}

/* gen88: true if word `w` is a negation marker that should cause the sub-turn
 * to be suppressed (e.g., "dont answer", "non rispondere"). */
static int is_negation_marker(Brain *b, const char *w) {
    const char *q[] = { w };
    return b && b->kb && w && kb_query(b->kb, "negation_marker", q, 1);
}

/* gen80: split `canon` on discourse connectors where the second half starts
 * with an intent marker, dispatch each sub-turn, and join responses. Returns
 * 1 if decomposition was applied, 0 to use normal dispatch. */
static int decompose_and_dispatch(Brain *b, const char *canon, const char *input,
                                   char *out, size_t out_size) {
    if (b && b->kb) {
        char guards[32][KB_TERM_LEN];
        const char *gq[] = { "decompose", NULL };
        size_t ng = kb_match(b->kb, "compound_guard", gq, 2, guards, 32);
        for (size_t gi = 0; gi < ng; gi++)
            if (kb_cue_match(b, kb_dequote(guards[gi]), canon)) return 0;

        /* Some typed spans are atomic for discourse decomposition. The KB
         * assigns that policy to an open span role; adding a cue to the role
         * inherits it without a new literal or branch here. */
        InputSpan spans[64]; int ambiguous = 0;
        size_t ns = input_segment(b->kb, input, spans, 64, &ambiguous);
        if (!ambiguous) {
            for (size_t i = 0; i < ns; i++) {
                char type[KB_TERM_LEN];
                input_span_type(&spans[i], type, sizeof type);
                const char *policy[] = { type, "whole" };
                if (kb_query(b->kb, "decompose_span_policy", policy, 2))
                    return 0;
            }
        }
    }
    /* Don't decompose structured prompts — they have their own parser. */
    if (strncmp(canon, "premise:", 8) == 0 ||
        strncmp(canon, "label premise:", 14) == 0 ||
        strncmp(canon, "explain premise:", 16) == 0 ||!lex_prefix_member(b, "99_registry_lex1963", canon) == 0 ||!lex_prefix_member(b, "99_registry_lex1964", canon) == 0 ||!lex_prefix_member(b, "99_registry_lex1965", canon) == 0 ||
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

    if (!is_intent_starter(b, first_word)) return 0;

    char sub1[256], sub2[256];
    size_t cpos = (size_t)(conn - canon);
    size_t len = strlen(canon);
    if (cpos >= sizeof sub1) cpos = sizeof sub1 - 1;
    memcpy(sub1, canon, cpos); sub1[cpos] = '\0';
    size_t s2len = len - cpos - conn_len;
    if (s2len >= sizeof sub2) s2len = sizeof sub2 - 1;
    memcpy(sub2, conn + conn_len, s2len); sub2[s2len] = '\0';

    /* gen240: peel a leading sequencer ("then", "also", "poi", …) off the second
     * sub-turn so "<acquire X> and THEN <use X>" dispatches the clean clause to its
     * module (e.g. "look up X and then tell me about X" -> acquire, then recall). */
    {
        /* The sequencers come from `sequencer/1` in lexicon.p0 — the same class
         * the rest of the engine reads — so one taught at runtime is peeled
         * here too. This was the third C copy of that list. */
        const char *seqvar[] = {NULL};
        char seqw[64][KB_TERM_LEN];
        size_t nseq = kb_match(b->kb, "sequencer", seqvar, 1, seqw, 64);
        for (;;) {
            char *s = sub2; while (*s == ' ' || *s == ',' || *s == '.') s++;
            size_t adv = 0;
            for (size_t k = 0; k < nseq; k++) {
                const char *w = kb_dequote(seqw[k]);
                size_t wl = strlen(w);
                if (strncmp(s, w, wl) == 0 && (s[wl] == ' ' || s[wl] == '\0'))
                    { adv = (size_t)(s - sub2) + wl; break; }
            }
            if (!adv) break;
            memmove(sub2, sub2 + adv, strlen(sub2 + adv) + 1);
        }
    }

    char r1[1024] = "", r2[1024] = "";
    int h1 = 0, h1_disc = 0, h2 = 0;
    int negate1 = 0, negate2 = 0;

    /* gen88: check if either sub-turn starts with a negation marker. */
    {
        char *sw[8]; char b1[256], b2[256];
        snprintf(b1, sizeof b1, "%s", sub1);
        snprintf(b2, sizeof b2, "%s", sub2);
        size_t n1 = split_words(b1, sw, 8);
        if (n1 > 0 && is_negation_marker(b, sw[0])) negate1 = 1;
        size_t n2 = split_words(b2, sw, 8);
        if (n2 > 0 && is_negation_marker(b, sw[0])) negate2 = 1;
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

    /* gen254: if the second half only earned repair's clarification ("who or
     * what does 'it' refer to?"), the WHOLE turn may still be answerable by a
     * downstream compound frame (mod_knowledge reads "capital of X, and what
     * year did it ..." in one piece). Prefer that full answer; keep the
     * half+clarification only when nothing downstream claims. */
    if (h1 && h2 && strstr(r2, "refer to?")) {
        char full[1024] = "";
        for (size_t i = 0; i < registry_len; i++) {
            if (strcmp(registry[i].name, "compose") == 0) continue;
            if (strcmp(registry[i].name, "repair") == 0) continue;
            if (registry[i].handle(b, canon, input, full, sizeof full)) {
                if (!strstr(full, "refer to?")) {
                    snprintf(out, out_size, "%s", full);
                    if (b) {
                        snprintf(b->last_reply, sizeof b->last_reply, "%s", full);
                        snprintf(b->last_module, sizeof b->last_module, "%s",
                                 registry[i].name);
                    }
                    return 1;
                }
                break;
            }
        }
    }

    /* gen349: a handler that reads the whole turn from `input` (e.g.
     * mod_wordproblem) answers both halves identically — emit it once, not
     * "15 dollars. 15 dollars.". */
    if (r1[0] && !strcmp(r1, r2))
        snprintf(out, out_size, "%s", r1);
    else
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
/* gen335 (long-conversation, R2 — F.'s steer: entities ACCUMULATE, so "not that one
 * but the one before" must resolve): the mentioned entities are an ORDERED HISTORY of
 * KB facts entity_mentioned(Name, Seq) — unbounded and queryable — NOT a single C
 * field (b->last_entity was the debt the plan names). A pronoun resolves to a POSITION
 * in that history (most recent by default; a back-reference "before"/"earlier"/"prima"
 * steps one earlier), open to more ordinals taught later. Seq lives in the REFLECTIVE
 * layer (entity_seq_max/1), never persisted. */
static long entity_max_seq(Brain *b, const char *name) {
    const char *q[2] = { name, NULL };
    char v[16][KB_TERM_LEN];
    size_t n = kb_match(b->kb, "entity_mentioned", q, 2, v, 16);
    long m = -1;
    for (size_t i = 0; i < n; i++) { long s = strtol(v[i], NULL, 10); if (s > m) m = s; }
    return m;
}
static long next_entity_seq(Brain *b) {
    const char *q[1] = { NULL };
    char v[1][KB_TERM_LEN]; long n = 0;
    if (kb_match(b->kb, "entity_seq_max", q, 1, v, 1) == 1) n = strtol(v[0], NULL, 10);
    long nx = n + 1;
    kb_set_origin(b->kb, KB_REFLECTIVE);
    if (n > 0) { char ob[24]; snprintf(ob, sizeof ob, "%ld", n);
                 const char *rq[1] = { ob }; kb_retract(b->kb, "entity_seq_max", rq, 1); }
    char nb[24]; snprintf(nb, sizeof nb, "%ld", nx);
    const char *na[1] = { nb }; kb_assert(b->kb, "entity_seq_max", na, 1);
    return nx;
}
/* Record a mentioned entity into the ordered KB history (lowercased). Skips a dup of
 * the current most-recent so a repeated mention does not spam the sequence. */
static void note_entity_seq(Brain *b, const char *raw_name) {
    if (!b || !b->kb || !raw_name) return;
    char low[KB_TERM_LEN]; size_t j = 0;
    for (size_t i = 0; raw_name[i] && j + 1 < sizeof low; i++) {
        unsigned char c = (unsigned char)raw_name[i];
        if (isalnum(c) || c == '_') low[j++] = (char)tolower(c);
    }
    low[j] = '\0';
    if (j < 2) return;
    const char *sq[1] = { NULL }; char sv[1][KB_TERM_LEN]; long top = 0;
    if (kb_match(b->kb, "entity_seq_max", sq, 1, sv, 1) == 1) top = strtol(sv[0], NULL, 10);
    if (entity_max_seq(b, low) == top && top > 0) return;    /* already most recent */
    long s = next_entity_seq(b);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    char sb[24]; snprintf(sb, sizeof sb, "%ld", s);
    const char *a[2] = { low, sb }; kb_assert(b->kb, "entity_mentioned", a, 2);
}

static int coref_resolve(Brain *b, const char *canon, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char nm[64][KB_TERM_LEN]; const char *q0[2] = { NULL, NULL };
    size_t nn = kb_match(b->kb, "entity_mentioned", q0, 2, nm, 64);
    if (nn == 0) return 0;
    char names[64][KB_TERM_LEN]; long seqs[64]; size_t nh = 0;
    for (size_t i = 0; i < nn && nh < 64; i++) {
        long s = entity_max_seq(b, nm[i]); if (s < 0) continue;
        snprintf(names[nh], KB_TERM_LEN, "%s", nm[i]); seqs[nh] = s; nh++;
    }
    for (size_t i = 1; i < nh; i++) {                 /* insertion sort, seq desc */
        char tn[KB_TERM_LEN]; snprintf(tn, sizeof tn, "%s", names[i]); long ts = seqs[i];
        size_t k = i;
        while (k > 0 && seqs[k - 1] < ts) {
            snprintf(names[k], KB_TERM_LEN, "%s", names[k - 1]); seqs[k] = seqs[k - 1]; k--;
        }
        snprintf(names[k], KB_TERM_LEN, "%s", tn); seqs[k] = ts;
    }

    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;                             /* a bare pronoun is not a query */
    size_t pidx = nw;
    for (size_t i = 0; i < nw; i++) {
        const char *t = w[i];
        if (lex_class_member(b, "99_registry_lex2183", t)||lex_class_member(b, "99_registry_lex2183_2", t)||lex_class_member(b, "99_registry_lex2183_3", t)||lex_class_member(b, "99_registry_lex2183_4", t)||
            lex_class_member(b, "99_registry_lex2184", t)||lex_class_member(b, "99_registry_lex2184_2", t)||lex_class_member(b, "99_registry_lex2184_3", t)) { pidx = i; break; }
    }
    if (pidx == nw) return 0;

    size_t offset = 0;                               /* ordinal into the history */
    if (kb_cue_match(b, "99_registry_lex2189", canon)||kb_cue_match(b, "99_registry_lex2189_2", canon)||kb_cue_match(b, "99_registry_lex2189_3", canon)||kb_cue_match(b, "99_registry_lex2189_4", canon)||kb_cue_match(b, "99_registry_lex2190", canon))
        offset = 1;
    if (offset >= nh) return 0;
    const char *ent = names[offset];

    char rw[256]; size_t off = 0; rw[0] = '\0';
    for (size_t i = 0; i < nw && off + 1 < sizeof rw; i++) {
        const char *tok = (i == pidx) ? ent : w[i];
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
    for (size_t i = 0; i < nw; i++) if (lex_class_member(b, "99_registry_lex2234", w[i])) { mi = i; break; }
    if (mi == nw || mi + 1 >= nw) return 0;

    /* the key is the run of words after "my", stopping at the first operator or
     * number — a key names a thing, it is not part of the computation. */
    size_t run = 0;
    while (mi + 1 + run < nw) {
        const char *t = w[mi + 1 + run];
        double dv;
        if (arith_op_char(b, t) || parse_value(t, &dv)) break;
        run++;
    }
    if (run == 0) return 0;
    /* an operator must follow the key (else it is a recall, not a computation). */
    if (mi + 1 + run >= nw || !arith_op_char(b, w[mi + 1 + run])) return 0;

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
/* ── gen387: LA DOMANDA DI SEGUITO CON IL SOGGETTO ELISO ────────────────────
 *
 * In conversazione reale la seconda domanda su un argomento non lo ripete:
 *
 *     you> quante sono le carte del poker   ->  A poker has 52 cards.
 *     you> e quanti giocatori               ->  Non capisco ancora.
 *
 * Non e' una lacuna di conoscenza — `game_players(poker, …)` c'e' e «how many
 * players in poker» risponde. Manca la continuita': il soggetto e' rimasto due
 * turni indietro. E' lo stesso sintomo del pronome implicito, in una forma piu'
 * dura, perche' qui non c'e' nemmeno un pronome da risolvere.
 *
 * Meccanismo gemello di `continue_resolve` (gen222), che fa esattamente questo
 * per le code aritmetiche: si sbuccia un connettore INIZIALE — obbligatorio,
 * cosi' scatta solo su una continuazione dichiarata e mai su una domanda nuova —
 * si verifica che il residuo non nomini gia' un'entita' propria, e si riscrive
 * il turno con l'entita' saliente in coda. Conservativo come i suoi fratelli:
 * rivendica solo se un modulo risponde davvero al turno riscritto, quindi non
 * dirotta mai un turno che gia' funziona. Glue come sostituzione deterministica
 * su stato reale, mai continuita' plausibile inventata (PRINCIPLES anti-impostore).
 *
 * Quali parole siano connettori e' conoscenza (`conjunction/1`), non un elenco. */
static int topic_continue_resolve(Brain *b, const char *canon,
                                  char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    /* L'ARGOMENTO SALIENTE. `last_entity` lo porta quando a rispondere e' stata
     * una facolta' che lo registra; ma non tutte lo fanno — «quante sono le
     * carte del poker» passa da mod_quantity, che risponde di poker e non lo
     * segna. Il topic non sopravviveva fra facolta', che e' esattamente il
     * sintomo «piu' sistemi indipendenti invece di un interlocutore».
     *
     * Finche' ogni facolta' non registra il proprio argomento, si ricade sul
     * TURNO PRECEDENTE: si cerca li' l'ultima entita' che la KB conosce. E'
     * stato reale e ispezionabile, non una continuita' inventata. */
    char topic[KB_TERM_LEN] = "";
    if (b->has_last_topic && b->last_topic[0])
        snprintf(topic, sizeof topic, "%s", b->last_topic);
    else if (b->has_last_entity && b->last_entity[0])
        snprintf(topic, sizeof topic, "%s", b->last_entity);
    else if (b->has_last_input && b->last_input_canon[0]) {
        char prev[256]; snprintf(prev, sizeof prev, "%s", b->last_input_canon);
        char *pw[64]; size_t npw = split_words(prev, pw, 64);
        for (size_t k = npw; k-- > 0; ) {
            char d[256];
            char *tk = strip_edge_punct(pw[k]);
            if (strlen(tk) < 3 || is_stopword(b, tk)) continue;
            if (kb_describe_entity(b->kb, tk, d, sizeof d)) {
                snprintf(topic, sizeof topic, "%s", tk);
                break;
            }
        }
    }
    if (!topic[0]) return 0;

    char buf[256]; size_t len = strlen(canon);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, canon, len + 1);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    if (nw < 2) return 0;

    const char *cq[] = { strip_edge_punct(w[0]) };
    if (!kb_query(b->kb, "conjunction", cq, 1)) return 0;   /* serve un connettore */

    /* Il residuo non deve gia' nominare un'entita': «e parlami del bridge» e'
     * una domanda su un ALTRO argomento, non la continuazione di questo. */
    for (size_t k = 1; k < nw; k++) {
        char d[256];
        char *tk = strip_edge_punct(w[k]);
        if (strlen(tk) < 3) continue;
        if (kb_describe_entity(b->kb, tk, d, sizeof d)) return 0;
    }

    char residue[256]; size_t off = 0; residue[0] = '\0';
    for (size_t k = 1; k < nw && off + 1 < sizeof residue; k++)
        off += (size_t)snprintf(residue + off, sizeof residue - off,
                                "%s%s", k > 1 ? " " : "", strip_edge_punct(w[k]));
    if (!residue[0]) return 0;

    char rw[320];
    snprintf(rw, sizeof rw, "%s %s", residue, topic);
    for (size_t r = 0; r < registry_len; r++) {
        if (strcmp(registry[r].name, "repair") == 0) continue;
        if (registry[r].handle(b, rw, rw, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", registry[r].name);
            return 1;
        }
    }
    return 0;
}

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
    while (i < nw && (lex_class_member(b, "99_registry_lex2433", w[i]) || lex_class_member(b, "99_registry_lex2433_2", w[i]) ||
                      lex_class_member(b, "99_registry_lex2434", w[i]) || lex_class_member(b, "99_registry_lex2434_2", w[i]) ||
                      lex_class_member(b, "99_registry_lex2435", w[i]) || lex_class_member(b, "99_registry_lex2435_2", w[i]) ||
                      lex_class_member(b, "99_registry_lex2436", w[i]))) i++;
    if (i >= nw || !arith_op_char(b, w[i])) return 0;      /* must lead with an operator */

    int saw_num = 0;                                    /* rest must be a pure arith tail */
    for (size_t k = i; k < nw; k++) {
        double dv;
            if (arith_op_char(b, w[k]) || lex_class_member(b, "99_registry_lex2442", w[k])) continue;
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

/* gen240 (universal-comprehension): the session CONVERSATION LOG. Every turn — what
 * the user said and what parrot0 replied — becomes session KB knowledge as
 * utterance(Seq, Speaker, "text") (Seq monotonic; Speaker user|self), so the
 * dialogue itself is queryable/inferable ("what was the last thing you said?",
 * "the first sentence I said"). Bounded: the oldest beyond a window is retracted.
 * Logged at end-of-turn, so a recall query sees up to the PREVIOUS turn (the
 * current question is not yet logged when it is being answered). */
#define CONV_LOG_WINDOW 80
static void conv_log_one(Brain *b, const char *speaker, const char *text) {
    if (!b || !b->kb || !text || !*text) return;
    /* gen417b: un turno RIPOSTO per verificare una riparazione non e'
     * conversazione. Senza questa riga, la verifica di non-regressione
     * inquinerebbe il log con i propri tentativi — e siccome legge il log per
     * decidere, si guarderebbe le mani mentre le muove. */
    {
        const char *rq[1] = { "1" };
        if (kb_query(b->kb, "repairing", rq, 1)) return;
    }
    char t[KB_TERM_LEN]; size_t o = 0;
    for (const char *c = text; *c && o + 4 < sizeof t; c++) {  /* leave room for quotes */
        char ch = *c;
        if (ch == '"') ch = '\'';
        if (ch == '\n' || ch == '\r' || ch == '\t') ch = ' ';
        t[o++] = ch;
    }
    t[o] = '\0';
    if (o == 0) return;
    char seq[24]; snprintf(seq, sizeof seq, "%ld", ++b->utter_seq);
    char quoted[KB_TERM_LEN + 4]; snprintf(quoted, sizeof quoted, "\"%s\"", t);
    kb_set_origin(b->kb, KB_SESSION);
    const char *a[] = { seq, speaker, quoted };
    kb_assert(b->kb, "utterance", a, 3);
    if (b->utter_seq > CONV_LOG_WINDOW) {                 /* drop the one out of window */
        char old[24]; snprintf(old, sizeof old, "%ld", b->utter_seq - CONV_LOG_WINDOW);
        char sp[1][KB_TERM_LEN]; const char *qs[] = { old, NULL, NULL };
        if (kb_match(b->kb, "utterance", qs, 3, sp, 1) > 0) {
            char tx[1][KB_TERM_LEN]; const char *qt[] = { old, sp[0], NULL };
            if (kb_match(b->kb, "utterance", qt, 3, tx, 1) > 0) {
                const char *r[] = { old, sp[0], tx[0] };
                kb_retract(b->kb, "utterance", r, 3);
            }
        }
    }
}
static void conv_log(Brain *b, const char *input, const char *reply) {
    conv_log_one(b, "user", input);
    conv_log_one(b, "self", reply);
}

/* gen404: L'USCITA UNICA DI UN TURNO.
 *
 * `brain_respond` aveva undici punti di ritorno, ognuno con la stessa coppia di
 * chiusure copiata a mano. Finche' erano due effetti innocui la duplicazione
 * costava poco; appena si e' aggiunta la terza — chiudere la lacuna quando il
 * turno RISPONDE — e' costata un bug silenzioso: la lacuna si chiudeva solo sui
 * turni che arrivavano in fondo alla funzione, e una risposta prodotta da una
 * scorciatoia lasciava il registro sporco per sempre.
 *
 * La chiusura non e' un giudizio di nessuno: una lacuna di macchineria e'
 * decidibile proprio perche' la prova del rimedio e' che il turno che murava ora
 * risponde. Senza di essa il registro sarebbe monotono crescente — un elenco di
 * rimpianti invece di una misura. */
static size_t turn_done(Brain *b, const char *canon, const char *input,
                        char *out, size_t out_size) {
    if (b && b->kb) kb_saturation_commit(b->kb);
    /* A result produced after a bounded enumeration may have been based on an
     * amputated view.  The KB decides which caps are consequential and which
     * response family says so; C only applies the general guard. */
    if (b && b->kb && out && *out) {
        char response[1][KB_TERM_LEN];
        const char *rq[1] = { NULL };
        int saturated = 0;
        char rows[32][KB_TERM_LEN];
        const char *sq[3] = { NULL, NULL, NULL };
        size_t ns = kb_match(b->kb, "saturated_read", sq, 3, rows, 32);
        for (size_t i = 0; i < ns; i++) {
            char arity[1][KB_TERM_LEN], cap[1][KB_TERM_LEN];
            const char *aq[3] = { rows[i], NULL, NULL };
            if (kb_match(b->kb, "saturated_read", aq, 3, arity, 1) != 1)
                continue;
            const char *cq[3] = { rows[i], arity[0], NULL };
            if (kb_match(b->kb, "saturated_read", cq, 3, cap, 1) != 1)
                continue;
            const char *gq[2] = { rows[i], cap[0] };
            if (kb_query(b->kb, "saturation_guard", gq, 2)) {
                saturated = 1; break;
            }
        }
        if (saturated && kb_match(b->kb, "saturation_response", rq, 1,
                                  response, 1) == 1) {
            char family[KB_TERM_LEN];
            snprintf(family, sizeof family, "%s", response[0]);
            if (kb_response_slots(b, kb_dequote(family), NULL, 0,
                                  out, out_size))
                snprintf(b->last_module, sizeof b->last_module,
                         "%s", "saturation_guard");
        }
    }
    note_arith_result(b, out);
    /* gen434: CHI ha risposto, e — se nessuno ha registrato un esito — che il
     * turno e' stato considerato RISPOSTO. E' la terza gamba della derivazione,
     * ed e' quella che rende visibile la classe silenziosa: un turno «risposto»
     * da una facolta' della famiglia dei template e' un sospetto, non un
     * successo (docs/plans/fix-patterns.md, forma G). */
    if (b && b->kb) {
        int prev = kb_origin(b->kb);
        kb_set_origin(b->kb, KB_REFLECTIVE);
        if (b->last_module[0]) {
            const char *ma[2] = { "current_turn", b->last_module };
            kb_assert(b->kb, "turn_module", ma, 2);
        }
        const char *oq[2] = { "current_turn", NULL };
        char seen[1][KB_TERM_LEN];
        if (kb_match(b->kb, "turn_outcome", oq, 2, seen, 1) == 0) {
            const char *oa[2] = { "current_turn", "answered" };
            kb_assert(b->kb, "turn_outcome", oa, 2);
        }
        kb_set_origin(b->kb, prev);
    }
    if (b && b->kb && strcmp(b->last_module, "fallback") != 0)
        machinery_gap_close(b, canon);
    conv_log(b, input, out);
    return strlen(out);
}

/* ── gen386: IL VINCOLO POSTO PRIMA PLASMA LA RISPOSTA DOPO ─────────────────
 *
 * E' l'ultimo dei cinque sintomi di the-linguistic-glue.md, qualitativo dal
 * gen222: «risposte corrette ma fuori contesto». "keep it short" veniva
 * riconosciuto e CONFERMATO — «Got it: I will keep it short.» — e poi la
 * risposta successiva era lunga come prima. Il vincolo era registrato e mai
 * applicato: peggio che ignorarlo, perche' prometteva.
 *
 * Qui il vincolo attivo si legge dalla KB (`active_constraint/1`, fatto di
 * sessione: persiste, si interroga, si ritira parlando) e la FORMA che impone e'
 * conoscenza a due livelli — quale operazione (`constraint_shape/2`) e con quale
 * misura (`constraint_limit/3`). Il motore sa solo eseguire operazioni generali
 * su un testo: prendere la prima frase, tenere N parole. Un vincolo nuovo —
 * "una riga sola", "niente elenchi" — e' una manciata di fatti.
 *
 * Applicato in UN punto, dopo il dispatch, perche' brain_respond ha undici
 * uscite e un vincolo che vale solo per alcune non e' un vincolo. */
static void apply_active_constraint(Brain *b, char *out, size_t out_size) {
    if (!b || !b->kb || !out || !*out) return;

    char cons[4][KB_TERM_LEN];
    const char *cq[1] = { NULL };
    size_t nc = kb_match(b->kb, "active_constraint", cq, 1, cons, 4);
    for (size_t i = 0; i < nc; i++) {
        /* Un vincolo di FORMA non deve mutilare una risposta il cui contenuto e'
         * per sua natura un elenco richiesto: «che cosa ricordi di me?» chiede
         * tutto, e accorciarlo toglie proprio cio' che era stato domandato.
         * L'esenzione e' un fatto sulla FACOLTA' che ha risposto, non un caso
         * speciale nel codice: `constraint_exempt(Vincolo, Modulo)`. */
        if (b->last_module[0]) {
            const char *eq[2] = { cons[i], b->last_module };
            if (kb_query(b->kb, "constraint_exempt", eq, 2)) continue;
        }
        char shape[1][KB_TERM_LEN];
        const char *sq[2] = { cons[i], NULL };
        if (kb_match(b->kb, "constraint_shape", sq, 2, shape, 1) != 1) continue;
        char opbuf[KB_TERM_LEN];
        snprintf(opbuf, sizeof opbuf, "%s", shape[0]);
        const char *op = kb_dequote(opbuf);

        long limit = 0;
        {
            char lim[1][KB_TERM_LEN];
            const char *lq[3] = { cons[i], "words", NULL };
            if (kb_match(b->kb, "constraint_limit", lq, 3, lim, 1) == 1)
                limit = atol(lim[0]);
        }

        if (!strcmp(op, "first_sentence")) {
            /* La prima frase e' gia' una risposta completa: accorciare cosi'
             * non tronca un pensiero a meta', che e' il modo sbagliato di
             * obbedire a "sii breve". */
            for (size_t k = 0; out[k]; k++) {
                int stop = (out[k] == '.' && (out[k+1] == '\0' || out[k+1] == ' '));
                /* Il punto e virgola separa due risposte complete nelle
                 * descrizioni della KB ("X e' A; X e' B"): la prima e' gia'
                 * una risposta intera, ed e' un confine migliore di un taglio a
                 * numero di parole, che cade a meta' sintagma. */
                if (out[k] == ';') {
                    if (k && out[k-1] == '.') out[k] = '\0';   /* non raddoppiare il punto */
                    else { out[k] = '.'; out[k+1] = '\0'; }
                    break;
                }
                if (stop) { out[k+1] = '\0'; break; }
            }
        }
        if (limit > 0) {
            size_t words = 0;
            for (size_t k = 0; out[k]; k++) {
                if (out[k] == ' ') {
                    words++;
                    if ((long)words >= limit) {
                        /* si taglia a parola intera e si chiude la frase */
                        out[k] = '\0';
                        size_t l = strlen(out);
                        while (l && (out[l-1] == ',' || out[l-1] == ';')) out[--l] = '\0';
                        if (l && out[l-1] != '.' && l + 4 < out_size)
                            snprintf(out + l, out_size - l, " ...");
                        break;
                    }
                }
            }
        }
    }
}

static size_t brain_respond_dispatch(Brain *b, const char *input,
                                     char *out, size_t out_size);

/* gen396: project the ONE universal input into the KB.  input_segment already
 * owns the fixed byte mechanics; every boundary, role and cue it returns was
 * selected by live KB evidence.  This adapter merely reifies that result and
 * asks the KB whether the spans compose a complete response plan.  It knows no
 * conditional connector, natural-language predicate, register or branch form. */
static int turn_quote(const char *src, size_t start, size_t len,
                      char *out, size_t out_size) {
    if (!src || !out || out_size < 3) return 0;
    size_t end = start + len;
    while (start < end && isspace((unsigned char)src[start])) start++;
    while (end > start && isspace((unsigned char)src[end - 1])) end--;
    if (end - start + 3 > out_size) return 0;
    size_t o = 0;
    out[o++] = '"';
    for (size_t i = start; i < end; i++)
        out[o++] = src[i] == '"' ? '\'' : src[i];
    out[o++] = '"';
    out[o] = '\0';
    return 1;
}

/* gen396: the word-shaped tokens of ONE span, as facts.
 *
 * Word boundaries are byte mechanics, which is why they are here; WHICH token a
 * turn is about is not, which is why nothing here chooses one. The historical
 * state consumer kept only the last identifier of the question span and asked
 * the evaluator about that, so «what is total in the end» threw away the name it
 * had just been given. Publishing every candidate lets the KB decide by JOIN —
 * against what the trace actually binds — instead of by position.
 *
 * A run is any alphanumeric/underscore sequence, so `7` is published beside
 * `total`. The first cut only published identifier-shaped runs, which was the C
 * deciding a LEXICAL CLASS — and a declared obligation («expected 7») needs the
 * numeral that the filter dropped. Which words are names and which are values is
 * now a KB question, answered by the only operator that has to tell them apart:
 * a token an obligation can compare against is one arithmetic accepts.
 *
 * The token is stored QUOTED, like turn_span_surface. That is not cosmetic: a
 * bare atom makes the turn's own words look like KB terms, and the informed wall
 * — which names the first word it holds no facts about — then finds "facts"
 * about every word of the sentence it is failing on. Working memory records what
 * was SAID; only a rule may promote a surface to a term. */
#define TURN_MAX_TOKENS 24
static void turn_publish_tokens(Brain *b, const char *surface,
                                const InputSpan *span, const char *index) {
    size_t start = span->start + (span->cue_len > span->len ? span->len
                                                            : span->cue_len);
    size_t end = span->start + span->len;
    /* gen419b — I SEPARATORI DICHIARATI TENGONO INSIEME IL TOKEN.
     *
     * gen399 aveva gia' scoperto la regola su un carattere: un punto FRA DUE
     * CIFRE appartiene al numero, altrimenti «3.14» si spezzava e «which is
     * greater» rispondeva «41». Lo stesso vale per i due punti di «14:30» e per
     * i trattini di «2024-02-28» — e la differenza e' che quei separatori sono
     * gia' dichiarati in KB dal transcoder (`transcode_shape/3`).
     *
     * Quindi la regola si generalizza senza cablare un secondo carattere: una
     * notazione nuova aggiunge una riga di KB e il tokenizzatore la segue. Se un
     * token non si tiene insieme, il transcoder non lo vede mai. */
    char seps[16]; size_t nsep = 0;
    {
        char shapes[16][KB_TERM_LEN];
        const char *nq[3] = { NULL, NULL, NULL };
        size_t ns = kb_match(b->kb, "transcode_shape", nq, 3, shapes, 16);
        for (size_t i = 0; i < ns && nsep + 1 < sizeof seps; i++) {
            const char *sq[3] = { shapes[i], NULL, NULL };
            char sv[1][KB_TERM_LEN];
            if (kb_match(b->kb, "transcode_shape", sq, 3, sv, 1) != 1) continue;
            char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", sv[0]);
            const char *sc = kb_dequote(sb);
            if (sc && *sc && !memchr(seps, sc[0], nsep)) seps[nsep++] = sc[0];
        }
    }
    size_t k = 0;
    for (size_t p = start; p < end && k < TURN_MAX_TOKENS; ) {
        if (!(isalnum((unsigned char)surface[p]) || surface[p] == '_')) { p++; continue; }
        size_t t = p;
        /* gen399: un punto FRA DUE CIFRE appartiene al numero. Spezzando «3.14»
         * in «3» e «14» la memoria di lavoro non registrava piu' cio' che il
         * turno aveva detto — e «which is greater, 3.14 or 3.41?» rispondeva
         * «41», che e' un pezzo di una parola. Il confine e' stretto apposta: il
         * punto di fine frase non ha una cifra dopo, quindi resta un confine. */
        while (p < end && (isalnum((unsigned char)surface[p]) || surface[p] == '_' ||
                           ((surface[p] == '.' || memchr(seps, surface[p], nsep)) &&
                            p > t && p + 1 < end &&
                            isdigit((unsigned char)surface[p - 1]) &&
                            isdigit((unsigned char)surface[p + 1])))) p++;
        char tok[KB_TERM_LEN];
        if (!turn_quote(surface, t, p - t, tok, sizeof tok)) continue;
        char pos[24];
        snprintf(pos, sizeof pos, "%zu", k++);
        const char *args[] = { "current_turn", index, pos, tok };
        kb_assert(b->kb, "turn_span_token", args, 4);
    }
}

/* gen396: the state a span ENDS IN, as facts.
 *
 * The evaluator is fixed mechanics and stays fixed: it is asked what the trace
 * leaves bound, never which binding the turn wants. The KB decides that a span
 * role HAS a state model at all (`span_state_model/2`), so retracting one fact
 * turns the whole projection off at runtime — the C names no register, no
 * language and no question. */
#define TURN_MAX_BINDINGS 16
static void turn_publish_state(Brain *b, const char *surface,
                               const InputSpan *span, const char *index) {
    const char *model[] = { span->role, NULL };
    char kind[1][KB_TERM_LEN];
    if (!span->role[0] ||
        kb_match(b->kb, "span_state_model", model, 2, kind, 1) != 1) return;
    if (span->len >= 65536) return;
    char *trace = malloc(span->len + 1);
    if (!trace) return;
    memcpy(trace, surface + span->start, span->len);
    trace[span->len] = '\0';

    char names[TURN_MAX_BINDINGS][KB_TERM_LEN];
    long vals[TURN_MAX_BINDINGS];
    size_t nb = code_eval_state_bindings(trace, names, vals, TURN_MAX_BINDINGS);
    free(trace);
    for (size_t i = 0; i < nb; i++) {
        /* Quoted for the same reason the tokens are: a binding NAME observed in
         * a snippet is surface the turn carried, not a term the KB knows. */
        char name[KB_TERM_LEN], value[64];
        snprintf(name, sizeof name, "\"%s\"", names[i]);
        snprintf(value, sizeof value, "\"%ld\"", vals[i]);
        const char *args[] = { "current_turn", index, name, value };
        kb_assert(b->kb, "turn_span_binding", args, 4);
    }
}

/* gen393: WHICH KB-declared surfaces occur in this turn, as facts.
 *
 * The turn's own words are already published (turn_span_token); what was missing
 * is the other half of a frame — which surface the KB itself declares, and that
 * the turn contains. Finding a declared string inside a byte range is mechanics,
 * which is why it is here; WHICH strings are worth looking for is knowledge,
 * which is why the C reads them from `turn_cue_registry(Relation, Position)` and
 * knows no cue, language, relation or domain. A new registry is one fact.
 *
 * The frame is materialized for EVERY turn, before any first-match dispatch.
 * That is the gen393 gate: a faculty must be able to consume a frame instead of
 * recognizing the sentence again, and a frame that only some surfaces produce
 * would leave the others with a private path.
 *
 * The cue is stored QUOTED, for the same reason turn_span_token is. Published
 * bare, the turn's own words start looking like KB terms: the informed wall —
 * which names the first word it holds no facts about — found "capital" inside
 * this very fact and fell back to the generic wall on the sentence it was
 * failing. Working memory records what was SEEN; only a rule may promote a
 * surface to a term, and `turn_cue_form/3` is that rule. */
#define TURN_MAX_CUES 512
/* Pubblica le regole morte come fatti interrogabili. Vedi la chiamata. */
static void dead_rules_publish(Brain *b) {
    if (!b || !b->kb) return;
    static char heads[256][KB_TERM_LEN];
    static char missing[256][KB_TERM_LEN];
    size_t n = kb_dead_rules(b->kb, heads, missing, 256);
    kb_set_origin(b->kb, KB_REFLECTIVE);
    for (size_t i = 0; i < n; i++) {
        const char *a[2] = { heads[i], missing[i] };
        kb_assert(b->kb, "dead_rule", a, 2);
        /* La vista unaria e' un FATTO, non una regola. `findall` su un predicato
         * derivato non raccoglieva niente e la risposta spariva in silenzio —
         * la stessa famiglia del bug di cattura del gen382o. Un elenco che
         * qualcuno dovra' scorrere e' meglio che sia materializzato. */
        const char *u[1] = { heads[i] };
        kb_assert(b->kb, "inert_rule", u, 1);
    }
    /* Il CONTO lo pubblica il motore, che ce l'ha gia'. Farlo contare alla KB
     * con `count_list` su un elenco di centinaia di elementi significa una
     * ricorsione profonda quanto l'elenco, e il turno non tornava affatto. */
    {
        char num[32];
        snprintf(num, sizeof num, "%zu", n);
        const char *c[1] = { num };
        kb_assert(b->kb, "inert_rule_count", c, 1);
    }
    kb_set_origin(b->kb, KB_SESSION);
}

/* La richiesta di ripararsi e' una cue come tutte le altre: sta nella KB. */
static int repair_requested(Brain *b, const char *canon, const char *input) {
    if (!b || !b->kb) return 0;
    char cues[12][KB_TERM_LEN];
    const char *q[1] = { NULL };
    size_t n = kb_match(b->kb, "self_repair_cue", q, 1, cues, 12);
    for (size_t i = 0; i < n; i++) {
        const char *needle = kb_dequote(cues[i]);
        if (*needle && (cue(canon, needle) || (input && cue(input, needle))))
            return 1;
    }
    return 0;
}

/* ── gen410: PROPONI E PROVA — il ciclo che chiude una lacuna da solo ──────
 *
 * Barriera C del piano (question-emergence.md), e i pezzi c'erano tutti da
 * prima: `KB_HYPOTHETICAL` per asserire senza impegno, `kb_retract_origin` per
 * ritirare, il registro delle lacune del gen404 per sapere COSA riprovare,
 * l'ancora del gen406 per sapere dove si e' fermato, e le forme-ponte del
 * gen409 per sapere che cosa proporre. Mancava solo che qualcuno li mettesse in
 * fila.
 *
 * Il ciclo, per ogni lacuna aperta:
 *
 *   1. genera i CANDIDATI — i prefissi del turno, da due a cinque parole, piu'
 *      il turno intero. Non e' una scelta furba: una cue e' quasi sempre
 *      l'apertura di cio' che si dice, e le aperture di un turno sono poche;
 *   2. per ogni forma-ponte dichiarata, ASSERISCE il candidato in ipotetico;
 *   3. RIPONE il turno che murava;
 *   4. se ora risponde, `turn_done` ritira la lacuna da solo (gen404) — quindi
 *      la verifica non e' un giudizio di nessuno: si guarda se il fatto e'
 *      sparito. Il candidato viene promosso a indotto e si passa alla lacuna
 *      successiva;
 *   5. altrimenti si ritira e non e' mai esistito.
 *
 * Il caso che l'ha fatto nascere e' una conversazione vera di F.: «siamo in
 * spiaggia» funzionava e «siamo al mare» no, per una cue che manca. E la stessa
 * riga chiude «dove hai fallito» — la superficie con cui parrot0 racconta le
 * proprie lacune, che era un frasario come tutti gli altri.
 *
 * Il costo e' un turno per tentativo, quindi questo NON gira dentro una
 * conversazione ordinaria: si chiede, oppure lo innesca il sogno. */
#define REPAIR_MAX_TRIES 240

static size_t repair_candidates(const char *turn, char out[][KB_TERM_LEN],
                                size_t max) {
    char buf[KB_TERM_LEN];
    snprintf(buf, sizeof buf, "%s", turn);
    char *w[32];
    size_t nw = split_words(buf, w, 32);
    size_t n = 0;
    for (size_t k = 2; k <= 5 && k <= nw && n < max; k++) {
        size_t o = 0;
        out[n][0] = '\0';
        for (size_t i = 0; i < k; i++)
            o += (size_t)snprintf(out[n] + o, KB_TERM_LEN - o, "%s%s",
                                  i ? " " : "", strip_edge_punct(w[i]));
        if (out[n][0]) n++;
    }
    if (n < max && nw > 5) snprintf(out[n++], KB_TERM_LEN, "%s", turn);
    return n;
}

/* I valori del SECONDO argomento che un registro usa gia'. Un ponte a due
 * argomenti («questa cue significa quell'operazione») non si inventa: si prova
 * a comportarsi come una cue che quel registro ha gia'. */
static size_t repair_second_args(Brain *b, const char *reg,
                                 char out[][KB_TERM_LEN], size_t max) {
    char firsts[24][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t nf = kb_match(b->kb, reg, q, 2, firsts, 24);
    size_t n = 0;
    for (size_t i = 0; i < nf && n < max; i++) {
        char val[1][KB_TERM_LEN];
        const char *vq[2] = { firsts[i], NULL };
        if (kb_match(b->kb, reg, vq, 2, val, 1) < 1) continue;
        int dup = 0;
        for (size_t j = 0; j < n; j++) if (!strcmp(out[j], val[0])) dup = 1;
        if (!dup) snprintf(out[n++], KB_TERM_LEN, "%s", val[0]);
    }
    return n;
}

/* gen410 — UN NUOVO MEMBRO DI UNA CLASSE DEVE ASSOMIGLIARE ALLA CLASSE.
 *
 * La verifica «ora il turno risponde» e' necessaria e NON basta, e il caso che
 * l'ha dimostrato e' istruttivo: proponendo `setting_cue("dove hai")` la domanda
 * «dove hai fallito» improvvisamente rispondeva — «Ci siamo, siamo in fallito»
 * — cioe' veniva letta come la dichiarazione di un luogo. Il ponte passava la
 * prova e diceva una sciocchezza. E' la falsificazione scritta nel piano: una
 * proposta che chiude il muro rispondendo a caso e' peggio del muro.
 *
 * Il criterio in piu' e' quello che questo progetto usa dappertutto: una cue
 * nuova e' un nuovo MEMBRO di una classe, e un membro assomiglia alla classe.
 * «siamo al» sta con «siamo a» e «siamo in»; «dove hai» sta con «dove ti sei
 * fermato», non con le dichiarazioni di luogo. Condividere la prima parola con
 * una cue che il registro ha gia' e' grezzo, falsificabile e discrimina
 * esattamente questi due casi. */
static int repair_fits_class(Brain *b, const char *reg, const char *cand) {
    char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", cand);
    char *cw[8];
    if (split_words(cb, cw, 8) == 0) return 0;
    const char *head = strip_edge_punct(cw[0]);
    if (!*head) return 0;
    char rows[64][KB_TERM_LEN];
    const char *q1[1] = { NULL };
    size_t n = kb_match(b->kb, reg, q1, 1, rows, 64);
    if (n == 0) {
        const char *q2[2] = { NULL, NULL };
        n = kb_match(b->kb, reg, q2, 2, rows, 64);
    }
    for (size_t i = 0; i < n; i++) {
        char eb[KB_TERM_LEN];
        snprintf(eb, sizeof eb, "%s", kb_dequote(rows[i]));
        char *ew[8];
        if (split_words(eb, ew, 8) == 0) continue;
        if (!strcmp(strip_edge_punct(ew[0]), head)) return 1;
    }
    return 0;
}

/* gen417b — «SE RISPONDE E NIENT'ALTRO REGREDISCE».
 *
 * La seconda meta' del criterio di gen410, mai implementata. Da sola, la prima
 * — «il turno adesso risponde» — non distingue una riparazione da un
 * dirottamento, e accendere l'autocorrezione faceva ricomparire il difetto da
 * una famiglia di cue dopo l'altra: gap_report_cue, poi gap_stop_cue, poi
 * bridge_ask_cue. Sedici famiglie: dichiararle una per una sarebbe chiudere casi
 * a mano.
 *
 * Il vincolo giusto non guarda la famiglia del ponte, guarda l'EFFETTO: si
 * ripongono i turni recenti che gia' rispondevano, e se una risposta cambia il
 * ponte si ritira. Il materiale c'e' — parrot0 tiene il log della conversazione
 * come conoscenza (`utterance/3`) — e questa e' la prima volta che gli serve per
 * decidere invece che per raccontare.
 *
 * I turni si FOTOGRAFANO prima di riporli: `brain_respond` fa crescere il log, e
 * iterare su una lista che si allunga mentre la si legge non finirebbe mai. */
#define REPAIR_CANARIES 4
static int repair_breaks_past(Brain *b, const char *under_repair) {
    if (!b || !b->kb) return 0;
    char turns[REPAIR_CANARIES][KB_TERM_LEN];
    char befores[REPAIR_CANARIES][KB_TERM_LEN];
    size_t nc = 0;
    for (long n = b->utter_seq; n > 1 && nc < REPAIR_CANARIES; n--) {
        char ns[24]; snprintf(ns, sizeof ns, "%ld", n - 1);
        const char *uq[3] = { ns, "user", NULL };
        char txt[1][KB_TERM_LEN];
        if (kb_match(b->kb, "utterance", uq, 3, txt, 1) != 1) continue;
        char rs[24]; snprintf(rs, sizeof rs, "%ld", n);
        const char *rq[3] = { rs, "self", NULL };
        char rep[1][KB_TERM_LEN];
        if (kb_match(b->kb, "utterance", rq, 3, rep, 1) != 1) continue;
        char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", txt[0]);
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", rep[0]);
        const char *tt = kb_dequote(tb);
        /* IL TURNO IN RIPARAZIONE NON E' CANARINO DI SE STESSO. Compare nel log
         * con il suo muro, e riposto adesso risponde — che e' lo scopo. Contarlo
         * come regressione faceva rifiutare ogni riparazione buona (misurato:
         * self_repair.p0t, sei assert rossi). */
        if (under_repair && !strcmp(tt, under_repair)) continue;
        snprintf(turns[nc], KB_TERM_LEN, "%s", tt);
        snprintf(befores[nc], KB_TERM_LEN, "%s", kb_dequote(rb));
        nc++;
    }
    for (size_t i = 0; i < nc; i++) {
        char now[900]; now[0] = '\0';
        unsigned long fb = b->fallbacks;
        brain_respond(b, turns[i], now, sizeof now);
        /* SOLO I TURNI CHE GIA' RISPONDEVANO fanno da canarino. Un turno che era
         * gia' un muro non ha niente da preservare — e per giunta il ripiego
         * varia apposta la frase per non ripetersi, quindi confrontarlo con se
         * stesso darebbe sempre «regredito» e nessuna riparazione passerebbe
         * mai (misurato: self_repair.p0t, sei assert rossi). */
        if (b->fallbacks != fb) continue;
        /* il log normalizza virgolette e a capo: si confronta sulla stessa forma */
        for (char *c = now; *c; c++) {
            if (*c == '"') *c = '\'';
            else if (*c == '\n' || *c == '\r' || *c == '\t') *c = ' ';
        }
        if (strncmp(now, befores[i], strlen(befores[i])) != 0) return 1;
    }
    return 0;
}

static int repair_try(Brain *b, const char *gapq, const char *turn,
                      const char *reg, const char *const *args, size_t argc) {
    kb_set_origin(b->kb, KB_HYPOTHETICAL);
    int ok = kb_assert(b->kb, reg, args, argc);
    kb_set_origin(b->kb, KB_SESSION);
    if (!ok) return 0;
    char reply[2048];
    brain_respond(b, turn, reply, sizeof reply);
    const char *ga[1] = { gapq };
    if (!kb_query(b->kb, "machinery_gap", ga, 1)) {
        /* gen417b: ha risposto — ma la meta' del criterio che conta di piu' e'
         * l'altra. Se una risposta che gia' funzionava e' cambiata, questo non
         * e' un ponte: e' un dirottamento, e va ritirato come se non fosse mai
         * esistito. */
        if (repair_breaks_past(b, turn)) {
            kb_retract(b->kb, reg, args, argc);
            return 0;
        }
        /* Ha risposto e non ha rotto niente: il candidato smette di essere
         * un'ipotesi. */
        kb_retract(b->kb, reg, args, argc);
        kb_set_origin(b->kb, KB_INDUCED);
        kb_assert(b->kb, reg, args, argc);
        /* Il marcatore che distingue il lavoro del ciclo dal resto
         * dell'induzione: i fatti estratti dalla prosa sono conoscenza sul
         * mondo e vanno altrove, questi sono macchineria che parrot0 si e'
         * insegnato. */
        const char *lb[2] = { reg, args[0] };
        kb_assert(b->kb, "learned_bridge", lb, 2);
        kb_set_origin(b->kb, KB_SESSION);
        return 1;
    }
    kb_retract(b->kb, reg, args, argc);
    return 0;
}

/* `only` non NULL = ripara SOLO quella lacuna (gen413).
 *
 * Serve all'autocorrezione sul muro, che avviene dentro il turno: riparare li'
 * tutte le lacune aperte fa diafonia — il turno che ha murato riceve la risposta
 * di un'altra frase, murata dieci turni prima. Misurato su arith.p0t, dove «tell
 * me about c» tornava indietro con il ponte di un'altra lacuna addosso. */
static int self_repair_target(Brain *b, const char *only, char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    char gaps[16][KB_TERM_LEN];
    const char *gq[1] = { NULL };
    size_t ng = kb_match(b->kb, "machinery_gap", gq, 1, gaps, 16);
    if (!ng) return 0;

    char regs[24][KB_TERM_LEN];
    const char *bq[2] = { "cue", NULL };
    size_t nr = kb_match(b->kb, "bridge_shape", bq, 2, regs, 24);

    size_t tries = 0, fixed = 0, o = 0;
    if (out_size) out[0] = '\0';
    for (size_t g = 0; g < ng && tries < REPAIR_MAX_TRIES; g++) {
        char gapq[KB_TERM_LEN];
        snprintf(gapq, sizeof gapq, "%s", gaps[g]);
        if (only) {                       /* una sola lacuna: quella del turno */
            char bare[KB_TERM_LEN];
            snprintf(bare, sizeof bare, "%s", gapq);
            if (strcmp(kb_dequote(bare), only) != 0) continue;
        }
        /* gen418 — IL RIMEDIO SI SCEGLIE PER FORMA DI LACUNA.
         *
         * Finora il ciclo provava le cue su qualunque lacuna, anche dove non
         * possono funzionare per costruzione: una forma incompleta (gen416) non
         * si ripara insegnando una cue — le manca uno SLOT, non un aggancio.
         * Provarci comunque brucia i tentativi e, peggio, puo' far passare per
         * caso un ponte che non c'entra.
         *
         * Quale rimedio per quale forma e' un fatto (`remedy_for/2`): oggi mappa
         * la sola `reachability` sulle cue, che e' la verita' di adesso, e una
         * forma nuova con il suo rimedio costa una riga. Una lacuna la cui forma
         * non ha rimedio dichiarato viene SALTATA — e saltarla dicendolo e'
         * meglio che ripararla a caso. */
        {
            const char *kq[2] = { gapq, NULL };
            char kind[1][KB_TERM_LEN];
            if (kb_match(b->kb, "gap_kind", kq, 2, kind, 1) == 1) {
                char kb2[KB_TERM_LEN];
                snprintf(kb2, sizeof kb2, "%s", kind[0]);
                const char *rq2[2] = { kb_dequote(kb2), "cue" };
                if (!kb_query(b->kb, "remedy_for", rq2, 2)) continue;
            }
        }
        /* Il turno da riporre e' quello VERO, non la sua canonicalizzazione:
         * una riparazione verificata contro una traduzione non e' una
         * riparazione. La lacuna e' indicizzata sul canon — due modi di dire la
         * stessa cosa sono la stessa lacuna — e `gap_source` conserva come e'
         * stata detta davvero. */
        char turn[KB_TERM_LEN];
        {
            const char *sq[2] = { gapq, NULL };
            char src[1][KB_TERM_LEN];
            if (kb_match(b->kb, "gap_source", sq, 2, src, 1) > 0)
                snprintf(turn, sizeof turn, "%s", kb_dequote(src[0]));
            else
                snprintf(turn, sizeof turn, "%s", kb_dequote(gapq));
        }
        char cand[8][KB_TERM_LEN];
        size_t nc = repair_candidates(turn, cand, 8);
        int done = 0;
        for (size_t r = 0; r < nr && !done && tries < REPAIR_MAX_TRIES; r++) {
            char reg[KB_TERM_LEN];
            snprintf(reg, sizeof reg, "%s", kb_dequote(regs[r]));
            /* l'arita' non e' dichiarata: si guarda quella che il registro usa */
            char probe[1][KB_TERM_LEN];
            const char *p1[1] = { NULL };
            int unary = kb_match(b->kb, reg, p1, 1, probe, 1) > 0;
            char vals[8][KB_TERM_LEN];
            size_t nv = unary ? 0 : repair_second_args(b, reg, vals, 8);
            for (size_t c = 0; c < nc && !done && tries < REPAIR_MAX_TRIES; c++) {
                char quoted[KB_TERM_LEN];
                snprintf(quoted, sizeof quoted, "\"%s\"", cand[c]);
                if (!repair_fits_class(b, reg, cand[c])) continue;
                /* gen417c — UN PONTE CHE COPRE TUTTO IL TURNO NON E' UN PONTE.
                 *
                 * `setting_cue("ci sistemiamo al")` e' un pezzo del turno: copre
                 * anche «ci sistemiamo al bivacco», che nessuno ha mai detto.
                 * `bridge_ask_cue("what is gold")` e' il turno INTERO: non
                 * generalizza niente, timbra quella frase dentro un registro e
                 * la fa rispondere da un modulo che non c'entra.
                 *
                 * E' il criterio di falsificazione che il piano si e' dato —
                 * «una per superficie invece che una per classe» — applicato al
                 * candidato invece che al risultato: non si aspetta di scoprire
                 * che il ciclo ha compilato un frasario, gli si impedisce di
                 * cominciare. Vale per tutte e sedici le famiglie di cue senza
                 * nominarne nessuna, che e' la differenza fra un vincolo e
                 * sedici casi chiusi a mano. */
                {
                    size_t lc = strlen(cand[c]), lt = strlen(turn);
                    if (lc >= lt) continue;
                }
                /* gen417 — LA PERTINENZA, come requisito di registro.
                 *
                 * La verifica di gen410 chiede «il turno adesso risponde?», e
                 * quella domanda da sola non distingue una riparazione da un
                 * DIROTTAMENTO. Misurato: davanti a un muro, il ciclo proponeva
                 * `gap_report_cue("what is gold")` — il turno «rispondeva», con
                 * «Nothing walled on me yet», che e' la risposta di un'altra
                 * domanda. Una risposta falsa data con sicurezza e' peggio del
                 * muro che sostituisce (MANTRA #7), ed e' per questo che
                 * l'autocorrezione sul muro e' rimasta spenta.
                 *
                 * Il criterio: certe famiglie di ponte hanno un REGISTRO, e
                 * possono essere proposte solo per turni di quel registro. Un
                 * cue di auto-resoconto non si insegna su una domanda sul mondo.
                 * Quale famiglia richieda quale registro e' un fatto
                 * (`bridge_needs_register/2`), e il registro del turno lo
                 * riconosce lo stesso meccanismo di gen415. */
                {
                    const char *nq[2] = { reg, NULL };
                    char need[1][KB_TERM_LEN];
                    if (kb_match(b->kb, "bridge_needs_register", nq, 2, need, 1) == 1) {
                        char nb2[KB_TERM_LEN];
                        snprintf(nb2, sizeof nb2, "%s", need[0]);
                        const char *want = kb_dequote(nb2);
                        char tb[400]; snprintf(tb, sizeof tb, "%s", turn);
                        char *tw[64];
                        size_t tn = split_words(tb, tw, 64);
                        char got[KB_TERM_LEN];
                        if (!register_of_turn(b, tw, tn, got, sizeof got) ||
                            strcmp(got, want) != 0)
                            continue;      /* fuori registro: non e' pertinente */
                    }
                }
                if (unary) {
                    const char *a[1] = { quoted };
                    tries++;
                    if (repair_try(b, gapq, turn, reg, a, 1)) {
                        done = 1;
                        o += (size_t)snprintf(out + o, out_size - o,
                                              "%s%s(\"%s\")", fixed ? ", " : "",
                                              reg, cand[c]);
                        fixed++;
                    }
                } else {
                    for (size_t v = 0; v < nv && !done &&
                                       tries < REPAIR_MAX_TRIES; v++) {
                        const char *a[2] = { quoted, vals[v] };
                        tries++;
                        if (repair_try(b, gapq, turn, reg, a, 2)) {
                            done = 1;
                            o += (size_t)snprintf(out + o, out_size - o,
                                                  "%s%s(\"%s\", %s)",
                                                  fixed ? ", " : "", reg,
                                                  cand[c], vals[v]);
                            fixed++;
                        }
                    }
                }
            }
        }
    }
    return (int)fixed;
}

/* Il ciclo come atto invocabile: vedi brain.h. Marca la propria durata con
 * `repairing/1`, cosi' che un ponte trovato PROPONENDO si distingua nel
 * registro da uno trovato leggendo o portato da qualcun altro — sono tre eventi
 * diversi, e solo il primo dice che il processo cammina da solo. */
/* gen411 — IL REGISTRO DELLE LACUNE SOPRAVVIVE AL PROCESSO.
 *
 * Un processo autonomo che riparte da zero a ogni avvio non ha nessuna agenda
 * su cui lavorare: e' il blocco vero fra «il ciclo funziona se glielo chiedi» e
 * «il ciclo gira da solo». Le lacune vanno in un file PROPRIO e non nell'albero
 * curato — `/save` instrada nella KB curata, ed e' giusto per la conoscenza ma
 * sbagliato per un registro di lavoro, che e' effimero per natura e non e'
 * qualcosa che parrot0 SA: e' qualcosa che parrot0 DEVE FARE.
 *
 * Si scrivono anche le sorgenti, perche' un ponte si prova riponendo il turno
 * vero: una lacuna senza la propria forma originale e' irreparabile. */
const char *brain_gaps_path(void);
int brain_gaps_save(Brain *b);
const char *brain_bridges_path(void);
int brain_bridges_save(Brain *b);

const char *brain_gaps_path(void) {
    const char *p = p0env("PARROT0_GAPS");
    return (p && *p) ? p : "kb/learning/gaps.p0";
}

int brain_gaps_save(Brain *b) {
    if (!b || !b->kb) return -1;
    char gaps[128][KB_TERM_LEN];
    const char *gq[1] = { NULL };
    size_t n = kb_match(b->kb, "machinery_gap", gq, 1, gaps, 128);
    const char *path = brain_gaps_path();
    if (n == 0) { remove(path); return 0; }
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "%% gaps.p0 — il registro di lavoro del processo autonomo (gen411).\n");
    fprintf(f, "%% Scritto dal motore, non a mano: sono i turni che hanno murato e\n");
    fprintf(f, "%% aspettano un ponte. Non e' conoscenza — e' un'agenda.\n");
    for (size_t i = 0; i < n; i++) {
        fprintf(f, "machinery_gap(%s).\n", gaps[i]);
        const char *sq[2] = { gaps[i], NULL };
        char src[1][KB_TERM_LEN];
        if (kb_match(b->kb, "gap_source", sq, 2, src, 1) > 0)
            fprintf(f, "gap_source(%s, %s).\n", gaps[i], src[0]);
        /* gen414: e di che FORMA e'. Senza, il registro dice che qualcosa e'
         * fallito ma non che cosa serva per ripararlo, e il rimedio torna a
         * essere uno solo per tutti. */
        char kind[1][KB_TERM_LEN];
        if (kb_match(b->kb, "gap_kind", sq, 2, kind, 1) > 0)
            fprintf(f, "gap_kind(%s, %s).\n", gaps[i], kind[0]);
    }
    fclose(f);
    return (int)n;
}

/* gen411 — I PONTI IMPARATI SOPRAVVIVONO AL PROCESSO.
 *
 * Senza questo, il bilancio del sogno era una misura FALSA: le lacune si
 * azzeravano dentro il giro e tornavano tutte alla conversazione successiva,
 * perche' cio' che il ciclo aveva imparato viveva solo in memoria. Un processo
 * autonomo il cui effetto non sopravvive al processo non e' un processo.
 *
 * Vanno in un file proprio, come le lacune e per la stessa ragione: sono
 * INDOTTI, non curati. Nessuno li ha scritti, si possono ritirare, e chi guarda
 * l'albero della conoscenza dev'essere in grado di distinguerli da cio' che una
 * persona ha deciso. Promuoverli a conoscenza ufficiale e' un'altra decisione,
 * ed e' la generazione successiva. */
const char *brain_bridges_path(void) {
    const char *p = p0env("PARROT0_BRIDGES");
    return (p && *p) ? p : "kb/learning/bridges.p0";
}

int brain_bridges_save(Brain *b) {
    if (!b || !b->kb) return -1;
    /* Si scrive cio' che il ciclo ha PROMOSSO, non «tutto cio' che e' indotto»:
     * l'induzione comprende anche i fatti estratti dalla prosa, che sono
     * conoscenza sul mondo e vanno altrove. `learned_bridge/2` e' il marcatore
     * che il ciclo lascia sul proprio lavoro. */
    char preds[64][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t n = kb_match(b->kb, "learned_bridge", q, 2, preds, 64);
    if (!n) return 0;
    FILE *f = fopen(brain_bridges_path(), "a");
    if (!f) return -1;
    int w = 0;
    for (size_t i = 0; i < n; i++) {
        char cues[16][KB_TERM_LEN];
        const char *cq[2] = { preds[i], NULL };
        size_t nc = kb_match(b->kb, "learned_bridge", cq, 2, cues, 16);
        for (size_t k = 0; k < nc; k++) {
            fprintf(f, "%s(%s).\n", kb_dequote(preds[i]), cues[k]);
            w++;
        }
    }
    fclose(f);
    return w;
}

static int self_repair(Brain *b, char *out, size_t out_size) {
    return self_repair_target(b, NULL, out, out_size);
}

int brain_self_repair(Brain *b, char *out, size_t out_size) {
    if (!b || !brain_kb(b)) return 0;
    const char *ra[1] = { "1" };
    kb_assert(b->kb, "repairing", ra, 1);
    int n = self_repair(b, out, out_size);
    kb_retract(b->kb, "repairing", ra, 1);
    return n;
}

/* gen419 — IL TRANSCODER: da token opaco a termine strutturato.
 *
 * Le procedure su orari, date e complessi (procedures.p0) sanno calcolare, ma
 * non servono a niente finche' «14:30» resta un token opaco: tracciato pezzo per
 * pezzo, parrot0 aveva l'aritmetica, le unita' di tempo e la relazione «un'ora ha
 * 60 minuti», e cadeva perche' nessuno leggeva quel token come una QUANTITA'.
 *
 * Qui un token che ha la forma dichiarata diventa un termine: `14:30` ->
 * `time(14, 30)`, `2024-02-28` -> `date(2024, 2, 28)`, e il fatto
 * `transcoded("14:30", time(14,30))` lo mette a disposizione di qualunque regola.
 * Il motore non sa che cosa sia un orario: sa spezzare un token su un separatore
 * e contarne i pezzi. QUALI forme esistano e' un fatto (`transcode_shape/3`),
 * quindi una notazione nuova — un'altra lingua, un altro formato di data — costa
 * una riga di KB.
 *
 * Additivo: pubblica fatti e non toglie niente. Un token che non ha nessuna forma
 * dichiarata resta esattamente quello che era. */
static void turn_publish_transcodes(Brain *b, const char *surface) {
    if (!b || !b->kb || !surface) return;
    char shapes[16][KB_TERM_LEN];
    const char *nq[3] = { NULL, NULL, NULL };
    size_t ns = kb_match(b->kb, "transcode_shape", nq, 3, shapes, 16);
    if (!ns) return;
    char buf[512];
    snprintf(buf, sizeof buf, "%s", surface);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    for (size_t i = 0; i < nw; i++) {
        char tok[KB_TERM_LEN];
        snprintf(tok, sizeof tok, "%s", w[i]);
        char *t = strip_edge_punct(tok);
        if (!*t) continue;
        for (size_t k = 0; k < ns; k++) {
            char fn[KB_TERM_LEN];
            snprintf(fn, sizeof fn, "%s", shapes[k]);
            const char *sepq[3] = { fn, NULL, NULL };
            char sep[1][KB_TERM_LEN];
            if (kb_match(b->kb, "transcode_shape", sepq, 3, sep, 1) != 1) continue;
            char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", sep[0]);
            const char *sc = kb_dequote(sb);
            if (!sc || !*sc) continue;
            const char *cntq[3] = { fn, sep[0], NULL };
            char cnt[1][KB_TERM_LEN];
            if (kb_match(b->kb, "transcode_shape", cntq, 3, cnt, 1) != 1) continue;
            char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", cnt[0]);
            long want = strtol(kb_dequote(cb), NULL, 10);
            if (want < 2 || want > KB_MAX_ARGS) continue;
            /* spezza il token e pretende che ogni pezzo sia un numero */
            char work[KB_TERM_LEN]; snprintf(work, sizeof work, "%s", t);
            char term[KB_TERM_LEN];
            size_t o = (size_t)snprintf(term, sizeof term, "%s(", fn);
            long parts = 0; int ok = 1;
            char *p = work;
            while (p && *p && ok) {
                char *q = strchr(p, sc[0]);
                if (q) *q = '\0';
                if (!*p) { ok = 0; break; }
                for (const char *d = p; *d; d++)
                    if (!isdigit((unsigned char)*d)) { ok = 0; break; }
                if (!ok) break;
                long v = strtol(p, NULL, 10);   /* toglie gli zeri iniziali */
                o += (size_t)snprintf(term + o, sizeof term - o, "%s%ld",
                                      parts ? ", " : "", v);
                parts++;
                p = q ? q + 1 : NULL;
            }
            if (!ok || parts != want || o + 2 >= sizeof term) continue;
            snprintf(term + o, sizeof term - o, ")");
            char quoted[KB_TERM_LEN];
            snprintf(quoted, sizeof quoted, "\"%s\"", t);
            const char *a[2] = { quoted, term };
            int prev = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, "transcoded", a, 2);
            kb_set_origin(b->kb, prev);
            break;                       /* una forma sola per token */
        }
    }
}

/* gen489 — CIO' CHE SI PUO' OMETTERE DENTRO UNA SUPERFICIE E' CONOSCENZA.
 *
 * Misurato al gen488, e segnalato da F. come P0.0 di `LEARN_TODO.md`:
 *
 *     which is bigger, 3 or 5           ->  5.
 *     which of these is bigger, 3 or 5  ->  Skin.
 *     which one is bigger, 3 or 5       ->  Skin.
 *
 * La cue e' una stringa CONTIGUA (`cue()` e' `strstr`), quindi due parole
 * interposte — «of these», «one» — la spezzano; la domanda numerica non viene
 * riconosciuta, un'altra facolta' prende il turno e risponde una cosa FALSA.
 * E' il caso peggiore del mantra #7: peggio di un muro, perche' sembra una
 * risposta.
 *
 * La correzione per istanza sarebbe stata una cue in piu' per ogni variante —
 * un frasario, il mantra #2 al contrario. Qui il motore fa invece UNA cosa
 * generale e cieca alla lingua: costruisce una SECONDA lettura del turno in cui
 * le locuzioni omissibili non ci sono, e cerca le cue in entrambe. Quali
 * locuzioni siano omissibili e' un fatto — `elidable_phrase/1` — quindi una
 * variante nuova, in qualunque lingua e per qualunque registro di cue, costa una
 * riga di KB e zero motore, e ritrarla la toglie nello stesso turno.
 *
 * Vale per TUTTI i registri, non per quello numerico: e' il produttore
 * universale, e vede le cue di chiunque. E non toglie niente — la superficie
 * originale resta cercata per prima — quindi aumenta cio' che parrot0 vede
 * senza ridurre cio' che vedeva (il criterio di evoluzione di `MANTRA.md`). */
static int turn_word_boundary(const char *s, size_t at, size_t len) {
    if (at > 0 && !isspace((unsigned char)s[at - 1])) return 0;
    char after = s[at + len];
    return after == '\0' || isspace((unsigned char)after) ||
           strchr(",.;:!?", after) != NULL;
}

static void turn_elide_phrase(char *s, const char *phrase) {
    size_t plen = strlen(phrase);
    if (plen == 0) return;
    for (char *hit = strstr(s, phrase); hit; hit = strstr(hit, phrase)) {
        size_t at = (size_t)(hit - s);
        if (!turn_word_boundary(s, at, plen)) { hit += 1; continue; }
        /* Si porta via anche UNO spazio adiacente, altrimenti resta un doppio
         * spazio e la lettura elisa non combacia con nessuna cue. */
        size_t cut = plen;
        if (isspace((unsigned char)s[at + cut])) cut++;
        else if (at > 0 && isspace((unsigned char)s[at - 1])) { hit--; at--; cut++; }
        memmove(s + at, s + at + cut, strlen(s + at + cut) + 1);
    }
}

static void turn_elide_surface(Brain *b, const char *surface,
                               char *out, size_t out_size) {
    snprintf(out, out_size, "%s", surface);
    if (!b || !b->kb) return;
    char (*ph)[KB_TERM_LEN] = NULL;
    size_t np = 0;
    const char *q[1] = { NULL };
    if (!kb_match_all(b->kb, "elidable_phrase", q, 1, &ph, &np)) { free(ph); return; }
    for (size_t k = 0; k < np; k++) {
        char pat[KB_TERM_LEN];
        snprintf(pat, sizeof pat, "%s", ph[k]);
        const char *p = kb_dequote(pat);
        if (*p) turn_elide_phrase(out, p);
    }
    free(ph);
}

static void turn_publish_cues(Brain *b, const char *surface) {
    /* gen432 — I REGISTRI SI ENUMERANO TUTTI, senza tetto.
     *
     * Erano letti con un tetto di SEDICI, e i registri dichiarati sono diciotto:
     * i due piu' recenti non venivano pubblicati e le loro cue non combaciavano
     * mai. Nessun errore, nessun avviso — la stessa specie di difetto della riga
     * della sterlina e dei frame al passato: conoscenza dichiarata che non puo'
     * funzionare, e che non si lamenta.
     *
     * Un tetto fisso su una lista che la KB fa crescere e' sempre una bomba a
     * tempo: `kb_match_all` esiste apposta per non averne uno. */
    char (*regs)[KB_TERM_LEN] = NULL;
    size_t nr = 0;
    const char *rq[2] = { NULL, NULL };
    /* La seconda lettura del turno, senza le locuzioni omissibili: si costruisce
     * UNA volta per turno e serve tutti i registri. */
    char elided[512];
    turn_elide_surface(b, surface, elided, sizeof elided);
    if (!kb_match_all(b->kb, "turn_cue_registry", rq, 2, &regs, &nr)) { free(regs); return; }
    for (size_t i = 0; i < nr; i++) {
        char reg[KB_TERM_LEN];
        snprintf(reg, sizeof reg, "%s", kb_dequote(regs[i]));
        if (!*reg) continue;
        char pos[4][KB_TERM_LEN];
        const char *pq[2] = { regs[i], NULL };
        if (kb_match(b->kb, "turn_cue_registry", pq, 2, pos, 4) == 0) continue;
        /* The registry may be a binary or a ternary relation; kb_match_all
         * collects the first unbound slot either way. Trying both keeps the KB
         * free to point at whichever relation actually carries the surfaces,
         * without the arity becoming a second thing to declare. A relation with
         * ground rows is also markedly cheaper to enumerate than a derived view:
         * this runs on every turn, so the C reads facts and lets the KB do the
         * deciding — publishing is seeing, filtering is understanding. */
        /* L'arita' del registro non e' una seconda cosa da dichiarare: si prova
         * quella unaria, poi la binaria, poi la ternaria, e la prima che porta
         * righe vince. Un elenco di superfici puo' essere `cue/1`, `cue/2` con
         * una classe davanti o `cue/3` con classe e valore — sono tutte forme
         * legittime della stessa idea, e chiedere alla KB di annunciare quale
         * sia significherebbe farle dichiarare la propria implementazione. */
        char (*rows)[KB_TERM_LEN] = NULL;
        size_t nrows = 0;
        const char *aq[3] = { NULL, NULL, NULL };
        size_t tries[3] = { 1, 2, 3 };
        for (size_t t = 0; t < 3 && nrows == 0; t++) {
            free(rows); rows = NULL; nrows = 0;
            if (!kb_match_all(b->kb, reg, aq, tries[t], &rows, &nrows)) { rows = NULL; nrows = 0; }
        }
        if (nrows == 0) { free(rows); continue; }
        char posbuf[KB_TERM_LEN];
        snprintf(posbuf, sizeof posbuf, "%s", pos[0]);
        int second = strcmp(kb_dequote(posbuf), "2") == 0;
        for (size_t k = 0; k < nrows && k < TURN_MAX_CUES; k++) {
            if (!second) {
                char probe[KB_TERM_LEN];
                snprintf(probe, sizeof probe, "%s", rows[k]);
                const char *needle = kb_dequote(probe);
                if (!*needle || (!cue(surface, needle) && !cue(elided, needle))) continue;
                char quoted[KB_TERM_LEN];
                if (!turn_quote(needle, 0, strlen(needle), quoted, sizeof quoted))
                    continue;
                const char *args[] = { "current_turn", reg, quoted };
                kb_assert(b->kb, "turn_cue", args, 3);
                continue;
            }
            /* Position 2: the first argument names a class, the second carries
             * the surface. Enumerating per class keeps the same indexed path. */
            char (*inner)[KB_TERM_LEN] = NULL;
            size_t ninner = 0;
            const char *iq[2] = { rows[k], NULL };
            if (!kb_match_all(b->kb, reg, iq, 2, &inner, &ninner)) { free(inner); continue; }
            for (size_t j = 0; j < ninner; j++) {
                char probe[KB_TERM_LEN];
                snprintf(probe, sizeof probe, "%s", inner[j]);
                const char *needle = kb_dequote(probe);
                if (!*needle || (!cue(surface, needle) && !cue(elided, needle))) continue;
                char quoted[KB_TERM_LEN];
                if (!turn_quote(needle, 0, strlen(needle), quoted, sizeof quoted))
                    continue;
                const char *args[] = { "current_turn", reg, quoted };
                kb_assert(b->kb, "turn_cue", args, 3);
            }
            free(inner);
        }
        free(rows);
    }
    free(regs);
}
static int universal_turn_lead(Brain *b, const char *surface,
                               char *out, size_t out_size) {
    if (!b || !b->kb || !surface || !*surface) return 0;
    InputSpan spans[64];
    int ambiguous = 0;
    size_t ns = input_segment(b->kb, surface, spans, 64, &ambiguous);
    if (ambiguous || ns == 0) return 0;

    kb_retract_pred(b->kb, "turn_span");
    kb_retract_pred(b->kb, "turn_span_cue");
    kb_retract_pred(b->kb, "turn_span_surface");
    kb_retract_pred(b->kb, "turn_span_token");
    kb_retract_pred(b->kb, "turn_span_binding");
    kb_retract_pred(b->kb, "turn_cue");
    input_structure_clear(b->kb, "current_turn");
    kb_set_origin(b->kb, KB_REFLECTIVE);
    turn_publish_cues(b, surface);
    turn_publish_transcodes(b, surface);
    for (size_t i = 0; i < ns; i++) {
        char index[24], type[KB_TERM_LEN];
        char text[KB_TERM_LEN], payload[KB_TERM_LEN];
        snprintf(index, sizeof index, "%zu", i);
        input_span_type(&spans[i], type, sizeof type);
        if (!turn_quote(surface, spans[i].start, spans[i].len,
                        text, sizeof text)) continue;
        size_t cue_len = spans[i].cue_len;
        if (cue_len > spans[i].len) cue_len = spans[i].len;
        if (!turn_quote(surface, spans[i].start + cue_len,
                        spans[i].len - cue_len, payload, sizeof payload))
            continue;
        const char *span_args[] = { "current_turn", index, type, payload };
        const char *surface_args[] = { "current_turn", index, text };
        const char *cue_args[] = { "current_turn", index,
                                   spans[i].cue[0] ? spans[i].cue : "\"\"" };
        kb_assert(b->kb, "turn_span", span_args, 4);
        kb_assert(b->kb, "turn_span_surface", surface_args, 3);
        kb_assert(b->kb, "turn_span_cue", cue_args, 3);
        input_structure_publish(b->kb, surface, &spans[i], "current_turn");
        turn_publish_tokens(b, surface, &spans[i], index);
        turn_publish_state(b, surface, &spans[i], index);
    }

    /* Resolve the accumulated hierarchy once and let the KB materialize its
     * unique semantic observation.  This call names only the open observation
     * protocol: words, languages, operators and word orders remain KB facts. */
    {
        char observed[1][KB_TERM_LEN];
        const char *q[] = { "current_turn", NULL };
        kb_match(b->kb, "input_frame_observe", q, 2, observed, 1);
    }
    kb_set_origin(b->kb, KB_SESSION);

    /* gen394: la CONTABILITA' del turno, e non e' una terza domanda per caso.
     *
     * Il motore ne faceva due — «questo turno appartiene a un piano?» e «qual e'
     * la risposta?» — ed entrambe devono restare pure: una risposta non puo'
     * dipendere da un effetto avvenuto mentre la si cercava. Ma una
     * conversazione deve ricordare anche cio' a cui NON ha risposto, e quel
     * turno esce di qui senza che nessuno gli chieda niente: il frame declina
     * apposta, per lasciare la parola al percorso storico.
     *
     * Questa terza domanda e' il posto dove la KB registra cio' che vuole
     * ricordare del turno. Il risultato non e' una risposta e viene ignorato: il
     * C non sa che cosa venga registrato, ne' se qualcosa lo sia.
     *
     * Si ENUMERA invece di chiedere se esista, e la differenza e' tutto il
     * punto: una domanda di esistenza si ferma al primo contabile che risponde,
     * quindi il secondo non lavorerebbe mai. Registrare un'issue aperta e
     * registrare uno stato descritto sono compiti indipendenti dello stesso
     * turno, e nessuno dei due e' il seguito dell'altro. */
    char keepers[16][KB_TERM_LEN];
    const char *any[1] = { NULL };
    size_t nk = kb_match(b->kb, "bookkeeper", any, 1, keepers, 16);
    for (size_t i = 0; i < nk; i++) {
        const char *one[] = { "current_turn", keepers[i] };
        kb_query(b->kb, "turn_bookkeeping", one, 2);
    }

    const char *candidate[] = { "current_turn" };
    if (!kb_query(b->kb, "turn_plan_candidate", candidate, 1)) return 0;

    char replies[1][KB_TERM_LEN];
    const char *q[] = { "current_turn", NULL };
    size_t nr = kb_match(b->kb, "turn_response", q, 2, replies, 1);
    if (nr != 1) return 0;
    put(kb_dequote(replies[0]), out, out_size);
    return 1;
}

const char *brain_last_module(Brain *b) {
    return (b && b->last_module[0]) ? b->last_module : "(nessuno)";
}

size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size) {
    /* gen422: la firma si azzera A OGNI TURNO. E' l'impronta di QUESTO
     * ragionamento, non della vita del processo — sommarli darebbe un numero che
     * cambia sempre e non dice niente. */
    DocumentRevisionSnapshot document_before = document_revision_snapshot(b);
    if (b && b->kb) kb_footprint_reset(b->kb);
    size_t n = brain_respond_dispatch(b, input, out, out_size);
    /* SC40-B: la fotografia precedente e quella corrente rendono osservabile il
     * delta reale. Un modulo autorizzato che non ha mutato membri semantici non
     * causa lavoro; una mutazione rivede soltanto il taglio scelto dalla KB. */
    document_revision_after_declared_module(b, &document_before);
    document_revision_snapshot_free(&document_before);
    /* gen422d: e CHI ha risposto fa parte della strada. Va piegato qui, una
     * volta, invece che nei dieci punti in cui `last_module` viene scritto. */
    if (b && b->kb) kb_footprint_mark(b->kb, b->last_module);
    /* ── gen388: L'ARGOMENTO DEL TURNO LO REGISTRA IL DISPATCH ──────────────
     *
     * `last_entity` lo scrivevano solo le facolta' che se ne ricordavano. Una
     * risposta di `mod_quantity` parla DI poker e non lo segnava, quindi il topic
     * non sopravviveva fra facolta' — che e' letteralmente il sintomo #5
     * dell'essay («piu' sistemi indipendenti invece di un interlocutore») visto da
     * dentro: ogni modulo teneva il filo per se'.
     *
     * Registrarlo qui, una volta, e' la forma giusta: e' una proprieta' del
     * TURNO, non di chi lo ha servito.
     *
     * Va in `last_topic` e NON in `last_entity`, e la distinzione non e'
     * pedanteria: scrivendo l'antecedente dei pronomi, «ogni cane e' un
     * mammifero» / «e' un mammifero?» smetteva di chiedere a chi si riferisse
     * "it" e sceglieva in silenzio. Un pronome vuole un REFERENTE introdotto; un
     * topic e' solo cio' di cui si parla. */
    if (b && b->kb && out && *out) {
        char cn[256];
        brain_canonical(b, input, cn, sizeof cn);
        char *cw[64]; size_t ncw = split_words(cn, cw, 64);
        for (size_t k = ncw; k-- > 0; ) {
            char d[256];
            char *tk = strip_edge_punct(cw[k]);
            if (strlen(tk) < 3 || is_stopword(b, tk)) continue;
            if (kb_describe_entity(b->kb, tk, d, sizeof d)) {
                snprintf(b->last_topic, sizeof b->last_topic, "%s", tk);
                b->has_last_topic = 1;
                break;
            }
        }
    }
    apply_active_constraint(b, out, out_size);
    n = strlen(out);
    if (b) snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
    return n;
}

/* gen407 — UNA PROSA CHE INSEGNA, riconosciuta per struttura e non per parole:
 * piu' di una frase, e nessuna domanda. Una frase sola ha gia' la sua strada
 * (mod_knowledge); una domanda non insegna niente.
 *
 * Serve in due posti opposti, ed e' il motivo per cui e' una funzione. Dice al
 * pianificatore analitico di NON reclamare il turno — un testo dichiarativo
 * multi-frase e' qualcuno che insegna, non qualcuno che chiede un'analisi — e
 * dice al percorso di apprendimento che quel turno lo riguarda. Senza la prima
 * meta', la seconda non riceverebbe mai il turno: misurato, il pianificatore lo
 * prendeva a stadio 0, cioe' prima di ogni consumatore specializzato. */
static int teaching_prose(const char *input) {
    if (!input || strchr(input, '?')) return 0;
    const char *dot = strchr(input, '.');
    return dot && strchr(dot + 1, '.') != NULL;
}

static int prose_learn_lead(Brain *b, const char *canon, const char *input,
                            char *out, size_t out_size) {
    if (!b || !b->kb || !canon || !input) return 0;
    /* Il cancello guarda il turno GREZZO, non `canon`: la canonicalizzazione
     * lavora in un buffer da 256 byte, e una prosa vera e' piu' lunga. Su
     * `canon` la stessa prosa risultava di UNA frase — la prima — e su quel
     * frammento il pianificatore analitico costruiva il suo saggio. Il taglio
     * e' un limite reale della strada detta, e va ricordato: qui si aggira
     * leggendo l'originale, ma la canonicalizzazione di un testo lungo resta da
     * fare.
     * TODO(kb-first): `canon[256]` tronca la prosa lunga prima del dispatch. */
    (void)canon;
    if (strchr(input, '?')) return 0;
    const char *dot = strchr(input, '.');
    if (!dot || !strchr(dot + 1, '.')) return 0;      /* una frase sola */
    char prose[4096];
    snprintf(prose, sizeof prose, "%s", input);
    char learned[700]; learned[0] = '\0';
    int nf = learn_from_prose(b, prose, learned, sizeof learned);
    if (nf <= 0 || !learned[0]) return 0;
    char msg[900];
    { 
      char _v0[48]; snprintf(_v0, sizeof _v0, "%d", nf);
  const KbResponseSlot _rs[] = { { "nf", _v0 }, { "learned", learned } };
      kb_term_say(b, "learned_x_facts_x", _rs, 2, msg, sizeof msg); }
    put(msg, out, out_size);
    return 1;
}

static size_t brain_respond_dispatch(Brain *b, const char *input, char *out, size_t out_size) {
    if (out_size == 0) return 0;
    if (b) {
        if (b->turns == 0) {
            b->start_time = time(NULL);
            if (timespec_get(&b->start_ts, TIME_UTC) == TIME_UTC)
                b->has_start_ts = 1;
        }
        b->turns++;
        b->turn_frame[0] = '\0';   /* gen363: provenance is per-turn */
        /* gen434 — LO STATO DEL TURNO E' UN FATTO, e i fatti di turno si
         * azzerano qui, prima di tutto: e' l'unico punto che ogni turno
         * attraversa. La specie della lacuna non e' piu' decisa dal C — si
         * DERIVA da questi tre (`kb/core/gap-kinds.p0`), e una specie nuova
         * domani e' una regola, non un ramo. */
        if (b->kb) {
            kb_retract_pred(b->kb, "turn_outcome");
            kb_retract_pred(b->kb, "turn_topic");
            kb_retract_pred(b->kb, "turn_module");
            kb_retract_pred(b->kb, "turn_register");
            kb_retract_pred(b->kb, "saturated_read");
        }
    }

    char norm[256];
    normalize(input, norm, sizeof norm);

    /* gen240 (universal-comprehension): record the CURRENT conversation language as
     * a session KB fact (current_language/1), so replies can be localized and the
     * language is itself queryable — never a C variable. Detected from the raw
     * normalized turn (before canonicalization folds Italian into English). */
    detect_set_language(b, norm);

    /* gen335 (KB-first teachability): try_teach_form runs BEFORE any content
     * module — a user teaching a new phrasing must not be intercepted by the
     * intent they're trying to retrain (e.g. "insegnami a rispondere il tuo nome"
     * must reach the teach handler, not mod_self). */
    /* SC32: ritrattare corre PRIMA di insegnare — «unlearn» contiene «learn»,
     * e una lezione che non si puo' togliere non e' una lezione. */
    if (b && try_forget_form(b, norm, input, out, out_size)) {
        note_arith_result(b, out); conv_log(b, input, out);
        return strlen(out);
    }

    if (b && try_teach_form(b, norm, input, out, out_size)) {
        note_arith_result(b, out); conv_log(b, input, out);
        return strlen(out);   /* insegnare non e' rispondere: nessuna lacuna si chiude */
    }

    if (b && universal_turn_lead(b, norm, out, out_size)) {
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "%s", "turn_plan");
        /* Qui `canon` non esiste ancora — la canonicalizzazione avviene piu' in
         * basso — quindi la chiusura usa `norm`. Per l'inglese sono la stessa
         * stringa; per una lingua che viene tradotta la lacuna resterebbe
         * aperta anche dopo essere stata colmata. E' un limite reale e va
         * chiuso spostando la canonicalizzazione prima del contratto di turno,
         * non nascosto qui. */
        return turn_done(b, norm, input, out, out_size);
    }

    /* gen43: canonicalize the parsing surface (function words -> English tokens)
     * before dispatch, so the reasoning core answers in any mapped language
     * without duplicating a module. `raw` (input) is left untouched, so the
     * reader still induces its generative model from the original prose. */
    char canon[256];
    canonicalize_lang(b, norm, canon, sizeof canon);

    /* gen431 — UNA RICHIESTA INCOMPLETA SI DICE SUBITO, PRIMA DI OGNI FACOLTA'.
     *
     * «explain this stack trace», «translate this paragraph», «continue this
     * story»: il testo non e' stato allegato, e nessuna facolta' puo' fare
     * meglio di dirlo. Finora il pianificatore analitico — che corre prima della
     * registry — ci costruiva sopra sei paragrafi che non nominano mai la cosa
     * chiesta: e' la classe piu' numerosa dei cento fallimenti, e la peggiore,
     * perche' SEMBRA una risposta.
     *
     * Il controllo sta qui, prima di tutti i lead, perche' e' una proprieta'
     * della RICHIESTA e non di una facolta': se manca il referente, manca per
     * tutti. Quali generi di contenuto esistano e' conoscenza. */
    if (b && b->kb) {
        char kind_[64];
        if (p0_unattached_kind(b, canon, input, kind_, sizeof kind_)) {
            char msg_[400];
            const KbResponseSlot sl_[] = { { "kind", kind_ } };
            if (kb_response_slots(b, "missing_referent", sl_, 1, msg_, sizeof msg_)) {
                put(msg_, out, out_size);
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s", "missing_referent");
                return turn_done(b, canon, input, out, out_size);
            }
        }
    }

    /* gen366: typed Task IR + proof-carrying operators run before the old open
     * analysis planner.  A complete operator result is more specific than a
     * rhetorical template; an incomplete IR or proof declines cleanly. */
    if (b && reasoning_task_lead(b, canon, input, out, out_size)) {
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "%s", "reasoning_task");
        return turn_done(b, canon, input, out, out_size);
    }

    /* gen360 (LLMSCORE-max): open analytical requests are planned before any
     * first-match consumer can turn their opener into a story, role or extracted
     * assertion. Both the act and the broad subject must win the universal KB
     * evidence scorer, and every answer_plan slot must be filled, so uncertain or
     * incomplete candidates decline without disturbing the established registry. */
    if (b && !teaching_prose(input) &&
        structured_analysis_lead(b, canon, input, 0, out, out_size)) {
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "%s", "analysis_plan");
        return turn_done(b, canon, input, out, out_size);
    }

    /* gen359 (LLMSCORE-max, motorize-the-class): a well-formed definitional or
     * analytical question about a specific concept parrot0 knows is answered from
     * the KB semantic projection before the ordinary first-match registry. */
    if (b && semantic_lead(b, canon, input, out, out_size)) {
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "%s", "semantic_lead");
        return turn_done(b, canon, input, out, out_size);
    }

    /* A reusable field family is broader than an indexed concept summary. Try
     * it only after semantic_lead had the chance to use concrete knowledge, so
     * words such as "proof", "physical", or "system" cannot replace a known
     * subject with generic methodology. */
    if (b && !teaching_prose(input) &&
        structured_analysis_lead(b, canon, input, 1, out, out_size)) {
        snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
        snprintf(b->last_module, sizeof b->last_module, "%s", "analysis_family");
        return turn_done(b, canon, input, out, out_size);
    }

    /* gen360: part_of materialization is an expensive reflective inference over
     * the complete world. Analytical projections above do not consume it, so
     * defer the work until a turn actually falls through to the general
     * reasoner. This preserves the once-per-brain semantics for every existing
     * consumer while keeping a proved analytical answer inside LLMSCORE's
     * one-second process budget. */
    if (b && b->kb && !b->relations_derived) {
        kb_derive_part_of(b->kb);
        b->relations_derived = 1;
    }

    /* gen455 — UNA CONCESSIVA AFFERMA ENTRAMBE LE PROPOSIZIONI.
     *
     * «although mercury is a metal, it is liquid at room temperature» cadeva al
     * muro su «although», e con essa si perdeva `metal(mercury)`, che stava li'
     * in chiaro. Il periodo articolato — principale piu' subordinate — e' la
     * forma in cui una persona insegna davvero qualcosa, e finche' si perde per
     * intero l'addestramento via prompt resta confinato alle frasi brevi.
     *
     * Il motore non sa che cosa sia una concessiva: chiede alla KB che cosa quel
     * subordinatore dica delle sue due proposizioni (`subordinator_stance/2`).
     * Con `both` le stacca e le manda al dispatch ordinario, una per una, cosi'
     * ciascuna incontra i lettori che gia' esistono. Nessuna delle due comincia
     * col subordinatore, quindi non c'e' ricorsione. */
    if (b && b->kb && canon[0]) {
        char cb[512];
        if (strlen(canon) < sizeof cb) {
            snprintf(cb, sizeof cb, "%s", canon);
            char *comma = strchr(cb, ',');
            char *sp = strchr(cb, ' ');
            if (comma && sp && sp < comma) {
                char head[KB_TERM_LEN];
                size_t hl = (size_t)(sp - cb);
                if (hl < sizeof head) {
                    memcpy(head, cb, hl); head[hl] = '\0';
                    for (char *c = head; *c; c++) *c = (char)tolower((unsigned char)*c);
                    const char *sq[2] = { head, "both" };
                    if (kb_query(b->kb, "subordinator_stance", sq, 2)) {
                        *comma = '\0';
                        const char *first = sp + 1;
                        const char *second = comma + 1;
                        while (*second == ' ') second++;
                        if (*first && *second) {
                            char r1[900] = "", r2[900] = "";
                            brain_respond(b, first, r1, sizeof r1);
                            brain_respond(b, second, r2, sizeof r2);
                            char joined[1900];
                            snprintf(joined, sizeof joined, "%s %s", r1, r2);
                            put(joined, out, out_size);
                            return turn_done(b, canon, input, out, out_size);
                        }
                    }
                }
            }
        }
    }

    /* gen458 — LA RELATIVA E' UNA SECONDA AFFERMAZIONE SUL SOGGETTO.
     *
     * «copper is a metal that conducts electricity» ne dice due, e parrot0 non
     * ne prendeva nessuna: cadeva su «conducts» e perdeva anche `metal(copper)`.
     * E' la stessa figura della concessiva del gen455 con un altro
     * subordinatore, quindi usa lo stesso meccanismo — si stacca e si manda al
     * dispatch ordinario — e la stessa disciplina: che cosa la subordinata
     * valga e' conoscenza (`relative_pronoun/1`), non un ramo qui.
     *
     * Solo sulle ASSERZIONI: in una domanda la relativa restringe il
     * riferimento invece di aggiungere un fatto, ed e' un'altra lettura. */
    if (b && b->kb && canon[0] && !strchr(canon, '?')) {
        char rb[512];
        if (strlen(canon) < sizeof rb) {
            snprintf(rb, sizeof rb, "%s", canon);
            char *words[64];
            char scan[512]; snprintf(scan, sizeof scan, "%s", canon);
            size_t nwr = split_words(scan, words, 64);
            size_t rel = nwr, cop = nwr;
            for (size_t i = 0; i < nwr; i++) {
                const char *cq[1] = { words[i] };
                if (cop == nwr && kb_query(b->kb, "generic_copula", cq, 1)) cop = i;
                if (rel == nwr && cop != nwr && i > cop &&
                    kb_query(b->kb, "relative_pronoun", cq, 1)) { rel = i; break; }
            }
            /* serve un soggetto prima della copula e almeno una parola dopo il
             * pronome, altrimenti non ci sono due proposizioni da staccare */
            if (rel != nwr && cop > 0 && rel + 1 < nwr) {
                char head[512] = "", tail[512] = "";
                size_t ho = 0, to = 0;
                for (size_t i = 0; i < rel; i++)
                    ho += (size_t)snprintf(head + ho, sizeof head - ho,
                                           "%s%s", i ? " " : "", words[i]);
                /* la relativa parla del soggetto: lo si rimette davanti */
                for (size_t i = 0; i < cop; i++)
                    to += (size_t)snprintf(tail + to, sizeof tail - to,
                                           "%s%s", i ? " " : "", words[i]);
                for (size_t i = rel + 1; i < nwr; i++)
                    to += (size_t)snprintf(tail + to, sizeof tail - to,
                                           " %s", words[i]);
                if (head[0] && tail[0]) {
                    char r1[900] = "", r2[900] = "";
                    brain_respond(b, head, r1, sizeof r1);
                    brain_respond(b, tail, r2, sizeof r2);
                    char joined[1900];
                    snprintf(joined, sizeof joined, "%s %s", r1, r2);
                    put(joined, out, out_size);
                    return turn_done(b, canon, input, out, out_size);
                }
            }
        }
    }

    /* gen80: try to decompose compound turns (e.g. "chi sei e ricordati X")
     * before the normal single-turn dispatch. */
    if (b && decompose_and_dispatch(b, canon, input, out, out_size))
        { return turn_done(b, canon, input, out, out_size); }

    /* gen142 (E3): peel a leading discourse-marker opener and re-dispatch the
     * residue, so a content task wrapped in a channel-management opener survives
     * ("anyway, is socrates a man" -> "Yes."). Only claims when the residue is
     * actually owned by a module; otherwise the original turn dispatches normally
     * and its pragmatic shape is read by mod_pragma. */
    if (b && pragma_peel(b, canon, input, out, out_size))
        { return turn_done(b, canon, input, out, out_size); }

    /* gen218: an explicit correction ("no, X is not a Y") peels its marker and
     * re-dispatches the negative claim with the correction flag set, so the
     * standing belief is overridden and the conclusion re-derives. */
    if (b && correction_peel(b, canon, input, out, out_size))
        { return turn_done(b, canon, input, out, out_size); }

    /* gen221 (glue, symptom #5): a numeric personal fact remembered earlier feeds a
     * later computation — resolve "my <key>" to its KB value and re-dispatch so the
     * arithmetic core computes it ("what is my favorite number plus 3" -> "10.").
     * Pre-dispatch so mod_memory cannot mis-claim the unresolved reference first. */
    if (b && memref_resolve(b, canon, out, out_size))
        { return turn_done(b, canon, input, out, out_size); }

    /* gen222 (glue, symptom #3): a precisation that continues the previous computation
     * ("and times 3" after "what is 2 plus 2") — prepend the last result, inferred from
     * the KB, and re-dispatch so the arithmetic core finishes it ("what is 4 times 3"
     * -> "12."). Pre-dispatch so the bare fragment cannot fall to not-understood. */
    if (b && topic_continue_resolve(b, canon, out, out_size)) {
        note_arith_result(b, out); conv_log(b, input, out);
        return strlen(out);
    }
    if (b && continue_resolve(b, canon, out, out_size))
        { return turn_done(b, canon, input, out, out_size); }

    /* gen335d (linguistic glue, KB-first): knowledge-gap bridge. The gap
     * state lives as KB facts (pending_gap/1, pending_gap_question/1) in the
     * REFLECTIVE layer — working memory, never persisted. On the next turn:
     * confirmation → acquire + re-answer; anything else → retract + dispatch. */
    if (b && b->kb) {
        const char *gq[] = { NULL };
        char gtopics[1][KB_TERM_LEN];
        if (kb_match(b->kb, "pending_gap", gq, 1, gtopics, 1) > 0) {
            char topic[KB_TERM_LEN];
            snprintf(topic, sizeof topic, "%s", kb_dequote(gtopics[0]));

            /* gen335 (KB-first morphology): normalize plural topic to singular
             * so Wikipedia finds the right page ("cavalli" → "cavallo"). */
            {
                char sing[4][KB_TERM_LEN];
                const char *sq[] = { topic, NULL };
                if (kb_match(b->kb, "singular", sq, 2, sing, 4) > 0)
                    snprintf(topic, sizeof topic, "%s", sing[0]);
            }
            /* Save the stored question BEFORE retracting */
            char stored_q[256] = "";
            {
                const char *sqq[] = { NULL };
                char sq_hit[1][KB_TERM_LEN];
                if (kb_match(b->kb, "pending_gap_question", sqq, 1, sq_hit, 1) > 0)
                    snprintf(stored_q, sizeof stored_q, "%s",
                             kb_dequote(sq_hit[0]));
            }
            /* Check for confirmation — against BOTH canonicalized form
             * (for English) and raw normalized form (for Italian "si" which
             * canonical_token maps "si"→"is"). */
            char clow[256]; snprintf(clow, sizeof clow, "%s", canon);
            for (char *cp = clow; *cp; cp++)
                *cp = (char)tolower((unsigned char)*cp);
            char rlow[256]; snprintf(rlow, sizeof rlow, "%s", input);
            for (char *cp = rlow; *cp; cp++)
                *cp = (char)tolower((unsigned char)*cp);
            int confirm = (lex_class_member(b, "99_registry_lex3877", clow) || lex_class_member(b, "99_registry_lex3877_2", clow) ||
                           lex_class_member(b, "99_registry_lex3878", clow) || lex_class_member(b, "99_registry_lex3878_2", clow) ||
                           lex_class_member(b, "99_registry_lex3879", clow) || lex_class_member(b, "99_registry_lex3879_2", clow) ||
                           lex_class_member(b, "99_registry_lex3880", clow) || lex_class_member(b, "99_registry_lex3880_2", clow) ||!lex_prefix_member(b, "99_registry_lex3879_3", clow) == 0 ||!lex_prefix_member(b, "99_registry_lex3880_3", clow) == 0 ||
                           lex_class_member(b, "99_registry_lex3882", rlow) || strcmp(rlow, "sì") == 0 ||
                           lex_class_member(b, "99_registry_lex3883", rlow) || lex_class_member(b, "99_registry_lex3883_2", rlow) ||
                           lex_class_member(b, "99_registry_lex3884", rlow));

            /* Always retract the gap facts — single-turn window consumed */
            { const char *rga[] = { gtopics[0] }; kb_retract(b->kb, "pending_gap", rga, 1); }
            {
                const char *rqq[] = { NULL };
                char rq_hit[1][KB_TERM_LEN];
                if (kb_match(b->kb, "pending_gap_question", rqq, 1, rq_hit, 1) > 0) {
                    const char *rqa[] = { rq_hit[0] };
                    kb_retract(b->kb, "pending_gap_question", rqa, 1);
                }
            }

            if (!confirm) {
                /* Not confirmed — dispatch normally below */
            } else if (stored_q[0]) {
                {
                    const KbResponseSlot slots[] = { {"topic", topic} };
                    kb_response_slots(b, "gap_looking_up", slots, 1, out, out_size);
                }
                char def[512] = "";
                /* gen335k: on user confirmation, try Wikipedia fetch first.
                 * acquire_knowledge only uses local sources — the network
                 * step is gated behind the user's explicit "si". */
                wiki_fetch_bilingual(b->kb, topic);
                int got = acquire_knowledge(b, topic, def, sizeof def);
                size_t ol = strlen(out);
                if (!got) {
                    /* gen335e: mark this topic as failed so not_understood
                     * won't re-offer the same gap on re-dispatch. */
                    const char *fa[] = { topic };
                    kb_assert(b->kb, "pending_gap_failed", fa, 1);
                }
                /* gen335g: after a successful acquire, also extract structured
                 * facts from the page prose (extract_page_facts). This gives
                 * the full pipeline: download → learn concept → extract facts. */
                int nf = 0;
                if (got) {
                    char facts[512] = "";
                    nf = extract_page_facts(b, topic, facts, sizeof facts);
                }
                /* Re-dispatch the original question through dispatch_one
                 * (NOT brain_respond — that would recurse and corrupt state).
                 * dispatch_one normalizes+canonicalizes and walks the registry. */
                char re_ans[256] = "";
                int re_ok = dispatch_one(b, stored_q, re_ans, sizeof re_ans);

                /* gen396: acknowledgement, then the ANSWER, then the bookkeeping.
                 *
                 * The definition used to be appended here AND repeated by the
                 * re-dispatched reply, with the extraction count wedged between,
                 * so a confirmed lookup read «Cerco informazioni su pompa...
                 * <def>. Ho estratto 2 fatti. So già qualcosa su pompa: <def>.»
                 * The user asked a question and said yes; what they are owed is
                 * one answer to it, and the note about what was learned comes
                 * after it. */
                if (re_ok && re_ans[0])
                    snprintf(out + ol, out_size - ol, " %s", re_ans);
                else if (got && def[0])
                    snprintf(out + ol, out_size - ol, " %s", def);
                else {
                    char tail[64];
                    kb_response_slots(b, got ? "gap_done" : "gap_not_found",
                                      NULL, 0, tail, sizeof tail);
                    snprintf(out + ol, out_size - ol, "%s", tail);
                }
                if (nf > 0) {
                    char fstr[16]; snprintf(fstr, sizeof fstr, "%d", nf);
                    char tail[64];
                    const KbResponseSlot fslots[] = { {"count", fstr} };
                    kb_response_slots(b, "gap_extracted", fslots, 1, tail, sizeof tail);
                    ol = strlen(out);
                    snprintf(out + ol, out_size - ol, "%s", tail);
                }
                if (re_ok) {
                    conv_log(b, input, out);
                    return strlen(out);
            }
        }
    }
    }

    /* Walk the registry; first module to claim the turn wins. */
    int handled = 0, handled_by_discourse = 0;

    /* gen83: extract capitalized words as named-entity candidates. */
    if (b) {
        char rbuf[256];
        snprintf(rbuf, sizeof rbuf, "%s", input);
        char *rw[64];
        size_t rnw = split_words(rbuf, rw, 64);
        for (size_t i = 0; i < rnw; i++) {
            if (!(isupper((unsigned char)rw[i][0]) && strlen(rw[i]) >= 2)) continue;
            note_entity_seq(b, rw[i]);      /* R2: unbounded KB history for ordinals */
            if (b->entity_count >= 8) continue;
            int dup = 0;
            for (size_t j = 0; j < b->entity_count; j++)
                if (strcmp(b->entities[j], rw[i]) == 0) { dup = 1; break; }
            if (!dup) {
                snprintf(b->entities[b->entity_count], sizeof b->entities[0], "%s", rw[i]);
                b->entity_count++;
            }
        }
    }

    /* gen105 (L20): record the real control-flow trace of this turn — the
     * modules consulted that declined, then the one that claimed it. */
    char declined[BRAIN_TRACE_MAX][24]; size_t ndecl = 0;
    const char *winner = "fallback";
    /* M1 assisted learning: routing knowledge may nominate one faculty for an
     * eager first offer.  The join is completely open: input_segment supplies
     * typed spans, faculty_for/2 names their consumers, faculty_dispatch/2
     * supplies policy, and the existing registry proves that a callable module
     * really exists.  No surface, language, role or privileged faculty is
     * compiled here.  If two eager faculties compete, ordinary arbitration is
     * retained rather than guessing between them. */
    size_t eager_idx = registry_len;
    if (b && b->kb) {
        InputSpan spans[64]; int ambiguous = 0, eager_ambiguous = 0;
        size_t ns = input_segment(b->kb, input, spans, 64, &ambiguous);
        if (!ambiguous) {
            for (size_t si = 0; si < ns && !eager_ambiguous; si++) {
                char type[KB_TERM_LEN];
                input_span_type(&spans[si], type, sizeof type);
                char faculties[16][KB_TERM_LEN];
                const char *fq[2] = { type, NULL };
                size_t nf = kb_match(b->kb, "faculty_for", fq, 2,
                                     faculties, 16);
                for (size_t fi = 0; fi < nf && !eager_ambiguous; fi++) {
                    const char *dq[2] = { faculties[fi], "eager" };
                    if (!kb_query(b->kb, "faculty_dispatch", dq, 2)) continue;
                    const char *faculty = kb_dequote(faculties[fi]);
                    for (size_t ri = 0; ri < registry_len; ri++) {
                        if (strcmp(registry[ri].name, faculty) != 0) continue;
                        if (eager_idx == registry_len || eager_idx == ri)
                            eager_idx = ri;
                        else
                            eager_ambiguous = 1;
                        break;
                    }
                }
            }
        }
        if (eager_ambiguous) eager_idx = registry_len;
    }
    if (eager_idx < registry_len &&
        registry[eager_idx].handle(b, canon, input, out, out_size)) {
        handled = 1;
        winner = registry[eager_idx].name;
        if (strcmp(winner, "discourse") == 0) handled_by_discourse = 1;
        if (b) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", winner);
        }
    } else if (eager_idx < registry_len && ndecl < BRAIN_TRACE_MAX) {
        snprintf(declined[ndecl++], sizeof declined[0], "%s",
                 registry[eager_idx].name);
    }
    for (size_t i = 0; !handled && i < registry_len; i++) {
        if (i == eager_idx) continue;       /* already offered exactly once */
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
        if (ndecl < BRAIN_TRACE_MAX) snprintf(declined[ndecl++], sizeof declined[0], "%s",
                                 registry[i].name);
    }

    /* Commit the trace for "why did you answer that way?" and the verbatim input
     * for "what would you have said without X?" — but NOT when this turn was
     * itself one of those introspective questions, so each reports the decision
     * it is being asked about, not its own lookup. */
    if (b && !lex_class_member(b, "99_registry_lex4012", winner) &&
        !lex_class_member(b, "99_registry_lex4013", winner)) {
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

    /* gen335 (R2): the salient topic entity a module just resolved (e.g. "heart",
     * lowercase — not a capitalized proper noun) also enters the accumulating KB
     * history, so a later pronoun can reach it. Bridges the old last_entity C field
     * into the KB-first entity_mentioned/2 record. */
    if (b && b->has_last_entity) note_entity_seq(b, b->last_entity);

    /* If no module claimed the turn, fall back to the honest not-understood reply
     * (gen15 retired the gen0 parrot-echo; gen55 made it non-repeating).
     * Honest admission, never a mirror or a wrong "No.". */
    /* gen362: a DECLARED GAP is not an answer. A consumer that claimed the turn
     * only to say it has no facts leaves the same guaranteed zero as the blind
     * wall, and it also blocks the last-resort planner behind `handled`. The
     * shapes of parrot0's own surrenders are now KB (`wall_marker/1`), so this
     * check is a rule over knowledge rather than a phrase list in C.
     *
     * gen363: matching parrot0's own sentences was itself the fragile part — a
     * consumer that said "for that SITUATION" escaped a marker written "for that
     * topic". The reply now carries PROVENANCE (`turn_frame`, recorded for every
     * rendered frame), and the KB says which frames are surrenders
     * (`gap_frame/1`). No phrase has to be recognized, in any language.
     * `wall_marker/1` stays as the secondary structure covering the consumers
     * that still write their own text (keep-secondary-structures). */
    if (handled && b && b->kb && out && *out) {
        char lowered[512];
        size_t li = 0;
        for (const char *p = out; *p && li + 1 < sizeof lowered; p++)
            lowered[li++] = (char)tolower((unsigned char)*p);
        lowered[li] = '\0';
        const char *gq[] = { b->turn_frame };
        int declared_gap = b->turn_frame[0] &&
                           kb_query(b->kb, "gap_frame", gq, 1);
        int surrendered = declared_gap ||
                          analysis_reply_ignores_subject(b, canon, input, out);
        /* A faculty may declare its result terminal even when the rendered
         * report does not repeat the turn's topic.  In particular, an honest
         * extraction count plus skipped clauses is an outcome, not a disguised
         * wall for prose_learn to overwrite.  Which modules have that contract
         * remains live KB policy. */
        {
            const char *pq[2] = { winner, "terminal" };
            if (kb_query(b->kb, "module_result_policy", pq, 2))
                surrendered = 0;
        }
        char markers[32][KB_TERM_LEN];
        const char *mq[] = { NULL };
        size_t nm = surrendered ? 0
                                : kb_match(b->kb, "wall_marker", mq, 1,
                                           markers, 32);
        /* gen407 — PRIMA DI COPRIRE UN MURO CON UN SAGGIO, PROVA A IMPARARE.
         *
         * Misurato: la prosa di una pagina, incollata in conversazione, rendeva
         * ZERO fatti e duecento parole di analisi generica; la stessa prosa
         * letta da quella pagina ne rendeva otto. La strada detta murava
         * davvero — semplicemente il muro non si vedeva, perche' l'analisi di
         * ultima istanza lo copriva.
         *
         * Il gancio sta qui e non prima apposta: non toglie il turno a nessuno,
         * perche' arriva solo dove il turno era gia' fallito. Ed e' anche il
         * posto giusto per un'altra ragione — un turno che nessuna facolta' ha
         * saputo servire, se e' prosa, e' esattamente la situazione in cui una
         * persona sta INSEGNANDO qualcosa. Rispondere con un saggio invece di
         * imparare e' il peggiore dei due esiti possibili.
         *
         * La condizione e' strutturale, non una lista: piu' di una frase, e non
         * una domanda. Una frase sola ha gia' la sua strada (mod_knowledge). */
        /* gen407: anche qui, prima di coprire un muro con un saggio, si prova a
         * imparare. Il turno e' formalmente `handled` — un modulo ha risposto —
         * ma la risposta ignora il soggetto, cioe' e' una resa travestita. Su
         * una prosa che insegna, imparare e' sempre meglio che commentare. */
        if (surrendered && teaching_prose(input) &&
            prose_learn_lead(b, canon, input, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", "prose_learn");
            surrendered = 0;
        }
        if (surrendered &&
            structured_analysis_lead(b, canon, input, 2, out, out_size)) {
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s",
                     "analysis_last_resort");
        }
        for (size_t i = 0; i < nm; i++) {
            char marker[KB_TERM_LEN];
            snprintf(marker, sizeof marker, "%s", markers[i]);
            size_t ml = strlen(marker);
            char *m = marker;
            if (ml >= 2 && m[0] == '"' && m[ml - 1] == '"') { m[ml - 1] = '\0'; m++; }
            if (!*m || !strstr(lowered, m)) continue;
            if (structured_analysis_lead(b, canon, input, 2, out, out_size)) {
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s",
                         "analysis_last_resort");
            }
            break;
        }
    }

    if (!handled) {
        /* gen216 (glue G2): before giving up, try resolving an entity pronoun to the
         * recent entity and re-dispatching — carries a reference across turns. */
        if (coref_resolve(b, canon, out, out_size)) {
            handled = 1;
            if (!handled_by_discourse) update_topics(b, canon);
        } else if (b && repair_requested(b, canon, input)) {
            /* gen410: il ciclo di autoriparazione si CHIEDE. Costa un turno per
             * ogni tentativo, quindi non puo' girare dentro una conversazione
             * ordinaria; e dev'essere esplicito anche per una ragione piu'
             * importante — parrot0 che cambia la propria macchineria e' un
             * atto, e un atto si annuncia. */
            char learned[900]; learned[0] = '\0';
            int nfix = brain_self_repair(b, learned, sizeof learned);
            char msg[1100];
            if (nfix > 0)
                { 
                  char _v0[48]; snprintf(_v0, sizeof _v0, "%d", nfix);
  const KbResponseSlot _rs[] = { { "nfix", _v0 }, { "learned", learned } };
                  kb_term_say(b, "i_bridged_x_of_my_own_walls_by_teaching_myse", _rs, 2, msg, sizeof msg); }
            else
                kb_term_say(b, "self_repair_none", NULL, 0, msg, sizeof msg);
            put(msg, out, out_size);
            handled = 1;
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", "self_repair");
    } else if (b && prose_learn_lead(b, canon, input, out, out_size)) {
            /* gen407 — PRIMA DI COPRIRE UN MURO CON UN SAGGIO, PROVA A IMPARARE.
             *
             * Misurato: la prosa della pagina di photosynthesis, incollata in
             * conversazione, rendeva ZERO fatti e duecento parole di analisi
             * generica; la stessa prosa letta da quella pagina ne rendeva otto.
             * La stessa conoscenza entrava o no a seconda di CHI l'aveva
             * portata — e il motivo per cui non se n'era accorto nessuno e' che
             * la strada detta non produceva un muro visibile: murava davvero, e
             * l'analisi di ultima istanza lo copriva.
             *
             * Il gancio sta qui, e non prima, apposta: non toglie il turno a
             * nessuno, perche' arriva solo dove nessuna facolta' ha saputo
             * servirlo. Ed e' anche il posto piu' giusto per un'altra ragione —
             * un turno che nessuno ha saputo servire, se e' prosa dichiarativa,
             * e' esattamente la situazione in cui una persona sta INSEGNANDO
             * qualcosa. Rispondere con un saggio invece di imparare e' il
             * peggiore dei due esiti possibili. */
            handled = 1;
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s", "prose_learn");
            if (!handled_by_discourse) update_topics(b, canon);
        } else if (b && structured_analysis_lead(b, canon, input, 2,
                                                 out, out_size)) {
            /* gen362 (Fase 5, anti-wall): the analytical planner runs a LAST
             * time here, past every specialized consumer, so it can never steal
             * a turn a competent module would have served — and so a compound
             * guard that yields to a consumer which then has no facts no longer
             * costs a blind wall. At this point nothing else claimed the turn,
             * so a complete plan bound to the turn's own subject is strictly
             * better than "I don't understand that yet", and it stays honest:
             * it reasons about what was named and asserts nothing about it. */
            handled = 1;
            snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
            snprintf(b->last_module, sizeof b->last_module, "%s",
                     "analysis_last_resort");
            if (!handled_by_discourse) update_topics(b, canon);
        } else {
            not_understood(b, canon, input, out, out_size);
            if (b) {
                snprintf(b->last_reply, sizeof b->last_reply, "%s", out);
                snprintf(b->last_module, sizeof b->last_module, "%s", "fallback");
            }
        }
    }
    /* gen222 (glue, symptom #3): carry a bare numeric answer as the KB last_result so a
     * later precisation ("and times 3") can continue the computation. No-op for any
     * non-numeric reply. */
    /* gen404: LA LACUNA SI CHIUDE DA SOLA — la terza freccia dell'anello.
     *
     * Una lacuna di macchineria e' l'unica classe DECIDIBILE proprio per questo:
     * la prova del rimedio non e' un giudizio, e' che il turno che murava ora
     * risponde. Quindi non serve nessuno che dichiari chiusa una lacuna — la
     * chiude il fatto stesso di aver risposto.
     *
     * Senza questa riga il registro sarebbe monotono crescente, cioe' un elenco
     * di rimpianti invece di una misura: il numero delle lacune aperte deve
     * SCENDERE quando la conoscenza cresce, altrimenti non misura niente. */
    return turn_done(b, canon, input, out, out_size);
}
