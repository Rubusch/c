/*
  debugging with ptrace


  show instruction pointer and stack pointer of another program in infinite loop

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

#include <syscall.h>

#define M_OFFSETOF(STRUCT, ELEMENT)                                            \
  (unsigned long int) &( (STRUCT*) NULL)->ELEMENT;

// used for filtering (turned off)
#define D_LINUXNONUSRCONTEXT 0x40000000

int main(int argc, char *argv[])
{
  pid_t tracee_pid, stat, res;
  long pres;
  long syscall;
  int signo;
  int ip, sp;
  int ipoffs, spoffs;
  int initialSP = -1;
  int initialIP = -1;

  // this program needs the PID of the target process as argument
  if (argv[1] == NULL) {
    fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  tracee_pid = strtoul(argv[1], NULL, 10);
  printf("tracing pid %d \n", tracee_pid);

  // get the offset into the user area of the IP and SP registers
  // get regs contents by dereferencing elements in global static "struct user"
  ipoffs = (unsigned long int) &( (struct user*) NULL)->regs.rip;
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - ipoffs\n", __func__, __LINE__, ipoffs);

  // ...or do it as macro
  spoffs = M_OFFSETOF(struct user, regs.rsp);
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - ipoffs\n", __func__, __LINE__, spoffs);

  // Attach to the process. This will cause the target process to become
  // the child of this process. The target will be sent a SIGSTOP. call
  // wait(2) after this to detect the child state change. We're expecting
  // the new child state to be STOPPED

  printf("Attaching to process %d\n", tracee_pid);
// TODO return value fixed
  //	if ((ptrace(PTRACE_ATTACH, tracee_pid, 0, 0)) != 0) {
  //		printf("Attach result %d\n",res);
  if (0 != (pres = ptrace(PTRACE_ATTACH, tracee_pid, 0, 0))) {
    printf("Attach result %lx\n", pres);
  }

  if ((tracee_pid != (res = waitpid(tracee_pid, &stat, WUNTRACED))) || !(WIFSTOPPED(stat))) {
    printf("Unexpected wait result res %d stat %x\n", res, stat);
    exit(EXIT_FAILURE);
  }

  printf("Wait result stat %x pid %d\n", stat, res);
  stat = 0;
  signo = 0;

  // Loop now, tracing the child
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
      fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - signo (SIGHUP or SIGINT)\n", __func__, __LINE__, signo);
      printf("Child took a SIGHUP or SIGINT. We are done\n");
      break;
    }

    printf("press ENTER\n");
    getchar();

    // Fetch the current IP and SP from the child's user area. Log them.
    // Get IP and SP either by taking the value by a corresponding offset
    // (as here), or get them directly as shown below. There can be a
    // difference in precision.
    ip = ptrace(PTRACE_PEEKUSER, tracee_pid, ipoffs, 0);
    sp = ptrace(PTRACE_PEEKUSER, tracee_pid, spoffs, 0);

    // Check to see where we are in the process. Using the ldd(1) utility, I
    // dumped the list of shared libraries that were required by this process.
    // This showed:
    //
    //     libc.so.6 => /lib/i686/libc.so.6 (0x40030000)
    //     /lib/ld-linux.so.2 => /lib/ld-linux.so.2 (0x40000000)
    //
    // So basically, we can assume that any execuable address pointed to by
    // the IP that is *over* 0x40000000 is either in ld.so, libc.so, or in
    // some sort of kernel state. We really don't care about these addresses
    // so we'll skip 'em. Also, nm(1) showed that all the local symbols
    // we would be interested in start in the 0x08000000 range.
    //
    // NOTE: this "filtering" is just an upgrade, depending if the addresses
    // are still valid for contemporary x86_64
    // NOTE: "over" is performed by a binary and operation
/*
    if (ip & D_LINUXNONUSRCONTEXT) {
      fprintf(stderr, "SKIPPED IP 0x%016lx\n", (long)ip);
      continue;
    }
// */

    if (initialIP == -1) {
      initialIP = ip;
      initialSP = sp;
      fprintf(stderr, "---- Starting LOOP IP %x SP %x ---- \n", initialIP, initialSP);
    } else {
      if ((ip == initialIP) && (sp == initialSP)) {
        ptrace(PTRACE_CONT, tracee_pid, 0, signo);
        fprintf(stderr, "----- LOOP COMPLETE -----\n");
        break;
      }
    }
    syscall = ptrace(PTRACE_PEEKUSER, tracee_pid, 8 * ORIG_RAX, 0);
    fprintf(stderr, "Stat 0x%x IP 0x%x [0x%016lx] SP x0%x [0x%016lx]  Last syscall: '%s'\n"
           , stat
           , ip
           , ptrace(PTRACE_PEEKUSER, tracee_pid, 8 * RIP, 0)
           , sp
           , ptrace(PTRACE_PEEKUSER, tracee_pid, 8 * RSP, 0)
           , syscall == SYS_restart_syscall ? "SYS_restart_syscall" : (syscall == 0xffffffffffffffff ? "unknown" : "SYS_something...")
            );
    // If we're back to where we started tracing the loop, then exit
  }
  printf("Debugging complete\n");

  exit(EXIT_SUCCESS);
}
