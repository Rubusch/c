/*
  single step through the instructions of an external program

  listing for rabbit.s written in assembly language and compiled as
  gcc -o rabbit.exe rabbit.s


.data
hello:
    .string "hello world\n"
.globl  main
main:
    movl    $4, %eax
    movl    $2, %ebx
    movl    $hello, %ecx
    movl    $12, %edx
    int     $0x80
    movl    $1, %eax
    xorl    %ebx, %ebx
    int     $0x80
    ret

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main( int argc, char** argv )
{
	pid_t child;

	child = fork();
	if (child == 0) {
		/* mark child PTRACE_TRACEME, and exec external program */
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("./rabbit.exe", "rabbit.exe", NULL);

	} else {
		int status;
		struct user_regs_struct regs;
		int start = 0;
		long ins;
		while(1) {
			/* child still alive */
			wait(&status);
			if (WIFEXITED(status)) {
				break;
			}

			/* read out registers -> regs for instruction pointer */
			ptrace(PTRACE_GETREGS, child, NULL, &regs);

			/* when start - fetch executed instruction by PTRACE_PEEKTEXT */
			if (start == 1) {
				/* get ins by regs.eip */
				ins = ptrace(PTRACE_PEEKTEXT,child,regs.eip,NULL);
				printf("EIP: %lx Instruction executed: %lx\n",regs.eip,ins);
			}

			/* start step-by-step at write syscall */
			if (regs.orig_eax == SYS_write) {
				start = 1;
				/* turn on PTRACE_SINGLESTEP */
				ptrace(PTRACE_SINGLESTEP,child,NULL,NULL);
			} else {
				/* for other syscall PTRACE_SYSCALL */
				ptrace(PTRACE_SYSCALL,child,NULL,NULL);
			}
		}
	}
	return 0;
}
