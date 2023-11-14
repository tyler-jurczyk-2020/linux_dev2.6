#include "../types.h"
#include "../filesystem/filesystem.h"
#include "pcb.h"

uint8_t process_ids[MAX_PROCESSES] = {0, 0, 0, 0, 0, 0};

/* int8_t find_terminal_id(requested);
 * Inputs: requested terminal
 * Return Value: pcb index of a process in said terminal
 * Function: iterates through pcbs to find a match*/
int8_t find_terminal_pid(uint8_t requested){
	uint8_t i;
	for(i = 0; i<MAX_PROCESSES; i++){
		pcb_t* traverse = (pcb_t *)(EIGHT_MB - (EIGHT_KB*(i+1)));
		if(process_ids[i] && traverse->terminal_info.terminal_num==requested){
			return i;
		}
	}
	return -1;
}

/* int8_t find_onscreen_terminal();
 * Inputs: none
 * Return Value: terminal number of the onscreen terminal
 * Function: iterates through pcbs to find a match*/
int8_t find_onscreen_terminal_num(){
	uint8_t i;
	for(i = 0; i<MAX_PROCESSES; i++){
		pcb_t* traverse = (pcb_t *)(EIGHT_MB - (EIGHT_KB*(i+1)));
		if(process_ids[i] && traverse->terminal_info.is_onscreen){
			return traverse->terminal_info.terminal_num;
		}
	}
	return -1;
}

pcb_t* find_next_active_pcb(uint32_t current_process){
	uint8_t i;
	pcb_t* traverse = (pcb_t*) current_process;
	for(i = 0; i<MAX_PROCESSES; i++){
		traverse = (pcb_t *)((uint32_t)traverse - EIGHT_KB);
		if((uint32_t)traverse == EIGHT_MB-EIGHT_KB*(MAX_PROCESSES)){
			traverse = EIGHT_MB-EIGHT_KB;
		}
		if(process_ids[i] && traverse->is_active == 1){
			return traverse;
		}
	}
	//should never happen
	return NULL;
}

/* int8_t find_onscreen_terminal_id();
 * Inputs: none
 * Return Value: pcb index of a process with an active terminal
 * Function: iterates through pcbs to find a match*/
int8_t find_onscreen_terminal_pid(){
	uint8_t i;
	for(i = 0; i<MAX_PROCESSES; i++){
		pcb_t* traverse = (pcb_t *)(EIGHT_MB - (EIGHT_KB*(i+1)));
		if(process_ids[i] && traverse->terminal_info.is_onscreen){
			return i;
		}
	}
	return -1;
}

/* uint32_t get_pcb_ptr(pcb_id);
 * Inputs: id of pcb to get
 * Return Value: ptr to pcb
 * Function: uses quick mafs to get the ptr*/
uint32_t get_pcb_ptr(uint8_t pcb_id){
	return (uint32_t)(EIGHT_MB - (EIGHT_KB*(pcb_id+1)));
}

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
/* re_open_fd
 * INPUTS: fd to reopen
 * OUTPUTS: NONE
 * SideEFFECTS: sets the fd to available
 */
void make_available_fd(int32_t fd){
	pcb_t *pcb = get_pcb();
	pcb->available[fd] = 1;
}

/*
 * uint32_t parse_arguments
 * INPUTS: char* command char* executable_buf, char* argument_buf
 * OUTPUTS: Size of argument buf
 * Function : parses the executable name and arguments from the command string
 */
uint32_t parse_arguments(char* command, char* executable_buf, char* argument_buf){
	uint32_t traverse = 0;
	uint32_t arg_count = 0;
	while(traverse < 32 && command[traverse] != '\0' && command[traverse] != ' ' && command[traverse] != '\n'){
		executable_buf[traverse] = command[traverse];
		traverse++;
	}
	executable_buf[traverse] = '\0';
	while(command[traverse] == ' '){
		traverse++;
	}
	while(traverse<128 && command[traverse] != '\n'){
		argument_buf[arg_count] = command[traverse];
		traverse++;
		arg_count++;
	}
	return arg_count;
}

