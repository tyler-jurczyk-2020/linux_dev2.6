#ifndef HANDLERS_H_
#define HANDLERS_H_

#include "../types.h"

/*Custom struct to hold the results of pushal*/
typedef struct  __attribute__((packed)) register_struct  {
    uint32_t EDI;
	uint32_t ESI;
	uint32_t EBP;
	uint32_t ESP;
	uint32_t EBX;
	uint32_t EDX;
	uint32_t ECX;
	uint32_t EAX;
    
}register_struct;
/*Custom struct to give names to a given vector, the exact implementation is in handlers.c*/
typedef struct interrupt_identifier{
	uint8_t index;
	const char *name;
} interrupt_identifier;

//handler for all exceptions as of now
void exception_handler(unsigned long vector, unsigned long flags, register_struct regs);
//handler for all system calls as of now
void system_call_handler(unsigned long vector, unsigned long flags, register_struct regs);
//function to fill up the IDT
void populate_idt();

#endif
