#include "pico/stdlib.h"
#include <stdio.h>
// #include "neopixel.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "adafruit_qtpy_rp2040.h" 
#include "hardware/gpio.h"
#include "register.h"

#include <stdio.h>
#include "register.h"



// #define MACHINE_SIZE 5
enum e_state{
INPUT=1,
READ_MEM,
WRITE_MEM,
OUTPUT
};
enum e_event{
GET_READ=1,
GET_WRITE, 
DISPLAY,
WAIT_INPUT
};
//
uint8_t event=WAIT_INPUT;
ADDRESS address;
VALUE value;
void funcInput(){
    char input[4];  //"w0x" or "r0x"  NO SPACE ACCEPT
    printf("* Read: r0x12345678\t\t* Write: w0x12345678 0x12345678\n");
    scanf("%3s",input);
    scanf("%x",&address);
    if(input[0]=='r'){
        event= GET_READ;
        for(int i=0;i<3;i++)
            printf("%c",input[i]);
        printf("%8x",address);
    }
    else if(input[0]=='w'){
        for(int i=0;i<3;i++)
            printf("%c",input[i]);
        printf("%8x",address);
        char buf[3]={0};
        scanf("%2s",input);
        scanf("%x",&value);
        printf(" 0x%8x",value);
        event = GET_WRITE;
    }
    return;
}
void funcRead(){
    printf("\nfuncRead address:0x%x",address);
    value = register_read(address);
    event = DISPLAY;
}
void funcWrite(){
    printf("\nfuncWrite address:0x%x,",address);
    register_write(address,value);
    event=DISPLAY;
}
void funcDisplay(){
    printf("\n\tValue=0x%x\n\n",value);
    event = WAIT_INPUT;
}
typedef struct state_table{
    uint8_t curState;
    void (*actFunc)();
    uint8_t triggerEvent;
    uint8_t nextState;
}t_state_table;
t_state_table __table[6]={
    /* curState,actFunc,triggerEvent,nextState*/
    {INPUT,     funcInput,    WAIT_INPUT,   INPUT},
    {INPUT,     funcRead,    GET_READ,   READ_MEM},
    {INPUT,     funcWrite,    GET_WRITE,  WRITE_MEM},
    {READ_MEM,  funcDisplay,     DISPLAY,    OUTPUT},
    {WRITE_MEM, funcDisplay,    DISPLAY,    OUTPUT},
    {OUTPUT,    funcInput,  WAIT_INPUT, INPUT  }
};

typedef struct state_machine{
    uint8_t curState;
    t_state_table *fsmTable;
}fsm;

void register_FSM(fsm* stateMachine,uint8_t* size){
    stateMachine->curState=OUTPUT;
    stateMachine->fsmTable=__table;
    *size = sizeof(__table)/sizeof(t_state_table);
}
void scheduler(uint8_t* cur_state,uint8_t event_, fsm* stateMachine,uint8_t size){
    for(uint8_t i=0;i<size;i++){
        if(*cur_state==stateMachine->fsmTable[i].curState 
                && event_==stateMachine->fsmTable[i].triggerEvent){
                    
            *cur_state=stateMachine->fsmTable[i].nextState;
            stateMachine->fsmTable[i].actFunc();
            break;
        }
    }
    return ;

}


int main() {
    stdio_init_all();
    while(stdio_usb_connected()!=true);
    fsm stateMachine;
    uint8_t size;
    register_FSM(&stateMachine,&size);
    uint8_t currentState=INPUT;
    while(1>0){
        scheduler(&currentState,event, & stateMachine,size);
        // sleep_ms(10); 
    }
    return 0;
}                  
   
