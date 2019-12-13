/*
  ptrace example

  fork child with PTRACE_TRACEME

  let it execute the a command, e.g. "/bin/pwd"

  in the parent set PTRACE_PEEK for the registers and PTRACE_CONT to continue

  display all instructions of the child process


  ---

  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
  * https://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
// arch: x86, 64bit
#include <sys/reg.h> /* on 64 bit, use this and ORIG_RAX */
#else
// arch: x86, 32bit (fallback)
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  pid_t child;
  long instr;

  if (0 > (child = fork())) {
    perror("fork failed");

  } else if (0 == child) {
    /* child: target */
    if (0 > ptrace(PTRACE_TRACEME, 0, 0, 0)) {
      perror("ptrace"); // ptrace, child
      exit(EXIT_FAILURE);
    }
    execl("/bin/pwd", "pwd", NULL);

  } else {
    /* parent: debugger */
    int status, cnt=0;
    wait(&status);
    while (WIFSTOPPED(status)) {
      cnt++;
      struct user_regs_struct regs;
      ptrace(PTRACE_GETREGS, child, 0, &regs);
#if __x86_64__
      instr = ptrace(PTRACE_PEEKTEXT, child, regs.rax, 0);
      fprintf(stderr, "%d. EIP = '0x%016llx', instr = '0x%016lx', RAX = '0x%016llx', RBX = '0x%016llx', RCX = '0x%016llx'\n", cnt, regs.rip, instr, regs.rax, regs.rbx, regs.rcx);
#else
      instr = ptrace(PTRACE_PEEKTEXT, child, regs.eax, 0);
      fprintf(stderr, "%d. EIP = '0x%08x', instr = '0x%08x', EAX = '0x%08x', EBX = '0x%08x', ECX = '0x%08x'\n", cnt, regs.eip, instr, regs.eax, regs.ebx, regs.ecx);
#endif

      if (0 > (ptrace(PTRACE_SINGLESTEP, child, 0, 0))) {
        perror("ptrace, single step");
        exit(EXIT_FAILURE);
      }
      wait(&status);
    }
  }
  puts("parent: READY.");

  exit(EXIT_SUCCESS);
}
