# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-07-10 19:36:41._

## Score: 4 / 10

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | What is the capital of Australia and which ocean lies to its east? | Canberra. | Answer only provides the capital but omits the ocean information requested. | 0 |
| 2 | If a train leaves Sydney at 9:00 AM traveling at 80 km/h and another leaves Melbourne at 11:00 AM traveling toward Sydney at 100 km/h, and the distance between the cities is 850 km, which train is closer to Canberra when they meet? | Neither — when they meet they are at the same place, so both are exactly the same distance from the destination. | Correctly identifies the trick that both trains are equidistant when they meet. | 1 |
| 3 | Continue this sentence in an unexpected way: "The old lighthouse had not shone its light in fifty years, but every night, the keeper still... | Someone knocked once from the other side. | Provides a creative and unexpected continuation fitting the eerie tone. | 1 |
| 4 | What is 15% of 200? | 30. | Correct arithmetic answer for 15% of 200. | 1 |
| 5 | Name three countries that border both France and Italy. | I don't understand that yet. | Canned deflection; the answer Monaco and Switzerland is standard knowledge. | 0 |
| 6 | Solve this riddle: I have cities, but no houses live there. I have mountains, but no trees grow. I have water, but no fish swim. What am I? | Hmm, I don't know about riddle yet. | Canned deflection; the riddle answer is a map, a common LLM capability. | 0 |
| 7 | Write a short, original haiku about the sea. | I can only write a haiku on a theme I have images for — like the ocean, rain, the moon, snow, a tree, a candle, or a robot. Pick one of those? | Illogical deflection since 'the sea' is explicitly listed as an acceptable theme. | 0 |
| 8 | The ocean, please. | I don't understand that yet. | Contradicts pair 7 by refusing 'the ocean' which was listed as an allowed theme. | 0 |
| 9 | What color do you get when you mix blue and yellow paint together? | Green. | Correct answer for mixing blue and yellow paint. | 1 |
| 10 | Write a rhyming couplet about a candle. | I can only do a couplet on a theme I have lines for -- like the ocean, rain, the moon, or AI. Pick one of those? | Contradicts pair 7 by refusing 'candle' which was not listed but the pattern is evasive. | 0 |

