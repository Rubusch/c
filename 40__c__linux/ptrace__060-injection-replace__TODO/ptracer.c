/*
  inject "hello world"

  attach an external process by declaring PTRACE_ATTACH;

  request its instruction (PTRACE_PEEKDATA) from process's registers
  (PTRACE_GETREGS), backup instructions and inject (PTRACE_POKEDATA) a "hello
  world" sequence as hex;

  write to instruction (PTRACE_POKEDATA) and set registers (PTRACE_SETREGS)

  continue the process (PTRACE_CONT) and let it run into the trap

  in the tracer wait, then replace the injected "hello world" in the process by
  the former, backuped, code (PTRACE_POKEDATA), and inject it into process's
  registers (PTRACE_SETREGS)

  when done, detaches with PTRACE_DETACH


  usage: compile it (Makefile) and in one shell window run
  $ ./rabbit.exe

  in another shell window run, within 10 sec
  $ ./ptracer.exe `pidof rabbit.exe`

  the result should inject code to print "Hello world" in the controlled process


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu
  or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int long_size = sizeof(long);

void get_data(pid_t child, long addr, char *str, int len)
{
  char *laddr;
  int i, j;
  union u {
    long val;
    char chars[long_size];
  } data;
  i = 0;
  j = len / long_size;
  laddr = str;
  while (i < j) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 8, NULL);
    memcpy(laddr, data.chars, long_size);
    ++i;
    laddr += long_size;
  }
  j = len % long_size;
  if (j != 0) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 8, NULL);
    memcpy(laddr, data.chars, j);
  }
  str[len] = '\0';
}


void put_data(pid_t child, long addr, char *str, int len)
{
  char *laddr;
  int i = 0, j = len / long_size;
  union u {
    long val;
    char chars[long_size];
  } data;
  laddr = str;
  while (i < j) {
    memcpy(data.chars, laddr, long_size);
    ptrace(PTRACE_POKEDATA, child, addr + i * 8, data.val);
    ++i;
    laddr += long_size;
  }
  j = len % long_size;
  if (j != 0) {
    memcpy(data.chars, laddr, j);
    ptrace(PTRACE_POKEDATA, child, addr + i * 8, data.val);
  }
}


int main(int argc, char *argv[])
{
#ifndef __x86_64__
  fprintf(stderr, "Source needs x86_64 to run!!\n");
  exit(EXIT_FAILURE);
#endif

  pid_t traced_process;
  struct user_regs_struct regs;

  char insertcode[] = { 0xeb, 0x19, 0x5e, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc7, 0x02, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2, 0x12, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xcc, 0xe8, 0xe2, 0xff, 0xff, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x58, 0x58, 0x58, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x0a, 0x00 };

  int len = sizeof(insertcode);

  fprintf(stderr, "%s()[%d]: sizeof insertcode: '%d'\n", __func__, __LINE__, len);
  char backup[len];

  if (argc != 2) {
    printf("Usage: %s <pid to be traced> \n", argv[0]);
    printf("  for running the example, compile it (Makefile) and in one shell "
           "window run\n");
    printf("  $ ./rabbit.exe\n");
    printf("\n");
    printf("  in another shell window run, within 10 sec\n");
    printf("  $ ./ptracer.exe `pidof rabbit.exe`\n");

    exit(EXIT_FAILURE);
  }

  traced_process = atoi(argv[1]);

  // attach process
  ptrace(PTRACE_SEIZE, traced_process, NULL, NULL);
  ptrace(PTRACE_INTERRUPT, traced_process, NULL, NULL);
  wait(NULL);

  // get registers
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  // backup instructions
  get_data(traced_process, regs.rip, backup, len);

  // inject new instructions
  put_data(traced_process, regs.rip, insertcode, len);

  // restore registers
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  printf("press ENTER\n");
  getchar();

  // continue process
  ptrace(PTRACE_CONT, traced_process, NULL, NULL);


  kill(traced_process, SIGINT);
  wait(NULL);


  printf("The process stopped, rabbit will now continue as a rabbit-xxx.exe\n");

// NOTE: to reset it to the regular behavior, inject the backup instructions and reset the registers
//  put_data(traced_process, regs.rip, backup, len);
//  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  // detach process
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  return 0;
}
