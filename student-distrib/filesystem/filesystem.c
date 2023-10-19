#include "filesystem.h"
#include "../multiboot.h"
#include "../types.h"
#include "../lib.h"

filesystem_t fs;
process_control_t pcb;


void init_filesystem(module_t* mod_info) {
    fs.boot = (boot_t *)(mod_info->mod_start);
    fs.inodes = ((inode_t *)(mod_info->mod_start)) + 1;
    fs.data_blocks = ((data_block_t *)(mod_info->mod_start)) + fs.boot->inode_count + 2;  
    fs.end = mod_info->mod_end;
}

void init_process_control() {
    // Initialize stdin and stdout file descriptors
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= fs.boot->dir_count) { // Number of dentries
        return -1; // Invalid index 
    }
    // For now copy whole filename, but not all files have 128 chars in name 
    memcpy(dentry->filename, fs.boot->dir_entries[index].filename, FILENAME_LEN);
    dentry->filetype = fs.boot->dir_entries[index].filetype;
    dentry->inode_num = fs.boot->dir_entries[index].inode_num;
    return 0;     
}

int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry) {
    unsigned int i;
    for(i=0; i<fs.boot->dir_count; i++) {
        int cmp = strncmp((int8_t *)fname, fs.boot->dir_entries[i].filename, strlen((int8_t *)fname));
        if (cmp == 0) {
            return read_dentry_by_index(i, dentry);
        }
    }
    return -2; // Exited without finding file;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    // Need to still sanitize and error check inputs
    inode_t inode_block = fs.inodes[inode];  
    unsigned int i;
    unsigned int bytes_not_read = length;
    if (offset > inode_block.length) {
        return -1; // Offset out of bounds 
    }
    for (i=offset/BYTES_PER_BLOCK; i<((offset+length-1)/BYTES_PER_BLOCK)+1; i++) {
        unsigned int j = 0;
        unsigned int end = BYTES_PER_BLOCK;
        // Calculate starting location in first block
        if (offset/BYTES_PER_BLOCK == i) {
            j = offset%BYTES_PER_BLOCK; 
        }
        // Calculate ending location in last block
        if ((offset+length-1)/BYTES_PER_BLOCK == i) {
            unsigned int desired_end = (offset+length)%(BYTES_PER_BLOCK+1);
            unsigned int actual_end = (inode_block.length)%(BYTES_PER_BLOCK+1);
            end = (desired_end > actual_end) ? actual_end : desired_end;
        }
        // May want to check pointer returned by memcpy 
        memcpy(buf, fs.data_blocks+inode_block.data_block_num[i]-1, end-j);
        buf += end-j;
        bytes_not_read -= end - j;
    } 
    return bytes_not_read;
}
