// itchy.c
/*
  unix socket - server
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../unixsocket.h"

#define ME "ITCHY:"
#define MESSAGE "Itchymessage"


int main(int argc, char **argv)
{
	int sd_listen = -1;
	int sd_client = -1;
	struct sockaddr_un adr_server;
	char buf[BUF_SIZE];
	char message[] = MESSAGE;
	memset(buf, '\0', BUF_SIZE);

	do {
		// set up client socket
		fprintf(stderr, "%s set up \"listen socket\"\n", ME);
		if (0 > (sd_listen = socket(AF_UNIX, SOCK_STREAM, 0))) {
			perror("socket failed");
			break;
		}

		// init server address
		fprintf(stderr, "%s init server address\n", ME);
		memset(&adr_server, 0, sizeof(adr_server));
		adr_server.sun_family = AF_UNIX;
		strncpy(adr_server.sun_path, SERVER_PATH, 1 + strlen(SERVER_PATH));

		// bind socket to address
		fprintf(stderr, "%s bind \"listen socket\" to the server address\n", ME);
		if (0 > bind(sd_listen, ( struct sockaddr * )&adr_server,
			     sizeof(adr_server))) {
			perror("bind failed");
			break;
		}

		// listen on socket with queue of max 3 candidantes
		fprintf(stderr, "%s listening\n", ME);
		if (0 > listen(sd_listen, 3)) {
			perror("listen failed");
			break;
		}

		// accept - accept() is blocking by default, thus the server waits until it
		// can connect
		fprintf(stderr, "%s waiting for accept..\n", ME);
		if (0 > (sd_client = accept(sd_listen, NULL, NULL))) {
			perror("accept failed");
			break;
		}

		/*
		// set sockoption - in this case (SO_RCVLOWAT) the socket won't wake up the
		// recv() until the buffer is full
		const int size = BUF_SIZE;
		if(0 > setsockopt(sd_client, SOL_SOCKET, SO_RCVLOWAT, (char*) &size,
		sizeof(size))){ perror("setsockopt failed"); break;
		}
		//*/

		// receive
		register ssize_t bytes = 0;
		if (0 > (bytes = recv(sd_client, buf, BUF_SIZE, 0))) {
			fprintf(stderr, "%s recv failed\n", ME);
			// nothing

		} else if (bytes == 0) {
			fprintf(stderr, "%s nothing received\n", ME);
			break;

		} else {
			buf[bytes] = '\0'; // add string termination!
			fprintf(stderr, "%s received \"%s\"\n", ME, buf);
		}

		// send
		fprintf(stderr, "%s send \"%s\"..\n", ME, message);
		memset(buf, '\0', BUF_SIZE);
		strncpy(buf, message, strlen(message));
		if (0 > send(sd_client, buf, sizeof(buf), 0)) {
			perror("send failed");
			break;
		}

	} while (0);


	// in case something crashed - clean up
	if (-1 != sd_listen)
		close(sd_listen);
	if (-1 != sd_client)
		close(sd_client);
	unlink(SERVER_PATH);

	fprintf(stderr, "%s done!\n", ME);
	exit(EXIT_SUCCESS);
}
