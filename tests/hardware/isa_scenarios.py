"""Executable memory, branch, repetition and integer CORDIC ISA scenarios.

Each function returns two raw observations in R30/R31. Scratch memory and LUT
contents are restored before returning. No GPIO or clock configuration is used.
"""
from collections import defaultdict
import math

BRANCHES = {'DJZ', 'DJNZ', 'DJF', 'DJNF', 'IJZ', 'IJNZ', 'TJZ', 'TJNZ'}
MEMORY = {'RDBYTE', 'RDWORD', 'RDLONG', 'WRBYTE', 'WRWORD', 'WRLONG', 'RDLUT', 'WRLUT'}
CORDIC = {'QMUL', 'QDIV', 'QFRAC', 'QSQRT', 'QLOG', 'QEXP'}
SUPPORTED = BRANCHES | MEMORY | CORDIC | {'REP'}


def immediate(record, field):
    return dict((n, k['def']) for k, n in record['OutOperandList']['args'] + record['InOperandList']['args'])[field] != 'P2GPR'


def fields(record, d, s):
    return ('##' + str(d) if immediate(record, 'd') else 'r30') + ', ' + ('##' + str(s) if immediate(record, 's') else 'r31')


def flags():
    return ['wrc r31', 'wrz pa', 'shl pa, #1', 'or r31, pa']


def scenarios(mnemonic, name, record):
    if mnemonic in BRANCHES:
        for i, value in enumerate([0, 1, 0xffffffff]):
            target = '.L' + name + str(i)
            d = (value + (1 if mnemonic.startswith('IJ') else -1 if mnemonic.startswith('DJ') else 0)) & 0xffffffff
            condition = (d == 0xffffffff) if mnemonic in {'DJF', 'DJNF'} else d == 0
            taken = not condition if mnemonic in {'DJNZ', 'DJNF', 'IJNZ', 'TJNZ'} else condition
            operand = '#' + target if immediate(record, 's') else 'r31'
            code = ['mov r30, ##' + str(value), 'mov r31, ##' + target,
                    mnemonic.lower() + ' r30, ' + operand,
                    'mov r31, #0', 'reta', target + ':', 'mov r31, #1', 'reta']
            yield code, d, int(taken)
    elif mnemonic == 'REP':
        for count in [1, 3, 7]:
            # Two repeated instructions, an ADD and a NOP. PA is the sum; D/S
            # stay unchanged for all four immediate/register operand variants.
            code = ['mov pa, #0', 'mov r30, #2', 'mov r31, #' + str(count),
                    'rep ' + fields(record, 2, count), 'add pa, #1', 'nop',
                    'mov r30, pa', 'mov r31, #0', 'reta']
            yield code, count, 0
    elif mnemonic in CORDIC:
        vectors = [(0, 1), (257, 7), (0xffffffff, 1000)]
        if mnemonic == 'QFRAC': vectors = [(0, 3), (1, 3), (7, 257)]
        if mnemonic == 'QSQRT': vectors = [(0, 0), (49, 0), (0, 1)]
        if mnemonic == 'QLOG': vectors = [(1, 0), (2, 0), (256, 0), (0x80000000, 0)]
        if mnemonic == 'QEXP': vectors = [(0, 0), (1 << 27, 0), (8 << 27, 0), (31 << 27, 0)]
        for d, s in vectors:
            operand = ('##' + str(d) if immediate(record, 'd') else 'r30') if mnemonic in {'QLOG', 'QEXP'} else fields(record, d, s)
            code = ['mov r30, ##' + str(d), 'mov r31, ##' + str(s), 'setq #0',
                    mnemonic.lower() + ' ' + operand, 'getqx r30']
            if mnemonic == 'QMUL':
                lo, hi = (d*s) & 0xffffffff, (d*s) >> 32
            elif mnemonic in {'QDIV', 'QFRAC'}:
                lo, hi = divmod(d if mnemonic == 'QDIV' else d << 32, s)
            elif mnemonic == 'QLOG': lo, hi = (d.bit_length()-1) << 27, 0
            elif mnemonic == 'QEXP': lo, hi = 1 << (d >> 27), 0
            else:
                lo, hi = math.isqrt((s << 32) | d), 0
            code += ['mov r31, #0' if mnemonic in {'QSQRT', 'QLOG', 'QEXP'} else 'getqy r31', 'reta']
            yield code, lo, hi
    elif mnemonic in MEMORY:
        lut, reading = mnemonic.endswith('LUT'), mnemonic.startswith('RD')
        read, write = ('rdlut', 'wrlut') if lut else ('rdlong', 'wrlong')
        address = '32' if lut else '.Lmemory'
        width = 8 if mnemonic.endswith('BYTE') else 16 if mnemonic.endswith('WORD') else 32
        for value in [0, 0x89abcdef]:
            effects = [0, 1, 2, 3] if reading else [0]
            for effect in effects:
                initial = value if reading else 0xa55a5aa5
                code = [read + ' pb, ##' + address,
                        write + ' ##' + str(initial) + ', ##' + address,
                        # Initial C=0,Z=1 makes flag writes distinguishable.
                        'mov r30, #0', 'cmp r30, #0 wcz',
                        'mov r30, ##' + str(value), 'mov r31, ##' + address]
                source = '##' + address if immediate(record, 's') else 'r31'
                dest = '##' + str(value) if immediate(record, 'd') else 'r30'
                code += [mnemonic.lower() + ' ' + dest + ', ' + source + ['', ' wz', ' wc', ' wcz'][effect]]
                mask = (1 << width)-1
                if reading:
                    lo = value & mask
                    c = ((lo >> (width-1)) & 1) if effect & 2 else 0
                    z = int(lo == 0) if effect & 1 else 1
                    hi = c | (z << 1)
                else:
                    code += [read + ' r30, ##' + address]
                    lo, hi = (initial & ~mask) | (value & mask), 2
                code += flags() + [write + ' pb, ##' + address, 'reta']
                yield code, lo, hi


def generate_scenarios(records, output, root):
    groups = defaultdict(list)
    for name, record in records.items():
        op = record['AsmString'].replace('$cc', '').strip().split()[0].upper()
        if op in SUPPORTED:
            groups[op].append((name, record))
    suites = []
    for op, members in sorted(groups.items()):
        source, driver = output / ('isa-' + op.lower() + '.s'), output / ('isa-' + op.lower() + '.c')
        assembly, declarations, calls, expected = ['.text'], ['#include "observe.h"'], [], {}
        for name, record in members:
            for i, (code, lo, hi) in enumerate(scenarios(op, name, record)):
                function = name + '_' + str(i)
                assembly += ['.globl ' + function, '.type ' + function + ',@function', function + ':']
                assembly += ['    ' + line for line in code]
                declarations.append('extern unsigned long long ' + function + '(void);')
                calls.append('    OBSERVE64("' + function + '", ' + function + '());')
                expected[function+'.lo'], expected[function+'.hi'] = hex(lo), hex(hi)
        assembly += ['.data', '.balign 4', '.Lmemory:', '.long 0']
        source.write_text('\n'.join(assembly) + '\n')
        driver.write_text('\n'.join(declarations + ['void test_body(void) {'] + calls + ['}']) + '\n')
        suites.append({'id': 'isa-' + op.lower(), 'sources': [str(source.relative_to(root))],
                       'driver': str(driver), 'host_portable': False,
                       'instruction_records': [name for name, _ in members], 'expected': expected})
    return suites
