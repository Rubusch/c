/*
  ptrace example: "do something funny"

  take the output string of a child process (PTRACE_TRACEME), using
  PTRACE_PEEKDATA, revert it, and inject the reverted back into the child with
  PTRACE_POKEDATA

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
/*
  int idx, jdx;
  char temp;
  for (idx = 0, jdx = strlen(str)-2; idx <= jdx; ++idx, --jdx) { // FIXME
    temp = str[idx];
    str[idx] = str[jdx];
    str[jdx] = temp;
  }
/*/
  int len;
  int idx, jdx;
  char temp;
  for (idx = 0, jdx = strlen(str)-2; idx <= jdx; ++idx, --jdx) { // FIXME: why '2' and not '1'?
    temp = str[idx];
    fprintf(stderr, "%s[%d]:     temp '%c'\n", __func__, __LINE__, temp);
    str[idx] = str[jdx];
    str[jdx] = temp;
  }

  len = strlen(str);
  fprintf(stderr, "%s[%d]:     str '%s', len = %d\n", __func__, __LINE__, str, len);
// */

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
    long params[3];
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

          /* params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RBX, NULL); */
          /* params[1] = ptrace(PTRACE_PEEKUSER, child, 8 * RCX, NULL); */
          /* params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX, NULL); */

// FIXME: are the registers correct for x86_64? as arguments
          params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RBX, NULL);
          params[1] = ptrace(PTRACE_PEEKUSER, child, 8 * RCX, NULL);
          params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX, NULL);

          // allocation
          str = ( char * )calloc((params[2] + 1), sizeof(char));
          fprintf(stderr, "AAA str = '%s'\n", str);

          // get data
          get_data(child, params[1], str, params[2]);
          fprintf(stderr, "BBB str = '%s'\n", str);

          // reverse
          reverse(str);
          fprintf(stderr, "CCC str = '%s'\n", str);

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
fprintf(stderr, "111\n");   // XXX 
      ptrace(PTRACE_SYSCALL, child, NULL, NULL);
fprintf(stderr, "222\n");   // XXX 
    }
fprintf(stderr, "333\n");   // XXX 
  }
  exit(EXIT_SUCCESS);
}
