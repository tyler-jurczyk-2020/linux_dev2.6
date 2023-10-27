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

.globl setup_exec_stack
setup_exec_stack:
    cli
    pushl %ebp
    movl %esp, %ebp
    pushl $0x002B // User Data Segment
    pushl $0x08400000 // Program Stack Pointer
    pushfl // EFLAGS
    popl %eax
    orl $0x0200, %eax
    pushl %eax
    pushl $0x0023 // User Code Segment
    pushl 8(%ebp) // EIP
    iret
.globl halt_process
halt_process:
    leave
    pushl %ebp
    movl %esp, %ebp
    movb 8(%ebp), %al
    movzx %al, %eax
    leave
    ret

