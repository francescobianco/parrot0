"""Mechanical tests of the gate, not evidence of parrot0 understanding."""
import copy
import importlib.util
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location('prose_gate', Path(__file__).resolve().parents[1] / 'scripts/prose-gate.py')
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)


def report():
    rows = []
    for i in range(62):
        rows.append(dict(index=str(i), question='same prefix ' * 4 + str(i),
                         kind='merito', expected='answer', cold='unknown',
                         hot='answer' if i < 45 else 'unknown',
                         cold_status='wall', hot_status='correct' if i < 45 else 'wall'))
    return dict(schema=1, complete=True, rows=rows,
                contract=dict(fixture_sha256='a', questions_sha256='b', kb_sha256='c',
                              probe_sha256='d', gate_sha256='e', lang='en', profile='agi',
                              timeout=1200, environment={}, case_count=62))


class GateTests(unittest.TestCase):
    def test_unchanged(self):
        self.assertEqual(gate.compare(report(), report()), [])

    def test_historical_collapse_45_to_6(self):
        after = report()
        for row in after['rows'][6:45]:
            row.update(hot_status='wall', hot='unknown')
        self.assertEqual(len(gate.compare(report(), after)), 39)

    def test_equal_total_cannot_hide_loss(self):
        after = report()
        after['rows'][0].update(hot_status='wall', hot='unknown')
        after['rows'][45].update(hot_status='correct', hot='answer')
        self.assertEqual(len(gate.compare(report(), after)), 1)

    def test_new_wrong_answer(self):
        after = report()
        after['rows'][61].update(hot_status='wrong', hot='unfounded')
        self.assertIn('non-wall', gate.compare(report(), after)[0])

    def test_already_known_still_protected(self):
        before = report()
        before['rows'][0].update(cold_status='correct', cold='answer')
        after = copy.deepcopy(before)
        after['rows'][0].update(hot_status='wall', hot='unknown')
        self.assertTrue(gate.compare(before, after))

    def test_cold_attribution_change_blocks(self):
        after = report()
        after['rows'][0].update(cold_status='correct', cold='answer')
        self.assertIn('attribution', gate.compare(report(), after)[0])

    def test_changed_wrong_answer_needs_review(self):
        before = report()
        before['rows'][61].update(hot_status='wrong', hot='wrong one')
        after = copy.deepcopy(before)
        after['rows'][61]['hot'] = 'wrong two'
        self.assertIn('review', gate.compare(before, after)[0])

    def test_incomplete_and_incomparable_rejected(self):
        for kind in ('missing', 'empty', 'reordered', 'oracle', 'kb', 'complete', 'identity'):
            with self.subTest(kind=kind):
                after = report()
                if kind == 'missing': after['rows'].pop()
                if kind == 'empty': after['rows'][0]['hot'] = ''
                if kind == 'reordered': after['rows'].reverse()
                if kind == 'oracle': after['contract']['questions_sha256'] = 'changed'
                if kind == 'kb': after['contract']['kb_sha256'] = 'changed'
                if kind == 'complete': after['complete'] = False
                if kind == 'identity': after['rows'][0]['question'] += 'changed'
                with self.assertRaises(ValueError):
                    gate.compare(report(), after)


if __name__ == '__main__':
    unittest.main()
