# 在这里写上你的代码 :-)

import serial
import time


def writemode():
    ser = serial.Serial('COM6', 115200, timeout=1)
    time.sleep(2)
    f = open("C:\Users\70995\pico\Downloads\sequence.txt", "w")
    print('write mode')
    if ser.is_open:C:\Users\70995\pico\Downloads
        # print(ser.readline())
        read = str(ser.readline())
        f.write(read)
        print(read)
        ser.close()
        f.close()
    print('writemode end')


while True:
    ser = serial.Serial('COM6', 115200, timeout=1)
    print('Enter Input:')
    x = input()
    if(x == 'r'):
        # ser = serial.Serial('COM6', 115200, timeout=1)
        time.sleep(2)
        ser.write(b'1')
        print('record mode')
        ser.close()
        time.sleep(10)
        print('recorded')
    else:
        # ser = serial.Serial('COM6', 115200, timeout=1)
        time.sleep(2)
        print("print mode")
        ser.write(b'0')
        ser.close()
        writemode()
        time.sleep(2)
        print("printed")
