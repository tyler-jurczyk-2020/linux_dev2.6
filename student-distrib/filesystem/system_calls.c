#include "../lib.h"
#include "../types.h"
#include "filesystem.h"


// For now, the follow operations only allow for one file to be open at a time!!
int32_t file_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    pcb.file_ops = NULL; // Note this should not be here, as file_open would be one of the file_ops 
    pcb.inode = dentry.inode_num;
    pcb.file_pos = 0;
    pcb.flags = 0; // Need to add flags
    return 0;
}

int32_t file_close(int32_t fd) {
    // May want to clean up process control block
    return 0;
}

int32_t file_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    int unread_bytes = read_data(pcb.inode, pcb.file_pos, buf, nbytes); 
    pcb.file_pos += nbytes - unread_bytes;
    return unread_bytes;
}

int32_t dir_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    pcb.file_ops = NULL; // Note this should not be here, as file_open would be one of the file_ops 
    pcb.inode = -1; // Indicates its a directory
    pcb.file_pos = 0;
    pcb.flags = 0; // Need to add flags
    return 0;
}

int32_t dir_close(int32_t fd) {
    return 0;
}

int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

//Currently just reads files names and shoves them into a buffer, probably not it chief...
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes) {
    unsigned int i;
    for(i=0; i<fs.boot->dir_count; i++) {
        dentry_t dentry; 
        int res = read_dentry_by_index(i, &dentry); 
        if (res != 0) {
            return res;
        }
        // May be problematic for shorter filenames?
        memcpy(buf, dentry.filename, FILENAME_LEN);        
        buf += FILENAME_LEN;
    }
    return 0; 
}
