#! /usr/bin/env python
#
# python I2C parallel port SDA/SCL mapping
#
# (C)2006 Patrick Nomblot <pyI2C@nomblot.org>
# this is distributed under a free software license, see license.txt

import sys
import time
import parallel

InterfaceName='LPT'


class Driver:
    """ I2C component on a parallel port 

   _______________________________________________________________________
  |   LPT PIN    |    port  IN/OUT   |   |   Fonction    |     EEPROM    |
  |--------------|-------------------|---|---------------|---------------|
  |   PTD0   (2) | LPT_DATA   bit 0  |OUT|    Data 0     |     VCC       |
  |   PTD1   (3) | LPT_DATA   bit 1  |OUT|    Data 1     |     VCC       |
  |   PTD2   (4) | LPT_DATA   bit 2  |OUT|    Data 2     |     VCC       |
  |   PTD3   (5) | LPT_DATA   bit 3  |OUT|    Data 3     |     VCC       |
  |   PTD4   (6) | LPT_DATA   bit 4  |OUT|    Data 4     |     VCC       |
  |   PTD5   (7) | LPT_DATA   bit 5  |OUT|    Data 5     |     VCC       |
  |   PTD6   (8) | LPT_DATA   bit 6  |OUT|    Data 6     |     SCL       |
  |   PTD7   (9) | LPT_DATA   bit 7  |OUT|    Data 7     |     SDA       |
  |--------------|-------------------|---|---------------|---------------|
  |   PD0    (1) | LPT_CONTROL bit 0 |OUT|  - STROBE     |               |
  |   PD1   (14) | LPT_CONTROL bit 1 |OUT|  - AUTOFDX    |               |
  |   PD2   (16) | LPT_CONTROL bit 2 |OUT|  -  INIT      |               |
  |   PD3   (17) | LPT_CONTROL bit 3 |OUT|  - SLCTIN     |               |
  |              | LPT_CONTROL bit 4 |   |  IRQ Enable   |               |
  |              | LPT_CONTROL bit 5 |   |               |               |
  |              | LPT_CONTROL bit 6 |   |               |               |
  |              | LPT_CONTROL bit 7 |   |               |               |
  |--------------|-------------------|---|---------------|---------------|
  |              | LPT_STATUS  bit 0 |   |               |               |
  |              | LPT_STATUS  bit 1 |   |               |               |
  |              | LPT_STATUS  bit 2 |   |               |               |
  |   PD3   (15) | LPT_STATUS  bit 3 |INP|    ERROR      |               |
  |   PD4   (13) | LPT_STATUS  bit 4 |INP|    SLCT       |               |
  |   PD5   (12) | LPT_STATUS  bit 5 |INP|    PE         |               |
  |   PD6   (10) | LPT_STATUS  bit 6 |INP|    ACK        |     SDA       |
  |   PD7   (11) | LPT_STATUS  bit 7 |INP|  - BUSY       |               |
  |--------------|-------------------|---|---------------|---------------|
  |   GND   (18) |                   |   |    0 v        |     Vss       |
  |______________|___________________|___|_______________|_______________|
                               
"""

    def __init__(self, name):
        self.setSpeed(1000) # speed depends on I2C interface
        self.port = parallel.Parallel(int(name[len(InterfaceName):])-1)

    def close(self):
        del (self.port)
        
    
    def setSpeed(self, freq):
        self.delay = 1/(0.0000001+freq) 

    def write(self, sda, scl):
        o = 0x3F             # I2C power ON
        if sda: o |= 0x80    
        if scl: o |= 0x40
        self.port.setData(o)
        time.sleep(self.delay)    

    def read(self):
        return self.port.getInSelected()


def getDevices():
    import parallel 
    available = []
    if not 'parallel' in dir():
        sys.stderr.write('pyParallel is not installed\n')
    else:
        for i in range(4):
            try:
                p = parallel.Parallel(i)
                available.append('%s%d'%(InterfaceName,i+1))
                del(p)
            except:
                pass
    return available


