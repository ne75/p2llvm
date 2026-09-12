"""Check the hardware fixture's literal expectations using exact integers."""
from pathlib import Path
import re
import unittest


class NegateVectorTests(unittest.TestCase):
    def test_literal_results_and_signed_overflow_boundary(self):
        source = (Path(__file__).resolve().parent / 'negate-driver.c').read_text()
        rows = re.findall(r'\{(0x[0-9a-f]+)ULL, (0x[0-9a-f]+)ULL\}', source)
        inputs = [int(a, 16) for a, _ in rows]
        self.assertEqual(len(inputs), 32)
        self.assertEqual(len(set(inputs)), 32)
        self.assertIn(0, inputs)
        self.assertIn(1 << 63, inputs)
        self.assertIn((1 << 64) - 1, inputs)
        for a, expected in rows:
            self.assertEqual(int(expected, 16), (-int(a, 16)) % (1 << 64), a)


if __name__ == '__main__':
    unittest.main()
