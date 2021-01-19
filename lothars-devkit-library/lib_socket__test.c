#include "lib_socket.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__fclose) {
// TODO
} TEST__END

TEST__BEGIN(lothars__accept) {
// TODO
} TEST__END

TEST__BEGIN(lothars__bind) {
// TODO
} TEST__END

TEST__BEGIN(lothars__connect) {
// TODO
} TEST__END

TEST__BEGIN(lothars__getpeername) {
// TODO
} TEST__END

TEST__BEGIN(lothars__getsockname) {
// TODO
} TEST__END

TEST__BEGIN(lothars__getsockopt) {
// TODO
} TEST__END

TEST__BEGIN(lothars__listen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__poll) {
// TODO
} TEST__END

TEST__BEGIN(lothars__recv) {
// TODO
} TEST__END

TEST__BEGIN(lothars__recvfrom) {
// TODO
} TEST__END

TEST__BEGIN(lothars__recvmsg) {
// TODO
} TEST__END

TEST__BEGIN(lothars__select) {
// TODO
} TEST__END

TEST__BEGIN(lothars__send) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sendto) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sendmsg) {
// TODO
} TEST__END

TEST__BEGIN(lothars__setsockopt) {
// TODO
} TEST__END

TEST__BEGIN(lothars__shutdown) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sockatmark) {
// TODO
} TEST__END

TEST__BEGIN(lothars__socket) {
// TODO
} TEST__END

TEST__BEGIN(lothars__socketpair) {
// TODO
} TEST__END

TEST__BEGIN(lothars__host_serv) {
// TODO
} TEST__END


/*
*/
int main(int argc, char* argv[])
{
	test__lothars__accept();
	test__lothars__bind();
	test__lothars__connect();
	test__lothars__getpeername();
	test__lothars__getsockname();
	test__lothars__getsockopt();
	test__lothars__listen();
	test__lothars__poll();
	test__lothars__recv();
	test__lothars__recvfrom();
	test__lothars__recvmsg();
	test__lothars__select();
	test__lothars__send();
	test__lothars__sendto();
	test__lothars__sendmsg();
	test__lothars__setsockopt();
	test__lothars__shutdown();
	test__lothars__sockatmark();
	test__lothars__socket();
	test__lothars__socketpair();
	test__lothars__host_serv();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
