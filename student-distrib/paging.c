#include "types.h"
#include "paging.h"

uint32_t page_tbl[TABLE_SZ] __attribute__((aligned(4096)));

/* # Declare page table for 0-4MB
 * .align 4096
 * page_tbl:
 * .fill VIDEO_ENT-1, 4, 0     
 * # Only Present set. Need Global?
 * .long 0x000B8001 # 0x000B8000
 * .fill 1024-VIDEO_ENT, 4, 0
 */
void setup_pager_table() {
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == PG_TBL_IDX) {
            page_tbl[i] =  0x000B8003;
        } 
        else {
            page_tbl[i] = 0x02;
        }
    }   
}
/* # Declare a page directory with 1024 entries
 * .align 4096
 * page_dir:
 * # Split into 4kB sectors
 * .long page_tbl+1
 * # 4 MB Kernel, set Global, Page Size, Present
 * # Defaults to Supervisor
 * .long 0x00400181 # 0x00400000
 * .fill 1022, 4, 0 # Not present
 */
void setup_pager_directory() {
    setup_pager_table();
    unsigned int i;
    for(i = 0; i < TABLE_SZ; i++) {
        if (i == 0) {
            page_dir[i] = ((uint32_t) page_tbl) | 3; // Sets the present and r/w priv
        }
        else if (i == 1) {
            page_dir[i] = 0x00400183;
        }
        else {
            page_dir[i] = 0x02;
        }
    }
}
