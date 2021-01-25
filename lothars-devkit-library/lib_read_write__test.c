#include "lib_read_write.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__read) {
// TODO
} TEST__END

TEST__BEGIN(lothars__readline_fd) {
// TODO
} TEST__END

TEST__BEGIN(lothars__readn) {
// TODO
} TEST__END

TEST__BEGIN(lothars__read_fd) {
// TODO
} TEST__END

TEST__BEGIN(lothars__write) {
// TODO
} TEST__END

TEST__BEGIN(lothars__writen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__write_fd) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__read();
	test__lothars__readline_fd();
	test__lothars__readn();
	test__lothars__read_fd();
	test__lothars__write();
	test__lothars__writen();
	test__lothars__write_fd();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
