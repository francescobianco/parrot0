#!/usr/bin/env bash
# gen151: the gen150 expert/skill knowledge must be SPOKEN, not dumped as raw
# Prolog. These tests query the domain knowledge conversationally and assert a
# natural sentence — proving the description facts verbalize through one general
# path (no per-concept phrasebook) and that no clause leaks as "pred(arg, ...)".
# EN + IT exercise the SAME render path, the bilingual ratchet (LOOP.md step 6).
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "knowledge: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/core/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS knowledge: $1"; pass=$((pass+1))
    else echo "FAIL knowledge: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- mathematics: concepts speak, not dump ---
expect "arithmetic: what is addition" \
    "what is addition" \
    "experts/mathematics/arithmetic.p0" \
    "addition is combining two numbers to get their sum."

expect "arithmetic: bare concept (no article)" \
    "what is prime" \
    "experts/mathematics/arithmetic.p0" \
    "prime is a number greater than 1 divisible only by 1 and itself."

# --- medicine: definite article + multiword topic reach the concept ---
expect "anatomy: what is the heart" \
    "what is the heart" \
    "experts/medicine/anatomy.p0" \
    "heart is muscular pump circulating blood through the body."

expect "anatomy: what is the femur" \
    "what is the femur" \
    "experts/medicine/anatomy.p0" \
    "femur is thigh bone -- longest and strongest bone in the body."

expect "anatomy: tell me about the brain" \
    "tell me about the brain" \
    "experts/medicine/anatomy.p0" \
    "brain is control center processing sensory information and directing responses."

# --- gen152: facts the old engine MANGLED (commas) or DROPPED (>63 chars) now
#     load intact through quote-aware parse_term + the 128-char atom capacity. ---
expect "long+comma description (was mangled): kiss" \
    "what is kiss" \
    "profiles/agi.p0" \
    "kiss is keep it simple, stupid — prefer simple, obvious solutions over clever ones."

expect "long description (was dropped): api" \
    "what is api" \
    "profiles/agi.p0" \
    "api is Application Programming Interface — a set of functions for building software."

# --- bilingual ratchet: the IT question hits the SAME render path ---
expect "IT: cos'e addition" \
    "cos'è addition" \
    "experts/mathematics/arithmetic.p0" \
    "addition is combining two numbers to get their sum."

expect "IT: cos'e multiplication" \
    "cos'è multiplication" \
    "experts/mathematics/arithmetic.p0" \
    "multiplication is repeated addition of a number."

# --- gen155: concept recall by SIMILARITY (no exact key named). Generalises to
#     held-out paraphrases via structural overlap with the KB's descriptions —
#     the first brick of a similarity space, not another cue list. ---
expect "recall by property (held-out): longest bone -> femur" \
    "what is the longest bone in the body" \
    "experts/medicine/anatomy.p0" \
    "You might mean femur: thigh bone -- longest and strongest bone in the body."

expect "recall by paraphrase (held-out): combines two numbers -> addition" \
    "what is the operation that combines two numbers" \
    "experts/mathematics/arithmetic.p0" \
    "You might mean addition: combining two numbers to get their sum."

# Cross-lingual: an Italian query resolves to an English-described concept via
# COGNATE overlap (circonferenza~circumference, diametro~diameter) — similarity
# crossing the language barrier with no translation table.
expect "IT cross-lingual cognate recall: -> pi" \
    "qual è il rapporto tra circonferenza e diametro" \
    "experts/mathematics/arithmetic.p0" \
    "You might mean pi: 3.14159... -- the ratio of a circle's circumference to its diameter."

# gen156: idf weighting breaks an agi-SCALE tie that made gen155 abstain. With
# hundreds of concepts, "blood" is common (low weight) and "pump" is rare (high),
# so the heart wins clearly where plain counting tied and walled.
expect "idf breaks agi-scale tie: pumps blood -> heart" \
    "what is the organ that pumps blood" \
    "profiles/agi.p0" \
    "You might mean heart: muscular pump circulating blood through the body."

# gen157: EMERGENT relational reasoning. parrot0 was never told "heart is part of
# circulatory" — it RECOVERS the relation because the circulatory description
# names the heart. A taxonomy derived from unstructured text, not asserted facts.
expect "emergent containment: heart -> circulatory" \
    "what is the heart part of" \
    "experts/medicine/anatomy.p0" \
    "heart is part of circulatory: heart and blood vessels -- transports blood, oxygen and nutrients."

expect "emergent containment: liver -> digestive" \
    "what system is the liver part of" \
    "experts/medicine/anatomy.p0" \
    "liver is part of digestive: stomach, intestines and liver -- breaks down and absorbs food."

# Bilingual: the IT containment cue "fa parte" reaches the SAME derivation.
expect "IT emergent containment: di cosa fa parte heart" \
    "di cosa fa parte heart" \
    "experts/medicine/anatomy.p0" \
    "heart is part of circulatory: heart and blood vessels -- transports blood, oxygen and nutrients."

# Honest: a concept nothing places in the text must NOT be invented a container.
expect "no fabricated relation: femur is unplaced" \
    "what is the femur part of" \
    "experts/medicine/anatomy.p0" \
    "I don't understand that yet."

# An unknown topic must NOT be force-fit to a concept (precision over recall).
expect "honest miss: unknown topic does not fuzzy-fire" \
    "what is the weather like today" \
    "experts/medicine/anatomy.p0" \
    "I don't understand that yet."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
