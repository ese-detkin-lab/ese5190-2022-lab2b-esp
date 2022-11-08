from collections import namedtuple
import serial



Command = namedtuple('Command', ['cmd_code', 'data'])



Seqstate = namedtuple('Seqstate', ['led_color_out', 'led_brightness_out', 'serial_out', 'register_write_out', 'boot_btn_in', 'serial_in', 'register_read_in'])


def send_sequence(filename, sequence):
    pass

def upload_sequence(filename):
    outfile = open(filename, 'w')
    while 1:
        rx = rp2040.readline()
        if rx == serial.to_bytes([0x3B, 0x0d, 0x0a]):
            return
        #print(rx)
        outfile.write(rx.decode().strip())
        outfile.write('\n')



if __name__ == '__main__':
    rp2040 = serial.Serial('COM4')
    
    internalstate = Seqstate(False,False,False,False,False,False,False)
    #set some defaults
    #rp2040.write(b'BC') #BOOT button input mode, LED color output mode
    #rp2040.read_until(b'BC') #wait for ACK from RP2040
    
    while (1):
        rp2040.close()
        rp2040.open()
        
        while(rp2040.isOpen()):
            usr_input = input('Select Option:')
            
            if usr_input == 'X':
                rp2040.write(b'X')
                
                print('RP2040 is recording...' )
                rp2040.read_until(b'x') # X will be sent as ACK, x will be sent when record is finished
                print('Finished recording.')
            elif usr_input == 'P':
                rp2040.write(b'P')
                print('Start playback...')
                rp2040.read_until(b'p') # P will be sent as ACK, p will be sent when playback finished
                print('Playback finished')
            elif usr_input == 'O':
                rp2040.write(b'O')
                rp2040.read_until(b'o')
            elif usr_input == 'C':
                rp2040.write(b'C')
                rp2040.read_until(b'C')
            elif usr_input == 'c':
                rp2040.write(b'c')
                rp2040.read_until(b'c')
            elif usr_input == 'L':
                rp2040.write(b'L')
                rp2040.read_until(b'L')
            elif usr_input == 'l':
                rp2040.write(b'l')
                rp2040.read_until(b'l')
            elif usr_input == 'W':
                rp2040.write(b'W')
                rp2040.read_until(b'W')
            elif usr_input == 'B':
                rp2040.write(b'B')
                rp2040.read_until(b'B')
            elif usr_input == 'b':
                rp2040.write(b'b')
                rp2040.read_until(b'b')
            elif usr_input == 'R':
                rp2040.write(b'R')
                rp2040.read_until(b'R')
            elif usr_input == 'r':
                rp2040.write(b'r')
                rp2040.read_until(b'r')
            elif usr_input == 'U':
                rp2040.write(b'U')
                rp2040.read_until(b'U')
                upload_sequence('saved_sequence.txt')
                rp2040.read_until(b'u')
                print('Sequence saved')
            else:
                print('Invalid input')
                

    
    