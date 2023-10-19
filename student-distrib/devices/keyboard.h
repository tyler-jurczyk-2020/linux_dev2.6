#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "../lib.h"

#define ENABLE_SCANNIG      0xF4
#define SUCCESS             0xFA
#define SET_TYPEMATIC       0xF3
#define TYPEMATIC_DATA      0x0F
#define IRQ1                0x01

#define ESC                 0xFF

#define LEFT_SHIFT_R        0xAA
#define RIGHT_SHIFT_R       0xB6
#define LEFT_ALT_R          0XB8
#define LEFT_CTRL_R         0x9D

#define LEFT_SHIFT_P        0x2A
#define RIGHT_SHIFT_P       0x36
#define LEFT_ALT_P          0x38
#define LEFT_CTRL_P         0x1D
#define CAPSLOCK_P          0x3A

#define QUOTE               39
#define BACK_SLASH          92


/* keyboard struct, feel free to add flags/anything else necessary.*/
typedef struct keyboard_struct  {
    uint8_t buffer[128]; //holds chars that have been typed
	uint8_t top; //holds index of top element
}keyboard_struct;

/* This function initialize the keyboard*/
extern void keyboard_init();
/*This is the keyboard handler*/
extern void handle_keyboard();


/*
Below are the terminal driver functions, these include open, close, read, and write

Open clears the screen and prepares the terminal to be written to, returns success
Close returns success (doesn't do anything)
Read will read from the keyboard buffer into an inputted buffer
Write will write an inputted buffer onto the screen
*/

extern int terminal_open(const uint8_t* filename);
extern int terminal_close(int32_t fd);
extern int terminal_read(uint32_t fd, uint8_t* buffer, uint32_t nbytes);
extern int terminal_write(int32_t fd, const uint8_t* buffer, uint32_t nbytes);

#endif