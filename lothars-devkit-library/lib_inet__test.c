#include "lib_inet.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"

// test definitions

TEST__BEGIN(lothars__inet_ntop)
{
	// TODO
}
TEST__END

TEST__BEGIN(lothars__inet_pton)
{
	// TODO
}
TEST__END

//TEST__BEGIN(lothars__inet_aton) // DEPRECATED
//{
//	// TODO
//}
//TEST__END

TEST__BEGIN(lothars__sock_ntop)
{
	// TODO
}
TEST__END

TEST__BEGIN(lothars__sock_ntop_host)
{
	// TODO
}
TEST__END

/*
*/
int main(int argc, char *argv[])
{
	test__lothars__inet_ntop();
	test__lothars__inet_pton();
//	test__lothars__inet_aton();
	test__lothars__sock_ntop();
	test__lothars__sock_ntop_host();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
