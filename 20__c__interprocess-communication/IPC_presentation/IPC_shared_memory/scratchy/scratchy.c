// scratchy.c
/*
  shared memory - sender
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "../sharedmemory.h"

#define ME "SCRATCHY:"
#define MESSAGE "Scratchymessage"


int main(int argc, char **argv)
{
  // shared memory key
  key_t shm_key = 1234;

  // shared memory id and get the shared memory
  fprintf(stderr,
          "%s set up shared memory key, ID and get some shared memory\n", ME);
  int shm_id = 0;
  int shm_flags = IPC_CREAT | 0666;
  if (0 > (shm_id = shmget(shm_key, SHM_SIZE, shm_flags))) {
    perror("shmget failed");
    exit(EXIT_FAILURE);
  }

  // attach shared memory to process
  fprintf(stderr, "%s attach shared memory to the process\n", ME);
  char *shm = NULL;
  if (NULL == (shm = shmat(shm_id, NULL, 0))) {
    perror("shmat failed");
    exit(EXIT_FAILURE);
  }

  /* send message (max SHM_SIZE) */
  fprintf(stderr, "%s send message \"%s\"\n", ME, MESSAGE);

  // set pointer to shared memory and reset
  char *ptr = shm;
  memset(ptr, '\0', SHM_SIZE);

  // if message size is ok, copy into shared memory
  if (strlen(MESSAGE) <= SHM_SIZE) {

    // write into shared memory via pointer
    strncpy(ptr, MESSAGE, strlen(MESSAGE));
  }

  // release pointer
  ptr = NULL;

  // wait until message was read
  while ('*' != *shm)
    sleep(1);


  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}
