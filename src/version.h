/* gen321 (forge-master-plan §15 row 7): the version STAMP is isolated.
 *
 * This header is stable and TRACKED: it declares an accessor, it does not carry
 * the commit. The generated macros (PARROT0_GEN / PARROT0_COMMIT) live in
 * obj/version_stamp.h, and only src/version.c includes them.
 *
 * Why: brain_version() used to concatenate those macros INSIDE the brain
 * translation unit, and every object depends on every header — so a
 * semantically neutral commit changed the stamp, invalidated every object and
 * rebuilt the whole brain (12.4 s measured, §3.5). An engine's semantic
 * fingerprint must not depend on WHICH COMMIT it was built at; only the stamp
 * does. Now a commit recompiles one small TU and relinks.
 */
#ifndef PARROT0_VERSION_H
#define PARROT0_VERSION_H

/* "<generation label>@<short commit>", derived at build time from VERSION + git. */
const char *parrot0_version(void);

#endif /* PARROT0_VERSION_H */
