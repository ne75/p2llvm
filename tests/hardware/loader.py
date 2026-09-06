"""Construct loadp2 commands and retain its detected device identity."""
import re


def arguments(args, binary):
    command = [args.loader.resolve(), '-v', '-q', '-t', '-ZERO', '-PATCH',
               '-' + args.reset, '-b', args.baud, '-l', args.loader_baud]
    if args.port:
        command += ['-p', args.port]
    if args.clock_hz is not None:
        command += ['-f', args.clock_hz]
    if args.clock_mode is not None:
        command += ['-m', hex(args.clock_mode)]
    if args.fifo is not None:
        command += ['-FIFO', args.fifo]
    return list(map(str, command + [binary]))


def identity(output):
    text = output.decode('ascii', errors='replace')
    result = {}
    match = re.search(r'P2 version (\S+) found on serial port (\S+)', text)
    if match:
        result.update(rom_version=match[1], detected_port=match[2])
    match = re.search(r'Setting clock_mode to ([0-9a-fA-F]+)', text)
    if match:
        result['detected_clock_mode'] = int(match[1], 16)
    return result
