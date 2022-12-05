#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>


#pragma once

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



