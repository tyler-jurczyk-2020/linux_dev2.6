#include "../types.h"
#include "../filesystem/filesystem.h"
#include "pcb.h"

uint8_t process_ids[MAX_PROCESSES] = {0, 0, 0, 0, 0, 0};

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
}

pcb_t *get_parent_pcb(uint8_t current_process) {
    if(!current_process) {
        return NULL;
    }
    return get_pcb();
}
