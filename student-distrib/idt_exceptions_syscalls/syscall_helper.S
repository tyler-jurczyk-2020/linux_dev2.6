
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
	
	
.globl stdin_table
stdin_table:
	.long dummy_open
	.long dummy_close
	.long terminal_read
	.long dummy_write
	
.globl stdout_table
stdout_table:
	.long dummy_open
	.long dummy_close
	.long dummy_read
	.long terminal_write
	
.globl file_table:
file_table:
	.long file_open
	.long file_close
	.long file_read
	.long file_write
	
.globl directory_table:
directory_table:
	.long dir_open
	.long dir_close
	.long dir_read
	.long dir_write
	
.globl rtc_table:
rtc_table:
	.long rtc_open
	.long rtc_close
	.long rtc_read
	.long rtc_write