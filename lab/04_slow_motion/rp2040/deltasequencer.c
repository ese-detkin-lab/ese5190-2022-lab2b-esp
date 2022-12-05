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


// defines which register to use as input/output for the squencer, and if that input is atomic
typedef struct {
    volatile uint32_t* address;
    uint32_t mask;
    bool is_binary;
} inoutreg;

typedef struct {
    uint32_t delta; //time from start of recording/playback
    inoutreg reg;
    uint32_t value;
} datapoint;

#define STORAGE_BUFFER_LENGTH 1000
typedef struct {
    datapoint data[STORAGE_BUFFER_LENGTH];
    int index;
    int recordlen; //set to index upon write, not reset for read/replay.
    absolute_time_t starttime;
} seqstorage;

const seqstorage STORAGE_INIT = {.index = 0, .recordlen = 0};

//only track off/on for each color and dim/bright for all colors, let's not go crazy...
typedef struct {
    bool red;
    bool green;
    bool blue;
    bool is_bright;
} ledstate;

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
    float scale;
} outputsettings;

inoutreg bootpin = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET), QTPY_BOOT_PIN_BITMASK, true};
inputsettings INPUT_SETTINGS = {false, false, false};
outputsettings OUTPUT_SETTINGS = {false, false, false, false, 1.};

void set_led_state(ledstate* state) {
    uint32_t red = (state->is_bright ? state->red * 0x70 : state->red * 0x20);
    uint32_t green = (state->is_bright ? state->green * 0x70 : state->green * 0x20);
    uint32_t blue = (state->is_bright ? state->blue * 0x70 : state->blue * 0x20);
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
//this is slooooow
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
        else if (c == ';') {
            putchar('$');
            continue_reading = false;
            end = true;
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
        else {
            
            if (charnum < 7) { //handle extra line endings or commas from previous
                ret = ret >> ((charnum + 1) * 4);
                end = true;
            }
        }
        if(charnum < 0) {
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
bool write_event_to_storage(seqstorage* store, uint32_t data, inoutreg* reg) {
    //if the data in this event is the same as the data in the last recorded point, do not store it
    if ((store->index != 0) && (store->data[store->index - 1].value == data)) {return (store->index < STORAGE_BUFFER_LENGTH);}
    
    datapoint point = {.delta = absolute_time_diff_us(store->starttime, get_absolute_time()), .value = data, .reg = *reg};
    store->data[store->index] = point;
    store->recordlen = store->index;
    store->index++;
    

    return (store->index < STORAGE_BUFFER_LENGTH);
}

// return next value in buffer, return false if all values have been read.
bool read_event_from_storage(seqstorage* store, uint32_t* data, inoutreg* reg, bool* newdata, float scale) {
    if (store->index >= store->recordlen) {return false;}
    *newdata = false;
    //if the time between now and the start of playback is less than the stored delta, do not output
    if (absolute_time_diff_us(store->starttime, get_absolute_time()) < (store->data[store->index].delta / scale)) {return (store->index <= store->recordlen);}
    *reg = store->data[store->index].reg;
    *data = store->data[store->index].value;
    store->index++;
    *newdata = true;
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
    uint32_t delta = 0;
    inoutreg reg;
    while (storage_available) {
        //putchar(store->index + 48);
        set_led_state(&(ledstate){.blue = false, .green = true, .red = false, .is_bright = false});
        if (!get_hex(&val)) {putchar('\x3B'); return;}
        delta = val;
        set_led_state(&(ledstate){.blue = false, .green = false, .red = true, .is_bright = false});
        if (!get_hex(&val)) {putchar('\x3B'); return;}
        reg.address = (volatile uint32_t*)val;
        set_led_state(&(ledstate){.blue = true, .green = false, .red = false, .is_bright = false});
        if (!get_hex(&val)) {putchar('\x3B'); return;}
        reg.mask = val;
        set_led_state(&(ledstate){.blue = true, .green = false, .red = true, .is_bright = false});
        if (!get_hex(&val)) {putchar('\x3B'); return;}

        storage_available = write_event_to_storage(store, val, &reg);
        store->data[store->index - 1].delta = delta; //go back in time and overwrite the delta
        sleep_ms(500);
    }
}

void export_sequence(seqstorage* store) {
    store->index = 0;
    while(store->index <= store->recordlen) {
        printf("%08X,%08X,%08X,%08X\n", store->data[store->index].delta,store->data[store->index].reg.address,store->data[store->index].reg.mask,store->data[store->index].value);
        store->index++;
    }
    printf("\x3B\n"); //write semicolon to signal EOF
}

//call this every loop while recording
// return false if storage full
bool record_sequence(inoutreg* inputreg, seqstorage* store, uint32_t serial) {
    bool continuerecording = false;
    uint32_t val;
    if (absolute_time_diff_us(store->starttime, get_absolute_time()) > 10000000) {return false;}
    if (INPUT_SETTINGS.boot_button) {
        val = (read_register_value(&bootpin));
        continuerecording = write_event_to_storage(store, val, inputreg);
    }
    if (INPUT_SETTINGS.serial) {
        //roominbuffer = write_event_to_storage(store, serial);
    }
    if (INPUT_SETTINGS.register_read) {
        val = read_register_value(inputreg);
        continuerecording = write_event_to_storage(store, val, inputreg);
    }
    return continuerecording;
}

//call every loop while replaying. Two LED options are mutually exclusive
//return false when finished
bool replay_sequence(inoutreg* outputreg, seqstorage* store, ledstate* state) {
    uint32_t data;
    bool newdata = false;
    if(read_event_from_storage(store, &data, outputreg, &newdata, OUTPUT_SETTINGS.scale)){
        if (newdata) {
            if (OUTPUT_SETTINGS.led_brightness) {
                state->is_bright = !!data;
                set_led_state(state);
            }
            else if (OUTPUT_SETTINGS.led_color) {
                if (!(!!data)) {
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
                    inputregister = bootpin;
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
                    read_reg_from_ser(&inputregister);
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
                    LED_STATE = (ledstate){true, false, true, false};//the yellow was giving me a headache
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
                    putchar('X');
                    store.starttime = get_absolute_time();
                    break;
                case 'x':
                    recordactive = false;//terminate a recording early
                    putchar('x');
                    break;
                case 'P':
                    playbackactive = true; //start playback
                    store.index = 0;
                    putchar('P');
                    int opt = getchar();
                    switch (opt) {
                        case '1':
                            OUTPUT_SETTINGS.scale = 2;
                            break;
                        case '2':
                            OUTPUT_SETTINGS.scale = 0.5;
                            break;
                        case '3':
                            OUTPUT_SETTINGS.scale = 0.25;
                            break;
                        default:
                            OUTPUT_SETTINGS.scale = 1;
                    }
                    putchar('P');
                    store.starttime = get_absolute_time();
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
                store.starttime = get_absolute_time();
            }
            if (!playbackactive) {putchar('p');}
        }
        sleep_ms(10);
    }
    return 0;
}                  
