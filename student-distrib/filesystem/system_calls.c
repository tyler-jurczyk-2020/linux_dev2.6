#include "../lib.h"
#include "../types.h"
#include "filesystem.h"
#include "paging.h"


/* int32_t file_open();
 * Inputs: const uint8_t *filename of the file to open 
 * Return Value: return non-zero if we cannot find the file, else 0 for success
 * Function: Attempts to open the file associated with the specified file name */
int32_t file_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    return dentry.inode_num;
}

/* int32_t file_close();
 * Inputs: fd 
 * Return Value: 0
 * Function: Closes the file with file descriptor fd */
int32_t file_close(int32_t fd) {
    // May want to clean up process control block
    return 0;
}

/* int32_t file_write();
 * Inputs: fd, buf, nbytes 
 * Return Value: -1
 * Function: Does nothing, cannot write to read-only fs */
int32_t file_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

/* int32_t file_read();
 * Inputs: file descriptor, buf to write to, number of bytes to read 
 * Return Value: Number of read bytes
 * Function: Reads data of the associated file descriptor */
int32_t file_read(int32_t fd, void *buf, int32_t nbytes) {
    // Read file and save position we read to
    file_descriptor_t *file_desc = get_fd(fd);
    int read_bytes = read_data(file_desc->inode, file_desc->file_pos.val, buf, nbytes); 
    file_desc->file_pos.val += read_bytes;
    return read_bytes;
}

/* int32_t dir_open();
 * Inputs: const uint8_t *filename of the directory to open 
 * Return Value: return non-zero if we cannot find the directory, else 0 for success
 * Function: Attempts to open the directory associated with the specified filename */

int32_t dir_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res != 0) {
        return res; 
    }
    return 0;
}

/* int32_t dir_close();
 * Inputs: fd 
 * Return Value: 0
 * Function: Closes the directory file */
int32_t dir_close(int32_t fd) {
    return 0;
}

/* int32_t dir_write();
 * Inputs: fd, buf, nbytes 
 * Return Value: -1
 * Function: Does nothing, cannot write to read-only fs */
int32_t dir_write(int32_t fd, const void *buf, int32_t nbytes) {
    return -1; // Not allowed to write to read only filesystem
}

/* int32_t dir_read();
 * Inputs: file descriptor, buf to write to, number of bytes to read 
 * Return Value: Number of read bytes
 * Function: Reads the names of the files in the current directory */
int32_t dir_read(int32_t fd, void *buf, int32_t nbytes) {
    uint32_t bytes_to_copy = nbytes;
    file_descriptor_t *file_desc = get_fd(fd);
    uint32_t i = file_desc->file_pos.dir_entry;
    uint32_t offset = file_desc->file_pos.entry_pos;
    uint32_t last_offset = offset;
    // Read all the dir_entries since filesystem is flat
    for(; i<fs.boot->dir_count; i++) {
        if (bytes_to_copy == 0) {
            break; 
        }
        dentry_t dentry; 
        int32_t res = read_dentry_by_index(i, &dentry); 
        if (res != 0) {
            return res;
        }
        // Copy the name into the buffer
        // We include newline characters into the count of bytes_to_copy
        uint32_t len_to_copy;
        // Determine if newline should be injected at the end of reading the filename
        int32_t is_newline = (bytes_to_copy != 0 && i != fs.boot->dir_count-1);
        if (dentry.filename[FILENAME_LEN-1] != '\0') {
            len_to_copy = (bytes_to_copy > FILENAME_LEN - offset) ? FILENAME_LEN - offset : bytes_to_copy-is_newline;
            last_offset = (bytes_to_copy > FILENAME_LEN - offset) ? 0 : bytes_to_copy-is_newline;
        }
        else {
            len_to_copy = (bytes_to_copy > strlen(dentry.filename) - offset) ? strlen(dentry.filename) - offset : bytes_to_copy-is_newline;
            last_offset = (bytes_to_copy > strlen(dentry.filename) - offset) ? 0 : bytes_to_copy-is_newline;
        }
        strncpy(buf, dentry.filename+offset, len_to_copy);
        bytes_to_copy -= len_to_copy + is_newline;
        if (bytes_to_copy != 0 && i != fs.boot->dir_count-1) {
            ((int8_t *)buf)[len_to_copy] = '\n';
        }
        buf += len_to_copy + is_newline;
        offset = 0;
    }
    // Update file position
    file_desc->file_pos.dir_entry = (last_offset != 0) ? i-1 : i;
    file_desc->file_pos.entry_pos = last_offset;
    // Check if the last character is newline, if so, set it to null to prevent doubling of newline in ls
    buf--;
    if (((char *)buf)[0] == '\n') {
        ((char *)buf)[0] = '\0'; 
    }
    return nbytes - bytes_to_copy; 
}

int32_t check_executable(const uint8_t *command){
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
    return dentry.inode_num;
}

int32_t open_executable(int32_t inode_num, uint32_t *eip) {
    read_data(inode_num, 24, (uint8_t *)eip, 4); 
    read_data(inode_num, 0, (uint8_t *)PROGRAM_START, FOUR_MB);
    return 0;
}
