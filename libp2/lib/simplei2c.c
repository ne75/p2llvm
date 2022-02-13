/**
 * @file simplei2c.c
 * Provides simple i2c start, stop, send, and receive functions
 *
 * Copyright (c) 2013, Parallax Inc.
 */

#include <stdlib.h>
#include "simplei2c.h"

void all_low(i2c *dev)
{
  _pinl(dev->scl);
  _pinl(dev->sda);

}

void all_high(i2c *dev)
{

  if(dev->drivescl) {
    _pinh(dev->sda);
    _pinh(dev->scl);
  }
  else {
    _pinh(dev->sda);
    _dirl(dev->scl); // float the pin
  }
}

void scl_low(i2c *dev)
{
  _pinl(dev->scl);
}

void sda_low(i2c *dev)
{
  _pinl(dev->sda);
}

void scl_high(i2c *dev)
{

  if(dev->drivescl) {
    _pinh(dev->scl);
  }
  else {
    _dirl(dev->scl); // float the pin
  }
}

void sda_high(i2c *dev)
{
  _pinh(dev->sda);
}


i2c *i2c_open(i2c *dev, int sclPin, int sdaPin, int sclDrive)
{
  dev->scl = sclPin;
  dev->sda = sdaPin;
  dev->drivescl = sclDrive;
  i2c_stop(dev);
  return dev;
}

void i2c_close(i2c *dev)
{
  _dirl(dev->scl);
  _dirl(dev->sda);
  free(dev);
}

void i2c_start(i2c *dev)
{
  all_high(dev);
  sda_low(dev);
  scl_low(dev);
}

void i2c_stop(i2c *dev)
{
  all_low(dev);
  scl_high(dev);
  sda_high(dev);
}

int i2c_writeByte(i2c *dev, int byte)
{
  int result;
  int count = 8;

  /* send the byte, high bit first */
  do {
    if (byte & 0x80)
      _dirl(dev->sda);
    else
      _dirh(dev->sda);
    scl_high(dev);
    byte <<= 1;
    scl_low(dev);
  } while(--count > 0);
  _dirl(dev->sda);
  /* get ack */
  scl_high(dev);
  result = _pinr(dev->sda);
  scl_low(dev);
  return result != 0;
}

int i2c_readByte(i2c *dev, int ackState)
{
  int byte = 0;
  int count = 8;

  _dirl(dev->sda);

  for (count = 8; --count >= 0; ) {
    byte <<= 1;
    scl_high(dev);
    byte = _pinr(dev->sda) ? 0: 1;
    while(!_pinr(dev->scl));  /* clock stretching */
    /* scl_low(bus); // slow */
    _dirl(dev->scl);
    // float pin
  }

  /* acknowledge */
  if (ackState)
    _dirl(dev->sda);
  else
    _dirh(dev->sda);
  scl_high(dev);
  scl_low(dev);

  return byte;
}

int  i2c_writeData(i2c *dev, const unsigned char *data, int count)
{
  int n  = 0;
  int rc = 0;
  while(count-- > 0) {
    rc |= i2c_writeByte(dev, (int) data[n]);
    if(rc)
      return n;
    n++;
  }
  return n;
}

int  i2c_readData(i2c *dev, unsigned char *data, int count)
{
  int n = 0;
  while(--count > 0) {
    data[n] = (unsigned char) i2c_readByte(dev, 0);
    n++;
  }
  data[n] = (unsigned char) i2c_readByte(dev, 1);
  return n;
}

int  i2c_poll(i2c *dev, int devaddr)
{
  int ack = 0;
  i2c_start(dev);
  ack = i2c_writeByte(dev, devaddr);
  return ack;
}

/*
+--------------------------------------------------------------------
| TERMS OF USE: MIT License
+--------------------------------------------------------------------
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
+--------------------------------------------------------------------
*/

