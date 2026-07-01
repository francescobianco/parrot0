# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-07-01 22:54:38._

## Score: 4 / 10

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | What is the capital of Australia and what ocean lies to its west? | Canberra. To its west lies the Indian Ocean. | Correct and fluent answer to factual question about Australia's capital and surrounding ocean. | 1 |
| 2 | If you drop a stone into a pond and it creates circular ripples expanding outward at constant speed, and you notice one ripple has a radius of 12 centimeters, what is the approximate circumference of that ripple? | I don't understand that yet. | Refusal to calculate simple circumference (2πr = ~75cm) using canned 'I don't understand' response. | 0 |
| 3 | Write a short haiku about the moon. | Pale lantern of night / drifting over silent fields / shadows softly bloom. | Appropriate creative haiku about the moon with correct 5-7-5 syllable structure. | 1 |
| 4 | If you have 3 apples, give away 1, then buy 5 more, then lose 2, how many apples do you end up with? | 5. | Arithmetic is correct: 3-1+5-2=5 apples. | 1 |
| 5 | A farmer has 20 sheep and 5 fences. Each fence can enclose a separate area, but the farmer wants to arrange them so that each enclosed area shares a fence with at least one other area. What's the maximum number of sheep that can be kept separate from all others, assuming each separate area must cont | 20. | Answer 20 ignores constraint of only 5 fences and incomplete question, failing to engage with the logic puzzle. | 0 |
| 6 | Continue this sentence in three different ways: "When the last light faded from the horizon, the traveler realized... | I don't understand that yet. | Refuses to complete creative writing prompt with 'I don't understand' response. | 0 |
| 7 | What's the opposite of "generous" and what word describes a person who always tells the truth? | I don't understand that yet. | Simple vocabulary questions answered with 'I don't understand' instead of providing 'stingy/selfish' and 'honest'. | 0 |
| 8 | What color do you get when you mix blue and yellow paint together? | Green. | Correct answer: mixing blue and yellow paint produces green. | 1 |
| 9 | I'm thinking of a number. It is greater than 10, less than 20, and not prime. What could my number be? | That sounds nice -- tell me more about it. | Dodges straightforward logic puzzle by not providing valid numbers (12,14,15,16,18) from the range. | 0 |
| 10 | If someone says "break a leg" before a performance, what's the usual intent behind those words? | I don't know what your a is called. | Nonsensical response fails to explain idiom meaning 'good luck'; does not address the question. | 0 |

