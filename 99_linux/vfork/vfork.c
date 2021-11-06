// vfork.c
/*
  vfork() is DEPRECATED and shouldn't be used - use pthreads/fork instead!

  #include <sys/types.h>
  #include <unistd.h>
  pid_t vfork();

  Create a child process and block the parent.

  Works similar to fork(), but it does not make a copy of the address space. The
  memory is shared reducing the overhead of spawning a new process with a unique
  copy of all memory.
  This is typically used when using fork() to exec() a process and terminate.
  The vfork() function also executes the child process first and resumes the
  parent process when the child terminates. The child terminates when the parent
  terminates! The variables are shared between parent and child using vfork!

  Returns 0 to the parent and the pid of the child to the child, if ok, else
  returns -1.

  Pitfall:
  Deadlock: if the child doesn't terminate, the parent process will not proceed
  as well. _exit() terminates the current process again.
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

extern pid_t vfork();

#define IDENTIFIER_SIZE 80
#define CHILD_TXT "child process - "
#define PARENT_TXT "parent process - "

int varGlobal = 2;

int main()
{
	int varStack = 20;
	pid_t pid = 0, my_pid = getpid(); // pid of PARENT
	char *identifier = NULL;
	if (NULL ==
	    (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))) {
		perror("calloc() failed");
		exit(1);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);

	/*
    START HERE
  //*/

	if (0 > (pid = vfork())) {
		// vfork failed
		perror("fork() failed");
		exit(1); // use "big" exit()

	} else if (pid == 0) {
		// executed by CHILD process
		strncpy(identifier, CHILD_TXT, strlen(CHILD_TXT) + 1);
		++varGlobal;
		++varStack;
		printf("%i %s varGlobal: %i (2), varStack: %i (20)\r\n", my_pid,
		       identifier, varGlobal, varStack);
		_exit(0); // exit only the "blocking" child

	} else {
		// executed by PARENT process
		identifier = PARENT_TXT;
	}

	// executed by BOTH
	printf("%i %s varGlobal: %i (2), varStack: %i (20)\r\n", my_pid,
	       identifier, varGlobal, varStack);

	// free memory! exit() doesn't do, thus use pthreads!
	exit(0);
}
