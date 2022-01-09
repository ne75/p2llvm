import csv
import re
import struct
import os
import argparse

Registers = {
    'r0': 0x1d0,
    'r1': 0x1d1,
    'r2': 0x1d2,
    'r3': 0x1d3,
    'r4': 0x1d4,
    'r5': 0x1d5,
    'r6': 0x1d6,
    'r7': 0x1d7,
    'r8': 0x1d8,
    'r9': 0x1d9,
    'r10': 0x1da,
    'r11': 0x1db,
    'r12': 0x1dc,
    'r13': 0x1dd,
    'r14': 0x1de,
    'r15': 0x1df,
    'r16': 0x1e0,
    'r17': 0x1e1,
    'r18': 0x1e2,
    'r19': 0x1e3,
    'r20': 0x1e4,
    'r21': 0x1e5,
    'r22': 0x1e6,
    'r23': 0x1e7,
    'r24': 0x1e8,
    'r25': 0x1e9,
    'r26': 0x1ea,
    'r27': 0x1eb,
    'r28': 0x1ec,
    'r29': 0x1ed,
    'r30': 0x1ee,
    'r31': 0x1ef,
}

Conditions = {
    '_ret_': 0x0,
    'if_nc_and_nz': 0x1,
    'if_nc_and_z': 0x2,
    'if_nc': 0x3,
    'if_c_and_nz': 0x4,
    'if_nz': 0x5,
    'if_c_ne_z': 0x6,
    'if_nc_or_nz': 0x7,
    'if_c_and_z': 0x8,
    'if_c_eq_z': 0x9,
    'if_z': 0xa,
    'if_nc_or_z': 0xb,
    'if_c': 0xc,
    'if_c_or_nz': 0xd,
    'if_c_or_z': 0xe,
    'always': 0xf
}

class Instruction():
    def __init__(self, e, op, czi, d, s) -> None:
        # store in most significant bit first for ease of readability and assignability
        self.e = e
        self.op = op
        self.czi = czi
        self.d = d
        self.s = s

        self.encoding_bits = self._to_bits(self.e, 4)  \
                + self._to_bits(self.op, 7) \
                + self._to_bits(self.czi, 3) \
                + self._to_bits(self.d, 9)  \
                + self._to_bits(self.s, 9)

    def encoding(self):
        enc = 0

        for i in range(32):
            enc += self.encoding_bits[i] << 31-i

        return struct.unpack("<I", struct.pack(">I", enc))[0]

    def _to_bits(self, n, l) -> list:
        s = format(n, '0' + str(l) + 'b')
        return [int(i, 2) for i in list(s)]

    def _to_reg(self, r):
        '''
        convert r to register mnemonic
        '''
        for reg in Registers:
            if Registers[reg] == r:
                return reg

        return '$' + str(r)

    def _to_cond(self, e):
        '''
        convert e to condition prefix
        '''
        if e == 0xf:
            return ''

        for cond in Conditions:
            if Conditions[cond] == e:
                return cond

        return '<unknown condition>'


    def _to_effect(self, cz):
        '''
        convert cz to wc/wz/wcz
        '''
        if (cz == 0):
            return ''

        if (cz == 1):
            return 'wz'

        if (cz == 2):
            return 'wc'

        if (cz == 3):
            return 'wcz'

        return ''


class InstNOP(Instruction):
    pattern = r'^0{32}$'

    def __init__(self) -> None:
        super().__init__(0, 0, 0, 0, 0)

    def get_vals(r):
        return {};

    def __str__(self):
        return self.mnemonic

class InstCZIDS(Instruction):
    pattern = r'^EEEE ([0-1]{7}) CZI D{9} S{9}$'

    def __init__(self, e, c, z, i, d, s) -> None:
        self.c = c
        self.z = z
        self.i = i
        super().__init__(e, self.opc, (c<<2) + (z<<1) + i, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ' ' + self._to_effect((self.c << 1) + self.z)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ' ' + self._to_effect((self.c << 1) + self.z)).strip()

class Inst3NIDS(Instruction):
    pattern = '^EEEE ([0-1]{6})N NNI D{9} S{9}$'

    def __init__(self, e, n, i, d, s) -> None:
        self.n = n
        self.i = i
        op = (self.opc << 1) + ((n >> 2) & 1)
        czi = ((n & 0x3) << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ', #' + str(self.n)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ', #' + str(self.n)).strip()

class Inst2NIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) NNI D{9} S{9}$'

    def __init__(self, e, n, i, d, s) -> None:
        self.n = n
        self.i = i
        op = self.opc
        czi = (n << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ', #' + str(self.n)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ', #' + str(self.n)).strip()

class Inst1NIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) ([0-1])NI D{9} S{9}$'

    def __init__(self, e, n, i, d, s) -> None:
        self.n = n
        self.i = i
        op = self.opc >> 1
        czi = ((self.opc & 1) << 2) + (n << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 1) + int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ', #' + str(self.n)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ', #' + str(self.n)).strip()

class InstIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) ([0-1]{2})I D{9} S{9}$'

    def __init__(self, e, i, d, s) -> None:
        self.i = i
        op = self.opc >> 2
        czi = ((self.opc & 0x3) << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 2) + int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s)).strip()

class InstZIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) ([0-1])ZI D{9} S{9}$'

    def __init__(self, e, z, i, d, s) -> None:
        self.z = z
        self.i = i
        op = self.opc >> 1
        czi = ((self.opc & 1) << 2) + (z << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 1) + int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ' ' + self._to_effect(self.z)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ' ' + self._to_effect(self.z)).strip()

class InstCIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) C([0-1])I D{9} S{9}$'

    def __init__(self, e, c, i, d, s) -> None:
        self.c = c
        self.i = i
        op = self.opc >> 1
        czi = (c << 2) + ((self.opc & 1) << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 1) + int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ' ' + self._to_effect(self.c << 1)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ' ' + self._to_effect(self.c << 1)).strip()

class InstLIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) ([0-1])LI D{9} S{9}$'

    def __init__(self, e, l, i, d, s) -> None:
        self.l = l
        self.i = i
        op = self.opc >> 1
        czi = ((self.opc & 1) << 2) + (l << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 1) + int(r.group(2), 2)
        }

    def __str__(self):
        if self.i and self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ', #' + str(self.s)).strip()

        if self.i and not self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s)).strip()

        if not self.i and self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ', ' + self._to_reg(self.s)).strip()

        if not self.i and not self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s)).strip()

class InstIS(Instruction):
    pattern = '^EEEE ([0-1]{7}) ([0-1]{2})I ([0-1]{9}) S{9}$'

    def __init__(self, e, i, s) -> None:
        self.i = i
        op = self.opc >> 2
        czi = ((self.opc & 0x3) << 1) + i
        d = self.subopc1
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 2) + int(r.group(2), 2),
            'subopc1': int(r.group(3), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.i:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.s)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.s)).strip()

class InstCLIDS(Instruction):
    pattern = '^EEEE ([0-1]{7}) CLI D{9} S{9}$'

    def __init__(self, e, c, l, i, d, s) -> None:
        self.c = c
        self.l = l
        self.i = i
        op = self.opc
        czi = (c << 2) + (l << 1) + i
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

    def __str__(self):
        if self.i and self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ', #' + str(self.s) + ' ' + self._to_effect(self.c << 1)).strip()

        if self.i and not self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', #' + str(self.s) + ' ' + self._to_effect(self.c << 1)).strip()

        if not self.i and self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ', ' + self._to_reg(self.s) + ' ' + self._to_effect(self.c << 1)).strip()

        if not self.i and not self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ', ' + self._to_reg(self.s) + ' ' + self._to_effect(self.c << 1)).strip()

class InstLD(Instruction):
    pattern = '^EEEE ([0-1]{7}) 00L D{9} ([0-1]{9})$'
    
    def __init__(self, e, l, d) -> None:
        self.l = l
        op = self.opc
        czi = l
        s = self.subopc1
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2),
            'subopc1': int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d)).strip()

class InstCLD(Instruction):
    pattern = '^EEEE ([0-1]{7}) C0[L|0] D{9} ([0-1]{9})$'

    def __init__(self, e, c, l, d) -> None:
        self.c = c
        self.l = l
        op = self.opc
        czi = (c << 2) + l
        s = self.subopc1
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2),
            'subopc1': int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ' ' + self._to_effect(self.c << 1)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ' ' + self._to_effect(self.c << 1)).strip()

class InstCZD(Instruction):
    pattern = '^EEEE ([0-1]{7}) C[Z|0]([0-1]) D{9} ([0-1]{9})$'

    def __init__(self, e, c, z, d) -> None:
        self.c = c
        self.z = z
        op = self.opc >> 1
        czi = (c << 2) + (z << 1) + (self.opc & 1)
        s = self.subopc1
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': (int(r.group(1), 2) << 1) + int(r.group(2), 2),
            'subopc1': int(r.group(3), 2)
        }

    def __str__(self):
        return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ' ' + self._to_effect((self.c << 1) + self.z)).strip()

class InstCZ(Instruction):
    pattern = '^EEEE ([0-1]{7}) CZ0 ([0-1]{9}) ([0-1]{9})$'

    def __init__(self, e, c, z) -> None:
        self.c = c
        self.z = z
        op = self.opc
        czi = (c << 2) + (z << 1)
        d = self.subopc1
        s = self.subopc2
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2),
            'subopc1': int(r.group(2), 2),
            'subopc2': int(r.group(3), 2)
        }

    def __str__(self):
        return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_effect((self.c << 1) + self.z)).strip()

class InstCZLD(Instruction):
    pattern = '^EEEE ([0-1]{7}) CZL D{9} ([0-1]{9})$'

    def __init__(self, e, c, z, l, d) -> None:
        self.c = c
        self.z = z
        self.l = l
        op = self.opc
        czi = (c << 2) + (z << 1) + l
        s = self.subopc1
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2),
            'subopc1': int(r.group(2), 2)
        }

    def __str__(self):
        # how the instruction is dissassembled 
        if self.l:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.d) + ' ' + self._to_effect((self.c << 1) + self.z)).strip()
        else:
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d) + ' ' + self._to_effect((self.c << 1) + self.z)).strip()

class InstD(Instruction):
    pattern = '^EEEE ([0-1]{7}) 000 D{9} ([0-1]{9})$'

    def __init__(self, e, d) -> None:
        op = self.opc
        czi = 0
        s = self.subopc1

        super().__init__(e, op, czi, d, s)
    
    def get_vals(r):
        return {
            'opc': int(r.group(1), 2),
            'subopc1': int(r.group(2), 2)
        }

    def __str__(self):
        return (self._to_cond(self.e) + ' ' + self.mnemonic + ' ' + self._to_reg(self.d)).strip()

class InstRA(Instruction):
    pattern = '^EEEE ([0-1]{7}) RA{2} A{9} A{9}$'

    def __init__(self, e, r, a) -> None:
        self.r = r
        self.a = a
        op = self.opc
        czi = (r << 2) + ((a >> 18) & 0x3)
        d = (a >> 9) & 0x1ff
        s = a & 0x1ff
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

    def __str__(self):
        if (self.mnemonic == 'calla'):
            # calla is special. it's always absolute, but doens't include the /
            return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.a)).strip()
        else:
            if self.r:
                return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.a)).strip()
            else:
                return (self._to_cond(self.e) + ' ' + self.mnemonic + ' /#' + str(self.a)).strip()

class InstWRA(Instruction):
    pattern = '^EEEE ([0-1]{5})WW RA{2} A{9} A{9}$'

    def __init__(self, e, w, r, a) -> None:
        raise(Exception("Instruction not implemented!"))
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }

class InstN(Instruction):
    pattern = '^EEEE ([0-1]{5})nn n{3} n{9} n{9}$'
    
    def __init__(self, e, n) -> None:
        self.n = n
        op = (self.opc << 2) + (n >> 21)
        czi = (n >> 18) & 0x8
        d = (n >> 9) & 0x1ff
        s = n & 0x1ff
        super().__init__(e, op, czi, d, s)

    def get_vals(r):
        return {
            'opc': int(r.group(1), 2)
        }
    
    def __str__(self):
        return (self._to_cond(self.e) + ' ' + self.mnemonic + ' #' + str(self.n)).strip()

# a list of all instruction formats
InstructionFormats = [
    InstNOP,
    InstCZIDS,
    Inst3NIDS,
    Inst2NIDS,
    Inst1NIDS,
    InstIDS,
    InstZIDS,
    InstCIDS,
    InstLIDS,
    InstIS,
    InstCLIDS,
    InstLD,
    InstCLD,
    InstCZD,
    InstCZ,
    InstCZLD,
    InstD,
    InstRA,
    InstWRA,
    InstN
]

# a dictionary of every instruction. variable fields (d, s, etc) are still variable here
# keyed by mnemonic
# an instance of one of these represents an exact instruction
InstructionClasses = {}
        
def generate_instruction_classes(mnemonic : str, enc_str : str):
    # print(mnemonic, enc_str)

    for i in InstructionFormats:
        r = re.search(i.pattern, enc_str)
        if (r):
            # this encoding/mnemonic matches i. Generate a subclass of i and added to InstructionClasses

            vals = i.get_vals(r)
            vals['mnemonic'] = mnemonic 

            inst = type(mnemonic, (i,), vals)
            InstructionClasses[mnemonic] = inst

    return None

def gen_sample_instructions(i):
    if issubclass(i, InstCZIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 0, 0, 1, Registers['r0'], 1)) # register, immediate
        insts.append(i(Conditions['_ret_'], 1, 0, 0, Registers['r0'], Registers['r1'])) # wc
        insts.append(i(Conditions['_ret_'], 0, 1, 0, Registers['r0'], Registers['r1'])) # wz
        insts.append(i(Conditions['_ret_'], 1, 1, 0, Registers['r0'], Registers['r1'])) # wcz

        return insts

    if issubclass(i, Inst3NIDS):
        raise(Exception("sample generation not implemented for " + str(i) + " of format " + str(Inst3NIDS)))

    if issubclass(i, Inst2NIDS):
        raise(Exception("sample generation not implemented for " + str(i) + " of format " + str(Inst2NIDS)))

    if issubclass(i, Inst1NIDS):
        raise(Exception("sample generation not implemented for " + str(i) + " of format " + str(Inst1NIDS)))

    if issubclass(i, InstIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 1, Registers['r0'], 3)) # register, immediate
        insts.append(i(Conditions['_ret_'], 0, Registers['r0'], Registers['r1'])) # all operand positions

        return insts

    if issubclass(i, InstZIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 0, 1, Registers['r0'], 3)) # register, immediate
        insts.append(i(Conditions['_ret_'], 1, 0, Registers['r0'], Registers['r1'])) # all operand positions

        return insts

    if issubclass(i, InstCIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 0, 1, Registers['r0'], 3)) # register, immediate
        insts.append(i(Conditions['_ret_'], 1, 0, Registers['r0'], Registers['r1'])) # all operand positions

        return insts

    if issubclass(i, InstLIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 0, 1, Registers['r0'], 3)) # register, immediate
        insts.append(i(Conditions['always'], 1, 0, 3, Registers['r0'])) # immediate, register
        insts.append(i(Conditions['always'], 1, 1, 3, 3)) # immediate, immediate
        insts.append(i(Conditions['_ret_'], 0, 0, Registers['r0'], Registers['r1'])) # all operand positions

        return insts

    if issubclass(i, InstIS):
        raise(Exception("sample generation not implemented for " + str(i) + " of format " + str(InstIS)))

    if issubclass(i, InstCLIDS):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, 0, Registers['r0'], Registers['r1']))

        insts.append(i(Conditions['always'], 0, 0, 1, Registers['r0'], 3)) # register, immediate
        insts.append(i(Conditions['always'], 0, 1, 0, 3, Registers['r0'])) # immediate, register
        insts.append(i(Conditions['always'], 0, 1, 1, 3, 3)) # immediate, immediate
        insts.append(i(Conditions['_ret_'], 1, 0, 0, Registers['r0'], Registers['r1'])) # all operand positions

        return insts

    if issubclass(i, InstLD):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, Registers['r0']))

        insts.append(i(Conditions['_ret_'], 1, 1))

        return insts

    if issubclass(i, InstCLD):
        insts = []

        insts.append(i(Conditions['_ret_'], 1, 0, Registers['r0'])) # all operand positions. Don't check L = 1 since some instructions don't have that variant

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0, Registers['r0']))

        return insts

    if issubclass(i, InstCZD):
        insts = []

        if i.mnemonic == 'getbrk':
            insts.append(i(Conditions['_ret_'], 1, 0, Registers['r0'])) # all operand positions. Don't check z = 1 since some instructions don't have that variant

            for e in Conditions:
                insts.append(i(Conditions[e], 1, 0, Registers['r0']))
        else:
            insts.append(i(Conditions['_ret_'], 1, 0, Registers['r0'])) # all operand positions. Don't check z = 1 since some instructions don't have that variant

            for e in Conditions:
                insts.append(i(Conditions[e], 0, 0, Registers['r0']))

        return insts

    if issubclass(i, InstCZ):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 0, 0))
        
        insts.append(i(Conditions['_ret_'], 1, 0)) # wc
        insts.append(i(Conditions['_ret_'], 0, 1)) # wz
        insts.append(i(Conditions['_ret_'], 1, 1)) # wcz

        return insts

    if issubclass(i, InstCZLD):
        insts = []

        if i.mnemonic == 'testp' or i.mnemonic == 'testpn':
            # testp requires wc/wz. empty or wcz is dirl
            for e in Conditions:
                insts.append(i(Conditions[e], 1, 0, 0, Registers['r0']))

            insts.append(i(Conditions['_ret_'], 1, 0, 0, Registers['r0'])) # wc
            insts.append(i(Conditions['_ret_'], 0, 1, 0, Registers['r0'])) # wz
        else:
            for e in Conditions:
                insts.append(i(Conditions[e], 0, 0, 0, Registers['r0']))

            insts.append(i(Conditions['always'], 0, 0, 1, 1))
            insts.append(i(Conditions['_ret_'], 1, 1, 0, Registers['r0'])) # wcz

        return insts

    if issubclass(i, InstD):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], Registers['r0']))

        return insts

    if issubclass(i, InstRA):
        insts = []

        if (i.mnemonic == 'calla'):
            # calla is special, we don't use the relative version, and the absolute version drops the / in the operand
            for e in Conditions:
                insts.append(i(Conditions[e], 0, 1))
        else:
            for e in Conditions:
                insts.append(i(Conditions[e], 1, 1))

            insts.append(i(Conditions['_ret_'], 0, 1))

        return insts

    if issubclass(i, InstWRA):
        raise(Exception("sample generation not implemented for " + str(i) + " of format " + str(InstWRA)))

    if issubclass(i, InstN):
        insts = []

        for e in Conditions:
            insts.append(i(Conditions[e], 1))

        return insts
        

instructions_to_test = [
    'sub',
    'ror',
    'rol',
    'shr',
    'shl',
    'rcr',
    'rcl',
    'sar',
    'sal',
    'add',
    'addx',
    'adds',
    'addsx',
    'subx',
    'subs',
    'subsx',
    'subr',
    'and',
    'or',
    'xor',
    'signx',
    'mul',
    'muls',
    'not',
    'abs',
    'neg',
    'rev',
    'encod',
    'decod',
    'ones',
    'getqx',
    'getqy',
    'qmul',
    'qdiv',
    'cmp',
    'cmp',
    'cmpr',
    'cmpr',
    'cmpx',
    'cmpx',
    'cmps',
    'cmps',
    'cmpsx',
    'cmpsx',
    'jmp',
    'tjz',
    'tjnz',
    'djnz',
    'mov',
    'rdbyte',
    'rdword',
    'rdlong',
    'wrbyte',
    'wrword',
    'wrlong',
    'rfbyte',
    'rfword',
    'rflong',
    'setq',
    'setq2',
    'rdlut',
    'wrlut',
    'rdfast',
    'wrfast',
    'wfbyte',
    'wfword',
    'wflong',
    'call',
    'calla',
    'dirl',
    'dirh',
    'outl',
    'outh',
    'outnot',
    'drvl',
    'drvh',
    'drvnot',
    # 'testp',
    'cogid',
    'getct',
    'coginit',
    'waitx',
    'hubset',
    'wrpin',
    'wxpin',
    'wypin',
    'rqpin',
    'rdpin',
    'locknew',
    'lockret',
    'locktry',
    'lockrel',
    'setxfrq',
    'xinit',
    'xcont',
    'addct1',
    'addct2',
    'addct3',
    'waitct1',
    'waitct2',
    'waitct3',
    'getbrk',
    'cogbrk',
    'brk',
    'wrc',
    'wrnc',
    'wrz',
    'wrnz',
    'augs',
    'augd'
]

test_template = '''
' RUN: llvm-mc -triple p2 -show-encoding < %s | FileCheck %s
' RUN: llvm-mc -filetype=obj -triple p2 < %s | llvm-objdump -d - | FileCheck --check-prefix=CHECK-INST %s

test:
{}

{}
'''

check_template = '''
' CHECK: {} ' encoding: [{},{},{},{}]
' CHECK-INST: {}
'''

def output_test(outdir, name, insts):
    '''
    insts list of instructions in the test
    '''

    test_file_name = os.path.join(outdir, 'inst-' + name + '.s')

    with open(test_file_name, 'w+') as f:
        # generate the test instructions
        lines = []
        for i in insts: 
            lines.append('\t' + str(i))
            
        inst_str = '\n'.join(lines)

        # generate checks
        checks = []
        for i in insts:
            enc = i.encoding()
            check = check_template.format(str(i), 
                                        '{:#04x}'.format((enc >> 24) & 0xff), 
                                        '{:#04x}'.format((enc >> 16) & 0xff), 
                                        '{:#04x}'.format((enc >> 8) & 0xff), 
                                        '{:#04x}'.format(enc & 0xff),
                                        str(i))

            checks.append(check)

        checks_str = '\n'.join(checks)

        f.write(test_template.format(inst_str, checks_str))

def main():
    parser = argparse.ArgumentParser(description='P2 LLVM Build Script')
    parser.add_argument('--output', type=str, required=True)
    args = parser.parse_args()

    # setup
    os.makedirs(args.output, exist_ok=True)

    # parse and generate all instruction classes
    with open('p2_instructions_revB.csv', newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            if row[4] != 'alias':
                generate_instruction_classes(row[1].split()[0].lower(), row[3])
                # print(row[3])

    # for i in InstructionClasses:
    #     print(InstructionClasses[i])

    for itest in instructions_to_test:
        if itest in InstructionClasses:
            insts = gen_sample_instructions(InstructionClasses[itest])
            if insts:
                output_test(args.output, itest, insts)
            else:
                print('{} does not generate any instructions!'.format(itest))
        else:
            print('{} is not parsed!'.format(itest))


if __name__ == "__main__":
    main()