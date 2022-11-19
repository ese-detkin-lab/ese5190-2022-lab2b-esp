import csv
import serial
import numpy as np
import time


while True:
    #Serial console
    qtpy = serial.Serial('/dev/tty.usbmodem14401',115200)
    print(qtpy.name)
    ## get user choice for mode
    print("Start recording\n");
    
    ##record button mode

    row_list = ['']*10000
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
        if c >= 500:
            break;

    print("Finished recording\r\n")

    #write to file
    #print(row_list)
    with open("pio_sequencer.csv", mode="w", newline="") as csvfile:
        # create a writer object
        csvwriter = csv.writer(csvfile)
        # use the writerows method
        csvwriter.writerows(row_list)
    qtpy.close()


    break
                

    #pixels.fill((200,100,50))


