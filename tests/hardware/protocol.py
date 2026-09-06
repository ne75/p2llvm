"""Strict decoding shared by host validation and actual P2 hardware runs."""
import re


def compare(output, run_id, expected):
    lines = output.decode('ascii', errors='replace').splitlines()
    begin = 'P2TEST ' + run_id
    if lines.count(begin) != 1:
        raise ValueError('missing or duplicate run identity')
    start = lines.index(begin)
    observed = {}
    ended = False
    for line in lines[start + 1:]:
        end = re.fullmatch(r'P2END ' + re.escape(run_id) + r' ([0-9a-f]{8})', line)
        if end:
            if int(end[1], 16) != len(observed):
                raise ValueError('observation count mismatch')
            ended = True
            break
        match = re.fullmatch(r'P2VALUE ([a-zA-Z0-9_.-]+) ([0-9a-f]{8}) ([0-9a-f]{8})', line)
        if not match:
            raise ValueError('malformed observation: ' + repr(line))
        name, value, complement = match.groups()
        value, complement = int(value, 16), int(complement, 16)
        if value ^ complement != 0xffffffff:
            raise ValueError('corrupt observation: ' + name)
        if name in observed:
            raise ValueError('duplicate observation: ' + name)
        observed[name] = value
    if not ended:
        raise ValueError('missing completion record')
    expected = {name: int(value, 0) for name, value in expected.items()}
    mismatches = []
    for name in sorted(expected.keys() | observed.keys()):
        if name not in expected or name not in observed or expected[name] != observed[name]:
            mismatches.append({'name': name, 'expected': expected.get(name), 'actual': observed.get(name)})
    return observed, mismatches
