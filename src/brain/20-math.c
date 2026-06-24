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

static int mod_arith(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)b; (void)raw;

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
        size_t mark = cnw;
        for (size_t i = 0; i < cnw; i++)
            if (!strcmp(cw[i], "percent") || !strcmp(cw[i], "%") ||
                !strcmp(cw[i], "cento")) { mark = i; break; }
        if (mark < cnw) {
            double pct = 0, base = 0; int havep = 0, haveb = 0;
            for (size_t i = mark; i-- > 0; ) {
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
            else if (!strcmp(cw[i],"even") || !strcmp(cw[i],"pari")) wants_even = 1;
            else if (!strcmp(cw[i],"odd") || !strcmp(cw[i],"dispari")) wants_odd = 1;
        }
        if (gn == 1 && (wants_prime || wants_even || wants_odd)) {
            for (size_t i = 0; i < cnw; i++) {
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

