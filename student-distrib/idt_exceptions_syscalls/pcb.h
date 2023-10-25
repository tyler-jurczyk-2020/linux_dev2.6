#ifndef _PCB
#define _PCB
#include "../types.h"

#define MAX_PROCESSES 6
#define MAX_FILES 8
#define EIGHT_KB 0x00002000

typedef struct {
    int32_t(*open)(const uint8_t *filename);
    int32_t(*close)(int32_t fd);
    int32_t(*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t(*write)(int32_t fd, const void *buf, int32_t nbytes);
} file_ops_table_t;

extern uint8_t process_ids[MAX_PROCESSES];

extern file_ops_table_t stdin_table;
extern file_ops_table_t stdout_table;
extern file_ops_table_t file_table; 
extern file_ops_table_t directory_table; 
extern file_ops_table_t rtc_table; 

typedef struct file_descriptor_t {
    file_ops_table_t *file_ops;
    int32_t inode;
    int32_t file_pos;
    int32_t flags;
} file_descriptor_t;

typedef struct pcb_t {
    uint32_t process_id;
    struct pcb_t *parent;
    uint32_t esp0;
    uint32_t available[8];
    file_descriptor_t fd[8]; 
} pcb_t;

uint8_t get_process_id();

void setup_pcb(pcb_t *pcb, uint32_t my_process_id, pcb_t *parent_pcb);

pcb_t *get_parent_pcb(uint8_t current_process);

extern pcb_t *get_pcb();

#endif
