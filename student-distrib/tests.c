#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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

	return 0;
}
//checks if dereferencing a nullptr causes an exception, doesn't work now (i think paging needs to be finished)
int null_ptr_test(){
	TEST_HEADER;
	int j = 0;
	char i = *((char*)(j));

	return 0;
}
//tries a system call
int sys_call_test(){
	TEST_HEADER;
	__asm__("MOVL $42, %EAX");
	__asm__("INT $0x80");

	return 0;
}

// infinit loop to try keyboard typing
int test_keyboard(){
	TEST_HEADER;
	while(1){

	}
	return 0;
}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	
	// launch your tests here
	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("divide_zero_test", divide_by_zero_test()); 
	//TEST_OUTPUT("null_ptr_test", null_ptr_test());
	//TEST_OUTPUT("sys_call_test", sys_call_test());
	//TEST_OUTPUT("keyboard test", test_keyboard());
}
