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

static void do_jump(int nvar, int rvar, int vvar)
{
	fprintf(stderr, "%s(): nvar=%d, rvar=%d, vvar=%d\n",
			__func__, nvar, rvar, vvar);
	longjmp(env, 1);
}

int main(int argc, char *argv[])
{
	int nvar;
	register int rvar;
	volatile int vvar;

	nvar = 111;
	rvar = 222;
	vvar = 333;

	if (0 == setjmp(env)) {
		nvar = 777;
		rvar = 888;
		vvar = 999;
		do_jump(nvar, rvar, vvar);
	} else {
		fprintf(stderr, "after longjmp(): nvar=%d rvar=%d vvar=%d\n",
				nvar, rvar, vvar);
	}

	puts("READY.");
	exit(EXIT_SUCCESS);
}
