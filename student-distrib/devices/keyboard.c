#include "keyboard.h"
#include "PS2.h"
#include "../lib.h"
#include "i8259.h"
#include "../x86_desc.h"

/*This is the tabled used to decode scan code from keyboard*/
static char scan_code_set_1_norm[59] = {	// 59 characters for what we care
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
	0x00,  ' ', 0x00
}; 

static char scan_code_set_1_cap[59] = {
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
	0x00,  ' ', 0x00
}; 

static char scan_code_set_1_shift[59] = {
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
	0x00,  ' ', 0x00
}; 

static char scan_code_set_1_shift_cap[59] = {
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
	0x00,  ' ', 0x00
}; 

keyboard_struct keyboard;

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
	int i;
	for(i = 0; i<128; i++){			// size = 128
		keyboard.buffer[i] = 0;
		keyboard.out_buffer[i] = 0;
	}
	keyboard.top = 0;
	keyboard.out_top = 0;
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
side effect: print th typed key on screen
*/
void handle_keyboard(){
    uint8_t temp;
    uint8_t current_char = 0x00;
	int i;
    temp = inb(DATA_PORT);
    if (temp != old_data || holding_count == 0){
		
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

        if (temp < 59){
			if (ctrl == 1){		
				if (temp == 0x26){				// scan code for l
					clear();
					screen_set_xy(0,0);
					update_cursor_pos(0,0);
				}		
				current_char = 0x00;			// do not print it
			}
			else if (alt == 1){
				//do something
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
            else if(current_char == '\b' && keyboard.top > 0){			//Check for backspace
				delc();
				keyboard.top--;
			}
			else if (current_char == '\t' && keyboard.top < 124){		// 123 + 4 = 127 , should not write after this position, leave a space for '\n'
				puts("    ");
				keyboard.buffer[keyboard.top] = ' ';
				keyboard.buffer[keyboard.top + 1] = ' ';
				keyboard.buffer[keyboard.top + 2] = ' ';
				keyboard.buffer[keyboard.top + 3] = ' ';
				keyboard.top += 4;										// 4 space for tab
			}
			else if(current_char != '\b' && keyboard.top < 128 && current_char != '\t'){//dont add to buffer if it's full
				if (keyboard.top == 127){								// check when reaches top
					if (current_char == '\n'){
						putc(current_char);
						keyboard.buffer[keyboard.top] = current_char;		
						keyboard.top++;
					}
				}
				else{
					putc(current_char);
					keyboard.buffer[keyboard.top] = current_char;		
					keyboard.top++;
				}
			}
        }
		if (current_char == '\n'){
			for (i = 0; i < keyboard.top; i++){
				keyboard.out_buffer[i] = keyboard.buffer[i];
			}
			keyboard.out_top = keyboard.top;
			keyboard.top = 0;
		}
    }
	else if (temp == old_data && holding_count != 0){
		holding_count --;
		//printf(" get here ");
	}
    old_data = temp;
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
	update_cursor_pos(0,0);
	clear();
	screen_set_xy(0,0);
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
int terminal_read(uint32_t fd, uint8_t* buffer, uint32_t nbytes){
	int i;
	if(nbytes == 0){
		return 0;
	}
	if(buffer == NULL){
		return 0;
	}
	//wait for enter key
	while(keyboard.out_top == 0 || keyboard.out_buffer[keyboard.out_top-1] != '\n');
	//fill in buffer, set to 0 if keyboard does not have that many presses
	cli();
	for(i = 0; i<nbytes; i++){
		if(i<keyboard.out_top){
			buffer[i] = keyboard.out_buffer[i];
		}else{
			buffer[i] = 0;
		}
	}
	if(keyboard.out_top>=i){//bring down the top to how many were left over, and return this amount
		keyboard.out_top = keyboard.out_top-i;
	}else{
		keyboard.out_top = 0;
	}
	sti();
	return i;
	
}
/*
terminal_write
description: Writes a buffer video memory
inputs: buffer to be written, size of buffer
outputs: success on successful writing
side effects: writes to video memory
*/
int terminal_write(int32_t fd, const uint8_t* buffer, uint32_t nbytes){
	int i;
	if(nbytes == 0){
		return 1;
	}
	if(buffer == NULL){
		return 1;
	}
	
	for(i = 0; i<nbytes; i++){
		if(buffer[i]!=NULL){
			putc(buffer[i]);
		}
	}
	return 0;
}
