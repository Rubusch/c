#include "lib_mcast.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__mcast_leave) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_join) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_leave_source_group) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_join_source_group) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_block_source) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_unblock_source) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_get_if) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_get_loop) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_get_ttl) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_set_if) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_set_loop) {
// TODO
} TEST__END

TEST__BEGIN(lothars__mcast_set_ttl) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__mcast_leave();
	test__lothars__mcast_join();
	test__lothars__mcast_leave_source_group();
	test__lothars__mcast_join_source_group();
	test__lothars__mcast_block_source();
	test__lothars__mcast_unblock_source();
	test__lothars__mcast_get_if();
	test__lothars__mcast_get_loop();
	test__lothars__mcast_get_ttl();
	test__lothars__mcast_set_if();
	test__lothars__mcast_set_loop();
	test__lothars__mcast_set_ttl();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
