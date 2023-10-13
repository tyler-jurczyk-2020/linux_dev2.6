#ifndef _PAGING
#define _PAGING

#define FOUR_KB 4096
#define TABLE_SZ 1024
#define PG_TBL_IDX 184

uint32_t page_dir[TABLE_SZ] __attribute__((aligned(4096)));

void setup_pager_directory();

void enable_paging();

#endif
