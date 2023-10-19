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
        uint32_t check_len = (strlen((int8_t *)fname) > FILENAME_LEN) ? FILENAME_LEN : strlen((int8_t *)fname);
        int cmp = strncmp((int8_t *)fname, fs.boot->dir_entries[i].filename, check_len);
        if (cmp == 0) {
            return read_dentry_by_index(i, dentry);
        }
    }
    return -2; // Exited without finding file;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    // Need to still sanitize and error check inputs
    inode_t inode_block = fs.inodes[inode];  
    if (offset > inode_block.length) {
        return -1; // Offset out of bounds 
    }

    uint32_t start = offset%BYTES_PER_BLOCK;
    uint32_t block_idx = offset/BYTES_PER_BLOCK;
    uint32_t bytes_to_read = (length > inode_block.length-offset) ? inode_block.length-offset : length;
    uint32_t bytes_not_read = length - bytes_to_read;
    
    while (bytes_to_read > 0) {
        uint8_t *start_of_copy = &(fs.data_blocks+inode_block.data_block_num[block_idx]-1)->bytes[start];
        if (start+bytes_to_read >= BYTES_PER_BLOCK) {
            memcpy(buf, start_of_copy, BYTES_PER_BLOCK - start);
            bytes_to_read -= BYTES_PER_BLOCK - start;
        } 
        else {
            memcpy(buf, start_of_copy, bytes_to_read);
            bytes_to_read = 0;
        }
        buf += BYTES_PER_BLOCK - start;
        start = 0;
    }
    return bytes_not_read;


/*
    for (i=offset/BYTES_PER_BLOCK; i<((offset+length)/(BYTES_PER_BLOCK))+1 && bytes_not_read > 0; i++) {
        unsigned int j = 0;
        unsigned int end = BYTES_PER_BLOCK;
        // Calculate starting location in first block
        if (offset/BYTES_PER_BLOCK == i) {
            j = offset%BYTES_PER_BLOCK; 
        }
        // Calculate ending location in last block
        if ((offset+length)/(BYTES_PER_BLOCK+1) == i && inode_block.length/(BYTES_PER_BLOCK+1) == i) {
            unsigned int desired_end = (offset+length)%(BYTES_PER_BLOCK+1);
            unsigned int actual_end = (inode_block.length)%(BYTES_PER_BLOCK+1);
            end = (desired_end > actual_end) ? actual_end : desired_end;
        }
        // May want to check pointer returned by memcpy 
        data_block_t *start_of_copy = &(fs.data_blocks+inode_block.data_block_num[i]-1)[j];
        memcpy(buf, start_of_copy, end-j);
        buf += end-j;
        bytes_not_read -= end - j;
    } 
    return bytes_not_read;
    */
}
