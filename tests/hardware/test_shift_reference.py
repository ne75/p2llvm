"""Validate the actual C word-at-a-time oracle against Python integer shifts."""
import ctypes
import os
from pathlib import Path
import random
import shlex
import subprocess
import tempfile
import unittest

HERE = Path(__file__).resolve().parent
MASK = (1 << 64) - 1


class ShiftReferenceTests(unittest.TestCase):
    def test_every_count_and_bit_against_exact_integers(self):
        bridge = '''
#include <stdint.h>
#include "shift-reference.h"
uint64_t reference(unsigned lo, unsigned hi, unsigned n, unsigned kind) {
    struct shift_words v = shift_reference((struct shift_words){lo, hi}, n, kind);
    return ((uint64_t)v.hi << 32) | v.lo;
}
'''
        with tempfile.TemporaryDirectory() as temp:
            library = Path(temp) / 'reference.so'
            compiler = shlex.split(os.environ.get('HOST_CC', 'cc'))
            subprocess.run([*compiler, '-std=c99', '-O2', '-shared', '-fPIC',
                            '-I', str(HERE), '-x', 'c', '-', '-o', str(library)],
                           input=bridge, text=True, check=True, capture_output=True)
            native = ctypes.CDLL(str(library)).reference
            native.argtypes = [ctypes.c_uint32] * 4
            native.restype = ctypes.c_uint64
            rng = random.Random(0x50325333)
            values = [0, MASK, 0x100000002, 0xaaaaaaaa55555555]
            values += [1 << bit for bit in range(64)]
            values += [MASK ^ (1 << bit) for bit in range(64)]
            values += [rng.getrandbits(64) for _ in range(200)]
            for a in values:
                signed = a - (1 << 64) if a >> 63 else a
                for n in range(64):
                    for kind, expected in enumerate(((a << n) & MASK, a >> n,
                                                     (signed >> n) & MASK)):
                        self.assertEqual(native(a & 0xffffffff, a >> 32, n, kind),
                                         expected, (hex(a), n, kind))


if __name__ == '__main__':
    unittest.main()
