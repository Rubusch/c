/*
  ptrace example

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */

#include <stdlib.h>
#include <stdio.h>

int
main( int argc, char** argv )
{
	pid_t child;
	long orig_eax;
	if( 0 > (child=fork()) ){
		perror("fork failed");

	}else if( 0 == child ){
		ptrace( PTRACE_TRACEME, 0, NULL, NULL ); // ptrace, child
		execl( "/bin/pwd", "ls", NULL );
		puts("child: if we get here, something's wrong");

	}else{
		wait( NULL ); // wait call
		orig_eax=ptrace( PTRACE_PEEKUSER, child, 4*ORIG_EAX, NULL ); // ptrace, fetch data (parent)
		fprintf( stderr, "parent: systemcall %ld by child\n", orig_eax );
		ptrace( PTRACE_CONT, child, NULL, NULL ); // ptrace, let child continue (parent)
	}
	puts("parent: READY.");
	exit( EXIT_SUCCESS );
}

