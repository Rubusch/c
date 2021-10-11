// sock_server.c
/*
  implementation of a server using pthreads for exactly one client!

  the server still misses "testing", it works on port 6500 and uses
  a TCP (stream) socket

  it should accept any in-address
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>

extern int pthread_setconcurrency(int);

// the TCP port that is used for this example
#define TCP_PORT 6500

// function prototypes and global variables
void *child_out(void *);
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int service_count;

/*
  main
//*/
int main()
{
	// init
	service_count = 0;

	// socket for server
	int sock_server;
	if (0 > (sock_server = socket(AF_INET, SOCK_STREAM, 0))) {
		fprintf(stderr, "server: can't open stream socket\n"), exit(0);
	}

	// set up server address
	struct sockaddr_in addr_server;
	memset((char *)&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(TCP_PORT);

	// bind socket to serveraddress
	if (0 > bind(sock_server, (struct sockaddr *)&addr_server,
		     sizeof(addr_server))) {
		fprintf(stderr, "server: can't bind local address\n"), exit(0);
	}

	// set the level of thread concurrency we desire
	pthread_setconcurrency(5);

	// listen on socket
	listen(sock_server, 5);

	// server loop
	/*
    ATTENTION !!!

    these data need to be done for each client, if there are more clients,
    when there are more clinets they should be set to pointers, a struct, etc
    to guarantee that destruction happens for each client individually in the
    client's thread
  //*/
	int sock_client;
	struct sockaddr_in addr_client;
	pthread_t thr_child;
	pthread_attr_t attr_child;
	while (1) {
		// accept an incoming client on the socket and set it to the client socket:
		// sock_client
		unsigned int client_addr_len;
		client_addr_len = sizeof(addr_client);
		if (0 > (sock_client = accept(sock_server,
					      (struct sockaddr *)&addr_client,
					      &client_addr_len))) {
			fprintf(stderr, "server: accept error\n"), exit(0);
		}

		// create a new thread to process the incomming request
		pthread_attr_init(&attr_child);
		pthread_attr_setdetachstate(&attr_child,
					    PTHREAD_CREATE_DETACHED);
		if (0 != pthread_create(&thr_child, &attr_child, child_out,
					(void *)&sock_client)) {
			perror("pthread create failed");
			break;
		}

		// wait for secure connection - another approach would be IPC (e.g. also
		// signals!)
		sleep(1);

		pthread_mutex_lock(&lock);
		if (0 < service_count)
			break;
		pthread_mutex_unlock(&lock);
		// the server is now free to accept another socket request...
	}

	// freeing stuff
	pthread_attr_destroy(&attr_child);
	puts("READY");
	return (0);
}

/*
   This is the routine that is executed from a new thread
*/
void *child_out(void *arg)
{
	pthread_mutex_lock(&lock);
	service_count = 1;
	pthread_mutex_unlock(&lock);

	unsigned long mysocfd = (unsigned long)arg;

	printf("Child thread [%lu]: Socket number = %lu\n",
	       (unsigned long)pthread_self(), mysocfd);

	// read from the given socket
	char data[100];
	read(mysocfd, data, 40);

	printf("Child thread [%lu]: My data = %s\n",
	       (unsigned long)pthread_self(), data);

	// simulate some processing
	sleep(2);

	printf("Child thread [%lu]: Done Processing...\n",
	       (unsigned long)pthread_self());

	// close the socket and exit this thread, and set count to 0
	pthread_mutex_lock(&lock);
	service_count = 0;
	pthread_mutex_unlock(&lock);

	close(mysocfd);
	pthread_exit((void *)0);
}
