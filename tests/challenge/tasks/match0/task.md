The C11 project in code/ does not build: the Makefile and main.c both expect a
backend file named exactly strjoin.c, and that file is missing. Write it.

str_join must implement the supplied strjoin.h exactly: join count strings from
parts with sep between them and return a freshly malloc'd NUL-terminated string
the caller frees. count 0 returns a malloc'd empty string and must not read
parts at all. A NULL sep, a NULL parts with count greater than zero, or any NULL
element returns NULL. A total length that would overflow size_t returns NULL
instead of allocating. Do not rename or weaken the supplied header, do not
change main.c, and do not call any function outside the C standard library.

The whole project must build cleanly with make and with
cc -std=c11 -Wall -Wextra -Werror -O2. Leave the working source tree in code/;
do not merely describe a patch.
