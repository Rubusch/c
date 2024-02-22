/*
  basic epoll demo - not functional

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
	int epfd;

	epfd = epoll_create1(0);
	if (-1 == epfd) {
		perror("epoll_create() failed");
		exit(EXIT_FAILURE);
	}

	close(epfd);

	puts("READY.");
	exit(EXIT_SUCCESS);
}

