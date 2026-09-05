#!/usr/bin/env python3
"""Probe one family of long prompts against a fresh parrot0 daemon.

Boots one test-engine daemon (make-chat profile, deterministic/offline), then
runs every generated item .p0t as an isolated test (fresh brain per prompt),
recording latency and the verbatim reply parrot0 produced.

Usage: probe_one.py FAM   (FAM in f01..f10)
Results appended to var/probe/results/FAM.tsv  (tab separated, reply JSON-escaped)
"""
import json, os, pathlib, re, socket, subprocess, sys, time

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent  # repo root
BIN = ROOT / "bin" / "parrot0"
ITEMS = ROOT / "var" / "probe" / "items"
RES = ROOT / "var" / "probe" / "results"

FAM = sys.argv[1] if len(sys.argv) > 1 else "f01"
famdir = ITEMS / FAM
RES.mkdir(parents=True, exist_ok=True)
out = RES / f"{FAM}.tsv"

ENV = dict(os.environ)
ENV.update({
    "PARROT0_TOOLS": "1",
    "PARROT0_SESSION": "",
    "PARROT0_PROFILE": str(ROOT / "kb/profiles/agi.p0"),
    "PARROT0_WIKI_FETCH": "0",
})
SOCK = ROOT / "obj" / f"probe-{FAM}.sock"
LOG = ROOT / "obj" / f"probe-{FAM}.log"
for p in (SOCK, LOG):
    try: p.unlink()
    except FileNotFoundError: pass

def run(cmd, timeout=120):
    return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, cwd=ROOT)

# boot the daemon
daemon = subprocess.Popen(
    [str(BIN), "--test-engine", "--sock", str(SOCK)],
    env=ENV, stdout=open(LOG, "w"), stderr=subprocess.STDOUT, cwd=ROOT)
try:
    deadline = time.time() + 60
    while time.time() < deadline:
        if SOCK.exists() and os.path.exists(str(SOCK)):
            try:
                s = socket.socket(socket.AF_UNIX)
                s.connect(str(SOCK)); s.close(); break
            except OSError:
                pass
        if daemon.poll() is not None:
            print(f"daemon died: {LOG.read_text()[-800:]}", file=sys.stderr)
            sys.exit(2)
        time.sleep(0.1)
    else:
        print("daemon did not come up", file=sys.stderr); sys.exit(2)

    files = sorted(famdir.glob("i*.p0t"))
    rows = []
    for f in files:
        t0 = time.time()
        try:
            r = run([str(BIN), "--test", str(f), "--sock", str(SOCK)])
        except subprocess.TimeoutExpired:
            rows.append((f.stem, round(time.time() - t0, 2), "", "TIMEOUT"))
            continue
        secs = round(time.time() - t0, 2)
        text = r.stdout + "\n" + r.stderr
        # collect every 'got:' block; reply may span lines until the FAIL summary
        gots = []
        lines = text.splitlines()
        i = 0
        while i < len(lines):
            m = re.match(r"^\s*got:\s*(.*)$", lines[i])
            if m:
                rep = [m.group(1)]
                j = i + 1
                while j < len(lines) and not re.match(r"^\s*(FAIL|Command exited|expected:)", lines[j]):
                    if lines[j].strip():
                        rep.append(lines[j])
                    j += 1
                gots.append("\n".join(rep).strip())
                i = j
            else:
                i += 1
        toks = [t.strip() for t in text.splitlines() if "turn took" in t]
        note = "; ".join(toks)[:300]
        reply = "\n<--TURN-->\n".join(gots) if gots else ("(no reply text; rc=%d %s)" % (r.returncode, note))
        rows.append((f.stem, secs, str(r.returncode), reply))
        print(f"{f.stem}\t{secs}s\trc={r.returncode}\t" + reply.replace("\n", " ")[:160], flush=True)

    with open(out, "w", encoding="utf-8") as fh:
        for stem, secs, rc, reply in rows:
            fh.write(f"{FAM}\t{stem}\t{secs}\t{rc}\t{json.dumps(reply, ensure_ascii=False)}\n")
    print(f"wrote {out}")
finally:
    daemon.terminate()
    try: daemon.wait(timeout=5)
    except subprocess.TimeoutExpired: daemon.kill()
