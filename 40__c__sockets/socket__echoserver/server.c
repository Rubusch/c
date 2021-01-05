// echoserver.c

#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERV_PORT 12345
#define MAXSIZE 4096


void worker(int sockfd)
{
	ssize_t nbytes;
	char buf[MAXSIZE];
	memset(buf, '\0', MAXSIZE);
	char *ptr = buf;

	puts("connected");
	do {
		if (0 == (nbytes = recv(sockfd, ptr, MAXSIZE - 1, 0))) {
			continue;
		} else if (0 > nbytes) {
			perror("recv() failed");
			break;
		}

		if (0 < (nbytes = strlen(buf))) {
			puts(buf);
			if (nbytes != send(sockfd, buf, nbytes, 0)) {
				perror("send() failed");
				break;
			}
		}
		memset(buf, '\0', MAXSIZE);
		ptr = buf;

	} while (1);
	puts("exit");
}


int main(int argc, char *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	do {
		if (0 > (listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))) {
			perror("socket() failed");
			break;
		}

		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(SERV_PORT);

		if (0 > bind(listenfd, ( struct sockaddr * )&servaddr, sizeof(servaddr))) {
			perror("bind() failed");
			break;
		}

		if (0 > listen(listenfd, 7)) {
			perror("listen() failed");
			break;
		}

		do {
			clilen = sizeof(cliaddr);
			if (0 >
			    (connfd = accept(listenfd, ( struct sockaddr * )&cliaddr, &clilen))) {
				perror("accept() failed");
				exit(EXIT_FAILURE);
			}

			if (0 > (childpid = fork())) {
				perror("fork() failed");
				exit(EXIT_FAILURE);

			} else if (0 == childpid) { // child
				close(listenfd);
				worker(connfd);
				exit(EXIT_SUCCESS);
			}

			close(connfd);
		} while (1);

	} while (0);

	return EXIT_SUCCESS;
}
