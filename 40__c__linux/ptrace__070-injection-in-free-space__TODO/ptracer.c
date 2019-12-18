/*
  inject "hello world" in free space



  attach to a running rabbit.exe process with PTRACE_SEIZE and
  PTRACE_INTERRUPT

  in the tracer obtain the current regs and instruction with PTRACE_GETREGS

  backup instruction pointer, rip

  it is best to store the instructions in free space, when injecting, so this
  version searches for "free space" to inject the instructions to

  replace the current instruction with some assembled (via gdb) sequence of
  another program which ends with a breakpoint SIGTRAP signal,

  continue the tracee rabbit.exe

  detach the rabbit.exe with PTRACE_DETACH

  rabbit.exe will now print "Hello XXX World" and end in a SIGTRAP

  stop at the breakpoint


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

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int long_size = sizeof(long);

void get_data(pid_t child, long addr, unsigned char *str, int len)
{
  unsigned char *laddr;
  int i, j;
  union u {
    long val;
    unsigned char chars[long_size];
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


void put_data(pid_t child, long addr, unsigned char *str, int len)
{
  unsigned char *laddr;
  int i = 0, j = len / long_size;
  union u {
    long val;
    unsigned char chars[long_size];
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


/*
  function to find free space for code injection via map from /proc filesystem
 */
long freespaceaddr(pid_t pid)
{
  FILE *fp;
  char filename[30];
  char line[85];
  long addr;
  unsigned char str[20];

  sprintf(filename, "/proc/%d/maps", pid);
  fp = fopen(filename, "r");
  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }
  while (fgets(line, 85, fp) != NULL) {
    sscanf(line, "%lx-%hhx %s", (long unsigned int*) &addr, str, str);
    if (strcmp((char*)str, "00:00") == 0) {
      break;
    }
  }
  fclose(fp);
  return addr;
}


int main(int argc, char *argv[])
{
  pid_t traced_process;
  struct user_regs_struct oldregs, regs;
  int len = 41;
  unsigned char insertcode[] = { 0xeb, 0x19, 0x5e, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc7, 0x02, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc2, 0x12, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xcc, 0xe8, 0xe2, 0xff, 0xff, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x58, 0x58, 0x58, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x0a, 0x00 };
  int len = sizeof(insertcode);
  unsigned char backup[len];
  long addr;

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

  // attach external process
  ptrace(PTRACE_SEIZE, traced_process, NULL, NULL);
  ptrace(PTRACE_INTERRUPT, traced_process, NULL, NULL);
  wait(NULL);

  // get process's registers
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);

  // find some free space for injection
  addr = freespaceaddr(traced_process);

  // get current instructions
  get_data(traced_process, addr, backup, len);

  // inject new instructions
  put_data(traced_process, addr, insertcode, len);

  memcpy(&oldregs, &regs, sizeof(regs));
  regs.rip = addr;

  // write registers back
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  // let external process continue
  ptrace(PTRACE_CONT, traced_process, NULL, NULL);

  wait(NULL);
  printf("The process stopped, Putting back the original instructions\n");

  // restore backuped instructions
  put_data(traced_process, addr, backup, len);

  // set registers
  ptrace(PTRACE_SETREGS, traced_process, NULL, &oldregs);

  printf("Letting it continue with original flow\n");

  // detach process
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);

  return 0;
}
