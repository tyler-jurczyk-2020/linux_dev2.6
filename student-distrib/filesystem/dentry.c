#include <stdint.h>
#include "filesystem.h"

boot_t boot_block;


void init_boot_block() {
    boot_block.dir_count = 0;
    boot_block.inode_count = 0;
    boot_block.data_count = 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
 return 0;     
}
