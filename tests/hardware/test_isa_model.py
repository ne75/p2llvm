"""Check oracle edge cases against explicit arithmetic/ISA examples."""
import unittest
from isa_model import scalar


class OracleTests(unittest.TestCase):
    def test_carry_borrow_and_extended_zero(self):
        self.assertEqual(scalar('ADD', 0xffffffff, 1, 0, 0), (0, 1, 1))
        self.assertEqual(scalar('SUBX', 0, 0, 1, 1), (0xffffffff, 1, 0))
        self.assertEqual(scalar('ADDX', 0xffffffff, 0, 1, 0), (0, 1, 0))
        self.assertEqual(scalar('CMPS', 0x80000000, 1, 0, 0), (0x80000000, 1, 0))

    def test_shifts_keep_the_documented_zero_count_carry(self):
        self.assertEqual(scalar('ROR', 0x80000001, 0, 0, 0), (0x80000001, 1, 0))
        self.assertEqual(scalar('RCL', 0x80000000, 1, 1, 0), (1, 1, 0))
        self.assertEqual(scalar('SAR', 0x80000000, 31, 0, 0), (0xffffffff, 0, 0))

    def test_fields_and_undefined_encod(self):
        self.assertEqual(scalar('SETNIB', 0x12345678, 1000, 1, 1, 7), (0x82345678, 1, 1))
        self.assertEqual(scalar('GETBYTE', 0, 0x12345678, 0, 1, 3), (0x12, 0, 1))
        self.assertIsNone(scalar('ENCOD', 0, 0, 0, 0))


if __name__ == '__main__':
    unittest.main()
