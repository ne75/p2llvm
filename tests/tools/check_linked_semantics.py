"""Check final fields against ELF symbol addresses and independent ISA rules."""
from pathlib import Path
import struct
import sys


class ELF:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        assert self.data[:6] == b'\x7fELF\x01\x01', 'expected little-endian ELF32'
        shoff = struct.unpack_from('<I', self.data, 32)[0]
        size, count = struct.unpack_from('<HH', self.data, 46)
        self.sections = [struct.unpack_from('<10I', self.data, shoff + i*size) for i in range(count)]
        self.symbols = {}
        for section in self.sections:
            if section[1] != 2:
                continue
            strings = self.sections[section[6]]
            text = self.data[strings[4]:strings[4]+strings[5]]
            for offset in range(section[4], section[4]+section[5], section[9]):
                name, value, _, _, _, _ = struct.unpack_from('<IIIBBH', self.data, offset)
                name = text[name:text.find(b'\0', name)].decode()
                self.symbols[name] = value

    def read(self, address, size=4):
        for section in self.sections:
            if section[1] != 8 and section[3] <= address and address+size <= section[3]+section[5]:
                offset = section[4] + address-section[3]
                return int.from_bytes(self.data[offset:offset+size], 'little')
        raise ValueError('address absent from ELF: ' + hex(address))


def check(path):
    elf = ELF(path)
    sym = elf.symbols
    def word(function, index=0): return elf.read(sym[function] + 4*index)
    for function, target in [('asm_address_add', sym['probe_data']+528),
                             ('asm_address_sub', sym['probe_data']-1)]:
        assert word(function) & 0x0f800000 == 0x0f000000, 'missing AUGS'
        assert word(function) & 0x7fffff == target >> 9
        assert word(function, 1) & 511 == target & 511
    target = sym['probe_data'] + 528
    slot = sym['probe_slot']
    assert word('asm_both_fields') & 0x0f800000 == 0x0f800000, 'missing AUGD'
    assert word('asm_both_fields') & 0x7fffff == target >> 9
    assert word('asm_both_fields', 1) & 0x7fffff == slot >> 9
    assert word('asm_both_fields', 2) >> 9 & 511 == target & 511
    assert word('asm_both_fields', 2) & 511 == slot & 511
    assert word('asm_call_sqrt') & 0xfffff == sym['sqrt'], 'HUB function name mistaken for LUT'
    assert word('asm_call_lut') & 0xfffff == 0x200 + (sym['lut_probe']-sym['__p2_lut_load_start'])//4
    assert elf.read(sym['probe_delta32']) == 8
    assert elf.read(sym['probe_delta64'], 8) == 0x800000008


if __name__ == '__main__':
    check(sys.argv[1])
