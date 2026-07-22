# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-07-23 01:39:07._

## Score: 4 / 10

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | Write a two-line rhyming poem about a robot discovering rain for the first time. | A robot meets the rain for the very first time; each cold bright drop a small uncharted rhyme. | Answer is a single line with semicolon, not two separate lines as requested. | 0 |
| 2 | If a rectangle has a perimeter of 24 centimeters and its length is twice its width, what are the dimensions of the rectangle? | 48. | Answer only provides a single number '48', which does not give dimensions and is incorrect. | 0 |
| 3 | Explain in simple terms why the sky appears blue during the day. | The sky looks blue because sunlight is scattered by the tiny gas molecules in the air, and blue light scatters much more than the other colors. | Answer correctly explains Rayleigh scattering in simple terms. | 1 |
| 4 | If all roses are flowers and some flowers fade quickly, does it follow that some roses fade quickly? Why or why not? | No -- that doesn't follow. From "all roses are flowers" and "some flowers ...", nothing follows about roses: the flowers in question need not be roses (the middle term is undistributed). | Answer correctly identifies that the conclusion does not follow and explains the undistributed middle. | 1 |
| 5 | What is the capital of Australia, and why was Canberra created as a compromise city rather than using Sydney or Melbourne? | Canberra. | Answer only states the capital but omits the explanation for why Canberra was chosen. | 0 |
| 6 | Continue this sentence in three different ways: "When the clock strikes midnight, the old house... | 1) ...every small sound in the house became enormous. 2) ...the stopped clock seemed to be waiting for an answer. 3) ...the whole room held its breath. | Answer provides three distinct continuations as requested. | 1 |
| 7 | If a store sells 3 apples for $2, how much would you pay for 15 apples, and how much change would you get from a $20 bill? | $10. | Answer only gives the price for 15 apples ($10) but fails to state the change from $20. | 0 |
| 8 | What year did World War II end, and which two cities received the first atomic bombs? | World War II ended in 1945; Japan's surrender was signed on Sept 2, 1945, aboard the USS Missouri in Tokyo Bay. | Answer gives the end year but does not mention the two cities bombed (Hiroshima and Nagasaki). | 0 |
| 9 | What does "The quick brown fox jumps over the lazy dog" contain in every English sentence? | It contains every letter of the alphabet -- it's a pangram. | Answer correctly identifies the phrase as a pangram containing every letter of the alphabet. | 1 |
| 10 | How many times does the digit 7 appear between 1 and 100? | 108. | Answer '108' is incorrect; the digit 7 appears 20 times between 1 and 100. | 0 |

