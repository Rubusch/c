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

// TODO comments

const int long_size = sizeof( long );

void
getdata( pid_t child, long addr, char* str, int len )
{
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	} data;

	i = 0;
	j = len / long_size;
	laddr=str;
	while( i < j ){
		/* get data 'val' */
		data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL );

		/* copy over 'chars' */
		memcpy( laddr, data.chars, long_size );

		/* increment read index */
		++i;

		/* increment write position - move pointer */
		laddr+=long_size;
	}
// TODO
	j = len % long_size;
	if( j!=0 ){
		data.val = ptrace( PTRACE_PEEKDATA, child, addr + i * 4, NULL );
		memcpy( laddr, data.chars, j );
	}
	str[len] = '\0';
}

void
reverse( char *str )
{
	int i, j;
	char temp;
	for( i=0, j=strlen(str)-2; i<=j; ++i,--j){
		/* swap */
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
	}
//	str[strlen(str)-1] = "\n"; // TODO
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
		/* copy 'chars' */
		memcpy( data.chars, laddr, long_size );

		/* write to val */
		ptrace( PTRACE_POKEDATA, child, addr + i * 4, data.val );

		/* next to read */
		++i;

		/* next to write */
		laddr += long_size;
	}

	j = len % long_size;

	if( j != 0 ){
		/* copy value to 'chars' */
		memcpy( data.chars, laddr, j );

		/*  print */
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

					/* allocation */
					str = (char*) calloc( (params[2]+1), sizeof(char) );

					/* get data */
					getdata( child, params[1], str, params[2] );

					/* reverse */
					reverse( str );

					/* print data */
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
