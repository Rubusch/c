/*
  attaching and detaching

  attaching (PTRACE_ATTACH) to some external process, a parent requests
  registers and instruction, prints it and, when done, detaches with
  PTRACE_DETACH

  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`

  ptrace.exe prints the next instruction


  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char** argv)
{
	pid_t traced_process;
	struct user_regs_struct regs;
	long ins;
	if (argc!=2) {
		printf("Usage: %s <pid to be traced: %s>\n",argv[0],argv[1]);
		exit(EXIT_FAILURE);
	}
	traced_process=atoi(argv[1]);

	/* attach to the child process */
	ptrace(PTRACE_ATTACH,traced_process,NULL,NULL);

	wait(NULL);

	/* read out registers */
	ptrace(PTRACE_GETREGS,traced_process,NULL,&regs);

	/* read out instruction */
	ins=ptrace(PTRACE_PEEKTEXT,traced_process,regs.eip,NULL);

	printf("EIP: %lx instruction executed: '%lx'\n",regs.eip,ins);

	/* detach */
	ptrace(PTRACE_DETACH,traced_process,NULL,NULL);
	return 0;
}
