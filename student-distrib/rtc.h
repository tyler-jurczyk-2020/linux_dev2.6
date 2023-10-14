#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"

#define PORT_RW 0x71 //port to read/write from/to byte of CMOS configuration space
#define PORT_index 0x70 //por tto specify index and disable NMI

#define Regiter_A 0x8A //setting to 0x80 bit to disable NMI
#define Regiter_B 0x8B //setting to 0x80 bit to disable NMI
#define Regiter_C 0x8C //setting to 0x80 bit to disable NMI

#define RTC_BIT 0x40 //turning on bit 6 
#define IRQ8 8 //irq number
#define rate_value 0x0F //rate must be above 2 and not over 15
#define MAX_FREQ 32768 //max interrupt rate of RTC (Hz)

/* initializing RTC */
void rtc_init();

/*Changing Interrupt Rate*/
void rtc_interrupt_rate(int frequency);


/* Handler for RTC interrupt occuring*/
void rtc_interrupt();
/**/

#endif 