import time

while 1:
    f = open('output.txt', 'r')
    text = f.readline()
    print(text)
    f.close()
    time.sleep(1)
