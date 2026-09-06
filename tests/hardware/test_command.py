from pathlib import Path
import sys
import tempfile
import unittest

from run import command, CommandFailure


class CommandTests(unittest.TestCase):
    def test_loader_input_has_no_premature_eof(self):
        with tempfile.TemporaryDirectory() as directory:
            code = 'import select,sys; print(bool(select.select([sys.stdin],[],[],0.02)[0]))'
            output = command([sys.executable, '-c', code], Path(directory)/'log', keep_input_open=True)
            self.assertEqual(output.strip(), b'False')

    def test_timeout_and_nonzero_exit_preserve_observations(self):
        for code, timeout in [("print('partial',flush=True); import time; time.sleep(5)", .1),
                              ("print('partial',flush=True); raise SystemExit(2)", 5)]:
            with tempfile.TemporaryDirectory() as directory:
                log = Path(directory)/'log'
                with self.assertRaises(CommandFailure) as caught:
                    command([sys.executable, '-c', code], log, timeout)
                self.assertIn(b'partial', caught.exception.output)
                self.assertIn(b'partial', log.read_bytes())


if __name__ == '__main__':
    unittest.main()
