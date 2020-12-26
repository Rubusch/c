// server.c
/*
  UDP echo server using select()

  demonstrates usage of select
*/

//#include "../lib_socket/lib_socket.h"

// MAXLINE
// LISTENQ

// forwards
// lothars__sendto()    
// lothars__setsockopt()    
// lothars__recvfrom()   
// lothars__close()    
// lothars__fork()    
// lothars__accept()    
// lothars__signal()    
// lothars__bind()    
// lothars__socket()    


/*
  mini signal handler, calls waitpid to avoid zombies
*/
void sig_child(int signo)
{
	pid_t pid;
	int status;

	while (0 < (pid = waitpid(-1, &status, WNOHANG))) {
		printf("child %d terminated\n", pid);
	}
}


/*
  udp echo server implementation via tcp connection
TODO investigate, comment                   

  creates a listening tcp socket for connection establishment,
  then forks out udp connections for performance
*/
int main(int argc, char** argv)
{
	int fd_listen, fd_conn, fd_udp, n_ready, maxfdpl;
	char msg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t num;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;

	// create listening TCP socket
	fd_listen = lothars__socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	lothars__setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	lothars__bind(fd_listen, (struct sockaddr*) &servaddr, LISTENQ);

	// create UDP socket
	fd_udp = lothars__socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	lothars__bind(fd_udp, (struct sockaddr*) &servaddr, sizeof(servaddr));

	lothars__signal(SIGCHLD, sig_child); // must call waitpid()

	// 1. zero fd_set - select()
	FD_ZERO(&rset);

	maxfdpl = max(fd_listen, fd_udp) + 1;
	while (1) {

		// 2. add descriptors to the fd_set - select()
		FD_SET(fd_listen, &rset);
		FD_SET(fd_udp, &rset);

		// 3. select() multiplexing
// TODO check for using lothars__select()                
		if (0 > (n_ready = select(maxfdpl, &rset, NULL, NULL, NULL))) {
			if (errno == EINTR) { // caught interrupt
				continue;
			} else {
				err_sys("select error");
			}
		}

		// 4. select case "fd_listen"
		if (FD_ISSET(fd_listen, &rset)) {
			len = sizeof(cliaddr);
			fd_conn = lothars__accept(fd_listen, (struct sockaddr*) &cliaddr, &len);

			if (0 == (childpid = lothars__fork())) {
				// child process
				lothars__close(fd_listen);
				str_echo(fd_conn);
				exit(EXIT_SUCCESS);
			}

			// parent process
			lothars__close(fd_conn);
		}

		// 4. select case "fd_udp"
		if (FD_ISSET(fd_udp, &rset)) {
			len = sizeof(cliaddr);
			num = lothars__recvfrom(fd_udp, msg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
			lothars__sendto(fd_udp, msg, num, 0, (struct sockaddr*) &cliaddr, len);
		}
	}

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}

