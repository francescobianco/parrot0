#include <string.h>
void str_reverse(char *s);

int main(void) {
    char buf[] = "abc";
    str_reverse(buf);
    return strcmp(buf, "cba") == 0 ? 0 : 1;   /* expects "cba" */
}
