"""CircuitPython Essentials HID Keyboard example"""
import time
from adafruit_apds9960.apds9960 import APDS9960
import board
# import usb_hid
# from adafruit_hid.keyboard import Keyboard
# from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
# from adafruit_hid.keycode import Keycode
# keyboard = Keyboard(usb_hid.devices)
# keyboard_layout = KeyboardLayoutUS(keyboard)  # We're in the US :)


if __name__ =="__main__":
    i2c = board.STEMMA_I2C()
    apds = APDS9960(i2c)
    apds.color_integration_time=16
    # apds.enable_proximity = True
    # apds.enable_gesture = True
    apds.enable_color=True
    bright=0
    bright_prev=0
    tolerance=200

    # The keyboard object!
    time.sleep(1)  # Sleep for a bit to avoid a race condition on some systems

    # if  input():
    #     pass

    print("Waiting for key pin...")
    r,g,b,bright=apds.color_data

    # keyboard.press( Keycode.INSERT)
    while (r+g+b)>10:
        r,g,b,bright=apds.color_data
        # print('{0},{1},{2}'.format(r,g,b))
        if bright>bright_prev+tolerance:
            while   abs(apds.color_data[3]-bright)>tolerance:
                pass
            print("------------------------input")
            bright_prev=apds.color_data[3]
            # keyboard.send(Keycode.O)

        elif bright<bright_prev-tolerance:
            while bright-apds.color_data[3]>tolerance:
                pass
            print("------------------------backspace")
            bright_prev=apds.color_data[3]
            # keyboard.send(Keycode.BACKSPACE)
        
        # beight_prev= bright
        print(print('{0},\t{1}'.format(bright,bright_prev)))
        time.sleep(0.2)
    print("Keyboard abort")
    # keyboard_layout.write("\nKEYBOARD ABORT")  