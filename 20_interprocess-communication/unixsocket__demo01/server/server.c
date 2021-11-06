// server.c
/*
  demonstrates the creation and use of a unix socket - AF_UNIX!!!
*/

#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../unixsocket.h"

char *strings[NUMBER_OF_STRINGS] = {
	"this is the first string form the server.",
	"this is the second string from the server.",
	"this is the third string from the server."
};

int main(int argc, char **argv)
{
	// socket
	register int sd_listen = 0;
	if (0 > (sd_listen = socket(AF_UNIX, SOCK_STREAM, 0))) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// address for the server - Unix (cast to sockaddr possible)
	struct sockaddr_un adr_server;
	memset(&adr_server, 0, sizeof(adr_server));
	adr_server.sun_family = AF_UNIX;
	strncpy(adr_server.sun_path, SERVER_PATH, 1 + strlen(SERVER_PATH));
	unlink(SERVER_PATH);

	// bind server to the address
	if (0 > bind(sd_listen, (struct sockaddr *)&adr_server,
		     sizeof(adr_server))) {
		perror("bind failed");
		unlink(SERVER_PATH);
		exit(EXIT_FAILURE);
	}

	// listen on the socket
	if (0 > listen(sd_listen, 5)) {
		perror("listen failed");
		unlink(SERVER_PATH);
		exit(EXIT_FAILURE);
	}

	/*
	  without loop accept() blocks and waits until it
	  receives a client request to connect to
	//*/

	// accept clients
	register int sd_client = 0;
	struct sockaddr_un adr_client;
	unsigned int len_addr_from = 0;
	if (0 > (sd_client = accept(sd_listen, (struct sockaddr *)&adr_client,
				    &len_addr_from))) {
		perror("accept failed");
		unlink(SERVER_PATH);
		exit(EXIT_FAILURE);
	}

	puts("connected");

	register int bytes = 0;
	register int idx = 0;
	char strReceived[RECEIVED_SIZE];
	do {
		// init
		memset(strReceived, '\0', RECEIVED_SIZE);
		bytes = 0;

		// receive
		if ((0 >= (bytes = recv(sd_client, strReceived, RECEIVED_SIZE,
					0))) &&
		    (bytes < RECEIVED_SIZE)) {
			perror("recv failed");

		} else {
			strReceived[bytes] = '\0';
			printf("received: \"%s\"\n", strReceived);

			// send
			printf("send \"%s\"\n", strings[idx]);
			if (0 > send(sd_client, strings[idx],
				     strlen(strings[idx]), 0)) {
				perror("send failed");
				break;
			} else {
				// next string
				++idx;
			}
		}

	} while (idx < NUMBER_OF_STRINGS);

	// over and close
	puts("READY.");
	unlink(SERVER_PATH);
	close(sd_listen);
	exit(EXIT_SUCCESS);
}
