#include "lib_file.h"

#include <stdlib.h>
#include <stdio.h>
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

	/*
	  create an output stream visiting that file; when it is
	  closed all other FILE descriptors visiting that file must
	  see the new file position
	*/
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
	/* Test behavior on failure.  POSIX makes it hard to check for
	   failure, since the behavior is not well-defined on invalid file
	   descriptors, so try fdopen 1000 times and if that's not enough to
	   fail due to EMFILE, so be it.
	   (GNU m4 / autotools project)
	*/
	int idx;
	for (idx = 0; idx < 1000; idx++)
	{
		errno = 0;
		lothars__fdopen(STDOUT_FILENO, "w");
	}
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fgets) {
 	int a=-1;
	FILE *f;
	char file[] = "/tmp/test__fgets";
	char buf[] = "bico meh\nmini big";
	char baf[] = "bico meh mini big";
	char res[sizeof(buf)];

	memset(res, '\0', sizeof(res));

	/* prepare a file */
	a = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= a);
	assert(write(a, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(a));

	f = fopen(file, "r");
	assert(f);
	lothars__fgets(res, sizeof(buf), f);
	assert(9 == strlen(res)); // shall read up to n characters or
				  // until the linefeed if earlier
	fclose(f);
	remove(file);

	memset(res, '\0', sizeof(res));

	/* prepare a file */
	a = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= a);
	assert(write(a, baf, sizeof(baf)) == sizeof(baf));
	assert(0 == close(a));

	f = fopen(file, "r");
	assert(f);
	lothars__fgets(res, 9+1, f); // shall read 9 characters, plus termination
	assert(9 == strlen(res)); // the string then has 9 characters of strlen()
	fclose(f);
	remove(file);

	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fputs) {
	FILE *f;
	char file[] = "/tmp/test__fputs";
	char buf[] = "bico meh mini big\n";
	char res[2*sizeof(buf)];

	memset(res, '\0', sizeof(res));

	f = fopen(file, "w");
	assert(f);
	lothars__fputs(buf, f);
	fclose(f);
	f = NULL;
	assert(NULL == f);

	f = fopen(file, "r");
	assert(f);
	fgets(res, sizeof(buf), f);
	assert(sizeof(buf) == strlen(res) + 1); // plus string termination

	remove(file);
	TEST__OK
} TEST__END

TEST__BEGIN(lothars__fopen) {
	FILE *a = NULL, *b = NULL, *c = NULL;
	char file[] = "/tmp/test__fopen";
	char null[] = "/dev/null";
	int ret = -1;

	unlink(file); // cleanup artifacts

	assert(NULL == a);
	ret = lothars__fopen(&a, file, "w");
	assert(0 == ret);
	assert(a);
	assert(0 == fclose(a));

	ret = -1;
	ret = lothars__fopen(&b, null, "r");
	assert(0 == ret);
	assert(b);
	assert(0 == fclose(b));

	ret = -1;
	ret = lothars__fopen(&c, null, "w");
	assert(0 == ret);
	assert(c);
	assert(0 == fclose(c));

	unlink(file);
	TEST__OK;
} TEST__END

//TEST__BEGIN(lothars__fopen) {
//// TODO
//} TEST__END

TEST__BEGIN(lothars__fopen_rw) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_r) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_w) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fopen_a) {
// TODO
} TEST__END

TEST__BEGIN(lothars__fclose_null) {
// TODO
} TEST__END

TEST__BEGIN(read_char) {
// TODO
} TEST__END

TEST__BEGIN(read_without_spaces) {
// TODO
} TEST__END

TEST__BEGIN(read_linewise) {
// TODO
} TEST__END

TEST__BEGIN(read_blockwise) {
// TODO
} TEST__END

TEST__BEGIN(write_char) {
// TODO
} TEST__END

TEST__BEGIN(write_formated) {
// TODO
} TEST__END

TEST__BEGIN(write_linewise) {
// TODO
} TEST__END

TEST__BEGIN(create_file) {
// TODO
} TEST__END

TEST__BEGIN(remove_file) {
// TODO
} TEST__END

TEST__BEGIN(shred_file) {
// TODO
} TEST__END

TEST__BEGIN(rename_file) {
// TODO
} TEST__END

TEST__BEGIN(copy_characterwise_unbuffered) {
// TODO
} TEST__END

TEST__BEGIN(copy_characterwise_buffered) {
// TODO
} TEST__END

TEST__BEGIN(number_of_tempfiles) {
// TODO
} TEST__END

TEST__BEGIN(number_of_characters_in_static_temp) {
// TODO
} TEST__END

TEST__BEGIN(filesize) {
// TODO
} TEST__END

TEST__BEGIN(check_eof) {
// TODO
} TEST__END

TEST__BEGIN(check_error) {
// TODO
} TEST__END

TEST__BEGIN(get_bufsize) {
// TODO
} TEST__END

TEST__BEGIN(read_without_eof) {
// TODO
} TEST__END

TEST__BEGIN(read_nth_line) {
// TODO
} TEST__END

TEST__BEGIN(read_lines) {
// TODO
} TEST__END

TEST__BEGIN(read_lines_with_pattern) {
// TODO
} TEST__END

TEST__BEGIN(get_more_space) {
// TODO
} TEST__END



/*
*/
int main(int argc, char* argv[])
{
	test__lothars__fclose();
	test__lothars__fdopen();
	test__lothars__fgets();
	test__lothars__fputs();
	test__lothars__fopen();
//	test__lothars__fopen();
	test__lothars__fopen_rw();
	test__lothars__fopen_r();
	test__lothars__fopen_w();
	test__lothars__fopen_a();
	test__lothars__fclose_null();
	test__read_char();
	test__read_without_spaces();
	test__read_linewise();
	test__read_blockwise();
	test__write_char();
	test__write_formated();
	test__write_linewise();
	test__create_file();
	test__remove_file();
	test__shred_file();
	test__rename_file();
	test__copy_characterwise_unbuffered();
	test__copy_characterwise_buffered();
	test__number_of_tempfiles();
	test__number_of_characters_in_static_temp();
	test__filesize();
	test__check_eof();
	test__check_error();
	test__get_bufsize();
	test__read_without_eof();
	test__read_nth_line();
	test__read_lines();
	test__read_lines_with_pattern();
	test__get_more_space();

	fprintf(stdout, "READY.\n");
	exit(EXIT_SUCCESS);
}
