"""Event fixtures using counter targets and LUT reads, without GPIO wiring."""

SUPPORTED = {prefix + str(n) for prefix, count in
             [('ADDCT', 3), ('POLLCT', 3), ('WAITCT', 3),
              ('SETSE', 4), ('POLLSE', 4), ('WAITSE', 4)]
             for n in range(1, count+1)} | {'POLLQMT'}


def scenarios(op, name, record):
    if op == 'POLLQMT':
        # Drain both results, then deliberately read an empty CORDIC. The
        # unspecified data is ignored; only the documented event is observed.
        yield ['qmul #3, #7', 'getqx pa', 'getqy pb', 'getqx pa',
               'pollqmt wcz', 'wrc r30', 'wrz r31',
               'pollqmt wcz', 'if_c add r30, #2', 'if_z add r31, #2', 'reta'], 1, 1
    elif 'CT' in op:
        n = op[-1]
        if op.startswith('POLL'):
            yield ['getct r30', 'addct' + n + ' r30, #32', 'waitx #100',
                   'pollct' + n + ' wcz', 'wrc r30', 'wrz r31',
                   'pollct' + n + ' wcz', 'if_c add r30, #2', 'if_z add r31, #2', 'reta'], 1, 1
        else:
            operand = 'r31' if name.endswith('rr') else '##65536'
            yield ['getct r30', 'mov pa, r30', 'mov r31, ##65536',
                   'addct' + n + ' r30, ' + operand, 'waitct' + n,
                   'getct r30', 'sub r30, pa', 'pollct' + n + ' wcz',
                   'wrc r31', 'if_z add r31, #2', 'reta'], {'min': '0x10000', 'max': '0x10400'}, 0
    else:
        n = op[-1]
        operand = 'r31' if name.endswith('r') else '#3'
        # Configuration 3 selects this cog reading LUT address 511. Do not
        # modify LUT contents. Pack initial/after-wait/positive/re-poll flags.
        code = ['mov r30, #0', 'mov r31, #3', 'setse' + n + ' ' + operand,
                'pollse' + n + ' wcz', 'if_c add r30, #1', 'if_z add r30, #2',
                'rdlut pa, ##511', 'waitse' + n,
                'pollse' + n + ' wcz', 'if_c add r30, #4', 'if_z add r30, #8',
                'rdlut pa, ##511', 'pollse' + n + ' wcz',
                'if_c add r30, #16', 'if_z add r30, #32',
                'pollse' + n + ' wcz', 'wrc r31', 'if_z add r31, #2',
                'setse' + n + ' #0', 'reta']
        yield code, 48, 0
