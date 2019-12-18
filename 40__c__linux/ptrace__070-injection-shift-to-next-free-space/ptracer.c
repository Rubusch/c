/*
  inject "hello world" in free space



  attach to a running rabbit.exe process with PTRACE_SEIZE and
  PTRACE_INTERRUPT

  in the tracer obtain the current regs and instruction with PTRACE_GETREGS

  backup instruction pointer, rip

  it is best to store the instructions in free space, when injecting, so this
  version searches for "free space" to inject

  inject the current code to the address of the "free space"

  set the rip to the "free space" and update the registers of the tracee

  continue the tracee

  detach the tracee with PTRACE_DETACH


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
  char line[120];
  long addr = -1;
  char str[20];
  char *is_found = NULL;

  sprintf(filename, "/proc/%d/maps", pid);
  if (NULL == (fp = fopen(filename, "r"))) {
    exit(EXIT_FAILURE);
  }
  while (NULL != fgets(line, 120, fp)) {
    sscanf(line, "%lx-%hhx %s", (long unsigned int*) &addr, str, str);
//    fprintf(stderr, "XXX %s()[%d]:\tline '%s'\n", __func__, __LINE__, line);
    if (NULL != (is_found = strstr( line, " 00:00 "))) {
      break;
    }
  }
  fclose(fp);
//  fprintf(stderr, "XXX %s()[%d]:\tis_found '%s'\n", __func__, __LINE__, (is_found)?"true":"false");

  return (is_found) ? addr : -1;
}


int main(int argc, char *argv[])
{
  pid_t traced_process;
  struct user_regs_struct oldregs, regs;
  int len = 42;
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

  // get process's registers and *backup*
  ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
  memcpy(&oldregs, &regs, sizeof(regs));

  fprintf(stderr, "XXX %s()[%d]:\t'0x%016lx' - regs.rip (original next instruction)\n", __func__, __LINE__, (long)regs.rip);

  // find some free space for injection
  if (0 >= (addr = freespaceaddr(traced_process))) {
    perror("Aborting, no free space found!");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "XXX %s()[%d]:\t'0x%016lx' - addr (free address, heap)\n", __func__, __LINE__, (long)addr);

  // get current instructions and *backup*
  get_data(traced_process, regs.rip, backup, len);

  // modify and write registers back
  regs.rip = addr;
  ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);

  printf("The process stopped\n");
  printf("press ENTER\n");
  getchar();

  // let external process continue
  ptrace(PTRACE_CONT, traced_process, NULL, NULL);
  wait(NULL);

  // restore backup instructions and register state at addr (shifted address)
  fprintf(stderr, "Let's shift the next instruction to addr (0x%016lx) and let it continue there..\n", (long)addr);
  put_data(traced_process, addr, backup, len);
  ptrace(PTRACE_SETREGS, traced_process, NULL, &oldregs);

  // detach process
  ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
  sleep(3);
  puts("READY.");

  return 0;
}
