#!/usr/bin/env python3
"""Run the shared stdio fixture natively; this does not execute P2 lock instructions."""
import json
import os
import shlex
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CC = shlex.split(os.environ.get('HOST_CC', 'cc'))
manifest = json.loads((ROOT / 'tests/hardware/cases.json').read_text())
suite = next(s for s in manifest['suites'] if s['id'] == 'runtime-stdio-locks')
expected = {name: int(value, 0) for name, value in suite['expected'].items()}
# Never interpose the host's own stdio functions with the target's FILE ABI.
renames = ['fopen', 'freopen', 'fclose', 'fflush', 'fmemopen']
with tempfile.TemporaryDirectory(prefix='p2-stdio-locks-') as temporary:
    out = Path(temporary)
    main = out / 'main.c'
    main.write_text(r"""
#include <stdio.h>
extern void test_body(void);
extern unsigned model_errors;
void observe(const char *name, unsigned value) { printf("%s %08x\n", name, value); }
int main(void) { test_body(); return model_errors != 0; }
""")
    for opt in ['O0', 'O2', 'Os']:
        objects = []
        sources = [ROOT / s for s in suite['sources'] if not s.startswith('libc/string/')]
        sources += [ROOT / 'tests/hardware' / suite['driver'], HERE / 'stdio-lock-model.c']
        for index, source in enumerate(sources):
            obj = out / f'{index}.o'
            flags = ['-' + opt, '-fno-builtin', '-DP2_TEST_HOST', '-include', HERE / 'stdio-lock-model.h']
            flags += ['-D' + name + '=p2_test_' + name for name in renames]
            for directory in suite['include_dirs']:
                flags += ['-I', ROOT / directory]
            subprocess.run([*CC, *flags, '-c', source, '-o', obj], check=True)
            objects.append(obj)
        exe = out / 'test'
        subprocess.run([*CC, *objects, main, '-o', exe], check=True)
        result = subprocess.run([exe], check=True, text=True, capture_output=True, timeout=10)
        observed = {}
        for line in result.stdout.splitlines():
            name, value = line.split()
            assert name not in observed, f'duplicate observation: {name}'
            observed[name] = int(value, 16)
        assert observed == expected, (opt, observed, expected)
        print(opt, 'PASS: stdio lifecycle with native lock model')
