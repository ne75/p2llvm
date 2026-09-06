#!/usr/bin/env python3
"""Build and run shared semantic fixtures, on the host or a P2 over loadp2."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import time
import uuid

from protocol import compare

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def command(argv, log, timeout=120):
    result = subprocess.run(list(map(str, argv)), stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=timeout)
    with log.open('ab') as stream:
        stream.write((repr(list(map(str, argv))) + '\n').encode())
        stream.write(result.stdout)
    if result.returncode:
        raise RuntimeError('command failed; see ' + str(log))
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, default=ROOT / 'build/phase0-llvm')
    parser.add_argument('--runtime-dir', type=Path, default=ROOT / 'build/phase0-libp2')
    parser.add_argument('--mode', choices=['build', 'host', 'hardware'], default='build')
    parser.add_argument('--case', action='append', help='suite ID; repeat to select several')
    parser.add_argument('--optimization', action='append', choices=['O0', 'O2', 'Os'])
    parser.add_argument('--port')
    parser.add_argument('--board', help='board identity included in the result record')
    parser.add_argument('--clock-hz', type=int)
    parser.add_argument('--clock-mode', type=lambda s: int(s, 0))
    parser.add_argument('--baud', type=int, default=115200)
    parser.add_argument('--loader-baud', type=int, default=2000000)
    parser.add_argument('--loader', type=Path, default=ROOT / 'loadp2/bin/loadp2')
    parser.add_argument('--timeout', type=float, default=30)
    args = parser.parse_args()
    if args.mode == 'hardware' and any(v is None for v in (args.port, args.board, args.clock_hz, args.clock_mode)):
        parser.error('hardware mode requires --port, --board, --clock-hz and --clock-mode')
    manifest = json.loads((HERE / 'cases.json').read_text())
    suites = manifest['suites']
    if args.case:
        unknown = set(args.case) - {s['id'] for s in suites}
        if unknown:
            parser.error('unknown case IDs: ' + ', '.join(sorted(unknown)))
        suites = [s for s in suites if s['id'] in args.case]
    build = args.build_dir.resolve()
    out = build / 'p2-hardware' / args.mode
    out.mkdir(parents=True, exist_ok=True)
    results = {'mode': args.mode, 'hardware_execution_attempted': False,
               'board': args.board, 'port': args.port, 'clock_hz': args.clock_hz,
               'clock_mode': args.clock_mode, 'baud': args.baud, 'cases': []}
    for suite in suites:
        for opt in args.optimization or ['O0', 'O2', 'Os']:
            run_id = uuid.uuid4().hex
            case_dir = out / (suite['id'] + '-' + opt)
            case_dir.mkdir(exist_ok=True)
            log = case_dir / 'commands.log'
            log.write_bytes(b'')
            record = {'id': suite['id'], 'optimization': opt, 'run_id': run_id,
                      'status': 'FAIL', 'log': str(log)}
            results['cases'].append(record)
            started = time.monotonic()
            try:
                sources = [ROOT / s for s in suite['sources']] + [HERE / suite['driver'], HERE / 'transport.c']
                record['source_sha256'] = {str(p.relative_to(ROOT)): digest(p) for p in sources}
                record['manifest_sha256'] = digest(HERE / 'cases.json')
                objects = []
                for index, source in enumerate(sources):
                    cxx = source.suffix == '.cpp'
                    compiler = ('clang++' if cxx else 'clang') if args.mode == 'host' else build / 'bin' / ('clang++' if cxx else 'clang')
                    obj = case_dir / (str(index) + '.o')
                    flags = ['-' + opt, '-I', HERE, '-ffunction-sections', '-fdata-sections',
                             '-DP2_TEST_RUN_ID="' + run_id + '"', '-DP2_TEST_BAUD=' + str(args.baud)]
                    if cxx:
                        flags += ['-fno-exceptions', '-fno-rtti']
                    if args.mode == 'host':
                        flags += ['-DP2_TEST_HOST']
                    else:
                        flags += ['--target=p2', '-mllvm', '-verify-machineinstrs']
                    if suite.get('assembly_roundtrip') and index < len(suite['sources']) and args.mode != 'host':
                        assembly = obj.with_suffix('.s')
                        command([compiler, *flags, '-S', source, '-o', assembly], log)
                        command([build / 'bin/llvm-mc', '-triple=p2', '-filetype=obj', assembly, '-o', obj], log)
                    else:
                        command([compiler, *flags, '-c', source, '-o', obj], log)
                    objects.append(obj)
                elf = case_dir / 'test.elf'
                if args.mode == 'host':
                    linker = 'clang++' if any(p.suffix == '.cpp' for p in sources) else 'clang'
                    command([linker, *objects, '-o', elf], log)
                else:
                    record['compiler_sha256'] = digest(build / 'bin/clang')
                    runtime = args.runtime_dir.resolve() / 'lib/libp2.a'
                    record['runtime_sha256'] = digest(runtime)
                    command([build / 'bin/ld.lld', '-T', ROOT / 'libp2/p2.ld', '--gc-sections',
                             *objects, runtime, '-o', elf], log)
                record['elf_sha256'] = digest(elf)
                if args.mode == 'build':
                    record['status'] = 'BUILT_NOT_RUN'
                    continue
                if args.mode == 'host':
                    output = command([elf], log, args.timeout)
                else:
                    binary = case_dir / 'test.bin'
                    command([build / 'bin/llvm-objcopy', '-O', 'binary', elf, binary], log)
                    record['firmware_sha256'] = digest(binary)
                    results['hardware_execution_attempted'] = True
                    output = command([args.loader.resolve(), '-q', '-t', '-ZERO', '-PATCH',
                                      '-p', args.port, '-f', args.clock_hz, '-m', hex(args.clock_mode),
                                      '-b', args.baud, '-l', args.loader_baud, binary], log, args.timeout)
                (case_dir / 'observations.log').write_bytes(output)
                expected = dict(manifest['transport_expected'], **suite['expected'])
                record['observed'], record['mismatches'] = compare(output, run_id, expected)
                record['status'] = 'FAIL' if record['mismatches'] else 'PASS'
            except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
                record['error'] = str(error)
            finally:
                record['seconds'] = round(time.monotonic() - started, 3)
                (out / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
                print(suite['id'], opt, record['status'], record.get('error', ''))
    print('Results:', out / 'results.json')
    return int(any(r['status'] == 'FAIL' for r in results['cases']))


if __name__ == '__main__':
    sys.exit(main())
