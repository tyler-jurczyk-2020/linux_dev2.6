#ifndef _FILESYSTEM
#define _FILESYSTEM

#include "../types.h" 

#define FILENAME_LEN 128

typedef struct {
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num; 
    int8_t reserved[24];
} dentry_t;

typedef struct {
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t dir_entries[63];
} boot_t;

typedef struct {
    int32_t length;
    int32_t data_block_num[1023];
} inode_t;
typedef struct {
    boot_t boot;
    inode_t inodes[63];
    int32_t data_blocks[1023*63];
} filesystem_t;

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif
