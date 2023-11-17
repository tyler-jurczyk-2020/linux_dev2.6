#include "keyboard.h"
#include "PS2.h"
#include "../lib.h"
#include "i8259.h"
#include "../x86_desc.h"
#include "../idt_exceptions_syscalls/pcb.h"
#include "../filesystem/paging.h"
#include "../filesystem/filesystem.h"
#include "../filesystem/paging.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000

/*This is the tabled used to decode scan code from keyboard*/
static char scan_code_set_1_norm[62] = {	// 59 characters for what we care
    0x00,  ESC, '1',  '2',
    '3',  '4',  '5',  '6', 
    '7',  '8',  '9',  '0',
    '-',  '=', '\b',  '\t',
    'q',  'w',  'e',  'r',
    't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',
    '\n', 0x00,  'a',  's',
    'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ';',
    QUOTE, '`', 0x00, BACK_SLASH,
    'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',
	'.',  '/', 0x00, 0x00,
	0x00,  ' ', 0x00, 0x00,
	0x00, 0x00
}; 

static char scan_code_set_1_cap[62] = {
    0x00,  ESC, '1',  '2',
    '3',  '4',  '5',  '6', 
    '7',  '8',  '9',  '0',
    '-',  '=', '\b',  '\t',
    'Q',  'W',  'E',  'R',
    'T',  'Y',  'U',  'I',
    'O',  'P',  '[',  ']',
    '\n', 0x00,  'A',  'S',
    'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ';',
    QUOTE, '`', 0x00, BACK_SLASH,
    'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  ',',
	'.',  '/', 0x00, 0x00,
	0x00,  ' ', 0x00, 0x00,
	0x00, 0x00
}; 

static char scan_code_set_1_shift[62] = {
    0x00,  ESC, '!',  '@',
    '#',  '$',  '%',  '^', 
    '&',  '*',  '(',  ')',
    '_',  '+', '\b',  '\t',
    'Q',  'W',  'E',  'R',
    'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',
    '\n', 0x00,  'A',  'S',
    'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ':',
    '"', '~', 0x00,  '|',
    'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',
	'>',  '?', 0x00, 0x00,
	0x00,  ' ', 0x00, 0x00,
	0x00, 0x00
}; 

static char scan_code_set_1_shift_cap[62] = {
    0x00,  ESC, '!',  '@',
    '#',  '$',  '%',  '^', 
    '&',  '*',  '(',  ')',
    '_',  '+', '\b',  '\t',
    'q',  'w',  'e',  'r',
    't',  'y',  'u',  'i',
    'o',  'p',  '{',  '}',
    '\n', 0x00,  'a',  's',
    'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ':',
    '"', '~', 0x00,  '|',
    'z',  'x',  'c',  'v',
    'b',  'n',  'm',  '<',
	'>',  '?', 0x00, 0x00,
	0x00,  ' ', 0x00, 0x00,
	0x00, 0x00
}; 

keyboard_struct keyboard[3];
cursor_pos cursor[3];
uint8_t color[3] = {0x07, 0x20, 0x30};

//volatile uint8_t keyboard_response;
volatile uint8_t old_data = 0x00;
static int shift;
static int capslock;
static int alt;
static int ctrl;
static int holding_count;
/*
Init keyboard
no input
no output
*/
void keyboard_init(){
	int curr_terminal;
	int i;
	for(curr_terminal = 0; curr_terminal < 3; curr_terminal++){
		for(i = 0; i<128; i++){			// size = 128
		keyboard[curr_terminal].buffer[i] = 0;
		}
		keyboard[curr_terminal].top = 0;
		keyboard[curr_terminal].enter_lock = 0;
	}
	shift = 0;
	capslock = 0;
	alt = 0;
	ctrl = 0;
	holding_count = HOLDING_INIT;
    enable_irq(IRQ1); 
}                  


/*handle_keyboard()
description: this is the handler for keyboard
input: none
output: none
side effect: print th typed key on screen, maintain buffer and out_buffer
*/
void handle_keyboard(){
	//ensure that typing interrupts only add to the ONSCREEN TERMINAL, not the terminal of the currently executing process
	int curr_terminal = (int)find_onscreen_terminal_num();
	if(curr_terminal < 0){
		send_eoi(1);
		return;
	}
    //Update paging so that putc actually puts to the screen 
    pcb_t *current_pcb = get_pcb();
    uint32_t old_page = NULL;
    if(!current_pcb->terminal_info.is_onscreen) {
        old_page = current_pcb->terminal_info.fake_page_addr;
        update_kernel_vmem(VIDEO, VIDEO);
        flush_tlbs();
        // If not currently on-screen, and we are not trying to switch terminals
        switch_cursor(current_pcb->terminal_info.terminal_num, curr_terminal);
        update_cursor(); 
    }
	
    uint8_t temp;
    uint8_t current_char = 0x00;
    temp = inb(DATA_PORT);
    if ((temp != old_data || holding_count == 0) && keyboard[curr_terminal].enter_lock == 0){
		
		if (temp != old_data){				// function key should only be triggered once
			
			holding_count = HOLDING_INIT;		

			if (old_data == DOUBLE_CODE){	// check for special double scan code
				switch (temp)
				{
				case RIGHT_ALT_R:
					alt = 0;
					break;
				case RIGHT_CTRL_R:
					ctrl = 0;
					break;

				case RIGHT_ALT_P:
					alt = 1;
					break;
				case RIGHT_CTRL_P:
					ctrl = 1;
					break;
				default:
					break;
				}
			}
			else{
				switch (temp)			// check normal function keys
				{
				case LEFT_SHIFT_R:		
					shift = 0;
					break;
				case RIGHT_SHIFT_R:		
					shift = 0;
					break;
				case LEFT_ALT_R:		
					alt = 0;
					break;
				case LEFT_CTRL_R:		
					ctrl = 0;
					break;

				case LEFT_SHIFT_P:		
					shift = 1;
					break;
				case RIGHT_SHIFT_P:		
					shift = 1;
					break;
				case LEFT_ALT_P:		
					alt = 1;
					break;
				case LEFT_CTRL_P:		
					ctrl = 1;
					break;

				case CAPSLOCK_P:		
					capslock ^= 1;
					break;

				default:
					break;
				}
			}
		}

        if (temp < 62){
			if (ctrl == 1){		
				if (temp == 0x26){				// scan code for l
					clear_screen();
				}		
				current_char = 0x00;			// do not print it
			}
			else if (alt == 1){
				//do something
				switch (temp)
				{
				case 59:
					switch_terminal(0);
					break;
				case 60:
					switch_terminal(1);
					break;
				case 61:
					switch_terminal(2);
					break;
				default:
					break;
				}
				current_char = 0x00;			// do not print it
			}
			else if (capslock == 1 && shift == 1){
				current_char = scan_code_set_1_shift_cap[(int)temp];
			}
			else if (capslock == 1){
				current_char = scan_code_set_1_cap[(int)temp];
			}
			else if (shift == 1){
				current_char = scan_code_set_1_shift[(int)temp];
			}
			else{
				current_char = scan_code_set_1_norm[(int)temp];
			}
        }

        if (current_char != 0x00){
			if (current_char == ESC){
				// Do something
			}
            else if(current_char == '\b' && keyboard[curr_terminal].top > 0){			//Check for backspace
				delc();
				keyboard[curr_terminal].top--;
			}
			else if (current_char == '\t' && keyboard[curr_terminal].top < 124){		// 123 + 4 = 127 , should not write after this position, leave a space for '\n'
				puts("    ");
				keyboard[curr_terminal].buffer[keyboard[curr_terminal].top] = ' ';
				keyboard[curr_terminal].buffer[keyboard[curr_terminal].top + 1] = ' ';
				keyboard[curr_terminal].buffer[keyboard[curr_terminal].top + 2] = ' ';
				keyboard[curr_terminal].buffer[keyboard[curr_terminal].top + 3] = ' ';
				keyboard[curr_terminal].top += 4;										// 4 space for tab
			}
			else if(current_char != '\b' && keyboard[curr_terminal].top < 128 && current_char != '\t'){//dont add to buffer if it's full
				if (keyboard[curr_terminal].top == 127){								// check when reaches top
					if (current_char == '\n'){
						putc(current_char);
						keyboard[curr_terminal].buffer[keyboard[curr_terminal].top] = current_char;		
						keyboard[curr_terminal].top++;
					}
				}
				else{
					putc(current_char);
					keyboard[curr_terminal].buffer[keyboard[curr_terminal].top] = current_char;		
					keyboard[curr_terminal].top++;
				}
			}
        }
		if (current_char == '\n'){		// locks the keyboard is enter pressed						
			keyboard[curr_terminal].enter_lock = 1;
			//to do
		}
    }
	else if (temp == old_data && holding_count != 0){			// check for holding
		holding_count --;
	}
    old_data = temp;
    //Reset paging right before return
    if(old_page != NULL) {
        update_kernel_vmem(current_pcb->terminal_info.fake_page_addr, VIDEO);
        flush_tlbs();
        switch_cursor(curr_terminal, current_pcb->terminal_info.terminal_num);
        update_cursor();
    }
    send_eoi(1);
}


/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BELOW ARE TERMINAL DRIVER FUNCTIONS:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/*
terminal_open
description: Clears the video memory and prepares the terminal to be written to
inputs: ignored
outputs: success
side effects: Video memory is cleared
*/
int terminal_open(const uint8_t* filename){
	disable_cursor();
	enable_cursor();
	clear_screen();
	return 0;
}
/*
terminal_close
description: "closes" the terminal
inputs: file descriptor (ignored)
outputs: none
side effects: returns success
*/
int terminal_close(int32_t fd){
	return 0;
}
/*
terminal_read
description: Takes in a buffer to read the keyboard buffer into  (keyboard_buf -> input_buf)
inputs: file descriptor (ignored), buffer to be written to, number of bytes in buffer
outputs: number of bytes written
side effects: Clears the keyboard buffer, waits for enter until read goes through
*/
int32_t terminal_read(int32_t fd, void* buffer, int32_t nbytes){
	//SET CURR TERMINAL to ACTIVE PROCESS' TERMINAL, this is a SYS call not the handler
	int curr_terminal = get_pcb()->terminal_info.terminal_num;
	if(curr_terminal < 0){
		return 0;
	}
	int i;
	int count = 0;
	if(nbytes == 0){
		return 0;
	}
	if(buffer == NULL){
		return 0;
	}
	
	//wait for enter key
	while(keyboard[curr_terminal].top == 0 || keyboard[curr_terminal].buffer[keyboard[curr_terminal].top-1] != '\n');
	//fill in buffer, set to 0 if keyboard does not have that many presses
	cli();
	for(i = 0; i<nbytes; i++){
		if(i<keyboard[curr_terminal].top){
			((uint8_t*)buffer)[i] = keyboard[curr_terminal].buffer[i];
			count++;
		}else{
			((uint8_t*)buffer)[i] = 0;
		}
	}
	if(keyboard[curr_terminal].top>=i){//bring down the top to how many were left over, and return this amount
		keyboard[curr_terminal].top = keyboard[curr_terminal].top-i;
	}else{
		keyboard[curr_terminal].top = 0;
	}
	sti();
	keyboard[curr_terminal].enter_lock = 0;
	return count;
	
}
/*
terminal_write
description: Writes a buffer video memory
inputs: buffer to be written, size of buffer
outputs: success on successful writing
side effects: writes to video memory
*/
int32_t terminal_write(int32_t fd, const void* buffer, int32_t nbytes){
	int i;
	if(nbytes == 0){
		return 1;
	}
	if(buffer == NULL){
		return 1;
	}
	
	for(i = 0; i<nbytes; i++){
		if(((const uint8_t *)buffer)[i]!=NULL){
			putc(((const uint8_t*)buffer)[i]);
		}
	}
	return 0;
}

/*
switch_terminal
description: Attempts to switch terminals/create new terminal if there does not exist one
inputs: requested terminal number
outputs: success on successful switch
side effects: allocates new PCB, switches paging as well
*/
int32_t switch_terminal(int8_t requested_terminal_num){
	/* get current terminal onscreen */
	int8_t onscreen_terminal_pid = find_onscreen_terminal_pid();
	
	pcb_t* active_terminal_pcb = (pcb_t*)get_pcb_ptr(onscreen_terminal_pid);

	terminal_t* onscreen_terminal = &(active_terminal_pcb->terminal_info);

	/*
	step through PCBs to determine if a terminal with the requested # exists
	*/
	ATTRIB = (uint8_t)color[(uint8_t)requested_terminal_num];

	int8_t requested_pid = find_terminal_pid(requested_terminal_num);
	if(requested_pid < 0){
		char* command = "shell";
		//BELOW IS PRETTY MUCH EXECUTE
		int32_t start_of_prog = check_executable((uint8_t *)command);
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
		
		//after switching terminals, old terminal shouldn't be displayed on screen
		parent->terminal_info.is_onscreen = 0;
		//set up vmem paging stuff so scheduler can switch correctly
		pcb_self->terminal_info.is_onscreen = 1;
		pcb_self->terminal_info.terminal_num = requested_terminal_num;
		pcb_self->terminal_info.user_page_addr = 0;
		pcb_self->terminal_info.fake_page_addr =  VIDEO + EIGHT_KB*(requested_terminal_num+1)/2;
		
		//set this new process as active unconditionally, since it is a new terminal
		pcb_self->is_active = 1;
		setup_pcb(pcb_self, avail_process, parent);

		terminal_t* requested_terminal = &(pcb_self->terminal_info);
		/*
		switch vmem:
		*/
        if (onscreen_terminal->terminal_num != requested_terminal->terminal_num) {
            switch_kernel_memory(onscreen_terminal->fake_page_addr, requested_terminal->fake_page_addr);
        }
		onscreen_terminal->is_onscreen = 0;
		requested_terminal->is_onscreen = 1;
		
		
		// Setup TSS
		tss.esp0 = EIGHT_MB - (EIGHT_KB*avail_process)-4;
		tss.ss0 = KERNEL_DS;
		flush_tlbs();
		save_regs((uint32_t)&(pcb_self->schedule_ebp),(uint32_t)&(pcb_self->schedule_esp));
		// Setup stack to return to new program
		send_eoi(1);
		clear_screen();
		setup_exec_stack(eip,(uint32_t)&(pcb_self->halt_ebp));
		return 0;
	}
	
	
	pcb_t* requested_pcb = (pcb_t*)get_pcb_ptr(requested_pid);
	terminal_t* requested_terminal = &(requested_pcb->terminal_info);
	/*
	switch vmem:
	*/
    if (onscreen_terminal->terminal_num != requested_terminal->terminal_num) {
        switch_kernel_memory(onscreen_terminal->fake_page_addr, requested_terminal->fake_page_addr);
    }
    onscreen_terminal->is_onscreen = 0;
	requested_terminal->is_onscreen = 1;
	return 1;
}

void switch_cursor(uint8_t curr_terminal, uint8_t tar_terminal){
	cursor[curr_terminal].screen_x = screen_x;
	cursor[curr_terminal].screen_y = screen_y;

	screen_x = cursor[tar_terminal].screen_x;
	screen_y = cursor[tar_terminal].screen_y;

	return;
}
