/*
  setting breakpoints


  attach to a running rabbit.exe process with PTRACE_SEIZE and
  PTRACE_INTERRUPT

  in the parent obtain the current regs and instruction with PTRACE_GETREGS

  wait on ENTER to continue the rabbit.exe after any syscall via PTRACE_CONT

  backup instruction pointer rip

  replace the current instruction with the breakpoint, SIGTRAP 0x03

  stop at the breakpoint

  then re-inject a break point SIGTRAP signal, i.e. {0xcd, 0x80, 0xcc, 0}

  detach the rabbit.exe with PTRACE_DETACH


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
  * Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/


// kill()
#define _POSIX_SOURCE
#include <syscall.h>
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
  int idx, jdx;
  union u {
    long val;
    char chars[long_size];
  } data;

  idx = 0;
  jdx = len / long_size;
  laddr = str;
//  fprintf(stderr, "DEBUG %s(), [%d]: idx = %d, jdx = %d\n", __func__, __LINE__, idx, jdx);

  // version via PTRACE_PEEKDATA and alignment to 8 (due to x86_64)
  while (idx < jdx) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + (idx) * 8, NULL);
    memcpy(laddr, data.chars, long_size);
    ++idx;
    laddr += long_size;
  }

  jdx = len % long_size;
//  fprintf(stderr, "DEBUG %s(), [%d]: idx = %d, jdx = %d\n", __func__, __LINE__, idx, jdx);

  // since long_size will be 8, we always will fall into this condition for reading
  if (jdx != 0) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + idx * 8, NULL);
    memcpy(laddr, data.chars, jdx);
  }

  str[len] = '\0';
}


void put_data(pid_t child, long addr, unsigned char *str, int len)
{
  unsigned char *laddr;
  int idx = 0, jdx = len / long_size;
  union u {
    long val;
    char chars[long_size];
  } data;

  laddr = str;
  while (idx < jdx) {
    memcpy(data.chars, laddr, long_size);
    ptrace(PTRACE_POKETEXT, child, addr + idx * 8, data.val);
    ++idx;
    laddr += long_size;
  }
  jdx = len % long_size;

  // since long_size will be 8, we always will fall into this condition for writing back
  if (jdx != 0) {
    memcpy(data.chars, laddr, jdx);
    ptrace(PTRACE_POKETEXT, child, addr + idx * 8, data.val);
  }
}


int main(int argc, char **argv)
{
  pid_t traced_process;
  struct user_regs_struct regs;
  unsigned char code[] = { 0xcd, 0x80, 0xcc, 0, 0, 0, 0 }; // NOTE: the instruction only has 3 elements, though we align it to 8 due to x86_64
  int len = 8; // aligned to 8 for simplification
  unsigned char backup[len+1]; memset(backup, '\0', len+1);

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
  ptrace(PTRACE_SEIZE, traced_process, NULL, NULL);
  ptrace(PTRACE_INTERRUPT, traced_process, NULL, NULL);
  wait(NULL);

  // INJECT: get all regs and display
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  // INJECT: backup original next instructions at instruction pointer (rip)
  get_data(traced_process, regs.rip, backup, len);
  fprintf(stderr, "captured next %d instructions (backup):\n", len);
  int cnt;
  for (int cnt=0; cnt<len; ++cnt) fprintf(stderr, "'0x%02x' ", backup[cnt]);
  fprintf(stderr, "\n");

  // INJECT: put breakpoint as next instruction
  // NOTE: remove this put_data() and the message "Hello World!" will appear before "press ENTER"
  put_data(traced_process, regs.rip, code, len);

  // INJECT: let the process continue
  ptrace(PTRACE_CONT, traced_process, NULL, NULL);
  wait(NULL);

  fprintf(stderr, "AWESOME: we got a BREAKPOINT - the process stopped!\n");
  printf("press ENTER\n");
  getchar();

  // RESET: put the original code back to the instruction pointer
  // NOTE: remove this put_data() and it will fall into "Trace/breakpoint trap   ./rabbit.exe"
  put_data(traced_process, regs.rip, backup, len);

  // RESET: resetting the regs to their orig config (before breakpoint)
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  // detach process again
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  return 0;
}
