pyI2C
=====

Overview
--------
This module encapsulates the access for I2C components connected to 
parallel or serial port (or any interface which can be reach from PYTHON)
It provides backends for Python running on Windows and Linux. 

This module is still under developement. But it may be useful for
developers.

It is released under a free software license, see LICENSE.txt for more
details.

(C) 2006 Patrick Nomblot pyI2C@nomblot.org

Homepage: http://pyI2C.sourceforge.net

Features
--------
   * handle SDA and SCL pins
   * auto-detect installed drivers
   * auto-detect available interfaces (COM1, COM2, LPT1, etc...) 
   * allow to adapt or add drivers (see directory I2C/drivers)
   * handle I2C components, actually :
      * LM75 familly (themperature sensor)


Requirements
------------
    * Python 2.2 or newer
    * pySerial for I2C on serial port 
    * pyParallel for I2C on parallel port :
       -->  on linux, module ppdev must be installed in place of lp
            (edit /etc/modules)
       -->  on windows, you must setup giveio
    * "Java Communications" (JavaComm) extension for Java/Jython

Installation
------------
Extract files from the archive, open a shell/console in that directory and
let Disutils do the rest:
python setup.py install

The files get installed in the "Lib/site-packages" directory in newer
Python versions.

Win32 Printer port privilege access
-----------------------------------
pyParallel will need you to install giveio.sys driver to let you access
to LPT port. For details, please have a look at http://irdeo.de/irdeosw.htm


Short introduction
------------------
example LM75 I2C temperature captor on COM1 (see drivers/com.py for 
hardware details, drivers.lpt.py for printer port solution).

>>> import I2C
>>> import I2C.sensors

>>> i2cBus = I2C.BusI2C('COM1')
>>> sonde = I2C.sensors.LM75('Room 1', i2cBus)
>>> print "T =  %02.03f C" % sonde.getTemperature()



Examples
--------
Please look in the CVS Repository. There is an example directory where you
can find a simple terminal and more.
http://pyI2C.cvs.sourceforge.net/pyserial/examples

