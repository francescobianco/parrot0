/* A module-level guard is knowledge: compound_guard(Module, CueClass) says that
 * a richer schema owns this surface. The fixed engine only enumerates guards
 * and asks the shared matcher for evidence. Adding or retracting a guard changes
 * routing at runtime, without recompiling or naming vocabulary here. */
static int kb_module_guarded(Brain *b, const char *module, const char *norm) {
    if (!b || !b->kb || !module || !norm) return 0;
    char guards[64][KB_TERM_LEN];
    const char *q[] = { module, NULL };
    size_t n = kb_match(b->kb, "compound_guard", q, 2, guards, 64);
    for (size_t i = 0; i < n; i++)
        if (kb_cue_match(b, kb_dequote(guards[i]), norm)) return 1;
    return 0;
}

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
    if (nw != 5 || !lex_class_member(b, "20_math_lex26", w[0]) || !lex_class_member(b, "20_math_lex26_2", w[3]))
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

static char arith_op_char(Brain *b, const char *s) {
    if (!b || !b->kb || !s) return 0;
    /* Le superfici degli operatori stanno in KB CITATE — `infix_operator("plus",
     * plus)` — perche' fra loro ci sono simboli come "+" e "*". Interrogare con
     * il token nudo non trovava mai niente, e da gen443 «what is 2 plus 2?»
     * rispondeva «I don't understand that yet.»: la lista era passata alla
     * conoscenza, la chiave di lettura era rimasta indietro. */
    char quoted[KB_TERM_LEN];
    snprintf(quoted, sizeof quoted, "\"%.*s\"",
             (int)(sizeof(quoted) - 3), s);
    const char *q[] = { quoted, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "infix_operator", q, 2, hit, 1) != 1) return 0;
    char hb[KB_TERM_LEN]; snprintf(hb, sizeof hb, "%s", hit[0]);
    const char *op = kb_dequote(hb);
    const char *sq[] = { op, NULL };
    char symbol[1][KB_TERM_LEN];
    if (kb_match(b->kb, "operator_symbol", sq, 2, symbol, 1) != 1) return 0;
    const char *sym_text = kb_dequote(symbol[0]);
    if (sym_text && strlen(sym_text) == 1) return sym_text[0];
    return 0;
}

static int is_arith_op(Brain *b, const char *s) {
    return arith_op_char(b, s) != 0;
}

/* A token repair is licensed by knowledge, while the named operation remains
 * fixed mechanics.  Querying the class with the operation bound means a new
 * variation can select this mechanic tomorrow without adding a C branch. */
static int token_variation_class(Brain *b, const char *operation,
                                 char *variation, size_t variation_size) {
    if (!b || !b->kb || !operation || !variation || variation_size == 0)
        return 0;
    char quoted[KB_TERM_LEN];
    snprintf(quoted, sizeof quoted, "\"%.*s\"",
             (int)(sizeof quoted - 3), operation);
    const char *q[] = { NULL, quoted };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "token_variation", q, 2, hit, 1) != 1) return 0;
    snprintf(variation, variation_size, "%s", hit[0]);
    return 1;
}

/* Keep the repair observable as evidence of this turn.  The original bytes
 * stay in utterance/3; this record says which reversible normalization the
 * arithmetic reader actually consumed and which KB class licensed it. */
static void note_turn_surface_repair(Brain *b, const char *variation,
                                     const char *operation,
                                     const char *original,
                                     const char *normalized) {
    if (!b || !b->kb || !variation || !operation || !original || !normalized)
        return;
    char oq[KB_TERM_LEN], nq[KB_TERM_LEN];
    snprintf(oq, sizeof oq, "\"%.*s\"", (int)(sizeof oq - 3), original);
    snprintf(nq, sizeof nq, "\"%.*s\"", (int)(sizeof nq - 3), normalized);
    const char *a[] = { variation, operation, oq, nq };
    int prev = kb_origin(b->kb);
    kb_set_origin(b->kb, KB_SESSION);
    kb_assert(b->kb, "turn_surface_repair", a, 4);
    kb_set_origin(b->kb, prev);
}

/* Apply an arithmetic operator, returning the result. Sets *ok=0 for unknown ops. */
static double apply_arith_op(Brain *b, const char *op, double a, double c, int *ok) {
    *ok = 0;
    if (!b || !b->kb || !op) return 0;
    char quoted[KB_TERM_LEN];
    snprintf(quoted, sizeof quoted, "\"%.*s\"", (int)(sizeof quoted - 3), op);
    const char *oq[] = { quoted, NULL };
    char opname[1][KB_TERM_LEN];
    if (kb_match(b->kb, "infix_operator", oq, 2, opname, 1) != 1)
        snprintf(opname[0], KB_TERM_LEN, "%s", op);
    char av[KB_TERM_LEN], cv[KB_TERM_LEN];
    snprintf(av, sizeof av, "%g", a); snprintf(cv, sizeof cv, "%g", c);
    const char *aq[] = { opname[0], av, cv, NULL };
    char result[1][KB_TERM_LEN];
    if (kb_match(b->kb, "apply_operator", aq, 4, result, 1) != 1) return 0;
    double value = 0;
    if (!parse_value(kb_dequote(result[0]), &value)) return 0;
    *ok = 1;
    return value;
}

static int algebra_inverse(Brain *b, const char *side, char op, double a,
                           double c, double *result, char *rhs, size_t rhs_n) {
    char sym[2] = { op, '\0' }, quoted[KB_TERM_LEN];
    snprintf(quoted, sizeof quoted, "\"%s\"", sym);
    const char *q[] = { side, quoted, NULL, NULL };
    char inv[1][KB_TERM_LEN];
    if (kb_match(b->kb, "algebra_inverse", q, 4, inv, 1) != 1) return 0;
    const char *sq[] = { side, quoted, inv[0], NULL };
    char render[1][KB_TERM_LEN];
    if (kb_match(b->kb, "algebra_inverse", sq, 4, render, 1) != 1) return 0;
    const char *oq[] = { side, quoted, NULL };
    char order[1][KB_TERM_LEN];
    if (kb_match(b->kb, "algebra_inverse_order", oq, 3, order, 1) != 1) return 0;
    int ok = 0;
    if (!strcmp(kb_dequote(order[0]), "swap")) {
        double tmp = a; a = c; c = tmp;
    }
    *result = apply_arith_op(b, kb_dequote(inv[0]), a, c, &ok);
    if (!ok) return 0;
    snprintf(rhs, rhs_n, "%g %s %g", a, kb_dequote(render[0]), c);
    return 1;
}

/* gen190: arithmetic in natural language. The catalogue (basic-chat cat.4) asks
 * the SAME four operations in many surface forms — "six times seven", "add 5 and
 * 7", "100 divided by 4", "half of 50". These are not new computations; they are
 * new ways of NAMING operands and operators. So instead of one phrase per shape,
 * the parser below extracts (operator, operands) structurally and folds them with
 * the existing oracle. Operator words are recognised in EN+IT (per/più/diviso),
 * so the bilingual probe rides the same path. */

/* Canonical operator char from a single operator word or symbol (EN+IT). */
/* TODO(kb-first): GLI OPERATORI SONO QUI, E NON DOVREBBERO ESSERCI.
 *
 * Questo `switch` e' la ragione per cui non si poteva insegnare un operatore
 * nuovo: aggiungerne uno voleva dire un `case` e una ricompilazione. A gen423 la
 * forma semplice «A op B» e' passata alla KB — `infix_operator/2` e
 * `apply_operator/4` in procedures.p0, dove `gcd` e `avg` esistono senza una
 * riga di C — ma tutto cio' che arriva QUI (espressioni composte, catene, la
 * divisione) calcola ancora cablato.
 *
 * La conversione va fatta per gradi e col cricchetto a proteggere ogni passo: il
 * consumatore generico c'e' gia' (`mod_operator`), manca che lo chiami anche chi
 * piega un'espressione con piu' operatori. Finche' resta cosi', un operatore
 * nuovo funziona da solo e non dentro un'espressione, e la differenza si vede
 * dalla firma del ragionamento (gen422): un turno che calcola senza interrogare
 * nessun predicato non e' KB-first, e ora e' un numero che si legge. */
static double apply_op_char(Brain *b, char op, double a, double c, int *ok) {
    char sym[2] = { op, '\0' };
    return apply_arith_op(b, sym, a, c, ok);
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

/* Read the first numeric value inside one byte span. Range vocabulary and span
 * boundaries come from the KB; this helper is only the fixed slot binder. */
static int arith_first_value_in_span(const char *text, size_t start, size_t end,
                                     double *out) {
    if (!text || !out || start >= end) return 0;
    size_t n = strlen(text);
    if (start >= n) return 0;
    if (end > n) end = n;
    if (end - start >= 256) return 0;
    char buf[256];
    memcpy(buf, text + start, end - start);
    buf[end - start] = '\0';
    char *w[32]; size_t nw = split_words(buf, w, 32);
    for (size_t i = 0; i < nw; i++)
        if (parse_value(strip_edge_punct(w[i]), out)) return 1;
    return 0;
}

/* Bind the two values of the first ordered range frame recognized by the KB.
 *
 *   range_frame(OpenCueClass, CloseCueClass)
 *
 * names compatible cue classes. kb_cue_match establishes that both classes are
 * present; kb_evidence_matches supplies their byte spans so C only checks order
 * and extracts slots. Adding/retracting either intent_cue changes this parser in
 * the running process without recompilation. */
static int arith_range_bounds(Brain *b, const char *text,
                              double *left, double *right) {
    if (!b || !b->kb || !text || !left || !right) return 0;
    KbEvidenceMatch asks[16];
    size_t na = kb_evidence_matches(b->kb, "intent_cue", "arith_count_request",
                                    text, asks, 16);
    if (na == 0) return 0;
    size_t ask_start = asks[0].start, ask_end = asks[0].start + asks[0].len;
    int found = 0, best_phase = 3;
    size_t best_distance = (size_t)-1, best_start = (size_t)-1;
    double best_left = 0, best_right = 0;
    char open_keys[16][KB_TERM_LEN];
    const char *fq[2] = { NULL, NULL };
    size_t nf = kb_match(b->kb, "range_frame", fq, 2, open_keys, 16);
    for (size_t f = 0; f < nf; f++) {
        char close_keys[8][KB_TERM_LEN];
        const char *cq[2] = { open_keys[f], NULL };
        size_t nc = kb_match(b->kb, "range_frame", cq, 2, close_keys, 8);
        for (size_t c = 0; c < nc; c++) {
            if (!kb_cue_match(b, open_keys[f], text) ||
                !kb_cue_match(b, close_keys[c], text))
                continue;
            KbEvidenceMatch opens[16], closes[16];
            size_t no = kb_evidence_matches(b->kb, "intent_cue", open_keys[f],
                                            text, opens, 16);
            size_t nx = kb_evidence_matches(b->kb, "intent_cue", close_keys[c],
                                            text, closes, 16);
            for (size_t oi = 0; oi < no; oi++) {
                size_t after_open = opens[oi].start + opens[oi].len;
                for (size_t ci = 0; ci < nx; ci++) {
                    if (closes[ci].start < after_open) continue;
                    size_t after_close = closes[ci].start + closes[ci].len;
                    double a, z;
                    if (!arith_first_value_in_span(text, after_open,
                                                   closes[ci].start, &a))
                        continue;
                    if (!arith_first_value_in_span(text, after_close,
                                                   strlen(text), &z))
                        continue;
                    /* Prefer a range that follows the count request; if none
                     * exists, use the nearest preceding range ("Between 1 and
                     * 10, how many ...?"). This rejects quoted/example ranges
                     * earlier in the turn without naming any surface word. */
                    int phase;
                    size_t distance;
                    if (opens[oi].start >= ask_end) {
                        phase = 0; distance = opens[oi].start - ask_end;
                    } else if (after_close <= ask_start) {
                        phase = 1; distance = ask_start - after_close;
                    } else {
                        phase = 0; distance = 0;
                    }
                    if (!found || phase < best_phase ||
                        (phase == best_phase && distance < best_distance) ||
                        (phase == best_phase && distance == best_distance &&
                         opens[oi].start < best_start)) {
                        found = 1; best_phase = phase;
                        best_distance = distance; best_start = opens[oi].start;
                        best_left = a; best_right = z;
                    }
                }
            }
        }
    }
    if (!found) return 0;
    *left = best_left; *right = best_right;
    return 1;
}

/* Fold ew[..] left-to-right as an arithmetic expression in any surface form:
 * skip leading non-numeric filler ("what is", "how much is"), then read
 * NUM (OP NUM)+ where OP is a word/symbol, consuming a "by" after
 * "divided"/"multiplied" and reading a bare "by" between numbers as times
 * ("six by seven"). Returns 1 with *res iff >=1 op applied and the whole tail
 * parsed cleanly (so prose like "5 apples ..." never matches). */
static int arith_eval_infix(Brain *b, char **ew, size_t enw, double *res) {
    size_t i = 0;
    double cur;
    while (i < enw && !parse_value(ew[i], &cur)) i++;
    if (i >= enw) return 0;
    i++;
    int ops = 0;
    while (i < enw) {
        char op = arith_op_char(b, ew[i]);
        if (!op) {
            return 0;
        }
        if ((op == '/' || op == '*') && i + 1 < enw &&
            arith_op_char(b, ew[i + 1]) == '*')
            i++;                                      /* consume the "by" */
        i++;
        if (i >= enw) return 0;
        double nx;
        if (!parse_value(ew[i], &nx)) return 0;
        int ok;
        cur = apply_op_char(b, op, cur, nx, &ok);
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
static int eval_operand(Brain *b, char **w, size_t s, size_t e, double *val) {
    if (s >= e) return 0;
    size_t ofp = e;
    for (size_t i = s; i < e; i++) {
        const char *q[] = { w[i] };
        if (b && b->kb && kb_query(b->kb, "fraction_connector", q, 1)) { ofp = i; break; }
    }

    /* P percent of N */
    long pctpos = -1;
    for (size_t i = s; i < e; i++) {
        const char *q[] = { w[i] };
        if (b && b->kb && kb_query(b->kb, "percentage_marker", q, 1)) { pctpos = (long)i; break; }
    }
    if (pctpos >= 0 && ofp < e) {
        double P = 0, N = 0; int haveP = 0, haveN = 0;
        for (size_t i = s; i < (size_t)pctpos; i++) if (parse_value(w[i], &P)) haveP = 1;
        for (size_t i = ofp + 1; i < e; i++) if (parse_value(w[i], &N)) { haveN = 1; break; }
        if (haveP && haveN) { *val = P / 100.0 * N; return 1; }
    }

    /* square root of N */
    for (size_t i = s; i < e; i++) {
        const char *q[] = { w[i] };
        if (b && b->kb && kb_query(b->kb, "root_word", q, 1)) {
            double N = 0; int haveN = 0;
            for (size_t j = s; j < e; j++) if (parse_value(w[j], &N)) { haveN = 1; break; }
            if (haveN && N >= 0) { *val = arith_sqrt(N); return 1; }
            return 0;
        }
    }

    /* fraction of N: half/third/quarter */
    if (ofp < e) {
        double denom = 0;
        for (size_t i = s; i < ofp; i++) {
            const char *q[] = { w[i], NULL };
            char hit[1][KB_TERM_LEN];
            if (b && b->kb && kb_match(b->kb, "fraction_word", q, 2, hit, 1) == 1)
                parse_value(kb_dequote(hit[0]), &denom);
        }
        if (denom > 0) {
            double N = 0; int haveN = 0;
            for (size_t i = ofp + 1; i < e; i++) if (parse_value(w[i], &N)) { haveN = 1; break; }
            if (haveN) { *val = N / denom; return 1; }
        }
    }

    /* N squared / N cubed */
    for (size_t i = s; i < e; i++) {
        const char *q[] = { w[i], NULL };
        char hit[1][KB_TERM_LEN];
        if (b && b->kb && kb_match(b->kb, "power_word", q, 2, hit, 1) == 1) {
            double exponent = 0;
            parse_value(kb_dequote(hit[0]), &exponent);
            double N = 0; int haveN = 0;
            for (size_t j = s; j < e; j++) if (parse_value(w[j], &N)) { haveN = 1; break; }
            if (haveN && exponent == 2) { *val = N * N; return 1; }
            if (haveN && exponent == 3) { *val = N * N * N; return 1; }
        }
    }

    /* plain: exactly one numeral in the span (rejects prose spans) */
    double only = 0; int cnt = 0;
    for (size_t i = s; i < e; i++) { double v; if (parse_value(w[i], &v)) { only = v; cnt++; } }
    if (cnt == 1) { *val = only; return 1; }
    return 0;
}

/* gen313: a spoken arithmetic EXPRESSION contains only expression vocabulary —
 * numerals, operators, computation forms and question lead-ins. Any other
 * content word ("father", "old", "years", "apples") means the sentence is
 * PROSE that merely mentions an operation; those turns belong to the
 * word-problem/algebra modules downstream, so the compound evaluator must
 * decline instead of stealing them ("four times as old as his son" is not
 * 4 * anything). The "exactly one numeral per span" check alone could not
 * see the difference. */
static int expr_vocab_ok(Brain *b, char **w, size_t nw) {
    for (size_t i = 0; i < nw; i++) {
        double v;
        if (!w[i][0] || parse_value(w[i], &v)) continue;
        if (!b || !b->kb) return 0;
        const char *q[] = { w[i] };
        if (kb_query(b->kb, "arithmetic_word", q, 1)) continue;
        /* Un membro puo' stare in KB CITATO — «%» lo e' da sempre, e da quando
         * la classe si deriva da `infix_operator/2` lo sono anche le parole
         * degli operatori. Chi chiede con il token nudo deve provare entrambe
         * le forme, altrimenti la conoscenza c'e' e non si vede. */
        char quoted[KB_TERM_LEN];
        snprintf(quoted, sizeof quoted, "\"%.*s\"",
                 (int)(sizeof(quoted) - 3), w[i]);
        const char *qq[] = { quoted };
        if (!kb_query(b->kb, "arithmetic_word", qq, 1)) return 0;
    }
    return 1;
}

/* Split norm into operand spans at top-level +,-,*,/ operators, evaluate each
 * with eval_operand, then fold (* and / bind before + and -). Returns 0 (fall
 * through to the single-op handlers) when there is no operator or any span is
 * not a clean operand. Uses its own wide tokenization because mod_arith's w[8]
 * would truncate a long expression before the second operand. */
static int arith_compound(Brain *b, const char *norm, char *out, size_t out_size) {
    char buf[256];
    size_t len = strlen(norm);
    if (len == 0 || len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (buf[len - 1] == '?' || buf[len - 1] == '.') buf[len - 1] = '\0';

    char *w[40];
    size_t nw = split_words(buf, w, 40);
    for (size_t i = 0; i < nw; i++) w[i] = strip_edge_punct(w[i]);
    if (!expr_vocab_ok(b, w, nw)) return 0;

    size_t opos[16]; char ochar[16]; size_t nop = 0;
    for (size_t i = 0; i < nw && nop < 16; i++) {
        char o = 0;
        o = arith_op_char(b, w[i]);
        if (o) { opos[nop] = i; ochar[nop] = o; nop++; }
    }
    if (nop == 0 || nop > 15) return 0;

    double vals[17]; size_t nv = 0, start = 0;
    for (size_t k = 0; k <= nop; k++) {
        size_t end = (k < nop) ? opos[k] : nw;
        double v;
        if (!eval_operand(b, w, start, end, &v)) return 0;
        vals[nv++] = v;
        if (k < nop) {
            start = opos[k] + 1;
            if ((ochar[k] == '*' || ochar[k] == '/') && start < nw &&
                arith_op_char(b, w[start]) == '*')
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

static unsigned long long p0_gcd_ull(unsigned long long a, unsigned long long b) {
    while (b) {
        unsigned long long r = a % b;
        a = b;
        b = r;
    }
    return a ? a : 1;
}

/* Invert the probability of two same-category draws without replacement.
 * Vocabulary and wording are KB-owned; C only binds the observed scalar to the
 * integer constraint R(R-1) / N(N-1) and finds a witness. */
static int p0_probability_inverse_draw(Brain *b, const char *norm,
                                       char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    if (!kb_cue_match(b, "probability_draw", norm) ||
        !kb_cue_match(b, "probability_inverse_draw", norm))
        return 0;
    const char *pq[] = { "inverse_pair_probability", "solver", NULL };
    char solver[1][KB_TERM_LEN];
    if (kb_match(b->kb, "probability_procedure", pq, 3, solver, 1) != 1)
        return 0;

    char buf[512];
    if (strlen(norm) >= sizeof buf) return 0;
    snprintf(buf, sizeof buf, "%s", norm);
    char *w[96];
    size_t nw = split_words(buf, w, 96);
    double probability = 0;
    char probability_text[32] = "";
    for (size_t i = 0; i < nw; i++) {
        char *tok = strip_edge_punct(w[i]);
        double v = 0;
        if (parse_num(tok, &v) && v > 0 && v < 1) {
            probability = v;
            snprintf(probability_text, sizeof probability_text, "%s", tok);
            break;
        }
    }
    if (probability <= 0 || probability >= 1) return 0;

    unsigned long long numerator = 0, denominator = 0;
    for (unsigned long long d = 2; d <= 1000; d++) {
        double scaled = probability * (double)d;
        unsigned long long n = (unsigned long long)(scaled + 0.5);
        double error = scaled - (double)n;
        if (error < 0) error = -error;
        if (n > 0 && n < d && error < 1e-9) {
            unsigned long long g = p0_gcd_ull(n, d);
            numerator = n / g;
            denominator = d / g;
            break;
        }
    }
    if (!numerator || !denominator) return 0;

    unsigned long long witness_total = 0, witness_target = 0;
    for (unsigned long long total = 2;
         total <= 1000 && !witness_total; total++) {
        unsigned long long rhs =
            numerator * total * (total - 1);
        for (unsigned long long target = 2; target <= total; target++) {
            unsigned long long lhs =
                denominator * target * (target - 1);
            if (lhs == rhs) {
                witness_total = total;
                witness_target = target;
                break;
            }
            if (lhs > rhs) break;
        }
    }
    if (!witness_total) return 0;

    char target_text[32], total_text[32];
    snprintf(target_text, sizeof target_text, "%llu", witness_target);
    snprintf(total_text, sizeof total_text, "%llu", witness_total);
    const KbResponseSlot slots[] = {
        { "probability", probability_text },
        { "target", target_text },
        { "total", total_text }
    };
    if (!kb_response_slots(b, "probability_inverse_pair_answer",
                           slots, 3, out, out_size))
        return 0;
    store_proof(b, "Integer witness satisfies R(R-1)/N(N-1) for two draws without replacement.");
    return 1;
}

static int p0_probability_draw(Brain *b, const char *norm,
                               char *out, size_t out_size) {
    if (!b || !b->kb) return 0;
    if (!kb_cue_match(b, "probability_draw", norm)) return 0;
    if (!kb_cue_match(b, "probability_at_least", norm)) return 0;

    char buf[512];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[96];
    size_t nw = split_words(buf, w, 96);
    struct CountWord { long long n; char word[KB_TERM_LEN]; size_t idx; } cw[32];
    size_t ncw = 0;
    for (size_t i = 0; i + 1 < nw && ncw < 32; i++) {
        double v = 0;
        const char *tok = strip_edge_punct(w[i]);
        const char *next = strip_edge_punct(w[i + 1]);
        if (!parse_num(tok, &v)) continue;
        long long iv = (long long)v;
        if ((double)iv != v || iv < 0) continue;
        if (!next[0] || !isalpha((unsigned char)next[0])) continue;
        cw[ncw].n = iv;
        snprintf(cw[ncw].word, sizeof cw[ncw].word, "%s", next);
        cw[ncw].idx = i;
        ncw++;
    }
    if (ncw < 3) return 0;

    size_t threshold_i = ncw - 1;
    size_t draw_i = threshold_i;
    while (draw_i > 0) {
        draw_i--;
        if (cw[draw_i].n > 0) break;
    }
    if (draw_i >= threshold_i) return 0;

    long long draw = cw[draw_i].n;
    long long need = cw[threshold_i].n;
    if (draw <= 0 || need <= 0) return 0;

    long long total_items = 0;
    long long target_items = 0;
    for (size_t i = 0; i < draw_i; i++) {
        total_items += cw[i].n;
        if (!strcmp(cw[i].word, cw[threshold_i].word))
            target_items += cw[i].n;
    }
    if (total_items <= 0 || target_items <= 0 || draw > total_items) return 0;

    long long other_items = total_items - target_items;
    char fav_pred[1][KB_TERM_LEN], den_pred[1][KB_TERM_LEN];
    const char *fpq[] = { "probability_at_least_count", "favorable", NULL };
    const char *dpq[] = { "probability_at_least_count", "denominator", NULL };
    if (kb_match(b->kb, "probability_procedure", fpq, 3, fav_pred, 1) != 1)
        return 0;
    if (kb_match(b->kb, "probability_procedure", dpq, 3, den_pred, 1) != 1)
        return 0;

    char total_items_s[32], target_s[32], other_s[32], draw_s[32], need_s[32];
    snprintf(total_items_s, sizeof total_items_s, "%lld", total_items);
    snprintf(target_s, sizeof target_s, "%lld", target_items);
    snprintf(other_s, sizeof other_s, "%lld", other_items);
    snprintf(draw_s, sizeof draw_s, "%lld", draw);
    snprintf(need_s, sizeof need_s, "%lld", need);

    char favhit[1][KB_TERM_LEN], denhit[1][KB_TERM_LEN], hg[128];
    snprintf(hg, sizeof hg, "hg(%s, %s, %s, %s)", target_s, other_s, draw_s, need_s);
    const char *fq[] = { hg, NULL };
    const char *dq[] = { total_items_s, draw_s, NULL };
    if (kb_match(b->kb, kb_dequote(fav_pred[0]), fq, 2, favhit, 1) != 1) return 0;
    if (kb_match(b->kb, kb_dequote(den_pred[0]), dq, 3, denhit, 1) != 1) return 0;

    char *end = NULL;
    unsigned long long fav = strtoull(kb_dequote(favhit[0]), &end, 10);
    if (!end || *end) return 0;
    end = NULL;
    unsigned long long total = strtoull(kb_dequote(denhit[0]), &end, 10);
    if (!end || *end || !total || !fav) return 0;

    unsigned long long g = p0_gcd_ull(fav, total);
    char fraction[64], favbuf[64], totalbuf[64], pctbuf[64];
    snprintf(fraction, sizeof fraction, "%llu/%llu", fav / g, total / g);
    snprintf(favbuf, sizeof favbuf, "%llu", fav);
    snprintf(totalbuf, sizeof totalbuf, "%llu", total);
    snprintf(pctbuf, sizeof pctbuf, "%.1f", ((double)fav * 100.0) / (double)total);

    const KbResponseSlot slots[] = {
        { "fraction", fraction },
        { "favorable", favbuf },
        { "total", totalbuf },
        { "percent", pctbuf }
    };
    if (kb_response_slots(b, "probability_at_least_count", slots, 4,
                          out, out_size)) {
        store_proof(b, "Hypergeometric draw solved by KB procedures choose/3 and fav_at_least/2.");
        return 1;
    }
    char msg[160];
    snprintf(msg, sizeof msg, "%s, about %s%%.", fraction, pctbuf);
    put(msg, out, out_size);
    return 1;
}

static int arith_digit_count_between(Brain *b, const char *norm,
                                     char *out, size_t out_size) {
    if (!b || !b->kb || !kb_cue_match(b, "digit_count_request", norm)) return 0;
    char nb[256];
    if (strlen(norm) >= sizeof nb) return 0;
    snprintf(nb, sizeof nb, "%s", norm);
    char *w[64];
    size_t nw = split_words(nb, w, 64);
    double nums[8];
    size_t nn = collect_numbers(w, nw, nums, 8);
    if (nn < 3) return 0;
    double low = 0, high = 0;
    int have_range = arith_range_bounds(b, norm, &low, &high);
    double digit = nums[0];
    if (!have_range) {
        low = nums[1];
        high = nums[2];
    }
    if (digit < 0 || digit > 9 ||
        digit != (double)(long long)digit ||
        low != (double)(long long)low ||
        high != (double)(long long)high)
        return 0;

    char db[32], lb[32], hb[32];
    snprintf(db, sizeof db, "%lld", (long long)digit);
    snprintf(lb, sizeof lb, "%lld", (long long)low);
    snprintf(hb, sizeof hb, "%lld", (long long)high);
    const char *q[] = { db, lb, hb, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "digit_count_between", q, 4, hit, 1) == 0) return 0;
    KbResponseSlot slots[] = {
        { "digit", db },
        { "low", lb },
        { "high", hb },
        { "count", kb_dequote(hit[0]) }
    };
    if (kb_response_slots(b, "digit_count_between_answer", slots, 4,
                          out, out_size)) {
        store_proof(b, "digit_count_between/4 enumerates the inclusive range and counts the requested digit.");
        return 1;
    }
    return 0;
}

/* Count fixed-length digit strings whose values lie in an explicit inclusive
 * range and whose sum is fixed. Request vocabulary is three independent KB cue
 * classes; C only extracts integers and performs bounded dynamic programming. */
static int arith_digit_sum_combinations(Brain *b, const char *norm,
                                        char *out, size_t out_size) {
    if (!b || !b->kb ||
        !kb_cue_match(b, "digit_sequence_container", norm) ||
        !kb_cue_match(b, "digit_sum_constraint", norm) ||
        !kb_cue_match(b, "combination_count_request", norm))
        return 0;

    long long values[16];
    size_t nv = 0;
    for (const char *p = norm; *p && nv < 16; ) {
        if (!isdigit((unsigned char)*p)) {
            p++;
            continue;
        }
        char *end = NULL;
        values[nv++] = strtoll(p, &end, 10);
        if (!end || end <= p) return 0;
        p = end;
    }
    if (nv < 4) return 0;

    long long width = values[0];
    long long low = values[1];
    long long high = values[2];
    long long target = values[nv - 1];
    if (width <= 0 || width > 16 || low < 0 || high < low ||
        high > 36 || target < 0 || target > 1000)
        return 0;

    unsigned long long ways[17][1001] = {{0}};
    ways[0][0] = 1;
    for (long long pos = 0; pos < width; pos++) {
        for (long long sum = 0; sum <= target; sum++) {
            if (!ways[pos][sum]) continue;
            for (long long digit = low;
                 digit <= high && sum + digit <= target; digit++) {
                unsigned long long old = ways[pos + 1][sum + digit];
                unsigned long long add = ways[pos][sum];
                ways[pos + 1][sum + digit] =
                    ULLONG_MAX - old < add ? ULLONG_MAX : old + add;
            }
        }
    }

    char count[64], width_s[32], target_s[32];
    snprintf(count, sizeof count, "%llu", ways[width][target]);
    snprintf(width_s, sizeof width_s, "%lld", width);
    snprintf(target_s, sizeof target_s, "%lld", target);
    const KbResponseSlot slots[] = {
        { "count", count },
        { "width", width_s },
        { "target", target_s }
    };
    if (kb_response_slots(b, "digit_sum_combination_answer", slots, 3,
                          out, out_size)) {
        store_proof(b, "Counted bounded digit strings by dynamic programming over position and partial sum.");
        return 1;
    }
    put(count, out, out_size);
    return 1;
}

static int arith_token_matches_cueclass(Brain *b, const char *cueclass,
                                        const char *tok) {
    if (!b || !b->kb || !cueclass || !tok) return 0;
    const char *q[] = { cueclass, NULL };
    char cues[16][KB_TERM_LEN];
    size_t nc = kb_match(b->kb, "intent_cue", q, 2, cues, 16);
    for (size_t i = 0; i < nc; i++)
        if (!strcmp(tok, kb_dequote(cues[i]))) return 1;
    return 0;
}

static int arith_ratio_word(Brain *b, const char *tok, double *ratio) {
    if (!b || !b->kb || !tok || !ratio) return 0;
    const char *q[] = { tok, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "ratio_word", q, 2, hit, 1) == 0) return 0;
    return parse_value(hit[0], ratio);
}

static int arith_schema_role_class(Brain *b, const char *schema,
                                   const char *role, char *out, size_t out_size) {
    if (!b || !b->kb || !schema || !role || !out || out_size == 0) return 0;
    const char *q[] = { schema, role, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match(b->kb, "schema_role_class", q, 3, hit, 1) == 0) return 0;
    snprintf(out, out_size, "%s", kb_dequote(hit[0]));
    return 1;
}

static int arith_paired_dimensions_from_doubled_sum(Brain *b, const char *norm,
                                                    const char *schema_intent,
                                                    char *out, size_t out_size) {
    if (!schema_intent) return 0;
    if (!b || !b->kb || !kb_cue_match(b, schema_intent, norm)) return 0;
    char sum_role[KB_TERM_LEN], major_role[KB_TERM_LEN], minor_role[KB_TERM_LEN];
    if (!arith_schema_role_class(b, schema_intent, "doubled_sum",
                                 sum_role, sizeof sum_role) ||
        !arith_schema_role_class(b, schema_intent, "major_dimension",
                                 major_role, sizeof major_role) ||
        !arith_schema_role_class(b, schema_intent, "minor_dimension",
                                 minor_role, sizeof minor_role))
        return 0;
    char rb[256];
    if (strlen(norm) >= sizeof rb) return 0;
    snprintf(rb, sizeof rb, "%s", norm);
    char *w[64];
    size_t nw = split_words(rb, w, 64);
    double per = -1, ratio = -1;
    size_t lpos = nw, wpos = nw, rpos = nw;
    char unit[32] = "units";
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (arith_token_matches_cueclass(b, major_role, t) && lpos == nw)
            lpos = i;
        if (arith_token_matches_cueclass(b, minor_role, t) && wpos == nw)
            wpos = i;
        if (arith_token_matches_cueclass(b, sum_role, t) && per < 0) {
            for (size_t j = i + 1; j <= i + 4 && j < nw; j++) {
                double v;
                if (parse_value(strip_edge_punct(w[j]), &v)) {
                    per = v;
                    if (j + 1 < nw) snprintf(unit, sizeof unit, "%s", strip_edge_punct(w[j + 1]));
                    break;
                }
            }
        }
        if (ratio < 0 && arith_ratio_word(b, t, &ratio)) rpos = i;
    }
    if (per <= 0 || ratio <= 0) return 0;
    int length_is_multiple = !(wpos < rpos && lpos > rpos);
    char ps[32], ks[32];
    format_num(per, ps, sizeof ps);
    format_num(ratio, ks, sizeof ks);
    const char *wq[] = { ps, ks, NULL, NULL };
    char wh[1][KB_TERM_LEN];
    if (kb_match(b->kb, "paired_dimensions_from_doubled_sum_ratio", wq, 4, wh, 1) == 0)
        return 0;
    const char *lq[] = { ps, ks, wh[0], NULL };
    char lh[1][KB_TERM_LEN];
    if (kb_match(b->kb, "paired_dimensions_from_doubled_sum_ratio", lq, 4, lh, 1) == 0)
        return 0;
    char ws[32], ls[32];
    snprintf(ws, sizeof ws, "%s", kb_dequote(wh[0]));
    snprintf(ls, sizeof ls, "%s", kb_dequote(lh[0]));
    KbResponseSlot slots[] = {
        { "length", length_is_multiple ? ls : ws },
        { "width", length_is_multiple ? ws : ls },
        { "unit", unit }
    };
    if (kb_response_slots(b, "paired_dimensions_answer", slots, 3,
                          out, out_size)) {
        store_proof(b, "paired_dimensions_from_doubled_sum_ratio/4 binds the two dimensions before rendering.");
        return 1;
    }
    return 0;
}


/* gen427 — VERIFICARE NON E' CALCOLARE.
 *
 * «2+2=4» non chiede quanto fa: chiede se e' vero. Il muro cieco lo prendeva
 * insieme a «2+2=5» e a «2 > 1», tre turni diversi con la stessa risposta, e le
 * classi misurate a cinque byte lo mostravano in una riga.
 *
 * Il motore qui fa tre cose e nessuna di piu': spezza il turno su un simbolo di
 * relazione, VALUTA i due lati (un numero e' se stesso, «A op B» passa da
 * `apply_operator/4`), e chiede alla KB il verdetto e la frase. Quali relazioni
 * esistano, che cosa significhi che tengono e come si dicono sta tutto in
 * `kb/core/claims.p0` — una relazione nuova costa tre righe di conoscenza.
 *
 * Sta PRIMA di mod_operator perche' «2+2=4» contiene «+»: chi calcola e basta
 * risponderebbe «4» a chi ha gia' scritto 4, cioe' non risponderebbe alla
 * domanda posta. */
static int claim_side_value(Brain *b, const char *text, char *val, size_t vsz) {
    char work[128];
    snprintf(work, sizeof work, "%s", text);
    char *t = work;
    while (*t == ' ') t++;
    size_t tl = strlen(t);
    while (tl && t[tl-1] == ' ') t[--tl] = '\0';
    if (!*t) return 0;
    /* un numero e' se stesso */
    const char *d = t;
    if ((*d == '-' || *d == '+') && d[1]) d++;
    int numeric = *d != '\0';
    for (const char *p = d; *p; p++)
        if (!isdigit((unsigned char)*p)) { numeric = 0; break; }
    if (numeric) { snprintf(val, vsz, "%s", t); return 1; }
    /* altrimenti: «A op B», con l'operatore preso dalla KB */
    char syms[32][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t ns = kb_match(b->kb, "infix_operator", q, 2, syms, 32);
    for (size_t i = 0; i < ns; i++) {
        char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", syms[i]);
        const char *sym = kb_dequote(sb);
        if (!sym || !*sym) continue;
        const char *at = strstr(t, sym);
        if (!at || at == t) continue;
        char left[64], right[64];
        size_t ll = (size_t)(at - t);
        if (ll == 0 || ll >= sizeof left) continue;
        memcpy(left, t, ll); left[ll] = '\0';
        snprintf(right, sizeof right, "%s", at + strlen(sym));
        char lv[KB_TERM_LEN], rv[KB_TERM_LEN];
        if (!claim_side_value(b, left, lv, sizeof lv)) continue;
        if (!claim_side_value(b, right, rv, sizeof rv)) continue;
        const char *nq[2] = { syms[i], NULL };
        char opname[1][KB_TERM_LEN];
        if (kb_match(b->kb, "infix_operator", nq, 2, opname, 1) != 1) continue;
        char ob[KB_TERM_LEN]; snprintf(ob, sizeof ob, "%s", opname[0]);
        const char *aq[4] = { kb_dequote(ob), lv, rv, NULL };
        char res[1][KB_TERM_LEN];
        if (kb_match(b->kb, "apply_operator", aq, 4, res, 1) != 1) continue;
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", res[0]);
        snprintf(val, vsz, "%s", kb_dequote(rb));
        return 1;
    }
    return 0;
}

static int mod_claim(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb || !norm || !*norm) return 0;
    char syms[16][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t ns = kb_match(b->kb, "infix_relation", q, 2, syms, 16);
    if (!ns) return 0;
    /* il simbolo piu' lungo per primo: «>=» non e' un «>» con un carattere in
     * piu', e leggerlo cosi' cambierebbe la relazione affermata. */
    size_t order[16];
    for (size_t i = 0; i < ns; i++) order[i] = i;
    for (size_t i = 0; i < ns; i++)
        for (size_t j = i + 1; j < ns; j++)
            if (strlen(syms[order[j]]) > strlen(syms[order[i]])) {
                size_t t = order[i]; order[i] = order[j]; order[j] = t;
            }
    for (size_t k = 0; k < ns; k++) {
        char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", syms[order[k]]);
        const char *sym = kb_dequote(sb);
        if (!sym || !*sym) continue;
        const char *at = strstr(norm, sym);
        if (!at || at == norm) continue;
        char left[128], right[128];
        size_t ll = (size_t)(at - norm);
        if (ll == 0 || ll >= sizeof left) continue;
        memcpy(left, norm, ll); left[ll] = '\0';
        snprintf(right, sizeof right, "%s", at + strlen(sym));
        char lv[KB_TERM_LEN], rv[KB_TERM_LEN];
        if (!claim_side_value(b, left, lv, sizeof lv)) {
            /* gen427 — «x = 1» NON E' UNA VERIFICA: e' un'ASSEGNAZIONE.
             *
             * A sinistra non c'e' un valore da confrontare, c'e' un NOME a cui
             * darne uno. La differenza fra «2+2=4» e «x = 1» e' tutta qui, e
             * riconoscerla costa una domanda sola: il lato sinistro si valuta?
             *
             * Si registra davvero, come fatto di sessione: rispondere «ho
             * capito» senza tenere il valore sarebbe fingere di aver capito, e
             * infatti subito dopo «what is x» risponde 1. QUALE relazione e'
             * un'assegnazione e come si dice sta in KB. */
            char nm[64];
            snprintf(nm, sizeof nm, "%s", left);
            char *np = nm; while (*np == ' ') np++;
            size_t nl = strlen(np); while (nl && np[nl-1] == ' ') np[--nl] = '\0';
            int ident = *np != '\0';
            for (const char *c = np; *c && ident; c++)
                if (!isalnum((unsigned char)*c) && *c != '_') ident = 0;
            if (!ident) continue;
            if (!claim_side_value(b, right, rv, sizeof rv)) continue;
            const char *aq[2] = { syms[order[k]], NULL };
            char arel[1][KB_TERM_LEN];
            if (kb_match(b->kb, "infix_relation", aq, 2, arel, 1) != 1) continue;
            char ab[KB_TERM_LEN]; snprintf(ab, sizeof ab, "%s", arel[0]);
            const char *aname = kb_dequote(ab);
            const char *bq[2] = { aname, NULL };
            char bind[1][KB_TERM_LEN];
            if (kb_match(b->kb, "relation_binds", bq, 2, bind, 1) != 1) continue;
            char bb[KB_TERM_LEN]; snprintf(bb, sizeof bb, "%s", bind[0]);
            const char *pred = kb_dequote(bb);
            const char *fa[2] = { np, rv };
            int prev = kb_origin(b->kb);
            kb_set_origin(b->kb, KB_SESSION);
            kb_assert(b->kb, pred, fa, 2);
            kb_set_origin(b->kb, prev);
            const KbResponseSlot asl[] = { { "name", np }, { "value", rv } };
            char amsg[400];
            if (kb_response_slots(b, "assignment_noted", asl, 2, amsg, sizeof amsg)) {
                put(amsg, out, out_size);
                return 1;
            }
            continue;
        }
        if (!claim_side_value(b, right, rv, sizeof rv)) continue;
        const char *rq[2] = { syms[order[k]], NULL };
        char relname[1][KB_TERM_LEN];
        if (kb_match(b->kb, "infix_relation", rq, 2, relname, 1) != 1) continue;
        char nb[KB_TERM_LEN]; snprintf(nb, sizeof nb, "%s", relname[0]);
        const char *rel = kb_dequote(nb);
        const char *cq[4] = { rel, lv, rv, NULL };
        char verdict[1][KB_TERM_LEN];
        if (kb_match(b->kb, "relation_check", cq, 4, verdict, 1) != 1) continue;
        const char *sq[3] = { rel, verdict[0], NULL };
        char tmpl[1][KB_TERM_LEN];
        if (kb_match(b->kb, "relation_say", sq, 3, tmpl, 1) != 1) continue;
        char tb[KB_TERM_LEN]; snprintf(tb, sizeof tb, "%s", tmpl[0]);
        const char *wq[2] = { rel, NULL };
        char rname[1][KB_TERM_LEN]; char nameb[KB_TERM_LEN] = "";
        if (kb_match(b->kb, "relation_name", wq, 2, rname, 1) == 1)
            snprintf(nameb, sizeof nameb, "%s", kb_dequote(rname[0]));
        /* i due lati si ridicono COME SONO STATI SCRITTI: chi ha scritto «2+2»
         * deve rileggere «2+2», non «4», altrimenti la correzione non si capisce. */
        char lt[128], rt[128];
        snprintf(lt, sizeof lt, "%s", left);
        snprintf(rt, sizeof rt, "%s", right);
        char *lp = lt; while (*lp == ' ') lp++;
        size_t x = strlen(lp); while (x && lp[x-1] == ' ') lp[--x] = '\0';
        char *rp = rt; while (*rp == ' ') rp++;
        x = strlen(rp); while (x && rp[x-1] == ' ') rp[--x] = '\0';
        const KbResponseSlot sl[] = {
            { "left", lp }, { "right", rp }, { "value", lv }, { "relation", nameb }
        };
        char msg[400];
        if (kb_response_slots(b, kb_dequote(tb), sl, 4, msg, sizeof msg)) {
            put(msg, out, out_size);
            return 1;
        }
    }
    return 0;
}

/* gen423 — L'OPERATORE E' UN FATTO, non un `case` (F.).
 *
 * Trovato dalla firma del ragionamento: «9-4» toccava gli stessi predicati di
 * «why», cioe' NESSUNO che c'entrasse col calcolo — perche' il calcolo stava
 * qui sotto come `case '+': return a + c;`. Un turno che risolve un'addizione
 * senza interrogare un solo predicato e' la definizione operativa di «non e'
 * KB-first», e ora si legge da un numero.
 *
 * F. l'ha detto nella forma che conta: «non si puo' insegnare alla KB che esiste
 * un operatore nuovo, e non mi piace». Ora si puo', e costa due righe di KB: il
 * massimo comun divisore e la media sono definiti li' e in nessun altro posto —
 * il `gcd` per giunta ricorsivo, che nessuna formula cablata potrebbe fingere.
 *
 * Il modulo e' STRETTO apposta: fira solo sulla forma «A op B» con due numeri e
 * nient'altro attorno. Tutto il resto — espressioni composte, parole-numero,
 * equazioni inverse — resta a mod_arith, che non e' stato toccato. E' la regola
 * delle strutture secondarie: si affianca, non si sostituisce.
 */
static int mod_operator(Brain *b, const char *norm, const char *raw,
                        char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb || !norm || !*norm) return 0;
    char syms[32][KB_TERM_LEN];
    const char *q[2] = { NULL, NULL };
    size_t ns = kb_match(b->kb, "infix_operator", q, 2, syms, 32);
    for (size_t i = 0; i < ns; i++) {
        char sb[KB_TERM_LEN]; snprintf(sb, sizeof sb, "%s", syms[i]);
        const char *sym = kb_dequote(sb);
        if (!sym || !*sym) continue;
        const char *at = strstr(norm, sym);
        if (!at || at == norm) continue;
        /* i due lati devono essere numeri interi, e basta: se avanza altro, il
         * turno non e' di questa forma e passa a chi lo sa leggere. */
        char left[64], right[64];
        size_t ll = (size_t)(at - norm);
        if (ll == 0 || ll >= sizeof left) continue;
        memcpy(left, norm, ll); left[ll] = '\0';
        snprintf(right, sizeof right, "%s", at + strlen(sym));
        /* NON si ripulisce la punteggiatura: si tolgono solo gli spazi. Ripulirla
         * faceva diventare «10 / x = 0» un «10 x 0», cioe' inventava una forma
         * che il turno non aveva. Se ai lati c'e' qualcosa che non e' una cifra,
         * questo modulo non e' quello giusto e deve lasciar passare. */
        char *l = left, *r = right;
        while (*l == ' ') l++;
        while (*r == ' ') r++;
        size_t rl = strlen(r);
        while (rl && r[rl-1] == ' ') r[--rl] = '\0';
        rl = strlen(l);
        while (rl && l[rl-1] == ' ') l[--rl] = '\0';
        if (!*l || !*r) continue;
        int ok = 1;
        for (const char *p = l; *p && ok; p++) if (!isdigit((unsigned char)*p)) ok = 0;
        for (const char *p = r; *p && ok; p++) if (!isdigit((unsigned char)*p)) ok = 0;
        if (!ok) continue;
        const char *nq[2] = { syms[i], NULL };
        char opname[1][KB_TERM_LEN];
        if (kb_match(b->kb, "infix_operator", nq, 2, opname, 1) != 1) continue;
        char ob[KB_TERM_LEN]; snprintf(ob, sizeof ob, "%s", opname[0]);
        const char *op = kb_dequote(ob);
        const char *aq[4] = { op, l, r, NULL };
        char res[1][KB_TERM_LEN];
        if (kb_match(b->kb, "apply_operator", aq, 4, res, 1) != 1) continue;
        char rb[KB_TERM_LEN]; snprintf(rb, sizeof rb, "%s", res[0]);
        char msg[128];
        snprintf(msg, sizeof msg, "%s.", kb_dequote(rb));
        put(msg, out, out_size);
        return 1;
    }
    return 0;
}

static int mod_arith(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw;

    if (p0_probability_inverse_draw(b, norm, out, out_size)) return 1;

    /* gen357: a scalar fold must not preempt a registered multi-step schema.
     * This is plan-first dispatch: the lexicon and ownership table live in KB. */
    if (kb_module_guarded(b, "arith", norm)) return 0;

    /* gen252: classic letter riddles contain numbers but are not arithmetic.
     * Catch this before expression folding turns "twice ... thousand" into math. */
    if (b && b->kb && kb_cue_match(b, "20_math_cue1085", norm) &&
        kb_cue_match(b, "20_math_cue1086", norm) && kb_cue_match(b, "20_math_cue1086_2", norm)) {
        if (kb_response(b, "riddle_letter_m", NULL, out, out_size)) return 1;
    }

    if (arith_digit_sum_combinations(b, norm, out, out_size)) return 1;
    if (arith_digit_count_between(b, norm, out, out_size)) return 1;
    if (arith_paired_dimensions_from_doubled_sum(b, norm,
                                                "rectangle_dimension_schema",
                                                out, out_size)) return 1;
    if (p0_faculty_yields(b, "arith", "late", norm, NULL)) return 0;

    /* gen312: compound arithmetic expression (self-guards: fires only when a
     * top-level operator joins clean operand phrases, else falls through). */
    if (arith_compound(b, norm, out, out_size)) return 1;

    if (p0_probability_draw(b, norm, out, out_size)) return 1;

    char buf[256];
    size_t len = strlen(norm);
    if (len >= sizeof buf) return 0;
    memcpy(buf, norm, len + 1);
    if (len > 0 && buf[len - 1] == '?') buf[len - 1] = '\0';

    char *w[8];
    size_t nw = split_words(buf, w, 8);

    if (b && b->kb && kb_cue_match(b, "handshake_problem", norm)) {
        double n = 0;
        int found = 0;
        char hb[256]; snprintf(hb, sizeof hb, "%s", norm);
        char *hw[64]; size_t hn = split_words(hb, hw, 64);
        for (size_t i = 0; i < hn; i++) {
            if (parse_num(strip_edge_punct(hw[i]), &n) && n >= 0) {
                found = 1;
                break;
            }
        }
        long long people = (long long)n;
        if (found && (double)people == n && people >= 2 && people <= 1000000) {
            long long total = people * (people - 1) / 2;
            char cbuf[64], pbuf[64];
            snprintf(cbuf, sizeof cbuf, "%lld", total);
            snprintf(pbuf, sizeof pbuf, "%lld", people);
            const KbResponseSlot slots[] = {
                { "count", cbuf },
                { "people", pbuf }
            };
            if (kb_response_slots(b, "handshake_count", slots, 2, out, out_size)) {
                store_proof(b, "Pairwise count: n people each connect once, so n*(n-1)/2.");
                return 1;
            }
            char msg[96]; snprintf(msg, sizeof msg, "%lld.", total);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* Expand tokens containing embedded operators (e.g. "2+2" -> "2","+","2").
     * Pure signed numbers stay intact unless the KB licenses their contextual
     * reading as an operator joined to a right-hand operand: after a complete
     * operand, "+3" can become "+", "3".  This is deliberately not a typo
     * dictionary: the original surface and the causal normalization are both
     * retained, and ablating token_variation/2 disables the family.
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
            char candidate[2] = { s[j], '\0' };
            /* A leading operator belongs to the KB-licensed joined-RHS family
             * below.  Operators inside a compact token (2+3) remain the older
             * mechanical compact-expression path.  Both inventories are read
             * through infix_operator/2, never through a symbol list in C. */
            if (j > 0 && is_arith_op(b, candidate)) has_embedded_op = 1;
        }
        double v;
        int is_num = parse_num(s, &v);
        char variation[KB_TERM_LEN];
        int split_prefix = 0;
        if (sl > 1 && enw > 0 &&
            token_variation_class(b, "split_operator_prefix",
                                  variation, sizeof variation)) {
            char prefix[2] = { s[0], '\0' };
            double left = 0, right = 0;
            if (is_arith_op(b, prefix) && parse_value(ew[enw - 1], &left) &&
                parse_value(s + 1, &right) &&
                exoff + 2 + strlen(s + 1) + 1 <= sizeof exbuf && enw + 2 <= 24) {
                exbuf[exoff] = prefix[0];
                exbuf[exoff + 1] = '\0';
                ew[enw++] = exbuf + exoff;
                exoff += 2;
                size_t rl = strlen(s + 1);
                memcpy(exbuf + exoff, s + 1, rl + 1);
                ew[enw++] = exbuf + exoff;
                exoff += rl + 1;
                char repaired[KB_TERM_LEN];
                snprintf(repaired, sizeof repaired, "%s %s", prefix, s + 1);
                note_turn_surface_repair(b, variation,
                                         "split_operator_prefix", s, repaired);
                split_prefix = 1;
            }
        }
        if (split_prefix) {
            continue;
        } else if (!is_num && has_embedded_op) {
            size_t start = 0;
            for (size_t j = 0; j <= sl && enw < 24; j++) {
                char candidate[2] = { j < sl ? s[j] : '\0', '\0' };
                int boundary = (j == sl ||
                    (j > 0 && is_arith_op(b, candidate)));
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

    /* A compact symbolic expression may carry its request words after the
     * expression ("8/0 quanto fa").  The ordinary infix fold deliberately
     * rejects non-expression tails, so bind the one binary expression only
     * when every remaining token belongs to the KB-owned arithmetic-request
     * class (or to the general stopword class).  The symbols and slot ordering
     * are mechanics; the natural-language vocabulary remains teachable. */
    if (kb_cue_match(b, "arith_request", norm)) {
        size_t pair = enw, pairs = 0;
        for (size_t i = 0; i + 2 < enw; i++) {
            double a, c;
            if (parse_value(ew[i], &a) && is_arith_op(b, ew[i + 1]) &&
                parse_value(ew[i + 2], &c)) {
                pair = i;
                pairs++;
            }
        }
        if (pairs == 1) {
            int clean = 1;
            for (size_t i = 0; i < enw; i++) {
                if (i >= pair && i <= pair + 2) continue;
                if (!is_stopword(b, ew[i]) &&
                    !kb_cue_match(b, "arith_request", ew[i])) {
                    clean = 0;
                    break;
                }
            }
            if (clean) {
                double a, c;
                parse_value(ew[pair], &a);
                parse_value(ew[pair + 2], &c);
                if (!strcmp(ew[pair + 1], "/") && c == 0)
                    return kb_term_say(b, "arith_division_zero", NULL, 0, out, out_size);
                int ok = 0;
                double r = apply_arith_op(b, ew[pair + 1], a, c, &ok);
                if (ok) {
                    arith_answer(r, out, out_size);
                    return 1;
                }
            }
        }
    }

    /* Exact-shape arith: "what is <a> OP <b>?" with expanded tokens. */
    if (enw == 5 && lex_class_member(b, "20_math_lex1235", ew[0]) && lex_class_member(b, "20_math_lex1235_2", ew[1]) &&
        is_arith_op(b, ew[3])) {
        double a, c;
        if (parse_value(ew[2], &a) && parse_value(ew[4], &c)) {
            int ok;
            double r = apply_arith_op(b, ew[3], a, c, &ok);
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
                    if (!is_arith_op(b, ew[i + 1])) continue;
                    double a, c;
                    if (parse_value(ew[i], &a) && parse_value(ew[i + 2], &c)) {
                        int ok;
                        double r = apply_arith_op(b, ew[i + 1], a, c, &ok);
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
    if (kb_cue_match(b, "20_math_chain1282", buf)) {
        for (size_t i = 0; i + 2 < enw; i++) {
            if (!is_arith_op(b, ew[i + 1])) continue;
            double a, c;
            if (!parse_num(ew[i], &a) || !parse_num(ew[i + 2], &c)) continue;
            int ok;
                double r = apply_arith_op(b, ew[i + 1], a, c, &ok);
            if (!ok) continue;
            const char *op = ew[i + 1];
            const char *verb = "combining", *noun = "result";
            char quoted_op[KB_TERM_LEN], verb_buf[KB_TERM_LEN], noun_buf[KB_TERM_LEN];
            snprintf(quoted_op, sizeof quoted_op, "\"%.*s\"",
                     (int)(sizeof quoted_op - 3), op);
            const char *vq[] = { quoted_op, NULL, NULL };
            char verb_row[1][KB_TERM_LEN];
            if (kb_match(b->kb, "operator_explanation", vq, 3, verb_row, 1) == 1) {
                snprintf(verb_buf, sizeof verb_buf, "%s", kb_dequote(verb_row[0]));
                const char *nq[] = { quoted_op, verb_row[0], NULL };
                char noun_row[1][KB_TERM_LEN];
                if (kb_match(b->kb, "operator_explanation", nq, 3, noun_row, 1) == 1) {
                    snprintf(noun_buf, sizeof noun_buf, "%s", kb_dequote(noun_row[0]));
                    verb = verb_buf; noun = noun_buf;
                }
            }
            char na[64], nb[64], nr[64], msg[320];
            format_num(a, na, sizeof na);
            format_num(c, nb, sizeof nb);
            format_num(r, nr, sizeof nr);
            { const KbResponseSlot _rs[] = { { "verb", verb }, { "na", na }, { "nb", nb }, { "nr", nr }, { "noun", noun } };
      kb_term_say(b, "because_x_x_and_x_gives_x_that_is_their_x", _rs, 5, msg, sizeof msg);
              put(msg, out, out_size); }
            return 1;
        }
    }

    /* "is <a> divisible by <b>?" -> yes/no via integer remainder */
    if (enw == 5 && lex_class_member(b, "20_math_lex1308", ew[0]) && lex_class_member(b, "20_math_lex1308_2", ew[2]) &&
        lex_class_member(b, "20_math_lex1309", ew[3])) {
        double a, c;
        if (!parse_num(ew[1], &a) || !parse_num(ew[4], &c)) return 0;
        if (c == 0)
            return kb_term_say(b, "arith_division_zero", NULL, 0, out, out_size);
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
            if (lex_class_member(b, "20_math_lex1360", cw[i]) || !strcmp(cw[i], "%") ||
                lex_class_member(b, "20_math_lex1361", cw[i])) { mark = i; break; }
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
        if (ri != cnw && !kb_cue_match(b, "20_math_cue1389", norm) && !kb_cue_match(b, "20_math_cue1389_2", norm)) {
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
                if (lex_class_member(b, "20_math_lex1401", cw[i]) || lex_class_member(b, "20_math_lex1401_2", cw[i])) sq = 1;
                else if (lex_class_member(b, "20_math_lex1402", cw[i]) || lex_class_member(b, "20_math_lex1402_2", cw[i])) cb = 1;
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
        if (gn == 1 && (kb_cue_match(b, "20_math_chain1417", norm))) {
            arith_answer(gnums[0] / 2.0, out, out_size); return 1;
        }
        /* double / twice / triple / thrice (optionally "of") N */
        if (gn == 1 && (kb_cue_match(b, "20_math_chain1421", norm))) {
            double mul = (kb_cue_match(b, "20_math_chain1423", norm)) ? 3 : 2;
            arith_answer(gnums[0] * mul, out, out_size); return 1;
        }
    }

    /* N-ary "sum of A and B and C..." and "average/mean of ...". */
    if (kb_cue_match(b, "20_math_chain1429", norm)) {
        double nums[16]; size_t n = collect_numbers(cw, cnw, nums, 16);
        if (n >= 1) {
            double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
            int avg = kb_cue_match(b, "20_math_cue1432", norm) || kb_cue_match(b, "20_math_cue1432_2", norm) || kb_cue_match(b, "20_math_cue1432_3", norm);
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
            if (lex_class_member(b, "20_math_lex1444", v0) || lex_class_member(b, "20_math_lex1444_2", v0) || lex_class_member(b, "20_math_lex1444_3", v0) ||
                lex_class_member(b, "20_math_lex1445", v0)) {
                double s = 0; for (size_t i = 0; i < n; i++) s += nums[i];
                arith_answer(s, out, out_size); return 1;
            }
            if (lex_class_member(b, "20_math_lex1449", v0) || lex_class_member(b, "20_math_lex1449_2", v0) || lex_class_member(b, "20_math_lex1449_3", v0)) {
                /* "subtract A from B" -> B - A */
                if (kb_cue_match(b, "20_math_chain1453", norm)) { arith_answer(nums[1] - nums[0], out, out_size); return 1; }
                arith_answer(nums[0] - nums[1], out, out_size); return 1;
            }
            if (lex_class_member(b, "20_math_lex1454", v0) || lex_class_member(b, "20_math_lex1454_2", v0)) {
                double p = 1; for (size_t i = 0; i < n; i++) p *= nums[i];
                arith_answer(p, out, out_size); return 1;
            }
            if (lex_class_member(b, "20_math_lex1458", v0) || lex_class_member(b, "20_math_lex1458_2", v0)) {
                if (nums[1] != 0) { arith_answer(nums[0] / nums[1], out, out_size); return 1; }
            }
        }
    }

    /* Number-property predicates: "is N prime / even / odd" (EN+IT property
     * words, any word order — "5 è un numero primo"). The property word is read
     * tokenwise (not as a substring) so "evening"/"Paris" never trigger it, and
     * it fires only when the turn also names an integer. */
    {
        int wants_prime = kb_cue_match(b, "arith_property_prime", norm);
        int wants_even = 0;
        int wants_odd = kb_cue_match(b, "arith_property_odd", norm);
        for (size_t i = 0; i < cnw; i++) {
            if (kb_cue_match(b, "arith_property_even", cw[i])) {
                /* gen254 (repair): concessive "even though/if/so" is not the
                 * number property; and a creative-continuation request must
                 * never be read as a parity question ("...began to chime, even
                 * though no one had touched it" answered "No, 1 is not even"). */
                if (i + 1 < cnw &&
                    kb_cue_match(b, "arith_even_concessive", cw[i + 1]))
                    continue;
                wants_even = 1;
            }
        }
        if (kb_cue_match(b, "arith_property_exclusion", norm))
            wants_prime = wants_even = wants_odd = 0;

        /* LLMSCORE gen333: quantify a COMPUTABLE number property over a bounded
         * inclusive interval.  `arith_is_prime` already judged one number; the
         * missing capability was the generic fold, not the memorized fact 25.
         * The same loop serves prime/even/odd and normalizes reversed bounds. */
        int wants_count = kb_cue_match(b, "arith_count_request", norm);
        double av = 0, bv = 0;
        int names_range = arith_range_bounds(b, norm, &av, &bv);
        if (wants_count && names_range &&
            (wants_prime || wants_even || wants_odd)) {
            if (av >= -10000000.0 && av <= 10000000.0 &&
                bv >= -10000000.0 && bv <= 10000000.0 &&
                (double)(long long)av == av && (double)(long long)bv == bv) {
                long long lo = (long long)av, hi = (long long)bv;
                if (lo > hi) { long long t = lo; lo = hi; hi = t; }
                if (hi - lo > 1000000) {
                    return kb_response(b, "arith_range_too_large", NULL,
                                       out, out_size);
                }
                long long count = 0;
                for (long long n = lo; ; n++) {
                    int matches = 1;
                    if (wants_prime) matches = matches && arith_is_prime(n);
                    if (wants_even) matches = matches && (n % 2 == 0);
                    if (wants_odd) matches = matches && (n % 2 != 0);
                    if (matches) count++;
                    if (n == hi) break;
                }
                const char *property_id =
                    wants_prime && wants_even && wants_odd ? "prime_even_odd" :
                    wants_prime && wants_even ? "even_prime" :
                    wants_prime && wants_odd ? "odd_prime" :
                    wants_even && wants_odd ? "even_odd" :
                    wants_prime ? "prime" : wants_even ? "even" : "odd";
                char property_rows[1][KB_TERM_LEN];
                const char *pq[2] = { property_id, NULL };
                if (kb_match(b->kb, "number_property_label", pq, 2,
                             property_rows, 1) != 1)
                    return 0;
                const char *property = kb_dequote(property_rows[0]);
                char count_s[32], lo_s[32], hi_s[32];
                snprintf(count_s, sizeof count_s, "%lld", count);
                snprintf(lo_s, sizeof lo_s, "%lld", lo);
                snprintf(hi_s, sizeof hi_s, "%lld", hi);
                const KbResponseSlot slots[] = {
                    { "count", count_s }, { "property", property },
                    { "low", lo_s }, { "high", hi_s }
                };
                return kb_response_slots(b, "arith_range_count", slots, 4,
                                         out, out_size);
            }
        }
        if (gn == 1 && (wants_prime || wants_even || wants_odd)) {
            for (size_t i = 0; i < cnw; i++) {
                /* "no one"/"someone" is a pronoun, not the number 1 */
                if (i > 0 && lex_class_member(b, "20_math_lex1541", cw[i]) &&
                    (lex_class_member(b, "20_math_lex1542", cw[i-1]) || lex_class_member(b, "20_math_lex1542_2", cw[i-1]) ||
                     lex_class_member(b, "20_math_lex1543", cw[i-1]) || lex_class_member(b, "20_math_lex1543_2", cw[i-1])))
                    continue;
                double v; if (!parse_value(cw[i], &v)) continue;
                if ((double)(long long)v != v) break;
                long long n = (long long)v;
                char msg[96];
                if (wants_prime) {
                    { 
                      char _v0[48]; snprintf(_v0, sizeof _v0, "%s", arith_is_prime(n) ? "Yes" : "No");
                      char _v1[48]; snprintf(_v1, sizeof _v1, "%lld", n);
                      char _v2[48]; snprintf(_v2, sizeof _v2, "%s", arith_is_prime(n) ? "" : "not ");
  const KbResponseSlot _rs[] = { { "No", _v0 }, { "n", _v1 }, { "not", _v2 } };
                      kb_term_say(b, "x_x_is_xa_prime_number", _rs, 3, msg, sizeof msg); }
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
        if (arith_eval_infix(b, ew, enw, &r)) { arith_answer(r, out, out_size); return 1; }
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
static char algebra_op(Brain *b, const char *s) {
    return arith_op_char(b, s);
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
        while (m > 0 && ispunct((unsigned char)store[n][m - 1]) &&
               store[n][m - 1] != '.')
            m--;
        store[n][m] = '\0'; toks[n] = store[n]; n++;
    }
    return n;
}

/* I riempitivi di un'equazione sono CONOSCENZA (mantra #2).
 *
 * Erano ventidue parole scritte a mano qui, in due lingue: aggiungere «ricava»,
 * «determina» o una terza lingua costava una ricompilazione. La lista e' andata
 * INTERA in `equation_filler/1` — non una copia, un trasloco: una rete di
 * compatibilita' lasciata qui avrebbe reso la riga KB un ornamento, e togliere
 * un membro parlando non avrebbe avuto effetto.
 *
 * Il test operativo passa in entrambi i versi: si aggiunge un membro parlando
 * («sbloccami is an equation filler») e si toglie ritrattandolo. */
static int algebra_is_filler(Brain *b, const char *s) {
    return b && lex_class_member(b, "equation_filler", s);
}

/* ── «x + 1 = 6, x = ?» — L'INCOGNITA SI PUO' NOMINARE A PARTE ──────────────
 *
 * `solve x + 1 = 6` funzionava gia'. La forma con cui la si scrive davvero — il
 * problema, poi la domanda — no: il lettore contava due `=` e si ritirava.
 *
 * La lettura che mancava non e' un caso: un turno puo' essere fatto di piu'
 * segmenti, e un segmento il cui lato destro e' VUOTO non e' un'equazione — sta
 * NOMINANDO l'incognita. Vale per «x + 1 = 6, x = ?», per «x = ?, x + 1 = 6» e
 * per «2y = 8, y = ?» insieme, perche' guarda la forma e non le parole.
 *
 * Toglie dal turno i segmenti che nominano, e restituisce cio' che resta. */
static void algebra_drop_unknown_naming(char *buf) {
    char work[256];
    snprintf(work, sizeof work, "%s", buf);
    char kept[256];
    size_t o = 0;
    kept[0] = '\0';
    char *save = NULL;
    for (char *seg = strtok_r(work, ",;", &save); seg;
         seg = strtok_r(NULL, ",;", &save)) {
        const char *eqp = strchr(seg, '=');
        int names_only = 0;
        if (eqp) {
            const char *r = eqp + 1;
            while (*r && isspace((unsigned char)*r)) r++;
            /* Lato destro vuoto oppure il solo `?`: in entrambi i casi il
             * segmento non porta un valore, sta chiedendolo. Il `?` finale del
             * turno viene tolto prima, quindi «x = ?» in coda arriva vuoto e
             * «x = ?» in mezzo arriva col punto interrogativo: e' lo stesso
             * segmento scritto in due posizioni, non due casi. */
            while (*r == '?') r++;
            while (*r && isspace((unsigned char)*r)) r++;
            if (!*r) {
                const char *l = seg;
                while (*l && isspace((unsigned char)*l)) l++;
                size_t ll = (size_t)(eqp - l);
                while (ll > 0 && isspace((unsigned char)l[ll - 1])) ll--;
                if (ll > 0 && ll <= 3) names_only = 1;
            }
        }
        if (names_only) continue;
        while (*seg && isspace((unsigned char)*seg)) seg++;
        if (!*seg) continue;
        o += (size_t)snprintf(kept + o, sizeof kept - o, "%s%s",
                              o ? " " : "", seg);
    }
    if (kept[0]) snprintf(buf, 256, "%s", kept);
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
    algebra_drop_unknown_naming(buf);

    char store[24][KB_TERM_LEN]; char *t[24];
    size_t nt = algebra_tokenize(buf, store, t, 24);

    /* drop a leading run of filler words ("solve", "risolvi", ...). */
    size_t s0 = 0;
    while (s0 < nt && algebra_is_filler(b, t[s0])) s0++;
    if (s0 + 1 < nt && strlen(t[s0]) == 1 && isalpha((unsigned char)t[s0][0])) {
        const char *next = t[s0 + 1];
        size_t d = 0;
        while (isdigit((unsigned char)next[d]) || next[d] == '.') d++;
        if (d > 0 && next[d] == t[s0][0] && next[d + 1] == '\0')
            s0++;
    }
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
        ta = tk[0]; op = algebra_op(b, tk[1]); tb = tk[2]; tc = tk[eq + 1];
    } else if (ln == 1 && rn == 3) {
        ta = tk[eq + 1]; op = algebra_op(b, tk[eq + 2]); tb = tk[eq + 3]; tc = tk[0];
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
        char surface[2] = { op, '\0' }; int ok = 0;
        r = apply_arith_op(b, surface, av, bv, &ok);
        if (!ok) return kb_term_say(b, "arith_division_zero", NULL, 0, out, out_size);
        snprintf(rhs, sizeof rhs, "%g %c %g", av, op, bv);
    } else if (!na) {                          /* ta unknown: invert around ta */
        x = ta;
        if (!algebra_inverse(b, "left", op, cv, bv, &r, rhs, sizeof rhs)) return 0;
    } else {                                   /* tb unknown: invert around tb */
        x = tb;
        if (!algebra_inverse(b, "right", op, cv, av, &r, rhs, sizeof rhs)) return 0;
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
        if (is_conjunction(b, tk) || lex_class_member(b, "20_math_lex1839", tk) || lex_class_member(b, "20_math_lex1839_2", tk)) continue;
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
    if (learned == 1) {
        const KbResponseSlot slots[] = { { "goal", goal }, { "step", last_step } };
        kb_term_say(b, "learned_requires", slots, 2, msg, sizeof msg);
    }
    else
        { 
          char _v0[48]; snprintf(_v0, sizeof _v0, "%zu", learned);
  const KbResponseSlot _rs[] = { { "learned", _v0 }, { "goal", goal } };
          kb_term_say(b, "learned_x_prerequisites_for_x", _rs, 2, msg, sizeof msg); }
    put(msg, out, out_size);
    return learned;
}

/* gen229 (LLMSCORE behavioural resemblance): counting as a genuine CAPABILITY,
 * never a stored phrasebook. An LLM trivially "counts to five"; parrot0 should
 * too, structurally — read a target (and optional start) as a digit or a small
 * number-word and GENERATE the sequence "1, 2, 3, 4, 5.". Honest and bounded
 * (the engine computes it; nothing is recited). EN+IT cues. This is the same
 * kind of honest competence as mod_arith's "2 plus 2", not an identity claim. */
static int word_to_int(Brain *b, const char *s, long *out) {
    if (*s && (isdigit((unsigned char)*s) ||
               ((*s == '-' || *s == '+') && isdigit((unsigned char)s[1])))) {
        char *end; long v = strtol(s, &end, 10);
        if (*end == '\0') { *out = v; return 1; }
        return 0;
    }
    if (b && b->kb) {
        const char *q[] = { s, NULL };
        char hit[1][KB_TERM_LEN];
        if (kb_match(b->kb, "number_word", q, 2, hit, 1) == 1) {
            char *end; long v = strtol(kb_dequote(hit[0]), &end, 10);
            if (*end == '\0') { *out = v; return 1; }
        }
    }
    return 0;
}

static int mod_count(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    (void)raw; (void)b;
    const char *buf = norm;
    int has_cue = kb_cue_match(b, "20_math_cue1895", buf) || kb_cue_match(b, "20_math_cue1895_2", buf) ||
                  kb_cue_match(b, "20_math_cue1896", buf) || kb_cue_match(b, "20_math_cue1896_2", buf) ||
                  kb_cue_match(b, "20_math_cue1897", buf) || kb_cue_match(b, "20_math_cue1897_2", buf) ||
                  kb_cue_match(b, "20_math_cue1898", buf) || kb_cue_match(b, "20_math_cue1898_2", buf) ||
                  kb_cue_match(b, "20_math_cue1899", buf) || kb_cue_match(b, "20_math_cue1899_2", buf);
    if (!has_cue) return 0;
    if (kb_cue_match(b, "20_math_chain1903", buf))
        return 0; /* let mod_sequence infer from the provided terms */
    int descending = kb_cue_match(b, "20_math_cue1903", buf) || kb_cue_match(b, "20_math_cue1903_2", buf) ||
                     kb_cue_match(b, "20_math_cue1904", buf) || kb_cue_match(b, "20_math_cue1904_2", buf) ||
                     kb_cue_match(b, "20_math_cue1905", buf);

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
            int of_step = lex_class_member(b, "20_math_lex1920", t) && i > 0 &&
                          (lex_class_member(b, "20_math_lex1921", strip_edge_punct(sw[i - 1])) ||
                           lex_class_member(b, "20_math_lex1922", strip_edge_punct(sw[i - 1])));
            int by_step = lex_class_member(b, "20_math_lex1923", t) &&
                          !(i > 0 && lex_class_member(b, "20_math_lex1924", strip_edge_punct(sw[i - 1])));
            if (by_step || lex_class_member(b, "20_math_lex1925", t) || of_step) {
                char nx[64]; snprintf(nx, sizeof nx, "%s", strip_edge_punct(sw[i + 1]));
                size_t nl = strlen(nx);            /* "3s" -> "3" */
                if (nl > 1 && nx[nl - 1] == 's') nx[nl - 1] = '\0';
                long v; if (word_to_int(b, nx, &v) && v > 0) { stepmag = v; break; }
            }
        }
    }
    for (char *t = strtok_r(tmp, " \t,.;:!?", &save);
         t && nn < 8; t = strtok_r(NULL, " \t,.;:!?", &save)) {
        long v; if (word_to_int(b, t, &v)) {
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
        kb_term_say(b, "that_s_a_long_way_to_count_give_me_a_smaller", NULL, 0, out, out_size);
        return 1;
    }
    int only_odd = kb_cue_match(b, "20_math_cue1955", buf) || kb_cue_match(b, "20_math_cue1955_2", buf) || kb_cue_match(b, "20_math_cue1955_3", buf);
    int only_even = kb_cue_match(b, "20_math_cue1956", buf) || kb_cue_match(b, "20_math_cue1956_2", buf) || kb_cue_match(b, "20_math_cue1956_3", buf);

    /* gen241 (LLMSCORE-check): a SKIP filter. "skip any number that ends in 5" /
     * "skip multiples of 3" -> drop matching terms while counting. The digit/divisor
     * is read from after the relevant phrase; honest deductive filtering, not a memo. */
    int skip_ends = -1, skip_mult = 0;
    if (kb_cue_match(b, "20_math_chain1965", buf)) {
        char fb[512]; snprintf(fb, sizeof fb, "%s", buf);
        char *fw[64]; size_t fnw = split_words(fb, fw, 64);
        for (size_t i = 0; i + 1 < fnw; i++) {
            char *t = strip_edge_punct(fw[i]);
            if ((lex_class_member(b, "20_math_lex1967", t) || lex_class_member(b, "20_math_lex1967_2", t) || lex_class_member(b, "20_math_lex1967_3", t)) &&
                (kb_cue_match(b, "20_math_chain1972", buf))) {
                long d; if (word_to_int(b, strip_edge_punct(fw[i + 1]), &d) && d >= 0 && d <= 9)
                    skip_ends = (int)d;
            }
            if ((lex_class_member(b, "20_math_lex1972", t) || lex_class_member(b, "20_math_lex1972_2", t) || lex_class_member(b, "20_math_lex1972_3", t)) &&
                (kb_cue_match(b, "20_math_chain1978", buf))) {
                long m; if (word_to_int(b, strip_edge_punct(fw[i + 1]), &m) && m > 0)
                    skip_mult = (int)m;
            }
        }
    }

    /* gen251: replacement filter. "say buzz instead of any number divisible by 3"
     * keeps the count range intact and substitutes matching terms instead of
     * dropping them (distinct from the skip filter above). */
    char repl[32] = "";
    int repl_mult = 0;
    if ((kb_cue_match(b, "20_math_chain1990", buf)) && kb_cue_match(b, "20_math_cue1985", buf)) {
        char rb[512]; snprintf(rb, sizeof rb, "%s", buf);
        char *rw[64]; size_t rnw = split_words(rb, rw, 64);
        for (size_t i = 0; i < rnw; i++) {
            char *t = strip_edge_punct(rw[i]);
            if (lex_class_member(b, "20_math_lex1990", t) && i + 1 < rnw && !repl[0]) {
                char *word = strip_edge_punct(rw[i + 1]);
                if (*word && strlen(word) < sizeof repl) snprintf(repl, sizeof repl, "%s", word);
            }
            if (lex_class_member(b, "20_math_lex1994", t) && i + 2 < rnw &&
                lex_class_member(b, "20_math_lex1995", strip_edge_punct(rw[i + 1]))) {
                long m;
                if (word_to_int(b, strip_edge_punct(rw[i + 2]), &m) && m > 0)
                    repl_mult = (int)m;
            }
            if ((lex_class_member(b, "20_math_lex2000", t) || lex_class_member(b, "20_math_lex2000_2", t)) &&
                i + 2 < rnw && lex_class_member(b, "20_math_lex2001", strip_edge_punct(rw[i + 1]))) {
                long m;
                if (word_to_int(b, strip_edge_punct(rw[i + 2]), &m) && m > 0)
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
/* gen294 (cat.52): read a category's members for a COUNTED head noun, robust to
 * English singularization edge cases — singularize_kb(b, "senses") wrongly gives "sens"
 * (the boxes->box rule), so a strip-one-'s' fallback recovers "sense". Tries a
 * compound qualifier first ("primary colors" -> primary_color), then the
 * singularized head, then the head minus a trailing 's', then the raw token.
 * Returns the member count (0 if the noun names no category). Whole-set
 * enumeration is no longer handled here: category_enumeration/2 in the KB owns
 * that procedure and answer_frame/2 supplies its generic conversational bridge. */
static size_t enum_category_lookup(Brain *b, const char *prevtok,
                                   const char *rawhead,
                                   char members[][KB_TERM_LEN], size_t max) {
    if (!b || !b->kb || !rawhead || !*rawhead) return 0;
    char head[KB_TERM_LEN];
    singularize_kb(b, rawhead, head, sizeof head);
    if (prevtok && *prevtok) {
        char comp[KB_TERM_LEN];
        snprintf(comp, sizeof comp, "%s_%s", prevtok, head);
        const char *cp[2] = { comp, NULL };
        size_t k = domain_match(b, "membership", cp, 2, members, max);
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
        size_t k = domain_match(b, "membership", pat, 2, members, max);
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

/* Unisce una coda di token in un atomo KB («south america» -> south_america).
 * Meccanico: nessuna parola, nessuna lingua. */
static int p0_join_tail(char **w, size_t from, size_t to,
                        char *out, size_t outsz) {
    size_t o = 0;
    out[0] = '\0';
    for (size_t i = from; i < to; i++) {
        const char *t = strip_edge_punct(w[i]);
        if (!*t) continue;
        int n = snprintf(out + o, outsz - o, "%s%s", o ? "_" : "", t);
        if (n < 0 || (size_t)n >= outsz - o) return 0;
        o += (size_t)n;
    }
    return out[0] != '\0';
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
    if (kb_cue_match(b, "20_math_cue2111", buf) || kb_cue_match(b, "20_math_cue2111_2", buf) || kb_cue_match(b, "20_math_cue2111_3", buf) ||
        kb_cue_match(b, "20_math_cue2112", buf) ||
        /* gen254: the interrogative form of the same intent — "WHAT ARE the
         * three primary colors?" is the counted pick phrased as a question. */
        kb_cue_match(b, "20_math_cue2115", buf) || kb_cue_match(b, "20_math_cue2115_2", buf)) {
        if (kb_cue_match(b, "20_math_chain2121", buf))
            return 0;
        int want = 0;
        char nb[256]; snprintf(nb, sizeof nb, "%s", buf);
        char *nw0[64]; size_t nn0 = split_words(nb, nw0, 64);
        size_t numpos = nn0;
        for (size_t i = 0; i < nn0 && !want; i++) {
            char *t = strip_edge_punct(nw0[i]);
            long value = 0;
            if (word_to_int(b, t, &value) && value >= 2 && value <= 10) {
                want = (int)value; numpos = i;
            }
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

    }

    /* Una classe sola, non sette `||`: vedi `name_instance_request` in
     * kb/core/intents.p0. Una forma nuova e' una riga di KB, non una condizione
     * in piu' qui. */
    int has_name = kb_cue_match(b, "name_instance_request", buf);
    if (!has_name) return 0;
    /* gen240: a relational constraint ("name a country that BORDERS X") is beyond
     * a plain category pick — defer to the borders handler downstream rather than
     * returning an arbitrary member that ignores the constraint. */
    if (kb_cue_match(b, "20_math_chain2162", buf)) return 0;

    char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
    char *w[64]; size_t nw = split_words(tmp, w, 64);

    /* OPTIONAL initial-letter constraint: token after "letter", else after "with". */
    char init = 0;
    if (kb_cue_match(b, "20_math_chain2169", buf)) {
        size_t li = find_token(w, nw, "letter");
        if (li != nw && li + 1 < nw) init = w[li + 1][0];
        if (!init) {
            size_t wi = find_token(w, nw, "with");
            if (wi != nw && wi + 1 < nw) init = strip_edge_punct(w[wi + 1])[0];
        }
    }

    /* category: the noun right after the article (a/an/any) following "name". */
    const char *category = NULL;
    size_t ci = nw;
    size_t ni = find_token(w, nw, "name");
    for (size_t i = (ni == nw ? 0 : ni); i + 1 < nw; i++)
        if (lex_class_member(b, "20_math_lex2177", w[i]) || lex_class_member(b, "20_math_lex2177_2", w[i]) || lex_class_member(b, "20_math_lex2177_3", w[i])) {
            category = strip_edge_punct(w[i + 1]); ci = i + 1; break;
        }
    if (!category || !*category) return 0;

    const char *pat[2] = { category, NULL };
    char members[64][KB_TERM_LEN];
    size_t k = domain_match(b, "membership", pat, 2, members, 64);
    if (k == 0) return 0;   /* unknown category: let an honest wall handle it */

    /* ── IL RESIDUO DEL TURNO E' UN VINCOLO: O LO SI VERIFICA, O SI CEDE ────
     *
     * Questa facolta' sceglieva un membro della categoria e lo diceva, buttando
     * via tutto cio' che veniva dopo. Misurato in chat:
     *
     *     tell me a country in asia    ->  andorra.
     *     tell me a country in europe  ->  argentina.
     *     tell me a country in africa  ->  australia.
     *
     * Tre risposte FALSE di fila, dette come fatti — e la KB sapeva verificarle
     * («where is austria» -> Europe). Peggio di un muro: un muro si vede.
     *
     * La guardia che c'era — `20_math_chain2162`: border, neighbour, neighbor —
     * era una classe popolata dai SINTOMI, cioe' l'elenco degli incidenti del
     * gen240. Non poteva vedere «che vive nell'acqua» ne' «in asia», e non
     * avrebbe mai potuto: ogni vincolo nuovo era una parola nuova.
     *
     * La lettura giusta e' strutturale e non nomina niente: se dopo la categoria
     * resta del turno, quel residuo E' un vincolo. Si tiene solo chi lo
     * soddisfa; se nessuno lo soddisfa, questa facolta' TACE — perche' non sa se
     * il vincolo sia falso o solo non verificabile, e in nessuno dei due casi ha
     * diritto di rispondere.
     *
     * Verificare non costa vocabolario: `member_satisfies/2` in KB chiede, via
     * `kb_fact/2`, se QUALCHE relazione lega il membro al valore. Una relazione
     * nuova vale subito, e il C non ne conosce nessuna. */
    if (!init && ci + 1 < nw) {
        size_t kept = 0;
        for (size_t vi = nw; vi > ci + 1 && kept == 0; vi--) {
            /* il valore del vincolo: la coda del residuo, la piu' lunga prima */
            char value[KB_TERM_LEN];
            if (!p0_join_tail(w, vi - 1, nw, value, sizeof value)) continue;
            for (size_t i = 0; i < k; i++) {
                const char *sq[2] = { members[i], value };
                if (!kb_query(b->kb, "member_satisfies", sq, 2)) continue;
                if (kept != i) snprintf(members[kept], KB_TERM_LEN, "%s", members[i]);
                kept++;
            }
        }
        if (kept == 0) {
            /* ⚠ TACERE QUI NON BASTA, ed e' la seconda meta' della stessa
             * lezione. Cedendo in silenzio, «tell me an animal that lives in
             * water» finiva a `personal`, che rispondeva «Got it, I'll remember
             * that.»: la domanda diventava una cosa da ricordare. Un turno che
             * questa facolta' ha CAPITO — la categoria esiste — e non sa
             * onorare non va passato a chi lo capira' peggio.
             *
             * Percio' il gap si DICHIARA: si dice quale categoria si e' letta e
             * quale vincolo non si e' potuto verificare. E' la differenza fra
             * «non lo so» e «ecco un membro a caso». */
            char cbuf[KB_TERM_LEN];
            if (!p0_join_tail(w, ci + 1, nw, cbuf, sizeof cbuf))
                snprintf(cbuf, sizeof cbuf, "%s", "");
            for (char *c = cbuf; *c; c++) if (*c == '_') *c = ' ';
            const KbResponseSlot rs[] = {
                { "category", category }, { "constraint", cbuf }
            };
            char msg[256];
            kb_term_say(b, "instance_constraint_unverified", rs, 2,
                        msg, sizeof msg);
            put(msg, out, out_size);
            store_proof(b, "The category resolved but no member satisfied the "
                           "turn's remaining constraint under any known relation.");
            return 1;
        }
        k = kept;
        store_proof(b, "Kept only the category members that a known relation "
                       "links to the constraint named by the turn.");
    }

    if (init) {             /* constrained: first member with that initial */
        for (size_t i = 0; i < k; i++)
            if (members[i][0] == init) {
                char pretty[KB_TERM_LEN];
                snprintf(pretty, sizeof pretty, "%s", members[i]);
                for (char *c = pretty; *c; c++) if (*c == '_') *c = ' ';
                char msg[200];
                kb_term_say(b, "instance_named", (const KbResponseSlot[]){
                                { "Member", pretty }, { "category", category } },
                            2, msg, sizeof msg);
                put(msg, out, out_size); return 1;
            }
        char msg[200];
        { 
          char _v1[48]; snprintf(_v1, sizeof _v1, "%c", init);
  const KbResponseSlot _rs[] = { { "category", category }, { "init", _v1 } };
          kb_term_say(b, "i_can_t_think_of_a_x_starting_with_x_from_wh", _rs, 2, msg, sizeof msg); }
        put(msg, out, out_size);
        return 1;
    }

    /* unconstrained ("name any animal"): return a member, rotating for variety. */
    size_t idx = b->response_pick % k;
    b->response_pick++;
    /* La resa e' KB (mantra #16), e l'atomo si dice in lingua: `carbon_dioxide`
     * e' come parrot0 lo memorizza, non come lo si dice. */
    char pretty[KB_TERM_LEN];
    snprintf(pretty, sizeof pretty, "%s", members[idx]);
    for (char *c = pretty; *c; c++) if (*c == '_') *c = ' ';
    char msg[200];
    kb_term_say(b, "instance_named", (const KbResponseSlot[]){
                    { "Member", pretty }, { "category", category } },
                2, msg, sizeof msg);
    put(msg, out, out_size);
    return 1;
}

/* ── Motore 1: lexical enumeration under a computable constraint (gen347) ────────
 * A word puzzle — "words that rhyme with X", "N-letter words from the letters L"
 * — is ENUMERATE(lexeme, constraint): the POOL (lexeme/1) is knowledge that grows,
 * the CONSTRAINTS (rhyme = shared final rime; buildable = letter-multiset subset;
 * length) are fixed string MOTORS, and the query TYPE is a KB intent (intent_cue,
 * matched by the shared hypothesis scorer). One mechanism, a whole family of
 * puzzles; adding one word extends them all. The pool is lazy-loaded so it never
 * bloats an ordinary brain (docs/plans/universal-input.md: motor in C, forms in KB). */
static void ensure_lexeme(Brain *b) {
    if (!b || !b->kb || b->lexeme_kb_loaded) return;
    kb_set_origin(b->kb, KB_BASE);
    kb_load(b->kb, "kb/core/lexeme.p0");
    kb_set_origin(b->kb, KB_SESSION);
    b->lexeme_kb_loaded = 1;
}

/* the rime: T's final K letters (K=3 for longer words, 2 for short) — an
 * orthographic near-rhyme, exactly what the task allows ("need not be perfect"). */
static int rime_of(const char *t, char *suf, size_t sz) {
    size_t l = strlen(t);
    if (l < 2) return 0;
    size_t k = (l >= 5) ? 3 : 2;
    if (k >= l) k = l - 1;
    snprintf(suf, sz, "%s", t + (l - k));
    return 1;
}

static int wq_num(Brain *b, const char *w) {
    long value = 0;
    if (word_to_int(b, w, &value) && value > 0 && value <= INT_MAX) return (int)value;
    if (w[0] >= '1' && w[0] <= '9') return atoi(w);
    return 0;
}

static int wq_max_double_runs(const char *word) {
    int best = 0, cur = 0;
    size_t n = strlen(word);
    for (size_t i = 0; i + 1 < n;) {
        if (isalpha((unsigned char)word[i]) &&
            tolower((unsigned char)word[i]) == tolower((unsigned char)word[i + 1])) {
            cur++;
            if (cur > best) best = cur;
            i += 2;
        } else {
            cur = 0;
            i++;
        }
    }
    return best;
}

static int wq_letter_counts(const char *s, int counts[26]) {
    int any = 0;
    memset(counts, 0, 26 * sizeof counts[0]);
    for (const char *p = s; p && *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (!isalpha(ch)) continue;
        counts[tolower(ch) - 'a']++;
        any = 1;
    }
    return any;
}

static int wq_same_letters(const char *a, const char *b) {
    int ca[26], cb[26];
    if (!wq_letter_counts(a, ca) || !wq_letter_counts(b, cb)) return 0;
    for (int i = 0; i < 26; i++) if (ca[i] != cb[i]) return 0;
    return 1;
}

#define WQ_MAX 40000
static int mod_wordquery(Brain *b, const char *norm, const char *raw,
                         char *out, size_t out_size) {
    if (!b || !b->kb || !norm) return 0;
    if (p0_faculty_yields(b, "wordquery", "open", norm, NULL)) return 0;
    int is_rhyme   = kb_cue_match(b, "word_rhyme", norm);
    int is_letters = kb_cue_match(b, "word_from_letters", norm);
    int is_starts  = kb_cue_match(b, "word_starts_with", norm);
    int is_double  = kb_cue_match(b, "word_double_runs", norm);
    int is_full_anagram = kb_cue_match(b, "full_anagram_request", norm) ||
        (is_letters && kb_cue_match(b, "anagram_request", norm) &&
         kb_cue_match(b, "anagram_output", norm));
    if (!is_rhyme && !is_letters && !is_starts && !is_double && !is_full_anagram) return 0;

    /* parse the shared modifiers: a length filter ("N letter(s)" / "N-letter"),
     * a list size ("five words"), and count vs list mode ("how many"). */
    char nb[512]; snprintf(nb, sizeof nb, "%s", norm);
    char *w[80]; size_t nw = split_words(nb, w, 80);
    int want_count = kb_cue_match(b, "20_math_cue2299", norm) || kb_cue_match(b, "20_math_cue2299_2", norm);
    int list_n = 0, len_filter = 0;
    for (size_t i = 0; i < nw; i++) {
        char *t = strip_edge_punct(w[i]);
        if (strstr(t, "-letter")) { int v = wq_num(b, t); if (v > 0) len_filter = v; continue; }
        int v = wq_num(b, t);
        if (v <= 0) continue;
        int is_len = (i + 1 < nw &&lex_prefix_member(b, "20_math_lex2306", strip_edge_punct(w[i + 1])));
        if (is_len) len_filter = v; else if (!list_n) list_n = v;
    }

    ensure_lexeme(b);
    char (*pool)[KB_TERM_LEN] = malloc((size_t)WQ_MAX * KB_TERM_LEN);
    if (!pool) return 0;
    const char *anyq[] = { NULL };
    size_t np = kb_match(b->kb, "lexeme", anyq, 1, pool, WQ_MAX);

    char hits[64][KB_TERM_LEN]; size_t nh = 0; int total = 0;

    if (is_full_anagram) {
        char srcbuf[KB_TERM_LEN] = "";
        if (raw) {
            const char *q1 = strchr(raw, '"');
            if (q1) {
                const char *q2 = strchr(q1 + 1, '"');
                if (q2 && q2 > q1 + 1) {
                    size_t n = (size_t)(q2 - q1 - 1);
                    if (n >= sizeof srcbuf) n = sizeof srcbuf - 1;
                    memcpy(srcbuf, q1 + 1, n); srcbuf[n] = '\0';
                }
            }
        }
        if (!srcbuf[0]) {
            for (size_t i = 0; i + 1 < nw && !srcbuf[0]; i++) {
                char *t = strip_edge_punct(w[i]);
                if (!lex_class_member(b, "20_math_lex2334", t) && !lex_class_member(b, "20_math_lex2334_2", t)) continue;
                char *src = strip_edge_punct(w[i + 1]);
                if (lex_class_member(b, "20_math_lex2336", src) && i + 2 < nw) src = strip_edge_punct(w[i + 2]);
                if (strlen(src) >= 3) snprintf(srcbuf, sizeof srcbuf, "%s", src);
            }
        }
        for (char *p = srcbuf; *p; p++) *p = (char)tolower((unsigned char)*p);
        strip_edge_punct(srcbuf);
        if (srcbuf[0]) {
            const char *q[] = { srcbuf, NULL };
            char hit[8][KB_TERM_LEN];
            size_t hn = kb_match(b->kb, "anagram_of", q, 2, hit, 8);
            for (size_t i = 0; i < hn && nh < 64; i++) {
                char *p = kb_dequote(hit[i]);
                if (wq_same_letters(srcbuf, p) && strcmp(srcbuf, p))
                    snprintf(hits[nh++], KB_TERM_LEN, "%s", p);
            }
            for (size_t i = 0; i < np && nh < 64; i++) {
                const char *lx = pool[i];
                if (strcmp(lx, srcbuf) && wq_same_letters(srcbuf, lx) &&
                    !seen_term(hits, nh, lx))
                    snprintf(hits[nh++], KB_TERM_LEN, "%s", lx);
            }
            free(pool);
            if (nh == 0) return 0;
            char msg[160]; snprintf(msg, sizeof msg, "\"%s\".", hits[0]);
            put(msg, out, out_size);
            return 1;
        }
    }

    if (is_double) {
        int need_runs = 0;
        for (size_t i = 0; i < nw; i++) {
            int v = wq_num(b, strip_edge_punct(w[i]));
            if (v > 0) { need_runs = v; break; }
        }
        if (need_runs <= 0) need_runs = 2;
        for (size_t i = 0; i < np && nh < 64; i++) {
            const char *lx = pool[i];
            if (wq_max_double_runs(lx) < need_runs) continue;
            snprintf(hits[nh++], KB_TERM_LEN, "%s", lx);
        }
        free(pool);
        if (nh == 0) return 0;
        for (size_t i = 0; i + 1 < nh; i++)
            for (size_t j = i + 1; j < nh; j++)
                if (strlen(hits[j]) < strlen(hits[i])) {
                    char tmp[KB_TERM_LEN]; snprintf(tmp, sizeof tmp, "%s", hits[i]);
                    snprintf(hits[i], KB_TERM_LEN, "%s", hits[j]);
                    snprintf(hits[j], KB_TERM_LEN, "%s", tmp);
                }
        char runs[16];
        snprintf(runs, sizeof runs, "%d", need_runs);
        const KbResponseSlot slots[] = {
            { "word", hits[0] },
            { "runs", runs }
        };
        if (kb_response_slots(b, "word_double_runs_answer", slots, 2,
                              out, out_size)) return 1;
        char msg[160];
        snprintf(msg, sizeof msg, "%s.", hits[0]);
        if (msg[0]) msg[0] = (char)toupper((unsigned char)msg[0]);
        put(msg, out, out_size);
        return 1;
    }

    if (is_rhyme) {
        const char *target = NULL;
        for (size_t i = 0; i < nw; i++)
            if (lex_class_member(b, "20_math_lex2404", strip_edge_punct(w[i])) && i + 1 < nw) {
                target = strip_edge_punct(w[i + 1]); break;
            }
        char suf[8];
        if (!target || !*target || !rime_of(target, suf, sizeof suf)) { free(pool); return 0; }
        size_t ls = strlen(suf), want = (size_t)(list_n > 0 ? list_n : 5);
        /* collect all matches, then prefer the shortest — a good rhyme is usually a
         * short common word ("cat" -> hat, not acrobat), which reads more like an
         * LLM than the alphabetically-first long word. */
        for (size_t i = 0; i < np && nh < 64; i++) {
            const char *lx = pool[i]; size_t ll = strlen(lx);
            if (ll <= ls || strcmp(lx + (ll - ls), suf) != 0) continue;
            if (strcmp(lx, target) == 0) continue;
            snprintf(hits[nh++], KB_TERM_LEN, "%s", lx);
        }
        free(pool);
        for (size_t i = 0; i + 1 < nh; i++)             /* stable sort by length asc */
            for (size_t j = i + 1; j < nh; j++)
                if (strlen(hits[j]) < strlen(hits[i])) {
                    char tmp[KB_TERM_LEN]; snprintf(tmp, sizeof tmp, "%s", hits[i]);
                    snprintf(hits[i], KB_TERM_LEN, "%s", hits[j]);
                    snprintf(hits[j], KB_TERM_LEN, "%s", tmp);
                }
        if (nh > want) nh = want;
        if (nh == 0) return 0;
        char list[512]; size_t o = 0;
        for (size_t i = 0; i < nh && o + 4 < sizeof list; i++)
            o += (size_t)snprintf(list + o, sizeof list - o, "%s%s",
                                  i ? (i + 1 == nh ? " and " : ", ") : "", hits[i]);
        char target_slot[KB_TERM_LEN];
        snprintf(target_slot, sizeof target_slot, "%s", target);
        kb_term_say(b, "near_rhymes_for_x", (const KbResponseSlot[]){
                        { "target", target_slot }, { "list", list } },
                    2, out, out_size);
        return 1;
    }

    /* starts-with: a first-letter filter over the lexeme. The letter is the token
     * after "with"/"letter" (a quoted 'q' or a bare q). Prefer short common words,
     * like the rhyme branch, so the reply reads like an LLM's pick. */
    if (is_starts) {
        char letter = 0;
        for (size_t i = 0; i + 1 < nw; i++) {
            char *t = strip_edge_punct(w[i]);
            if (!lex_class_member(b, "20_math_lex2446", t) && !lex_class_member(b, "20_math_lex2446_2", t)) continue;
            char *nx = strip_edge_punct(w[i + 1]);
            if (nx[0] && isalpha((unsigned char)nx[0])) {
                letter = (char)tolower((unsigned char)nx[0]); break;
            }
        }
        if (!letter) { free(pool); return 0; }
        for (size_t i = 0; i < np && nh < 64; i++) {
            const char *lx = pool[i];
            if ((char)tolower((unsigned char)lx[0]) != letter) continue;
            if (len_filter && (int)strlen(lx) != len_filter) continue;
            snprintf(hits[nh++], KB_TERM_LEN, "%s", lx);
        }
        free(pool);
        if (nh == 0) return 0;
        for (size_t i = 0; i + 1 < nh; i++)
            for (size_t j = i + 1; j < nh; j++)
                if (strlen(hits[j]) < strlen(hits[i])) {
                    char tmp[KB_TERM_LEN]; snprintf(tmp, sizeof tmp, "%s", hits[i]);
                    snprintf(hits[i], KB_TERM_LEN, "%s", hits[j]);
                    snprintf(hits[j], KB_TERM_LEN, "%s", tmp);
                }
        size_t want = list_n > 0 ? (size_t)list_n : 1;
        if (nh > want) nh = want;
        char msg[400];
        size_t o = (size_t)snprintf(msg, sizeof msg,
            nh > 1 ? "Words that start with \"%c\": " : "A word that starts with \"%c\": ",
            letter);
        for (size_t i = 0; i < nh && o + 4 < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                  i ? (i + 1 == nh ? " and " : ", ") : "", hits[i]);
        if (o + 2 < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
        put(msg, out, out_size);
        return 1;
    }

    /* from-letters: the letter pool is a quoted span in the raw turn, else the
     * words right after a "letters in/of/from" marker. */
    int avail[26] = {0}; int have_src = 0;
    if (raw) {
        const char *q1 = strchr(raw, '"');
        if (q1) { const char *q2 = strchr(q1 + 1, '"');
            if (q2) { for (const char *p = q1 + 1; p < q2; p++) {
                char c = (char)tolower((unsigned char)*p);
                if (c >= 'a' && c <= 'z') { avail[c - 'a']++; have_src = 1; } } } }
    }
    if (!have_src) {                       /* fall back to words after the marker */
        for (size_t i = 0; i + 1 < nw; i++) {
            char *t = strip_edge_punct(w[i]);
            if (lex_class_member(b, "20_math_lex2495", t) || lex_class_member(b, "20_math_lex2495_2", t)) {
                for (size_t j = i + 1; j < nw; j++) {
                    char *s = strip_edge_punct(w[j]);
                    if (lex_class_member(b, "20_math_lex2498", s) || lex_class_member(b, "20_math_lex2498_2", s) || lex_class_member(b, "20_math_lex2498_3", s) ||
                        lex_class_member(b, "20_math_lex2499", s)) continue;
                    /* single letters ARE the payload ("t, s, a, r" — 'a' is a
                     * letter here, not the article); only a multi-char stopword
                     * ends the list. */
                    if (strlen(s) > 1 && is_stopword(b, s)) break;
                    for (char *p = s; *p; p++) { char c = (char)tolower((unsigned char)*p);
                        if (c >= 'a' && c <= 'z') { avail[c - 'a']++; have_src = 1; } }
                }
                break;
            }
        }
    }
    if (!have_src) { free(pool); return 0; }

    for (size_t i = 0; i < np; i++) {
        const char *lx = pool[i]; size_t ll = strlen(lx);
        if (len_filter && (int)ll != len_filter) continue;
        int need[26] = {0}, ok = 1;
        for (const char *p = lx; *p; p++) if (*p >= 'a' && *p <= 'z') need[*p - 'a']++;
        for (int c = 0; c < 26 && ok; c++) if (need[c] > avail[c]) ok = 0;
        if (!ok) continue;
        total++;
        if (nh < 64) snprintf(hits[nh++], KB_TERM_LEN, "%s", lx);
    }
    free(pool);
    if (total == 0) return 0;

    char msg[700];
    if (want_count) {
        char _t1[512];
        char _t1_v0[48]; snprintf(_t1_v0, sizeof _t1_v0, "%d", total);
        char _t1_v1[48]; snprintf(_t1_v1, sizeof _t1_v1, "%s", len_filter ? "" : "");
        char _t1_v2[48]; snprintf(_t1_v2, sizeof _t1_v2, "%s", total == 1 ? "" : "s");
        const KbResponseSlot _r1[] = { { "total", _t1_v0 }, { "len_filter", _t1_v1 }, { "s", _t1_v2 } };
        kb_term_say(b, "i_count_xx_wordx_you_can_build", _r1, 3, _t1, sizeof _t1);
        size_t o = (size_t)snprintf(msg, sizeof msg, "%s", _t1);
        size_t show = nh < 12 ? nh : 12;
        for (size_t i = 0; i < show && o + 4 < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s", i ? ", " : "", hits[i]);
        if (nh > show && o + 8 < sizeof msg) o += (size_t)snprintf(msg + o, sizeof msg - o, ", …");
        if (o + 2 < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
    } else {
        size_t want = list_n > 0 ? (size_t)list_n : (nh < 8 ? nh : 8);
        char _t2[512];
        const KbResponseSlot _r2[] = { { "x", "" } };
        kb_term_say(b, "you_can_make", _r2, 0, _t2, sizeof _t2);
        size_t o = (size_t)snprintf(msg, sizeof msg, "%s", _t2);
        for (size_t i = 0; i < nh && i < want && o + 4 < sizeof msg; i++)
            o += (size_t)snprintf(msg + o, sizeof msg - o, "%s%s",
                                  i ? (i + 1 == want ? " and " : ", ") : "", hits[i]);
        if (o + 2 < sizeof msg) snprintf(msg + o, sizeof msg - o, ".");
    }
    put(msg, out, out_size);
    return 1;
}

/* ── ONE generic curated-answer motor (gen347) ──────────────────────────────────
 * The KB-first cure for "a module per capability": a recognized request whose
 * answer is a fixed piece of KNOWLEDGE — a suggestion, a constrained sentence, a
 * specific fact the structured KB can't derive — is NOT a new C module. It is a
 * pair of facts read by ONE motor:
 *     qa_cue(Id, "word")   — a discriminative word the request must contain (many
 *                            per Id: ALL must be present for Id to fire)
 *     qa_reply(Id, "text") — the answer, spoken verbatim
 * The most-specific Id (most cues matched) wins, so a broad request never steals a
 * narrow one. A whole new curated capability — advice, a sentence with fixed
 * anchors, a tricky historical detail — costs facts, zero C. (Computation-bearing
 * capabilities — rhyme/anagram enumeration, question decomposition — stay real
 * motors; only the "retrieve curated knowledge by cue" family collapses here.) */
/* INSEGNARE UNA RISPOSTA PARLANDO (gen382c).
 *
 * mod_qa legge qa_cue/qa_reply dal gen347 — "quando la richiesta contiene queste
 * parole, rispondi questa" — ma quella coppia si poteva alimentare solo
 * scrivendo nei file. Il motore c'era e l'ATTO no, che e' il gemello del
 * "consumer gap" di gen306: li' un fatto senza chi lo interrogasse, qui un
 * interrogatore senza modo di essere istruito.
 *
 * Questa funzione non e' un secondo motore di risposte: ritaglia i due lati di
 * "quando ti chiedo di X rispondi Y" e li asserisce come la stessa coppia che
 * mod_qa gia' consuma. Tutto cio' che e' vocabolario sta in KB — le formulazioni
 * dell'atto (intent_cue(teach_reply, ...)), il separatore
 * (teach_reply_pivot/1), la conferma (response_template(taught_reply, ...)) —
 * quindi una nuova formulazione, o una nuova lingua, e' un fatto.
 *
 * L'id e' derivato dalla situazione, cosi' insegnare due volte sulla stessa
 * cosa CORREGGE invece di accumulare: e' "al posto di rispondere cosi', adesso
 * rispondi cosi'". */
static int mod_teach_reply(Brain *b, const char *norm, const char *raw,
                           char *out, size_t out_size) {
    if (!b || !b->kb || !norm) return 0;
    if (!kb_cue_match(b, "teach_reply", norm)) return 0;

    /* gen428 — IL SEPARATORE SI CERCA DOPO LA FORMULAZIONE DELL'ATTO.
     *
     * Misurato provando a insegnare una risposta parlando: «when i say bonjour
     * answer hello there» finiva in «when you ask about I will say so», con la
     * situazione ridotta a «i». Il motivo e' una collisione, non un difetto di
     * quella frase: «say» e' anche un separatore, quindi il taglio cadeva DENTRO
     * la formulazione dell'atto («when i | say ...») e la situazione spariva.
     *
     * Cercare il separatore solo DOPO la cue toglie l'intera classe di
     * collisioni: da qui in avanti una formulazione nuova puo' contenere una
     * parola che altrove separa, e resta una riga di KB. */
    size_t cue_end = 0;
    {
        char icues[16][KB_TERM_LEN];
        const char *icq[] = { "teach_reply", NULL };
        size_t inc = kb_match(b->kb, "intent_cue", icq, 2, icues, 16);
        for (size_t i = 0; i < inc; i++) {
            const char *c = kb_dequote(icues[i]);
            const char *at = c && *c ? strstr(norm, c) : NULL;
            if (at && (size_t)(at - norm) + strlen(c) > cue_end)
                cue_end = (size_t)(at - norm) + strlen(c);
        }
    }
    /* il separatore fra la situazione e la risposta: quale parola lo sia e' KB */
    char pivots[16][KB_TERM_LEN];
    const char *pq[] = { NULL };
    size_t npv = kb_match(b->kb, "teach_reply_pivot", pq, 1, pivots, 16);
    const char *cut = NULL; size_t plen = 0;
    for (size_t i = 0; i < npv; i++) {
        char pat[KB_TERM_LEN];
        snprintf(pat, sizeof pat, " %s ", kb_dequote(pivots[i]));
        const char *hit = strstr(norm + cue_end, pat);
        if (hit && (!cut || hit < cut)) { cut = hit; plen = strlen(pat); }
    }
    if (!cut) return 0;

    /* la SITUAZIONE: cio' che sta fra la formulazione dell'atto e il separatore */
    char lhs[256];
    size_t ll = (size_t)(cut - norm);
    if (ll >= sizeof lhs) ll = sizeof lhs - 1;
    memcpy(lhs, norm, ll); lhs[ll] = '\0';

    char cues[8][KB_TERM_LEN];
    const char *cq[] = { "teach_reply", NULL };
    size_t nc = kb_match(b->kb, "intent_cue", cq, 2, cues, 8);
    const char *topic = lhs;
    for (size_t i = 0; i < nc; i++) {
        const char *c = kb_dequote(cues[i]);
        const char *at = strstr(lhs, c);
        if (at && at + strlen(c) > topic) topic = at + strlen(c);
    }
    while (*topic == ' ') topic++;
    /* La canonicalizzazione porta "del" a "of the": quelle parole aprono o
     * chiudono un sintagma, non fanno parte della SITUAZIONE. Si tolgono
     * leggendo le classi che gia' esistono — nessun elenco nuovo. */
    for (;;) {
        char first[KB_TERM_LEN]; size_t k = 0;
        while (topic[k] && topic[k] != ' ' && k + 1 < sizeof first) { first[k] = topic[k]; k++; }
        first[k] = '\0';
        if (!k || !topic[k]) break;
        const char *fq[] = { first };
        if (!kb_query(b->kb, "np_opener", fq, 1) && !kb_query(b->kb, "np_closer", fq, 1))
            break;
        topic += k;
        while (*topic == ' ') topic++;
    }
    if (!*topic) return 0;

    /* La SITUAZIONE si cerca nella forma canonica, perche' li' va confrontata.
     * La RISPOSTA no: e' testo da PRONUNCIARE, non struttura da analizzare, e
     * presa dal canonico uscirebbe deformata ("non ho sensori" -> "not i have
     * sensori"). Si ritaglia dal turno grezzo, dopo lo stesso separatore. */
    const char *reply = cut + plen;
    if (raw && *raw) {
        for (size_t i = 0; i < npv; i++) {
            char pat[KB_TERM_LEN];
            snprintf(pat, sizeof pat, " %s ", kb_dequote(pivots[i]));
            const char *hit = strstr(raw, pat);
            if (hit) { reply = hit + strlen(pat); break; }
        }
    }
    while (*reply == ' ') reply++;
    if (!*reply) return 0;

    /* Un id derivato dalla situazione: reinsegnare la stessa cosa CORREGGE. */
    char id[KB_TERM_LEN];
    snprintf(id, sizeof id, "taught_%.*s", (int)sizeof id - 12, topic);
    for (char *c = id; *c; c++) if (*c == ' ') *c = '_';

    char q_cue[KB_TERM_LEN], q_rep[KB_TERM_LEN];
    snprintf(q_cue, sizeof q_cue, "\"%s\"", topic);
    snprintf(q_rep, sizeof q_rep, "\"%s\"", reply);
    kb_set_origin(b->kb, KB_SESSION);
    /* Reinsegnare la stessa situazione CORREGGE invece di accumulare: e' il
     * senso di "al posto di rispondere cosi', adesso rispondi cosi'". Si toglie
     * il valore vecchio leggendolo, perche' la retrazione vuole il fatto esatto. */
    for (const char *pred = "qa_cue"; pred; pred = (pred[3] == 'c') ? "qa_reply" : NULL) {
        char old_v[4][KB_TERM_LEN];
        const char *oq[] = { id, NULL };
        size_t no = kb_match(b->kb, pred, oq, 2, old_v, 4);
        for (size_t i = 0; i < no; i++) {
            const char *ra_old[] = { id, old_v[i] };
            kb_retract(b->kb, pred, ra_old, 2);
        }
    }
    const char *ca[] = { id, q_cue };
    const char *ra[] = { id, q_rep };
    if (!kb_assert(b->kb, "qa_cue", ca, 2) ||
        !kb_assert(b->kb, "qa_reply", ra, 2)) return 0;

    const KbResponseSlot sl[] = { { "cue", topic } };
    if (kb_response_slots(b, "taught_reply", sl, 1, out, out_size)) return 1;
    kb_term_say(b, "understood", NULL, 0, out, out_size);
    return 1;
}

static int mod_qa(Brain *b, const char *norm, const char *raw,
                  char *out, size_t out_size) {
    (void)raw;
    if (!b || !b->kb || !norm) return 0;
    char ids[128][KB_TERM_LEN];
    const char *aq[2] = { NULL, NULL };
    size_t nid = kb_match(b->kb, "qa_reply", aq, 2, ids, 128);   /* the Id column */
    const char *best = NULL; size_t best_id = 0, best_n = 0;
    for (size_t i = 0; i < nid; i++) {
        char cues[16][KB_TERM_LEN];
        const char *cq[2] = { ids[i], NULL };
        size_t nc = kb_match(b->kb, "qa_cue", cq, 2, cues, 16);
        if (nc == 0) continue;
        int all = 1;
        for (size_t c = 0; c < nc; c++) {
            char cb[KB_TERM_LEN]; snprintf(cb, sizeof cb, "%s", cues[c]);
            const char *cd = kb_dequote(cb);
            if (!*cd || !cue(norm, cd)) { all = 0; break; }
        }
        if (all && nc > best_n) { best_n = nc; best = ids[i]; best_id = i; }
    }
    if (!best) return 0;
    const char *rq[2] = { ids[best_id], NULL };
    char rep[1][KB_TERM_LEN];
    if (kb_match(b->kb, "qa_reply", rq, 2, rep, 1) != 1) return 0;
    put(kb_dequote(rep[0]), out, out_size);
    store_proof(b, "Answered from a qa_cue/qa_reply pattern in the KB.");
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
    if (!(kb_cue_match(b, "20_math_chain2745", buf)))
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
/* gen385 — I VICINI DI UNA STRINGA, per deformazione inversa.
 *
 * Le OPERAZIONI sui caratteri sono meccanica fissa e cieca alla lingua: togliere
 * una lettera, aggiungerne una, sostituirne una, scambiare due adiacenti. QUALI
 * classi provare e' conoscenza — `surface_variation(Classe, Operazione)` — quindi
 * una classe nuova (accento caduto, spazio mancante) costa un fatto e nessun
 * motore. La VALIDAZIONE e' `lexeme/1`: un candidato che non e' una parola nota
 * non esce di qui, ed e' precisamente cio' che rende questa riparazione piu'
 * affidabile di quella di un LLM (question-emergence.md §13.2).
 *
 * Restituisce quanti candidati distinti ha scritto in `out`. */
static size_t p0_repair_candidates(Brain *b, const char *word,
                                   char out[][64], size_t max) {
    if (!b || !b->kb || !word || max == 0) return 0;
    size_t wl = strlen(word);
    if (wl < 3 || wl > 40) return 0;

    char ops[8][KB_TERM_LEN];
    const char *vq[2] = { NULL, NULL };
    size_t nops = kb_match(b->kb, "surface_variation", vq, 2, ops, 8);
    if (nops == 0) return 0;

    size_t n = 0;
    char buf[64];
    for (size_t oi = 0; oi < nops && n < max; oi++) {
        char row[1][KB_TERM_LEN];
        const char *rq[2] = { ops[oi], NULL };
        if (kb_match(b->kb, "surface_variation", rq, 2, row, 1) != 1) continue;
        char opbuf[KB_TERM_LEN];
        snprintf(opbuf, sizeof opbuf, "%s", row[0]);
        const char *op = kb_dequote(opbuf);

        for (size_t i = 0; i <= wl && n < max; i++) {
            int tries = 1;
            if (!strcmp(op, "substitute_one") || !strcmp(op, "insert_one")) tries = 26;
            for (int c = 0; c < tries && n < max; c++) {
                if (!strcmp(op, "drop_one")) {
                    if (i >= wl) break;
                    snprintf(buf, sizeof buf, "%.*s%s", (int)i, word, word + i + 1);
                } else if (op[0] == 's' && !strcmp(op, "substitute_one")) {
                    if (i >= wl) break;
                    if (word[i] == 'a' + c) continue;
                    snprintf(buf, sizeof buf, "%.*s%c%s", (int)i, word,
                             (char)('a' + c), word + i + 1);
                } else if (!strcmp(op, "insert_one")) {
                    snprintf(buf, sizeof buf, "%.*s%c%s", (int)i, word,
                             (char)('a' + c), word + i);
                } else if (!strcmp(op, "swap_adjacent")) {
                    if (i + 1 >= wl) break;
                    snprintf(buf, sizeof buf, "%.*s%c%c%s", (int)i, word,
                             word[i + 1], word[i], word + i + 2);
                } else break;

                if (strlen(buf) < 2) continue;
                const char *lq[] = { buf };
                if (!kb_query(b->kb, "lexeme", lq, 1)) continue;
                int dup = 0;
                for (size_t k = 0; k < n; k++) if (!strcmp(out[k], buf)) dup = 1;
                if (!dup) snprintf(out[n++], 64, "%s", buf);
            }
        }
    }
    return n;
}

static int mod_spell(Brain *b, const char *norm, const char *raw,
                     char *out, size_t out_size) {
    const char *buf = norm;

    /* LLMSCORE gen333: initials are a transformation over the QUOTED byte span,
     * not an anagram lookup over whichever later token happens to be known.  The
     * old knowledge handler saw "letters of" + "form", eventually encountered
     * `dog`, and confidently answered its stored anagram `god`.  Compute the
     * requested projection directly so held-out phrases transfer. */
    int wants_initials =
        kb_cue_match(b, "initials_projection", buf) &&
        kb_cue_match(b, "initials_word_scope", buf) &&
        !kb_cue_match(b, "initials_exclusion", buf);
    if (wants_initials && raw) {
        /* Choose the first quoted data span after both recognized cue roles.
         * Their byte positions come from the same KB evidence used by
         * kb_cue_match, so a quoted aside before the instruction cannot become
         * the projection target. */
        size_t anchor = 0;
        KbEvidenceMatch cues[16];
        size_t nc = kb_evidence_matches(b->kb, "intent_cue",
                                        "initials_projection", raw, cues, 16);
        if (nc) anchor = cues[0].start + cues[0].len;
        nc = kb_evidence_matches(b->kb, "intent_cue",
                                 "initials_word_scope", raw, cues, 16);
        if (nc && cues[0].start + cues[0].len > anchor)
            anchor = cues[0].start + cues[0].len;
        const char *q1 = NULL, *q2 = NULL, *scan = raw;
        for (;;) {
            const char *open = strchr(scan, '"');
            const char *close = open ? strchr(open + 1, '"') : NULL;
            if (!open || !close) break;
            if ((size_t)(open - raw) >= anchor) {
                q1 = open; q2 = close; break;
            }
            scan = close + 1;
        }
        if (q1 && q2 && q2 > q1 + 1) {
            char initials[KB_TERM_LEN]; size_t n = 0;
            int in_word = 0;
            for (const char *p = q1 + 1; p < q2; p++) {
                unsigned char c = (unsigned char)*p;
                if (isalnum(c)) {
                    if (!in_word && n + 1 < sizeof initials)
                        initials[n++] = (char)toupper(c);
                    in_word = 1;
                } else if (c != '\'' && c != '-') {
                    in_word = 0;
                }
            }
            initials[n] = '\0';
            if (n >= 1) {
                char quoted[KB_TERM_LEN + 3];
                snprintf(quoted, sizeof quoted, "\"%s\"", initials);
                return kb_response(b, "initials_projection", quoted,
                                   out, out_size);
            }
        }
    }

    /* gen246: sequential word transformation. The word is data from the prompt;
     * operations are structural ("remove first/last letter", "add letter X to
     * end/start"), so held-out words transfer. */
    if ((kb_cue_match(b, "20_math_chain2959", buf)) &&
        kb_cue_match(b, "20_math_cue2950", buf) && kb_cue_match(b, "20_math_cue2950_2", buf)) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
        char *w[64]; size_t nw = split_words(tmp, w, 64);
        char word[128] = "";
        for (size_t i = 0; i + 1 < nw && !word[0]; i++) {
            char *t = strip_edge_punct(w[i]);
            if (!lex_class_member(b, "20_math_lex2956", t) && !lex_class_member(b, "20_math_lex2956_2", t)) continue;
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

    if ((kb_cue_match(b, "20_math_cue2978", buf) || kb_cue_match(b, "20_math_cue2978_2", buf) || cue(buf, "word \"")) &&
        (kb_cue_match(b, "20_math_chain2989", buf)) &&
        kb_cue_match(b, "20_math_cue2980", buf) && kb_cue_match(b, "20_math_cue2980_2", buf)) {
        char tmp[256]; snprintf(tmp, sizeof tmp, "%s", buf);
        char *w[64]; size_t nw = split_words(tmp, w, 64);
        char word[128] = "", add[8] = "";
        for (size_t i = 0; i + 1 < nw && !word[0]; i++) {
            if (!lex_class_member(b, "20_math_lex2985", strip_edge_punct(w[i]))) continue;
            char *t = strip_edge_punct(w[i + 1]);
            int alpha = 1;
            for (size_t k = 0; t[k]; k++)
                if (!isalpha((unsigned char)t[k])) { alpha = 0; break; }
            if (alpha && strlen(t) > 1) snprintf(word, sizeof word, "%s", t);
        }
        for (size_t i = 0; i < nw && !add[0]; i++) {
            if (!lex_class_member(b, "20_math_lex2993", strip_edge_punct(w[i]))) continue;
            for (size_t j = i + 1; j < nw; j++) {
                char *t = strip_edge_punct(w[j]);
                if (lex_class_member(b, "20_math_lex2996", t) || lex_class_member(b, "20_math_lex2996_2", t) || lex_class_member(b, "20_math_lex2996_3", t) ||
                    lex_class_member(b, "20_math_lex2997", t) || lex_class_member(b, "20_math_lex2997_2", t) || lex_class_member(b, "20_math_lex2997_3", t) ||
                    lex_class_member(b, "20_math_lex2998", t)) continue;
                if (strlen(t) == 1 && isalpha((unsigned char)t[0])) {
                    snprintf(add, sizeof add, "%s", t);
                    break;
                }
            }
        }
        if (word[0]) {
            char res[160]; snprintf(res, sizeof res, "%s", word);
            size_t rl = strlen(res);
            if ((kb_cue_match(b, "20_math_chain3018", buf)) && rl > 0) {
                memmove(res, res + 1, rl);
                rl--;
            }
            if ((kb_cue_match(b, "20_math_chain3023", buf)) && rl > 0) {
                res[--rl] = '\0';
            }
            if (add[0] && (kb_cue_match(b, "20_math_chain3027", buf))) {
                snprintf(res + strlen(res), sizeof res - strlen(res), "%s", add);
            } else if (add[0] && (kb_cue_match(b, "20_math_chain3030", buf))) {
                char tmp2[160]; snprintf(tmp2, sizeof tmp2, "%s%s", add, res);
                snprintf(res, sizeof res, "%s", tmp2);
            }
            char msg[180]; snprintf(msg, sizeof msg, "%s.", res);
            put(msg, out, out_size);
            return 1;
        }
    }

    /* gen385: le superfici della richiesta erano un elenco in C. Ora sono fatti
     * (`spell_request_cue/1`): una formulazione nuova, in qualunque lingua, costa
     * una riga di .p0 — che e' il test operativo del mantra #2. */
    if (!kb_cue_match(b, "spell_request", buf)) return 0;
    /* gen240: don't misfire on an anagram/rearrange task that merely mentions
     * "spell" in an example ("rearrange Listen to spell Silent — now do X"). That
     * is not a spelling request; spelling a stray word there is nonsense. */
    if (kb_cue_match(b, "20_math_chain3049", buf))
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
        if (lex_class_member(b, "20_math_lex3049", t) || lex_class_member(b, "20_math_lex3049_2", t) || lex_class_member(b, "20_math_lex3049_3", t) ||
            lex_class_member(b, "20_math_lex3050", t) || lex_class_member(b, "20_math_lex3050_2", t) || lex_class_member(b, "20_math_lex3050_3", t) ||
            lex_class_member(b, "20_math_lex3051", t) || lex_class_member(b, "20_math_lex3051_2", t) || lex_class_member(b, "20_math_lex3051_3", t))
            continue;
        target = t; break;
    }
    if (!target) return 0;

    /* ── gen385: SCANDIRE UNA PAROLA ROTTA E' UNA MENZOGNA ──────────────────
     *
     * Misurato con la sonda all'oracolo (question-emergence.md §13):
     *
     *     how do you correctly spell recieve  ->  r-e-c-i-e-v-e
     *
     * La domanda chiedeva la grafia CORRETTA e la risposta ha risillabato
     * l'errore, con sicurezza. E' peggio del muro che le sta accanto in
     * italiano: un muro non afferma nulla, questo conferma uno sbaglio.
     *
     * Il rimedio non e' un correttore ortografico nel motore — sarebbe
     * vocabolario nel C, e per giunta monolingue. E' quello che l'oracolo fa, e
     * che parrot0 puo' fare MEGLIO perche' ha di che verificare: si generano i
     * VICINI della stringa per deformazione inversa, si tengono solo quelli che
     * sono parole note (`lexeme/1`), e il risultato si PROPONE. Dove l'LLM e'
     * fluente e a volte falso — il modello debole della sonda ha risposto
     * "pannino" per "pamino" — qui l'ipotesi e' controllata, non generata: se
     * "pannino" non e' un lessema, non puo' essere proposto.
     *
     * Le quattro mosse dell'oracolo, tutte e quattro presenti:
     *   ipotizza / enumera se i vicini sono piu' d'uno / non ripara cio' che non
     *   e' rotto / non inventa quando vicini non ce ne sono. */
    ensure_lexeme(b);
    int target_known = 0;
    {
        const char *lq[] = { target };
        target_known = b && b->kb && kb_query(b->kb, "lexeme", lq, 1);
    }

    char cand[8][64]; size_t ncand = 0;
    if (!target_known && b && b->kb)
        ncand = p0_repair_candidates(b, target, cand, 4);

    char line[512]; size_t pos = 0; line[0] = '\0';
    for (size_t i = 0; target[i] && pos + 3 < sizeof line; i++) {
        if (i) line[pos++] = '-';
        line[pos++] = target[i];
    }
    line[pos] = '\0';

    if (!target_known) {
        char msg[600];
        if (ncand == 1) {
            const KbResponseSlot s[] = { {"guess", cand[0]}, {"spelled", line} };
            if (kb_response_slots(b, "spell_repair_one", s, 2, msg, sizeof msg)) {
                put(msg, out, out_size); return 1;
            }
        } else if (ncand > 1) {
            char list[256]; size_t lp = 0; list[0] = '\0';
            for (size_t i = 0; i < ncand && lp + 2 < sizeof list; i++)
                lp += (size_t)snprintf(list + lp, sizeof list - lp, "%s%s",
                                       i ? ", " : "", cand[i]);
            const KbResponseSlot s[] = { {"guesses", list} };
            if (kb_response_slots(b, "spell_repair_many", s, 1, msg, sizeof msg)) {
                put(msg, out, out_size); return 1;
            }
        } else {
            /* Nessun vicino: si dichiara, non si inventa. E' la mossa che il
             * modello forte della sonda ha fatto su "zqxvbn". */
            const KbResponseSlot s[] = { {"word", target}, {"spelled", line} };
            if (kb_response_slots(b, "spell_unknown_word", s, 2, msg, sizeof msg)) {
                put(msg, out, out_size); return 1;
            }
        }
    }

    put(line, out, out_size);
    return 1;
}
