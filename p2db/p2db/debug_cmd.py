import cmd
import p2info
from colorama import Fore

class P2DBPrompt(cmd.Cmd):

    prompt = ''
    use_rawinput = False
    ruler = u'\x1b(0q\x1b(B'

    def init(self, ser):
        self.ser = ser
        self.ri = p2info.P2RegInfo()
        self.status = [None]*8
        self.dira = 0
        self.dirb = 0
        self.ina = 0
        self.inb = 0
        self.outa = 0
        self.outb = 0

        self.current_cog = 0

    def get_status(self):
        return self.status[self.current_cog]

    def get_response_size(c):
        if (c == b'g'):
            return 16
        
        if (c == b'r'):
            return 4

        if (c == b'h'):
            return 1

        return 0

    def get_response(self, raw=False):
        char = self.ser.read(1)
        if char == b'~':
            header = self.ser.read(2)
            msg_id = header[0:1]
            cog = int.from_bytes(header[1:2], 'little')

            # print(msg_id, cog)

            n = P2DBPrompt.get_response_size(msg_id)

            if n == 0:
                self.stdout.write("unknown response: {}".format(msg_id) + "\n");
                return None
            else:
                data = self.ser.read(n)

            if msg_id == b'g':
                if raw:
                    return data

                self.status[cog] = p2info.Status(data, cog)
                return self.status[cog]

            if msg_id == b'h':
                if raw:
                    return data

                return int.from_bytes(data, 'little')

            if msg_id == b'r':
                if raw:
                    return data

                return int.from_bytes(data, 'little')

    def getbyte(self, addr, raw=False):
        self.send_cmd(b'h', self.current_cog, addr)
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
            self.stdout.write(Fore.RED + "Invalid register address: " + str(addr) + "\n")
            self.stdout.write("Cog RAM registers address must be less than 0x200" + Fore.RESET + "\n")
        else:
            self.send_cmd(b'r', self.current_cog, addr)

            return self.get_response()

    def getpins(self):
        self.dira = self.getreg(self.ri.getRegAddr("dira"))
        self.dirb = self.getreg(self.ri.getRegAddr("dirb"))
        self.outa = self.getreg(self.ri.getRegAddr("outa"))
        self.outb = self.getreg(self.ri.getRegAddr("outb"))
        self.ina = self.getreg(self.ri.getRegAddr("ina"))
        self.inb = self.getreg(self.ri.getRegAddr("inb"))

    def update(self):
        # because a step could start a new cog, we want to read out all responses to see connections to any cogs that start up
        while self.get_response() is not None:
            pass

        self.getpins()

    def send_cmd(self, command, cog, val):
        '''
        command: bytes object with command byte
        cog: int with cog number to send to
        val: int with value to send
        '''
        cmd = b'$' + command + cog.to_bytes(1, 'little') + val.to_bytes(4, 'little')
        self.ser.write(cmd) 

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
        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        self.send_cmd(b'b', self.current_cog, 1)

        self.update()
    
    def do_break(self, arg):
        '''
        break <addr>

        set a breakpoint at address <addr> and run until then 
        '''
        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = int(arg, 16)
        self.send_cmd(b'b', self.current_cog, (addr << 12) + (1 << 10))

        self.update()

    def do_o(self, arg):
        '''
        alias for stepover
        '''
        self.do_stepover(arg)

    def do_stepover(self, arg):
        '''
        stepover
        
        step over the next instruction, useful for skipping calls
        '''
        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = self.status[self.current_cog].pc + 4
        self.do_break("{:x}".format(addr))


    def do_reset(self, arg):
        '''
        TODO 
        reload the current application
        '''
        pass

    def do_getbyte(self, arg):
        '''
        getbyte <addr>

        Get a byte from hub RAM address <addr>. Must be given in hex format
        '''
        if not arg:
            print("Address argument required")
            return

        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = int(arg, 16)
        self.stdout.write("byte @ {:#02x} -> {:#02x}".format(addr, self.getbyte(addr)) + "\n")

    def do_getlong(self, arg):
        '''
        getlong <addr>

        Get a long (4 bytes) from hub RAM address <addr>. Must be given in hex format
        '''

        if not arg:
            print("Address argument required")
            return

        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = int(arg, 16)
        self.stdout.write("long @ {:#02x} -> {:#02x}".format(addr, self.getlong(addr)) + "\n")


    def do_getreg(self, arg):
        '''
        getreg <addr/name>

        Get the register given by addr or name 
        '''

        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        if not arg:
            print("Address or name argument required")
            return

        named = False

        try:
            addr = int(arg, 16)
        except ValueError:
            addr = self.ri.getRegAddr(arg)
            named = True

        if addr == None:
            self.stdout.write("*** Unknown register: " + arg + "\n")
            return

        if named:
            self.stdout.write("{} -> {:#02x}".format(arg, self.getreg(addr)) + "\n")
        else:
            self.stdout.write("reg @ {:#02x} -> {:#02x}".format(addr, self.getreg(addr)) + "\n")

    def do_cog(self, arg):
        '''
        set the cog to debug
        '''
        self.current_cog = int(arg)

    def do_q(self, arg):
        return self.do_quit(arg)

    def do_quit(self, arg):
        return True

    def preloop(self):
        while (self.status[0] == None):
            self.get_response()

        self.getpins()