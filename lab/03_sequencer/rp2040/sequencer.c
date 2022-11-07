#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "neopixel.h"
#include "hardware/gpio.h"

#define QTPY_BOOT_PIN 21
#define QTPY_BOOT_PIN_BITMASK 0x200000
#define QTPY_GPIO_A0_BITMASK 0x20000000
#define GPIO_IN_OFFSET 0x4

//should incoming events be stored as bits or bytes?
#define BITMODE 1
#define BYTEMODE 2

typedef volatile uint32_t* regaddr;

#define STORAGE_BUFFER_LENGTH 5000
typedef struct {
    uint32_t data[STORAGE_BUFFER_LENGTH];
    int index;
    uint8_t bitcount;
    int storage_mode; //BITMODE or BYTEMODE
} seqstorage;

const seqstorage STORAGE_INIT = {.index = 0, .bitcount = 0, .storage_mode = BYTEMODE};

typedef struct {
    volatile uint32_t* address;
    uint32_t value;
    uint32_t mask;
    char* msg;
} UiData;

//only track off/on for each color and dim/bright for all colors, let's not go crazy...
typedef struct {
    bool red;
    bool green;
    bool blue;
    bool is_bright;
} ledstate;

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
} inputsettings;

typedef struct {
    bool led_color;
    bool led_brightness;
    bool serial_out;
    bool register_write;
} outputsettings;

inoutreg bootpin = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET), QTPY_BOOT_PIN_BITMASK, true};
inputsettings INPUT_SETTINGS = {false, false, false};
outputsettings OUTPUT_SETTINGS = {false, false, false, false};

void set_led_state(ledstate* state) {
    uint32_t red = (state->is_bright ? state->red * 0x90 : state->red * 0x20);
    uint32_t green = (state->is_bright ? state->green * 0x90 : state->green * 0x20);
    uint32_t blue = (state->is_bright ? state->blue * 0x90 : state->blue * 0x20);
    neopixel_set_rgb((red << 16) | (green << 8) | blue);
}

uint32_t get_dword_from_serial() {
    uint32_t ret = 0;
    int bytesrxd = 0;
    int inchar;
    while (bytesrxd < 4) {
        inchar = getchar_timeout_us(0);        
        if(inchar == PICO_ERROR_TIMEOUT) {
            
        }
        else if(inchar != '\n' && inchar != '\r' && inchar != PICO_ERROR_TIMEOUT) {
            ret |= ((uint32_t)inchar << (4 * bytesrxd));
            bytesrxd++;
        }
    }
    return ret;
}

void read_reg_from_ser(inoutreg* reg) {
    reg->address = (regaddr)get_dword_from_serial();
    reg->mask = get_dword_from_serial();
    reg->is_binary = !!(getchar_timeout_us(5000) & 1u);
}

// store value in buffer, return true if value stored successfully
bool write_event_to_storage(seqstorage* store, uint32_t data) {
    if (store->storage_mode == BITMODE) {
        uint32_t mask = (data & 1u) << store->bitcount;
        //clear the bit being assigned, then set it to the value in data
        store->data[store->index] = ((0xffffffff & mask) & store->data[store->index]) | mask;
        store->bitcount++;
        if (store->bitcount > 31) {
            store->bitcount = 0;
            store->index++;
        } 
    }
    else if (store->storage_mode == BYTEMODE) {
        store->data[store->index] = data;
        store->index++;
    }
    return (store->index < STORAGE_BUFFER_LENGTH);
}

bool read_event_from_storage(seqstorage* store, uint32_t* data, bool* edgedetected) {
    if (store->storage_mode == BITMODE) {
        *data = !!(store->data[store->index] & (1u << store->bitcount));
        store->bitcount++;
        if (store->bitcount > 31) {
            store->bitcount = 0;
            store->index++;
        }
    }
    else if (store->storage_mode == BYTEMODE) {
        *data = store->data[store->index];
        store->index++;
    }
    
    return (store->index < STORAGE_BUFFER_LENGTH);
}

uint32_t read_register_value(inoutreg* reg) {
    if (reg->is_binary) {
        return !!(*(reg->address) & reg->mask);
    }
    else {
        return *(reg->address) & reg->mask;
    }
}

void write_register_value(inoutreg* reg, uint32_t value) {
    
    if (reg->is_binary) {
        *(reg->address) = value ? reg->mask : 0u;
    }
    else {
        *(reg->address) = value & reg->mask;
    }

}

//call this every loop while recording
// return false if storage full
bool record_sequence(inoutreg* inputreg, seqstorage* store, uint32_t serial) {
    bool roominbuffer = false;
    uint32_t val;
    if (INPUT_SETTINGS.boot_button) {
        val = read_register_value(&bootpin);
        roominbuffer = write_event_to_storage(store, val);
    }
    if (INPUT_SETTINGS.serial) {
        roominbuffer = write_event_to_storage(store, serial);
    }
    if (INPUT_SETTINGS.register_read) {
        val = read_register_value(inputreg);
        roominbuffer = write_event_to_storage(store, val);
    }
    return roominbuffer;
}

//call every loop while replaying. Two LED options are mutually exclusive
//return false when finished
bool replay_sequence(inoutreg* outputreg, seqstorage* store, ledstate* state) {
    uint32_t data;
    bool edgedetected;
    if(read_event_from_storage(store, &data, &edgedetected)){
        if (OUTPUT_SETTINGS.led_brightness) {
            state->is_bright = !!data;
            set_led_state(state);
        }
        else if (OUTPUT_SETTINGS.led_color) {
            if ((!!data) && edgedetected) {
                ledstate temp = *state;
                state->red = temp.blue;
                state->green = temp.red;
                state->blue = temp.green;
                set_led_state(state);
            }
        }
        if (OUTPUT_SETTINGS.register_write) {
            write_register_value(outputreg, data);
        }
        if (OUTPUT_SETTINGS.serial_out) {
            putchar(!!data ? '1' : 0);
        }
        return true;
    }
    else {
        return false;
    }
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



void report_error(char* msg) {
    putchar('E');
    printf(msg);
    printf("\n");
}

int main() {
    bool recordactive = false;
    bool playbackactive = false;
    bool loopplayback = false;
    bool optflag = false;
    int msgtimer = 0;
    
    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    while(!stdio_usb_connected());
    report_error("test error everything is fine");

    //init in/output reg to boot button/GPIO to be safe
    inoutreg inputregister = bootpin;
    inoutreg outputregister = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_OUT_XOR_OFFSET), QTPY_GPIO_A0_BITMASK, true};

    //init storage struct
    seqstorage store = STORAGE_INIT;

    ledstate LED_STATE = {false, false, false, false};

    //var to hold incoming data from serial
    uint32_t serinput = 0;

    while (true) {
        int c = getchar_timeout_us(0);
        if(c == PICO_ERROR_TIMEOUT) {

        }
        else if(c != '\n' && c != '\r' && c != PICO_ERROR_TIMEOUT) {
            /* 
             *  Data (registers and data to read/write) will be sent in binary form
             *  All dword data will be preceeded by a character that denotes what it is
             *  e.g. 'r'0xffffffff is an input register change and the new register address
             *  Some characters are atomic operations, e.g. activate/deactivate an input type 
             */
            switch (c) {
                case 'B': 
                    INPUT_SETTINGS.boot_button = true;
                    putchar('B');
                    break;
                case 'b':
                    INPUT_SETTINGS.boot_button = false;
                    putchar('b');
                    break;
                case 'T': 
                    INPUT_SETTINGS.serial = true;
                    putchar('T');
                    break;
                case 't':
                    INPUT_SETTINGS.serial = false;
                    putchar('t');
                    break;
                case 'R': 
                    INPUT_SETTINGS.register_read= true;
                    putchar('R');
                    break;
                case 'r':
                    INPUT_SETTINGS.register_read = false;
                    putchar('r');
                    break;
                case 'C':
                    OUTPUT_SETTINGS.led_color = true;
                    OUTPUT_SETTINGS.led_brightness = false;
                    LED_STATE = (ledstate){true, true, false, false};
                    putchar('C');
                    set_led_state(&LED_STATE);
                    break;
                case 'c':
                    OUTPUT_SETTINGS.led_color = false;
                    putchar('c');
                    break;
                case 'L':
                    OUTPUT_SETTINGS.led_brightness = true;
                    OUTPUT_SETTINGS.led_color = false;
                    //LED_STATE = (ledstate){true, true, false, false};
                    putchar('L');
                    set_led_state(&(ledstate){true, true, false, false});
                    break;
                case 'l':
                    OUTPUT_SETTINGS.led_brightness = false;
                    putchar('l');
                    break;
                 case 'S': 
                    OUTPUT_SETTINGS.serial_out = true;
                    break;
                case 's':
                    OUTPUT_SETTINGS.serial_out = false;
                    break;
                case 'W':
                    OUTPUT_SETTINGS.register_write= true;
                    putchar('W');
                    break;
                case 'w':
                    OUTPUT_SETTINGS.register_write = false;
                    putchar('w');
                    break;    
                case 'X':
                    recordactive = true; //start a recording
                    store.index = 0;
                    store.bitcount = 0;
                    putchar('X');
                    break;
                case 'x':
                    recordactive = false;//terminate a recording early
                    putchar('x');
                    break;
                case 'P':
                    playbackactive = true; //start playback
                    store.index = 0;
                    store.bitcount = 0;
                    putchar('P');
                    break;
                case 'p':
                    playbackactive = false;//terminate playback
                    putchar('p');
                    break;
                case 'O':
                    loopplayback = true;
                    putchar('O');
                    break;
                case 'o':
                    loopplayback = false;
                    putchar('o');
                    break;
                default:
                    
                    break;
            }
        }
        if(recordactive) {
            recordactive = record_sequence(&inputregister, &store, serinput);
            if (!recordactive) {putchar('x');}
        }
        if (playbackactive) {
            playbackactive = replay_sequence(&outputregister, &store, &LED_STATE);
            if (!playbackactive && loopplayback) {
                playbackactive = true;
                store.index = 0;
                store.bitcount = 0;
            }
        }
        
    }
    return 0;
}                  
