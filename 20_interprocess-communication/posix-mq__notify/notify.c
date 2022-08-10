/*
  POSIX messagequeue - receive and notify

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


#define NOTIFY_SIG SIGUSR1

static void
handler(int sig)
{
	// just interrupt sigsuspend()
}

int
main(int argc, char *argv[])
{
	struct sigevent sev;
	mqd_t mqd;
	struct mq_attr attr;
	void *buffer;
	ssize_t nread;
	sigset_t block_mask, empty_mask;
	struct sigaction sa;

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <mq name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
	if ((mqd_t) -1 == mqd) {
		perror("mq_open()");
		exit(EXIT_FAILURE);
	}

	if (-1 == mq_getattr(mqd, &attr)) {
		perror("mq_getattr()");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (NULL == buffer) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	/* mask for notify */
	sigemptyset(&block_mask);
	sigaddset(&block_mask, NOTIFY_SIG);
	if (-1 == sigprocmask(SIG_BLOCK, &block_mask, NULL)) {
		perror("sigprocmask()");
		exit(EXIT_FAILURE);
	}

	/* register sigaction handler */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (-1 == sigaction(NOTIFY_SIG, &sa, NULL)) {
		perror("sigaction()");
		exit(EXIT_FAILURE);
	}

	/* signal event */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = NOTIFY_SIG;
	if (-1 == mq_notify(mqd, &sev)) {
		perror("mq_notify()");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&empty_mask);

	while (true) {
		sigsuspend(&empty_mask);

		if (-1 == mq_notify(mqd, &sev)) {
			perror("mq_notify()");
			exit(EXIT_FAILURE);
		}

		/* receive message, then notify */
		while (0 <= (nread = mq_receive(mqd, buffer, attr.mq_msgsize, NULL))) {
			fprintf(stderr, "receive: read %ld bytes\n",
				(long) nread);
		}

		if (EAGAIN != errno) {
			perror("mq_receive()");
			exit(EXIT_FAILURE);
		}
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
