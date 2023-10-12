#define ASM     1
# vim:ts=4 noexpandtab
#include "idt_entries.h"

.globl isr_stub_table



#define INTR_LINK(name,func)    \
.globl name       			   ;\
	name:				       ;\
		pushal				   ;\
		pushfl				   ;\
		call func			   ;\
		popfl 				   ;\
		popal				   ;\
		iret				  


INTR_LINK(exception_linkage,exception_handler)

#define isr_exception(vector)   \
.globl isr_stub_##vector         ;\
	isr_stub_##vector:           ;\
		pushl $##vector          ;\
		call exception_linkage ;\
		addl $4, %ESP		   ;\
		iret				  


isr_exception(0)
isr_exception(1)
isr_exception(2)
isr_exception(3)
isr_exception(4)
isr_exception(5)
isr_exception(6)
isr_exception(7)
isr_exception(8)
isr_exception(9)
isr_exception(10)
isr_exception(11)
isr_exception(12)
isr_exception(13)
isr_exception(14)
isr_exception(15)
isr_exception(16)
isr_exception(17)
isr_exception(18)
isr_exception(19)


isr_stub_table:
	.long isr_stub_0
	.long isr_stub_1
	.long isr_stub_2
	.long isr_stub_3
	.long isr_stub_4
	.long isr_stub_5
	.long isr_stub_6
	.long isr_stub_7
	.long isr_stub_8
	.long isr_stub_9
	.long isr_stub_10
	.long isr_stub_11
	.long isr_stub_12
	.long isr_stub_13
	.long isr_stub_14
	.long isr_stub_15
	.long isr_stub_16
	.long isr_stub_17
	.long isr_stub_18
	.long isr_stub_19
	

