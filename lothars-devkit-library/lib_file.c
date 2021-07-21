/*
  file/streams

  A collection of readers and writers to files, in case also
  streams. Ranging from simple wrappers, convenience wrappers to
  elaborated solutions and technical implementations.

  author: Lothar Rubusch, GPL

  ---
  References:
  Unix Network Programming, Stevens
  various
*/

#include "lib_file.h"


/*
  The fclose() function flushes the stream pointed to by fp (writing
  any buffered output data using fflush(3)) and closes the underlying
  file descriptor.

  #include <stdio.h>

  @fp: The specified stream.
*/
void lothars__fclose(FILE **fp)
{
	if (NULL == *fp)
		return;

	if (0 != fclose(*fp)) {
                err_sys("%s() error", __func__);
	}

	*fp = NULL;
	sync();
}


/*
  The fdopen() function associates a stream with the existing file
  descriptor, fd. The mode of the stream (one of the values "r", "r+",
  "w", "w+", "a", "a+") must be compatible with the mode of the file
  descriptor. The file position indicator of the new stream is set to
  that belonging to fd, and the error and end-of-file indicators are
  cleared. Modes "w" or "w+" do not cause truncation of the file. The
  file descriptor is not dup'ed, and will be closed when the stream
  created by fdopen() is closed. The result of applying fdopen() to a
  shared memory object is undefined.

  #include <stdio.h>

  @fd: A file descriptor for the stream.
  @mode: The argument mode.
*/
FILE* lothars__fdopen(int fd, const char *mode)
{
	FILE *fp = NULL;
	if (NULL == (fp = fdopen(fd, mode))) {
                err_sys("%s() error", __func__);
	}
	return fp;
}


/*
  fgets() reads in at most one less than size [n] characters from
  stream and stores them into the buffer pointed to by s
  [ptr]. Reading stops after an EOF or a newline. If a newline is
  read, it is stored into the buffer. A terminating null byte (aq\0aq)
  is stored after the last character in the buffer.

  Never use gets(). Because it is impossible to tell without knowing
  the data in advance how many characters gets() will read, and
  because gets() will continue to store characters past the end of the
  buffer, it is extremely dangerous to use. It has been used to break
  computer security. Use fgets() instead.

  #include <stdio.h>

  @ptr: The memory to store the read characters.
  @n: The number of characters to read.
  @stream: The stream to read from.
*/
char* lothars__fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = NULL;
	if ( (NULL == (rptr = fgets(ptr, n, stream))) && ferror(stream)) {
		err_sys("%s() error", __func__);
	}
	return rptr;
}


/*
  fputs() writes the string s to stream, without its terminating null
  byte.

  #include <stdio.h>

  @ptr: The string to be writen to the stream.
  @stream: The stream to write the string to.
*/
void lothars__fputs(const char *ptr, FILE *stream)
{
	if (EOF == fputs(ptr, stream)) {
                err_sys("%s() error", __func__);
	}
}


/*
  The fopen() function opens the file whose name is the string pointed
  to by path and associates a stream with it.

  #include <stdio.h>

  @fp: The file pointer to be initialized.
  @path: The valid path.
  @mode: The mode to open the file pointer.

  Returns 0 for ok, or -1 when failed.
*/
int lothars__fopen(FILE **fp, char *path, const char *mode)
{
	if (0 == strlen(path)) {
		err_msg("%s() path empty", __func__);
		return -1;
	}
	if (NULL == (*fp = fopen(path, mode))) {
		err_msg("%s() error", __func__);
		return -1;
	}
	return 0;
}


/*
  The wrapper inits a valid FILE file pointer to read and write.

  @fp: The file pointer to be initialized.
  @path: The path, limited to FILENAME_MAX (Linux system wide file
  path length limit, for ext4 4096 bytes).

  Returns 0 for ok, or -1 when failed.
*/
int lothars__fopen_rw(FILE **fp, char path[FILENAME_MAX])
{
	return lothars__fopen(fp, path, "rw+");
}


/*
  The wrapper inits a valid FILE file pointer to read.

  @fp: The file pointer to be initialized.
  @path: The path, limited to FILENAME_MAX (Linux system wide file
  path length limit, for ext4 4096 bytes).

  Returns 0 for ok, or -1 when failed.
*/
int lothars__fopen_r(FILE **fp, char path[FILENAME_MAX])
{
	return lothars__fopen(fp, path, "r");
}


/*
  The wrapper inits a valid FILE file pointer to write.

  @fp: The file pointer to be initialized.
  @path: The path, limited to FILENAME_MAX (Linux system wide file
  path length limit, for ext4 4096 bytes).

  Returns 0 for ok, or -1 when failed.
*/
int lothars__fopen_w(FILE **fp, char path[FILENAME_MAX])
{
	return lothars__fopen(fp, path, "w");
}


/*
  The wrapper inits a valid FILE file pointer to append.

  @fp: The file pointer to be initialized.
  @path: The path, limited to FILENAME_MAX (Linux system wide file
  path length limit, for ext4 4096 bytes).

  Returns 0 for ok, or -1 when failed.
*/
int lothars__fopen_a(FILE **fp, char path[FILENAME_MAX])
{
	return lothars__fopen(fp, path, "a");
}


// TODO rm, fclose() is already null-ing
///*
//  The fclose() function flushes the stream pointed to by fp (writing
//  any buffered output data using fflush(3)) and closes the underlying
//  file descriptor.
//
//  Note that fclose() only flushes the user-space buffers provided by
//  the C library. To ensure that the data is physically stored on disk
//  the kernel buffers must be flushed too, for example, with sync(2) or
//  fsync(2).
//
//  The wrapper nulls the fp, and executes sync() as available.
//
//  #include <stdio.h>
//
//  @fp: The file pointer to close.
//
//  Returns 0 upon completion, else -1.
//*/
//int lothars__fclose_null(FILE **fp)
//{
//	int res = -1;
//	if (NULL == *fp) {
//		err_msg("%() fp was already null", __func__);
//		return -1;
//	}
//	if (0 != (res = fclose(*fp))) {
//		err_msg("%() error", __func__);
//		return -1;
//	}
//	*fp = NULL;
//#if defined(_BSD_SOURCE) || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
//	sync();
//#endif
//	return res;
//}


/*
  This wrapper reads from a file pointer via fgetc() characterwise

  NB: Allocates memory!

  @fp: The file/stream descriptor to read from via fgetc().
  @content: The text to be read.
  @content_size: The size of the text read.

  Returns 0 if successful, or -1 alternatively.
*/
int read_char(FILE *fp, char **content, unsigned long int *content_size)
{
	int ch = 0;
	unsigned long int idx = 0;
	const unsigned long int INITIAL_SIZE = *content_size;

	if (NULL == fp) {
		fprintf(stderr, "%s() error: fp was NULL\n", __func__);
		return -1;
	}
	if (0 == INITIAL_SIZE) {
		fprintf(stderr, "%s() error: content_size was 0\n", __func__);
		return -1;
	}

	while (EOF != (ch = fgetc(fp))) {
		(*content)[idx] = ch;
		if (idx >= (*content_size) - 2) {
			if (-1 == get_more_space(content, content_size, INITIAL_SIZE)) {
				err_sys("%s() error", __func__);
			}
		}
		++idx;
	}
	(*content)[idx] = '\0';
	return 0;
}


/*
  reads a formated stream into a char*

  NB: Allocates memory!

  UNSAFE:
  fscanf() reads into the buffer but if there is no more space, it
  will continue to write into that buffer!!!
  Therefore the magic number of e.g. 128 tokens is necessary here!!!
*/
int read_without_spaces(FILE *fp, char **content,
                        unsigned long int *content_size)
{
	if (NULL == fp) {
		fprintf(stderr, "%s() error: fp was NULL\n", __func__);
		return -1;
	}
	const unsigned long int PBUF_SIZE = 128;
	char pBuf[PBUF_SIZE];
	memset(pBuf, '\0', sizeof(pBuf));

	const unsigned long int INITIAL_SIZE = *content_size;
	unsigned long int idx = 1; // for the '\0' token
	while ((fscanf(fp, "%127s", pBuf)) != EOF) { // FIXME uses magic number        
		pBuf[sizeof(pBuf)-1] = '\0';
		idx += strlen(pBuf);
		if (idx >= *content_size) {
			if (-1 == get_more_space(content, content_size, INITIAL_SIZE)) {
				err_sys("%s() error", __func__);
			}
		}
		strncat(*content, pBuf, PBUF_SIZE);
	}
#ifdef DEBUG
	printf("%s", *content);
#endif
	return 0;
}


/*
  getline approach() - NOT ANSI C - NOT TESTED

  NB: Allocates memory!

  reads the whole file linewise into a char* with a buffer of the
  given size
*/
// CHECKED - OK
int read_linewise(FILE *fp, char **content, unsigned long int *content_size)
{
#ifdef DEBUG
	printf("\tfo::read_linewise(*fp, **content, *content_size)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - *content == NULL\n", (*content == NULL));
	printf("\t%i - *content_size\n", *content_size);
#endif
	if (fp == NULL)
		return -1;
	memset(*content, '\0', *content_size);
	unsigned long idx = 0;
	const unsigned long int INITIAL_SIZE = *content_size;
	rewind(fp);
	/*
	  #ifdef __unix__
	  int c;
	  size_t *tp = NULL;
	  char **gptr = NULL;

	  if((gptr = malloc(sizeof(*gptr))) == NULL) return -1;
	  *gptr = NULL;

	  // use getline() - unix, NOT ANSI!!! function not present!!!
	  while( (c=getline(gptr, tp, fp)) > 0){
	  idx += strlen(*gptr);
	  if(idx >= (*content_size-2)){
	  if(get_more_space(content, content_size, INITIAL_SIZE) == -1){
	  fprintf(stderr, "fo::read_linewise(*fp, char**, unsigned long int*) -
	  Failed!\n"); exit(EXIT_FAILURE);
	  }
	  }
	  strncat( *content, *gptr, strlen(*gptr));
	  }
	  *content_size = strlen(*content) + 1;

	  if(tp != NULL) free(tp); // dangerous!
	  free(gptr); // dangerous!
	  return 0;
	  #endif
	//*/
	char *pBuf = NULL;
	if ((pBuf = calloc(INITIAL_SIZE, sizeof(*pBuf))) == NULL)
		;
	memset(pBuf, '\0', INITIAL_SIZE);
	idx = 1; // 1 because of '\0'
	while ((fgets(pBuf, (INITIAL_SIZE), fp) != NULL) &&
	       (*pBuf != EOF)) {         // EOF or EOL?
		pBuf[INITIAL_SIZE - 1] = '\0'; // fgets doesn't set '\0' (?)
		idx += strlen(pBuf);
		if (idx > *content_size) {
			if (get_more_space(content, content_size, INITIAL_SIZE) == -1) {
				fprintf(
					stderr,
					"fo::read_linewise(*fp, char**, unsigned long int*) - Failed!\n");
				free(pBuf);
				return -1;
			}
		}
		strncat(*content, pBuf, INITIAL_SIZE);
	}
	free(pBuf);
	return 0;
}


/*
  reads blockwise into a char* from a "R'n'B"-opened file ;)

// CHECKED - OK
*/
int read_blockwise(FILE *fp, char *content, const unsigned int CONTENT_SIZE)
{
#ifdef DEBUG
	printf("\tfo::read_blockwise(*fp, *content, objCount)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - content == NULL\n", (content == NULL));
#endif
	if (fp == NULL)
		return -1;

	unsigned int count = fread(content, sizeof(*content), CONTENT_SIZE, fp);
	if (count > CONTENT_SIZE)
		return -1;
	content[count] = '\0';

	return 0;
}


/***

    write to a FILE*

***/


/*
  writes characterwise from content into a FILE pointer
//*/
// CHECKED - OK
int write_char(FILE *fp, char *content, const unsigned long int CONTENT_SIZE)
{
#ifdef DEBUG
	printf("\tfo::write_char(*fp, *content, content_size)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - content\n", content);
	printf("\t%i - CONTENT_SIZE\n", CONTENT_SIZE);
#endif
	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;
	int c;
	unsigned long int idx = 0;
	while ((c = *content++) != '\0') {
		++idx;
		if (idx >= CONTENT_SIZE) {
			fprintf(stderr,
				"fo::write_char(FILE*, char*, unsigned long int) - Failed!\n");
			return -1;
		}
		putc(c, fp);
	}
	return 0;
}


/*
  writes formated (using printf) into FILE* from char*
  - unsafe, no fixed length -
  - needs fixed formatting (hardcoded!) -
//*/
// CHECKED - OK
int write_formated(FILE *fp, char *content)
{
#ifdef DEBUG
	printf("\tfo::write_formated(*fp, *content)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - content\n", content);
#endif

	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;
	fprintf(fp, "%s", content); // needs extern formatting!

	return 0;
}


/*
  writes linewise into FILE* from char*
  - changes the string of the file pointer fp ->???!!!
//*/
// CHECKED - OK
int write_linewise(FILE *fp, char *content,
                   const unsigned long int CONTENT_SIZE)
{
#ifdef DEBUG
	printf("\tfo::write_linewise(*fp, *content, lenContent)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - content\n", content);
	printf("\t%i - CONTENT_SIZE\n", CONTENT_SIZE);
#endif
	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;

	char bufLine[BUFSIZ];
	int idxLine = 0;
	int idxContent = 0;
	char *pData = &content[0];
	//  strncpy(bufLine, "", 1); // TODO: test
	memset(bufLine, '\0', BUFSIZ);
	while ((idxLine < BUFSIZ) && (idxContent < CONTENT_SIZE) &&
	       ((bufLine[idxLine] = *(pData++)) != '\0')) {

		if (idxLine >= BUFSIZ) {
			fprintf(
				stderr,
				"fo::write_linewise(FILE*, char*, const unsigned long) - Failed!\n");
			return -1;
		}

		if (((idxLine == CONTENT_SIZE - 2) && (bufLine[idxLine] != '\n')) ||
		    (*(pData + 1) == '\0')) {
			bufLine[idxLine + 1] = '\0';
			fputs(bufLine, fp); // write line
			break;

		} else if (bufLine[idxLine] == '\n') {
			fputs(bufLine, fp); // write line
			idxLine = 0;
		} else {
			++idxLine;
		}
		++idxContent;
	}
	fputs("\n\0", fp); // tailing linefeed + linebreak
	return 0;
}


/***

    file operations

***/


/*
  create_file() - Create a file with a given size. At last put a marker and 'x', then
  close the file pointer.

  @filename: The path and name of the file.
  @filesize: the provided size of the file.
*/
// TODO what is this good for??!!!
// CHECKED - OK
int create_file(const char *filename, const unsigned long int filesize)
{
	if (filename == NULL)
		return -1;

	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "%s() failed!\n", __func__);
		return -1;
	}
	fseek(fp, filesize - 1, SEEK_SET);
	putc('x', fp);
	fclose(fp);

	return 0;
}


/*
  remove a file
  - in case use a shred_file, remove only frees the bytes, but doesn't remove
  the content
//*/
// CHECKED - OK (used by shred_file() )
int remove_file(const char *path)
{
#ifdef DEBUG
	printf("\tfo::remove_file(*path)\n");
	printf("\t\'%s\' - path\n", path);
#endif
	if (path == NULL)
		return -1;
	if (remove(path) < 0) {
		fprintf(stderr, "fo::remove_file(char*) - Failed!\n");
		return -1;
	}
	return 0;
}


/*
  shred a file
  - depending on remove_file(const char* path)
*/
// CHECKED - OK
int shred_file(const char *path)
{
	unsigned long count;
	FILE *fp;

	if (path == NULL)
		return -1;

	fp = fopen(path, "w+b");
	if (fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		count = ftell(fp);
		fwrite("", 1, count, fp);
		fclose(fp);
		return remove_file(path);
	}
	return -1;
}


/*
  rename a file
//*/
// CHECKED - OK
int rename_file(const char *oldname, const char *newname)
{
#ifdef DEBUG
	printf("\tfo::rename_file(*oldname, *newname)\n");
	printf("\t\'%s\' - oldname\n", oldname);
	printf("\t\'%s\' - newname\n", newname);
#endif
	if (oldname == NULL)
		return -1;
	if (newname == NULL)
		return -1;
	if (rename(oldname, newname) < 0) {
		fprintf(stderr, "fo::rename_file(const char*, const char*) - Failed!\n");
		return -1;
	}
	return 0;
}


/*
  copy a file unbuffered characterwise
//*/
// CHECKED - OK
int copy_characterwise_unbuffered(const char *src, const char *dest)
{
#ifdef DEBUG
	printf("\tfo::copy_characterwise_unbuffered(*src, *dest)\n");
	printf("\t\'%s\' - src\n", src);
	printf("\t\'%s\' - dest\n", dest);
#endif
	if (src == NULL)
		return -1;
	if (dest == NULL)
		return -1;

	FILE *fpSrc, *fpDest;
	int c;

	// characterwise
	fpSrc = fopen(src, "rb");
	if (fpSrc != NULL) {
		fpDest = fopen(dest, "wb");
		if (fpDest == NULL) {
			fprintf(stderr, "fo::copy_characterwise_unbuffered(const char*, const "
				"char*) - Failed!\n");
			return -1;
		}
	} else {
		fprintf(stderr, "fo::copy_characterwise_unbuffered(const char*, const "
			"char*) - Failed!\n");
		return -1;
	}

	// unbuffered
	setvbuf(fpSrc, NULL, _IONBF, BUFSIZ);
	setvbuf(fpDest, NULL, _IONBF, BUFSIZ);

	while ((c = getc(fpSrc)) != EOF)
		putc(c, fpDest);

	fclose(fpSrc);
	fclose(fpDest);

	return 0;
}


/*
  copy a file buffered characterwise - provide a buffer size
//*/
// CHECKED - OK
int copy_characterwise_buffered(const char *src, const char *dest,
                                const unsigned long int BUFFER_SIZE)
{
#ifdef DEBUG
	printf("\tfo::copy_characterwise_buffered(*src, *dest, BUFFER_SIZE)\n");
	printf("\t\'%s\' - src\n", src);
	printf("\t\'%s\' - dest\n", dest);
	printf("\t%i - BUFFER_SIZE\n", BUFFER_SIZE);
#endif
	if (src == NULL)
		return -1;
	if (dest == NULL)
		return -1;

	//  unsigned long BUF = BUFFER_SIZE;
	FILE *fpSrc, *fpDest;
	int c;

	// characterwise
	fpSrc = fopen(src, "rb");
	if (fpSrc != NULL) {
		fpDest = fopen(dest, "wb");
		if (fpDest == NULL) {
			fprintf(stderr, "fo::copy_characterwise_buffered(const char*, const "
				"char*) - Failed!\n");
			return -1;
		}
	} else {
		fprintf(stderr, "fo::copy_characterwise_buffered(const char*, const char*) "
			"- Failed!\n");
		return -1;
	}

	// buffered
	setvbuf(fpSrc, NULL, _IOFBF, BUFFER_SIZE);
	setvbuf(fpDest, NULL, _IOFBF, BUFFER_SIZE);

	while ((c = getc(fpSrc)) != EOF)
		putc(c, fpDest);

	fclose(fpSrc);
	fclose(fpDest);

	return 0;
}


/***

    temp file operations

***/


/*
  number of possible temp files
//*/
// CHECKED - OK
unsigned int number_of_tempfiles()
{
#ifdef DEBUG
	printf("\tfo::number_of_tempfiles()\n");
#endif

	return TMP_MAX;
}


/*
  number of possible characters in the static tmpnam(NULL)
  - Attention: uses and overwrites always in the same space!
//*/
// CHECKED - OK
unsigned int number_of_characters_in_static_temp()
{
#ifdef DEBUG
	printf("\tfo::number_of_characters_in_static_temp()\n");
#endif

	return L_tmpnam;
}


/*
  create a temp file
  - don't forget to close and remove the tmp file, (like any other FILE*)
  - use the tmp like any other file
  - should work ;)

  - dangerous!!! better use mkstemp() than tmpnam()

// CHECKED - OK
int create_tmp(FILE** fp, char szTmp[L_tmpnam])
{
#ifdef DEBUG
	printf("\tcreate_tmp(**fp, szTmp[])\n");
	printf("\t%i - *fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%s - szTmp\n", szTmp);
#endif
	if(*fp != NULL) return -1;
	if(szTmp == NULL){
		szTmp = tmpnam(szTmp);
// create - no name given..
		if( (*fp = fopen(szTmp, "rw")) == NULL){
			fprintf(stderr, "fo::create_fmp(FILE*, char[]) - Failed!\n");
			return -1;
		}
	}else{
		if((*fp = tmpfile()) == NULL){
			fprintf(stderr, "fo::create_tmp(FILE*, char[]) - Failed!\n");
			return -1;
		}
	}

	return 0;
}
//*/

/*
  closes a temporary file

// CHECKED - OK
int close_tmp(FILE** fp)
{
#ifdef DEBUG
printf("\tfo::close_tmp(**fp)\n");
printf("\t%i - fp == NULL\t", (*fp == NULL));
printf("\t%i - &*fp\n", &*fp);
#endif
if(fp == NULL) return -1;
rewind(*fp); // reset the filestream!
fclose(*fp);
*fp = NULL;

return 0;
}
//*/


/***

    other operations

***/


/*
  reads the size of the file into an unsigned long int*,
  returns -1 in case of error, else 0
//*/
// CHECKED - OK
int filesize(FILE *fp, unsigned long int *size)
{
#ifdef DEBUG
	printf("\tfo::filesize(file[], *size\n");
	printf("\t\'%s\' - file\n", file);
	printf("\t%i - size == NULL\n", (size == NULL));
#endif
	if (size == NULL)
		return -1;
	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	return 0;
}


/*
  checks for EOF - returns 1 if no EOF is found,
  else 0, and -1 if the file pointer was corrupt
//*/
// CHECKED - OK
int check_eof(FILE *fp)
{
#ifdef DEBUG
	printf("\tfo::check_eof(*fp)\n");
	printf("\t%i - fp == NULL\t", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
#endif
	if (fp == NULL)
		return -1;
	unsigned long size = 0;
	filesize(fp, &size);

	int cnt;
	for (cnt = 0; (cnt < size) && !feof(fp); ++cnt)
		;
	if (cnt == size)
		return 1;

	return 0;
}


/*
  check file for errors in a file
//*/
// CHECKED - OK
int check_error(FILE *fp)
{
#ifdef DEBUG
	printf("\tfo::check_error(*fp)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
#endif
	if (fp == NULL)
		return -1;
	rewind(fp); // reset filestream
	if (ferror(fp)) {
		clearerr(fp);
		return -1;
	} else
		return 0;
}


/*
  reads without the EOF character into a given Buffer - OK
//*/
// CHECKED - OK
int read_without_eof(FILE *fp, char *content, const unsigned int CONTENT_SIZE)
{
#ifdef DEBUG
	printf("\tfo::read_wo_eof(*fp, *content, CONTENT_SIZE)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - content == NULL\n", (content == NULL));
	printf("\t%i - CONTENT_SIZE\n", CONTENT_SIZE);
#endif
	if (fp == NULL)
		return -1;

	int c;
	unsigned long int idx = 0;
	while ((c = getc(fp)) != 0) {
#ifdef DEBUG
		printf("%i. #", idx);
		putchar(c); // print reading out on screen
		printf("#\n");
#endif
		unsigned int over_read = 100; // to be set
		if (idx >=
		    CONTENT_SIZE +
		    over_read) { // jumps out when read size (+ over_space) expired
			break;
		}
		//*/
		if (c == EOF)
			ungetc(c, fp); // This is rather experimental !!!!
		if (c == '\0')
			c = 'x'; // to be set instead of '\0'
		else
			content[idx] = c;
		++idx;
	}
	return 0;
}


/*
  read line number x into char* lineContent
  The Buffer should be set up to cover one line!
//*/
// CHECKED - OK
int read_nth_line(FILE *fp, char *line, const unsigned long int LINE_SIZE,
                  const unsigned long int LINE_NUMBER)
{
#ifdef DEBUG
	printf("\tfo::read_nth_line(*fp, *line, LINE_SIZE, LINE_NUMBER)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - line == NULL\n", (line == NULL));
	printf("\t%i - LINE_SIZE\n", LINE_SIZE);
	printf("\t%i - LINE_NUMBER\n", LINE_NUMBER);
#endif
	if (fp == NULL)
		return -1;
	rewind(fp); // reset filestream
	char temp[LINE_SIZE];
	int cnt = 0;
	for (cnt = 0; cnt < LINE_NUMBER - 1; ++cnt)
		if (fgets(temp, LINE_SIZE, fp) == NULL)
			return -1;
	if (fgets(temp, LINE_SIZE, fp) == NULL)
		return -1;
	strncat(line, temp, LINE_SIZE);

	return 0;
}


/*
  reads a line from start (inclusive) to end (inclusive)
//*/
// CHECKED - OK
int read_lines(FILE *fp, char *lines, const unsigned long int LINES_SIZE,
               const unsigned long int start, const unsigned long int end)
{
#ifdef DEBUG
	printf("\tfo::read_lines(*fp, *lines, start, end)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - lines == NULL\n", (lines == NULL));
	printf("\t%i - LINES_SIZE\n", LINES_SIZE);
	printf("\t%i - start\n", start);
	printf("\t%i - end\n", end);
#endif
	if (fp == NULL)
		return -1;
	rewind(fp); // reset filestream
	if ((start == 0) || (end == 0))
		return -1;
	const unsigned long int iStart = start - 1;
	const unsigned long int iEnd = end - 1;
	if (iEnd < iStart)
		return -1;
	char temp[LINES_SIZE];
	int cnt = 0;
	for (cnt = 0; cnt < iStart; ++cnt) {
		// strcpy(temp, ""); // TODO: rm
		memset(temp, '\0', LINES_SIZE);
		if (fgets(temp, LINES_SIZE, fp) == NULL)
			return -1;
	}
	for (; cnt <= iEnd; ++cnt) {
		if (fgets(temp, LINES_SIZE, fp) == NULL)
			return -1;
		strncat(lines, temp, LINES_SIZE);
	}
	return 0;
}


/*
  Reads lines into char* that contain a certain pattern (also within a
  word).

  @fp: The file descriptor to read from.
  @lines: The string to be appended to.
  @lines_size: The size of the string to append.
  @pattern: The pattern to watch for.

  Returns o in case of success, alternatively -1 is returned.
*/
int read_lines_with_pattern(FILE *fp, char *lines,
                            const unsigned long int lines_size, char *pattern)
{
	if (NULL == fp)
		return -1;
	rewind(fp); // reset filestream
	if (NULL == pattern)
		return -1;
	char temp[lines_size];
	while (NULL != fgets(temp, lines_size, fp)) {
		if (0 != strstr(temp, pattern)) {
			strncat(lines, temp, lines_size);
			strncat(lines, "\n", 2);
			// counter++; // for linenumbers
		}
	}
	return 0;
}


/***

    Buffer operations

***/


/*
  does a save re-allocation for char - or is supposed to do so.

  @str: Points to the string to be extended.
  @str_size: The size of the string.
  @nbytes: How many bytes more?

  Returns 0 in case of success and -1 in case of error.
*/
int get_more_space(char **str, unsigned long int *str_size,
                   const unsigned long int nbytes)
{
	char *tmp = NULL;

	// using realloc makes using strncat() here unnecessary
	if (NULL == (tmp = realloc(*str, (*str_size + nbytes)))) {
		if (NULL != tmp)
			free(tmp);
		tmp = NULL;
		return -1;
	}
	// using realloc makes free(*str) here unnecessary
	*str = tmp;
	*str_size += nbytes;
	return 0;
}


/*
  writes a struct blockwise into a "w+b" opened FILE* from a char*
  this method needs either a typedef or a struct by the name "Data"

// CHECKED - OK (not used, used for writing structs blockwise!)
int write_blockwise(FILE* fp, struct Data content)
{
#ifdef DEBUG
	printf("write_blockwise(fp, \'%s\')\n", content);
	printf("\tfp == NULL: %i\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
#endif
	if(fp == NULL) return -1;
	fwrite(&content, sizeof(struct Data), l, fp);

	return 0;
}
//*/



