// kill.c
/*
  Send SIGKILL (not only SIGTERM!) to a process group and kill it definitly

  #include <signal.h>

  int killpg(pid_t prgrp, int signal)
      send signal to a process group
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

extern int killpg(pid_t, int);
extern pid_t getpgid(pid_t);

#define CHILD_TXT "\tchild - "
#define PARENT_TXT "parent - "
#define IDENTIFIER_SIZE 10

int main()
{
	pid_t pid = 0;
	char *identifier = NULL;
	if (NULL ==
	    (identifier = calloc(IDENTIFIER_SIZE, sizeof(*identifier)))) {
		perror("calloc() failed");
		exit(1);
	}
	memset(identifier, '\0', IDENTIFIER_SIZE);

	// fork()
	if (0 > (pid = fork())) {
		perror("fork() failed");
		exit(1);

	} else if (0 == pid) {
		// child code
		strncpy(identifier, CHILD_TXT, IDENTIFIER_SIZE);
		printf("%spid %i, group pid %i\r\n", identifier, getpid(),
		       getpgid(getpid()));
		printf("%sin infinite loop\r\n", identifier);
		while (1)
			;
		printf("%sdone\r\n", identifier);
		exit(0);

	} else {
		// parent code
		strncpy(identifier, PARENT_TXT, IDENTIFIER_SIZE);
		printf("%spid %i, group pid %i\r\n", identifier, getpid(),
		       getpgid(getpid()));
		printf("%ssleeps 5 secs\r\n", identifier);
		sleep(5);

		printf("%skill child process - killpg(pid = %i, SIGKILL)\r\n",
		       identifier, getpgid(getpid()));
		killpg(getpgid(getpid()), SIGKILL); // kills the whole group

		// won't be executed
		printf("%sdone\r\n", identifier);
		exit(0);
	}
}
