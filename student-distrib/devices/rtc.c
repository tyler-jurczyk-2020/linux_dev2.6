#include "rtc.h"
#include "../lib.h"

 																	/******* MP3.1 Functions *******/

/* rtc_init
 * DESCRIPTION: Turning on periodic interrupt and setting frequency
 * INPUTS: none
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */

void rtc_init() {
	interrupt = 0; //no interrupt occured
	outb(Register_B, PORT_index); //disable NMI, and selecting register B
	char prev = inb(PORT_RW); //reading current value of B
	outb(Register_B, PORT_index); //setting index
	outb(prev | RTC_BIT, PORT_RW); // writing previous value with 0x40
	
	//enable interrupts
	enable_irq(IRQ8);

	//rtc_interrupt_rate(2); //setting interrupt rate, 1024Hz is default value of output divider frequency
} 

/* rtc_interrupt_rate
 * DESCRIPTION: Setting the frequency value 
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: Used by rtc_open and rtc_write 
 */
int rtc_interrupt_rate(uint32_t frequency) {
	int updated_rate = 6;

	//ERROR CHECKING
	if (frequency < LOW_FREQ || frequency > HIGH_FREQ) {
		return -1;
	} 

	//Checking if frequency is a power of 2 
	if ((frequency & (frequency - 1))) {
		return -1; 
	}
	
	//frequency = 32768 >> (rate-1)
	//All posible frequency rates
	if (frequency == HIGH_FREQ) {	
		updated_rate = 6; //0x06
	} else if (frequency == 512) {	
		updated_rate = 7; //0x07
	} else if (frequency == 256) {
		updated_rate = 8; //0x08
	} else if (frequency == 128) {	
		updated_rate = 9; //0x09
	} else if (frequency == 64) {	
		updated_rate = 10; //0x0A
	} else if (frequency == 32) {	
		updated_rate = 11; //0x0B
	} else if (frequency == 16) {	
		updated_rate = 12; //0x0C
	} else if (frequency == 8) {	
		updated_rate = 13; //0x0D
	} else if (frequency == 4) {	
		updated_rate = 14; //0x0E
	} else if (frequency == LOW_FREQ) {	
		updated_rate = 15; //0x0F
	} else if (frequency == 0) {
		updated_rate = 0;
	}

	return updated_rate;
}

/* rtc_interrupt
 * DESCRIPTION: cases when RTC interrupt occurs, utulize test_interrupt in lib.c
 * INPUTS: frequency
 * OUTPUT: none
 * RETURN VALUE: none
 * SIDE EFFECT: none
 */
 void rtc_interrupt() {

	cli(); 
	
	outb(Register_C, PORT_index); //selecting register C
	inb(PORT_RW); //clearing the contents
	//test_interrupts();
	send_eoi(8);

	interrupt = 1; //interrupt occured

	sti();
	
 }

 																	/******* MP3.2 Functions *******/
 
 /* rtc_open
 * DESCRIPTION: Initializing the frequency value to a 2Hz.
 * INPUTS: 
 			filename - pointer to filename
 * OUTPUT: none
 * RETURN VALUE: 
 			0 - Success
			-1 - Failure
 * SIDE EFFECT: none
 */
 int32_t rtc_open (const uint8_t* filename) {
	/*
		1. Use RTC's Periodic Interrupt function
		2. Device opened => 2 Hz
		3. For simplicity, RTC interrupts should remain on at all times. 	
	*/

	//ERROR CHECKING
	uint8_t buf[128];
	buf[0] = 0;
	rtc_init();

	//rate *= 2
	int rate = HIGH_FREQ; //Lowest possible frequency, starting with low frequency 
	rate &= rate_value; //rate above 2 and not over 15

	
	
	outb(Register_A, PORT_index); //set index to reg A
	char prev = inb(PORT_RW); //initial value of reg A
	outb(Register_A, PORT_RW); //reset index to A
	outb( (prev & 0xF0) | rate, PORT_RW); //write rate to A, rate is bottom 4 bits

	rtc_write(0, buf, 16);
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
 * RETURN VALUE: 
 			0 - Success
			-1 - Failure
 * SIDE EFFECT: none
 */
 int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes) {
	/*
		Hint:
		1. Behaves like an infinite loop waiting for some event to be true
			a. Event => RTC Interrupt
		2. When event occurs, record that state and return it

		Steps:
		1. Set Flag and wait until interrupt handler clears it, then return 0
		2. Jump Table reference by task's file array to call from generic handler into file-type-specific function
			a. Inserted into file array on open sys call
	*/

	//ERROR CHECKING
	// if (nbytes != 4 || buf == NULL) {
	// 	return -1; //nbytes should never not be 4 OR buffer shouldn't be NULL
	// }

	interrupt = 0; //no interrupts has occured
	while (interrupt != 1) {
		//empty while loop
		
	}
	interrupt = 0; //resetting flag to 0, no interrupt occurs
	return 0;

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
		4. Call returns # of bytes written or -1 on failure
	*/

	//ERROR CHECKING
	// nbytes != 4 ||
	if ( buf == NULL) {
		return -1; //nbytes should never not be 4 OR buffer shouldn't be NULL
	}

	uint32_t rate = nbytes;
	int new_rate = rtc_interrupt_rate(rate);
	
	new_rate &= rate_value; 

	outb(Register_A, PORT_index); //set index to reg A
	char prev = inb(PORT_RW); //initial value of reg A
	outb(Register_A, PORT_RW); //reset index to A
	outb((prev & 0xF0) | new_rate, PORT_RW); //write rate to A, rate is bottom 4 bits

	return 0; //Success 
 }


 /* rtc_close
 * DESCRIPTION: Closes the specified file descriptor and makes it available 
				for return from later calls to open
 * INPUTS: 
 			fd - an array for the data structure for a file descriptor
 * OUTPUT: none
 * RETURN VALUE:
			0 - Success
			-1 - Failure
 * SIDE EFFECT: none
 */
int32_t rtc_close (int32_t fd) {

	//ERROR CHECKING

	return 0;
}
