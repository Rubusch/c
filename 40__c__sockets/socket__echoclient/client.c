// echoclient.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
  constants
*/

#define SERV_PORT 12345
#define MAXSIZE 4096


/*
  forwards
*/

// errors
void err_sys(const char *, ...);


/*
  internal helpers
*/

/*
  print message and return to caller Caller specifies "errnoflag"

  error handling taken from "Unix Sockets" (Stevens)
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1]; memset(buf, '\0', sizeof(buf));
	errno_save = errno; // value caller might want printed
	vsnprintf(buf, MAXLINE, fmt, ap);
	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");

	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);

	return;
}


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

/*
  fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


       


/********************************************************************************************/
// worker implementation




/********************************************************************************************/


/*
  main()
*/
int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	char buf[MAXSIZE]; memset(buf, '\0', MAXSIZE);
	unsigned int echolen;
	int nbytes = 0;
	int read = 0;
	char text[4096]; memset(text, '\0', 4096);
	char serverip[16]; memset(serverip, '\0', sizeof(serverip));
	char port[16]; memset(port, '\0', sizeof(port));

	if (3 != argc) {
		fprintf(stderr, "usage: %s <serverip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	strncpy(serverip, argv[1], sizeof(serverip));
	fprintf(stdout, "serverip: '%s'\n", serverip);

	strncpy(port, argv[2], sizeof(port));
	fprintf(stdout, "port: '%s'\n", port);

	strcpy(text,
	       "<?xml version=\"1.0\"?> \n<TASK task_name=\"guiagent\">\n  <DATA_OUT "
	       "name=\"xxx\" dim=\"0\" type=\"cmd\" value=\"agtstart\">\n <DST "
	       "name=\"agentname\" />\n </DATA_OUT>\n </TASK>");

	fprintf(stdout, "socket()\n");
	if (0 > (sockfd = socket(PF_INET, SOCK_STREAM,  
				 IPPROTO_TCP))) { // PF_INET for packets
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET; // AF_INET for addresses
	serveraddr.sin_addr.s_addr = inet_addr(serverip);
	serveraddr.sin_port = htons(atoi(port));

	fprintf(stdout, "connect()\n");
	if (0 >
	    connect(sockfd, ( struct sockaddr * )&serveraddr, sizeof(serveraddr))) {  
		perror("connect() failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "connect() - ok\n");

	fprintf(stdout, "send()\n");
	echolen = strlen(text);
	if (echolen != send(sockfd, text, echolen, 0)) {  
		perror("send() failed");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "send() - ok\n");

	fprintf(stdout, "received: ");
	while (nbytes < echolen) {
		read = 0;
		puts("recv()");
		if (1 > (read = recv(sockfd, buf, MAXSIZE - 1, 0))) { 
			perror("recv() failed");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "recv() - ok\n");
		nbytes += read;
		buf[read] = '\0';
		puts(buf);
	}
	close(sockfd); 

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
