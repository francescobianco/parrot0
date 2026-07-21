#ifndef PARROT0_ENV_H
#define PARROT0_ENV_H

/* gen346 — runtime configuration layer (docs/plans/test-engine.md §mock).
 *
 * Every environment variable that steers parrot0 at runtime is read through this
 * layer instead of getenv() directly, so the values are frozen into pilotable
 * runtime globals. The test-engine's `!set NAME=VALUE` drives them; PID and HOME
 * and the other system-derived reads are pilotable the same way (PARROT0_PID as
 * the override key for getpid()).
 *
 * A subset of names is MEMORY-AFFECTING: they are consumed when the brain boots
 * (which KB files load, the policy facts, the session language/PID). Changing one
 * means the loaded knowledge must be re-derived, so `!set` on such a name asks the
 * caller to reload the brain — but only when the value actually changed, so a test
 * that touches nothing memory-related pays no reload (smart / non-redundant).
 */

/* The effective value of `name`: the override if one was set (even ""), else the
 * process environment. Returns NULL only when neither exists. */
const char *p0env(const char *name);

/* Set an override for `name`. Returns 1 when the brain should be reloaded — i.e.
 * `name` is memory-affecting AND its effective value actually changed — else 0. */
int p0env_set(const char *name, const char *value);

/* 1 if `name` is consumed at brain boot (so a change requires a reload). */
int p0env_affects_memory(const char *name);

/* Drop every override, returning to the raw process environment. Returns 1 if any
 * dropped override was memory-affecting (so the caller reloads back to default). */
int p0env_clear(void);

/* Write a stable, strcmp-comparable signature of every memory-affecting value's
 * CURRENT effective state into `out`. The test-engine reloads the brain only when
 * this signature differs from the one it last loaded with — so `!set`s that don't
 * actually change any effective value (identical repeats, or a value equal to the
 * real current one) never trigger a reload. */
void p0env_mem_signature(char *out, size_t n);

#endif /* PARROT0_ENV_H */