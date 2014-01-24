from numpy import *
import time
import random
import cProfile


while 1:
    f = open('output.txt', 'w')
    time.sleep(1)
    v = array([random.random(), random.random(), random.random()])
    print(str(v))
#    timeStart = (time.time())
    f.write(str(v))
#    print('Time difference: ' + str(time.time() - timeStart))
#    print(' ms')
    f.close()
    time.sleep(1)
