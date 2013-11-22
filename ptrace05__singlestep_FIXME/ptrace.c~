/*
  ptrace example

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch

  single step through the code

  listing for dummy1.s written in assembly language and compiled as
  gcc -o dummy1 dummy1.s:


.data
hello:
    .string "hello world\n"
.globl  main
main:
    movl    $4, %eax
    movl    $2, %ebx
    movl    $hello, %ecx
    movl    $12, %edx
    int     $0x80
    movl    $1, %eax
    xorl    %ebx, %ebx
    int     $0x80
    ret


  [example based on Linux Journal (Oct 31, 2002 By Pradeep Padala) ]
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

