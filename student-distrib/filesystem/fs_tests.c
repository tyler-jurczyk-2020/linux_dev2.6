#include "filesystem.h"
#include "../lib.h"

int8_t buffer[2*BYTES_PER_BLOCK];

// Clears the testing buffer
void clear_buffer() {
    memset(buffer, '\0', 2*BYTES_PER_BLOCK);
}

// Try reading the entirety of a big file
void test_large_file_single() {
    clear_buffer();
    file_open((uint8_t *)"verylargetextwithverylongname.txt");
    clear();
    file_read(-1 , buffer, 2*BYTES_PER_BLOCK);
    puts(buffer);
}

// Try reading small chunks of a big file
void test_large_file_multi() {
    clear_buffer();
    file_open((uint8_t *)"verylargetextwithverylongname.txt");
    uint32_t i;
    for(i=0; i<3; i++) {
        clear();
        file_read(-1 , buffer+(200*i), 200);
        puts(buffer);
    }
}

// Try reading the entirety of a small file
void test_small_file() {
    clear_buffer();
    file_open((uint8_t *)"frame1.txt");
    clear();
    file_read(-1 , buffer, BYTES_PER_BLOCK);
    puts(buffer);
}

// Try read the entirety of a directory
void test_dir_read_single() {
    clear_buffer();
    dir_open((uint8_t *)".");
    dir_read(-1 , buffer, 2*BYTES_PER_BLOCK);
    clear();
    uint32_t i;
    for (i=0; i<BYTES_PER_BLOCK/2; i++) {

        putc(buffer[i]);    
    }
}

// Try reading small chunks of a directory
void test_dir_read_multi() {
    clear_buffer();
    dir_open((uint8_t *)".");
    clear();
    uint32_t i;
    for(i=0; i<3; i++) {
        dir_read(-1 , buffer+(200*i), 200);
    }
    for (i=0; i<BYTES_PER_BLOCK/2; i++) {
        putc(buffer[i]);    
    }
}
