#!/usr/bin/env python3
"""prose-bench-coverage.py — IL BANCO COPRE TUTTO IL TESTO, O SOLO LE FRASI FACILI?

18 settembre 2026 (piano lettura-della-prosa.md §7, anti-malizia). Un banco
che si allarga solo sulle frasi che parrot0 sa gia' leggere alza il numero
senza alzare la comprensione. Questo strumento non giudica parrot0: giudica il
BANCO, prima di ogni misura.

Per ogni piolo (coppia .txt/.q) stampa:
  - la raggiungibilita' del cancello (Σ parole delle domande nel merito vs
    parole del testo, margine P0_BENCH_MARGIN, default 1,25);
  - quante domande nel merito puntano a ciascuna FRASE del testo (una domanda
    «punta» a una frase se una delle sue risposte attese vi compare): le frasi
    con ZERO domande sono il punto cieco del banco, e vanno coperte;
  - le domande la cui risposta attesa non compare in NESSUNA frase: un errore
    del banco (la risposta deve essere SCRITTA nel testo).

Uso:  scripts/prose-bench-coverage.py tests/fixtures/prose/ladder/r340.txt [...]
      scripts/prose-bench-coverage.py tests/fixtures/prose/ladder/r3*.txt
Esce 1 se un banco e' insufficiente o ha risposte assenti dal testo.
"""
import math, os, re, sys

MARGIN = float(os.environ.get('P0_BENCH_MARGIN', '1.25'))

def sentences(text):
    return [s.strip() for s in re.split(r'(?<=[.!?])\s+', text.strip()) if s.strip()]

def main(paths):
    bad = 0
    for txt in paths:
        q = txt[:-4] + '.q'
        if not os.path.exists(q):
            print(f'{txt}: nessun banco .q'); continue
        text = open(txt, encoding='utf-8').read()
        sents = sentences(text); low = [s.lower() for s in sents]
        words = len(text.split())
        rows = [l.rstrip('\n').split('\t') for l in open(q, encoding='utf-8') if l.strip()]
        merito = [r for r in rows if len(r) >= 2 and (len(r) < 4 or r[3] in ('', 'merito'))]
        qwords = sum(len(r[0].split()) for r in merito)
        need = math.ceil(words * MARGIN)
        hits = [0] * len(sents); orphan = []
        for r in merito:
            pats = [p for p in r[1].split('|') if p]
            found = False
            for i, s in enumerate(low):
                if any(re.search(p, s, re.I) for p in pats):
                    hits[i] += 1; found = True
            if not found: orphan.append(r[0])
        state = 'ok' if qwords >= need else ('INSUFFICIENTE' if qwords <= words else 'STRETTO')
        print(f'\n═══ {os.path.basename(txt)} — {words} parole, {len(sents)} frasi, {len(merito)} domande nel merito, Σ {qwords} parole (serve ≥ {need}: {state})')
        for i, s in enumerate(sents):
            mark = '⛔ 0' if hits[i] == 0 else f'{hits[i]:>4}'
            print(f'  {mark}  [{i+1:>2}] {s[:96]}')
        zero = sum(1 for h in hits if h == 0)
        if zero: print(f'  ⛔ {zero} frasi su {len(sents)} senza nessuna domanda: il banco non le misura.')
        if orphan:
            print(f'  ⛔ {len(orphan)} domande con risposta attesa ASSENTE dal testo (errore del banco):')
            for o in orphan: print(f'     - {o}')
        if state != 'ok' or orphan: bad = 1
    return bad

if __name__ == '__main__':
    if len(sys.argv) < 2: print(__doc__); sys.exit(2)
    sys.exit(main(sys.argv[1:]))
