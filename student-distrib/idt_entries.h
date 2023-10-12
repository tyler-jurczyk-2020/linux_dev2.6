#ifndef _IDT_ENTRIES_H
#define _IDT_ENTRIES_H


#include "types.h"

#ifndef ASM

extern void exception_linkage(void);
extern uint32_t isr_stub_table[20];

#endif  //ASM
#endif  //IDT_ENTRIES_H

