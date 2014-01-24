#! /usr/bin/env python
#
# python I2C
#
# (C)2006 Patrick Nomblot <pyI2C@nomblot.org>
# this is distributed under a free software license, see license.txt



import os
import sys
import thread
import time
import logging
import drivers

I2C_REGISTER_WRITE = 0
I2C_REGISTER_READ = 1

log = logging.getLogger("I2C")

class ProtocolError:pass

version = '0.3'

#---------------------------------------------------------------------------------
class I2Cbits:
    """ I2C SDA/SCL bit level management """


    def _start(self):
        self.bus.write( sda=1, scl=1 )
        self.bus.write( sda=0, scl=1 )
        self.bus.write( sda=0, scl=0 )


    def _send(self, value):
        x=0x80
        while x:
            self.bus.write( sda=((x & value) and 1), scl=0 )
            self.bus.write( sda=((x & value) and 1), scl=1 )
            self.bus.write( sda=((x & value) and 1), scl=0 )
            x=x>>1

    def _read(self):
        x=0x80
        value=0
        while x:
            self.bus.write( sda=1, scl=1 )
            if self.bus.read():
                value+=x  
            self.bus.write( sda=1, scl=0 )
            x=x>>1
        return(value) 


    def _ack(self,error=None):
        self.bus.write( sda=1, scl=0 )
        self.bus.write( sda=1, scl=1 )
        sda=self.bus.read()
        self.bus.write( sda=1, scl=0 )

        if sda and error!=None :
            self.error("I2C NACK: %s"%error)
            self.protocolError = True
        else:
            self.protocolError = False # cancel any previous error
        return(not sda) 


    def _sendAck(self):
        self.bus.write( sda=0, scl=0 )
        self.bus.write( sda=0, scl=1 )
        self.bus.write( sda=0, scl=0 )


    def _sendNack(self):
        self.bus.write( sda=1, scl=0 )
        self.bus.write( sda=1, scl=1 )
        self.bus.write( sda=1, scl=0 )
        self.bus.write( sda=0, scl=0 )


    def _stop(self):
        self.bus.write( sda=0, scl=0 )
        self.bus.write( sda=0, scl=1 )
        self.bus.write( sda=1, scl=1 )
 


#---------------------------------------------------------------------------------
class BusI2C(I2Cbits):
    """ I2C bus """

    def __init__(self, pport='LPT1'):
	self.bus = _DRIVER_FACTORY.create(pport)
        self.protocolError = False
        log.info('I2C started on [%s]'%pport)

    def scan(self, I2C_start_address):
        addressOK = None
        for address in range(I2C_start_address, I2C_start_address+0x10, 2):
            log.info("Scanning address : %02X"%address) 
            self._start() 
            self._send(address)  
            if self._ack():
		addressOK=address
            self._read()
            self._sendNack() 
            self._stop() 
            if addressOK:
                return addressOK
        self.error("no I2C component found !")

    def error(self, error=None):
        raise ProtocolError        


    def writeRegister(self, address, register, value, err="I2C target register access denied !"):
        self._start() 
        self._send(address | I2C_REGISTER_WRITE)
        self._ack(err) 
        self._send(register)
        self._ack(err) 
        self._send(value) 
        self._ack(err) 
        self._send(0) 
        self._ack(err) 
        self._stop() 

    def readRegister(self, address, register, err="cannot _read I2C target register !"):
        self._start() 
        self._send(address | I2C_REGISTER_WRITE)
        self._ack(err) 
        self._send(register)
        self._ack(err) 
        self._stop() 
        time.sleep(0.1)
        self._start() 
        self._send(address | I2C_REGISTER_READ)
        self._ack(err) 
        data=self._read()
        self._sendNack() 
        self._stop() 
        return data

    def readRegisterWord(self, address, register, err="cannot _read I2C target register !"):
        self._start() 
        self._send(address | I2C_REGISTER_WRITE)
        self._ack(err) 
        self._send(register)
        self._ack(err) 
        self._stop() 
        time.sleep(0.1)
        self._start() 
        self._send(address | I2C_REGISTER_READ)
        self._ack(err) 
        data1=self._read()
        self._sendAck() 
        data2=self._read()
        self._sendNack() 
        self._stop() 
        return data1, data2
        


#---------------------------------------------------------------------------------
class DriversFactory:
    """ this class scan for available drivers, available interfaces 
        and build a list for driver factory.

    Each driver returns a list of names corresponding on interface 
    he can handle plus the the way it does that. Two drivers can handle 
    the same periphal in two different ways (not the same I2C pinning), 
    they simply must return different names.

    For example, drivers/Philipps.py could be a copy of drivers/lpt.py 
    adapted to the Philipps I2C demo card pinning. This driver must simply 
    returns a different key string for each port he can drive.

    You may also add drivers for LP printer port (actually, only ppdev is 
    available thrue pyParalell) or KeyBoard, USB, etc...

    Your contribution is welcome !

    """  

    def __init__(self):
        self._drivers = {}

    def register(self, driver, name):
        log.debug('registering driver %s'%name)
        self._drivers[name] = driver

    def create(self, name, *args, **kw):
        driverFactory = self._drivers.get(name)
        if driverFactory is None:
            raise RuntimeError("No driver for %s or ressource busy" % name)
        driver = driverFactory(name, *args, **kw)
        return driver

    def getDrivers(self):
        return self._drivers


_DRIVER_FACTORY = DriversFactory()



# Dynamic driver load is not compatible with py2exe ...
#


import drivers
for driverName in dir(drivers):
    if not driverName.startswith('_'):
        driver=getattr(drivers,driverName)
        for device in driver.getDevices():
            _DRIVER_FACTORY.register(getattr(driver, 'Driver'), device)

#for file in [ file[:-3] for file in os.listdir(os.path.join(installDir, 'drivers')) if file[-3:] == '.py' and file != "__init__.py" ]:
#    driverPath='I2C.drivers.'+file
#    driver=getattr(__import__(driverPath).drivers,file)
#    for device in driver.getDevices():
#        _DRIVER_FACTORY.register(getattr(getattr(__import__(driverPath).drivers,file), 'Driver'), device)

   
		
interfaces = _DRIVER_FACTORY.getDrivers().keys()
interfaces.sort()
