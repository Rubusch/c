/*
  setting breakpoints


  attach to a running rabbit.exe process with PTRACE_SEIZE and
  PTRACE_INTERRUPT

  in the parent obtain the current regs and instruction with PTRACE_GETREGS

  wait on ENTER to continue the rabbit.exe after any syscall via PTRACE_CONT

  if the RAX (EAX) register shows the syscall SYS_write,
  obtain the argument (address in: rsi, length rdx), and store it in 'backup',
  and inject a break point SIGTRAP signal, i.e. {0xcd, 0x80, 0xcc, 0}

  stop the attached rabbit.exe and show the captured 'backup' manually,
  use either PTRACE_PEEKDATA or PTRACE_PEEKTEXT

  detach the rabbit.exe with PTRACE_DETACH

  observation: at the end, the breakpoint would write a message


  ---

  NOTE: error code handling of ptrace() calls are removed to make it more
  readable, for productive code consider error code evaluation

  if (0 > ptrace(...)) {
      // free resources
      perror("something failed");
      exit(EXIT_FAILURE);
  }


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

// kill()
#define _POSIX_SOURCE
#include <syscall.h>
#include <signal.h>

// ptrace()
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define long_size sizeof(long)

void get_data(pid_t child, long addr, unsigned char *str, int len)
{
	unsigned char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	} data;

	i = 0;
	j = len / long_size;

	laddr = str;

	//*
	// version via PTRACE_PEEKDATA and alignment to 8 (due to x86_64)
	while (i < j) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + (i)*8, NULL);
		memcpy(laddr, data.chars, long_size);
		++i;
		laddr += long_size;
	}
	/*/
  // version via PTRACE_PEEKTEXT (x86_64 no alignment needed)
  memset(laddr, '\0', len);
  int idx=0;
  for (idx=0; idx<len-1; ++idx) {
    laddr[idx] = ptrace(PTRACE_PEEKTEXT, child, addr+idx, NULL);
  }
// */

	j = len % long_size;

	// since long_size will be 8, we always will fall into this condition for reading
	if (j != 0) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 8, NULL);
		memcpy(laddr, data.chars, j);
	}

	str[len] = '\0';
}

void put_data(pid_t child, long addr, unsigned char *str, int len)
{
	unsigned char *laddr;
	int i = 0, j = len / long_size;
	union u {
		long val;
		char chars[long_size];
	} data;
	laddr = str;
	while (i < j) {
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr + i * 8, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;

	// since long_size will be 4, we always will fall into this condition for writing back
	if (j != 0) {
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr + i * 8, data.val);
	}
}

int main(int argc, char **argv)
{
	pid_t traced_process;
	struct user_regs_struct regs;
	int len = 32;
	unsigned char code[] = {
		0xcd, 0x80, 0xcc, 0
	}; /*  int 0x80, int3 - but also 64bit safe, see binutils-gdb */

	unsigned char backup[len];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pid to be traced> \n", argv[0]);
		fprintf(stderr,
			"$ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe)\n");
		exit(EXIT_FAILURE);
	}

#ifndef __x86_64__
	fprintf(stderr, "Source needs x86_64 to run!!\n");
	exit(EXIT_FAILURE);
#endif

	printf("runing with long_size = '%ld'\n", long_size);

	// get traced process pid, and attach
	traced_process = atoi(argv[1]);

	// attach process
	ptrace(PTRACE_SEIZE, traced_process, NULL, NULL);
	ptrace(PTRACE_INTERRUPT, traced_process, NULL, NULL);

	while (1) {
		wait(NULL);

		// get all regs and display
		ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
		fprintf(stderr, "syscall capturing, regs.orig_rax: '0x%lx'\n",
			(long)regs.orig_rax);

		printf("press ENTER\n");
		getchar();

		// check for SYS_write
		if (regs.orig_rax == SYS_write) {
			fprintf(stderr,
				"AWESOME: we got a SYS_write,...STOP the process!\n");

			// INJECT: get instructions
			get_data(traced_process, regs.rsi, backup, len);

			// INJECT: set breakpoint
			put_data(traced_process, regs.rip, code, len);

			// backup instructions
			fprintf(stderr, "AWESOME: captured data backup: '%s'\n",
				backup);

			break;
		}

		// continue if nothing interesting happened
		ptrace(PTRACE_SYSCALL, traced_process, 0, 0);
	}

	// let child continue
	ptrace(PTRACE_CONT, traced_process, NULL, NULL);

	// detach process again
	ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

	return 0;
}
