#include "../lib.h"
#include "../types.h"
#include "filesystem.h"


/* void file_open();
 * Inputs: const uint8_t *filename of the file to open 
 * Return Value: return non-zero if we cannot find the file, else 0 for success
 * Function: Attempts to open the file associated with the specified file name */
int32_t file_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    // Setup pcb entry
    pcb.file_ops = NULL; // Note this should not be here, as file_open would be one of the file_ops 
    pcb.inode = dentry.inode_num;
    pcb.file_pos = 0;
    pcb.flags = 0; // Need to add flags
    return 0;
}

/* void file_close();
 * Inputs: fd 
 * Return Value: 0
 * Function: Closes the file with file descriptor fd */
int32_t file_close(int32_t fd) {
    // May want to clean up process control block
    return 0;
}

/* void file_write();
 * Inputs: fd, buf, nbytes 
 * Return Value: -1
 * Function: Does nothing, cannot write to read-only fs */
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

/* void file_read();
 * Inputs: file descriptor, buf to write to, number of bytes to read 
 * Return Value: Number of unread bytes
 * Function: Reads data of the associated file descriptor */
int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    // Read file and save position we read to
    int unread_bytes = read_data(pcb.inode, pcb.file_pos, buf, nbytes); 
    pcb.file_pos += nbytes - unread_bytes;
    return unread_bytes;
}

/* void dir_open();
 * Inputs: const uint8_t *filename of the directory to open 
 * Return Value: return non-zero if we cannot find the directory, else 0 for success
 * Function: Attempts to open the directory associated with the specified filename */

int32_t dir_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    // Setup pcb entry
    pcb.file_ops = NULL; // Note this should not be here, as file_open would be one of the file_ops 
    pcb.inode = -1; // Indicates its a directory
    pcb.file_pos = 0;
    pcb.flags = 0; // Need to add flags
    return 0;
}

/* void dir_close();
 * Inputs: fd 
 * Return Value: 0
 * Function: Closes the directory file */
int32_t dir_close(int32_t fd) {
    return 0;
}

/* void dir_write();
 * Inputs: fd, buf, nbytes 
 * Return Value: -1
 * Function: Does nothing, cannot write to read-only fs */
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

/* void dir_read();
 * Inputs: file descriptor, buf to write to, number of bytes to read 
 * Return Value: Number of unread bytes
 * Function: Reads the names of the files in the current directory */
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes) {
    unsigned int i;
    // Read all the dir_entries since filesystem is flat
    for(i=0; i<fs.boot->dir_count; i++) {
        dentry_t dentry; 
        int res = read_dentry_by_index(i, &dentry); 
        if (res != 0) {
            return res;
        }
        // Copy the name into the buffer
        memcpy(buf, dentry.filename, FILENAME_LEN);        
        buf += FILENAME_LEN;
    }
    return 0; 
}
