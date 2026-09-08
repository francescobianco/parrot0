#!/usr/bin/env python3
"""Il banco di comprensione: dieci famiglie di prompt lunghi, risposta verbatim.

    tests/comprehension-probe/probe.py [f01 ... f10 | all | summary | smoke]

Per ogni famiglia avvia UN demone .p0t fresco (profilo di make chat, rete
spenta), aspetta che risponda a un health check VERO (non basta il socket: la
KB carica in qualche secondo e il client ha 3s di retry — era la causa dei 16
`rc=2` del gen505), poi manda ogni item come test isolato (cervello pulito per
prompt) e registra latenza, esito di trasporto e risposta verbatim.

Scrive results/FAM.tsv (gitignorato) e stampa un riassunto per famiglia:
  answered   la risposta c'e' e non e' un muro
  wall       la risposta e' un muro dichiarato (non capisco / non so / vuoi che impari)
  transport  il demone non ha risposto (NON e' incomprensione)
Il riassunto di tutte le famiglie va in LEDGER.md a mano, con la data e il commit.
"""
import json, os, pathlib, re, socket, subprocess, sys, time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent.parent
# PARROT0_BIN misura un binario candidato (es. `make build BIN=obj/p0next`)
# senza toccare bin/parrot0 mentre la suite lo sta usando.
BIN = pathlib.Path(os.environ.get("PARROT0_BIN") or (ROOT / "bin" / "parrot0")).resolve()
ITEMS = HERE / "items"
RES = HERE / "results"
HEALTH = ROOT / "tests" / "p0t" / "health.p0t"

WALL_RE = re.compile(
    r"^(I don't understand|I'm not sure I followed|Non capisco|Non sono sicuro|"
    r"Hmm, I don't know about|I don't know about|I haven't|I can't|I cannot|"
    r"Sorry|I do not know|I don't know|Fair enough|I understood «|"
    r"That turn joins|Quel turno unisce|I couldn't read «|Non ho saputo leggere «|"
    r"I can't hold «|Non so ancora tenere «|Want me to learn|Or teach me|Vuoi che cerchi|Oppure insegnamelo)", re.I)

def wall(reply):
    """Un muro e' un muro solo se TUTTA la risposta lo e'. Dal gen506c un turno
    composto risponde per clausole («Learned: … I couldn't read «…»»): un pezzo
    che ha imparato o risposto rende la risposta `answered`, e va letta."""
    text = reply.strip()
    if not text: return False
    pieces = re.split(r"(?<=[.!?»]) +(?=[A-Z«])", text.splitlines()[0])
    return all(WALL_RE.match(piece) for piece in pieces if piece.strip())

def summary():
    """Rilegge results/*.tsv e riclassifica con il `wall()` corrente: la tabella
    da copiare nel LEDGER, senza rilanciare il banco."""
    print("famiglia\tanswered\twall\thung\ttransport\tsecondi(max)")
    tot = {"answered": 0, "wall": 0, "hung": 0, "transport": 0}
    for f in sorted(RES.glob("f*.tsv")):
        c = {"answered": 0, "wall": 0, "hung": 0, "transport": 0}; mx = 0.0
        for ln in f.read_text(encoding="utf-8").splitlines():
            fam, stem, secs, status, reply, note = ln.split("\t")
            reply = json.loads(reply); mx = max(mx, float(secs))
            if status in ("answered", "wall"):
                gots = reply.split("\n<--TURN-->\n")
                status = "wall" if all(wall(g) for g in gots) else "answered"
            c[status] += 1
        for k in tot: tot[k] += c[k]
        print(f"{f.stem}\t{c['answered']}\t{c['wall']}\t{c['hung']}\t{c['transport']}\t{mx:.1f}")
    print(f"totale\t{tot['answered']}\t{tot['wall']}\t{tot['hung']}\t{tot['transport']}")

def run_family(fam):
    famdir = ITEMS / fam
    RES.mkdir(parents=True, exist_ok=True)
    out = RES / f"{fam}.tsv"
    env = dict(os.environ)
    env.update({"PARROT0_TOOLS": "1", "PARROT0_SESSION": "",
                "PARROT0_PROFILE": str(ROOT / "kb/profiles/agi.p0"),
                "PARROT0_TE_HARD": env.get("PARROT0_TE_HARD", "120")})
    sock = ROOT / "obj" / f"probe-{fam}.sock"
    log = ROOT / "obj" / f"probe-{fam}.log"
    for p in (sock, log):
        try: p.unlink()
        except FileNotFoundError: pass
    daemon = subprocess.Popen([str(BIN), "--test-engine", "--sock", str(sock)],
                              env=env, stdout=open(log, "w"),
                              stderr=subprocess.STDOUT, cwd=ROOT)
    rows = []
    try:
        # health check vero: il socket c'e' prima che la KB sia caricata
        deadline = time.time() + 120
        ok = False
        while time.time() < deadline and not ok:
            if daemon.poll() is not None:
                print(f"{fam}: daemon died: {log.read_text()[-600:]}", file=sys.stderr)
                return None
            if sock.exists():
                r = subprocess.run([str(BIN), "--test", str(HEALTH), "--sock", str(sock)],
                                   capture_output=True, text=True, cwd=ROOT, timeout=60)
                ok = r.returncode == 0 and "ok" in r.stdout
            if not ok: time.sleep(0.3)
        if not ok:
            print(f"{fam}: daemon never answered the health check", file=sys.stderr)
            return None
        for f in sorted(famdir.glob("i*.p0t")):
            t0 = time.time()
            try:
                r = subprocess.run([str(BIN), "--test", str(f), "--sock", str(sock)],
                                   capture_output=True, text=True, cwd=ROOT, timeout=300)
            except subprocess.TimeoutExpired:
                rows.append((f.stem, round(time.time() - t0, 2), "transport", "", "client timeout"))
                continue
            secs = round(time.time() - t0, 2)
            text = r.stdout + "\n" + r.stderr
            gots, lines, i = [], text.splitlines(), 0
            while i < len(lines):
                m = re.match(r"^\s*got:\s*(.*)$", lines[i])
                if m:
                    rep, j = [m.group(1)], i + 1
                    while j < len(lines) and not re.match(r"^\s*(FAIL|ok|expected)", lines[j]):
                        if lines[j].strip(): rep.append(lines[j])
                        j += 1
                    gots.append("\n".join(rep).strip()); i = j
                else:
                    i += 1
            hung = [t.strip() for t in lines if "turn HUNG" in t]
            if "cannot reach engine" in text or (not gots and not hung):
                status, reply, note = "transport", "", text.strip()[-200:]
            elif hung:
                status, reply, note = "hung", "", hung[0]
            else:
                reply = "\n<--TURN-->\n".join(gots)
                status = "wall" if all(wall(g) for g in gots) else "answered"
                note = ""
            rows.append((f.stem, secs, status, reply, note))
            print(f"{fam}/{f.stem}\t{secs}s\t{status}\t" + reply.replace("\n", " ")[:140], flush=True)
            if daemon.poll() is not None:
                print(f"{fam}: daemon stopped after {f.stem} (watchdog); restarting", file=sys.stderr)
                daemon = subprocess.Popen([str(BIN), "--test-engine", "--sock", str(sock)],
                                          env=env, stdout=open(log, "a"),
                                          stderr=subprocess.STDOUT, cwd=ROOT)
                time.sleep(3)
    finally:
        daemon.terminate()
        try: daemon.wait(timeout=5)
        except subprocess.TimeoutExpired: daemon.kill()
        try: sock.unlink()
        except FileNotFoundError: pass
    with open(out, "w", encoding="utf-8") as fh:
        for stem, secs, status, reply, note in rows:
            fh.write(f"{fam}\t{stem}\t{secs}\t{status}\t{json.dumps(reply, ensure_ascii=False)}\t{json.dumps(note, ensure_ascii=False)}\n")
    n = len(rows)
    c = {k: sum(1 for r in rows if r[2] == k) for k in ("answered", "wall", "hung", "transport")}
    print(f"== {fam}: {n} item — answered {c['answered']}, wall {c['wall']}, hung {c['hung']}, transport {c['transport']}", flush=True)
    return c

def smoke():
    """Il banco PICCOLO: gli item elencati in smoke.txt (fam/item per riga,
    default: il primo di ogni famiglia), un demone solo, un minuto. E' la
    prova da lanciare a OGNI modifica, prima della suite: intercetta un prompt
    disfunzionale (eco, risposta di registro, fatto falso) in secondi. Stampa
    la risposta verbatim: va letta, non contata."""
    lst = HERE / "smoke.txt"
    picks = [ln.strip() for ln in lst.read_text().splitlines() if ln.strip() and not ln.startswith("#")] if lst.exists() else []
    if not picks:
        picks = [f"{d.name}/i01" for d in sorted(ITEMS.iterdir()) if d.is_dir()]
    files = [ITEMS / pk.split("/")[0] / (pk.split("/")[1] + ".p0t") for pk in picks]
    env = dict(os.environ)
    env.update({"PARROT0_TOOLS": "1", "PARROT0_SESSION": "",
                "PARROT0_PROFILE": str(ROOT / "kb/profiles/agi.p0"),
                "PARROT0_TE_HARD": env.get("PARROT0_TE_HARD", "60")})
    sock = ROOT / "obj" / "probe-smoke.sock"
    try: sock.unlink()
    except FileNotFoundError: pass
    daemon = subprocess.Popen([str(BIN), "--test-engine", "--sock", str(sock)], env=env,
                              stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT, cwd=ROOT)
    try:
        deadline = time.time() + 120; ok = False
        while time.time() < deadline and not ok:
            if daemon.poll() is not None: print("smoke: daemon died", file=sys.stderr); return
            if sock.exists():
                r = subprocess.run([str(BIN), "--test", str(HEALTH), "--sock", str(sock)],
                                   capture_output=True, text=True, cwd=ROOT, timeout=60)
                ok = r.returncode == 0 and "ok" in r.stdout
            if not ok: time.sleep(0.3)
        last = None
        for f in files:
            t0 = time.time()
            r = subprocess.run([str(BIN), "--test", str(f), "--sock", str(sock)],
                               capture_output=True, text=True, cwd=ROOT, timeout=300)
            text = r.stdout + "\n" + r.stderr
            gots = [m.strip() for m in re.findall(r"^\s*got:\s*(.*)$", text, re.M)]
            reply = " | ".join(gots)
            flag = ""
            if not gots: flag = "  ⚠ TRANSPORT/HUNG"
            elif last is not None and reply == last: flag = "  ⚠ ECO del turno prima"
            elif not wall(reply) and re.search(r"I don't have any of my own|Fair enough|What number should I use", reply): flag = "  ⚠ risposta di registro"
            last = reply
            print(f"{f.parent.name}/{f.stem}  {time.time()-t0:.1f}s  {'wall' if wall(reply) else 'answered'}{flag}\n    {reply[:400]}", flush=True)
    finally:
        daemon.terminate()
        try: daemon.wait(timeout=5)
        except subprocess.TimeoutExpired: daemon.kill()
        try: sock.unlink()
        except FileNotFoundError: pass

def main():
    fams = sys.argv[1:] or ["all"]
    if fams == ["summary"]:
        summary(); return
    if fams == ["smoke"]:
        smoke(); return
    if fams == ["all"]:
        fams = sorted(p.name for p in ITEMS.iterdir() if p.is_dir())
    total = {}
    for fam in fams:
        c = run_family(fam)
        if c: total[fam] = c
    if len(total) > 1:
        print("\n== totale")
        for fam, c in total.items():
            print(f"{fam}\tanswered {c['answered']}\twall {c['wall']}\thung {c['hung']}\ttransport {c['transport']}")

if __name__ == "__main__":
    main()
