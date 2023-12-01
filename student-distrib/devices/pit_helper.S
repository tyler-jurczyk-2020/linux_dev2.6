
# inputs: pcb->ebp pcb->esp pointers
# outputs: none
# effects: stores the current ebp and esp to the pcb of the process that is losing control
.globl save_regs
save_regs:
	movl %EBP, %EAX
	#setup stack
	pushl %EBP
	movl %ESP,%EBP
	#c calling convention
	pushl %EBX
	pushl %ESI
	pushl %EDI
	
	#put ebp into current pcb
	movl 8(%EBP),%EDI
	movl %EAX, (%EDI)
	#put esp into current pcb
	movl 12(%EBP),%EDI
	movl %ESP, (%EDI)
	
	#c calling convention
	popl %EBX
	popl %ESI
	popl %EDI
	#teardown and return
	leave
	ret
	
#inputs : ebp and esp of the process that are regaining control
#outputs: none
#side effects: returns to pithandler->linkage->iret to user program that is regaining control
.globl do_schedule
do_schedule:

	movl 4(%ESP),%EBP
	movl 8(%ESP),%ESP
	
	leave
	ret

