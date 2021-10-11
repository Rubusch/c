#include "snippet_ifi.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"

// test definitions

TEST__BEGIN(get_ifi_info)
{
	// TODO
}
TEST__END

TEST__BEGIN(lothars__get_ifi_info)
{
	// TODO
}
TEST__END

TEST__BEGIN(free_ifi_info)
{
	// TODO
}
TEST__END

/*
*/
int main(int argc, char *argv[])
{
	test__get_ifi_info();
	test__lothars__get_ifi_info();
	test__free_ifi_info();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
