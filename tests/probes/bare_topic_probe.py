#!/usr/bin/env python3
"""bare_topic_probe — che cosa fa un LLM quando il turno e' UN NOME E BASTA?

Sonda comportamentale, nella tradizione di ambiguity_probe.py: non si misura la
correttezza, si misura la MOSSA. Lo stimolo e' un turno reale, segnalato da F.:

    you> milano
    Ciao! Di cosa ti va di parlare?
    you> milano
    Non capisco ancora.

Due difetti in due righe, e il secondo e' peggiore del primo:

1. un nome nudo ha ricevuto un RIEMPITIVO SOCIALE (P0.4: «un muro e' un muro,
   non una battuta»);
2. lo STESSO identico turno, subito dopo, ha avuto una risposta diversa. Non e'
   una risposta peggiore: e' l'assenza di una politica.

E c'e' un terzo fatto che rende la sonda interessante: la mossa giusta parrot0
la sa gia' fare, ma per una classe strettissima.

    you> chess      ->  «Che cosa vorresti sapere su scacchi?»     <- la mossa
    you> milano     ->  «Ciao! Di cosa ti va di parlare?»          <- riempitivo
    you> mercurio   ->  «Non capisco ancora.»                      <- muro

Quindi non manca la mossa: manca la CLASSE. La domanda della sonda e' che cosa
faccia un modello forte davanti a un nome nudo — offre conoscenza? elicita?
enumera le letture quando il nome e' ambiguo? e cosa fa quando la parola non la
conosce, che e' il solo caso in cui un muro e' onesto?

L'ipotesi da falsificare: **un nome nudo non e' una domanda mancata, e' la
NOMINA DI UN TEMA** — un atto dialogico a se', e la risposta corretta e' una
funzione di quanto il sistema sa di quel tema:

    so molto      -> dico la cosa saliente e offro di continuare
    so poco       -> dico quel poco e chiedo che taglio interessa
    ambiguo       -> nomino le letture e chiedo quale
    non lo so     -> muro ONESTO, che nomina la parola

Provider/auth come le altre sonde (opencode-GO, $OPENCODE_API_KEY).
Trascritti in tests/sym/bare-topic-*.md.

Uso:
  python3 tests/probes/bare_topic_probe.py --model gpt-5.6-luna
  python3 tests/probes/bare_topic_probe.py --no-llm      # solo parrot0 (gratis)
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys, time
import urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

SYS = ("You are a concise, friendly chatbot. Answer the user's next message "
       "naturally and briefly, in the user's own language. Keep it short.")

# cat -> (stimolo, che cosa stiamo osservando)
STIMULI = [
    ("nudo_noto", "milano",
     "LA SONDA: un nome proprio nudo, di una cosa molto nota. Offre? elicita? saluta?"),
    ("nudo_noto", "chess",
     "lo stesso, in inglese e su un dominio che parrot0 conosce a fondo"),
    ("nudo_noto", "il fegato",
     "nome comune con articolo: cambia qualcosa rispetto al nome proprio?"),

    ("nudo_ambiguo", "mercurio",
     "AMBIGUO A TRE VIE (pianeta / metallo / dio): enumera le letture o ne sceglie una?"),
    ("nudo_ambiguo", "python",
     "ambiguo a due vie, e una lettura e' molto piu' probabile dell'altra"),

    ("nudo_non_nome", "rosso",
     "non e' un tema ma una PROPRIETA': la mossa e' la stessa o cambia?"),
    ("nudo_non_nome", "2+2",
     "un nome nudo che ha una lettura ovvia e una sola: la esegue senza chiedere?"),

    ("ignoto", "zorblat",
     "IL CASO ONESTO: parola che nessuno conosce. Qui il muro e' la mossa giusta —"
     " ma nomina la parola? offre di impararla?"),

    ("fatico", "ok",
     "CONTROLLO NEGATIVO: qui il riempitivo sociale e' CORRETTO, non e' un tema"),
    ("fatico", "ciao",
     "controllo negativo 2: il saluto e' un saluto"),

    ("ripetuto", "milano",
     "LA STABILITA': lo stesso turno una seconda volta. La politica e' la stessa?"),

    ("nudo_composto", "la capitale della francia",
     "un sintagma nudo che DESCRIVE un valore senza chiederlo: lo restituisce?"),
    ("nudo_composto", "guerra e pace",
     "titolo nudo: riconosce che e' un'opera, o legge la congiunzione?"),
]


def call_oracle(key: str, model: str, user: str, temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 500,
        "temperature": temperature, "messages": [
            {"role": "system", "content": SYS},
            {"role": "user", "content": user}]}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-bare-topic-probe/1.0"})
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


# Le MOSSE davanti a un nome nudo. Etichette grossolane e dichiarate: servono a
# leggere il trascritto in fretta, non a dare un voto.
GREET = ("ciao!", "ciao,", "hello", "hi ", "hey", "di cosa ti va di parlare",
         "come posso aiutarti", "how can i help", "what would you like to talk")
WALL  = ("non capisco", "don't understand", "non ho afferrato", "va un po' oltre",
         "beyond me", "non sono sicuro di aver seguito", "not sure i followed")

def moves(reply: str) -> list[str]:
    r = reply.lower().strip()
    m = []
    if r.startswith("["):
        return ["ERRORE"]
    if any(k in r for k in WALL):
        m.append("MURO")
    if any(r.startswith(g) or g in r[:60] for g in GREET):
        m.append("riempitivo-sociale")
    if "?" in r:
        m.append("elicita")
    # dice qualcosa DI SOSTANZA sul tema: una frase dichiarativa non vuota che
    # non sia solo la domanda di rimando
    body = re.sub(r"[^.!?]*\?", "", r).strip()
    if len(body) > 25:
        m.append("offre-conoscenza")
    if any(k in r for k in ("oppure", " o il ", " o la ", "intendi", "ti riferisci",
                            "do you mean", "several", "puo' riferirsi", "può riferirsi",
                            "diverse cose", "more than one")):
        m.append("enumera-letture")
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
        print("bare_topic_probe: $OPENCODE_API_KEY assente — usa --no-llm", file=sys.stderr)
        return 2

    rows = []
    for cat, text, why in STIMULI:
        p0 = ask_parrot0(a.binary, text, a.profile)
        llm = "" if a.no_llm else call_oracle(key, a.model, text, a.temperature)
        rows.append((cat, text, why, p0, llm))
        print(f"[{cat}] {text}")
        print(f"   parrot0 : {p0[:220]}   -> {'+'.join(moves(p0))}")
        if llm:
            print(f"   oracolo : {llm[:420]}   -> {'+'.join(moves(llm))}")
        print()

    out = a.out or f"tests/sym/bare-topic-{time.strftime('%Y%m%d-%H%M%S')}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w") as f:
        f.write(f"# bare_topic_probe — {a.model}\n\n")
        f.write("Che cosa fa un modello forte quando il turno e' UN NOME E BASTA.\n")
        f.write("Non si misura la correttezza: si misura la mossa.\n\n")
        for cat, text, why, p0, llm in rows:
            f.write(f"## [{cat}] `{text}`\n\n*{why}*\n\n")
            f.write(f"- **parrot0** — {'+'.join(moves(p0))}\n\n  > {p0}\n\n")
            if llm:
                f.write(f"- **oracolo** — {'+'.join(moves(llm))}\n\n  > {llm}\n\n")
    print(f"-> {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
