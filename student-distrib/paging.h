#ifndef _PAGING
#define _PAGING

#include "types.h"

#define FOUR_KB 4096
#define TABLE_SZ 1024
#define PG_TBL_IDX 184
#define VIDEO_PAGE_FLAGS 0x000B8003
#define KERNEL_PAGE_FLAGS 0x00400183
#define EMPTY_PAGE 0x02

typedef struct {
    union {
        uint32_t entry;
        struct {
            uint32_t present       : 1;
            uint32_t r_w           : 1;
            uint32_t usr_supr      : 1;
            uint32_t write_thru    : 1;
            uint32_t disable_cache : 1;
            uint32_t accessed      : 1;
            uint32_t reserved      : 1;
            uint32_t page_size     : 1;
            uint32_t global        : 1;
            uint32_t avail         : 3;
            uint32_t base_addr     : 20;
            } __attribute__ ((packed));
    };
} page_directory_entry_kb_t;

typedef struct {
    union {
        uint32_t entry;
        struct {
            uint32_t present       : 1;
            uint32_t r_w           : 1;
            uint32_t usr_supr      : 1;
            uint32_t write_thru    : 1;
            uint32_t disable_cache : 1;
            uint32_t accessed      : 1;
            uint32_t dirty         : 1;
            uint32_t page_size     : 1;
            uint32_t global        : 1;
            uint32_t avail         : 3;
            uint32_t tbl_attr_idx  : 1;
            uint32_t reserved      : 9;
            uint32_t base_addr     : 10;
            } __attribute__ ((packed));
    };
} page_directory_entry_mb_t;

typedef struct {
    union {
        page_directory_entry_kb_t kb;
        page_directory_entry_mb_t mb;       
        uint32_t empty;
    };
} page_directory_entry_t;

typedef struct {
    union {
        uint32_t entry;
        struct {
            uint32_t present       : 1;
            uint32_t r_w           : 1;
            uint32_t usr_supr      : 1;
            uint32_t write_thru    : 1;
            uint32_t disable_cache : 1;
            uint32_t accessed      : 1;
            uint32_t dirty         : 1;
            uint32_t tbl_attr_idx  : 1;
            uint32_t global        : 1;
            uint32_t avail         : 3;
            uint32_t base_addr     : 20;
            } __attribute__ ((packed));
    };
} page_table_entry_t;

page_directory_entry_t page_dir[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

page_table_entry_t page_tbl[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

void setup_pager_directory();

void enable_paging();

void mod_cr3(uint32_t *);

#endif
