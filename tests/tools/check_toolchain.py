#!/usr/bin/env python3
"""Hermetic driver/link checks: never borrow an installed production SDK."""
from pathlib import Path
import subprocess
import sys
import tempfile

bindir = Path(sys.argv[1]).resolve()

def run(tool, *args):
    p = subprocess.run([str(bindir / tool), *map(str, args)], text=True,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    assert p.returncode == 0, p.stderr
    return p.stdout + p.stderr

with tempfile.TemporaryDirectory(prefix='p2-sdk-') as directory:
    work = Path(directory)
    obj = work / 'input.o'
    source = work / 'input.c'
    source.write_text('void __entry(void) {}\n')
    run('clang', '--target=p2', '-c', source, '-o', obj)
    sdks = [work / 'sdk one', work / 'sdk two']
    for number, sdk in enumerate(sdks):
        for library, parent in [('c', 'libc'), ('p2', 'libp2'), ('p2db', 'libp2')]:
            libdir = sdk / parent / 'lib'
            libdir.mkdir(parents=True, exist_ok=True)
            headerdir = sdk / parent / 'include'
            headerdir.mkdir(exist_ok=True)
            (headerdir / (library + '.h')).write_text(f'#define SDK_{library} {number}\n')
            source.write_text(f'int sdk{number}_{library} = {number + 1};\n')
            marker = work / (library + '.o')
            run('clang', '--target=p2', '-c', source, '-o', marker)
            run('llvm-ar', 'rcs', libdir / ('lib' + library + '.a'), marker)
        for script in ('p2.ld', 'p2_debug.ld'):
            (sdk / script).write_text('ENTRY(__entry)\nSECTIONS { . = 0xa00; .text : { *(.text*) } .data : { *(.data*) } }\n')

    for number, sdk in enumerate(sdks):
        flags = ['--target=p2', '--sysroot=' + str(sdk)]
        output = work / 'image.elf'
        run('clang', *flags, obj, '-o', output)
        symbols = run('llvm-readobj', '--symbols', output)
        assert f'sdk{number}_c' in symbols and f'sdk{number}_p2' in symbols
        assert f'sdk{1-number}_' not in symbols and f'sdk{number}_p2db' not in symbols
        run('clang', *flags, '-mp2db', obj, '-o', output)
        assert f'sdk{number}_p2db' in run('llvm-readobj', '--symbols', output)
        trace = run('clang', *flags, '-mp2db', '-###', obj, '-o', output)
        assert str(sdk / 'p2_debug.ld') in trace
        assert '/opt/p2llvm' not in trace and '/opt/p2/' not in trace
        source.write_text('#include <c.h>\n#include <p2.h>\n'
                          f'_Static_assert(SDK_c == {number} && SDK_p2 == {number}, "wrong SDK");\n')
        run('clang', *flags, '-fsyntax-only', source)

    sdk = sdks[0]
    flags = ['--target=p2', '--sysroot=' + str(sdk)]
    output = work / 'image.elf'
    # Explicit library directories override SDK defaults.
    run('clang', *flags, '-L' + str(sdks[1] / 'libc/lib'), obj, '-o', output)
    symbols = run('llvm-readobj', '--symbols', output)
    assert 'sdk1_c' in symbols and 'sdk0_c' not in symbols and 'sdk0_p2' in symbols
    custom = work / 'custom.ld'
    custom.write_text('ENTRY(__entry)\nSECTIONS { . = 0x1000; .text : { *(.text*) } .data : { *(.data*) } }\n')
    for scriptargs in (['-T', str(custom)], ['-Wl,-T,' + str(custom)],
                       ['-Xlinker', '--script=' + str(custom)]):
        run('clang', *flags, *scriptargs, obj, '-o', output)
        assert 'Address: 0x1000' in run('llvm-readobj', '--sections', output)
        trace = run('clang', *flags, '-###', *scriptargs, obj, '-o', output)
        assert str(sdk / 'p2.ld') not in trace
    for option in ('-nostdlib', '-nodefaultlibs', '-r'):
        run('clang', *flags, option, obj, '-o', output)
        assert 'sdk0_' not in run('llvm-readobj', '--symbols', output)
        trace = run('clang', *flags, option, '-###', obj, '-o', output)
        assert '"-lc"' not in trace and '"-lp2"' not in trace
        if option != '-nodefaultlibs':
            assert str(sdk / 'p2.ld') not in trace
        if option == '-r':
            assert 'Type: Relocatable' in run('llvm-readobj', '--file-headers', output)
    error = subprocess.run([str(bindir / 'clang'), *flags, '-nostartfiles', str(obj)],
                           text=True, capture_output=True)
    assert error.returncode != 0 and 'unsupported option' in error.stderr
    # A missing SDK must fail instead of silently using production libraries.
    error = subprocess.run([str(bindir / 'clang'), '--target=p2',
                            '--sysroot=' + str(work / 'missing'), str(obj), '-o', str(output)],
                           text=True, capture_output=True)
    assert error.returncode != 0
    # A compiler next to a relocated SDK chooses that SDK without --sysroot.
    relocated = sdk / 'bin'
    relocated.mkdir()
    for name in ('clang', 'ld.lld'):
        (relocated / name).symlink_to(bindir / name)
    p = subprocess.run([str(relocated / 'clang'), '--target=p2', '-no-canonical-prefixes',
                        str(obj), '-o', str(output)], text=True, capture_output=True)
    assert p.returncode == 0, p.stderr
    assert 'sdk0_p2' in run('llvm-readobj', '--symbols', output)
print('P2 SDK headers, libraries, scripts, overrides and partial links: PASS')
