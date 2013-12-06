/*
  setting breakpoints

  init the child with PTRACE_TRACEME and start external program,
  then attach a parent process by declaring PTRACE_ATTACH,
  it requests registers and instruction, prints it and,
  when done, detaches with PTRACE_DETACH

  usage:
  1) start dummy
  $ ./rabbit.exe &
  [1] 14204

  2) start ptracer with obtained pid
  $ ./ptrace.exe 14204


  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
*/

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
	while(i<j){
		data.val=ptrace(PTRACE_PEEKDATA,child,addr+i*4,NULL);
		memcpy(laddr,data.chars,long_size);
		++i;
		laddr+=long_size;
	}
	j=len%long_size;
	if(j!=0){
		data.val=ptrace(PTRACE_PEEKDATA,child,addr+i*4,NULL);
		memcpy(laddr,data.chars,j);
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
	laddr=str;
	while(i<j){
		memcpy(data.chars,laddr,long_size);
		ptrace(PTRACE_POKEDATA,child,addr+i*4,data.val);
		++i;
		laddr+=long_size;
	}
	j=len%long_size;
	if(j!=0){
		memcpy(data.chars,laddr,j);
		ptrace(PTRACE_POKEDATA,child,addr+i*4,data.val);
	}
}

int
main(int argc, char **argv)
{
	pid_t traced_process;
	struct user_regs_struct regs, newregs;
	long ins;

/* TODO

   int 0x80, int3


*/
	char code[] = {0xcd, 0x80, 0xcc, 0};
	

	char backup[4];
	if(argc!=2){
		printf("Usage: %s <pid to be traced> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* get traced process pid, and attach */
	traced_process = atoi(argv[1]);
	ptrace(PTRACE_ATTACH,traced_process,NULL,NULL);

	wait(NULL);
	ptrace(PTRACE_GETREGS,traced_process,NULL,&regs);

	/* backup instructions */
	get_data(traced_process,regs.eip,backup,3);

	/* place breakpoint */
	put_data(traced_process,regs.eip,code,3);
	

	/* let process continue, and execute the 3 instructions */
	ptrace(PTRACE_CONT,traced_process,NULL,NULL);

	wait(NULL);
	printf("the process stopped, restoring the original instructions\n");
	printf("press ENTER\n");

	getchar();
	put_data(traced_process,regs.eip,backup,3);

	/* setting the eip back to the original instruction, and let process continue */
	ptrace(PTRACE_SETREGS,traced_process,NULL,&regs);

	ptrace(PTRACE_DETACH,traced_process,NULL,NULL);

	return 0;
}
