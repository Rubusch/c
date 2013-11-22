/*
  ptrace example

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch

  using the user_regs_struct to handle register values directly,

  [example based on Linux Journal (Oct 31, 2002 By Pradeep Padala) ]
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#include <sys/syscall.h>
#include <sys/user.h>

#include <stdlib.h>
#include <stdio.h>

int
main( int argc, char** argv )
{
	pid_t child;
	long orig_eax, eax;
	int status;
	int insyscall = 0;
	struct user_regs_struct regs; /* registers */

	if(0 > (child = fork() )){
		perror( "fork() failed" );
	}else if( 0 == child ){
		/* child */
		ptrace( PTRACE_TRACEME, 0, NULL, NULL );
		execl( "/bin/pwd", "pwd", NULL );
	}else{
		while( 1 ){

			/* check if ptrace stopped child, or if it exited */
			wait( &status );
			if( WIFEXITED(status) ){
				break;
			}

			orig_eax = ptrace( PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL );
			if( orig_eax == SYS_write ){
				if( insyscall == 0 ){
					/* syscall entry */
					insyscall = 1;

					/* get registers */
					ptrace( PTRACE_GETREGS, child, NULL, &regs );
					fprintf( stderr, "parent: write called with %lu, %lu, %lu\n", regs.ebx, regs.ecx, regs.edx );
				}else{
					/* syscall exit */
					eax = ptrace( PTRACE_PEEKUSER, child, 4 * EAX, NULL );
					fprintf( stderr, "parent: write returned with %lu\n", eax );
					insyscall = 0;
				}
			}
			ptrace( PTRACE_SYSCALL, child, NULL, NULL );
		}
	}
	return 0;
}
