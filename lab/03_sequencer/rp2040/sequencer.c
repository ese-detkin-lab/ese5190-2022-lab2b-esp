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


// Already not a fan of this setup but I'll wait to refactor until the next section
// should have array of struct to store source register (with magic number to represent serial),
// mask, and value so that when writing, the relevant bit would be known no matter the source (since
// multiple sources per capture is desired). None of this bitmode/bytemode stuff. Workaround for now 
// is to condense value to a boolean when outputting to a binary format (all LED modes).
#define STORAGE_BUFFER_LENGTH 1000
typedef struct {
    uint32_t data[STORAGE_BUFFER_LENGTH];
    int index;
    uint8_t bitcount;
    int storage_mode; //BITMODE or BYTEMODE.
    int recordlen; //set to index upon write, not reset for read/replay.
} seqstorage;

const seqstorage STORAGE_INIT = {.index = 0, .bitcount = 0, .storage_mode = BYTEMODE, .recordlen = STORAGE_BUFFER_LENGTH};

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

//consume a 32-bit hexadecimal number from the serial buffer
bool get_hex(uint32_t* val) {
    bool end = false;
    bool continue_reading = true;
    uint32_t ret = 0;
    int charnum = 7;
    while(!end) {
        bool outputchar = true;
        int c = getchar_timeout_us(0);
        if(c == PICO_ERROR_TIMEOUT) {
            end = false;
        }
        else if(c != '\n' && c != '\r' && c != ',' && c != PICO_ERROR_TIMEOUT) {
            switch (c) {
            case '0':
                break;
            case '1':
                ret += (0x1 << (charnum * 4));
                break;
            case '2':
                ret += (0x2 << (charnum * 4));
                break;
            case '3':
                ret += (0x3 << (charnum * 4));
                break;
            case '4':
                ret += (0x4 << (charnum * 4));
                break;
            case '5':
                ret += (0x5 << (charnum * 4));
                break;
            case '6':
                ret += (0x6 << (charnum * 4));
                break;
            case '7':
                ret += (0x7 << (charnum * 4));
                break;
            case '8':
                ret += (0x8 << (charnum * 4));
                break;
            case '9':
                ret += (0x9 << (charnum * 4));
                break;
            case 'a':
            case 'A':
                ret += (0xA << (charnum * 4));
                break;
            case 'b':
            case 'B':
                ret += (0xB << (charnum * 4));
                break;
            case 'c':
            case 'C':
                ret += (0xC << (charnum * 4));
                break;
            case 'd':
            case 'D':
                ret += (0xD << (charnum * 4));
                break;
            case 'e':
            case 'E':
                ret += (0xE << (charnum * 4));
                break;  
            case 'f':
            case 'F':
                ret += (0xF << (charnum * 4));
                break;
            
            default:
                outputchar = false;
                break;
            }
            if(outputchar) {
                //buf[7-charnum] = c;
                //putchar(c);
                charnum--;
            }
            
        }
        else if (c == '\0') {
            continue_reading = false;
            end = true;
        }
        else {
            //buf[7-charnum] = 0;
            ret = ret >> ((charnum + 1) * 4);
            end = true;
        }
        if(charnum < 0) {
            //buf[8] = 0;
            //printf("max len reached\n");
            end = true;
        }
    }
    //printf("end hex: %08X\n", ret);
    *val = ret;
    return continue_reading;
}

void read_reg_from_ser(inoutreg* reg) {
    reg->address = (regaddr)get_dword_from_serial();
    reg->mask = get_dword_from_serial();
    reg->is_binary = !!(getchar_timeout_us(5000) & 1u);
}

// store value in buffer, return true if buffer did not become full (can store more)
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
        store->recordlen = store->index;
    }
    return (store->index < STORAGE_BUFFER_LENGTH);
}

// return next value in buffer, return false if all values have been read
bool read_event_from_storage(seqstorage* store, uint32_t* data, bool* edgedetected) {
    if (store->index >= store->recordlen) {return false;}
    *data = store->data[store->index];
    store->index++;
    if (store->index != 0) {*edgedetected = !!(*data) ^ !!(store->data[store->index]);}
    return (store->index <= store->recordlen);
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

void import_sequence(seqstorage* store) {
    bool storage_available = true;
    store->index = 0;
    uint32_t val = 0;
    while (storage_available) {
        if (!get_hex(&val)) {
            putchar('\x00');
            return;
        }
        storage_available = write_event_to_storage(store, val);
    }
}

void export_sequence(seqstorage* store) {
    uint32_t data;
    bool* edge = false;
    store->index = 0;
    while(read_event_from_storage(store, &data, edge)) {
        printf("%08X\n", data);
    }
    printf("\x3B\n");
}

//call this every loop while recording
// return false if storage full
bool record_sequence(inoutreg* inputreg, seqstorage* store, uint32_t serial) {
    bool roominbuffer = false;
    uint32_t val;
    if (INPUT_SETTINGS.boot_button) {
        val = (read_register_value(&bootpin));
        roominbuffer = write_event_to_storage(store, val);
    }
    if (INPUT_SETTINGS.serial) {
        //roominbuffer = write_event_to_storage(store, serial);
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
    bool edgedetected = false;
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

void report_error(char* msg) {
    putchar('E');
    printf(msg);
    printf("\n");
}

int main() {
    bool recordactive = false;
    bool playbackactive = false;
    bool loopplayback = false;
    
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
                //case 'T': 
                    //INPUT_SETTINGS.serial = true;
                    //putchar('T');
                    //break;
                //case 't':
                    //INPUT_SETTINGS.serial = false;
                    //putchar('t');
                    //break;
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
                    LED_STATE = (ledstate){true, false, false, false};
                    putchar('l');
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
                    putchar('c');
                    putchar('L');
                    LED_STATE = (ledstate){true, true, false, false};
                    set_led_state(&LED_STATE);
                    break;
                case 'l':
                    OUTPUT_SETTINGS.led_brightness = false;
                    putchar('l');
                    break;
                 //case 'S': 
                    //OUTPUT_SETTINGS.serial_out = true;
                    //break;
                //case 's':
                    //OUTPUT_SETTINGS.serial_out = false;
                    //break;
                case 'W':
                    if (!(playbackactive || recordactive)) {
                        OUTPUT_SETTINGS.register_write= true;
                        putchar('W');
                    }
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
                case 'U':
                    putchar('U');
                    export_sequence(&store);
                    putchar('u');
                    break;
                case 'D':
                    putchar('D');
                    import_sequence(&store);
                    putchar('d');
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
            if (!playbackactive) {putchar('p');}
        }
        sleep_ms(10);
    }
    return 0;
}                  
