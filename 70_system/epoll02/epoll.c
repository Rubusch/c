/*
  adding the epoll_ctl() - still not functional

  resources:
  https://suchprogramming.com/epoll-in-3-easy-steps/
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

int main()
{
	struct epoll_event ev;
	int epfd;

	epfd = epoll_create1(0);
	if (epfd == -1) {
		perror("epoll_create() failed");
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = 0;

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev)) {
		perror("epoll_ctl() failed");
		close(epfd);
		exit(EXIT_FAILURE);
	}

	close(epfd);

	puts("READY.");
	exit(EXIT_SUCCESS);
}
