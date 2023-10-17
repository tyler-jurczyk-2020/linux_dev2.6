#include "../types.h"
#include "filesystem.h"
#include "../lib.h"

boot_t boot_block;
filesystem_t fs;



void init_filesystem() {
    // Initialize boot block on fs
    boot_block.dir_count = 0;
    boot_block.inode_count = 0;
    boot_block.data_count = 0;
    fs.boot = boot_block;

    // Read in data from fsdir
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= fs.boot.dir_count + fs.boot.inode_count) {
        return -1; // Invalid index 
    }
    // For now copy whole filename, but not all files have 128 chars in name 
    memcpy(dentry, fs.boot.dir_entries[index].filename, 128);
    dentry->filetype = fs.boot.dir_entries[index].filetype;
    dentry->inode_num = fs.boot.dir_entries[index].inode_num;
    return 0;     
}


