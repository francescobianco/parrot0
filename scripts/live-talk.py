#!/usr/bin/env python3
"""live-talk: un modello locale (LM Studio) e parrot0 parlano fra loro.

Guidato da scripts/live-talk.sh. Il modello conduce, parrot0 risponde; ogni
riga va nel transcript appena esiste, cosi' chi guarda (`live-talk.sh watch`)
segue lo scambio dal vivo. Nessun oracolo remoto: il modello gira in LM Studio
(`lms server start`, `lms load MODELLO`), endpoint OpenAI-compatibile.
"""
import argparse
import json
import os
import subprocess
import time
import urllib.request

SYS = {
    "en": ("You are chatting with another assistant. Talk naturally, like a curious "
           "person: ask questions, react to what it says, change topic sometimes. "
           "One or two short sentences per turn."),
    "it": ("Stai chiacchierando con un altro assistente. Parla in modo naturale, come "
           "una persona curiosa: fai domande, reagisci a quello che dice, ogni tanto "
           "cambia argomento. Una o due frasi brevi per turno. Scrivi in italiano."),
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--model", default="liquid/lfm2.5-1.2b")
    ap.add_argument("--turns", type=int, default=12)
    ap.add_argument("--lang", default="en", choices=sorted(SYS))
    ap.add_argument("--opener", default=None, help="prima battuta del modello (default: la sceglie lui)")
    ap.add_argument("--endpoint", default="http://localhost:1234/v1/chat/completions")
    ap.add_argument("--log", required=True)
    ap.add_argument("--wait", type=float, default=30, help="secondi oltre i quali un turno di parrot0 e' BLOCCATO")
    a = ap.parse_args()

    log = open(a.log, "a", buffering=1)
    note = lambda s: log.write(s + "\n")

    env = dict(os.environ, PARROT0_PROFILE="kb/profiles/agi.p0", PARROT0_LANG=a.lang, PARROT0_SESSION="")
    p = subprocess.Popen(["./bin/parrot0"], env=env, stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    buf = b""

    def reply(limit):
        nonlocal buf
        end = time.time() + limit
        os.set_blocking(p.stdout.fileno(), False)
        while b">>> " not in buf:
            if time.time() > end:
                return None
            try:
                chunk = os.read(p.stdout.fileno(), 65536)
            except BlockingIOError:
                time.sleep(0.05)
                continue
            if not chunk:
                return None
            buf += chunk
        r, _, buf = buf.partition(b">>> ")
        return r.decode(errors="replace").strip().replace("\n", " ")

    note(f"# parrot0 in avvio (KB viva completa)…")
    reply(120)
    note(f"# pronto — modello {a.model}, {a.turns} scambi, lingua {a.lang}")

    hist = [{"role": "system", "content": SYS[a.lang]}]
    hist.append({"role": "user", "content": "Ciao." if a.lang == "it" else "Hi."})
    try:
        for i in range(a.turns):
            if i == 0 and a.opener:
                said = a.opener
            else:
                body = json.dumps({"model": a.model, "temperature": 0.7, "max_tokens": 120,
                                   "messages": hist}).encode()
                req = urllib.request.Request(a.endpoint, data=body,
                                             headers={"Content-Type": "application/json"})
                t0 = time.time()
                d = json.loads(urllib.request.urlopen(req, timeout=300).read())
                said = d["choices"][0]["message"]["content"].strip().replace("\n", " ")
                note(f"# modello: {time.time() - t0:.1f} s")
            hist.append({"role": "assistant", "content": said})
            note(f"M> {said}")
            t0 = time.time()
            p.stdin.write((said + "\n").encode())
            p.stdin.flush()
            ans = reply(a.wait)
            dt = time.time() - t0
            if ans is None:
                note(f"# BLOCCATO: parrot0 non ha risposto in {a.wait:.0f} s — fine della conversazione")
                break
            note(f"P< {ans}   [{dt:.1f} s]")
            hist.append({"role": "user", "content": ans or "..."})
    finally:
        note("# fine")
        p.terminate()


if __name__ == "__main__":
    main()
