#! /usr/bin/env python
#
# python I2C serial port SDA/SCL mapping
#
# (C)2006 Patrick Nomblot <pyI2C@nomblot.org>
# this is distributed under a free software license, see license.txt


import sys
import time
import serial

InterfaceName='COM'


#---------------------------------------------------------------------------------
class Driver:
    """ 

 I2C component on serial com port

   __________________________________________________________________
  | V24 PIN |    Fonction       |  Registre     |  port   | EEPROM   |
  |---------|-------------------|---------------|---------|----------|
  |   PIN   |   No  | Abv.| Abv.|               | PC<->TD |          |
  |DB25| DB9| CCITT | Ang.| Fr. |               |ETTD ETCD|          |
  |---------|-------|-----|-----|---------------|---------|----------|
  |  7 |  5 |  102  | SG  | TS  |               |  -----  |   Vss    |
  |  2 |  3 |  103  | TxD | ED  | COM_TBR       |  -----> |          |
  |  3 |  2 |  104  | RxD | RD  | COM_RBR       | <-----  |          |
  |  4 |  7 |  105  | RTS | DPE | COM_MCR bit 1 |  -----> |  SDA out |
  |  5 |  8 |  106  | CTS | PAE | COM_MSR bit 4 | <-----  |  SDA in  |
  |  6 |  6 |  107  | DSR | PDP | COM_MSR bit 5 | <-----  |          |
  | 20 |  4 |  108  | DTR | TDP | COM_MCR bit 0 |  -----> |   SCL    |
  |  8 |  1 |  109  | DCD | DS  | COM_MSR bit 6 | <-----  |          |
  | 22 |  9 |  125  | RI  | IA  | COM_MSR bit 7 | <-----  |          |
  |_________|___________________|_______________|_________|__________|


_____________________________________________________________________________ 


                                                                        
DTR (4/DB9)                 DEL                                             
 O--o---R---o-----------o---|<---o----o     o---------------o-------o SCL
       470  |           |          ...|.....|..             |
            |           _          :  -     / :             |
            |           v          :  ^    |  :             |
            |           |          :..|.....\.:             |
            |           |             |     |               R 10k
            |           o-------------o     |               |
            |                         |     |               |              
            |                       -----  -----            |
            |                       /////  \\\\\            | 
            |           D                                   |     
 (-12v)     o-------o--->|---o--------o     o-------o---R---o + 5v
 TxD (3/DB9)                 |     ...|.....|..        470  |    
 o-------------o             |     :  \     v :             |
               |             R 47k :   |    - :             |
               R 4.7k        |     :../.....|.:             |
 CTS (8/DB9)   |             |        |     |               R 10k
 o-------------o-------------o--------o     |               |
                                            |               |
                                            |               |
 RTS (7/DB9)                     DEL        |               |
 O--o---R---o----------------o---<|---o     o---------------o-------o SDA
       470                   |        |     |
                             |     ...|.....|..
                             v D   :  _     / :
                             -     :  ^    |  :
                             |     :..|.....\.:
                             |        |     |
                             o--------o     |
                                      |     |
                                    -----  -----                         
                                    /////   \\\\\

 

"""

    def __init__(self, name):
        self.setSpeed(1200)   # speed depends on hardware
        self.port = serial.Serial(int(name[len(InterfaceName):])-1)

    def close(self):
        self.port.close()

    def setSpeed(self, freq):
        self.delay = 1/(0.0000001+freq)


    def write(self, sda, scl):
        self.port.setRTS(sda)
        self.port.setDTR(scl)
        time.sleep(self.delay)

    def read(self):
        return not self.port.getCTS()


def getDevices():
    # Gives back availables interfaces names
    available = []
    import serial
    if not 'serial' in dir():
        sys.stderr.write('pySerial is not installed\n')
    else:
        for i in range(16):
            try:
                p = serial.Serial(i)
                available.append('%s%d'%(InterfaceName,i+1))
                del(p)
            except serial.SerialException:
                pass
    return available

