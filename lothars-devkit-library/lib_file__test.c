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

TEST__BEGIN(lothars__fopen_rw) {
	int fd = -1;
	FILE *f = NULL;
	char file[] = "/tmp/test__fopen";
	char buf[] = "bico meh mini big\n";
	char baf[] = "maxi muxi\n";
	char buf_read[2*sizeof(buf)];
	int ret = -1;

	memset(buf_read, '\0', sizeof(buf_read));
	unlink(file); // cleanup artifacts

	// prepare a file
	fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= fd);
	assert(write(fd, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(fd));

	// call
	assert(NULL == f);
	ret = lothars__fopen_rw(&f, file);
	assert(0 == ret);
	assert(f);

	// read
	fgets(buf_read, sizeof(buf_read), f);
	assert(sizeof(buf) == strlen(buf_read) + 1); // plus string termination

	// close
	assert(0 == fclose(f));
	f = NULL;

	// open the file with content (again)
	assert(NULL == f);
	ret = lothars__fopen_rw(&f, file);
	assert(0 == ret);
	assert(f);

	// write buf (append)
	fputs(baf, f);
	memset(buf_read, '\0', sizeof(buf_read));
	assert(0 == strlen(buf_read));

	// close
	assert(0 == fclose(f));
	f = NULL;

	// open the file with content (again)
	assert(NULL == f);
	ret = lothars__fopen_rw(&f, file);
	assert(0 == ret);
	assert(f);

	// read, again
	fgets(buf_read, sizeof(buf_read), f);
	assert(sizeof(baf) == strlen(buf_read) + 1); // plus string termination

	// close
	assert(0 == fclose(f));
	unlink(file);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fopen_r) {
	int fd = -1;
	FILE *f = NULL;
	char file[] = "/tmp/test__fopen";
	char buf[] = "bico meh mini big\n";
	char buf_read[2*sizeof(buf)];
	int ret = -1;

	// prepare a file
	fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= fd);
	assert(write(fd, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(fd));

	// call
	assert(NULL == f);
	ret = lothars__fopen_r(&f, file);
	assert(0 == ret);
	assert(f);

	// read
	fgets(buf_read, sizeof(buf_read), f);
	assert(sizeof(buf) == strlen(buf_read) + 1); // plus string termination

	// close
	assert(0 == fclose(f));
	unlink(file);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fopen_w) {
	FILE *f = NULL;
	char file[] = "/tmp/test__fopen";
	char buf[] = "bico meh mini big\n";
	char buf_read[2*sizeof(buf)];
	int ret = -1;

	memset(buf_read, '\0', sizeof(buf_read));
	unlink(file); // cleanup artifacts

	// call
	assert(NULL == f);
	ret = lothars__fopen_w(&f, file);
	assert(0 == ret);
	assert(f);

	// write buf (append)
	fputs(buf, f);
	memset(buf_read, '\0', sizeof(buf_read));
	assert(0 == strlen(buf_read));

	// close
	assert(0 == fclose(f));
	f = NULL;

	// open the file with content (again)
	assert(NULL == f);
	f = fopen(file, "r");
	assert(f);

	// read, again
	fgets(buf_read, sizeof(buf_read), f);
	assert(sizeof(buf) == strlen(buf_read) + 1); // plus string termination

	// close
	assert(0 == fclose(f));
	unlink(file);
	TEST__OK;
} TEST__END

TEST__BEGIN(lothars__fopen_a) {
	int fd = -1;
	FILE *f = NULL;
	char file[] = "/tmp/test__fopen";
	char buf[] = "bico meh mini big\n";
	char baf[] = "maxi muxi\n";
	char buf_read[2*sizeof(buf)];
	int ret = -1;

	memset(buf_read, '\0', sizeof(buf_read));
	unlink(file); // cleanup artifacts

	// prepare a file
	fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= fd);
	assert(write(fd, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(fd));

	// call
	assert(NULL == f);
	ret = lothars__fopen_a(&f, file);
	assert(0 == ret);
	assert(f);

	// write buf (append)
	fputs(baf, f);
	memset(buf_read, '\0', sizeof(buf_read));
	assert(0 == strlen(buf_read));

	// close
	assert(0 == fclose(f));
	f = NULL;

	// open the file with content (again)
	assert(NULL == f);
	f = fopen(file, "r");
	assert(f);

	// read, again (the first line is read, since we are sure the
	// text is written to the file [standard functions] we test if
	// the orig first line is still the first line and was not
	// overwritten
	fgets(buf_read, sizeof(buf_read), f);
	assert(sizeof(buf) == strlen(buf_read) + 1); // plus string termination

	// close
	assert(0 == fclose(f));
	unlink(file);
	TEST__OK;
} TEST__END

TEST__BEGIN(read_char) {
	int fd = -1;
	FILE *f = NULL;
	char file[] = "/tmp/test__read_char";
	char buf[] = "bico meh mini big\n"; // size: 19 (inclusively
					    // the \n and \0)
	char *content = NULL;
	unsigned long int content_size = 7; // is going to be
					    // increased by number of
					    // reallocated memory,
					    // let's start with 7
					    // bytes
	int ret = -1;

	unlink(file); // cleanup artifacts

	// prepare a file
	fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= fd);
	assert(write(fd, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(fd));

	// read_char extends with realloc,
	// thus needs a pointer to allocated memory
	if (NULL == (content = calloc(2, sizeof(buf)))) {
		perror("test read_char() - calloc failed.");
		exit(EXIT_FAILURE);
	}

	// call
	assert(NULL == f);
	f = fopen(file, "r");
	assert(7 == content_size);
	ret = read_char(f, &content, &content_size);
	assert(0 == ret);
	assert(0 == strncmp(buf, content, sizeof(buf)));
	assert(content_size > sizeof(buf));
	assert(19 == sizeof(buf)); // 19, see above
	assert(21 == content_size); // initial content_size = 7,
				    // allocates in multiples of
				    // initial content_size, thus 3 x
				    // 7 == 21

	// close
	assert(0 == fclose(f));
	unlink(file);
	free(content);
	TEST__OK;
} TEST__END

TEST__BEGIN(read_without_spaces) {
	int fd = -1;
	FILE *f = NULL;
	char file[] = "/tmp/test__read_without_spaces";
	char buf[] = "bico meh mini big\n"; // size: 19 (inclusively
					    // the \n and \0)
	char *content = NULL;
	unsigned long int content_size = 7;
	int ret = -1;

	unlink(file); // cleanup artifacts

	// prepare a file
	fd = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= fd);
	assert(write(fd, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(fd));

	// read_char extends with realloc,
	// thus needs a pointer to allocated memory
	if (NULL == (content = calloc(2, sizeof(buf)))) {
		perror("test read_char() - calloc failed.");
		exit(EXIT_FAILURE);
	}

	// call
	assert(NULL == f);
	f = fopen(file, "r");
	assert(7 == content_size);
	ret = read_without_spaces(f, &content, &content_size);
	assert(0 == ret);
	assert(0 == strncmp("bicomehminibig", content, 14));
	assert(content_size >= sizeof(buf)); // allocated in blocks is greater or equal
	assert(14 == strlen(content));
	assert(21 == content_size);

	// close
	assert(0 == fclose(f));
	unlink(file);
	free(content);
	TEST__OK;
} TEST__END

TEST__BEGIN(read_linewise) {

// TODO: int read_linewise(FILE *, char **, unsigned long int *);

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
/* TODO - WIP
	int a = -1;
	char file[] = "/tmp/test__rename_file";
	char buf[] = "bicomeh";
	char baf[] = "minibig";

	// prepare a file
        a = open(file, O_RDWR|O_CREAT|O_TRUNC, 0600);
	assert(0 <= a);
	assert(write(a, buf, sizeof(buf)) == sizeof(buf));
	assert(0 == close(a));
// */

	/* call
TODO assert - stat file under old name does exist
TODO assert - stat file under new name does not exist
	assert(NULL == b);
	ret = lothars__rename_file(&a, &b);
	assert(0 == ret);
TODO rename_file() - to be called on stream, filedescriptor or filename?
TODO assert - stat file under old name does not exist anymore
TODO assert - stat file under new name does exist
	*/

/*
	// close
	assert(0 == fclose(a));
	unlink(file);
	TEST__OK;
// */
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
//	test__lothars__fclose_null();
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
