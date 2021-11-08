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