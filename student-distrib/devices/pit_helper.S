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
	
	
.globl do_schedule
do_schedule:

	movl 4(%ESP),%EBP
	movl 8(%ESP),%ESP
	
	leave
	ret

