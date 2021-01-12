#include "lib_if-name-index.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__if_indextoname) {
// TODO
} TEST__END

TEST__BEGIN(lothars__if_nametoindex) {
// TODO
} TEST__END

TEST__BEGIN(lothars__if_nameindex) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__if_indextoname();
	test__lothars__if_nametoindex();
	test__lothars__if_nameindex();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
