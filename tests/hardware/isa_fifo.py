"""FIFO observations executed from LUT, leaving HUB instruction fetch available.

RDFAST with D[31]=0 waits for pending writes before readback. Never return to
HUB code with undrained WRFAST writes. Each generated suite fits the LUT image.
"""

SUPPORTED = {'RDFAST', 'WRFAST', 'RFBYTE', 'RFWORD', 'RFLONG',
             'WFBYTE', 'WFWORD', 'WFLONG', 'POLLFBW'}


def scenarios(op, name, record):
    operands = dict((n, k['def']) for k, n in
                    record['OutOperandList']['args'] + record['InOperandList']['args'])
    if op == 'POLLFBW':
        # A one-block FIFO wraps after 64 bytes. Consume a whole block to
        # ensure the prefetcher has wrapped before observing its event.
        yield ['rdfast #1, ##.Lfifo', 'rep #1, #16', 'rflong pa',
               'pollfbw wcz', 'wrc r30', 'wrz r31', 'reta'], 1, 1
    elif op in {'RDFAST', 'WRFAST'}:
        d = 'r30' if operands['d'] == 'P2GPR' else '#0'
        s = 'r31' if operands['s'] == 'P2GPR' else '##.Lfifo'
        code = ['wrlong ##0x89abcdef, ##.Lfifo', 'mov r30, #0',
                'mov r31, ##.Lfifo', op.lower() + ' ' + d + ', ' + s]
        if op == 'WRFAST':
            code += ['wflong ##0x12345678', 'rdfast #0, ##.Lfifo']
        code += ['rflong r30', 'mov r31, #0', 'reta']
        yield code, 0x12345678 if op == 'WRFAST' else 0x89abcdef, 0
    elif op.startswith('RF'):
        width = {'RFBYTE': 8, 'RFWORD': 16, 'RFLONG': 32}[op]
        for value in [0, 0x89abcdef]:
            for effect in range(4):
                code = ['wrlong ##' + str(value) + ', ##.Lfifo',
                        'rdfast #0, ##.Lfifo', 'mov r30, #0', 'cmp r30, #0 wcz',
                        op.lower() + ' r30' + ['', ' wz', ' wc', ' wcz'][effect],
                        'wrc r31', 'wrz pa', 'shl pa, #1', 'or r31, pa', 'reta']
                lo = value & ((1 << width)-1)
                c = (lo >> (width-1)) if effect & 2 else 0
                z = int(lo == 0) if effect & 1 else 1
                yield code, lo, c | (z << 1)
    else:
        width = {'WFBYTE': 8, 'WFWORD': 16, 'WFLONG': 32}[op]
        value = 0x89abcdef
        operand = 'r30' if operands['d'] == 'P2GPR' else '##' + str(value)
        code = ['wrlong #0, ##.Lfifo', 'rdfast #0, ##.Lfifo',
                'wrfast #0, ##.Lfifo', 'mov r30, ##' + str(value)]
        code += [op.lower() + ' ' + operand] * (32//width)
        code += ['rdfast #0, ##.Lfifo', 'rflong r30', 'mov r31, #0', 'reta']
        lo = sum((value & ((1 << width)-1)) << shift for shift in range(0, 32, width))
        yield code, lo, 0
