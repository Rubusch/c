// fo.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 */
/**************************************************************************

  implementations of some common file operations

**************************************************************************/

#include "fo.h"

// use for debugging output
//#define DEBUG

/***
  extern's
***/

extern char *strcpy(char *, const char *);
extern char *strncat(char *, const char *, size_t);
extern size_t strlen(const char *);
extern char *strstr(const char *, const char *);
extern void *memset(void *, const void *, size_t);

/***

    get a FILE pointer

***/

/*
  sets a valid read/write file pointer, 0 - ok, -1 - failed
//*/
int get_read_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	if (*fp != NULL)
		return -1;

	if (filename == NULL)
		return -1;
	if ((*fp = fopen(filename, "rw")) == NULL) {
		fprintf(stderr,
			"fo::get_read_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}
	return 0;
}

/*
  sets a valid read FILE*, 0 - ok, -1 - failed
//*/
int get_read_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	if (*fp != NULL)
		return -1;
	if (filename == NULL)
		return -1;

	if ((*fp = fopen(filename, "r")) == NULL) {
		perror("fo::get_read_file_pointer(FILE**, char[] - Failed!");
		return -1;
	}

	return 0;
}

/*
  sets a write file pointer, 0 - ok, -1 - failed
//*/
int get_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	if (*fp != NULL)
		return -1;
	if (filename == NULL)
		return -1;

	if ((*fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr,
			"fo::get_write_file_pointer(FILE**, char[]) - Failed!\n");
		return -1;
	}

	return 0;

	/*
    sets an append file pointer, 0 - ok, -1 - failed
  //*/
	int get_append_file_pointer(FILE * *fp, char filename[FILENAME_MAX])
	{
		if (*fp != NULL)
			return -1;
		if (filename == NULL)
			return -1;

		if ((*fp = fopen(filename, "a")) == NULL) {
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
	int read_char(FILE * fp, char **content,
		      unsigned long int *content_size)
	{
		if (fp == NULL)
			return -1;
		if (*content_size == 0)
			return -1;
		int c = 0;
		unsigned long int idx = 0;
		const unsigned long int INITIAL_SIZE = *content_size;

		// if pTemp full, append and allocate new space
		while ((c = fgetc(fp)) != EOF) {
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
	int read_without_spaces(FILE * fp, char **content,
				unsigned long int *content_size)
	{
		if (fp == NULL)
			return -1;
		char pBuf[128]; // uses magic number ;-)
		strcpy(pBuf, "\0");
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
		return 0;
	}

	/*
    getline approach() - NOT ANSI C - NOT TESTED

    reads the whole file linewise into a char* with a buffer of the given size
  //*/
	int read_linewise(FILE * fp, char **content,
			  unsigned long int *content_size)
	{
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
		       (*pBuf != EOF)) { // EOF or EOL?
			pBuf[INITIAL_SIZE - 1] =
				'\0'; // fgets doesn't set '\0' (?)
			idx += strlen(pBuf);
			if (idx > *content_size) {
				if (get_more_space(content, content_size,
						   INITIAL_SIZE) == -1) {
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
  //*/
	int read_blockwise(FILE * fp, char *content,
			   const unsigned int CONTENT_SIZE)
	{
		if (fp == NULL)
			return -1;

		unsigned int count =
			fread(content, sizeof(*content), CONTENT_SIZE, fp);
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
	int write_char(FILE * fp, char *content,
		       const unsigned long int CONTENT_SIZE)
	{
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
	int write_formated(FILE * fp, char *content)
	{
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
	int write_linewise(FILE * fp, char *content,
			   const unsigned long int CONTENT_SIZE)
	{
		if (fp == NULL)
			return -1;
		if (content == NULL)
			return -1;

		char bufLine[BUFSIZ];
		int idxLine = 0;
		int idxContent = 0;
		char *pData = &content[0];
		strcpy(bufLine, "");

		while ((idxLine < BUFSIZ) && (idxContent < CONTENT_SIZE) &&
		       ((bufLine[idxLine] = *(pData++)) != '\0')) {
			if (idxLine >= BUFSIZ) {
				fprintf(stderr,
					"fo::write_linewise(FILE*, char*, const unsigned long) "
					"- Failed!\n");
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
	int create_file(const char *filename, const unsigned long int SIZE)
	{
		if (filename == NULL)
			return -1;

		FILE *fp = fopen(filename, "wb");
		if (fp == NULL) {
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
	int remove_file(const char *path)
	{
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
  //*/
	int shred_file(const char *path)
	{
		if (path == NULL)
			return -1;
		unsigned long count;
		FILE *fp = fopen(path, "w+b");

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
	int rename_file(const char *oldname, const char *newname)
	{
		if (oldname == NULL)
			return -1;
		if (newname == NULL)
			return -1;
		if (rename(oldname, newname) < 0) {
			fprintf(stderr,
				"fo::rename_file(const char*, const char*) - Failed!\n");
			return -1;
		}
		return 0;
	}

	/*
    copy a file unbuffered characterwise
  //*/
	int copy_characterwise_unbuffered(const char *src, const char *dest)
	{
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

		while ((c = getc(fpSrc)) != EOF)
			putc(c, fpDest);

		fclose(fpSrc);
		fclose(fpDest);

		return 0;
	}

	/*
    copy a file buffered characterwise - provide a buffer size
  //*/
	int copy_characterwise_buffered(const char *src, const char *dest,
					const unsigned long int BUFFER_SIZE)
	{
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
				fprintf(stderr,
					"fo::copy_characterwise_buffered(const char*, const "
					"char*) - Failed!\n");
				return -1;
			}
		} else {
			fprintf(stderr,
				"fo::copy_characterwise_buffered(const char*, const "
				"char*) - Failed!\n");
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
	unsigned int number_of_tempfiles()
	{
		return TMP_MAX;
	}

	/*
    number of possible characters in the static tmpnam(NULL)
    - Attention: uses and overwrites always in the same space!
  //*/
	// CHECKED - OK
	unsigned int number_of_characters_in_static_temp()
	{
		return L_tmpnam;
	}

	/*
    create a temp file
    - don't forget to close and remove the tmp file, (like any other FILE*)
    - use the tmp like any other file
    - should work ;)
  //*/
	int create_tmp(FILE * *fp, char szTmp[L_tmpnam])
	{
		if (*fp != NULL)
			return -1;
		if (szTmp == NULL) {
			szTmp = tmpnam(szTmp);
			// create - no name given..
			if ((*fp = fopen(szTmp, "rw")) == NULL) {
				fprintf(stderr,
					"fo::create_fmp(FILE*, char[]) - Failed!\n");
				return -1;
			}
		} else {
			if ((*fp = tmpfile()) == NULL) {
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
	int close_tmp(FILE * *fp)
	{
		if (fp == NULL)
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
	int close_stream(FILE * *fp)
	{
		if (*fp == NULL)
			return -1;
		int iRes = fclose(*fp);
		*fp = NULL;

		return iRes;
	}

	/*
    reads the size of the file into an unsigned long int*,
    returns -1 in case of error, else 0
  //*/
	int filesize(FILE * fp, unsigned long int *size)
	{
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
	int check_eof(FILE * fp)
	{
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
	int check_error(FILE * fp)
	{
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
	int read_without_eof(FILE * fp, char *content,
			     const unsigned int CONTENT_SIZE)
	{
		if (fp == NULL)
			return -1;

		int c;
		unsigned long int idx = 0;
		while ((c = getc(fp)) != 0) {
			unsigned int over_read = 100; // to be set
			if (idx >=
			    CONTENT_SIZE +
				    over_read) { // jumps out when read size (+ over_space) expired
				break;
			}
			//*/
			if (c == EOF)
				ungetc(c,
				       fp); // This is rather experimental !!!!
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
	int read_nth_line(FILE * fp, char *line,
			  const unsigned long int LINE_SIZE,
			  const unsigned long int LINE_NUMBER)
	{
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
	int read_lines(FILE * fp, char *lines,
		       const unsigned long int LINES_SIZE,
		       const unsigned long int start,
		       const unsigned long int end)
	{
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
			strcpy(temp, "");
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
    reads lines into char* that contain a certain pattern (also within a word)
  //*/
	int read_lines_with_pattern(FILE * fp, char *lines,
				    const unsigned long int LINES_SIZE,
				    char *pattern)
	{
		if (fp == NULL)
			return -1;
		rewind(fp); // reset filestream
		if (pattern == NULL)
			return -1;
		char temp[LINES_SIZE];
		while (fgets(temp, LINES_SIZE, fp) != NULL) {
			if (strstr(temp, pattern) != 0) {
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
	int get_bufsize()
	{
		return BUFSIZ;
	}

	/*
    does a save re-allocation for char - or is supposed to do so, returns 0 in
  case of success and -1 in case of error
  //*/
	int get_more_space(char **str, unsigned long int *str_size,
			   const unsigned long int how_much_more)
	{
		char *tmp = NULL;

		// using realloc makes using strncat() here unnecessary
		if ((tmp = realloc(*str, (*str_size + how_much_more))) ==
		    NULL) {
			if (tmp != NULL)
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
