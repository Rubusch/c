/*
  modified hello world from before...

  example prepared for code injection in my ptracer,
  the NASM (Net-ASM) assembler syntax was translated to GAS (GCC Assembler
  Syntax), for compiling it as inlined __asm__ source inside linux/c gcc


TODO rework this
  breakpoint will replace the following instruction:
         int    $0x80\n\

  after sleeping for 10 sec the rabbit.exe is going to enter its asm
  instructions where it falls into trap and cannot end only when ptracer.exe
  receives enter, it leaves the attached rabbit.exe

TODO check

  NOTE: rabbit.exe was stopped, w/o being a child, nor having declared
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
