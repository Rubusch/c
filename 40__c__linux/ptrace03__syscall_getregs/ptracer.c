/*
  ptrace example

  tracking system calls - when a write syscall was caught in the child, ptrace
  reads out child's registers %ebx, %ecx and %edx one by one, else it prints
  the %eax register for other syscalls

  reading out registers one by one at syscall can be cumbersome, getregs fetches
  all registers at a single call


  email: L.Rubusch@gmx.ch

  resources: Linux Journal, Nov 30, 2002  By Pradeep Padala ppadala@cise.ufl.edu or p_padala@yahoo.com
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if __x86_64__
//#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
//#include <linux/ptrace.h>
#include <sys/reg.h>
#else
#include <asm/ptrace-abi.h> /* constants, e.g. ORIG_EAX, etc. */
#endif
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
#if __x86_64__
			/* check if ptrace stopped child, or if it exited */
			wait( &status );
			if( WIFEXITED(status) ){
				break;
			}

			orig_eax = ptrace( PTRACE_PEEKUSER, child, 4 * ORIG_RAX, NULL );
			if( orig_eax == SYS_write ){
				if( insyscall == 0 ){
					/* syscall entry */
					insyscall = 1;

					/* PTRACE: get a handle on all registers */
					ptrace( PTRACE_GETREGS, child, NULL, &regs );
					fprintf( stderr, "parent: write called with %llu, %llu, %llu\n", regs.rbx, regs.rcx, regs.rdx );
				}else{
					/* syscall exit */
					eax = ptrace( PTRACE_PEEKUSER, child, 4 * RAX, NULL );
					fprintf( stderr, "parent: write returned with %lu\n", eax );
					insyscall = 0;
				}
			}
			ptrace( PTRACE_SYSCALL, child, NULL, NULL );
#else
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
#endif
		}
	}
	return 0;
}
