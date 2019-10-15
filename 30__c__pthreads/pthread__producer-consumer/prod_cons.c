// prod_cons.c
/*
  copying data with a producer / consumer implementation using POSIX threads

  reads one file and writes into another
  uses read() and write() which is pretty much low level and fast
  since this copying is done by threads - it seems to be extremely fast
copying!!!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFSIZE 512
#define BUFCNT 4

extern int pthread_setconcurrency(int);

/*
   this is the data structure that is used between the producer
   and consumer threads
*/
struct {
  char buffer[BUFCNT][BUFSIZE];
  int byteinbuf[BUFCNT];

  pthread_mutex_t buflock;
  pthread_mutex_t donelock;

  pthread_cond_t adddata;
  pthread_cond_t remdata;

  int nextadd, nextrem, occ, done;
} Buf;

void *consumer(void *);


/*
  main
//*/
int main(int argc, char **argv)
{
  int fd_input;
  unsigned long fd_output;
  pthread_t thr_consumer;

  // check the command line arguments
  if (argc != 3)
    printf("Usage: %s <infile> <outfile>\n", argv[0]), exit(0);

  // open the input file for the producer to use
  if ((fd_input = open(argv[1], O_RDONLY)) == -1) {
    fprintf(stderr, "Can't open file %s\n", argv[1]);
    exit(1);
  }

  // open the output file for the consumer to use
  if ((fd_output = open(argv[2], O_WRONLY | O_CREAT, 0666)) == -1) {
    fprintf(stderr, "Can't open file %s\n", argv[2]);
    exit(1);
  }

  // zero all counters
  Buf.nextadd = Buf.nextrem = Buf.occ = Buf.done = 0;

  // set the thread concurrency to 2 so the producer and consumer can run
  // concurrently
  pthread_setconcurrency(2);

  /*
    consumer
  //*/

  // create the consumer thread
  if (0 != pthread_create(&thr_consumer, NULL, consumer, ( void * )fd_output)) {
    perror("pthread create failed");
    exit(EXIT_FAILURE);
  }

  /*
    producer
  //*/

  // run the producer looped
  while (1) {

    // ***** lock
    pthread_mutex_lock(&Buf.buflock);

    /*
       check to see if any buffers are empty
       If not then wait for that condition to become true
    */
    while (Buf.occ == BUFCNT) {
      pthread_cond_wait(&Buf.remdata, &Buf.buflock);
    }

    // read from the file and put data into a buffer
    Buf.byteinbuf[Buf.nextadd] =
        read(fd_input, Buf.buffer[Buf.nextadd], BUFSIZE);

    // check to see if done reading
    if (Buf.byteinbuf[Buf.nextadd] == 0) {

      // *** lock the done lock
      pthread_mutex_lock(&Buf.donelock);

      // set the done flag
      Buf.done = 1;

      // release done lock ***
      pthread_mutex_unlock(&Buf.donelock);

      // signal the consumer to start consuming (still locked!)
      pthread_cond_signal(&Buf.adddata);

      // release mutex *****
      pthread_mutex_unlock(&Buf.buflock);

      break;
    }

    /*
      since we're still in the while loop the code below will be executed if we
      could not enter the if clause.
      if so the break would have jumped over the following code lines
    //*/

    // set next buffer to fill
    Buf.nextadd = (1 + Buf.nextadd) % BUFCNT;

    // increment the number of buffers that are filled
    Buf.occ++;

    // signal the consumer to start consuming
    pthread_cond_signal(&Buf.adddata);

    // release mutex *****
    pthread_mutex_unlock(&Buf.buflock);
  }

  close(fd_input);


  // wait for the consumer to finish
  if (0 != pthread_join(thr_consumer, NULL)) {
    perror("pthread join failed");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}


/*
   The consumer thread
*/
void *consumer(void *arg)
{
  unsigned long fd = ( unsigned long )arg;

  while (1) {

    // ***** lock the mutex
    pthread_mutex_lock(&Buf.buflock);

    if (!Buf.occ && Buf.done) {
      // unlock, to prevent deadlocks *****
      pthread_mutex_unlock(&Buf.buflock);
      break;
    }

    // check to see if any buffers are filled if not then wait for the
    // condition to become true
    while (Buf.occ == 0 && !Buf.done) {
      pthread_cond_wait(&Buf.adddata, &Buf.buflock);
    }

    // write the data from the buffer to the file
    write(fd, Buf.buffer[Buf.nextrem], Buf.byteinbuf[Buf.nextrem]);

    // set the next buffer to write from
    Buf.nextrem = (1 + Buf.nextrem) % BUFCNT;

    // decrement the number of buffers that are full
    Buf.occ--;

    // signal the producer that a buffer is empty
    pthread_cond_signal(&Buf.remdata);

    // release the mutex *****
    pthread_mutex_unlock(&Buf.buflock);
  }


  pthread_exit(0);
}
