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
        # Rev B/C v35, pp. 42-46: POLL/WAIT clear the event unless the
        # sensor sets it again. A past CT target keeps that condition true;
        # rearm a future target to test that the event actually clears.
        # 65536 ticks leave ample time to poll before either future target.
        if op.startswith('POLL'):
            # Each return word packs C or Z from four polls, low bit first:
            # before target / past target / repeated / rearmed = 0,1,1,0.
            yield ['getct r30', 'addct' + n + ' r30, ##65536',
                   'pollct' + n + ' wcz', 'wrc r30', 'wrz r31',
                   'waitx ##65536',
                   'pollct' + n + ' wcz', 'if_c add r30, #2', 'if_z add r31, #2',
                   'pollct' + n + ' wcz', 'if_c add r30, #4', 'if_z add r31, #4',
                   'getct pb', 'addct' + n + ' pb, ##65536',
                   'pollct' + n + ' wcz', 'if_c add r30, #8', 'if_z add r31, #8',
                   'reta'], 6, 6
        else:
            operand = 'r31' if name.endswith('rr') else '##65536'
            # Low word is elapsed CT ticks. High word packs C,Z pairs:
            # before wait / after wait / repeated / rearmed = 0,3,3,0.
            yield ['getct r30', 'mov pa, r30', 'mov r31, ##65536',
                   'addct' + n + ' r30, ' + operand,
                   'pollct' + n + ' wcz', 'wrc r31', 'if_z add r31, #2',
                   'waitct' + n, 'getct r30', 'sub r30, pa',
                   'pollct' + n + ' wcz', 'if_c add r31, #4', 'if_z add r31, #8',
                   'pollct' + n + ' wcz', 'if_c add r31, #16', 'if_z add r31, #32',
                   'getct pb', 'addct' + n + ' pb, ##65536',
                   'pollct' + n + ' wcz', 'if_c add r31, #64', 'if_z add r31, #128',
                   'reta'], {'min': '0x10000', 'max': '0x10400'}, 0x3c
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
