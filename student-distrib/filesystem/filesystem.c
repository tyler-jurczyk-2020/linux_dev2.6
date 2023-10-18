#include "filesystem.h"
#include "../multiboot.h"
#include "../types.h"
#include "../lib.h"

filesystem_t fs;


void init_filesystem(module_t* mod_info) {
    fs.boot = (boot_t *)(mod_info->mod_start);
    fs.inodes = ((inode_t *)(mod_info->mod_start)) + 1;
    fs.data_blocks = ((data_block_t *)(mod_info->mod_start)) + fs.boot->inode_count + 2;  
    fs.end = mod_info->mod_end;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= fs.boot->dir_count) { // Number of dentries
        return -1; // Invalid index 
    }
    // For now copy whole filename, but not all files have 128 chars in name 
    memcpy(dentry->filename, fs.boot->dir_entries[index].filename, 128);
    dentry->filetype = fs.boot->dir_entries[index].filetype;
    dentry->inode_num = fs.boot->dir_entries[index].inode_num;
    return 0;     
}

int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry) {
    unsigned int i;
    for(i=0; i<fs.boot->dir_count; i++) {
        int found_file = 0;
        unsigned int j;
        for(j=0; j<FILENAME_LEN; j++) {
            if (fname[j] != fs.boot->dir_entries[i].filename[j]) {
                break; 
            }
            else if (j == FILENAME_LEN - 1) {
                found_file = 1;
            }
        } 
        if (found_file) {
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
        return bytes_not_read; 
    }
    for (i=offset/BYTES_PER_BLOCK; i<(inode_block.length/BYTES_PER_BLOCK)+1; i++) {
        unsigned int j = 0;
        unsigned int end = BYTES_PER_BLOCK;
        // Calculate starting location in first block
        if (offset/BYTES_PER_BLOCK == i) {
            j = offset%BYTES_PER_BLOCK; 
        }
        // Calculate ending location in last block
        if (inode_block.length/BYTES_PER_BLOCK == i) {
            unsigned int desired_off = (length%BYTES_PER_BLOCK)+offset; 
            unsigned int off_limit = (inode_block.length%BYTES_PER_BLOCK);
            end = (desired_off < off_limit) ? desired_off : off_limit;
        }
        // Iterate over data in block
        for (; j<end; j++) {
            *buf = fs.data_blocks[i].bytes[j]; 
            buf++;
            bytes_not_read--;
        }
    } 
    return bytes_not_read;
}
