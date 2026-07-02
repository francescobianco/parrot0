# RULESCORE — can parrot0 turn RULES told in text into a working game?

Inventor/judge: **minimax-m2.5** (opencode-GO). The inventor makes up 5 terminal mini-games described ONLY as numbered rules; parrot0 must implement each as a C program; the harness compiles and sample-runs whatever code comes back; the judge scores each attempt **0** (nothing implemented) to **5** (every rule visibly encoded, compiles, runs) and must justify the number rule by rule.

> Purpose: training pressure for the F4 frontier — the bridge from free text to code (docs/plans/coding-agent-evolution.md §3, CODE-MASTERY §4). This report is a MAP of where text→rules→code breaks, not a vanity metric. Honesty contract: an honest decline scores 0 and is named as honest; FABRICATION (code presented as working that is not) is flagged and is worse than a wall. Scores may only climb by category-level capability (categories-not-prompts, gen254) — never by gaming the judge.

_Generated 2026-07-02 18:33:05._

## Total: 0 / 25

---
## Game 1 — score 0/5

### Spec (the rules parrot0 was given)

```
**Target Thirty-One**

1. The computer secretly chooses a target integer T between 10 and 99 (uniformly random).
2. The player has exactly 10 guesses to find T.
3. Each round, the player types a guess G:
   - If G equals T: the player wins immediately.
   - If G is less than T: the computer outputs "LOW by X" where X = T - G.
   - If G is greater than T: the computer outputs "HIGH by X" where X = G - T.
4. If the guess is wrong, the player's score decreases by 1 (score starts at 10, minimum 0).
5. The game ends when the player guesses correctly (win) or after 10 wrong guesses (lose).
6. The player's final score equals the number of guesses remaining when they win, or 0 if they lose.
```

### parrot0's answer

```
That is a rules spec — "Target Thirty-One", 6 numbered rules; I can recognize the input, state and end rules, but I cannot yet translate free-text rules into a program. My code synthesis covers verified schemas (a sort from a learned shape, arithmetic composition), not open rule specs — the text-to-code bridge is the gap.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

Agent declined to provide code; no implementation attempted.

### Judge — justification for the score

The agent explicitly stated inability to translate the spec into code and supplied no C code, so there is nothing to compile or run.

---
## Game 2 — score 0/5

### Spec (the rules parrot0 was given)

```
**Survive the Deck**

1. The game consists of 5 rounds. At the start of each round, the computer displays a hidden card value V (integer from 1 to 10, uniformly random).
2. The player's current score S starts at 0. After all 5 rounds, S is the sum of all card values the player chose to keep.
3. Each round, the player sees their remaining rounds R (starting at 5) and current score S. The player chooses one of three actions by typing:
   - **KEEP**: The hidden card value V is added to S, and the round ends.
   - **PEEK**: The hidden card value V is revealed, then the player must type a second choice of KEEP or PASS.
   - **PASS**: V is discarded; S and R are unchanged.
4. After all 5 rounds, the computer reveals the sum of all 5 hidden card values (the "deck total").
5. If the player's final S is greater than half of the deck total (rounded up), the player wins. Otherwise, the computer wins.
6. The game outputs: the player's final score, the deck total, and whether the player won or lost.
```

### parrot0's answer

```
That is a rules spec — "Survive the Deck", 6 numbered rules; I can recognize the input, state and end rules, but I cannot yet translate free-text rules into a program. My code synthesis covers verified schemas (a sort from a learned shape, arithmetic composition), not open rule specs — the text-to-code bridge is the gap.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

honest decline - the agent explicitly stated it cannot translate free-text rules into program code; no C code was produced; the answer recognized the game's structure but declined implementation due to missing text-to-code capability

### Judge — justification for the score

The agent provided no code whatsoever and explicitly declined the task, citing the inability to bridge free-text specifications to code synthesis. This is a transparent, honest refusal rather than a failed attempt or fabrication.

---
## Game 3 — score 0/5

### Spec (the rules parrot0 was given)

```
**Oddball Wager**

1. The computer secretly generates an integer N by rolling two dice (each die is 1–6, so N ranges from 2 to 12, each sum equally likely).
2. Before seeing N, the player types a number of tokens T to wager (0 to 99). This wager is locked in.
3. The computer reveals N.
4. The player then types a final bet B, choosing either "ODD" or "EVEN".
5. If N is odd and the player chose ODD, or N is even and the player chose EVEN: the player wins and gains T tokens (starting bank is 50).
6. Otherwise, the player loses their wager T from their bank. The game ends when the bank reaches 0 (lose) or reaches 100 (win).
```

### parrot0's answer

```
That is a rules spec — "Oddball Wager", 6 numbered rules; I can recognize the input and end rules, but I cannot yet translate free-text rules into a program. My code synthesis covers verified schemas (a sort from a learned shape, arithmetic composition), not open rule specs — the text-to-code bridge is the gap.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

honest decline

### Judge — justification for the score

The agent explicitly declined to implement the game, stating it cannot translate free-text rules into program code. No C code was provided in the answer. Consequently, none of the six rules (secret die roll, wager input, reveal, bet choice, win/loss logic, or bank termination) were encoded. The harness found no code to compile or run.

---
## Game 4 — score 0/5

### Spec (the rules parrot0 was given)

```
**Mind-Reader Duel**

1. At the start of each round, both player and computer secretly choose an integer from 1 to 20. The player types a number; the computer generates one uniformly at random.
2. Players alternate making comparative statements about their own number. The player goes first each round.
3. On the player's turn: the player types a guess G (1-20). The computer answers "LOW" if the player's number is less than G, "HIGH" if greater, or "EXACT" if correct.
4. On the computer's turn: the computer guesses a number C based on its turn count and all previous feedback. The computer always guesses the midpoint of its remaining possible range (low+high)/2, rounded down. The player answers "LOW", "HIGH", or "EXACT".
5. The round ends when either side gets "EXACT". If the player guesses first, they earn (7 minus player turn count) points. If the computer guesses first, the computer earns (7 minus computer turn count) points.
6. The match consists of 5 rounds. After 5 rounds, the player with more total points wins the match; if tied, the result is "DRAW".
```

### parrot0's answer

```
That is a rules spec — "Mind-Reader Duel", 6 numbered rules; I can recognize the input, state and end rules, but I cannot yet translate free-text rules into a program. My code synthesis covers verified schemas (a sort from a learned shape, arithmetic composition), not open rule specs — the text-to-code bridge is the gap.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

honest decline — the agent explicitly stated it cannot translate free-text rules into a program, acknowledging the text-to-code bridge as beyond its current capability. No C code was provided, no compilation was attempted, and no sample run was produced.

### Judge — justification for the score

The agent provided no code and honestly declined the task, citing that its code synthesis covers only verified schemas rather than open rule specs. The evidence confirms no code extraction was possible. This is the correct behavior when a capability is missing.

---
## Game 5 — score 0/5

### Spec (the rules parrot0 was given)

```
**Sum Wager**

1. The computer generates three secret integers A, B, and C, each uniformly random from 1 to 5.
2. Before seeing the numbers, the player types a wager W (integer 0 to current tokens). The player starts with 50 tokens.
3. After entering the wager, the player types a guess G for the sum S = A + B + C.
4. The computer reveals A, B, C, and S, then computes the absolute difference D = |G - S|:
   - If D = 0: the player wins 5 tokens plus their wager W.
   - If D = 1: the player wins 2 tokens.
   - If D = 2: the player wins 1 token.
   - If D ≥ 3: the player loses their wager W.
5. The player's token count updates accordingly. The round ends and a new round begins with fresh random values.
6. The game ends when the player reaches 100 tokens (win) or 0 tokens (lose).
```

### parrot0's answer

```
That is a rules spec — "Sum Wager", 6 numbered rules; I can recognize the input and end rules, but I cannot yet translate free-text rules into a program. My code synthesis covers verified schemas (a sort from a learned shape, arithmetic composition), not open rule specs — the text-to-code bridge is the gap.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

honest decline: Agent explicitly stated inability to translate free-text rules into program code, citing lack of text-to-code bridge capability. No C code was provided; no implementation attempted.

### Judge — justification for the score

The agent honestly declined to implement the game, correctly identifying that its code synthesis capabilities cover verified schemas but not open rule specifications. All six rules (random generation, wager input, guess input, reward computation, token updates, and win/lose conditions) remain unimplemented. The evidence confirms no code extraction or compilation was possible.

