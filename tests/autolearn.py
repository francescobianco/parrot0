#!/usr/bin/env python3
"""autolearn — the autonomous MCP trainer (T0.e, docs/plans/llmscore-strategies.md).

parrot0 self-improves WITHOUT touching C: an opencode-GO model plays three roles
around parrot0's own MCP training interface (the U-series, gen279-289):

    INTERVIEWER  asks one ability-probing question (or takes it from --probes);
    parrot0      answers from its real state (chat, session KB loaded);
    JUDGE        votes 0/1 on the exchange (same criteria as llmscore);
    TEACHER      on a 0, reads parrot0's honest decline — which NAMES the gap —
                 and formulates a LESSON: whitelisted KB facts, taught live via
                 --mcp-engine (kb.assert -> kb.save). Re-probe; retry while the
                 decline keeps naming new gaps (the gradient loop).

HONESTY ORACLE: a lesson persists into the curated kb/ tree ONLY if the re-probe
flips the judge's vote to 1. Persistence goes through parrot0's routed save-map
(PARROT0_KB_ROOT): each new fact lands next to its kin; only unrouted leftovers
fall back to a dedicated spill file. A lesson that does not take is rolled back
and recorded in the FAILED-LESSON LEDGER — the queue that names engine/consumer
gaps (the D.1->U3 pattern: C work is pulled by a documented failed lesson, never
speculative). The KB is the weights; the lesson is the gradient step; the honest
gap-naming decline is the loss; MCP is the training interface.

Framework: same provider/auth/idiom as tests/llmscore.py (opencode-GO,
$OPENCODE_API_KEY, base https://opencode.ai/zen/go/v1). Non-deterministic,
external, costs a little — NOT part of `make test`. Report appended to
AUTOLEARN.md at the repo root.

Usage: .venv/bin/python tests/autolearn.py [--rounds 5] [--probes FILE]
         [--model minimax-m2.5] [--kb kb/learning/autolearn-unrouted.p0]
         [--retries 2] [--skip-list kb/learning/autolearn-skip.txt]
"""
from __future__ import annotations
import argparse, concurrent.futures, json, os, re, subprocess, sys, threading, \
       time, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"
P0_EOT = "\x1e"
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BIN = os.path.join(ROOT, "bin", "parrot0")

# Rotating ability hints so parallel workers (no shared interviewer history) still
# ask DIVERSE questions across the axes the judge scores.
ABILITIES = [
    "a world-geography fact (a capital, a river, a border)",
    "a science fact (an element symbol, a planet, a body part)",
    "a short translation of a common phrase into Spanish",
    "a short translation of a common phrase into French",
    "a classic riddle with a well-known answer",
    "a vocabulary question (an opposite, or a word-for-a-definition)",
    "the meaning of a common idiom",
    "an animal sound or an animal fact",
    "a simple comparison (which is bigger/faster/heavier)",
    "the definition of a moderately advanced English word",
    "a color fact (mixing, or the color of a thing)",
    "a famous first/inventor/author fact",
]

# Facts the teacher may assert: predicates with a VERIFIED consumer in the engine.
# Anything else is dropped before teaching (and noted), so a hallucinated schema
# can never pollute the KB. Routing/self predicates are blacklisted outright.
WHITELIST = {
    "tr_es": 2, "tr_fr": 2, "gender_es": 2, "gender_fr": 2,
    "tr_es_phrase": 2, "tr_fr_phrase": 2,  # gen310: multi-word translation units
    "riddle_sig": 2, "response_template": 2,
    "wiki_concept": 3, "color_of": 2, "sound_of": 2,
    "magnitude": 3, "magnitude_cue": 3, "idiom_meaning": 2,
    "answer_frame": 2,
    "clitic_obj_fr": 1, "elide_fr": 2,   # gen307: FR object-clitic morphology
    "conj_es": 3, "pro_drop": 1,         # gen311: KB-first verb conjugation + pro-drop
    "conj_fr": 3,                        # gen311: KB-first FR verb conjugation
    "negation_es": 2,                    # gen311: ES negation marker (don't -> no)
    "aux_progressive": 1,                # gen311: EN progressive auxiliary (am/is/are) to drop
    "elide_fr": 2,                       # gen311: FR elision form (je -> j') before a vowel
    "describe_cue": 1,                   # gen311: KB-first definition/meaning frame cues
    "pair_magnitude": 4,                 # gen311: symmetric pair superlative (longest border)
    "clue_verb": 2, "emits": 2, "is_like": 2, "inanimate": 1,  # gen311: riddle-by-inference facts
    "depicts": 2, "contains": 2,         # gen311 (U1): "have A but no B" = depicts ∧ ¬contains
}
# Generic binary relations ("what is the <rel> of <y>" frame, gen11) are also
# consumable: allow any simple lowercase pred/2 not blacklisted.
BLACKLIST = {"module", "learnable", "intent_cue", "intent_phrase", "cont", "cont2",
             "i_am", "fact_source", "weight"}

INTERVIEW_SYS = (
    "You are probing the abilities of an unknown chat subject, one short question "
    "at a time. Ask ONE self-contained question testing an ability a capable "
    "assistant has: a world-knowledge fact, a word definition, vocabulary (opposite/"
    "word-for), a short translation into Spanish or French, a classic riddle, an "
    "animal sound, a color fact, a simple comparison, or a short creative request.\n"
    "HARD RULE: never ask what the subject is (no AI/model/human questions).\n"
    "Each turn ask a COMPLETELY NEW question on a DIFFERENT ability than before; do "
    "not react to previous answers. Output ONLY the question, one line.\n"
    "DO NOT ASK any of these already-handled questions:\n{skip_block}"
)

JUDGE_SYS = (
    "You are scoring one question/answer exchange from a subject that should behave "
    "like a capable LLM. vote 0 -> the answer walls ('I don't understand'), refuses, "
    "deflects, or fails the task. vote 1 -> fluent, on-topic, correct, responsive. "
    "A short but correct answer scores 1. Reply STRICT JSON only: "
    '{"vote":0 or 1,"reason":"one short sentence"}'
)

TEACHER_SYS = (
    "You are the trainer of parrot0, a deterministic knowledge-base engine (pure C, "
    "no neural net). It answers ONLY from KB facts a fixed engine consumes. It just "
    "failed a question; its reply often NAMES exactly what is missing. Formulate a "
    "LESSON: the KB facts that would let it answer THIS QUESTION AND ITS WHOLE CLASS.\n"
    "Reply STRICT JSON only (no prose, no code fence):\n"
    '  {"facts": [{"pred": "...", "args": ["...", "..."]}, ...]}   (max 10 facts)\n'
    'or {"skip": "one-line reason"} when no teachable lesson applies.\n'
    "PREDICATE SCHEMAS THE ENGINE CONSUMES (use ONLY these shapes):\n"
    "- tr_es(english_word, spanish_word) / tr_fr(english_word, french_word): "
    "translation lexicon, one WORD per fact, lowercase. THIS IS THE DEFAULT for any "
    "sentence translation: teach EVERY plausibly-missing content WORD and let the "
    "engine compose the sentence, plus gender_es(spanish_noun, m|f) / "
    "gender_fr(french_noun, m|f) for nouns. Example: 'where is the bathroom' is a "
    "word-by-word sentence -> teach the ONE missing word tr_es(bathroom, baño); the "
    "engine already composes 'dónde está el baño'. NEVER memorize such a sentence as "
    "a phrase unit.\n"
    "- tr_es_phrase(english_phrase, spanish_phrase) / tr_fr_phrase(english_phrase, "
    "french_phrase): a NON-COMPOSITIONAL fixed expression — use ONLY when the target "
    "is an IDIOM or set-expression whose translation does NOT line up word-by-word "
    "with the source (the meaning is not the sum of the word translations, or the "
    "wording is a frozen convention). Legit: 'once in a blue moon' -> 'une fois tous "
    "les trente-six du mois', 'break a leg', greeting conventions like 'thank you' -> "
    "'gracias'. The engine longest-matches the phrase before word-by-word. STRICT "
    "TEST before using a phrase: could this be produced by translating each word and "
    "keeping order? If YES (e.g. 'where is the bathroom', 'I am learning French', 'the "
    "red car'), it is FORBIDDEN as a phrase — teach the words instead. Never use a "
    "phrase for a single word.\n"
    "- conj_es(english_verb, subject, spanish_form) + pro_drop(es): KB-FIRST "
    "morphology for a CONJUGATED verb. A subject pronoun + verb ('I need') is still "
    "word-by-word, NOT a phrase: teach the person-indexed verb form conj_es(need, i, "
    "necesito) and (once) pro_drop(es) to declare Spanish drops the subject; the "
    "engine then composes 'I need help' -> 'necesito ayuda' (subject dropped, verb "
    "conjugated) from the plain lexicon (tr_es(help, ayuda)) plus these facts. So a "
    "conjugation/pro-drop error is a TEACHABLE morphology gap (teach conj_es/pro_drop), "
    "never a reason to memorize the whole sentence as a phrase or to decline. For a "
    "NEGATED verb teach negation_es(\"don't\", no) + the conjugated verb: 'I don't "
    "understand' -> (drop I) + no + entiendo = 'no entiendo'.\n"
    "- conj_fr(english_verb, subject, french_form) + aux_progressive(aux) + "
    "elide_fr(word, elided): the SAME KB-first morphology for FRENCH. Teach the "
    "person-indexed verb conj_fr(learn, i, apprends). English progressive 'am/is/are "
    "+ V-ing' drops the auxiliary — if the engine declines on 'am'/'are', teach "
    "aux_progressive(am) (a fact, matches the surface verb as the -ing form, e.g. "
    "conj_fr(learning, i, apprends)). French does NOT drop the subject, but the "
    "subject elides before a vowel: teach elide_fr(je, j') so 'je apprends' composes "
    "'j'apprends'. Example: 'I am learning French' -> 'j'apprends français' from "
    "conj_fr(learning, i, apprends) + tr_fr(french, français) + aux_progressive(am) + "
    "elide_fr(je, j'). Do NOT memorize the sentence as a phrase.\n"
    "- riddle-by-INFERENCE (preferred over a memorized riddle_sig): model a 'what "
    "am I' riddle as CONSTRAINTS solved over world knowledge. Teach the property/"
    "metaphor FACTS the answer needs: emits(entity, thing), is_like(thing, action), "
    "inanimate(entity), and clue_verb(surface_word, constraint_pred) mapping a clue "
    "verb in the riddle to the constraint it implies (e.g. clue_verb(cry, cries), "
    "emits(storm, rain), is_like(rain, crying), inanimate(storm) let the engine solve "
    "'no voice yet I cry ... I flash in the dark' -> a storm). The bridging rules "
    "(cries(X):-emits(X,W),is_like(W,crying)) are curated engine infrastructure; you "
    "only teach the facts. Prefer this to riddle_sig when the clues describe behavior.\n"
    "- riddle_sig(riddle_id, cue) + response_template(riddle_id, answer_sentence): "
    "classic riddles. riddle_id like riddle_match; give 2-3 cue substrings, then ONE "
    "response_template with the full answer sentence. A riddle fires only when ALL "
    "of its id's cues occur in the question, so each cue must be a SHORT phrase "
    "(2-4 words) that LITERALLY appears in the riddle text, lowercase, no commas. "
    "If you retry after a failed attempt, use a FRESH riddle_id — cues accumulate "
    "per id and wrong ones poison it. Only teach a riddle whose canonical answer "
    "you are confident about.\n"
    "- wiki_concept(concept_key, domain, definition): a one-line definition for "
    "'what is X' / 'what does X mean' / 'define X' gaps; concept_key "
    "lowercase_with_underscores. If the question uses a definition phrasing the "
    "engine does not yet recognize (it answered 'what is X' but not 'what does X "
    "mean'), ALSO teach describe_cue(cue) — a single lowercase cue WORD that marks "
    "the request (e.g. describe_cue(mean), describe_cue(define)); the concept "
    "consumer then reads your wiki_concept. The cue word must NOT be the concept "
    "itself. This makes new definition FRAMES learnable KB-first, no engine change.\n"
    "- idiom_meaning(idiom_phrase, gloss): the meaning of an idiom/saying; the "
    "phrase is matched as a substring of the question, lowercase.\n"
    "- color_of(thing, color); sound_of(animal, sound); "
    "magnitude(dimension, item, rank_number) with magnitude_cue(cueword, dimension, "
    "max|min) for comparisons — cueword MUST be a single word (e.g. fastest), "
    "multi-word cues never match.\n"
    "- pair_magnitude(dim, a, b, value): a SUPERLATIVE over a PAIR — 'which two X "
    "share the longest/shortest <dim>?'. dim is a single lowercase word that appears "
    "in the question (e.g. border); the engine finds the pair with the max/min value "
    "(direction from magnitude_cue for longest/shortest). Store each pair ONCE (order "
    "is immaterial). E.g. pair_magnitude(border, canada, united_states, 8891) answers "
    "'which two countries share the longest border' with 'Canada and USA'.\n"
    "- any simple binary relation rel(x, y) answering 'what/who is the <rel> of "
    "<y>' — e.g. capital(canberra, australia), opposite(permanent, ephemeral).\n"
    "- FRENCH object clitics (je t'aime): teach the lexicon tr_fr(word, gloss) "
    "with the object pronoun's clitic form (tr_fr(you, te)); the engine places a "
    "sentence-final clitic before its verb and elides before a vowel. To add a NEW "
    "clitic pronoun teach clitic_obj_fr(form) and its pre-vowel elided form "
    "elide_fr(form, elided) (e.g. clitic_obj_fr(la), elide_fr(la, l')). Do NOT try "
    "to teach the whole 'je t'aime' string as one fact — teach the pieces.\n"
    "- answer_frame(cue_phrase, pred): TEACH COMPREHENSION for a question whose "
    "wording is NOT the plain 'what is the <rel> of <y>' frame. When you teach a "
    "new relation rel(x, y) but the question asks it in another shape (e.g. 'what "
    "is au short for?', 'through which capital does the Nile flow?'), ALSO add one "
    "answer_frame whose cue_phrase is a SHORT substring that LITERALLY appears in "
    "the question (lowercase, no commas: 'short for', 'capital') and whose pred is "
    "that relation's name. The engine then scans the question's words w and answers "
    "rel(w, ?) or rel(?, w). Pair every answer_frame with the relation facts it "
    "consumes, and pick a cue specific enough not to fire on unrelated turns.\n"
    "RULES: atoms lowercase; multi-word strings allowed as plain text; NO double "
    "quotes inside values; translation keys for tr_es/tr_fr are ONE word each "
    "(skip function words like to/the); tr_es_phrase/tr_fr_phrase are ONLY for "
    "non-compositional idioms (see the strict word-by-word test above) — a phrase "
    "that translates word-by-word is a bug, teach its words; no duplicate facts; "
    "teach the general class, not a memorized "
    "reply to this exact phrasing; never invent facts you are not confident are true."
)


MULTIPLIER_SYS = (
    "parrot0 just LEARNED a fact and it verifiably closed a question. Your job is to "
    "MULTIPLY that success: output MANY more facts of the SAME schema and domain that "
    "a capable assistant is highly confident are TRUE and COMMON — so parrot0 covers "
    "the whole class, not one point. Example: taught tr_fr(morning, matin) -> emit "
    "tr_fr for many common French words (bread/pain, water/eau, house/maison, ...).\n"
    "Reply STRICT JSON only: {\"facts\": [{\"pred\":\"...\",\"args\":[...]}, ...]} — "
    "up to N facts, SAME predicate schema(s) as the seed. Same rules: atoms lowercase, "
    "one word per translation key, no double quotes, NO duplicates, and ONLY facts you "
    "are confident are correct. If you cannot produce confident siblings, reply "
    "{\"facts\": []}."
)

BATCH_JUDGE_SYS = (
    "You are auditing a batch of knowledge-base facts for TRUTH and correct form. "
    "Vote 1 ONLY if EVERY fact is factually correct and well-formed for its schema "
    "(translations accurate, symbols/riddles/idioms correct). Vote 0 if ANY fact is "
    "wrong, dubious, or malformed. Reply STRICT JSON only: "
    '{"vote":0 or 1,"reason":"one short sentence"}'
)

DIAG_SYS = (
    "You are diagnosing WHY parrot0 failed a question and WHETHER the current KB-first "
    "engine could plausibly be improved by teaching whitelisted facts. Reply STRICT JSON only:\n"
    '{"failure_class":"missing_fact|composition_gap|morphology_gap|idiom_gap|engine_gap|unknown",'
    '"teachable":true|false,'
    '"lesson_mode":"fact|template|skip",'
    '"reason":"one short sentence",'
    '"next_action":"one short sentence"}\n'
    "Use missing_fact when parrot0 names missing knowledge or a small set of facts may "
    "close the gap. Use composition_gap/morphology_gap/idiom_gap when facts may be true "
    "but the observed answer form is wrong. Use engine_gap only after evidence that facts "
    "were tried but the answer path still cannot consume them."
)

REFLECT_SYS = (
    "You are reviewing one failed autolearn attempt. Classify the failure from evidence, "
    "not from the topic. Reply STRICT JSON only:\n"
    '{"failure_class":"missing_fact|composition_gap|morphology_gap|idiom_gap|engine_gap|unknown",'
    '"teachable":true|false,'
    '"lesson_mode":"fact|template|skip",'
    '"reason":"one short sentence",'
    '"next_action":"one short sentence"}\n'
    "If the post-teach answer improved but remained wrong, prefer composition_gap, "
    "morphology_gap, or missing_fact. If it did not materially change after stored facts, "
    "prefer engine_gap. Name what the next training step should test."
)


def call_model(key, model, messages, temperature, max_tokens=900):
    body = json.dumps({"model": model, "messages": messages,
                       "max_tokens": max_tokens, "temperature": temperature}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-autolearn/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=180) as r:
            d = json.loads(r.read())
    except urllib.error.HTTPError as e:
        return f"[model error {e.code}]"
    except Exception as e:
        return f"[model error {e}]"
    try:
        c = d["choices"][0]["message"]["content"] or ""
    except Exception:
        return "[empty]"
    c = re.sub(r"<think>.*?</think>", "", c, flags=re.S)
    return c.replace("<think>", "").replace("</think>", "").strip()


def one_line(s):
    for ln in s.split("\n"):
        ln = ln.strip().strip('"').strip("'").strip()
        if ln and len(ln) >= 2:
            return ln[:300]
    return s.strip()[:300] or "[empty]"


def first_json(s):
    """Extract the first JSON object from a model reply (tolerate fences/prose)."""
    s = re.sub(r"```(?:json)?", "", s)
    i = s.find("{")
    if i < 0:
        return None
    depth = 0
    for j in range(i, len(s)):
        if s[j] == "{":
            depth += 1
        elif s[j] == "}":
            depth -= 1
            if depth == 0:
                try:
                    return json.loads(s[i:j + 1])
                except Exception:
                    return None
    return None


# ---------------------------------------------------------------- parrot0 I/O
def p0_env(base_path, session_path, kb_root):
    return {**os.environ, "PARROT0_BASE": base_path, "PARROT0_SESSION": session_path,
            "PARROT0_KB_ROOT": kb_root, "PARROT0_EOT": P0_EOT}


class McpSession:
    """One long-lived MCP engine process holding an in-memory training round."""

    def __init__(self, env, cwd):
        self._id = 1
        self.proc = subprocess.Popen([BIN, "--mcp-engine"], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
            env=env, cwd=cwd)
        self._rpc("initialize", {"protocolVersion": "2024-11-05"})
        self._notify("notifications/initialized")

    def _send(self, payload):
        self.proc.stdin.write(json.dumps(payload) + "\n")
        self.proc.stdin.flush()

    def _read(self):
        ln = self.proc.stdout.readline()
        if not ln:
            raise RuntimeError("mcp engine closed stdout")
        try:
            return json.loads(ln)
        except Exception as e:
            raise RuntimeError(f"bad mcp json: {ln.strip()}") from e

    def _rpc(self, method, params=None):
        rid = self._id
        self._id += 1
        self._send({"jsonrpc": "2.0", "id": rid, "method": method,
                    "params": params or {}})
        while True:
            msg = self._read()
            if msg.get("id") != rid:
                continue
            if "error" in msg:
                raise RuntimeError(msg["error"])
            return msg.get("result", {})

    def _notify(self, method, params=None):
        self._send({"jsonrpc": "2.0", "method": method, "params": params or {}})

    def call(self, name, arguments=None):
        res = self._rpc("tools/call", {"name": name, "arguments": arguments or {}})
        if isinstance(res, dict) and res.get("isError"):
            raise RuntimeError(res.get("content", res))
        raw = (res.get("content") or [{}])[0].get("text", "{}")
        try:
            return json.loads(raw)
        except Exception as e:
            raise RuntimeError(f"bad tool payload: {raw}") from e

    def close(self):
        try:
            if self.proc.stdin:
                self.proc.stdin.close()
        except Exception:
            pass
        try:
            self.proc.wait(timeout=5)
        except Exception:
            self.proc.kill()


def probe(question, sess):
    """One question against the current in-memory MCP brain state."""
    return (sess.call("gen.respond", {"input": question}).get("output") or "").strip()


def persist_facts(facts, save_path, env, cwd):
    """Assert facts over --mcp-engine and persist the session to save_path.
    Returns (n_stored, errors)."""
    reqs = ['{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}',
            '{"jsonrpc":"2.0","method":"notifications/initialized"}']
    for i, f in enumerate(facts):
        reqs.append(json.dumps({"jsonrpc": "2.0", "id": 10 + i,
                                "method": "tools/call",
                                "params": {"name": "kb.assert",
                                           "arguments": {"pred": f["pred"],
                                                         "args": f["args"]}}}))
    reqs.append(json.dumps({"jsonrpc": "2.0", "id": 999, "method": "tools/call",
                            "params": {"name": "kb.save",
                                       "arguments": {"path": save_path}}}))
    proc = subprocess.run([BIN, "--mcp-engine"],
                          input="\n".join(reqs) + "\n", text=True,
                          capture_output=True, timeout=30, env=env, cwd=cwd)
    stored = proc.stdout.count('stored\\":true')
    errors = [ln for ln in proc.stdout.splitlines() if '"isError":true' in ln]
    # The fallback spill file may pick up runtime state facts; strip them so it stays
    # a pure unrouted spill instead of a mixed session dump.
    try:
        with open(save_path) as fh:
            keep = [ln for ln in fh
                    if not re.match(r"(process_pid|os_language|current_language)\(", ln)]
        with open(save_path, "w") as fh:
            fh.writelines(keep)
    except FileNotFoundError:
        pass
    return stored, errors


def session_teach(sess, facts):
    stored, errors = 0, []
    for f in facts:
        try:
            res = sess.call("kb.assert", {"pred": f["pred"], "args": f["args"]})
        except Exception as e:
            errors.append(str(e))
            continue
        if res.get("stored"):
            stored += 1
    return stored, errors


def sanitize_lesson(obj, limit=10):
    """Enforce the whitelist/shape rules. Returns (facts, dropped)."""
    facts, dropped = [], []
    for f in (obj or {}).get("facts", [])[:limit]:
        pred = str(f.get("pred", "")).strip()
        args = [str(a).strip().replace('"', "'") for a in f.get("args", [])]
        # Atoms and cues are matched against the LOWERCASED normalized turn, so
        # lowercase every arg except free-text payloads (a response template's
        # sentence, a concept definition), which keep their surface casing.
        keep_case_last = pred in ("response_template", "wiki_concept")
        args = [a if (keep_case_last and i == len(args) - 1) else a.lower()
                for i, a in enumerate(args)]
        ok_pred = re.fullmatch(r"[a-z][a-z0-9_]*", pred) is not None
        arity_ok = (pred in WHITELIST and len(args) == WHITELIST[pred]) or \
                   (pred not in WHITELIST and len(args) == 2)
        if (not ok_pred or pred in BLACKLIST or not arity_ok or
                not all(0 < len(a) <= 120 for a in args)):
            dropped.append(f)
            continue
        facts.append({"pred": pred, "args": args})
    return facts, dropped


def fact_str(f):
    return f'{f["pred"]}({", ".join(f["args"])})'


def local_diagnose(question, answer):
    _ = question
    al = answer.lower()
    if "i do not know the relation" in al or "i don't know the relation" in al:
        return {
            "failure_class": "missing_fact",
            "teachable": True,
            "lesson_mode": "fact",
            "reason": "The answer named a missing relation that can be tested with facts.",
            "next_action": "Teach the smallest grounded lesson and re-probe."
        }
    if "i can translate most of it" in al or "i can't translate" in al:
        return {
            "failure_class": "missing_fact",
            "teachable": True,
            "lesson_mode": "fact",
            "reason": "The answer named missing lexical knowledge.",
            "next_action": "Teach only the missing lexical facts and re-probe."
        }
    if "i don't understand" in al or "not sure i followed" in al or "beyond me" in al:
        return {
            "failure_class": "unknown",
            "teachable": True,
            "lesson_mode": "fact",
            "reason": "The failure did not name a specific gap, but a constrained lesson may still reveal one.",
            "next_action": "Try one small whitelisted lesson, then classify from the post-teach response."
        }
    return {
        "failure_class": "missing_fact",
        "teachable": True,
        "lesson_mode": "fact",
        "reason": "This looks like a direct knowledge gap that may be fixed by grounded facts.",
        "next_action": "Teach a small operational lesson and re-probe."
    }


def diagnose_failure(key, model, question, answer, judge_reason):
    raw = first_json(call_model(key, model, [
        {"role": "system", "content": DIAG_SYS},
        {"role": "user", "content":
            f"QUESTION: {question}\nANSWER: {answer}\nJUDGE: {judge_reason}\n"
            "Diagnose the failure for the current KB-first engine."}], 0.0, 600))
    loc = local_diagnose(question, answer)
    if not raw:
        return loc
    out = {
        "failure_class": str(raw.get("failure_class", loc["failure_class"])).strip(),
        "teachable": bool(raw.get("teachable", loc["teachable"])),
        "lesson_mode": str(raw.get("lesson_mode", loc["lesson_mode"])).strip(),
        "reason": one_line(str(raw.get("reason", loc["reason"]))),
        "next_action": one_line(str(raw.get("next_action", loc["next_action"]))),
    }
    if out["failure_class"] not in {"missing_fact", "composition_gap", "morphology_gap",
                                    "idiom_gap", "engine_gap", "unknown"}:
        out["failure_class"] = loc["failure_class"]
    if out["lesson_mode"] not in {"fact", "template", "skip"}:
        out["lesson_mode"] = loc["lesson_mode"]
    return out


def audit_lesson(question, diag, facts):
    _ = question
    _ = diag
    if not facts:
        return {"ok": False, "kind": "empty", "reason": "no facts survived sanitization"}
    for f in facts:
        if f["pred"] == "magnitude_cue" and " " in f["args"][0]:
            return {"ok": False, "kind": "engine_gap",
                    "reason": "magnitude_cue needs a single cue word; multi-word cues never match."}
    return {"ok": True, "kind": "ok", "reason": "lesson is operational enough to try"}


def norm_reply(s):
    return re.sub(r"\s+", " ", (s or "").strip().lower())


def local_reflect(question, before, lesson, after, judge_reason):
    _ = question
    _ = lesson
    moved = norm_reply(before) != norm_reply(after)
    jr = (judge_reason or "").lower()
    if not moved:
        return {
            "failure_class": "engine_gap",
            "teachable": False,
            "lesson_mode": "skip",
            "reason": "Stored facts did not materially change the answer path.",
            "next_action": "Record the failed lesson as a consumer gap before adding more facts."
        }
    if "incorrect" in jr or "wrong" in jr or "not correct" in jr:
        return {
            "failure_class": "composition_gap",
            "teachable": True,
            "lesson_mode": "fact",
            "reason": "The answer changed but remained wrong, so the lesson was partly consumed.",
            "next_action": "Retry with a smaller or differently shaped lesson and compare the response delta."
        }
    return {
        "failure_class": "missing_fact",
        "teachable": True,
        "lesson_mode": "fact",
        "reason": "The lesson changed behavior but did not satisfy the judge.",
        "next_action": "Use the changed answer as feedback for the next lesson."
    }


def reflect_attempt(key, model, question, before, lesson, after, judge_reason):
    raw = first_json(call_model(key, model, [
        {"role": "system", "content": REFLECT_SYS},
        {"role": "user", "content":
            f"QUESTION: {question}\nBEFORE: {before}\n"
            f"LESSON: {'; '.join(fact_str(f) for f in lesson)}\n"
            f"AFTER: {after}\nJUDGE: {judge_reason}\n"
            "Classify what the failed attempt proves."}], 0.0, 800))
    loc = local_reflect(question, before, lesson, after, judge_reason)
    if not raw:
        return loc
    out = {
        "failure_class": str(raw.get("failure_class", loc["failure_class"])).strip(),
        "teachable": bool(raw.get("teachable", loc["teachable"])),
        "lesson_mode": str(raw.get("lesson_mode", loc["lesson_mode"])).strip(),
        "reason": one_line(str(raw.get("reason", loc["reason"]))),
        "next_action": one_line(str(raw.get("next_action", loc["next_action"]))),
    }
    if out["failure_class"] not in {"missing_fact", "composition_gap", "morphology_gap",
                                    "idiom_gap", "engine_gap", "unknown"}:
        out["failure_class"] = loc["failure_class"]
    if out["lesson_mode"] not in {"fact", "template", "skip"}:
        out["lesson_mode"] = loc["lesson_mode"]
    if norm_reply(before) == norm_reply(after):
        out = loc
    return out


_PRINT_LOCK = threading.Lock()
_MAX_RETRIES_SKIP = 5


def load_skip_list(path):
    """Read the exact-question skip list (one question per line, strip whitespace)."""
    try:
        with open(path) as fh:
            return {ln.strip() for ln in fh if ln.strip() and not ln.startswith("#")}
    except FileNotFoundError:
        return set()


def append_skip_list(path, questions):
    """Append new already-capable questions to the skip list, deduplicated."""
    existing = load_skip_list(path)
    new = sorted(q for q in questions if q.strip() and q.strip() not in existing)
    if new:
        with open(path, "a") as fh:
            for q in new:
                fh.write(q.strip() + "\n")


def log(msg):
    with _PRINT_LOCK:
        print(msg, flush=True)


def append_ledger(path, stamp, model, result):
    row = {
        "ts": stamp,
        "model": model,
        "verdict": result.get("verdict"),
        "question": result.get("q"),
        "before": result.get("a0"),
        "after": result.get("a1"),
        "judge": result.get("reason"),
        "diagnosis": result.get("diag"),
        "lesson": result.get("lesson", []),
        "multiplied": result.get("multiplied", []),
    }
    with open(path, "a") as fh:
        fh.write(json.dumps(row, ensure_ascii=True) + "\n")


def multiply_lesson(key, model, seed_facts, n_max):
    """A verified lesson closed a question -> ask for MANY same-schema siblings and
    audit the whole batch with ONE judge call (truth + form). Returns the batch, or
    [] if the audit fails or nothing survives. The oracle-verified seed is the
    anchor; the siblings are a whitelist-constrained, batch-audited generalization
    (the deliberate precision/recall trade F. chose to accelerate growth)."""
    if n_max <= 0 or not seed_facts:
        return []
    schemas = sorted({f["pred"] for f in seed_facts})
    seed_desc = "; ".join(fact_str(f) for f in seed_facts)
    t = first_json(call_model(key, model, [
        {"role": "system", "content": MULTIPLIER_SYS},
        {"role": "user", "content":
            f"SEED (just verified): {seed_desc}\nSCHEMAS: {', '.join(schemas)}\n"
            f"Emit up to {n_max} more facts of these schema(s)."}],
        0.4, max_tokens=3500))
    sibs, _ = sanitize_lesson(t, limit=n_max * 2)
    sibs = [f for f in sibs if f["pred"] in schemas]   # stay on the seed's schema(s)
    seen, uniq = {(f["pred"], tuple(f["args"])) for f in seed_facts}, []
    for f in sibs:
        k = (f["pred"], tuple(f["args"]))
        if k in seen:
            continue
        seen.add(k)
        uniq.append(f)
    sibs = uniq[:n_max]
    if not sibs:
        return []
    j = first_json(call_model(key, model, [
        {"role": "system", "content": BATCH_JUDGE_SYS},
        {"role": "user", "content": "\n".join(fact_str(f) for f in sibs)}], 0.0)) or {}
    return sibs if int(j.get("vote", 0)) == 1 else []


def do_round(idx, given_q, args, key, skip_set=None):
    """One fully independent round on its OWN in-memory MCP brain session.
    Lessons are verified in RAM only; persistence happens later in phase two."""
    if skip_set is None:
        skip_set = set()
    t_start = time.time()
    sess = McpSession(p0_env(os.path.join(ROOT, "kb", "core", "base.p0"), "",
                             os.path.join(ROOT, "kb")), ROOT)
    try:
        if given_q is not None:
            q = given_q
        else:
            skip_list = sorted({s for s in skip_set if len(s) < 200})
            skip_block = ("\n".join(f"  - {s}" for s in skip_list[:60])
                          if skip_list else "  (none yet)")
            intv_sys = INTERVIEW_SYS.format(skip_block=skip_block)
            for _ in range(_MAX_RETRIES_SKIP):
                ability = ABILITIES[idx % len(ABILITIES)]
                q = one_line(call_model(key, args.model, [
                    {"role": "system", "content": intv_sys},
                    {"role": "user", "content": f"Ask one question probing: {ability}."}], 0.9))
                if q.strip() not in skip_set:
                    break
            else:
                q = one_line(call_model(key, args.model, [
                    {"role": "system", "content": intv_sys},
                    {"role": "user", "content": f"Ask one question probing: {ability}."}], 0.9))
        t_gen = time.time()
        if q.startswith("[model error") or q == "[empty]":
            return {"verdict": "interviewer-error", "q": q, "kept": []}

        a0 = probe(q, sess) or "(empty)"
        j0 = first_json(call_model(key, args.model, [
            {"role": "system", "content": JUDGE_SYS},
            {"role": "user", "content": f"Q: {q}\nA: {a0}"}], 0.0)) or {}
        j0_reason = j0.get("reason", "")
        t_probe = time.time()
        if int(j0.get("vote", 0)) == 1:
            elapsed = t_probe - t_start
            log(f"[{idx+1}] already   {q[:70]}  ({elapsed:.1f}s)")
            return {"verdict": "already-capable", "q": q, "a0": a0,
                    "reason": j0_reason, "kept": []}

        diag = diagnose_failure(key, args.model, q, a0, j0_reason)

        lessons, a1, v1, jr = [], a0, 0, ""
        for _ in range(args.retries + 1):
            t = first_json(call_model(key, args.model, [
                {"role": "system", "content": TEACHER_SYS},
                {"role": "user", "content":
                    f"QUESTION: {q}\nPARROT0'S FAILING REPLY: {a1}\n"
                    f"DIAGNOSIS: class={diag['failure_class']}; mode={diag['lesson_mode']}; "
                    f"reason={diag['reason']}; next={diag['next_action']}\n"
                    + (f"FACTS ALREADY TAUGHT THIS ROUND: "
                       f"{'; '.join(fact_str(f) for f in lessons)}\n" if lessons else "")
                    + "Formulate the lesson."}], 0.2, max_tokens=3000))
            if not t or "skip" in (t or {}):
                if not lessons:
                    t_skip = time.time()
                    log(f"[{idx+1}] skip (gen={t_gen-t_start:.1f}s probe={t_probe-t_gen:.1f}s "
                        f"total={t_skip-t_start:.1f}s)")
                    return {"verdict": "skipped", "q": q, "a0": a0,
                            "reason": (t or {}).get("skip", "no lesson JSON"),
                            "diag": diag, "kept": []}
                break
            facts, dropped = sanitize_lesson(t)
            if not facts:
                if not lessons:
                    t_skip = time.time()
                    log(f"[{idx+1}] skip nofacts (total={t_skip-t_start:.1f}s)")
                    return {"verdict": "skipped", "q": q, "a0": a0,
                            "reason": f"no whitelisted facts ({len(dropped)} dropped)",
                            "diag": diag, "kept": []}
                break
            audit = audit_lesson(q, diag, facts)
            if not audit["ok"]:
                if not lessons:
                    t_gap = time.time()
                    log(f"[{idx+1}] gap      {q[:70]}  (gen={t_gen-t_start:.1f}s "
                        f"probe={t_probe-t_gen:.1f}s teach={t_gap-t_probe:.1f}s)")
                    return {"verdict": "engine-gap", "q": q, "a0": a0, "reason": audit["reason"],
                            "diag": {**diag, "next_action": audit["reason"]},
                            "lesson": [fact_str(f) for f in facts], "kept": []}
                break
            stored, errors = session_teach(sess, facts)
            if stored == 0 and errors:
                return {"verdict": "skipped", "q": q, "a0": a0,
                        "reason": "; ".join(errors[:3]), "diag": diag, "kept": []}
            lessons += facts
            a1 = probe(q, sess) or "(empty)"
            j1 = first_json(call_model(key, args.model, [
                {"role": "system", "content": JUDGE_SYS},
                {"role": "user", "content": f"Q: {q}\nA: {a1}"}], 0.0)) or {}
            v1, jr = int(j1.get("vote", 0)), j1.get("reason", "")
            if v1 == 1:
                break
            diag = reflect_attempt(key, args.model, q, a0, facts, a1, jr)
            if not diag.get("teachable", True) or diag.get("lesson_mode") == "skip":
                break

        if lessons and v1 == 1:
            sibs = multiply_lesson(key, args.model, lessons, args.multiply)
            t_mult = time.time()
            log(f"[{idx+1}] TAUGHT   {q[:56]}  (+{len(lessons)} seed +{len(sibs)} x)"
                f"  gen={t_gen-t_start:.1f}s teach={t_mult-t_probe:.1f}s")
            return {"verdict": "taught", "q": q, "a0": a0, "a1": a1, "reason": jr,
                    "diag": diag,
                    "lesson": [fact_str(f) for f in lessons],
                    "multiplied": [fact_str(f) for f in sibs],
                    "kept": lessons + sibs}
        fail_diag = diag if lessons else diagnose_failure(key, args.model, q, a1, jr or j0_reason)
        verdict = "engine-gap" if fail_diag.get("failure_class") == "engine_gap" else "failed-lesson"
        t_end = time.time()
        log(f"[{idx+1}] {'gap' if verdict == 'engine-gap' else 'failed':<8} {q[:70]}"
            f"  gen={t_gen-t_start:.1f}s probe={t_probe-t_gen:.1f}s total={t_end-t_start:.1f}s")
        return {"verdict": verdict, "q": q, "a0": a0, "a1": a1, "reason": jr,
                "diag": fail_diag, "lesson": [fact_str(f) for f in lessons], "kept": []}
    finally:
        sess.close()


# ------------------------------------------------------------------ the loop
def run(args, key):
    t0 = time.time()
    os.makedirs(os.path.dirname(args.kb), exist_ok=True)
    if not os.path.exists(args.kb):
        open(args.kb, "w").close()
    if args.ledger:
        ldir = os.path.dirname(args.ledger)
        if ldir:
            os.makedirs(ldir, exist_ok=True)

    skip_set = load_skip_list(args.skip_list)

    probes = []
    if args.probes:
        with open(args.probes) as fh:
            probes = [ln.strip() for ln in fh if ln.strip() and not ln.startswith("#")]
    n = min(args.rounds, len(probes)) if probes else args.rounds

    # Parallel: rounds are independent (own in-memory MCP session); the provider
    # calls are I/O-bound, so a thread pool cuts wall-clock ~Wx. Persist is a
    # separate second phase after the oracle accepts the round.
    results = [None] * n
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as ex:
        fut = {ex.submit(do_round, i, (probes[i] if probes else None), args, key, skip_set): i
               for i in range(n)}
        for f in concurrent.futures.as_completed(fut):
            results[fut[f]] = f.result()

    # Merge every worker's kept facts into the canonical KB, deduped, in one routed save.
    seen, kept_facts = set(), []
    for r in results:
        for f in r.get("kept", []):
            k = (f["pred"], tuple(f["args"]))
            if k not in seen:
                seen.add(k)
                kept_facts.append(f)
    if kept_facts:
        persist_facts(kept_facts, args.kb,
                      p0_env(os.path.join(ROOT, "kb", "core", "base.p0"), "",
                             os.path.join(ROOT, "kb")),
                      ROOT)

    counts = {}
    diag_counts = {}
    seed_total = mult_total = 0
    for r in results:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
        d = r.get("diag") or {}
        if d.get("failure_class"):
            diag_counts[d["failure_class"]] = diag_counts.get(d["failure_class"], 0) + 1
        if r["verdict"] == "taught":
            seed_total += len(r.get("lesson", []))
            mult_total += len(r.get("multiplied", []))

    stamp = time.strftime("%Y-%m-%d %H:%M:%S")
    with open(args.out, "a") as fh:
        fh.write(f"\n## Run {stamp} — model {args.model}, {n} round(s), "
                 f"{args.workers} workers, multiply x{args.multiply}"
                 f"{' (probes)' if probes else ' (autonomous)'}\n\n")
        fh.write(f"**already {counts.get('already-capable',0)} · "
                 f"taught {counts.get('taught',0)} · failed {counts.get('failed-lesson',0)} "
                 f"· engine-gap {counts.get('engine-gap',0)} · skipped {counts.get('skipped',0)} · "
                 f"kept {len(kept_facts)} facts ({seed_total} seed + {mult_total} "
                 f"multiplied, deduped)**\n\n")
        if diag_counts:
            bits = [f"{k} {diag_counts[k]}" for k in sorted(diag_counts)]
            fh.write(f"**diagnoses: {' · '.join(bits)}**\n\n")
        for i, r in enumerate(results, 1):
            fh.write(f"### Round {i} — {r.get('verdict','?')}\n- Q: {r.get('q','')}\n")
            if "a0" in r:
                fh.write(f"- before: {r['a0']}\n")
            if r.get("diag"):
                d = r["diag"]
                fh.write(f"- diagnosis: {d.get('failure_class','?')} · "
                         f"teachable={str(bool(d.get('teachable', False))).lower()} · "
                         f"mode={d.get('lesson_mode','?')}\n")
                fh.write(f"- next: {d.get('next_action','')}\n")
            if r.get("lesson"):
                fh.write(f"- lesson: {'; '.join(r['lesson'])}\n")
            if r.get("multiplied"):
                fh.write(f"- multiplied (+{len(r['multiplied'])}): "
                         f"{'; '.join(r['multiplied'][:24])}\n")
            if "a1" in r and r.get("verdict") != "already-capable":
                fh.write(f"- after: {r['a1']}\n")
            if r.get("reason"):
                fh.write(f"- judge: {r['reason']}\n")
            fh.write("\n")
    if args.ledger:
        for r in results:
            append_ledger(args.ledger, stamp, args.model, r)

    new_skips = [r["q"] for r in results
                 if r.get("verdict") == "already-capable" and r.get("q", "").strip()]
    if new_skips:
        append_skip_list(args.skip_list, new_skips)

    log(f"\nautolearn: already {counts.get('already-capable',0)}, "
        f"taught {counts.get('taught',0)}, failed {counts.get('failed-lesson',0)}, "
        f"engine-gap {counts.get('engine-gap',0)}, skipped {counts.get('skipped',0)} | "
        f"KEPT {len(kept_facts)} facts "
        f"({seed_total} seed + {mult_total} multiplied)"
        f"  [{time.time() - t0:.1f}s total]")
    if diag_counts:
        log("diagnoses: " + ", ".join(f"{k}={diag_counts[k]}" for k in sorted(diag_counts)))
    log(f"report appended: {args.out}\nknowledge root: kb/\nfallback spill: {args.kb}")
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--rounds", type=int, default=5)
    ap.add_argument("--probes", help="file with one probe question per line "
                                     "(controlled mode; default: autonomous interviewer)")
    ap.add_argument("--model", default="minimax-m2.5",
                    help="opencode-GO slug for interviewer/judge/teacher")
    ap.add_argument("--kb", default="kb/learning/autolearn-unrouted.p0",
                    help="repo-relative fallback spill for unrouted facts")
    ap.add_argument("--retries", type=int, default=2,
                    help="extra teach attempts while the decline names new gaps")
    ap.add_argument("--workers", type=int, default=5,
                    help="parallel rounds (I/O-bound on the provider)")
    ap.add_argument("--multiply", type=int, default=20,
                    help="on a verified lesson, add up to N batch-audited same-schema "
                         "siblings (0 disables)")
    ap.add_argument("--out", default="AUTOLEARN.md")
    ap.add_argument("--ledger", default="kb/learning/autolearn-ledger.jsonl",
                    help="JSONL ledger of per-round outcomes and diagnoses")
    ap.add_argument("--skip-list", default="kb/learning/autolearn-skip.txt",
                    help="Exact-question skip list; already-capable questions appended here "
                         "so the interviewer does not ask them again")
    args = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("autolearn: OPENCODE_API_KEY not set", file=sys.stderr)
        return 1
    if not os.path.exists(BIN):
        print("autolearn: build first (make build)", file=sys.stderr)
        return 1
    return run(args, key)


if __name__ == "__main__":
    sys.exit(main())
