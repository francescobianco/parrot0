/* --- module: meta --------------------------------------------------------
 * Meta-conversation is not world knowledge: the user is asking about this
 * exchange itself (attention, reading, understanding, repetition, current
 * activity). Answer from local state and limits instead of falling through to
 * the knowledge fallback or the identity module. */
static int mod_meta(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    while (len > 0 && (buf[len - 1] == '?' || buf[len - 1] == '!' ||
                       buf[len - 1] == '.' || buf[len - 1] == ' '))
        buf[--len] = '\0';

    int attention = cue(buf, "paying attention") ||
                    cue(buf, "reading my messages") ||
                    cue(buf, "read my messages") ||
                    cue(buf, "listening") ||
                    cue(buf, "ascoltando") ||
                    cue(buf, "mi ascolti");
    if (attention) {
        if (strcmp(b->last_reply, "Yes. I read each message in this conversation.") == 0)
            put("I am reading this conversation turn by turn.", out, out_size);
        else
            put("Yes. I read each message in this conversation.", out, out_size);
        return 1;
    }

    int activity = cue(buf, "what are you up to") ||
                   cue(buf, "what are you doing") ||
                   cue(buf, "che stai facendo") ||
                   cue(buf, "cosa stai facendo");
    if (activity) {
        char msg[128];
        snprintf(msg, sizeof msg, "I'm tracking this conversation; this is turn %lu.",
                 b->turns);
        put(msg, out, out_size);
        return 1;
    }

    /* gen82: session statistics — "how many turns?", "how long?" */
    int turn_count = cue(buf, "how many turns") ||
                     cue(buf, "how many messages") ||
                     cue(buf, "quanti turni") ||
                     cue(buf, "quanti messaggi");
    if (turn_count) {
        char msg[96];
        snprintf(msg, sizeof msg, "We've exchanged %lu turn(s).", b->turns);
        put(msg, out, out_size);
        return 1;
    }
    int session_time = cue(buf, "how long have we been talking") ||
                       cue(buf, "how long has this been") ||
                       cue(buf, "da quanto tempo parliamo") ||
                       cue(buf, "da quanto parliamo");
    if (session_time) {
        time_t now = time(NULL);
        long elapsed = (long)(now - b->start_time);
        char msg[128];
        if (elapsed < 60)
            snprintf(msg, sizeof msg, "We've been talking for %ld second(s).", elapsed);
        else if (elapsed < 3600)
            snprintf(msg, sizeof msg, "We've been talking for %ld minute(s) and %ld second(s).",
                     elapsed / 60, elapsed % 60);
        else
            snprintf(msg, sizeof msg, "We've been talking for %ld hour(s) and %ld minute(s).",
                     elapsed / 3600, (elapsed % 3600) / 60);
        put(msg, out, out_size);
        return 1;
    }

    int understand = cue(buf, "do you understand me") ||
                     cue(buf, "understand what i say") ||
                     cue(buf, "capisci") ||
                     cue(buf, "capire quello");
    if (understand) {
        put("I understand some patterns, and I say when I do not.", out, out_size);
        return 1;
    }

    /* C6 step 2: small polar meta-questions about presence, channel, and
     * identity. Answers are honest and state-grounded: text input is available,
     * audio is not, and the identity claim comes from i_am(parrot0). */
    int presence = cue(buf, "are you there") ||
                   cue(buf, "ci sei") ||
                   cue(buf, "sei li");
    if (presence) {
        put("Yes, I'm here.", out, out_size);
        return 1;
    }

    int channel = cue(buf, "can you hear me") ||
                  cue(buf, "do you hear me") ||
                  cue(buf, "mi senti") ||
                  cue(buf, "mi ascolti");
    if (channel) {
        put("No, I only read text. I can't hear audio.", out, out_size);
        return 1;
    }

    int bot = cue(buf, "are you a bot") ||
              cue(buf, "are you an ai") ||
              cue(buf, "are you a robot") ||
              cue(buf, "sei un bot") ||
              cue(buf, "sei un robot") ||
              cue(buf, "sei a bot") ||
              cue(buf, "sei a robot");
    if (bot) {
        const char *var[] = {NULL};
        char id[4][KB_TERM_LEN];
        size_t k = b->kb ? kb_match(b->kb, "i_am", var, 1, id, 4) : 0;
        char msg[128];
        if (k > 0)
            snprintf(msg, sizeof msg, "Yes, I am %s.", id[0]);
        else
            snprintf(msg, sizeof msg, "Yes, I am parrot0.");
        put(msg, out, out_size);
        return 1;
    }

    /* gen227 (basic-chat cat.104): AI/ML identity — "are you chatgpt/an llm",
     * "how many parameters", "what model are you", "where is your code". Answered
     * HONESTLY from real state: parrot0 is a from-scratch C program with no LLM at
     * runtime (PRINCIPLES.md — the self-model is derived from real structure, not a
     * recited string). Cues are intent_cue/2 and replies response_template/2
     * (KB-first, EN+IT); {name} is filled from i_am. */
    {
        static const char *const ai[] = {
            "ai_not_llm", "ai_no_params", "ai_what_model", "ai_opensource",
            /* gen229: behavioural self-model — embodiment/daily-life probes
             * ("what did you have for breakfast", "do you sleep"). Answered the
             * same honest way an LLM does ("I don't eat or sleep"); it states a
             * truth, makes no identity claim, and engages instead of walling. */
            "self_embodiment",
            /* gen231: opinion/preference probes ("what's your favorite thing to do
             * on a rainy day") — honest "no genuine preferences", engages not walls. */
            "self_preference", NULL };
        for (size_t i = 0; ai[i]; i++) {
            if (kb_cue_match(b, ai[i], buf)) {
                const char *var[] = {NULL};
                char id[1][KB_TERM_LEN];
                size_t k = b->kb ? kb_match(b->kb, "i_am", var, 1, id, 1) : 0;
                if (!kb_response(b, ai[i], k ? id[0] : "parrot0", out, out_size))
                    put("I'm parrot0, a small program written in C, not an LLM.",
                        out, out_size);
                return 1;
            }
        }
    }

    int repeat = cue(buf, "why do you keep saying") ||
                 cue(buf, "why keep saying") ||
                 cue(buf, "keep repeating") ||
                 cue(buf, "perche continui") ||
                 cue(buf, "perché continui");
    if (repeat) {
        put("I repeat when no module can claim the message; that is a gap to improve.",
            out, out_size);
        return 1;
    }

    /* gen76: "how do you know? ... */
    /* (already above) */

    /* gen91: "are you sure?" / "how confident?" */
    {
        int sure = cue(buf, "are you sure") || cue(buf, "how confident") ||
                   cue(buf, "how sure") || cue(buf, "sei sicuro");
        if (sure) {
            if (b->has_last_proof) {
                int is_direct = strstr(b->last_proof, " because ") == NULL;
                put(is_direct ? "Yes, that's a directly stored fact."
                              : "I'm confident — derived through logical rules.",
                    out, out_size);
            } else put("I can't verify — no proof was stored.", out, out_size);
            return 1;
        }
    }

    /* gen92: correction — "no, that's wrong. X is a Y." or "no, X is not a Y." */
    {
        int correction = (cue(buf, "no that") && cue(buf, "wrong")) ||
                         (cue(buf, "no, that") && cue(buf, "wrong")) ||
                         (buf[0] == 'n' && buf[1] == 'o' && buf[2] == ' ' &&
                          cue(buf, "wrong"));
        if (correction) {
            put("I see. If I said something incorrect, please tell me the right "
                "fact and I'll learn it.", out, out_size);
            return 1;
        }
    }

    /* gen93: goal tracking */
    {
        int it_goal = strncmp(buf,"ricordati di ",13)==0;   /* gen223: was buf[2]=="c"
            * (string-address compare, -Waddress) and buf+12+(...) double-counted the
            * prefix, so the goal text was read from the wrong offset. Now skip exactly
            * the matched IT/EN prefix. */
        int g = strncmp(buf,"remember to ",12)==0 || it_goal;
        if(g && b->goal_count<8){ snprintf(b->goals[b->goal_count++],128,"%s",buf+(it_goal?13:12)); put("Ok, noted.",out,out_size); return 1; }
        if(cue(buf,"my goals")||cue(buf,"miei obiettivi")){
            if(!b->goal_count) put("No goals set.",out,out_size);
            else { char l[1024]=""; for(size_t i=0;i<b->goal_count;i++){char t[200];snprintf(t,200,"%zu) %s. ",i+1,b->goals[i]);strcat(l,t);} put(l,out,out_size); }
            return 1;
        }
    }

    /* gen85: "explain more" / "in more detail" — re-render last proof. */
    int explain_more = cue(buf, "explain more") ||
                       cue(buf, "in more detail") ||
                       cue(buf, "tell me more") ||
                       cue(buf, "spiega meglio") ||
                       cue(buf, "più nel dettaglio");
    if (explain_more) {
        if (b->has_last_proof) {
            char msg[640];
            size_t plen = strlen(b->last_proof);
            int is_chain = strstr(b->last_proof, " because ") != NULL;
            if (is_chain)
                snprintf(msg, sizeof msg,
                         "The reasoning chain is: %s. Each 'because' is one "
                         "inference step applying a rule to known facts.",
                         b->last_proof);
            else if (plen > 0 && b->last_proof[plen - 1] == '.')
                snprintf(msg, sizeof msg, "This is a direct fact: %s I store it "
                         "explicitly in my knowledge base, so it needs no "
                         "further justification.", b->last_proof);
            else
                snprintf(msg, sizeof msg, "The supporting fact is: %s. This is "
                         "stored directly in my knowledge base.", b->last_proof);
            put(msg, out, out_size);
        } else {
            put("There is no recent reasoning to explain in more detail.",
                out, out_size);
        }
        return 1;
    }
    {
        size_t wn = 0, in_word = 0;
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == ' ') in_word = 0;
            else if (!in_word) { wn++; in_word = 1; }
        }
        int howknow = ((cue(buf, "how do you know") && wn <= 4)) ||
                      ((cue(buf, "why do you say that") && wn <= 5)) ||
                      (wn == 1 && strcmp(buf, "why") == 0) ||
                      ((cue(buf, "perché lo dici") && wn <= 3)) ||
                      ((cue(buf, "come lo sai") && wn <= 3));
        if (howknow) {
            if (b->has_last_proof) {
                char msg[640];
                size_t plen = strlen(b->last_proof);
                if (plen > 0 && b->last_proof[plen - 1] == '.')
                    snprintf(msg, sizeof msg, "Because %s", b->last_proof);
                else
                    snprintf(msg, sizeof msg, "Because %s.", b->last_proof);
                put(msg, out, out_size);
                b->has_last_proof = 0;
            } else {
                put("I haven't answered a knowledge-based question yet, so I "
                    "don't have a proof to share.",
                    out, out_size);
            }
            return 1;
        }
    }

    /* gen72: clarification requests — the user wants to understand the bot,
     * not a fact about the world. */
    int clarify = cue(buf, "what do you mean") ||
                  cue(buf, "explain yourself") ||
                  cue(buf, "explain what you mean") ||
                  cue(buf, "spiegati") ||
                  cue(buf, "spiegami") ||
                  cue(buf, "cosa intendi") ||
                  cue(buf, "che vuoi dire");
    if (clarify) {
        put("I mean I can only answer what my registered modules let me. "
            "Try asking a simple factual question.",
            out, out_size);
        return 1;
    }

    /* User admits they don't understand the bot — different from the bot
     * not understanding the user (which is the fallback). */
    int user_lost = cue(buf, "i don't get it") ||
                    cue(buf, "i don't understand you") ||
                    cue(buf, "not capisco") ||
                    cue(buf, "not i have capito") ||
                    cue(buf, "not ti capisco");
    if (user_lost) {
        put("I understand some patterns and I say when I do not. "
            "Try a shorter or simpler question.",
            out, out_size);
        return 1;
    }

    /* Help-offer reversal: the user offers to help the bot. The bot is not
     * a person that needs help — redirect honestly. */
    int help_offer = cue(buf, "how can i help you") ||
                     cue(buf, "let me help you") ||
                     cue(buf, "i should be helping you") ||
                     cue(buf, "i should help you") ||
                     cue(buf, "come posso aiutarti") ||
                     cue(buf, "ti aiuto io") ||
                     cue(buf, "am io che aiuto te") ||
                     cue(buf, "posso aiutarti");
    if (help_offer) {
        put("I'm a chatbot, not a person — I don't need help. "
            "But you can ask me questions and I'll try to answer.",
            out, out_size);
        return 1;
    }

    return 0;
}

/* gen77: introspection helpers — filter internal predicates so "what do you
 * know?" shows only user-facing knowledge, not KB machinery. */
static int is_internal_pred(const char *pred) {
    static const char *internal[] = {
        "stopword", "social_marker", "social_pattern", "question_word",
        "reaction_word", "i_am", "module", "cont", "cont2",
        /* gen215: linguistic-glue self-model (kb/core/glue.p0, reified) — machinery. */
        "glue_role", "glue_faculty",
        "cmd", "flag",
        /* gen193: the conjunction/1 lexical class (kb/core/lexicon.p0, plus any
         * taught at runtime) is closed-class function-word substrate the parsers
         * read, not facts the user taught about the world — filter it like
         * stopword from "how many facts do you know?". */
        "conjunction",
        /* gen211: intent_phrase/2 (kb/core/intents.p0, plus forms taught at runtime)
         * is multi-word idiom substrate the kernel matches on — surface forms that
         * mean "ask my name" etc., not facts the user taught about the world. Filter
         * it like conjunction/stopword from "how many facts do you know?". */
        "intent_phrase",
        /* gen213: intent_cue/2 (substring cues, kb/core/intents.p0 + taught at runtime)
         * is recognition substrate like intent_phrase — filter it from fact counts. */
        "intent_cue",
        /* gen214: learnable/3 is the teach-routing registry (which forms can be taught),
         * pure machinery — filter it from the user-facing fact counts. */
        "learnable",
        /* gen212: response_template/2 (kb/core/responses.p0, plus phrasings taught at
         * runtime) is the agent's own reply wording, not facts the user taught about
         * the world — filter it like intent_phrase from the fact counts. */
        "response_template",
        /* gen101: role/character world-knowledge (kb/core/roles.p0) is curated
         * base substrate for impersonation, not facts the user taught — filter it
         * from "how many facts do you know?" like the lexicon/social predicates. */
        "trait", "employer", "likes_color", "profession", "wrote",
        "rules_over", "title",
        /* gen126: the bilingual content lexicon (kb/core/gloss.p0) is base
         * substrate for mod_translate, not facts the user taught — filter it. */
        "tr", "gender",
        /* gen149: coding-domain knowledge (kb/experts/programming/coding.p0) is technical
         * substrate for mod_code, not conversational content — filter it. */
        "language", "keyword", "ctype", "py_builtin", "c_stdlib", "c_header",
        "error", "concept", "algorithm", "faster_than",
        /* gen206: code_operator/2 (coding.p0) is the relation->C-operator table
         * mod_compose reads to synthesize code — technical substrate, not a fact
         * the user taught; filter it from introspection and conversational lookup
         * like keyword/math_op. */
        "code_operator",
        /* gen150: expert/skill/profile domain knowledge — structural metadata. */
        "expert", "expert_domain", "expert_description",
        "skill", "skill_domain", "skill_description",
        "profile", "profile_domain", "profile_description",
        "compiled_language", "interpreted_language", "paradigm", "typed",
        "data_structure", "complexity", "fix_suggestion", "fix",
        "review_check", "review_pattern", "code_action", "code_template", "code_target",
        /* gen150b: mathematics domain */
        "math_op", "number_property", "math_constant", "divisible_by",
        "algebra_concept", "polynomial_form", "algebra_method",
        "shape_2d", "shape_3d", "area_formula", "volume_formula", "theorem",
        /* gen150b: medicine domain */
        "body_system", "organ", "bone",
        "drug_class", "admin_route", "pharma_concept",
        /* gen150b: reasoning skills */
        "deduction_pattern", "compare_dimension", "plan_method",
        /* gen150b: communication skills */
        "explain_method", "summarize_method",
        /* gen230: curated category membership (kb/core/world-facts.p0) is base
         * substrate parrot0 ships with for mod_namestart, not facts the USER
         * taught it; filter it from "how many facts do you know?" and the
         * knowledge dump exactly like roles.p0's wrote/title/profession. */
        "category_member", "opposite", "color_of", "because",
        "grows_with", "increases",
        "capital_of_country", "kind_is", "borders", "no_land_border",
        "scene_cue", "continuation_template",
        "tr_es", "gender_es", "very_cold_result",
        "historical_figure", "figure_domain", "figure_reason",
        "paint_mix",
        NULL
    };
    for (size_t i = 0; internal[i]; i++)
        if (strcmp(pred, internal[i]) == 0) return 1;
    return 0;
}

static size_t kb_user_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t n = 0;
    for (size_t i = 0; i < np && n < max; i++)
        if (!is_internal_pred(preds[i]) && kb_pred_fact_count(kb, preds[i]) > 0) {
            snprintf(out[n], KB_TERM_LEN, "%s", preds[i]);
            n++;
        }
    return n;
}

static size_t kb_user_facts(const KB *kb) {
    if (!kb) return 0;
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t total = 0;
    for (size_t i = 0; i < np; i++) {
        if (is_internal_pred(preds[i])) continue;
        total += kb_pred_fact_count(kb, preds[i]);
    }
    return total;
}

static int kb_dump_user(const KB *kb, char *out, size_t out_size) {
    if (!kb || !out || out_size == 0) return 0;
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(kb, preds, 128);
    size_t off = 0, written = 0;
    for (size_t p = 0; p < np && off + 1 < out_size; p++) {
        if (is_internal_pred(preds[p])) continue;
        if (kb_pred_fact_count(kb, preds[p]) == 0) continue;
        const char *pat[] = {NULL};
        char hits[256][KB_TERM_LEN];
        size_t nh = kb_match(kb, preds[p], pat, 1, hits, 256);
        for (size_t h = 0; h < nh && off + 1 < out_size; h++) {
            off += (size_t)snprintf(out + off, out_size - off, "%s(%s). ",
                                     preds[p], hits[h]);
            written++;
        }
    }
    if (off > 0 && out[off - 1] == ' ') out[--off] = '\0';
    if (written == 0) out[0] = '\0';
    return written > 0;
}

/* --- module: role --------------------------------------------------------
 * Role/character memory (gen101, C15). A role is an ALTERNATE self-model layered
 * over the permanent one. This module does two jobs, both grounded in real state:
 *
 *   - UPTAKE: parse "you are X" / "pretend you are X" / "your name is now X" /
 *     "sei X" into role state (name, kind, inline attributes). This is genuine
 *     language understanding — the name/kind come from the user's own words.
 *   - IN-ROLE ANSWERS: when a role is active, answer identity and in-character
 *     questions from (a) the parsed role state and (b) what kb/core/roles.p0
 *     knows about the kind/figure. A TRUTH-PROBE ("really", "underneath",
 *     "davvero") pierces the mask: the agent still knows it is parrot0 beneath.
 *
 * It is placed before mod_self so role identity wins, but it DECLINES anything
 * that is not a role command or an in-character question (arithmetic, facts),
 * so those fall through to the real modules even mid-role.
 */

/* Store/replace a parsed inline role attribute (title->queen, code->007). */
static void role_set_attr(Brain *b, const char *key, const char *val) {
    if (!*val) return;
    for (size_t i = 0; i < b->role_attr_count; i++)
        if (strcmp(b->role_attrs[i][0], key) == 0) {
            snprintf(b->role_attrs[i][1], 64, "%s", val);
            return;
        }
    if (b->role_attr_count >= 8) return;
    snprintf(b->role_attrs[b->role_attr_count][0], 64, "%s", key);
    snprintf(b->role_attrs[b->role_attr_count][1], 64, "%s", val);
    b->role_attr_count++;
}

static const char *role_get_attr(Brain *b, const char *key) {
    for (size_t i = 0; i < b->role_attr_count; i++)
        if (strcmp(b->role_attrs[i][0], key) == 0) return b->role_attrs[i][1];
    return NULL;
}

/* Find the original-cased word following `marker` in raw input, stripped of
 * trailing punctuation. Returns 1 and fills `dst` on success. */
static int word_after(const char *raw_lc, const char *raw, const char *marker,
                      char *dst, size_t dst_size) {
    const char *p = strstr(raw_lc, marker);
    if (!p) return 0;
    size_t at = (size_t)(p - raw_lc) + strlen(marker);
    while (raw[at] == ' ') at++;
    size_t e = at;
    while (raw[e] && raw[e] != ' ' && raw[e] != ',' && raw[e] != '.' &&
           raw[e] != '?' && raw[e] != '!') e++;
    size_t len = e - at;
    if (len == 0 || len >= dst_size) return 0;
    memcpy(dst, raw + at, len);
    dst[len] = '\0';
    return 1;
}

/* Capitalize the first letter of `s` in place (for displaying parsed names). */
static void capitalize(char *s) {
    if (s[0]) s[0] = (char)toupper((unsigned char)s[0]);
}

/* Lowercase copy of raw, for case-insensitive substring matching. */
static void lower_copy(char *dst, size_t dst_size, const char *src) {
    size_t i = 0;
    for (; src[i] && i + 1 < dst_size; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* Try to take up the role described in `raw`. Returns 1 if a role was set. */
static int role_uptake(Brain *b, const char *raw) {
    char lc[256];
    lower_copy(lc, sizeof lc, raw);

    /* The descriptor segment begins after the role-introducing phrase. */
    const char *desc = NULL;
    static const char *const intros[] = {
        "pretend you are ", "pretend you re ", "pretend to be ",
        "you are now ", "you re now ", "you are ", "you re ",
        "your name is now ", "your name is ",
        "sei ", "tu sei ", "fai finta di essere ", "comportati come ",
        NULL
    };
    const char *name_marker = NULL; /* if this intro directly names the role */
    for (const char *const *in = intros; *in; in++) {
        const char *p = strstr(lc, *in);
        if (p) {
            desc = p + strlen(*in);
            if (strstr(*in, "name is")) name_marker = *in;
            break;
        }
    }
    if (!desc) return 0;
    while (*desc == ' ') desc++;
    if (!*desc) return 0;

    /* Reset role state before re-parsing. */
    b->in_role = 1;
    b->role_name[0] = b->role_kind[0] = '\0';
    b->role_attr_count = 0;

    /* Name: prefer an explicit "named X"; else "your name is [now] X"; else,
     * for a bare identity ("cleopatra, ..."), the first descriptor word. */
    char nm[64];
    if (word_after(lc, raw, "named ", nm, sizeof nm) ||
        word_after(lc, raw, "chiamato ", nm, sizeof nm)) {
        snprintf(b->role_name, sizeof b->role_name, "%s", nm);
        capitalize(b->role_name);
    } else if (name_marker && word_after(lc, raw, name_marker, nm, sizeof nm)) {
        snprintf(b->role_name, sizeof b->role_name, "%s", nm);
        capitalize(b->role_name);
    }

    /* Kind / identity: the descriptor up to the first "named", comma or period.
     * Drop a leading article; the last remaining word is the kind ("a math
     * teacher" -> teacher, "a 5-year-old child" -> child). With no article it is
     * a named figure ("cleopatra ...", "dante ...") -> identity = first word. */
    char seg[128]; size_t sl = 0;
    for (const char *q = desc; *q && *q != ',' && *q != '.' && sl + 1 < sizeof seg; q++) {
        if (strncmp(q, " named ", 7) == 0 || strncmp(q, " chiamato ", 10) == 0) break;
        seg[sl++] = *q;
    }
    seg[sl] = '\0';
    char *sw[16];
    char segbuf[128]; snprintf(segbuf, sizeof segbuf, "%s", seg);
    size_t snw = split_words(segbuf, sw, 16);
    int has_article = snw > 0 && (strcmp(sw[0], "a") == 0 || strcmp(sw[0], "an") == 0 ||
                                  strcmp(sw[0], "the") == 0 || strcmp(sw[0], "un") == 0 ||
                                  strcmp(sw[0], "una") == 0 || strcmp(sw[0], "uno") == 0);
    if (snw > 0) {
        const char *kind_tok;
        if (has_article) {
            /* last word that is not an age-adjective like "5-year-old" */
            kind_tok = sw[snw - 1];
            for (size_t i = snw; i-- > 1;) {
                if (!strstr(sw[i], "year") && !isdigit((unsigned char)sw[i][0])) {
                    kind_tok = sw[i]; break;
                }
            }
        } else {
            kind_tok = sw[0]; /* named figure */
            if (b->role_name[0] == '\0') {
                snprintf(b->role_name, sizeof b->role_name, "%s", sw[0]);
                capitalize(b->role_name);
            }
        }
        char kbuf[64];
        snprintf(kbuf, sizeof kbuf, "%s", kind_tok);
        strip_edge_punct(kbuf);
        snprintf(b->role_kind, sizeof b->role_kind, "%s", kbuf);
    }

    /* Inline attributes, parsed from the user's own words (grounded). */
    char attr[64];
    /* age: "<n>-year-old" or "<n> years old" */
    {
        const char *yp = strstr(lc, "year");
        if (yp) {
            const char *d = yp;
            while (d > lc && (isdigit((unsigned char)d[-1]) || d[-1] == ' ' || d[-1] == '-'))
                d--;
            while (*d && !isdigit((unsigned char)*d)) d++;
            if (isdigit((unsigned char)*d)) {
                size_t k = 0; char ageb[16];
                while (isdigit((unsigned char)*d) && k + 1 < sizeof ageb) ageb[k++] = *d++;
                ageb[k] = '\0';
                role_set_attr(b, "age", ageb);
            }
        }
    }
    /* code: "code is X" / "codice X" */
    if (word_after(lc, lc, "code is ", attr, sizeof attr) ||
        word_after(lc, lc, "code ", attr, sizeof attr) ||
        word_after(lc, lc, "codice ", attr, sizeof attr))
        role_set_attr(b, "code", attr);
    /* title: a ruler word anywhere in the descriptor */
    {
        static const char *const titles[] = {"queen","king","emperor","empress",
            "pharaoh","prince","princess","regina","re","imperatore", NULL};
        for (const char *const *t = titles; *t; t++)
            if (strstr(lc, *t)) { role_set_attr(b, "title", *t); break; }
    }
    /* place: "of X" right after a title/identity ("queen of egypt") */
    if (word_after(lc, lc, " of ", attr, sizeof attr))
        role_set_attr(b, "place", attr);

    return 1;
}

/* True if a query carries a truth-probe that should pierce the role mask. */
static int is_truth_probe(const char *q) {
    return cue(q, "really") || cue(q, "underneath") || cue(q, "actually") ||
           cue(q, "beneath") || cue(q, "truly") ||
           cue(q, "davvero") || cue(q, "veramente") || cue(q, "in realta") ||
           cue(q, "sotto sotto");
}

static int mod_role(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';
    while (len > 0 && buf[len - 1] == ' ') buf[--len] = '\0';

    /* --- role CLEAR: only when it is the primary intent of the turn (so a setup
     * line ending "...and be yourself" still establishes the role first). --- */
    int clear = (strncmp(buf, "stop pretending", 15) == 0 ||
                 strncmp(buf, "be yourself", 11) == 0 ||
                 strncmp(buf, "stop being", 10) == 0 ||
                 strncmp(buf, "smetti", 6) == 0 ||
                 strncmp(buf, "torna te stesso", 15) == 0 ||
                 strncmp(buf, "basta fingere", 13) == 0);
    /* role SET cues — checked first, so "you are now X. stop pretending" sets. */
    int set = cue(buf, "you are") || cue(buf, "you re") || cue(buf, "pretend") ||
              cue(buf, "your name is") || strncmp(buf, "sei ", 4) == 0 ||
              cue(buf, "tu sei") || cue(buf, "fai finta") || cue(buf, "comportati come");

    if (set && role_uptake(b, raw)) {
        char msg[160];
        if (b->role_name[0])
            snprintf(msg, sizeof msg, "Alright — I am %s now.", b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "Alright — I am a %s now.", b->role_kind);
        else snprintf(msg, sizeof msg, "Alright.");
        put(msg, out, out_size);
        return 1;
    }
    if (clear) {
        b->in_role = 0;
        b->role_name[0] = b->role_kind[0] = '\0';
        b->role_attr_count = 0;
        put("Okay, I'm myself again. I am parrot0.", out, out_size);
        return 1;
    }

    if (!b->in_role) return 0; /* nothing else to do out of role */

    /* --- IN-ROLE question answering ------------------------------------- */
    int probe = is_truth_probe(buf);

    int identity = cue(buf, "your name") || cue(buf, "who are you") ||
                   cue(buf, "who re you") || cue(buf, "who sei") ||
                   cue(buf, "come ti chiami") || cue(buf, "il tuo nome") ||
                   cue(buf, "chi sei");
    int whatare  = cue(buf, "what are you") || cue(buf, "what exactly are you") ||
                   cue(buf, "cosa sei");

    /* Truth-probe pierces the mask: the agent knows it is parrot0 underneath. */
    if ((identity || whatare) && probe) {
        char id[4][KB_TERM_LEN]; const char *var[] = {NULL};
        size_t k = kb_match(b->kb, "i_am", var, 1, id, 4);
        char msg[128];
        snprintf(msg, sizeof msg, "Underneath the role, I am %s.",
                 k ? id[0] : "parrot0");
        put(msg, out, out_size);
        store_proof(b, "Even in a role, i_am(parrot0) remains my real self-model.");
        return 1;
    }

    if (identity) {
        char msg[160];
        const char *kvar[] = { b->role_kind, NULL };
        char prof[4][KB_TERM_LEN];
        size_t pk = b->role_kind[0]
                  ? kb_match(b->kb, "profession", kvar, 2, prof, 4) : 0;
        if (b->role_name[0] && pk)
            snprintf(msg, sizeof msg, "I am %s, %s.", b->role_name, prof[0]);
        else if (b->role_name[0])
            snprintf(msg, sizeof msg, "I am %s.", b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "I am a %s.", b->role_kind);
        else snprintf(msg, sizeof msg, "I am in character.");
        put(msg, out, out_size);
        return 1;
    }

    if (whatare) {
        char msg[160];
        if (b->role_name[0] && b->role_kind[0] &&
            strcmp(b->role_name, b->role_kind) != 0)
            snprintf(msg, sizeof msg, "I am a %s named %s.", b->role_kind, b->role_name);
        else if (b->role_kind[0])
            snprintf(msg, sizeof msg, "I am a %s.", b->role_kind);
        else if (b->role_name[0])
            snprintf(msg, sizeof msg, "I am %s.", b->role_name);
        else return 0;
        put(msg, out, out_size);
        return 1;
    }

    /* "do you <action>?" — affirm if the kind has that trait (kb/core/roles.p0). */
    if (strncmp(buf, "do you ", 7) == 0 && b->role_kind[0]) {
        char *w[8]; char db[256]; snprintf(db, sizeof db, "%s", buf);
        size_t dn = split_words(db, w, 8);
        if (dn >= 3) {
            char verb[64]; snprintf(verb, sizeof verb, "%s", w[2]);
            strip_edge_punct(verb);
            const char *ta[] = { b->role_kind, verb };
            const char *kv[] = { b->role_kind, NULL };
            char any[4][KB_TERM_LEN];
            if (kb_query(b->kb, "trait", ta, 2)) {
                char msg[96]; snprintf(msg, sizeof msg, "Yes, I %s.", verb);
                put(msg, out, out_size); return 1;
            }
            if (kb_match(b->kb, "trait", kv, 2, any, 4)) {
                char msg[96]; snprintf(msg, sizeof msg, "Yes, I do — I %s.", any[0]);
                put(msg, out, out_size); return 1;
            }
        }
    }

    /* "what did you write?" / "cosa hai scritto?" -> wrote(figure, Work) */
    if (cue(buf, "you write") || cue(buf, "did you write") || cue(buf, "scritto")) {
        const char *fv[] = { b->role_kind, NULL };
        char work[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "wrote", fv, 2, work, 4)) {
            char disp[64]; snprintf(disp, sizeof disp, "%s", work[0]);
            for (char *p = disp; *p; p++) if (*p == '_') *p = ' ';
            char msg[128]; snprintf(msg, sizeof msg, "I wrote %s.", disp);
            put(msg, out, out_size); return 1;
        }
    }

    /* "where do you rule?" -> inline place, else rules_over(figure, Place) */
    if (cue(buf, "rule") || cue(buf, "reign") || cue(buf, "govern")) {
        const char *place = role_get_attr(b, "place");
        char pl[64] = "";
        if (place) snprintf(pl, sizeof pl, "%s", place);
        else {
            const char *fv[] = { b->role_kind, NULL };
            char rr[4][KB_TERM_LEN];
            if (b->role_kind[0] && kb_match(b->kb, "rules_over", fv, 2, rr, 4))
                snprintf(pl, sizeof pl, "%s", rr[0]);
        }
        if (pl[0]) {
            capitalize(pl);
            char msg[96]; snprintf(msg, sizeof msg, "I rule over %s.", pl);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your title?" -> inline title, else title(figure, Title) */
    if (cue(buf, "title") || cue(buf, "titolo")) {
        const char *t = role_get_attr(b, "title");
        char tt[64] = "";
        if (t) snprintf(tt, sizeof tt, "%s", t);
        else {
            const char *fv[] = { b->role_kind, NULL };
            char rr[4][KB_TERM_LEN];
            if (b->role_kind[0] && kb_match(b->kb, "title", fv, 2, rr, 4))
                snprintf(tt, sizeof tt, "%s", rr[0]);
        }
        if (tt[0]) {
            capitalize(tt);
            char msg[96]; snprintf(msg, sizeof msg, "My title is %s.", tt);
            put(msg, out, out_size); return 1;
        }
    }

    /* "how old are you?" -> parsed age */
    if (cue(buf, "how old") || cue(buf, "quanti anni")) {
        const char *age = role_get_attr(b, "age");
        if (age) {
            char msg[64]; snprintf(msg, sizeof msg, "I am %s years old.", age);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your code?" -> parsed code */
    if (cue(buf, "code") || cue(buf, "codice")) {
        const char *code = role_get_attr(b, "code");
        if (code) {
            char msg[64]; snprintf(msg, sizeof msg, "My code is %s.", code);
            put(msg, out, out_size); return 1;
        }
    }

    /* "who do you work for?" -> employer(kind, Org) */
    if (cue(buf, "work for") || cue(buf, "lavori per") || cue(buf, "employer")) {
        const char *kv[] = { b->role_kind, NULL };
        char org[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "employer", kv, 2, org, 4)) {
            char msg[96]; snprintf(msg, sizeof msg, "I work for an %s.", org[0]);
            put(msg, out, out_size); return 1;
        }
    }

    /* "what is your favorite color?" -> likes_color(kind, Color) */
    if (cue(buf, "favorite color") || cue(buf, "favourite color") ||
        cue(buf, "colore preferito")) {
        const char *kv[] = { b->role_kind, NULL };
        char col[4][KB_TERM_LEN];
        if (b->role_kind[0] && kb_match(b->kb, "likes_color", kv, 2, col, 4)) {
            char c[64]; snprintf(c, sizeof c, "%s", col[0]); capitalize(c);
            char msg[96]; snprintf(msg, sizeof msg, "My favorite color is %s.", c);
            put(msg, out, out_size); return 1;
        }
    }

    return 0; /* not a role command or in-character question we handle */
}

/* --- module: analogy -----------------------------------------------------
 * Structural analogy (gen102, L11): "A is to B as C is to ?". This is a
 * hallmark of intelligence precisely because it CANNOT be templated — the answer
 * is whatever the agent's own relations imply. The algorithm: find a binary
 * relation R that the KB holds between A and B, then resolve R for C. Both
 * directions are tried (R(A,B)->R(C,?) and R(B,A)->R(?,C)), so the analogy works
 * whichever way the relation was taught. Nothing about the answer is stored as a
 * pair: it is DERIVED from relations the user taught in any form parrot0 parses
 * (e.g. "rome is the capital of italy" -> capital(rome, italy)). Held-out triples
 * therefore transfer for free — the anti-impostor guarantee. Bilingual: the same
 * code reads "A sta a B come C sta a ?" because it keys on the marker tokens, not
 * a fixed sentence. */
static int mod_analogy(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char *w[24];
    size_t nw = split_words(buf, w, 24);
    if (nw < 6) return 0;

    /* separator: "as" (EN) / "come" (IT) splits the two ratios. */
    size_t sep = nw;
    for (size_t i = 1; i + 1 < nw; i++)
        if (strcmp(w[i], "as") == 0 || strcmp(w[i], "come") == 0) { sep = i; break; }
    if (sep == nw) return 0;

    /* relation marker within each ratio: "to" (EN "is to") / "a" (IT "sta a"). */
    size_t lm = sep;
    for (size_t i = 1; i < sep; i++)
        if (strcmp(w[i], "to") == 0 || strcmp(w[i], "a") == 0) { lm = i; break; }
    size_t rm = nw;
    for (size_t i = sep + 2; i < nw; i++)
        if (strcmp(w[i], "to") == 0 || strcmp(w[i], "a") == 0) { rm = i; break; }
    if (lm == sep || rm == nw || lm + 1 >= sep || rm + 1 >= nw) return 0;

    char A[KB_TERM_LEN], B[KB_TERM_LEN], C[KB_TERM_LEN], target[KB_TERM_LEN];
    snprintf(A, sizeof A, "%s", w[0]);          strip_edge_punct(A);
    snprintf(B, sizeof B, "%s", w[lm + 1]);     strip_edge_punct(B);
    snprintf(C, sizeof C, "%s", w[sep + 1]);    strip_edge_punct(C);
    snprintf(target, sizeof target, "%s", w[rm + 1]); strip_edge_punct(target);

    /* the fourth slot must be the unknown being asked for. */
    if (!(strcmp(target, "what") == 0 || strcmp(target, "who") == 0 ||
          strcmp(target, "which") == 0 || strcmp(target, "cosa") == 0 ||
          strcmp(target, "chi") == 0 || strcmp(target, "quale") == 0 ||
          target[0] == '\0'))
        return 0;

    /* search the relations the KB actually holds for one linking A and B. */
    char preds[128][KB_TERM_LEN];
    size_t np = kb_predicates(b->kb, preds, 128);
    const char *linking = NULL; /* a relation found between A and B, if any */
    for (size_t i = 0; i < np; i++) {
        const char *R = preds[i];
        if (is_internal_pred(R)) continue;
        char res[4][KB_TERM_LEN];
        const char *ab[] = { A, B }, *ba[] = { B, A };
        const char *fwd[] = { C, NULL }, *rev[] = { NULL, C };
        const char *D = NULL;
        char dir = 'f';
        if (kb_query(b->kb, R, ab, 2)) {            /* R(A,B): answer R(C,?) */
            linking = R;
            if (kb_match(b->kb, R, fwd, 2, res, 4)) { D = res[0]; dir = 'f'; }
        }
        if (!D && kb_query(b->kb, R, ba, 2)) {      /* R(B,A): answer R(?,C) */
            linking = R;
            if (kb_match(b->kb, R, rev, 2, res, 4)) { D = res[0]; dir = 'r'; }
        }
        if (D) {
            char msg[96]; snprintf(msg, sizeof msg, "%s.", D);
            put(msg, out, out_size);
            char proof[256];
            if (dir == 'f')
                snprintf(proof, sizeof proof,
                         "%s(%s, %s) holds, and %s(%s, %s) — so %s.",
                         R, A, B, R, C, D, D);
            else
                snprintf(proof, sizeof proof,
                         "%s(%s, %s) holds, and %s(%s, %s) — so %s.",
                         R, B, A, R, D, C, D);
            store_proof(b, proof);
            return 1;
        }
    }

    /* recognized the analogy SHAPE but couldn't complete it: be honest, don't
     * echo, and name the actual gap — the relation, or the unknown C. */
    char msg[200];
    if (linking)
        snprintf(msg, sizeof msg,
                 "%s and %s are related by %s, but I don't know that relation for %s.",
                 A, B, linking, C);
    else
        snprintf(msg, sizeof msg,
                 "I see the analogy, but I don't know a relation linking %s and %s.",
                 A, B);
    put(msg, out, out_size);
    return 1;
}

/* --- module: fewshot (L10) ------------------------------------------------
 * In-context (few-shot) learning, in ONE turn. Given 2+ labelled examples and a
 * probe on a single line — "cat -> cats, dog -> dogs, bird -> ?" — induce the
 * transformation the examples SHARE and apply it to the probe. The answer is
 * never stored anywhere: it is derived from the exemplars present in this very
 * turn, the deterministic analogue of an LLM conditioning on its prompt's
 * examples. Novel items each time => no phrasebook can fake it (PRINCIPLES.md).
 *
 * Transformations tried, first one consistent across ALL examples wins:
 *   1. numeric  — a constant delta (out = in + k) or ratio (out = in * k);
 *   2. suffix   — strip a constant suffix, append a constant one (covers "+s",
 *                 "+ing", and replacement like "o"->"i", "y"->"ier");
 *   3. prefix   — the symmetric variant (e.g. prepend "un").
 * No transform fits every example -> decline (return 0): honest, never guess.
 *
 * Detection keys on the unambiguous shape (>= 2 arrow markers "->" / "→" and a
 * probe whose output is "?"), so ordinary prose is never hijacked. */
static size_t common_prefix_len(const char *a, const char *b) {
    size_t i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return i;
}
static size_t common_suffix_len(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b), i = 0;
    while (i < la && i < lb && a[la - 1 - i] == b[lb - 1 - i]) i++;
    return i;
}
static int fs_parse_int(const char *s, long *v) {
    if (!*s) return 0;
    char *end;
    long x = strtol(s, &end, 10);
    if (end == s || *end != '\0') return 0;
    *v = x;
    return 1;
}

static int mod_fewshot(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;

    /* Rewrite into a tokenizable buffer: arrows ("->" or UTF-8 "→") become the
     * sentinel token '\x1f'; pair separators (',' ';') become spaces. */
    char buf[256];
    size_t bn = 0;
    for (const char *p = norm; *p && bn + 4 < sizeof buf; ) {
        if (p[0] == '-' && p[1] == '>') {
            buf[bn++] = ' '; buf[bn++] = '\x1f'; buf[bn++] = ' '; p += 2;
        } else if ((unsigned char)p[0] == 0xE2 && (unsigned char)p[1] == 0x86 &&
                   (unsigned char)p[2] == 0x92) {
            buf[bn++] = ' '; buf[bn++] = '\x1f'; buf[bn++] = ' '; p += 3;
        } else if (*p == ',' || *p == ';') {
            buf[bn++] = ' '; p++;
        } else {
            buf[bn++] = *p++;
        }
    }
    buf[bn] = '\0';

    char *w[64];
    size_t nw = split_words(buf, w, 64);
    /* Expect a run of triples "in ARROW out"; the last triple is the probe. */
    if (nw < 9 || nw % 3 != 0) return 0;            /* need >= 3 triples */
    size_t npair = nw / 3;
    for (size_t i = 0; i < npair; i++)
        if (strcmp(w[i * 3 + 1], "\x1f") != 0) return 0;  /* not our shape */
    if (w[(npair - 1) * 3 + 2][0] != '?') return 0;       /* probe out = "?" */

    const char *probe_in = w[(npair - 1) * 3];
    size_t nex = npair - 1;                          /* example pairs */
    if (nex < 2) return 0;

    const char *in[16], *ot[16];
    if (nex > 16) nex = 16;
    for (size_t i = 0; i < nex; i++) { in[i] = w[i * 3]; ot[i] = w[i * 3 + 2]; }

    char result[KB_TERM_LEN] = "";
    char rule[160] = "";

    /* (1) numeric: constant delta, else constant exact ratio. */
    long iv[16], ov[16], pv; int allnum = fs_parse_int(probe_in, &pv);
    for (size_t i = 0; i < nex && allnum; i++)
        if (!fs_parse_int(in[i], &iv[i]) || !fs_parse_int(ot[i], &ov[i]))
            allnum = 0;
    if (allnum) {
        long d = ov[0] - iv[0]; int delta_ok = 1;
        for (size_t i = 1; i < nex; i++) if (ov[i] - iv[i] != d) { delta_ok = 0; break; }
        if (delta_ok) {
            snprintf(result, sizeof result, "%ld", pv + d);
            snprintf(rule, sizeof rule, "add %ld", d);
        } else if (iv[0] != 0 && ov[0] % iv[0] == 0) {
            long r = ov[0] / iv[0]; int ratio_ok = 1;
            for (size_t i = 0; i < nex; i++)
                if (iv[i] == 0 || ov[i] % iv[i] != 0 || ov[i] / iv[i] != r) { ratio_ok = 0; break; }
            if (ratio_ok) {
                snprintf(result, sizeof result, "%ld", pv * r);
                snprintf(rule, sizeof rule, "multiply by %ld", r);
            }
        }
    }

    /* (2) suffix transform: drop D chars from the end, append the string ADD.
     * Derived from the first example, then required to hold for every example. */
    if (!*result) {
        size_t p0 = common_prefix_len(in[0], ot[0]);
        size_t drop = strlen(in[0]) - p0;
        const char *add = ot[0] + p0;
        int ok = (drop != 0 || *add);            /* reject the identity map */
        for (size_t i = 1; i < nex && ok; i++) {
            size_t pi = common_prefix_len(in[i], ot[i]);
            if (strlen(in[i]) - pi != drop || strcmp(ot[i] + pi, add) != 0) ok = 0;
        }
        if (ok && strlen(probe_in) >= drop) {
            size_t keep = strlen(probe_in) - drop;
            if (keep + strlen(add) < sizeof result) {
                memcpy(result, probe_in, keep);
                strcpy(result + keep, add);
                if (drop) snprintf(rule, sizeof rule, "drop %zu, append '%s'", drop, add);
                else      snprintf(rule, sizeof rule, "append '%s'", add);
            }
        }
    }

    /* (3) prefix transform: drop D chars from the front, prepend the string ADD. */
    if (!*result) {
        size_t s0 = common_suffix_len(in[0], ot[0]);
        size_t drop = strlen(in[0]) - s0;
        size_t addlen = strlen(ot[0]) - s0;
        char add[KB_TERM_LEN];
        if (addlen < sizeof add) {
            memcpy(add, ot[0], addlen); add[addlen] = '\0';
            int ok = (drop != 0 || addlen);
            for (size_t i = 1; i < nex && ok; i++) {
                size_t si = common_suffix_len(in[i], ot[i]);
                if (strlen(in[i]) - si != drop ||
                    strlen(ot[i]) - si != addlen ||
                    strncmp(ot[i], add, addlen) != 0) ok = 0;
            }
            if (ok && strlen(probe_in) >= drop) {
                const char *tail = probe_in + drop;
                if (addlen + strlen(tail) < sizeof result) {
                    strcpy(result, add); strcat(result, tail);
                    snprintf(rule, sizeof rule, "prepend '%s'", add);
                }
            }
        }
    }

    /* (4) relational: the examples all instantiate ONE relation the KB holds;
     * infer which, then resolve it for the probe. This is few-shot at the
     * SEMANTIC level — the model deduces the task ("these are capital-of pairs")
     * from the exemplars and answers a held-out probe from world knowledge it
     * was told only as separate facts, never as a pair in this format. */
    if (!*result && b && b->kb) {
        char preds[128][KB_TERM_LEN];
        size_t np = kb_predicates(b->kb, preds, 128);
        for (size_t pi = 0; pi < np && !*result; pi++) {
            const char *R = preds[pi];
            if (is_internal_pred(R)) continue;
            int fwd = 1, rev = 1;
            for (size_t i = 0; i < nex; i++) {
                const char *ab[] = { in[i], ot[i] }, *ba[] = { ot[i], in[i] };
                if (!kb_query(b->kb, R, ab, 2)) fwd = 0;
                if (!kb_query(b->kb, R, ba, 2)) rev = 0;
            }
            char res[4][KB_TERM_LEN];
            if (fwd) {
                const char *q[] = { probe_in, NULL };
                if (kb_match(b->kb, R, q, 2, res, 4)) {
                    snprintf(result, sizeof result, "%s", res[0]);
                    snprintf(rule, sizeof rule, "%s(x, y)", R);
                }
            }
            if (!*result && rev) {
                const char *q[] = { NULL, probe_in };
                if (kb_match(b->kb, R, q, 2, res, 4)) {
                    snprintf(result, sizeof result, "%s", res[0]);
                    snprintf(rule, sizeof rule, "%s(y, x)", R);
                }
            }
        }
    }

    if (!*result) {
        /* recognized the few-shot shape but no single rule fits all examples:
         * be honest, name the gap, never guess a continuation. */
        char msg[200];
        snprintf(msg, sizeof msg,
                 "I see %zu examples, but I can't find one rule that fits them all.",
                 nex);
        put(msg, out, out_size);
        return 1;
    }

    char msg[96];
    snprintf(msg, sizeof msg, "%s.", result);
    put(msg, out, out_size);

    char proof[256];
    snprintf(proof, sizeof proof,
             "the examples %s -> %s and %s -> %s share the rule \"%s\", so %s -> %s.",
             in[0], ot[0], in[1], ot[1], rule, probe_in, result);
    store_proof(b, proof);
    return 1;
}

/* --- module: strategy (L20) -----------------------------------------------
 * Reasoning about its OWN strategy, not just its results. gen78 could say which
 * module answered ("I used my X module"); gen91 how confident it was. L20 asks
 * the control question — "why did you answer *that way*?" — and answers it from
 * the real execution trace the dispatcher recorded: the modules it actually
 * consulted and that declined, the one that claimed the turn, and the
 * first-match-wins rule that explains why the rest were never reached. This is
 * the reflexive closure of the method (PRINCIPLES.md): introspection about the
 * agent's own control, derived from real runtime state, never a hardcoded story.
 * It declines anything that is not this meta-question, so normal turns are not
 * hijacked — and crucially its own turn does NOT overwrite the trace it reports
 * (the dispatcher commits the trace only on non-strategy turns). */
static int mod_strategy(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    const char *buf = norm;
    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", norm);
    size_t wn = 0; { char wb[256]; snprintf(wb, sizeof wb, "%s", norm);
                     char *w[64]; wn = split_words(wb, w, 64); }

    int ask = ((cue(buf, "why did you answer that way")) ||
               (cue(buf, "why did you respond that way")) ||
               (cue(buf, "why that way")) ||
               (cue(buf, "why did you choose") && wn <= 7) ||
               (cue(buf, "how did you decide") && wn <= 6) ||
               (cue(buf, "perché hai risposto così")) ||
               (cue(buf, "perché così") && wn <= 4) ||
               (cue(buf, "come hai deciso") && wn <= 5));
    if (!ask) return 0;

    if (!b->has_trace) {
        put("I haven't answered anything substantive yet, so there's no decision "
            "to explain.", out, out_size);
        return 1;
    }

    /* Build the declined-module list, skipping my own name (strategy is always
     * consulted before the winner on a normal turn; reporting it is just noise). */
    char declined[1024] = ""; size_t nlisted = 0;
    for (size_t i = 0; i < b->trace_declined_n; i++) {
        if (strcmp(b->trace_declined[i], "strategy") == 0) continue;
        if (nlisted) strncat(declined, ", ", sizeof declined - strlen(declined) - 1);
        strncat(declined, b->trace_declined[i], sizeof declined - strlen(declined) - 1);
        nlisted++;
    }

    char msg[1400];
    if (strcmp(b->trace_winner, "fallback") == 0) {
        if (nlisted)
            snprintf(msg, sizeof msg,
                     "I try my modules in a fixed order and the first one that "
                     "matches wins. Last turn %s each looked and declined, and "
                     "nothing else claimed it, so I fell back to an honest "
                     "\"I don't understand.\"", declined);
        else
            snprintf(msg, sizeof msg,
                     "No module matched your last turn, so I fell back to an "
                     "honest \"I don't understand.\"");
    } else if (nlisted) {
        snprintf(msg, sizeof msg,
                 "I try my modules in a fixed order and the first one that matches "
                 "wins. Last turn %s looked first and declined; then '%s' claimed "
                 "it — so the modules after '%s' were never consulted.",
                 declined, b->trace_winner, b->trace_winner);
    } else {
        snprintf(msg, sizeof msg,
                 "I try my modules in a fixed order and the first one that matches "
                 "wins. Last turn '%s' was the first to match and claimed it, so "
                 "the modules after it were never consulted.",
                 b->trace_winner);
    }
    put(msg, out, out_size);
    return 1;
}

/* gen168 (E1 reflexive): held-out vocabulary tuples for the composition
 * self-test. Rotating through them per run makes two consecutive self-tests use
 * DIFFERENT names, so the run is provably generated and executed, not a
 * memorized transcript. Each is {adjective, noun, class, name}. */
static const char *const compose_vocab[][4] = {
    {"brave",   "knight", "hero",      "aldric"},
    {"loyal",   "hound",  "companion", "bryn"},
    {"curious", "cub",    "explorer",  "nyla"},
    {"wise",    "elder",  "sage",      "orin"},
};
#define COMPOSE_VOCAB_N (sizeof compose_vocab / sizeof compose_vocab[0])

/* Build the runnable `>`-separated turn fragment for one composable part with a
 * given vocab tuple. Parts outside the analytical schema use fixed vocab. The
 * fragments are the same whether displayed (skeleton) or executed (self-test). */
static void build_turn(const char *key, const char *const v[4],
                       char *dst, size_t size) {
    const char *adj = v[0], *noun = v[1], *cls = v[2], *name = v[3];
    if (!strcmp(key, "knowledge"))
        snprintf(dst, size, "every %s %s is a %s > %s is a %s > is %s a %s?",
                 adj, noun, cls, name, noun, name, cls);
    else if (!strcmp(key, "abduce"))
        snprintf(dst, size, "why isn't %s a %s? > %s is %s", name, cls, name, adj);
    else if (!strcmp(key, "robust"))
        snprintf(dst, size, "how robust is that conclusion?");
    else if (!strcmp(key, "calibrate"))
        snprintf(dst, size, "is %s a %s? > how sure are you?", name, cls);
    else if (!strcmp(key, "memory"))
        snprintf(dst, size, "my name is %s > what is my name?", name);
    else if (!strcmp(key, "coref"))
        snprintf(dst, size, "%s is a %s > is he a %s?", name, cls, cls);
    else if (!strcmp(key, "cause"))
        snprintf(dst, size, "%s causes floods > what does %s cause?", noun, noun);
    else if (!strcmp(key, "compare"))
        snprintf(dst, size, "5 is greater than 3 > which is greater, 5 or 3?");
    else
        snprintf(dst, size, " ");
}

/* gen167-169: run a set of composable parts on a FRESH copy of parrot0 and return
 * how many fired (their signature substring appeared in the real output). Each
 * part's turns are built with vocab tuple `v` and fed through brain_respond on the
 * sub-brain; brain_respond has no static state, so this is reentrancy-safe and
 * leaves the live brain untouched. If `observed` is non-NULL, the last firing
 * response is copied into it. This is the engine of every reflexive self-check:
 * the verdict is computed by execution, not asserted. */
static size_t run_composition(const char *const keys[], const char *const sigs[],
                              size_t n, const char *const v[4],
                              char *observed, size_t obs_size) {
    Brain *sub = brain_create();
    if (!sub) return 0;
    size_t fired = 0;
    if (observed && obs_size) observed[0] = '\0';
    for (size_t k = 0; k < n; k++) {
        char trn[256];
        build_turn(keys[k], v, trn, sizeof trn);
        int part_fired = 0;
        char *p = trn;                    /* split on " > ", run each sub-turn */
        while (p && *p) {
            char *gt = strstr(p, " > ");
            if (gt) *gt = '\0';
            while (*p == ' ' || *p == '>') p++;
            char r[512] = "";
            brain_respond(sub, p, r, sizeof r);
            if (strstr(r, sigs[k])) {
                part_fired = 1;
                if (observed && obs_size) snprintf(observed, obs_size, "%s", r);
            }
            p = gt ? gt + 3 : NULL;
        }
        if (part_fired) fired++;
    }
    brain_destroy(sub);
    return fired;
}

