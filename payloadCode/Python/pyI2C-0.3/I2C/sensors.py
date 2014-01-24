#! /usr/bin/env python
#
# python I2C : temperature I2C sensors
#
# (C)2006 Patrick Nomblot <pyI2C@nomblot.org>
# this is distributed under a free software license, see license.txt



import os
import sys
import time
import logging
import thread
import I2C


log = logging.getLogger("I2C")


#---------------------------------------------------------------------------------
class LM75:
    """ LM75 """
    LM75_CFG         = 0x01
    LM75_TH          = 0x02
    LM75_TOS         = 0x03

    def __init__(self, id='', i2c=None, address=0x90, pauseBackground=2, quick=True):
        self.id=id
        self.i2c = i2c
        self.I2Caddress = address
        self.pauseBackground = pauseBackground
        self.init(self.I2Caddress, quick)
        if self.pauseBackground:
            thread.start_new(self.measureBackground, ())
        self.temperature=None
        time.sleep(0.5)


    def word2temp(self, (data1, data2)):
        data2=data2>>4
        temp=float(data1 & 0x7F)
        if data2 !=0 :
            temp += 1/float(data2)
        if (data1 & 0x80) :
            temp*=-1
        return temp


    def init(self, I2Caddress, quick):
        log.debug("LM75 init")
        self.I2Caddress=I2Caddress
        time.sleep (0.1)

        self.i2c.writeRegister(self.I2Caddress, self.LM75_CFG, 0x60)

        if quick==False:
            self.i2c.writeRegister(self.I2Caddress, self.LM75_TOS, 25)
            self.i2c.writeRegister(self.I2Caddress, self.LM75_TH, 25)
            tos = self.word2temp(self.i2c.readRegisterWord(self.I2Caddress, self.LM75_TOS))
            log.debug("LM75 TOS= %01.01f "%tos) 
            th=self.i2c.readRegister(self.I2Caddress, self.LM75_TH)
            log.debug("LM75 TH = %01.01f"%th) 


    def measure(self):
        return self.word2temp(self.i2c.readRegisterWord(self.I2Caddress, 0))

    def measureBackground(self):
        while (1):
            try:
                self.temperature=self.measure()
                log.debug("new value : %01.03f C"%self.temperature)
                time.sleep(self.pauseBackground) 
            except I2C.ProtocolError:
                self.temperature=None
                log.error("LM75 protocol error")
                time.sleep(0.1)

    def getTemperature(self, wait=True):
        if self.pauseBackground:
            while wait and self.temperature == None:
                time.sleep(0.1)
        else:
            self.temperature = self.measure()
        return self.temperature


    def close(self):
        #del (self.i2c)
        pass





#---------------------------------------------------------------------------

if __name__ == '__main__':

    logging.basicConfig()
    log = logging.getLogger("I2C")
    log.setLevel(logging.DEBUG)
    log.info("----- I2C sensors test ---------")

    i2cBus = I2C.BusI2C('LPT1')
    sonde = LM75('Room 1', i2cBus)
    for i in range(10):
        log.debug("T =  %02.03f C" % sonde.getTemperature())
        time.sleep(1)


