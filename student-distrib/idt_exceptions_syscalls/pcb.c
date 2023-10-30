#include "../types.h"
#include "../filesystem/filesystem.h"
#include "pcb.h"

uint8_t process_ids[MAX_PROCESSES] = {0, 0, 0, 0, 0, 0};

/* int8_t get_process_id();
 * Inputs: none
 * Return Value: -1 if faliure, else the index of next available process id
 * Function: Gets the index to the next available spot in the process array */

uint8_t get_process_id() {
    uint32_t i;
    for (i=0; i<MAX_PROCESSES; i++) {
        if (process_ids[i] == 0) {
            process_ids[i] = 1;
            return i; 
        } 
    }
    return -1;
}

/* void setup_pcb();
 * Inputs: pointer to the pcb, index of the current process, pointer to parent pcb
 * Return Value: none
 * Function: Sets up the pcb on the kernel stack of the process that is currently executing */

void setup_pcb(pcb_t *pcb, uint32_t my_process_id, pcb_t *parent_pcb) {
    pcb->process_id = my_process_id;
    pcb->parent = parent_pcb;
    pcb->esp0 = EIGHT_MB - (EIGHT_KB*(my_process_id));
    pcb->fd[0] = (file_descriptor_t) {
        .file_ops = &stdin_table,
        .inode = -1,
        .file_pos = 0,
        .flags = 0
    };
    pcb->fd[1] = (file_descriptor_t) {
        .file_ops = &stdout_table,
        .inode = -1,
        .file_pos = 0,
        .flags = 0
    };
    uint32_t i;
    for (i=0; i<8; i++) {
        pcb->available[i] = 1; 
    }
    pcb->available[0] = 0;
    pcb->available[1] = 0;
}

/* pcb_t *get_parent_pcb();
 * Inputs: Index of the current process
 * Return Value: NULL if at the root process, else, return the pointer to parent pcb 
 * Function: Wrapper out get_pcb, which obtains a pointer to the parent pcb, which is the current process setting up the child process */
pcb_t *get_parent_pcb(uint8_t current_process) {
    if(!current_process) {
        return NULL;
    }
    return get_pcb();
}

/* file_descriptor_t *get_fd();
 * Inputs: Index into to file descriptor array 
 * Return Value: NULL if invalid index, else a pointer to the file descriptor in the file descriptor array
 * Function: Helper function to get a pointer to the file descriptor based using the pcb inside the current progam's kernel stack */
file_descriptor_t *get_fd(int32_t fd) {
    if (fd < 0 || fd > 7) {
        return NULL; 
    }
    pcb_t *pcb = get_pcb();
    return &pcb->fd[fd];
}

/* int32_t get_avail_fd();
 * Inputs: none 
 * Return Value: -1 if failure, else return the index of the next available file descriptor 
 * Function: Gets the index of the next available file descriptor from the pcb inside the current program's kernel stack*/
int32_t get_avail_fd() {
    pcb_t *pcb = get_pcb();
    uint32_t i;
    for (i=0; i<8; i++) {
        if (pcb->available[i]) {
            pcb->available[i] = 0;
            return i; 
        } 
    }
    return -1;
}

