#
# get_pcb: Gets the PCB* associated with this kernel stack
# Inputs: none
# Outputs: PCB* (by and-ing the stack ptr)
# Side Effects: EAX is set to PCB*
#
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

# setup_exec_stack: Prepares and returns from kernel space to newly executing user space program
# Inputs: EIP of the new process, PCB of new process
# Outputs: return value from halt system call
# Side Effects: jumps to user space

.globl setup_exec_stack
setup_exec_stack:
	cli
	pushl %EBP
	movl %ESP, %EBP
	pushl %EDI
	#move base pointer to struct to jump back
	movl 12(%EBP),%EDI
	movl %EBP,(%EDI)
	popl %EDI
	
	#change data segment register
	pushw $0x002B
	popw %DS
	#User data segment
    pushl $0x002B 
	#program stack pointer
    pushl $0x083ffffc
	#eflags
    pushfl
    popl %eax
    orl $0x0200, %eax
    pushl %eax
	#user code segment
    pushl $0x0023 
	#eip
    pushl 8(%ebp)
    iret
.globl halt_process
halt_process:
	#set return val
	andl $0x000000FF, %EAX;
    movb 4(%esp), %al
	movsx %al,%eax
	#reset base ptr
	movl 8(%esp),%ebp
	leave
    ret


/*
 * flush_tlbs: flushes tlbs
 * Inputs : none
 * Outputs: none
 * Side Effects: flushes tlbs (wow!)
 */
.globl flush_tlbs
flush_tlbs:
	pushl %EBP
	movl %ESP, %EBP
	#flush tlbs
	movl	%cr3,%eax
	movl	%eax,%cr3
	leave	
	ret
	


