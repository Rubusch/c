/*
  single step (x86_64, and x86 32bit part kept from Linux Journal article)

  monitor a child process (PTRACE_TRACEME), watchout if a 'write' system call
  (SYS_write) was made; if so, turn on (start flag) the single step
  (PTRACE_SINGLESTEP) which will stop/continue the child process; here, read out
  the instruction and instruction pointer (PTRACE_PEEKTEXT) from the registers
  (PTRACE_GETREGS) of the child

  the instructions, will be HEX code in the binary (ASM) address space of the
  child, thus it's not possible to print them in a different way, than either a
  hexadecimal code, or the offset to the start address of main

  listing for rabbit.s written in assembly language and compiled as
  $ gcc -o rabbit.exe rabbit.s

  run the example (ptracer.exe will fork and run the rabbit.exe)
  $ ./ptracer.exe


  author: Lothar Rubusch, L.Rubusch@gmx.ch
  GPLv3

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
    // start external program, and set PTRACE_TRACEME, and exec external program
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("./rabbit.exe", "rabbit.exe", NULL);

  } else {
    int status;
    struct user_regs_struct regs;
    int start = 0;
    long ins;
    char* str;

    while (1) {
#if __x86_64__
      // child still alive
      wait(&status);
      if (WIFEXITED(status))
        break;

      // read out registers -> regs for instruction pointer
      ptrace(PTRACE_GETREGS, child, NULL, &regs);

      // when start - fetch executed instruction by PTRACE_PEEKTEXT
      if (start == 1) {
        // get ins by regs.eip
        ins = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);

        // read out string argument from program
        str = (char*) calloc((regs.rdx+1), sizeof(char)); // size of string is in rdx
        memset(str, '\0', (regs.rdx+1));
        int idx=0;
        for (idx=0; idx< (regs.rdx+1); ++idx) {
          str[idx] = ptrace(PTRACE_PEEKTEXT, child, regs.rsi+idx, NULL); // start address of string is in rsi
        }
        str[regs.rdx-1] = '\0'; // display cosmetics: replace \n by \0

        // display
        fprintf(stderr, "RIP: 0x%lx Instruction executed: 0x%04lx [ins], '%s' [string], 0x%04lx [rsi, start address of string], %lld [rdx, size of string]\n"
                , (long)regs.rip, (long)ins, str, (long)regs.rsi , regs.rdx);

        // free memory
        free(str);
        str=NULL;
      }

      // start step-by-step when a write syscall was made
      if (regs.orig_rax == SYS_write) {
        start = 1;
        // turn on PTRACE_SINGLESTEP
        ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
      } else {
        // else: check for syscalls, PTRACE_SYSCALL
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);
      }
#else
      // child still alive
      wait(&status);
      if (WIFEXITED(status))
        break;

      // read out registers -> regs for instruction pointer
      ptrace(PTRACE_GETREGS, child, NULL, &regs);

      // when start - fetch executed instruction by PTRACE_PEEKTEXT
      if (start == 1) {
        // get ins by regs.eip
        ins = ptrace(PTRACE_PEEKTEXT, child, regs.eip, NULL);
        printf("EIP: %lx Instruction executed: %lx\n", regs.eip, ins);
      }

      // start step-by-step when a write syscall was made
      if (regs.orig_eax == SYS_write) {
        start = 1;
        // turn on PTRACE_SINGLESTEP
        ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
      } else {
        // else: check for syscalls, PTRACE_SYSCALL
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);
      }
#endif
    }
  }

  exit(EXIT_SUCCESS);
}
