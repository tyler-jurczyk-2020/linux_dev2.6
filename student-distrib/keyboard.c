#include "keyboard.h"
#include "PS2.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"

/*This is the tabled used to decode scan code from keyboard*/
static char scan_code_set_1[51] = {
    0x00, 0x00, '1',  '2',
    '3',  '4',  '5',  '6', 
    '7',  '8',  '9',  '0',
    0x00, 0x00, 0x00, 0x00,
    'q',  'w',  'e',  'r',
    't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',
    0x00, 0x00,  'a',  's',
    'd',  'f',  'g',  'h',
    'j',  'k',  'l',  ';',
    0x00, 0x00, 0x00, 0x00,
    'z',  'x',  'c',  'v',
    'b',  'n',  'm'
}; 

//volatile uint8_t keyboard_response;
volatile uint8_t old_data = 0x00;

/*
Init keyboard
no input
no output
*/
void keyboard_init(){
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
    //printf("get to keyboard handler");
    temp = inb(DATA_PORT);
    if (temp != old_data){
        if (temp < 51){
            current_char = scan_code_set_1[(int)temp];
        }
        if (current_char != 0x00){
            printf("%c", current_char);
        }
    }
    old_data = temp;
    send_eoi(1);
    //sti();
}
