import csv
import serial
import numpy as np

#Serial console
qtpy = serial.Serial('/dev/tty.usbmodem141301',115200)
row_list = ['']*10000000
print(qtpy.name)
c = 0
while(True):
    #i = input()
    #if i == 'exit':
     #   break
    #qtpy.write(bytearray(i,'ascii'))
##    if(c%2 == 0):
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


#Read from file
with open('sequencer.csv', newline='') as recorded:
    reader = csv.reader(recorded)
    for row in reader:
        print(type(row))
        print(row)
        break

#pixels.fill((200,100,50))


