#!/usr/bin/env python3
"""ambiguity_probe — come un LLM MENTALIZZA una domanda di conteggio ambigua?

Sonda comportamentale, nella tradizione di symbench.py e repair_probe.py: non si
misura la correttezza, si misura la MOSSA. Lo stimolo e' un turno reale:

    you> quanti pezzi ci sono negli scacchi
    Non capisco ancora.

La domanda sembra elementare e non lo e'. Porta DUE ambiguita' sovrapposte, ed e'
per questo che vale come sonda:

1. **Registro.** Nel linguaggio tecnico scacchistico il PEDONE non e' un «pezzo»
   (i pezzi sono re, donna, torre, alfiere, cavallo); nell'uso corrente lo e'.
   Nessuno dei due usi e' l'errore dell'altro: sono due strati, e la KB di parrot0
   ha gia' il registro come DIMENSIONE (`concept_label/4`, `preferred_register`).
2. **Quantizzazione.** «Quanti» puo' chiedere: 32 (tutti, bianchi + neri), 16
   (per giocatore), 6 (tipi distinti), oppure 5 se si escludono i pedoni nel
   registro tecnico.

Quindi la risposta giusta non e' un numero: e' una mossa. Quale mossa faccia un
modello forte — sceglie e dichiara? enumera le letture? chiede? — e' cio' che
questa sonda va a vedere, per ispirare il trattamento in parrot0.

Provider/auth come symbench (opencode-GO, $OPENCODE_API_KEY).
Trascritti in tests/sym/ambiguity-*.md.

Uso:
  python3 tests/ambiguity_probe.py --model gpt-5.6-luna
  python3 tests/ambiguity_probe.py --no-llm      # solo parrot0 (gratis)
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys, time
import urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

SYS = ("You are a concise, friendly chatbot. Answer the user's next message "
       "naturally and briefly, in the user's own language. Keep it short.")

# cat -> (stimolo, che cosa stiamo osservando)
STIMULI = [
    ("ambigua_nuda", "quanti pezzi ci sono negli scacchi",
     "LA SONDA: due ambiguita' insieme (registro + quantizzazione). Sceglie? enumera? chiede?"),
    ("disambiguata", "quanti pezzi ha ogni giocatore a scacchi",
     "una sola lettura possibile: cambia la mossa quando l'ambiguita' sparisce?"),
    ("tipi", "quanti tipi di pezzi diversi ci sono negli scacchi",
     "la lettura 'tipi distinti': il pedone viene contato?"),
    ("registro", "il pedone e' un pezzo degli scacchi?",
     "IL REGISTRO NUDO: dichiara che dipende dall'uso, o sceglie un lato?"),
    ("registro_tecnico", "in notazione scacchistica tecnica il pedone e' un pezzo?",
     "col registro esplicitato, la risposta cambia?"),
    ("inglese", "how many pieces are there in chess",
     "stessa ambiguita', altra lingua: la mossa e' la stessa?"),
    ("controllo", "quanti giocatori ci sono a scacchi",
     "CONTROLLO NEGATIVO: domanda NON ambigua — non deve disambiguare nulla"),

    # --- A CHE LIVELLO vive la distinzione pezzo/pedone? (secondo giro) ---
    # L'ipotesi da falsificare: NON e' tassonomica (una proprieta' del pedone) ma
    # PRAGMATICA — vive dentro certi USI, e fuori di quelli non esiste. Se e'
    # cosi', un modello deve rispondere «si'» a «il pedone e' un pezzo?» e
    # contemporaneamente escludere i pedoni da «ho vinto un pezzo».
    ("livello_idioma", "negli scacchi cosa vuol dire vincere un pezzo",
     "L'IDIOMA: e' qui che la distinzione morde davvero? i pedoni sono esclusi?"),
    ("livello_idioma", "negli scacchi ho vinto un pedone, ho vinto un pezzo?",
     "il caso che mette in contraddizione la tassonomia e l'idioma"),
    ("livello_differenza", "che differenza c'e' tra un pezzo e un pedone negli scacchi",
     "chiesto di petto: risponde con una tassonomia o con un uso?"),
    ("livello_sottoclasse", "quanti pezzi minori ci sono negli scacchi",
     "'pezzi minori' e' un livello ANCORA piu' fine: alfieri e cavalli"),
    ("livello_conteggio_tecnico",
     "in una partita di scacchi, quanti pezzi ha ogni giocatore escludendo i pedoni",
     "il conteggio nel registro tecnico: 8"),

    # --- IL REGISTRO SULL'ETICHETTA DI UNA RELAZIONE (terzo giro) ---
    # Stesso fenomeno del pezzo/pedone su un altro asse: «mangiare» e «catturare»
    # denotano la stessa relazione in due registri, e uno dei due e' marcato come
    # SCORRETTO nell'uso formale. Le domande separano tre cose diverse:
    # capire il registro volgare / conoscerne lo statuto / quale usare rispondendo.
    ("relazione_volgare", "negli scacchi il cavallo puo' mangiare l'alfiere?",
     "CAPIRE: il registro volgare arriva a destinazione?"),
    ("relazione_specchio", "il mio pedone ha mangiato la torre, cosa succede ora",
     "LA DOMANDA CHIAVE: rispecchia il registro dell'utente o lo corregge?"),
    ("relazione_statuto", "negli scacchi si dice mangiare o catturare",
     "conosce lo STATUTO dei due termini, o li tratta come sinonimi pari?"),
    ("relazione_formale", "in notazione formale come si chiama mangiare un pezzo",
     "col registro esplicitato, sceglie il termine giusto?"),
]


def call_oracle(key: str, model: str, user: str, temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 500,
        "temperature": temperature, "messages": [
            {"role": "system", "content": SYS},
            {"role": "user", "content": user}]}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-ambiguity-probe/1.0"})
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
    return " ".join(c.replace("<think>", "").replace("</think>", "").split())[:900]


def ask_parrot0(binary: str, text: str, profile: str) -> str:
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


# Le MOSSE di fronte all'ambiguita'. Etichette grossolane e dichiarate: servono a
# leggere il trascritto in fretta, non a dare un voto.
def moves(reply: str) -> list[str]:
    r = reply.lower()
    m = []
    if r.startswith("[") or "non capisco" in r or "don't understand" in r \
       or "non ho afferrato" in r or "va un po' oltre" in r or "beyond me" in r:
        return ["MURO"]
    nums = set(re.findall(r"\b(32|16|6|5|8|2)\b", r))
    if len(nums) >= 2:
        m.append("enumera-letture")
    elif nums:
        m.append("un-numero")
    if "?" in r:
        m.append("chiede")
    if any(k in r for k in ("dipende", "depends", "a seconda", "se conti",
                            "if you count", "in senso", "tecnicamente",
                            "technically", "strettamente")):
        m.append("dichiara-la-lettura")
    if any(k in r for k in ("pedon", "pawn")):
        m.append("nomina-il-pedone")
    return m or ["altro"]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="gpt-5.6-luna")
    ap.add_argument("--temperature", type=float, default=0.3)
    ap.add_argument("--binary", default="bin/parrot0")
    ap.add_argument("--profile", default="kb/profiles/agi.p0")
    ap.add_argument("--no-llm", action="store_true")
    ap.add_argument("--out", default=None)
    a = ap.parse_args()

    key = os.environ.get("OPENCODE_API_KEY", "")
    if not a.no_llm and not key:
        print("ambiguity_probe: $OPENCODE_API_KEY assente — usa --no-llm", file=sys.stderr)
        return 2

    rows = []
    for cat, text, why in STIMULI:
        p0 = ask_parrot0(a.binary, text, a.profile)
        llm = "" if a.no_llm else call_oracle(key, a.model, text, a.temperature)
        rows.append((cat, text, why, p0, llm))
        print(f"[{cat}] {text}")
        print(f"   parrot0 : {p0[:200]}   -> {'+'.join(moves(p0))}")
        if llm:
            print(f"   oracolo : {llm[:500]}   -> {'+'.join(moves(llm))}")
        print()

    out = a.out or f"tests/sym/ambiguity-{time.strftime('%Y%m%d-%H%M%S')}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w") as f:
        f.write(f"# ambiguity_probe — {a.model}\n\n")
        f.write("Come un LLM mentalizza una domanda di conteggio con due ambiguita'\n")
        f.write("sovrapposte: il REGISTRO (il pedone e' un pezzo?) e la QUANTIZZAZIONE\n")
        f.write("(32 / 16 / 6 / 5). Non si misura la correttezza: si misura la mossa.\n\n")
        for cat, text, why, p0, llm in rows:
            f.write(f"## [{cat}] `{text}`\n\n*{why}*\n\n")
            f.write(f"- **parrot0** — {'+'.join(moves(p0))}\n\n  > {p0}\n\n")
            if llm:
                f.write(f"- **oracolo** — {'+'.join(moves(llm))}\n\n  > {llm}\n\n")
    print(f"trascritto -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
