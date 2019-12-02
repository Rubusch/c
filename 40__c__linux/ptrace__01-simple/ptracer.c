/*
  ptrace example

  fork child with PTRACE_TRACEME, let it execute the "pwd" command

  in parent set then PTRACE_PEEK for the registers and PTRACE_CONT to continue


  email: L.Rubusch@gmx.ch

  resources
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
  long orig_eax;

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

    wait(NULL); // wait call
#if __x86_64__
//    orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_RAX, NULL); // ptrace, fetch data (parent)
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, child, 0, &regs);
    orig_eax = ptrace(PTRACE_PEEKTEXT, child, regs.rax, 0);
#else
    orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
#endif
    fprintf(stderr, "parent: child's systemcall '%lX' in EAX\n", orig_eax);
    ptrace(PTRACE_CONT, child, NULL, NULL); // ptrace, let child continue (parent)
  }
  puts("parent: READY.");

  exit(EXIT_SUCCESS);
}
