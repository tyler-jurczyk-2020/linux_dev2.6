.globl get_pcb
get_pcb:
	#setup stack
	pushl %EBP
	movl %ESP,%EBP
	#c calling convention
	pushl %EBX
	pushl %ESI
	pushl %EDI
	#and stack ptr with magic round down number
	movl %ESP, %EAX
	andl $0xFFFFE000, %EAX
	#pop things
	popl %EBX
	popl %ESI
	popl %EDI
	#teardown and return
	leave
	ret

