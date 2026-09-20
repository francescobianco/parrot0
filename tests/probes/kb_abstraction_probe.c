/* Diagnostic probe, not a comprehension score or a regression golden.
 * One Brain with the full agi KB. Invented clauses test representation only;
 * they are never saved. Run with kb_abstraction_probe.sh from this directory.
 */
#include "brain.h"
#include "kb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clause(KB *kb, const char *s) {
    if (!kb_load_clause(kb, s)) {
        fprintf(stderr, "Cannot load probe clause: %s\n", s);
        exit(2);
    }
}

static void support(KB *kb, const char *label, const char *pred,
                    const char *const *args, size_t argc) {
    char rows[64][KB_TERM_LEN];
    size_t n = 0;
    int query = kb_query(kb, pred, args, argc);
    int proof = kb_prove_support(kb, pred, args, argc, rows, 64, &n);
    printf("%s: query=%d proof=%d support=%zu\n", label, query, proof, n);
    for (size_t i = 0; i < n; i++) printf("  %s\n", rows[i]);
}

int main(void) {
    Brain *b = brain_create();
    if (!b) return 2;
    brain_boot(b);
    setvbuf(stdout, NULL, _IOLBF, 0);
    KB *kb = brain_kb(b);
    printf("Full profile: facts=%zu rules=%zu\n", kb_size(kb), kb_rule_count(kb));
    const char *turns[] = {
        "io is a glorp. every glorp is a dax. is io a dax?",
        "rex is a dog. all cats are animals. is rex an animal?",
        "socrates is a man. all men are mortal. is socrates mortal?",
        "rex is a dog. all dogs are animals. is rex an animal?"
    };
    for (size_t i = 0; i < sizeof turns / sizeof *turns; i++) {
        char answer[4096];
        brain_respond(b, turns[i], answer, sizeof answer);
        printf("NL: %s\n  %s\n", turns[i], answer);
    }

    const char *alternative =
        "zelvo is a narp. every narp is a vorn. is zelvo a vorn?";
    char answer[4096];
    brain_respond(b, alternative, answer, sizeof answer);
    printf("NL ALTERNATIVE, before teaching: %s\n  %s\n", alternative, answer);
    brain_respond(b, "zelvo is a vorn", answer, sizeof answer);
    printf("NL LESSON: zelvo is a vorn\n  %s\n", answer);
    brain_respond(b, alternative, answer, sizeof answer);
    printf("NL ALTERNATIVE, after teaching: %s\n  %s\n", alternative, answer);
    const char *zelvo[] = {"zelvo"};
    kb_retract(kb, "vorn", zelvo, 1);
    brain_respond(b, alternative, answer, sizeof answer);
    printf("NL ALTERNATIVE, direct fact retracted: %s\n  %s\n", alternative, answer);

    kb_set_origin(kb, KB_SESSION);
    const char *x[] = {"x"};
    /* A direct proof is encountered before a second, independent proof. */
    clause(kb, "abstraction_target(x).");
    clause(kb, "abstraction_seed(x).");
    clause(kb, "abstraction_target($X) :- abstraction_seed($X).");
    support(kb, "ALTERNATIVES, both present", "abstraction_target", x, 1);
    kb_retract(kb, "abstraction_target", x, 1);
    support(kb, "ALTERNATIVES, direct retracted", "abstraction_target", x, 1);

    /* Same predicate names, different argument bindings and meaning. */
    clause(kb, "abstraction_edge(a, b).");
    clause(kb, "abstraction_path($X, $Y) :- abstraction_edge($X, $Y).");
    kb_journal_start(kb);
    clause(kb, "abstraction_path($X, $Y) :- abstraction_edge($Y, $X).");
    char (*journal)[KB_TERM_LEN] = NULL;
    size_t nj = kb_journal_stop(kb, &journal);
    for (size_t i = 0; i < nj; i++) printf("INVERSE RULE JOURNAL: %s\n", journal[i]);
    free(journal);
    const char *ab[] = {"a", "b"}, *ba[] = {"b", "a"};
    support(kb, "FORWARD", "abstraction_path", ab, 2);
    support(kb, "INVERSE", "abstraction_path", ba, 2);
    /* M1 (20 settembre 2026, corretta dalla revisione): le stesse due regole
     * lette come CONTENUTI INTERI — due identita', due premesse distinte; il
     * giornale qui sopra le collassa ancora. */
    {
        char ids[8][KB_TERM_LEN], prem[8][KB_TERM_LEN];
        const char *qi[] = { NULL,
            "app(abstraction_path, cons(var(0), cons(var(1), nil)))", "0", "1" };
        printf("M1 kb_clause contents for abstraction_path: %zu\n",
               kb_match(kb, "kb_clause", qi, 4, ids, 8));
        const char *qp[] = { "$Id",
            "app(abstraction_path, cons(var(0), cons(var(1), nil)))", "1", NULL };
        size_t np = kb_match(kb, "kb_clause", qp, 4, prem, 8);
        for (size_t i = 0; i < np; i++) printf("  premise 1: %s\n", prem[i]);
    }

    /* The explanatory classifier must not erase a logical dependency. */
    support(kb, "BEFORE machinery", "abstraction_seed", x, 1);
    clause(kb, "machinery(abstraction_seed).");
    support(kb, "AFTER machinery", "abstraction_seed", x, 1);
    const char *seed[] = {"abstraction_seed"};
    kb_retract(kb, "machinery", seed, 1);
    clause(kb, "abstraction_mechanism(x).");
    clause(kb, "machinery(abstraction_mechanism).");
    support(kb, "machinery DECLARED BEFORE QUERY", "abstraction_mechanism", x, 1);

    /* Existing contexts preserve a proposition but do not execute its content. */
    clause(kb, "context(abstraction_context, hypothesis).");
    clause(kb, "holds_in(abstraction_context, abstraction_local(x)).");
    const char *cp[] = {"abstraction_context", "abstraction_local(x)"};
    printf("CONTEXT: holds=%d visible=%d unqualified=%d\n",
           kb_query(kb, "holds_in", cp, 2),
           kb_query(kb, "context_visible_belief", cp, 2),
           kb_query(kb, "abstraction_local", x, 1));

    /* Repeat one content under a second origin: check whether both assertion
     * occurrences survive and whether the proof can tell them apart. */
    clause(kb, "abstraction_shared(x).");
    kb_set_origin(kb, KB_HYPOTHETICAL);
    clause(kb, "abstraction_shared(x).");
    support(kb, "REPEATED IN ANOTHER ORIGIN", "abstraction_shared", x, 1);
    printf("REPEATED IN ANOTHER ORIGIN: session=%d hypothetical=%d\n",
           kb_query_origin(kb, KB_SESSION, "abstraction_shared", x, 1),
           kb_query_origin(kb, KB_HYPOTHETICAL, "abstraction_shared", x, 1));
    kb_retract_origin(kb, KB_HYPOTHETICAL);
    kb_set_origin(kb, KB_SESSION);
    support(kb, "HYPOTHETICAL RETRACTED", "abstraction_shared", x, 1);

    brain_destroy(b);
    return 0;
}
