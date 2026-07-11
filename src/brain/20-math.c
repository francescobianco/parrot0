static int mod_compare(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)b; (void)raw;
    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);
    if (nw != 5 || strcmp(w[0], "is") != 0 || strcmp(w[3], "than") != 0)
        return 0;

    int greater = compare_word(w[2]);
    if (greater < 0) return 0;

    double a, c;
    if (!parse_num(w[1], &a) || !parse_num(w[4], &c)) return 0; /* not numbers */

    put(magnitude_more(a, c, greater) ? "Yes." : "No.", out, out_size);
    return 1;
}

/* --- module: arith -------------------------------------------------------
 * Arithmetic over numbers (gen35). gen27/gen28 could *order* magnitudes but
 * never *compute* with them — numbers were inert (Decision D-2026-06-15a). The
 * first SuperGLUE BoolQ #6 ("can an odd number be divided by an even number")
 * is about arithmetic. This part computes `what is <a> plus/minus/times <b>?`
 * and decides `is <a> divisible by <b>?` by integer remainder, over literal
 * numbers (parsed with the shared `parse_num`). Operator set kept tiny on
 * purpose; non-numbers are declined and fall through. */

/* Format a value as a clean integer when it is integral, else compactly. */
static void format_num(double v, char *buf, size_t sz) {
    long long iv = (long long)v;
    if ((double)iv == v) snprintf(buf, sz, "%lld", iv);
    else                 snprintf(buf, sz, "%g", v);
}

/* True if `s` is a supported arithmetic operator keyword or symbol. */
static int is_arith_op(const char *s) {
    return strcmp(s, "plus") == 0 || strcmp(s, "minus") == 0 ||
           strcmp(s, "times") == 0 ||
           strcmp(s, "+") == 0 || strcmp(s, "-") == 0 || strcmp(s, "*") == 0;
}

/* Apply an arithmetic operator, returning the result. Sets *ok=0 for unknown ops. */
static double apply_arith_op(const char *op, double a, double c, int *ok) {
    *ok = 1;
    if (strcmp(op, "plus") == 0 || strcmp(op, "+") == 0) return a + c;
    if (strcmp(op, "minus") == 0 || strcmp(op, "-") == 0) return a - c;
    if (strcmp(op, "times") == 0 || strcmp(op, "*") == 0) return a * c;
    if (strcmp(op, "/") == 0) { if (c == 0) { *ok = 0; return 0; } return a / c; }
    *ok = 0;
    return 0;
}

/* gen190: arithmetic in natural language. The catalogue (basic-chat cat.4) asks
 * the SAME four operations in many surface forms — "six times seven", "add 5 and
 * 7", "100 divided by 4", "half of 50". These are not new computations; they are
 * new ways of NAMING operands and operators. So instead of one phrase per shape,
 * the parser below extracts (operator, operands) structurally and folds them with
 * the existing oracle. Operator words are recognised in EN+IT (per/più/diviso),
 * so the bilingual probe rides the same path. */

/* Canonical operator char from a single operator word or symbol (EN+IT). */
static char arith_op_char(const char *s) {
    if (!strcmp(s,"plus")||!strcmp(s,"+")||!strcmp(s,"più")||!strcmp(s,"piu")) return '+';
    if (!strcmp(s,"minus")||!strcmp(s,"-")||!strcmp(s,"meno")) return '-';
    if (!strcmp(s,"times")||!strcmp(s,"*")||!strcmp(s,"x")||!strcmp(s,"per")) return '*';
    if (!strcmp(s,"multiplied")) return '*';
    if (!strcmp(s,"divided")||!strcmp(s,"diviso")||!strcmp(s,"over")||!strcmp(s,"/")) return '/';
    return 0;
}

static double apply_op_char(char op, double a, double c, int *ok) {
    *ok = 1;
    switch (op) {
        case '+': return a + c;
        case '-': return a - c;
        case '*': return a * c;
        case '/': if (c == 0) { *ok = 0; return 0; } return a / c;
    }
    *ok = 0; return 0;
}

/* Square root without <math.h> (Newton's method); for our integer operands this
 * lands exactly on perfect squares. */
static double arith_sqrt(double x) {
    if (x < 0) return -1;
    if (x == 0) return 0;
    double g = x > 1 ? x : 1;
    for (int i = 0; i < 80; i++) g = 0.5 * (g + x / g);
    return g;
}

static int arith_is_prime(long long n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (long long d = 3; d * d <= n; d += 2)
        if (n % d == 0) return 0;
    return 1;
}

/* Fold ew[..] left-to-right as an arithmetic expression in any surface form:
 * skip leading non-numeric filler ("what is", "how much is"), then read
 * NUM (OP NUM)+ where OP is a word/symbol, consuming a "by" after
 * "divided"/"multiplied" and reading a bare "by" between numbers as times
 * ("six by seven"). Returns 1 with *res iff >=1 op applied and the whole tail
 * parsed cleanly (so prose like "5 apples ..." never matches). */
static int arith_eval_infix(char **ew, size_t enw, double *res) {
    size_t i = 0;
    double cur;
    while (i < enw && !parse_value(ew[i], &cur)) i++;
    if (i >= enw) return 0;
    i++;
    int ops = 0;
    while (i < enw) {
        char op = arith_op_char(ew[i]);
        if (!op) {
            if (!strcmp(ew[i], "by")) op = '*';      /* "six by seven" */
            else return 0;
        }
        if ((!strcmp(ew[i], "divided") || !strcmp(ew[i], "multiplied")) &&
            i + 1 < enw && !strcmp(ew[i + 1], "by"))
            i++;                                      /* consume the "by" */
        i++;
        if (i >= enw) return 0;
        double nx;
        if (!parse_value(ew[i], &nx)) return 0;
        int ok;
        cur = apply_op_char(op, cur, nx, &ok);
        if (!ok) return 0;
        i++; ops++;
    }
    if (ops == 0) return 0;
    *res = cur;
    return 1;
}

/* Emit a numeric answer as "<n>." */
static void arith_answer(double v, char *out, size_t out_size) {
    char num[64], msg[80];
    format_num(v, num, sizeof num);
    snprintf(msg, sizeof msg, "%s.", num);
    put(msg, out, out_size);
}

/* gen312 (P2: routing NL->calcolo). The single-op handlers below each compute
 * ONE operation ("15% of 80" -> 12, "square root of 144" -> 12). A COMPOUND
 * expression joins several such operands with +,-,*,/ ("15% of 80 plus the
 * square root of 144" -> 24). KB-first note: composition is not a new fact but
 * a new WAY OF COMBINING computations parrot0 already knows -- so instead of a
 * handler per shape we evaluate each operand phrase, then fold the operators.
 *
 * eval_operand evaluates one operand span w[s..e) as a small computation:
 * "P percent of N", "square root of N", "half/third/quarter of N",
 * "N squared/cubed", or a plain number (exactly one numeral in the span). */
static int eval_operand(char **w, size_t s, size_t e, double *val) {
    if (s >= e) return 0;
    size_t ofp = e;
    for (size_t i = s; i < e; i++) if (!strcmp(w[i], "of")) { ofp = i; break; }

    /* P percent of N */
    long pctpos = -1;
    for (size_t i = s; i < e; i++)
        if (!strcmp(w[i], "percent") || !strcmp(w[i], "%")) { pctpos = (long)i; break; }
    if (pctpos >= 0 && ofp < e) {
        double P = 0, N = 0; int haveP = 0, haveN = 0;
        for (size_t i = s; i < (size_t)pctpos; i++) if (parse_value(w[i], &P)) haveP = 1;
        for (size_t i = ofp + 1; i < e; i++) if (parse_value(w[i], &N)) { haveN = 1; break; }
        if (haveP && haveN) { *val = P / 100.0 * N; return 1; }
    }

    /* square root of N */
    for (size_t i = s; i < e; i++)
        if (!strcmp(w[i], "sqrt") || !strcmp(w[i], "root")) {
            double N = 0; int haveN = 0;
            for (size_t j = s; j < e; j++) if (parse_value(w[j], &N)) { haveN = 1; break; }
            if (haveN && N >= 0) { *val = arith_sqrt(N); return 1; }
            return 0;
        }

    /* fraction of N: half/third/quarter */
    if (ofp < e) {
        double denom = 0;
        for (size_t i = s; i < ofp; i++) {
            if (!strcmp(w[i], "half")) denom = 2;
            else if (!strcmp(w[i], "third")) denom = 3;
            else if (!strcmp(w[i], "quarter") || !strcmp(w[i], "fourth")) denom = 4;
        }
        if (denom > 0) {
            double N = 0; int haveN = 0;
            for (size_t i = ofp + 1; i < e; i++) if (parse_value(w[i], &N)) { haveN = 1; break; }
            if (haveN) { *val = N / denom; return 1; }
        }
    }

    /* N squared / N cubed */
    for (size_t i = s; i < e; i++)
        if (!strcmp(w[i], "squared") || !strcmp(w[i], "cubed")) {
            double N = 0; int haveN = 0;
            for (size_t j = s; j < e; j++) if (parse_value(w[j], &N)) { haveN = 1; break; }
            if (haveN) { *val = !strcmp(w[i], "squared") ? N * N : N * N * N; return 1; }
        }

    /* plain: exactly one numeral in the span (rejects prose spans) */
    double only = 0; int cnt = 0;
    for (size_t i = s; i < e; i++) { double v; if (parse_value(w[i], &v)) { only = v; cnt++; } }
    if (cnt == 1) { *val = only; return 1; }
    return 0;
}

/* Split norm into operand spans at top-level +,-,*,/ operators, evaluate each
 * with eval_operand, then fold (* and / bind before + and -). Returns 0 (fall
 * through to the single-op handlers) when there is no operator or any span is
 * not a clean operand. Uses its own wide tokenization because mod_arith's w[8]
 * would truncate a long expression before the second operand. */
static int arith_compound(const char *norm, char *out, size_t out_size) {
    char buf[256];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (buf[len - 1] == '?' || buf[len - 1] == '.') buf[len - 1] = '\0';

    char *w[40];
    size_t nw = split_words(buf, w, 40);
    for (size_t i = 0; i < nw; i++) w[i] = strip_edge_punct(w[i]);

    size_t opos[16]; char ochar[16]; size_t nop = 0;
    for (size_t i = 0; i < nw && nop < 16; i++) {
        char o = 0;
        if (!strcmp(w[i], "plus")) o = '+';
        else if (!strcmp(w[i], "minus")) o = '-';
        else if (!strcmp(w[i], "times") || !strcmp(w[i], "multiplied")) o = '*';
        else if (!strcmp(w[i], "divided")) o = '/';
        if (o) { opos[nop] = i; ochar[nop] = o; nop++; }
    }
    if (nop == 0 || nop > 15) return 0;

    double vals[17]; size_t nv = 0, start = 0;
    for (size_t k = 0; k <= nop; k++) {
        size_t end = (k < nop) ? opos[k] : nw;
        double v;
        if (!eval_operand(w, start, end, &v)) return 0;
        vals[nv++] = v;
        if (k < nop) {
            start = opos[k] + 1;
            if ((ochar[k] == '*' || ochar[k] == '/') && start < nw && !strcmp(w[start], "by"))
                start++;                    /* "multiplied by" / "divided by" */
        }
    }

    double acc[17]; char aop[16]; size_t na = 0, np = 0;
    acc[na++] = vals[0];
    for (size_t k = 0; k < nop; k++) {
        if (ochar[k] == '*') acc[na - 1] *= vals[k + 1];
        else if (ochar[k] == '/') { if (vals[k + 1] == 0) return 0; acc[na - 1] /= vals[k + 1]; }
        else { aop[np++] = ochar[k]; acc[na++] = vals[k + 1]; }
    }
    double res = acc[0];
    for (size_t k = 0; k < np; k++) res = (aop[k] == '+') ? res + acc[k + 1] : res - acc[k + 1];
    arith_answer(res, out, out_size);
    return 1;
}

static int mod_arith(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;

    /* gen252: classic letter riddles contain numbers but are not arithmetic.
     * Catch this before expression folding turns "twice ... thousand" into math. */
    if (b && b->kb && cue(norm, "once in a minute") &&
        cue(norm, "twice in a moment") && cue(norm, "thousand years")) {
        if (kb_response(b, "riddle_letter_m", NULL, out, out_size)) return 1;
    }

    /* gen312: compound arithmetic expression (self-guards: fires only when a
     * top-level operator joins clean operand phrases, else falls through). */
    if (arith_compound(norm, out, out_size)) return 1;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    /* Expand tokens containing embedded operators (e.g. "2+2" -> "2","+","2").
     * Pure numbers (which may start with '-') are left intact so parse_num works.
     * The expansion is done in a secondary buffer; `ew` points into it. */
    char exbuf[256];
    size_t exoff = 0;
    char *ew[24];
    size_t enw = 0;
    for (size_t i = 0; i < nw && enw < 24; i++) {
        const char *s = w[i];
        size_t sl = strlen(s);
        int has_embedded_op = 0;
        for (size_t j = 0; j < sl; j++) {
            if (s[j] == '+' || s[j] == '*' || s[j] == '/') has_embedded_op = 1;
            else if (s[j] == '-' && j > 0) has_embedded_op = 1;
        }
        double v;
        int is_num = parse_num(s, &v);
        if (!is_num && has_embedded_op) {
            size_t start = 0;
            for (size_t j = 0; j <= sl && enw < 24; j++) {
                int boundary = (j == sl || s[j] == '+' || s[j] == '*' ||
                    s[j] == '/' || (s[j] == '-' && j > 0));
                if (boundary) {
                    if (j > start && exoff + (j - start) + 1 <= sizeof exbuf) {
                        memcpy(exbuf + exoff, s + start, j - start);
                        exbuf[exoff + (j - start)] = '\0';
                        ew[enw++] = exbuf + exoff;
                        exoff += (j - start) + 1;
                    }
                    if (j < sl && exoff + 2 <= sizeof exbuf) {
                        exbuf[exoff] = s[j];
                        exbuf[exoff + 1] = '\0';
                        ew[enw++] = exbuf + exoff;
                        exoff += 2;
                    }
                    start = j + 1;
                }
            }
        } else {
            if (exoff + sl + 1 <= sizeof exbuf) {
                memcpy(exbuf + exoff, s, sl);
                exbuf[exoff + sl] = '\0';
                ew[enw++] = exbuf + exoff;
                exoff += sl + 1;
            }
        }
    }

    /* Exact-shape arith: "what is <a> OP <b>?" with expanded tokens. */
    if (enw == 5 && strcmp(ew[0], "what") == 0 && strcmp(ew[1], "is") == 0 &&
        is_arith_op(ew[3])) {
        double a, c;
        if (parse_value(ew[2], &a) && parse_value(ew[4], &c)) {
            int ok;
            double r = apply_arith_op(ew[3], a, c, &ok);
            if (ok) {
                char num[64], msg[80];
                format_num(r, num, sizeof num);
                snprintf(msg, sizeof msg, "%s.", num);
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* Flexible search: find "what"+"is", then scan for NUM OP NUM anywhere after.
     * "what is the result of 2 plus 3", "what is 2 + 3" (already matched above). */
    {
        size_t wi = find_token(ew, enw, "what");
        if (wi < enw) {
            size_t si = find_token(ew + wi, enw - wi, "is");
            if (si < enw - wi) {
                si += wi;
                for (size_t i = si + 1; i + 2 < enw; i++) {
                    if (!is_arith_op(ew[i + 1])) continue;
                    double a, c;
                    if (parse_value(ew[i], &a) && parse_value(ew[i + 2], &c)) {
                        int ok;
                        double r = apply_arith_op(ew[i + 1], a, c, &ok);
                        if (ok) {
                            char num[64], msg[80];
                            format_num(r, num, sizeof num);
                            snprintf(msg, sizeof msg, "%s.", num);
                            put(msg, out, out_size);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    /* gen101: "explain why <a> plus <b> is <c>" — give the JUSTIFICATION, not
     * just the value. Pulled by the impersonation benchmark's math-teacher role,
     * but it is a general capability: the answer is grounded in the operation
     * (adding -> sum), so it transfers to any operands and any of the operators. */
    if (cue(buf, "explain") || cue(buf, "why")) {
        for (size_t i = 0; i + 2 < enw; i++) {
            if (!is_arith_op(ew[i + 1])) continue;
            double a, c;
            if (!parse_num(ew[i], &a) || !parse_num(ew[i + 2], &c)) continue;
            int ok;
            double r = apply_arith_op(ew[i + 1], a, c, &ok);
            if (!ok) continue;
            const char *op = ew[i + 1];
            const char *verb = "combining", *noun = "result";
            if (strcmp(op, "plus") == 0 || strcmp(op, "+") == 0) { verb = "adding"; noun = "sum"; }
            else if (strcmp(op, "minus") == 0 || strcmp(op, "-") == 0) { verb = "subtracting"; noun = "difference"; }
            else if (strcmp(op, "times") == 0 || strcmp(op, "*") == 0) { verb = "multiplying"; noun = "product"; }
            char na[64], nb[64], nr[64], msg[320];
            format_num(a, na, sizeof na);
            format_num(c, nb, sizeof nb);
            format_num(r, nr, sizeof nr);
            snprintf(msg, sizeof msg,
                     "Because %s %s and %s gives %s — that is their %s.",
                     verb, na, nb, nr, noun);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* "is <a> divisible by <b>?" -> yes/no via integer remainder */
    if (enw == 5 && strcmp(ew[0], "is") == 0 && strcmp(ew[2], "divisible") == 0 &&
        strcmp(ew[3], "by") == 0) {
        double a, c;
        if (!parse_num(ew[1], &a) || !parse_num(ew[4], &c)) return 0;
        if (c == 0) { put("I can't divide by zero.", out, out_size); return 1; }
        long long ai = (long long)a, ci = (long long)c;
        int divisible;
        if ((double)ai == a && (double)ci == c) {
            divisible = (ai % ci == 0);
        } else {
            double q = a / c;
            double rem = a - c * (double)(long long)q;
            divisible = (rem > -1e-9 && rem < 1e-9);
        }
        put(divisible ? "Yes." : "No.", out, out_size);
        return 1;
    }

    /* gen190: natural-language arithmetic shapes (basic-chat cat.4). All of these
     * reduce to the four ops over operands named in prose; each is a structural
     * extractor, not a stored phrase. They share parse_value (digits + number
     * words) and the existing oracle, so they generalize over operands.
     *
     * NOTE: `buf` was clobbered in place by split_words above (it null-terminates
     * each token), and `w[8]` truncates long prompts. So these frames read cues
     * from the intact `norm` and re-split a fresh copy into a larger token array. */
    char cbuf[256]; char *cw[32]; size_t cnw = 0;
    {
        size_t cl = strlen(norm);
        if (cl < sizeof cbuf) {
            memcpy(cbuf, norm, cl + 1);
            if (cl > 0 && cbuf[cl - 1] == '?') cbuf[cl - 1] = '\0';
            cnw = split_words(cbuf, cw, 32);
        }
    }
    /* How many numeric operands the turn names. Several arithmetic cue words
     * ("half"/"halve", "double", "triple", "even", "odd") also appear as repeated
     * ACTIONS or branch conditions in agent/process descriptions ("halve until
     * below 5", "if it is even, ...; if it is odd, ..."). Those name >=2 numbers,
     * so the ambiguous frames below fire only when the turn names exactly one
     * number — keeping mod_agent's loops intact. */
    double gnums[16]; size_t gn = collect_numbers(cw, cnw, gnums, 16);

    /* "P percent of N" / "P per cento di N" -> N * P / 100. The percent marker is
     * "percent"/"%" (EN) or the "per cento" pair (IT, marked by the "cento" token,
     * which must NOT be read as the operand 100): P is the number just before it,
     * N the number just after. */
    {
        size_t mark = cnw; int attached = 0;
        for (size_t i = 0; i < cnw; i++) {
            if (!strcmp(cw[i], "percent") || !strcmp(cw[i], "%") ||
                !strcmp(cw[i], "cento")) { mark = i; break; }
            /* gen240: "15%" is one token — a number with a trailing '%'. */
            size_t li = strlen(cw[i]);
            if (li > 1 && cw[i][li - 1] == '%') { mark = i; attached = 1; break; }
        }
        if (mark < cnw) {
            double pct = 0, base = 0; int havep = 0, haveb = 0;
            if (attached) {
                char tmp[32]; snprintf(tmp, sizeof tmp, "%.*s",
                                       (int)(strlen(cw[mark]) - 1), cw[mark]);
                if (parse_value(tmp, &pct)) havep = 1;
            }
            for (size_t i = mark; !havep && i-- > 0; ) {
                double v; if (parse_value(cw[i], &v)) { pct = v; havep = 1; break; }
            }
            for (size_t i = mark + 1; i < cnw; i++) {
                double v; if (parse_value(cw[i], &v)) { base = v; haveb = 1; break; }
            }
            if (havep && haveb) { arith_answer(base * pct / 100.0, out, out_size); return 1; }
        }
    }

    /* Unary "of"-frames and suffix frames over a single operand (EN+IT). */
    {
        /* square root of N / radice quadrata di N (declines a negative operand,
         * whose real square root does not exist — that is cat.5, not cat.4). */
        size_t ri = find_token(cw, cnw, "root");
        if (ri == cnw) ri = find_token(cw, cnw, "radice");
        if (ri != cnw && !cue(norm, "negative") && !cue(norm, "negativo")) {
            for (size_t i = ri + 1; i < cnw; i++) {
                double v; if (parse_value(cw[i], &v)) {
                    if (v >= 0) { arith_answer(arith_sqrt(v), out, out_size); return 1; }
                    break;
                }
            }
        }
        /* N squared / N cubed, N al quadrato / al cubo. */
        if (gn == 1) {
            int sq = 0, cb = 0;
            for (size_t i = 0; i < cnw; i++) {
                if (!strcmp(cw[i],"squared") || !strcmp(cw[i],"quadrato")) sq = 1;
                else if (!strcmp(cw[i],"cubed") || !strcmp(cw[i],"cubo")) cb = 1;
            }
            if (sq) { arith_answer(gnums[0] * gnums[0], out, out_size); return 1; }
            if (cb) { arith_answer(gnums[0] * gnums[0] * gnums[0], out, out_size); return 1; }
        }
        /* N factorial / fattoriale di N (single non-negative integer <= 20). */
        if (gn == 1 && (find_token(cw,cnw,"factorial") != cnw ||
                        find_token(cw,cnw,"fattoriale") != cnw)) {
            double v = gnums[0];
            if (v >= 0 && v <= 20 && (double)(long long)v == v) {
                long long f = 1; for (long long k = 2; k <= (long long)v; k++) f *= k;
                arith_answer((double)f, out, out_size); return 1;
            }
        }
        /* half of N / metà di N */
        if (gn == 1 && (cue(norm, "half") || cue(norm, "metà") || cue(norm, "meta"))) {
            arith_answer(gnums[0] / 2.0, out, out_size); return 1;
        }
        /* double / twice / triple / thrice (optionally "of") N */
        if (gn == 1 && (cue(norm, "double") || cue(norm, "twice") || cue(norm, "doppio") ||
            cue(norm, "triple") || cue(norm, "thrice") || cue(norm, "triplo"))) {
            double mul = (cue(norm, "triple") || cue(norm, "thrice") || cue(norm, "triplo")) ? 3 : 2;
            arith_answer(gnums[0] * mul, out, out_size); return 1;
        }
    }

    /* N-ary "sum of A and B and C..." and "average/mean of ...". */
    if (cue(norm, "sum of") || cue(norm, "average of") || cue(norm, "mean of") ||
        cue(norm, "somma di") || cue(norm, "media di")) {
        double nums[16]; size_t n = collect_numbers(cw, cnw, nums, 16);
        if (n >= 1) {
            double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
            int avg = cue(norm, "average") || cue(norm, "mean") || cue(norm, "media");
            arith_answer(avg ? s / (double)n : s, out, out_size);
            return 1;
        }
    }

    /* Verb-led imperative frames: "add A and B", "subtract A from B",
     * "multiply A by B", "divide A by B" (EN+IT). */
    if (cnw >= 1) {
        const char *v0 = cw[0];
        double nums[16]; size_t n = collect_numbers(cw, cnw, nums, 16);
        if (n >= 2) {
            if (!strcmp(v0,"add") || !strcmp(v0,"sum") || !strcmp(v0,"aggiungi") ||
                !strcmp(v0,"somma")) {
                double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
                arith_answer(s, out, out_size); return 1;
            }
            if (!strcmp(v0,"subtract") || !strcmp(v0,"sottrai") || !strcmp(v0,"togli")) {
                /* "subtract A from B" -> B - A */
                if (cue(norm, "from") || cue(norm, " da ")) { arith_answer(nums[1] - nums[0], out, out_size); return 1; }
                arith_answer(nums[0] - nums[1], out, out_size); return 1;
            }
            if (!strcmp(v0,"multiply") || !strcmp(v0,"moltiplica")) {
                double p = 1; for (size_t i = 0; i < n; i++) p *= nums[i];
                arith_answer(p, out, out_size); return 1;
            }
            if (!strcmp(v0,"divide") || !strcmp(v0,"dividi")) {
                if (nums[1] != 0) { arith_answer(nums[0] / nums[1], out, out_size); return 1; }
            }
        }
    }

    /* Number-property predicates: "is N prime / even / odd" (EN+IT property
     * words, any word order — "5 è un numero primo"). The property word is read
     * tokenwise (not as a substring) so "evening"/"Paris" never trigger it, and
     * it fires only when the turn also names an integer. */
    {
        int wants_prime = 0, wants_even = 0, wants_odd = 0;
        for (size_t i = 0; i < cnw; i++) {
            if (!strcmp(cw[i],"prime") || !strcmp(cw[i],"primo") || !strcmp(cw[i],"prima"))
                wants_prime = 1;
            else if (!strcmp(cw[i],"even") || !strcmp(cw[i],"pari")) {
                /* gen254 (repair): concessive "even though/if/so" is not the
                 * number property; and a creative-continuation request must
                 * never be read as a parity question ("...began to chime, even
                 * though no one had touched it" answered "No, 1 is not even"). */
                if (i + 1 < cnw && (!strcmp(cw[i+1],"though") ||
                    !strcmp(cw[i+1],"if") || !strcmp(cw[i+1],"so") ||
                    !strcmp(cw[i+1],"when"))) continue;
                wants_even = 1;
            }
            else if (!strcmp(cw[i],"odd") || !strcmp(cw[i],"dispari")) wants_odd = 1;
        }
        if (cue(buf, "continue") || cue(buf, "complete this") ||
            cue(buf, "finish this"))
            wants_prime = wants_even = wants_odd = 0;
        if (gn == 1 && (wants_prime || wants_even || wants_odd)) {
            for (size_t i = 0; i < cnw; i++) {
                /* "no one"/"someone" is a pronoun, not the number 1 */
                if (i > 0 && !strcmp(cw[i], "one") &&
                    (!strcmp(cw[i-1],"no") || !strcmp(cw[i-1],"some") ||
                     !strcmp(cw[i-1],"any") || !strcmp(cw[i-1],"every")))
                    continue;
                double v; if (!parse_value(cw[i], &v)) continue;
                if ((double)(long long)v != v) break;
                long long n = (long long)v;
                char msg[96];
                if (wants_prime) {
                    snprintf(msg, sizeof msg, "%s, %lld is %sa prime number.",
                             arith_is_prime(n) ? "Yes" : "No", n,
                             arith_is_prime(n) ? "" : "not ");
                } else {
                    int even = (n % 2 == 0);
                    int yes = wants_even ? even : !even;
                    snprintf(msg, sizeof msg, "%s, %lld is %s%s number.",
                             yes ? "Yes" : "No", n, yes ? "an " : "not an ",
                             wants_even ? "even" : "odd");
                }
                put(msg, out, out_size);
                return 1;
            }
        }
    }

    /* General fallback: fold any infix expression ("six times seven",
     * "100 divided by 4", "how much is 1+1+1+1+1"). */
    {
        double r;
        if (arith_eval_infix(ew, enw, &r)) { arith_answer(r, out, out_size); return 1; }
    }

    return 0;
}

/* --- module: algebra (L17) ------------------------------------------------
 * One-step equation solving: "x + 3 = 7" -> x = 4. gen35 could COMPUTE a op b;
 * L17 INVERTS it — given an equation with one unknown and one operation, solve
 * for the unknown by applying the inverse operation. This is a genuine reasoning
 * step (the unknown is never on the answer side to begin with), not a lookup. It
 * reuses the same number parsing/formatting as mod_arith, and is symbolic — the
 * equation "x - 4 = 1" is identical in any language, so the bilingual ratchet is
 * almost free (only leading filler words differ). Operators: + - * / (symbols)
 * and the plus/minus/times words, EN+IT. '-' is always a binary operator here
 * (no negative-literal operands), which keeps one-step school algebra simple. */
static char algebra_op(const char *s) {
    if (!strcmp(s, "+") || !strcmp(s, "plus")  || !strcmp(s, "più"))  return '+';
    if (!strcmp(s, "-") || !strcmp(s, "minus") || !strcmp(s, "meno")) return '-';
    if (!strcmp(s, "*") || !strcmp(s, "times") || !strcmp(s, "per"))  return '*';
    if (!strcmp(s, "/")) return '/';
    return 0;
}

/* Split on whitespace and on the operator/equals chars (+ - * / =), each of the
 * latter emitted as its own one-char token. Tokens point into `store`. */
static size_t algebra_tokenize(const char *s, char store[][KB_TERM_LEN],
                               char *toks[], size_t max) {
    size_t n = 0, k = 0;
    while (s[k] && n < max) {
        while (s[k] && isspace((unsigned char)s[k])) k++;
        if (!s[k]) break;
        char c = s[k];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=') {
            store[n][0] = c; store[n][1] = '\0'; toks[n] = store[n]; n++; k++;
            continue;
        }
        size_t m = 0;
        while (s[k] && !isspace((unsigned char)s[k]) && s[k] != '+' &&
               s[k] != '-' && s[k] != '*' && s[k] != '/' && s[k] != '=' &&
               m + 1 < KB_TERM_LEN) {
            store[n][m++] = s[k++];
        }
        store[n][m] = '\0'; toks[n] = store[n]; n++;
    }
    return n;
}

static int algebra_is_filler(const char *s) {
    static const char *const f[] = {
        "solve","find","what","is","the","value","of","for","if","compute",
        "calculate","please","equation",
        "risolvi","trova","quanto","vale","il","valore","di","se","calcola",
        "equazione", NULL };
    return matches_any(s, f);
}

static int mod_algebra(Brain *b, const char *norm, const char *raw,
                       char *out, size_t out_size) {
    (void)raw;
    if (!strchr(norm, '=')) return 0;            /* an equation has '=' */

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[--len] = '\0';

    char store[24][KB_TERM_LEN]; char *t[24];
    size_t nt = algebra_tokenize(buf, store, t, 24);

    /* drop a leading run of filler words ("solve", "risolvi", ...). */
    size_t s0 = 0;
    while (s0 < nt && algebra_is_filler(t[s0])) s0++;
    char **tk = t + s0; size_t n = nt - s0;

    /* locate the single '=' */
    size_t eq = n;
    for (size_t i = 0; i < n; i++) if (strcmp(tk[i], "=") == 0) {
        if (eq != n) return 0;                   /* more than one '=' */
        eq = i;
    }
    if (eq == n) return 0;
    size_t ln = eq, rn = n - eq - 1;             /* token counts each side */

    /* gen113: one-step coefficient form, "5y = 20" -> y = 4. One side is a bare
     * number, the other a coefficient*variable token (a number alone would be
     * trivial, so we require the coefficient). */
    if (ln == 1 && rn == 1) {
        const char *lhs = tk[0], *rhs = tk[eq + 1];
        double lv, rv; int lnum = parse_value(lhs, &lv), rnum = parse_value(rhs, &rv);
        const char *unk = NULL; double val = 0;
        if (lnum && !rnum) { unk = rhs; val = lv; }
        else if (rnum && !lnum) { unk = lhs; val = rv; }
        if (!unk) return 0;
        size_t d = 0; while (isdigit((unsigned char)unk[d]) || unk[d] == '.') d++;
        if (d == 0 || unk[d] == '\0') return 0;  /* no coefficient -> trivial */
        char cb[32]; if (d >= sizeof cb) return 0;
        memcpy(cb, unk, d); cb[d] = '\0';
        double coef = strtod(cb, NULL);
        if (coef == 0) return 0;
        double res = val / coef; const char *var = unk + d;
        char num[64]; format_num(res, num, sizeof num);
        char msg[96]; snprintf(msg, sizeof msg, "%s = %s.", var, num);
        put(msg, out, out_size);
        char proof[256];
        snprintf(proof, sizeof proof, "%s = %g, so %s = %g / %g = %s.",
                 unk, val, var, val, coef, num);
        store_proof(b, proof);
        return 1;
    }

    /* exactly one side is "operand OP operand" (3 tokens), the other a lone
     * operand (1 token). Canonicalize to: a <op> b = c. */
    char *ta, *tb, *tc; char op;
    if (ln == 3 && rn == 1) {
        ta = tk[0]; op = algebra_op(tk[1]); tb = tk[2]; tc = tk[eq + 1];
    } else if (ln == 1 && rn == 3) {
        ta = tk[eq + 1]; op = algebra_op(tk[eq + 2]); tb = tk[eq + 3]; tc = tk[0];
    } else {
        return 0;
    }
    if (!op) return 0;

    /* exactly one of ta, tb, tc is the unknown (a non-numeric identifier).
     * Operands may be digits or number words ("x plus three = seven"). */
    double av, bv, cv;
    int na = parse_value(ta, &av), nb = parse_value(tb, &bv), nc = parse_value(tc, &cv);
    int unknowns = (!na) + (!nb) + (!nc);
    if (unknowns != 1) return 0;

    const char *x; double r; char rhs[96];   /* rhs = the inverse expression */
    if (!nc) {                                 /* tc unknown: just compute ta op tb */
        x = tc;
        switch (op) {
            case '+': r = av + bv; break;
            case '-': r = av - bv; break;
            case '*': r = av * bv; break;
            case '/': if (bv == 0) { put("I can't divide by zero.", out, out_size); return 1; }
                      r = av / bv; break;
            default: return 0;
        }
        snprintf(rhs, sizeof rhs, "%g %c %g", av, op, bv);
    } else if (!na) {                          /* ta unknown: invert around ta */
        x = ta;
        switch (op) {
            case '+': r = cv - bv; snprintf(rhs, sizeof rhs, "%g - %g", cv, bv); break;
            case '-': r = cv + bv; snprintf(rhs, sizeof rhs, "%g + %g", cv, bv); break;
            case '*': if (bv == 0) return 0; r = cv / bv; snprintf(rhs, sizeof rhs, "%g / %g", cv, bv); break;
            case '/': r = cv * bv; snprintf(rhs, sizeof rhs, "%g * %g", cv, bv); break;
            default: return 0;
        }
    } else {                                   /* tb unknown: invert around tb */
        x = tb;
        switch (op) {
            case '+': r = cv - av; snprintf(rhs, sizeof rhs, "%g - %g", cv, av); break;
            case '-': r = av - cv; snprintf(rhs, sizeof rhs, "%g - %g", av, cv); break;
            case '*': if (av == 0) return 0; r = cv / av; snprintf(rhs, sizeof rhs, "%g / %g", cv, av); break;
            case '/': if (cv == 0) return 0; r = av / cv; snprintf(rhs, sizeof rhs, "%g / %g", av, cv); break;
            default: return 0;
        }
    }

    /* gen113: two-step linear form — the unknown may carry a coefficient written
     * adjacently ("2x"). Then r is the value the term `coef*var` must equal, so
     * the variable is r / coef. This turns "2x + 1 = 7" into x = (7-1)/2 = 3. */
    char varname[KB_TERM_LEN]; double coef; int two_step = 0;
    {
        size_t d = 0;
        while (isdigit((unsigned char)x[d]) || x[d] == '.') d++;
        if (d > 0 && x[d] != '\0' && d < sizeof varname) {
            char cb[32]; if (d < sizeof cb) {
                memcpy(cb, x, d); cb[d] = '\0';
                coef = strtod(cb, NULL);
                if (coef != 0) {
                    snprintf(varname, sizeof varname, "%s", x + d);
                    r /= coef; two_step = 1;
                }
            }
        }
    }

    char num[64]; format_num(r, num, sizeof num);
    char msg[96];
    snprintf(msg, sizeof msg, "%s = %s.", two_step ? varname : x, num);
    put(msg, out, out_size);

    char proof[256];
    if (two_step)
        snprintf(proof, sizeof proof,
                 "%s %c %s = %s, so %s = %s, and %s = %s.",
                 ta, op, tb, tc, x, rhs, varname, num);
    else
        snprintf(proof, sizeof proof,
                 "%s %c %s = %s, so %s = %s = %s.", ta, op, tb, tc, x, rhs, num);
    store_proof(b, proof);
    return 1;
}

/* --- module: plan (L13) ---------------------------------------------------
 * Ordered procedure to a goal: a tiny planner. Prerequisites are taught as
 * `requires(Goal, Step)` facts in ANY order ("cake requires batter", "batter
 * requires eggs"); asking "how do I make cake?" returns a correctly ORDERED
 * plan, derived by a depth-first topological sort of the dependency DAG so every
 * step precedes what depends on it. The sequence is never stored — it is
 * recomputed from the scattered facts each time, so a goal the system was taught
 * only as loose prerequisites yields a coherent procedure (anti-impostor: the
 * order is reasoned, not recited). Cycles are detected and reported. */
static int plan_dfs(Brain *b, const char *node, const char *parent,
                    char done[][KB_TERM_LEN], size_t *ndone,
                    char stack[][KB_TERM_LEN], size_t depth,
                    char order[][KB_TERM_LEN], char par[][KB_TERM_LEN],
                    size_t *norder, size_t maxn) {
    for (size_t i = 0; i < *ndone; i++)
        if (strcmp(done[i], node) == 0) return 1;        /* already placed */
    for (size_t i = 0; i < depth; i++)
        if (strcmp(stack[i], node) == 0) return 0;       /* back-edge => cycle */
    if (depth >= maxn) return 0;
    snprintf(stack[depth], KB_TERM_LEN, "%s", node);

    char pre[64][KB_TERM_LEN];
    const char *pat[] = { node, NULL };
    size_t k = kb_match(b->kb, "requires", pat, 2, pre, 64);
    for (size_t j = 0; j < k; j++)
        if (!plan_dfs(b, pre[j], node, done, ndone, stack, depth + 1,
                      order, par, norder, maxn))
            return 0;

    if (*norder < maxn && *ndone < maxn) {
        snprintf(order[*norder], KB_TERM_LEN, "%s", node);
        snprintf(par[*norder], KB_TERM_LEN, "%s", parent);   /* who needs it */
        (*norder)++;
        snprintf(done[*ndone], KB_TERM_LEN, "%s", node);     (*ndone)++;
    }
    return 1;
}

/* gen110: learn a prerequisite LIST from one sentence — conjunction and optional
 * quantities. "cake requires eggs and flour" -> two requires() facts; "batter
 * requires 3 eggs and 2 flour" also records amount(batter, eggs, 3) etc. Each
 * item is an optional leading number then a single step token; a coordinating
 * conjunction (queried from the KB conjunction/1 class, gen193) and the
 * list-filler "of"/"di" are skipped. Returns the count learned and writes the
 * reply. */
static size_t plan_learn_list(Brain *b, const char *goal, char **w,
                              size_t start, size_t nw, char *out, size_t out_size) {
    long pend = -1; size_t learned = 0; char last_step[KB_TERM_LEN] = "";
    for (size_t i = start; i < nw; i++) {
        char *tk = strip_edge_punct(w[i]);
        if (!*tk) continue;
        if (is_conjunction(b, tk) || !strcmp(tk, "of") || !strcmp(tk, "di")) continue;
        double v;
        if (parse_num(tk, &v)) { pend = (long)v; continue; }
        const char *ar[] = { goal, tk };
        kb_assert(b->kb, "requires", ar, 2);
        if (pend >= 0) {
            char qs[24]; snprintf(qs, sizeof qs, "%ld", pend);
            const char *aq[] = { goal, tk, qs };
            kb_assert(b->kb, "amount", aq, 3);
        }
        pend = -1; learned++;
        snprintf(last_step, sizeof last_step, "%s", tk);
    }
    if (learned == 0) return 0;
    char msg[256];
    if (learned == 1)
        snprintf(msg, sizeof msg, "Learned: requires(%s, %s).", goal, last_step);
    else
        snprintf(msg, sizeof msg, "Learned %zu prerequisites for %s.", learned, goal);
    put(msg, out, out_size);
    return learned;
}

/* gen229 (LLMSCORE behavioural resemblance): counting as a genuine CAPABILITY,
 * never a stored phrasebook. An LLM trivially "counts to five"; parrot0 should
 * too, structurally — read a target (and optional start) as a digit or a small
 * number-word and GENERATE the sequence "1, 2, 3, 4, 5.". Honest and bounded
 * (the engine computes it; nothing is recited). EN+IT cues. This is the same
 * kind of honest competence as mod_arith's "2 plus 2", not an identity claim. */
static int word_to_int(const char *s, long *out) {
    if (*s && (isdigit((unsigned char)*s) ||
               ((*s == '-' || *s == '+') && isdigit((unsigned char)s[1])))) {
        char *end; long v = strtol(s, &end, 10);
        if (*end == '\0') { *out = v; return 1; }
        return 0;
    }
    static const struct { const char *w; long n; } NW[] = {
        {"zero",0},{"one",1},{"two",2},{"three",3},{"four",4},{"five",5},
        {"six",6},{"seven",7},{"eight",8},{"nine",9},{"ten",10},{"eleven",11},
        {"twelve",12},{"thirteen",13},{"fourteen",14},{"fifteen",15},
        {"sixteen",16},{"seventeen",17},{"eighteen",18},{"nineteen",19},
        {"twenty",20},{"thirty",30},{"forty",40},{"fifty",50},{"sixty",60},
        {"seventy",70},{"eighty",80},{"ninety",90},{"hundred",100},
        {"uno",1},{"due",2},{"tre",3},{"quattro",4},{"cinque",5},{"sei",6},
        {"sette",7},{"otto",8},{"nove",9},{"dieci",10},{"venti",20},{"cento",100},
        {NULL,0}
    };
    for (size_t i = 0; NW[i].w; i++)
        if (!strcmp(s, NW[i].w)) { *out = NW[i].n; return 1; }
    return 0;
}

static int mod_count(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw; (void)b;
    const char *buf = norm;
    int has_cue = cue(buf, "count to") || cue(buf, "count up to") ||
                  cue(buf, "count from") || cue(buf, "count backwards") ||
                  cue(buf, "count backward") || cue(buf, "count down") ||
                  cue(buf, "counting to") || cue(buf, "conta fino") ||
                  cue(buf, "conta da") || cue(buf, "conta fino a");
    if (!has_cue) return 0;
    if (cue(buf, "what comes next") || cue(buf, "comes next") ||
        cue(buf, "next number") || cue(buf, "next term"))
        return 0; /* let mod_sequence infer from the provided terms */
    int descending = cue(buf, "count down") || cue(buf, "backwards") ||
                     cue(buf, "backward") || cue(buf, "all'indietro") ||
                     cue(buf, "all indietro");

    char tmp[512]; snprintf(tmp, sizeof tmp, "%s", buf);
    long nums[8]; size_t nn = 0; char *save = NULL;
    /* gen241 (LLMSCORE-check): a "by N(s)" / "in steps of N" / "every N" group is the
     * STEP, not a bound. Capture it and drop that number from the start/end list, so
     * "count backward from 20 by 3s" -> 20, 17, 14, ... not a unit countdown. */
    long stepmag = 0;
    {
        char sb[512]; snprintf(sb, sizeof sb, "%s", buf);
        char *sw[64]; size_t snw = split_words(sb, sw, 64);
        for (size_t i = 0; i + 1 < snw; i++) {
            char *t = strip_edge_punct(sw[i]);
            /* "of" is a step only in "steps of N" -- NOT in "multiples of N" (a skip
             * filter). Guard on the previous token (gen241). */
            int of_step = !strcmp(t, "of") && i > 0 &&
                          (!strcmp(strip_edge_punct(sw[i - 1]), "steps") ||
                           !strcmp(strip_edge_punct(sw[i - 1]), "step"));
            int by_step = !strcmp(t, "by") &&
                          !(i > 0 && !strcmp(strip_edge_punct(sw[i - 1]), "divisible"));
            if (by_step || !strcmp(t, "every") || of_step) {
                char nx[64]; snprintf(nx, sizeof nx, "%s", strip_edge_punct(sw[i + 1]));
                size_t nl = strlen(nx);            /* "3s" -> "3" */
                if (nl > 1 && nx[nl - 1] == 's') nx[nl - 1] = '\0';
                long v; if (word_to_int(nx, &v) && v > 0) { stepmag = v; break; }
            }
        }
    }
    for (char *t = strtok_r(tmp, " \t,.;:!?", &save);
         t && nn < 8; t = strtok_r(NULL, " \t,.;:!?", &save)) {
        long v; if (word_to_int(t, &v)) {
            /* drop the step value itself when it appears as a standalone number */
            if (stepmag && v == stepmag && nn >= 1) continue;
            nums[nn++] = v;
        }
    }
    if (nn == 0) return 0;  /* a count cue with no number is not ours */

    long start, end;
    if (nn >= 2)        { start = nums[0]; end = nums[1]; }
    else if (descending){ start = nums[0]; end = stepmag ? 0 : 1; } /* by-step -> toward 0 */
    else                { start = 1;       end = nums[0]; }
    if (stepmag && nn < 2 && !descending) end = start + stepmag * 9; /* ~10 terms */

    long span = start <= end ? end - start : start - end;
    if (span > 99) {
        put("That's a long way to count. Give me a smaller range and I'll list it.",
            out, out_size);
        return 1;
    }
    int only_odd = cue(buf, "only the odd") || cue(buf, "only odd") || cue(buf, "odd numbers");
    int only_even = cue(buf, "only the even") || cue(buf, "only even") || cue(buf, "even numbers");

    /* gen241 (LLMSCORE-check): a SKIP filter. "skip any number that ends in 5" /
     * "skip multiples of 3" -> drop matching terms while counting. The digit/divisor
     * is read from after the relevant phrase; honest deductive filtering, not a memo. */
    int skip_ends = -1, skip_mult = 0;
    if (cue(buf, "skip") || cue(buf, "except") || cue(buf, "but not") ||
        cue(buf, "leave out") || cue(buf, "omit")) {
        char fb[512]; snprintf(fb, sizeof fb, "%s", buf);
        char *fw[64]; size_t fnw = split_words(fb, fw, 64);
        for (size_t i = 0; i + 1 < fnw; i++) {
            char *t = strip_edge_punct(fw[i]);
            if ((!strcmp(t, "in") || !strcmp(t, "with") || !strcmp(t, "ends")) &&
                (cue(buf, "ends in") || cue(buf, "ending in") || cue(buf, "end in") ||
                 cue(buf, "ends with") || cue(buf, "ending with"))) {
                long d; if (word_to_int(strip_edge_punct(fw[i + 1]), &d) && d >= 0 && d <= 9)
                    skip_ends = (int)d;
            }
            if ((!strcmp(t, "of") || !strcmp(t, "multiple") || !strcmp(t, "multiples")) &&
                (cue(buf, "multiple of") || cue(buf, "multiples of"))) {
                long m; if (word_to_int(strip_edge_punct(fw[i + 1]), &m) && m > 0)
                    skip_mult = (int)m;
            }
        }
    }

    /* gen251: replacement filter. "say buzz instead of any number divisible by 3"
     * keeps the count range intact and substitutes matching terms instead of
     * dropping them (distinct from the skip filter above). */
    char repl[32] = "";
    int repl_mult = 0;
    if ((cue(buf, "instead of") || cue(buf, "replace")) && cue(buf, "say")) {
        char rb[512]; snprintf(rb, sizeof rb, "%s", buf);
        char *rw[64]; size_t rnw = split_words(rb, rw, 64);
        for (size_t i = 0; i < rnw; i++) {
            char *t = strip_edge_punct(rw[i]);
            if (!strcmp(t, "say") && i + 1 < rnw && !repl[0]) {
                char *word = strip_edge_punct(rw[i + 1]);
                if (*word && strlen(word) < sizeof repl) snprintf(repl, sizeof repl, "%s", word);
            }
            if (!strcmp(t, "divisible") && i + 2 < rnw &&
                !strcmp(strip_edge_punct(rw[i + 1]), "by")) {
                long m;
                if (word_to_int(strip_edge_punct(rw[i + 2]), &m) && m > 0)
                    repl_mult = (int)m;
            }
            if ((!strcmp(t, "multiple") || !strcmp(t, "multiples")) &&
                i + 2 < rnw && !strcmp(strip_edge_punct(rw[i + 1]), "of")) {
                long m;
                if (word_to_int(strip_edge_punct(rw[i + 2]), &m) && m > 0)
                    repl_mult = (int)m;
            }
        }
    }

    char line[1024]; size_t pos = 0; line[0] = '\0';
    long mag = stepmag > 0 ? stepmag : 1;
    long step = (start <= end) ? mag : -mag;
    size_t emitted = 0;
    for (long v = start; ; v += step) {
        /* stop once we'd pass the end bound (step may overshoot it exactly). */
        if ((step > 0 && v > end) || (step < 0 && v < end)) break;
        long av = v < 0 ? -v : v;
        int skip = (skip_ends >= 0 && (av % 10) == skip_ends) ||
                   (skip_mult > 0 && (av % skip_mult) == 0);
        if (!skip && (!only_odd || (v % 2 != 0)) && (!only_even || (v % 2 == 0))) {
            char nbuf[40];
            const char *term = nbuf;
            if (repl[0] && repl_mult > 0 && (av % repl_mult) == 0)
                term = repl;
            else
                snprintf(nbuf, sizeof nbuf, "%ld", v);
            int w = snprintf(line + pos, sizeof line - pos, "%s%s",
                             emitted ? ", " : "", term);
            if (w < 0 || (size_t)w >= sizeof line - pos) break;
            pos += (size_t)w;
            emitted++;
        }
        if (emitted > 200) break;
    }
    if (pos + 2 <= sizeof line) { line[pos++] = '.'; line[pos] = '\0'; }
    put(line, out, out_size);
    return 1;}

/* gen230 (LLMSCORE): "name a <category> that starts with <letter>" — a grounded
 * generative capability. The members live in the KB (category_member/2,
 * world-facts.p0), never in C; the recognizer reads the category and the target
 * initial and returns the first known member with that initial. Honest: it can
 * only name what it actually knows, and says so when it knows none. Not a
 * phrasebook — add a category_member fact and the capability extends for free. */
/* gen294 (cat.52): read a category's members for a (plural) head noun, robust to
 * English singularization edge cases — singularize("senses") wrongly gives "sens"
 * (the boxes->box rule), so a strip-one-'s' fallback recovers "sense". Tries a
 * compound qualifier first ("primary colors" -> primary_color), then the
 * singularized head, then the head minus a trailing 's', then the raw token.
 * Returns the member count (0 if the noun names no category). KB-first: the
 * members live in category_member/2, so a new set extends "list the X" for free. */
static size_t enum_category_lookup(Brain *b, const char *prevtok,
                                   const char *rawhead,
                                   char members[][KB_TERM_LEN], size_t max) {
    if (!b || !b->kb || !rawhead || !*rawhead) return 0;
    char head[KB_TERM_LEN];
    singularize(rawhead, head, sizeof head);
    if (prevtok && *prevtok) {
        char comp[KB_TERM_LEN];
        snprintf(comp, sizeof comp, "%s_%s", prevtok, head);
        const char *cp[2] = { comp, NULL };
        size_t k = kb_match(b->kb, "category_member", cp, 2, members, max);
        if (k) return k;
    }
    char s1[KB_TERM_LEN];
    snprintf(s1, sizeof s1, "%s", rawhead);
    size_t sl = strlen(s1);
    if (sl > 1 && s1[sl - 1] == 's') s1[sl - 1] = '\0'; else s1[0] = '\0';
    const char *cands[3] = { head, s1, rawhead };
    for (size_t c = 0; c < 3; c++) {
        if (!cands[c] || !cands[c][0]) continue;
        const char *pat[2] = { cands[c], NULL };
        size_t k = kb_match(b->kb, "category_member", pat, 2, members, max);
        if (k) return k;
    }
    return 0;
}

/* gen294: format up to `lim` members into "A, B, and C." with an initial cap.
 * A member stored quoted (a multi-word atom like "north america") is dequoted
 * for display. */
static void enum_format(char members[][KB_TERM_LEN], size_t lim,
                        char *out, size_t out_size) {
    char msg[512]; size_t off = 0;
    for (size_t j = 0; j < lim && off + 2 < sizeof msg; j++) {
        const char *m = members[j];
        char dq[KB_TERM_LEN];
        size_t ml = strlen(m);
        if (ml >= 2 && m[0] == '"' && m[ml - 1] == '"') {
            snprintf(dq, sizeof dq, "%.*s", (int)(ml - 2), m + 1);
            m = dq;
        }
        off += (size_t)snprintf(msg + off, sizeof msg - off, "%s%s",
            j ? (j + 1 == lim ? ", and " : ", ") : "", m);
    }
    if (off + 2 < sizeof msg) snprintf(msg + off, sizeof msg - off, ".");
    if (msg[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
    put(msg, out, out_size);
}

static int mod_namestart(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb) return 0;
    const char *buf = norm;

    /* gen241 (LLMSCORE-check): "name three primary colors" / "list two animals" — a
     * COUNTED pick. The members live in the KB (category_member/2); the C reads the
     * count word and the category noun and returns that many distinct members. KB-first:
     * add a category_member fact and the capability extends for free. */
    if (cue(buf, "name ") || cue(buf, "list ") || cue(buf, "give me ") ||
        cue(buf, "tell me ") ||
        /* gen254: the interrogative form of the same intent — "WHAT ARE the
         * three primary colors?" is the counted pick phrased as a question. */
        cue(buf, "what are") || cue(buf, "which are")) {
        if (cue(buf, "border") || cue(buf, "neighbour") || cue(buf, "neighbor"))
            return 0;
        static const struct { const char *w; int n; } nums[] = {
            {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6},
            {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10},
            {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6},
            {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10}, {NULL, 0} };
        int want = 0;
        char nb[256]; snprintf(nb, sizeof nb, "%s", buf);
        char *nw0[64]; size_t nn0 = split_words(nb, nw0, 64);
        size_t numpos = nn0;
        for (size_t i = 0; i < nn0 && !want; i++) {
            char *t = strip_edge_punct(nw0[i]);
            for (size_t k = 0; nums[k].w; k++)
                if (!strcmp(t, nums[k].w)) { want = nums[k].n; numpos = i; break; }
        }
        if (want >= 2 && numpos + 1 < nn0) {
            /* category: the (possibly multi-word) noun right after the count; try the
             * last token first (the head noun), singularized — "primary colors"->color. */
            for (size_t i = nn0; i-- > numpos + 1;) {
                char *prevtok = (i > numpos + 1) ? strip_edge_punct(nw0[i - 1]) : NULL;
                char members[64][KB_TERM_LEN];
                size_t k = enum_category_lookup(b, prevtok,
                                                strip_edge_punct(nw0[i]), members, 64);
                if (k == 0) continue;
                size_t lim = (size_t)want < k ? (size_t)want : k;
                enum_format(members, lim, out, out_size);
                return 1;
            }
        }

        /* gen294 (cat.52): the WHOLE-SET form — "list the days of the week",
         * "what are the continents", "name the primary colors": the SAME outer
         * trigger (name/list/what are/…) but NO count and a definite "the"
         * (the whole-set signal). Enumerate ALL members. The head noun is found by
         * scanning backward (so "colors of the rainbow" finds rainbow before
         * color), and only a genuine category_member set is claimed — any
         * non-category turn ("what are you") matches nothing and falls through.
         * A single "the" token gates it (not a cue OR-chain), so recognized
         * vocabulary stays out of C per the cuechains ratchet. */
        int has_the = 0;
        for (size_t i = 0; i < nn0 && !has_the; i++)
            if (!strcmp(strip_edge_punct(nw0[i]), "the")) has_the = 1;
        if (want == 0 && has_the) {
            for (size_t i = nn0; i-- > 0;) {
                char *tok = strip_edge_punct(nw0[i]);
                /* the head must be PLURAL (a set request lists many): require a
                 * trailing 's', so "the largest PLANET and a moon" (singular
                 * superlative) is left to the superlative handler, while "the
                 * PLANETS" enumerates. */
                size_t tl = strlen(tok);
                if (tl < 2 || tok[tl - 1] != 's') continue;
                char *prevtok = (i > 0) ? strip_edge_punct(nw0[i - 1]) : NULL;
                char members[64][KB_TERM_LEN];
                size_t k = enum_category_lookup(b, prevtok, tok, members, 64);
                if (k == 0) continue;
                enum_format(members, k < 32 ? k : 32, out, out_size);
                return 1;
            }
        }
    }

    int has_name = cue(buf, "name a") || cue(buf, "name an") ||
                   cue(buf, "name any") || cue(buf, "name me a") ||
                   cue(buf, "give me a") || cue(buf, "tell me a") ||
                   cue(buf, "can you name");
    if (!has_name) return 0;
    /* gen240: a relational constraint ("name a country that BORDERS X") is beyond
     * a plain category pick — defer to the borders handler downstream rather than
     * returning an arbitrary member that ignores the constraint. */
    if (cue(buf, "border") || cue(buf, "neighbour") || cue(buf, "neighbor")) return 0;

    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
    char *w[64]; size_t nw = split_words(tmp, w, 64);

    /* OPTIONAL initial-letter constraint: token after "letter", else after "with". */
    char init = 0;
    if (cue(buf,"start with")||cue(buf,"starts with")||cue(buf,"starting with")||
        cue(buf,"begin with")||cue(buf,"begins with")||cue(buf,"beginning with")) {
        size_t li = find_token(w, nw, "letter");
        if (li != nw && li + 1 < nw) init = w[li + 1][0];
        if (!init) {
            size_t wi = find_token(w, nw, "with");
            if (wi != nw && wi + 1 < nw) init = strip_edge_punct(w[wi + 1])[0];
        }
    }

    /* category: the noun right after the article (a/an/any) following "name". */
    const char *category = NULL;
    size_t ni = find_token(w, nw, "name");
    for (size_t i = (ni == nw ? 0 : ni); i + 1 < nw; i++)
        if (!strcmp(w[i], "a") || !strcmp(w[i], "an") || !strcmp(w[i], "any")) {
            category = strip_edge_punct(w[i + 1]); break;
        }
    if (!category || !*category) return 0;

    const char *pat[2] = { category, NULL };
    char members[64][KB_TERM_LEN];
    size_t k = kb_match(b->kb, "category_member", pat, 2, members, 64);
    if (k == 0) return 0;   /* unknown category: let an honest wall handle it */

    if (init) {             /* constrained: first member with that initial */
        for (size_t i = 0; i < k; i++)
            if (members[i][0] == init) {
                char msg[160]; snprintf(msg, sizeof msg, "%s.", members[i]);
                put(msg, out, out_size); return 1;
            }
        char msg[200];
        snprintf(msg, sizeof msg,
                 "I can't think of a %s starting with '%c' from what I know.",
                 category, init);
        put(msg, out, out_size);
        return 1;
    }

    /* unconstrained ("name any animal"): return a member, rotating for variety. */
    size_t idx = b->response_pick % k;
    b->response_pick++;
    char msg[160]; snprintf(msg, sizeof msg, "%s.", members[idx]);
    put(msg, out, out_size);
    return 1;
}

/* gen231 (LLMSCORE, ambitious): continue a number sequence. "what comes next 2 4
 * 6 8" -> 10. Detects an arithmetic (constant difference) or geometric (constant
 * ratio) progression from >=3 given terms and extends it by one — the rule is
 * computed from the data, not guessed. Honest: declines when the terms fit no
 * simple rule rather than inventing a continuation. */
static int mod_sequence(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw; (void)b;
    const char *buf = norm;
    if (!(cue(buf, "comes next") || cue(buf, "come next") ||
          cue(buf, "next number") || cue(buf, "next term") ||
          cue(buf, "next in the") || cue(buf, "continue the sequence") ||
          cue(buf, "continue the pattern") || cue(buf, "continue this sequence") ||
          cue(buf, "viene dopo") || cue(buf, "numero successivo")))
        return 0;

    const char *seq_src = strchr(buf, ':');
    seq_src = seq_src ? seq_src + 1 : buf;
    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", seq_src);
    char *w[64]; size_t nw = split_words(tmp, w, 64);
    double seq[32]; size_t ns = 0;
    for (size_t i = 0; i < nw && ns < 32; i++) {
        double v;
        if (parse_num(strip_edge_punct(w[i]), &v)) seq[ns++] = v;
    }

    /* gen240 (LLMSCORE): LETTER sequences. With no numbers, convert single-letter
     * terms to alphabet positions (A=1..Z=26) and run the SAME arithmetic/geometric
     * engine, emitting the next letter. Terms taken after the ':' if present, so
     * frame words ("a"/"i") don't pollute. Honest: a non-constant pattern (e.g. the
     * "straight-line letters" puzzle) fits no simple rule and declines, per the
     * manifesto — we never invent a continuation. */
    int letters = 0;
    if (ns < 3) {
        const char *colon = strchr(buf, ':');
        const char *scan = colon ? colon + 1 : buf;
        ns = 0;
        for (const char *p = scan; *p; p++) {
            if (isalpha((unsigned char)p[0]) &&
                !isalpha((unsigned char)(p == scan ? ' ' : p[-1])) &&
                !isalpha((unsigned char)p[1])) {
                if (ns < 32) seq[ns++] = (double)(tolower((unsigned char)p[0]) - 'a' + 1);
            }
        }
        if (ns >= 3) letters = 1; else return 0;
    }
    if (ns < 3) return 0;   /* too few terms to infer a rule honestly */

    const double EPS = 1e-9;
    double d = seq[1] - seq[0];
    int arith = 1;
    for (size_t i = 2; i < ns; i++) {
        double e = (seq[i] - seq[i - 1]) - d;
        if (e > EPS || e < -EPS) { arith = 0; break; }
    }
    double nextv = 0; int ok = 0;
    if (arith) { nextv = seq[ns - 1] + d; ok = 1; }
    else if (seq[0] != 0) {
        double r = seq[1] / seq[0]; int geo = 1;
        for (size_t i = 2; i < ns; i++) {
            if (seq[i - 1] == 0) { geo = 0; break; }
            double e = (seq[i] / seq[i - 1]) - r;
            if (e > EPS || e < -EPS) { geo = 0; break; }
        }
        if (geo) { nextv = seq[ns - 1] * r; ok = 1; }
    }
    /* gen240: second-order arithmetic (constant SECOND difference), e.g.
     * 2,6,12,20,30 (diffs 4,6,8,10) -> next diff 12 -> 42. Needs >=4 terms. */
    if (!ok && ns >= 4) {
        double dd = (seq[2] - seq[1]) - (seq[1] - seq[0]); int quad = 1;
        for (size_t i = 3; i < ns; i++) {
            double e = ((seq[i] - seq[i - 1]) - (seq[i - 1] - seq[i - 2])) - dd;
            if (e > EPS || e < -EPS) { quad = 0; break; }
        }
        if (quad) { nextv = seq[ns - 1] + (seq[ns - 1] - seq[ns - 2]) + dd; ok = 1; }
    }
    if (!ok) return 0;   /* no simple rule: decline honestly */

    if (letters) {
        long pos = (long)(nextv + 0.5);
        if (pos < 1 || pos > 26) return 0;   /* runs off the alphabet: decline */
        char msg[16]; snprintf(msg, sizeof msg, "%c.", (char)('A' + pos - 1));
        put(msg, out, out_size);
        return 1;
    }

    char num[64]; format_num(nextv, num, sizeof num);
    char msg[80]; snprintf(msg, sizeof msg, "%s.", num);
    put(msg, out, out_size);
    return 1;
}

/* gen231 (LLMSCORE): spell a word letter by letter — "spell necessary" ->
 * "n-e-c-e-s-s-a-r-y". A structural lexical capability (the word is the data, not a
 * stored list); spells the last real word after the cue. Declines if none is found. */
static int mod_spell(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)b; (void)raw;
    const char *buf = norm;

    /* gen246: sequential word transformation. The word is data from the prompt;
     * operations are structural ("remove first/last letter", "add letter X to
     * end/start"), so held-out words transfer. */
    if ((cue(buf, "letters in") || cue(buf, "letters of")) &&
        cue(buf, "reverse") && cue(buf, "alphabet")) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
        char *w[64]; size_t nw = split_words(tmp, w, 64);
        char word[128] = "";
        for (size_t i = 0; i + 1 < nw && !word[0]; i++) {
            char *t = strip_edge_punct(w[i]);
            if (strcmp(t, "in") && strcmp(t, "of")) continue;
            char *cand = strip_edge_punct(w[i + 1]);
            int alpha = 1;
            for (size_t k = 0; cand[k]; k++)
                if (!isalpha((unsigned char)cand[k])) { alpha = 0; break; }
            if (alpha && *cand) snprintf(word, sizeof word, "%s", cand);
        }
        if (word[0]) {
            size_t n = strlen(word);
            for (size_t i = 0; i < n; i++) {
                for (size_t j = i + 1; j < n; j++) {
                    if (tolower((unsigned char)word[j]) > tolower((unsigned char)word[i])) {
                        char c = word[i]; word[i] = word[j]; word[j] = c;
                    }
                }
            }
            char msg[160]; snprintf(msg, sizeof msg, "%s.", word);
            put(msg, out, out_size);
            return 1;
        }
    }

    if ((cue(buf, "take the word") || cue(buf, "the word") || cue(buf, "word \"")) &&
        (cue(buf, "remove") || cue(buf, "drop")) &&
        cue(buf, "letter") && cue(buf, "add")) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
        char *w[64]; size_t nw = split_words(tmp, w, 64);
        char word[128] = "", add[8] = "";
        for (size_t i = 0; i + 1 < nw && !word[0]; i++) {
            if (strcmp(strip_edge_punct(w[i]), "word") != 0) continue;
            char *t = strip_edge_punct(w[i + 1]);
            int alpha = 1;
            for (size_t k = 0; t[k]; k++)
                if (!isalpha((unsigned char)t[k])) { alpha = 0; break; }
            if (alpha && strlen(t) > 1) snprintf(word, sizeof word, "%s", t);
        }
        for (size_t i = 0; i < nw && !add[0]; i++) {
            if (strcmp(strip_edge_punct(w[i]), "add") != 0) continue;
            for (size_t j = i + 1; j < nw; j++) {
                char *t = strip_edge_punct(w[j]);
                if (!strcmp(t, "the") || !strcmp(t, "letter") || !strcmp(t, "to") ||
                    !strcmp(t, "end") || !strcmp(t, "beginning") || !strcmp(t, "start") ||
                    !strcmp(t, "of")) continue;
                if (strlen(t) == 1 && isalpha((unsigned char)t[0])) {
                    snprintf(add, sizeof add, "%s", t);
                    break;
                }
            }
        }
        if (word[0]) {
            char res[160]; snprintf(res, sizeof res, "%s", word);
            size_t rl = strlen(res);
            if ((cue(buf, "remove the first") || cue(buf, "drop the first") ||
                 cue(buf, "remove first") || cue(buf, "drop first")) && rl > 0) {
                memmove(res, res + 1, rl);
                rl--;
            }
            if ((cue(buf, "remove the last") || cue(buf, "drop the last") ||
                 cue(buf, "remove last") || cue(buf, "drop last")) && rl > 0) {
                res[--rl] = '\0';
            }
            if (add[0] && (cue(buf, "to the end") || cue(buf, "at the end") ||
                           cue(buf, "end"))) {
                snprintf(res + strlen(res), sizeof res - strlen(res), "%s", add);
            } else if (add[0] && (cue(buf, "to the start") ||
                                  cue(buf, "to the beginning") ||
                                  cue(buf, "beginning") || cue(buf, "start"))) {
                char tmp2[160]; snprintf(tmp2, sizeof tmp2, "%s%s", add, res);
                snprintf(res, sizeof res, "%s", tmp2);
            }
            char msg[180]; snprintf(msg, sizeof msg, "%s.", res);
            put(msg, out, out_size);
            return 1;
        }
    }

    if (!(cue(buf, "spell ") || cue(buf, "how do you spell") ||
          cue(buf, "can you spell") || cue(buf, "come si scrive")))
        return 0;
    /* gen240: don't misfire on an anagram/rearrange task that merely mentions
     * "spell" in an example ("rearrange Listen to spell Silent — now do X"). That
     * is not a spelling request; spelling a stray word there is nonsense. */
    if (cue(buf, "rearrange") || cue(buf, "anagram") || cue(buf, "unscramble") ||
        cue(buf, "scrambled") || cue(buf, "form a word") || cue(buf, "real word"))
        return 0;

    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
    char *w[64]; size_t nw = split_words(tmp, w, 64);
    /* the word to spell is the last all-alphabetic token that isn't a frame word. */
    const char *target = NULL;
    for (size_t i = nw; i-- > 0; ) {
        char *t = strip_edge_punct(w[i]);
        size_t tl = strlen(t);
        if (tl < 2) continue;
        int alpha = 1;
        for (size_t k = 0; k < tl; k++)
            if (!isalpha((unsigned char)t[k])) { alpha = 0; break; }
        if (!alpha) continue;
        if (!strcmp(t, "spell") || !strcmp(t, "word") || !strcmp(t, "the") ||
            !strcmp(t, "you") || !strcmp(t, "how") || !strcmp(t, "can") ||
            !strcmp(t, "please") || !strcmp(t, "scrive") || !strcmp(t, "come"))
            continue;
        target = t; break;
    }
    if (!target) return 0;

    char line[512]; size_t pos = 0; line[0] = '\0';
    for (size_t i = 0; target[i] && pos + 3 < sizeof line; i++) {
        if (i) line[pos++] = '-';
        line[pos++] = target[i];
    }
    line[pos] = '\0';
    put(line, out, out_size);
    return 1;
}
