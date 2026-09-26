#!/usr/bin/env python3
"""coherence-bench.py — il banco di coerenza di L4 (docs/plans/l4-upgrade.md, L4-0).

Misura, per ogni lezione di una batteria, le cinque cose della definizione di
coerenza (l4-upgrade.md §2):

  prima        la regola e' gia' operante? (esempio diverso da quello di dopo)
  dopo         la stessa regola su un esempio nuovo, dopo la lezione
  tenuto fuori un caso mai nominato nella lezione
  contrasto    una domanda vicina: la risposta NON deve cambiare con la lezione
  riavvio      dopo e tenuto fuori rifatti dopo /save + /restore (dal disco)

e in piu' quanti fatti la lezione ha scritto nella KB (il `/save` dopo ogni
lezione dice quante clausole ha instradato): una lezione su una regola gia'
operante che scrive fatti e' la rottura R1/R2; una lezione che non cambia niente
e scrive fatti e' un fatto spazzatura.

Un solo processo parrot0, la KB viva COMPLETA ma in una COPIA (sandbox): il /save
del banco non tocca l'albero del repo. Il diff fra la copia e kb/ mostra
esattamente che cosa le lezioni hanno scritto e dove.

  scripts/coherence-bench.py tests/coherence/grammar.json [--only G2,G3] [--turn-timeout 10]

Esce un referto per lezione, un coefficiente di coerenza, e il jsonl in logs/.
Il giudizio delle risposte e' dello strumento (sonde e atteso), non di parrot0:
e' uno strumento di progetto, come le sonde dei piani.
"""
import argparse, json, os, re, shutil, subprocess, sys, time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

WALLS = ["don't know", "do not know", "cannot settle", "not proved", "don't understand",
         "not sure i followed", "i looked up", "want me to learn", "nothing i hold",
         "i didn't keep", "no fact", "can't show", "couldn't read", "can't hold"]


def norm(s):
    return re.sub(r"[_\-\s]+", " ", s.lower()).strip()


def first(r):
    """la prima frase, normalizzata: una ripetizione accorcia la coda di cortesia
    («Want me to learn about it?») senza cambiare la risposta"""
    return norm(re.split(r"(?<=[.!?])\s", r.strip(), maxsplit=1)[0])


def is_wall(r):
    n = r.lower()
    return any(w in n for w in WALLS)


def verdict(expect, resp):
    if expect in ("yes", "no"):
        body = re.sub(r"^reading «[^»]*» as «[^»]*»\.\s*", "", resp.strip().lower())
        return body.startswith(expect)
    return norm(expect) in norm(resp) and not is_wall(resp)


class Parrot:
    """Un processo parrot0 guidato riga per riga; la risposta finisce al prompt '>>> '."""

    def __init__(self, root, turn_timeout):
        env = dict(os.environ, PARROT0_ROOT=root, PARROT0_PROFILE="kb/profiles/agi.p0",
                   PARROT0_LANG=os.environ.get("PARROT0_LANG", "en"))
        self.p = subprocess.Popen(["stdbuf", "-oL", os.path.join(REPO, "bin/parrot0")], cwd=root, env=env,
                                  stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, bufsize=0)
        # il prompt '>>> ' va su stderr (main.c, prompt_str): i due flussi si
        # uniscono, come in scripts/live-teach.sh
        self.timeout = turn_timeout
        self.boot = self._read_reply(boot=True)

    def _read_reply(self, boot=False):
        buf = b""
        t0 = time.time()
        limit = 120 if boot else self.timeout
        os.set_blocking(self.p.stdout.fileno(), False)
        while True:
            try:
                chunk = self.p.stdout.read(65536)
            except BlockingIOError:
                chunk = None
            if chunk:
                buf += chunk
                if buf.endswith(b">>> "):
                    break
            elif self.p.poll() is not None:
                raise RuntimeError("parrot0 e' uscito")
            else:
                time.sleep(0.02)
            if time.time() - t0 > limit:
                raise TimeoutError(f"nessuna risposta entro {limit}s")
        txt = buf.decode("utf-8", "replace")[:-4]
        txt = "\n".join(l for l in txt.replace(">>> ", "").splitlines() if l.strip())
        return txt.strip(), time.time() - t0

    def say(self, line):
        self.p.stdin.write((line + "\n").encode())
        self.p.stdin.flush()
        reply, dt = self._read_reply()
        return reply, dt, reply.splitlines()

    def close(self):
        try:
            self.p.stdin.close(); self.p.wait(timeout=5)
        except Exception:
            self.p.kill()


def make_sandbox(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    shutil.copytree(os.path.join(REPO, "kb"), os.path.join(path, "kb"), symlinks=True)
    for f in ("world.p0",):
        src = os.path.join(REPO, f)
        if os.path.exists(src):
            shutil.copy2(src, path)
    os.symlink(os.path.join(REPO, "bin"), os.path.join(path, "bin"))


class KbWatch:
    """Che cosa e' cambiato nell'albero della sandbox dall'ultima volta: le righe
    aggiunte, per file. Si leggono solo i file toccati (mtime)."""

    def __init__(self, root):
        self.root = os.path.join(root, "kb")
        self.mark = time.time()
        self.cache = {}

    def _lines(self, path):
        try:
            return open(path, encoding="utf-8", errors="replace").read().splitlines()
        except OSError:
            return []

    def added(self):
        out = []
        now = time.time()
        for d, _, fs in os.walk(self.root):
            for f in fs:
                path = os.path.join(d, f)
                try:
                    if os.path.getmtime(path) < self.mark - 1:
                        continue
                except OSError:
                    continue
                rel = os.path.relpath(path, self.root)
                old = self.cache.get(rel)
                if old is None:
                    old = self._lines(os.path.join(REPO, "kb", rel))
                new = self._lines(path)
                seen = {}
                for l in old:
                    seen[l] = seen.get(l, 0) + 1
                for l in new:
                    if seen.get(l, 0):
                        seen[l] -= 1
                    elif l.strip() and not l.lstrip().startswith("%"):
                        out.append((rel, l.strip()))
                self.cache[rel] = new
        self.mark = now
        return out


# i fatti di servizio (menzioni, fonti, letture, lacune) non sono cio' che la
# lezione ha insegnato: si contano a parte
SERVICE = re.compile(r"^(fact_mention|fact_source|reading_fact|semantic_binding|semantic_proposition|"
                     r"machinery_gap|pending_gap_failed|bridged|class_surface|condition_closed_world)\(")


def service(rel, line):
    """kb/machinery/ e' il registro di servizio (trascrizioni, lacune, letture):
    non e' cio' che la lezione ha insegnato al mondo"""
    return rel.startswith("machinery/") or rel == "savemap.tsv" or bool(SERVICE.match(line))


def routed(errlines):
    for l in errlines:
        m = re.search(r"routed (\d+) clause", l)
        if m:
            return int(m.group(1))
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("battery")
    ap.add_argument("--only", default="")
    ap.add_argument("--turn-timeout", type=float, default=10.0)
    ap.add_argument("--sandbox", default=os.path.join(REPO, "var/coherence-sandbox"))
    a = ap.parse_args()

    bat = json.load(open(a.battery))
    lessons = bat["lessons"]
    if a.only:
        keep = set(a.only.split(","))
        lessons = [l for l in lessons if l["id"] in keep]

    make_sandbox(a.sandbox)
    os.makedirs(os.path.join(REPO, "logs"), exist_ok=True)
    out_path = os.path.join(REPO, "logs", f"coherence-{bat['battery']}.jsonl")
    p = Parrot(a.sandbox, a.turn_timeout)
    watch = KbWatch(a.sandbox)
    print(f"# boot {p.boot[1]:.1f}s — batteria {bat['battery']}, {len(lessons)} lezioni, sandbox {a.sandbox}", flush=True)
    slow = []

    def ask(probe):
        last, tot = "", 0.0
        for part in [x.strip() for x in probe.split("||") if x.strip()]:
            last, dt, _ = p.say(part)
            tot += dt
            if dt > 2.0:
                slow.append((part, round(dt, 2)))
        return last, tot

    def run_probes(pairs):
        res = []
        for q, e in pairs:
            r, dt = ask(q)
            res.append({"probe": q, "expect": e, "reply": r, "ok": verdict(e, r), "s": round(dt, 2)})
        return res

    recs = []
    for L in lessons:
        before = run_probes(L["before"])
        # il contrasto si chiede SUBITO prima e SUBITO dopo la lezione: fra le due
        # domande c'e' solo la lezione, e cio' che cambia e' suo
        contrast_before = [ask(c if isinstance(c, str) else c[0])[0] for c in L.get("contrast", [])]
        # `/save` conta TUTTE le clausole della sessione (kb_save_routed), non le
        # nuove: la lezione e' la differenza fra il conto prima e quello dopo
        p.say("/save")
        watch.added()
        lesson_replies = [p.say(line)[0] for line in L["lesson"]]
        p.say("/save")
        added = watch.added()
        facts = [(f, l) for f, l in added if not service(f, l)]
        wrote = len(facts)
        contrast_after = [ask(c if isinstance(c, str) else c[0])[0] for c in L.get("contrast", [])]
        contrast_ok = all(
            (verdict(c[1], y) if not isinstance(c, str) else first(x) == first(y))
            for c, x, y in zip(L.get("contrast", []), contrast_before, contrast_after))
        after = run_probes(L["after"])
        held = run_probes(L.get("held", []))
        recs.append({"id": L["id"], "rule": L["rule"], "lesson": L["lesson"],
                     "lesson_reply": " / ".join(lesson_replies), "wrote": wrote,
                     "facts": [f"{f}: {l}" for f, l in facts],
                     "service": [f"{f}: {l}" for f, l in added if service(f, l)],
                     "before": before, "after": after, "held": held,
                     "contrast": [{"probe": q if isinstance(q, str) else q[0], "before": x, "after": y}
                                  for q, x, y in zip(L.get("contrast", []), contrast_before, contrast_after)],
                     "contrast_ok": contrast_ok})
        r = recs[-1]
        print(f"  {r['id']:4s} prima={'si' if all(x['ok'] for x in before) else 'no'} "
              f"dopo={'si' if all(x['ok'] for x in after) else 'no'} "
              f"fuori={'si' if all(x['ok'] for x in held) else 'no'} "
              f"contrasto={'=' if contrast_ok else '≠'} scritti={wrote} | {r['lesson_reply'][:70]}", flush=True)

    # riavvio: la KB ricaricata dal disco (quella che le lezioni hanno lasciato)
    # si rifanno solo le lezioni che hanno cambiato qualcosa: le altre non hanno
    # niente da perdere, e il banco resta corto
    p.say("/restore")
    for r, L in zip(recs, lessons):
        learned = not all(x["ok"] for x in r["before"]) and any(x["ok"] for x in r["after"] + r["held"])
        r["restart"] = run_probes(L["after"] + L.get("held", [])) if learned else []
    p.close()

    def ok(xs):
        return all(x["ok"] for x in xs)

    n_coh = 0
    for r in recs:
        pre, post, held, rs = ok(r["before"]), ok(r["after"]), ok(r["held"]), ok(r["restart"])
        overlap_said = "already" in r["lesson_reply"].lower()
        if pre:
            # gia' operante: coerente se la lezione lo riconosce, non scrive, non sporca
            coherent = overlap_said and not r["wrote"] and post and r["contrast_ok"]
            out = ("sovrapposizione riconosciuta" if coherent else
                   "sovrapposizione NON riconosciuta" + (", fatti scritti" if r["wrote"] else ""))
        else:
            coherent = post and held and r["contrast_ok"] and rs
            if coherent and is_wall(r["lesson_reply"]) and not r["wrote"]:
                # la lezione ha preso un muro e non ha scritto niente: cio' che
                # ora riesce non viene da lei, e il banco non glielo accredita
                coherent = False
                out = "riesce, ma non per la lezione (sonda da rivedere)"
            elif coherent:
                out = "innesto coerente"
            elif post or held:
                out = "innesto parziale" + ("" if rs else ", perso al riavvio") + ("" if r["contrast_ok"] else ", contamina")
            elif r["wrote"]:
                out = "fatti scritti senza effetto (spazzatura)"
            else:
                out = "non innestata, niente scritto"
        if not r["contrast_ok"] and "contamina" not in out:
            out += ", contamina"
        r["coherent"] = coherent
        r["outcome"] = out
        n_coh += coherent

    with open(out_path, "w") as f:
        for r in recs:
            f.write(json.dumps(r, ensure_ascii=False) + "\n")

    print(f"\n# referto — {bat['battery']}")
    for r in recs:
        print(f"  {r['id']:4s} {'✓' if r['coherent'] else '✗'} {r['outcome']:52s} {r['rule']}")
    written = sum(r["wrote"] or 0 for r in recs)
    print(f"\n# coerenza: {n_coh}/{len(recs)} lezioni; fatti scritti dalle lezioni: {written}")
    for r in recs:
        for x in r["facts"][:4]:
            print(f"    {r['id']:4s} + {x[:110]}")
    if slow:
        print(f"# turni oltre 2 s: {len(slow)} — " + "; ".join(f"{q[:40]} {s}s" for q, s in slow[:8]))
    print(f"# dettaglio: {out_path}\n# che cosa e' stato scritto: git diff --no-index --stat kb {a.sandbox}/kb")


if __name__ == "__main__":
    main()
