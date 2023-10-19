#include "idt_exceptions_syscalls.h"
#include "idt_entries.h"
#include "../x86_desc.h"
#include "../lib.h"

/*
Struct to map vector#'s to their english error meaning
Struct defined in handlers.h
*/
interrupt_identifier interrupt_msg_mapper[] = {
	{0, "Divide Error"},
	{1, "reserved_2"},
	{2, "nmi_interrupt"},
	{3, "breakpoint"},
	{4, "overflow"},
	{5, "bound_range_exceeded"},
	{6, "invalid_opcode"},
	{7, "device_not_available"},
	{8, "double_fault"},
	{9, "coprocessor_segment_overrun"},
	{10, "invalid_tss"},
	{11, "segment_not_present"},
	{12, "stack_segment_fault"},
	{13, "general_protection"},
	{14, "page_fault"},
	{15, "reserved_15"},
	{16, "x87_fpu_floating_point_error"},
	{17, "alignment_check"},
	{18, "machine_check"},
	{19, "SIM_floating_point_exception"},
	{80, "system_call"},
	
};
/*
populate_idt: Called in kernel.c during setup, populates the IDT with correct function ptrs. These are listed in idt_entries.S
 inputs: none
 outputs: none
 side effects: sets entries in the idt based on the isr_stub_table created in idt_entries.h/.S
*/
void populate_idt(){
	int i;
	for(i = 0; i<NUM_VEC; i++){
		
		if(isr_stub_table[i] != 0){
			SET_IDT_ENTRY(idt[i], isr_stub_table[i]);
			idt[i].present = 1;
		}else{
			idt[i].present = 0;
		}
		
		idt[i].dpl = 0;
		idt[i].seg_selector = KERNEL_CS;
		idt[i].size = 1;
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 0;
		idt[i].reserved4 = 0;
	}
	idt[128].dpl = 3;//set sys call priveledge level down to user
	idt[128].reserved3 = 1; //set system call to a trap gate
}

/*
system_call_handler: Called whenever a INT occurs (vector x80)
 inputs: parameter(passed through eax) , eflags, general purpose regs
 outputs: none
 side effects: holds the system in a permanent while loop and prints the corresponding parameter
*/
void system_call_handler(unsigned long parameter, unsigned long flags, register_struct regs){
	printf("systemcall number: %x",parameter);
	while(1);
}

/*
Exception_Handler: Called whenever an exception occurs (vectors 1-31 on the table)
 inputs: IDT vector, eflags, general purpose regs
 outputs: none
 side effects: holds the system in a permanent while loop and prints the corresponding error message for the exception
*/
void exception_handler(unsigned long vector, unsigned long flags, register_struct regs){
	printf("exception received... ~\\(0.0)/~ ... oops\n");
	if(vector<20){
		printf("exception: ");
		printf("%s",interrupt_msg_mapper[vector].name);
	}else if (vector<32){
		printf("exception vector: %x",vector);
	}else{
		printf("exception vector: %x",vector);
		printf("invalid exception vector??");
	}
	
	while(1);
}

/* 
**************************************************
BELOW ARE SYSTEM CALL FUNCTIONS + FUNCTION HEADERS
**************************************************
*/
uint32_t halt(uint8_t status){
	putc('1');
	return 0;
}
uint32_t execute(const uint8_t* command){
	putc('2');
	return 0;
}
uint32_t read(uint32_t fd, void* buf, uint32_t nbytes){
	putc('3');
	return 0;
}
uint32_t write(uint32_t fd, const void* buf, uint32_t nbytes){
	putc('4');
	return 0;
}
uint32_t open(const uint8_t* filename){
	putc('5');
	return 0;
}
uint32_t close(uint32_t fd){
	putc('6');
	return 0;
}
uint32_t getargs(uint8_t* buf, uint32_t nbytes){
	putc('7');
	return 0;
}
uint32_t vidmap(uint8_t** screen_start){
	putc('8');
	return 0;
}
uint32_t set_handler(uint32_t signum, void* handler_address){
	putc('9');
	return 0;
}
uint32_t sigreturn(void){
	putc('A');
	return 0;
}