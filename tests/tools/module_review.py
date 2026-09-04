#!/usr/bin/env python3
"""Il cricchetto della REVIEW DEI MODULI (gen502, mantra #21).

Una review dichiara il TITOLO di una facolta' a rivendicare un turno. Questo
controllo non la conferma — la puo' solo FALSIFICARE, ed e' tutta la differenza:

  ⛔ misurato il 2026-09-04: contando PER MODULO (non per file), `mod_compose` —
     il primo ladro di turni — ha zero parole compilate e zero TODO kb-first,
     esattamente come `mod_codeast` che ha risposto bene. Nessuna metrica
     distingue un modulo maturo da uno immaturo.

Quindi la maturita' resta un GIUDIZIO scritto e datato in testa al modulo, e i
numeri servono a provare che quel giudizio mente. Cio' che qui e' un errore:

  1. una review in KB senza la sua testata nel C (o viceversa);
  2. `maturity kb_first` con parole di dominio compilate o TODO(kb-first) aperti;
  3. un `module_evidence` che non combacia con la misura di oggi;
  4. una facolta' recensita che non esiste nel registro.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BRAIN = ROOT / "src" / "brain"
REVIEW_KB = ROOT / "kb" / "core" / "module-review.p0"

KB_CALL = re.compile(r'\b(kb_cue_match|kb_query|kb_match|kb_match_all|kb_hypothesis_best'
                     r'|kb_knows_pred|kb_intent|p0_faculty_yields)\s*\(')
FRAME = re.compile(r'\b(input_frame\w*|input_segment|input_span\w*|segment_role|p0_turn_is'
                   r'|turn_frame\w*|input_structure\w*|p0_turn_pattern\w*)\s*\(')
LITERAL = re.compile(r'\b(strcmp|strncmp|strstr|strcasecmp|strncasecmp)\s*'
                     r'\([^;]*?"([a-zA-Z][a-zA-Z \'-]{2,})"')
TODO = re.compile(r'TODO\(kb-first\)|TODO kb-first')
HEADER = re.compile(r'MODULE REVIEW — ([a-z_0-9]+)')


def body(src: str, start: int) -> str:
    i = src.index("{", start)
    depth = 0
    for j in range(i, len(src)):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                return src[i:j + 1]
    return src[i:]


def measure() -> tuple[dict[str, dict[str, int]], set[str]]:
    """Per ogni mod_X: i numeri di oggi, e chi porta una testata di review."""
    rows: dict[str, dict[str, int]] = {}
    headed: set[str] = set()
    for path in sorted(BRAIN.glob("*.c")):
        src = path.read_text(errors="replace")
        headed.update(HEADER.findall(src))
        for m in re.finditer(r'^(?:static\s+)?int\s+(mod_[a-z_0-9]+)\s*\(', src, re.M):
            b = body(src, m.start())
            # ⚠ Un confronto contro un campo di PROVENIENZA non e' vocabolario:
            # `strcmp(b->last_module, "gen")` e' una facolta' che riconosce se
            # stessa, cioe' meccanismo. Contarlo come parola di dominio ha
            # falsificato una review corretta al primo giro del cricchetto —
            # e una misura che accusa il codice giusto e' peggio di nessuna.
            words = [x for x in LITERAL.finditer(b)
                     if "last_module" not in x.group(0) and "->name" not in x.group(0)]
            rows[m.group(1)[len("mod_"):]] = {
                "lines": b.count("\n") + 1,
                "kb_lookups": len(KB_CALL.findall(b)),
                "frame_uses": len(FRAME.findall(b)),
                "compiled_words": len(words),
                "todo_kb_first": len(TODO.findall(b)),
            }
    return rows, headed


def parse_reviews() -> tuple[dict[str, str], dict[str, str], dict[str, dict[str, int]]]:
    text = REVIEW_KB.read_text()
    maturity = dict(re.findall(r'^module_maturity\(([a-z_0-9]+),\s*([a-z_]+)\)\.', text, re.M))
    right = dict(re.findall(r'^module_claim_right\(([a-z_0-9]+),\s*([a-z_]+)\)\.', text, re.M))
    evidence: dict[str, dict[str, int]] = {}
    for fac, field, value in re.findall(
            r'^module_evidence\(([a-z_0-9]+),\s*([a-z_]+),\s*(-?\d+)\)\.', text, re.M):
        evidence.setdefault(fac, {})[field] = int(value)
    return maturity, right, evidence


def registry_names() -> set[str]:
    src = (BRAIN / "99-registry.c").read_text(errors="replace")
    block = src[src.index("static const Module registry[]"):]
    block = block[:block.index("};")]
    return set(re.findall(r'\{\s*"([a-z_0-9]+)"\s*,', block))


def main() -> int:
    rows, headed = measure()
    maturity, right, evidence = parse_reviews()
    known = registry_names()
    problems: list[str] = []

    for fac in sorted(set(maturity) | set(right) | set(evidence)):
        if fac not in known:
            problems.append(f"{fac}: recensito ma non nel registro dei moduli")
            continue
        if fac not in headed:
            problems.append(f"{fac}: review in KB senza la testata nel C "
                            f"(mantra #21: la review sta IN TESTA al modulo)")
        measured = rows.get(fac)
        if measured is None:
            problems.append(f"{fac}: nessun mod_{fac} misurabile")
            continue
        if maturity.get(fac) == "kb_first":
            if measured["compiled_words"]:
                problems.append(
                    f"{fac}: dichiara kb_first ma ha {measured['compiled_words']} "
                    f"parole di dominio compilate")
            if measured["todo_kb_first"]:
                problems.append(
                    f"{fac}: dichiara kb_first con {measured['todo_kb_first']} "
                    f"TODO(kb-first) aperti")
        for field, declared in evidence.get(fac, {}).items():
            if field not in measured:
                problems.append(f"{fac}: evidenza sconosciuta {field!r}")
            elif measured[field] != declared:
                problems.append(
                    f"{fac}: {field} dichiarato {declared}, misurato {measured[field]} "
                    f"— la review e' invecchiata, va rivista o corretta")

    for fac in sorted(headed):
        if fac not in maturity:
            problems.append(f"{fac}: testata nel C senza la proiezione in "
                            f"kb/core/module-review.p0 (l'arbitrato non la vede)")

    for line in problems:
        print(f"FAIL module-review: {line}")
    reviewed = len(set(maturity))
    demoted = sum(1 for v in right.values() if v == "fallback")
    print(f"module-review: {reviewed} recensiti su {len(known)} moduli, "
          f"{demoted} retrocessi, {len(problems)} problemi")
    return 1 if problems else 0


if __name__ == "__main__":
    raise SystemExit(main())
