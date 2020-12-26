//
/*
  simple udp echo server example


  socket functions
  for UDP client/server
                                UDP server

                               +----------+
                               | socket() |
                               +----------+
                                    |
                                    V
                                +--------+
                                | bind() |
                                +--------+
                                    |
                                    V
                              +------------+
       UDP client             | recvfrom() |<-------+
                              +------------+        |
      +----------+                  |               |
      | socket() |                  V               |
      +----------+          blocks until datagram   |
           |                received from client    |
           V                        |               |
      +----------+                  |               |
  +-->| sendto() |-----______       |               |
  |   +----------+           ------>|               |
  |        |                        |               |
  |        |                        V               |
  |        |                  process request       |
  |        |                        |               |
  |        |                        V               |
  |        |                   +----------+         |
  | +------------+     ____----| sendto() |---------+
  +-| recvfrom() |<----        +----------+
    +------------+
           |
           V
      +---------+
      | close() |
      +---------+

*/

// TODO 


int main(int argc, char **argv)
{
	int fd_sock;
	struct sockaddr_in servaddr, cliaddr;
	char port[16]; memset(port, '\0', sizeof(port));

	if (2 != argc) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	// socket
	fd_sock = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(port));

	// bind
	lothars__bind(fd_sock, (struct sockaddr*) &servaddr, sizeof(servaddr));

	// worker
	{
		struct sockaddr *pcliaddr = (struct sockaddr*) &cliaddr;

		int n_bytes;
		socklen_t len;
		char msg[MAXLINE]; memset(msg, '\0', sizeof(msg));

		len = sizeof(cliaddr);
		while (1) {
			n_bytes = lothars__recvfrom(fd_sock, msg, MAXLINE, 0, pcliaddr, &len);
			lothars__sendto(fd_sock, msg, n_bytes, 0, pcliaddr, len);
		}
	}

	exit(EXIT_SUCCESS);
}
