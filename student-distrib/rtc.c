#include "rtc.h"


 																	/******* MP3.1 Functions *******/

/* rtc_init
 * DESCRIPTION: Turning on periodic interrupt and setting frequency
 * INPUTS: none
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */

void rtc_init() {
	
	outb(Register_B, PORT_index); //disable NMI, and selecting register B
	char prev = inb(PORT_RW); //reading current value of B
	outb(Register_B, PORT_index); //setting index
	outb(prev | RTC_BIT, PORT_RW); // writing previous value with 0x40
	
	//enable interrupts
	enable_irq(IRQ8);

	//rtc_interrupt_rate(1024); //setting interrupt rate, 1024Hz is default value of output divider frequency
} 

/* rtc_interrupt_rate
 * DESCRIPTION: Setting the frequency value 
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: Used by rtc_open and rtc_write 
 */
void rtc_interrupt_rate(int frequency) {
	//Check for frequency range is accurate
	
	/* TODO: Maximize frequency to virtualize*/
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
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */
 void rtc_interrupt() {

	outb(Register_C, PORT_index); //selecting register C
	inb(PORT_RW); //clearing the contents
	
 }

 																	/******* MP3.2 Functions *******/
 
 /* rtc_open
 * DESCRIPTION: Initializing the frequency value to a 2Hz.
 * INPUTS: 
 			filename - pointer to filename
 * OUTPUT: none
 * RETURN VALUE: 0 - Success
 * SIDE EFFECT: none
 */
 int32_t rtc_open (const uint8_t* filename) {
	/*
		1. Use RTC's Periodic Interrupt function
		2. Device opened => 2 Hz
		3. For simplicity, RTC interrupts should remain on at all times. 	
	*/

	//rate *= 2
	int rate = 0;
	rate &= rate_value; 

	cli(); //disable interrupts

	outb(Register_A, PORT_index); //set index to reg A
	char prev = inb(PORT_RW); //initial value of reg A
	outb(Register_A, PORT_RW); //reset index to A
	outb( (prev & 0xF0) | rate, PORT_RW); //write rate to A, rate is bottom 4 bits
	
	//enabling interrupts
	enable_irq(IRQ8); 
	sti();

	return 0; // Success

 }

 /* rtc_read
 * DESCRIPTION: Record and return any RTC interrupt that has occurred.  
 *				Block until next interrupt. Not for reading RTC frequency
 * INPUTS:
			fd - an array for the data structure for a file descriptor
 			buf - pointer to buffer
			nbytes - number of bytes
 * OUTPUT: none
 * RETURN VALUE: return 0
 * SIDE EFFECT: none
 */
 int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes) {
	/*
		Hint:
		1. Behaves like an infinite loop waiting ofr some event to be true
			a. Event => RTC Interrupt
		2. When event occurs, record that state and return it

		Steps:
		1. Set Flag and wait until interrupt handler clears it, then return 0
		2. Jump Table reference by task's file array to call from generic handler into file-type-specific function
			a. Inserted into file array on open sys call
	*/
	return 0; // Success

 }

 /* rtc_write
 * DESCRIPTION: Ability to change frequency. 
 *				New frequency passed through buf or count? Frequency power of 2.
				Input parameter from buf, and not read directly
 * INPUTS: 
 			fd - an array for the data structure for a file descriptor
 			buf - pointer to buffer
			nbytes - number of bytes 
 * OUTPUT: none
 * RETURN VALUE: 
 			0 - Success
			-1 - Failure
 * SIDE EFFECT: none
 */
 int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes) {
	/*
		Hint: 
		1. Pass a variable called frequency into rtc write()
		2. Stored frequency in an int that buf pointed to  
		3. User input frequency should be a power of 2, else failure
			a. one line to check if number is power of two
			b. don't need lookup table or switch statement
			Hint:
				i. Think Binary or in terms of bits 

		Steps: 
		1. Accept 4-byte integer specifying interrupt rate in Hz
		2. Set the rate of periodic interrupts
		3. Writes to regular files return -1, indicating failure since file system is read-only
		4. Call returns # of bytes written pr -1 on failure
	*/

	int rate = 0;
	rate &= rate_value; 

	cli(); //disable interrupts

	outb(Register_A, PORT_index); //set index to reg A
	char prev = inb(PORT_RW); //initial value of reg A
	outb(Register_A, PORT_RW); //reset index to A
	outb( (prev & 0xF0) | rate, PORT_RW); //write rate to A, rate is bottom 4 bits
	
	//enabling interrupts
	enable_irq(IRQ8); 
	sti();

	return -1; //Failed 
 }