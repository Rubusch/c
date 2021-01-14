// shmserver.c
/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZ 27


int main(int argc, char **argv)
{
	// name of the shared memory segment - "5678"
	key_t shm_key = 5678;

	// shmget - create shared memory segment
	int shm_id;
	if (0 > (shm_id = shmget(shm_key, SHM_SIZ, IPC_CREAT | 0666))) {
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}

	// shmat - attach process to shared memory section
	char *shm = NULL;
	if (NULL == (shm = shmat(shm_id, NULL, 0))) {
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}

	// put things into the memory
	char *str = shm;
	char chr;
	printf("send string #");
	for (chr = 'a'; chr <= 'z'; ++chr) {
		printf("%c", chr);
		*str++ = chr;
	}
	printf("#\n");
	/*
	  set pointer to NULL to detach it from the shared memory
	//*/
	str = NULL;

	// wait until the other process changes the first character
	// polling every second
	while (*shm != '*')
		sleep(1);

	puts("READY.");
	exit(EXIT_SUCCESS);
}
