#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "lib.h"

#define ENABLE_SCANNIG      0xF4
#define SUCCESS             0xFA
#define SET_TYPEMATIC       0xF3
#define TYPEMATIC_DATA      0x0F
#define IRQ1                0x01
extern void keyboard_init();

extern void handle_keyboard();

#endif
