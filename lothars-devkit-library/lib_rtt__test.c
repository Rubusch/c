#include "lib_rtt.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(rtt_debug) {
// TODO
} TEST__END

TEST__BEGIN(rtt_init) {
// TODO
} TEST__END

TEST__BEGIN(rtt_newpack) {
// TODO
} TEST__END

TEST__BEGIN(rtt_start) {
// TODO
} TEST__END

TEST__BEGIN(rtt_stop) {
// TODO
} TEST__END

TEST__BEGIN(rtt_timeout) {
// TODO
} TEST__END

TEST__BEGIN(rtt_ts) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__rtt_debug();
	test__rtt_init();
	test__rtt_newpack();
	test__rtt_start();
	test__rtt_stop();
	test__rtt_timeout();
	test__rtt_ts();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
