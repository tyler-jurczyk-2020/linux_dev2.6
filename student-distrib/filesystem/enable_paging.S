.text
.globl enable_paging
.globl mod_cr3
.globl get_cr3

 /* void enable_paging();
 * Inputs: none 
 * Return Value: none
 *  Function: Modify control registers to enable paging with mixed sizes */
 
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

 /* void mod_cr3(uint32_t *);
 * Inputs: pointer to a page directory 
 * Return Value: none
 *  Function: Set the page directory to be used in control register 3 */

mod_cr3:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl %eax, %cr3
    leave
    ret

/* void get_cr3(uint32_t *);
 * Inputs: none 
 * Return Value: pointer to current page directory 
 *  Function: Retrieve a pointer to the page directory stored in control register 3 */

get_cr3:
    pushl %ebp
    movl %esp, %ebp
    movl %cr3, %eax
    leave
    ret
