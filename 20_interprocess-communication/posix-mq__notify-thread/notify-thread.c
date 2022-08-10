/*
  POSIX messagequeue - receive and notify a thread


  preparation:
  $ sudo su
  # mkdir /dev/mqueue
  # mount -t mqueue none /dev/mqueue
  # exit

  $ cat /proc/mounts | grep mqueue
    mqueue /dev/mqueue mqueue rw,nosuid,nodev,noexec,relatime 0 0
  $ ls -ld /dev/mqueue
    drwxrwxrwt 2 root root 40 Jan  1  1970 /dev/mqueue

  usage:
  $ ./posix-mq__create/create.elf -c /mq
    READY.

  $ ./posix-mq__send/send.elf /mq abcdefg
    READY.

  $ cat /dev/mqueue/mq
    QSIZE:7          NOTIFY:0     SIGNO:0     NOTIFY_PID:0

  $ ./posix-mq__notify/notify.elf /mq &
    [1] 23165

  $ cat /dev/mqueue/mq
    QSIZE:7          NOTIFY:0     SIGNO:10    NOTIFY_PID:23165

  $ kill %1
    [1]+  Terminated              ./posix-mq__notify/notify.elf /mq

  $ ./posix-mq__notify-thread/notify-thread.elf /mq &
    [1] 23780

  $ cat /dev/mqueue/mq
    QSIZE:7          NOTIFY:2     SIGNO:0     NOTIFY_PID:23780

  $ ./posix-mq__unlink/unlink.elf /mq
    READY.


  The listing provides an example of message notification using
  signals. This program performs the following steps:

  1. Open the message queue named on the command line in nonblocking
     mode, determine the mq_msgsize attribute for the queue, and
     allocate a buffer of that size for receiving messages.

  2. Bock the notification signal (SIGUSR1) and establish a handler
     for it.

  3. Make an initial call to mq_notify() to register the process to
     receive message notification.

  4. Execute an infinite loop that performs the following steps:

     - Call sigsuspend(), which unblocks the notification signal and
       waits until the signal is caught. Return from this system call
       indicates that a message notification has occurred. At this
       point, the process will have been deregistered for message
       notification.

     - Call mq_notify() to reregister this process to receive message
       notification.

     - Execute a while loop that drains the queue by reading as many
       messages as possible.


  Additionally:

  - When a message notification occurs, the program reenables
    notification before draining the queue.

  - Nonblocking mode is employed so that, after receiving a
    notification, we can completely drain the queue without blocking.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1080
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h> /* link with -lrt */
#include <signal.h>
#include <stdbool.h>
#include <errno.h>


static void notify_setup(mqd_t *mqdp);

static void
thread_func(union sigval sv)
{
	ssize_t nread;
	mqd_t *mqdp;
	void *buffer;
	struct mq_attr attr;

	mqdp = sv.sival_ptr;

	if (-1 == mq_getattr(*mqdp, &attr)) {
		perror("mq_getattr()");
		_exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (NULL == buffer) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	notify_setup(mqdp);

	while (0 <= (nread = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL))) {
		fprintf(stderr, "receive: read %ld bytes\n",
			(long) nread);
	}

	if (EAGAIN != errno) {
		perror("mq_receive()");
		exit(EXIT_FAILURE);
	}

	free(buffer);
}

static void notify_setup(mqd_t *mqdp)
{
	struct sigevent sev;
	sev.sigev_notify = SIGEV_THREAD;     // notify via thread
	sev.sigev_notify_function = thread_func;
	sev.sigev_notify_attributes = NULL;  // could be pointer to pthread_attr_t structure
	sev.sigev_value.sival_ptr = mqdp;    // argument to thread_func()

	if (-1 == mq_notify(*mqdp, &sev)) {
	       perror("mq_notify()");
	       _exit(EXIT_FAILURE);
	}
}

int
main(int argc, char *argv[])
{
	mqd_t mqd;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd = mq_open(argv[1], O_RDONLY|O_NONBLOCK);
	if ((mqd_t) -1 == mqd) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	notify_setup(&mqd);
	pause();   // wait for notification via thread function

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
