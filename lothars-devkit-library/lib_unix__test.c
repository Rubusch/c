#include "lib_unix.h"

//#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"



// test definitions

TEST__BEGIN(min) {
	int a=1, b=-2;
	int res=0;
	res = min(a, b);
	assert(-2 == res);
	TEST__OK;
} TEST__END

TEST__BEGIN(max) {
	int a=1, b=-2;
	int res=0;
	res = max(a, b);
	assert(1 == res);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__calloc) {
	int* ptr = NULL;
	size_t nmemb = MAXLINE;
	size_t size = sizeof(*ptr);
	assert(NULL == ptr); /* pre-condition */
	ptr = lothars__calloc(nmemb, size);
	assert(NULL != ptr); /* post-condition */
	free(ptr);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__close) {
	int a;
	int backup;
	a = open("/dev/null", O_WRONLY, 0600);
	backup = a;
	assert(STDERR_FILENO < a);
	assert(is_fd_open(a));
	assert(!is_fd_open(a+1)); /* verify no other fds leaked into this process */
	assert(!is_fd_open(a+2));
	assert(0 != a);

	lothars__close(&a);

	assert(0 == a);
	assert(!is_fd_open(backup));
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__dup2) {
	int a;
	int backup=0;
	a = open("/dev/null", O_WRONLY, 0600);
	backup = a;
	assert(STDERR_FILENO < a);
	assert(is_fd_open(a));
	assert(!is_fd_open(a+1)); /* verify no other fds leaked into this process */
	assert(!is_fd_open(a+2));
	assert(0 != a); /* pre-condition */

	lothars__dup2(a, a); /* must be a no-op */

	assert(is_fd_open(a));

	errno = 0;
	lothars__dup2(a, a+2); /* dup2 can scip fds */

	assert(is_fd_open(a));
	assert(!is_fd_open(a+1));
	assert(is_fd_open(a+2));

	close(a);
	close(backup);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fcntl) {
	int a;
	a = open("/dev/null", O_WRONLY, 0600);
	assert(STDERR_FILENO < a);
	assert(is_fd_open(a));

	assert((0 <= lothars__fcntl(a, F_GETFL, 0)));
	/* invalid fds will abort the program */

	assert(0 != a);
	close(a);
	TEST__OK;
} TEST__END



#include <time.h> /* struct tm, ... */
TEST__BEGIN(lothars__gettimeofday) {
	int res = -1;
	time_t t = 0;
	struct tm *lt;
	struct tm saved_lt;
	struct timeval tv;
	lt = localtime (&t);
	saved_lt = *lt;

//	gettimeofday (&tv, NULL);
	lothars__gettimeofday(&tv);

	if (memcmp (lt, &saved_lt, sizeof (struct tm)) != 0)
	{
		fprintf (stderr, "gettimeofday still clobbers the localtime buffer!\n");
		res = 1;
	}
	res = 0;
	assert(0 == res);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__ioctl) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fork) {
	pid_t a=-1, res=-1;
	a = getpid();
//	if (0 == (res = fork())) {
	if (0 == (res = lothars__fork())) {
//		fprintf(stderr, "%s(): child - res %d, a %d, getpid() %d\n", __func__, res, a, getpid());
		assert(a != getpid()); // child
		abort();
	} else {
//		fprintf(stderr, "%s(): parent - res %d, a %d, getpid() %d\n", __func__, res, a, getpid());
		assert(a == getpid()); // parent
		assert(res != getpid());
	}
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__malloc) {
	char *ptr=NULL;
	assert(NULL == ptr);
	ptr = lothars__malloc(123);
	assert(NULL != ptr);
	free(ptr);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__mkstemp) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mmap) {
// TODO
} TEST__END

TEST__BEGIN(lothars__open) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pipe) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigaddset) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigdelset) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigemptyset) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigfillset) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigismember) {
// TODO
} TEST__END

TEST__BEGIN(lothars__signal) {
// TODO
} TEST__END

TEST__BEGIN(lothars__signal_intr) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigpending) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sigprocmask) {
// TODO
} TEST__END

TEST__BEGIN(lothars__strdup) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sysconf) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sysctl) {
// TODO
} TEST__END

TEST__BEGIN(lothars__unlink) {
// TODO
} TEST__END

TEST__BEGIN(lothars__wait) {
// TODO
} TEST__END

TEST__BEGIN(lothars__waitpid) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__min();
	test__max();
	test__lothars__calloc();
	test__lothars__close();
	test__lothars__dup2();
	test__lothars__fcntl();
	test__lothars__gettimeofday();
	test__lothars__ioctl();
	test__lothars__fork();
	test__lothars__malloc();
	test__lothars__mkstemp();
	test__lothars__mmap();
	test__lothars__open();
	test__lothars__pipe();
	test__lothars__sigaddset();
	test__lothars__sigdelset();
	test__lothars__sigemptyset();
	test__lothars__sigfillset();
	test__lothars__sigismember();
	test__lothars__signal();
	test__lothars__signal_intr();  // missing
	test__lothars__sigpending();
	test__lothars__sigprocmask();
	test__lothars__strdup();
	test__lothars__sysconf();
	test__lothars__sysctl();
	test__lothars__unlink();
	test__lothars__wait();
	test__lothars__waitpid();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
