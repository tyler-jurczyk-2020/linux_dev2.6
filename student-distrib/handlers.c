#include "handlers.h"
#include "idt_entries.h"
#include "x86_desc.h"
#include "lib.h"

void exception_handler(){
	printf("exception");
	while(1);
}
void populate_idt(){
	printf("%x",isr_stub_table[0]);
}
