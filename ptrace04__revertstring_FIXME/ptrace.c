/*
  ptrace example

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch


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
#include <string.h>

// FIXME, comments

const int long_size = sizeof( long );

void
reverse( char *str )
{
	int i, j;
	char temp;
	for( i=0, j=strlen(str)-2; i<=j; ++i,--j){
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	}
}

void
getdata( pid_t child, long addr, char* str, int len )
{
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	} data;

	i=0;
	j=len/long_size;
	laddr=str;
	while( i < j ){
		data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL );
		memcpy( laddr, data.chars, long_size );
		++i;
		laddr+=long_size;
	}
	j=len%long_size;
	if(j!=0){
		data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL );
		memcpy( laddr, data.chars, j );
	}
	str[len] = '\0';
}

void
putdata( pid_t child, long addr, char* str, int len )
{
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	} data;

	i=0;
	j=len/long_size;
	laddr = str;
	while( i < j ){
		memcpy( data.chars, laddr, long_size );
		ptrace( PTRACE_POKEDATA, child, addr + i * 4, data.val );
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if( j != 0 ){
		memcpy( data.chars, laddr, j );
		ptrace( PTRACE_POKEDATA, child, addr + i * 4, NULL );
	}
}


int
main( int argc, char** argv )
{
	pid_t child;

	if( 0 > (child = fork()) ){
		perror( "fork() failed" );
	}else if( 0 == child ){
		ptrace( PTRACE_TRACEME, 0, NULL, NULL );
		execl( "/bin/pwd", "pwd", NULL );
	}else{
		long orig_eax;
		long params[3];
		int status;
		char *str;
		int toggle = 0;
		while( 1 ){

			/* ptrace stopped child, or it exited? */
			wait( &status );
			if( WIFEXITED( status ) ){
				break;
			}

			orig_eax = ptrace( PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL );
			if( SYS_write == orig_eax ){
				if( 0 == toggle ){
					toggle=1;
					params[0] = ptrace( PTRACE_PEEKUSER, child, 4 * EBX, NULL );
					params[1] = ptrace( PTRACE_PEEKUSER, child, 4 * ECX, NULL );
					params[2] = ptrace( PTRACE_PEEKUSER, child, 4 * EDX, NULL );

					str = (char*) calloc( (params[2]+1), sizeof(char) );  

					getdata( child, params[1], str, params[2] );
					reverse( str );
					putdata( child, params[1], str, params[2] );
				}else{
					toggle=0;
				}
			}
			ptrace( PTRACE_SYSCALL, child, NULL, NULL );
		}
	}
	exit( EXIT_SUCCESS );
}
