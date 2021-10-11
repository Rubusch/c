// tempfile.c

// needed for mkstemp()
#define _GNU_SOURCE

#include <stdio.h> /* BUFSIZ = 8912 */
#include <stdlib.h> /* mkstemp() */
#include <string.h> /* memset() */

#include <unistd.h> /* unlink */

//#define DEBUG 1

/*
// DEPRECATED!!!
int create_tmp(FILE** fp, char szTmp[L_tmpnam])
{
  if(szTmp == NULL){
    szTmp = tmpnam(szTmp); // WARNING! tmpnam is dangerous, and nowadays
deprecated!!! use mkstemp() if( (*fp = fopen(szTmp, "rw")) == NULL){
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
/*/
// version with mkstemp()
// use #define _GNU_SOURCE
// and #include <unistd.h> // for unlink()
int create_tmp(FILE **fp, char szTmp[L_tmpnam])
{
	if (NULL == szTmp) {
		strncpy(szTmp, "/tmp/tmpFile-XXXXXX",
			21); // TODO szTmp[L_tmpnam] ???
		if (0 > mkstemp(szTmp)) {
			fprintf(stderr, "mkstemp() - Failed!\n");
			return -1;
		}
		unlink(szTmp); // will be removed at reboot automatically

		// create - no name given..
		if (NULL == (*fp = fopen(szTmp, "rw"))) {
			fprintf(stderr, "fopen() - Failed!\n");
			return -1;
		}

	} else {
		if (NULL == (*fp = tmpfile())) {
			fprintf(stderr, "tmpfile() - Failed!\n");
			return -1;
		}
	}

	return 0;
}
// */

int close_tmp(FILE **fp)
{
	if (fp == NULL)
		return -1;
	fclose(*fp);
	*fp = NULL;
	return 0;
}

int write_linewise(FILE *fp, char *content, const unsigned long CONTENT_SIZE)
{
	if (fp == NULL)
		return -1;
	if (content == NULL)
		return -1;

	// fprintf(stderr, "%s( 0x%lX, '%s', %lu )\n", __func__, (unsigned long) fp,
	// content, CONTENT_SIZE);

	char bufLine[BUFSIZ];
	//  int idxLine = 0;
	int idxContent = 0;
	char *pData = &content[0];
	strcpy(bufLine, "");

	rewind(fp); // reset filestream
	// this will overwrite the already existing content for each call (which is
	// the behavior of the 'deprecated' old function, leave this away and the temp
	// file will grow by lines instead of being overwritten

	while ((idxContent < CONTENT_SIZE) &&
	       ((bufLine[idxContent] = *pData++) != '\0')) {
		// idxLine, idx within the line until '\n', then reset to '0'
		// idxContent, idx until end of text

		if ((idxContent == CONTENT_SIZE - 2) ||
		    (*(pData + 1) == '\0')) {
			bufLine[idxContent + 1] = '\n';
			bufLine[idxContent + 2] = '\0';
			fputs(bufLine, fp); // write line
			break;
		}

		++idxContent;
	}
	return 0;
}

int read_nth_line(FILE *fp, char *line, const unsigned long int LINE_SIZE,
		  const unsigned long int LINE_NUMBER)
{
#ifdef DEBUG
	printf("\tfo::read_nth_line(*fp, *line, LINE_SIZE, LINE_NUMBER)\n");
	printf("\tfp == NULL: %s\n", (fp == NULL) ? "true" : "false");
	printf("\t0x%lX - &*fp\n", (unsigned long)&*fp);
	printf("\tline == NULL: %s\n", (line == NULL) ? "true" : "false");
	printf("\t%li - LINE_SIZE\n", LINE_SIZE);
	printf("\t%li - LINE_NUMBER\n", LINE_NUMBER);
#endif
	if (fp == NULL)
		return -1;
	rewind(fp); // reset filestream
	char temp[LINE_SIZE];
	int cnt = 0;
	for (cnt = 0; cnt < LINE_NUMBER - 1; ++cnt) {
		if (fgets(temp, LINE_SIZE, fp) == NULL) {
			return -1;
		}
	}
	if (fgets(temp, LINE_SIZE, fp) == NULL) {
		return -1;
	}
	strcat(line, temp);

	return 0;
}

int main()
{
	// init
	FILE *fp = NULL;
	fp = malloc(sizeof(FILE));

	char text1[] =
		"Jack and Jill went up the hill to fetch a pail of water\n";
	char text2[] =
		"Jack fell down and broke his crown\nAnd Jill came tumbling after.\n";
	char text3[] =
		"Up got Jack, and home did trot\nAs fast as he could caper\n";
	char text4[] =
		"He went to bed and bound his head\nWith vinegar and brown paper.\n";

	unsigned long int text1_size = sizeof(text1);
	unsigned long int text2_size = sizeof(text2);
	unsigned long int text3_size = sizeof(text3);
	unsigned long int text4_size = sizeof(text4);

	unsigned long int content_size = BUFSIZ;

	char *content = malloc(content_size * sizeof(char));

	strcpy(content, "");
	char szTmp[] = "/tmp/tmpFile-XXXXXX";

	// create FILE*
	printf("%i - Create temp file pointer (read/write): %s\n",
	       create_tmp(&fp, szTmp), szTmp);

	// write the lines
	printf("%i - Append some text to the \'%s\' (linewise)\n",
	       write_linewise(fp, text1, text1_size), szTmp);
	printf("%i - Append some text to the \'%s\' (linewise)\n",
	       write_linewise(fp, text2, text2_size), szTmp);
	printf("%i - Append some text to the \'%s\' (linewise)\n",
	       write_linewise(fp, text3, text3_size), szTmp);
	printf("%i - Append some text to the \'%s\' (linewise)\n",
	       write_linewise(fp, text4, text4_size), szTmp);

	// read the nth line
	printf("%i - Read the %i. line (first line index: x) of \'%s\'\n",
	       read_nth_line(fp, content, content_size, 2), 2, szTmp);
	puts("");

	printf("content (%i. line, in case prints first part of the string until "
	       "'\\n'):\n\'%s\'\n",
	       2, content);
	strcpy(content, "");
	puts("");

	// close the tmp file
	printf("%i - And close the \'%s\' file again\n", close_tmp(&fp), szTmp);
	printf("Done.\n\n");
	//*/

	return EXIT_SUCCESS;
}
