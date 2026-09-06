import argparse
from pathlib import Path
import unittest

from loader import arguments, identity


class LoaderTests(unittest.TestCase):
    def options(self, **overrides):
        values = dict(loader=Path('/opt/p2llvm/bin/loadp2'), reset='RTS',
                      baud=2000000, loader_baud=2000000, port=None,
                      clock_hz=None, clock_mode=None, fifo=10000)
        values.update(overrides)
        return argparse.Namespace(**values)

    def test_user_defaults_keep_auto_port_and_clock(self):
        cmd = arguments(self.options(), Path('/tmp/test.bin'))
        for option in ['-RTS', '-PATCH', '-ZERO', '-q', '-v', '-t']:
            self.assertIn(option, cmd)
        for option in ['-p', '-f', '-m']:
            self.assertNotIn(option, cmd)
        self.assertEqual(cmd[cmd.index('-FIFO')+1], '10000')
        self.assertEqual(cmd[cmd.index('-b')+1], '2000000')

    def test_explicit_overrides_and_detection(self):
        cmd = arguments(self.options(port='/dev/test', clock_hz=80000000,
                                     clock_mode=0x123), Path('/tmp/test.bin'))
        self.assertEqual(cmd[cmd.index('-p')+1], '/dev/test')
        self.assertEqual(cmd[cmd.index('-f')+1], '80000000')
        self.assertEqual(cmd[cmd.index('-m')+1], '0x123')
        self.assertEqual(identity(b'P2 version G found on serial port /dev/test\nSetting clock_mode to 123\n'),
                         dict(rom_version='G', detected_port='/dev/test', detected_clock_mode=0x123))


if __name__ == '__main__':
    unittest.main()
