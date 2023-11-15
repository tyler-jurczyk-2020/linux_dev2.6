#include "i8259.h"
#include "pit.h"
#include "../lib.h"
#include "../idt_exceptions_syscalls/pcb.h"

void init_pit(){
    enable_irq(IRQ0);
    timer_set(RUNNING_F_L);
}

void pit_handler(){
    /* get current pcb */
	pcb_t* curr_pcb = get_pcb();
	if(curr_pcb == NULL){
		return;
	}
	//save ebp/esp to use as context to return when the scheduler comes back
	save_regs((uint32_t)&(curr_pcb->schedule_ebp),(uint32_t)&(curr_pcb->schedule_esp));
	
	pcb_t* next_active_pcb = find_next_active_pcb((uint32_t)curr_pcb);
	if(next_active_pcb == NULL){
		send_eoi(0);
		return;
	}
	send_eoi(0);
	do_schedule((uint32_t)next_active_pcb->schedule_ebp,(uint32_t)next_active_pcb->schedule_esp);
}

void timer_set(int freq){
    int rate = BASE_F / freq;
    outb(WRITE_CHO, COMMAND_PORT);
    outb(rate&0xFF, CH0_DATA_PORT);         //0xFF is mast for bit 0-7
    outb(rate&0xFF00 >> 8, CH0_DATA_PORT);  //0xFF00 is mast for bit 8-15, 8 is shift distance
}
