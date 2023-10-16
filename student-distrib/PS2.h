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

/* This function writes ps2 configuration register*/
void write_config(uint8_t temp);
/* This function gets response from ps2 or devices*/
extern uint8_t get_response();
/* This function writes into specified device*/
extern void write_to_device(int port, uint8_t data_to_write);
#endif
