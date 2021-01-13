#include "lib_streams.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__fclose) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fdopen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fgets) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fputs) {
// TODO
} TEST__END


/*
*/
int main(int argc, char* argv[])
{
	test__lothars__fclose();
	test__lothars__fdopen();
	test__lothars__fgets();
	test__lothars__fopen();
	test__lothars__fputs();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
