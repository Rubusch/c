/*
  join

- pthread_attr_init()

  sets up the pthread_attr_t object, to a default set of parameters,
  if not used, the same set already should be the default at thread
  creation. The most important is: PTHREAD_CREATE_JOINABLE is default!


- pthread_create()

  creates a new thread


- pthread_join()

  The pthread_join() function blocks the calling thread until the
  specified thread terminates. Hence, a join means "join to the same
  used memory" (more or less), thus on pthread_exit() on one of the
  joined threads, the allocated memory is NOT freed.


- pthread_detach()

  says explicitely that the thread will not be joined and, thus, has
  its own "detached" space of memory to maintain. On pthread_exit()
  the allocated heap space is freed automatically! But there is no
  "synchronization" with other threads, like on pthread_join(). [when
  the other thread is blocked, this is a "synchronized" way to access
  the resources, i.e. the memory.]


- pthread_exit()

  terminates the thread - you MUST do a free in case the thread was
  "joinable" or "joined" and the thread allocated memory before
  (memory leak if not!). If you explicitely "detached" the thread
  before, the memory is automatically freed!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

// a value
unsigned int value;

// a thread function - the other will be the main thread, created with
// pthread_self()
void *thr_add(void *);

/*
  main
*/
int main()
{
	puts("POSIX threads: join demo");
	puts("init..");
	pthread_t tid_main = pthread_self(), tid_add = 0;
	printf("\ttid_main set to %lu\n", (unsigned long)tid_main);
	printf("\ttid_add set to %d\n", (int)tid_add);
	value = 8;
	printf("\tvalue set to %d\n", value);
	puts("init done.\n");

	do {
		/*
		  1. set up the attributes

		  pthread_create creates by default PTHREAD_CREATE_JOINABLE, also
		  without explicitely set up attr's!!!

		  Only when disabling the joining, it is necessary to set up the
		  attributes to PTHREAD_CREATE_DETACHED.
		*/
		pthread_attr_t attr_add;
		pthread_attr_init(&attr_add);
		// try to uncomment this line!
		//    pthread_attr_setdetachstate(&attr_add, PTHREAD_CREATE_DETACHED);

		/*
		  2. create the thread

		  created with *pthread_attr_t == NULL, this uses the default
		  setting: PTHREAD_CRATE_JOINABLE
		*/
		puts("MAIN: create thread tid_add");
		if (0 !=
		    pthread_create(&tid_add, &attr_add, thr_add, &tid_main)) {
			perror("MAIN: pthread_create failed");
			break;
		}

		/*
		  3. It's the JOIN - EPMD rulez! =D

		  join to tid_add! This means: make the calling thread [tid_main]
		  wait (suspend!), untill the joined [tid_add] has finished

		  Just to see what happens without JOIN, comment out the paragraph
		  below by removing the "// * /"
		*/
		int return_status = 0;
		if (0 > pthread_join(tid_add, (void *)&return_status)) {
			perror("MAIN: pthread_join failed");
			pthread_exit((
				void *)11); // just any return code,
					    // here: 11 - can also be
					    // NULL
		}
		printf("MAIN: joining to tid_add %s\n",
		       (return_status == 22) ? "succeeded" : "FAILED!!!\n");
		//*/

		/*
		  4. operation in tid_main: divide

		  the following code here is just executed in tid_main, tid_add
		  already is running in parallel at this time. This means either
		  the faster one wins or, in case of tid_main was blocked, tid_add
		  already finished its work on the data [value, in this case] and
		  tid_main continues.

		  joining is a form of blocking another thread and, thus a form of
		  synchronization!
		*/
		printf("MAIN: %d / 2 ", value);
		value /= 2;
		printf("= %d\n", value);

		// show end result
		printf("MAIN: end result value = %d (should be 5)\n", value);

		// exit tid_main
		puts("MAIN: detaching tid_main, to clean up the threads stack (just safer)");
		pthread_detach(tid_main);

		puts("MAIN: READY.");
		pthread_exit(&tid_main);
	} while (0);

	/*
	  CAUTION: this code won't be executed if the thread creation
	  worked!!!  It is safe here to procede like this, since
	  allocated memory should be freed in case the thread crashes
	  (objects on the thread's stack are usually not destroyed as
	  well, a SIGSEGV is the logical consequence!
	*/
	puts("\nREADY.");
	exit(EXIT_SUCCESS);
}

/*
  the adding thread:

  here nothing special, this is just the executed code of thr_add
  thr_add returns 22 (which needs to be typedef-ed to void*, since
  POSIX threads demand here a void* as return type
*/
void *thr_add(void *arg)
{
	puts("ADD: started");

	// operation
	printf("ADD: %d + 2 ", value);
	value += 2;
	printf("= %d\n", value);

	// exit
	puts("ADD: exited");
	pthread_exit((void *)22);
}
