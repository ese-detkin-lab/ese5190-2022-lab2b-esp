#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "registers.h"
#include "hardware/gpio.h"

typedef struct {
    uint32_t target_address;
    uint32_t value;
    uint32_t write;
    int counter;
    int counter_value;
    int time;
    char address_ch[8];
    char value_ch[8];
    bool rORw;
} Repl; 

void render_to_console(Repl status, char input) {
    // adjust console window height to match 'frame size'
    for (uint8_t i=0; i<10; i++) { 
        printf("\n");
    }
    
    
    if (status.rORw){
        printf("Read Mode\n");
    }else{
        printf("Write Mode\n");
    }
    printf("Current register:  0x%08x\n",   status.target_address);
    printf("Register value:    0x%08x\n",   status.value); 
    printf("Write:             0x%08x\n",   status.write);
    printf("Counter: %d\n",   status.counter);
    printf("Counter_value: %d\n",   status.counter_value);
    printf("input:             %s \n", input);
    printf("Time:              %d \n", status.time);
}  

uint32_t get_uint32_from_string(char charArray[]){
    int p = 0;
    int power = 7;
    uint32_t result = 0x00000000;
    uint32_t currentBitNumber = 0x0;
    int currentnumber = 0;
    while(p < 8){
        if(charArray[p]){
            if(charArray[p] >= '0' && charArray[p] <= '9'){
                currentnumber = (charArray[p] - '0');
            }else{
                currentnumber = (charArray[p] - 'A') + 10;
            }
            currentBitNumber = (uint32_t)currentnumber * (pow(0x10, power));
            result = result + currentBitNumber;
            p++;
            power--;
        }
    }
    return result;
}

int main() {
    stdio_init_all();

    // initialize the register status
    Repl status;
    status.target_address =  0x00000000;
    status.value = 0x00000000;
    status.write = 0x00000000;
    status.rORw = true;
    status.counter = 0;
    status.counter_value = 0;
    status.time = 0;


    char input;
    
    while (true) {
        while(status.counter < 8){
            // read the last serials input
            input = getchar_timeout_us(0);

            // read the mode
            if (input == 'R' || input == 'r'){status.rORw = true;}
            else if (input == 'W' || input == 'w'){status.rORw = false;}

            // ckeck if the input is valid
            if ((input <= '9' && input >= '0') || (input >= 'A' && input <= 'F')){
                status.address_ch[status.counter] = input;
                status.counter ++; 
            }

            render_to_console(status, input);
            sleep_ms(100); // don't DDOS the serial console
            status.time ++;
        }
        
        while(status.counter == 8){
            status.target_address = get_uint32_from_string(status.address_ch);

            // read mode
            if (status.rORw){
                status.value = register_read((volatile uint32_t *) status.target_address);
                // reset the counter
                status.counter = 0;
            }

            // write mode
            else if (!status.rORw) {
                while (status.counter_value < 8){
                    input = getchar_timeout_us(0);

                    if ((input <= '9' && input >= '0') || (input >= 'A' && input <= 'F')){
                        status.value_ch[status.counter_value] = input;
                        status.counter_value ++; 
                    }

                    render_to_console(status, input);
                    sleep_ms(100); // don't DDOS the serial console
                    status.time ++;
                }

                status.write = get_uint32_from_string(status.value_ch);
                register_write((volatile uint32_t *) status.target_address, status.write);
                status.value = register_read((volatile uint32_t *) status.target_address);
                // reset the counter
                status.counter = 0;
                // reset the value counter
                status.counter_value = 0;
            }
        }
    }
}
