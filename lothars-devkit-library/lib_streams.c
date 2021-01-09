// wrap_stdio.c
/*
  wrapper for streams and file streams
*/



// forwards

void lothars__fclose(FILE *);
FILE* lothars__fdopen(int, const char *);
char* lothars__fgets(char *, int, FILE *);
FILE* lothars__fopen(const char *, const char *);
void lothars__fputs(const char *, FILE *);



// implementation

/*
  The fclose() function flushes the stream pointed to by fp (writing
  any buffered output data using fflush(3)) and closes the underlying
  file descriptor.

  #include <stdio.h>

  @fp: The specified stream.
*/
void lothars__fclose(FILE *fp)
{
	if (0 != fclose(fp)) {
                err_sys("%s() error", __func__);
	}
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
  The fopen() function opens the file whose name is the string pointed
  to by path and associates a stream with it.

  The argument mode points to a string beginning with one of the
  following sequences: r, r+, w, w+, a or a+ (see manpage).

  #include <stdio.h>

  @filename: The file to open.
  @mode: The argument mode.
*/
FILE* lothars__fopen(const char *filename, const char *mode)
{
	FILE *fp = NULL;
	if (NULL == (fp = fopen(filename, mode))) {
                err_sys("%s() error", __func__);
	}
	return fp;
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
