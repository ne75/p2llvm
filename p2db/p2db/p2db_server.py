import threading
import time
import io
import serial
import queue

from . import p2info
from . import p2tools

import logging
log = logging.getLogger('main')

class CogState:
    DISCONNECTED = 0    # this cog has not been seen and we can't talk to it
    IDLE = 1            # this cog is idle and ready to accept data
    EXECUTING = 2       # this cog is executing code 

    def __init__(self, cog_num):
        self.status = None
        self.cog = cog_num
        self.state = CogState.DISCONNECTED 

    def update_status(self, stat):
        self.status = stat
        self.state = CogState.IDLE

    def get_state(self):
        return self.state

class CogPacket():

    UNKNOWN = 0
    STATUS = 1
    HUBBYTE = 2
    REG = 3

    type_dict = {
        UNKNOWN: "unknown",
        STATUS: "status",
        HUBBYTE: "hub byte",
        REG: "register"
    }

    def get_response_size(c):
        if (c == b'g'):
            return 20
        
        if (c == b'r'):
            return 4

        if (c == b'h'):
            return 1

        return 0

    def __init__(self, dat) -> None:
        self.bytes = dat 
        self.cog = int.from_bytes(dat[1:2], 'little')

        self.msgid = dat[0:1]
        self.raw = dat[2:]

        if self.msgid == b'h':
            self.value = int.from_bytes(self.raw, 'little')
            self.type = CogPacket.HUBBYTE
        elif self.msgid == b'r':
            self.value = int.from_bytes(self.raw, 'little')
            self.type = CogPacket.REG
        elif self.msgid == b'g':
            self.value = p2info.Status(self.raw, self.cog)
            self.type = CogPacket.STATUS
        else:
            self.type = CogPacket.UNKNOWN
            self.value = None

    def get_value(self):
        return self.value

    def get_raw(self):
        return self.raw
    
    def get_bytes(self):
        return self.bytes

    def __str__(self) -> str:
        return CogPacket.type_dict[self.type] + ": " + str(self.value) + ", " + str(self.bytes)

class P2DBServer:
    '''
    Responsible for communicating with a Propeller 2 

    it should run a separate thread with our serial reading code from before and maintain the state of 
    the chip
    '''

    GETBYTE = 1
    GETREG = 2
    BREAK = 3
    STATUS = 4

    command_dict = {
        GETBYTE: b'h',
        GETREG: b'r',
        BREAK: b'b',
        STATUS: b's'
    }

    def __init__(self, port):
        self.cog_states = [CogState(i) for i in range(8)]
        self.stat_dirty = True

        self.ser = serial.Serial()
        self.ser.baudrate = 2000000
        self.ser.timeout = None
        self.ser.port = port
        self.ser.open()
        
        self.log_queue = queue.Queue()
        self.ret_addr = 0  #if we step into a function, store the return address so we can continue until the return

        self.current_cog = 0
        self.timeout = 2
        self.ri = p2info.P2RegInfo()

        self.dira = 0
        self.dirb = 0
        self.outa = 0
        self.outb = 0
        self.ina = 0
        self.inb = 0

        self.have_pin_data = False

        '''
        commands will be a dict:
        {
            "cmd": cmd
            "timeout": if not None, expect a reponse within this many seconds
            "value": value associated with comamnd (hub or reg)
            "response": response packet. should be None initially and set by the server thread
        } 
        
        '''
        self.cmd = {
            "cmd": None,
            "timeout": None,
            "value": None,
            "response": None
        }

    def _set_cmd(self, cmd, value, timeout=None):
        self.cmd = {
            "cmd": cmd,
            "timeout": timeout,
            "value": value,
            "response": None
        }

    def _wait_for_response(self):
        self._wait_for_cmd_send()
        self.cog_states[self.current_cog].state = CogState.EXECUTING

        while(self.cmd["response"] == None):
            time.sleep(0.001)

        self.cog_states[self.current_cog].state = CogState.IDLE

    def _wait_for_cmd_send(self):
        while (self.cmd["cmd"] != None):
            time.sleep(0.001)

    def get_status(self):
        return self.cog_states[self.current_cog].status

    def load_app(self, port='', app='', verbose=False):
        self.port = port
        self.app = app
        self.verbose = verbose
        return p2tools.load(port, app, self.ser.baudrate, verbose)

    def start(self):
        '''
        start the server
        '''
        log.info("Start P2DB Server...")
        t = threading.Thread(target=self.server_main, daemon=True)
        t.start()

    def set_objdata(self, dat):
        self.obj_data = dat

    def update_pins(self):
        _, self.dira = self.get_reg('dira')
        _, self.dirb = self.get_reg('dirb')
        _, self.outa = self.get_reg('outa')
        _, self.outb = self.get_reg('outb')
        _, self.ina = self.get_reg('ina')
        _, self.inb = self.get_reg('inb')
        self.have_pin_data = True

    def get_byte(self, addr, raw=False):
        '''
        get a byte from the hub.

        return a tuple (str, int) with string result and byte. if string is empty, byte is the byte read in, if string is non-empty, there 
        was an error and byte will be 0
        '''
        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return ("No connection to cog " + str(self.current_cog), 0)

        log.info("get byte @ " + addr)

        try:
            addr = int(addr, 16)
        except ValueError:
            return ("Invalid hub address: " + str(addr), 0)

        self._set_cmd(self.GETBYTE, addr)
        self._wait_for_response()

        if raw:
            return ("", self.cmd["response"].get_raw())
        else:
            return ("", self.cmd["response"].get_value())

    def get_long(self, addr):
        '''
        get a long by reading 4 bytes
        '''

        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return ("No connection to cog " + str(self.current_cog), 0)

        log.info("get long @ " + addr)

        try:
            addr = int(addr, 16)
        except ValueError:
            return ("Invalid hub address: " + str(addr), 0)

        dat = b''
        for i in range(4):
            dat = dat + self.get_byte(str(addr + i), raw=True)[1]
        
        return ('', int.from_bytes(dat, 'little'))

    def get_reg(self, reg):
        '''
        get a register value. reg should be either a named register or an address

        returns tuple: (str, int). The string is empty if the register was read ok, or an error message
        to print. if the string is not empty, register value will be 0
        '''

        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return ("No connection to cog " + str(self.current_cog), 0)

        log.debug("get reg " + reg)

        try:
            addr = int(reg, 16)
        except ValueError:
            addr = self.ri.getRegAddr(reg)

        if addr == None:
            return ("Unknown register: " + str(reg) + "\n", 0)

        if (addr <= 0xf):
            # this range was spilled, read it from teh hub instead
            return self.get_long(str(0xfff80 - 0x80*self.current_cog + 4*addr))
        elif (addr >= 0xf0 and addr <= 0x1f0):
            # this range was spilled, read it from the hub instead
            return self.get_long(str(0xfdc00 - 0x400*self.current_cog + 4*(addr - 0xf0)))
        elif (addr > 0x1ff):
            # not a valid register address
            return ("Invalid register address: " + str(addr) + "\nCog RAM registers address must be less than 0x200", 0)
        else:
            self._set_cmd(self.GETREG, addr)
            self._wait_for_response()

            return ("", self.cmd["response"].get_value())

    def _switch_cog_isr(self, cog):
        self._set_cmd(self.STATUS, 0, self.timeout)
        self._wait_for_response()
        
        if (isinstance(self.cmd["response"], str) and self.cmd["response"] == "timeout_error"):
            self.cog_states[self.current_cog].state = CogState.DISCONNECTED
            return "Cog response timeout"

        return ""

    def continue_exec(self):
        '''
        Continue execution until the cog interrupts again. Useful if aync interrupts are used
        '''
        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return "No connection to cog " + str(self.current_cog)

        self._set_cmd(self.BREAK, 0)
        self._wait_for_cmd_send()

        self.cog_states[self.current_cog].state = CogState.DISCONNECTED

        return ""

    def step(self):
        '''
        step by 1 instruction. will step over function calls

        returns a string with error or empty if no error
        '''
        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return "No connection to cog " + str(self.current_cog)

        i = p2tools.get_inst(self.obj_data, self.get_status().get_mem_pc())
        if i and 'call' in i[1]:
            addr = self.get_status().get_mem_pc() + 4
            log.info('Stepping over call instruction...')
            return self.breakpoint("{:x}".format(addr))
        else:
            self._set_cmd(self.BREAK, 1)
            self._wait_for_cmd_send()

            # switch the isr to the current cog
            return self._switch_cog_isr(self.current_cog)

    def stepin(self):
        '''
        step into a function call

        return a string with error or empty if no error
        '''

        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return "No connection to cog " + str(self.current_cog)

        i = p2tools.get_inst(self.obj_data, self.get_status().get_mem_pc())
        if 'call' in i[1]:
            self.ret_addr = self.get_status().get_mem_pc() + 4
            log.info("Stepping into function call, return to " + str(self.ret_addr))
            self._set_cmd(self.BREAK, 1)
            self._wait_for_cmd_send();

            return self._switch_cog_isr(self.current_cog)
        else:
            return "Can't step in: not a call* instruction"

    def stepout(self):
        '''
        step out of the current function call
        '''

        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return "No connection to cog " + str(self.current_cog)

        self.breakpoint("{:x}".format(self.ret_addr))

    def breakpoint(self, addr):
        if self.cog_states[self.current_cog].get_state() == CogState.DISCONNECTED:
            return "No connection to cog " + str(self.current_cog)

        addr = int(addr, 16)
        i = p2tools.get_inst(self.obj_data, addr)
        
        if 'call' in i[1]:
            self.ret_addr = addr + 4

        s = self.get_status()
        if s.exec_mode == 'cogex': # convert to a cog address
            addr -= s.get_cog_addr()
            addr /= 4
            addr = int(addr)
        elif s.exec_mode == 'lutex':
            addr -= 0x200
            addr /= 4
            addr = int(addr) + 0x200

        log.info("breakpoint at address " + str(addr))
        self._set_cmd(self.BREAK, (addr << 12) + (1 << 10))
        self._wait_for_cmd_send()

        # switch the isr to the current cog
        self._switch_cog_isr(self.current_cog)

        return ""

    def set_cog(self, cog):
        self.current_cog = cog

    def process_ser_data(self):
        '''
        process all current serial data into packets
        '''
        data = self.ser.read(self.ser.in_waiting)
        l = len(data)

        packet_buffer = b''
        packets = []

        if (data):
            log.debug("buffer: len = {}, ".format(l) + str(data))

        with io.BytesIO(data) as data_stream:
            def fill_datastream():
                '''
                get more data into the datastream due to message fragmentation

                returns the number of bytes added
                '''
                data_frag = self.ser.read(self.ser.in_waiting)
                data_frag_l = len(data_frag)
                data_stream.write(data_frag)
                data_stream.seek(-data_frag_l, 2)
                log.debug("added to buffer: {}".format(str(data_frag)))

                return data_frag_l

            while data_stream.tell() < l:
                char = data_stream.read(1)

                if char == b'\xdb':
                    log.debug("have start of message at {}".format(data_stream.tell()-1))
                        
                    header = data_stream.read(2)

                    while (len(header) != 2):
                        # we know more data should come in, so keep trying to read the header
                        l += fill_datastream()
                        header += data_stream.read(2-len(header))

                    log.debug("have header: {}".format(str(header)))

                    packet_buffer += header

                    msg_id = header[0:1]

                    n = CogPacket.get_response_size(msg_id)

                    if (n != 0):
                        msg_data = data_stream.read(n)

                        while len(msg_data) != n:
                            # we know more data should come in, so keep trying to read until we fill the packet
                            log.debug("fragmented packet. Expecting {}, have {}".format(n, len(msg_data)))

                            l += fill_datastream()
                            msg_data += data_stream.read(n-len(msg_data))

                        packet_buffer += msg_data
                        packets.append(CogPacket(packet_buffer))
                        packet_buffer = b''

                    else:
                        log.error("unknown response: {}".format(msg_id))
                            
                        packet_buffer = b''
                else:
                    log.debug("non-debeug char {} at {}".format(str(char), data_stream.tell()-1))

                    if (char >= b'\x80'):
                        self.log_queue.put(str(char))
                    else:
                        self.log_queue.put(char.decode('ascii'))

        return packets

    def send_cmd(self, command, cog, val):
        '''
        command: bytes object with command byte
        cog: int with cog number to send to
        val: int with value to send
        '''
        cmd = b'\xdb' + command + cog.to_bytes(1, 'little') + val.to_bytes(4, 'little')
        log.debug("cmd: " + str(cmd))
        self.ser.write(cmd)

    def pin_updater(self):
        while(1):
            self.update_pins()
            time.sleep(0.5)

    def server_main(self):
        while(1):
            expect_response = None

            if (self.cmd["cmd"]):
                log.info("Process command: " + str(self.cmd))
                if self.cmd["cmd"] == self.GETREG:
                    expect_response = CogPacket.REG

                if self.cmd["cmd"] == self.GETBYTE:
                    expect_response = CogPacket.HUBBYTE

                if self.cmd["cmd"] == self.STATUS:
                    expect_response = CogPacket.STATUS

                # send the command
                self.send_cmd(self.command_dict[self.cmd['cmd']], self.current_cog, self.cmd['value'])
                self.cmd["cmd"] = None

            done = False
            timeout = self.cmd["timeout"]
            t_start = time.time()

            while not done:
                packets = self.process_ser_data()

                if packets:
                    self.stat_dirty = True
                    log.info([str(p) for p in packets])

                for p in packets:
                    if (p.type == CogPacket.STATUS):
                        # because we construct a new state, we need to copy over the cog_exec address from the existing status
                        cog_addr_last = self.cog_states[p.cog].status.get_cog_addr() if self.cog_states[p.cog].status else -1
                        p.get_value().set_cog_addr(cog_addr_last)
                        self.cog_states[p.cog].update_status(p.get_value())
                        self.cmd["cmd"] = None
                    
                    if (p.type == expect_response and p.cog == self.current_cog):
                        done = True
                        self.cmd["response"] = p

                if timeout and time.time() > t_start + timeout:
                    log.warning("Cog response timeout")
                    self.cmd["response"] = "timeout_error"
                    done = True

                if not expect_response:
                    done = True

            time.sleep(0.001)