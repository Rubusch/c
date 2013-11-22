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
#include <sys/syscall.h>

#include <stdlib.h>
#include <stdio.h>

int
main( int argc, char** argv )
{
	
}
