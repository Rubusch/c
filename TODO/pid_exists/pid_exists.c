// pid_exists.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <signal.h>
#include <errno.h>

/**
 * 
 * 
 * @param pid 
 * 
 * @return -1 in case of error, 0 in case of doesn't exist and >0 in case it exists
 */
int
pid_exists( pid_t pid )
{
  int ret;
  if( pid < 0 ) return 0;

  // waitpid( pid, 0, WNOHANG ); // for child processes

  ret = kill( pid, 0 );
  if( -1 == ret ){
    if( errno == ESRCH ){ // the pid does not exist
      return 0;
    }
    // else, "kill" could send a signal to pid, but we have no permissions
    //    puts("exists, but no permissions!");
    return pid;
  }else if( 0 == ret ){ // "kill" could send a signal to pid
    return pid;
  }else{ // undefined, impossible
    return -1;
  }
}


int main( int argc, char** argv )
{
  int res=0;
  if( argc != 2 ) exit( EXIT_FAILURE );
  res = pid_exists( atoi( argv[1] ) );
  fprintf( stdout, "pid \"%s\" %s.\n", argv[1]
           , ( (res == 0) ? "does not exist" 
               : ((res > 0) ? "exists" : "ERROR" ) ) );
  
  exit( EXIT_SUCCESS );
}
