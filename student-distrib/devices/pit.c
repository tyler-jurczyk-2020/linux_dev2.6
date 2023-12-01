#include "i8259.h"
#include "pit.h"
#include "rtc.h"
#include "../lib.h"
#include "../idt_exceptions_syscalls/pcb.h"
#include "../x86_desc.h"
#include "../filesystem/paging.h"
#include "keyboard.h"
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000
#define VIDEO       0xB8000
//self explanatory
void init_pit(){
    enable_irq(IRQ0);
    timer_set(RUNNING_F_H);
}
/* pit_handler()
 * INPUTS : none
 * OUTPUTS: none
 * Function: finds next active process, switches stacks, and returns control to the next active process.
 * Side effects: paging is changed for VMEM, b8 page, and user program page (0x8048...) as well as terminal color and cursor
*/
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
	/* change the paging to allow for scheduler to return to that program*/
	uint8_t avail_process = next_active_pcb->process_id;
    set_pager_dir_entry(EIGHT_MB + FOUR_MB*avail_process);
	flush_tlbs();
	tss.esp0 = EIGHT_MB - (EIGHT_KB*avail_process)-4;
    tss.ss0 = KERNEL_DS;
	
	if(next_active_pcb->terminal_info.is_onscreen){
	    update_kernel_vmem(VIDEO, VIDEO);	
        update_vidmap_vmem(VIDEO, USER_PAGE);
	}else{
        update_kernel_vmem(next_active_pcb->terminal_info.fake_page_addr, VIDEO);
        update_vidmap_vmem(next_active_pcb->terminal_info.fake_page_addr, USER_PAGE);
	}
    flush_tlbs();
    switch_cursor(curr_pcb->terminal_info.terminal_num, next_active_pcb->terminal_info.terminal_num);
    if (next_active_pcb->terminal_info.is_onscreen) {
        update_cursor();
    }
	ATTRIB = (uint8_t)color[(uint8_t)next_active_pcb->terminal_info.terminal_num];
	send_eoi(0);
	do_schedule((uint32_t)next_active_pcb->schedule_ebp,(uint32_t)next_active_pcb->schedule_esp);

	
}
//self explanatory
void timer_set(int freq){
    int rate = BASE_F / freq;
    outb(WRITE_CHO, COMMAND_PORT);
    outb(rate&0xFF, CH0_DATA_PORT);         //0xFF is mast for bit 0-7
    outb(rate&0xFF00 >> 8, CH0_DATA_PORT);  //0xFF00 is mast for bit 8-15, 8 is shift distance
}
