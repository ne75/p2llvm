import cmd
from colorama import Fore
import io

from . import p2info
from . import p2tools

class P2DBPrompt(cmd.Cmd):

    prompt = ''
    use_rawinput = False
    ruler = u'\x1b(0q\x1b(B'

    def init(self, ser, objdata):
        self.ser = ser
        self.obj_data = objdata 

        self.ri = p2info.P2RegInfo()
        self.status = [None]*8
        self.active = [False]*8
        self.dira = 0
        self.dirb = 0
        self.ina = 0
        self.inb = 0
        self.outa = 0
        self.outb = 0

        self.current_cog = 0
        self.cog_exec_addr = -1

    def get_status(self):
        return self.status[self.current_cog]

    def process_response(self):
        '''
        get and process serial data, blocking until a response packet is found
        '''

        r_packet = None

        while r_packet == None:
            packets = self.process_ser_data()

            if packets:
                with open("p2db.log", 'a') as f:
                    f.write(str(packets) + "\n")

            for p in packets:
                if (p.type == CogPacket.STATUS):
                    self.status[p.cog] = p.get_value()
                    self.status[p.cog].set_cog_addr(self.cog_exec_addr)
                    self.active[p.cog] = True
                elif (p.type != CogPacket.UNKNOWN):
                    r_packet = p

        return r_packet

    def process_status(self):
        '''
        get and process serial data, blocking until we've received a status packet, which means there's a cog that's ready to accept commands
        '''
        done = False

        while not done:
            packets = self.process_ser_data()

            if packets:
                with open("p2db.log", 'a') as f:
                    f.write(str(packets) + "\n")

            for p in packets:
                if (p.type == CogPacket.STATUS):
                    self.status[p.cog] = p.get_value()
                    self.status[p.cog].set_cog_addr(self.cog_exec_addr)
                    self.active[p.cog] = True
                    done = True


    def getbyte(self, addr, raw=False):
        self.send_cmd(b'h', self.current_cog, addr)

        if raw:
            return self.process_response().get_raw()
        else:
            return self.process_response().get_value()

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
            return self.getlong(0xfff80 - 0x80*self.current_cog + 4*addr)
        elif (addr >= 0xf0 and addr <= 0x1f0):
            # this range was spilled, read it from the hub instead
            return self.getlong(0xfdc00 - 0x400*self.current_cog + 4*(addr - 0xf0))
        elif (addr > 0x1ff):
            # not a valid register address
            self.stdout.write(Fore.RED + "Invalid register address: " + str(addr) + "\n")
            self.stdout.write("Cog RAM registers address must be less than 0x200" + Fore.RESET + "\n")
        else:
            self.send_cmd(b'r', self.current_cog, addr)
            return self.process_response().get_value()

    def getpins(self):
        '''
        get the status of the pin registers for the current cog 
        '''
        self.dira = self.getreg(self.ri.getRegAddr("dira"))
        self.dirb = self.getreg(self.ri.getRegAddr("dirb"))
        self.outa = self.getreg(self.ri.getRegAddr("outa"))
        self.outb = self.getreg(self.ri.getRegAddr("outb"))
        self.ina = self.getreg(self.ri.getRegAddr("ina"))
        self.inb = self.getreg(self.ri.getRegAddr("inb"))

    def update(self):
        self.process_status()
        if self.active[self.current_cog]:
            self.getpins()

    def send_cmd(self, command, cog, val):
        '''
        command: bytes object with command byte
        cog: int with cog number to send to
        val: int with value to send
        '''
        cmd = b'\xdb' + command + cog.to_bytes(1, 'little') + val.to_bytes(4, 'little')
        with open("p2db.log", 'a') as f:
            f.write("cmd: " + str(cmd) + "\n")
        self.ser.write(cmd)

    def do_continue(arg):
        '''
        continue this cog's execution. will "disconnect from the cog" and not wait for any response

        This cog can be reconnected by on of: 
        1. the cog's code hiting a hardcoded breakpoint 
        2. another cog generates an asynchronous breakpoint (not supported to set up right now)
        '''
        pass

    def step(self):
        self.send_cmd(b'b', self.current_cog, 1)
        self.active[self.current_cog] = False
        self.update()

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
        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        i = p2tools.get_inst(self.obj_data, self.get_status().get_mem_pc())
        if i and 'call' in i[1]:
            addr = self.status[self.current_cog].get_mem_pc() + 4
            self.do_break("{:x}".format(addr))
        else:
            self.step()

    def do_i(self, arg):
        '''
        Alias for stepin
        '''
        self.do_stepin(arg)

    def do_stepin(self, arg):
        '''
        step into a function call
        '''
        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)

        i = p2tools.get_inst(self.obj_data, self.get_status().get_mem_pc())
        if 'call' in i[1]:
            self.step()
        else:
            self.stdout.write("Can't step in: not a call* instruction\n")


    def do_break(self, arg):
        '''
        break <addr>

        set a breakpoint at address <addr> and run until then 
        '''
        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = int(arg, 16)
        s = self.get_status()
        if s.exec_mode == 'cogex': # convert to a cog address
            addr -= s.get_cog_addr()
            addr /= 4
            addr = int(addr)
        elif s.exec_mode == 'lutex':
            addr -= 0x200
            addr /= 4
            addr = int(addr) + 0x200

        self.send_cmd(b'b', self.current_cog, (addr << 12) + (1 << 10))
        self.active[self.current_cog] = False
        self.update()

    def do_cogaddr(self, arg):
        '''
        set the base address of this cog's code. only useful if the current cog is in cogex mode. 
        Value is ignored in hubex mode
        '''
        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return
        
        self.cog_exec_addr = int(arg, 16)
        self.get_status().set_cog_addr(self.cog_exec_addr)

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

        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        if not arg:
            self.stdout.write("Address argument required\n")
            return

        if not self.get_status():
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        addr = int(arg, 16)
        b = self.getbyte(addr)

        self.stdout.write("byte @ {:#02x} -> {:#02x}".format(addr, b) + "\n")

    def do_getlong(self, arg):
        '''
        getlong <addr>

        Get a long (4 bytes) from hub RAM address <addr>. Must be given in hex format
        '''
        if not self.active[self.current_cog]:
            self.stdout.write(Fore.RED + "*** No connection to cog\n" + Fore.RESET)
            return

        if not arg:
            self.stdout.write("Address argument required\n")
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
            self.stdout.write("Address or name argument required\n")
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
            self.stdout.write("reg {:#02x} -> {:#02x}".format(addr, self.getreg(addr)) + "\n")

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
        # wait for cog 0 to send a status message and is connected
        while not self.get_status():
            self.process_status()

        self.getpins()