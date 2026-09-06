"""Executable memory, branch, repetition and integer CORDIC ISA scenarios.

Each function returns two raw observations in R30/R31. Scratch memory and LUT
contents are restored before returning. No GPIO or clock configuration is used.
"""
from collections import defaultdict
import math
import isa_events
import isa_fifo

BRANCHES = {'DJZ', 'DJNZ', 'DJF', 'DJNF', 'IJZ', 'IJNZ', 'TJZ', 'TJNZ'}
MEMORY = {'RDBYTE', 'RDWORD', 'RDLONG', 'WRBYTE', 'WRWORD', 'WRLONG', 'RDLUT', 'WRLUT'}
CORDIC = {'QMUL', 'QDIV', 'QFRAC', 'QSQRT', 'QLOG', 'QEXP', 'QROTATE', 'QVECTOR'}
SUPPORTED = BRANCHES | MEMORY | CORDIC | isa_events.SUPPORTED | isa_fifo.SUPPORTED | {'REP', 'ALTS', 'ALTD', 'SETQ2', 'CALL', 'CALLA', 'JMP', 'RETB', 'XORO32'}


def immediate(record, field):
    return dict((n, k['def']) for k, n in record['OutOperandList']['args'] + record['InOperandList']['args'])[field] != 'P2GPR'


def fields(record, d, s):
    return ('##' + str(d) if immediate(record, 'd') else 'r30') + ', ' + ('##' + str(s) if immediate(record, 's') else 'r31')


def flags():
    return ['wrc r31', 'wrz pa', 'shl pa, #1', 'or r31, pa']


def scenarios(mnemonic, name, record):
    if mnemonic == 'XORO32':
        # Rev B/C xoroshiro32++ [13,5,10,9], two 16-bit iterations.
        # Chip Gracey's implementation and published seed-1 sequence:
        # https://forums.parallax.com/discussion/comment/1448460/
        # https://forums.parallax.com/discussion/168188/xoroshiro-random-number-generator
        def rotate(x, count):
            return ((x << count) | (x >> (16-count))) & 0xffff
        for seed in [1, 0x12345678, 0xffffffff, 0x80000000]:
            state, result = seed, 0
            for shift in [0, 16]:
                a, b = state & 0xffff, state >> 16
                result |= ((rotate((a+b) & 0xffff, 9)+a) & 0xffff) << shift
                b ^= a
                state = (rotate(a, 13) ^ ((b << 5) & 0xffff) ^ b) | (rotate(b, 10) << 16)
            if seed == 1:
                assert result == 0x62690201
            yield ['mov r30, ##' + str(seed), 'xoro32 r30', 'mov r31, #0', 'reta'], state, result
    elif mnemonic in isa_fifo.SUPPORTED:
        yield from isa_fifo.scenarios(mnemonic, name, record)
    elif mnemonic in isa_events.SUPPORTED:
        yield from isa_events.scenarios(mnemonic, name, record)
    elif mnemonic in {'ALTS', 'ALTD'}:
        for packed, delta in [(512, 1), (0x3fe00, -1)]:
            # S[17:9] is a signed increment; S[8:0] is the register offset.
            # Redirect the next instruction to PB (COG address 0x1f7).
            source = '##' + str(packed) if immediate(record, 's') else 'r31'
            code = ['mov pb, #42', 'mov r30, ##0x1f7', 'mov r31, ##' + str(packed),
                    mnemonic.lower() + ' r30, ' + source, 'mov pa, r31',
                    'mov r31, r30', 'mov r30, ' + ('pa' if mnemonic == 'ALTS' else 'pb')]
            if immediate(record, 's'):
                # Rev B/C v35, p. 4: ALTx #S uses AUGS without cancelling it.
                # Results are captured; consume it in scratch PA so it cannot
                # augment an immediate in the C caller after RETA.
                code += ['mov pa, #0']
            code += ['reta']
            yield code, 42 if mnemonic == 'ALTS' else packed, 0x1f7 + delta
    elif mnemonic == 'SETQ2':
        operand = '#1' if immediate(record, 'd') else 'r31'
        code = ['rdlut pb, #32', 'rdlut pa, #33', 'mov r31, #1',
                'setq2 ' + operand, 'rdlong $32, ##.Lblock',
                'rdlut r30, #32', 'rdlut r31, #33', 'wrlut pb, #32', 'wrlut pa, #33', 'reta']
        yield code, 0x12345678, 0x9abcdef0
    elif mnemonic in {'CALL', 'CALLA', 'JMP'}:
        target = '.L' + name + '_target'
        suffix = name[len(mnemonic):]
        if suffix == 'r': operand = 'r31'
        elif suffix == 'a' or mnemonic == 'CALLA': operand = '#\\' + target
        else: operand = '#' + target
        code = ['mov r30, #9', 'mov r31, ##' + target, mnemonic.lower() + ' ' + operand]
        if mnemonic == 'JMP':
            code += ['add r30, #128', target + ':', 'add r30, #19', 'mov r31, #0', 'reta']
            yield code, 28, 0
        else:
            code += ['add r30, #7', 'mov r31, #0', 'reta', target + ':', 'add r30, #19',
                     'reta' if mnemonic == 'CALLA' else 'ret']
            yield code, 35, 0
    elif mnemonic == 'RETB':
        code = ['mov pb, ptrb', 'wrlong ##.Lretb_after, ##.Lmemory',
                'mov ptrb, ##.Lmemory+4', 'retb', '.Lretb_after:',
                'mov ptrb, pb', 'mov r30, #73', 'mov r31, #0', 'reta']
        yield code, 73, 0
    elif mnemonic in BRANCHES:
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
        if mnemonic == 'QROTATE': vectors = [(1 << 24, 0x10000000), (1 << 24, 0x20000000)]
        if mnemonic == 'QVECTOR': vectors = [(3 << 20, 4 << 20), (4 << 20, 3 << 20)]
        if mnemonic == 'QEXP': vectors = [(0, 0), (1 << 27, 0), (8 << 27, 0), (31 << 27, 0)]
        for d, s in vectors:
            operand = ('##' + str(d) if immediate(record, 'd') else 'r30') if mnemonic in {'QLOG', 'QEXP'} else fields(record, d, s)
            code = ['mov r30, ##' + str(d), 'mov r31, ##' + str(s), 'setq #0',
                    mnemonic.lower() + ' ' + operand, 'getqx r30']
            if mnemonic == 'QMUL':
                lo, hi = (d*s) & 0xffffffff, (d*s) >> 32
            elif mnemonic in {'QDIV', 'QFRAC'}:
                lo, hi = divmod(d if mnemonic == 'QDIV' else d << 32, s)
            elif mnemonic in {'QROTATE', 'QVECTOR'}:
                # Deliberately coarse quadrant/scale checks. This acceptance
                # window is not a claimed silicon accuracy specification.
                def window(value):
                    center = round(value)
                    return {'min': hex(center-4096), 'max': hex(center+4096)}
                if mnemonic == 'QROTATE':
                    angle = s * (2*math.pi) / (1 << 32)
                    lo, hi = window(d*math.cos(angle)), window(d*math.sin(angle))
                else:
                    lo = window(math.hypot(d, s))
                    hi = window(math.atan2(s, d)*(1 << 32)/(2*math.pi))
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
        if op in isa_fifo.SUPPORTED:
            assembly = ['.section .lut.fixture,"ax",@progbits']
        for name, record in members:
            for i, (code, lo, hi) in enumerate(scenarios(op, name, record)):
                function = name + '_' + str(i)
                assembly += ['.globl ' + function, '.type ' + function + ',@function', function + ':']
                assembly += ['    ' + line for line in code]
                declarations.append('extern unsigned long long ' + function + '(void);')
                calls.append('    OBSERVE64("' + function + '", ' + function + '());')
                expected[function+'.lo'] = hex(lo) if isinstance(lo, int) else lo
                expected[function+'.hi'] = hex(hi) if isinstance(hi, int) else hi
        assembly += ['.data', '.balign 4', '.Lmemory:', '.long 0', '.Lblock:', '.long 0x12345678, 0x9abcdef0']
        if op in isa_fifo.SUPPORTED:
            assembly += ['.balign 4', '.Lfifo:', '.zero 64']
        source.write_text('\n'.join(assembly) + '\n')
        driver.write_text('\n'.join(declarations + ['void test_body(void) {'] + calls + ['}']) + '\n')
        suites.append({'id': 'isa-' + op.lower(), 'sources': [str(source.relative_to(root))],
                       'driver': str(driver), 'host_portable': False,
                       'instruction_records': [name for name, _ in members] + (['RET'] if op == 'CALL' else ['GETQX', 'GETQY', 'SETQi'] if op == 'QMUL' else []), 'expected': expected})
    return suites
