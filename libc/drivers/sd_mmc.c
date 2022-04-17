/**
 * @brief Interface Driver for SD cards
 * @author Michael Burmeister
 * @date Feburary 1, 2021
 * @version 1.0
 * 
*/

//#define _DEBUG

#include <stdio.h>
#include <propeller.h>
#include "sd_mmc.h"

struct
{
    uint8_t CS;
    uint8_t CLK;
    uint8_t MOSI;
    uint8_t MISO;
} Pins;


void SetSD(int cs, int clk, int mosi, int miso)
{
    Pins.CS = cs;
    Pins.CLK = clk;
    Pins.MOSI = mosi;
    Pins.MISO = miso;
}


void EnableSD(void)
{
    wait(10);
    dirh(Pins.CS);
    pinh(Pins.CS);
    dirh(Pins.CLK);
    pinl(Pins.CLK);
    dirh(Pins.MOSI);
    dirl(Pins.MISO);
}

void SendSD(const BYTE* buff, unsigned int bc)
{
    int i = 0;
    const BYTE *b;
    int x = 0;
    int p = Pins.MOSI;
    int pp = Pins.CLK;
    int j = 0;
    int t = bc;

    b = buff;
    asm volatile ("mov %[i], %[t]\n"
                  ".Li: mov %[j], #8\n"
                  "rdbyte %[x], %[b]\n"
                  ".Lj: test %[x], #0x80 wz\n"
                  "if_z drvl %[p]\n"
                  "if_nz drvh %[p]\n"
                  "drvh %[pp]\n"
                  "drvl %[pp]\n"
                  "rol %[x], #1\n"
                  "djnz %[j], #.Lj\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li\n"
                  :[i]"+r"(i), [j]"+r"(j), [x]"+r"(x), [b]"+r"(b)
                  :[p]"r"(p), [pp]"r"(pp), [t]"r"(t));

}

void ReceiveSD(BYTE *buff, unsigned int bc)
{
    BYTE *b = buff;
    unsigned int c = bc;
    int m = Pins.MOSI;
    int n = Pins.MISO;
    int x = Pins.CLK;
    int i = 0;
    int j = 0;
    int v = 0;

    asm volatile ("drvh %[m]\n"
                  "waitx #8\n"
                  "mov %[i], %[c]\n"
                  ".li: mov %[v], #0\n"
                  "mov %[j], #8\n"
                  ".lj: rol %[v], #1\n"
                  "testp %[n] wc\n"
                  "if_c add %[v], #1\n"
                  "drvh %[x]\n"
                  "drvl %[x]\n"
                  "waitx #8\n"
                  "djnz %[j], #.lj\n"
                  "wrbyte %[v], %[b]\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.li\n"
                  :[i]"+r"(i), [j]"+r"(j), [v]"+r"(v), [b]"+r"(b)
                  :[x]"r"(x), [n]"r"(n), [m]"r"(m), [c]"r"(c));
}

int GetStatus(void)
{
    int i = 0;
    BYTE b = 0;
    
    for (i=0;i<50000;i++)
    {
        ReceiveSD(&b, 1);
        if (b == 0xff)
        	return 1;
        waitus(10);
    }
	return 0;
}

void ReleaseSD(void)
{
    BYTE b;
    
    pinh(Pins.CS);
    ReceiveSD(&b, 1);
}

int SelectSD(void)
{
    BYTE b;
    
    pinl(Pins.CS);
    ReceiveSD(&b, 1);
    if (GetStatus())
    	return 1;
    ReleaseSD();
    return 0;
}

int ReceiveBlock(BYTE *buff, unsigned int bc)
{
    BYTE b[2];
    int i;
   
    for (i=0;i<5000;i++)
    {
        ReceiveSD(b, 1);
        if (*b != 0xff)
        	break;
        waitus(10);
    }
	if (*b != 0xfe)
		return 0;
	ReceiveSD(buff, bc);
	ReceiveSD(b, 2);    // Ditch CRC
	return 1;
}

int SendBlock(const BYTE *buff, int token)
{
    BYTE b[2];
    
    if (GetStatus() == 0)
    	return 0;
    
    b[0] = token;
    SendSD(b, 1);
    if (token != 0xfd)
    {
        SendSD(buff, 512);
        ReceiveSD(b, 2);  // CRC value
        ReceiveSD(b, 1);
        if ((*b & 0x1f) != 0x05)
        	return 0;
    }
	return 1;    
}

BYTE SendCommand(BYTE cmd, unsigned int arg)
{
    BYTE b[6];
    BYTE n;
    
    if (cmd & 0x80)
    {
        cmd = cmd & 0x7f;
        n = SendCommand(CMD55, 0);
        if (n > 1)
        	return n;
    }
    
    if (cmd != CMD12)
    {
        ReleaseSD();
        if (!SelectSD())
        	return 0xff;
    }
    
    b[0] = 0x40 | cmd;
    b[1] = arg >> 24;
    b[2] = arg >> 16;
    b[3] = arg >> 8;
    b[4] = arg;
    n = 0x01;
    if (cmd == CMD0)
    	n = 0x95;
    if (cmd == CMD8)
    	n = 0x87;
    b[5] = n;
    SendSD(b, 6);
    
    if (cmd == CMD12)
    	ReceiveSD(&n, 1);

    for (int i=0;i<10;i++)
    {
        ReceiveSD(&n, 1);
        if ((n & 0x80) == 0)
        	break;
    }
#ifdef _DEBUG
    __builtin_printf("Command: %d (%x) --> %d\n", cmd, arg, n);
#endif
    return n;
}
