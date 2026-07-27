#!/usr/bin/env bash
# Focused ratchet for gen366 Task IR + proof-carrying reasoning operators.
# No full suite and no LLMSCORE remote call.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
pass=0 fail=0

probe() { # id, prompt, required fragment
    local id="$1" prompt="$2" want="$3" got rc
    got="$(printf '%s\n/quit\n' "$prompt" |
        timeout 1 env PARROT0_BASE= PARROT0_SESSION= \
            PARROT0_WORLD_FACTS=1 "$BIN" 2>/dev/null)"
    rc=$?
    got="${got%%$'\n'*}"
    if [ "$rc" -eq 0 ] && [[ "$got" == *"$want"* ]]; then
        echo "PASS reasoning-operators $id"
        pass=$((pass + 1))
    else
        echo "FAIL reasoning-operators $id rc=$rc want=[$want] got=[$got]" >&2
        fail=$((fail + 1))
    fi
}

probe "train transport" \
  "Compare a bicycle and a car for a low-emission urban trip. Which should I choose and why?" \
  "a bicycle is the better fit"
probe "train data structures" \
  "Compare an array and a linked list for a random access workload. Which is better and why?" \
  "an array is the better fit"
probe "held-out materials" \
  "Compare glass and polycarbonate for an impact-resistant guard. Which should I choose and why?" \
  "polycarbonate is the better fit"
probe "R2 train classifier" \
  "Why is a spam filter susceptible to adversarial spelling? Explain with an example." \
  "inserted punctuation"
probe "R2 train control" \
  "Why is an autopilot vulnerable to sensor spoofing? Explain with an example." \
  "mutually consistent false measurements"
probe "R2 held-out biology" \
  "Why is the immune system susceptible to autoimmune disease? Explain with an example." \
  "type 1 diabetes"
probe "R2 frozen optical eval" \
  "Explain why the human brain is susceptible to optical illusions and provide an example that illustrates this susceptibility." \
  "Müller-Lyer illusion"
probe "R3 train coffee" \
  "Give step-by-step instructions for making pour-over coffee." \
  "wait about 30 seconds for the bloom"
probe "R3 train release" \
  "Give step-by-step instructions for a software release." \
  "Promote the verified artifact gradually"
probe "R3 held-out emergency kit" \
  "Give step-by-step instructions for building an emergency kit." \
  "medication, and utility-shutoff instructions"
probe "R3 frozen constrained recipe" \
  "Provide a step-by-step recipe for a three-course meal that can be prepared in under an hour using only a microwave and a toaster." \
  "about 50 minutes"
probe "R3 frozen flat-pack manual" \
  "Write a concise instruction manual for assembling a piece of flat-pack furniture without using the included diagrams." \
  "corner-to-corner diagonals"
probe "R3 frozen brewing process" \
  "Describe the process of brewing beer at home, emphasizing the steps that most affect flavor." \
  "yeast-derived ester and phenol profile"

MCP_DIR="${TMPDIR:-/tmp}/parrot0-mcp-reasoning-$$"
export PARROT0_MCP_DIR="$MCP_DIR"
cleanup() {
    "$ROOT/scripts/mcp-live.sh" stop >/dev/null 2>&1 || true
    rm -f "$MCP_DIR/res" "$MCP_DIR/log"
    rmdir "$MCP_DIR" 2>/dev/null || true
}
trap cleanup EXIT
"$ROOT/scripts/mcp-live.sh" start PARROT0_BASE= PARROT0_WORLD_FACTS=1 >/dev/null
call() { "$ROOT/scripts/mcp-live.sh" call "$1" "$2"; }

transport="Compare a bicycle and a car for a low-emission urban trip. Which should I choose and why?"
materials="Compare glass and polycarbonate for an impact-resistant guard. Which should I choose and why?"

# The prompt leaves a queryable typed trace.
call gen.respond "{\"input\":\"$transport\"}" >/dev/null
ir_op="$(call kb.query '{"pred":"task_ir","args":["operation","goal_comparison"]}')"
ir_a="$(call kb.query '{"pred":"task_ir","args":["argument_1","bicycle"]}')"
ir_b="$(call kb.query '{"pred":"task_ir","args":["argument_2","car"]}')"
ir_goal="$(call kb.query '{"pred":"task_ir","args":["goal","low_emission_urban_trip"]}')"
if [[ "$ir_op" == *'"provable":true'* ]] &&
   [[ "$ir_a" == *'"provable":true'* ]] &&
   [[ "$ir_b" == *'"provable":true'* ]] &&
   [[ "$ir_goal" == *'"provable":true'* ]]; then
    echo "PASS reasoning-operators Task IR projection"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators Task IR projection" >&2
    fail=$((fail + 1))
fi

# The conclusions are proved by variable-bearing clauses, not stored answers.
diff_proof="$(call kb.explain \
  '{"pred":"task_difference","args":["bicycle","car","emissions","pair(low,high)"]}')"
goal_proof="$(call kb.explain \
  '{"pred":"task_goal_match","args":["bicycle","low_emission_urban_trip","emissions","low"]}')"
if [[ "$diff_proof" == *'"provable":true'* ]] &&
   [[ "$diff_proof" == *"property(bicycle, emissions, low)"* ]] &&
   [[ "$goal_proof" == *'"provable":true'* ]] &&
   [[ "$goal_proof" == *"goal_prefers"* ]]; then
    echo "PASS reasoning-operators clause proofs"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators clause proofs" >&2
    fail=$((fail + 1))
fi

failure_proof="$(call kb.explain \
  '{"pred":"failure_mechanism","args":["spam_filter","adversarial_spelling","lexical_pattern_matching","surface_form_shift"]}')"
if [[ "$failure_proof" == *'"provable":true'* ]] &&
   [[ "$failure_proof" == *"system_relies_on"* ]] &&
   [[ "$failure_proof" == *"phenomenon_exploits"* ]]; then
    echo "PASS reasoning-operators R2 clause proof"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators R2 clause proof" >&2
    fail=$((fail + 1))
fi

process_proof="$(call kb.explain \
  '{"pred":"process_reaches","args":["pour_over_coffee","coffee_ready"]}')"
if [[ "$process_proof" == *'"provable":true'* ]] &&
   [[ "$process_proof" == *"action_requires"* ]] &&
   [[ "$process_proof" == *"action_produces"* ]]; then
    echo "PASS reasoning-operators R3 recursive process proof"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators R3 recursive process proof" >&2
    fail=$((fail + 1))
fi

# Rule-family ablation: one activation fact controls both general clauses.
before="$(call gen.respond "{\"input\":\"$transport\"}")"
call kb.retract \
  '{"pred":"reasoning_operator_active","args":["goal_comparison"]}' >/dev/null
ablated="$(call gen.respond "{\"input\":\"$transport\"}")"
call kb.assert \
  '{"pred":"reasoning_operator_active","args":["goal_comparison"]}' >/dev/null
restored="$(call gen.respond "{\"input\":\"$transport\"}")"
if [[ "$before" == *"a bicycle is the better fit"* ]] &&
   [[ "$ablated" != *"a bicycle is the better fit"* ]] &&
   [[ "$restored" == *"a bicycle is the better fit"* ]]; then
    echo "PASS reasoning-operators cross-domain operator ablation"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators cross-domain operator ablation" >&2
    fail=$((fail + 1))
fi

# A local fact ablation breaks only the held-out material decision.
train_before="$(call gen.respond "{\"input\":\"$transport\"}")"
held_before="$(call gen.respond "{\"input\":\"$materials\"}")"
call kb.retract \
  '{"pred":"property","args":["polycarbonate","impact_resistance","high"]}' >/dev/null
train_after="$(call gen.respond "{\"input\":\"$transport\"}")"
held_after="$(call gen.respond "{\"input\":\"$materials\"}")"
call kb.assert \
  '{"pred":"property","args":["polycarbonate","impact_resistance","high"]}' >/dev/null
if [[ "$train_before" == *"a bicycle is the better fit"* ]] &&
   [[ "$train_after" == *"a bicycle is the better fit"* ]] &&
   [[ "$held_before" == *"polycarbonate is the better fit"* ]] &&
   [[ "$held_after" != *"polycarbonate is the better fit"* ]]; then
    echo "PASS reasoning-operators local fact ablation"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators local fact ablation" >&2
    fail=$((fail + 1))
fi

failure_prompt="Why is a spam filter susceptible to adversarial spelling? Explain with an example."
failure_before="$(call gen.respond "{\"input\":\"$failure_prompt\"}")"
call kb.retract \
  '{"pred":"reasoning_operator_active","args":["failure_explanation"]}' >/dev/null
failure_ablated="$(call gen.respond "{\"input\":\"$failure_prompt\"}")"
call kb.assert \
  '{"pred":"reasoning_operator_active","args":["failure_explanation"]}' >/dev/null
failure_restored="$(call gen.respond "{\"input\":\"$failure_prompt\"}")"
if [[ "$failure_before" == *"inserted punctuation"* ]] &&
   [[ "$failure_ablated" != *"inserted punctuation"* ]] &&
   [[ "$failure_restored" == *"inserted punctuation"* ]]; then
    echo "PASS reasoning-operators R2 operator ablation"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators R2 operator ablation" >&2
    fail=$((fail + 1))
fi

process_prompt="Give step-by-step instructions for making pour-over coffee."
process_before="$(call gen.respond "{\"input\":\"$process_prompt\"}")"
call kb.retract \
  '{"pred":"reasoning_operator_active","args":["ordered_procedure"]}' >/dev/null
process_ablated="$(call gen.respond "{\"input\":\"$process_prompt\"}")"
call kb.assert \
  '{"pred":"reasoning_operator_active","args":["ordered_procedure"]}' >/dev/null
process_restored="$(call gen.respond "{\"input\":\"$process_prompt\"}")"
if [[ "$process_before" == *"wait about 30 seconds"* ]] &&
   [[ "$process_ablated" != *"wait about 30 seconds"* ]] &&
   [[ "$process_restored" == *"wait about 30 seconds"* ]]; then
    echo "PASS reasoning-operators R3 operator ablation"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators R3 operator ablation" >&2
    fail=$((fail + 1))
fi

# Runtime growth for each new surface registry.
op_prompt="Please weigh-up a bicycle and a car for a low-emission urban trip. Recommend one and explain why."
op_before="$(call gen.respond "{\"input\":\"$op_prompt\"}")"
call kb.assert \
  '{"pred":"task_operation_cue","args":["goal_comparison","weigh-up"]}' >/dev/null
op_after="$(call gen.respond "{\"input\":\"$op_prompt\"}")"
call kb.retract \
  '{"pred":"task_operation_cue","args":["goal_comparison","weigh-up"]}' >/dev/null
op_gone="$(call gen.respond "{\"input\":\"$op_prompt\"}")"
if [[ "$op_before" != *"a bicycle is the better fit"* ]] &&
   [[ "$op_after" == *"a bicycle is the better fit"* ]] &&
   [[ "$op_gone" != *"a bicycle is the better fit"* ]]; then
    echo "PASS reasoning-operators operation cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators operation cue growth" >&2
    fail=$((fail + 1))
fi

goal_prompt="Compare a bicycle and a car for clean-city-use. Which should I choose and why?"
goal_before="$(call gen.respond "{\"input\":\"$goal_prompt\"}")"
call kb.assert \
  '{"pred":"task_goal_cue","args":["low_emission_urban_trip","clean-city-use"]}' >/dev/null
goal_after="$(call gen.respond "{\"input\":\"$goal_prompt\"}")"
call kb.retract \
  '{"pred":"task_goal_cue","args":["low_emission_urban_trip","clean-city-use"]}' >/dev/null
goal_gone="$(call gen.respond "{\"input\":\"$goal_prompt\"}")"
if [[ "$goal_before" != *"a bicycle is the better fit"* ]] &&
   [[ "$goal_after" == *"a bicycle is the better fit"* ]] &&
   [[ "$goal_gone" != *"a bicycle is the better fit"* ]]; then
    echo "PASS reasoning-operators goal cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators goal cue growth" >&2
    fail=$((fail + 1))
fi

entity_prompt="Compare cycle-alpha and auto-beta for a low-emission urban trip. Which should I choose and why?"
entity_before="$(call gen.respond "{\"input\":\"$entity_prompt\"}")"
call kb.assert \
  '{"pred":"task_entity_cue","args":["bicycle","cycle-alpha"]}' >/dev/null
call kb.assert \
  '{"pred":"task_entity_cue","args":["car","auto-beta"]}' >/dev/null
entity_after="$(call gen.respond "{\"input\":\"$entity_prompt\"}")"
call kb.retract \
  '{"pred":"task_entity_cue","args":["bicycle","cycle-alpha"]}' >/dev/null
call kb.retract \
  '{"pred":"task_entity_cue","args":["car","auto-beta"]}' >/dev/null
entity_gone="$(call gen.respond "{\"input\":\"$entity_prompt\"}")"
if [[ "$entity_before" != *"a bicycle is the better fit"* ]] &&
   [[ "$entity_after" == *"a bicycle is the better fit"* ]] &&
   [[ "$entity_gone" != *"a bicycle is the better fit"* ]]; then
    echo "PASS reasoning-operators entity cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators entity cue growth" >&2
    fail=$((fail + 1))
fi

focus_prompt="Describe the process of brewing beer at home, emphasizing taste-impact-alpha."
focus_before="$(call gen.respond "{\"input\":\"$focus_prompt\"}")"
call kb.assert \
  '{"pred":"task_focus_cue","args":["flavor","taste-impact-alpha"]}' >/dev/null
focus_after="$(call gen.respond "{\"input\":\"$focus_prompt\"}")"
call kb.retract \
  '{"pred":"task_focus_cue","args":["flavor","taste-impact-alpha"]}' >/dev/null
focus_gone="$(call gen.respond "{\"input\":\"$focus_prompt\"}")"
if [[ "$focus_before" != *"strongly affects"* ]] &&
   [[ "$focus_after" == *"strongly affects"* ]] &&
   [[ "$focus_gone" != *"strongly affects"* ]]; then
    echo "PASS reasoning-operators focus cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators focus cue growth" >&2
    fail=$((fail + 1))
fi

deadline_prompt="Provide a step-by-step recipe for a three-course meal within cap-alpha using only a microwave and a toaster."
deadline_before="$(call gen.respond "{\"input\":\"$deadline_prompt\"}")"
call kb.assert \
  '{"pred":"task_deadline_cue","args":["minutes_60","cap-alpha"]}' >/dev/null
deadline_after="$(call gen.respond "{\"input\":\"$deadline_prompt\"}")"
call kb.retract \
  '{"pred":"task_deadline_cue","args":["minutes_60","cap-alpha"]}' >/dev/null
deadline_gone="$(call gen.respond "{\"input\":\"$deadline_prompt\"}")"
if [[ "$deadline_before" != *"about 50 minutes"* ]] &&
   [[ "$deadline_after" == *"about 50 minutes"* ]] &&
   [[ "$deadline_gone" != *"about 50 minutes"* ]]; then
    echo "PASS reasoning-operators deadline cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators deadline cue growth" >&2
    fail=$((fail + 1))
fi

resource_prompt="Provide a step-by-step recipe for a three-course meal under an hour using wave-box and bread-slot."
resource_before="$(call gen.respond "{\"input\":\"$resource_prompt\"}")"
call kb.assert \
  '{"pred":"task_resource_cue","args":["microwave","wave-box"]}' >/dev/null
call kb.assert \
  '{"pred":"task_resource_cue","args":["toaster","bread-slot"]}' >/dev/null
resource_after="$(call gen.respond "{\"input\":\"$resource_prompt\"}")"
call kb.retract \
  '{"pred":"task_resource_cue","args":["microwave","wave-box"]}' >/dev/null
call kb.retract \
  '{"pred":"task_resource_cue","args":["toaster","bread-slot"]}' >/dev/null
resource_gone="$(call gen.respond "{\"input\":\"$resource_prompt\"}")"
if [[ "$resource_before" != *"three finished courses"* ]] &&
   [[ "$resource_after" == *"three finished courses"* ]] &&
   [[ "$resource_gone" != *"three finished courses"* ]]; then
    echo "PASS reasoning-operators resource cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators resource cue growth" >&2
    fail=$((fail + 1))
fi

constraint_prompt="Write an instruction manual for assembling flat-pack furniture, constraint-alpha."
call gen.respond "{\"input\":\"$constraint_prompt\"}" >/dev/null
constraint_before="$(call kb.query '{"pred":"task_ir","args":["constraint","no_diagrams"]}')"
call kb.assert \
  '{"pred":"task_constraint_cue","args":["no_diagrams","constraint-alpha"]}' >/dev/null
call gen.respond "{\"input\":\"$constraint_prompt\"}" >/dev/null
constraint_after="$(call kb.query '{"pred":"task_ir","args":["constraint","no_diagrams"]}')"
call kb.retract \
  '{"pred":"task_constraint_cue","args":["no_diagrams","constraint-alpha"]}' >/dev/null
call gen.respond "{\"input\":\"$constraint_prompt\"}" >/dev/null
constraint_gone="$(call kb.query '{"pred":"task_ir","args":["constraint","no_diagrams"]}')"
if [[ "$constraint_before" == *'"provable":false'* ]] &&
   [[ "$constraint_after" == *'"provable":true'* ]] &&
   [[ "$constraint_gone" == *'"provable":false'* ]]; then
    echo "PASS reasoning-operators constraint cue growth"
    pass=$((pass + 1))
else
    echo "FAIL reasoning-operators constraint cue growth" >&2
    fail=$((fail + 1))
fi

cleanup
trap - EXIT
echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
