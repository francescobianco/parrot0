/* small string utilities */

/* return the length of s, not counting the terminator */
int str_len(const char *s) {
    int n = 0;
    while (s[n] != '\0') n++;
    return n;
}

/* reverse s in place */
void str_reverse(char *s) {
    int n = str_len(s);
    for (int i = 0; i < n / 2; i++) {
        char t = s[i];
        s[i] = s[n - i];
        s[n - i] = t;
    }
}
