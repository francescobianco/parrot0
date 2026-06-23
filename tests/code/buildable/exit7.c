/* gen198: a runnable fixture whose exit status is NOT zero, so the run-grounding
 * gate proves parrot0 reports the program's REAL exit code (read from the
 * process), not a constant. */
int main(void) { return 7; }