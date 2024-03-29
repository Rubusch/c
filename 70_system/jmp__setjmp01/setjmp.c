#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdnoreturn.h>

/*
  regular 'goto' does not allow to jump outside the current function (linux)
  setting a marker with setjmp(), then executing a longjmp() from another
  function performs that

  setjmp/longjmp is a debugging technique, used for provoking situations,
  debugging protocol implementations, hardware stuff, etc. and should never be
  used in productive code

  demo taken from:
  https://en.cppreference.com/w/c/program/setjmp
*/

jmp_buf jump_buffer;

noreturn void func(int count)
{
	printf("call: %s(count = %d)\n", __func__, count);

	printf("\tlongjmp(jump_buffer, count+1 = %d)\n", count + 1);
	longjmp(jump_buffer, count + 1); // will return count+1 out of setjmp
}

int main(void)
{
	volatile int count = 0; // modified local vars in setjmp scope must be volatile

	if (setjmp(jump_buffer) != 9) { // compare against constant in an if-condition
		printf("\tsetjmp(jump_buffer) != 9\n");
		func(++count); // calls "longjmp"
	}
	printf("\tsetjmp(jump_buffer) == 9 ---> EXIT\n");

	puts("READY.");
	exit(EXIT_SUCCESS);
}
