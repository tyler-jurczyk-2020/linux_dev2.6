#include "rtc.h"


/* rtc_init
 * DESCRIPTION: Turning on periodic interrupt and setting frequency 
 *
 * INPUTS: none
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */

void rtc_init() {
	
	outb(Register_B, PORT_index); //disable NMI, and selecting register B
	char prev = inb(PORT_RW); //reading current value of B
	outb(Register_B, PORT_index); //setting index
	outb(prev | RTC_BIT, PORT_index); // writing previous value with 0x40
	
	//enable interrupts
	enable_irq(IRQ8);

	rtc_interrupt_rate(1024); //setting interrupt rate, 1024Hz is default value of output divider frequency
} 

/* rtc_interrupt_rate
 * DESCRIPTION: Setting the frequency value 
 *
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */
void rtc_interrupt_rate(int frequency) {
	//Check for frequency range is accurate
	
	/* TODO: Maximize frequency to virtualiz*/
	int rate = 0;
	rate &= rate_value; 

	cli(); //disable interrupts

	outb(Register_A, PORT_index); //set index to reg A
	char prev = inb(PORT_RW); //initial value of reg A
	outb(Register_A, PORT_RW); //reset index to A
	outb( (prev & 0xF0) | rate, PORT_RW);
	
	//enabling interrupts
	enable_irq(IRQ8); 
	sti();
	
}

/* rtc_interrupt
 * DESCRIPTION: cases when RTC interrupt occurs, utulize test_interrupt in lib.c
 *
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */
 void rtc_interrupt() {

	outb(Register_C, PORT_index); //selecting register C
	inb(PORT_RW); //clearing the contents
	
 }
 
