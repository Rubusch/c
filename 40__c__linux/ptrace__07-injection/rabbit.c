/*
  fabricated hello world

  example prepared for code injection for a ptracer, breakpoint will replace the
following instruction:
         int    $0x80\n\

  for running the example, compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`

  after sleeping for 10 sec the rabbit.exe is going to enter its asm
instructions where it falls into trap and cannot end only when ptracer.exe
receives enter, it leaves the attached rabbit.exe

TODO check
  Note: rabbit.exe was stopped, w/o being a child, nor having declared
PTRACE_TRACEME explicitely!

 */

#include <unistd.h>

int main()
{
  sleep(10);

  __asm__("\n \
  mov rax, 1 \n \
  mov rdi, 1 \n \
  mov rsi, msg \n \
  mov rdx, msglen \n \
  syscall \n \
  \n \
  mov rax, 60 \n \
  mov rdi, 0 \n \
  syscall \n \
  \n \
section .rodata \n \
  msg: db \"Hello, world!\", 10 \n \
  msglen: equ $ - msg \n \
  ");

  return 0;
}
