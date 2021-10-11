/*
  single step


  fork child with PTRACE_TRACEME

  let it execute a separate rabbit64.s process

  in the parent read out the RAX (EAX) register with PTRACE_PEEKUSER, aligned to 8

  if the RAX (EAX) register shows the syscall SYS_write (i.e. write to a stream),
  turn on (start flag) the single step PTRACE_SINGLESTEP which will stop/continue the child process

  read all syscall registers with PTRACE_GETREGS

  show the contents of the registers rdi, rsi, rdx, r10, r8, r9

  print EVERY step as ASM instruction of a simple hello world application


  ---


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
  * assembly by Jim Fisher (a snippet found somewhere on the web)
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	pid_t child;

	child = fork();
	if (child == 0) {
		// mark child PTRACE_TRACEME, and exec external program
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("./rabbit.exe", "rabbit.exe", NULL);

	} else {
		int status;
		struct user_regs_struct regs;
		long count = 0;
		long ins;

		while (1) {
#if __x86_64__
			// child still alive
			wait(&status);
			if (WIFEXITED(status))
				break;

			// display syscall
			switch (regs.orig_rax) {
#ifdef SYS_read
			case SYS_read:
				fprintf(stderr,
					"\tSYSCALL: SYS_read, '0x%04lx'\n",
					(long)regs.orig_rax);
				break;
#endif
#ifdef SYS_write
			case SYS_write:
				fprintf(stderr,
					"\tSYSCALL: SYS_write, '0x%04lx'\n",
					(long)regs.orig_rax);
				break;
#endif
#ifdef SYS_open
			case SYS_execve:
				fprintf(stderr,
					"\tSYSCALL: SYS_execve, '0x%04lx'\n",
					(long)regs.orig_rax);
				break;
#endif
#ifdef SYS_exit_group
			case SYS_exit_group:
				fprintf(stderr,
					"\tSYSCALL: SYS_exit_group, '0x%04lx'\n",
					(long)regs.orig_rax);
				break;
#endif
			default:
				fprintf(stderr,
					"\tSYSCALL: uncaught or no syscall\n");
			}

			// read out registers -> regs for instruction pointer
			ptrace(PTRACE_GETREGS, child, NULL, &regs);

			// get ins by regs.eip
			ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
			printf("%ld. RIP: 0x%08llx Instruction executed: 0x%016lx\n",
			       count, regs.rip, ins);

			// turn on PTRACE_SINGLESTEP
			ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);

			// increment line counter
			count++;
#else
			// child still alive
			wait(&status);
			if (WIFEXITED(status))
				break;

			// read out registers -> regs for instruction pointer
			ptrace(PTRACE_GETREGS, child, NULL, &regs);

			// get ins by regs.eip
			ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
			printf("%ld. EIP: 0x%08lx Instruction executed: 0x%08lx\n",
			       count, regs.eip, ins);

			// turn on PTRACE_SINGLESTEP
			ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);

			// increment line counter
			count++;
#endif
		}
	}

	exit(EXIT_SUCCESS);
}
