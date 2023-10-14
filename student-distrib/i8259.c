/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
unsigned char cashed_8259[2] = {0xFF, 0xFF};        // Fb because port 2 connected to slave, should be enabled by default
#define cashed_21 (cashed_8259[0])
#define cashed_A1 (cashed_8259[1])
//spinlock_t i8259_lock = SPIN_LOCK_UNLOCKED;
/* Initialize the 8259 PIC */
void i8259_init(void) {

    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW4, MASTER_8259_DATA);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    outb(cashed_8259[0], MASTER_8259_DATA);
    outb(cashed_8259[1], SLAVE_8259_DATA);

	enable_irq(2);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if (irq_num < 8){
        cashed_8259[0] &= ~(1 << irq_num);
    }
    else{
        cashed_8259[1] &= ~(1 << (irq_num - 8));
    }
    outb(cashed_21, MASTER_8259_DATA);
    outb(cashed_A1, SLAVE_8259_DATA);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if (irq_num < 8){
        cashed_8259[0] |= (1 << irq_num);
    }
    else{
        cashed_8259[1] |= (1 << (irq_num - 8));
    }
    outb(cashed_21, MASTER_8259_DATA);
    outb(cashed_A1, SLAVE_8259_DATA);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    outb(EOI, MASTER_8259_PORT);
    
    if(irq_num>=8){
        outb(EOI, SLAVE_8259_PORT);
    }
}
