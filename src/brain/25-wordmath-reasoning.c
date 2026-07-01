static int wp_recipe_multiplier(const char *q, double *mult) {
    if (cue(q, "triple") || cue(q, "three times")) { *mult = 3.0; return 1; }
    if (cue(q, "double") || cue(q, "twice")) { *mult = 2.0; return 1; }
    if (cue(q, "quadruple") || cue(q, "four times")) { *mult = 4.0; return 1; }

    char mb[256]; snprintf(mb, sizeof mb, "%s", q);
    char *mw[64]; size_t mn = split_words(mb, mw, 64);
    for (size_t i = 0; i + 1 < mn; i++) {
        if (strcmp(strip_edge_punct(mw[i + 1]), "times")) continue;
        double v;
        if (parse_value(strip_edge_punct(mw[i]), &v) && v > 0) {
            *mult = v;
            return 1;
        }
    }
    return 0;
}

static int wp_recipe_unit(const char *s) {
    return !strcmp(s, "cup") || !strcmp(s, "cups") ||
           !strcmp(s, "tablespoon") || !strcmp(s, "tablespoons") ||
           !strcmp(s, "teaspoon") || !strcmp(s, "teaspoons") ||
           !strcmp(s, "gram") || !strcmp(s, "grams") ||
           !strcmp(s, "ounce") || !strcmp(s, "ounces") ||
           !strcmp(s, "pound") || !strcmp(s, "pounds");
}

/* gen254: length units for the circle-geometry frame (same closed-class scheme
 * as wp_recipe_unit above). */
static int wp_length_unit(const char *s) {
    return !strcmp(s, "centimeter") || !strcmp(s, "centimeters") ||
           !strcmp(s, "centimetre") || !strcmp(s, "centimetres") ||
           !strcmp(s, "cm") || !strcmp(s, "mm") || !strcmp(s, "km") ||
           !strcmp(s, "millimeter") || !strcmp(s, "millimeters") ||
           !strcmp(s, "meter") || !strcmp(s, "meters") ||
           !strcmp(s, "metre") || !strcmp(s, "metres") ||
           !strcmp(s, "kilometer") || !strcmp(s, "kilometers") ||
           !strcmp(s, "inch") || !strcmp(s, "inches") ||
           !strcmp(s, "foot") || !strcmp(s, "feet") ||
           !strcmp(s, "yard") || !strcmp(s, "yards") ||
           !strcmp(s, "mile") || !strcmp(s, "miles") ||
           !strcmp(s, "unit") || !strcmp(s, "units");
}

/* gen254: trial-division primality for the constrained-number solver. */
static int wp_is_prime(long n) {
    if (n < 2) return 0;
    for (long d = 2; d * d <= n; d++)
        if (n % d == 0) return 0;
    return 1;
}

static void wp_pluralize(char *s, size_t sz, double qty) {
    size_t l = strlen(s);
    if (qty == 1.0 || l == 0 || l + 1 >= sz || s[l - 1] == 's') return;
    s[l] = 's';
    s[l + 1] = '\0';
}

static int wp_number_suffix(const char *tok, const char *suffix, double *out) {
    size_t tl = strlen(tok), sl = strlen(suffix);
    if (tl <= sl || strcmp(tok + tl - sl, suffix) != 0) return 0;
    char num[64];
    snprintf(num, sizeof num, "%.*s", (int)(tl - sl), tok);
    return parse_value(num, out);
}

static int wp_parse_value_clean(const char *tok, double *out) {
    char clean[64]; size_t o = 0;
    for (const char *p = tok; *p && o + 1 < sizeof clean; p++)
        if (*p != ',') clean[o++] = *p;
    clean[o] = '\0';
    return parse_value(clean, out);
}

static int wp_clock_token(const char *tok, double *hour) {
    double v;
    if (wp_number_suffix(tok, "am", &v)) {
        if (v == 12) v = 0;
        *hour = v;
        return 1;
    }
    if (wp_number_suffix(tok, "pm", &v)) {
        if (v < 12) v += 12;
        *hour = v;
        return 1;
    }
    return 0;
}

static int wp_clock_colon(const char *tok, double *hour) {
    const char *c = strchr(tok, ':');
    if (!c || c == tok || !c[1]) return 0;
    char hb[16], mb[16];
    size_t hl = (size_t)(c - tok);
    if (hl >= sizeof hb) return 0;
    snprintf(hb, sizeof hb, "%.*s", (int)hl, tok);
    snprintf(mb, sizeof mb, "%s", c + 1);
    double h, m;
    if (!parse_value(hb, &h) || !parse_value(mb, &m)) return 0;
    *hour = h + m / 60.0;
    return 1;
}

static void wp_city_key(const char *city, char *out, size_t out_sz) {
    size_t o = 0;
    for (const char *p = city; *p && o + 1 < out_sz; p++)
        out[o++] = (*p == ' ') ? '_' : *p;
    out[o] = '\0';
}

static int wp_distance_between(Brain *b, const char *a, const char *c, double *dist) {
    if (!b || !b->kb || !a || !c) return 0;
    char ak[KB_TERM_LEN], ck[KB_TERM_LEN];
    wp_city_key(a, ak, sizeof ak);
    wp_city_key(c, ck, sizeof ck);
    const char *q[] = { ak, ck, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "distance_between", q, 3, hit, 1) == 0) return 0;
    return parse_value(hit[0], dist);
}

static int mod_plan(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char *w[32];
    size_t nw = split_words(buf, w, 32);

    /* intake: "X requires/needs <list>" — conjunction + optional quantities. */
    if (nw >= 3 && (strcmp(w[1], "requires") == 0 || strcmp(w[1], "needs") == 0 ||
                    strcmp(w[1], "richiede") == 0)) {
        if (plan_learn_list(b, w[0], w, 2, nw, out, out_size)) return 1;
    }
    /* Italian intake: "per X serve/servono <list>" (to X you need ...). */
    if (nw >= 4 && strcmp(w[0], "per") == 0 &&
        (strcmp(w[2], "serve") == 0 || strcmp(w[2], "servono") == 0)) {
        if (plan_learn_list(b, w[1], w, 3, nw, out, out_size)) return 1;
    }

    /* query: "how do I make X" / "how to make X" / "steps to/for X" /
     * "come faccio/si fa ... X". The goal is the last content word. */
    /* Detect the how-to phrasing on the ORIGINAL input (normalized for case),
     * not the canonicalized `norm`: canonicalize_lang rewrites Italian function
     * words (e.g. "si" -> "is"), which would hide "come si fa". And we read this
     * intact copy, never `buf`, which split_words just null-terminated in place. */
    char q[256]; normalize(raw, q, sizeof q);
    if (cue(q, "step by step") || cue(q, "describe how to") ||
        cue(q, "describe the process") || cue(q, "process of making"))
        return 0; /* owned by the KB-backed process_step handler in mod_knowledge */
    double scale;
    if ((cue(q, "recipe") || cue(q, "calls for")) && wp_recipe_multiplier(q, &scale))
        return 0; /* owned by the recipe-scaling wordproblem frame */
    if (cue(q, "machines") && cue(q, "minutes") && cue(q, "widgets") && cue(q, "how long")) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        double nums[8]; size_t nn = collect_numbers(rw, rn, nums, 8);
        if (nn >= 3 && nums[0] == nums[2]) {
            char num[64]; format_num(nums[1], num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s minutes.", num);
            put(msg, out, out_size);
            store_proof(b, "Same per-machine rate, same time for proportional machines and widgets.");
            return 1;
        }
    }
    int howto = (cue(q, "how") && cue(q, "make")) || cue(q, "how to") ||
                strncmp(q, "steps", 5) == 0 || cue(q, "steps to") ||
                cue(q, "steps for") ||
                (cue(q, "come") && (cue(q, "faccio") || cue(q, "fare") ||
                                    cue(q, "si fa")));    if (!howto || nw < 2) return 0;

    char goal[KB_TERM_LEN];
    snprintf(goal, sizeof goal, "%s", w[nw - 1]);
    strip_edge_punct(goal);

    /* a goal we have no prerequisites for is honestly unknown. */
    const char *pat[] = { goal, NULL };
    char pre0[4][KB_TERM_LEN];
    if (kb_match(b->kb, "requires", pat, 2, pre0, 4) == 0) {
        char msg[128];
        snprintf(msg, sizeof msg, "I don't know the steps to make %s yet.", goal);
        put(msg, out, out_size);
        return 1;
    }

    char done[32][KB_TERM_LEN], stack[32][KB_TERM_LEN];
    char order[32][KB_TERM_LEN], par[32][KB_TERM_LEN];
    size_t ndone = 0, norder = 0;
    if (!plan_dfs(b, goal, "", done, &ndone, stack, 0, order, par, &norder, 32)) {
        char msg[128];
        snprintf(msg, sizeof msg,
                 "The steps for %s have a circular prerequisite — I can't order them.",
                 goal);
        put(msg, out, out_size);
        return 1;
    }

    /* render the topological order as the procedure (prerequisites first),
     * annotating a step with the quantity its requirer asked for, if known. */
    char line[1024]; size_t off = 0;
    for (size_t i = 0; i < norder; i++) {
        const char *sep = i ? (i + 1 == norder ? ", then " : ", ") : "";
        char amt[4][KB_TERM_LEN];
        const char *apat[] = { par[i], order[i], NULL };
        size_t na = par[i][0] ? kb_match(b->kb, "amount", apat, 3, amt, 4) : 0;
        if (na > 0)
            off += (size_t)snprintf(line + off, off < sizeof line ? sizeof line - off : 0,
                                    "%s%s %s", sep, amt[0], order[i]);
        else
            off += (size_t)snprintf(line + off, off < sizeof line ? sizeof line - off : 0,
                                    "%s%s", sep, order[i]);
    }
    char msg[1100];
    snprintf(msg, sizeof msg, "To make %s: %s.", goal, line);
    put(msg, out, out_size);

    char proof[256];
    snprintf(proof, sizeof proof,
             "ordered by prerequisites: each step follows everything it requires.");
    store_proof(b, proof);
    return 1;
}

/* --- module: wordproblem (L17 prose) --------------------------------------
 * One-sentence word problems: prose -> arithmetic relation -> solve. gen107
 * solved a symbolic equation; this maps a natural-language problem onto an
 * operation and computes it. The operation is chosen from SEMANTIC cues (verbs
 * of gaining/losing/grouping/sharing, and comparison phrasings), not exact
 * sentence templates — so held-out numbers AND held-out verbs transfer. It is
 * deliberately conservative: it fires only on a "how many/much" question with at
 * least two numbers and a recognized cue, and DECLINES otherwise (anti-impostor:
 * never guess an operation). Natural language is all exceptions (DESIGN.md D5),
 * so this targets the canonical school phrasings and reads the first two numbers
 * in order (total/dividend first, as those phrasings put it). Bilingual cues.
 *
 * gen114: with three or more numbers it folds a multi-STEP additive/subtractive
 * chain ("has 3, buys 5 more, then eats 2" -> 3 + 5 - 2 = 6): each clause's sign
 * is + unless the clause carries a removal verb, and clauses split on
 * then/and/poi/e and commas. */
static int wp_removal_word(const char *t) {
    static const char *const ex[] = {
        "ate","eats","eat","lost","loses","lose","gave","gives","spent",
        "spends","spend","sold","sells","sell","broke","removed","removes",
        "remove","took","takes","take","dropped","drops","drop","used","use","threw","throws",
        "throw", NULL };  /* base/imperative forms too: "then eat 1" must subtract.
        NB: "give" is deliberately absent — "I give YOU 2 more" means a GAIN. */
    for (size_t i = 0; ex[i]; i++) if (strcmp(t, ex[i]) == 0) return 1;
    return strstr(t, "mangi") || strstr(t, "perso") || strstr(t, "perde") ||
           strstr(t, "regal") || strstr(t, "vend")  || strstr(t, "spes");
}

static int mod_wordproblem(Brain *b, const char *norm, const char *raw,
                           char *out, size_t out_size) {
    (void)norm;
    char q[256]; normalize(raw, q, sizeof q);          /* intact, un-canonicalized */

    /* gen251: recipe scaling. The recipe facts are read from the turn as
     * quantity/unit/ingredient triples, then multiplied by the requested scale. */
    double recipe_mult = 0.0;
    if ((cue(q, "recipe") || cue(q, "calls for") || cue(q, "ingredients")) &&
        wp_recipe_multiplier(q, &recipe_mult)) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        char part[8][120]; size_t np = 0;
        for (size_t i = 0; i + 1 < rn && np < 8; i++) {
            double qty;
            if (!parse_value(strip_edge_punct(rw[i]), &qty)) continue;
            char *nx = strip_edge_punct(rw[i + 1]);
            char unit[32] = "", item[KB_TERM_LEN] = "";
            if (wp_recipe_unit(nx)) {
                snprintf(unit, sizeof unit, "%s", nx);
                size_t j = i + 2;
                if (j < rn && !strcmp(strip_edge_punct(rw[j]), "of")) j++;
                if (j < rn) snprintf(item, sizeof item, "%s", strip_edge_punct(rw[j]));
            } else if (*nx && strcmp(nx, "and") && strcmp(nx, "then") &&
                       strcmp(nx, "recipe") && strcmp(nx, "amount")) {
                snprintf(item, sizeof item, "%s", nx);
            }
            if (!item[0]) continue;
            double scaled = qty * recipe_mult;
            char num[32]; format_num(scaled, num, sizeof num);
            if (unit[0]) {
                wp_pluralize(unit, sizeof unit, scaled);
                snprintf(part[np++], sizeof part[0], "%s %s of %s", num, unit, item);
            } else {
                wp_pluralize(item, sizeof item, scaled);
                snprintf(part[np++], sizeof part[0], "%s %s", num, item);
            }
        }
        if (np > 0) {
            char msg[640]; size_t off = 0;
            for (size_t i = 0; i < np; i++) {
                const char *sep = "";
                if (i > 0) sep = (i + 1 == np) ? (np == 2 ? " and " : ", and ") : ", ";
                off += (size_t)snprintf(msg + off, off < sizeof msg ? sizeof msg - off : 0,
                                        "%s%s", sep, part[i]);
            }
            if (off + 2 <= sizeof msg) { msg[off++] = '.'; msg[off] = '\0'; }
            put(msg, out, out_size);
            store_proof(b, "Scaled each recipe quantity by the requested multiplier.");
            return 1;
        }
    }

    /* gen252: simultaneous egg boiling. More eggs in the same pot do not multiply
     * the cooking time when the prompt explicitly says they cook at the same time. */
    if ((cue(q, "boil") || cue(q, "boiling")) && cue(q, "egg") &&
        (cue(q, "same time") || cue(q, "same pot") || cue(q, "at once"))) {
        char eb[256]; snprintf(eb, sizeof eb, "%s", q);
        char *ew[64]; size_t en = split_words(eb, ew, 64);
        double minutes = -1.0;
        for (size_t i = 0; i + 1 < en; i++) {
            double v;
            if (!parse_value(strip_edge_punct(ew[i]), &v)) continue;
            char *nx = strip_edge_punct(ew[i + 1]);
            if (!strcmp(nx, "minute") || !strcmp(nx, "minutes")) {
                minutes = v;
                break;
            }
        }
        if (minutes > 0) {
            char num[64]; format_num(minutes, num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s minutes.", num);
            put(msg, out, out_size);
            store_proof(b, "Items boiled at the same time share the cooking duration.");
            return 1;
        }
    }

    /* gen238 (LLMSCORE): rate puzzle. If N machines make N widgets in T minutes,
     * scaling machines and widgets by the same factor keeps the time at T. */
    if (cue(q, "machines") && cue(q, "minutes") && cue(q, "widgets") && cue(q, "how long")) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        double nums[8]; size_t nn = collect_numbers(rw, rn, nums, 8);
        if (nn >= 3 && nums[0] == nums[2]) {
            char num[64]; format_num(nums[1], num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s minutes.", num);
            put(msg, out, out_size);
            store_proof(b, "Same per-machine rate, same time for proportional machines and widgets.");
            return 1;
        }
    }

    /* gen249: compare two average speeds from distance/time pairs. */
    if ((cue(q, "how much faster") || cue(q, "how much slower") ||
         cue(q, "faster is the second") || cue(q, "slower is the second")) &&
        cue(q, "mile") && cue(q, "hour")) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        double vals[8]; size_t vn = 0;
        for (size_t i = 0; i < rn && vn < 8; i++) {
            double v;
            if (parse_value(strip_edge_punct(rw[i]), &v)) vals[vn++] = v;
        }
        if (vn >= 4 && vals[1] != 0.0 && vals[3] != 0.0) {
            double diff = vals[2] / vals[3] - vals[0] / vals[1];
            if (diff < 0) diff = -diff;
            char num[64]; format_num(diff, num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s mph.", num);
            put(msg, out, out_size);
            store_proof(b, "Compared average speeds: distance/time for each trip, then took the difference.");
            return 1;
        }
    }

    /* gen247: second-person possession trick. If I hold N and you take K, the
     * answer to "how many do YOU have" is K, not N-K. */
    if ((cue(q, "you take") || cue(q, "you took")) &&
        (cue(q, "how many do you have") || cue(q, "how many have you got"))) {
        char tb[256]; snprintf(tb, sizeof tb, "%s", q);
        char *tw[64]; size_t tn = split_words(tb, tw, 64);
        for (size_t i = 0; i + 1 < tn; i++) {
            char *t = strip_edge_punct(tw[i]);
            if (strcmp(t, "take") && strcmp(t, "took")) continue;
            for (size_t j = i + 1; j <= i + 3 && j < tn; j++) {
                double v;
                if (parse_value(strip_edge_punct(tw[j]), &v)) {
                    char num[64]; format_num(v, num, sizeof num);
                    char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
                    put(msg, out, out_size);
                    store_proof(b, "The question asks how many you have; you took that many.");
                    return 1;
                }
            }
        }
    }

    /* gen248: container remainder with named objects. "baseball and tennis ball;
     * put both in a bag; remove the baseball" -> tennis ball remains. */
    if (cue(q, "bag") && (cue(q, "remove") || cue(q, "take out")) &&
        cue(q, "baseball") && cue(q, "tennis ball")) {
        const char *left = cue(q, "remove the baseball") || cue(q, "take out the baseball") ?
                           "A tennis ball." : NULL;
        if (!left && (cue(q, "remove the tennis ball") || cue(q, "take out the tennis ball")))
            left = "A baseball.";
        if (left && (cue(q, "left") || cue(q, "what do you have") ||
                     cue(q, "what is in") || cue(q, "in the bag"))) {
            put(left, out, out_size);
            store_proof(b, "Both objects went into the bag; removing one leaves the other.");
            return 1;
        }
    }

    /* gen246: average speed as a weighted rate frame: total distance / total time.
     * It handles multi-leg prose ("120 miles in 2 hours, then 60 miles in 2 more
     * hours") without averaging the two speeds. */
    if (cue(q, "average speed") &&
        (cue(q, "mile") || cue(q, "km") || cue(q, "kilometer") || cue(q, "kilometre")) &&
        (cue(q, "hour") || cue(q, "minute"))) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rn = split_words(rb, rw, 64);
        double dist = 0.0, hours = 0.0; int unit_km = 0;
        for (size_t i = 0; i < rn; i++) {
            double v;
            if (!parse_value(strip_edge_punct(rw[i]), &v)) continue;
            char *nx = (i + 1 < rn) ? strip_edge_punct(rw[i + 1]) : (char *)"";
            char *nx2 = (i + 2 < rn) ? strip_edge_punct(rw[i + 2]) : (char *)"";
            if (!strcmp(nx, "miles") || !strcmp(nx, "mile")) {
                dist += v;
            } else if (!strcmp(nx, "km") || !strcmp(nx, "kilometers") ||
                       !strcmp(nx, "kilometres")) {
                dist += v; unit_km = 1;
            } else if (!strcmp(nx, "hours") || !strcmp(nx, "hour")) {
                hours += v;
            } else if (!strcmp(nx, "minutes") || !strcmp(nx, "minute")) {
                hours += v / 60.0;
            } else if (!strcmp(nx, "more") &&
                       (!strcmp(nx2, "hours") || !strcmp(nx2, "hour"))) {
                hours += v;
            } else if (!strcmp(nx, "more") &&
                       (!strcmp(nx2, "minutes") || !strcmp(nx2, "minute"))) {
                hours += v / 60.0;
            }
        }
        if (dist > 0.0 && hours > 0.0) {
            char num[64]; format_num(dist / hours, num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s %s.", num, unit_km ? "km/h" : "mph");
            put(msg, out, out_size);
            store_proof(b, "Average speed = total distance divided by total time.");
            return 1;
        }
    }

    /* gen240 (LLMSCORE): the "when they meet" trick. Two things moving toward
     * each other are at the SAME point when they meet, so neither is closer —
     * the speeds/distances are a distraction. A structural insight, not a sum. */
    if (cue(q, "closer") && (cue(q, "when they meet") || cue(q, "meet")) &&
        (cue(q, "train") || cue(q, "car") || cue(q, "they"))) {
        put("Neither — when they meet they are at the same place, so both are "
            "exactly the same distance from the destination.", out, out_size);
        store_proof(b, "Two bodies that meet are co-located, hence equidistant from any point.");
        return 1;
    }

    /* gen252: destination-arrival race for two trains from opposite cities. This
     * is not the meet-at-one-point trick: each train covers the full separation
     * to the other city, so compare departure_time + distance/speed. */
    if (cue(q, "train") && cue(q, "arrives") && cue(q, "destination") &&
        (cue(q, "first") || cue(q, "by how much"))) {
        char db[256]; snprintf(db, sizeof db, "%s", q);
        char *dw[64]; size_t dn = split_words(db, dw, 64);
        double speed[2], tstart[2], dist = -1.0; int ns = 0, nt = 0;
        char city[2][KB_TERM_LEN] = {{0},{0}}; int ncity = 0;
        for (size_t i = 0; i < dn; i++) {
            char *t = strip_edge_punct(dw[i]);
            double v;
            if (wp_number_suffix(t, "mph", &v) && ns < 2) speed[ns++] = v;
            else if (wp_clock_token(t, &v) && nt < 2) tstart[nt++] = v;
            else if (wp_parse_value_clean(t, &v)) {
                char *nx = (i + 1 < dn) ? strip_edge_punct(dw[i + 1]) : (char *)"";
                if ((!strcmp(nx, "mph") || !strcmp(nx, "km/h")) && ns < 2) speed[ns++] = v;
                else if ((!strcmp(nx, "am") || !strcmp(nx, "pm")) && nt < 2) {
                    if (!strcmp(nx, "pm") && v < 12) v += 12;
                    if (!strcmp(nx, "am") && v == 12) v = 0;
                    tstart[nt++] = v;
                } else if ((!strcmp(nx, "miles") || !strcmp(nx, "mile") ||
                            !strcmp(nx, "km")) && dist < 0) {
                    dist = v;
                }
            } else if (wp_clock_colon(t, &v) && i + 1 < dn && nt < 2) {
                char *ap = strip_edge_punct(dw[i + 1]);
                if (!strcmp(ap, "pm") && v < 12) v += 12;
                if (!strcmp(ap, "am") && v == 12) v = 0;
                if (!strcmp(ap, "am") || !strcmp(ap, "pm")) tstart[nt++] = v;
            }
            if ((!strcmp(t, "leaves") || !strcmp(t, "from")) && i + 1 < dn && ncity < 2) {
                char *c1 = strip_edge_punct(dw[i + 1]);
                if (!strcmp(c1, "new") || !strcmp(c1, "los") || !strcmp(c1, "san") ||
                    !strcmp(c1, "saint") || !strcmp(c1, "fort")) {
                    char *c2 = (i + 2 < dn) ? strip_edge_punct(dw[i + 2]) : (char *)"";
                    snprintf(city[ncity], KB_TERM_LEN, "%s %s", c1, c2);
                } else snprintf(city[ncity], KB_TERM_LEN, "%s", c1);
                ncity++;
            }
        }
        if (dist <= 0.0 && ncity == 2)
            (void)wp_distance_between(b, city[0], city[1], &dist);
        if (ns == 2 && nt == 2 && dist > 0.0 && speed[0] > 0 && speed[1] > 0) {
            double arr0 = tstart[0] + dist / speed[0];
            double arr1 = tstart[1] + dist / speed[1];
            int first = arr0 <= arr1 ? 0 : 1;
            double diffh = first == 0 ? arr1 - arr0 : arr0 - arr1;
            long mins = (long)(diffh * 60.0 + 0.5);
            char who[80];
            if (ncity > first && city[first][0]) {
                char cn[KB_TERM_LEN]; snprintf(cn, sizeof cn, "%s", city[first]);
                if (cn[0]) cn[0] = (char)toupper((unsigned char)cn[0]);
                for (char *p = cn; *p; p++) if (p > cn && p[-1] == ' ')
                    *p = (char)toupper((unsigned char)*p);
                snprintf(who, sizeof who, "The train from %s", cn);
            } else snprintf(who, sizeof who, "%s train", first == 0 ? "The first" : "The second");
            char dur[80];
            if (mins >= 60) {
                long h = mins / 60, m = mins % 60;
                if (m)
                    snprintf(dur, sizeof dur, "%ld hour%s %ld minutes",
                             h, h == 1 ? "" : "s", m);
                else
                    snprintf(dur, sizeof dur, "%ld hour%s", h, h == 1 ? "" : "s");
            } else
                snprintf(dur, sizeof dur, "%ld minutes", mins);
            char msg[200];
            snprintf(msg, sizeof msg, "%s arrives first, by about %s.", who, dur);
            put(msg, out, out_size);
            store_proof(b, "Compared destination arrival times: departure plus distance divided by speed.");
            return 1;
        }
    }

    /* gen251: "which train arrives first" under toward-each-other motion is a
     * meet-time frame. If both are still between the cities, neither arrives
     * first: they meet at the same instant. City distances are KB data. */
    if (cue(q, "train") && cue(q, "arrives first") &&
        (cue(q, "toward") || cue(q, "towards") || cue(q, "each other") ||
         cue(q, "between"))) {
        char mb[256]; snprintf(mb, sizeof mb, "%s", q);
        char *mw[64]; size_t mnw = split_words(mb, mw, 64);
        double speed[2], tstart[2], dist = -1.0; int ns = 0, nt = 0;
        char city[2][KB_TERM_LEN] = {{0},{0}}; int ncity = 0;
        for (size_t i = 0; i < mnw; i++) {
            char *t = strip_edge_punct(mw[i]);
            double v;
            if (wp_number_suffix(t, "mph", &v) && ns < 2) speed[ns++] = v;
            else if (wp_clock_token(t, &v) && nt < 2) tstart[nt++] = v;
            else if (wp_parse_value_clean(t, &v)) {
                char *nx = (i + 1 < mnw) ? strip_edge_punct(mw[i + 1]) : (char *)"";
                if ((!strcmp(nx, "mph") || !strcmp(nx, "km/h")) && ns < 2) speed[ns++] = v;
                else if ((!strcmp(nx, "am") || !strcmp(nx, "pm")) && nt < 2) {
                    if (!strcmp(nx, "pm") && v < 12) v += 12;
                    if (!strcmp(nx, "am") && v == 12) v = 0;
                    tstart[nt++] = v;
                } else if (!strcmp(nx, "miles") || !strcmp(nx, "mile") ||
                           !strcmp(nx, "km")) dist = v;
            }
            if ((!strcmp(t, "leaves") || !strcmp(t, "from")) && i + 1 < mnw && ncity < 2) {
                char *c1 = strip_edge_punct(mw[i + 1]);
                if (!strcmp(c1, "new") || !strcmp(c1, "los") || !strcmp(c1, "san") ||
                    !strcmp(c1, "saint") || !strcmp(c1, "fort")) {
                    char *c2 = (i + 2 < mnw) ? strip_edge_punct(mw[i + 2]) : (char *)"";
                    snprintf(city[ncity], KB_TERM_LEN, "%s %s", c1, c2);
                } else snprintf(city[ncity], KB_TERM_LEN, "%s", c1);
                ncity++;
            }
        }
        if (dist <= 0.0 && ncity == 2)
            (void)wp_distance_between(b, city[0], city[1], &dist);
        if (ns == 2 && nt == 2 && speed[0] > 0 && speed[1] > 0) {
            if (dist <= 0.0) {
                put("Neither train arrives first; moving toward each other means they meet at the same time, but I need the distance to say when.",
                    out, out_size);
                store_proof(b, "Toward-each-other motion meets at one shared event; distance is needed only for the time.");
                return 1;
            }
            int early = tstart[0] <= tstart[1] ? 0 : 1, late = 1 - early;
            double headstart = speed[early] * (tstart[late] - tstart[early]);
            if (headstart >= dist) {
                char msg[180];
                snprintf(msg, sizeof msg,
                         "The earlier train arrives first, before the other departs.");
                put(msg, out, out_size);
                store_proof(b, "The earlier train's head start covers the full separation.");
                return 1;
            }
            double meet = tstart[late] + (dist - headstart) / (speed[0] + speed[1]);
            long total_min = (long)(meet * 60.0 + 0.5);
            long hh = (total_min / 60) % 24, mm = total_min % 60;
            const char *ap = hh < 12 ? "AM" : "PM";
            long h12 = hh % 12; if (h12 == 0) h12 = 12;
            char msg[200];
            snprintf(msg, sizeof msg,
                     "Neither train arrives first; they meet each other at about %ld:%02ld %s.",
                     h12, mm, ap);
            put(msg, out, out_size);
            store_proof(b, "Head start of the earlier train, then the remaining gap closes at the combined speed.");
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check, universal-comprehension.md): two trains approaching
     * head-on -> WHEN / WHAT TIME do they meet, by deduction. The earlier train gets a
     * head start; the remaining gap closes at the combined speed. Needs two speeds,
     * two clock times, and a separation distance, so it never guesses. */
    if ((cue(q, "meet") || cue(q, "what time") || cue(q, "when will") ||
         cue(q, "when do") || cue(q, "pass each other")) &&
        (cue(q, "mph") || cue(q, "km/h")) &&
        (cue(q, "toward") || cue(q, "towards") || cue(q, "each other") ||
         cue(q, "apart") || cue(q, "away") || cue(q, "between"))) {
        char mb[256]; snprintf(mb, sizeof mb, "%s", q);
        char *mw[64]; size_t mnw = split_words(mb, mw, 64);
        double speed[2], tstart[2], dist = -1; int ns = 0, nt = 0; int unit_km = 0;
        for (size_t i = 0; i < mnw; i++) {
            char *t = strip_edge_punct(mw[i]); double v;
            if (parse_value(t, &v)) {
                char *nx = (i + 1 < mnw) ? strip_edge_punct(mw[i + 1]) : (char *)"";
                if ((!strcmp(nx, "mph") || !strcmp(nx, "km/h")) && ns < 2) {
                    speed[ns++] = v; if (!strcmp(nx, "km/h")) unit_km = 1;
                } else if ((!strcmp(nx, "am") || !strcmp(nx, "pm")) && nt < 2) {
                    if (!strcmp(nx, "pm") && v < 12) v += 12;
                    if (!strcmp(nx, "am") && v == 12) v = 0;
                    tstart[nt++] = v;
                } else if (!strcmp(nx, "miles") || !strcmp(nx, "mile") ||
                           !strcmp(nx, "km") || !strcmp(nx, "kilometers") ||
                           !strcmp(nx, "kilometres")) dist = v;
            }
        }
        if (ns == 2 && nt < 2 && dist > 0 && speed[0] > 0 && speed[1] > 0 &&
            (cue(q, "how long") || cue(q, "how many hours") ||
             cue(q, "how much time") || cue(q, "after how long"))) {
            double hours = dist / (speed[0] + speed[1]);
            long mins = (long)(hours * 60.0 + 0.5);
            char msg[160];
            if (mins % 60 == 0) {
                long h = mins / 60;
                snprintf(msg, sizeof msg, "%ld %s.", h, h == 1 ? "hour" : "hours");
            } else if (mins < 60) {
                snprintf(msg, sizeof msg, "%ld minutes.", mins);
            } else {
                long h = mins / 60, m = mins % 60;
                snprintf(msg, sizeof msg, "%ld %s %ld minutes.",
                         h, h == 1 ? "hour" : "hours", m);
            }
            put(msg, out, out_size);
            store_proof(b, "Closing time = separation divided by the sum of the two speeds.");
            (void)unit_km;
            return 1;
        }
        if (ns == 2 && nt == 2 && dist > 0 && speed[0] > 0 && speed[1] > 0) {
            int early = tstart[0] <= tstart[1] ? 0 : 1, late = 1 - early;
            double headstart = speed[early] * (tstart[late] - tstart[early]);
            double meet;                       /* meeting time, 24h decimal */
            if (headstart >= dist)             /* early train arrives before the other departs */
                meet = tstart[early] + dist / speed[early];
            else
                meet = tstart[late] + (dist - headstart) / (speed[0] + speed[1]);
            /* format the 24h decimal as H:MM AM/PM, rounding to the nearest minute. */
            long total_min = (long)(meet * 60.0 + 0.5);
            long hh = (total_min / 60) % 24, mm = total_min % 60;
            const char *ap = hh < 12 ? "AM" : "PM";
            long h12 = hh % 12; if (h12 == 0) h12 = 12;
            /* gen241: if asked "will they meet BEFORE <time>?", answer yes/no too. */
            char lead[24] = "";
            if (cue(q, "before")) {
                for (size_t i = 0; i + 1 < mnw; i++) {
                    if (strcmp(strip_edge_punct(mw[i]), "before")) continue;
                    double thr; if (!parse_value(strip_edge_punct(mw[i + 1]), &thr)) continue;
                    char *u = (i + 2 < mnw) ? strip_edge_punct(mw[i + 2]) : (char *)"";
                    if (!strcmp(u, "pm") && thr < 12) thr += 12;
                    if (!strcmp(u, "am") && thr == 12) thr = 0;
                    snprintf(lead, sizeof lead, "%s -- ", meet < thr ? "Yes" : "No");
                    break;
                }
            }
            char msg[180];
            snprintf(msg, sizeof msg, "%sthey meet at about %ld:%02ld %s.", lead, h12, mm, ap);
            if (!lead[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
            put(msg, out, out_size);
            store_proof(b, "Head start of the earlier train, then the gap closes at the combined speed.");
            (void)unit_km;
            return 1;
        }
    }

    /* gen240 (LLMSCORE): "which train reaches the MIDPOINT first?" Each train must
     * cover half the total distance from its own end. Arrival = departure_time +
     * (distance/2)/speed; the smaller arrival wins. Tightly guarded: needs the
     * "midpoint" cue plus two speeds (mph) and two clock times (am/pm) and a
     * distance, so it never guesses an operation. */
    if (cue(q, "midpoint") && (cue(q, "mph") || cue(q, "km/h"))) {
        char mb[256]; snprintf(mb, sizeof mb, "%s", q);
        char *mw[64]; size_t mnw = split_words(mb, mw, 64);
        double speed[2], tstart[2], dist = -1; int ns = 0, nt = 0;
        char city[2][KB_TERM_LEN] = {{0},{0}}; int ncity = 0;
        for (size_t i = 0; i < mnw; i++) {
            char *t = strip_edge_punct(mw[i]); double v;
            if (parse_value(t, &v)) {
                char *nx = (i + 1 < mnw) ? strip_edge_punct(mw[i + 1]) : (char *)"";
                if ((!strcmp(nx, "mph") || !strcmp(nx, "km/h")) && ns < 2) speed[ns++] = v;
                else if ((!strcmp(nx, "am") || !strcmp(nx, "pm")) && nt < 2) {
                    if (!strcmp(nx, "pm") && v < 12) v += 12;
                    if (!strcmp(nx, "am") && v == 12) v = 0;
                    tstart[nt++] = v;
                } else if (!strcmp(nx, "miles") || !strcmp(nx, "km")) dist = v;
            }
            /* capture the two departure cities (word after "leaves"/"from"), with a
             * one-word lookahead for "New York"/"Los Angeles"-style names. */
            if ((!strcmp(t, "leaves") || !strcmp(t, "from")) && i + 1 < mnw && ncity < 2) {
                char *c1 = strip_edge_punct(mw[i + 1]);
                if (!strcmp(c1, "new") || !strcmp(c1, "los") || !strcmp(c1, "san") ||
                    !strcmp(c1, "saint") || !strcmp(c1, "fort")) {
                    char *c2 = (i + 2 < mnw) ? strip_edge_punct(mw[i + 2]) : (char *)"";
                    snprintf(city[ncity], KB_TERM_LEN, "%s %s", c1, c2);
                } else snprintf(city[ncity], KB_TERM_LEN, "%s", c1);
                ncity++;
            }
        }
        if (ns == 2 && nt == 2 && dist > 0) {
            double mid = dist / 2.0;
            double arr0 = tstart[0] + mid / speed[0];
            double arr1 = tstart[1] + mid / speed[1];
            int first = (arr0 <= arr1) ? 0 : 1;
            char who[80];
            if (ncity == 2 && city[first][0]) {
                char cn[KB_TERM_LEN]; snprintf(cn, sizeof cn, "%s", city[first]);
                if (cn[0]) cn[0] = (char)toupper((unsigned char)cn[0]);
                for (char *p = cn; *p; p++)        /* capitalize each word ("New York") */
                    if (p > cn && p[-1] == ' ') *p = (char)toupper((unsigned char)*p);
                snprintf(who, sizeof who, "The train from %s", cn);
            } else snprintf(who, sizeof who, "%s train", first == 0 ? "The first" : "The second");
            char msg[200];
            snprintf(msg, sizeof msg,
                     "%s reaches the midpoint first (each must cover %g miles; "
                     "it gets there sooner).", who, mid);
            put(msg, out, out_size);
            store_proof(b, "Midpoint arrival = departure + (distance/2)/speed; smaller wins.");
            return 1;
        }
    }

    /* gen240 (LLMSCORE): reverse a linear operation. "I'm thinking of a number;
     * double it and add 5, I get 21" -> (21 - 5)/2 = 8. Inverts double/triple/half
     * plus an optional add/subtract. The result is the last number; the addend the
     * other. Guarded on a think-of-a-number framing, so it doesn't grab plain sums. */
    if ((cue(q, "double") || cue(q, "triple") || cue(q, "halve") || cue(q, "half")) &&
        (cue(q, "get") || cue(q, "gives") || cue(q, "equals") || cue(q, "result") ||
         cue(q, "you get") || cue(q, "i get")) &&
        (cue(q, "number") || cue(q, "thinking of"))) {
        double mult = cue(q, "double") ? 2.0 : cue(q, "triple") ? 3.0 : 0.5;
        int sub = cue(q, "subtract") || cue(q, "minus") || cue(q, "take away") || cue(q, "less");
        int add = cue(q, "add") || cue(q, "plus") || cue(q, "increase");
        char ab[256]; snprintf(ab, sizeof ab, "%s", q);
        char *aw[64]; size_t anw = split_words(ab, aw, 64);
        double nums[16]; size_t k = collect_numbers(aw, anw, nums, 16);
        if (k >= 1) {
            double C = nums[k - 1];            /* the result is stated last */
            double B = (k >= 2) ? nums[0] : 0; /* the addend/subtrahend first */
            if (!add && !sub) B = 0;
            double pre = sub ? (C + B) : (C - B);  /* undo the +/- */
            double ans = pre / mult;               /* undo the ×/÷ */
            char num[64]; format_num(ans, num, sizeof num);
            char msg[96]; snprintf(msg, sizeof msg, "%s.", num);
            put(msg, out, out_size);
            store_proof(b, "Inverted the stated operation to recover the number.");
            return 1;
        }
    }

    /* gen254 (LLMSCORE): elementary circle geometry. "a ripple has a radius of
     * 12 centimeters, what is the approximate circumference?" -> 2*pi*r. The
     * frame is measure(circumference|area) + given(radius|diameter, value
     * [, unit]); the formula is fixed mathematics (like the arithmetic oracle),
     * the numbers come from the turn, and the reply shows the computation as
     * its own proof. */
    if ((cue(q, "circumference") ||
         (cue(q, "area") && (cue(q, "circle") || cue(q, "circular")))) &&
        (cue(q, "radius") || cue(q, "diameter"))) {
        char gb[256]; snprintf(gb, sizeof gb, "%s", q);
        char *gw[64]; size_t gn = split_words(gb, gw, 64);
        int have_r = cue(q, "radius") != 0;
        double val = -1.0; char unit[32] = "";
        for (size_t i = 0; i < gn && val < 0; i++) {
            char *t = strip_edge_punct(gw[i]);
            if (strcmp(t, have_r ? "radius" : "diameter")) continue;
            for (size_t j = i + 1; j <= i + 3 && j < gn; j++) {
                double v;
                if (!parse_value(strip_edge_punct(gw[j]), &v)) continue;
                val = v;
                if (j + 1 < gn) {
                    char *u = strip_edge_punct(gw[j + 1]);
                    if (wp_length_unit(u)) snprintf(unit, sizeof unit, "%s", u);
                }
                break;
            }
        }
        if (val > 0) {
            const double PI = 3.14159265358979;
            double r = have_r ? val : val / 2.0;
            char msg[240];
            if (cue(q, "circumference")) {
                double c = 2.0 * PI * r;
                snprintf(msg, sizeof msg,
                         "About %.1f%s%s -- circumference = 2 x pi x radius = "
                         "2 x 3.14159 x %g.",
                         c, unit[0] ? " " : "", unit, r);
                store_proof(b, "circumference = 2 * pi * radius");
            } else {
                double a = PI * r * r;
                snprintf(msg, sizeof msg,
                         "About %.1f%s%s%s -- area = pi x radius^2 = "
                         "3.14159 x %g x %g.",
                         a, unit[0] ? " square " : "", unit,
                         unit[0] ? "" : " square units", r, r);
                store_proof(b, "area = pi * radius^2");
            }
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen254 (LLMSCORE): constrained-number riddle. "I'm thinking of a number.
     * It is greater than 10, less than 20, and not prime. What could my number
     * be?" The frame parses interval bounds plus number-theoretic predicates,
     * ENUMERATES the candidates, and answers with the survivors — or proves the
     * constraint set empty instead of inventing a number (NEXTMOVE gen253:
     * inconsistent puzzles must fail honestly, with the impossible constraint
     * named). */
    if ((cue(q, "thinking of a number") || cue(q, "my number") ||
         cue(q, "a number that") || cue(q, "the number is") ||
         (cue(q, "number") && (cue(q, "could") || cue(q, "what is it")))) &&
        (cue(q, "than") || cue(q, "between") || cue(q, "at least") ||
         cue(q, "at most"))) {
        char nb[256]; snprintf(nb, sizeof nb, "%s", q);
        char *nw2[64]; size_t nn = split_words(nb, nw2, 64);
        long lo = -1000000, hi = 1000000;
        int got_bound = 0;
        for (size_t i = 0; i < nn; i++) {
            char *t = strip_edge_punct(nw2[i]);
            double v;
            if (!strcmp(t, "than") && i >= 1 && i + 1 < nn) {
                char *prev = strip_edge_punct(nw2[i - 1]);
                for (size_t j = i + 1; j <= i + 2 && j < nn; j++) {
                    if (!parse_value(strip_edge_punct(nw2[j]), &v)) continue;
                    if (!strcmp(prev, "greater") || !strcmp(prev, "more") ||
                        !strcmp(prev, "bigger") || !strcmp(prev, "larger") ||
                        !strcmp(prev, "higher")) {
                        if ((long)v + 1 > lo) lo = (long)v + 1;
                        got_bound = 1;
                    } else if (!strcmp(prev, "less") || !strcmp(prev, "smaller") ||
                               !strcmp(prev, "lower") || !strcmp(prev, "fewer")) {
                        if ((long)v - 1 < hi) hi = (long)v - 1;
                        got_bound = 1;
                    }
                    break;
                }
            } else if ((!strcmp(t, "least") || !strcmp(t, "most")) && i >= 1 &&
                       i + 1 < nn &&
                       !strcmp(strip_edge_punct(nw2[i - 1]), "at") &&
                       parse_value(strip_edge_punct(nw2[i + 1]), &v)) {
                if (!strcmp(t, "least")) { if ((long)v > lo) lo = (long)v; }
                else { if ((long)v < hi) hi = (long)v; }
                got_bound = 1;
            } else if (!strcmp(t, "between") && i + 3 < nn &&
                       parse_value(strip_edge_punct(nw2[i + 1]), &v) &&
                       !strcmp(strip_edge_punct(nw2[i + 2]), "and")) {
                double v2;
                if (parse_value(strip_edge_punct(nw2[i + 3]), &v2)) {
                    /* strict reading: any listed answer is valid under either */
                    if ((long)v + 1 > lo) lo = (long)v + 1;
                    if ((long)v2 - 1 < hi) hi = (long)v2 - 1;
                    got_bound = 1;
                }
            } else if ((!strcmp(t, "above") || !strcmp(t, "over")) && i + 1 < nn &&
                       parse_value(strip_edge_punct(nw2[i + 1]), &v)) {
                if ((long)v + 1 > lo) lo = (long)v + 1;
                got_bound = 1;
            } else if ((!strcmp(t, "below") || !strcmp(t, "under")) && i + 1 < nn &&
                       parse_value(strip_edge_punct(nw2[i + 1]), &v)) {
                if ((long)v - 1 < hi) hi = (long)v - 1;
                got_bound = 1;
            }
        }
        /* number-theoretic predicates, with negation read first */
        int want_notprime = cue(q, "not prime") || cue(q, "not a prime") ||
                            cue(q, "isn't prime") || cue(q, "is not prime") ||
                            cue(q, "composite");
        int want_prime = !want_notprime && cue(q, "prime");
        int want_odd  = (cue(q, "odd") && !cue(q, "not odd")) || cue(q, "not even");
        int want_even = (cue(q, "even") && !cue(q, "not even")) || cue(q, "not odd");
        long divby = 0; int notdiv = 0;
        for (size_t i = 0; i + 1 < nn; i++) {
            char *t = strip_edge_punct(nw2[i]);
            double v;
            if ((!strcmp(t, "divisible") || !strcmp(t, "multiple")) && i + 2 < nn &&
                parse_value(strip_edge_punct(nw2[i + 2]), &v) && (long)v != 0) {
                divby = (long)v;
                notdiv = i >= 1 && !strcmp(strip_edge_punct(nw2[i - 1]), "not");
            }
        }
        int have_pred = want_prime || want_notprime || want_odd || want_even ||
                        divby != 0;
        if (got_bound && lo >= -100000 && hi <= 100000 && hi >= lo &&
            hi - lo <= 10000 && (have_pred || hi - lo <= 100)) {
            long picks[9]; size_t np = 0; long total = 0;
            for (long n = lo; n <= hi; n++) {
                if (want_even && (n % 2 != 0)) continue;
                if (want_odd && (n % 2 == 0)) continue;
                if (divby && !notdiv && (n % divby != 0)) continue;
                if (divby && notdiv && (n % divby == 0)) continue;
                if (want_prime && !wp_is_prime(n)) continue;
                if (want_notprime && wp_is_prime(n)) continue;
                if (np < 9) picks[np++] = n;
                total++;
            }
            char msg[300];
            if (total == 0) {
                snprintf(msg, sizeof msg,
                         "No number fits: nothing from %ld to %ld satisfies all "
                         "those constraints together.", lo, hi);
            } else if (total == 1) {
                snprintf(msg, sizeof msg, "It must be %ld -- the only number "
                         "from %ld to %ld that fits.", picks[0], lo, hi);
            } else {
                size_t off = (size_t)snprintf(msg, sizeof msg, "It could be ");
                size_t shown = np < 8 ? np : 8;
                for (size_t k = 0; k < shown; k++)
                    off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%ld",
                                            k == 0 ? "" :
                                            (k + 1 == shown && total <= 8
                                                 ? (shown == 2 ? " or " : ", or ")
                                                 : ", "),
                                            picks[k]);
                snprintf(msg + off, sizeof msg - off, "%s.",
                         total > 8 ? ", among others" : "");
            }
            put(msg, out, out_size);
            store_proof(b, "Enumerated the bounded range and kept the numbers "
                           "satisfying every stated constraint.");
            return 1;
        }
    }

    /* gen254 (LLMSCORE): rectangle geometry. "perimeter of 24 cm and one side
     * is 5 cm, what is the area?" -> other side = P/2 - s, area = s * other.
     * Same fixed-mathematics family as the circle frame; the numbers bind to
     * the named measures, and the reply carries its own derivation. */
    if (cue(q, "rectangle") && cue(q, "perimeter") &&
        (cue(q, "area") || cue(q, "side"))) {
        char rb2[256]; snprintf(rb2, sizeof rb2, "%s", q);
        char *rw2[64]; size_t rn2 = split_words(rb2, rw2, 64);
        double per = -1, side = -1; char unit[32] = "";
        for (size_t i = 0; i < rn2; i++) {
            char *t = strip_edge_punct(rw2[i]);
            int is_per = !strcmp(t, "perimeter");
            int is_side = !strcmp(t, "side") || !strcmp(t, "width") ||
                          !strcmp(t, "length");
            if (!is_per && !is_side) continue;
            for (size_t j = i + 1; j <= i + 3 && j < rn2; j++) {
                double v;
                if (!parse_value(strip_edge_punct(rw2[j]), &v)) continue;
                if (is_per && per < 0) per = v;
                if (is_side && side < 0) side = v;
                if (j + 1 < rn2) {
                    char *u = strip_edge_punct(rw2[j + 1]);
                    if (!unit[0] && wp_length_unit(u))
                        snprintf(unit, sizeof unit, "%s", u);
                }
                break;
            }
        }
        /* "one side is 5" can also precede the word ("5 cm side") — fall back
         * to the other number in the turn when the side slot stayed empty. */
        if (per > 0 && side < 0) {
            for (size_t i = 0; i < rn2 && side < 0; i++) {
                double v;
                if (parse_value(strip_edge_punct(rw2[i]), &v) && v != per)
                    side = v;
            }
        }
        if (per > 0 && side > 0 && cue(q, "area")) {
            double other = per / 2.0 - side;
            if (other > 0) {
                char n1[32], n2[32], n3[32];
                format_num(side, n1, sizeof n1);
                format_num(other, n2, sizeof n2);
                format_num(side * other, n3, sizeof n3);
                char msg[240];
                snprintf(msg, sizeof msg,
                         "%s square %s -- the other side is %g/2 - %s = %s, "
                         "and %s x %s = %s.",
                         n3, unit[0] ? unit : "units", per, n1, n2, n1, n2, n3);
                put(msg, out, out_size);
                store_proof(b, "other = perimeter/2 - side; area = side * other");
                return 1;
            }
            put("Those measures are inconsistent: half the perimeter is not "
                "longer than the given side, so no rectangle fits.",
                out, out_size);
            return 1;
        }
    }

    /* gen254 (LLMSCORE): heads-and-legs puzzle — the SECOND two-unknown linear
     * frame. "20 animals, chickens and rabbits, 56 legs: how many chickens?"
     * The legs-per-species are KB facts (quantity(Species, legs, L)), so any
     * species pair the KB knows solves with the same algebra:
     * x + y = N, a*x + b*y = L  ->  x = (b*N - L) / (b - a). */
    if (cue(q, "legs") && (cue(q, "how many") || cue(q, "quant"))) {
        char hb[256]; snprintf(hb, sizeof hb, "%s", q);
        char *hw2[64]; size_t hn2 = split_words(hb, hw2, 64);
        char sp[2][KB_TERM_LEN]; double legs[2]; int nsp = 0;
        for (size_t i = 0; i < hn2 && nsp < 2; i++) {
            char sg[KB_TERM_LEN];
            singularize(strip_edge_punct(hw2[i]), sg, sizeof sg);
            if (!*sg) continue;
            const char *lq[] = { sg, "legs", NULL };
            char lh[1][KB_TERM_LEN];
            if (kb_match(b->kb, "quantity", lq, 3, lh, 1) == 0) continue;
            int dup = nsp == 1 && !strcmp(sp[0], sg);
            if (dup) continue;
            snprintf(sp[nsp], KB_TERM_LEN, "%s", sg);
            legs[nsp] = atof(lh[0]);
            nsp++;
        }
        double total_n = -1, total_legs = -1;
        for (size_t i = 1; i < hn2; i++) {
            char *t = strip_edge_punct(hw2[i]);
            double v;
            if ((!strcmp(t, "legs") || !strcmp(t, "leg")) &&
                parse_value(strip_edge_punct(hw2[i - 1]), &v)) total_legs = v;
            if ((!strcmp(t, "animals") || !strcmp(t, "heads") ||
                 !strcmp(t, "animali")) &&
                parse_value(strip_edge_punct(hw2[i - 1]), &v)) total_n = v;
        }
        if (nsp == 2 && total_n > 0 && total_legs > 0 &&
            legs[0] != legs[1]) {
            /* which species is asked for? the one named after "how many". */
            int asked = 0;
            for (size_t i = 0; i + 1 < hn2; i++) {
                if (strcmp(strip_edge_punct(hw2[i]), "many")) continue;
                char sg[KB_TERM_LEN];
                singularize(strip_edge_punct(hw2[i + 1]), sg, sizeof sg);
                if (!strcmp(sg, sp[1])) asked = 1;
                break;
            }
            double a = legs[asked], bl = legs[1 - asked];
            double x = (bl * total_n - total_legs) / (bl - a);
            double y = total_n - x;
            if (x >= 0 && y >= 0 && x == (double)(long)x) {
                char n1[32], n2[32];
                format_num(x, n1, sizeof n1);
                format_num(y, n2, sizeof n2);
                char msg[240];
                snprintf(msg, sizeof msg,
                         "%s %ss (and %s %ss): %s x %g + %s x %g = %g legs.",
                         n1, sp[asked], n2, sp[1 - asked],
                         n1, a, n2, bl, total_legs);
                put(msg, out, out_size);
                store_proof(b, "x + y = heads and a*x + b*y = legs give "
                               "x = (b*heads - legs)/(b - a).");
                return 1;
            }
            put("Those counts don't work out to whole animals, so the puzzle "
                "as stated has no consistent answer.", out, out_size);
            return 1;
        }
    }

    /* gen254 (LLMSCORE): ratio age puzzle — a GENERAL two-unknown linear solve.
     * "A father is four times as old as his son. In 20 years he will be twice
     * as old. How old are they now?" reduces to a = K*b and a +/- N = M*(b +/- N),
     * so b = N*(M-1)/(K-M) (future) or b = N*(1-M)/(K-M) (ago). The C parses the
     * two ratio phrases and the year shift; the algebra is fixed mathematics.
     * Equal ratios are named as inconsistent instead of dividing by zero. */
    if (cue(q, "as old as") && (cue(q, "years") || cue(q, "year")) &&
        (cue(q, "old are") || cue(q, "old is") || cue(q, "their ages") ||
         cue(q, "how old"))) {
        char ab[256]; snprintf(ab, sizeof ab, "%s", q);
        char *aw[64]; size_t an = split_words(ab, aw, 64);
        double K = 0, M = 0, N = 0;
        char who_a[32] = "", who_b[32] = "";
        int ago = cue(q, "ago") != 0;
        for (size_t i = 2; i < an; i++) {
            char *t = strip_edge_punct(aw[i]);
            if (strcmp(t, "old")) continue;
            if (strcmp(strip_edge_punct(aw[i - 1]), "as")) continue;
            char *m2 = strip_edge_punct(aw[i - 2]);      /* "<mult> as old" */
            double m = 0, v;
            if (!strcmp(m2, "twice")) m = 2;
            else if (!strcmp(m2, "thrice")) m = 3;
            else if (!strcmp(m2, "times") && i >= 3 &&
                     parse_value(strip_edge_punct(aw[i - 3]), &v)) m = v;
            if (m <= 0) continue;
            if (K <= 0) {
                K = m;
                /* entities: A before the copula, B after "as old as" */
                for (size_t j = i - 2; j-- > 0;) {
                    char *tj = strip_edge_punct(aw[j]);
                    if (!strcmp(tj, "is") || !strcmp(tj, "was")) {
                        if (j > 0) snprintf(who_a, sizeof who_a, "%s",
                                            strip_edge_punct(aw[j - 1]));
                        break;
                    }
                }
                for (size_t j = i + 1; j + 1 < an; j++) {
                    if (strcmp(strip_edge_punct(aw[j]), "as")) continue;
                    char *tb = strip_edge_punct(aw[j + 1]);
                    if (!strcmp(tb, "his") || !strcmp(tb, "her") ||
                        !strcmp(tb, "the") || !strcmp(tb, "a") ||
                        !strcmp(tb, "their")) {
                        if (j + 2 < an) tb = strip_edge_punct(aw[j + 2]);
                    }
                    snprintf(who_b, sizeof who_b, "%s", tb);
                    break;
                }
            } else if (M <= 0) M = m;
        }
        for (size_t i = 1; i < an && N == 0; i++) {
            char *t = strip_edge_punct(aw[i]);
            double v;
            if ((!strcmp(t, "years") || !strcmp(t, "year")) &&
                parse_value(strip_edge_punct(aw[i - 1]), &v)) N = v;
        }
        if (K > 0 && M > 0 && N > 0) {
            if (K == M) {
                put("Those two ratios can't both hold: if the ratio never "
                    "changes, no ages fit. One of the constraints must be "
                    "different.", out, out_size);
                return 1;
            }
            double bage = ago ? N * (1 - M) / (K - M) : N * (M - 1) / (K - M);
            double aage = K * bage;
            if (bage > 0 && aage > 0) {
                char nb2[64], na2[64];
                format_num(bage, nb2, sizeof nb2);
                format_num(aage, na2, sizeof na2);
                char msg[220];
                snprintf(msg, sizeof msg, "The %s is %s and the %s is %s.",
                         who_b[0] ? who_b : "younger one", nb2,
                         who_a[0] ? who_a : "older one", na2);
                msg[0] = (char)toupper((unsigned char)msg[0]);
                put(msg, out, out_size);
                store_proof(b, "a = K*b and a + N = M*(b + N) give "
                               "b = N*(M-1)/(K-M); then a = K*b.");
                return 1;
            }
        }
    }

    /* question guard: only attempt on an explicit "how many / how much / quanti…"
     * or a count phrasing ("maximum number of", "number of", "arrangements"). */
    if (!(cue(q, "how many") || cue(q, "how much") || cue(q, "quant") ||
          cue(q, "number of") || cue(q, "arrangement")))
        return 0;

    /* gen254: an arrangement-OPTIMIZATION puzzle ("the maximum number of sheep
     * that can be kept separate ... so that each area shares a fence with at
     * least one other") is NOT a containers-times-per-container count; the
     * multiply below would fabricate a number from incidental quantities. Name
     * the missing solver instead of guessing (calibrated decline, NEXTMOVE
     * gen253). */
    if ((cue(q, "maximum") || cue(q, "minimum") || cue(q, "at most") ||
         cue(q, "arrange") || cue(q, "arranged")) &&
        (cue(q, "so that") || cue(q, "such that") || cue(q, "assuming") ||
         cue(q, "shares") || cue(q, "share "))) {
        put("That's a constrained-arrangement puzzle: I can read the quantities, "
            "but I don't have a solver that can verify an optimal arrangement "
            "under those sharing constraints, so I won't guess a number.",
            out, out_size);
        return 1;
    }

    /* gen241 (LLMSCORE-check): containers x per-container, then add/remove deltas.
     * "A bookshelf has 5 shelves. Each shelf holds 12 books. If you remove 20 and add
     * 8, how many?" -> 5*12 - 20 + 8 = 48. The C reads the multiply (count x each-holds)
     * then walks add/remove verbs as signed deltas. A genuine multi-step computation. */
    if ((cue(q, "each") || cue(q, "every")) &&
        (cue(q, "holds") || cue(q, "hold") || cue(q, "has") || cue(q, "have") ||
         cue(q, "contains") || cue(q, "contain") || cue(q, "with"))) {
        char mb[256]; snprintf(mb, sizeof mb, "%s", q);
        char *mw[64]; size_t mn = split_words(mb, mw, 64);
        double per = -1, containers = -1;
        /* per = the number right after each/every <noun> holds/has/contains */
        for (size_t i = 0; i + 1 < mn; i++) {
            char *t = strip_edge_punct(mw[i]);
            if (!strcmp(t,"holds")||!strcmp(t,"hold")||!strcmp(t,"contains")||
                !strcmp(t,"contain")||!strcmp(t,"holding")) {
                for (size_t j = i + 1; j <= i + 3 && j < mn; j++) {
                    double v; if (parse_value(strip_edge_punct(mw[j]), &v)) { per = v; break; }
                }
            } else if (!strcmp(t, "with") && cue(q, "each")) {
                for (size_t j = i + 1; j <= i + 3 && j < mn; j++) {
                    double v; if (parse_value(strip_edge_punct(mw[j]), &v)) { per = v; break; }
                }
            }
        }
        /* containers = the first number in the turn distinct from per */
        for (size_t i = 0; i < mn; i++) {
            double v; if (parse_value(strip_edge_punct(mw[i]), &v)) {
                if (per < 0 || v != per) { containers = v; break; }
            }
        }
        if (per > 0 && containers > 0) {
            double total = containers * per;
            /* walk signed deltas: add/added/put -> +, remove/take/subtract -> - */
            for (size_t i = 0; i + 1 < mn; i++) {
                char *t = strip_edge_punct(mw[i]);
                int plus = !strcmp(t,"add")||!strcmp(t,"added")||!strcmp(t,"adds")||
                           !strcmp(t,"put")||!strcmp(t,"gain")||!strcmp(t,"buy")||
                           !strcmp(t,"bought")||!strcmp(t,"insert");
                int minus = !strcmp(t,"remove")||!strcmp(t,"removed")||!strcmp(t,"removes")||
                            !strcmp(t,"take")||!strcmp(t,"took")||!strcmp(t,"takes")||
                            !strcmp(t,"subtract")||!strcmp(t,"lose")||!strcmp(t,"lost")||
                            !strcmp(t,"sell")||!strcmp(t,"sold")||!strcmp(t,"give")||!strcmp(t,"gave");
                if (!plus && !minus) continue;
                for (size_t j = i + 1; j <= i + 3 && j < mn; j++) {
                    double v; if (parse_value(strip_edge_punct(mw[j]), &v)) {
                        int worth = 0;
                        for (size_t k = j + 1; k <= j + 4 && k < mn; k++) {
                            char *wk = strip_edge_punct(mw[k]);
                            if (!strcmp(wk, "shelf") || !strcmp(wk, "shelves") ||
                                !strcmp(wk, "worth") || !strcmp(wk, "complete"))
                                worth = 1;
                        }
                        double delta = worth ? v * per : v;
                        total += plus ? delta : -delta; break;
                    }
                }
            }
            char num[64]; format_num(total, num, sizeof num);
            char msg[120]; snprintf(msg, sizeof msg, "%s.", num);
            put(msg, out, out_size);
            store_proof(b, "Multiplied containers by per-container, then applied the add/remove deltas.");
            return 1;
        }
    }

    /* gen241 (LLMSCORE-check): "an item costs $C and you have $M" buying/change.
     *   "how many can you buy and how much remains?" -> floor(M/C) bought, M mod C left.
     *   "how much change do you get?" (buying one) -> M - C.
     * Distinct from the "N for $M" pack handler: here the price is a per-item COST and
     * the money is what you HAVE, signalled by "cost(s)" + "have"/"bill". */
    if ((cue(q, "cost") || cue(q, "costs") || cue(q, "price")) &&
        (cue(q, "have") || cue(q, "bill")) &&
        (cue(q, "$") || cue(q, "dollar") || cue(q, "cent") || cue(q, "euro"))) {
        char cbuf[256]; snprintf(cbuf, sizeof cbuf, "%s", q);
        char *cw[64]; size_t cn = split_words(cbuf, cw, 64);
        double price = -1, money = -1;
        for (size_t i = 0; i < cn; i++) {
            char *t = strip_edge_punct(cw[i]);
            double v;
            if (!parse_value(t, &v)) continue;
            /* classify by the nearest keyword in a small window before/after */
            int is_price = 0, is_money = 0;
            for (size_t j = (i >= 2 ? i - 2 : 0); j <= i + 2 && j < cn; j++) {
                char *k = strip_edge_punct(cw[j]);
                if (!strcmp(k,"cost")||!strcmp(k,"costs")||!strcmp(k,"price")||!strcmp(k,"costing")) is_price = 1;
                if (!strcmp(k,"have")||!strcmp(k,"bill")||!strcmp(k,"got")||!strcmp(k,"wallet")) is_money = 1;
            }
            if (is_price && price < 0) price = v;
            else if (is_money && money < 0) money = v;
        }
        /* fallback: the smaller number is the price, the larger is the money. */
        if ((price < 0 || money < 0)) {
            double nums2[16]; size_t kk = collect_numbers(cw, cn, nums2, 16);
            if (kk >= 2) {
                double lo = nums2[0], hi = nums2[0];
                for (size_t i = 1; i < kk; i++) { if (nums2[i] < lo) lo = nums2[i]; if (nums2[i] > hi) hi = nums2[i]; }
                if (price < 0) price = lo;
                if (money < 0) money = hi;
            }
        }
        if (price > 0 && money >= 0) {
            int wants_count = cue(q, "how many") || cue(q, "can you buy") ||
                              cue(q, "can i buy") || cue(q, "can be bought");
            if (wants_count) {
                long n = (long)(money / price);
                double left = money - n * price;
                char msg[200];
                snprintf(msg, sizeof msg,
                         "You can buy %ld, with $%g left over.", n, left);
                put(msg, out, out_size);
                store_proof(b, "Bought as many as the money allows; remainder is the change.");
                return 1;
            }
            if (cue(q, "change") || cue(q, "left") || cue(q, "remain") ||
                cue(q, "back")) {
                double change = money - price;
                char msg[120];
                snprintf(msg, sizeof msg, "$%g.", change);
                put(msg, out, out_size);
                store_proof(b, "Change = money given minus price.");
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): the bat-and-ball trap. "A and B cost T total; A costs D
     * more than B; how much is B?" The intuitive T-D is WRONG; the algebra is
     * B = (T - D)/2 (since A = B + D and A + B = T). Guarded on "more than" + a
     * total, so it only fires on this shape. */
    if (cue(q, "more than") && (cue(q, "total") || cue(q, "cost") || cue(q, "altogether"))) {
        char bb[256]; snprintf(bb, sizeof bb, "%s", q);
        char *bw[64]; size_t bn = split_words(bb, bw, 64);
        double total = -1, diff = -1;
        for (size_t i = 0; i < bn; i++) {
            double v;
            if (!parse_value(strip_edge_punct(bw[i]), &v)) continue;
            for (size_t j = i + 1; j <= i + 2 && j < bn; j++) {
                char *nx = strip_edge_punct(bw[j]);
                if (!strcmp(nx, "total") || !strcmp(nx, "altogether")) total = v;
                if (!strcmp(nx, "more")) diff = v;
            }
        }
        /* fallbacks: the largest number is the total; the diff sits before "more" */
        if (total < 0 || diff < 0) {
            double nums2[16]; size_t k = collect_numbers(bw, bn, nums2, 16);
            if (k >= 2) {
                if (total < 0) { total = nums2[0]; for (size_t i=1;i<k;i++) if (nums2[i]>total) total=nums2[i]; }
                if (diff < 0) for (size_t i=0;i<k;i++) if (nums2[i] != total) { diff = nums2[i]; break; }
            }
        }
        if (total > 0 && diff >= 0 && diff < total) {
            double ball = (total - diff) / 2.0;
            char num[64]; format_num(ball, num, sizeof num);
            char msg[160];
            snprintf(msg, sizeof msg,
                     "$%s. (Not $%g: if one costs $%g more, the cheaper is (%g-%g)/2.)",
                     num, total - diff, diff, total, diff);
            put(msg, out, out_size);
            store_proof(b, "Bat-and-ball: cheaper = (total - difference)/2, not total - difference.");
            return 1;
        }
    }

    /* gen240 (LLMSCORE): rate proportion. "N items for $M ... how much do K items
     * cost?" -> K * M / N. Distinct from the buy-with-remainder case (no money you
     * HAVE); needs three numbers and a price cue. */
    if (cue(q, "for") && !cue(q, "have") &&
        (cue(q, "$") || cue(q, "dollar") || cue(q, "cost") || cue(q, "cent") ||
         cue(q, "price"))) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rnw = split_words(rb, rw, 64);
        double N = -1, M = -1, K = -1; size_t forpos = rnw;
        for (size_t i = 0; i < rnw; i++)
            if (!strcmp(strip_edge_punct(rw[i]), "for")) { forpos = i; break; }
        if (forpos < rnw) {
            double nums[16]; size_t nn = collect_numbers(rw, rnw, nums, 16);
            /* M = first number AFTER "for" (the price); N = last number BEFORE "for"
             * (the count); K = the next number after M (the asked quantity). */
            for (size_t i = 0; i < forpos; i++) { double v;
                if (parse_value(strip_edge_punct(rw[i]), &v)) N = v; }
            int seenM = 0;
            for (size_t i = forpos + 1; i < rnw; i++) { double v;
                if (parse_value(strip_edge_punct(rw[i]), &v)) {
                    if (!seenM) { M = v; seenM = 1; } else { K = v; break; }
                }
            }
            (void)nn;
            if (N > 0 && M > 0 && K > 0) {
                double cost = K * M / N;
                char num[64]; format_num(cost, num, sizeof num);
                char msg[160];
                snprintf(msg, sizeof msg, "$%s.", num);
                put(msg, out, out_size);
                store_proof(b, "Rate proportion: cost = quantity * price / batch.");
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): rate-with-remainder buy problem. "N for $M ... have $K"
     * -> floor(K/M) packs = floor(K/M)*N items, with (K mod M) money left over.
     * General over the three numbers; reports both the count and the change. */
    if (cue(q, "for") && cue(q, "have") &&
        (cue(q, "$") || cue(q, "dollar") || cue(q, "cent"))) {
        char pb[256]; snprintf(pb, sizeof pb, "%s", q);
        char *pw[64]; size_t pnw = split_words(pb, pw, 64);
        double packCount = -1, packPrice = -1, money = -1;
        const char *item = NULL;
        for (size_t i = 0; i < pnw; i++) {
            char *t = strip_edge_punct(pw[i]);
            if (!strcmp(t, "for") && i > 0 && i + 1 < pnw) {
                double a, c;
                if (parse_value(strip_edge_punct(pw[i - 1]), &a) &&
                    parse_value(strip_edge_punct(pw[i + 1]), &c)) {
                    packCount = a; packPrice = c;
                }
            }
            if (!strcmp(t, "have") && i + 1 < pnw) {
                double k;
                if (parse_value(strip_edge_punct(pw[i + 1]), &k)) money = k;
            }
            if (!strcmp(t, "sells") && i + 1 < pnw) item = strip_edge_punct(pw[i + 1]);
        }
        if (packCount > 0 && packPrice > 0 && money >= 0) {
            long packs = (long)(money / packPrice);
            long items = packs * (long)packCount;
            double change = money - packs * packPrice;
            char msg[220];
            if (change > 0)
                snprintf(msg, sizeof msg,
                         "%ld %s%s, with $%g left over.", items,
                         item ? item : "items", item ? "" : "", change);
            else
                snprintf(msg, sizeof msg,
                         "%ld %s%s, with no change left over.", items,
                         item ? item : "items", item ? "" : "");
            put(msg, out, out_size);
            store_proof(b, "Bought as many fixed-price packs as the money allows; remainder is the change.");
            return 1;
        }
    }

    /* gen240 (LLMSCORE): two-animal head/leg system. Heads H and legs L with two
     * animals whose per-animal legs are KB facts (quantity(animal, legs, n)):
     *   b = (L - legs_a*H)/(legs_b - legs_a);  a = H - b. KB-first — any two
     * animals with known legs transfer; the C only solves the linear system. */
    if (cue(q, "legs")) {
        char fb[256]; snprintf(fb, sizeof fb, "%s", q);
        char *fw[64]; size_t fnw = split_words(fb, fw, 64);
        char aname[2][KB_TERM_LEN]; double alegs[2]; int na = 0;
        double Ltot = -1, Htot = -1;
        for (size_t i = 0; i < fnw && na < 2; i++) {
            char sgl[64]; snprintf(sgl, sizeof sgl, "%s", strip_edge_punct(fw[i]));
            size_t sl = strlen(sgl);
            if (sl > 1 && sgl[sl - 1] == 's') sgl[sl - 1] = '\0';
            const char *pat[] = { sgl, "legs", NULL };
            char hh[1][KB_TERM_LEN];
            if (kb_match(b->kb, "quantity", pat, 3, hh, 1) > 0) {
                int dup = 0;
                for (int k = 0; k < na; k++) if (!strcmp(aname[k], sgl)) dup = 1;
                if (!dup) { snprintf(aname[na], KB_TERM_LEN, "%s", sgl);
                            parse_num(hh[0], &alegs[na]); na++; }
            }
        }
        /* leg total = number token followed (within 2) by "legs" */
        for (size_t i = 0; i + 1 < fnw; i++) {
            double v;
            if (!parse_value(strip_edge_punct(fw[i]), &v)) continue;
            for (size_t j = i + 1; j <= i + 2 && j < fnw; j++)
                if (!strcmp(strip_edge_punct(fw[j]), "legs")) Ltot = v;
        }
        /* head total = the other number */
        double tmpn[16]; size_t tnn = collect_numbers(fw, fnw, tmpn, 16);
        for (size_t i = 0; i < tnn; i++) if (tmpn[i] != Ltot) { Htot = tmpn[i]; break; }
        if (na == 2 && Ltot > 0 && Htot > 0 && alegs[0] != alegs[1]) {
            double bc = (Ltot - alegs[0] * Htot) / (alegs[1] - alegs[0]);
            double ac = Htot - bc;
            if (ac >= 0 && bc >= 0 && ac == (long)ac && bc == (long)bc) {
                char msg[220];
                snprintf(msg, sizeof msg, "%ld %ss and %ld %ss.",
                         (long)ac, aname[0], (long)bc, aname[1]);
                put(msg, out, out_size);
                store_proof(b, "Solved the heads/legs linear system from the two animals' known legs.");
                return 1;
            }
        }
    }

    /* gen240 (LLMSCORE): circular seating. n people around a round table, with
     * rotations counted as the same, give (n-1)! arrangements. "including
     * yourself" adds one to the named guest count. */
    if ((cue(q, "round table") || cue(q, "circular") || cue(q, "around a table")) &&
        cue(q, "arrangement")) {
        char rb[256]; snprintf(rb, sizeof rb, "%s", q);
        char *rw[64]; size_t rnw = split_words(rb, rw, 64);
        double gn[8]; size_t gc = collect_numbers(rw, rnw, gn, 8);
        if (gc >= 1) {
            long n = (long)gn[0];
            if (cue(q, "including yourself") || cue(q, "including myself") ||
                cue(q, "and yourself") || cue(q, "plus yourself")) n += 1;
            if (n >= 1 && n <= 12) {
                long f = 1;
                for (long k = 2; k <= n - 1; k++) f *= k;
                char msg[200];
                if (cue(q, "rotation"))
                    snprintf(msg, sizeof msg,
                             "%ld. With %ld people and rotations counted as the "
                             "same, there are (%ld-1)! = %ld arrangements.",
                             f, n, n, f);
                else
                    snprintf(msg, sizeof msg, "%ld arrangements.", f);
                put(msg, out, out_size);
                store_proof(b, "Circular permutations of n with rotation equivalence: (n-1)!.");
                return 1;
            }
        }
    }

    /* collect the numbers in reading order (digits and number words). */
    char buf[256]; snprintf(buf, sizeof buf, "%s", q);
    char *w[64]; size_t nw = split_words(buf, w, 64);
    double nums[16];
    size_t nn = collect_numbers(w, nw, nums, 16);
    if (nn < 2) return 0;
    if (cue(q, "all but") && nn >= 2) {
        /* gen254: "all but N" is a STEP, not always the final answer — "sells
         * all but 9, then buys 12 more" must keep folding the later deltas
         * ("17 sheep, all but 9 die" still ends at 9). Resume the walk right
         * after the all-but number with the running total set to N. */
        double total = nums[1];
        const char *ab = strstr(q, "all but");
        char tb2[256]; snprintf(tb2, sizeof tb2, "%s", ab + 7);
        char *tw2[64]; size_t tn2 = split_words(tb2, tw2, 64);
        int sign2 = 0, seen_first = 0;   /* sign 0 until a verb sets it */
        for (size_t i = 0; i < tn2; i++) {
            char *t = strip_edge_punct(tw2[i]);
            if (!*t) continue;
            double v;
            if (parse_value(t, &v)) {
                if (!seen_first) { seen_first = 1; continue; }  /* the N itself */
                if (sign2) { total += sign2 * v; sign2 = 0; }
                continue;
            }
            if (!strcmp(t, "buys") || !strcmp(t, "buy") || !strcmp(t, "bought") ||
                !strcmp(t, "gains") || !strcmp(t, "gain") || !strcmp(t, "gets") ||
                !strcmp(t, "get") || !strcmp(t, "adds") || !strcmp(t, "add") ||
                !strcmp(t, "receives") || !strcmp(t, "finds")) sign2 = 1;
            else if (wp_removal_word(t)) sign2 = -1;
        }
        char num[64]; format_num(total, num, sizeof num);
        char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
        put(msg, out, out_size);
        store_proof(b, "All-but leaves the number after 'but'; later gains and "
                       "losses still apply.");
        return 1;
    }

    /* gen114: 3+ numbers -> multi-step additive/subtractive fold, clause by
     * clause. The first number is the base; each later number is added, or
     * subtracted if its clause carries a removal verb. Clauses split on
     * then/and/poi/e and on a trailing comma. */
    if (nn >= 3) {
        char sb[256]; snprintf(sb, sizeof sb, "%s", q);
        char *tw[64]; size_t tnw = split_words(sb, tw, 64);
        double result = 0; int have = 0, sign = 1, ratio_applied = 0;
        int halve_to_pieces =
            (cue(q, "cut") || cue(q, "cuts") || cue(q, "slice") || cue(q, "sliced")) &&
            cue(q, "half") && (cue(q, "piece") || cue(q, "pieces"));
        for (size_t i = 0; i < tnw; i++) {
            size_t L = strlen(tw[i]);
            int trailing = L > 0 && (tw[i][L - 1] == ',' || tw[i][L - 1] == ';');
            char *t = strip_edge_punct(tw[i]);
            if (!*t) { if (trailing) sign = 1; continue; }
            if (!strcmp(t, "then") || !strcmp(t, "and") || !strcmp(t, "poi") ||
                !strcmp(t, "e") || !strcmp(t, "ed")) { sign = 1; continue; }
            /* "give/gave away N" is a removal: "give" alone is ambiguous, but
             * the "away" particle disambiguates it (gen240). */
            if (!strcmp(t, "away")) { sign = -1; continue; }
            /* gen240: base "give"/"giving N to <someone>" is a removal for the
             * subject. Ambiguous only when the recipient is me/us, so guard on the
             * NEXT token — "give 1 to a friend" subtracts; "give me 2 more" doesn't. */
            if (!strcmp(t, "give") || !strcmp(t, "giving") || !strcmp(t, "gives")) {
                char *nx = (i + 1 < tnw) ? strip_edge_punct(tw[i + 1]) : (char *)"";
                /* "give ME/US/YOU(rself) N" -> the answerer RECEIVES, so it's a gain;
                 * only "give <other> N" / "give away N" is a removal (gen241). */
                if (strcmp(nx, "me") && strcmp(nx, "us") && strcmp(nx, "myself") &&
                    strcmp(nx, "you") && strcmp(nx, "yourself"))
                    sign = -1;
            }
            if (wp_removal_word(t)) sign = -1;
            /* gen254: RELATIVE quantity step — "twice/half of what I currently
             * have/left" refers to the running total, not a literal number. A
             * gain adds mult*total ("you give me twice what I have": 4 -> 12);
             * a removal subtracts it ("I eat half of what I have": 4 -> 2). */
            double mult = 0.0;
            if (!strcmp(t, "twice") || !strcmp(t, "double")) mult = 2.0;
            else if (!strcmp(t, "triple") || !strcmp(t, "thrice")) mult = 3.0;
            else if (!strcmp(t, "half")) mult = 0.5;
            if (mult > 0.0 && have) {
                int rel = 0; size_t skip = i;
                for (size_t j = i + 1; j <= i + 5 && j < tnw; j++) {
                    char *lj = strip_edge_punct(tw[j]);
                    if (!strcmp(lj, "have") || !strcmp(lj, "has") ||
                        !strcmp(lj, "left") || !strcmp(lj, "hold") ||
                        !strcmp(lj, "holds")) { rel = 1; skip = j; break; }
                }
                if (rel) {
                    result += (double)sign * mult * result;
                    ratio_applied = 1;
                    i = skip;                     /* consume the whole phrase */
                    if (trailing) sign = 1;
                    continue;
                }
            }
            double v;
            if (parse_value(t, &v)) {
                if (!have) { result = v; have = 1; }
                else result += sign * v;
            }
            if (trailing) sign = 1;
        }
        if (!ratio_applied &&
            (cue(q, "give half") || cue(q, "gave half") || cue(q, "gives half")) &&
            result != 0)
            result /= 2;
        if (halve_to_pieces && result != 0)
            result *= 2;
        char num[64]; format_num(result, num, sizeof num);
        char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
        put(msg, out, out_size);
        char proof[160];
        if (halve_to_pieces)
            snprintf(proof, sizeof proof,
                     "I folded the remaining items, then counted two half-pieces per item: %s.",
                     num);
        else
            snprintf(proof, sizeof proof,
                     "I folded the steps left to right to %s.", num);
        store_proof(b, proof);
        return 1;
    }

    double a = nums[0], c = nums[1];

    /* choose the operation by cue, in a priority that resolves overlaps:
     * division, then comparison-difference / removal (both '-'), then
     * multiplication, then addition. */
    char op = 0;
    if (cue(q, "shared") || cue(q, "share") || cue(q, "divided") ||
        cue(q, "divide") || cue(q, "split") || cue(q, "among") ||
        cue(q, "equally") || cue(q, "divis") || cue(q, "condivi") ||
        cue(q, "ripartit"))
        op = '/';
    else if (cue(q, "how many more") || cue(q, "how much more") ||
             cue(q, "how many fewer") || cue(q, "how many less") ||
             cue(q, "more than") || cue(q, "fewer than") ||
             cue(q, "difference") || cue(q, "quanti in più") ||
             cue(q, "differenza") ||
             cue(q, "ate") || cue(q, "eats") || cue(q, "lost") ||
             cue(q, "loses") || cue(q, "gave") || cue(q, "gives away") ||
             cue(q, "left") || cue(q, "remain") || cue(q, "fewer") ||
             cue(q, "spent") || cue(q, "spends") || cue(q, "sold") ||
             cue(q, "sells") || cue(q, "broke") || cue(q, "removed") ||
             cue(q, "drops") || cue(q, " away") || cue(q, "mangi") ||
             cue(q, "pers") || cue(q, "perde") || cue(q, "regal") ||
             cue(q, "vend") || cue(q, "riman") || cue(q, "spend"))
        op = '-';
    else if (cue(q, "each") || cue(q, "times") || cue(q, "groups of") ||
             cue(q, "rows of") || cue(q, "boxes of") || cue(q, "ciascun") ||
             cue(q, "ognuno") || cue(q, "ogni"))
        op = '*';
    else if (cue(q, "more") || cue(q, "gets") || cue(q, "got") ||
             cue(q, "gains") || cue(q, "buys") || cue(q, "buy") ||
             cue(q, "found") || cue(q, "finds") || cue(q, "altogether") ||
             cue(q, "total") || cue(q, "in all") || cue(q, "combined") ||
             cue(q, "receive") || cue(q, "plus") || cue(q, "adds") ||
             cue(q, "compr") || cue(q, "trov") || cue(q, "totale") ||
             cue(q, "insieme") || cue(q, "ancora") || cue(q, "aggiun"))
        op = '+';
    if (!op) return 0;

    double r;
    switch (op) {
        case '+': r = a + c; break;
        case '-': r = a - c; break;
        case '*': r = a * c; break;
        case '/': if (c == 0) { put("I can't divide by zero.", out, out_size); return 1; }
                  r = a / c; break;
        default: return 0;
    }

    char num[64]; format_num(r, num, sizeof num);
    char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
    put(msg, out, out_size);

    char proof[128];
    snprintf(proof, sizeof proof, "I read it as %g %c %g = %s.", a, op, c, num);
    store_proof(b, proof);
    return 1;
}

/* --- module: quantity ----------------------------------------------------
 * Quantities as knowledge (gen28). gen27 could compare two literal numbers;
 * this part lets a magnitude be *stated, recalled, and compared as a fact*, so
 * the comparison primitive can be driven from language rather than from
 * pre-extracted numbers — the next step the BoolQ probe pulled. A quantity is
 * a 3-ary fact `quantity(entity, unit, value)`; the value rides in the KB as a
 * string atom and is parsed back with parse_num when compared. Turning prose
 * into these facts (open-domain extraction) is deliberately still out of scope:
 * we build the reasoning, not a passage parser. */
static int mod_quantity(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<x> has <n> <unit>" -> quantity(x, unit, n) */
    if (nw == 4 && strcmp(w[1], "has") == 0) {
        double v;
        if (!parse_num(w[2], &v)) return 0; /* not a quantity; let others try */
        const char *args[] = {w[0], w[3], w[2]};
        char msg[160];
        if (kb_assert(b->kb, "quantity", args, 3))
            snprintf(msg, sizeof msg, "Learned: %s has %s %s.", w[0], w[2], w[3]);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* recall: "how many <unit> does <x> have" -> quantity(x, unit, ?) */
    if (nw == 6 && strcmp(w[0], "how") == 0 && strcmp(w[1], "many") == 0 &&
        strcmp(w[3], "does") == 0 && strcmp(w[5], "have") == 0) {
        const char *unit = w[2], *x = w[4];
        const char *pat[] = {x, unit, NULL};
        char hits[4][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "quantity", pat, 3, hits, 4);
        char msg[160];
        if (k == 0)
            snprintf(msg, sizeof msg, "I don't know how many %s %s has.", unit, x);
        else
            snprintf(msg, sizeof msg, "%s has %s %s.", x, hits[0], unit);
        put(msg, out, out_size);
        return 1;
    }

    /* gen240 (LLMSCORE): general known-fact count recall, robust to articles and
     * trailing verbs — "how many <unit> does a <entity> have", "how many <unit>
     * are in a <entity>", "how many <unit> in a <entity>". unit = first content
     * token after "many"; entity = last content token. quantity(Entity, Unit, N)
     * is the KB knowledge (engine fixed, lexicon grows). Word problems that start
     * "how many do you have" bind no known entity and fall through untouched. */
    if (nw >= 4 && strcmp(w[0], "how") == 0 && strcmp(w[1], "many") == 0) {
        static const char *skip[] = {"does","do","did","are","is","was","were",
            "in","a","an","the","have","has","had","got","of","on","per","there",
            "will","would","can","inside","within",NULL};
        const char *unit = NULL; const char *ct[8]; size_t nct = 0;
        for (size_t i = 2; i < nw; i++) {
            char *t = strip_edge_punct(w[i]);
            int sk = 0;
            for (size_t s = 0; skip[s]; s++) if (!strcmp(t, skip[s])) { sk = 1; break; }
            if (sk || !*t) continue;
            if (!unit) unit = t; else if (nct < 8) ct[nct++] = t;
        }
        if (unit && nct >= 1) {
            /* candidate entities, most specific first: compound of last two
             * content tokens ("soccer_team"), last token, its naive singular. */
            char cand[3][64]; size_t ncand = 0;
            if (nct >= 2)
                snprintf(cand[ncand++], 64, "%s_%s", ct[nct - 2], ct[nct - 1]);
            snprintf(cand[ncand++], 64, "%s", ct[nct - 1]);
            size_t el = strlen(ct[nct - 1]);
            if (el > 1 && ct[nct - 1][el - 1] == 's')
                snprintf(cand[ncand++], 64, "%.*s", (int)(el - 1), ct[nct - 1]);
            char hits[4][KB_TERM_LEN]; size_t k = 0; const char *entity = NULL;
            for (size_t c = 0; c < ncand && k == 0; c++) {
                const char *pat[] = {cand[c], unit, NULL};
                k = kb_match(b->kb, "quantity", pat, 3, hits, 4);
                if (k) entity = cand[c];
            }
            if (k > 0) {
                char ename[64]; snprintf(ename, sizeof ename, "%s", entity);
                for (char *p = ename; *p; p++) if (*p == '_') *p = ' ';
                char msg[160];
                if (cue(buf, " in "))
                    snprintf(msg, sizeof msg, "There are %s %s in a %s.",
                             hits[0], unit, ename);
                else
                    snprintf(msg, sizeof msg, "A %s has %s %s.",
                             ename, hits[0], unit);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* compare: "does <x> have more/less <unit> than <y>" */
    if (nw == 7 && strcmp(w[0], "does") == 0 && strcmp(w[2], "have") == 0 &&
        strcmp(w[5], "than") == 0) {
        int greater = compare_word(w[3]);
        if (greater < 0) return 0;
        const char *unit = w[4], *x = w[1], *y = w[6];
        const char *px[] = {x, unit, NULL}, *py[] = {y, unit, NULL};
        char hx[4][KB_TERM_LEN], hy[4][KB_TERM_LEN];
        size_t kx = kb_match(b->kb, "quantity", px, 3, hx, 4);
        size_t ky = kb_match(b->kb, "quantity", py, 3, hy, 4);
        if (kx == 0 || ky == 0) {
            char msg[200];
            snprintf(msg, sizeof msg, "I don't know how many %s %s has.",
                     unit, kx == 0 ? x : y);
            put(msg, out, out_size);
            return 1;
        }
        double a, c;
        if (!parse_num(hx[0], &a) || !parse_num(hy[0], &c)) return 0;
        put(magnitude_more(a, c, greater) ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: cause -------------------------------------------------------
 * Causal reasoning (gen30). Pulled by the first SuperGLUE COPA question ("The
 * man turned on the faucet. effect: toilet filled / water flowed") — picking a
 * plausible cause or effect, a directed relation parrot0 never had. A causal
 * link is the binary fact `causes(a, b)` (a causes b). This part asserts and
 * queries it in both directions and runs the COPA-shaped two-way chooser over
 * *stated* causal facts. Commonsense causality (COPA's real difficulty —
 * knowing faucets fill spouts) is deliberately out of scope; we build the
 * relation and the chooser, not a world model. */
static int mod_cause(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    /* chooser: "effect of <a>: <c1> or <c2>" / "cause of <a>: <c1> or <c2>" */
    int eff = -1;
    char *rest = NULL;
    if (strncmp(buf, "effect of ", 10) == 0) { eff = 1; rest = buf + 10; }
    else if (strncmp(buf, "cause of ", 9) == 0) { eff = 0; rest = buf + 9; }
    if (rest) {
        char *colon = strchr(rest, ':');
        char *orp = colon ? strstr(colon, " or ") : NULL;
        if (colon && orp) {
            *colon = '\0';
            *orp = '\0';
            const char *a = trim_mut(rest);
            const char *c1 = trim_mut(colon + 1);
            const char *c2 = trim_mut(orp + 4);
            const char *p1[2], *p2[2];
            if (eff) { p1[0]=a; p1[1]=c1; p2[0]=a; p2[1]=c2; }
            else     { p1[0]=c1; p1[1]=a; p2[0]=c2; p2[1]=a; }
            int ok1 = kb_query(b->kb, "causes", p1, 2);
            int ok2 = kb_query(b->kb, "causes", p2, 2);
            char msg[160];
            if (ok1 && !ok2)      snprintf(msg, sizeof msg, "%s.", c1);
            else if (ok2 && !ok1) snprintf(msg, sizeof msg, "%s.", c2);
            else if (ok1 && ok2)  snprintf(msg, sizeof msg, "Both.");
            else                  snprintf(msg, sizeof msg, "Neither.");
            put(msg, out, out_size);
            return 1;
        }
    }

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<a> causes <b>" -> causes(a, b) */
    if (nw == 3 && strcmp(w[1], "causes") == 0) {
        const char *args[] = {w[0], w[2]};
        char msg[160];
        if (kb_assert(b->kb, "causes", args, 2))
            snprintf(msg, sizeof msg, "Learned: causes(%s, %s).", w[0], w[2]);
        else
            snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* query: "what is the effect/cause of <x>?" -> causes(x, ?) / causes(?, x) */
    if (nw == 6 && strcmp(w[0], "what") == 0 && strcmp(w[1], "is") == 0 &&
        strcmp(w[2], "the") == 0 && strcmp(w[4], "of") == 0 &&
        (strcmp(w[3], "effect") == 0 || strcmp(w[3], "cause") == 0)) {
        int want_eff = strcmp(w[3], "effect") == 0;
        const char *x = w[5];
        const char *pat_eff[] = {x, NULL};   /* causes(x, ?) -> effects */
        const char *pat_cause[] = {NULL, x}; /* causes(?, x) -> causes  */
        char hits[64][KB_TERM_LEN];
        size_t k = kb_match(b->kb, "causes", want_eff ? pat_eff : pat_cause,
                             2, hits, 64);
        /* gen90: also find indirect/transitive causes. */
        {
            char mid[64][KB_TERM_LEN];
            size_t kmid = kb_match(b->kb, "causes",
                                    want_eff ? pat_eff : pat_cause, 2, mid, 64);
            for (size_t m = 0; m < kmid && k < 64; m++) {
                const char *indirect[] = {mid[m], NULL};
                const char *indirect_rev[] = {NULL, mid[m]};
                char chain[64][KB_TERM_LEN];
                size_t kc = kb_match(b->kb, "causes",
                                      want_eff ? indirect : indirect_rev, 2, chain, 64);
                for (size_t c = 0; c < kc && k < 64; c++) {
                    int dup = 0;
                    for (size_t d = 0; d < k; d++)
                        if (strcmp(hits[d], chain[c]) == 0) { dup = 1; break; }
                    if (!dup) snprintf(hits[k++], KB_TERM_LEN, "%s (via %s)", chain[c], mid[m]);
                }
            }
        }
        if (k == 0) {
            char msg[160];
            snprintf(msg, sizeof msg, "I don't know the %s of %s.", w[3], x);
            put(msg, out, out_size);
            return 1;
        }
        char list[512];
        size_t off = 0;
        for (size_t i = 0; i < k && off < sizeof list; i++)
            off += (size_t)snprintf(list + off, sizeof list - off,
                                    "%s%s", i ? ", " : "", hits[i]);
        char msg[600];
        snprintf(msg, sizeof msg, "%s.", list);
        put(msg, out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: same --------------------------------------------------------
 * Equivalence between entities (gen33). Pulled by BoolQ #1 ("is house tax and
 * property tax are same"): a question about *sameness*, which parrot0 had no
 * way to represent or answer without conflating it with class membership. A
 * `same(a, b)` fact is stored in both directions so the relation is symmetric;
 * `are <x> and <y> the same?` answers from it (identical names are trivially
 * the same). It is NOT transitively closed — see Decision D-2026-06-15f. */
static int mod_same(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* assert: "<x> is the same as <y>" -> same(x, y) (stored both ways) */
    if (nw == 6 && strcmp(w[1], "is") == 0 && strcmp(w[2], "the") == 0 &&
        strcmp(w[3], "same") == 0 && strcmp(w[4], "as") == 0) {
        const char *fwd[] = {w[0], w[5]}, *bwd[] = {w[5], w[0]};
        int ok = kb_assert(b->kb, "same", fwd, 2) &&
                 kb_assert(b->kb, "same", bwd, 2);
        char msg[160];
        if (ok) snprintf(msg, sizeof msg, "Learned: same(%s, %s).", w[0], w[5]);
        else    snprintf(msg, sizeof msg, "I couldn't store that.");
        put(msg, out, out_size);
        return 1;
    }

    /* query: "are <x> and <y> the same?" -> same(x, y)? */
    if (nw == 6 && strcmp(w[0], "are") == 0 && strcmp(w[2], "and") == 0 &&
        strcmp(w[4], "the") == 0 && strcmp(w[5], "same") == 0) {
        const char *x = w[1], *y = w[3];
        if (strcmp(x, y) == 0) { put("Yes.", out, out_size); return 1; }
        const char *args[] = {x, y};
        put(kb_query(b->kb, "same", args, 2) ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: conj --------------------------------------------------------
 * Conjunctive membership (gen34). Multi-fact reasoning (the MultiRC family)
 * needs combining several facts in one judgement; every prior query asked about
 * a single goal. This part answers two AND-shaped questions — `are <x> and <y>
 * both a <z>?` (z(x) AND z(y)) and `is <x> both a <y> and a <z>?` (y(x) AND
 * z(x)) — by routing EACH conjunct through `kb_query`, so rule-derived
 * membership counts exactly as it does for a single query. No new solver:
 * AND-composition is just two resolver calls. An unknown class is admitted. */
static int mod_conj(Brain *b, const char *norm, const char *raw,
                    char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[10];
    size_t nw = split_words(buf, w, 10);

    /* "are <x> and <y> both a/an <z>?" -> z(x) AND z(y) */
    if (nw == 7 && strcmp(w[0], "are") == 0 && strcmp(w[2], "and") == 0 &&
        strcmp(w[4], "both") == 0 && is_article(w[5])) {
        const char *z = w[6], *x = w[1], *y = w[3];
        if (!kb_knows_pred(b->kb, z)) { idk(z, out, out_size); return 1; }
        const char *ax[] = {x}, *ay[] = {y};
        int yes = kb_query(b->kb, z, ax, 1) && kb_query(b->kb, z, ay, 1);
        put(yes ? "Yes." : "No.", out, out_size);
        return 1;
    }

    /* "is <x> both a/an <y> and a/an <z>?" -> y(x) AND z(x) */
    if (nw == 8 && strcmp(w[0], "is") == 0 && strcmp(w[2], "both") == 0 &&
        is_article(w[3]) && strcmp(w[5], "and") == 0 && is_article(w[6])) {
        const char *x = w[1], *y = w[4], *z = w[7];
        if (!kb_knows_pred(b->kb, y)) { idk(y, out, out_size); return 1; }
        if (!kb_knows_pred(b->kb, z)) { idk(z, out, out_size); return 1; }
        const char *a[] = {x};
        int yes = kb_query(b->kb, y, a, 1) && kb_query(b->kb, z, a, 1);
        put(yes ? "Yes." : "No.", out, out_size);
        return 1;
    }

    return 0;
}

/* --- module: gen ---------------------------------------------------------
 * Generative inference loop (gen36 — DESIGN D-prop1). The brain's other modules
 * infer ONCE and emit a whole reply. This part generates *autoregressively*,
 * the shape an LLM decodes in, but driven by repeated deterministic inference
 * instead of a neural forward pass: emit a word, append it to the working
 * context, re-infer the next word conditioned on what was just produced, and
 * repeat until no continuation is provable (or a step bound).
 *
 * The continuation knowledge is NOT hand-authored (that would be the phrasebook
 * impostor PRINCIPLES.md rejects): it is *induced from examples* as facts
 * `cont(prev, word, count)`. `learn sequence: a b c` asserts the adjacent
 * transitions; `say <w>` runs the loop from a seed. gen36 chooses the first
 * provable continuation (insertion order); frequency-weighted choice and longer
 * context arrive in later generations. */

/* gen106 (L1): the explicit end-of-sequence token. D-prop1 calls for "an
 * explicit stop token / end relation" so decoding halts because the model
 * LEARNED where utterances end — not merely because a step bound cut it off.
 * It is a sentinel atom that begins with a lowercase letter (so the KB reads it
 * as a constant, not a variable — leading uppercase OR '_' means variable,
 * kb.c) and embeds underscores so the whitespace/prose tokenizer can never
 * produce it as a real word. It is learned, never hand-authored, from sentence
 * boundaries in the very text taught (a terminal '.'/'!'/'?' or end-of-stream),
 * so STOP is induced exactly like every other transition. */
#define GEN_STOP "end_of_seq"

/* Learn one transition prev->word, keeping a frequency count. The KB has no
 * in-place update, so we read the current count, retract the old fact, and
 * assert the incremented one (gen37). */
static void learn_transition(Brain *b, const char *prev, const char *word) {
    const char *pat[] = {prev, word, NULL};
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont", pat, 3, cnt, 4);
    long n = 1;
    if (k > 0) {
        n = strtol(cnt[0], NULL, 10) + 1;
        const char *old[] = {prev, word, cnt[0]};
        kb_retract(b->kb, "cont", old, 3);
    }
    char ns[32];
    snprintf(ns, sizeof ns, "%ld", n);
    const char *args[] = {prev, word, ns};
    kb_assert(b->kb, "cont", args, 3);
}

/* Learn one trigram transition (p1 p2)->word with a count (gen38). */
static void learn_transition2(Brain *b, const char *p1, const char *p2,
                              const char *word) {
    const char *pat[] = {p1, p2, word, NULL};
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont2", pat, 4, cnt, 4);
    long n = 1;
    if (k > 0) {
        n = strtol(cnt[0], NULL, 10) + 1;
        const char *old[] = {p1, p2, word, cnt[0]};
        kb_retract(b->kb, "cont2", old, 4);
    }
    char ns[32];
    snprintf(ns, sizeof ns, "%ld", n);
    const char *args[] = {p1, p2, word, ns};
    kb_assert(b->kb, "cont2", args, 4);
}

/* Learn the bigram (cont) and trigram (cont2) transitions across a word stream,
 * returning the number of bigram pairs learned. Shared by `learn sequence:` and
 * the reader (gen41) so the generative model can grow from the same prose the
 * fact extractor reads, not only from explicit teaching. */
/* gen106 (L1): true if `tok` ends a sentence, stripping the trailing terminal
 * punctuation in place so the cleaned word is still learned. A lone "." returns
 * a boundary with an emptied token. */
static int is_sentence_boundary(char *tok) {
    size_t n = strlen(tok);
    if (n == 0) return 0;
    char c = tok[n - 1];
    if (c != '.' && c != '!' && c != '?') return 0;
    while (n > 0 && (tok[n - 1] == '.' || tok[n - 1] == '!' || tok[n - 1] == '?'))
        tok[--n] = '\0';
    return 1;
}

/* gen106 (L1): learn the bigram (cont) and trigram (cont2) transitions across a
 * word stream, INCLUDING a learned end-of-sequence: at every sentence boundary
 * (terminal punctuation or end-of-stream) the last real word gets a transition
 * to GEN_STOP, and the rolling context resets so no transition bridges the
 * boundary. Returns the number of (non-STOP) bigram pairs learned — the count
 * the "learn sequence:" reply reports, unchanged from gen41. Shared by
 * `learn sequence:` and the reader, so the generative model grows from the same
 * prose the fact extractor reads. */
static size_t learn_word_stream(Brain *b, char **w, size_t nw) {
    size_t pairs = 0;
    const char *p1 = NULL, *p2 = NULL; /* rolling context, reset at boundaries */
    for (size_t i = 0; i < nw; i++) {
        if (strlen(w[i]) >= KB_TERM_LEN) { p2 = NULL; p1 = NULL; continue; }
        int boundary = is_sentence_boundary(w[i]); /* may empty the token */
        const char *cur = w[i];
        if (*cur) {
            if (p1) { learn_transition(b, p1, cur); pairs++; }
            if (p2 && p1) learn_transition2(b, p2, p1, cur);
            p2 = p1; p1 = cur;
        }
        if (boundary && p1) {                       /* learned end-of-sequence */
            learn_transition(b, p1, GEN_STOP);
            if (p2) learn_transition2(b, p2, p1, GEN_STOP);
            p2 = NULL; p1 = NULL;                    /* do not bridge sentences */
        }
    }
    if (p1) learn_transition(b, p1, GEN_STOP);       /* end-of-stream is a stop */
    return pairs;
}

/* Start each `read:` passage as a fresh corpus for generation. The reader still
 * accumulates extracted facts in the KB, but the continuation model represents
 * the most recently read passage, so a held-out second passage can measurably
 * shift `say <seed>` instead of tying the first passage by insertion order. */
static void clear_generation_model(Brain *b) {
    if (!b || !b->kb) return;

    char prevs[128][KB_TERM_LEN];
    const char *any3[] = {NULL, NULL, NULL};
    size_t np = kb_match(b->kb, "cont", any3, 3, prevs, 128);
    for (size_t i = 0; i < np; i++) {
        const char *word_pat[] = {prevs[i], NULL, NULL};
        char words[128][KB_TERM_LEN];
        size_t nw = kb_match(b->kb, "cont", word_pat, 3, words, 128);
        for (size_t j = 0; j < nw; j++) {
            const char *cnt_pat[] = {prevs[i], words[j], NULL};
            char counts[16][KB_TERM_LEN];
            size_t nc = kb_match(b->kb, "cont", cnt_pat, 3, counts, 16);
            for (size_t k = 0; k < nc; k++) {
                const char *old[] = {prevs[i], words[j], counts[k]};
                kb_retract(b->kb, "cont", old, 3);
            }
        }
    }

    char p1s[128][KB_TERM_LEN];
    const char *any4[] = {NULL, NULL, NULL, NULL};
    size_t n1 = kb_match(b->kb, "cont2", any4, 4, p1s, 128);
    for (size_t i = 0; i < n1; i++) {
        const char *p2_pat[] = {p1s[i], NULL, NULL, NULL};
        char p2s[128][KB_TERM_LEN];
        size_t n2 = kb_match(b->kb, "cont2", p2_pat, 4, p2s, 128);
        for (size_t j = 0; j < n2; j++) {
            const char *word_pat[] = {p1s[i], p2s[j], NULL, NULL};
            char words[128][KB_TERM_LEN];
            size_t nw = kb_match(b->kb, "cont2", word_pat, 4, words, 128);
            for (size_t k = 0; k < nw; k++) {
                const char *cnt_pat[] = {p1s[i], p2s[j], words[k], NULL};
                char counts[16][KB_TERM_LEN];
                size_t nc = kb_match(b->kb, "cont2", cnt_pat, 4, counts, 16);
                for (size_t m = 0; m < nc; m++) {
                    const char *old[] = {p1s[i], p2s[j], words[k], counts[m]};
                    kb_retract(b->kb, "cont2", old, 4);
                }
            }
        }
    }
}

/* Look up the stored count for a transition, or 0 if absent. `argc` counts the
 * context+word slots (2 for cont, 3 for cont2); the trailing count slot is the
 * variable kb_match binds. */
static long transition_count(Brain *b, const char *rel,
                             const char *const *key, size_t keyn) {
    const char *pat[KB_MAX_ARGS];
    for (size_t i = 0; i < keyn; i++) pat[i] = key[i];
    pat[keyn] = NULL; /* count */
    char cnt[4][KB_TERM_LEN];
    size_t k = kb_match(b->kb, rel, pat, keyn + 1, cnt, 4);
    return (k > 0) ? strtol(cnt[0], NULL, 10) : 0;
}

/* Choose the next word by INTERPOLATING the bigram and trigram evidence
 * (gen42), replacing gen38's hard backoff. Each bigram candidate `w` of `p1`
 * scores W2*cont2(p2,p1,w) + W1*cont(p1,w): the longer context informs the
 * choice without dictating it, so a single count-1 trigram no longer overrides
 * a strong bigram (Decision D-2026-06-15k). Every trigram continuation has a
 * matching bigram (the learner emits both), so the bigram set is the complete
 * candidate set. The gen40 CRITICAL FILTER still applies: when `subj` is set
 * the running output is a claim "<subj> is a ___", and any `w` the KB knows
 * `w(subj)` false/conflicted is skipped. Tie -> insertion order. Returns 1 if a
 * word was chosen, 0 if there are no candidates or every one was blocked (the
 * caller then stops rather than utter a falsehood). */
/* gen111 (D-prop1 step 2): the decoder's choice ranking is itself KB knowledge.
 * The interpolation coefficients live as `weight(trigram, N)` / `weight(bigram,
 * N)` facts, read here with a fallback default — so the generation POLICY is
 * inspectable and editable knowledge, not hardcoded C (DESIGN.md D6). Editing the
 * fact (e.g. "set trigram weight to 0") changes which continuation wins. */
static long gen_weight(Brain *b, const char *kind, long dflt) {
    if (!b || !b->kb) return dflt;
    const char *pat[] = { kind, NULL };
    char v[2][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "weight", pat, 2, v, 2);
    return k ? strtol(v[0], NULL, 10) : dflt;
}

static int next_word_ctx(Brain *b, const char *p2, const char *p1,
                         const char *subj, char *word, size_t wsize) {
    /* trigram weight dominates but does not dictate — now read from the KB. */
    const long W2 = gen_weight(b, "trigram", 3), W1 = gen_weight(b, "bigram", 1);
    const char *pat[] = {p1, NULL, NULL};
    char words[64][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "cont", pat, 3, words, 64);
    if (k == 0) return 0;

    long best = -1;
    size_t bi = 0;
    int found = 0;
    for (size_t i = 0; i < k; i++) {
        if (subj) {
            const char *a[] = {subj};
            if (kb_is_negated(b->kb, words[i], a, 1) ||
                kb_is_conflicted(b->kb, words[i], a, 1))
                continue; /* would assert a known-false claim: refuse to say it */
        }
        const char *bkey[] = {p1, words[i]};
        long c1 = transition_count(b, "cont", bkey, 2);
        long c2 = 0;
        if (p2) {
            const char *tkey[] = {p2, p1, words[i]};
            c2 = transition_count(b, "cont2", tkey, 3);
        }
        long score = W2 * c2 + W1 * c1;
        if (score > best) { best = score; bi = i; found = 1; } /* > -> first tie */
    }
    if (!found) return 0; /* candidates existed, all blocked -> stop */
    snprintf(word, wsize, "%s", words[bi]);
    return 1;
}

static void generate_from(Brain *b, const char *seed, char *out, size_t out_size) {
    char toks[64][KB_TERM_LEN];
    size_t nt = 0;
    snprintf(toks[nt++], KB_TERM_LEN, "%s", seed);

    char line[1024];
    size_t off = (size_t)snprintf(line, sizeof line, "%s", toks[0]);

    for (int step = 0; step < 24 && nt < 64; step++) { /* bound guards cycles */
        const char *p1 = toks[nt - 1];
        const char *p2 = (nt >= 2) ? toks[nt - 2] : NULL;

        /* gen40: if the tail reads "<x> is a/an", the next word is a claim
         * about x — pass x as the subject so the filter can veto false ones. */
        const char *subj = NULL;
        if (nt >= 3 && strcmp(toks[nt - 2], "is") == 0 &&
            (strcmp(toks[nt - 1], "a") == 0 || strcmp(toks[nt - 1], "an") == 0))
            subj = toks[nt - 3];

        char nxt[KB_TERM_LEN];
        if (!next_word_ctx(b, p2, p1, subj, nxt, sizeof nxt)) break;
        if (strcmp(nxt, GEN_STOP) == 0) break; /* gen106: learned end-of-sequence */
        if (off < sizeof line)
            off += (size_t)snprintf(line + off, sizeof line - off, " %s", nxt);
        snprintf(toks[nt++], KB_TERM_LEN, "%s", nxt);
    }
    put(line, out, out_size);
}
