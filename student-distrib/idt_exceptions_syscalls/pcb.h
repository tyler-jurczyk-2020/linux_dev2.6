#ifndef _PCB
#define _PCB
#include "../types.h"
#include "../devices/keyboard.h"
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

typedef struct terminal_t{
	uint8_t is_onscreen;
	uint8_t terminal_num;
	uint32_t user_page_addr;
	uint32_t fake_page_addr;
} terminal_t;

typedef struct pcb_t {
    uint32_t process_id;
    
	struct pcb_t *parent;
	struct terminal_t terminal_info;
	
	uint32_t schedule_esp;
	uint32_t schedule_ebp;
	
    uint32_t esp0;
	uint32_t halt_ebp; //to be set when this process is executed, keeps track of where to return
	int8_t args[128];
    uint32_t available[8];
    file_descriptor_t fd[8]; 
} pcb_t;

/* returns terminal num (0,1,or 2) */
int8_t find_onscreen_terminal_num();
/* returns process id of onscreen terminal (0-5) */
int8_t find_onscreen_terminal_pid();
/* finds process id of requested terminal */
int8_t find_terminal_pid(uint8_t requested);

uint8_t get_process_id();
/* gets a pointer to the pcb with the inputted pid */
uint32_t get_pcb_ptr(uint8_t pcb_id);

void setup_pcb(pcb_t *pcb, uint32_t my_process_id, pcb_t *parent_pcb);

pcb_t *get_parent_pcb(uint8_t current_process);

uint32_t parse_arguments(char* command, char* executable_buf, char* argument_buf);

extern pcb_t *get_pcb();

file_descriptor_t *get_fd(int32_t fd);

int32_t get_avail_fd();
void make_available_fd(int32_t fd);


#endif
