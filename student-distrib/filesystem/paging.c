#include "../types.h"
#include "paging.h"
#include "../lib.h"
#include "../idt_exceptions_syscalls/idt_exceptions_syscalls.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define LOCATION_OF_THE_KRABBY_PATTY_SECRET_FORMULA 0x3FF000


page_directory_entry_t page_dir[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

page_table_entry_t page_tbl[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

page_table_entry_t page_tbl_vmem[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

/* void setup_pager_table();
 * Inputs: none 
 * Return Value: none
 *  Function: Setup the page table in the 0-4MB range for video memory,
 *  which itself only uses 0xB8000-0xB9000 */

void setup_pager_table() {
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == PG_TBL_IDX) {
            page_tbl[i].entry =  VIDEO_PAGE_FLAGS;
        } 
        else if (i == PG_TBL_IDX + 1) {
            page_tbl[i].entry = VIDEO_PAGE_FLAGS + FOUR_KB; 
        }
        else if (i == PG_TBL_IDX + 2) {
            page_tbl[i].entry = VIDEO_PAGE_FLAGS + 2*FOUR_KB;
        }
        else if(i == PG_TBL_IDX + 3) {
            page_tbl[i].entry = VIDEO_PAGE_FLAGS + 3*FOUR_KB;
        }
        else if(i == TABLE_SZ - 1) {
            page_tbl[i].entry = VIDEO_PAGE_FLAGS;
        }
        else {
            page_tbl[i].entry = EMPTY_PAGE;
        }
    }   
}

/* void setup_pager_directory();
 * Inputs: none 
 * Return Value: none
 *  Function: Setup the kernel page in the 4-8MB range */

void setup_pager_directory() {
    setup_pager_table();
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == 0) {
            page_dir[i].kb.entry = ((uint32_t) page_tbl) | 3; // Sets the present and r/w priv
        }
        else if (i == 1) {
            page_dir[i].mb.entry = KERNEL_PAGE_FLAGS;
        }
        else {
            page_dir[i].empty = EMPTY_PAGE;
        }
    }
}

/* void set_pager_dir_entry();
 * Inputs: address to physical memory to map 128mb address to
 * Return Value: none
 *  Function: Setup the program page to point to the specified physical address */

void set_pager_dir_entry(uint32_t page_addr) {
    page_directory_entry_t* cur_page_dir = get_cr3();  
    page_directory_entry_mb_t* entry = &cur_page_dir[PROGRAM_IDX].mb;
    entry->present = 1;
    entry->r_w = 1;
    entry->usr_supr = 1;
    entry->write_thru = 0;
    entry->disable_cache = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->page_size = 1;
    entry->global = 0;
    entry->avail = 0;
    entry->tbl_attr_idx = 0;
    entry->reserved = 0;
    entry->base_addr = page_addr >> 22;
}

/* void setup_pager_vidmap_entry();
 * Inputs: virtual address, and physical address to map the virtual to
 * Return Value: none
 * Function: Sets an entry the in vidmap page table based on the full virtual address
 * to map to, as well as the kernel page that the userspace page should point to.
 */
void setup_pager_vidmap_entry(uint32_t vmem_addr, uint32_t kernel_page) {
    page_table_entry_t* entry = &page_tbl_vmem[(vmem_addr >> 12) & 0x3FF];
    entry->present = 1;
    entry->r_w = 1;
    entry->usr_supr = 1;
    entry->write_thru = 0;
    entry->disable_cache = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->tbl_attr_idx = 0;
    entry->global = 0;
    entry->avail = 0;
    entry->base_addr = kernel_page >> 12;
}

/* void setup_pager_vidmap_table();
 * Inputs: virtual address of the single userspace page for accessing vmem
 * Return Value: none
 * Function: Sets up the entire page table associated with vidmap, where only
 * one entry from the table is set to point to the appropriate kernel page
 */
void setup_pager_vidmap_table(uint32_t vmem_addr) {
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == ((vmem_addr >> 12) & 0x3FF)) {
            setup_pager_vidmap_entry(vmem_addr, KERNEL_VMEM); // Map to real terminal 0
        } 
        else {
            page_tbl_vmem[i].entry = EMPTY_PAGE;
        }
    }   
    page_directory_entry_t* cur_page_dir = get_cr3();  
    cur_page_dir[vmem_addr >> 22].kb.entry = ((uint32_t)page_tbl_vmem | 7);
}

/* void update_kernel_vmem();
 * Inputs: virtual address, and physical address that the virtual address points to
 * Return Value: none
 * Function: Updates a 4kb video memory entry in the 0-4mb page table to point
 * to the given physical address
 */
void update_kernel_vmem(uint32_t physical, uint32_t virtual) {
    page_table_entry_t* entry = &page_tbl[(virtual >> 12) & 0x3FF];
    entry->present = 1;
    entry->r_w = 1;
    entry->usr_supr = 0;
    entry->write_thru = 0;
    entry->disable_cache = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->tbl_attr_idx = 0;
    entry->global = 0;
    entry->avail = 0;
    entry->base_addr = physical >> 12;
}

/* void update_vidmap_vmem();
 * Inputs: virtual address, and physical address that the virtual address points to
 * Return Value: none
 * Function: Changes the vidmap virtual address to point to the given physical address
 * so that the kernel and userspace vmems point to the same physical page always
 */
void update_vidmap_vmem(uint32_t physical, uint32_t virtual) {
    page_table_entry_t* entry = &page_tbl_vmem[(virtual >> 12) & 0x3FF];
    entry->present = 1;
    entry->r_w = 1;
    entry->usr_supr = 1;
    entry->write_thru = 0;
    entry->disable_cache = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->tbl_attr_idx = 0;
    entry->global = 0;
    entry->avail = 0;
    entry->base_addr = physical >> 12;
}

/* void switch_kernel_memory();
 * Inputs: Fake addresses associated with both on_screen terminal and off_screen terminal
 * Return Value: none
 * Function: Copies the on_screen to its fake page, and copies the off screen's fake page
 * to on_screen vmem. Meant to be used during process switch invoked by scheduler
 */
void switch_kernel_memory(uint32_t on_screen, uint32_t off_screen) { // Note: These should be both the fake addresses
    memcpy((void *) on_screen, (void *) LOCATION_OF_THE_KRABBY_PATTY_SECRET_FORMULA, NUM_ROWS*NUM_COLS*2);
    memcpy((void *) LOCATION_OF_THE_KRABBY_PATTY_SECRET_FORMULA, (void *) off_screen, NUM_ROWS*NUM_COLS*2);
}

