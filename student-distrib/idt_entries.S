#define ASM     1
# vim:ts=4 noexpandtab

#Below is a macro used to link the C exception handler to ASM, pushes arguments and flags/regs to stack as well as clears interrupts
#define isr_exception(vector)     \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		cli                      ;\
		pushal  				 ;\
		pushfl					 ;\
		pushl $##vector          ;\
		call exception_handler   ;\
		addl $4, %ESP		     ;\
		popfl 					 ;\
		popal					 ;\
		sti                      ;\
		iret		

#Below is a macro used to link the C syscall handler to ASM, pushes arguments and flags/regs to stack
#define isr_system_call(vector)   \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		pushal  				 ;\
		pushfl					 ;\
		pushl %eax		         ;\
		call system_call_handler ;\
		addl $4, %ESP		     ;\
		popfl 					 ;\
		popal					 ;\
		iret			
		
#Below is a macro used to link the C rtc handler to ASM, pushes args and flags/regs to stack
#define isr_rtc(vector)   \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		pushal  				 ;\
		pushfl					 ;\
		call test_interrupts 	 ;\
		popfl 					 ;\
		popal					 ;\
		iret			


#TODO REPLACE WITH KEYBOARD HANDLER
#Below is a macro used to link the C keyboard handler to ASM, pushes args and flags/regs to stack
#define isr_keyboard(vector)   \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		pushal  				 ;\
		pushfl					 ;\
		call exception_handler 	 ;\
		popfl 					 ;\
		popal					 ;\
		iret			
		
		

#These below use the macros above to make functions that call their respective handler
#They are then placed in the stub table below according to their vector# index to be used by handlers.c to populate the idt
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
isr_keyboard(33) #0x21, irq1 on the pic
isr_rtc(40) #0x28, irq8 on the pic
isr_system_call(128)











.globl isr_stub_table
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
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long isr_stub_33
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long isr_stub_40
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long isr_stub_128
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0
	.long 0

	