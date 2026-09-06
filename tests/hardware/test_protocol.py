"""The hardware verdict must reject corruption, omission and stale output."""
import unittest

from protocol import compare


class ProtocolTests(unittest.TestCase):
    good = b'P2TEST fresh\nP2VALUE x 00000007 fffffff8\nP2END fresh 00000001\n'

    def test_exact_observation(self):
        self.assertEqual(compare(self.good, 'fresh', {'x': '7'}), ({'x': 7}, []))

    def test_wrong_value_is_failure(self):
        self.assertEqual(compare(self.good, 'fresh', {'x': '8'})[1][0]['actual'], 7)

    def test_missing_or_extra_cases_are_failures(self):
        self.assertTrue(compare(self.good, 'fresh', {'x': '7', 'y': '9'})[1])
        self.assertTrue(compare(self.good, 'fresh', {})[1])

    def test_invalid_transcripts(self):
        variants = [self.good.replace(b'fresh', b'stale'),
                    self.good.replace(b'fffffff8', b'ffffffff'),
                    self.good.split(b'P2END')[0],
                    self.good.replace(b'00000001', b'00000002'),
                    self.good.replace(b'P2END', b'P2VALUE x 00000007 fffffff8\nP2END')]
        for text in variants:
            with self.subTest(text=text), self.assertRaises(ValueError):
                compare(text, 'fresh', {'x': '7'})


if __name__ == '__main__':
    unittest.main()
