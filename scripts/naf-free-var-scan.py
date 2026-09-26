#!/usr/bin/env python3
# naf-free-var-scan.py — trova in kb/ le negazioni `naf(...)` con una variabile che
# non compare altrove nella clausola: il solver le rifiuta (floundering) e la
# regola fallisce sempre, in silenzio. Causa di quattro rotture il 26 settembre
# 2026 (train-the-learning-process.md). Uso: scripts/naf-free-var-scan.py
import re, glob, sys
# Clausole .p0: testa :- corpo. (anche su piu' righe). Una variabile dentro naf(...)
# che non compare in NESSUN'altra parte della clausola e' libera: naf fallisce sempre.
def clauses(text):
    buf=[]; out=[]; line0=0
    for i,line in enumerate(text.split('\n'),1):
        l=re.sub(r'%.*$','',line) if not re.search(r'"[^"]*%[^"]*"',line) else line
        if not buf and not l.strip(): continue
        if not buf: line0=i
        buf.append(l)
        if re.search(r'\.\s*$', l) and ':-' in ''.join(buf) or (re.search(r'\.\s*$', l) and not buf[0].strip().startswith('%') and ':-' not in ''.join(buf)):
            out.append((line0,' '.join(buf))); buf=[]
    return out
def naf_terms(body):
    res=[]; i=0
    while True:
        j=body.find('naf(',i)
        if j<0: break
        d=0; k=j+3
        for k in range(j+3,len(body)):
            if body[k]=='(': d+=1
            elif body[k]==')':
                d-=1
                if d==0: break
        res.append((j,k,body[j:k+1])); i=k+1
    return res
hits=[]
for f in glob.glob('kb/**/*.p0', recursive=True):
    try: text=open(f,encoding='utf-8',errors='replace').read()
    except: continue
    for ln,c in clauses(text):
        if ':-' not in c or 'naf(' not in c: continue
        for (a,b,t) in naf_terms(c):
            rest=c[:a]+c[b+1:]
            vs=set(re.findall(r'\$[A-Za-z_][A-Za-z0-9_]*', t))
            free=[v for v in vs if not re.search(re.escape(v)+r'\b', rest)]
            if free: hits.append((f,ln,t.strip()[:90],free))
for h in hits: print(f"{h[0]}:{h[1]}  {h[2]}  free={h[3]}")
print(len(hits), file=sys.stderr)
