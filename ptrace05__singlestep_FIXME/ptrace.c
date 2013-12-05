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


  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Oct 31, 2002  By Pradeep Padala
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

int
main( int argc, char** argv )
{
pid_t child;
const int long_size = sizeof(long);
child = fork();
if(child == 0) {
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execl("./dummy1", "dummy1", NULL);
 }
 else {
   int status;
   union u {
     long val;
     char chars[long_size];
   }data;
   struct user_regs_struct regs;
   int start = 0;
   long ins;
   while(1) {
     wait(&status);
     if(WIFEXITED(status))
       break;
     ptrace(PTRACE_GETREGS,
            child, NULL, &regs);
     if(start == 1) {
       ins = ptrace(PTRACE_PEEKTEXT,
                    child, regs.eip,
                    NULL);
       printf("EIP: %lx Instruction "
              "executed: %lx\n",
              regs.eip, ins);
     }
     if(regs.orig_eax == SYS_write) {
       start = 1;
       ptrace(PTRACE_SINGLESTEP, child,
              NULL, NULL);
     }
     else
       ptrace(PTRACE_SYSCALL, child,
              NULL, NULL);
   }
 }
return 0;
}
