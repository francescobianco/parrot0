#!/usr/bin/env python3
"""prose-diff.py — CHE COSA E' CAMBIATO FRA DUE REFERTI DEL BANCO DELLA PROSA.

18 settembre 2026 (piano lettura-della-prosa.md §7). Il confronto a mano fra
due referti (`P0_PROBE_STEP2=1 scripts/prose-probe.sh …` salvati in
docs/labs/…) e' il passo che si ripete a OGNI giro e a ogni piolo: quali
domande sono passate, quali si sono perse, quali hanno cambiato risposta pur
restando dello stesso esito. Farlo a occhio costa minuti e sbaglia; qui e'
un comando.

Uso:  scripts/prose-diff.py PRIMA.txt DOPO.txt
Esce 0 sempre: e' un referto, non un cricchetto. Le righe del referto sono
quelle stampate dal banco (domanda troncata a 32 colonne, specie, esito ✓/·/già,
risposta): la chiave e' la domanda troncata + la specie.
"""
import re, sys

LINE = re.compile(r'^\s{2}(?P<q>.{32})\s+(?P<k>merito|meta|struttura)\s+(?P<v>✓|·|già)\s+(?P<a>.*)$')

def load(path):
    rows = {}; seen = {}
    for line in open(path, encoding='utf-8', errors='replace'):
        m = LINE.match(line.rstrip('\n'))
        if not m: continue
        # due domande con lo stesso prefisso di 32 colonne («what was the
        # economic value of c…» 2020 e 2014) sono chiavi diverse: si numera
        # l'occorrenza, nell'ordine del banco, che e' lo stesso nei due referti.
        base = (m['q'].rstrip(), m['k'])
        seen[base] = seen.get(base, 0) + 1
        key = (base[0] if seen[base] == 1 else f"{base[0]}#{seen[base]}", base[1])
        rows[key] = (m['v'], m['a'].strip())
    tot = {}
    for line in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'^\s+(merito|meta|struttura)\s+(\d+)/(\d+)', line)
        if m: tot[m[1]] = (int(m[2]), int(m[3]))
    gate = re.search(r'CANCELLO: (\d+) parole', open(path, encoding='utf-8', errors='replace').read())
    return rows, tot, (int(gate[1]) if gate else None)

def main():
    if len(sys.argv) != 3:
        print(__doc__); sys.exit(2)
    a, ta, ga = load(sys.argv[1]); b, tb, gb = load(sys.argv[2])
    gained = [k for k in b if k in a and a[k][0] != '✓' and b[k][0] == '✓']
    lost   = [k for k in b if k in a and a[k][0] == '✓' and b[k][0] != '✓']
    changed = [k for k in b if k in a and a[k][0] == b[k][0] and a[k][1] != b[k][1]]
    new = [k for k in b if k not in a]; gone = [k for k in a if k not in b]
    def show(title, keys, both=True):
        if not keys: return
        print(f'\n{title} ({len(keys)})')
        for q, k in keys:
            if both: print(f'  {q:<32} {k:<9} {a[(q,k)][0]} «{a[(q,k)][1][:60]}»  →  {b[(q,k)][0]} «{b[(q,k)][1][:60]}»')
            else:
                src = b if (q, k) in b else a
                print(f'  {q:<32} {k:<9} {src[(q,k)][0]} «{src[(q,k)][1][:70]}»')
    for k in ('merito', 'meta', 'struttura'):
        if k in ta or k in tb:
            print(f'{k:<10} {ta.get(k,("-","-"))[0]}/{ta.get(k,("-","-"))[1]}  →  {tb.get(k,("-","-"))[0]}/{tb.get(k,("-","-"))[1]}')
    print(f'cancello   {ga}  →  {gb}')
    show('✅ GUADAGNATE', gained); show('⛔ PERSE', lost); show('~ stesso esito, altra risposta', changed)
    show('+ solo nel secondo referto', new, both=False); show('− solo nel primo referto', gone, both=False)
    # le risposte non-muro sbagliate del secondo referto: la specie peggiore
    bad = [k for k in b if b[k][0] == '·' and not re.search(r"don.t know|don.t understand|not sure|can.t show|I have no|looked up|couldn.t|beyond me|Ask me whether|non so|non capisco|non ho|non riesco", b[k][1], re.I)]
    show('⚠ nel secondo referto: NON muri e NON giuste (da rivedere a mano: possibili bugie)', bad, both=False)

if __name__ == '__main__':
    main()
