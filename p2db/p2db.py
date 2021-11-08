import argparse
from os import stat
import serial
import subprocess
from colorama import Fore
from bitstring import BitArray
from pprint import pprint
import cmd
from reginfo import P2RegInfo

verbose = False

ser = serial.Serial()
ser.baudrate = 3000000

load_cmd = '/opt/p2llvm/bin/loadp2'
load_args = ['-ZERO', '-l', str(ser.baudrate), '-v', '-FIFO', '2048']

ri = P2RegInfo()

class Status:
    def __init__(self, status_bytes) -> None:
        '''
        Stores the current cog status/processor. built each time a command is executed
        See documentation for what each bit is
        '''
        self._status_bytes = status_bytes

        stat1 = int.from_bytes(status_bytes[0:4], 'little')
        stat2 = int.from_bytes(status_bytes[4:8], 'little')
        stat3 = int.from_bytes(status_bytes[8:12], 'little')
        self.pc = int.from_bytes(status_bytes[12:16], 'little') & 0xfffff # the next instruction that will execute

        self.brk_code = (stat1 >> 24) & 0xff
        self.coginit = ((stat1 >> 23) & 1) == 1
        self.colorspace_converter_active = ((stat1 >> 22) & 1) == 1
        self.streamer_active = ((stat1 >> 21) & 1) == 1

        self.int3_event = (stat1 >> 16) & 0xf
        self.int2_event = (stat1 >> 12) & 0xf
        self.int1_event = (stat1 >> 8) & 0xf

        int3_state = (stat1 >> 6) & 0x3
        if int3_state == 3:
            self.int3_state = "isr executing"
        elif int3_state == 2:
            self.int3_state = "pending"
        else:
            self.int3_state = "idle"

        int2_state = (stat1 >> 6) & 0x3
        if int2_state == 3:
            self.int2_state = "isr executing"
        elif int2_state == 2:
            self.int2_state = "pending"
        else:
            self.int2_state = "idle"

        int1_state = (stat1 >> 6) & 0x3
        if int1_state == 3:
            self.int1_state = "isr executing"
        elif int1_state == 2:
            self.int1_state = "pending"
        else:
            self.int1_state = "idle"

        self.stalli = ((stat1 >> 1) & 1) == 1

        self.init_exec_mode = "hubex" if (stat1 & 1) == 1 else "cogex"

        self.getq_error = ((stat2 >> 15) & 1) == 1
        self.cog_attn = ((stat2 >> 14) & 1) == 1

        self.streamer_read_lut_end = ((stat2 >> 13) & 1) == 1
        self.streamer_nco_rollover = ((stat2 >> 12) & 1) == 1
        self.streamer_done = ((stat2 >> 11) & 1) == 1
        self.streamer_ready = ((stat2 >> 10) & 1) == 1
        self.fifo_loaded = ((stat2 >> 9) & 1) == 1
        self.pin_pattern_matched = ((stat2 >> 8) & 1) == 1

        self.se4_event_occured = ((stat2 >> 7) & 1) == 1
        self.se3_event_occured = ((stat2 >> 6) & 1) == 1
        self.se2_event_occured = ((stat2 >> 5) & 1) == 1
        self.se1_event_occured = ((stat2 >> 4) & 1) == 1

        self.ct_passed_ct1 = ((stat2 >> 3) & 1) == 1
        self.ct_passed_ct2 = ((stat2 >> 2) & 1) == 1
        self.ct_passed_ct3 = ((stat2 >> 1) & 1) == 1

        self.int_occured = (stat2 & 1) == 1

def load(port, app, retries=3):
        
    result = False

    while retries:
        args = load_args.copy()
        args.append('-p')
        args.append(port)

        args.append('@0={},@fc800=build/p2db.bin,@ffc00=build/p2db_isr.bin'.format(app))

        p = subprocess.Popen([load_cmd] + args, stdout=subprocess.PIPE)

        if verbose:
            print(" ".join([load_cmd] + args))
        
        output = p.communicate()[0].decode('ascii')

        if p.returncode != 0:
            print(Fore.RED + output + Fore.RESET)
            print(Fore.YELLOW + "Load failed, retrying...\r" + Fore.RESET)
            retries -= 1;
            result = False;
        else:
            result = True;
            break

    if not result:
        print(Fore.RED + "Failed to load app\r" + Fore.RESET)
        if verbose:
            print("Load command was: " + str([load_cmd] + args))

    return result

def get_response_size(c):
    if (c == b'g'):
        return 16
    
    if (c == b'r'):
        return 4

    if (c == b'h'):
        return 1

    return 0

class P2DBPrompt(cmd.Cmd):
    prompt = '(p2db) '

    def get_response(self, raw=False):
        char = ser.read(1)
        if char == b'~':
            char = ser.read(1)

            n = get_response_size(char)

            if n == 0:
                print("unknown response: {}".format(char));
            else:
                data = ser.read(n)

            if char == b'g':
                if raw:
                    return data

                self.status = Status(data)
                return self.status

            if char == b'h':
                if raw:
                    return data

                return int.from_bytes(data, 'little')

            if char == b'r':
                if raw:
                    return data

                return int.from_bytes(data, 'little')

    def getbyte(self, addr, raw=False):
        cmd = b'$h' + addr.to_bytes(4, 'little')
        ser.write(cmd);
        return self.get_response(raw)

    def getlong(self, addr, raw=False):
        dat = b''
        for i in range(4):
            dat = dat + self.getbyte(addr + i, raw=True)

        if raw:
            return dat
        
        return int.from_bytes(dat, 'little')

    def getreg(self, addr):
        if (addr <= 0xf):
            # this range was spilled, read it from teh hub instead
            return self.getlong(0xfff80 + 4*addr)
        elif (addr >= 0xf0 and addr <= 0x1f0):
            # this range was spilled, read it from the hub instead
            return self.getlong(0xfdc00 + 4*(addr - 0xf0))
        elif (addr > 0x1ff):
            # not a valid register address
            print(Fore.RED + "Invalid register address: " + str(addr))
            print("Cog RAM registers address must be less than 0x200" + Fore.RESET);
        else:
            cmd = b'$r' + addr.to_bytes(4, 'little')
            ser.write(cmd);

            return self.get_response()

    def do_s(self, arg):
        '''
        Alias for step
        '''
        self.do_step(arg)

    def do_step(self, arg):
        '''
        step

        Step forward by a single instruction
        '''
        cmd = b'$b' + int(1).to_bytes(4, 'little')
        ser.write(cmd)

        self.get_response();
        pprint(vars(self.status))

    def do_getbyte(self, arg):
        '''
        getbyte <addr>

        Get a byte from hub RAM address <addr>. Must be given in hex format
        '''
        if not arg:
            print("Address argument required")
            return

        addr = int(arg, 16)
        print("{:#02x}".format(self.getbyte(addr)))

    def do_getlong(self, arg):
        '''
        getlong <addr>

        Get a long (4 bytes) from hub RAM address <addr>. Must be given in hex format
        '''

        if not arg:
            print("Address argument required")
            return

        addr = int(arg, 16)
        print("{:#02x}".format(self.getlong(addr)))


    def do_getreg(self, arg):
        '''
        getreg <addr/name>

        Get the register given by addr or name 
        '''

        if not arg:
            print("Address or name argument required")
            return

        try:
            addr = int(arg, 16)
        except ValueError:
            addr = ri.getRegAddr(arg)

        if addr == None:
            print("Unknown register: " + arg)
            return

        print("{:#02x}".format(self.getreg(addr)))

    def do_dump(self, arg):

        if (arg == 'regfile'):
            for r in ri.getRegs():
                print(r + ": {:#02x}".format(self.getreg(ri.getRegAddr(r))))

    def do_stat(self, arg):
        pprint(vars(self.status))

    def preloop(self):
        self.status = None

        while (self.status == None):
            self.get_response()

        pprint(vars(self.status))

    def do_EOF(self, line):
        return True

def main():
    global verbose

    parser = argparse.ArgumentParser(description='P2DB Debugger')
    parser.add_argument('port', type=str)
    parser.add_argument('app', type=str)
    parser.add_argument('--verbose', nargs='?', const=True, default=False);

    args = parser.parse_args()

    verbose = args.verbose

    ser.timeout = 0.1
    ser.port = args.port
    ser.open()

    if not load(args.port, args.app):
        return
    
    P2DBPrompt().cmdloop(intro='Successfully loaded ' + args.app)


if __name__ == "__main__":
    main()