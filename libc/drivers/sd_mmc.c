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

unsigned long Pins[2];

void SetSD(int drive, int cs, int clk, int mosi, int miso)
{
    Pins[drive] = cs | (clk << 8) | (mosi << 16) | (miso << 24);
}


void EnableSD(int drive)
{
    int cs, clk, mosi, miso;

    wait(10);
    cs = Pins[drive] & 0xff;
    clk = (Pins[drive] >> 8) & 0xff;
    mosi = (Pins[drive] >> 16) & 0xff;
    miso = (Pins[drive] >> 24) & 0xff;
    dirh(cs);
    pinh(cs);
    dirh(clk);
    pinl(clk);
    dirh(mosi);
    dirl(miso);
}

void SendSD(int drive, const BYTE* buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int i,j,x;

    asm volatile ("mov %[i], %[bc]\n"
             ".Li: mov %[j], #8\n"
                  "rdbyte %[x], %[b]\n"
                  "shl %[x], #24\n"
             ".Lj: shl %[x], #1 wc\n"
             "if_c drvh %[mosi]\n"
            "if_nc drvl %[mosi]\n"
                  "drvh %[clk]\n"
                  "waitx #2\n"
                  "drvl %[clk]\n"
                  "waitx #2\n"
                  "djnz %[j], #.Lj\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li\n"
                  :[b]"+r"(buff), [i]"+r"(i), [j]"+r"(j), [x]"+r"(x),
                  [mosi]"+r"(mosi), [clk]"+r"(clk), [bc]"+r"(bc):);

}

void ReceiveSD(int drive, BYTE *buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int miso = (Pins[drive] >> 24) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int i, j, x;

    asm volatile ("drvh %[mosi]\n"
                  "mov %[i], %[bc]\n"
             ".Li2: mov %[x], #0\n"
                  "mov %[j], #8\n"
             ".Lj2: testp %[miso] wc\n"
                  "drvh %[clk]\n"
                  "rcl %[x], #1\n"
                  "drvl %[clk]\n"
                  "waitx #4\n"
                  "djnz %[j], #.Lj2\n"
                  "wrbyte %[x], %[b]\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li2\n"
                  :[b]"+r"(buff), [i]"+r"(i), [j]"+r"(j), [x]"+r"(x),
                  [clk]"+r"(clk), [miso]"+r"(miso), [mosi]"+r"(mosi), [bc]"+r"(bc):);
}

//__attribute__ ((section ("lut"), cogtext, no_builtin("memset")))
void SendSDB(int drive, const BYTE* buff, unsigned int bc)
{
    int i;
    unsigned int x;
    int mosi = (Pins[drive] >> 16) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int j;

    asm volatile (
            "rdfast #0, %[b]\n"
            "mov   %[i], %[t]\n"
    ".Lxi:   mov   %[j], #8\n"
            "rfbyte %[x]\n"
            "shl %[x], #24\n"
    ".Lxj:   shl %[x], #1  wc\n"
    "if_c    drvh %[mosi]\n"
    "if_nc   drvl %[mosi]\n"
            "drvh %[clk]\n"
            "nop\n"
            "drvl %[clk]\n"
            "nop\n"
            "djnz %[j], #.Lxj\n"
            "djnz %[i], #.Lxi\n"
            :[i]"+r"(i), [j]"+r"(j), [x]"+r"(x), [b]"+r"(buff),
            [mosi]"+r"(mosi), [clk]"+r"(clk), [t]"+r"(bc):);
}

//__attribute__ ((section ("lut"), cogtext, no_builtin("memset")))
void ReceiveSDB(int drive, BYTE *buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int miso = (Pins[drive] >> 24) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int j;
    unsigned int v;

    asm volatile (
           "wrfast #0, %[b]\n"
           "drvh %[mosi]\n"
           "mov R9, %[c]\n"
    ".LxI:  mov %[v], #0\n"
           "mov %[j], #8\n"
    ".LxJ:  drvh %[clk]\n"
           "testp %[miso] wc\n"
           "drvl %[clk]\n"
           "rcl %[v], #1\n"
           "waitx #4\n"
           "djnz %[j], #.LxJ\n"
           "wfbyte %[v]\n"
           "djnz R9, #.LxI\n"
           :[j]"+r"(j), [v]"+r"(v), [b]"+r"(buff),
           [clk]"+r"(clk), [miso]"+r"(miso), [mosi]"+r"(mosi), [c]"+r"(bc));
}

int GetStatus(int drive)
{
    int i = 0;
    BYTE b = 0;
    
    for (i=0;i<50000;i++)
    {
        ReceiveSD(drive, &b, 1);
        if (b == 0xff)
        	return 1;
    }
#ifdef _DEBUG
    __builtin_printf("Drive Not Read\n");
#endif
	return 0;
}

void ReleaseSD(int drive)
{
    int cs;
    BYTE b;
    
    cs = Pins[drive] & 0xff;
    pinh(cs);
    ReceiveSD(drive, &b, 1);
}

int SelectSD(int drive)
{
    int cs;
    BYTE b;
    
    cs = Pins[drive] & 0xff;
    pinl(cs);
    ReceiveSD(drive, &b, 1);
    if (GetStatus(drive))
    	return 1;
    ReleaseSD(drive);
    return 0;
}

int ReceiveBlock(int drive, BYTE *buff, unsigned int bc)
{
    BYTE b[2];
    int i;
   
    for (i=0;i<5000;i++)
    {
        ReceiveSD(drive, b, 1);
        if (*b != 0xff)
        	break;
    }
	if (*b != 0xfe)
		return 0;

    ReceiveSD(drive, buff, bc);
	ReceiveSD(drive, b, 2);    // Ditch CRC
	return 1;
}

int SendBlock(int drive, const BYTE *buff, int token)
{
    BYTE b[2];
    
    if (GetStatus(drive) == 0)
    	return 0;
    
    b[0] = token;
    SendSD(drive, b, 1);
    if (token != 0xfd)
    {
        SendSD(drive, buff, 512);
        ReceiveSD(drive, b, 2);  // CRC value
        ReceiveSD(drive, b, 1);
        if ((*b & 0x1f) != 0x05)
        	return 0;
    }
	return 1;    
}

BYTE SendCommand(int drive, BYTE cmd, unsigned int arg)
{
    BYTE b[6];
    BYTE n;
    
    if (cmd & 0x80)
    {
        cmd = cmd & 0x7f;
        n = SendCommand(drive, CMD55, 0);
        if (n > 1)
        	return n;
    }
    
    if (cmd != CMD12)
    {
        ReleaseSD(drive);
        if (!SelectSD(drive))
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
    SendSD(drive, b, 6);
    
    if (cmd == CMD12)
    	ReceiveSD(drive, &n, 1);

    for (int i=0;i<10;i++)
    {
        ReceiveSD(drive, &n, 1);
        if ((n & 0x80) == 0)
        	break;
    }
#ifdef _DEBUG
    __builtin_printf("Command: %d (%x) --> %d\n", cmd, arg, n);
#endif
    return n;
}
