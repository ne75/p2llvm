class Status:
    def __init__(self, status_bytes, cog) -> None:
        '''
        Stores the current cog/processor status. built each time a command is executed
        See documentation for what each bit is
        '''
        self._status_bytes = status_bytes

        stat1 = int.from_bytes(status_bytes[0:4], 'little')
        stat2 = int.from_bytes(status_bytes[4:8], 'little')
        stat3 = int.from_bytes(status_bytes[8:12], 'little')
        self.pc = int.from_bytes(status_bytes[12:16], 'little') & 0xfffff # the next instruction that will execute
        
        self._cog_exec_base_addr = -1 # if we are in cogex, this is the base address that was loaded into the cog
        self.exec_mode = "cogex" if self.pc < 0x400 else "hubex"
        self.cog = cog
        self.brk_code = (stat1 >> 24) & 0xff
        self.coginit = ((stat1 >> 23) & 1) == 1
        self.colorspace_conv_active = ((stat1 >> 22) & 1) == 1
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
    
    def set_cog_addr(self, addr):
        self._cog_exec_base_addr = addr

    def get_cog_addr(self):
        return self._cog_exec_base_addr

    def get_mem_pc(self):
        if self.exec_mode == "cogex" and self._cog_exec_base_addr != -1:
            return self.pc*4 + self._cog_exec_base_addr
        else:
            return self.pc

class P2RegInfo:
    def __init__(self) -> None:
        self.reg_table = {};

        for i in range(32):
            self.reg_table["r" + str(i)] = 0x1d0 + i;

        self.reg_table["ijmp3"] = 0x1f0
        self.reg_table["iret3"] = 0x1f1
        self.reg_table["ijmp2"] = 0x1f2
        self.reg_table["iret2"] = 0x1f3
        self.reg_table["ijmp1"] = 0x1f4
        self.reg_table["iret1"] = 0x1f5
        self.reg_table["pa"] = 0x1f6
        self.reg_table["pb"] = 0x1f7
        self.reg_table["ptra"] = 0x1f8
        self.reg_table["ptrb"] = 0x1f9
        self.reg_table["dira"] = 0x1fa
        self.reg_table["dirb"] = 0x1fb
        self.reg_table["outa"] = 0x1fc
        self.reg_table["outb"] = 0x1fd
        self.reg_table["ina"] = 0x1fe
        self.reg_table["inb"] = 0x1ff

    def getRegAddr(self, s):
        return self.reg_table.get(s, None)

    def getRegs(self):
        return self.reg_table.keys()