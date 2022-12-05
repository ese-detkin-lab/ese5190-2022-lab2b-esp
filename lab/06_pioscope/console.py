from collections import namedtuple
import serial

def upload_sequence(filename):
    outfile = open(filename, 'w')
    while 1:
        rx = rp2040.readline()
        print(rx)
        if serial.to_bytes([0x3B]) in rx:
            return
        #print(rx)
        outfile.write(rx.decode().strip())
        outfile.write('\n')

if __name__ == '__main__':
    rp2040 = serial.Serial('COM4')
    capturenum = 1
    while (1):
        rp2040.close()
        rp2040.open()
        
        while(rp2040.isOpen()):
                rp2040.read_until(b'U')
                upload_sequence(''.join(['lab/06_pioscope/capture_', str(capturenum), '.csv']))
                capturenum += 1

                print('Sequence saved: ', capturenum)

                

    
    