/*
  single step

  monitor a child process (PTRACE_TRACEME), watchout if a 'write' system call
  (SYS_write) was made; if so, turn on (start flag) the single step
  (PTRACE_SINGLESTEP) which will stop/continue the child process; here, read out
  the instruction and instruction pointer (PTRACE_PEEKTEXT) from the registers
  (PTRACE_GETREGS) of the child

  the instructions, will be HEX code in the binary (ASM) address space of the
  child, thus it's not possible to print them in a different way, than either a
  hexadecimal code, or the offset to the start address of main
# TODO check    

  listing for rabbit.s written in assembly language and compiled as
  gcc -o rabbit.exe rabbit.s


  a possible child may be the following rabbit.s

  .data
  hello:
              .string "hello world\n"
  .globl      main
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
  original: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
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
				ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
				printf("EIP: %lx Instruction executed: %lx\n", regs.eip, ins);
			}

			/* start step-by-step when a write syscall was made */
			if (regs.orig_eax == SYS_write) {
				start = 1;
				/* turn on PTRACE_SINGLESTEP */
				ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
			} else {
				/* else: check for syscalls, PTRACE_SYSCALL */
				ptrace(PTRACE_SYSCALL, child, NULL, NULL);
			}
		}
	}

        exit(EXIT_SUCCESS);
}
