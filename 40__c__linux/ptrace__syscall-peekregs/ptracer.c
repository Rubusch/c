/*
  ptrace example: read registers on x86_64!

  ptrace is a system call found in Unix and several Unix-like operating systems.
  By using ptrace (the name is an abbreviation of "process trace") one process
  can control another, enabling the controller to inspect and manipulate the
  internal state of its target. ptrace is used by debuggers and other
  code-analysis tools, mostly as aids to software development.
  (wikipedia)

  tracking system calls - when a write syscall was caught in the child, ptrace
  reads out child's registers %ebx, %ecx and %edx one by one, else it prints
  the %eax register for other syscalls

  system call numbers can be found in /usr/include/asm/unistd.h and unistd_64.h,
  respectively


  email: L.Rubusch@gmx.ch

  resources:
  https://nullprogram.com/blog/2018/06/23/ by Christopher Wellons
  Linux Journal, Nov 30, 2002 by Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  pid_t child;

#ifndef __x86_64__
  fprintf(stderror, "ABORTING! programmed for x86_64 only!!!\n");
  exit(EXIT_FAILURE);
#endif

  if (0 > (child = fork())) {
    perror("fork() failed");

  } else if (0 == child) {
    /* child: tracee */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/bin/ls", "ls", NULL);

  } else {
    /* parent: tracer */
    waitpid(child, 0, 0);
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);

    while (1) {
      // restart the stopped tracee

/*
  read out registers
 */

      if (0 > (ptrace(PTRACE_SYSCALL, child, 0, 0))) {
        perror("ptrace: PTRACE_SYSCALL failed");
        exit(EXIT_FAILURE);
      }

      if (0 > waitpid(child, 0, 0)) {
        perror("waitpid: failed");
        exit(EXIT_FAILURE);
      }

      struct user_regs_struct regs;
      if (0 > ptrace(PTRACE_GETREGS, child, 0, &regs)) {
        perror("ptrace: PTRACE_GETREGS failed");
        exit(EXIT_FAILURE);
      }

      // rax will be overwritten with the return value of the syscall,
      // thus orig_rax contains the initial rax value
      long syscall = regs.orig_rax;

      fprintf(stderr
              , "syscall: '%lx(%016lx [rdi], %016lx [rsi], %016lx [rdx], %016lx [r10], %016lx [r8], %016lx [r9])'\n"
              , syscall
              , (long)regs.rdi
              , (long)regs.rsi
              , (long)regs.rdx
              , (long)regs.r10
              , (long)regs.r8
              , (long)regs.r9);

/*
  read out result
 */

      // execute systemcall and stop on exit
      if (0 > ptrace(PTRACE_SYSCALL, child, 0, 0)) {
        perror("ptrace: second PTRACE_SYSCALL failed");
        exit(EXIT_FAILURE);
      }

      if (0 > waitpid(child, 0, 0)) {
        perror("waitpid: second waitpid failed");
        exit(EXIT_FAILURE);
      }

      // get syscall results (copied from Christopher Wellons
      if (0 > ptrace(PTRACE_GETREGS, child, 0, &regs)) {
        fputs(" = ?\n", stderr);
        if (errno == ESRCH) { // ESRCH: No such process
          exit(regs.rdi); // syscall was 'exit' or similar
        }
        perror("ptrace: final PTRACE_GETREGS failed");
      }

      fprintf(stderr, " = '%016lx' [rax]\n\n", (long)regs.rax);
    }
  }
  return 0;
}
