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
//handler for all system calls (unused)
void system_call_handler(unsigned long vector, unsigned long flags, register_struct regs);
//function to fill up the IDT
void populate_idt();

extern void setup_exec_stack(uint32_t,uint32_t);
extern uint8_t halt_process(uint32_t,uint32_t);
extern void flush_tlbs();

/*
**********************************
Below are syscall function headers
**********************************
*/
uint32_t halt(uint8_t status);
uint32_t execute(const uint8_t* command);
uint32_t read(uint32_t fd, void* buf, uint32_t nbytes);
uint32_t write(uint32_t fd, const void* buf, uint32_t nbytes);
uint32_t open(const uint8_t* filename);
uint32_t close(uint32_t fd);
uint32_t getargs(uint8_t* buf, uint32_t nbytes);
uint32_t vidmap(uint8_t** screen_start);
uint32_t set_handler(uint32_t signum, void* handler_address);
uint32_t sigreturn(void);
#endif
