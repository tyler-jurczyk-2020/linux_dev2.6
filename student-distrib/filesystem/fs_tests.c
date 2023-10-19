#include "filesystem.h"

void test_file_open() {
    file_open((uint8_t *)"frame0.txt");
}

void test_file_read() {
    // File descriptor parameter effectively ignored for now
    uint8_t buffer[4096];
    file_read(-1 , buffer, 4096);
}
