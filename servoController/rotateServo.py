import Adafruit_BBIO.PWM as PWM
import time

servo_pin = "P8_13"

# The duty (for 'duty cycle') is the percentage of
# time a signal is high. This controls the speed at which
# the servo rotates.
duty = 85

PWM.start(servo_pin, 100, 60.0)
PWM.set_duty_cycle(servo_pin, duty)

# Give the servo time to rotate.
time.sleep(4)

PWM.stop(servo_pin)
PWM.cleanup()
