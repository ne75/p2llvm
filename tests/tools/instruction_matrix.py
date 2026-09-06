#!/usr/bin/env python3
"""Audit every concrete P2 TableGen record against MC and the ISA reference.

TableGen supplies the inventory and operand structure, not an independent ISA
oracle. The CSV independently checks the fixed opcode bits. Handwritten boundary
and semantic regressions complement this structural matrix.
"""
import argparse
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
import csv
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def bits_value(bits):
    return sum(int(bit) << index for index, bit in enumerate(bits))


def reference_patterns():
    result = defaultdict(list)
    with (ROOT / 'utils/p2_instructions_revB.csv').open() as stream:
        for row in csv.reader(stream):
            if len(row) < 4 or not row[1].strip():
                continue
            bits = ''.join(row[3].split())
            if len(bits) != 32:
                continue
            mask = value = 0
            for i, bit in enumerate(reversed(bits)):
                if bit in '01':
                    mask |= 1 << i
                    value |= int(bit) << i
            result[row[1].split()[0].lower()].append((mask, value))
    return result


def make_case(name, record, references):
    operands = record['OutOperandList']['args'] + record['InOperandList']['args']
    types = {n: t['def'] for t, n in operands}
    values = {}
    text = {}
    for n, kind in types.items():
        if kind == 'P2Cond':
            values[n], text[n] = 15, ''
        elif kind == 'P2Effect':
            fixed = record.get(n)
            values[n] = bits_value(fixed) if isinstance(fixed, list) and all(isinstance(b, int) for b in fixed) else 0
            text[n] = ['', 'wz', 'wc', 'wcz'][values[n]]
        elif kind in ('P2Implicit', 'P2RegOrImm32', 'P2RegOrImm64'):
            values[n], text[n] = 0, ''
        elif kind == 'P2GPR':
            values[n] = 0x1d1 if n == 's' else 0x1d0
            text[n] = 'r1' if n == 's' else 'r0'
        elif kind in ('abscalltarget', 'absjmptarget'):
            values[n], text[n] = 0x1000, '#\\4096'
        elif kind in ('relcalltarget', 'reljmptarget', 'jmp9target'):
            values[n], text[n] = 4, '#4'
        elif kind == 'i32imm':
            values[n] = 1 if n in ('nn', 'n23') else 7
            text[n] = '#' + str(values[n])
        else:
            raise ValueError('unsupported operand class ' + kind)
    assembly = re.sub(r'\$(\w+)', lambda m: text[m[1]], record['AsmString']).strip()
    # JMP exposes its address as $d but its encoded field is named a.
    if 'a' not in values and 'd' in values:
        values['a'] = values['d']
    word = 0
    for index, bit in enumerate(record['Inst']):
        if isinstance(bit, int):
            value = bit
        else:
            value = (values[bit['var']] >> bit['index']) & 1
        word |= value << index
    errors = []
    mnemonic = assembly.split()[0]
    patterns = references.get(mnemonic, [])
    if not patterns:
        errors.append('no independent ISA opcode pattern for ' + mnemonic)
    elif not any(word & mask == value for mask, value in patterns):
        errors.append('TableGen opcode disagrees with ISA reference')
    flags = bits_value(record['TSFlags'])
    names = [n for _, n in operands]
    for field, present_bit, offset in [('s', 5, 8), ('d', 6, 11), ('nn', 7, 14)]:
        if flags & (1 << present_bit):
            operand_index = (flags >> offset) & 7
            if operand_index >= len(names) or names[operand_index] != field:
                errors.append('TSFlags ' + field + ' index does not identify its operand')
    return {'instruction': name, 'assembly': assembly, 'expected_word': hex(word), 'errors': errors}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--bin-dir', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    source = ROOT / 'llvm-project/llvm'
    command = [str(args.bin_dir / 'llvm-tblgen'), '-dump-json', '-I', str(source / 'include'),
               '-I', str(source / 'lib/Target/P2'), str(source / 'lib/Target/P2/P2.td')]
    data = json.loads(subprocess.check_output(command))
    references = reference_patterns()
    cases = []
    for name in data['!instanceof']['Instruction']:
        record = data[name]
        if 'P2Inst' in record['!superclasses'] and not record['isPseudo']:
            cases.append(make_case(name, record, references))

    def check(case):
        result = subprocess.run([str(args.bin_dir / 'llvm-mc'), '-triple=p2', '-show-encoding'],
                                input=case['assembly'] + '\n', text=True, capture_output=True, timeout=20)
        encoded = re.findall(r'encoding: \[([^]]+)\]', result.stdout)
        if result.returncode or len(encoded) != 1:
            case['errors'].append('assembler rejected or expanded basic instruction: ' + result.stderr.strip())
        else:
            actual = int.from_bytes(bytes(int(x.strip(), 16) for x in encoded[0].split(',')), 'little')
            case['actual_word'] = hex(actual)
            if actual != int(case['expected_word'], 16):
                case['errors'].append('MC encoding differs from TableGen operand encoding')
        return case

    with ThreadPoolExecutor(max_workers=4) as pool:
        cases = list(pool.map(check, cases))
    failures = [case for case in cases if case['errors']]
    report = {'concrete_instructions': len(cases), 'passed': len(cases) - len(failures),
              'failed': len(failures), 'cases': cases}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print('Instruction matrix: {} passed, {} failed, {} total'.format(report['passed'], len(failures), len(cases)))
    for case in failures:
        print('  ' + case['instruction'] + ': ' + '; '.join(case['errors']))
    return int(bool(failures))


if __name__ == '__main__':
    sys.exit(main())
