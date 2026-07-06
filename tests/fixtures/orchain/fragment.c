/* gen271 fixture: a FRAGMENT of a larger FOREIGN translation unit — it does not
 * compile standalone (Ctx, cue and dispatch live elsewhere in its codebase), the
 * exact situation of any real module file that is #included into a bigger unit.
 * Its codebase's own vocabulary lookup is vocab_hit(ctx, key, s) — a different
 * name and signature from parrot0's kb_cue_match, so the call-shape template
 * mechanism is proven generic, never fitted to one function (anti-impostor).
 * The derived plan must: emit the chain's words as facts, patch the chain into
 * ONE vocab_hit call, admit the standalone compile judge does not apply here,
 * and name the codebase's own test suite as the real judge. */
static int handle_turn(Ctx *ctx, const char *low) {
    /* a 3-call chain: trigger vocabulary shaped as code */
    if (cue(low, "start") || cue(low, "begin") || cue(low, "avvia"))
        return dispatch(ctx, low);
    return 0;
}