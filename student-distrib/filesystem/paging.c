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

void setup_pager_vidmap_table(uint32_t vmem_addr) {
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == ((vmem_addr >> 12) & 0x3FF)) {
            setup_pager_vidmap_entry(vmem_addr, KERNEL_VMEM); // Map to real terminal 0
        } 
        else if (i == (((vmem_addr + FOUR_KB) >> 12) & 0x3FF)) {
            setup_pager_vidmap_entry(vmem_addr + FOUR_KB, KERNEL_VMEM + 2*FOUR_KB); // Map to fake terminal 1
        }
        else if (i == (((vmem_addr + 2*FOUR_KB) >> 12) & 0x3FF)) {
            setup_pager_vidmap_entry(vmem_addr + 2*FOUR_KB, KERNEL_VMEM + 3*FOUR_KB); // Map to fake terminal 2
        }
        else {
            page_tbl_vmem[i].entry = EMPTY_PAGE;
        }
    }   
    page_directory_entry_t* cur_page_dir = get_cr3();  
    cur_page_dir[vmem_addr >> 22].kb.entry = ((uint32_t)page_tbl_vmem | 7);
}

void update_kernel_vmem(uint32_t physical, uint32_t virtual) {
    page_table_entry_t* entry = &page_tbl[virtual >> 12];
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

void switch_kernel_memory(uint32_t on_screen, uint32_t off_screen) { // Note: These should be both the fake addresses
    memcpy((void *) on_screen, (void *) LOCATION_OF_THE_KRABBY_PATTY_SECRET_FORMULA, NUM_ROWS*NUM_COLS*2);
    memcpy((void *) LOCATION_OF_THE_KRABBY_PATTY_SECRET_FORMULA, (void *) off_screen, NUM_ROWS*NUM_COLS*2);
}

void swap_vmem(uint32_t active, uint32_t inactive) {
    setup_pager_vidmap_entry(VMEM_ADDR + active * FOUR_KB, KERNEL_VMEM + active * FOUR_KB);
    setup_pager_vidmap_entry(VMEM_ADDR + inactive * FOUR_KB, KERNEL_VMEM);
}

