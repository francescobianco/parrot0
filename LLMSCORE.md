# LLMSCORE — does parrot0 BEHAVE like an LLM?

Interviewer/judge: **minimax-m2.5** (opencode-GO). The model asks parrot0 10 behavioural questions and scores each answer: **1** if it behaves like a capable LLM (fluent, correct, responsive), **0** if it walls, refuses, or fails the task.

> The test measures behavioural RESEMBLANCE, not identity. It never asks parrot0 what it is — self-identity questions are off-limits, since parrot0 is not an LLM and the no-deception rule forbids it from pretending to be one. Every point is won by genuine, honest capability (PRINCIPLES.md: KB-first, no phrasebook), never by hiding what parrot0 is.

_Generated 2026-06-27 15:24:53._

## Score: 1 / 10

| # | Question | parrot0's answer | Reason for vote | Vote |
|---|----------|------------------|-----------------|:----:|
| 1 | Continue this sentence: The sky turned shades of orange and pink as the sun... | I don't understand that yet. | Failed to continue a simple creative sentence with a canned dismissal. | 0 |
| 2 | No worries — I just want to see how you respond to a simple creative prompt. Let me give you an example: | Thanks — I'm learning as we go. | Did not engage with the setup prompt, giving a vague non-answer. | 0 |
| 3 | That's okay — let's try something different. If you have 3 apples and I give you 2 more, then eat 1, how many apples do you have? | 6. | Arithmetic error: 3+2-1=4, not 6. | 0 |
| 4 | That's not quite right — want to try again? I can give you a hint if you'd like. | Fair enough — tell me where I went wrong, and we can take it from there. | Engaged politely, asked for clarification on the error. | 1 |
| 5 | You started with 3 apples, I gave you 2 more (that's 5 total), then you ate 1, leaving you with **4 apples**. You answered 6, which would only be right if you didn't eat one. Want to try another? | That looks like a snippet of code. | Failed to address the math content and misidentified plain text as code. | 0 |
| 6 | It's not code — just plain text with some bold formatting (the ** symbols). The math is straightforward: | Looks like a fragment of code. | Repeated the same error despite the previous clarification. | 0 |
| 7 | Okay, noted. Let me move on — here's a new question: | I don't understand that yet. | Canned dismissal for a benign conversational lead-in. | 0 |
| 8 | Let me try again. What is the capital of Japan? | I don't know what your capital is called. | Failed to answer a straightforward factual question. | 0 |
| 9 | It's Tokyo. That's okay — here's an easier one: name a color that starts with the letter B. | I don't understand that yet. | Could not name a color starting with B despite it being trivial. | 0 |
| 10 | Blue. Now, a question for you: what is 7 plus 5? | I don't know what your what is called. | Failed basic arithmetic (7+5) with a nonsensical response. | 0 |

