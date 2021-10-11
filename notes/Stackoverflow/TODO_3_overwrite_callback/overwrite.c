// overwrite.c
/*
  this code overwrites the callback of the function by shifting
  the pointer of the subfunction

  the shift is in "byte", thus it's easier shift the pointer as char pointer
  (offset 1 byte) and pass it's address to an int pointer, since we want to
  overwrite an int (offset of int is 4 byte)


  show the code in assembler:
    gcc -S -o overwrite.s overwrite.c

  dissassemble the code to figer out the shift
  - do a make
  - open gdb <prgname>
  - [gdb] disassemble main

  get address of function call
  (...)
  0x0804843e - call 0x80483d4 <func>    - call
  0x8084843e - movl $0x1,-0x8(%ebp)     - return
  0x8084844a - mov  -0x8(%ebp),%edx     -   to be overjumped
  0x8084844d - lea  -0x8(%ebp),%eax     - continue
  0x80848450 - mov  %edx,0x8(%esp)
  (...)

  // TODO: check size of func
  // TODO: move return point of func, past the assignment x=1

//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHIFT 53

//* // gives a warning
unsigned long sp(void)
{
	__asm__("movl %esp,%eax");
}
//*/

void func(int a, int b, int c)
{
	char buf_1[5];
	char *buf_2 = buf_1;

	// move pointer to the address to be changed
	buf_2 += SHIFT;

	// modify address / instruction
	unsigned long **tmp;
	*tmp = (unsigned long *)buf_2;
	(*tmp) += SHIFT;
	printf("*tmp\t= 0x%lx - **tmp = 0x%lx\n", (unsigned long)*tmp, **tmp);
}

int main(void)
{
	int x;

	// TODO: check
	printf("stack\t= 0x%lx\n", sp());

	x = 0;
	printf("&x\t= 0x%lx - x = %d\n", (unsigned long)&x, x);

	func(1, 2, 3);

	x = 1;
	printf("&x\t= 0x%lx - x = %d\n", (unsigned long)&x, x);

	exit(EXIT_SUCCESS);
}
