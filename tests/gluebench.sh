#!/usr/bin/env bash
#
# gen215: make glue-bench — the LINGUISTIC-GLUE discovery harness
# (docs/plans/the-linguistic-glue.md, G1). The essay's insight: the "colla linguistica"
# becomes visible WHEN IT IS MISSING. So we make its five absence-symptoms into metrics:
# multi-turn held-out dialogues where a later turn depends on an earlier one, and we check
# whether parrot0 carried the continuity. Modelled on swe-bench: a DISCOVERY instrument,
# not a pass/fail gate — it never fails the build; its product is the gap map that PULLS
# the next connective mechanism (G2). It is NOT part of `make test`.
#
# Each case: ID  SYMPTOM  CHECK  DIALOGUE(turns joined by '|').
#   CHECK = has:STR  -> glue HELD iff the LAST reply contains STR
#           no:STR   -> glue HELD iff the LAST reply does NOT contain STR
#           show     -> qualitative; print the reply, count as observed (no verdict)
# Every symptom is probed in English AND Italian (the bilingual ratchet).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
# A knowledgeable agent (anatomy etc.) so content turns have something to cohere around.
export PARROT0_PROFILE="${PARROT0_PROFILE:-$ROOT/kb/profiles/agi.p0}"
export PARROT0_SESSION=

[ -x "$BIN" ] || { echo "glue-bench: binary not built ($BIN)" >&2; exit 1; }

# id  symptom  check  dialogue(|-separated)
CASES=(
  # --- S2: implicit reference (a pronoun/bare reference to a prior entity) ---
  "ref-dog-en|implicit-reference|has:Rex|my dog is Rex|what is his name"
  "ref-dog-it|implicit-reference|has:Rex|il mio cane è Rex|come si chiama"
  "ref-heart-en|implicit-reference|has:circulatory|tell me about the heart|what is it part of"

  # --- S4: integrating a later correction (re-derive the changed conclusion) ---
  "corr-en|correction|no:Yes|socrates is a man|is socrates a man|no, socrates is not a man|is socrates a man"
  "corr-it|correction|no:Yes|socrate è un uomo|socrate è un uomo?|no, socrate non è un uomo|socrate è un uomo?"

  # --- S1: a constraint set earlier should shape a later answer (qualitative) ---
  "brevity-en|out-of-context|show|keep it short|tell me about the heart"
  "brevity-it|out-of-context|show|sii breve|parlami del cuore"

  # --- S3: a precisation that continues a prior request (qualitative) ---
  "precise-en|over-literal|show|what is 2 plus 2|and times 3"

  # --- S5: one interlocutor across faculties (memory -> arithmetic) (qualitative) ---
  "chain-en|one-interlocutor|show|remember my favorite number is 7|what is my favorite number plus 3"
)

last_reply() {                      # dialogue with '|' separators -> last non-empty line
  local d="$1"; local IFS='|'; local turns; read -ra turns <<< "$d"
  printf '%s\n' "${turns[@]}" | "$BIN" 2>/dev/null | awk 'NF{l=$0} END{print l}'
}

echo "LINGUISTIC-GLUE discovery harness — the 5 absence-symptoms as metrics (degrade mode)."
echo "A 'gap' is the essay's point made visible: continuity that did NOT carry across turns."
echo

held=0; gap=0; shown=0
declare -A sym_gap sym_tot
for row in "${CASES[@]}"; do
  IFS='|' read -r id symptom check rest <<< "$row"
  dialogue="$rest"                  # read put the remaining |-joined turns here
  reply="$(last_reply "$dialogue")"

  verdict=""
  case "$check" in
    has:*) want="${check#has:}"; case "$reply" in *"$want"*) verdict="HELD";; *) verdict="GAP";; esac ;;
    no:*)  bad="${check#no:}";   case "$reply" in *"$bad"*)  verdict="GAP";;  *) verdict="HELD";; esac ;;
    show)  verdict="SHOWN" ;;
  esac

  case "$verdict" in
    HELD)  held=$((held+1)); sym_tot[$symptom]=$(( ${sym_tot[$symptom]:-0} + 1 )) ;;
    GAP)   gap=$((gap+1));   sym_tot[$symptom]=$(( ${sym_tot[$symptom]:-0} + 1 ))
           sym_gap[$symptom]=$(( ${sym_gap[$symptom]:-0} + 1 )) ;;
    SHOWN) shown=$((shown+1)) ;;
  esac

  printf "  [%-4s] %-16s %-22s -> %s\n" "$verdict" "$symptom" "$id" "$reply"
done

echo
echo "--- gap map (next pulls for G2) ---"
for s in "${!sym_tot[@]}"; do
  printf "  %-18s %d/%d carried\n" "$s" "$(( ${sym_tot[$s]} - ${sym_gap[$s]:-0} ))" "${sym_tot[$s]}"
done | sort
echo
echo "checked: $((held+gap)) (held: $held, gap: $gap)   qualitative: $shown"
echo "PASS glue-bench: discovery instrument ran; gaps are the next pulls, not build failures."
