#include "pcb.h"
#include "../filesystem/filesystem.h"
#include "../devices/keyboard.h"
#include "../devices/rtc.h"

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BELOW ARE DUMMY FUNCTIONS FOR STDIN/STDOUT
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

int32_t dummy_open(const uint8_t *filename){
	return -1;
}
int32_t dummy_close(int32_t fd){
	return -1;
}
int32_t dummy_write(int32_t fd, const void *buf, int32_t nbytes){
	return -1;
}
int32_t dummy_read(int32_t fd, void *buf, int32_t nbytes){
	return -1;
}

file_ops_table_t stdin_table = {
    .open = &dummy_open, 
    .close = &dummy_close,
    .read = &terminal_read,
    .write = &dummy_write
};

file_ops_table_t stdout_table = {
    .open = &dummy_open,
    .close = &dummy_close,
    .read = &dummy_read,
    .write = &terminal_write
};

file_ops_table_t file_table = {
    .open = &file_open,
    .close = &file_close,
    .read = &file_read,
    .write = &file_write
};

file_ops_table_t directory_table = {
    .open = &dir_open,
    .close = &dir_close,
    .read = &dir_read,
    .write = &dir_write,
};

file_ops_table_t rtc_table = {
    .open = &rtc_open,
    .close = &rtc_close,
    .read = &rtc_read,
    .write = &rtc_write
};
