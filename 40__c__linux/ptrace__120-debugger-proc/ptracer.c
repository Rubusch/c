/*
  debugger examples

  (reworked copy of Jim Blakey's original whitepaper example and text)

  This program is a quick example of using the /proc filesystem
  to access a process memory space.

  This process will scan through all addresses on 1k boundaries
  looking for readable segments by lseek()ing through the /proc/PID/mem
  file. Once it finds a valid segment, it will read buffers and search
  for specific values

  On Non-Linux systems, it will then change the first character of the
  buffer to an 'A'

  to run:
  $ readproc <PID>  <"string to search for ">

  +-------------------+-------------------------------------------------------+
  | PTRACE_ATTACH     | This 'attaches' the parent process to the target PID. |
  |                   | In otherwords, the process with the supplied PID is   |
  |                   | stopped and becomes a child of the parent, allowing   |
  |                   | the parent access into its process space. The parent  |
  |                   | will be trapped on all child state changes. This      |
  |                   | doesn't work so well on Solaris                       |
  +-------------------+-------------------------------------------------------+
  | PTRACE_TRACEME    | This is a call from a target process to tell the      |
  |                   | parent to trace it. This is usually done after the    |
  |                   | parent fork(2)s the child and before the child        |
  |                   | exec(3)s the new image. The parent will then be able  |
  |                   | to trace the child from the start                     |
  +-------------------+-------------------------------------------------------+
  | PTRACE_SINGLESTEP | This places the target child in 'singlestep' mode.    |
  |                   | Since the target process is a child of the ptrace     |
  |                   | debugging process, the parent will get a child state  |
  |                   | changed trap (child changed to STOPPTED) that can be  |
  |                   | detected with the wait(2) call.                       |
  +-------------------+-------------------------------------------------------+
  | PTRACE_SYSCALL    | This will trap the parent process for each system call|
  |                   | (context change into kernel mode) made by the child.  |
  |                   | Appendix A describes the strace(2) utility which uses |
  |                   | this.                                                 |
  +-------------------+-------------------------------------------------------+
  | PTRACE_CONT       | This call continues execution of the STOPPED target   |
  |                   | process. If the target process stopped on a signal,   |
  |                   | the parent must deliver the signal on to the child    |
  |                   | through this call.                                    |
  +-------------------+-------------------------------------------------------+
  | PTRACE_PEEKTEXT   | Allows the program to read or write the word at addr  |
  | PTRACE_POKETEXT   | in the target's TEXT memory area.                     |
  +-------------------+-------------------------------------------------------+
  | PTRACE_PEEKUSER   | Read or write a word from the target process's USER   |
  | PTRACE_POKEUSER   | structure. This structure holds the registers, stack  |
  |                   | and text start address, context information, and      |
  |                   | other process information. See sys/user.h             |
  +-------------------+-------------------------------------------------------+
  | PTRACE_GETREGS    | Read/write a copy of the target's general purpose     |
  | PTRACE_SETREGS    | registers to/from the supplied location. This will be |
  |                   | OS and machine architecture specific.                 |
  +-------------------+-------------------------------------------------------+
  | PTRACE_GETFPREGS  | Read/write a copy of the target's floating point      |
  | PTRACE_SETFPREGS  | registers to/from the supplied location. This will be |
  |                   | OS and machine architecture specific.                 |
  +-------------------+-------------------------------------------------------+


  /proc

  +-------------------+-------------------------------------------------------+
  | /proc/PID/stat    | This file contains information about the current      |
  |                   | status of the process. This is the file the ps        |
  |                   | utility uses for its listing. Under Linux, this is an |
  |                   | ASCII printout. Under Solaris, this is a binary       |
  |                   | structure found in sys/procfs.h                       |
  +-------------------+-------------------------------------------------------+
  | /proc/PID/maps    | This file contains entries for each currently mapped  |
  |                   | memory region, its address offset and size, and the   |
  |                   | read/write/execute permission on it.                  |
  +-------------------+-------------------------------------------------------+
  | /proc/PID/cmdline | This is a copy of the command line that started the   |
  |                   | process.                                              |
  +-------------------+-------------------------------------------------------+
  | /proc/PID/fd      | This is a subdirectory containing one entry for each  |
  |                   | File Descriptor the process has open. The entries in  |
  |                   | this subdirectory are special 'links' to the actual   |
  |                   | files opened.                                         |
  +-------------------+-------------------------------------------------------+
  | /proc/PID/mem     | This is an access point into the memory space for the |
  |                   | target process. On Solaris, this is called            |
  |                   | /proc/PID/as                                          |
  +-------------------+-------------------------------------------------------+


  AUTHOR: Jim Blakey (jdblakey@innovative-as.com),
  reworked Lothar Rubusch, (L.Rubusch@gmx.ch)


  RESOURCES:
  * based on a true example, Jim Blakey (jdblakey@innovative-as.com),
  http://www.secretmango.com/jimb/Whitepapers/ptrace/ptrace.html
*/


#define POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE 1

#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define D_LINUX 1


int main(int argc, char *argv[])

{
  int tracee_pid;
  int pfd;
  char buff[1024];
  char *srchstr;
  char *eptr;
  unsigned int addr;
  int goodaddr;
  int goodread;
  int srchlen;

  // Get arguments. Since this is an example program, I won't validate them
  if (argv[1] == NULL || argv[2] == NULL) {
    fprintf(stderr, "Usage: %s <pid> <string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  tracee_pid = strtoul(argv[1], &eptr, 10);
  srchstr = ( char * )strdup(argv[2]);
  srchlen = strlen(srchstr);
  printf("readproc: Tracing PID %d for string [%s] len %d\n"
         , tracee_pid, srchstr, srchlen);

  // In order to read /proc/PID/mem from this process, I have to have already
  // stopped it via Ptrace(). (This is not documented anywhere, by the
  // way). Anyway, this will leave the process in a STOPPED state. We'll
  // start it again in a minute...
  if ((ptrace(PTRACE_ATTACH, tracee_pid, 0, 0)) != 0) {
    printf("procexa: Attached to process %d\n", tracee_pid);
  }

  // Create the string and open the proc mem file. Note under Linux this
  // file is /proc/PID/mem while under Solaris this is /proc/PID/as
  // Also, even though we open this RDWR, Linux will not allow us to write
  // to it.
  sprintf(buff, "/proc/%d/mem", tracee_pid);
  printf("procexa:opening [%s]\n", buff);
  if ((pfd = open(buff, O_RDWR)) <= 0) {
    perror("Error opening /proc/PID/mem file");
    exit(EXIT_FAILURE);
  }

  // Start at zero, lseek and try to read. increment by 1024 bytes. If
  // lseek returns a good status, then the address range is mapped. It
  // should be readable. Print out start and end of valid mapped address
  // ranges.
  goodaddr = 0;
  goodread = 0;
  for (addr = 0; addr < ( unsigned int )0xf0000000; addr += 1024) {
    if (lseek(pfd, addr, SEEK_SET) != addr) {
      if (goodaddr == 1) {
        printf("Address: %x RANGE END\n", addr);
        goodaddr = 0;
      }
      continue;
    } else {
      if (goodaddr == 0) {
        printf("Address %x RANGE START\n", addr);
        goodaddr = 1;
      }
    }

    // Read a 1k buffer and search it for the supplied text string
    if (read(pfd, buff, 1024) <= 0) {
      if (goodread == 1) {
        printf("READ address %x RANGE END\n", addr);
        goodread = 0;
      }
      continue;
    } else {
      if (goodread == 0) {
        printf("READ address %x RANGE START\n", addr);
        goodread = 1;
      }
      int jdx;
      for (jdx = 0; jdx < 1024; jdx++) {
        if (memcmp(&buff[jdx], srchstr, srchlen) == 0) {
          printf("*****Pattern found %x\n", addr + jdx);
          buff[jdx] = 'A';
        }
      }
    }
  }
  printf("Stopped at address %x\n", addr);
}
