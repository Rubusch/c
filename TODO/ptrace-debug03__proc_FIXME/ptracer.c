/*
  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Debugging with PTrace Utility, Jim Blakey jdblakey@innovative-as.com
 */

/* ------------------------------------------------------------------
** readmem.c
**
** This program is a quick example of using the /proc filesystem
** to access a process memory space. 
**
** This process will scan through all addresses on 1k boundaries
** looking for readable segments by lseek()ing through the /proc/PID/mem
** file. Once it finds a valid segment, it will read buffers and search
** for specific values
**
** On Non-Linux systems, it will then change the first character of the
** buffer to an 'A'
**
** To run:
**
**     readproc  PID  "string to search for "
**
** ------------------------------------------------------------------
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/signal.h>


#define D_LINUX 1


main(int argc, char *argv[]) 

{
  int Tpid;
  int pfd;
  char buff[1024];
  char *srchstr;
  char *eptr;
  unsigned int addr;
  int  goodaddr;
  int goodread;
  int srchlen;
  int j;

  /*
  ** Get arguments. Since this is an example program, I won't validate them
  */
  if ((argv[1] == NULL) || (argv[2] == NULL)) {
    printf("Need PID and string to search for\n");
    printf("usage: readmem PID string\n");
    exit(1);
  }
  
  Tpid = strtoul(argv[1], &eptr, 10);
  srchstr = (char *)strdup(argv[2]);
  srchlen = strlen(srchstr);
  printf("readproc: Tracing PID %d for string [%s] len %d\n",Tpid,
         srchstr, srchlen);
  /*
  ** In order to read /proc/PID/mem from this process, I have to have already
  ** stopped it via Ptrace(). (This is not documented anywhere, by the
  ** way). Anyway, this will leave the process in a STOPPED state. We'll
  ** start it again in a minute...
  */
  if ((ptrace(PTRACE_ATTACH, Tpid, 0, 0)) != 0) {
    printf("procexa: Attached to process %d\n",Tpid);
  }
  /*
  ** Create the string and open the proc mem file. Note under Linux this
  ** file is /proc/PID/mem while under Solaris this is /proc/PID/as
  ** Also, even though we open this RDWR, Linux will not allow us to write
  ** to it.
  */
  sprintf(buff,"/proc/%d/as", Tpid);
  printf("procexa:opening [%s]\n",buff);
  if ((pfd = open(buff,O_RDWR)) <= 0) {
    perror("Error opening /proc/PID/mem file");
    exit(2);
  }
  /*
  ** Start at zero, lseek and try to read. increment by 1024 bytes. If
  ** lseek returns a good status, then the address range is mapped. It
  ** should be readable. Print out start and end of valid mapped address
  ** ranges.
  */
  goodaddr = 0; goodread = 0;
  for (addr = 0; addr < (unsigned int)0xf0000000; addr += 1024) {
    if (lseek(pfd, addr, SEEK_SET) != addr) {
      if (goodaddr == 1) {
        printf("Address: %x RANGE END\n",addr);
        goodaddr = 0;
      } 
      continue;
    } else {
      if (goodaddr == 0) {
        printf("Address %x RANGE START\n",addr);
        goodaddr = 1;
      }
    }
    /*
    ** Read a 1k buffer and search it for the supplied text string
    */
    if (read(pfd, buff, 1024) <= 0) {
      if (goodread == 1) {
        printf("READ address %x RANGE END\n",addr);
        goodread = 0;
      }
      continue;
    } else {
      if (goodread == 0) {
        printf("READ address %x RANGE START\n",addr);
        goodread = 1;
      }
      for (j = 0; j < 1024; j++) {
        if (memcmp(&buff[j], srchstr, srchlen) == 0) {
          printf("*****Pattern found %x\n",
                 addr + j);
          buff[j] = 'A';
        }
      }
      /*
      ** If NOT Linux, then write the modified buffer back to the address 
      ** space.
      */
#ifndef D_LINUX
      lseek(pfd, addr, SEEK_SET);
      if (write(pfd, buff, 1024) <= 0) {
        printf("Nope on write\n");
      }
#endif
    }
  }
  printf("Stopped at address %x\n",addr);
}
