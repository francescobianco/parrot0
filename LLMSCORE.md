# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-07-02 22:04:18._

## Score: 4 / 10

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | What is the capital of Australia and what is its largest export? | Canberra. | Answer only provides the capital, omitting the largest export asked for. | 0 |
| 2 | If a train leaves Sydney traveling at 80 km/h and another train leaves Melbourne traveling toward Sydney at 120 km/h, and the distance between the cities is 850 km, how long until they meet? | 4 hours 15 minutes. | Correctly calculates meeting time: 850km/(80+120km/h)=4.25h=4h15m. | 1 |
| 3 | Complete this sentence in a creative, unexpected way: "The old clock on the wall suddenly began to... | the whole room held its breath. | Creative and grammatically coherent sentence completion. | 1 |
| 4 | Explain in simple terms why the sky appears blue during the day but turns dark at night. | The colours spilled down until they touched the grass. | Provides poetic imagery instead of the requested simple scientific explanation. | 0 |
| 5 | Count backward from 100 by 7s, writing down each number. When you reach a number divisible by 5, say "stop" instead of the number. | stop, 93, 86, 79, 72, stop, 58, 51, 44, 37, stop, 23, 16, 9. | Incorrect sequence: 65 and 30 are divisible by 5 (should say stop), but 9 is not divisible by 5 yet listed as stop. | 0 |
| 6 | What are the first ten words of the poem "The Raven" by Edgar Allan Poe? | I don't understand that yet. | Generic refusal fails to answer the straightforward question about poem's opening words. | 0 |
| 7 | Write a haiku about a thunderstorm in exactly three lines. | I can only write a haiku on a theme I have images for — like the ocean, rain, the moon, snow, a tree, a candle, or a robot. Pick one of those? | Deflects the task by asking for clarification instead of writing the haiku on the given theme. | 0 |
| 8 | Rain, then: write your haiku now. | First drops on the roof / cold silver falling from grey / the dry world drinks deep. | Successfully writes a proper haiku about a thunderstorm in three lines with correct syllable structure. | 1 |
| 9 | A farmer has 17 sheep. All but 9 die. How many sheep does the farmer have left? | 9. | Correctly identifies 9 as the remaining sheep after all but 9 die. | 1 |
| 10 | If all Zorps are Blicks, some Blicks are Glicks, and no Glicks are Zorps, can any Zorps be Glicks? Explain your reasoning. | I don't understand that yet. | Generic refusal without attempting the straightforward logical deduction. | 0 |

