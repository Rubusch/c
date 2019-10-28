// semaphore.c
/*
  We fork a child process so that we have two processes running:
  each process communicates via a semaphore.

  The respective process can only do its work (not much here)
  When it notices that the semaphore track is free when it returns
  to 0 each process must modify the semaphore accordingly.

  the key elemnts of this programm are as follows:
   - after a semaphore is created with as simple key 1234, two processes
      are forked
   - each process (parent and child) essentially permorms the same
      operations:
   -- each process accesses the same semaphore track( sops[].sem_num=0 )
   -- each process waits for the track to become free and then attempts
      to take control of track
      this is achieved by setting appropriate sops[].sem_op values in
      the array
   -- once the process has control it sleeps for 5 secs (in reality some
      processing would take place of this simple illustration)
   -- the process then gives up control of the track sops[1].sem_op = -1
   -- an additional sleep operation is then performed to ensure that the
      other process has time to access the semaphore before a subsequent
      (same process) semaphore read.

  Note: there is no synchronization here in this simple example an we
  have no control over how the OS will schedule the process!
//*/
// FIXME: deadlocks!


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};


int main(int argc, char **argv)
{
  unsigned int i;

  // index for the sem_ops array
  int idx = 0;

  // sem_id of the semaphore set
  unsigned int sem_id = 0;

  // key to pass to semget()
  key_t key = 1234;

  // sem_flag to pass to semget()
  unsigned int sem_flag = IPC_CREAT | 0666;

  // nsems to pass to semget()
  unsigned int nsems = 1;

  // number of operations to do
  unsigned int nsem_ops = 0;

  // pointer to operation to perform
  struct sembuf *sem_ops = NULL;
  if (NULL == (sem_ops = calloc(2, sizeof(*sem_ops)))) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  // set up semaphore
  fprintf(stderr, "setting up semaphore: semget(%#lx, %d, %#o)\n",
          ( long unsigned int )key, nsems, sem_flag);

  if (-1 == (sem_id = semget(key, nsems, sem_flag))) {
    perror("semget failed");
    exit(EXIT_FAILURE);
  } else {
    fprintf(stderr, "semget: semget returned %d\n", sem_id);
  }

  // fork process
  unsigned int pid;
  if (0 > (pid = fork())) {
    perror("fork failed");
    exit(EXIT_FAILURE);

  } else if (pid == 0) {
    /*
      child code
    //*/

    i = 0; // TODO

    // allow for 3 semaphore sets
    while (i < 3) {
      nsem_ops = 2;

      // wait for semaphore to reach zero

      // we only use one track
      sem_ops[0].sem_num = 0;

      // wait for semaphore flag to become zero
      sem_ops[0].sem_op = 0;

      // take off semaphore asynchronous
      sem_ops[0].sem_flg = SEM_UNDO;


      sem_ops[1].sem_num = 0;

      // increment semaphore - sake control of track
      sem_ops[1].sem_op = 1;

      // take off semaphore
      sem_ops[1].sem_flg = SEM_UNDO | IPC_NOWAIT;

      /*
        recap the call to be made
      //*/

      fprintf(stderr, "child calling semop(%d, &sem_ops, %d)\n", sem_id,
              nsem_ops);
      for (idx = 0; idx < nsem_ops; ++idx) {
        fprintf(stderr, "\n\tchild: sem_ops[%d]..sem_num = %d, ", idx,
                sem_ops[idx].sem_num);
        fprintf(stderr, "sem_op = %d, ", sem_ops[idx].sem_op);
        fprintf(stderr, "sem_flag = %#o\n", sem_ops[idx].sem_flg);
      }

      // make the semop() call and report the result
      if (-1 == (idx = semop(sem_id, sem_ops, nsem_ops))) {
        perror("child: semop failed");
      } else {
        fprintf(stderr, "child: semop returned %d\n", idx);
        fprintf(stderr,
                "child process taking control of the track: %d/3 times\n",
                i + 1);

        sleep(5);

        nsem_ops = 1;

        // wait for semaphore to reach zero
        sem_ops[0].sem_num = 0;

        // give up control of track
        sem_ops[0].sem_op = -1;

        // take off semaphore, asynchronous
        sem_ops[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

        if (0 > (idx = semop(sem_id, sem_ops, nsem_ops))) {
          perror("child: semop failed");
        } else {
          fprintf(stderr,
                  "child process giving up control of track: %d/3 times\n",
                  i + 1);

          // halt process for 5 seconds to allow parent to catch semaphor change
          // first
          sleep(5);
        }
        ++i;
      }
    }
  } else {
    /*
      parent code
    //*/

    i = 0;

    // allow for 3 semaphore sets
    while (i < 3) {
      nsem_ops = 2;


      // wait for semaphore to reach zero
      sem_ops[0].sem_num = 0;

      // wait for semaphore flag to become zero
      sem_ops[0].sem_op = 0;

      // take off semaphore asynchronous
      sem_ops[0].sem_flg = SEM_UNDO;


      sem_ops[1].sem_num = 0;

      // increment semaphore - take control of track
      sem_ops[1].sem_op = 1;

      // take off semaphore
      sem_ops[1].sem_flg = SEM_UNDO | IPC_NOWAIT;


      /* recap the call to be made //*/
      fprintf(stderr, "parent caling semop(%d, &sem_ops, %d) with:\n", sem_id,
              nsem_ops);
      for (idx = 0; idx < nsem_ops; ++idx) {
        fprintf(stderr, "\tparent: sem_ops[%d].sem_num = %d, ", idx,
                sem_ops[idx].sem_num);
        fprintf(stderr, "sem_op = %d, ", sem_ops[idx].sem_op);
        fprintf(stderr, "sem_flg = %#o\n", sem_ops[idx].sem_flg);
      }

      // make the semop() call and report the results
      if (0 > (idx = semop(sem_id, sem_ops, nsem_ops))) {
        perror("parent: semop failed");
      } else {
        fprintf(stderr, "parent: semop returned %d\n", idx);

        sleep(5);

        nsem_ops = 1;

        // wait for semaphore to reach zero
        sem_ops[0].sem_num = 0;

        // give up control of track
        sem_ops[0].sem_op = -1;

        // take off semaphore, asynchronous
        sem_ops[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

        if (0 > (idx = semop(sem_id, sem_ops, nsem_ops))) {
          perror("parent: semop failed");
        } else {
          fprintf(stderr,
                  "parent process giving up control of track: %d/3 times \n",
                  i + 1);
        }

        // halt process for 5 secs to allow child to catch semaphor change first
        sleep(5);
      }
      ++i;
    }
  }

  puts("READY.");
  // TODO: free space
  exit(EXIT_SUCCESS);
}
