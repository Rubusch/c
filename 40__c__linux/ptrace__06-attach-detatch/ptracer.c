/*
  attaching and detaching

  attaching (PTRACE_ATTACH) to some external process, a parent requests
  registers and instruction, prints it and, when done, detaches with
  PTRACE_DETACH

  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe)

  ptrace.exe prints the next instruction


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

  if (argc != 2) {
    printf("Usage: %s <pid to be traced: %s>\n", argv[0], argv[1]);
    printf("e.g.:\n$ ./rabbit.exe & %s $(pidof rabbit.exe)\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  traced_process = atoi(argv[1]);

  // attach to the child process
  ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);

  wait(NULL);

  // read out registers
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  // read out instruction
  ins = ptrace(PTRACE_PEEKTEXT, traced_process, regs.rip, NULL);
  fprintf(stderr, "RIP: process %d with instruction pointer '0x%08lx' [rip] executed instruction: '0x%08lx' [ins]\n", traced_process, (long)regs.rip, (long)ins);

  // detach
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  // clean up
  fprintf(stderr, "...killing the lab animal...\n");
  kill(traced_process, SIGSTOP);

  return 0;
}
