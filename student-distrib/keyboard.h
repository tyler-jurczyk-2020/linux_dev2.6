#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "lib.h"

#define ENABLE_SCANNIG      0xF4
#define SUCCESS             0xFA
#define SET_TYPEMATIC       0xF3
#define TYPEMATIC_DATA      0x0F
#define IRQ1                0x01
/* This function initialize the keyboard*/
extern void keyboard_init();
/*This is the keyboard handler*/
extern void handle_keyboard();

#endif
