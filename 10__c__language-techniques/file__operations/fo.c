/*
  fo.c

  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
*/

/**************************************************************************

  implementations of some common file operations

**************************************************************************/

#include "fo.h"

// use for debugging output
//#define DEBUG

/***
  extern's
//*/
/*
extern char* strncat(char*, const char*, size_t); // better thant strcat(), same
reason extern size_t strlen(const char*); extern char* strstr(const char*, const
char*); extern void* memset(void*, const void*, size_t);
//*/
/***

    get a FILE pointer

***/

/*
  sets a valid read/write file pointer, 0 - ok, -1 - failed
//*/
// CHECKED - OK
int get_read_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
#ifdef DEBUG
	printf("\tfo::get_read_write_file_pointer(**fp, filename[])\n");
	printf("\t%i - fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - filename\n", filename);
#endif
	if (NULL != *fp)
		return -1;
	if (NULL == filename)
		return -1;
	if (NULL == (*fp = fopen(filename, "rw"))) {
		fprintf(stderr,
			"fo::get_read_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}
	return 0;
}

/*
  sets a valid read FILE*, 0 - ok, -1 - failed
//*/
// CHECKED - OK
int get_read_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
#ifdef DEBUG
	printf("\tfo::get_read_file_pointer(**fp, filename[])\n");
	printf("\t%i - fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - filename\n", filename);
#endif
	if (NULL != *fp)
		return -1;
	if (NULL == filename)
		return -1;
	if (NULL == (*fp = fopen(filename, "r"))) {
		perror("fo::get_read_file_pointer(FILE**, char[] - Failed!");
		return -1;
	}

	return 0;
}

/*
  sets a write file pointer, 0 - ok, -1 - failed
//*/
// CHECKED - OK
int get_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
#ifdef DEBUG
	printf("\tfo::get_write_file_pointer(**fp, filename[])\n");
	printf("\t%i - fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - filename\n", filename);
#endif
	if (NULL != *fp)
		return -1;
	if (NULL == filename)
		return -1;
	if (NULL == (*fp = fopen(filename, "w"))) {
		fprintf(stderr,
			"fo::get_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}

#ifdef DEBUG
	printf("\t%i - &*fp\n", &*fp);
#endif

	return 0;
}

/*
  sets an append file pointer, 0 - ok, -1 - failed
//*/
// CHECKED - OK
int get_append_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
#ifdef DEBUG
	printf("\tfo::get_append_file_pointer(**fp, filename[])\n");
	printf("\t%i - fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t\'%s\' - filename\n", filename);
#endif
	if (NULL != *fp)
		return -1;
	if (NULL == filename)
		return -1;
	if (NULL == (*fp = fopen(filename, "a"))) {
		fprintf(stderr,
			"fo::get_apprend_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}

	return 0;
}

/***

    read from a FILE*

***/

/*
  read characterwise into a char*
//*/
// CHECKED - OK
int read_char(FILE *fp, char **content, unsigned long int *content_size)
{
#ifdef DEBUG
	printf("\tfo::read_char(*fp, **content, *content_size)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - *content == NULL\n", (*content == NULL));
	printf("\t%i - *content_size\n", *content_size);
#endif
	if (NULL == fp)
		return -1;
	if (0 == *content_size)
		return -1;

	int c = 0;
	unsigned long int idx = 0;
	const unsigned long int INITIAL_SIZE = *content_size;

	// if pTemp full, append and allocate new space
	while (EOF != (c = fgetc(fp))) {
#ifdef DEBUG
		putchar(c); // print out on screen
#endif
		(*content)[idx] = c;
		if (idx >= (*content_size) - 2) {
			if (get_more_space(content, content_size,
					   INITIAL_SIZE) == -1) {
				fprintf(stderr,
					"fo::read_char(*fp, char**, unsigned long int*) - Failed!\n");
				exit(EXIT_FAILURE);
			}
		}
		++idx;
	}
	(*content)[idx] = '\0';
	return 0;
}

/*
  reads a formated stream into a char*

  UNSAFE:
  fscanf() reads into the buffer but if there is no more space, it
  will continue to write into that buffer!!!
  Therefore the magic number of e.g. 128 tokens is necessary here!!!
//*/
// CHECKED - OK
int read_without_spaces(FILE *fp, char **content,
			unsigned long int *content_size)
{
#ifdef DEBUG
	printf("\tfo::read_formated(*fp, **content, *content_size)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - *content == NULL\n", (*content == NULL));
	printf("\t%i - *content_size\n", *content_size);
#endif

	if (NULL == fp)
		return -1;

	char pBuf[128];
	memset(pBuf, '\0', 128); // magic number ;-)

	const unsigned long int INITIAL_SIZE = *content_size;
	unsigned long int idx = 1; // for the '\0' token
	while ((fscanf(fp, "%127s", pBuf)) != EOF) {
		pBuf[128] = '\0';
		idx += strlen(pBuf);
		if (idx >= *content_size) {
			if (get_more_space(content, content_size,
					   INITIAL_SIZE) == -1) {
				fprintf(stderr,
					"fo::read_without_spaces(*fp, char**, unsigned long "
					"int*) - Failed!\n");
				exit(EXIT_FAILURE);
			}
		}
		strncat(*content, pBuf, 128);
	}
#ifdef DEBUG
	printf("%s", *content);
#endif

	return 0;
}

/*
  getline approach() - NOT ANSI C - NOT TESTED

  reads the whole file linewise into a char* with a buffer of the given size
//*/
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
	if (NULL == fp)
		return -1;

	memset(*content, '\0', *content_size);
	unsigned long idx = 0;
	const unsigned long int INITIAL_SIZE = *content_size;

	rewind(fp); // reset to start

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

	if (NULL == (pBuf = calloc(INITIAL_SIZE, sizeof(*pBuf))))
		;
	memset(pBuf, '\0', INITIAL_SIZE);
	idx = 1; // 1 because of '\0'
	while (NULL != (fgets(pBuf, (INITIAL_SIZE), fp)) &&
	       (EOF != *pBuf)) { // EOF or EOL?
		pBuf[INITIAL_SIZE - 1] = '\0'; // fgets doesn't set '\0' (?)
		idx += strlen(pBuf);
		if (idx > *content_size) {
			if (-1 == get_more_space(content, content_size,
						 INITIAL_SIZE)) {
				fprintf(stderr,
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
//*/
int read_blockwise(FILE *fp, char *content, const unsigned int CONTENT_SIZE)
{
#ifdef DEBUG
	printf("\tfo::read_blockwise(*fp, *content, objCount)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - content == NULL\n", (content == NULL));
#endif
	if (NULL == fp)
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
	if (NULL == fp)
		return -1;
	if (NULL == content)
		return -1;

	int c;
	unsigned long int idx = 0;

	while ('\0' != (c = *content++)) {
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

	if (NULL == fp)
		return -1;
	if (NULL == content)
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
	if (NULL == fp)
		return -1;
	if (NULL == content)
		return -1;

	char bufLine[BUFSIZ];
	memset(bufLine, '\0', BUFSIZ);
	int idxLine = 0; // index within one line, until '\n'
	int idxContent = 0; // index for entire content, until '\0'
	char *pData = &content[0];

	while ((idxLine < BUFSIZ) && (idxContent < CONTENT_SIZE) &&
	       ((bufLine[idxLine] = *(pData++)) != '\0')) {
		if (idxLine >= BUFSIZ) {
			fprintf(stderr,
				"fo::write_linewise(FILE*, char*, const unsigned long) - Failed!\n");
			return -1;
		}

		if (((idxLine == CONTENT_SIZE - 2) &&
		     (bufLine[idxLine] != '\n')) ||
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
  create a file with a given size
//*/
// CHECKED - OK
int create_file(const char *filename, const unsigned long int SIZE)
{
#ifdef DEBUG
	printf("\tfo::create_file(*filename, SIZE)\n");
	printf("\t%i - SIZE\n", SIZE);
#endif
	if (NULL == filename)
		return -1;

	FILE *fp = fopen(filename, "wb");
	if (NULL == fp) {
		fprintf(stderr,
			"fo::create_file(const char*, const unsigned long int) - Failed!\n");
		return -1;
	}
	fseek(fp, SIZE - 1, SEEK_SET);
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
	if (NULL == path)
		return -1;
	if (0 > remove(path)) {
		fprintf(stderr, "fo::remove_file(char*) - Failed!\n");
		return -1;
	}

	return 0;
}

/*
  shred a file
  - depending on remove_file(const char* path)
//*/
// CHECKED - OK
int shred_file(const char *path)
{
#ifdef DEBUG
	printf("\tfo::shred_file(*path)\n");
	printf("\t\'%s\' - path\n", path);
#endif
	if (NULL == path)
		return -1;

	unsigned long count;
	FILE *fp = fopen(path, "w+b");

	if (NULL != fp) {
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
	if (NULL == oldname)
		return -1;
	if (NULL == newname)
		return -1;
	if (0 > rename(oldname, newname)) {
		fprintf(stderr,
			"fo::rename_file(const char*, const char*) - Failed!\n");
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
	if (NULL == src)
		return -1;
	if (NULL == dest)
		return -1;

	FILE *fpSrc, *fpDest;
	int c;

	// characterwise
	fpSrc = fopen(src, "rb");
	if (NULL != fpSrc) {
		fpDest = fopen(dest, "wb");
		if (NULL == fpDest) {
			fprintf(stderr,
				"fo::copy_characterwise_unbuffered(const char*, const "
				"char*) - Failed!\n");

			return -1;
		}
	} else {
		fprintf(stderr,
			"fo::copy_characterwise_unbuffered(const char*, const "
			"char*) - Failed!\n");

		return -1;
	}

	// unbuffered
	setvbuf(fpSrc, NULL, _IONBF, BUFSIZ);
	setvbuf(fpDest, NULL, _IONBF, BUFSIZ);
	/*
    old Version
    setbuf(fpSrc, NULL);
    setbuf(fpDest, NULL);
  //*/

	while (EOF != (c = getc(fpSrc))) {
		putc(c, fpDest);
	}

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
	if (NULL == src)
		return -1;
	if (NULL == dest)
		return -1;

	//  unsigned long BUF = BUFFER_SIZE;
	FILE *fpSrc = NULL, *fpDest = NULL;
	int c = -1;

	// characterwise
	fpSrc = fopen(src, "rb");
	if (NULL != fpSrc) {
		fpDest = fopen(dest, "wb");
		if (NULL == fpDest) {
			fprintf(stderr,
				"fo::copy_characterwise_buffered(const char*, const "
				"char*) - Failed!\n");
			return -1;
		}
	} else {
		fprintf(stderr,
			"fo::copy_characterwise_buffered(const char*, const char*) "
			"- Failed!\n");
		return -1;
	}

	// buffered
	setvbuf(fpSrc, NULL, _IOFBF, BUFFER_SIZE);
	setvbuf(fpDest, NULL, _IOFBF, BUFFER_SIZE);

	/*
    old Version
    setbuf(fpSrc, NULL);
    setbuf(fpDest, NULL);
  //*/

	while ((c = getc(fpSrc)) != EOF) {
		putc(c, fpDest);
	}

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
//*/
// CHECKED - OK
int create_tmp(FILE **fp, char szTmp[L_tmpnam])
{
#ifdef DEBUG
	printf("\tcreate_tmp(**fp, szTmp[])\n");
	printf("\t%i - *fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%s - szTmp\n", szTmp);
#endif
	int fd = -1;

	if (NULL != *fp)
		return -1;
	if (NULL == szTmp) {
		strncpy(szTmp, "/tmp/tmpFile-XXXXXX", 21);
		if (0 > (fd = mkstemp(szTmp))) {
			fprintf(stderr, "mkstemp() - Failed!\n");
			return -1;
		}
		unlink(szTmp); // will be removed at reboot automatically

		// create - no name given..
		if (NULL == (*fp = fopen(szTmp, "rw"))) {
			fprintf(stderr,
				"fo::create_fmp(FILE*, char[]) - Failed!\n");
			return -1;
		}
	} else {
		if (NULL == (*fp = tmpfile())) {
			fprintf(stderr,
				"fo::create_tmp(FILE*, char[]) - Failed!\n");
			return -1;
		}
	}

	return 0;
}

/*
  closes a temporary file
//*/
// CHECKED - OK
int close_tmp(FILE **fp)
{
#ifdef DEBUG
	printf("\tfo::close_tmp(**fp)\n");
	printf("\t%i - fp == NULL\t", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
#endif
	if (NULL == fp)
		return -1;

	rewind(*fp); // reset the filestream!
	fclose(*fp);
	*fp = NULL;

	return 0;
}

/***

    other operations

***/

/*
  closes the open FILE*
//*/
// CHECKED - OK
int close_stream(FILE **fp)
{
#ifdef DEBUG
	printf("\tfo::close_stream(**fp)\n");
	printf("\t%i - *fp == NULL\n", (*fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
#endif
	if (NULL == *fp)
		return -1;

	int iRes = fclose(*fp);

	*fp = NULL;
#ifdef DEBUG
	printf("\t%i - *fp == NULL\n", (*fp == NULL));
#endif

	return iRes;
}

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
	if (NULL == size)
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
	if (NULL == fp)
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
	if (NULL == fp) {
		return -1;
	}

	rewind(fp); // reset filestream
	if (ferror(fp)) {
		clearerr(fp);

		return -1;
	}

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
	if (NULL == fp)
		return -1;

	int c;
	unsigned long int idx = 0;

	while (0 != (c = getc(fp))) {
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

		if (EOF == c)
			ungetc(c, fp); // This is rather experimental !!!!
		if ('\0' == c)
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
	if (NULL == fp)
		return -1;

	rewind(fp); // reset filestream

	char temp[LINE_SIZE];
	int cnt = 0;
	for (cnt = 0; cnt < LINE_NUMBER - 1; ++cnt) {
		if (NULL == fgets(temp, LINE_SIZE, fp)) {
			return -1;
		}
	}
	if (NULL == fgets(temp, LINE_SIZE, fp))
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
	if (NULL == fp)
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
		if (NULL == fgets(temp, LINES_SIZE, fp))
			return -1;
	}
	for (; cnt <= iEnd; ++cnt) {
		if (NULL == fgets(temp, LINES_SIZE, fp))
			return -1;
		strncat(lines, temp, LINES_SIZE);
	}

	return 0;
}

/*
  reads lines into char* that contain a certain pattern (also within a word)
//*/
// CHECKED - OK
int read_lines_with_pattern(FILE *fp, char *lines,
			    const unsigned long int LINES_SIZE, char *pattern)
{
#ifdef DEBUG
	printf("\tfo::read_lines_with_pattern(*fp, *lines, *pattern)\n");
	printf("\t%i - fp == NULL\n", (fp == NULL));
	printf("\t%i - &*fp\n", &*fp);
	printf("\t%i - lines == NULL\n", (lines == NULL));
	printf("\t%i - LINES_SIZE\n", LINES_SIZE);
	printf("\t\'%s\' - pattern\n", pattern);
#endif
	if (NULL == fp)
		return -1;
	rewind(fp); // reset filestream
	if (NULL == pattern)
		return -1;
	char temp[LINES_SIZE];
	memset(temp, '\0', LINES_SIZE);
	while (NULL != fgets(temp, LINES_SIZE, fp)) {
		if (0 != strstr(temp, pattern)) {
			strncat(lines, temp, LINES_SIZE);
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
  get the BUFSIZE - the read/write buffer size (dependent on the compiler)
//*/
// CHECKED - OK
int get_bufsize()
{
#ifdef DEBUG
	printf("\tfo::get_bufsize()\n");
#endif

	return BUFSIZ;
}

/*
  does a save re-allocation for char - or is supposed to do so, returns 0 in
case of success and -1 in case of error
//*/
// CHECKED - OK
int get_more_space(char **str, unsigned long int *str_size,
		   const unsigned long int how_much_more)
{
#ifdef DEBUG
	printf("\tfo::get_more_space(**str, *str_size, how_much_more)\n");
	printf("\t\'%s\' - *str\n", *str);
	printf("\t%i - *str_size\n", *str_size);
	printf("\t%i - how_much_more\n", how_much_more);
#endif
	char *tmp = NULL;

	// using realloc makes using strncat() here unnecessary
	if (NULL == (tmp = realloc(*str, (*str_size + how_much_more)))) {
		if (NULL != tmp)
			free(tmp);
		tmp = NULL;

		return -1;
	}
	// using realloc makes free(*str) here unnecessary
	*str = tmp;
	*str_size += how_much_more;

	return 0;
}

/*
  writes a struct blockwise into a "w+b" opened FILE* from a char*
  this method needs either a typedef or a struct by the name "Data"
* /
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
