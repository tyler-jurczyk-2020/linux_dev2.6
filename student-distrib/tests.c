#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "devices/keyboard.h"
#include "filesystem/fs_tests.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
 
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here
//Checks if division by 0 causes an exception
int divide_by_zero_test(){
	TEST_HEADER;

	int i = 3/0;

	return FAIL;
}
//checks if dereferencing a nullptr causes an exception, doesn't work now (i think paging needs to be finished)
int null_ptr_test(){
	TEST_HEADER;
	int j = 0;
	char i = *((char*)(j));

	return FAIL;
}
//Test memory bounds
int valid_mem_test(char *addr){
	TEST_HEADER;
	char i = *addr;
	return PASS;
}

int invalid_mem_test(char *addr){
	TEST_HEADER;
	char i = *addr;
	return FAIL;
}

//tries a system call
int sys_call_test(){
	TEST_HEADER;
	__asm__("MOVL $42, %EAX");
	__asm__("INT $0x80");

	return FAIL;
}

// infinit loop to try keyboard typing
int test_keyboard(){
	TEST_HEADER;
	while(1){

	}
	return FAIL;
}
/* Checkpoint 2 tests */
int terminal_test(){
	TEST_HEADER;
	uint8_t name[1];
	name[0] = 'A';
	terminal_open(name);
	uint8_t buf[128];
	uint8_t size = 128;
	while(1){
		size = terminal_read(0, buf, size);
		terminal_write(0,buf,size);
	}
	return PASS;
}
/* Checkpoint 3 tests */
int syscall_jump(){
	TEST_HEADER;
	uint8_t name[1];
	name[0] = 'A';
	terminal_open(name);
	__asm__("MOVL $5, %EAX");
	__asm__("INT $0x80");
	return PASS;
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	
	// launch your tests here
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("divide_zero_test", divide_by_zero_test());
	//TEST_OUTPUT("null_ptr_test", null_ptr_test());
	//TEST_OUTPUT("sys_call_test", sys_call_test());

    /* Test various bounds of the memory*/

    //TEST_OUTPUT("valid_mem_test", invalid_mem_test((char *)0xB7FFF));
    //TEST_OUTPUT("valid_mem_test", valid_mem_test((char *)0xB8000));
    //TEST_OUTPUT("valid_mem_test", valid_mem_test((char *)0xB8FFF));
    //TEST_OUTPUT("valid_mem_test", invalid_mem_test((char *)0xB9000));

    //TEST_OUTPUT("valid_mem_test", invalid_mem_test((char *)0x3FFFFF));
    //TEST_OUTPUT("valid_mem_test", valid_mem_test((char *)0x400000));
    //TEST_OUTPUT("valid_mem_test", valid_mem_test((char *)0x7FFFFF));
    //TEST_OUTPUT("valid_mem_test", invalid_mem_test((char *)0x800000));

	//TEST_OUTPUT("terminal_test", terminal_test());
	//TEST_OUTPUT("syscall_jump", syscall_jump());
    
    test_file_open();
    test_file_read();
}
