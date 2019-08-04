/*
  ptrace example

  set up child with PTRACE_TRACEME, in parent then PTRACE_PEEK for the registers
  and PTRACE_CONT to continue


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#if __x86_64__
#include <sys/reg.h> /* on 64 bit, use this and ORIG_RAX */
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif

#include <stdlib.h>
#include <stdio.h>

int main( int argc, char** argv )
{
	pid_t child;
	long orig_eax;

	if ( 0 > (child=fork()) ) {
		perror("fork failed");

	} else if ( 0 == child ) {
		/* child */
		ptrace( PTRACE_TRACEME, 0, NULL, NULL ); // ptrace, child
		execl( "/bin/pwd", "ls", NULL );
		puts("child: if we get here, something's wrong");

	} else {
		/* parent */
		wait( NULL ); // wait call
#if __x86_64__
		orig_eax=ptrace( PTRACE_PEEKUSER, child, 4*ORIG_RAX, NULL ); // ptrace, fetch data (parent)
#else
		orig_eax=ptrace( PTRACE_PEEKUSER, child, 4*ORIG_EAX, NULL );
#endif
		fprintf( stderr, "parent: systemcall %ld by child\n", orig_eax );
		ptrace( PTRACE_CONT, child, NULL, NULL ); // ptrace, let child continue (parent)
	}
	puts("parent: READY.");
	exit( EXIT_SUCCESS );
}

