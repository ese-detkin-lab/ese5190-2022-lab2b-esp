#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h" // pio0

#define GPIO_REGISTER_BASE 0x40014000

//typedef          uint32_t   VALUE;
typedef volatile uint32_t * ADDRESS;

uint32_t get_register_value(uint32_t reg_addr_val){
	volatile uint32_t address=GPIO_REGISTER_BASE+reg_addr_val;
	printf("\n%x\n",address);
	return address;
}

uint32_t read_reg(ADDRESS reg_val, int shift_bit)
{
    uint32_t pin_mask = 1u << shift_bit;
    uint32_t pin_state = 0;
    uint32_t shifted_pin_state = 0;
	printf("Pin mask: %x\n",pin_mask);
	printf("Pin state: %x\n",pin_state);
	printf("Shifted Pin state: %x\n",shifted_pin_state);
	printf("val at addr: %x\n",*reg_val);
	pin_state = (*reg_val) & pin_mask;
	printf("Pin state: %x\n",pin_state);
    shifted_pin_state = pin_state >> shift_bit;
    printf("Shifted Pin state: %x\n",shifted_pin_state);
    printf("===============================\n"); 
    return shifted_pin_state;       
}

void write_reg(ADDRESS addr, uint32_t value){
    *addr = value;
}


int main() {
    stdio_init_all();
  

    uint32_t regs[]={0x00000000, 0x000000a8, 0x000000aa, 0x0000007e, 0x00000058, 0x00000039, 0x000000c4, 0x0000006b, 0x00000022, 0x00000091};
	ADDRESS reg_addr;
	uint32_t w_val = 0;
	int ki = 0;
	int bi = 0;

	char b;
    char k;
    while (1) {
		do {
			printf("\nSelect register (0 to 9): ");
			scanf("%c", &k);
		} while(k<'0' || k>'9');
		ki = k - '0';
		do {
			printf("\nSelect valid bit (0 to 7): ");
			scanf("%c", &b);
		} while(b<'0' || b>'7');
		bi = b - '0';
		reg_addr = (ADDRESS) get_register_value(regs[ki]);
		printf("\nYou chose bit %d.\nAnd register %d with address %x.\n", bi, ki, reg_addr); //Check output
		printf("%x", read_reg(reg_addr, bi));
		printf("\nValue to write to %x? ", reg_addr);
		scanf("%x", &w_val);
		printf("\nWriting %x to %x", w_val, reg_addr);
		write_reg(reg_addr, w_val);
		printf("\nAfter write: %x at %x ", *reg_addr, reg_addr);
		sleep_ms(50);
    }
}
