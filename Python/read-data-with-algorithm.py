import time
import numpy as NP
from bbio import *

accelXCurr = 0
accelYCurr = 0
accelZCurr = 0

accelXPrev = 0
accelYPrev = 0
accelZPrev = 0

velXCurr = 0
velYCurr = 0
velZCurr = 0

velXPrev = 0
velYPrev = 0
velZPrev = 0

posXCurr = 0
posYCurr = 0
posZCurr = 0

posXPrev = 0
posYPrev = 0
posZPrev = 0

def setup():
  print("Setup!")

  pinMode(USR2, OUTPUT)
  pinMode(USR3, OUTPUT)

  digitalWrite(USR2, HIGH)
  digitalWrite(USR3, LOW)

def loop():
  global velXPrev
  global velYPrev
  global velZPrev
  global posXPrev
  global posYPrev
  global posZPrev
  global accelXCurr
  global accelYCurr
  global accelZCurr
  global accelXPrev
  global accelYPrev
  global accelZPrev

  value = 1

  print("Loop")
  f = open('output.txt', 'r')
  text = f.readline()
  accelCurr = NP.fromstring(text, sep="  ", count=3, dtype=NP.int8)
  accelXCurr = accelCurr[0]
  accelYCurr = accelCurr[1]
  accelZCurr = accelCurr[2]

  velXCurr = NP.add(velXPrev, accelXPrev)
  velYCurr = NP.add(velYPrev, accelYPrev)
  velZCurr = NP.add(velZPrev, accelZPrev)

  posXCurr = NP.add(posXPrev, velXPrev)
  posYCurr = NP.add(posYPrev, velYPrev)
  posZCurr = NP.add(posZPrev, velZPrev)

#  print("Acceleration: (" + str(accelXCurr) + ", " + str(accelYCurr) + 
#", " + str(accelZCurr) + ")")
#  print("Velocity: (" + str(velXCurr) + ", " + str(velYCurr) + ", " + 
#str(velZCurr) + ")")
#  print("Position: (" + str(posXCurr) + ", " + str(posYCurr) + ", " + 
#str(posZCurr) + ")\n") 

  f.close()

  accelXPrev = accelXCurr
  accelYPrev = accelYCurr
  accelZPrev = accelZCurr

  velXPrev = velXCurr
  velYPrev = velYCurr
  velZPrev = velZCurr

  posXPrev = posXCurr
  posYPrev = posYCurr
  posZPrev = posZCurr

  toggle(USR2)
  toggle(USR3)

  analogWrite('P9.14', 1)
 
  time.sleep(1)

run(setup, loop)

# Create a setup function:
#def setup():
#  # Set the two LEDs as outputs: 
#  pinMode(USR2, OUTPUT)
#  pinMode(USR3, OUTPUT)
#
#  # Start one high and one low:
#  digitalWrite(USR2, HIGH)
#  digitalWrite(USR3, LOW)

# Create a main function:
#def loop():
#  # Toggle the two LEDs and sleep a few seconds:
#  toggle(USR2)
#  toggle(USR3)
#  delay(500)

# Start the loop:
#run(setup, loop)
