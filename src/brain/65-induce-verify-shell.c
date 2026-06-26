static int mod_induce(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Must carry the intent to USE an induced rule, distinct from a fewshot
     * "probe -> ?" line. */
    enum { Q_NONE, Q_CONT, Q_NEXT, Q_RULE } q = Q_NONE;
    const char *np = NULL;             /* where the query's argument starts */
    const char *m;
    if ((m = strstr(low, "continue from")))      { q = Q_CONT; np = m + 13; }
    else if ((m = strstr(low, "continua da")))   { q = Q_CONT; np = m + 11; }
    else if ((m = strstr(low, "what comes after"))){ q = Q_NEXT; np = m + 16; }
    else if ((m = strstr(low, "cosa viene dopo"))){ q = Q_NEXT; np = m + 15; }
    else if ((m = strstr(low, "apply it to")))   { q = Q_NEXT; np = m + 11; }
    else if ((m = strstr(low, "applicala a")))   { q = Q_NEXT; np = m + 11; }
    else if (strstr(low,"what is the rule") || strstr(low,"what's the rule") ||
             strstr(low,"qual è la regola") || strstr(low,"quale regola") ||
             strstr(low,"che regola")) q = Q_RULE;
    if (q == Q_NONE) return 0;

    /* Parse the transition pairs. Examples live before the query phrase. */
    char ex[256];
    size_t exlen = m ? (size_t)(m - low) : strlen(low);
    if (exlen >= sizeof ex) exlen = sizeof ex - 1;
    memcpy(ex, low, exlen); ex[exlen] = '\0';

    char buf[256]; size_t bn = 0;
    for (const char *p = ex; *p && bn + 4 < sizeof buf; ) {
        if (p[0]=='-' && p[1]=='>') { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=2; }
        else if ((unsigned char)p[0]==0xE2 && (unsigned char)p[1]==0x86 &&
                 (unsigned char)p[2]==0x92) { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=3; }
        else if (*p==',' || *p==';') { buf[bn++]=' '; p++; }
        else buf[bn++]=*p++;
    }
    buf[bn] = '\0';
    char *w[96]; size_t nw = split_words(buf, w, 96);

    long in[16], out_[16]; size_t npair = 0;
    for (size_t i = 0; i + 2 < nw && npair < 16; ) {
        double a, c;
        if (strcmp(w[i+1], "\x1f")==0 && parse_value(strip_edge_punct(w[i]), &a) &&
            parse_value(strip_edge_punct(w[i+2]), &c) &&
            (double)(long)a==a && (double)(long)c==c) {
            in[npair]=(long)a; out_[npair]=(long)c; npair++; i+=3;
        } else i++;
    }
    if (npair < 2) return 0;

    /* Induce a rule from the examples. If nothing fits, claim the turn (the
     * examples are unmistakably ours) but decline honestly. */
    InducedRule r; char rule[160];
    if (!induce_rule(in, out_, npair, &r, rule, sizeof rule)) {
        put("Those examples don't all follow one rule I can express yet.",
            out, out_size);
        return 1;
    }

    char msg[640];
    if (q == Q_RULE) {
        snprintf(msg, sizeof msg, "The rule is %s.", rule);
        put(msg, out, out_size);
        store_proof(b, rule);
        return 1;
    }

    /* Q_CONT / Q_NEXT need the query argument N — copy just its digits/sign so
     * trailing punctuation ("12.", "10?") doesn't defeat the parse. */
    while (np && *np && !(isdigit((unsigned char)*np) ||
           (*np=='-' && isdigit((unsigned char)np[1])))) np++;
    char numbuf[32]; size_t kk = 0;
    if (np && *np == '-') numbuf[kk++] = *np++;
    while (np && *np && isdigit((unsigned char)*np) && kk + 1 < sizeof numbuf)
        numbuf[kk++] = *np++;
    numbuf[kk] = '\0';
    double n0;
    if (kk == 0 || !parse_value(numbuf, &n0)) {
        put("I found the rule but not the number to apply it to.", out, out_size);
        store_proof(b, rule);
        return 1;
    }

    if (q == Q_NEXT) {
        double v = apply_rule(&r, n0);
        char nb[64]; format_num(v, nb, sizeof nb);
        snprintf(msg, sizeof msg, "%s. (rule: %s.)", nb, rule);
        put(msg, out, out_size);
        char proof[256]; snprintf(proof, sizeof proof, "induced %s", rule);
        store_proof(b, proof);
        return 1;
    }

    /* Q_CONT: run the induced rule from N, stopping at the 1-attractor, a fixed
     * point, or a term cap — and show the trajectory it generated. */
    double cur = n0;
    char traj[400], nb[64];
    format_num(cur, nb, sizeof nb);
    size_t to = (size_t)snprintf(traj, sizeof traj, "%s", nb);
    for (int step = 0; step < 14; step++) {
        if (cur >= 1 - 1e-9 && cur <= 1 + 1e-9) break;
        double nx = apply_rule(&r, cur);
        if (nx == cur) break;            /* fixed point */
        cur = nx;
        format_num(cur, nb, sizeof nb);
        to += (size_t)snprintf(traj + to, sizeof traj - to, " -> %s", nb);
        if (to > sizeof traj - 40) break;
    }
    snprintf(msg, sizeof msg, "%s. (induced %s.)", traj, rule);
    put(msg, out, out_size);
    store_proof(b, traj);
    return 1;
}

/* gen119 (rung 19 + rung 13, deeper): goal-directed SEARCH — the planner agent.
 * gen116 iterated ONE given action; gen118 INDUCED a rule. gen119 is the
 * deductive complement: given a start, a target, and a SET of available actions
 * ("from 3, using times 3 and add 1, reach 28"), the agent SEARCHES the action
 * space with breadth-first search and synthesizes the SHORTEST sequence of tool
 * calls that reaches the goal — means-ends problem solving. The plan is not
 * stored; it is found each time, so held-out start/target/ops transfer, and it
 * is verified by replaying it (the trajectory shown is the real one). A node cap
 * and a value-range prune keep the search bounded and honest about failure. */
static void search_op_label(const AgentOp *op, char *buf, size_t sz) {
    char k[32]; format_num(op->k, k, sizeof k);
    snprintf(buf, sz, "%c%s", op->op, k);
}

static int mod_search(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Gate: an explicit action set ("using …") and a reach-target marker. */
    const char *uk = strstr(low, "using"); if (!uk) uk = strstr(low, "usando");
    if (!uk) return 0;
    const char *rk = NULL; const char *cues[] =
        { "reach", "make", "get to", "raggiungere", "arrivare a", "ottenere", NULL };
    for (size_t i = 0; cues[i]; i++) { const char *p = strstr(low, cues[i]);
        if (p && p > uk && (!rk || p < rk)) rk = p; }
    if (!rk) return 0;

    /* start: first number before "using". */
    char head[256]; size_t hl = (size_t)(uk - low);
    if (hl >= sizeof head) hl = sizeof head - 1;
    memcpy(head, low, hl); head[hl] = '\0';
    char hb[256]; snprintf(hb, sizeof hb, "%s", head);
    char *hw[64]; size_t hnw = split_words(hb, hw, 64);
    double snums[8]; size_t sn = collect_numbers(hw, hnw, snums, 8);
    if (sn == 0) return 0;

    /* available actions: the op sequence parsed from between "using" and the
     * reach marker — each parsed op is a SEPARATE available action. */
    char opreg[256]; size_t ol = (size_t)(rk - (uk + 5));
    const char *ops_start = uk + 5;
    if (ol >= sizeof opreg) ol = sizeof opreg - 1;
    memcpy(opreg, ops_start, ol); opreg[ol] = '\0';
    AgentOp acts[6]; size_t nacts = parse_branch_ops(opreg, acts, 6);
    if (nacts == 0) return 0;

    /* target: first number after the reach marker. */
    char tail[128]; snprintf(tail, sizeof tail, "%s", rk);
    char *tw[64]; size_t tnw = split_words(tail, tw, 64);
    double tnums[8]; size_t tn = collect_numbers(tw, tnw, tnums, 8);
    if (tn == 0) return 0;

    double start = snums[0], target = tnums[0];

    /* Breadth-first search. Each node remembers its parent and the action taken,
     * so the shortest plan can be reconstructed. Visited values are kept unique;
     * states wandering far outside [−,+] the target range are pruned. */
    enum { CAP = 6000 };
    static double val[CAP]; static int par[CAP]; static int act[CAP];
    size_t nnodes = 0, qh = 0;
    double lo = -1e6, hi = (target > 0 ? target : -target) * 8 + 2000;
    val[nnodes] = start; par[nnodes] = -1; act[nnodes] = -1; nnodes++;
    int goal = (start == target) ? 0 : -1;

    while (qh < nnodes && goal < 0) {
        double cur = val[qh];
        for (size_t a = 0; a < nacts && goal < 0; a++) {
            int ok; char o[2] = { acts[a].op, 0 };
            double nx = apply_arith_op(o, cur, acts[a].k, &ok);
            if (!ok) continue;
            if ((double)(long long)nx != nx) continue;   /* integer states only */
            if (nx < lo || nx > hi) continue;
            int seen = 0;
            for (size_t i = 0; i < nnodes; i++) if (val[i] == nx) { seen = 1; break; }
            if (seen) continue;
            if (nnodes >= CAP) { qh = nnodes; break; }   /* exhausted budget */
            val[nnodes] = nx; par[nnodes] = (int)qh; act[nnodes] = (int)a;
            if (nx == target) goal = (int)nnodes;
            nnodes++;
        }
        qh++;
    }

    if (goal < 0) {
        char sb[64], tb[64]; format_num(start, sb, sizeof sb);
        format_num(target, tb, sizeof tb);
        char msg[256];
        snprintf(msg, sizeof msg,
                 "I couldn't reach %s from %s with those operations.", tb, sb);
        put(msg, out, out_size);
        return 1;
    }

    /* Reconstruct the plan from the goal node back to the start. */
    int chain[64]; size_t clen = 0;
    for (int i = goal; i >= 0 && clen < 64; i = par[i]) chain[clen++] = i;

    char traj[400]; size_t to = 0;
    char nb[64];
    format_num(val[chain[clen - 1]], nb, sizeof nb);
    to += (size_t)snprintf(traj + to, sizeof traj - to, "%s", nb);
    for (size_t i = clen - 1; i-- > 0; ) {
        int node = chain[i];
        char lbl[40]; search_op_label(&acts[act[node]], lbl, sizeof lbl);
        format_num(val[node], nb, sizeof nb);
        to += (size_t)snprintf(traj + to, sizeof traj - to, " -[%s]-> %s", lbl, nb);
        if (to > sizeof traj - 48) break;
    }

    size_t steps = clen - 1;
    char sb[64], tb[64]; format_num(start, sb, sizeof sb); format_num(target, tb, sizeof tb);
    char msg[520];
    snprintf(msg, sizeof msg, "Reached %s from %s in %zu step%s: %s.",
             tb, sb, steps, steps == 1 ? "" : "s", traj);
    put(msg, out, out_size);
    char proof[460];
    snprintf(proof, sizeof proof, "search plan (%zu steps): %s", steps, traj);
    store_proof(b, proof);
    return 1;
}

/* gen120 (rung 19 + rung 16, the capstone): hypothesis TESTING / falsification.
 * gen118 forms a law from examples; gen120 puts it on trial. Shown examples plus
 * a held-out transition ("… does 10 -> 21 fit?"), the agent induces the rule from
 * the examples, applies it to the test input, and either CONFIRMS the transition
 * or REFUTES it — naming exactly what the rule predicted instead. This closes the
 * loop the whole experiment runs (observe → hypothesize → predict → TEST) as a
 * feature inside the agent: PRINCIPLES.md's "introspection proposes; the tests
 * dispose", one level down. A wrong datum cannot hide — the predicted value is
 * computed, not asserted. */
static int mod_verify(Brain *b, const char *norm, const char *raw,
                      char *out, size_t out_size) {
    (void)norm;
    if (!b) return 0;

    char low[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof low) return 0;
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)raw[i]);

    /* Intent to judge a transition against a rule. */
    if (!(strstr(low,"fit") || strstr(low,"consistent") || strstr(low,"refute") ||
          strstr(low,"hold") || strstr(low,"coerente") || strstr(low,"rispetta")))
        return 0;

    /* Tokenize, turning arrows into a sentinel, and collect integer pairs. */
    char buf[256]; size_t bn = 0;
    for (const char *p = low; *p && bn + 4 < sizeof buf; ) {
        if (p[0]=='-' && p[1]=='>') { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=2; }
        else if ((unsigned char)p[0]==0xE2 && (unsigned char)p[1]==0x86 &&
                 (unsigned char)p[2]==0x92) { buf[bn++]=' '; buf[bn++]='\x1f'; buf[bn++]=' '; p+=3; }
        else if (*p==',' || *p==';') { buf[bn++]=' '; p++; }
        else buf[bn++]=*p++;
    }
    buf[bn] = '\0';
    char *w[96]; size_t nw = split_words(buf, w, 96);
    long in[16], out_[16]; size_t npair = 0;
    for (size_t i = 0; i + 2 < nw && npair < 16; ) {
        double a, c;
        if (strcmp(w[i+1], "\x1f")==0 && parse_value(strip_edge_punct(w[i]), &a) &&
            parse_value(strip_edge_punct(w[i+2]), &c) &&
            (double)(long)a==a && (double)(long)c==c) {
            in[npair]=(long)a; out_[npair]=(long)c; npair++; i+=3;
        } else i++;
    }
    if (npair < 3) return 0;        /* need >= 2 examples + 1 test transition */

    /* The last transition is the one under test; the rest are the evidence. */
    long tin = in[npair-1], tout = out_[npair-1];
    size_t nex = npair - 1;

    InducedRule r; char rule[160];
    if (!induce_rule(in, out_, nex, &r, rule, sizeof rule)) {
        put("Those examples don't all follow one rule I can express yet.",
            out, out_size);
        return 1;
    }

    double pred = apply_rule(&r, (double)tin);
    char ib[64], tb[64], pb[64];
    format_num((double)tin, ib, sizeof ib);
    format_num((double)tout, tb, sizeof tb);
    format_num(pred, pb, sizeof pb);
    char msg[400];
    if (pred == (double)tout)
        snprintf(msg, sizeof msg, "Yes — %s -> %s fits the rule (%s).", ib, tb, rule);
    else
        snprintf(msg, sizeof msg,
                 "No — the rule (%s) predicts %s -> %s, not %s.", rule, ib, pb, tb);
    put(msg, out, out_size);
    store_proof(b, rule);
    return 1;
}

/* Run the real shell command via popen and capture its stdout. */
static int run_shell(const char *cmd, char *out, size_t out_size) {
    FILE *f = popen(cmd, "r");
    if (!f) return 0;
    size_t n = fread(out, 1, out_size - 1, f);
    out[n] = '\0';
    pclose(f);
    return 1;
}

static int mod_shell(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    (void)norm;

    /* lowercased probe to detect the question shape; the command line is sliced
     * from raw with case preserved. */
    char low[256], rw[256];
    size_t rl = strlen(raw);
    if (rl >= sizeof rw) return 0;
    memcpy(rw, raw, rl + 1);
    while (rl > 0 && (rw[rl-1]=='?'||rw[rl-1]=='.'||rw[rl-1]==' '||rw[rl-1]=='\n'))
        rw[--rl] = '\0';
    for (size_t i = 0; i <= rl; i++) low[i] = (char)tolower((unsigned char)rw[i]);

    /* gen62: oracle-grounded output prediction for pure commands.
     * Only runs when PARROT0_ORACLE=1. */
    const char *oracle_env = getenv("PARROT0_ORACLE");
    int oracle_on = oracle_env && strcmp(oracle_env, "1") == 0;
    const char *pred_cmd = NULL;
    if (oracle_on && strncmp(low, "what does ", 10) == 0) {
        const char *print_pos = strstr(low + 10, " print");
        if (print_pos) {
            size_t cmdlen = (size_t)(print_pos - (low + 10));
            rw[10 + cmdlen] = '\0';
            pred_cmd = rw + 10;
        }
    } else if (oracle_on && strncmp(low, "predict the output of ", 22) == 0) {
        pred_cmd = rw + 22;
    }
    if (pred_cmd) {
        char predicted[4096], actual[4096];
        if (!simulate_pipeline(pred_cmd, predicted, sizeof predicted)) {
            put("I can't predict the output of that yet.", out, out_size);
            return 1;
        }
        char safe_cmd[512];
        snprintf(safe_cmd, sizeof safe_cmd, "%s", pred_cmd);
        if (!run_shell(safe_cmd, actual, sizeof actual)) {
            put("I couldn't run the shell oracle.", out, out_size);
            return 1;
        }
        if (strcmp(predicted, actual) == 0) {
            char show[4096];
            snprintf(show, sizeof show, "%s", predicted);
            size_t sl = strlen(show);
            if (sl > 0 && show[sl - 1] == '\n') show[sl - 1] = '\0';
            char msg[256];
            snprintf(msg, sizeof msg, "It prints: %s.", show);
            put(msg, out, out_size);
        } else {
            char msg[16384];
            snprintf(msg, sizeof msg,
                     "I predicted [%s] but the shell said [%s].",
                     predicted, actual);
            put(msg, out, out_size);
        }
        return 1;
    }

    char *cl;
    if (strncmp(low, "what does ", 10) == 0) {
        size_t tl = rl;
        if (tl >= 3 && strcmp(low + tl - 3, " do") == 0) rw[tl - 3] = '\0';
        else if (tl >= 5 && strcmp(low + tl - 5, " mean") == 0) rw[tl - 5] = '\0';
        else return 0;
        cl = rw + 10;
    } else if (strncmp(low, "explain ", 8) == 0) {
        cl = rw + 8;
    } else {
        return 0;
    }

    /* gen61: simple pipeline support. Split on '|' and compose descriptions. */
    char pipeline[256];
    snprintf(pipeline, sizeof pipeline, "%s", cl);
    if (strchr(pipeline, '|')) {
        char *segs[8];
        size_t nseg = 0;
        char *p = pipeline;
        while (p && *p && nseg < 8) {
            char *next = strchr(p, '|');
            if (next) *next++ = '\0';
            while (*p && isspace((unsigned char)*p)) p++;
            segs[nseg++] = p;
            p = next;
        }
        char desc[8][512];
        size_t got = 0;
        for (size_t i = 0; i < nseg; i++) {
            if (describe_command(b, segs[i], desc[got], sizeof desc[got])) {
                size_t dl = strlen(desc[got]);
                if (dl > 0 && desc[got][dl - 1] == '.')
                    desc[got][dl - 1] = '\0';
                got++;
            }
        }
        if (got == 0) return 0;
        char msg[2048];
        size_t o = (size_t)snprintf(msg, sizeof msg, "%s", desc[0]);
        for (size_t i = 1; i < got && o < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, ", then %s", desc[i]);
        if (o < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
        put(msg, out, out_size);
        return 1;
    }

    return describe_command(b, cl, out, out_size) ? 1 : 0;
}

/* --- module: discourse ---------------------------------------------------
 * gen58: a minimal discourse memory. The brain keeps a small rolling buffer of
 * recent content words extracted from user turns, and this module answers
 * summary questions from that real state rather than a canned line.
 * The topic extraction is intentionally shallow — non-stopword alphabetic
 * tokens, len>=3 — so it stays honest and testable; it is not a summarizer. */
static void update_topics(Brain *b, const char *norm) {
    if (!b) return;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) len = sizeof buf - 1;
    memcpy(buf, norm, len + 1);
    char *w[64];
    size_t nw = split_words(buf, w, 64);
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strlen(t) < 3 || !isalpha((unsigned char)t[0]) || is_stopword(b, t))
            continue;
        int dup = 0;
        for (size_t j = 0; j < b->topic_count; j++)
            if (strcmp(b->topics[j], t) == 0) { dup = 1; break; }
        if (dup) continue;
        if (b->topic_count < BRAIN_TOPICS_MAX) {
            copy_trim(b->topics[b->topic_count], sizeof b->topics[b->topic_count], t);
            b->topic_count++;
        }
    }
}

/* gen121 (L6): the content words of proposition `i`, lowercased. A content word
 * is alphabetic, length >= 3, not a stop-word — the same shallow, honest filter
 * update_topics uses. Returns how many were written. */
static size_t prop_content_words(Brain *b, size_t i, char words[][KB_TERM_LEN],
                                 size_t max) {
    char buf[192]; snprintf(buf, sizeof buf, "%s", b->props[i]);
    for (char *p = buf; *p; p++) *p = (char)tolower((unsigned char)*p);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    size_t n = 0;
    for (size_t j = 0; j < nw && n < max; j++) {
        char *t = strip_edge_punct(w[j]);
        if (strlen(t) < 3 || !isalpha((unsigned char)t[0]) || is_stopword(b, t))
            continue;
        int dup = 0;
        for (size_t k = 0; k < n; k++) if (strcmp(words[k], t) == 0) { dup = 1; break; }
        if (dup) continue;
        snprintf(words[n], KB_TERM_LEN, "%s", t); n++;
    }
    return n;
}

/* gen121 (L6): a real EXTRACTIVE summary. The shallow topic-word answer in
 * mod_discourse was honest but not a summary. This ranks the propositions a
 * `read:` passage actually yielded by CONCEPT CENTRALITY — each sentence scored
 * by how often its content words recur across the whole passage, so the
 * sentences about the most-connected concepts surface — and quotes the top few
 * REAL sentences, in original order for readability. It summarizes only what was
 * genuinely parsed into facts; whatever the reader skipped is, honestly, not
 * here. Held-out: any passage of parseable propositions transfers. */
static int mod_summary(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    int want_sum = cue(norm, "summarize") || cue(norm, "summary") ||
                   cue(norm, "riassumi") || cue(norm, "riassunto") ||
                   cue(norm, "in short") || cue(norm, "in breve");
    /* gen122: the GIST — the single central concept and the most salient
     * sentence, for "what is this about?" rather than a multi-sentence digest. */
    int want_gist = cue(norm, "what is this about") || cue(norm, "what's this about") ||
                    cue(norm, "what is it about") || cue(norm, "what's it about") ||
                    cue(norm, "main point") || cue(norm, "the gist") ||
                    cue(norm, "di cosa parla") || cue(norm, "punto principale");

    /* gen123: query-FOCUSED comprehension — "what did you learn/read about X?"
     * pulls the sentences about X. Gated to learn/read phrasing so a bare "what
     * do you know about X" stays with mod_knowledge's entity description; this is
     * the PASSAGE digest. The focus word follows "about"/"di"/"su". */
    int focus_intent = cue(norm, "learn about") || cue(norm, "learned about") ||
                       cue(norm, "read about") || cue(norm, "imparato") ||
                       cue(norm, "letto") || (want_sum && cue(norm, "about"));
    char focus[KB_TERM_LEN] = "";
    if (focus_intent) {
        const char *mk = strstr(norm, "about ");
        if (mk) mk += 6;
        else if ((mk = strstr(norm, " su "))) mk += 4;
        else if ((mk = strstr(norm, " di "))) mk += 4;
        if (mk) {
            size_t k = 0;
            while (*mk && !isalnum((unsigned char)*mk)) mk++;
            while (*mk && (isalnum((unsigned char)*mk)) && k + 1 < sizeof focus)
                focus[k++] = (char)tolower((unsigned char)*mk++);
            focus[k] = '\0';
        }
    }
    int want_focus = (*focus && strcmp(focus,"this") && strcmp(focus,"it") &&
                      !is_stopword(b, focus));
    if ((!want_sum && !want_gist && !want_focus) || b->prop_count == 0) return 0;

    /* Global content-word frequencies across all propositions. */
    char vocab[128][KB_TERM_LEN]; size_t freq[128]; size_t nv = 0;
    char words[24][KB_TERM_LEN];
    for (size_t i = 0; i < b->prop_count; i++) {
        size_t nwc = prop_content_words(b, i, words, 24);
        for (size_t j = 0; j < nwc; j++) {
            size_t v; for (v = 0; v < nv; v++) if (strcmp(vocab[v], words[j]) == 0) break;
            if (v == nv && nv < 128) { snprintf(vocab[nv], KB_TERM_LEN, "%s", words[j]); freq[nv]=0; nv++; }
            if (v < 128) freq[v]++;
        }
    }

    /* Score each proposition = sum of its content words' global frequencies. */
    long score[BRAIN_PROPS_MAX];
    for (size_t i = 0; i < b->prop_count; i++) {
        size_t nwc = prop_content_words(b, i, words, 24);
        long s = 0;
        for (size_t j = 0; j < nwc; j++)
            for (size_t v = 0; v < nv; v++)
                if (strcmp(vocab[v], words[j]) == 0) { s += (long)freq[v]; break; }
        score[i] = s;
    }

    /* gen123: a focused digest — the sentences whose content words include the
     * focus, in original order. Honest when the passage says nothing about it. */
    if (want_focus) {
        char msg[640]; size_t o = (size_t)snprintf(msg, sizeof msg, "About %s:", focus);
        size_t hits = 0; char words[24][KB_TERM_LEN];
        for (size_t i = 0; i < b->prop_count; i++) {
            size_t nwc = prop_content_words(b, i, words, 24);
            int has = 0;
            for (size_t j = 0; j < nwc; j++) if (strcmp(words[j], focus) == 0) { has = 1; break; }
            if (!has) continue;
            char s[192]; snprintf(s, sizeof s, "%s", b->props[i]);
            size_t sl = strlen(s);
            while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
            o += (size_t)snprintf(msg + o, sizeof msg - o, " %s.", s);
            hits++;
        }
        if (hits == 0)
            snprintf(msg, sizeof msg, "The passage doesn't say anything about %s.", focus);
        put(msg, out, out_size);
        return 1;
    }

    /* gen122: the gist — the most central concept (highest content-word
     * frequency) and the single most salient sentence (highest score). */
    if (want_gist) {
        size_t cv = 0;
        for (size_t v = 1; v < nv; v++) if (freq[v] > freq[cv]) cv = v;
        size_t ti = 0;
        for (size_t i = 1; i < b->prop_count; i++) if (score[i] > score[ti]) ti = i;
        char s[192]; snprintf(s, sizeof s, "%s", b->props[ti]);
        size_t sl = strlen(s);
        while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
        char msg[320];
        snprintf(msg, sizeof msg, "Mainly about %s: %s.", nv ? vocab[cv] : "", s);
        put(msg, out, out_size);
        return 1;
    }

    /* Select the top-k indices (k = up to 3), ties broken by original order. */
    size_t k = b->prop_count < 3 ? b->prop_count : 3;
    int chosen[BRAIN_PROPS_MAX]; size_t nc = 0;
    for (size_t pick = 0; pick < k; pick++) {
        long best = -1; size_t bi = 0; int found = 0;
        for (size_t i = 0; i < b->prop_count; i++) {
            int taken = 0; for (size_t c = 0; c < nc; c++) if (chosen[c]==(int)i) { taken=1; break; }
            if (taken) continue;
            if (!found || score[i] > best) { best = score[i]; bi = i; found = 1; }
        }
        if (found) chosen[nc++] = (int)bi;
    }
    /* Emit the chosen sentences in original passage order. */
    int order[BRAIN_PROPS_MAX]; size_t no = 0;
    for (size_t i = 0; i < b->prop_count; i++)
        for (size_t c = 0; c < nc; c++) if (chosen[c]==(int)i) { order[no++]=(int)i; break; }

    char msg[640]; size_t o = (size_t)snprintf(msg, sizeof msg, "In short:");
    for (size_t i = 0; i < no; i++) {
        char s[192]; snprintf(s, sizeof s, "%s", b->props[order[i]]);
        size_t sl = strlen(s);
        while (sl > 0 && (s[sl-1]=='.'||s[sl-1]==' ')) s[--sl] = '\0';
        o += (size_t)snprintf(msg + o, sizeof msg - o, " %s.", s);
    }
    put(msg, out, out_size);
    return 1;
}

static int mod_discourse(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b) return 0;
    int summary = cue(norm, "what did we talk about") ||
                  cue(norm, "what were we talking about") ||
                  cue(norm, "what have we talked about") ||
                  cue(norm, "cosa abbiamo detto") ||
                  cue(norm, "di cosa abbiamo parlato") ||
                  cue(norm, "summarize") ||
                  cue(norm, "riassumi") ||
                  cue(norm, "riassunto");
    if (!summary) return 0;
    if (b->topic_count == 0) {
        put("We haven't talked about much yet.", out, out_size);
        return 1;
    }
    char msg[256];
    size_t off = (size_t)snprintf(msg, sizeof msg, "We talked about ");
    for (size_t i = 0; i < b->topic_count && off < sizeof msg; i++) {
        const char *sep;
        if (i == 0) sep = "";
        else if (i + 1 == b->topic_count) sep = " and ";
        else sep = ", ";
        off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%s", sep, b->topics[i]);
    }
    if (off < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
    put(msg, out, out_size);
    return 1;
}

/* --- module: social ------------------------------------------------------
 * The dialogue-act layer (gen52). parrot0 had only CONTENT modules
 * (assert/query/reason); a curt "ciao" or "thanks" carries no proposition, so it
 * hit the blank wall and the agent felt dumb. This part recognizes the PHATIC
 * register — utterances whose function is to open/close/maintain the channel —
 * by communicative ACT, not by enumerating canned replies:
 *   1. a small CLOSED-CLASS marker set (greetings/closings/thanks/wellbeing),
 *      multilingual — the "function words" of conversation, like articles, not
 *      content (matched whole-token, so "hi" never fires inside "which");
 *   2. DISCOURSE POSITION (`b->turns`): the same "ciao" opens early and closes
 *      late — structure disambiguating what vocabulary cannot;
 *   3. the ELIMINATION move: a single contentless word that NO content module
 *      claimed, arriving as the opener, is by exclusion a phatic contact act —
 *      so a novel minimal opener is handled without being listed, answered with
 *      an opening that invites content (honest: it does not feign understanding).
 * Runs last (before the not-understood fallback), so content always wins.
 *
 * gen56 (C2b): a social marker must not hijack a turn that also carries real
 * content. "hi, what can you do?" and "thanks, that was wrong" are MIXED acts:
 * the marker acknowledges, but the substance is answered by a content module.
 * We detect mixed turns by the co-occurrence of a phatic marker with a question
 * word or with explicit negative/corrective content after thanks. */
/* gen73: social register as KB knowledge (PLAN.md Phase 3). The word lists
 * formerly hardcoded in C arrays now live in kb/core/social.p0. */
static int is_social_marker(Brain *b, const char *type, const char *word) {
    if (!b || !b->kb) return 0;
    const char *args[] = {type, word};
    return kb_query(b->kb, "social_marker", args, 2);
}

static int has_social_pattern(Brain *b, const char *type, const char *text) {
    if (!b || !b->kb) return 0;
    const char *pat[] = {type, NULL};
    char patterns[64][KB_TERM_LEN];
    size_t n = kb_match(b->kb, "social_pattern", pat, 2, patterns, 64);
    for (size_t i = 0; i < n; i++)
        if (strstr(text, patterns[i]) != NULL) return 1;
    return 0;
}

/* gen225: true when the WHOLE turn is exactly one registered social_pattern
 * phrase (any act type). Lets is_mixed_turn keep a multi-word phatic opener or
 * valediction ("good morning", "good night", "see you") instead of mistaking
 * its component words for substantive content and declining. The act TYPES are
 * structural machinery; the phrases themselves stay KB knowledge. */
static int is_exact_social_pattern(Brain *b, const char *buf) {
    if (!b || !b->kb) return 0;
    static const char *const types[] = {
        "opening", "closing", "wellbeing", "goodnight", "felicitation",
        "wellwish", "condolence", "blessing", "politeness", NULL };
    for (size_t t = 0; types[t]; t++) {
        const char *pat[] = {types[t], NULL};
        char pp[64][KB_TERM_LEN];
        size_t n = kb_match(b->kb, "social_pattern", pat, 2, pp, 64);
        for (size_t i = 0; i < n; i++)
            if (strcmp(buf, pp[i]) == 0) return 1;
    }
    return 0;
}

static int tok_is_marker(Brain *b, const char *type, char **w, size_t nw) {
    if (!b) return 0;
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (is_social_marker(b, type, t)) return 1;
    }
    return 0;
}

static int has_any_question(Brain *b, const char *buf, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        char tmp[64];
        snprintf(tmp, sizeof tmp, "%s", w[i]);
        const char *t = strip_edge_punct(tmp);
        if (t && *t && b && b->kb) {
            const char *args[] = {t};
            if (kb_query(b->kb, "question_word", args, 1)) return 1;
        }
    }
    /* fallback substring cue for multi-word question forms */
    const char *qc[] = {"who","what","where","when","why","how","which",
                        "chi","che","cosa","dove","quando","perche","perché","come"};
    for (size_t i = 0; i < sizeof qc / sizeof qc[0]; i++)
        if (cue(buf, qc[i])) return 1;
    return 0;
}

/* True for a token that contributes real lexical content: long enough, not a
 * stopword, and not itself a social marker. Used to separate phatic-only turns
 * from mixed turns that carry substance beyond the marker. */
static int is_substantive(Brain *b, const char *t) {
    char tmp[64];
    if (strlen(t) >= sizeof tmp) return 0;
    strcpy(tmp, t);
    const char *s = strip_edge_punct(tmp);
    if (strlen(s) < 3) return 0;
    if (is_stopword(b, s)) return 0;
    /* gen73: markers come from KB social_marker facts */
    if (is_social_marker(b, "opening", s) ||
        is_social_marker(b, "closing", s) ||
        is_social_marker(b, "thanks", s) ||
        is_social_marker(b, "apology", s) ||
        is_social_marker(b, "ambiguous", s)) return 0;
    return 1;
}

/* True when the substantive part of the turn is itself a wellbeing check-in;
 * such turns are still owned by the social module (marker + wellbeing is not a
 * mixed act we want to decline). */
static int is_wellbeing_content(const char *buf) {
    /* gen73: patterns from social_pattern(wellbeing, ...) are matched by
     * has_social_pattern in mod_social; this fast substring check stays
     * as a guard in is_mixed_turn. */
    return cue(buf, "how are you") || cue(buf, "how r u") ||
           cue(buf, "how do you do") || cue(buf, "how is it going") ||
           cue(buf, "come stai") || cue(buf, "come va");
}

/* A mixed act combines a social marker with substantive content. If mixed, the
 * social module declines so the content module can own the turn. */
static int is_mixed_turn(Brain *b, const char *buf, char **w, size_t nw,
                         int has_opening, int has_closing, int has_thanks,
                         int has_apology, int has_ambiguous) {
    int has_marker = has_opening || has_closing || has_thanks || has_apology || has_ambiguous;
    if (!has_marker) return 0;

    /* gen225: a turn that is EXACTLY a phatic phrase ("good morning", "good
     * night") is pure social, never mixed — its words only look substantive. */
    if (is_exact_social_pattern(b, buf)) return 0;

    /* question word + marker -> substance wins ("hey, who are you?"),
     * unless the substance is a wellbeing check the social module handles. */
    if (has_any_question(b, buf, w, nw) && !is_wellbeing_content(buf)) return 1;

    /* marker + substantive content -> substance wins
     * ("Hello there, I hope you don't mind me reaching out.")
     * Thanks-based turns are not mixed here; they have their own rule below. */
    if ((has_opening || has_closing || has_apology || has_ambiguous) && !has_thanks) {
        if (is_wellbeing_content(buf)) return 0;
        for (size_t i = 0; i < nw; i++)
            if (is_substantive(b, w[i])) return 1;
    }

    /* thanks + explicit negative/corrective content is not a plain thank-you */
    if (has_thanks) {
        if (cue(buf, "wrong") || cue(buf, "bad") || cue(buf, "not") ||
            cue(buf, "no") || cue(buf, "sbagliato") || cue(buf, "errore") ||
            cue(buf, "male"))
            return 1;
    }

    return 0;
}

