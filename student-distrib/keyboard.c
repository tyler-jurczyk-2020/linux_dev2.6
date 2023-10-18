#include "keyboard.h"
#include "PS2.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"

/*This is the tabled used to decode scan code from keyboard*/
static char scan_code_set_1_norm[59] = {
    0x00,  ESC, '1',  '2',
    '3',  '4',  '5',  '6', 
    '7',  '8',  '9',  '0',
    '-',  '=', '\b',  TAB,
    'q',  'w',  'e',  'r',
    't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',
    '\n', CTRL,  'a',  's',
    'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ';',
    QUOTE, '`', SHIFT, BACK_SLASH,
    'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',
	'.',  '/', SHIFT, 0x00,
	ALT,  ' ', CAPSLOCK
}; 

static char scan_code_set_1_cap[59] = {
    0x00,  ESC, '1',  '2',
    '3',  '4',  '5',  '6', 
    '7',  '8',  '9',  '0',
    '-',  '=', '\b',  TAB,
    'Q',  'W',  'E',  'R',
    'T',  'Y',  'U',  'I',
    'O',  'P',  '[',  ']',
    '\n', CTRL,  'A',  'S',
    'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ';',
    QUOTE, '`', SHIFT, BACK_SLASH,
    'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  ',',
	'.',  '/', SHIFT, 0x00,
	ALT,  ' ', CAPSLOCK
}; 

static char scan_code_set_1_shift[59] = {
    0x00,  ESC, '!',  '@',
    '#',  '$',  '%',  '^', 
    '&',  '*',  '(',  ')',
    '_',  '+', '\b',  TAB,
    'Q',  'W',  'E',  'R',
    'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',
    '\n', CTRL,  'A',  'S',
    'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ':',
    '"', '~', SHIFT,  '|',
    'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',
	'>',  '?', SHIFT, 0x00,
	ALT,  ' ', CAPSLOCK
}; 

keyboard_struct keyboard;

//volatile uint8_t keyboard_response;
volatile uint8_t old_data = 0x00;
static int shift;
static int capslock;
static int alt;
static int ctrl;

/*
Init keyboard
no input
no output
*/
void keyboard_init(){
	int i;
	for(i = 0; i<128; i++){
		keyboard.buffer[i] = 0;
	}
	keyboard.top = 0;
	shift = 0;
	capslock = 0;
	alt = 0;
	ctrl = 0;
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
    char current_char = 0x00;
    //cli();
    temp = inb(DATA_PORT);
    if (temp != old_data){
        if (temp < 59){
			if (capslock == 1){
				current_char = scan_code_set_1_cap[(int)temp];
			}
			else if (shift == 1){
				current_char = scan_code_set_1_shift[(int)temp];
			}
			else{
				current_char = scan_code_set_1_norm[(int)temp];
			}
        }

		switch (current_char)		//check for function keys (esc and tab not included)
		{
		case SHIFT:
			shift ^= 1;
			break;
		case ALT:
			alt ^= 1;
			break;
		case CAPSLOCK:
			capslock ^= 1;
			break;
		case CTRL:
			ctrl ^= 1;
			break;
		default:
			break;
		}

        if (current_char != 0x00 && current_char != SHIFT && current_char != ALT 
			&& current_char != CAPSLOCK && current_char != CTRL){
			if (current_char == ESC){
				// Do something
			}
			else if(current_char == TAB){
				// Do something
			}
            else if(current_char == '\b' && keyboard.top > 0){//Check for backspace
				delc();
				keyboard.top--;
			}else if(keyboard.top < 128){//dont add to buffer if it's full
				putc(current_char);
				keyboard.buffer[keyboard.top] = current_char;		
				keyboard.top++;
			}
			
        }
    }
    old_data = temp;
    send_eoi(1);
    //sti();
}


/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BELOW ARE TERMINAL DRIVER FUNCTIONS:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/*
terminal_open
description: Clears the video memory and prepares the terminal to be written to
inputs: none
outputs: success
side effects: Video memory is cleared
*/
int terminal_open(){
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
	while(keyboard.top == 0 || keyboard.buffer[keyboard.top-1] != '\n');
	//fill in buffer, set to 0 if keyboard does not have that many presses
	cli();
	for(i = 0; i<nbytes; i++){
		if(i<keyboard.top){
			buffer[i] = keyboard.buffer[i];
		}else{
			buffer[i] = 0;
		}
	}
	if(keyboard.top>=i){//bring down the top to how many were left over, and return this amount
		keyboard.top = keyboard.top-i;
	}else{
		keyboard.top = 0;
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
