#include "filesystem.h"
#include "../multiboot.h"
#include "../types.h"
#include "../lib.h"

filesystem_t fs;

/* int32_t init_filesystem();
 * Inputs: pointer to the filesystem module
 * Return Value: none
 * Function: Initializes the filesystem struct for accessing
 * in-memory filesystem */
void init_filesystem(module_t* mod_info) {
    fs.boot = (boot_t *)(mod_info->mod_start);
    fs.inodes = ((inode_t *)(mod_info->mod_start)) + 1;
    fs.data_blocks = ((data_block_t *)(mod_info->mod_start)) + fs.boot->inode_count + 2;  
    fs.end = mod_info->mod_end;
}

/* int32_t read_dentry_by_index();
 * Inputs: uint32_t index into dir_entries, dentry_t *dentry to store result
 * Return Value: -1 if invalid index provided
 * Function: Read the dentry at the provided index and store the resulting dentry
 * into the dentry_t dentry parameter*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    if (index >= fs.boot->dir_count) { // Number of dentries
        return -1; // Invalid index 
    }
    // For now copy whole filename, along with necessary fields
    memcpy(dentry->filename, fs.boot->dir_entries[index].filename, FILENAME_LEN);
    dentry->filetype = fs.boot->dir_entries[index].filetype;
    dentry->inode_num = fs.boot->dir_entries[index].inode_num;
    return 0;     
}

/* int32_t read_dentry_by_name();
 * Inputs: const uint8_t *fname of the file name to look for, dentry_t *dentry to store result
 * Return Value: -2 if file not found, else return read_dentry_by_index return code
 * Function: Search for the dentry with the passed fname, and if found, retrieve the dentry
 * using read_dentry_by_index */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry) {
    unsigned int i;
    // Iterate over all dir_entries and find the name through string comparison
    for(i=0; i<fs.boot->dir_count; i++) {
        uint32_t check_len = (strlen((int8_t *)fs.boot->dir_entries[i].filename) > FILENAME_LEN) ? FILENAME_LEN : strlen((int8_t *)fs.boot->dir_entries[i].filename);
        int cmp = strncmp((int8_t *)fname, fs.boot->dir_entries[i].filename, check_len);
        if (cmp == 0) {
            return read_dentry_by_index(i, dentry);
        }
    }
    return -2; // Exited without finding file;
}

/* int32_t read_data();
 * Inputs: inode we want to read, file position offset, buf to store data retrieved, number of bytes to attempt to read 
 * Return Value: -1 if offset is out of bounds, else the number of bytes we are unable to read
 * Function: We attempt to read length bytes from the collection of data blocks associated with the inode. If we are able to,
 * we read all the data requested, otherwise we just read as much as we can*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length) {
    inode_t inode_block = fs.inodes[inode];  
    if (offset > inode_block.length) {
        return -1; // Offset out of bounds 
    }

    uint32_t start = offset%BYTES_PER_BLOCK;
    uint32_t block_idx = offset/BYTES_PER_BLOCK;
    // Determine the number of bytes we are allowed to read
    uint32_t bytes_to_read = (length > inode_block.length-offset) ? inode_block.length-offset : length;
    // # Bytes of the request that is ultimately read
    uint32_t bytes_read = bytes_to_read;
    
    // Read out all the bytes that we are able to
    while (bytes_to_read > 0) {
        int8_t *start_of_copy = &(fs.data_blocks+inode_block.data_block_num[block_idx]-1)->bytes[start];
        // Check if we can read to the end
        if (start+bytes_to_read >= BYTES_PER_BLOCK) {
            memcpy(buf, start_of_copy, BYTES_PER_BLOCK - start);
            bytes_to_read -= BYTES_PER_BLOCK - start;
        } 
        // If we can't read to the end of the block
        else {
            memcpy(buf, start_of_copy, bytes_to_read);
            bytes_to_read = 0;
        }
        buf += BYTES_PER_BLOCK - start;
        block_idx++;
        start = 0;
    }
    return bytes_read;
}
