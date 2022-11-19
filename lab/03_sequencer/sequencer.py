import csv
import serial
import numpy as np
import time


while True:
    #Serial console
    qtpy = serial.Serial('/dev/tty.usbmodem141301',115200)
    print(qtpy.name)
    ## get user choice for mode
    print("What do you want to do?\n1. Record button pressing for 5s\n2. Replay button pressing via LED\n");
    ans = input()
    print(type(ans))
    print(ans)
    if ans == '1' or ans == '2' or ans == '0':
        qtpy.write(bytearray(ans,'ascii'))
        print("your input is: " + ans)

    ##record button mode
    if ans == '1':
        row_list = ['']*10000000
        c = 0
        while(True):
            this_row = qtpy.readline()
            print(str(this_row))
            if(this_row == b'reached\r\n'):
                break
            this_row = str(this_row)[2:-5]
            this_row = this_row.split(',')
            row_list[c] = this_row
            c = c+1

        print("Finished recording\r\n")

        #write to file
        #print(row_list)
        with open("sequencer.csv", mode="w", newline="") as csvfile:
            # create a writer object
            csvwriter = csv.writer(csvfile)
            # use the writerows method
            csvwriter.writerows(row_list)
        qtpy.close()


    if ans == '2':
        while True:
            ready = qtpy.readline()
            if ready == b'Ready to start replay on LED...\r\n':
                print(ready)
                break

        #Read from file
        with open('sequencer.csv', newline='') as recorded:
            reader = csv.reader(recorded)
            pre_status = 0
            for row in reader:
                if row == []:
                    break
                led_status = row[1] #clean the " 0" to "0"
                led_status = led_status[1:]
                
                #print(led_status)
                if led_status == '1' and pre_status == '0':
                    #print(qtpy.readline())
                    qtpy.write(bytearray('1','ascii'))
                    #print("writing:" + led_status)
                    
                if led_status == '0' and pre_status == '1':
                    qtpy.write(bytearray('0','ascii'))
                    
                pre_status = led_status  
               
        qtpy.close()
        time.sleep(1)
    if ans =='0':
        break
                

