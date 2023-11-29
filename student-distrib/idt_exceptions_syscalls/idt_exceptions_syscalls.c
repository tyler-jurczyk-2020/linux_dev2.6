#include "idt_exceptions_syscalls.h"
#include "idt_entries.h"
#include "../x86_desc.h"
#include "../lib.h"
#include "../filesystem/paging.h"
#include "../filesystem/filesystem.h"
#include "pcb.h"
#include "../devices/rtc.h"
#include "../devices/keyboard.h"
#include "../devices/pit.h"

#define VIDEO       0xB8000
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
/* NOT IN USE NOW */
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
		printf("%s\n",interrupt_msg_mapper[vector].name);
	}else if (vector<32){
		printf("exception vector: %x\n",vector);
	}else{
		printf("exception vector: %x\n",vector);
		printf("invalid exception vector??\n");
	}
	
	halt(255);
}

/* 
**************************************************
BELOW ARE SYSTEM CALL FUNCTIONS + FUNCTION HEADERS
**************************************************
*/

/*
 * Halt: Called whenever a program finishes execution or a program raises an exception
 * Inputs : Status, 0 if finished normally, 256 if raised an exception, neither for special status
 * Outputs: none (should not return from this function)
 * Side Effects: Frees the current process' kernel stack and jumps to caller's kernel stack to return from execute call
 *               Updates paging and the TSS
 */
uint32_t halt(uint8_t status){
	cli();
    pcb_t *pcb_self = get_pcb();
    // Get parent process
    pcb_t *pcb_parent = get_pcb()->parent;
    if (pcb_parent == NULL) {
		//re execute
		process_ids[pcb_self->process_id] = 0; 
        execute((const uint8_t *)"shell"); 
		return -1;
    }
	pcb_self->is_active = 0;
	//if not from same terminal, should not give back active tag, and should wait until pit moves away
	if(pcb_parent->terminal_info.terminal_num != pcb_self->terminal_info.terminal_num){
		switch_terminal(0);
		process_ids[pcb_self->process_id] = 0;
		sti();
		while(1);
	}

	pcb_parent->is_active = 1;
	switch_terminal(pcb_parent->terminal_info.terminal_num);
	//hand back active tag to parent so scheduler can find
	
	
    process_ids[pcb_self->process_id] = 0; 
	
	
	// Update page directory 
    uint8_t avail_process = pcb_parent->process_id;
    set_pager_dir_entry(EIGHT_MB + FOUR_MB*avail_process);
	flush_tlbs();
    // Setup tss to return to parent
    tss.esp0 = pcb_parent->esp0;
    tss.ss0 = KERNEL_DS;
    // Halt
    halt_process(status,pcb_self->halt_ebp);
	return -1;
}

/*
 * Execute: Tries to execute the inputted command
 * Inputs : command/program to be run
 * Outputs: value passed to halt
 * Side Effects: Changes the paging, opens files, copies executable to memory, allocates kernel stack
 *               Sets up the PCB, gets parent's info, sets up TSS, pushes info to stack to iret into user space
 */
uint32_t execute(const uint8_t* command){
    cli();
	// Check executable
	
    uint8_t copy_cmd[strlen((int8_t *)command)+1];
    copy_cmd[strlen((int8_t *)command)] = '\0';
    strncpy((int8_t *)copy_cmd, (int8_t *)command, strlen((int8_t *)command));
	
	//parse the executable name and arguments from the command
	int8_t parsed_executable[33];
	int8_t parsed_arguments[128];
	uint32_t arg_count = parse_arguments((char*)copy_cmd, (char*)parsed_executable,(char*) parsed_arguments);
	
    int32_t start_of_prog = check_executable((uint8_t *)parsed_executable);
    if (start_of_prog < 0) {
        return -1; 
    }
    // Determine process id
    int8_t avail_process = get_process_id();
    if (avail_process < 0) {
        return -1; 
    }
    // Setup paging for executable
    set_pager_dir_entry(EIGHT_MB + FOUR_MB*avail_process);
	flush_tlbs();
    // Copy executable to memory
    uint32_t eip;
    open_executable(start_of_prog, &eip); 
    // Setup child pcb
    pcb_t *pcb_self = (pcb_t *)(EIGHT_MB - (EIGHT_KB*(avail_process+1))); 
    pcb_t *parent = get_parent_pcb(avail_process);
	if(parent){
		//copy terminal info from parent, set parent to not onscreen
		pcb_self->terminal_info.is_onscreen = parent->terminal_info.is_onscreen;
		pcb_self->terminal_info.terminal_num = parent->terminal_info.terminal_num;
		pcb_self->terminal_info.user_page_addr = parent->terminal_info.user_page_addr;
		pcb_self->terminal_info.fake_page_addr = parent->terminal_info.fake_page_addr;
		parent->terminal_info.is_onscreen = 0;
		//only allow one active process per terminal
		pcb_self->is_active = parent->is_active;
		parent->is_active = 0;
	}else{
		//this is the case of the first terminal
		init_pit();

		pcb_self->terminal_info.is_onscreen = 1;
		pcb_self->terminal_info.terminal_num = 0;
		pcb_self->terminal_info.fake_page_addr = VIDEO + EIGHT_KB/2;

		//set active so scheduler can find it
		pcb_self->is_active = 1;
	}
    setup_pcb(pcb_self, avail_process, parent);
	//TODO make sure this works
	save_regs((uint32_t)&(pcb_self->schedule_ebp),(uint32_t)&(pcb_self->schedule_esp));
	
	strncpy((char*)pcb_self->args,(char*)parsed_arguments,arg_count);
    // Setup TSS
    tss.esp0 = EIGHT_MB - (EIGHT_KB*avail_process)-4;
    tss.ss0 = KERNEL_DS;
    // Setup stack to return to new program
	setup_exec_stack(eip,(uint32_t)&(pcb_self->halt_ebp));

	return 0;
}

/*
 * Read: Uses the read function associated with the file descriptor
 * Inputs : File descriptor (file index), buffer to be copied into, nbytes to copy
 * Outputs: number of bytes read
 * Side Effects: none
 */
uint32_t read(uint32_t fd, void* buf, uint32_t nbytes){
	//check if fd is valid number
	if(fd > 7){
		return -1;
	}
    pcb_t *cur_pcb = get_pcb();
	//check if the file is in use/opened
	if(cur_pcb->available[fd] == 1){
		return -1;
	}
	return cur_pcb->fd[fd].file_ops->read(fd, buf, nbytes);
}
/*
 * Write: Uses the write function associated with the file descriptor
 * Inputs : File descriptor (file index), buffer to write into file, nbytes to write
 * Outputs: number of bytes written
 * Side Effects: none
 */
uint32_t write(uint32_t fd, const void* buf, uint32_t nbytes){
	//check if fd is valid number
	if(fd > 7){
		return -1;
	}
    pcb_t *cur_pcb = get_pcb();
	//check if the file is in use/opened
	if(cur_pcb->available[fd] == 1){
		return -1;
	}
    return cur_pcb->fd[fd].file_ops->write(fd, buf, nbytes);
}
/*
 * Open: Attempts to open a file with the given filename
 * Inputs : filename (name of file to be opened)
 * Outputs: success (filedescriptor 'file index'), or failure (-1)
 * Side Effects: Sets up the pcb entry if filename is correct & there is space
 */
uint32_t open(const uint8_t* filename){
    // Setup pcb entry
    int32_t fd_idx = get_avail_fd();
	if(fd_idx<0){
		make_available_fd(fd_idx);
		return -1;
	}
    file_descriptor_t *file_desc = get_fd(fd_idx);
    if (file_desc == NULL) {
		make_available_fd(fd_idx);
        return -1; 
    }
    int32_t open_res;
    if (!strncmp((const int8_t *)filename, (const int8_t *)"rtc", 3)) {
        open_res = rtc_open((const uint8_t *)"rtc"); 
        if (open_res != 0) {
			make_available_fd(fd_idx);
            return -1;
        }
        file_desc->file_ops = &rtc_table;
        file_desc->inode = -1;
    }
    else if (!strncmp((const int8_t *)filename, (const int8_t *)".", 1)) {
        open_res = dir_open((const uint8_t *)"."); 
        if (open_res != 0) {
			make_available_fd(fd_idx);
            return -1;
        }
        file_desc->file_ops = &directory_table;
        file_desc->inode = -1;
    }
    else {
        open_res = file_open(filename);
        if (open_res < 0) {
			make_available_fd(fd_idx);
            return -1;
        }
        file_desc->file_ops = &file_table;
        file_desc->inode = open_res;
    }
    file_desc->file_pos = 0;
    file_desc->flags = 0;
	return fd_idx;
}

/*
 * Close: Uses the close function associated with the file descriptor, if available
 * Inputs : File descriptor (file index)
 * Outputs: Success/Failue
 * Side Effects: none
 */
uint32_t close(uint32_t fd){
    if (fd < 2 || fd > 7) {
        return -1; 
    }
    pcb_t *cur_pcb = get_pcb();
	//check if the file is in use/opened
	if(cur_pcb->available[fd] == 1){
		return -1;
	}
    cur_pcb->available[fd] = 1;
	return cur_pcb->fd[fd].file_ops->close(fd);
}
uint32_t getargs(uint8_t* buf, uint32_t nbytes){
	pcb_t *cur_pcb = get_pcb();
	int8_t* arguments = cur_pcb->args;
	if(buf == NULL || strlen((const int8_t *)buf) == 0 || nbytes < strlen(arguments)+1){
		return -1;
	}
	strncpy((char*)buf, (char*)arguments, strlen(arguments));
	buf[strlen(arguments)] = '\0';
	return 0;
}
uint32_t vidmap(uint8_t** screen_start){
    if ((int32_t) screen_start < PROGRAM_ADDR || (int32_t) screen_start > PROGRAM_ADDR + FOUR_MB) {
        return -1; 
    }
    *screen_start = (uint8_t *)VMEM_ADDR;//TODO:(get_pcb()->terminal_info.user_page_addr);
	return 0;
}
uint32_t set_handler(uint32_t signum, void* handler_address){
	return -1;
}
uint32_t sigreturn(void){
	return -1;
}
