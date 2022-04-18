/*
  usage:
  $ ./inotify.elf <path>

  e.g.
  $ ./inotify.elf $(pwd) &
    [1] 9230
    watch /usr/src/github__c/70_system/inotify using wd 1]
  $ ls
    read 16 bytes from inotify fd
            wd =  1; mask = IN_ISDIR IN_OPEN
    read 16 bytes from inotify fd
            wd =  1; mask = IN_ACCESS IN_ISDIR
    read 16 bytes from inotify fd
            wd =  1; mask = IN_ACCESS IN_ISDIR
    read 16 bytes from inotify fd
            wd =  1; mask = IN_CLOSE_NOWRITE IN_ISDIR
    inotify.c  inotify.elf  inotify.o  Makefile

  $ killall inotify.elf


  demostrates listening on all events happening at a provided path


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 382
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>

static void
display_inotify_event(struct inotify_event *ie)
{
	fprintf(stderr, "\twd = %2d; ", ie->wd);
	if (ie->cookie > 0) {
		fprintf(stderr, "cookie = %4d; ", ie->cookie);
	}

	fprintf(stderr, "mask = ");
	if (ie->mask & IN_ACCESS) { fprintf(stderr, "IN_ACCESS "); }
	if (ie->mask & IN_ATTRIB) { fprintf(stderr, "IN_ATTRIB "); }
	if (ie->mask & IN_CLOSE_NOWRITE) { fprintf(stderr, "IN_CLOSE_NOWRITE "); }
	if (ie->mask & IN_CLOSE_WRITE) { fprintf(stderr, "IN_CLOSE_WRITE "); }
	if (ie->mask & IN_CREATE) { fprintf(stderr, "IN_CREATE "); }
	if (ie->mask & IN_DELETE) { fprintf(stderr, "IN_DELETE "); }
	if (ie->mask & IN_DELETE_SELF) { fprintf(stderr, "IN_DELETE_SELF "); }
	if (ie->mask & IN_IGNORED) { fprintf(stderr, "IN_IGNORED "); }
	if (ie->mask & IN_ISDIR) { fprintf(stderr, "IN_ISDIR "); }
	if (ie->mask & IN_MODIFY) { fprintf(stderr, "IN_MODIFY "); }
	if (ie->mask & IN_MOVE_SELF) { fprintf(stderr, "IN_MOVE_SELF "); }
	if (ie->mask & IN_MOVED_FROM) { fprintf(stderr, "IN_MOVED_FROM "); }
	if (ie->mask & IN_MOVED_TO) { fprintf(stderr, "IN_MOVED_TO "); }
	if (ie->mask & IN_OPEN) { fprintf(stderr, "IN_OPEN "); }
	if (ie->mask & IN_Q_OVERFLOW) { fprintf(stderr, "IN_Q_OVERFLOW "); }
	if (ie->mask & IN_UNMOUNT) { fprintf(stderr, "IN_UNMOUNT "); }
	fprintf(stderr, "\n");

	if (ie->len > 0) {
		fprintf(stderr, "\t\tname = %s\n", ie->name);
	}
}

void
usage(const char* name)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "$ %s <path>\n", name);
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int
main(int argc, char *argv[])
{
	int fd_inotify, wd, idx;
	char buf[BUF_LEN] __attribute__((aligned(8)));
	ssize_t nread;
	char *ptr;
	struct inotify_event *event;

	if (argc < 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// 1.) create inotify instance, init file descriptor
	fd_inotify = inotify_init();
	if (-1 == fd_inotify) {
		perror("inotify_init() failed");
		exit(EXIT_FAILURE);
	}

	for (idx = 1; idx < argc; idx++) {
		// 2.) add a watch item for each of the files named in
		// the command-line argument fo the program; each
		// watch item watches for all possible events
		wd = inotify_add_watch(fd_inotify, argv[idx], IN_ALL_EVENTS);
		if (-1 == wd) {
			perror("inotify_add_watch() failed");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "watch %s using wd %d]\n", argv[idx], wd);
	}

	for (;;) { // inifinet loop

		// 3.) inside the loop, reads a buffer of events from
		// the inotify file descriptor
		nread = read(fd_inotify, buf, BUF_LEN);
		if (0 == nread) {
			fprintf(stderr, "read() from inotify fd returned 0!\n");
		}

		if (0 > nread) {
			fprintf(stderr, "read() failed\n");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "read %ld bytes from inotify fd\n", (long) nread);

		// process all of the events in buffer returned by read()
		for (ptr = buf; ptr < buf + nread; ) {
			// 4.) inside the loop, also, calls the
			// function to display the contents of each of
			// the inotify_event structures within that
			// buffer
			event = (struct inotify_event *) ptr;
			display_inotify_event(event);

			ptr += sizeof(struct inotify_event) + event->len;
		}
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
