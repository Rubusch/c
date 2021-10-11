// pthreads_ipc.c
/*
  ipc synchronization in memory mapping using POSIX Semaphores
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// a structure that will be used between processes
typedef struct {
	sem_t mysema;
	int num;
} buf_t;

int main()
{
	// open a file to use in a memory mapping
	int fd = open("/dev/zero", O_RDWR);

	/*
     create a shared memory map with the open file for the data
     structure that will be shared between processes
  */
	buf_t *buf = (buf_t *)mmap(NULL, sizeof(buf_t), PROT_READ | PROT_WRITE,
				   MAP_SHARED, fd, 0);

	/*
    initialize the semaphore -- note the USYNC_PROCESS flag; this makes
    the semaphore visible from a process level
  */
	sem_init(&buf->mysema, 0, 0);

	// fork a new process
	int pid = fork();
	if (0 > pid) {
		// failed
		perror("fork failed");
		exit(EXIT_FAILURE);

	} else if (0 == pid) {
		// child code
		int j_idx;
		for (j_idx = 0; j_idx < 5; j_idx++) {
			// have the child "wait" for the semaphore

			printf("Child PID(%d): waiting...\n", getpid());
			sem_wait(&buf->mysema);

			/*
         the child decremented the semaphore
      */

			printf("Child PID(%d): decrement semaphore.\n",
			       getpid());
		}
		// exit the child process
		printf("Child PID(%d): exiting...\n", getpid());
		exit(0);
	} else {
		// parent code

		sleep(2);

		int i_idx;
		for (i_idx = 0; i_idx < 5; i_idx++) {
			// increment (post) the semaphore
			printf("Parent PID(%d): posting semaphore.\n",
			       getpid());
			sem_post(&buf->mysema);

			// wait a second
			sleep(1);
		}

		// exit the parent process
		printf("Parent PID(%d): exiting...\n", getpid());

		exit(EXIT_SUCCESS);
	}
}
