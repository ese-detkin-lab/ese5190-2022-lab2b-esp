#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "neopixel.h"
#include "hardware/gpio.h"

#define QTPY_BOOT_PIN 21

#define QTPY_BOOT_PIN_BITMASK 0x200000

#define GPIO_IN_OFFSET 0x4

typedef struct {
    volatile uint32_t* address;
    uint32_t value;
    uint32_t mask;
    char* msg;
} UiData;

const char uiheader[] = "\n\nCommands: [Change (A)ddress] [Change (V)alue] [Change (M)ask]\n\
          [(W)rite masked value to register] [(C)lear (A|V|M)]\n\
          [Write only masked (B)its to register]\n\
          [(R)ead value from register]\n\n";


void to_bin(uint32_t num, char* buf) {
    char bits[50];
    for (int i = 0; i < 32; i++) {
        bits[i] = (char)(48 + ((num >> (31-i)) & 1u)); //48 is '0' char
    }
    bits[32] = '\n';
    bits[33] = '\0';
    sprintf(buf,"0x%08X    ", num);
    strcat(buf, bits);
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

uint32_t get_hex(char* buf) {
    bool end = false;
    uint32_t ret = 0;
    int charnum = 7;
    while(!end) {
        bool outputchar = true;
        int c = getchar_timeout_us(500);
        if(c == PICO_ERROR_TIMEOUT) {
            end = false;
        }
        else if(c != '\n' && c != '\r' && c != PICO_ERROR_TIMEOUT) {
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
                putchar(c);
                charnum--;
            }
            
        }
        else {
            //buf[7-charnum] = 0;
            ret = ret >> ((charnum + 1) * 4);
            end = true;
        }
        if(charnum < 0) {
            //buf[8] = 0;
            printf("max len reached\n");
            return ret;
        }
    }
    printf("end hex: %08X\n", ret);
    return ret;
}

void update_display(UiData data) {
    char addrbuf[70];
    char valbuf[70];
    char maskbuf[70];
    char maskvalbuf[70];
    to_bin((uint32_t)data.address, addrbuf);
    to_bin(data.value, valbuf);
    to_bin(data.mask, maskbuf);
    to_bin(data.value & data.mask, maskvalbuf);

    printf(uiheader);
    printf("Address:        ");
    printf(addrbuf);
    printf("Value:          ");
    printf(valbuf);
    printf("Mask:           ");
    printf(maskbuf);
    printf("Masked Value:   ");
    printf(maskvalbuf);
    printf("\n");
    printf(data.msg);

}

int main() {
    //volatile uint32_t* address = 1;
    UiData data;
    bool clearflag = false;
    bool optflag = false;
    int msgtimer = 0;
    
    char input[100] = ""; 
    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    sleep_ms(1000);
    while(!stdio_usb_connected());
    data.value = 0;
    data.address = (volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET);
    data.mask = 0x00200000;
    data.msg = "Enter a command: ";
    printf("\n");

    printf("address: %X\n", data.address);
    while (true) {
        int c = getchar_timeout_us(0);
        if(c == PICO_ERROR_TIMEOUT) {
            msgtimer++;
            if(msgtimer > 50) {
                msgtimer = 0;
                data.msg = "Enter a command: ";
                clearflag = false;
                optflag = false;
            }
        }
        else if(c != '\n' && c != '\r' && c != PICO_ERROR_TIMEOUT) {
            switch (c) {
                case 'A':
                case 'a':
                    data.msg = "Set new address: ";
                    update_display(data);
                    data.address = clearflag ? 0 : (volatile uint32_t*)get_hex(input);
                    data.msg = "Address set!";
                    break;
                case 'V':
                case 'v':
                    data.msg = "Set new value: ";
                    update_display(data);
                    data.value = clearflag ? 0 : get_hex(input);
                    data.msg = "Value set!";
                    break;
                case 'M':
                case 'm':
                    data.msg = "Set new mask: ";
                    update_display(data);
                    data.mask = clearflag ? 0 : get_hex(input);
                    data.msg = "Mask set!";
                    break;
                case 'C':
                case 'c':
                    clearflag = true;
                    data.msg = "Enter (A) (V) or (M) to clear that input";
                    break;
                case 'W':
                case 'w':
                    *data.address = (data.value & data.mask);
                    break;
                case 'B':
                case 'b':
                    *data.address |= (data.value & data.mask);
                    break;
                case 'R':
                case 'r':
                    data.value = *data.address;
                    break;
                case 'O':
                case 'o':
                    data.msg = "Register Options: (N)ormal, Atomic (X)OR, Atomic (S)et, Atomic Cl(E)ar:";
                    optflag = true;
                    break;
                case 'N':
                case 'n':
                    if(!optflag){break;}
                    data.address = (volatile uint32_t*)((uint32_t)data.address & 0xffffcfff);
                    optflag = false;
                    data.msg = "Normal register R/W";
                    break;
                case 'X':
                case 'x':
                    if(!optflag){break;}
                    data.address = (volatile uint32_t*)(((uint32_t)data.address & 0xffffcfff) | 0x00001000);
                    optflag = false;
                    data.msg = "Mode: Register XOR on write";
                    break;
                case 'S':
                case 's':
                    if(!optflag){break;}
                    data.address = (volatile uint32_t*)(((uint32_t)data.address & 0xffffcfff) | 0x00002000);
                    optflag = false;
                    data.msg = "Mode: Register set on write";
                    break;
                case 'E':
                case 'e':
                    if(!optflag){break;}
                    data.address = (volatile uint32_t*)((uint32_t)data.address | 0x00003000);
                    optflag = false;
                    data.msg = "Mode: Register clear on write";
                    break;
                default:
                    data.msg = "Unknown command";
                    break;
            }
        }
        update_display(data);
        sleep_ms(100); // don't DDOS the serial console
    }
    return 0;
}                  
