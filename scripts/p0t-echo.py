#!/usr/bin/env python3
"""Eco verbatim delle risposte di un file .p0t, in un motore fresco via MCP.

Il runner (`parrot0 --test FILE.p0t`) verifica le attese ma non mostra le
risposte; questo driver le stampa con il tempo del turno, per LEGGERE che cosa
parrot0 dice davvero (mantra #9). Legge le righe `> testo` come turni e le
righe `? pred a b _` come interrogazioni dirette della KB (`_` = variabile
libera). Non verifica nulla: non certifica un banco.

    python3 scripts/p0t-echo.py tests/p0t/reasoning/taught_decision.p0t
    LANGX=it python3 scripts/p0t-echo.py FILE.p0t     # lingua del discorso

Scrive run-<file>-<ora>.txt e trace-<file>-<ora>.log in logs/p0t-echo/ (non nella
radice del repository: F., 19 settembre 2026).
"""
import importlib.util, os, pathlib, subprocess, sys, time

root = pathlib.Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location('sq', root / 'scripts/self-questions.py')
mod = importlib.util.module_from_spec(spec); spec.loader.exec_module(mod)
eng = mod.Engine.__new__(mod.Engine); eng.n = 0
src = pathlib.Path(sys.argv[1])
tag = src.stem + '-' + time.strftime('%H%M%S')
logdir = root / 'logs' / 'p0t-echo'
logdir.mkdir(parents=True, exist_ok=True)
out = logdir / ('run-' + tag + '.txt')
log = open(logdir / ('trace-' + tag + '.log'), 'w')
env = dict(os.environ, PARROT0_SESSION='', PARROT0_PROFILE='kb/profiles/agi.p0',
           PARROT0_WIKI_FETCH='0', PARROT0_TOOLS='1', PARROT0_LANG=os.environ.get('LANGX', 'en'))
eng.p = subprocess.Popen([str(root / 'bin/parrot0'), '--mcp-engine'], cwd=root, env=env,
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=log, text=True, bufsize=1)
lines = []
try:
    eng.rpc('initialize', {})
    for line in src.read_text().splitlines():
        if line.startswith('> '):
            q = line[2:]; t = time.monotonic(); r = eng.respond(q, 30); dt = round(time.monotonic() - t, 2)
            l = f'> {q}\n  = {r}   [{dt}s]'
        elif line.startswith('? '):
            pred, _, args = line[2:].partition(' ')
            a = [None if x == '_' else x for x in args.split()]
            l = f'? {pred} {a}\n  = {eng.match(pred, a)}'
        else:
            continue
        print(l, flush=True); lines.append(l)
finally:
    try: eng.close()
    except Exception: pass
    log.close(); out.write_text('\n'.join(lines) + '\n')
