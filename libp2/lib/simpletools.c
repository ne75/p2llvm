/*
 * @file high.c
 *
 * @author Andy Lindsay
 *
 * @version 0.85
 *
 * @copyright Copyright (C) Parallax, Inc. 2012.  See end of file for
 * terms of use (MIT License).
 *
 * @brief high function source, see simpletools.h for documentation.
 *
 * @detail Please submit bug reports, suggestions, and improvements to
 * this code to editor@parallax.com.
 */

#include <stdlib.h>
#include <string.h>
#include <smartpins.h>
#include "simpletools.h"                      // simpletools function prototypes
#include "simplei2c.h"
#include "Flash.h"

void ee_init(void);


int _PWMCycle = 4000;
int st_pauseTicks = 200000;
unsigned char _dacPin = 64;
unsigned char _swp = 64;
int st_eeInitFlag = 0;
flash_t *_Flash;


extern void high(int pin);                            // high function definition

extern void low(int pin);                             // low function definition

extern int input(int pin);                            // input function definition

extern unsigned int toggle(int pin);                  // toggle function definition

extern unsigned int reverse(int pin);                 // reverse function definition

extern unsigned int get_state(int pin);               // getState function definition

unsigned int get_direction(int pin)           // getDirection function definition
{
    unsigned int out;

    out = DIRA;
    if (pin > 31)
    {
        pin -= 32;
        out = DIRB;
    }
    //testb not here
    //asm("testb %1, %0 wc\n"
    //    "wrc %0\n"
    //    :"+r"(pin)
    //    :"r"(out));

  return pin;                 // Shift right and mask
}

extern unsigned int get_output(int pin);                        // getOutput function definition

void set_output(int pin, int state)           // setOutput function definition
{
  if(state == 0)
  {
    asm("dirl %0\n"::"r"(pin));
  }
  else
  {
    asm("dirh %0\n"::"r"(pin));
  }
}

unsigned int get_states(int endPin, 
                        int startPin)         // getStates function definition
{
  unsigned int pattern = INA;                 // Copy INA
  if(endPin < startPin)                       // Handle reverse pin size
  {
    int temp = startPin;
    startPin = endPin;
    endPin = temp;
  }
  pattern <<= (31 - endPin);                  // Remove zeros to left
  pattern >>= (31 - endPin + startPin);       // Right justify & return
  return pattern;                             // Right justify & return
}

unsigned int get_directions(int endPin, 
                            int startPin)     // getStates function definition
{
  unsigned int pattern = DIRA;                // Copy INA
  if(endPin < startPin)                       // Handle reverse pin size
  {
    int temp = startPin;
    startPin = endPin;
    endPin = temp;
  }
  pattern <<= (31 - endPin);                  // Remove zeros to left
  pattern >>= (31 - endPin + startPin);       // Right justify & return
  return pattern ;                            // Right justify & return
}

unsigned int get_outputs(int endPin, 
                         int startPin)        // getOutputs function definition
{
  unsigned int pattern = OUTA;                // Copy OUTA
  if(endPin < startPin)
  {
    int temp = startPin;
    startPin = endPin;
    endPin = temp;
  }
  pattern <<= (31 - endPin);                  // Remove values to left
  pattern >>= (31 - endPin + startPin);       // Shift right & return
  return pattern;
}

void set_directions(int endPin, int startPin, unsigned int pattern)
{
  if(endPin < startPin)                       // Handle reverse pin size
  {
    int temp = startPin;
    startPin = endPin;
    endPin = temp;
  }
  unsigned int andMask = -1;
  andMask <<= (31 - (endPin-startPin));
  andMask >>= (31 - endPin);
  andMask = ~andMask;
  unsigned int orMask = pattern << (startPin);
  DIRA = (DIRA & andMask) | orMask;
}

void set_outputs(int endPin, int startPin, unsigned int pattern)
{
  if(endPin < startPin)                       // Handle reverse pin size
  {
    int temp = startPin;
    startPin = endPin;
    endPin = temp;
  }
  unsigned int andMask = -1;
  andMask <<= (31 - (endPin-startPin));
  andMask >>= (31 - endPin);
  andMask = ~andMask;
  unsigned int orMask = pattern << (startPin);
  OUTA = (OUTA & andMask) | orMask;
}

void pause(int time)                          // pause function definition
{ 
  while(time--)                               // Count down pause increments
    _waitx(st_pauseTicks);              // Delay one pause increment
}

void set_pause_dt(int clockticks)               // setPauseDt function definition
{
  st_pauseTicks = clockticks;
}

long count(int pin, long duration)            // count function definition
{
  long transitions;

  long tf = duration * st_pauseTicks;            // Set timeout
  _pinstart(pin, P_COUNT_HIGHS, tf, 0);
  transitions = _rdpin(pin);
  
  return transitions;                         // Return transitions
}

void dac_ctr(int pin, int channel, int dacVal)
{
  _pinl(pin);
  dacVal = dacVal | 0xff;
  _wrpin(pin, P_DAC_124R_3V | dacVal << 8);
  _dacPin = pin;
}

void dac_ctr_res(int bits)
{
  // not implemented
}

void dac_ctr_stop(void)
{
  _pinclr(_dacPin);
}

void freqout(int pin, int msTime, int frequency)
{
  unsigned int f;

  f = _clkfreq / 100000;
  f = frequency * f;
  f = f / 100; 
  _pinstart(pin, P_NCO_FREQ | P_OE, 1, f);
  _wait(msTime);
  _dirl(pin);
}

int pwm_start(unsigned int cycleMicroseconds)
{
  int us = _clkfreq / 1000000;
  _PWMCycle = cycleMicroseconds * us;
  return 0;
}

void pwm_set(int pin, int channel, int tHigh)
{
  _pinstart(pin, P_PWM_TRIANGLE | P_OE, tHigh, 0);
}

void pwm_stop(int pin)
{
  _dirl(pin);
  _wrpin(pin, 0);
}

long pulse_in(int pin, int state)              // pulseIn function definition
{
  unsigned int i;
  unsigned int j;

  j = _clkfreq / 1000000; // milliseconds

  if (state)
      _pinstart(pin, P_HIGH_TICKS | P_INVERT_A, 0, 0);
  else
      _pinstart(pin, P_HIGH_TICKS, 0, 0);

  while ((i = _pinr(pin)) == 0)
      _waitus(10);
  i = _rdpin(pin);
  while ((i = _pinr(pin)) == 0)
      _waitus(10);
  i = _rdpin(pin);
  _dirl(pin);
  _wrpin(pin, 0);

  return i / j;
}

void pulse_out(int pin, int time)              // pulseOut function definition
{
  unsigned int i;
  unsigned int j;

  j = _clkfreq / 1000000;
  time = time * j;
  if (_pinr(pin))
  {
    _pinl(pin);
    _waitx(time);
    _pinh(pin);
  }
  else
  {
    _pinh(pin);
    _waitx(time);
    _pinl(pin);
  }
}

long rc_time(int pin, int state)              // rcTime function definition
{
  unsigned int tDecay;

  return tDecay;                              // Return measurement
}

void square_wave(int pin, int channel, int freq)
{
  unsigned int f;

  f = _clkfreq / 100000;
  f = freq * f;
  f = f / 100; 
  _pinstart(pin, P_NCO_FREQ | P_OE, 1, f);
  _swp = pin;
}

void square_wave_stop(void)
{
  _dirl(_swp);
  _wrpin(_swp, 0);
  _swp = 64;
}

void set_io_timeout(long clockTicks)          // setTimeout function definition
{
  //st_timeout = clockTicks;
}

void set_io_dt(long clockticks)                 // setst_iodt function definition
{
  //st_iodt = clockticks;
}

int shift_in(int pinDat, int pinClk, int mode, int bits)
{
  int vi, vf, inc;
  int value = 0; 
  int preflag = 0;
  if((mode == MSBPRE)||(mode == LSBPRE)) preflag = 1;
  switch(mode)
  {
    case MSBPRE:
      vi = bits - 1;
      vf = -1;
      inc = -1;
      //value |= (_pinr(pinDat) << bits);
      break;
    case LSBPRE:
      vi = 0;
      vf = bits;
      inc = 1;
      value |= _pinr(pinDat);
      break;
    case MSBPOST:
      vi = bits -1;
      vf = -1;
      inc = -1;
      break;
    default: // case LSBPOST:
      vi = 0;
      vf = bits;
      inc = 1;
      break;
  }   
  _pinl(pinClk);
  int i;
  for(i = vi; i != vf; i += inc)
  {
    if(preflag) value |= (_pinr(pinDat) << i); 
    toggle(pinClk);
    toggle(pinClk);
    //if(!i)
    //{
    //  if(preflag) break;
    //}
    if(!preflag) value |= (_pinr(pinDat) << i); 
  }
  return value;
}

void shift_out(int pinDat, int pinClk, int mode, int bits, int value)
{
  int vi, vf, inc;
  set_direction(pinDat, 1);
  if(mode == LSBFIRST)
  {
    vi = 0;
    vf = bits;
    inc = 1;
  }
  else
  {
    vi = bits - 1;
    vf = -1;
    inc = -1;
  }
  _pinl(pinClk);
  int i;
  for(i = vi; i != vf; i += inc)
  {
    set_output(pinDat, (value >> i) & 1);
    toggle(pinClk);
    toggle(pinClk);
  }
}

i2c *i2c_newbus(int sclPin, int sdaPin, int sclDrive)
{
  i2c *dev = malloc(sizeof(i2c));

  i2c *busID = i2c_open(dev, sclPin, sdaPin, sclDrive);

  return busID;
}  

int  i2c_out(i2c *busID, int i2cAddr, int memAddr, int memAddrCount, 
                     const unsigned char *data, int dataCount)
{
  int n  = 0;
  i2cAddr <<= 1;
  i2cAddr &= -2;
  i2c_start(busID);
  if(i2c_writeByte(busID, i2cAddr)) return n; else n++;
  int m;

  unsigned char *temp;

  if(memAddrCount)
  {
    if(memAddrCount > 0)
    {
      endianSwap(&m, &memAddr, memAddrCount);
    }  
    else 
    {
      m = memAddr;
      memAddrCount = - memAddrCount;
    }  
    n += i2c_writeData(busID, (unsigned char*) &m, memAddrCount);
  }  
  if(dataCount)
  {
    if(dataCount > 0)
      n += i2c_writeData(busID, data, dataCount);
    else  
    {
      dataCount = -dataCount;
      temp = malloc(dataCount);
      endianSwap(temp, (void*) data, dataCount);
      n += i2c_writeData(busID, temp, dataCount);
      free(temp);
    }        
  }  
  i2c_stop(busID);
  return n;  
}

int  i2c_in(i2c *busID, int i2cAddr, int memAddr, int memAddrCount, 
                     unsigned char *data, int dataCount)
{
  int n  = 0;
  i2cAddr <<= 1;
  i2cAddr &= -2;                                        // Clear i2cAddr.bit0 (write)

  unsigned char *temp;

  i2c_start(busID);
  if(i2c_writeByte(busID, i2cAddr)) return n; else n++;
  if(memAddrCount) 
  {
    int m;
    if(memAddrCount)
    {
      if(memAddrCount > 0)
      {
        endianSwap(&m, &memAddr, memAddrCount);
      }  
      else 
      {
        m = memAddr;
        memAddrCount = - memAddrCount;
      }  
      n += i2c_writeData(busID, (unsigned char*) &m, memAddrCount);
    }  
  }  
  i2cAddr |= 1;                                       // Set i2cAddr.bit0 (read)
  i2c_start(busID);
  if(i2c_writeByte(busID, i2cAddr)) return n; else n++;
  n += i2c_readData(busID, data, abs(dataCount));
  i2c_stop(busID);
  if(dataCount < 0)
  {
    dataCount = -dataCount;
    temp = malloc(dataCount);
    memcpy(temp, data, dataCount);
    endianSwap(data, temp, dataCount);
    free(temp);
  }
  return n;  
}

int i2c_busy(i2c *busID, int i2cAddr)
{
  i2cAddr <<= 1;
  i2cAddr &= -2;
  int status = i2c_poll(busID, i2cAddr);
  return status;
}  

void ee_putByte(unsigned char value, int addr)
{
  if (!st_eeInitFlag)
    ee_init();
  Flash_Write(_Flash, addr, &value, 1);
}

unsigned char ee_getByte(int addr)
{
  unsigned char x;

  if (!st_eeInitFlag)
    ee_init();
  Flash_Read(_Flash, addr, (unsigned char*)&x, 1);
  return x;
}

void ee_putInt(int value, int addr)
{
  if (!st_eeInitFlag)
    ee_init();
  Flash_Write(_Flash, addr, (unsigned char *)&value, 4);
}

int ee_getInt(int addr)
{
  int x;

  if (!st_eeInitFlag)
    ee_init();
  Flash_Read(_Flash, addr, (unsigned char*)&x, 4);
  return x;
}

void ee_putStr(unsigned char *s, int n, int addr)
{
  if (!st_eeInitFlag)
    ee_init();
  Flash_Write(_Flash, addr, s, n);
}

unsigned char* ee_getStr(unsigned char* s, int n, int addr)
{
  if (!st_eeInitFlag)
    ee_init();
  Flash_Read(_Flash, addr, s, n);
  return s;
}

void ee_putFloat32(float value, int addr)
{
  if (!st_eeInitFlag)
    ee_init();
  Flash_Write(_Flash, addr, (unsigned char *)&value, 8);
}

float ee_getFloat32(int addr)
{
  float x;

  if (!st_eeInitFlag)
    ee_init();
  Flash_Read(_Flash, addr, (unsigned char *)&x, 8);
  return x;
}

void ee_config(int mosi, int miso, int clk, int dta)
{
  st_eeInitFlag = 1;
  _Flash = Flash_Init(mosi, miso, clk, dta);

}

void ee_init()
{
  st_eeInitFlag = 1;
  _Flash = Flash_Init(59, 58, 60, 61);

}


/**
 * TERMS OF USE: MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
