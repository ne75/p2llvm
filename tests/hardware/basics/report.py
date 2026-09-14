#!/usr/bin/env python3
"""Render a production/cleanup basic-benchmark comparison as Markdown."""
import argparse
import json
from pathlib import Path
import statistics


def frames(directory):
    result = {}
    for source in directory.glob('*.su'):
        for line in source.read_text().splitlines():
            location, size, kind = line.split('\t')
            symbol = location.rsplit(':', 1)[-1]
            if symbol.startswith('basic_'):
                result[symbol[6:]] = int(size)
    return result


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('production', type=Path)
    p.add_argument('cleanup', type=Path)
    args = p.parse_args()
    a = json.loads((args.production / 'results.json').read_text())
    b = json.loads((args.cleanup / 'results.json').read_text())
    if a['source_sha256'] != b['source_sha256']:
        p.error('different fixture sources; a timing comparison would be misleading')
    for key in ('clock_hz', 'clock_mode'):
        if a['arguments'][key] != b['arguments'][key]:
            p.error('different clock configuration')
    print('# Basic hardware comparison\n')
    print('Cycles are median batch cycles / 64 calls (five samples). They include the '
          'indirect call, loop and timer overhead; the empty case shows that cost. '
          'Do not interpret these as isolated instruction latency. Lower is faster.\n')
    print('Frames are compiler-reported bytes for each kernel, including its fixed '
          'frame and register saves. They exclude nested calls and do not measure '
          'peak stack use. Memory fills include write completion in both profiles.\n')
    print(f"Requested clock: {a['arguments']['clock_hz']} Hz. Runtime libraries use Oz;")
    print('the optimization rows refer to the kernels and harness.\n')
    baseline = {r['optimization']: r for r in a['cases']}
    for current in b['cases']:
        opt = current['optimization']
        original = baseline[opt]
        print(f"## {opt}: {original['status']} / {current['status']}\n")
        print(f"Firmware: {original.get('firmware_bytes')} → {current.get('firmware_bytes')} bytes.\n")
        if original['status'] != 'PASS' or current['status'] != 'PASS':
            print('See results.json for the failed or unexecuted checks.\n')
            continue
        print('| Case | Production cycles/call | Cleanup cycles/call | Change | Frame bytes P → C |')
        print('|---|---:|---:|---:|---:|')
        fa, fb = frames(args.production / opt), frames(args.cleanup / opt)
        for key in original['observed']:
            if not key.endswith('.hash'):
                continue
            name = key[:-5]
            old, new = [statistics.median(row['observed'][name + '.cycles' + str(i)]
                        for i in range(5)) / 64 for row in (original, current)]
            print(f'| {name} | {old:.1f} | {new:.1f} | {(new / old - 1) * 100:+.1f}% | '
                  f'{fa.get(name, "?")} → {fb.get(name, "?")} |')
        print()


if __name__ == '__main__':
    main()
