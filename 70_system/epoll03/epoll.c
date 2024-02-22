/*
  functional epoll demo

  Level triggered and edge triggered event notifications

  Level-triggered and edge-triggered are terms borrowed from
  electrical engineering. When we’re using epoll the difference is
  important. In edge triggered mode we will only receive events when
  the state of the watched file descriptors change; whereas in level
  triggered mode we will continue to receive events until the
  underlying file descriptor is no longer in a ready state. Generally
  speaking level triggered is the default and is easier to use and is
  what I’ll use for this tutorial, though it’s good to know edge
  triggered mode is available.

  resources:
  https://suchprogramming.com/epoll-in-3-easy-steps/
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

#define MAX_EVENTS 5
#define READ_SIZE 10

int main()
{
	int event_count, idx;
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];
	struct epoll_event ev, evts[MAX_EVENTS];
	int epfd;

	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create() failed");
		exit(EXIT_FAILURE);
	}

	// obtain file descriptors here, then...

	ev.events = EPOLLIN;
	ev.data.fd = 0;  // <--- ...obtained descriptor goes here! 0 == stdin

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev)) {
		perror("epoll_ctl() failed");
		close(epfd);
		exit(EXIT_FAILURE);
	}

	// repeat the above per file descriptor

	printf("\nWaiting for input...\n");
	event_count = epoll_wait(epfd, evts, MAX_EVENTS, 30000);

	printf("%d ready events\n", event_count);
	for (idx = 0; idx < event_count; idx++) {
		printf("Reading file descriptor '%d' -- ", evts[idx].data.fd);
		bytes_read = read(evts[idx].data.fd, read_buffer, READ_SIZE);

		printf("%zd bytes read.\n", bytes_read);
		read_buffer[bytes_read] = '\0';

		printf("Read '%s'\n", read_buffer);
	}

	close(epfd);

	exit(EXIT_SUCCESS);
}

