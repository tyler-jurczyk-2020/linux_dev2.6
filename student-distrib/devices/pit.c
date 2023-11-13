#include "i8259.h"
#include "pit.h"
#include "../lib.h"

void init_pit(){
    enable_irq(IRQ0);
    timer_set(RUNNING_F_L);
}

void pit_handler(){
    /* need to be done */
    printf("PIT! ");
    send_eoi(IRQ0);
}

void timer_set(int freq){
    int rate = BASE_F / freq;
    outb(WRITE_CHO, COMMAND_PORT);
    outb(rate&0xFF, CH0_DATA_PORT);         //0xFF is mast for bit 0-7
    outb(rate&0xFF00 >> 8, CH0_DATA_PORT);  //0xFF00 is mast for bit 8-15, 8 is shift distance
}
