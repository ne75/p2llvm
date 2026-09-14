#!/usr/bin/env python3
"""Compare portable C/C++ kernels with a native oracle and time them on a P2."""
import argparse
import hashlib
import json
from pathlib import Path
import sys
import uuid

HERE = Path(__file__).resolve().parent


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--hardware-dir', type=Path, default=HERE.parent)
    p.add_argument('--bin', type=Path, required=True, help='matching SDK compiler wrappers')
    p.add_argument('--libc', type=Path, required=True, help='matching libc archive directory')
    p.add_argument('--libp2', type=Path, required=True, help='matching libp2 archive directory')
    p.add_argument('--out', type=Path, required=True, help='new directory; never overwrites evidence')
    p.add_argument('--hardware', action='store_true')
    p.add_argument('--no-verifier', action='store_true', help='only for the saved production compiler')
    p.add_argument('--loader', type=Path, default=Path('/opt/p2llvm/bin/loadp2'))
    p.add_argument('--port')
    p.add_argument('--reset', choices=['RTS', 'DTR'], default='RTS')
    p.add_argument('--baud', type=int, default=2000000)
    p.add_argument('--loader-baud', type=int, default=2000000)
    p.add_argument('--fifo', type=int, default=10000)
    p.add_argument('--timeout', type=float, default=300)
    p.add_argument('--clock-hz', type=int, default=160000000)
    p.add_argument('--clock-mode', type=lambda s: int(s, 0), default=None)
    args = p.parse_args()
    sys.path.insert(0, str(args.hardware_dir.resolve()))
    from run import command
    from protocol import compare
    import loader
    args.out.mkdir(parents=True, exist_ok=False)
    sources = [HERE / n for n in ('basics.c', 'basics.cpp', 'driver.c')]
    sources.append(args.hardware_dir / 'transport.c')
    results = {'arguments': {k: str(v) if isinstance(v, Path) else v for k, v in vars(args).items()},
               'source_sha256': {s.name: digest(s) for s in sources + [HERE / 'basics.h']},
               'runtime_sha256': {n: digest(d / n) for n, d in [('libc.a', args.libc), ('libp2.a', args.libp2)]},
               'compiler_driver_sha256': digest(args.bin / 'clang'), 'cases': []}

    def build(directory, opt, run_id, host=False):
        directory.mkdir()
        log = directory / 'commands.log'
        objects = []
        flags = ['-' + opt, '-fno-builtin', '-ffunction-sections', '-fdata-sections',
                 '-I', args.hardware_dir, '-DP2_TEST_RUN_ID="' + run_id + '"',
                 '-DP2_TEST_BAUD=' + str(args.baud)]
        if host:
            flags += ['-DP2_TEST_HOST']
        else:
            flags += ['--target=p2', '-fstack-usage']
            if not args.no_verifier:
                flags += ['-mllvm', '-verify-machineinstrs']
        for index, source in enumerate(sources):
            cxx = source.suffix == '.cpp'
            name = 'clang++' if cxx else 'clang'
            compiler = name if host else args.bin / name
            obj = directory / (str(index) + '.o')
            extra = ['-std=c++17', '-fno-exceptions', '-fno-rtti'] if cxx else []
            command([compiler, *flags, *extra, '-c', source, '-o', obj], log)
            objects.append(obj)
        elf = directory / 'test.elf'
        linker = 'clang++' if host else args.bin / 'clang++'
        linkflags = [] if host else ['--target=p2', '-L' + str(args.libc), '-L' + str(args.libp2),
                                    '-Wl,--gc-sections', '-Wl,-Map=' + str(directory / 'test.map')]
        command([linker, *linkflags, *objects, '-o', elf], log)
        return elf, log

    run_id = uuid.uuid4().hex
    elf, log = build(args.out / 'host', 'O2', run_id, host=True)
    output = command([elf], log, args.timeout)
    (log.parent / 'observations.log').write_bytes(output)
    oracle, _ = compare(output, run_id, {})
    results['host_status'] = 'PASS'
    results['host_observations'] = oracle
    assert all(v == 0 for k, v in oracle.items() if k.endswith('.memory')), 'host buffer oracle failed'
    expected = {k: ({'min': '1', 'max': '0x7fffffff'} if '.cycles' in k else hex(v)) for k, v in oracle.items()}
    expected['clock_hz'] = hex(args.clock_hz)
    (args.out / 'expected.json').write_text(json.dumps(expected, indent=2) + '\n')
    for opt in ('O0', 'O2', 'Oz'):
        run_id = uuid.uuid4().hex
        record = {'optimization': opt, 'run_id': run_id, 'status': 'FAIL'}
        results['cases'].append(record)
        try:
            elf, log = build(args.out / opt, opt, run_id)
            record['elf_sha256'] = digest(elf)
            binary = elf.with_suffix('.bin')
            command([args.bin / 'llvm-objcopy', '-O', 'binary', elf, binary], log)
            record.update(firmware_sha256=digest(binary), firmware_bytes=binary.stat().st_size)
            record['status'] = 'BUILT_NOT_RUN'
            if args.hardware:
                argv = loader.arguments(args, binary)
                record['loader_command'] = argv
                output = command(argv, log, args.timeout, keep_input_open=True)
                (log.parent / 'observations.log').write_bytes(output)
                record.update(loader.identity(output))
                record['observed'], record['mismatches'] = compare(output, run_id, expected)
                record['status'] = 'FAIL' if record['mismatches'] else 'PASS'
        except Exception as error:
            record.update(status='FAIL', error=str(error))
            if hasattr(error, 'output'):
                (args.out / opt / 'failure.log').write_bytes(error.output)
        (args.out / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
        print(opt, record['status'], record.get('error', ''), flush=True)
    return int(any(c['status'] == 'FAIL' for c in results['cases']))


if __name__ == '__main__':
    sys.exit(main())
