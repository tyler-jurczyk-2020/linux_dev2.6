#include "../types.h"
#include "paging.h"

page_directory_entry_t page_dir[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

page_table_entry_t page_tbl[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

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
