"""Generate executable scalar ISA fixtures from the TableGen inventory.

TableGen chooses operand syntax and enumerates records. isa_model.py provides
independent expected values. Generated code/results stay in the build directory.
"""
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re
import subprocess

from isa_model import SCALAR, scalar

ROOT = Path(__file__).resolve().parents[2]


def inventory(bindir):
    source = ROOT / 'llvm-project/llvm'
    argv = [bindir / 'llvm-tblgen', '-dump-json', '-I', source / 'include',
            '-I', source / 'lib/Target/P2', source / 'lib/Target/P2/P2.td']
    data = json.loads(subprocess.check_output(list(map(str, argv))))
    return {name: data[name] for name in data['!instanceof']['Instruction']
            if 'P2Inst' in data[name]['!superclasses'] and not data[name]['isPseudo']}


def render(record, s, n):
    operands = record['OutOperandList']['args'] + record['InOperandList']['args']
    replacements, effect = {}, 0
    for kind, name in operands:
        kind = kind['def']
        if kind == 'P2Cond': replacements[name] = ''
        elif kind == 'P2Effect':
            fixed = record.get(name)
            effect = sum(bit << i for i, bit in enumerate(fixed)) if isinstance(fixed, list) and all(isinstance(bit, int) for bit in fixed) else 3
            if record['AsmString'].split()[1] in {'mul', 'muls'}:
                effect = 1
            replacements[name] = ['', 'wz', 'wc', 'wcz'][effect]
        elif kind == 'P2GPR': replacements[name] = 'r31' if name == 's' else 'r30'
        elif kind == 'i32imm': replacements[name] = '#' + str(n) if name == 'nn' else '##' + str(s)
        elif kind in {'P2Implicit', 'P2RegOrImm32', 'P2RegOrImm64'}: replacements[name] = ''
        else: raise ValueError('ISA fixture operand needs explicit support: ' + kind)
    return re.sub(r'\$(\w+)', lambda m: replacements[m[1]], record['AsmString']).strip(), effect


def generate(bindir, output):
    records = inventory(bindir)
    output.mkdir(parents=True, exist_ok=True)
    groups = defaultdict(list)
    missing = []
    for name, record in records.items():
        mnemonic = record['AsmString'].replace('$cc', '').strip().split()[0].upper()
        if mnemonic not in SCALAR:
            missing.append(name)
            continue
        groups[mnemonic].append((name, record))
    suites = []
    for mnemonic, members in sorted(groups.items()):
        stem = 'isa-' + mnemonic.lower()
        assembly, driver, expected = ['.text'], ['#include "observe.h"'], {}
        calls, covered = [], []
        for name, record in members:
            covered.append(name)
            n = 7 if mnemonic.endswith('NIB') else 3 if mnemonic.endswith('BYTE') else 1
            # Include zero, carry/sign boundaries, and both ends of shift ranges.
            vectors = [(0x80000001, 7), (0xffffffff, 1), (0, 0), (0x89abcdef, 31),
                       (0x89abcdef, 0x12345678), (0x7fffffff, 0x80000001)]
            for vi, (d, s) in enumerate(vectors):
                for flags in range(4):
                    c, z = flags & 1, flags >> 1
                    result = scalar(mnemonic, d, s, c, z, n)
                    if result is None:
                        continue
                    instruction, effect = render(record, s, n)
                    value, nc, nz = result
                    observed_flags = (nc if effect & 2 else c) | ((nz if effect & 1 else z) << 1)
                    function = name + '_' + str(vi) + '_' + str(flags)
                    assembly += ['.globl ' + function, '.type ' + function + ',@function', function + ':',
                                 '    mov r30, #0', '    cmp r30, #' + str(c) + ' wc',
                                 '    mov r30, #' + str(z), '    testb r30, #0 wz',
                                 '    mov r30, ##' + str(d), '    mov r31, ##' + str(s),
                                 '    ' + instruction, '    wrc r31', '    wrz pa',
                                 '    shl pa, #1', '    or r31, pa', '    reta']
                    driver.append('extern unsigned long long ' + function + '(void);')
                    calls.append('    OBSERVE64("' + function + '", ' + function + '());')
                    expected[function + '.lo'], expected[function + '.hi'] = hex(value), hex(observed_flags)
        source, adapter = output / (stem + '.s'), output / (stem + '.c')
        source.write_text('\n'.join(assembly) + '\n')
        adapter.write_text('\n'.join(driver + ['void test_body(void) {'] + calls + ['}']) + '\n')
        suites.append({'id': stem, 'sources': [str(source.relative_to(ROOT))],
                       'driver': str(adapter), 'host_portable': False,
                       'instruction_records': covered, 'expected': expected})
    report = {'inventory': len(records), 'modeled_records': sum(len(g) for g in groups.values()),
              'missing_records': sorted(missing), 'suites': suites,
              'oracle_sha256': hashlib.sha256((Path(__file__).with_name('isa_model.py')).read_bytes()).hexdigest(),
              'generator_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report
