// wait.c - part 1
/*
  #include <sys/wait.h>
  pid_t wait(int* status);

  Wait for child process termination. Makes a parent wait until a child process
  is completed. BLOCKS calling process until the child terminates. If the parent
  has various child processes, wait() returns when one returns.

  status: the termination status of the process, can be NULL.
  Returns pid of terminated child on success, else -1
//*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define PARENT_TXT "parent - "
#define CHILD_TXT "\tchild - "
#define IDENTIFIER_SIZE 10


int main()
{
	char* identifier=NULL;
	if(NULL == (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))){
		perror("calloc() failed");
		exit(1);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);
	pid_t pid=0, pid_parent=getpid();

	// fork
	puts("fork()");
	if(0 > (pid = fork())){
		perror("fork() failed");
		exit(1);

	}else if(pid == 0){
		// child process
		strncpy(identifier, CHILD_TXT, strlen(CHILD_TXT));

		// child code
		printf("%schild pid: %i, parent: %i\r\n", identifier, getpid(), pid_parent);
		printf("%ssleeps\r\n", identifier);
		sleep(5);
		printf("%sawakes\r\n", identifier);
		printf("%sdone\r\n", identifier);
		exit(0);

	}else{
		// parent process
		strncpy(identifier, PARENT_TXT, strlen(PARENT_TXT));

		printf("%schild pid: %i, parent: %i\r\n", identifier, pid, getpid());
		printf("%swaiting on pid %i\r\n", identifier, pid);

		/*
		  1. child exit status
		//*/
		int childExitStatus=0;

		/*
		  2. wait on all childs
		//*/
		while(0 == wait(&childExitStatus))
			;
		printf("%swaiting done\r\n", identifier);

		printf("%sdone\r\n", identifier);
		exit(0);
	}
}

