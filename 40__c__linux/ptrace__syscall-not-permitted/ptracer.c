/*
  ptrace example: x86_64 only!

  capture tracee's syscalls and answer them with EPERM (not permitted).


  email: L.Rubusch@gmx.ch

  resources:
  https://nullprogram.com/blog/2018/06/23/ by Christopher Wellons
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
    execl("/bin/pwd", "pwd", NULL);

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

      if (0 > (ptrace(PTRACE_SYSCALL, child, 0, 0))) {
        perror("ptrace: PTRACE_SYSCALL failed");
        exit(EXIT_FAILURE);
      }

      if (0 > waitpid(child, 0, 0)) {
        perror("waitpid: failed");
        exit(EXIT_FAILURE);
      }


      // set syscall to blocked by EPERM (permission denied)
      //
      // NOTE: the following error in the logs
      // pwd: error while loading shared libraries: libc.so.6: cannot open shared object file: Operation not permitted
      //
      // still the program will show some output
      // the error disappears when the following lines are commented out
      regs.rax = -EPERM;
      ptrace(PTRACE_SETREGS, child, 0, &regs);
    }
  }
  return 0;
}
