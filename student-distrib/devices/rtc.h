#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "../lib.h"

#define PORT_RW 0x71 //port to read/write from/to byte of CMOS configuration space
#define PORT_index 0x70 //por tto specify index and disable NMI

#define Register_A 0x8A //setting to 0x80 bit to disable NMI
#define Register_B 0x8B //setting to 0x80 bit to disable NMI
#define Register_C 0x8C //setting to 0x80 bit to disable NMI

#define RTC_BIT 0x40 //turning on bit 6 
#define IRQ8 8 //irq number
#define rate_value 0x0F //rate must be above 2 and not over 15
#define MAX_FREQ 32768 //max interrupt rate of RTC (Hz)
#define LOW_FREQ 2 //lowest possible frequency that RTC can run at (Hz)
#define HIGH_FREQ 1024 //highest possible frequency that our kernel RTC should run at (Hz) [Max typically 8192 Hz]

#define RTC_V_RATE 7
#define RTC_HIGHEST_RATE 6
#define RTC_V_BASE_F 512


volatile int interrupt; //interrupt flag

// typedef struct rtc_v{
//     uint32_t rate;
//     uint32_t base_f;
//     uint32_t frequency;
//     uint32_t count_num;
//     //uint32_t count_down;
// } rtc_v;


/* initializing RTC */
void rtc_init();

/* Changing Interrupt Rate */
int rtc_interrupt_rate(uint32_t frequency);

/* Handler for RTC interrupt occuring*/
void rtc_interrupt();

/* Resetting Frequency to 2Hz */
int32_t rtc_open (const uint8_t* filename);

/* Return any RTC interrupts */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes);

/* Change the frequency */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

/* Close specified file descriptor */
int32_t rtc_close (int32_t fd);

void write_portA(uint8_t data);

#endif
