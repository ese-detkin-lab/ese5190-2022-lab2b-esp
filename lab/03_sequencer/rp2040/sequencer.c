#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "neopixel.h"
#include "hardware/gpio.h"

#define QTPY_BOOT_PIN 21
#define QTPY_BOOT_PIN_BITMASK 0x200000
#define GPIO_IN_OFFSET 0x4

//should incoming events be stored as bits or bytes?
#define BITMODE 1
#define BYTEMODE 2

typedef volatile uint32_t* regaddr;

#define STORAGE_BUFFER_LENGTH 1000
typedef struct {
    uint32_t data[STORAGE_BUFFER_LENGTH];
    int index;
    uint8_t bitcount;
} seqstorage;

const seqstorage STORAGE_INIT = {.index = 0, .bitcount = 0};

typedef struct {
    volatile uint32_t* address;
    uint32_t value;
    uint32_t mask;
    char* msg;
} UiData;

// defines which register to use as input/output for the squencer, and if that input is atomic
typedef struct {
    volatile uint32_t* address;
    uint32_t mask;
    bool is_binary;
} inoutreg;

typedef struct {
    bool boot_button;
    bool serial;
    bool register_read;
    int storage_mode; //BITMODE or BYTEMODE
} inputsettings;

inoutreg bootpin = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET), QTPY_BOOT_PIN_BITMASK, true};
inputsettings INPUT_SETTINGS = {false, false, false, BITMODE};

uint32_t get_dword_from_serial() {
    return 0xDEADBEEF; //TODO
}

void read_reg_from_ser(inoutreg* reg) {
    reg->address = (regaddr)get_dword_from_serial();
    reg->mask = get_dword_from_serial();
    reg->is_binary = !!(getchar_timeout_us(5000) & 1u);
}

void write_event_to_storage(seqstorage* store, uint32_t data, int mode) {
    //TODO
}

//return true if seqstorage.index is less than STORAGE_BUFFER_LENGTH
inline bool check_storage_capacity(seqstorage* store) {
    return true; //TODO
}


uint32_t read_register_value(regaddr address, uint32_t mask) {
    return 0x00000001; //TODO
}

//call this every loop while recording
// return false if storage full
bool record_sequence(inoutreg* inputreg, seqstorage* store, uint32_t serial) {
    bool storagefull = false;
    uint32_t val;
    if (INPUT_SETTINGS.boot_button) {
        val = read_register_value(bootpin.address,bootpin.mask);
        write_event_to_storage(store, val, INPUT_SETTINGS.storage_mode);
    }
    if (INPUT_SETTINGS.serial) {
        write_event_to_storage(store, serial, INPUT_SETTINGS.storage_mode);
    }
    if (INPUT_SETTINGS.register_read) {
        val = read_register_value(inputreg->address, inputreg->mask);
        write_event_to_storage(store, val, INPUT_SETTINGS.storage_mode);
    }
    return !storagefull;
}

void get_line(char* buf) {
    bool end = false;
    int charnum = 0;
    while(!end) {
        char c = getchar_timeout_us(0);
        if(c != '\n' && c != '\r') {
            buf[charnum] = c;
            putchar(c);
            charnum++;
        }
        else {
            buf[charnum] = 0;
            end = true;
        }
    }
}

int main() {
    bool recordactive = false;
    bool optflag = false;
    int msgtimer = 0;
    
    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    while(!stdio_usb_connected());

    //init input reg to boot button to be safe
    inoutreg inputregister = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET), QTPY_BOOT_PIN_BITMASK, true};

    //init storage struct
    seqstorage store = STORAGE_INIT;

    //var to hold incoming data from serial
    uint32_t serinput = 0;

    while (true) {
        int c = getchar_timeout_us(0);
        if(c == PICO_ERROR_TIMEOUT) {

        }
        else if(c != '\n' && c != '\r' && c != PICO_ERROR_TIMEOUT) {
            /* 
             *  all dword data will be preceeded by a character that denotes what it is
             *  e.g. 'r'0xffffffff is an input register change and the new register address
             *  Some characters are atomic operations, e.g. activate/deactivate an input type 
             */
            switch (c) {
                case 'B': //example of a command to change mode
                    INPUT_SETTINGS.boot_button = true;
                    break;
                case 'b':
                    INPUT_SETTINGS.register_read = false;
                    break;
                case 'X':
                    recordactive = true; //start a recording
                    //clear data here?
                    break;
                case 'x':
                    recordactive = false;//terminate a recording early
                    break;
                default:
                    
                    break;
            }
        }
        if(recordactive) {
            recordactive = record_sequence(&inputregister, &store, serinput);
        }
        
    }
    return 0;
}                  
