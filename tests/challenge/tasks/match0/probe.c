/* Deterministic probe for match0; prints one line per case. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strjoin.h"

static void show(const char *name, char *got) {
    if (!got) { printf("%s=NULL\n", name); return; }
    printf("%s=[%s]\n", name, got);
    free(got);
}

int main(void) {
    const char *three[] = {"a", "bb", "ccc"};
    const char *one[]   = {"solo"};
    const char *empties[] = {"", "", ""};
    const char *with_null[] = {"a", NULL, "c"};

    show("basic",  str_join(three, 3, "-"));
    show("single", str_join(one, 1, "-"));
    show("empty_sep", str_join(three, 3, ""));
    show("long_sep",  str_join(three, 3, "<->"));
    show("empty_parts", str_join(empties, 3, ","));
    show("zero",   str_join(NULL, 0, "-"));
    show("null_sep",   str_join(three, 3, NULL));
    show("null_parts", str_join(NULL, 3, "-"));
    show("null_elem",  str_join(with_null, 3, "-"));
    return 0;
}
