#!/usr/bin/env python3
"""Run P2 tests without installing Python packages or requiring hardware."""
import argparse
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, default=ROOT / 'build/phase0-llvm')
    parser.add_argument('--suite', choices=['all', 'integration', 'llvm', 'instructions'], default='all')
    parser.add_argument('-j', '--jobs', type=int, default=4)
    parser.add_argument('--filter', help='lit test-name regular expression')
    args = parser.parse_args()
    build = args.build_dir.resolve()
    out = build / 'p2-test-results'
    out.mkdir(parents=True, exist_ok=True)
    lit = ROOT / 'llvm-project/llvm/utils/lit/lit.py'
    env = dict(os.environ, PATH=str(build / 'bin') + os.pathsep + os.environ.get('PATH', ''))
    failed = False
    if args.suite in ('all', 'instructions'):
        command = [sys.executable, str(ROOT / 'tests/tools/instruction_matrix.py'),
                   '--bin-dir', str(build / 'bin'), '--output', str(out / 'instructions.json')]
        failed |= subprocess.run(command, cwd=ROOT, env=env).returncode != 0
    suites = []
    if args.suite in ('all', 'integration'):
        suites.append(('integration', [ROOT / 'tests']))
    if args.suite in ('all', 'llvm'):
        suites.append(('llvm', [ROOT / 'llvm-project/llvm/test/MC/P2',
                               ROOT / 'llvm-project/llvm/test/CodeGen/P2']))
    for name, paths in suites:
        command = [sys.executable, str(lit), '-sv', '-j', str(args.jobs),
                   '--param', 'p2_build=' + str(build),
                   '--param', 'build_mode=Release',
                   '--output', str(out / (name + '.json'))]
        if args.filter:
            command.extend(['--filter', args.filter])
        # LLVM's site config selects the build and tools for its own test suites.
        if name == 'llvm':
            command.extend(['--param', 'llvm_site_config=' + str(build / 'test/lit.site.cfg.py')])
        command.extend(map(str, paths))
        failed |= subprocess.run(command, cwd=ROOT, env=env).returncode != 0
    print('Results:', out)
    return int(failed)


if __name__ == '__main__':
    sys.exit(main())
