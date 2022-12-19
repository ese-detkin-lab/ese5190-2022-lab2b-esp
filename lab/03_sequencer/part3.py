import serial
import time
import csv
import numpy
import neopixel

ser=serial.Serial('COM3', 115200)
log = numpy.zeros((1001, 1))
n=0

file = open('ButtonLog.csv', 'w', newline='')
writer = csv.writer(file)

while n < 1000: #time.time() <= t_end:
    line = ser.readline()
    #log[n,0] = chr(line) #line[9]
    n=n+1
    writer.writerow(chr(line)) #line[9]
    time.sleep(.01)
    if n == 1001:
        print(log)
        file.close()
