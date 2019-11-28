/*
  example of single stepping through code using ptrace

  FIXME: this code snipped is somehow broken, rather a SNIPPET, applying some
  alternative writings

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch

  resource: Linux Gazette, http://linuxgazette.net/issue81/sandeep.html,
  by Sandeep S, sk_nellayi@rediffmail.com
*/

#include <stdio.h>
#include <stdlib.h>

//#include <signal.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syscall.h>
#include <unistd.h>

//#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
//#include <sys/syscall.h>
// TODO is there a way to get the syscall in text by hex code of instruction?
#include <sys/user.h> /* struct user_regs_struct */

#include <errno.h>


int main(void)
{
  struct user_regs_struct regs; /* read out registers */
  long long counter = 0;        /*  machine instruction counter */
  long ins;                     /* current instruction */

  int status; /*  child's return value        */
  int pid;    /*  child's process id          */

  puts("Please wait");

  switch (pid = fork()) {
  case -1:
    perror("fork");
    break;
  case 0: /*  child process starts */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    /*
     *  must be called in order to allow the
     *  control over the child process
     */
    execl("/bin/ls", "ls", NULL);
    //		execl("/opt/git_c/ptrace__singlestep01/rabbit.exe", "rabbit.exe",
    //NULL);
    /*
     *  executes the program and causes
     *  the child to stop and send a signal
     *  to the parent, the parent can now
     *  switch to PTRACE_SINGLESTEP
     */
    break; /* never reached */

  default: /*  parent process starts */
    wait(&status);
    if (WIFEXITED(status)) {
      break;
    }
    /*
     *   parent waits for child to stop at next
     *   instruction (execl())
     */
    // TODO check val 1407
    while (status == 1407) {
      counter++;
      //*
      // TODO explain the behavior, what does this PTRACE_SINGLESTEP exactly?
      // PTRACE_SINGLESTEP stops and resumes one step
      if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0) {
        perror("ptrace");
      }
      //*/
      // TODO ok, while wait status is 1407, increment a counter - what does the
      // PTRACE_SINGLESTEP actually, is this _really_ singlestepping through
      // instructions? the following will always reutrn just 0xfffffffff -
      // invalid
      /*
                              ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                              if (ins != 0xffffffff) {
                                      ins = ptrace(PTRACE_PEEKTEXT, pid,
      regs.eip, NULL); fprintf( stderr, "EIP: %lx instruction executed: %lx\n",
      regs.eip, ins);
                              }
      //*/
      /*
       *   switch to singlestep tracing and
       *   release child
       *   if unable call error.
       */

      wait(&status);
      /*   wait for next instruction to complete  */
    }
    /*
     * continue to stop, wait and release until
     * the child is finished; status != 1407
     * Low=0177L and High=05 (SIGTRAP)
     */
    printf("Number of machine instructions : %lld\n", counter);
  }

  exit(EXIT_SUCCESS);
}
