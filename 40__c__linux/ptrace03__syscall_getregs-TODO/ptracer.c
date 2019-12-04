/*
  ptrace example

  tracking system calls - when a write syscall was caught in the child, ptrace
  reads out child's registers %ebx, %ecx and %edx one by one, else it prints
  the %eax register for other syscalls

  reading out registers at syscall WRITE, then printing return value (eax / rax)

  NOTE: for x86_64 it needs to be aligned by 8, for x86 needs to be aligned by 4


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
//#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
//#include <linux/ptrace.h>
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  pid_t child;
  long orig_eax, eax;
  int status;
  int insyscall = 0;
  struct user_regs_struct regs; /* registers */

  if (0 > (child = fork())) {
    perror("fork() failed");
  } else if (0 == child) {
    /* tracee */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/bin/pwd", "pwd", NULL);
  } else {
    /* tracer */
    while (1) {
      // check if ptrace stopped child, or if it exited
      wait(&status);
      if (WIFEXITED(status)) {
        break;
      }

#if __x86_64__
      orig_eax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
#else
      orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
#endif
      if (orig_eax == SYS_write) {
        if (insyscall == 0) {
          // syscall entry
          insyscall = 1;

          // PTRACE: get a handle on all registers
          ptrace(PTRACE_GETREGS, child, NULL, &regs);

#if __x86_64__ // 64 bit
          fprintf(stderr, "parent: write called with 0x%08llx [rbx], 0x%08llx [rcx], 0x%08llx [rdx]\n", regs.rbx, regs.rcx, regs.rdx);
#else          // 32 bit
          fprintf(stderr, "parent: write called with 0x%04lx [ebx], 0x%04lx [ecx], 0x%04lx [edx]\n", regs.ebx, regs.ecx, regs.edx);
#endif

        } else {
          // syscall exit

#if __x86_64__ // 64 bit
          eax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
          fprintf(stderr, "parent: write returned with 0x%08lx [rax]\n", eax);
#else          // 32 bit
          eax = ptrace(PTRACE_PEEKUSER, child, 4 * EAX, NULL);
          fprintf(stderr, "parent: write returned with 0x%04lx [eax]\n", eax);
#endif
          insyscall = 0;
        }
      }
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    }
  }
  return 0;
}
