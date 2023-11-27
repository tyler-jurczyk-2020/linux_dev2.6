#ifndef _PAGING
#define _PAGING

#include "../types.h"

#define FOUR_KB 4096
#define TABLE_SZ 1024
#define PG_TBL_IDX 184
#define VIDEO_PAGE_FLAGS 0x000B8003
#define KERNEL_PAGE_FLAGS 0x00400183
#define EMPTY_PAGE 0x02
#define PROGRAM_IDX 32
#define PROGRAM_ADDR 0x08000000 //128mb
#define VMEM_ADDR 0x08400000 // 132mb
#define KERNEL_VMEM 0xB8000

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

extern page_directory_entry_t page_dir[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

extern page_table_entry_t page_tbl[TABLE_SZ] __attribute__((aligned(FOUR_KB)));

void setup_pager_directory();

void setup_pager_table();

void enable_paging();

void mod_cr3(uint32_t *);

page_directory_entry_t *get_cr3();

void set_pager_dir_entry(uint32_t page_addr);

void setup_pager_vidmap_entry(uint32_t vmem_addr, uint32_t kernel_page);

void setup_pager_vidmap_table(uint32_t vmem_addr);

void switch_kernel_memory(uint32_t on_screen, uint32_t off_screen);

void update_kernel_vmem(uint32_t physical, uint32_t virt);

void update_vidmap_vmem(uint32_t physical, uint32_t virt);

#endif
