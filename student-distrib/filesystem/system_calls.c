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
    if (res < 0) {
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
    int read_bytes = read_data(file_desc->inode, file_desc->file_pos, buf, nbytes); 
    file_desc->file_pos += read_bytes;
    return read_bytes;
}

/* int32_t dir_open();
 * Inputs: const uint8_t *filename of the directory to open 
 * Return Value: return non-zero if we cannot find the directory, else 0 for success
 * Function: Attempts to open the directory associated with the specified filename */

int32_t dir_open(const uint8_t *filename) {
    dentry_t dentry;
    int res = read_dentry_by_name(filename, &dentry);
    if (res < 0) {
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
    file_descriptor_t *file_desc = get_fd(fd);
    uint32_t i = file_desc->file_pos;
	uint32_t len_to_copy;
	dentry_t dentry; 
	//if last dir entry read, return 0 bytes read
	if(i>=fs.boot->dir_count){
		return 0;
	}
	//try to read dir
	int32_t res = read_dentry_by_index(i, &dentry); 
    if (res < 0) {
        return res;
    }
	//figure out how much to copy
	if (dentry.filename[FILENAME_LEN-1] != '\0') {
            len_to_copy = (nbytes >= FILENAME_LEN) ? FILENAME_LEN : nbytes;     
    }else{
            len_to_copy = (nbytes >= strlen(dentry.filename)) ? strlen(dentry.filename) : nbytes;
    }
	//copy
	strncpy(buf, dentry.filename, len_to_copy);
	//update dir index
	file_desc->file_pos = i+1;
	//return how much was copied
	return len_to_copy;
	/*  no longer used keep for reference
    // Read all the dir_entries since filesystem is flat
    for(; i<fs.boot->dir_count; i++) {
        dentry_t dentry; 
        int32_t res = read_dentry_by_index(i, &dentry); 
        if (res != 0) {
            return res;
        }
        // Copy the name into the buffer
        // We include newline characters into the count of bytes_to_copy
        uint32_t len_to_copy;
        if (dentry.filename[FILENAME_LEN-1] != '\0') {
            len_to_copy = (bytes_to_copy >= FILENAME_LEN) ? FILENAME_LEN : 0;     
        }
        else {
            len_to_copy = (bytes_to_copy >= strlen(dentry.filename)) ? strlen(dentry.filename) : 0;
        }
        if (len_to_copy == 0) {
            break; 
        }
        strncpy(buf, dentry.filename, len_to_copy);
        // Perform check to see if we need to inject a newline character
        int32_t is_newline = (len_to_copy != bytes_to_copy);
        bytes_to_copy -= len_to_copy + is_newline;
        if (is_newline) {
            ((int8_t *)buf)[len_to_copy] = '\n';
        }
        buf += len_to_copy + is_newline;
    }
    // Check if the last character is newline, if so, reset it to prevent doubling of newline in ls
    buf--;
    if (((char *)buf)[0] == '\n') {
        ((char *)buf)[0] = '\0'; 
    }
    // Update offset
    file_desc->file_pos = i;
    return nbytes - bytes_to_copy; 
	*/
}

/* int32_t check_executable();
 * Inputs: Pointer to array of chars holding the name of binary to execute 
 * Return Value: Negative if faliure, else return the inode_num associated with binary
 * Function: Performs a check on the passed binary to see if it is a valid executable */

int32_t check_executable(const uint8_t *command){
    dentry_t dentry;
    int res = read_dentry_by_name(command, &dentry);
    if (res < 0) {
        return res; 
    }
    uint32_t magic_numbers;
    read_data(dentry.inode_num, 0, (uint8_t *)(&magic_numbers), 4); 
    if (magic_numbers != MAGIC) {
        return -1;
    }
    return dentry.inode_num;
}

/* int32_t open_executable();
 * Inputs: inode number associated with the exectuable and a pointer to the instruciton pointer 
 * Return Value: Negative if faliure, else return 0 for success 
 * Function: Attempts to open the executable by copying it to the correct place in memory */

int32_t open_executable(int32_t inode_num, uint32_t *eip) {
    int res = read_data(inode_num, 24, (uint8_t *)eip, 4); 
    if (res < 0) {
        return res;
    }
    res = read_data(inode_num, 0, (uint8_t *)PROGRAM_START, FOUR_MB);
    if (res < 0) {
        return res;
    }
    return 0;
}
