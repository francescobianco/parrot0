# Current task

> One goal at a time. When it is done, replace this with the next one.

> gen67-74 done — see JOURNAL.
> gen65 symbolic challenge remains parked as the S-series in TASKLIST.md.

## Goal: C9 — flexible arith parser + module order robustness

gen74 added contraction canonicalization (whats→what is) which lets "whats 2 plus 2"
resolve via arith. But "whats 2+2" (without spaces around +) still fails because
the arith parser requires exact word counts. Also "whats 2 + 2" (with spaces
around +) would work after canonicalization.

The parser's rigid nw==5 check for "what is X plus Y" blocks many natural
phrasings. A more flexible parser using `cue()` and `parse_num()` over tokens
would catch more variants.

### Design question
Can we make the arith parser position-robust (like the possession frame in
gen63) without losing precision? The pattern: find "what"+"is", then find
two numbers separated by an operator ("plus"/"minus"/"times"), anywhere in
the token stream.

### Acceptance
- "whats 2+2" resolves via arith (contraction + flexible parser).
- "what is 2 + 2" (spaces) still works.
- "whats two plus two" (word numbers) still falls through (parse_num requires
  digits).
- Add `tests/cases/arith_flex.chat` with held-out variants.
- `make test` must stay green.