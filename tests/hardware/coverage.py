#!/usr/bin/env python3
"""Report executable ISA fixture coverage; optionally enforce completeness."""
import argparse
import csv
import json
from pathlib import Path
import sys

from isa_generate import generate, inventory

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, default=ROOT / 'build/phase0-llvm')
    parser.add_argument('--require-complete', action='store_true')
    args = parser.parse_args()
    build = args.build_dir.resolve()
    records = inventory(build / 'bin')
    generated = generate(build / 'bin', build / 'p2-isa')
    manual = json.loads(Path(__file__).with_name('cases.json').read_text())
    owners = {name: [] for name in records}
    for suite in manual['suites'] + generated['suites']:
        for name in suite.get('instruction_records', []):
            if name not in owners:
                raise ValueError('fixture refers to unknown instruction: ' + name)
            owners[name].append(suite['id'])
    missing = sorted(name for name, suites in owners.items() if not suites)
    out = build / 'p2-test-results'
    out.mkdir(parents=True, exist_ok=True)
    report = {'instruction_records': len(records), 'with_executable_fixture': len(records)-len(missing),
              'missing_records': missing, 'hardware_execution_claimed': False,
              'note': 'Fixture readiness is not hardware execution. Consult a matching hardware-mode result record.'}
    (out / 'hardware-coverage.json').write_text(json.dumps(report, indent=2) + '\n')
    with (out / 'hardware-coverage.csv').open('w', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['instruction', 'fixture_readiness', 'suites'])
        for name, suites in sorted(owners.items()):
            writer.writerow([name, 'READY_NOT_HARDWARE_VALIDATED' if suites else 'MISSING', ';'.join(suites)])
    print(f"Executable ISA fixtures: {report['with_executable_fixture']}/{len(records)}; {len(missing)} missing")
    print('Report:', out / 'hardware-coverage.csv')
    return int(args.require_complete and bool(missing))


if __name__ == '__main__':
    sys.exit(main())
