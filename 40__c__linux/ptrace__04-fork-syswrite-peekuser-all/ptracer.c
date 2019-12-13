/*
  ptrace example: "do something funny"

  take the output string of a child process (PTRACE_TRACEME), using
  PTRACE_PEEKDATA, revert it, and inject the reverted back into the child with
  PTRACE_POKEDATA

  the proggy forks of a 'pwd' shell command, it then catches a SYS_write call of
  the 'pwd' and kidnaps its argument (the current path) to be writen to stdout
  (rdi == 1); then it reverts the argument as string and re-injects
  (PTRACE_POKEDATA) the reverted string, which then goes to be displayed

  NOTE: the implementation is for x86_64 only!


  author: Lothar Rubusch, L.Rubusch@gmx.ch

  resources
  Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const int long_size = sizeof(long);


void get_data(pid_t child, long addr, char *str, int len)
{
  char *laddr;
  int idx, jdx;
  // data structure for reversing the string
  union u {
    long val;
    char chars[long_size];
  } data;

  idx = 0;
  jdx = len / long_size;
  laddr = str;
  while (idx < jdx) {
    // get 'val' for data
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + idx * 8, NULL);

    // copy over to 'chars'
    memcpy(laddr, data.chars, long_size);

    // increment read index
    ++idx;

    // increment write position - move pointer
    laddr += long_size;
  }

  // copy over remaining characters to 'chars'
  jdx = len % long_size;
  if (jdx != 0) {
    data.val = ptrace(PTRACE_PEEKDATA, child, addr + idx * 8, NULL);
    memcpy(laddr, data.chars, jdx);
  }
  str[len] = '\0';
}


void reverse(char *str)
{
  int idx, jdx;
  char temp;
  // NOTE: keep '-2' as upper bound, due to '\0' and '\n'!!
  for (idx = 0, jdx = strlen(str)-2; idx <= jdx; ++idx, --jdx) {
    temp = str[idx];
    str[idx] = str[jdx];
    str[jdx] = temp;
  }
}


void put_data(pid_t child, long addr, char *str, int len)
{
  char *laddr;
  int idx, jdx;
  union u {
    long val;
    char chars[long_size];
  } data;

  idx = 0;
  jdx = len / long_size;
  laddr = str;
  while (idx < jdx) {
    // copy 'chars'
    memcpy(data.chars, laddr, long_size);

    // write from 'val'
    ptrace(PTRACE_POKEDATA, child, addr + idx * 8, data.val);

    // next to read
    ++idx;

    // next to write
    laddr += long_size;
  }

  // remaining characters...
  jdx = len % long_size;
  if (jdx != 0) {
    // copy value to 'chars'
    memcpy(data.chars, laddr, jdx);

    //  print
    ptrace(PTRACE_POKEDATA, child, addr + idx * 8, NULL);
  }
}


int main(int argc, char **argv)
{
  pid_t child;
#ifndef __x86_64__
  fprintf(stderr, "Source needs x86_64 to run!!\n");
  exit(EXIT_FAILURE);
#endif

  if (0 > (child = fork())) {
    perror("fork() failed");

  } else if (0 == child) {
    /* tracee */
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl("/bin/pwd", "pwd", NULL);

  } else {
    /* tracer */
    long orig_eax;
    long params[6];
    int status;
    char *str;
    int toggle = 0;
    while (1) {
      // ptrace stopped child, or it exited?
      wait(&status);
      if (WIFEXITED(status)) {
        break;
      }

      orig_eax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
      if (SYS_write == orig_eax) {
        if (0 == toggle) {
          // turn peek-reverse-poke off
          toggle = 1;

          params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RDI, NULL);
          params[1] = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL);
          params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX, NULL);
          params[3] = ptrace(PTRACE_PEEKUSER, child, 8 * R10, NULL);
          params[4] = ptrace(PTRACE_PEEKUSER, child, 8 * R8, NULL);
          params[5] = ptrace(PTRACE_PEEKUSER, child, 8 * R9, NULL);

          // allocation
          str = (char*) calloc((params[2] + 1), sizeof(char));

          // get data
          get_data(child, params[1], str, params[2]);
          fprintf(stderr, "\tsyscall: 0x%lx( 0x%08lx [rdi], 0x%08lx [rsi], '%s' [rdx], 0x%08lx [r10], 0x%08lx [r8], 0x%08lx [r9]\n\n"
                  , (long)orig_eax, (long)params[0], (long)params[1], str, (long)params[3], (long)params[4], (long)params[5]);

          // reverse
          reverse(str);
          fprintf(stderr, "\treverting: str = '%s'\n\n", str);

          // print data
          put_data(child, params[1], str, params[2]);

          free(str);
          str = NULL;

        } else {
          // turn on peek-reverse-poke of this if-cluase
          toggle = 0;

          // append linefeed
          puts("");
        }
      }
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    }
  }
  puts("READY.");

  exit(EXIT_SUCCESS);
}
