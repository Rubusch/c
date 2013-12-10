/*
  setting breakpoints

  init the child with PTRACE_TRACEME and start external program,
  then attach a parent process by declaring PTRACE_ATTACH,
  it requests registers and instruction, prints it and,
  when done, detaches with PTRACE_DETACH

  setting a breakpoint is replacing the specified instruction by a trap
  instruction, e.g. 0xcc

  the replaced instruction needs to be stored and reinjected again, when
  continuing

  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`


  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
*/
// FIXME child process resumes with segmentations fault

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int long_size = sizeof(long);

void
get_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i, j;
	union u{
		long val;
		char chars[long_size];
	}data;
	i=0;
	j=len/long_size;
	laddr=str;
	while (i < j) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}
	j = len%long_size;
	/* since long_size will be 4, we always will fall into this condition for reading */
	if (j != 0) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, j);
	}
	str[len]='\0';
}


void
put_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i=0, j=len/long_size;
	union u{
		long val;
		char chars[long_size];
	} data;
	laddr = str;
	while (i < j) {
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr+i*4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;

	/* since long_size will be 4, we always will fall into this condition for writing back */
	if (j != 0) {
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr+i*4, data.val);
	}
}

int
main(int argc, char **argv)
{
	pid_t traced_process;
	struct user_regs_struct regs;
	char code[] = {0xcd, 0x80, 0xcc, 0}; /*  int 0x80, int3 */
	char backup[4];

	if (argc != 2) {
		printf("Usage: %s <pid to be traced> \n", argv[0]);
		printf("  for running the example, compile it (Makefile) and in one shell window run\n");
		printf("  $ ./rabbit.exe\n");
		printf("\n");
		printf("  in another shell window run, within 10 sec\n");
		printf("  $ ./ptracer.exe `pidof rabbit.exe`\n");

		exit(EXIT_FAILURE);
	}

	printf("runing with long_size = '%d'\n", long_size);

	/* get traced process pid, and attach */
	traced_process = atoi(argv[1]);
	ptrace(PTRACE_ATTACH, traced_process, NULL,NULL);

	wait(NULL);

	ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

	/* backup instructions */
// TODO check if len is correct?
	get_data(traced_process, regs.eip, backup, 3);  

	/* put breakpoint */
// TODO check if len is correct?
	put_data(traced_process, regs.eip, code, 3);  

	/* let process continue, and execute int3 instruction */
	ptrace(PTRACE_CONT, traced_process, NULL, NULL);

	wait(NULL);

	printf("the process stopped, restoring the original instructions\n");
// TODO actually the process does not stop if not attached successfully - test with external window, probably not controllable since it is neither a child, nor declares PTRACE_TRACEME
	printf("press ENTER\n");
	getchar();
	put_data(traced_process, regs.eip, backup, 3);

	/* setting the eip back to the original instruction, and let process continue */
	ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

	ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

	return 0;
}
