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
    if (has_thanks) { put("You're welcome!", out, out_size); return 1; }

    /* apology — "scusa", "sorry", "mi dispiace" etc. */
    if (has_apology) { put("No problem.", out, out_size); return 1; }

    /* wellbeing check-in — gen73: patterns from kb/core/social.p0 */
    if (has_social_pattern(b, "wellbeing", buf))
        { put("I'm well, thanks. How can I help?", out, out_size); return 1; }

    /* gen225 (basic-chat cat.1): phatic speech acts beyond hello/bye/thanks —
     * valediction, felicitation, well-wishing, condolence, blessing, politeness.
     * Recognized phrases are social_pattern(type, …) and replies are
     * response_template(type, …), both KB knowledge (EN+IT). The C knows only
     * the structural list of act TYPES, not the vocabulary; kb_response falls
     * back to a literal only if the KB file is absent, so the agent is engaged
     * but never mute. */
    {
        static const char *const phatic[] = {
            "goodnight", "felicitation", "wellwish",
            "condolence", "blessing", "politeness", NULL };
        for (size_t i = 0; phatic[i]; i++) {
            if (has_social_pattern(b, phatic[i], buf)) {
                if (!kb_response(b, phatic[i], NULL, out, out_size))
                    put("That's kind of you — thank you!", out, out_size);
                return 1;
            }
        }
    }

    /* position-disambiguated ambiguous marker: "ciao" opens early, closes late */
    if (has_ambiguous) {
        put(b->turns <= 2 ? "Hi there!" : "Goodbye!", out, out_size);
        return 1;
    }

    /* explicit opening / closing markers */
    if (has_opening) { put("Hi there!", out, out_size); return 1; }
    if (has_closing) { put("Goodbye!", out, out_size); return 1; }

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
        put(":) Glad you're enjoying the conversation.", out, out_size);
        return 1;
    }

    /* the elimination move: a single contentless word as the opener is, by
     * exclusion, phatic contact — greet and invite content, without listing it.
     * Pure numbers are not contact, so require an alphabetic token. */
    if (nw == 1 && b->turns <= 1 && isalpha((unsigned char)w[0][0])) {
        put("Hi there! What would you like to talk about?", out, out_size);
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
static int is_binary_choice(const char *norm) {
    char buf[128];
    if (strlen(norm) >= sizeof buf) return 0;
    strcpy(buf, norm);
    char *w[16];
    size_t nw = split_words(buf, w, 16);
    if (nw < 3 || nw > 5) return 0;
    size_t orpos = nw, orcount = 0;
    for (size_t i = 0; i < nw; i++)
        if (strcmp(w[i], "or") == 0) { orpos = i; orcount++; }
    if (orcount != 1 || orpos == 0 || orpos == nw - 1) return 0;
    size_t left = orpos, right = nw - orpos - 1;
    if (left < 1 || left > 2 || right < 1 || right > 2) return 0;
    static const char *const bad[] = {
        "what","which","who","how","when","where","why","is","are","am","do",
        "does","did","can","could","will","would","should","cosa","che","qual",
        "quale","dove","quando","perche", NULL };
    for (size_t i = 0; bad[i]; i++)
        if (strcmp(w[0], bad[i]) == 0) return 0;
    return 1;
}

static int mod_chitchat(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    if (!b) return 0;

    /* gen228 (basic-chat cat.86): a bare binary choice answered honestly —
     * parrot0 has no genuine preference. "would you rather" is its own honest
     * opener. Cues/replies are KB-first (intent_cue/response_template, EN+IT). */
    if (kb_cue_match(b, "would_rather", norm)) {
        if (!kb_response(b, "would_rather", NULL, out, out_size))
            put("I'll play along — what are my two options?", out, out_size);
        return 1;
    }
    if (is_binary_choice(norm)) {
        if (!kb_response(b, "binary_choice", NULL, out, out_size))
            put("Between the two? I don't have a real preference.", out, out_size);
        return 1;
    }

    /* register signals: emoji, ASCII emoticons, laughter, stage-direction emotes */
    int emoji = has_emoji(raw);
    int emoticon = cue(norm, ":)") || cue(norm, ":-)") || cue(norm, ";)") ||
                   cue(norm, ":d") || cue(norm, ":p") || cue(norm, " xd") ||
                   cue(norm, ":(") || cue(norm, "<3");
    int laugh = cue(norm, "haha") || cue(norm, "lol") || cue(norm, "hehe") ||
                cue(norm, "ahah") || cue(norm, "lmao") || cue(norm, "hihi") ||
                cue(norm, "rofl") || cue(norm, "eheh") || cue(norm, "ahaha") ||
                cue(norm, "battuta") || cue(norm, "scherzo") || cue(norm, "hilarious");
    const char *t = raw; while (*t && isspace((unsigned char)*t)) t++;
    int emote = (*t == '*');     /* "*sighs heavily*", "*rolls eyes*" */

    /* Tailored sub-registers — clear enough to answer on the cue alone. NOTE:
     * apology is deliberately NOT handled here — pure apologies are mod_social's,
     * and an apology tangled with a real request must stay an honest wall. */
    int frustration = cue(norm, "repeat") || cue(norm, "repeating") ||
                      cue(norm, "ripetiz") || cue(norm, "stessa cosa") ||
                      cue(norm, "copying") || cue(norm, "copiando") ||
                      cue(norm, "broken record") || cue(norm, "stuck") ||
                      cue(norm, "useless") || cue(norm, "inutile") ||
                      cue(norm, "sei rotto") || cue(norm, "you keep") ||
                      cue(norm, "keep saying") || cue(norm, "three times") ||
                      cue(norm, "on purpose") || cue(norm, "getting stale") ||
                      cue(norm, "default reply") || cue(norm, "not a good chatbot") ||
                      cue(norm, "dont know anything") || cue(norm, "can not even") ||
                      cue(norm, "can not learn") || cue(norm, "robots can not") ||
                      cue(norm, "doing this on purpose") || cue(norm, "broken") ||
                      cue(norm, "glitch") || cue(norm, "trolling") ||
                      cue(norm, "echoing") || cue(norm, "repetitive") ||
                      cue(norm, "even trying") || cue(norm, "done here") ||
                      cue(norm, "schifo") || cue(norm, "vague phrases") ||
                      cue(norm, "do not know anything") || cue(norm, "confusing") ||
                      cue(norm, "flip a table") || cue(norm, "what a day");
    int encourage   = cue(norm, "imparerai") || cue(norm, "you'll learn") ||
                      cue(norm, "youll learn") || cue(norm, "no worries") ||
                      cue(norm, "nessun problema") || cue(norm, "non preoccupar") ||
                      cue(norm, "take your time") || cue(norm, "keep trying") ||
                      cue(norm, "tranquillo") || cue(norm, "figurati") ||
                      cue(norm, "takes time") || cue(norm, "figure it out") ||
                      cue(norm, "well figure") || cue(norm, "no rush") ||
                      cue(norm, "we'll get there") || cue(norm, "niente paura") ||
                      cue(norm, "provo a") || cue(norm, "take it back") ||
                      cue(norm, "ancora imparare") || cue(norm, "devi imparare");
    /* agreement / acknowledgement of parrot0's honest self-description */
    int agree       = cue(norm, "fair enough") || cue(norm, "that's fair") ||
                      cue(norm, "thats fair") || cue(norm, "fair actually") ||
                      cue(norm, "makes sense") || cue(norm, "fair point") ||
                      cue(norm, "capisco") || cue(norm, "thats reasonable") ||
                      cue(norm, "that's reasonable");
    /* casual contact phrasings that no content module claims */
    int casual      = cue(norm, "what's up") || cue(norm, "whats up") ||
                      cue(norm, "what is up") || cue(norm, "just saying hi") ||
                      cue(norm, "my bad") || cue(norm, "wassup") ||
                      cue(norm, "what do you want") || cue(norm, "what do you know") ||
                      cue(norm, "che fai") || cue(norm, "keep it formal") ||
                      cue(norm, "your day") || cue(norm, "u good") ||
                      cue(norm, "you good") || cue(norm, "you work") ||
                      cue(norm, "you there") || cue(norm, "solo chat") ||
                      cue(norm, "reaching out") || cue(norm, "what even are you") ||
                      cue(norm, "pappagallo") || cue(norm, "sei onesto") ||
                      cue(norm, "wait what") || cue(norm, "okay helpful") ||
                      cue(norm, "vuol dire") || cue(norm, "che significa") ||
                      cue(norm, "literally anything") || cue(norm, "lovely to have") ||
                      cue(norm, "figuring it out") || cue(norm, "sono qui") ||
                      cue(norm, "whole time");
    /* terms of endearment / excitement mark the casual register too */
    int endear      = cue(norm, "sweety") || cue(norm, "sweetie") ||
                      cue(norm, "babes") || cue(norm, "friend") ||
                      cue(norm, "buddy") || cue(norm, "amico") ||
                      cue(norm, "tesoro");
    int language    = cue(norm, "in inglese") || cue(norm, "in italiano") ||
                      cue(norm, "in english") || cue(norm, "in italian") ||
                      cue(norm, "parliamo in") || cue(norm, "talk in") ||
                      cue(norm, "english only") || cue(norm, "parli italiano") ||
                      cue(norm, "speak english") || cue(norm, "parli inglese");
    int filler      = cue(norm, "nothing much") || cue(norm, "just hanging") ||
                      cue(norm, "just bored") || cue(norm, "just vibing") ||
                      cue(norm, "just chatting") || cue(norm, "hanging out") ||
                      cue(norm, "solo chiacchiere") || cue(norm, "parliamo e basta") ||
                      cue(norm, "just chat") || cue(norm, "just talk") ||
                      cue(norm, "can we talk") || cue(norm, "lets talk") ||
                      cue(norm, "let us talk") || cue(norm, "parliamo e basta") ||
                      cue(norm, "facciamo due chiacchiere");
    int no_topic    = cue(norm, "i do not know what to say") ||
                      cue(norm, "i don't know what to say") ||
                      cue(norm, "i dont know what to say") ||
                      cue(norm, "non so cosa dire") || cue(norm, "not so cosa dire") ||
                      cue(norm, "boh") || cue(norm, "no idea what to say") ||
                      cue(norm, "say something") || cue(norm, "tell me something") ||
                      cue(norm, "dimmi qualcosa") || cue(norm, "intrattienimi") ||
                      cue(norm, "entertain me");
    int mood_bored  = cue(norm, "i am bored") || cue(norm, "im bored");
    int mood_down   = mood_bored || cue(norm, "i am tired") || cue(norm, "im tired") ||
                      cue(norm, "sono stanco") || cue(norm, "sono stanca") ||
                      cue(norm, "am stanco") || cue(norm, "am stanca") ||
                      cue(norm, "bad day") || cue(norm, "giornata no") ||
                      cue(norm, "mi annoio") || cue(norm, "sono annoiato") ||
                      cue(norm, "sono annoiata") || cue(norm, "rough day") ||
                      cue(norm, "not feeling great");
    int mood_up     = cue(norm, "i am happy") || cue(norm, "im happy") ||
                      cue(norm, "sono felice") || cue(norm, "great day") ||
                      cue(norm, "bella giornata") || cue(norm, "that is cool") ||
                      cue(norm, "thats cool") || cue(norm, "che bello") ||
                      cue(norm, "nice") || cue(norm, "cool");

    if (frustration) {
        put("I know I repeat myself — I'm a small bot and honest about my limits. "
            "What would you like to try?", out, out_size);
        return 1;
    }
    if (encourage) { put("Thanks — I'm learning as we go.", out, out_size); return 1; }
    if (agree)     { put("Glad that lands. What would you like to do next?",
                         out, out_size); return 1; }
    if (language)  { put("We can chat in either language — I'll do my best.",
                         out, out_size); return 1; }
    if (no_topic)  { put("We can start simple: tell me something about your day, or ask me to remember or reason about a small fact.",
                         out, out_size); return 1; }
    if (mood_down) {
        snprintf(b->user_mood, sizeof b->user_mood, "%s", mood_bored ? "bored" : "tired");
        b->has_user_mood = 1;
        put("Sounds like a low-energy moment. We can keep it light — tell me one thing that happened, or ask me something small.",
            out, out_size);
        return 1;
    }
    if (mood_up) {
        snprintf(b->user_mood, sizeof b->user_mood, "%s", "happy");
        b->has_user_mood = 1;
        put("Nice. Tell me what made it good, or give me a small thing to reason about.",
            out, out_size);
        return 1;
    }
    if (filler)    { put("Happy to just chat. Tell me a little, and I'll follow the thread as best I can.",
                         out, out_size); return 1; }
    if (casual)    { put("Hey! I'm here. Ask me something, or tell me about your day?",
                         out, out_size); return 1; }

    /* Generic affective contact (emoji/emoticon/laughter/emote/endearment and
     * nothing more specific): engage in register, rotating so it never becomes a
     * broken record. Still honest — it names no understanding of the content. */
    if (emoji || emoticon || laugh || emote || endear) {
        static const char *const v[] = {
            "I'm enjoying this — what's on your mind?",
            "Ha, you're playful! Ask me something?",
            "I'm a simple bot, but I'm here for it. Go on?",
        };
        put(v[b->turns % 3], out, out_size);
        return 1;
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
static int is_discourse_opener(char **w, size_t nw, size_t *skip) {
    if (nw == 0) return 0;
    char tmp[64];
    snprintf(tmp, sizeof tmp, "%s", w[0]);
    const char *t = strip_edge_punct(tmp);
    static const char *const one[] = {
        "anyway", "anyhow", "so", "well", "ok", "okay", "now", "look", "listen",
        "actually", "right", "alright", "hey", "um", "uh", "hmm",
        "comunque", "allora", "insomma", "senti", "guarda", "dunque", "beh",
        NULL
    };
    /* "by the way" -> skip 3 tokens */
    if (nw >= 3 && strcmp(t, "by") == 0) {
        char a[64], c[64];
        snprintf(a, sizeof a, "%s", w[1]); snprintf(c, sizeof c, "%s", w[2]);
        if (strcmp(strip_edge_punct(a), "the") == 0 &&
            strcmp(strip_edge_punct(c), "way") == 0) { *skip = 3; return 1; }
    }
    for (const char *const *p = one; *p; p++)
        if (strcmp(t, *p) == 0) { *skip = 1; return 1; }
    return 0;
}

/* a hedge / hesitation marker anywhere in the turn. */
static int has_hedge(char **w, size_t nw) {
    static const char *const h[] = {
        "maybe", "perhaps", "guess", "suppose", "dunno", "probably", "unsure",
        "kinda", "sorta", "idk", "forse", "magari", "boh", "credo",
        NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = h; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* a contrastive connective anywhere in the turn. */
static int has_contrastive(char **w, size_t nw) {
    static const char *const c[] = {
        "but", "however", "though", "although", "yet",
        "pero", "per`o", "tuttavia", "invece",
        NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = c; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* a negation marker anywhere in the turn. */
static int has_negation(char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (strcmp(t, "not") == 0 || strcmp(t, "no") == 0 ||
            strcmp(t, "never") == 0 || strcmp(t, "dont") == 0 ||
            strcmp(t, "nor") == 0) return 1;
    }
    return 0;
}

/* True if token `t` is a STANCE PREDICATE: a word that, when negated, expresses
 * a disagreement about the prior claim — "(don't) agree", "(not) right/sure/
 * true/correct/convinced", "(doesn't) make sense". These are PREDICATES, not
 * mere objects: "that"/"you" alone are not stance ("dont say that" is an order,
 * not a disagreement), so the move keys on the predicate. */
static int is_stance_pred(const char *t) {
    static const char *const s[] = {
        "agree", "right", "sure", "true", "correct", "convinced", "sense",
        "convince", "persuaded", "ragione", "giusto", "vero", "accordo",
        "sicuro", "convinto", "convince", "d'accordo", NULL
    };
    for (const char *const *p = s; *p; p++)
        if (strcmp(t, *p) == 0) return 1;
    return 0;
}

/* A disagreement is a NEGATED stance predicate ("i don't agree", "that is not
 * right", "not so sure", "non sono d'accordo"), or the standalone verb
 * "disagree"/"dissento". Shape, not phrase: any negation co-occurring with a
 * stance predicate in a short turn reads as pushback. */
static int is_disagreement(char **w, size_t nw) {
    int neg = 0, stance = 0, plain = 0;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (strcmp(t, "not") == 0 || strcmp(t, "no") == 0 ||
            strcmp(t, "never") == 0 || strcmp(t, "dont") == 0) neg = 1;
        if (is_stance_pred(t)) stance = 1;
        if (strcmp(t, "disagree") == 0 || strcmp(t, "dissento") == 0 ||
            strcmp(t, "dubito") == 0) plain = 1;
    }
    return plain || (neg && stance);
}

/* True if token is an OPEN QUANTIFIER object — the placeholder a content-free
 * soft request reaches for ("something", "anything", "qualcosa", "qualunque
 * cosa"). A CONCRETE object ("a story", "about C") is not open, so "tell me a
 * story" stays a real (unfulfillable) request and hits the honest wall instead
 * of this fill-the-silence move. */
static int has_open_quantifier(char **w, size_t nw) {
    static const char *const q[] = {
        "something", "anything", "qualcosa", "qualunque", "whatever", NULL
    };
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        for (const char *const *p = q; *p; p++)
            if (strcmp(t, *p) == 0) return 1;
    }
    return 0;
}

/* True if the turn carries a CONTENT PREDICATE — something a content module
 * could actually act on. This is the gate that keeps the pragmatic moves from
 * swallowing real tasks: a digit or arithmetic operator, an assertion shape
 * (" is a "/" is an "), or a token that is a known KB predicate or entity. */
static int has_content_predicate(Brain *b, const char *canon, char **w, size_t nw) {
    if (cue(canon, " is a ") || cue(canon, " is an ") ||
        cue(canon, " plus ") || cue(canon, " minus ") ||
        cue(canon, " times ") || cue(canon, "+") || cue(canon, "-") ||
        cue(canon, "*") || cue(canon, "=")) return 1;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (!t || !*t) continue;
        if (isdigit((unsigned char)t[0])) return 1;
        if (b && b->kb && strlen(t) >= 3) {
            if (kb_knows_pred(b->kb, t) && !is_internal_pred(t)) return 1;
        }
    }
    return 0;
}

/* Pull the TOPIC object out of a topic-intro frame: the head noun after
 * "about/discuss/discutere/switch to/change to/parlare di/parliamo di". Returns
 * 1 and writes the object (first substantive token after the cue) into dst. */
static int topic_object(char **w, size_t nw, char *dst, size_t dstn) {
    static const char *const after[] = {
        "about", "discuss", "to", "of", "di", "su", NULL
    };
    for (size_t i = 0; i + 1 < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        int is_after = 0;
        for (const char *const *p = after; *p; p++)
            if (strcmp(t, *p) == 0) { is_after = 1; break; }
        if (!is_after) continue;
        for (size_t j = i + 1; j < nw; j++) {
            char o[64];
            snprintf(o, sizeof o, "%s", w[j]);
            const char *ot = strip_edge_punct(o);
            /* the head noun: first alphabetic token that is not a bare article.
             * We deliberately do NOT filter on the general stopword lexicon — a
             * topic word like "formaggio" happens to be listed there for a
             * chitchat test, but after "di"/"about" it is the genuine topic. */
            static const char *const arts[] = {
                "the","a","an","il","lo","la","i","gli","le","un","una","uno",NULL
            };
            int art = 0;
            for (const char *const *p = arts; *p; p++)
                if (strcmp(ot, *p) == 0) { art = 1; break; }
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

    int hedge       = has_hedge(w, nw);
    int contrastive = has_contrastive(w, nw);
    int negation    = has_negation(w, nw);
    int disagree    = is_disagreement(w, nw);

    /* ---- MOVE 5: disagreement. A NEGATED stance predicate ("i don't agree",
     * "that's not right", "non sono d'accordo"), or a contrastive pushback that
     * also negates ("nice try, but no") — with no content to act on. Keys on the
     * stance predicate, so an imperative like "dont say that" is NOT disagreement. */
    if (disagree || (contrastive && negation && nw <= 8)) {
        put("Fair enough — tell me where I went wrong, and we can take it from there.",
            out, out_size);
        return 1;
    }

    /* ---- MOVE 4: hesitation. A hedge with nothing concrete to chew on. */
    if (hedge && nw <= 9) {
        put("No pressure — we can take it slowly. What's on your mind?",
            out, out_size);
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
        int modal_open = strcmp(first, "can") == 0 || strcmp(first, "could") == 0 ||
                         strcmp(first, "shall") == 0 || strcmp(first, "lets") == 0 ||
                         strcmp(first, "let") == 0;
        int switch_verb = cue(buf, "switch to") || cue(buf, "change to") ||
                          cue(buf, "move on to") || cue(buf, "cambiamo");
        int frame = cue(buf, "talk about") || cue(buf, "chat about") ||
                    cue(buf, "discuss") || cue(buf, "talk of") ||
                    cue(buf, "parlare di") || cue(buf, "parliamo di") ||
                    cue(buf, "discutere") || cue(buf, "discutiamo");
        int invite = switch_verb || (modal_open && frame);
        char topic[40];
        if (invite && topic_object(w, nw, topic, sizeof topic)) {
            snprintf(b->current_topic, sizeof b->current_topic, "%s", topic);
            b->has_current_topic = 1;
            char msg[160];
            snprintf(msg, sizeof msg,
                     "Sure, let's talk about %s. What about %s is on your mind?",
                     topic, topic);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* ---- MOVE 2: soft request. An imperative directed at me ("tell/give/show
     * me", "say") with NO content object — an open request to fill the silence. */
    {
        int soft = strcmp(first, "tell") == 0 || strcmp(first, "give") == 0 ||
                   strcmp(first, "say") == 0 || strcmp(first, "show") == 0 ||
                   strcmp(first, "share") == 0 || strcmp(first, "dimmi") == 0 ||
                   strcmp(first, "dammi") == 0 || strcmp(first, "raccontami") == 0;
        /* OPEN-ended only: the object must be a quantifier placeholder
         * ("something/anything/qualcosa"), which is what distinguishes a
         * fill-the-silence request from a real (often unfulfillable) one — "tell
         * me a story about dragons" / "tell me about C" name a CONCRETE object, so
         * they fall through to the honest wall, not this move. The bare 3-token
         * "tell me something" family stays chitchat's established no-topic
         * register, so we require >= 4 tokens here and leave those to chitchat. */
        if (soft && has_open_quantifier(w, nw) && nw >= 4 && nw <= 8) {
            put("Happy to. Pick a thread — your day, a small fact to remember, or something to reason about — and I'll run with it.",
                out, out_size);
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
static int looks_solfege(const char *s) {
    static const char *notes[] = {"do","re","mi","fa","sol","la","si","ti",NULL};
    char buf[256]; copy_trim(buf, sizeof buf, s);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    if (nw < 3) return 0;
    for (size_t i = 0; i < nw; i++) {
        int ok = 0;
        for (size_t k = 0; notes[k]; k++) if (strcmp(w[i], notes[k]) == 0) { ok = 1; break; }
        if (!ok) return 0;
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
static int looks_code(const char *s, char **w, size_t nw) {
    /* Strong, unambiguous code markers. NOTE: a bare '(' is NOT one of these —
     * a natural-language sentence with a parenthetical aside ("Chicago (800
     * miles away)", "scatters (spreads out)") is prose, not code (gen240). */
    if (cue(s, "{") || cue(s, "}") || cue(s, ";") ||
        cue(s, "==") || cue(s, "<html") || cue(s, "select * from"))
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
        static const char *kw[] = {"while","for","if","def","class","else",
                                   "elif","try","with",NULL};
        for (size_t k = 0; kw[k]; k++)
            if (strcmp(w[0], kw[k]) == 0) return 1;
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

static int find_code_section(const char *input, char *code, size_t code_size) {
    const char *p = input;
    while (*p && *p != ':') p++;
    if (*p == ':') {
        p++;
        while (*p && isspace((unsigned char)*p)) p++;
        size_t n = 0;
        while (*p && n + 1 < code_size) { code[n++] = *p; p++; }
        while (n > 0 && isspace((unsigned char)code[n - 1])) n--;
        code[n] = '\0';
        return n > 1;
    }
    return 0;
}

static int identify_code_lang(const char *code, Brain *b) {
    int c_kw = 0, py_kw = 0;
    if (b && b->kb) {
        char buf[512]; snprintf(buf, sizeof buf, "%s", code);
        char *w[128]; size_t nw = split_words(buf, w, 128);
        for (size_t i = 0; i < nw; i++) {
            char *t = w[i]; size_t tl = strlen(t);
            while (tl > 0 && (t[tl-1] == ';' || t[tl-1] == ':' ||
                              t[tl-1] == '(' || t[tl-1] == ')')) { t[tl-1] = '\0'; tl--; }
            if (tl < 2) continue;
            const char *args_c[] = {"c", t};
            if (kb_query(b->kb, "keyword", args_c, 2)) c_kw++;
            const char *args_py[] = {"python", t};
            if (kb_query(b->kb, "keyword", args_py, 2)) py_kw++;
        }
    }
    /* Surface features — used as tiebreaker and also when KB is sparse.
     * C markers: int, void, #include, semicolons, braces, printf, scanf, malloc.
     * Python markers: def, import, print(, : after for/if/while/def/class, indentation. */
    if (strstr(code, "int ") || strstr(code, "void ") || strstr(code, "char ") ||
        strstr(code, "#include") || strstr(code, "printf(") || strstr(code, "scanf(") ||
        strstr(code, "malloc("))
        c_kw += 2;
    if (strstr(code, "def ") || strstr(code, "import ") || strstr(code, "print(") ||
        strstr(code, "class ") || strstr(code, "elif ") || strstr(code, "lambda ") ||
        strstr(code, "self.") || strstr(code, "__init__"))
        py_kw += 2;
    /* Semicolons and braces strongly suggest C over Python */
    if (strchr(code, ';') || strchr(code, '{'))
        c_kw++;
    if (c_kw > py_kw) return 1;  /* C */
    if (py_kw > c_kw) return 2;  /* Python */
    return 0;                    /* unknown */
}

static int check_missing_semicolons(const char *code, char *findings,
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
    static const char *const kw[] = {"int","char","float","double","void","long","return",NULL};
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
        int is_kw = 0;
        for (const char *const *k = kw; *k; k++)
            if (strcmp(fw, *k) == 0) { is_kw = 1; break; }
        if (is_kw && l[len-1] != '}') issues++;
        /* Check for statement before closing brace: scan for "keyword ... }"
         * e.g. "int main() { return 0 }" — "return 0 }" has no semicolon */
        if (l[len-1] == '}' && strchr(l, '{')) {
            const char *p = l;
            while ((p = strstr(p, "return ")) != NULL) {
                const char *q = p + 7; while (*q && isspace((unsigned char)*q)) q++;
                /* Find next } or end */
                const char *br = strchr(q, '}');
                size_t n = br ? (size_t)(br - p) : strlen(p);
                if (n > 0 && p[n-1] != ';') { issues++; break; }
                p++;
            }
        }
    }
    if (issues == 1)
        snprintf(findings, findings_size, "Missing semicolon at the end of a statement.");
    else if (issues > 1)
        snprintf(findings, findings_size, "Missing semicolons at the end of %d statements.", issues);
    return issues;
}

static int check_type_mismatch(const char *code, char *findings,
                                size_t findings_size) {
    /* Simple patterns: "int x = \"...\"" (string assigned to int)
     *                 "char y = 42" (number assigned to char pointer) */
    if (strstr(code, "int ") && strstr(code, "= \"") && strstr(code, "\"")) {
        snprintf(findings, findings_size,
            "Type mismatch: a string is assigned to an int variable. "
            "Use char * or char[] for strings.");
        return 1;
    }
    /* char *x = number (without quotes) */
    { const char *cp = code;
      while ((cp = strstr(cp, "char *")) != NULL) {
          const char *eq = strstr(cp, "=");
          if (eq) {
              const char *v = eq + 1;
              while (*v && isspace((unsigned char)*v)) v++;
              if (*v == '\"' || isalpha((unsigned char)*v)) {
                  int has_digit = 0;
                  for (const char *d = v; *d && *d != ';' && *d != '\n'; d++)
                      if (isdigit((unsigned char)*d)) { has_digit = 1; break; }
                  if (has_digit && !strstr(v, "\"")) {
                      snprintf(findings, findings_size,
                          "Suspicious assignment: a number assigned to a char * variable. "
                          "If this is meant as a character literal, use single quotes: 'x'.");
                      return 1;
                  }
              }
          }
          cp++;
      }
    }
    return 0;
}

static int check_unclosed_string(const char *code, char *findings,
                                  size_t findings_size) {
    int quotes = 0;
    for (const char *p = code; *p; p++)
        if (*p == '\"' && (p == code || *(p-1) != '\\')) quotes++;
    if (quotes % 2 != 0) {
        snprintf(findings, findings_size,
            "Unclosed string literal: there is an odd number of double-quote characters.");
        return 1;
    }
    return 0;
}

static int check_balanced_braces(const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '{') open++;
        if (*p == '}') open--;
    }
    if (open > 0) {
        snprintf(findings, findings_size,
            "Unbalanced braces: %d more opening brace(s) than closing.", open);
        return 1;
    }
    if (open < 0) {
        snprintf(findings, findings_size,
            "Unbalanced braces: %d more closing brace(s) than opening.", -open);
        return 1;
    }
    return 0;
}

static int check_balanced_parens(const char *code, char *findings,
                                  size_t findings_size) {
    int open = 0;
    for (const char *p = code; *p; p++) {
        if (*p == '(') open++;
        if (*p == ')') open--;
    }
    if (open != 0) {
        snprintf(findings, findings_size,
            "Unbalanced parentheses: %d more %s than %s.",
            open > 0 ? open : -open,
            open > 0 ? "opening" : "closing",
            open > 0 ? "closing" : "opening");
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
            if (strcmp(fname, "if") == 0 || strcmp(fname, "while") == 0 ||
                strcmp(fname, "for") == 0 || strcmp(fname, "return") == 0 ||
                strcmp(fname, "sizeof") == 0 || strcmp(fname, "switch") == 0)
                continue;
            /* Check against KB */
            { const char *fa[] = { fname };
            if (!kb_query(b->kb, "c_stdlib", fa, 1)) {
                size_t fl = strlen(t);
                if (fl < sizeof(fname)) {
                    snprintf(findings, findings_size,
                        "Unknown function: \"%.*s\" is not a standard C library function. "
                        "Did you spell it correctly or forget an #include?", (int)fl, t);
                    return 1;
                }
            }
            }
        }
    }
    return 0;
}

static void lang_name(int lang, char *out, size_t out_size) {
    if (lang == 1) snprintf(out, out_size, "C");
    else if (lang == 2) snprintf(out, out_size, "Python");
    else snprintf(out, out_size, "unknown");
}

