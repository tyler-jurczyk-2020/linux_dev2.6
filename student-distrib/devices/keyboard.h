#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "../lib.h"

#define ENABLE_SCANNIG      0xF4
#define SUCCESS             0xFA
#define SET_TYPEMATIC       0xF3
#define TYPEMATIC_DATA      0x0F
#define IRQ1                0x01

#define ESC                 0xFF

#define DOUBLE_CODE         0xE0

#define RIGHT_ALT_P         0x38
#define RIGHT_CTRL_P        0x1D

#define RIGHT_ALT_R         0xB8
#define RIGHT_CTRL_R        0x9D

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
#define HOLDING_INIT        3


/* keyboard struct, feel free to add flags/anything else necessary.*/
typedef struct keyboard_struct  {
    uint8_t buffer[128]; //holds chars that have been typed
	uint8_t top; //holds index of top element
    uint8_t enter_lock;
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

/*open terminal*/
extern int terminal_open(const uint8_t* filename);
/*do nothing*/
extern int terminal_close(int32_t fd);
/*read from keyboard buffer*/
extern int32_t terminal_read(int32_t fd, void* buffer, int32_t nbytes);
/*puts what is read on to terminal*/
extern int32_t terminal_write(int32_t fd, const void* buffer, int32_t nbytes);

extern int dummy_read();

extern int dummy_open();

extern int dummy_write();

extern int dummy_close();

extern void setup_exec_stack(uint32_t,uint32_t);
extern uint8_t halt_process(uint32_t,uint32_t);
extern void flush_tlbs();

#endif
