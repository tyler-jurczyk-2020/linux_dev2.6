#include "lib.h"
#include "PS2.h"

// output buffer: contain data from controller to OS
// input buffer:  contain data from OS to controller
volatile uint8_t response;
volatile uint8_t buffer_check;
static uint8_t dual = 0;

void write_config(uint8_t temp){
    outb(WRITE_CONFIG, REGISTER_PORT);  
    buffer_check = inb(REGISTER_PORT);
    while ((buffer_check & WRITE_MASK) != 0){
        buffer_check = inb(REGISTER_PORT);
    }
    outb(temp, DATA_PORT);
}

uint8_t get_response(){
    uint8_t temp;
    buffer_check = inb(REGISTER_PORT); 
    while((buffer_check & RESPOND_MASK) == 0){    // make sure data arrived
        buffer_check = inb(REGISTER_PORT);
    }
    temp = inb(DATA_PORT);
    return temp;
}

void write_to_device(int device, uint8_t data_to_write){
    if (device == 1){
        buffer_check = inb(REGISTER_PORT);
        while ((buffer_check & WRITE_MASK) != 0){
            buffer_check = inb(REGISTER_PORT);
        }
        outb(data_to_write, DATA_PORT);
    }
    else if(device ==2){
        outb(WRITE_SECOND_PORT, REGISTER_PORT);
        buffer_check = inb(REGISTER_PORT);
        while ((buffer_check & WRITE_MASK) != 0){ 
            buffer_check = inb(REGISTER_PORT);
        }
        outb(data_to_write, DATA_PORT);
    }
    else{
        printf("invalid device\n");
        return;
    }
}
