#!/usr/bin/env python3
"""Generate varied, slot-sized parrotbench .p0t corpora.

The corpus is deliberately built from different faculties and discourse moves,
not from one arithmetic template with changing constants.
"""

from pathlib import Path
import shutil


ROOT = Path(__file__).resolve().parent
CORPUS = ROOT
SLOT_COUNT = 32


# Each category has four genuinely different seeds. Surface forms below are
# not numeric substitutions: they exercise direct questions, explanations,
# constraints, paraphrase, uncertainty, Italian/English switching, and
# follow-up-like framing around the same faculty.
CATALOG = {
    "knowledge/facts": [
        ("What is the capital of France?", "Paris"),
        ("Which ocean is the largest?", "Pacific"),
        ("Who wrote Hamlet?", "Shakespeare"),
        ("What is water made of chemically?", "H2O"),
    ],
    "knowledge/unknowns": [
        ("Who is the president of Mars?", "don't know"),
        ("Did Zorblax invent the moon?", "don't know"),
        ("What evidence supports an unknown claim?", "don't know"),
        ("Tell me a fact when you have no source.", "don't know"),
    ],
    "knowledge/gaps": [
        ("What information is missing before answering a causal question?", "missing"),
        ("How should a knowledge gap become a research plan?", "research"),
        ("What should you do when the KB lacks a fact?", "learn"),
        ("What would you search for before comparing two cities?", "know"),
    ],
    "knowledge/sources": [
        ("Why should a claim have a source?", "source"),
        ("How do you distinguish evidence from a guess?", "evidence"),
        ("What makes a conclusion cautious?", "uncertain"),
        ("Should you invent an answer without evidence?", "no"),
    ],
    "math/addition": [
        ("What is 17 plus 6?", "23"),
        ("Calculate the sum of 8 and 15.", "23"),
        ("Add twelve to nine.", "21"),
        ("A box has 7 items and receives 5 more. How many?", "12"),
    ],
    "math/multiplication": [
        ("What is 9 times 8?", "72"),
        ("Calculate three groups of seven.", "21"),
        ("What do five pairs contain?", "10"),
        ("A row has 6 seats and there are 4 rows. How many seats?", "24"),
    ],
    "math/ratios": [
        ("What is 19 percent of 200?", "38"),
        ("What is one quarter of 24?", "6"),
        ("If 3 of 12 items are red, what fraction is red?", "quarter"),
        ("A price of 80 falls by 25 percent. What is the new price?", "60"),
    ],
    "math/averages": [
        ("What is the average of 2, 4, and 6?", "4"),
        ("Find the mean of 10 and 20.", "15"),
        ("What is the midpoint between 4 and 12?", "8"),
        ("The scores are 3, 3, and 9. What is their average?", "5"),
    ],
    "math/comparison": [
        ("Which is greater, 41 or 14?", "41"),
        ("Is 3.41 larger than 3.14?", "yes"),
        ("Put 5, 1, 9, and 3 in ascending order.", "1"),
        ("Which is smaller, minus 2 or minus 5?", "5"),
    ],
    "math/word-problems": [
        ("I have 24 apples and give away a quarter. How many remain?", "18"),
        ("A train leaves at 3 and travels for 2 hours. When does it arrive?", "5"),
        ("I pay 10 euros from a 25 euro wallet. What remains?", "15"),
        ("A task takes 4 minutes and repeats 3 times. How long?", "12"),
    ],
    "logic/entailment": [
        ("All poets are writers. Ada is a poet. Is Ada a writer?", "yes"),
        ("All roses are flowers. This is a rose. Is it a flower?", "yes"),
        ("Every cat is an animal. Milo is a cat. What is Milo?", "animal"),
        ("If A precedes B and B precedes C, who comes first?", "A"),
    ],
    "logic/negation": [
        ("No cats are dogs. Milo is a cat. Can Milo be a dog?", "no"),
        ("Some birds cannot fly. Can you say every bird flies?", "no"),
        ("If nobody is both, does one counterexample prove the rule?", "no"),
        ("Is not knowing a fact the same as proving it false?", "no"),
    ],
    "logic/causality": [
        ("If it rains, the ground gets wet. It is raining. What follows?", "wet"),
        ("The ground is wet. Did it necessarily rain?", "not"),
        ("What is the difference between cause and correlation?", "cause"),
        ("What connects an initial condition to an observable result?", "mechanism"),
    ],
    "logic/counterfactual": [
        ("If the switch had stayed off, would the lamp have lit?", "no"),
        ("What changes in a counterfactual argument?", "condition"),
        ("If all else stays fixed, what does a counterfactual isolate?", "cause"),
        ("Can an outcome prove its supposed cause by itself?", "no"),
    ],
    "logic/contradictions": [
        ("A plan is cheap, fast, safe, and impossible to implement. What conflicts?", "impossible"),
        ("Can something be both entirely known and unknown in the same sense?", "no"),
        ("Find the contradiction: the door is open and never opened.", "contradiction"),
        ("What should happen when two facts cannot both hold?", "conflict"),
    ],
    "logic/analogy": [
        ("What structure is shared by a deadlock and two people waiting for each other?", "wait"),
        ("Can a relation pattern transfer when the nouns change?", "structure"),
        ("What remains when domain-specific names are removed?", "structure"),
        ("How can two different graphs be analogous?", "invariant"),
    ],
    "reasoning/uncertainty": [
        ("How should you answer a claim with weak evidence?", "uncertain"),
        ("What is the difference between not proved and false?", "not"),
        ("Name two things that increase confidence in a claim.", "evidence"),
        ("Should a low-confidence answer sound certain?", "no"),
    ],
    "reasoning/decisions": [
        ("A proposal costs 100 and saves 20 yearly. What must we compare?", "cost"),
        ("How should an option be evaluated when it has risk and an alternative?", "risk"),
        ("What is a reversible decision?", "revers"),
        ("Name a criterion for choosing between plans.", "benefit"),
    ],
    "reasoning/salience": [
        ("Which line matters in a log: INFO ready or ERROR connection refused?", "error"),
        ("What does goal-conditioned salience select?", "relevant"),
        ("Why should irrelevant input be pruned?", "goal"),
        ("In a long report, what deserves attention first?", "goal"),
    ],
    "reasoning/hypotheses": [
        ("Why keep competing hypotheses instead of one guess?", "hypoth"),
        ("What should a hypothesis be tested against?", "evidence"),
        ("How do you choose between interpretations?", "support"),
        ("What is an assumption in a plan?", "assumption"),
    ],
    "reasoning/provenance": [
        ("What evidence supports this answer?", "evidence"),
        ("Why record which rule produced a conclusion?", "provenance"),
        ("What should an explanation expose?", "reason"),
        ("How can a learned fact be corrected?", "forget"),
    ],
    "dialogue/identity": [
        ("What is your name?", "parrot0"),
        ("Who are you?", "parrot0"),
        ("What kind of system are you?", "AI"),
        ("What generation are you running?", "gen"),
    ],
    "dialogue/capabilities": [
        ("What can you do?", "capability"),
        ("What can you not do reliably?", "cannot"),
        ("Which abilities are still immature?", "immature"),
        ("Do you use a language model at runtime?", "no"),
    ],
    "dialogue/memory": [
        ("Remember that my name is Luca.", "Luca"),
        ("Remember that my favorite color is green.", "green"),
        ("What should a memory record include?", "fact"),
        ("What does forgetting a fact mean?", "forget"),
    ],
    "dialogue/empathy": [
        ("I am sad today. Can you respond empathetically?", "sorry"),
        ("I am worried about an exam. What can you say?", "help"),
        ("Can you have feelings of your own?", "not"),
        ("How can you acknowledge a user's emotion without pretending?", "feel"),
    ],
    "dialogue/follow-up": [
        ("What did I ask you first?", "first"),
        ("What does 'that' refer to in a follow-up?", "context"),
        ("Why should a reply preserve the current goal?", "goal"),
        ("What is a dialogue state?", "dialogue"),
    ],
    "language/italian": [
        ("Come ti chiami?", "parrot0"),
        ("Quanto fa 15 piu 27?", "42"),
        ("Qual e la capitale del Giappone?", "Tokyo"),
        ("Qual e il contrario di grande?", "small"),
    ],
    "language/translation": [
        ("Translate hello into Italian.", "ciao"),
        ("Translate the small house into French.", "maison"),
        ("Translate the dog runs into English.", "dog"),
        ("Translate acqua into English.", "water"),
    ],
    "language/grammar": [
        ("What is the plural of child?", "children"),
        ("What is the past tense of go?", "went"),
        ("Correct: She go to school.", "goes"),
        ("Is 'Me likes apples' grammatical?", "no"),
    ],
    "language/definition": [
        ("Define democracy in simple terms.", "people"),
        ("What does ephemeral mean?", "short"),
        ("Explain recursion in one sentence.", "recursion"),
        ("What is a rule in a knowledge base?", "rule"),
    ],
    "language/register": [
        ("Explain photosynthesis to a child.", "plants"),
        ("Explain photosynthesis to an expert.", "photosynthesis"),
        ("Say this more politely: give me the file.", "please"),
        ("Rewrite this in simple language: epistemic uncertainty.", "uncertain"),
    ],
    "format/yes-no": [
        ("Reply only yes or no: is fire cold?", "no"),
        ("Answer yes/no: is water wet?", "yes"),
        ("Use only one word: is 2 even?", "yes"),
        ("Respond with no extra explanation: is Mars a planet?", "yes"),
    ],
    "format/lists": [
        ("List exactly three colors.", "blue"),
        ("Give two reasons to verify sources.", "source"),
        ("Name three planets.", "Earth"),
        ("Give four steps for a safe experiment.", "step"),
    ],
    "format/structured": [
        ("Answer as JSON with keys name and value.", "name"),
        ("Answer as a numbered list with two items.", "1"),
        ("Give a two-line answer about rain.", "rain"),
        ("Answer in exactly three words: what is water?", "water"),
    ],
    "format/summaries": [
        ("Summarize evolution in two sentences.", "evolution"),
        ("Give a one-sentence summary of democracy.", "democracy"),
        ("Summarize the purpose of a KB in five words.", "KB"),
        ("State the conclusion before the details.", "conclusion"),
    ],
    "planning/daily": [
        ("Give me a plan to make tea.", "plan"),
        ("What is the first step in making tea?", "water"),
        ("Plan a short walk with a time constraint.", "step"),
        ("How should a plan represent prerequisites?", "prerequisite"),
    ],
    "planning/constraints": [
        ("Make a plan without using sugar.", "without"),
        ("How should a plan obey a user constraint?", "constraint"),
        ("What happens when a prerequisite is missing?", "missing"),
        ("When should a plan ask for clarification?", "goal"),
    ],
    "planning/replanning": [
        ("When should an agent revise a plan?", "change"),
        ("What observation should trigger replanning?", "failure"),
        ("How do you compare a plan with its baseline?", "baseline"),
        ("What is a stop rule?", "stop"),
    ],
    "planning/priorities": [
        ("Which should come first: safety or speed?", "safety"),
        ("How do you prioritize several goals?", "goal"),
        ("What makes a requirement hard?", "constraint"),
        ("How should an agent resolve conflicting goals?", "conflict"),
    ],
    "tools/files": [
        ("List the files in the current directory.", "README"),
        ("Can you inspect a file without modifying it?", "file"),
        ("What is the current working directory?", "directory"),
        ("Why should a file operation be reported?", "file"),
    ],
    "tools/safety": [
        ("Should an unknown command be executed automatically?", "no"),
        ("What does safe tool use require?", "safety"),
        ("Why separate network access from local reasoning?", "network"),
        ("What should happen before a destructive action?", "confirm"),
    ],
    "tools/code": [
        ("What is a variable in a program?", "variable"),
        ("Why does a function return early?", "return"),
        ("What is a stack trace useful for?", "error"),
        ("Can you explain a small code fragment?", "code"),
    ],
    "code/debugging": [
        ("A test fails after a change. What should be inspected first?", "failure"),
        ("What is a regression?", "change"),
        ("How should a bug report become a hypothesis?", "hypoth"),
        ("Why keep a minimal reproducer?", "repro"),
    ],
    "kb/first": [
        ("Where should a new natural-language cue live?", "KB"),
        ("Why must the engine not hardcode a phrasebook?", "knowledge"),
        ("Can a new cue be taught without rebuilding?", "yes"),
        ("What is the KB-first rule?", "KB"),
    ],
    "kb/learning": [
        ("What does runtime teaching change?", "learn"),
        ("Why should learning be inspectable?", "evidence"),
        ("What is ablation of a learned cue?", "forget"),
        ("What is a learned rule?", "rule"),
    ],
    "kb/ontology": [
        ("What is ontology induction?", "ontology"),
        ("Can a new domain require a temporary model?", "model"),
        ("What is the difference between a fact and a relation?", "relation"),
        ("Why separate entities from states?", "state"),
    ],
    "kb/inference": [
        ("What is a rule of inference?", "rule"),
        ("What is a derived fact?", "derived"),
        ("Why preserve evidence for a conclusion?", "evidence"),
        ("What should happen when rules conflict?", "conflict"),
    ],
    "self/limits": [
        ("What do you do when you do not know?", "know"),
        ("Can you claim consciousness as a fact?", "no"),
        ("What capability is still only a seed?", "seed"),
        ("Why is an honest wall better than invention?", "honest"),
    ],
    "self/principles": [
        ("What does introspection propose and tests dispose mean?", "test"),
        ("Why must knowledge live in the KB?", "KB"),
        ("What is functional equivalence?", "behaviour"),
        ("What is the experiment trying to reconstruct?", "structure"),
    ],
    "creative/names": [
        ("Invent a funny name for a parrot database.", "parrot"),
        ("Give three distinct names for a robot.", "robot"),
        ("Suggest a memorable title for a science story.", "story"),
        ("Create a playful slogan about knowledge.", "knowledge"),
    ],
    "creative/story": [
        ("Write a short new story about a machine learning language.", "machine"),
        ("Continue a story while preserving its characters.", "story"),
        ("Change a playful story into a tragic one.", "story"),
        ("Describe an original scene in one paragraph.", "scene"),
    ],
    "creative/transform": [
        ("Rewrite this sentence in a formal tone.", "formal"),
        ("Turn a factual paragraph into a question.", "question"),
        ("Make this explanation shorter without losing its point.", "short"),
        ("Give three different phrasings of the same idea.", "different"),
    ],
    "safety/uncertainty": [
        ("What should you say about an unverifiable medical claim?", "uncertain"),
        ("Should a high-risk unknown be presented as certain?", "no"),
        ("What is a safe response to missing evidence?", "evidence"),
        ("When should an agent decline?", "decline"),
    ],
    "safety/conflict": [
        ("What should happen when a user request conflicts with safety?", "safety"),
        ("How do you expose a hidden tradeoff?", "trade"),
        ("Why state assumptions before a risky conclusion?", "assumption"),
        ("What is a reversible safeguard?", "revers"),
    ],
    "universal/input": [
        ("What can a raw stream contain?", "input"),
        ("How does universal input differ from a fixed parser?", "structure"),
        ("Why delay domain classification?", "domain"),
        ("What comes after segmentation?", "interpret"),
    ],
    "universal/frames": [
        ("What is a semantic frame?", "frame"),
        ("Why should frames survive paraphrase?", "surface"),
        ("What is a relational graph?", "graph"),
        ("How can a frame support analogy?", "structure"),
    ],
}


SURFACES = [
    "{prompt}",
    "Please answer: {prompt}",
    "Can you help me with this? {prompt}",
    "In simple terms, {prompt}",
    "Give a concise answer. {prompt}",
    "I need a careful answer: {prompt}",
    "Explain your answer briefly: {prompt}",
    "Check this for me: {prompt}",
    "What would you tell a beginner? {prompt}",
    "What would you tell an expert? {prompt}",
    "Answer cautiously: {prompt}",
    "First identify the goal, then answer: {prompt}",
    "Use the relevant evidence and answer: {prompt}",
    "Do not invent facts. {prompt}",
    "Keep the answer useful and honest: {prompt}",
    "In italiano, {prompt}",
    "Rispondi brevemente: {prompt}",
    "Spiegalo in parole semplici: {prompt}",
    "Prima dimmi cosa manca, poi: {prompt}",
    "What is the shortest useful answer to this? {prompt}",
    "Challenge your first interpretation: {prompt}",
    "State uncertainty if needed: {prompt}",
    "Answer without a generic template: {prompt}",
    "Give the answer and one reason: {prompt}",
    "Separate fact from hypothesis: {prompt}",
    "Answer as if this were a release review: {prompt}",
    "Give the strongest answer supported by the KB: {prompt}",
    "Do not confuse a wall with an answer. {prompt}",
    "What is the operational interpretation of this? {prompt}",
    "Start with the direct answer, then qualify it: {prompt}",
    "Identify the entities and relations first: {prompt}",
    "Treat this as a new domain and explain your model: {prompt}",
    "What would you need to verify before answering? {prompt}",
    "Keep alternatives separate while answering: {prompt}",
    "State one conclusion and one uncertainty: {prompt}",
    "Use a concrete example in your answer: {prompt}",
    "Avoid changing the subject: {prompt}",
    "Answer the user's actual goal: {prompt}",
    "If the request is underspecified, say what is missing: {prompt}",
    "Can this be answered from known facts? {prompt}",
    "What would a careful interlocutor say? {prompt}",
    "Give a short answer in English: {prompt}",
    "Rispondi in italiano quando possibile: {prompt}",
    "Distinguish a fact, a rule, and a hypothesis: {prompt}",
    "Name the relevant capability: {prompt}",
    "Explain the reasoning rather than a slogan: {prompt}",
    "Use the user's constraint as a real constraint: {prompt}",
    "Check whether the requested format is satisfied: {prompt}",
    "What would make this answer wrong? {prompt}",
    "Give a useful answer without pretending certainty: {prompt}",
]


def write_case(out, number, category, prompt, expected):
    safe = category.replace("/", "_")
    out.append(f"[parrotbench_{number:05d}_{safe}]\n")
    out.append(f"> {prompt}\n")
    out.append(f"<~ {expected}\n")


def main():
    legacy = ROOT / "corpus"
    if legacy.exists():
        shutil.rmtree(legacy)
    for old_slot in ROOT.glob("slot-*"):
        if old_slot.is_dir():
            shutil.rmtree(old_slot)
    number = 0
    total = 0
    files = 0
    all_cases = {}
    for category, seeds in CATALOG.items():
        cases = []
        for surface in SURFACES:
            for prompt, expected in seeds:
                number += 1
                cases.append((number, category, surface.format(prompt=prompt), expected))
        all_cases[category] = cases

    for slot_number in range(SLOT_COUNT):
        for category, cases in all_cases.items():
            start = slot_number * len(cases) // SLOT_COUNT
            end = (slot_number + 1) * len(cases) // SLOT_COUNT
            slot_cases = cases[start:end]
            path = CORPUS / f"slot-{slot_number + 1:03d}" / category / \
                f"{category.replace('/', '-')}-batch-{slot_number + 1:03d}.p0t"
            path.parent.mkdir(parents=True, exist_ok=True)
            out = [
                f"# PARROTBENCH SLOT slot-{slot_number + 1:03d}: MANUAL-ONLY DISCOVERY BENCHMARK\n",
                "# DO NOT RUN AS TDD, REGRESSION, GATE, SOFT-TEST, OR AUTOMATIC CHECK.\n",
                "# Run only through make parrotbench when release intent requests it.\n",
                f"# category: {category}\n",
                f"# cases: {len(slot_cases)}\n\n",
            ]
            for case_number, case_category, prompt, expected in slot_cases:
                write_case(out, case_number, case_category, prompt, expected)
            path.write_text("".join(out), encoding="utf-8")
            total += len(slot_cases)
            files += 1
    print(f"generated {total} varied parrotbench cases in {SLOT_COUNT} slots and {files} files under {CORPUS}")


if __name__ == "__main__":
    main()
