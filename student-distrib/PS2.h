#ifndef _PS2_H
#define _PS2_H
#include "lib.h"
#define DATA_PORT           0X60
#define REGISTER_PORT       0X64

#define DISABLE_FIRST       0xAD
#define ENABLE_FIRST        0xAE

#define DISABLE_SECOND      0xA7
#define ENABLE_SECOND       0xA8

#define READ_CONFIG         0x20
#define WRITE_CONFIG        0x60
#define DISABLE_IRQ         0xBC
#define TEST_CONRTOLLER     0xAA
#define TEST_PORT_1         0xAB
#define TEST_PORT_2         0xA9
#define WRITE_SECOND_PORT   0xD4 

#define RESPOND_MASK        0x01
#define WRITE_MASK          0x02
#define DUAL_MASK           0x10


void write_config(uint8_t temp);

extern uint8_t get_response();

extern void write_to_device(int port, uint8_t data_to_write);
#endif
