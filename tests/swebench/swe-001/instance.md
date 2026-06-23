#id: swe-001
#title: str_reverse off-by-one corrupts the string
#repo: tests/swebench/swe-001/repo
#problem: Reversing a string corrupts it. str_reverse("abc") does not produce "cba"; the swap in the reversal loop seems to use the wrong index.
#gold_file: strutil.c
#gold_function: str_reverse
