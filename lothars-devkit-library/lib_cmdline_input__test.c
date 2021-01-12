#include "lib_cmdline_input.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN() {
// TODO
} TEST__END

TEST__BEGIN(readdigit) {
// TODO
} TEST__END

TEST__BEGIN(isnumber) {
// TODO
} TEST__END

TEST__BEGIN(readnumber) {
// TODO
} TEST__END

TEST__BEGIN(readlongnumber) {
// TODO
} TEST__END

TEST__BEGIN(readstring) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__readdigit();
	test__isnumber();
	test__readnumber();
	test__readlongnumber();
	test__readstring();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
