// shmclient.c
/*
  the client to the shared memory application
  unsynchronized!
*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHM_SIZ 27


int main(int argc, char **argv)
{
	// shm_key
	key_t shm_key = 5678;

	// shm_id - locate the segment
	int shm_id;
	if (0 > (shm_id = shmget(shm_key, SHM_SIZ, 0666))) {
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}

	// shmat - attach the shared memory
	char *shm = NULL;
	if ((( char * )-1) == (shm = shmat(shm_id, NULL, 0))) {
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}

	// read things from the memory
	char str[SHM_SIZ];
	memset(str, '\0', SHM_SIZ);
	strncpy(str, shm, SHM_SIZ);
	printf("received: #%s#\n", str);

	// change the first to indicate "done"
	puts("shut down communication");
	*shm = '*';

	puts("READY.");
	exit(EXIT_SUCCESS);
}
