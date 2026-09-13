"""Build the whole runtime and check every retained LUT section fits the chip."""
from pathlib import Path
import struct
import subprocess
import sys

root, bindir, output = map(lambda p: Path(p).resolve(), sys.argv[1:])
output.mkdir(parents=True, exist_ok=True)
with (output / 'build.log').open('w') as log:
    for command in [
        ['cmake', '-S', root / 'libp2', '-B', output, '-G', 'Ninja', '-Dllvm=' + str(bindir)],
        ['cmake', '--build', output, '--target', 'p2', '--parallel', '4'],
    ]:
        subprocess.run(list(map(str, command)), stdout=log, stderr=subprocess.STDOUT, check=True)

archive = output / 'lib/libp2.a'
members = subprocess.check_output([bindir / 'llvm-ar', 't', archive]).decode().splitlines()
total = 0
for member in members:
    data = subprocess.check_output([bindir / 'llvm-ar', 'p', archive, member])
    assert data[:6] == b'\x7fELF\x01\x01', 'expected P2 little-endian ELF32'
    offset = struct.unpack_from('<I', data, 32)[0]
    entry_size, count, names_index = struct.unpack_from('<HHH', data, 46)
    headers = [struct.unpack_from('<10I', data, offset + i * entry_size) for i in range(count)]
    names_header = headers[names_index]
    names = data[names_header[4]:names_header[4] + names_header[5]]
    for header in headers:
        name = names[header[0]:].split(b'\0', 1)[0].decode()
        if name not in ('lut', '.lut') and not name.startswith('.lut.'):
            continue
        alignment = max(header[8], 1)
        total = (total + alignment - 1) // alignment * alignment + header[5]
        print(f'{member}:{name}: {header[5]} bytes; retained total {total}')

print(f'Whole runtime LUT: {total}/2048 bytes')
if total > 2048:
    raise SystemExit('runtime alone exceeds the P2 LUT; ordinary whole-archive application links will fail')
