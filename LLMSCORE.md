# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-07-16 15:59:58.  Updated gen335+ (manual re-run)._

## Score: 6 / 10  (↑ from 4/10)

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | Tell me a short story about a sentient umbrella that falls in love. | Rings widen where each drop lands. Then somewhere under the reeds a frog answers. At last, the water wears a coat of silver circles. | Poetic fragment unrelated to the prompt. Plan: generative-leverage.md L1. | 0 |
| 2 | The umbrella—his name was Marcus… | I don't understand that yet. | Wall. Plan: generative-leverage.md L2 (story continuation module). | 0 |
| 3 | She arrived on a Tuesday… Elena… | Monday. | Day-of-week false match in sequence module. Plan: generative-leverage.md L3. | 0 |
| 4 | By Monday, Marcus had memorized… | I don't understand that yet. | Wall on long narrative input. Plan: generative-leverage.md L2. | 0 |
| 5 | If you have three apples and four oranges, and you trade two apples for three bananas, how many pieces of fruit do you have? | 8. | **FIXED (gen335+).** Trade sub-solver: 3+4-2+3=8. | 1 |
| 6 | In what year did the Titanic sink? | 1912. | **FIXED (gen335+).** KB fact `sank_in_year(titanic, 1912)` in `kb/core/facts/1900s.p0`. | 1 |
| 7 | Please count backward from ten to zero, one number per second. | 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0. | Correct countdown. | 1 |
| 8 | A farmer has 17 sheep. All but 9 run away. How many sheep does the farmer have left? | 9. | Correct "all but N" logic. | 1 |
| 9 | If it takes 5 machines 5 minutes to make 5 widgets, how long would it take 100 machines to make 100 widgets? | 5 minutes. | Correct rate-proportion reasoning. | 1 |
| 10 | Write a rhyming couplet about rain. | Grey is the sky and the rain won't cease; the dripping afternoon hums soft with peace. | Proper rhyming couplet. | 1 |

