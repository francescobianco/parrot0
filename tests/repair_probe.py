#!/usr/bin/env python3
"""repair_probe — che cosa fa un LLM quando la PAROLA della domanda e' rotta?

Sonda comportamentale, nella tradizione di symbench.py: non si misura la
correttezza, si misura **come reagisce**. Lo stimolo che l'ha originata e' un
turno reale di parrot0:

    you> come si scrive correttamente pamino
    Non capisco ancora.

La domanda e' ben formata, l'intento e' chiarissimo (ortografia), e l'unica cosa
ignota e' la stringa su cui verte. Un LLM non mura: propone ipotesi e chiede.
Questa sonda serve a vedere *quali mosse* fa, perche' quelle mosse sono la
specifica del comportamento che manca a parrot0 — non un frasario da copiare.

Gli stimoli coprono la CLASSE, non il caso: parola rotta ambigua, parola rotta
ovvia, parola gia' corretta (il controllo negativo: non deve "riparare" cio' che
non e' rotto), non-parola senza vicini (non deve inventare), e la stessa forma in
inglese (l'asimmetria fra lingue di question-emergence.md §11.3).

Provider/auth come symbench (opencode-GO, $OPENCODE_API_KEY). Trascritti in
tests/sym/repair-*.md.

Uso:
  .venv/bin/python tests/repair_probe.py --model kimi-k2.6
  .venv/bin/python tests/repair_probe.py --no-llm     # solo parrot0 (gratis)
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys, time
import urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

# Stessa cornice per entrambi i rispondenti: un chatbot conciso. Nessun
# suggerimento che esista una risposta "giusta" — vogliamo la reazione libera.
SYS = ("You are a concise, friendly chatbot. Answer the user's next message "
       "naturally and briefly, in the user's own language. Keep it short.")

# cat -> (stimolo, che cosa stiamo osservando)
STIMULI = [
    ("rotta_ambigua",  "come si scrive correttamente pamino",
     "parola rotta con PIU' vicini plausibili — enumera o sceglie?"),
    ("rotta_ambigua",  "come si scrive correttamente pocker",
     "rotta, un vicino ovvio, ma dentro un dominio che parrot0 conosce"),
    ("rotta_ovvia",    "come si scrive correttamente ambiete",
     "un solo vicino plausibile — ripara secco o chiede comunque?"),
    ("rotta_ovvia",    "come si scrive correttamente perche",
     "manca solo l'accento: la deformazione piu' comune dell'italiano scritto"),
    ("gia_corretta",   "come si scrive correttamente pesce",
     "CONTROLLO NEGATIVO: non deve riparare cio' che non e' rotto"),
    ("senza_vicini",   "come si scrive correttamente zqxvbn",
     "CONTROLLO NEGATIVO: nessun vicino — deve dichiarare, non inventare"),
    ("inglese",        "how do you correctly spell recieve",
     "stessa forma, altra lingua: la mossa cambia?"),
    ("nuda",           "pamino",
     "la stessa stringa SENZA la domanda: quanto della reazione viene dalla forma?"),
]


def call_oracle(key: str, model: str, user: str, temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 400,
        "temperature": temperature, "messages": [
            {"role": "system", "content": SYS},
            {"role": "user", "content": user}]}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-repair-probe/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=120) as r:
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
    return " ".join(c.replace("<think>", "").replace("</think>", "").split())[:600]


def ask_parrot0(binary: str, text: str, profile: str) -> str:
    """Un turno isolato attraverso il motore MCP (lo stesso cervello della chat)."""
    env = dict(os.environ)
    env["PARROT0_PROFILE"] = profile
    lines = [json.dumps({"jsonrpc": "2.0", "id": 0, "method": "initialize", "params": {}}),
             json.dumps({"jsonrpc": "2.0", "id": 1, "method": "tools/call",
                         "params": {"name": "gen.respond", "arguments": {"input": text}}})]
    try:
        p = subprocess.run([binary, "--mcp-engine"], input="\n".join(lines) + "\n",
                           capture_output=True, text=True, timeout=300, env=env)
    except Exception as e:
        return f"[parrot0 error {e}]"
    for ln in p.stdout.splitlines():
        try:
            j = json.loads(ln)
        except Exception:
            continue
        if j.get("id") != 1:
            continue
        r = j.get("result", {})
        txt = "".join(c.get("text", "") for c in r.get("content", []))
        try:
            return json.loads(txt).get("output", txt)
        except Exception:
            return txt
    return "[no reply]"


# --- classificazione della MOSSA, non della risposta --------------------------
# Etichette grossolane e dichiarate: servono a leggere il trascritto in fretta,
# non a dare un voto. Chi legge deve poter smentirle guardando il testo.
def moves(reply: str) -> list[str]:
    r = reply.lower()
    m = []
    if r.startswith("[") or "non capisco" in r or "don't understand" in r \
       or "non ho afferrato" in r or "va un po' oltre" in r or "beyond me" in r:
        m.append("MURO")
        return m
    if "?" in r:
        m.append("chiede")
    if any(k in r for k in ("forse", "intendi", "intendevi", "did you mean",
                            "perhaps", "maybe", "probabilmente", "suppongo")):
        m.append("ipotizza")
    if any(k in r for k in (" o ", " oppure ", " or ", ",")) and "ipotizza" in m:
        m.append("enumera")
    if any(k in r for k in ("non esiste", "non e' una parola", "non è una parola",
                            "isn't a word", "not a word", "non risulta",
                            "nessuna parola")):
        m.append("dichiara-ignoto")
    if any(k in r for k in ("si scrive", "corretta è", "corretta e'",
                            "is spelled", "correct spelling", "si scrivono")):
        m.append("ripara")
    return m or ["altro"]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="kimi-k2.6")
    ap.add_argument("--temperature", type=float, default=0.3)
    ap.add_argument("--binary", default="bin/parrot0")
    ap.add_argument("--profile", default="kb/profiles/agi.p0")
    ap.add_argument("--no-llm", action="store_true")
    ap.add_argument("--out", default=None)
    a = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY", "")
    if not a.no_llm and not key:
        print("repair_probe: $OPENCODE_API_KEY assente — usa --no-llm", file=sys.stderr)
        return 2

    rows = []
    for cat, text, why in STIMULI:
        p0 = ask_parrot0(a.binary, text, a.profile)
        llm = "" if a.no_llm else call_oracle(key, a.model, text, a.temperature)
        rows.append((cat, text, why, p0, llm))
        print(f"[{cat}] {text}")
        print(f"   parrot0 : {p0}   -> {'+'.join(moves(p0))}")
        if llm:
            print(f"   oracolo : {llm[:180]}   -> {'+'.join(moves(llm))}")
        print()

    walls_p0 = sum(1 for r in rows if "MURO" in moves(r[3]))
    walls_or = sum(1 for r in rows if r[4] and "MURO" in moves(r[4]))
    print(f"muri — parrot0 {walls_p0}/{len(rows)}"
          + (f", oracolo {walls_or}/{len(rows)}" if not a.no_llm else ""))

    out = a.out or f"tests/sym/repair-{time.strftime('%Y%m%d-%H%M%S')}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w") as f:
        f.write(f"# repair_probe — {a.model}\n\n")
        f.write("Che mossa fa un LLM quando la PAROLA della domanda e' rotta.\n")
        f.write("Non si misura la correttezza: si misura la mossa.\n\n")
        for cat, text, why, p0, llm in rows:
            f.write(f"## [{cat}] `{text}`\n\n*{why}*\n\n")
            f.write(f"- **parrot0** — {'+'.join(moves(p0))}\n\n  > {p0}\n\n")
            if llm:
                f.write(f"- **oracolo** — {'+'.join(moves(llm))}\n\n  > {llm}\n\n")
        f.write(f"\nmuri: parrot0 {walls_p0}/{len(rows)}")
        if not a.no_llm:
            f.write(f", oracolo {walls_or}/{len(rows)}")
        f.write("\n")
    print(f"trascritto -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
