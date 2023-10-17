#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "lib.h"

#define ENABLE_SCANNIG      0xF4
#define SUCCESS             0xFA
#define SET_TYPEMATIC       0xF3
#define TYPEMATIC_DATA      0x0F
#define IRQ1                0x01


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

extern int terminal_open();
extern int terminal_close(int32_t fd);
extern int terminal_read(uint32_t fd, uint8_t* buffer, uint32_t nbytes);
extern int terminal_write(int32_t fd, const uint8_t* buffer, uint32_t nbytes);

#endif
