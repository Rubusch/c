// select.c
/*
  select example of the manpage
//*/


#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


/*
  watch stdin (fd 0) to see when it has input, wait up to five seconds
  then don't rely on the value of "tv"
//*/
int main(int argc, char** argv)
{
  puts("select example - either press <ENTER> to give some\ninput on the observed readfds or let it timeout in 5sec");

  // 0. define a set
  fd_set rset;
  struct timeval tv;
  int retval;

  // 1. zero the set
  FD_ZERO(&rset);

  // 2. set up a new set, with user inputstream
  FD_SET(0, &rset);

  tv.tv_sec = 5; // secs and..
  tv.tv_usec = 0; // ..msecs to wait

  /*
    3. call select(nfds, readfds, writefds, exceptfds, timeout)

    nfds      - highest-numbered file descriptor in any of the three sets
    readfds   - read filedescriptor (will be watched for reading) 
    writefds  - write filedescriptor (pointer on write stream)
    exceptfds - exception filedescriptor (pointer on exception stream)
    timeout   - an upper timeout until select returns
  */
  retval = select(1, &rset, NULL, NULL, &tv);


  if(-1 == retval){
    perror("select");
  }else if(retval){
    puts("data is available");
    if(FD_ISSET(0, &rset)){
      puts("FD_ISSET(0, &rset) is true now");
    }

  }else{
    puts("no data within five seconds");

  }
  
  exit(0);
}

