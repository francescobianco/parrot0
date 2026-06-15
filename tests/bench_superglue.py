#!/usr/bin/env python3
"""Run parrot0 against the official SuperGLUE dataset.

This is intentionally not the local benchmark-driver slice. It downloads/loads
SuperGLUE via Hugging Face `datasets`, evaluates every labeled example in the
selected split (validation by default), and reports SuperGLUE-style task scores.

The official SuperGLUE test split has hidden labels, so local scoring is only
possible on labeled splits such as validation. To evaluate the hidden test split,
produce predictions and submit them to the official evaluation server.
"""
from __future__ import annotations

import argparse
import collections
import math
import os
import re
import string
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import Any, Iterable

try:
    from datasets import load_dataset
except ModuleNotFoundError:
    print(
        "bench-superglue: missing Python package 'datasets'.\n"
        "Install it with:\n"
        "  python3 -m venv .venv\n"
        "  .venv/bin/python -m pip install datasets\n"
        "Then rerun: make bench-superglue",
        file=sys.stderr,
    )
    sys.exit(2)

TASKS = ["boolq", "cb", "copa", "multirc", "record", "rte", "wic", "wsc"]
DIAGNOSTICS = ["axb", "axg"]
DATASET_NAMES = ["aps/super_glue", "super_glue", "nyu-mll/super_glue"]


@dataclass
class TaskResult:
    task: str
    examples: int
    invalid: int
    metrics: dict[str, float]
    score: float | None
    elapsed_ms: int


def normalize_text(s: str) -> str:
    s = s.lower()
    s = "".join(ch for ch in s if ch not in string.punctuation)
    s = " ".join(s.split())
    return s


def f1_text(prediction: str, ground_truth: str) -> float:
    pred_tokens = normalize_text(prediction).split()
    gold_tokens = normalize_text(ground_truth).split()
    if not pred_tokens and not gold_tokens:
        return 1.0
    if not pred_tokens or not gold_tokens:
        return 0.0
    common = collections.Counter(pred_tokens) & collections.Counter(gold_tokens)
    num_same = sum(common.values())
    if num_same == 0:
        return 0.0
    precision = num_same / len(pred_tokens)
    recall = num_same / len(gold_tokens)
    return 2 * precision * recall / (precision + recall)


def exact_text(prediction: str, ground_truth: str) -> float:
    return 1.0 if normalize_text(prediction) == normalize_text(ground_truth) else 0.0


def safe_label_names(ds: Any) -> list[str] | None:
    try:
        label = ds.features["label"]
        return list(label.names)
    except Exception:
        return None


def load_superglue(task: str, split: str, cache_dir: str | None) -> Any:
    last_error: Exception | None = None
    for name in DATASET_NAMES:
        try:
            return load_dataset(name, task, split=split, cache_dir=cache_dir)
        except Exception as exc:  # try legacy/canonical names in order
            last_error = exc
    assert last_error is not None
    raise last_error


def run_parrot(binary: str, prompt: str, timeout: float) -> str:
    one_line = " ".join(str(prompt).split())
    proc = subprocess.run(
        [binary],
        input=one_line + "\n/quit\n",
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        timeout=timeout,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": ""},
        check=False,
    )
    return proc.stdout.splitlines()[0].strip() if proc.stdout.splitlines() else ""


def parse_bool(text: str) -> int | None:
    t = normalize_text(text)
    words = t.split()
    if not words:
        return None
    if words[0] in {"yes", "true", "entailed", "entailment"}:
        return 1
    if words[0] in {"no", "false", "contradicted", "contradiction"}:
        return 0
    if " yes " in f" {t} ":
        return 1
    if " no " in f" {t} ":
        return 0
    return None


def parse_choice12(text: str) -> int | None:
    t = normalize_text(text)
    if re.search(r"\b(choice\s*)?1\b", t):
        return 0
    if re.search(r"\b(choice\s*)?2\b", t):
        return 1
    return None


def parse_cb(text: str, label_names: list[str]) -> int | None:
    t = normalize_text(text)
    for i, name in enumerate(label_names):
        n = normalize_text(name)
        if n and n in t:
            return i
    if "entail" in t:
        for i, name in enumerate(label_names):
            if "entail" in normalize_text(name):
                return i
    if "contradict" in t:
        for i, name in enumerate(label_names):
            if "contrad" in normalize_text(name):
                return i
    if "neutral" in t or "unknown" in t or "not entailed" in t:
        for i, name in enumerate(label_names):
            if "neutral" in normalize_text(name):
                return i
    return None


def label_is_missing(label: Any) -> bool:
    return label is None or label == -1


def accuracy(preds: list[int | None], labels: list[int]) -> tuple[float, int]:
    correct = sum(1 for p, y in zip(preds, labels) if p is not None and p == y)
    return (correct / len(labels) if labels else math.nan), sum(1 for p in preds if p is None)


def macro_f1(preds: list[int | None], labels: list[int], classes: Iterable[int]) -> float:
    scores = []
    for c in classes:
        tp = sum(1 for p, y in zip(preds, labels) if p == c and y == c)
        fp = sum(1 for p, y in zip(preds, labels) if p == c and y != c)
        fn = sum(1 for p, y in zip(preds, labels) if p != c and y == c)
        if tp == 0 and fp == 0 and fn == 0:
            scores.append(0.0)
        elif tp == 0:
            scores.append(0.0)
        else:
            precision = tp / (tp + fp)
            recall = tp / (tp + fn)
            scores.append(2 * precision * recall / (precision + recall))
    return sum(scores) / len(scores) if scores else math.nan


def prompt_for(task: str, ex: dict[str, Any]) -> str:
    if task == "boolq":
        return f"SuperGLUE BoolQ. Passage: {ex['passage']} Question: {ex['question']} Answer yes or no."
    if task == "cb":
        return f"SuperGLUE CB. Premise: {ex['premise']} Hypothesis: {ex['hypothesis']} Answer entailment, contradiction, or neutral."
    if task == "copa":
        return f"SuperGLUE COPA. Premise: {ex['premise']} Question: {ex['question']} Choice 1: {ex['choice1']} Choice 2: {ex['choice2']} Answer 1 or 2."
    if task == "multirc":
        return f"SuperGLUE MultiRC. Paragraph: {ex['paragraph']} Question: {ex['question']} Candidate answer: {ex['answer']} Is this answer correct? Answer yes or no."
    if task == "record":
        return f"SuperGLUE ReCoRD. Passage: {ex['passage']} Query: {ex['query']} Fill @placeholder with the answer."
    if task == "rte":
        return f"SuperGLUE RTE. Premise: {ex['premise']} Hypothesis: {ex['hypothesis']} Answer entailment or not_entailment."
    if task == "wic":
        return f"SuperGLUE WiC. Sentence 1: {ex['sentence1']} Sentence 2: {ex['sentence2']} Word: {ex['word']} Same meaning? Answer yes or no."
    if task == "wsc":
        return f"SuperGLUE WSC. Text: {ex['text']} Span 1: {ex['span1_text']} Span 2: {ex['span2_text']} Do they corefer? Answer yes or no."
    if task in {"axb", "axg"}:
        return f"SuperGLUE diagnostic. Premise: {ex.get('sentence1', ex.get('premise', ''))} Hypothesis: {ex.get('sentence2', ex.get('hypothesis', ''))} Answer entailment, contradiction, or neutral."
    raise ValueError(task)


def evaluate_classification(task: str, ds: Any, binary: str, timeout: float, max_examples: int | None) -> TaskResult:
    start = time.time_ns()
    label_names = safe_label_names(ds) or []
    preds: list[int | None] = []
    labels: list[int] = []
    for i, ex in enumerate(ds):
        if max_examples is not None and i >= max_examples:
            break
        y = ex.get("label")
        if label_is_missing(y):
            continue
        out = run_parrot(binary, prompt_for(task, ex), timeout)
        if task == "copa":
            pred = parse_choice12(out)
        elif task == "cb":
            pred = parse_cb(out, label_names)
        elif task in {"rte", "axb", "axg"} and label_names:
            pred = parse_cb(out, label_names)
        else:
            pred = parse_bool(out)
        preds.append(pred)
        labels.append(int(y))
    acc, invalid = accuracy(preds, labels)
    metrics: dict[str, float] = {"accuracy": acc}
    score = acc
    if task == "cb":
        classes = sorted(set(labels) | set(range(len(label_names))))
        f1 = macro_f1(preds, labels, classes)
        metrics["macro_f1"] = f1
        score = (acc + f1) / 2
    elapsed_ms = (time.time_ns() - start) // 1_000_000
    return TaskResult(task, len(labels), invalid, metrics, score, elapsed_ms)


def evaluate_multirc(ds: Any, binary: str, timeout: float, max_examples: int | None) -> TaskResult:
    start = time.time_ns()
    preds: list[int | None] = []
    labels: list[int] = []
    groups: dict[tuple[Any, Any], list[tuple[int | None, int]]] = collections.defaultdict(list)
    for i, ex in enumerate(ds):
        if max_examples is not None and i >= max_examples:
            break
        y = ex.get("label")
        if label_is_missing(y):
            continue
        out = run_parrot(binary, prompt_for("multirc", ex), timeout)
        pred = parse_bool(out)
        # Bool parser returns yes=1/no=0, matching MultiRC label convention in HF.
        preds.append(pred)
        labels.append(int(y))
        idx = ex.get("idx", {})
        key = (idx.get("paragraph", i), idx.get("question", i)) if isinstance(idx, dict) else (i, i)
        groups[key].append((pred, int(y)))
    invalid = sum(1 for p in preds if p is None)
    tp = sum(1 for p, y in zip(preds, labels) if p == 1 and y == 1)
    fp = sum(1 for p, y in zip(preds, labels) if p == 1 and y == 0)
    fn = sum(1 for p, y in zip(preds, labels) if p != 1 and y == 1)
    f1a = 0.0 if tp == 0 else (2 * tp / (2 * tp + fp + fn))
    em = 0.0
    if groups:
        em = sum(1 for vals in groups.values() if all(p == y for p, y in vals)) / len(groups)
    score = (f1a + em) / 2
    elapsed_ms = (time.time_ns() - start) // 1_000_000
    return TaskResult("multirc", len(labels), invalid, {"f1a": f1a, "em": em}, score, elapsed_ms)


def record_answers(ex: dict[str, Any]) -> list[str]:
    answers = ex.get("answers", [])
    if isinstance(answers, dict):
        text = answers.get("text", [])
        return list(text) if isinstance(text, list) else [str(text)]
    if isinstance(answers, list):
        return [str(a) for a in answers]
    return [str(answers)] if answers else []


def evaluate_record(ds: Any, binary: str, timeout: float, max_examples: int | None) -> TaskResult:
    start = time.time_ns()
    ems: list[float] = []
    f1s: list[float] = []
    invalid = 0
    for i, ex in enumerate(ds):
        if max_examples is not None and i >= max_examples:
            break
        answers = record_answers(ex)
        if not answers:
            continue
        out = run_parrot(binary, prompt_for("record", ex), timeout)
        if not out or out == "I don't understand that yet.":
            invalid += 1
        ems.append(max(exact_text(out, a) for a in answers))
        f1s.append(max(f1_text(out, a) for a in answers))
    em = sum(ems) / len(ems) if ems else math.nan
    f1 = sum(f1s) / len(f1s) if f1s else math.nan
    score = (em + f1) / 2
    elapsed_ms = (time.time_ns() - start) // 1_000_000
    return TaskResult("record", len(ems), invalid, {"em": em, "f1": f1}, score, elapsed_ms)


def evaluate_task(task: str, split: str, binary: str, timeout: float, cache_dir: str | None, max_examples: int | None) -> TaskResult:
    ds = load_superglue(task, split, cache_dir)
    if task == "multirc":
        return evaluate_multirc(ds, binary, timeout, max_examples)
    if task == "record":
        return evaluate_record(ds, binary, timeout, max_examples)
    return evaluate_classification(task, ds, binary, timeout, max_examples)


def fmt_pct(x: float | None) -> str:
    if x is None or math.isnan(x):
        return "n/a"
    return f"{100 * x:.2f}%"


def main() -> int:
    parser = argparse.ArgumentParser(description="Run parrot0 on official SuperGLUE via Hugging Face datasets")
    parser.add_argument("--binary", default="bin/parrot0", help="parrot0 binary path")
    parser.add_argument("--split", default="validation", help="SuperGLUE split to score locally; validation by default")
    parser.add_argument("--cache-dir", default=None, help="optional Hugging Face datasets cache dir")
    parser.add_argument("--timeout", type=float, default=2.0, help="timeout per example in seconds")
    parser.add_argument("--max-examples", type=int, default=None, help="debug limit per task; default runs all examples")
    parser.add_argument("--include-diagnostics", action="store_true", help="also run AX-b/AX-g diagnostics if labels are available")
    args = parser.parse_args()

    if not os.path.exists(args.binary) or not os.access(args.binary, os.X_OK):
        print(f"bench-superglue: binary not executable: {args.binary}", file=sys.stderr)
        return 2

    tasks = list(TASKS)
    if args.include_diagnostics:
        tasks += DIAGNOSTICS

    print("Official SuperGLUE benchmark run for parrot0")
    print(f"Dataset: Hugging Face aps/super_glue (official SuperGLUE mirror)")
    print(f"Split:   {args.split}")
    print(f"Binary:  {args.binary}")
    if args.max_examples is not None:
        print(f"Limit:   first {args.max_examples} examples per task")
    if args.split == "test":
        print("Note: SuperGLUE test labels are hidden; local scoring may be unavailable.")
    print()

    results: list[TaskResult] = []
    start_all = time.time_ns()
    for task in tasks:
        try:
            result = evaluate_task(task, args.split, args.binary, args.timeout, args.cache_dir, args.max_examples)
        except Exception as exc:
            print(f"FAIL {task:<8} could not run: {exc}", file=sys.stderr)
            continue
        results.append(result)
        metric_str = " ".join(f"{k}={fmt_pct(v)}" for k, v in result.metrics.items())
        score = fmt_pct(result.score)
        print(
            f"{task:<8} examples={result.examples:<6} invalid={result.invalid:<6} "
            f"score={score:<8} {metric_str} elapsed={result.elapsed_ms}ms"
        )

    scored = [r.score for r in results if r.task in TASKS and r.score is not None and not math.isnan(r.score)]
    elapsed_ms = (time.time_ns() - start_all) // 1_000_000
    print("---")
    if scored:
        overall = sum(scored) / len(scored)
        print(f"superglue_score: {fmt_pct(overall)}")
    else:
        print("superglue_score: n/a")
    print(f"tasks_scored:    {len(scored)}/{len(TASKS)}")
    print(f"elapsed:         {elapsed_ms} ms")
    print("invalid means parrot0 emitted an answer that could not be mapped to the task label space.")
    return 0 if len(scored) == len(TASKS) else 1


if __name__ == "__main__":
    raise SystemExit(main())
