/*
  debugging with ptrace


  Implements a watchpoint on a supplied memory location. This will
  let you know when this address gets overwritten.

  ---


  NOTE: error code handling of ptrace() calls are removed to make it more
  readable, for productive code consider error code evaluation

  if (0 > ptrace(...)) {
      // free resources
      perror("something failed");
      exit(EXIT_FAILURE);
  }


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * based on a true example, Jim Blakey (jdblakey@innovative-as.com),
  http://www.secretmango.com/jimb/Whitepapers/ptrace/ptrace.html
*/


#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>
#include <wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_OFFSETOF(STRUCT, ELEMENT)                                            \
  ( long unsigned int )&(( STRUCT * )NULL)->ELEMENT;

int main(int argc, char *argv[])
{
  pid_t tracee_pid, stat, res;
  long pres;
  int signo;
  int ipoffs, spoffs;
  unsigned int memcontents = 0, startcontents = 0, watchaddr = 0;

  if (argv[1] == NULL || argv[2] == NULL) {
    fprintf(stderr, "Usage: %s <pid> <watchpoint addr>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  tracee_pid = strtoul(argv[1], NULL, 10);
  watchaddr = strtoul(argv[2], NULL, 16);
  printf("tracing pid %d. checking for change to 0x%016lx \n", tracee_pid, (long)watchaddr);

  // get the offset into the user area of the IP and SP registers. We'll need this later
  ipoffs = M_OFFSETOF(struct user, regs.rip);
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - ipoffs\n", __func__, __LINE__, ipoffs);

  spoffs = M_OFFSETOF(struct user, regs.rsp);
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - spoffs\n", __func__, __LINE__, spoffs);

  // Attach to the process. This will cause the target process to become
  // the child of this process. The target will be sent a SIGSTOP. call
  // wait(2) after this to detect the child state change. We're expecting
  // the new child state to be STOPPED
  printf("Attaching to process %d\n", tracee_pid);
  if (0 != (pres = ptrace(PTRACE_ATTACH, tracee_pid, 0, 0))) {
    fprintf(stderr, "FAILED! attach result %lx\n", pres);
  }

  if ((tracee_pid != (res = waitpid(tracee_pid, &stat, WUNTRACED))) || !(WIFSTOPPED(stat))) {
    fprintf(stderr, "FAILED! unexpected wait result res %d stat %x\n", res, stat);
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "DEBUG %s()[%d]:\twait result stat 0x%016x, pid %d\n", __func__, __LINE__, stat, res);
  stat = 0;
  signo = 0;

  // Get the starting value at the requested watch location. The PTRACE_PEEKEXT
  // option allows you to reach into the tartet process address space, using
  // its relocation maps, and read/change values
  startcontents = ptrace(PTRACE_PEEKTEXT, tracee_pid, watchaddr, 0);
  fprintf(stderr, "DEBUG %s()[%d]:\tstarting value at 0x%016lx, is 0x%016lx\n", __func__, __LINE__, (long)watchaddr, (long)startcontents);

  sleep(1);

  while (1) {
    // Ok, now we will continue the child, but set the single step bit in
    // the psw. This will cause the child to exeute just one instruction and
    // trap us again. The wait(2) catches the trap.
    if (0 > (pres = ptrace(PTRACE_SINGLESTEP, tracee_pid, 0, signo))) {
      perror("Ptrace singlestep error");
      exit(EXIT_FAILURE);
    }

    res = wait(&stat);
    // The previous call to wait(2) returned the child's signal number.
    // If this is a SIGTRAP, then we set it to zero (this does not get
    // passed on to the child when we PTRACE_CONT or PTRACE_SINGLESTEP
    // it).
    if ((signo = WSTOPSIG(stat)) == SIGTRAP) {
      fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - signo (SIGTRAP)\n", __func__, __LINE__, signo);
      signo = 0;
    }

    // If it is the SIGHUP, then PTRACE_CONT the child and we can exit.
    if ((signo == SIGHUP) || (signo == SIGINT)) {
      ptrace(PTRACE_CONT, tracee_pid, 0, signo);
      fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - signo (SIGHUP or SIGINT). Child took a SIGHUP or SIGINT. We are done\n", __func__, __LINE__, signo);
      break;
    }

    printf("press ENTER\n");
    getchar();

    // get the current value from the watched address and see if it is
    // different from the starting value. If so, then get the instruction
    // pointer from the target's user area, 'cause this is the instruction
    // that changed the value!
    memcontents = ptrace(PTRACE_PEEKTEXT, tracee_pid, watchaddr, 0);
    if (memcontents != startcontents) {
      printf("!!!!! Watchpoint address changed !!!!!\n");
      printf("Instruction that changed it at 0x%016lx\n", ptrace(PTRACE_PEEKUSER, tracee_pid, ipoffs, 0));
      printf("New contents of address 0x%x\n", memcontents);
      break;
    }
  }
  printf("Debugging complete\n");
  return (0);
}
