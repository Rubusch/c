/*
  single step (x86_64, and x86 32bit part kept from Linux Journal article)

  print EVERY step as ASM instruction of a simple hello world application



  author: Lothar Rubusch, L.Rubusch@gmx.ch

  resources:
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

      // read out registers -> regs for instruction pointer
      ptrace(PTRACE_GETREGS, child, NULL, &regs);

      // get ins by regs.eip
      ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
      printf("%ld. RIP: %llx Instruction executed: %lx\n", count, regs.rip,
             ins);

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
      printf("%ld. EIP: %lx Instruction executed: %lx\n", count, regs.eip, ins);

      // turn on PTRACE_SINGLESTEP
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);

      // increment line counter
      count++;
#endif
    }
  }

  exit(EXIT_SUCCESS);
}
