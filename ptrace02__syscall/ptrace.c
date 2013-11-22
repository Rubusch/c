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
#include <sys/syscall.h> /* SYS_write */

#include <stdlib.h>
#include <stdio.h>

int
main( int argc, char** argv)
{
	pid_t child;
	long orig_eax, eax;
	long params[3];
	int status;
	int insyscall = 0;

	if( 0 > (child = fork()) ){
		perror( "fork() failed" );
	}else if( 0 == child ){
		/* child */
		ptrace( PTRACE_TRACEME, 0, NULL, NULL);
		execl( "/bin/pwd", "pwd", NULL );
	}else{
		/* parent */

		while( 1 ){

			/* check wether the child was stopped by ptrace or exited */
			wait( &status );
			if( WIFEXITED(status) ){
				break;
			}

			orig_eax = ptrace( PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL );

                        /*
			  tracking the 'write' syscall
			*/
			if( orig_eax == SYS_write ){
				if( insyscall == 0 ){
					/* syscall entry

					   PTRACE_PEEKUSER looks into the arguments of the child
					 */
					insyscall = 1;
					params[0] = ptrace( PTRACE_PEEKUSER, child, 4 * EBX, NULL );
					params[1] = ptrace( PTRACE_PEEKUSER, child, 4 * ECX, NULL );
					params[2] = ptrace( PTRACE_PEEKUSER, child, 4 * EDX, NULL );
					fprintf( stderr, "parent: write called with %lu, %lu, %lu\n", params[0], params[1], params[2]);
				}else{
					/* syscall exit */
					eax = ptrace( PTRACE_PEEKUSER, child, 4 * EAX, NULL );
					fprintf( stderr, "parent: write returned with %lu\n", eax );
					insyscall = 0;
				}
			}

			/*
			  stop the child process whenever a syscall entry/exit was received
			*/
			ptrace( PTRACE_SYSCALL, child, NULL, NULL );
		}
	}
	return 0;
}

