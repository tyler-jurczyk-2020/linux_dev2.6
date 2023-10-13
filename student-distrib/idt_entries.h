#ifndef _IDT_ENTRIES_H
#define _IDT_ENTRIES_H


#include "types.h"

#ifndef ASM
//This table below contains function pointers to their appropriate linker
//The linkers call a corresponding exception, system call, etc handler in handlers.c
//handlers.c also sets up the IDT table using the indexes in this
extern uint32_t isr_stub_table[256];
#endif  //ASM
#endif  //IDT_ENTRIES_H

