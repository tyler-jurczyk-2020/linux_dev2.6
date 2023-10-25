#include "../lib.h"
#include "../types.h"
#include "filesystem.h"
#include "paging.h"


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
    int32_t fd_idx = get_avail_fd();
    file_descriptor_t *file_desc = get_fd(fd_idx);
    if (file_desc == NULL) {
        return -1; 
    }
    file_desc->file_ops = &file_table; // Note this should not be here, as file_open would be one of the file_ops 
    file_desc->inode = dentry.inode_num;
    file_desc->file_pos = 0;
    file_desc->flags = 0; // Need to add flags
    return fd_idx;
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
    file_descriptor_t *file_desc = get_fd(fd);
    int unread_bytes = read_data(file_desc->inode, file_desc->file_pos, buf, nbytes); 
    file_desc->file_pos += nbytes - unread_bytes;
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
    int32_t fd_idx = get_avail_fd();
    file_descriptor_t *file_desc = get_fd(fd_idx);
    if (file_desc == NULL) {
        return -1; 
    }
    file_desc->file_ops = &directory_table; // Note this should not be here, as file_open would be one of the file_ops 
    file_desc->inode = -1; // Indicates its a directory
    file_desc->file_pos = 0;
    file_desc->flags = 0; // Need to add flags
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
    uint32_t bytes_to_copy = nbytes;
    uint32_t i;
    // Read all the dir_entries since filesystem is flat
    for(i=0; i<fs.boot->dir_count; i++) {
        if (bytes_to_copy == 0) {
            break; 
        }
        dentry_t dentry; 
        int res = read_dentry_by_index(i, &dentry); 
        if (res != 0) {
            return res;
        }
        // Copy the name into the buffer
        // We include newline characters into the count of bytes_to_copy
        uint32_t len_to_copy;
        if (dentry.filename[FILENAME_LEN-1] != '\0') {
            len_to_copy = (bytes_to_copy > FILENAME_LEN) ? FILENAME_LEN : bytes_to_copy-1;     
        }
        else {
            len_to_copy = (bytes_to_copy > strlen(dentry.filename)) ? strlen(dentry.filename) : bytes_to_copy-1;
        }
        strncpy(buf, dentry.filename, len_to_copy);
        ((int8_t *)buf)[len_to_copy] = '\n';
        buf += len_to_copy+1;
        bytes_to_copy -= len_to_copy+1;
    }
    return bytes_to_copy; 
}

int32_t open_executable(const uint8_t *command, uint32_t *eip) {
    dentry_t dentry;
    int res = read_dentry_by_name(command, &dentry);
    if (res != 0) {
        return res; 
    }
    uint32_t magic_numbers;
    read_data(dentry.inode_num, 0, (uint8_t *)(&magic_numbers), 4); 
    if (magic_numbers != MAGIC) {
        return -1;
    }
    read_data(dentry.inode_num, 24, (uint8_t *)eip, 4); 
    read_data(dentry.inode_num, 0, (uint8_t *)PROGRAM_START, FOUR_MB);
    return 0;
}
