/*
  setting breakpoints

  attach an external process by declaring PTRACE_ATTACH;

  request its instruction (PTRACE_PEEKDATA) from process's registers
  (PTRACE_GETREGS), backup instruction and inject (PTRACE_POKEDATA) a trap;

  let the process continue (PTRACE_CONT) and let it run into the trap

  in the tracer wait for "hit ENTER" - when enter is hit, replace the injected
  trap in the process by the former, backuped code (PTRACE_POKEDATA), and inject
  it into process's registers (PTRACE_SETREGS)

  when done, detaches with PTRACE_DETACH


  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
  or p_padala@yahoo.com
*/

// FIXME: segmentation fault at finish

// kill()
#define _POSIX_SOURCE
#include <signal.h>

// ptrace()
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define long_size sizeof(long)

void get_data(pid_t child, long addr, unsigned char *str, int len)
{
  unsigned char *laddr;
  int i, j;
  union u {
    long val;
    char chars[long_size];
  } data;
  i = 0;
  j = len / long_size;
  laddr = str;
  while (i < j) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
    memcpy(laddr, data.chars, long_size);
    ++i;
    laddr += long_size;
  }
  j = len % long_size;
  // since long_size will be 4, we always will fall into this condition for reading
  if (j != 0) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
    memcpy(laddr, data.chars, j);
  }
  str[len] = '\0';
}


void put_data(pid_t child, long addr, unsigned char *str, int len)
{
  unsigned char *laddr;
  int i = 0, j = len / long_size;
  union u {
    long val;
    char chars[long_size];
  } data;
  laddr = str;
  while (i < j) {
    memcpy(data.chars, laddr, long_size);
    ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
    ++i;
    laddr += long_size;
  }
  j = len % long_size;

  // since long_size will be 4, we always will fall into this condition for writing back
  if (j != 0) {
    memcpy(data.chars, laddr, j);
    ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
  }
}

int main(int argc, char **argv)
{
  pid_t traced_process;
  struct user_regs_struct regs;
  int len = 4;
  unsigned char code[] = {0xcd, 0x80, 0xcc, 0}; /*  int 0x80, int3 */
  unsigned char backup[4];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pid to be traced> \n", argv[0]);
    fprintf(stderr, "$ ./rabbit.exe & ./ptracer.exe $(pidof rabbit.exe)\n");
    exit(EXIT_FAILURE);
  }

#ifndef __x86_64__
  fprintf(stderr, "Source needs x86_64 to run!!\n");
  exit(EXIT_FAILURE);
#endif

  printf("runing with long_size = '%ld'\n", long_size);

  // get traced process pid, and attach
  traced_process = atoi(argv[1]);

  // attach process
  ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
  wait(NULL);

  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  /* backup instructions */
// TODO check if len is correct?
  get_data(traced_process, regs.rip, backup, len);

  /* put trap into child */
// TODO check if len is correct?
  put_data(traced_process, regs.rip, code, len);

// TODO no PTRACE_POKEDATA, then PTRACE_SETREGS needed here?
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);


  /* let child continue, run into trap, and execute int3 instruction */
  ptrace(PTRACE_CONT, traced_process, NULL, NULL);


  /* why does this work? */
  kill(traced_process, SIGINT);
  wait(NULL);

  printf("the process stopped, restoring the original instructions\n");
// TODO actually the process does not stop if not attached successfully - test
  // with external window, probably not controllable since it is neither a
  // child, nor declares PTRACE_TRACEME
  printf("press ENTER\n");
  getchar();

  /* restore the backuped code */
  put_data(traced_process, regs.rip, backup, len);

  /* setting the eip back to the original instruction, and let process continue
   */
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  return 0;
}
