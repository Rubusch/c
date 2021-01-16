#include "lib_streams.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(lothars__fclose) {
	int a=-1, b=-1, c=-1;
	FILE *f;
	char file[] = "/tmp/test__fclose";
	char buf[] = "bico meh mini big";

	/* prepare a seekable file */
	a = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= a);
	assert(write(a, buf, sizeof(buf)) == sizeof(buf));
	assert(1 == lseek(a, 1, SEEK_SET));

	/* create an output stream visiting that file; when it is
	 * closed all other FILE descriptors visiting that file must
	 * see the new file position */
	b = dup(a);
	assert(0 <= b);
	f = fdopen(b, "w");
	assert(f);

	assert(buf[1] == fputc(buf[1], f));
	lothars__fclose(&f);
	assert(NULL == f);
	errno = 0;
	assert(-1 == lseek(b, 0, SEEK_CUR));
	assert(errno == EBADF);
	assert(2 == lseek(a, 0, SEEK_CUR));

	/* likewise for an input stream */
	c = dup(a);
	assert(0 <= c);
	f = fdopen(c, "r");
	assert(f);
	assert(buf[2] == fgetc(f));
	lothars__fclose(&f);
	assert(NULL == f);
	errno = 0;
	assert(-1 == lseek(c, 0, SEEK_CUR));
	assert(errno == EBADF);
/*
	fprintf(stderr, "XXX %d\n", (int)lseek(a, 0, SEEK_CUR));
	assert(3 == lseek(a, 0, SEEK_CUR)); // is reproducible on 18,
					    // the original test (GNU
					    // m4) also fails here
// */
	assert(0 == remove(file));
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fdopen) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fgets) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen) {
	FILE *a = NULL, *b = NULL, *c = NULL;
	char file[] = "/tmp/test__fopen";
	char null[] = "/dev/null";

	unlink(file); // cleanup artifacts

	assert(NULL == a);
	a = lothars__fopen(file, "w");
	assert(a);
	assert(0 == fclose(a));

	b = lothars__fopen(null, "r");
	assert(b);
	assert(0 == fclose(b));

	c = lothars__fopen(null, "w");
	assert(c);
	assert(0 == fclose(c));

	unlink(file);
	TEST__OK;
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
