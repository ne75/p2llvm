"""Independent integer ISA oracle from utils/p2_instructions_revB.csv.

No instruction encodings or generated assembly are read here. Values are
mathematical Python integers; truncation and flag updates are explicit.
"""
MASK = 0xffffffff


def signed(value, bits=32):
    value &= (1 << bits) - 1
    return value - (1 << bits) if value >> (bits - 1) else value


SCALAR = set(('ADD ADDX ADDS ADDSX SUB SUBX SUBS SUBSX SUBR CMP CMPX CMPS CMPSX CMPR '
              'FGE FLE FGES FLES SUMC SUMNC SUMZ SUMNZ AND ANDN OR XOR TEST '
              'MOV NOT ABS NEG ZEROX SIGNX ENCOD ONES ROR ROL SHR SHL RCR RCL SAR SAL '
              'SETNIB SETBYTE SETWORD GETNIB GETBYTE GETWORD ROLNIB ROLBYTE ROLWORD '
              'DECOD BMASK MOVBYTS REV SPLITB SPLITW MERGEB MERGEW RGBSQZ RGBEXP '
              'WRC WRNC WRZ WRNZ TESTB TESTBN MUL MULS NOP').split())


def scalar(op, d, s, c, z, n=1):
    """Return (D, potential C, potential Z); caller applies WC/WZ masks."""
    result, carry, zero = d, c, z
    if op in {'ADD', 'ADDX', 'ADDS', 'ADDSX', 'SUB', 'SUBX', 'SUBS', 'SUBSX',
              'SUBR', 'CMP', 'CMPX', 'CMPS', 'CMPSX', 'CMPR'}:
        extended = op.endswith('X')
        is_signed = op in {'ADDS', 'ADDSX', 'SUBS', 'SUBSX', 'CMPS', 'CMPSX'}
        a, b = (signed(d), signed(s)) if is_signed else (d, s)
        if op in {'SUBR', 'CMPR'}:
            a, b = b, a
        add = op.startswith('ADD')
        exact = a + b + (c if extended else 0) if add else a - b - (c if extended else 0)
        result = exact & MASK
        carry = int(exact < 0) if is_signed or not add else int(exact > MASK)
        zero = int(result == 0) & (z if extended else 1)
        if op.startswith('CMP'):
            result = d
        return result, carry, zero
    if op in {'FGE', 'FLE', 'FGES', 'FLES'}:
        a, b = (signed(d), signed(s)) if op.endswith('S') else (d, s)
        carry = int(a < b if op.startswith('FGE') else a > b)
        result = s if carry else d
    elif op.startswith('SUM'):
        subtract = {'SUMC': c, 'SUMNC': 1-c, 'SUMZ': z, 'SUMNZ': 1-z}[op]
        exact = signed(d) + (-signed(s) if subtract else signed(s))
        result, carry = exact & MASK, int(exact < 0)
    elif op in {'AND', 'ANDN', 'OR', 'XOR', 'TEST'}:
        result = {'AND': d & s, 'ANDN': d & ~s, 'OR': d | s,
                  'XOR': d ^ s, 'TEST': d & s}[op] & MASK
        carry, zero = result.bit_count() & 1, int(result == 0)
        return d if op == 'TEST' else result, carry, zero
    elif op in {'MOV', 'NOT', 'ABS', 'NEG', 'ENCOD', 'ONES'}:
        if op == 'ENCOD' and s == 0:
            return None  # Do not invent a defined result for ENCOD(0).
        result = {'MOV': s, 'NOT': ~s, 'ABS': abs(signed(s)), 'NEG': -s,
                  'ENCOD': s.bit_length()-1, 'ONES': s.bit_count()}[op] & MASK
        carry = (s >> 31) if op == 'ABS' else (int(s != 0) if op == 'ENCOD'
                 else result & 1 if op == 'ONES' else result >> 31)
    elif op in {'ZEROX', 'SIGNX'}:
        width = (s & 31) + 1
        result = (d & ((1 << width)-1)) if op == 'ZEROX' else signed(d, width) & MASK
        carry = result >> 31
    elif op in {'ROR', 'ROL', 'SHR', 'SHL', 'RCR', 'RCL', 'SAR', 'SAL'}:
        shift = s & 31
        left = op in {'ROL', 'SHL', 'RCL', 'SAL'}
        carry = (d >> (32-shift if shift else 31)) & 1 if left else (d >> (shift-1 if shift else 0)) & 1
        if shift:
            if op == 'ROR': result = (d >> shift) | (d << (32-shift))
            elif op == 'ROL': result = (d << shift) | (d >> (32-shift))
            elif op == 'SHR': result = d >> shift
            elif op == 'SHL': result = d << shift
            elif op == 'SAR': result = signed(d) >> shift
            elif op == 'SAL': result = (d << shift) | (((1 << shift)-1) if d & 1 else 0)
            elif op == 'RCR': result = (d >> shift) | (((1 << shift)-1) << (32-shift) if c else 0)
            elif op == 'RCL': result = (d << shift) | (((1 << shift)-1) if c else 0)
            result &= MASK
    elif op in {'TESTB', 'TESTBN'}:
        bit = ((d >> (s & 31)) & 1) ^ (op == 'TESTBN')
        return d, int(bit), int(bit)
    elif op in {'MUL', 'MULS'}:
        a, b = (signed(d, 16), signed(s, 16)) if op == 'MULS' else (d & 65535, s & 65535)
        return (a*b) & MASK, c, int(a == 0 or b == 0)
    elif op[:3] in {'SET', 'GET', 'ROL'} and op.endswith(('NIB', 'BYTE', 'WORD')):
        width = 4 if op.endswith('NIB') else 8 if op.endswith('BYTE') else 16
        mask = (1 << width)-1
        field = (s >> (n*width)) & mask
        if op.startswith('SET'): result = (d & ~(mask << (n*width))) | ((s & mask) << (n*width))
        elif op.startswith('GET'): result = field
        else: result = ((d << width) | field) & MASK
        return result, c, z
    elif op == 'DECOD': return 1 << (s & 31), c, z
    elif op == 'BMASK': return ((2 << (s & 31))-1) & MASK, c, z
    elif op == 'MOVBYTS': return sum(((d >> (((s >> (2*i)) & 3)*8)) & 255) << (8*i) for i in range(4)), c, z
    elif op == 'REV': return int(f'{d:032b}'[::-1], 2), c, z
    elif op in {'SPLITB', 'SPLITW', 'MERGEB', 'MERGEW'}:
        group = 4 if op.endswith('B') else 2
        source = (lambda i: (i % (32//group))*group + i//(32//group)) if op.startswith('SPLIT') else (lambda i: (i % group)*(32//group) + i//group)
        return sum(((d >> source(i)) & 1) << i for i in range(32)), c, z
    elif op == 'RGBEXP':
        r, g, b = (d >> 11) & 31, (d >> 5) & 63, d & 31
        return (((r << 3) | (r >> 2)) << 24) | (((g << 2) | (g >> 4)) << 16) | (((b << 3) | (b >> 2)) << 8), c, z
    elif op == 'RGBSQZ': return (((d >> 27) & 31) << 11) | (((d >> 18) & 63) << 5) | ((d >> 11) & 31), c, z
    elif op in {'WRC', 'WRNC', 'WRZ', 'WRNZ'}: return {'WRC': c, 'WRNC': 1-c, 'WRZ': z, 'WRNZ': 1-z}[op], c, z
    elif op == 'NOP': return d, c, z
    else: raise ValueError('missing ISA model: ' + op)
    return result, carry, int(result == 0)
