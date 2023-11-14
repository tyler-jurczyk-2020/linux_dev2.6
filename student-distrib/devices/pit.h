#define IRQ0            0x00

#define CH0_DATA_PORT   0x40
#define CH1_DATA_PORT   0x41
#define CH2_DATA_PORT   0x42
#define COMMAND_PORT    0x43

#define WRITE_CHO       0x36

#define BASE_F          1193180
#define RUNNING_F_L     20
#define RUNNING_F_H     100

extern void init_pit();
extern void pit_handler();

extern void save_regs(uint32_t ebp_pcb_addr, uint32_t esp_pcb_addr);
extern void do_schedule(uint32_t ebp, uint32_t esp);

void timer_set(int freq);
