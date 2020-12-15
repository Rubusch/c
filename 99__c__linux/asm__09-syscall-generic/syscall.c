/*
  a wrapper function for a syscall on x86_64

  NOTE: syscalls on x86_64 take arguments in the following general purpose registers:
  rdi, rsi, rdx, r10, r8, r9


  system V ABI needs the arguments in the following registers:
  rdi, rsi, rdx, rcx, r8, r9


  author: L.Rubusch@gmx.ch

  resources
  https://wiki.osdev.org/System_V_ABI
  https://nullprogram.com/blog/2015/05/15/
  https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

#include <syscall.h>

#include <unistd.h>
// arch: x86, 64bit
#include <sys/reg.h> /* on 64 bit, use this and ORIG_RAX */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


long system_caller(long syscall_number, long rdi, long rsi, long rdx, long r10, long r8, long r9)
{
  long val;

  // convert params to 'register long' register values in asm
  register long r10_ __asm__ ("r10") = r10;
  register long r8_ __asm__ ("r8") = r8;
  register long r9_ __asm__ ("r9") = r9;

  __asm__ volatile ("syscall"
                    : "=a" (val)
                    : "a" (syscall_number), "D" (rdi), "S" (rsi), "d" (rdx), "r" (r10_), "r" (r8_), "r" (r9_)
                    : "rcx", "r11", "cc", "memory" );

    return val;
}



int main(int argc, char **argv)
{
  static const char msg[] = "Ipsum Lorum" "\n"; // to be sent via rsi

  // example: syscall "write", standard output,...
  // NOTE: SYS_write == 1, equally use '1' instead
  system_caller(SYS_write, 1, (long)msg, sizeof(msg), 0, 0, 0 );

  exit(EXIT_SUCCESS);
}
