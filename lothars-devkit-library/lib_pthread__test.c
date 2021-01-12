#include "lib_pthread.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__pthread_create) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_join) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_detach) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_kill) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_mutexattr_init) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_mutexattr_setpshared) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_mutex_init) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_mutex_lock) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_mutex_unlock) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_cond_broadcast) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_cond_signal) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_cond_wait) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_cond_timedwait) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_key_create) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_setspecific) {
// TODO
} TEST__END

TEST__BEGIN(lothars__pthread_once) {
// TODO
} TEST__END


/*
*/
int main(int argc, char* argv[])
{
	test__lothars__pthread_create();
	test__lothars__pthread_join();
	test__lothars__pthread_detach();
	test__lothars__pthread_kill();
	test__lothars__pthread_mutexattr_init();
	test__lothars__pthread_mutexattr_setpshared();
	test__lothars__pthread_mutex_init();
	test__lothars__pthread_mutex_lock();
	test__lothars__pthread_mutex_unlock();
	test__lothars__pthread_cond_broadcast();
	test__lothars__pthread_cond_signal();
	test__lothars__pthread_cond_wait();
	test__lothars__pthread_cond_timedwait();
	test__lothars__pthread_key_create();
	test__lothars__pthread_setspecific();
	test__lothars__pthread_once();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
