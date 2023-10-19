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

#Below is a macro used to link the C syscall handlers to ASM, pushes arguments and flags/regs to stack
#Makes use of a jump table defined at the end of this file
#First saves flags/regs, then checks if syscall num is valid, returns -1 if not
# uses jump table to call handler, then pops args off
#define isr_system_call(vector)   	 \
.globl isr_stub_##vector         	 ;\
.align 4						 	 ;\
	isr_stub_##vector:           	 ;\
		pushal  				  	 ;\
		pushfl					  	 ;\
		cmpl $0, %EAX			 	 ;\
		jle isr_invalid_##vector 	 ;\
		cmpl $10, %EAX			  	 ;\
		ja isr_invalid_##vector  	 ;\
		pushl %EDX		         	 ;\
		pushl %ECX				 	 ;\
		pushl %EBX				 	 ;\
		call *syscall_table(,%EAX,4) ;\
		popl %EBX			     	 ;\
		popl %ECX			     	 ;\
		popl %EDX			     	 ;\
		jmp isr_leave_##vector 	 	 ;\
	isr_invalid_##vector: 	  	 	 ;\
		movl $-1, %EAX    	 	 	 ;\
	isr_leave_##vector:			 	 ;\
		popfl 					 	 ;\
		popal					 	 ;\
		iret			
		
#Below is a macro used to link the C rtc handler to ASM, pushes args and flags/regs to stack
#define isr_rtc(vector)   \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		pushal  				 ;\
		pushfl					 ;\
		call rtc_interrupt	 	 ;\
		popfl 					 ;\
		popal					 ;\
		iret			


#Below is a macro used to link the C keyboard handler to ASM, pushes args and flags/regs to stack
#define isr_keyboard(vector)   \
.globl isr_stub_##vector         ;\
.align 4						 ;\
	isr_stub_##vector:           ;\
		cli                      ;\
		pushal  				 ;\
		pushfl					 ;\
		call handle_keyboard 	 ;\
		popfl 					 ;\
		popal					 ;\
		sti                      ;\
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

.globl syscall_table
syscall_table:
	.long 0
	.long halt
	.long execute
	.long read
	.long write
	.long open
	.long close
	.long getargs
	.long vidmap
	.long set_handler
	.long sigreturn