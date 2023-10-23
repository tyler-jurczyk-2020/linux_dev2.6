#ifndef _FILESYSTEM
#define _FILESYSTEM

#include "../types.h" 
#include "../multiboot.h"

#define FILENAME_LEN 32 
#define BYTES_PER_BLOCK 4096
#define EIGHT_MB 0x00800000
#define FOUR_MB 0x00400000
#define PROGRAM_START 0x08048000
#define MAGIC 0x464c457f

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
    int8_t bytes[BYTES_PER_BLOCK];
} data_block_t;

typedef struct {
    boot_t *boot;
    inode_t *inodes;
    data_block_t *data_blocks;
    uint32_t end;
} filesystem_t;

typedef struct {
    int32_t *file_ops;
    int32_t inode;
    int32_t file_pos;
    int32_t flags;
} process_control_t;

void init_filesystem(module_t* mod_info);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t file_open(const uint8_t *filename); 
int32_t file_close(int32_t fd);
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t file_read(int32_t fd, void *buf, int32_t nbytes);

int32_t dir_open(const uint8_t *filename);
int32_t dir_close(int32_t fd);
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes);

int32_t open_executable(const uint8_t *command, uint32_t *eip);

extern process_control_t pcb;
extern filesystem_t fs;

#endif
