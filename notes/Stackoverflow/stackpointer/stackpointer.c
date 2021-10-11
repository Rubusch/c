// stackpointer.c
/*
  should print out the current stackpoitner
  on a x86 architecture and linux

  returns the address of the current stackpoitner!

  prints a warning at compilation!!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long sp(void)
{
	__asm__("movl %esp,%eax");
}

int main(void)
{
	printf("0x%lx\n", sp());

	exit(EXIT_SUCCESS);
}
