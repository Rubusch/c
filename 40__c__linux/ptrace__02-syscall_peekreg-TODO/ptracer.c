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

FIXME: not working for 64 bit (so far)


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

      // for internal kernel purposes, the system call number is stored in
      // orig_rax rather than rax
      // all the other system call arguments are straightforward
      long syscall = regs.orig_rax;

      fprintf(stderr, "syscall: '%ld(%ld [rdi], %ld [rsi], %ld [rdx], %ld [r10], %ld [r8], %ld [r9])'\n", syscall, (long)regs.rdi, (long)regs.rsi, (long)regs.rdx, (long)regs.r10, (long)regs.r8, (long)regs.r9);

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

      fprintf(stderr, " = '%ld' [rax]\n", (long)regs.rax);
    }
  }
  return 0;
}
