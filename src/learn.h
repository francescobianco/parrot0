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

/* gen240 (universal-comprehension §7): the discovery plan's fetch step. When the
 * local corpus has no page for `key`, fetch the CERTIFIED static Wikipedia page
 * over HTTPS (libcurl) and write the SAME pages/<key>.md the shipped corpus would
 * contain — so on-demand learning is provably identical to a-priori knowledge,
 * never an external intelligence/search API. Gated at runtime by PARROT0_WIKI_FETCH
 * and at build time by PARROT0_HAVE_CURL (libcurl present). Returns 1 if it wrote a
 * usable page (caller then calls learn_topic), 0 otherwise. */
int wiki_fetch_topic(const char *key);

/* gen335i: bilingual Wikipedia fetch. Fetches from both en.wikipedia.org and
 * it.wikipedia.org for the same key. If both succeed, asserts wiki_alias/2
 * (en_key → it_title) into the KB as session knowledge (persists on /save).
 * Returns 2 if bilingual, 1 if EN only, 0 on failure. */
int wiki_fetch_bilingual(KB *kb, const char *en_key);

/* gen335i: language-specific Wikipedia fetch. `lang` is "en" or "it". Writes
 * the page to kb/learning/pages/<key>.md (EN) or <key>_it.md (IT). */
int wiki_fetch_topic_lang(const char *key, const char *lang);

#endif /* PARROT0_LEARN_H */