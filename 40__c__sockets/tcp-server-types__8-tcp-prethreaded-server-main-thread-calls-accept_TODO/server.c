// server.c
/*
  tcp server types

  prethreaded, main thread calls accept (blocking)

  TODO check
*/

#include "thread.h"


/*
  constants
*/

// TODO


/*
  forwards
*/

// TODO


/*
  internal helpers
*/

// TODO


/*
  helpers / wrappers

  mainly to cover error handling and display error message
*/

// TODO


static int n_threads;

pthread_mutex_t mutex_fd_cli = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_fd_cli = PTHREAD_COND_INITIALIZER;


/********************************************************************************************/
// example code - code for some demo actions, not necessarily related to the server type

// max line size
#define MAXN 1234

/*
  example code: do anything, read, write, etc.. some action
*/
void web_child(int fd_sock)
{
	int n_towrite;
	ssize_t n_read;
	char line[MAXLINE], result[MAXN];

	for(;;){
		// read
		if(0 == (n_read = _readline(fd_sock, line, MAXLINE))){
			return; // connection closed by other end
		}

		// get as long
		n_towrite = atol(line);

		// check if valid
		if((0 >= n_towrite) || (MAXN < n_towrite)){
			err_quit("client request for %d bytes", n_towrite);
		}

		// if ok, write to socket
		_write(fd_sock, result, n_towrite);
	}
}


/*
  example code: print system time
*/
void pr_cpu_time()
{
	double user, sys;
	struct rusage usage_parent, usage_child;
	if(0 > getrusage(RUSAGE_SELF, &usage_parent)){
		err_sys("getrusage(parent) error");
	}

	if(0 > getrusage(RUSAGE_CHILDREN, &usage_child)){
		err_sys("getrusage(child) error");
	}

	user = (double) usage_parent.ru_utime.tv_sec + usage_parent.ru_utime.tv_usec / 1000000.0;
	user += (double) usage_child.ru_utime.tv_sec + usage_child.ru_utime.tv_usec / 1000000.0;

	sys = (double) usage_parent.ru_stime.tv_sec + usage_parent.ru_stime.tv_usec / 1000000.0;
	sys += (double) usage_child.ru_stime.tv_sec + usage_child.ru_stime.tv_usec / 1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}


/*
  example code: mini signal handler printing system time when shutting down
*/
void sig_int(int signo)
{
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}

/********************************************************************************************/


/*
  main _-*,.
*/
int main(int argc, char** argv)
{
	int idx, fd_listen, fd_conn;
	socklen_t addrlen, clilen;
	struct sockaddr* cliaddr = NULL;

	// init "listen socket"
	fd_listen = _tcp_listen(NULL, "27976", &addrlen); // listen on any ip, portnumber = 27976

	// allocate dynamic space for the address structure
	cliaddr = _malloc(addrlen);

	// init thread pointer, number of threads = 7
	n_threads = 7;
	thread_ptr = _calloc(n_threads, sizeof(*thread_ptr));
	idx_thread = idx_mainthread = 0;

	// create threadpool with n_threads threads
	for(idx=0; idx < n_threads; ++idx){
		thread_make(idx); // only main thread returns
	}

	// wake thread in case thread index (idx_thread) was main thread index
	// (idx_mainthread), and therefore thread was waiting under conditional
	_signal(SIGINT, sig_int);

	for(;;){
		clilen = addrlen;
		fd_conn = _accept(fd_listen, cliaddr, &clilen);

		_pthread_mutex_lock(&mutex_fd_cli);
		fd_cli[idx_mainthread] = fd_conn;

		// increment index idx_mainthread, in case reset to 0
		if(++idx_mainthread == MAXNCLI) idx_mainthread = 0;

		// check if main thread index (idx_mainthread) is thread index (idx_thread)
		if(idx_mainthread == idx_thread){
			err_quit("idx_mainthread = idx_thread = %d", idx_mainthread);
		}

		_pthread_cond_signal(&cond_fd_cli);
		_pthread_mutex_unlock(&mutex_fd_cli);
	}
}
