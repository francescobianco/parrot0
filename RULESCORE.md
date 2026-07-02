# RULESCORE — can parrot0 turn RULES told in text into a working game?

Inventor/judge: **minimax-m2.5** (opencode-GO). The inventor makes up 5 terminal mini-games described ONLY as numbered rules; parrot0 must implement each as a C program; the harness compiles and sample-runs whatever code comes back; the judge scores each attempt **0** (nothing implemented) to **5** (every rule visibly encoded, compiles, runs) and must justify the number rule by rule.

> Purpose: training pressure for the F4 frontier — the bridge from free text to code (docs/plans/coding-agent-evolution.md §3, CODE-MASTERY §4). This report is a MAP of where text→rules→code breaks, not a vanity metric. Honesty contract: an honest decline scores 0 and is named as honest; FABRICATION (code presented as working that is not) is flagged and is worse than a wall. Scores may only climb by category-level capability (categories-not-prompts, gen254) — never by gaming the judge.

_Generated 2026-07-02 17:32:59._

## Total: 0 / 25

---
## Game 1 — score 0/5

### Spec (the rules parrot0 was given)

```
**Coin Rows**

1. **Setup** – The program generates a row of 5 hidden coin values. Each value is an integer chosen uniformly at random from 1 through 9 inclusive. The row is displayed as five numbers separated by spaces (e.g., `4 7 2 9 3`).  

2. **Turn order** – Player and program alternate, with the player moving first on every round.  

3. **Player move** – On the player’s turn they must type a single character: `L` (or lowercase `l`) to take the leftmost remaining coin, or `R` (or lowercase `r`) to take the rightmost remaining coin. The program then automatically takes the opposite end coin (the other remaining end of the row). Both taken coins are removed from the row.  

4. **Feedback** – After each pair of moves the program prints:  
   `You took <value>. Program took <value>.`  
   followed by the current row of remaining coins (still shown as numbers).  

5. **Termination** – The game ends when no coins remain in the row. The program then computes the sum of the values the player collected and the sum of the values the program collected.  

6. **Winner** –  
   * If the player’s total > program’s total → output `You win!`  
   * If the player’s total < program’s total → output `Program wins.`  
   * If the totals are equal → output `It's a tie.`
```

### parrot0's answer

```
Ha, you're playful! Ask me something?
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

honest decline - The agent did not provide any code implementation of the Coin Rows game. The answer was a non-sequitur ('Ha, you're playful! Ask me something?') that ignored the specification entirely. No C code was found in the answer, no compilation was attempted, and no game logic was provided.

### Judge — justification for the score

The agent completely declined to implement the game. There is no code, no attempt at handling the rules (random coin generation, turn alternation, move validation, opposite-end auto-take, feedback printing, sum calculation, win/tie determination), and no evidence of any effort. This is an honest decline with zero implementation.

---
## Game 2 — score 0/5

### Spec (the rules parrot0 was given)

```
**Risk Reward**

1. **Initial state** – The program sets the player's score to 10 points. The program secretly chooses a random integer target T in the range 1 to 100 inclusive. No target is shown yet.

2. **Betting phase** – Each round begins with the program displaying the current score and prompting the player to enter a bet B, where B is an integer with 1 ≤ B ≤ score. Any other input is rejected and the prompt repeats.

3. **Guessing phase** – The program then displays the current round number and prompts for a guess G, an integer in the range 1 to 100 inclusive. Any other input is rejected and the prompt repeats.

4. **Reveal and scoring** – The program prints `Target: T`.  
   - If G = T: the player wins 2×B points (added to score) and a new target is chosen for the next round.  
   - If G ≠ T: the player loses B points (subtracted from score) and a new target is chosen for the next round.

5. **Termination condition** – The game ends when the player's score reaches 0 or drops below 0. The program prints `Game Over` followed by `You survived N rounds.` where N is the number of completed rounds played.

6. **No separate win state** – The game is purely a survival challenge; the only outcome is the round count at the moment of bankruptcy.
```

### parrot0's answer

```
I'm a simple bot, but I'm here for it. Go on?
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

No C code provided in answer; agent responded with off-topic text ('I'm a simple bot, but I'm here for it. Go on?') rather than any implementation. Evidence confirms: 'code extracted: no', 'compile result: no C code found'. None of the 6 SPEC rules are encoded: no initial state (score=10, random target), no betting phase with validation, no guessing phase with validation, no reveal/scoring logic (2×B win / B loss), no termination check (score ≤ 0), no 'Game Over' or survival count output.

### Judge — justification for the score

Agent provided no code whatsoever and gave a non-substantive response, failing to implement any part of the specification. Evidence shows no code was extracted and compilation was not attempted. This is a complete failure to deliver a working game.

---
## Game 3 — score 0/5

### Spec (the rules parrot0 was given)

```
**Memory Hunt**

1. **Setup** – Each round uses a square grid of size N×N, where N starts at 3 and increases by 1 each round until reaching 6. The program secretly places H hidden targets in H distinct cells chosen uniformly at random, where H = min(3, N). The player is given A attempts equal to (N×N) minus 1.

2. **Display** – At the start of each round, the program shows the grid as N rows of N space-separated positions labeled `1` through N×N in row-major order. The program also shows `Score: X Hits: Y Z`, where X is the cumulative score, Y is hits this round, and Z is the number of remaining targets.

3. **Player probe** – The player enters a single integer P (1 to N×N) to probe a position. If that position was already probed, the program rejects the input and prompts again. Otherwise, the program reveals:
   - `Hit!` if P contains a target, then marks it found.
   - `Miss.` if P is empty.

4. **Round end** – A round ends when Y = H (all targets found) or the player has used A attempts. The program prints `Round N complete: found H targets.`

5. **Scoring** – The player's cumulative score is the total number of targets found across all rounds. After round 6, the game ends.

6. **Final output** – The program prints `Final Score: X` where X is the total targets found across all 4 rounds, followed by `Play again? (y/n)`. A `y` response restarts the game; any other input terminates the program.
```

### parrot0's answer

```
That looks like a snippet of code.
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

No code provided. Agent responded with 'That looks like a snippet of code.', which is not an implementation. All rules (1–6) are unaddressed.

### Judge — justification for the score

The answer contains no C source code, does not compile, and produces no output. The agent made no attempt to encode the game setup, display, probing, scoring, round termination, or final output. Consequently, every rule from the specification is missing.

---
## Game 4 — score 0/5

### Spec (the rules parrot0 was given)

```
**Target Fifteen**

1. **Setup** – The program fills a 3×3 grid with the numbers 1 through 9 in random order, each used exactly once. The program also displays the current column sums below each column and the current row sums after each row. The program secretly computes C = (sum of all 5 odd numbers) mod 10. The player starts with 0 points.

2. **Player move** – The player enters a single integer: the position number (1–9) they wish to modify. Positions are numbered row-major order (1–3 first row, 4–6 second row, 7–9 third row). If the chosen position's value is 0 (already modified), the input is rejected and the prompt repeats.

3. **Modification** – The program prompts `+/-?` and the player must type `+` or `-`. The chosen cell's value is either increased (`+`) or decreased (`-`) by 3. The value wraps: if a `+` would exceed 9, it wraps to 1; if a `-` would go below 1, it wraps to 9. Modified cells are displayed with the new value surrounded by parentheses. The player loses 1 point per move.

4. **End condition** – After each move, all row sums and column sums are recomputed and displayed. The game ends immediately when any one of the three row sums equals 15, or any one of the three column sums equals 15.

5. **Scoring** – The player's final score is 20 minus the number of moves taken. If the player takes more than 20 moves, the final score is 0.

6. **Output** – The program prints `Score: X` where X is the final score, followed by a blank line and the final grid.
```

### parrot0's answer

```
Ha, you're playful! Ask me something?
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

Agent did not provide any C code. Answer only says 'Ha, you're playful! Ask me something?' which is not a solution to the specification.

### Judge — justification for the score

The agent declined to implement the game and supplied no code, so the program cannot compile or run, meaning none of the rules are encoded.

---
## Game 5 — score 0/5

### Spec (the rules parrot0 was given)

```
**Survival Nim**

1. **Setup** – The program generates a random pile size S in the range 10–20 inclusive. Player always moves first. On each turn a player removes 1, 2, or 3 stones, never more than the current pile size.

2. **Player input** – The player enters a single integer M (1–3). If M exceeds the remaining stones, the program prints `Too many.` and prompts again.

3. **Program response** – After a valid player move, the program removes stones as follows:
   - If pile ≥ 5, program removes `pile % 4` stones (treating 0 as 4).
   - If pile < 5, program removes `pile - 1` stones.
   The program prints `Program takes N stone(s).` after each move.

4. **Display** – After both moves each round, the program prints `Stones remaining: S` where S is the updated pile size.

5. **Round win** – If the program takes the last stone, the player wins the round. The program prints `You win this round!` and begins a new round with pile size increased by 10.

6. **Game over** – If the player takes the last stone, the game ends. The program prints `You took the last stone. Game Over.` followed by `You survived N round(s).` where N is the number of completed rounds won.
```

### parrot0's answer

```
I'm a simple bot, but I'm here for it. Go on?
```

### Mechanical evidence

- compile: no C code found in the answer

### Judge — where it failed

No code provided. Agent response 'I'm a simple bot, but I'm here for it. Go on?' does not contain any C code implementation. No compilation attempted. All rules (1-6) unaddressed.

### Judge — justification for the score

The agent provided no implementation whatsoever of the Survival Nim game. There is no C code to compile or test. This is a complete failure to attempt the task, scoring 0.

