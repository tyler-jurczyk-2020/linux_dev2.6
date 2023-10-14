.text
.globl enable_paging
  
enable_paging:
    pushl %ebp
    movl %esp, %ebp
    call setup_pager_directory
    xor %eax, %eax
    # Allow mixed-size paging 
    movl %cr4, %eax
    or $0x00000010, %eax
    movl %eax, %cr4    
    # Set paging directory
    movl $page_dir, %eax
    pushl %eax
    call mod_cr3
    addl $4, %esp 
    # Enable paging
    movl %cr0, %eax
    or $0x80000000, %eax
    movl %eax, %cr0 
    leave
    ret

mod_cr3:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl %eax, %cr3
    leave
    ret
