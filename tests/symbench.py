#!/usr/bin/env python3
"""symbench — the CRYPTIC-stimulus challenge: how does an LLM *behave*?

A discovery harness in the spirit of chatsim.py, but the inputs are NOT prose
and — crucially — NOT puzzles with a checkable answer. They are short, often
CRYPTIC symbolic stimuli with no single "correct" completion: ASCII art and
faces, strings with odd symmetries, alternative symbol use (leetspeak, numbers
for letters), Morse code (dots/dashes, and Morse of known words), musical notes
(do re mi), and fragments of incomplete code.

We do NOT grade correctness. The oracle's reply is a FREE behavioural signal:
we want to see HOW a real LLM reacts to ambiguity — does it name the pattern,
ask, play along, complete the fragment, refuse? — so we can grow parrot0 toward
the same behaviour (PRINCIPLES.md: recover, behaviourally, the structure the
model runs in latent space). The artifact is the side-by-side transcript; the
only number that matters is engagement: the oracle almost always engages, today
parrot0 almost always walls.

Both responders are treated as a concise chatbot receiving the same message, so
the comparison is fair (parrot0 IS a chatbot). Provider/auth as chatsim
(opencode-GO, $OPENCODE_API_KEY); default model is a NON-reasoning instruct
model (a thinking model leaks its working into the reply). Transcripts -> tests/sym/.

Usage:
  .venv/bin/python tests/symbench.py --per-cat 4 --model kimi-k2.6
  .venv/bin/python tests/symbench.py --no-llm     # parrot0-only dry run (free)
"""
from __future__ import annotations
import argparse, json, os, random, re, subprocess, sys, time
import urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"  # opencode-GO, like chatsim

# Frame both responders identically: a concise chatbot reacting to whatever the
# user just sent. NO hint that there is a "right" answer — we want free reaction.
SYS = (
    "You are a concise, friendly chatbot. The user's next message may be cryptic "
    "or non-prose — symbols, ASCII art, Morse, musical notes, a code fragment, a "
    "weird pattern. There is no single correct answer. Just react naturally and "
    "briefly, the way a smart, curious person would: say what you make of it, "
    "name the pattern, play along, or ask. Keep it to one short line."
)


def call_oracle(key: str, model: str, user: str, temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 400,
        "temperature": temperature, "messages": [
            {"role": "system", "content": SYS},
            {"role": "user", "content": user}]}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}", "Content-Type": "application/json",
        "User-Agent": "parrot0-symbench/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=120) as r:
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
    c = c.replace("<think>", "").replace("</think>", "").strip()
    # take the last non-empty line (a stray reasoning preamble sits above it),
    # but for a non-reasoning model this is just the single reply line.
    lines = [ln.strip() for ln in c.split("\n") if ln.strip()]
    return (lines[-1][:200] if lines else "[empty]")


# --- cryptic-stimulus generators ---------------------------------------------
# Each yields dicts {cat, input, why}. `input` may contain newlines (2-D art);
# `why` is a human label for the transcript, never a graded answer.

def gen_leet(rng):
    words = ["hello", "elite", "leet", "code", "ghost", "secret", "online",
             "robot", "noise", "tools", "see", "boobies"]
    sub = {"o": "0", "e": "3", "i": "1", "a": "4", "s": "5", "t": "7", "l": "1",
           "b": "8", "g": "9"}
    out = []
    for _ in range(6):
        w = rng.choice(words)
        leet = "".join(sub.get(ch, ch) if rng.random() < 0.8 else ch for ch in w)
        out.append({"cat": "leet", "input": leet, "why": "alt symbols (leetspeak)"})
    out += [{"cat": "leet", "input": s, "why": "alt symbols"} for s in
            ["1337", "5UP", "n00b", "h4x0r", "0wn3d", "4ll y0ur b4s3"]]
    return out


def gen_morse(rng):
    M = {"a":".-","b":"-...","c":"-.-.","d":"-..","e":".","f":"..-.","g":"--.",
         "h":"....","i":"..","j":".---","k":"-.-","l":".-..","m":"--","n":"-.",
         "o":"---","p":".--.","q":"--.-","r":".-.","s":"...","t":"-","u":"..-",
         "v":"...-","w":".--","x":"-..-","y":"-.--","z":"--.."}
    def morse(word): return " ".join(M[c] for c in word)
    out = [{"cat": "morse", "input": "... --- ...", "why": "Morse (SOS)"},
           {"cat": "morse", "input": ".... .. -.-. .- -..", "why": "Morse word"},
           {"cat": "morse", "input": ".- .-.. ", "why": "Morse fragment"}]
    for w in ["sos", "help", "hi", "ok", "morse"]:
        out.append({"cat": "morse", "input": morse(w), "why": "Morse of a word"})
    out.append({"cat": "morse", "input": "-.-. --.- -.-. --.-", "why": "Morse rhythm"})
    return out


def gen_notes(rng):
    out = [{"cat": "notes", "input": s, "why": "musical notes"} for s in
           ["do re mi", "do re mi fa sol la si", "do mi sol", "do re mi do",
            "sol fa mi re do", "la si do", "C D E F G A B", "C E G", "C G C"]]
    return out


def gen_symmetry(rng):
    out = [{"cat": "symmetry", "input": s, "why": "symmetric string / palindrome"}
           for s in ["abccba", "racecar", "level", "12321", "()()()",
                     "([{}])", "<<>>", "><><><", "abcba", "1 2 3 2 1",
                     "/\\/\\/\\", "][][]["]]
    return out


def gen_code(rng):
    out = [{"cat": "code", "input": s, "why": "incomplete code fragment"} for s in
           ["def foo(", "for i in range(", "if x ==", "while True:",
            "int main() {", "print(", "SELECT * FROM", "}", "};",
            "import ", "return", "x = [1, 2,", "function() {", "<html>"]]
    return out


def gen_ascii(rng):
    # single-line faces, arrows, bars, runs (parrot0 is one-line-in/one-line-out,
    # so 2-D art is handled separately and flagged).
    out = [{"cat": "ascii", "input": s, "why": "single-line ASCII / pattern"} for s in
           [":-)", ":-(", "<(^_^)>", "¯\\_(ツ)_/¯", "(╯°□°)╯", "o_O", ">_<",
            "=> => =>", "[####------] 40%", "***---***", "~~~~~~", "8======D",
            "[ ] [x] [ ]", "...---...", "+1", "#FF0000"]]
    return out


def gen_cryptic(rng):
    out = [{"cat": "cryptic", "input": s, "why": "cryptic token / code"} for s in
           ["404", "0x1F", "::1", "/dev/null", "C-x C-s", "rm -rf /", "200 OK",
            "TODO:", "Ctrl+Alt+Del", "git push --force", "42", "3.14159...",
            "lorem ipsum", "qwerty", "asdf", "..."]]
    return out


def gen_art2d(rng):
    # genuine 2-D ASCII art: sent raw to the oracle; for parrot0 the rows are
    # joined with " / " since it cannot receive a multi-line turn (itself a
    # finding). Flagged via the trailing note.
    arts = [
        "  /\\\n /  \\\n/____\\",
        "+--+\n|  |\n+--+",
        "*\n**\n***",
        "^_^",
        " __\n(  )\n ~~",
    ]
    return [{"cat": "art2d", "input": a, "why": "2-D ASCII art (multi-line)"}
            for a in arts]


GENERATORS = [gen_leet, gen_morse, gen_notes, gen_symmetry, gen_code,
              gen_ascii, gen_cryptic, gen_art2d]

WALL_MARKERS = (
    "I don't understand that yet.",
    "I didn't quite catch that.",
    "I'm not sure I followed.",
)

def is_wall(resp: str) -> bool:
    r = resp.strip()
    return (r.startswith("Hmm, I don't know about")
            or any(r.startswith(m) for m in WALL_MARKERS))


def feed_parrot0(text: str) -> str:
    """parrot0 is strictly one line in / one line out, so a multi-line stimulus
    is flattened to a single line (rows joined by ' / '). Returns its reply."""
    return text.replace("\n", " / ")


P0_EOT = "\x1e"


def read_reply(proc):
    """gen269: replies may span several lines (markdown-fenced code). The CLI
    prints an explicit end-of-turn marker line when PARROT0_EOT is set; read
    until it so a multi-line reply stays one turn."""
    lines = []
    while True:
        ln = proc.stdout.readline()
        if not ln:
            break
        ln = ln.rstrip("\n")
        if ln == P0_EOT:
            break
        lines.append(ln)
    return "\n".join(lines)


def ask_parrot0(cases: list) -> list:
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": "",
             "PARROT0_EOT": P0_EOT})
    out = []
    try:
        for c in cases:
            proc.stdin.write(feed_parrot0(c["input"]) + "\n"); proc.stdin.flush()
            out.append(read_reply(proc))
    finally:
        try: proc.stdin.write("/quit\n"); proc.stdin.flush()
        except Exception: pass
        proc.terminate()
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--per-cat", type=int, default=None,
                    help="cap cases per category (default: all generated)")
    ap.add_argument("--model", default="kimi-k2.6",
                    help="prefer a non-reasoning instruct model: a thinking "
                         "model leaks its working into the reply line")
    ap.add_argument("--temperature", type=float, default=0.8)
    ap.add_argument("--no-llm", action="store_true",
                    help="skip the oracle (parrot0-only, free dry run)")
    ap.add_argument("--seed", type=int, default=None)
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    if not os.access("./bin/parrot0", os.X_OK):
        print("symbench: build first (make build)", file=sys.stderr); return 2
    key = os.environ.get("OPENCODE_API_KEY")
    if not args.no_llm and not key:
        print("symbench: OPENCODE_API_KEY not set (or pass --no-llm)",
              file=sys.stderr); return 2

    rng = random.Random(args.seed)
    cases = []
    for g in GENERATORS:
        gcases = g(rng)
        if args.per_cat:
            gcases = gcases[:args.per_cat]
        cases += gcases

    os.makedirs("tests/sym", exist_ok=True)
    out = args.out or f"tests/sym/{time.strftime('%Y%m%d-%H%M%S')}.log"
    lines = []
    def log(s): lines.append(s); print(s)

    log(f"# symbench (cryptic stimuli) — model={args.model} cases={len(cases)} "
        f"llm={'off' if args.no_llm else 'on'}")

    p0 = ask_parrot0(cases)

    llm = []
    for c in cases:
        if args.no_llm:
            llm.append("[skipped]"); continue
        r = call_oracle(key, args.model, c["input"], args.temperature)
        if r.startswith("[model error") or r == "[empty]":
            time.sleep(1.5)
            r = call_oracle(key, args.model, c["input"], args.temperature)
        llm.append(r)

    # behavioural accounting — engagement, NOT correctness
    cats = {}
    for c, pr in zip(cases, p0):
        st = cats.setdefault(c["cat"], dict(n=0, wall=0))
        st["n"] += 1
        if is_wall(pr): st["wall"] += 1

    for cat in cats:
        log(f"\n=== {cat} ===")
        for c, pr, lr in zip(cases, p0, llm):
            if c["cat"] != cat: continue
            shown = c["input"].replace("\n", "  ⏎  ")
            log(f"  in       : {shown}    ({c['why']})")
            log(f"  oracle   : {lr}")
            log(f"  parrot0  : {pr}")

    log("\n--- behaviour report (engagement, not correctness) ---")
    tot_n = tot_w = 0
    for cat, st in cats.items():
        tot_n += st["n"]; tot_w += st["wall"]
        log(f"{cat:10s} n={st['n']:2d}  parrot0_wall={100*st['wall']//st['n']:3d}%"
            f"  parrot0_engaged={100*(st['n']-st['wall'])//st['n']:3d}%")
    log(f"\nTOTAL      n={tot_n:2d}  parrot0_wall={100*tot_w//(tot_n or 1)}%"
        f"  parrot0_engaged={100*(tot_n-tot_w)//(tot_n or 1)}%")
    log("\nRead the transcript, not a score: the question is HOW the oracle "
        "engages cryptic input (names it, plays along, asks) and how to grow "
        "parrot0 toward that, instead of walling. No answer is ever hardcoded.")

    with open(out, "w") as f: f.write("\n".join(lines) + "\n")
    log(f"\ntranscript saved: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
