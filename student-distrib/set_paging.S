# Following divides VIDEO in lib.c by 4096
#define VIDEO_ENT 184

.text
.globl enable_paging
  
enable_paging:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    call setup_pager_directory
    xor %eax, %eax
    # Allow mixed-size paging 
    movl $0x00000010, %eax
    movl %eax, %cr4    
    # Set paging directory
    mov $page_dir, %eax
    movl %eax, %cr3
    # Enable paging
    movl $0x80000000, %eax
    movl %eax, %cr0 
    popl %eax
    leave
    ret
