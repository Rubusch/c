#include "lib_unix.h"

#include <stdlib.h>
#include <stdio.h>

//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>


// test helpers

#define TEST__BEGIN(FUNC)					\
	void test__##FUNC()					\
	{							\
	char test_res[8] = "TODO";				\
	fprintf(stdout, "executing test__%s()...", #FUNC);


#define TEST__END					\
	fprintf(stdout, "%s\n", test_res);		\
	}

#define TEST__OK strcpy(test_res, "OK");

int TEST__IS_FD_OPEN(int fd)
{
	return (0 <= fcntl(fd, F_GETFL)); // avoid function calls inside macros
}


TEST__BEGIN(lothars__calloc) {
	int* ptr = NULL;
	size_t nmemb = MAXLINE;
	size_t size = sizeof(*ptr);
	assert(NULL == ptr); /* pre-condition */
	ptr = lothars__calloc(nmemb, size);
	assert(NULL != ptr); /* post-condition */
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__close_null) {
	int a;
	int backup;
	a = open("/dev/null", O_WRONLY, 0600);
	backup = a;
	assert(STDERR_FILENO < a);
	assert(TEST__IS_FD_OPEN(a));
	assert(!TEST__IS_FD_OPEN(a+1)); /* verify no other fds leaked into this process */
	assert(!TEST__IS_FD_OPEN(a+2));
	assert(0 != a);

	lothars__close_null(&a);

	assert(0 == a);
	assert(!TEST__IS_FD_OPEN(backup));
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__dup2) {
	int a;
	int backup;
	a = open("/dev/null", O_WRONLY, 0600);
	backup = a;
	assert(STDERR_FILENO < a);
	assert(TEST__IS_FD_OPEN(a));
	assert(!TEST__IS_FD_OPEN(a+1)); /* verify no other fds leaked into this process */
	assert(!TEST__IS_FD_OPEN(a+2));
	assert(0 != a); /* pre-condition */

	lothars__dup2(a, a); /* must be a no-op */

	assert(TEST__IS_FD_OPEN(a));

	errno = 0;
	lothars__dup2(a, a+2); /* dup2 can scip fds */

	assert(TEST__IS_FD_OPEN(a));
	assert(!TEST__IS_FD_OPEN(a+1));
	assert(TEST__IS_FD_OPEN(a+2));

	close(a);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fcntl) {
} TEST__END

TEST__BEGIN(lothars__gettimeofday) {
} TEST__END

TEST__BEGIN(lothars__ioctl) {
} TEST__END

TEST__BEGIN(lothars__fork) {
} TEST__END

TEST__BEGIN(lothars__malloc) {
} TEST__END

TEST__BEGIN(lothars__mkstemp) {
} TEST__END

TEST__BEGIN(lothars__mmap) {
} TEST__END

TEST__BEGIN(lothars__open) {
} TEST__END

TEST__BEGIN(lothars__pipe) {
} TEST__END

TEST__BEGIN(lothars__sigaddset) {
} TEST__END

TEST__BEGIN(lothars__sigdelset) {
} TEST__END

TEST__BEGIN(lothars__sigemptyset) {
} TEST__END

TEST__BEGIN(lothars__sigfillset) {
} TEST__END

TEST__BEGIN(lothars__sigismember) {
} TEST__END

TEST__BEGIN(lothars__signal) {
} TEST__END

TEST__BEGIN(lothars__signal_intr) {
} TEST__END

TEST__BEGIN(lothars__sigpending) {
} TEST__END

TEST__BEGIN(lothars__sigprocmask) {
} TEST__END

TEST__BEGIN(lothars__strdup) {
} TEST__END

TEST__BEGIN(lothars__sysconf) {
} TEST__END

TEST__BEGIN(lothars__sysctl) {
} TEST__END

TEST__BEGIN(lothars__unlink) {
} TEST__END

TEST__BEGIN(lothars__wait) {
} TEST__END

TEST__BEGIN(lothars__waitpid) {
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__calloc();
	test__lothars__close_null();
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
	// TODO

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
