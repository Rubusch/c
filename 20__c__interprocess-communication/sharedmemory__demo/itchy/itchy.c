// itchy.c
/*
  shared memory - receiver
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "../sharedmemory.h"

#define ME "ITCHY:"

int main(int argc, char **argv)
{
	// shared memory key
	key_t shm_key = 1234; // FIXME: ftok?

	// shared memory ID
	fprintf(stderr,
		"%s set up shared memory key, ID and get some shared memory\n",
		ME);
	int shm_id = 0;
	if (0 > (shm_id = shmget(shm_key, SHM_SIZE, 0666))) {
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}

	// attach shared memory to a process
	fprintf(stderr, "%s attach shared memory to process \"%s\"\n", ME, ME);
	/*
	  The shmat() function attaches the shared memory segment
	  associated with the shared memory identifier specified by
	  shmid to the address space of the calling process.
	*/
	char *shm = NULL;
	if (0 > *(int *)(shm = shmat(shm_id, NULL, 0))) {
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}

	// read message from shared memory
	char buf[SHM_SIZE];
	memset(buf, '\0', SHM_SIZE);
	strncpy(buf, shm, SHM_SIZE);
	memset(shm, '\0', SHM_SIZE);
	fprintf(stderr, "%s received \"%s\" and confirmed with an \'*\'\n", ME,
		buf);

	// confirm by setting "*" (done)
	*shm = '*';

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
