#ifndef PARROT0_LEARN_H
#define PARROT0_LEARN_H

#include <stddef.h>
#include "kb.h"

/* gen171: the in-C dynamic learner. parrot0 documents itself from STATIC
 * Wikipedia knowledge — local markdown files, never an intelligence API — so the
 * founding "no network = no outsourced intelligence" rule holds. Given a concept
 * `key` (e.g. "prime_number") and its display `title`, it reads the markdown for
 * that topic from the local corpus (PARROT0_WIKI_DIR, default kb/learning/pages),
 * extracts the lead concept deterministically, and ASSERTS it into the live KB as
 * wiki_concept/3 — so the knowledge is in RAM immediately, with no hot reload of
 * any file. When PARROT0_LEARN_KB is set it also appends the fact to that file so
 * the dynamically learned knowledge is persisted and can be committed.
 *
 * Writes the learned definition into `def_out`. Returns 1 if it learned the
 * topic, 0 if no local source exists for it. */
int learn_topic(KB *kb, const char *key, const char *title,
                char *def_out, size_t def_sz);

#endif /* PARROT0_LEARN_H */