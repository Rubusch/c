/*
  Attaching and Detaching

  attaching (PTRACE_ATTACH) to some external process, a parent requests
  registers and instruction, prints it and, when done, detaches with
  PTRACE_DETACH

  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe)



  Manpage:


  Attaching and Detaching

  A thread can be attached to the tracer using

  the call

  ptrace(PTRACE_ATTACH, pid, 0, 0);

  or

  ptrace(PTRACE_SEIZE, pid, 0, PTRACE_O_flags);

  PTRACE_ATTACH sends SIGSTOP to this thread. If the tracer wants this SIGSTOP
  to have no effect, it needs to suppress it. Note that if other signals are
  concurrently sent to this thread during attach, the tracer may see the tracee
  enter signal-delivery-stop with other signal(s) first! The usual practice is
  to reinject these signals until SIGSTOP is seen, then suppress SIGSTOP
  injection. The design bug here is that a ptrace attach and a concurrently
  delivered SIGSTOP may race and the concurrent SIGSTOP may be lost.

  Since attaching sends SIGSTOP and the tracer usually suppresses it, this may
  cause a stray EINTR return from the currently executing system call in the
  tracee, as described in the "Signal injection and suppression" section.

  Since Linux 3.4, PTRACE_SEIZE can be used instead of PTRACE_ATTACH.
  PTRACE_SEIZE does not stop the attached process. If you need to stop it after
  attach (or at any other time) without sending it any signals, use
  PTRACE_INTERRUPT command.

  The request

  ptrace(PTRACE_TRACEME, 0, 0, 0);

  turns the calling thread into a tracee. The thread continues to run (doesn't
  enter ptrace-stop). A common practice is to follow the PTRACE_TRACEME with

  raise(SIGSTOP);

  and allow the parent (which is our tracer now) to observe our
  signal-delivery-stop.

  If the PTRACE_O_TRACEFORK, PTRACE_O_TRACEVFORK, or PTRACE_O_TRACECLONE options
  are in effect, then children created by, respectively, vfork(2) or clone(2)
  with the CLONE_VFORK flag, fork(2) or clone(2) with the exit signal set to
  SIGCHLD, and other kinds of clone(2), are automatically attached to the same
  tracer which traced their parent. SIGSTOP is delivered to the children,
  causing them to enter signal-delivery-stop after they exit the system call
  which created them.




  Detaching of the tracee is performed by:

  ptrace(PTRACE_DETACH, pid, 0, sig);

  PTRACE_DETACH is a restarting operation; therefore it requires the tracee to
  be in ptrace-stop. If the tracee is in signal-delivery-stop, a signal can be
  injected. Otherwise, the sig parameter may be silently ignored.

  If the tracee is running when the tracer wants to detach it, the usual
  solution is to send SIGSTOP (using tgkill(2), to make sure it goes to the
  correct thread), wait for the tracee to stop in signal-delivery-stop for
  SIGSTOP and then detach it (suppressing SIGSTOP injection). A design bug is
  that this can race with concurrent SIGSTOPs. Another complication is that the
  tracee may enter other ptrace-stops and needs to be restarted and waited for
  again, until SIGSTOP is seen. Yet another complication is to be sure that the
  tracee is not already ptrace-stopped, because no signal delivery happens while
  it is - not even SIGSTOP.

  If the tracer dies, all tracees are automatically detached and restarted,
  unless they were in group-stop. Handling of restart from group-stop is
  currently buggy, but the "as planned" behavior is to leave tracee stopped and
  waiting for SIGCONT. If the tracee is restarted from signal-delivery-stop, the
  pending signal is injected.


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
  or p_padala@yahoo.com
*/

// kill()
#define _POSIX_SOURCE
#include <signal.h>

// ptrace(), kill,..
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
  pid_t traced_process;
  struct user_regs_struct regs;
  long ins;

#ifndef __x86_64__
  fprintf(stderr, "Source needs x86_64 to run!!\n");
  exit(EXIT_FAILURE);
#endif

  if (argc < 2) {
    printf("Usage: %s <pid to be traced: %s>\n", argv[0], argv[1]);
    printf("e.g.:\n$ ./rabbit.exe & %s $(pidof rabbit.exe)\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  traced_process = atoi(argv[1]);

  // attach to the child process
/*
  // old version
  ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
/*/
  // somehow in manpage described "new" process
  ptrace(PTRACE_SEIZE, traced_process, NULL, NULL);
  ptrace(PTRACE_INTERRUPT, traced_process, NULL, NULL);
// */

  wait(NULL);

  // read out registers
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  // read out instruction
  ins = ptrace(PTRACE_PEEKTEXT, traced_process, regs.rip, NULL);

  // read argument
  fprintf(stderr, "RIP: process %d with instruction pointer '0x%08lx' [rip]: '0x%08lx' [ins]\n", traced_process, (long)regs.rip, (long)ins);

  switch(regs.orig_rax) {
#ifdef SYS_nanosleep
  case SYS_nanosleep:
    fprintf(stderr, "\tSYSCALL: SYS_nanosleep, '0x%08lx'\n", (long)regs.orig_rax);
    break;
#endif
  default:
    fprintf(stderr, "\tSYSCALL: uncaught, '0x%08lx' XXXXXXX\n", (long)regs.orig_rax);
  }

  // detach
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  // clean up
  fprintf(stderr, "...killing the lab animal...\n");
  kill(traced_process, SIGKILL);

  return 0;
}
