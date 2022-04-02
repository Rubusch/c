#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/*
  based on: The Linux Programming Interface, Michael Kerrisk, 2010, p. 134

  no args, enters func1() and will return env with '1'
  -> func1()
     -> <longjmp>

  one or more args, env == '2'
  -> func2()
     -> <longjump>
*/

static jmp_buf env;

static void func2(void)
{
	longjmp(env, 2);
}

static void func1(int argc)
{
	if (argc == 1) {
		longjmp(env, 1);
	}
	func2();
}

int main(int argc, char *argv[])
{
	switch(setjmp(env)) {
		case 0:
			fprintf(stderr, "initial setjump\n");
			func1(argc); // never returns
			break;

		case 1:
			fprintf(stderr, "jumped back from func1()\n");
			break;

		case 2:
			fprintf(stderr, "jumped back from func2()\n");
			break;
	}

	puts("READY.");
	exit(EXIT_SUCCESS);
}
