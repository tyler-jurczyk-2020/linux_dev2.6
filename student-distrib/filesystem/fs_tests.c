#include "filesystem.h"
#include "../lib.h"

void test_file_open() {
    file_open((uint8_t *)"verylargetextwithverylongname.txt");
}
void test_dir_open() {
    dir_open(".");
}

void test_file_read() {
    // File descriptor parameter effectively ignored for now
    uint8_t buffer[4*4096];
    // First four should read completely, last should read nothing
    clear();
    file_read(-1 , buffer, 200);
    puts(buffer);

    clear();
    file_read(-1 , buffer, 200);
    puts(buffer);

    clear();
    file_read(-1 , buffer, 200);
    puts(buffer);
}

void test_dir_read() {
    uint8_t buffer[4*4096];
    clear();
    dir_read(-1 , buffer, 200);
    puts(buffer);

    clear();
    dir_read(-1 , buffer, 200);
    puts(buffer);

    clear();
    dir_read(-1 , buffer, 200);
    puts(buffer);
}
