// tempfile.c

// needed for mkstemp()
#define _GNU_SOURCE

#include <stdlib.h> /* mkstemp() */
#include <stdio.h>
#include <string.h>

#include <unistd.h> /* unlink */

/*
// DEPRECATED!!!
int create_tmp(FILE** fp, char szTmp[L_tmpnam])
{
  if(szTmp == NULL){
    szTmp = tmpnam(szTmp); // WARNING! tmpnam is dangerous, and nowadays deprecated!!! use mkstemp()
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
/*/
// version with mkstemp()
// use #define _GNU_SOURCE
// and #include <unistd.h> // for unlink()
int create_tmp(FILE** fp, char szTmp[L_tmpnam])
{
  if (NULL == szTmp) {
fprintf(stderr, "XXX szTmp was NULL\n");
    strncpy(szTmp, "/tmp/tmpFile-XXXXXX", 21); // TODO szTmp[L_tmpnam] ???
    if (0 > mkstemp(szTmp)) {
      fprintf(stderr, "mkstemp() - Failed!\n");
      return -1;
    }
    unlink(szTmp); // will be removed at reboot automatically

    // create - no name given..
    if ( NULL == (*fp = fopen(szTmp, "rw"))) {
      fprintf(stderr, "fopen() - Failed!\n");
      return -1;
    }

  } else {
fprintf(stderr, "XXX szTmp was NOT NULL, '%s'\n", szTmp);
    if (NULL == (*fp = tmpfile())) {
      fprintf(stderr, "tmpfile() - Failed!\n");
      return -1;
    }
  }

  return 0;
}
// */

int close_tmp(FILE** fp)
{
  if(fp == NULL) return -1;
  fclose(*fp);
  *fp = NULL;
  return 0;
}


int write_linewise(FILE* fp, char* content, const unsigned long int CONTENT_SIZE)
{
  if(fp == NULL) return -1;
  if(content == NULL) return -1;

  char bufLine[BUFSIZ];
  int idxLine = 0;
  int idxContent = 0;
  char *pData = &content[0];
  strcpy(bufLine, "");

  rewind(fp); // reset filestream
  while ((idxLine < BUFSIZ)
	&& (idxContent < CONTENT_SIZE)
	&& ((bufLine[idxLine] = *(pData++)) != '\0')) {

    if (idxLine >= BUFSIZ) {
      fprintf(stderr, "fo::write_linewise(FILE*, char*, const unsigned long) - Failed!\n");
      return -1;
    }

    if ( ((idxLine == CONTENT_SIZE-2) && (bufLine[idxLine] != '\n'))
	|| (*(pData+1) == '\0' )) {
      bufLine[idxLine+1] = '\0';
fprintf(stderr, "AAA fputs bufLine '%s'!\n", bufLine);            
      fputs(bufLine, fp); // write line
      break;

    }else if(bufLine[idxLine] == '\n'){
fprintf(stderr, "BBB fputs bufLine '%s'!\n", bufLine);            
      fputs(bufLine, fp); // write line
      idxLine = 0;
    }else{
      ++idxLine;
    }
    ++idxContent;
  }
  fputs("\n\0", fp); // tailing linefeed + linebreak
  return 0;
}


int read_nth_line(FILE* fp,  char* line, const unsigned long int LINE_SIZE, const unsigned long int LINE_NUMBER)
{
#ifdef DEBUG
  printf("\tfo::read_nth_line(*fp, *line, LINE_SIZE, LINE_NUMBER)\n");
  printf("\t%i - fp == NULL\n", (fp == NULL));
  printf("\t%i - &*fp\n", &*fp);
  printf("\t%i - line == NULL\n", (line == NULL));
  printf("\t%i - LINE_SIZE\n", LINE_SIZE);
  printf("\t%i - LINE_NUMBER\n", LINE_NUMBER);
#endif
  if(fp == NULL) return -1;
  rewind(fp); // reset filestream
  char temp[LINE_SIZE];
  int cnt=0;
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
  FILE* fp = NULL;
  fp = malloc(sizeof(FILE));

  char text1[] = "Jack and Jill went up the hill to fetch a pail of water\n";
  char text2[] = "Jack fell down and broke his crown\nAnd Jill came tumbling after.\n";
  char text3[] = "Up got Jack, and home did trot\nAs fast as he could caper\n";
  char text4[] = "He went to bed and bound his head\nWith vinegar and brown paper.\n";

  unsigned long int text1_size = sizeof(text1);
  unsigned long int text2_size = sizeof(text2);
  unsigned long int text3_size = sizeof(text3);
  unsigned long int text4_size = sizeof(text4);
  unsigned long int content_size = 2 * BUFSIZ;

  char *content = malloc(content_size*sizeof(char));

  strcpy(content,"");
  char szTmp[] = "tmp.txt";
//  char szTmp[] = "/tmp/tmpFile-XXXXXX";

  // create FILE*
  printf("%i - Create temp file pointer (read/write): %s\n", create_tmp(&fp, szTmp), szTmp);

  // write the lines
  printf("%i - Append some text to the \'%s\' (linewise)\n", write_linewise(fp, text1, text1_size), szTmp);
  printf("%i - Append some text to the \'%s\' (linewise)\n", write_linewise(fp, text2, text2_size), szTmp);
  printf("%i - Append some text to the \'%s\' (linewise)\n", write_linewise(fp, text3, text3_size), szTmp);
  printf("%i - Append some text to the \'%s\' (linewise)\n", write_linewise(fp, text4, text4_size), szTmp);

  // read the nth line
  printf("%i - Read the %i. line (first line index: 1) of \'%s\'\n", read_nth_line(fp, content, content_size, 2), 2, szTmp); // FIXME
  printf("content:\n\'%s\'\n", content);
  strcpy(content,"");

  // close the tmp file
  printf("%i - And close the \'%s\' file again\n", close_tmp(&fp), szTmp);
  printf("Done.\n\n");
  //*/

  return EXIT_SUCCESS;
}
