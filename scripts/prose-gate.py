#!/usr/bin/env python3
"""Record the living-KB prose bench; fail on per-question regressions.

record REPORT.json [--fixture PATH] [--lang en|it] [--timeout SECONDS]
compare BEFORE.json AFTER.json

Exit 0: comparable, no regression; 1: regression; 2: invalid/incomplete run.
This preserves the probe's regex oracle; it does not certify comprehension.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ('index', 'question', 'kind', 'expected', 'cold', 'hot',
          'cold_status', 'hot_status')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def tree_digest():
    h = hashlib.sha256()
    for path in sorted((ROOT / 'kb').rglob('*.p0')):
        h.update(str(path.relative_to(ROOT)).encode() + b'\0')
        h.update(path.read_bytes() + b'\0')
    return h.hexdigest()


def validate(report):
    if report.get('schema') != 1 or report.get('complete') is not True:
        raise ValueError('unsupported or incomplete report')
    contract = report['contract']
    required = ('fixture_sha256', 'questions_sha256', 'kb_sha256', 'probe_sha256',
                'gate_sha256', 'lang', 'profile', 'timeout', 'environment', 'case_count')
    if any(key not in contract for key in required):
        raise ValueError('incomplete measurement contract')
    rows = report['rows']
    if not rows or len(rows) != contract['case_count']:
        raise ValueError('missing questions')
    for i, row in enumerate(rows):
        if set(row) != set(FIELDS) or row['index'] != str(i):
            raise ValueError('missing, duplicated or reordered question')
        if not all(isinstance(value, str) and value for value in row.values()):
            raise ValueError('empty answer or field')
        if any(row[key] not in ('correct', 'wall', 'wrong')
               for key in ('cold_status', 'hot_status')):
            raise ValueError('unknown verdict')


def compare(before, after):
    validate(before)
    validate(after)
    if before['contract'] != after['contract']:
        raise ValueError('incomparable corpus, KB, judge or execution settings; remeasure both')
    failures = []
    for old, new in zip(before['rows'], after['rows']):
        if any(old[k] != new[k] for k in ('index', 'question', 'kind', 'expected')):
            raise ValueError('question identities or expected answers changed')
        for phase in ('cold', 'hot'):
            previous, current = old[phase + '_status'], new[phase + '_status']
            reason = None
            if previous == 'correct' and current != 'correct':
                reason = 'lost correct answer'
            elif previous != 'wrong' and current == 'wrong':
                reason = 'new non-wall incorrect answer'
            elif previous == current == 'wrong' and old[phase] != new[phase]:
                reason = 'changed incorrect answer: review required'
            if reason:
                failures.append(f"{phase}: {old['question']}: {reason}\n"
                                f"  {old[phase]!r} -> {new[phase]!r}")
        if old['cold_status'] != 'correct' and new['cold_status'] == 'correct':
            failures.append(f"cold: {old['question']}: attribution changed; review required")
    return failures


def record(args):
    target = Path(args.report).resolve()
    log = target.with_suffix('.log')
    if target.exists() or log.exists():
        raise ValueError('report/log already exists; choose a new path')
    fixture = (ROOT / args.fixture).resolve()
    questions = fixture.with_suffix('.q')
    words = len(fixture.read_text().split())
    expected = []
    for line in questions.read_text().splitlines():
        fields = line.split('\t')
        if len(fields) < 2 or not fields[0] or not fields[1]:
            raise ValueError('invalid question file')
        if len(fields) > 2 and fields[2] and int(fields[2]) > words:
            continue
        expected.append((fields[0], fields[3] if len(fields) > 3 and fields[3] else 'merito', fields[1]))
    if not expected:
        raise ValueError('no eligible questions')
    # No inherited PARROT0/P0 knobs: cold and hot use the same full agi profile.
    env = {k: v for k, v in os.environ.items()
           if not k.startswith(('PARROT0_', 'P0'))}
    env.update(P0LANG=args.lang, P0_PROBE_STEP2='1', P0_PROBE_WHO='0',
               P0_BENCH_MARGIN='1.25', LC_ALL='C.UTF-8')
    contract = dict(fixture_sha256=digest(fixture), questions_sha256=digest(questions),
                    kb_sha256=tree_digest(), probe_sha256=digest(ROOT / 'scripts/prose-probe.sh'),
                    gate_sha256=digest(Path(__file__)), lang=args.lang,
                    profile='kb/profiles/agi.p0', timeout=args.timeout,
                    environment={k: env[k] for k in ('P0_PROBE_STEP2', 'P0_PROBE_WHO',
                                 'P0_BENCH_MARGIN', 'LC_ALL')}, case_count=len(expected))
    binary = digest(ROOT / 'bin/parrot0')
    target.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='prose-gate-') as tmp:
        data = Path(tmp) / 'rows.nul'
        env['P0_PROBE_DATA'] = str(data)
        with log.open('x') as output:
            proc = subprocess.Popen(['bash', 'scripts/prose-probe.sh', str(fixture)],
                                    cwd=ROOT, env=env, stdout=output, stderr=subprocess.STDOUT,
                                    start_new_session=True)
            try:
                code = proc.wait(timeout=args.timeout)
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid, signal.SIGKILL)
                proc.wait()
                raise ValueError(f'benchmark timeout; incomplete log: {log}') from None
        engine_log = Path(str(data) + '.engine.log').read_text()
        with log.open('a') as output:
            output.write('\n--- full engine output ---\n' + engine_log)
        if code or 'PARSE ERROR' in engine_log or 'PROSE_RUN_ERROR' in engine_log:
            raise ValueError(f'benchmark/engine failed; see {log}')
        raw = data.read_bytes().split(b'\0')
        if raw[-1] != b'' or (len(raw) - 1) % len(FIELDS):
            raise ValueError('truncated machine records')
        values = [v.decode('utf-8') for v in raw[:-1]]
        rows = [dict(zip(FIELDS, values[i:i + len(FIELDS)]))
                for i in range(0, len(values), len(FIELDS))]
    if [(r['question'], r['kind'], r['expected']) for r in rows] != expected:
        raise ValueError('bench did not return the complete question sequence')
    if (contract['kb_sha256'] != tree_digest() or binary != digest(ROOT / 'bin/parrot0')
            or contract['fixture_sha256'] != digest(fixture)
            or contract['questions_sha256'] != digest(questions)
            or contract['probe_sha256'] != digest(ROOT / 'scripts/prose-probe.sh')
            or contract['gate_sha256'] != digest(Path(__file__))):
        raise ValueError('inputs changed during measurement')
    report = dict(schema=1, complete=True, contract=contract, rows=rows,
                  binary_sha256=binary, fixture=str(fixture.relative_to(ROOT)),
                  measured_at=datetime.now(timezone.utc).isoformat(),
                  git_head=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
                  git_status=subprocess.check_output(['git', 'status', '--short'], cwd=ROOT, text=True))
    validate(report)
    with target.open('x') as output:
        json.dump(report, output, ensure_ascii=False, indent=2)
        output.write('\n')
    for kind in ('merito', 'meta', 'struttura'):
        eligible = [r for r in rows if r['kind'] == kind and r['cold_status'] != 'correct']
        print(f"{kind}: {sum(r['hot_status'] == 'correct' for r in eligible)}/{len(eligible)}")
    print(f'Recorded {len(rows)} questions: {target}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    rec = commands.add_parser('record')
    rec.add_argument('report')
    rec.add_argument('--fixture', default='tests/fixtures/prose/ladder/r300.txt')
    rec.add_argument('--lang', choices=('en', 'it'), default='en')
    rec.add_argument('--timeout', type=int, default=1200)
    comp = commands.add_parser('compare')
    comp.add_argument('before')
    comp.add_argument('after')
    args = parser.parse_args()
    try:
        if args.command == 'record':
            if args.timeout <= 0:
                raise ValueError('timeout must be positive')
            record(args)
        else:
            failures = compare(json.loads(Path(args.before).read_text()),
                               json.loads(Path(args.after).read_text()))
            print('\n'.join(failures) if failures else 'PASS: comparable runs, no per-question regression')
            return int(bool(failures))
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f'INVALID: {error}', file=sys.stderr)
        return 2
    return 0


if __name__ == '__main__':
    sys.exit(main())
