/*
  inject "hello world"

  attach an external process by declaring PTRACE_ATTACH;

  request its instruction (PTRACE_PEEKDATA) from process's registers
  (PTRACE_GETREGS), backup instructions and inject (PTRACE_POKEDATA) a "hello
  world" sequence as hex;

  write to instruction (PTRACE_POKEDATA) and set registers (PTRACE_SETREGS)

  continue the process (PTRACE_CONT) and let it run into the trap

  in the tracer wait, then replace the injected "hello world" in the process by
  the former, backuped, code (PTRACE_POKEDATA), and inject it into process's
  registers (PTRACE_SETREGS)

  when done, detaches with PTRACE_DETACH


  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`

  the result should inject code to print "Hello world" in the controlled process

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/
// FIXME process resumes with segmentations fault

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
	} data;
	i = 0;
	j = len / long_size;
	laddr = str;
	while (i < j) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if (j != 0) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr+i*4, NULL);
		memcpy(laddr, data.chars, j);
	}
	str[len] = '\0';
}


void
put_data(pid_t child, long addr, char *str, int len)
{
	char *laddr;
	int i = 0, j = len / long_size;
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
	if (j != 0) {
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr+i*4, data.val);
	}
}


int main(int argc, char *argv[])
{
	pid_t traced_process;
	struct user_regs_struct regs;
// TODO check if this length is actually correct
	int len = 41;   
	char insertcode[] =
		"\xeb\x15\x5e\xb8\x04\x00"
		"\x00\x00\xbb\x02\x00\x00\x00\x89\xf1\xba"
		"\x0c\x00\x00\x00\xcd\x80\xcc\xe8\xe6\xff"
		"\xff\xff\x48\x65\x6c\x6c\x6f\x20\x57\x6f"
		"\x72\x6c\x64\x0a\x00";
	char backup[len];

	if (argc != 2) {
		printf("Usage: %s <pid to be traced> \n", argv[0]);
		printf("  for running the example, compile it (Makefile) and in one shell window run\n");
		printf("  $ ./rabbit.exe\n");
		printf("\n");
		printf("  in another shell window run, within 10 sec\n");
		printf("  $ ./ptracer.exe `pidof rabbit.exe`\n");

		exit(EXIT_FAILURE);
	}

	traced_process = atoi(argv[1]);

        /* attach process */
	ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
	wait(NULL);

        /* get registers */
	ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

        /* backup instructions */
	get_data(traced_process, regs.eip, backup, len);

        /* inject new instructions */
	put_data(traced_process, regs.eip, insertcode, len);

        /* restore registers */
	ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

        /* continue process */
	ptrace(PTRACE_CONT, traced_process, NULL, NULL);

	wait(NULL);

	printf("The process stopped, Putting back the original instructions\n");

        /* put back backuped instructions */
	put_data(traced_process, regs.eip, backup, len);

        /* set registers */
	ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

	printf("Letting it continue with original flow\n");

        /* detach process */
	ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

	return 0;
}
