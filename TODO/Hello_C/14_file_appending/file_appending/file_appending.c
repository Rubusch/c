// file_appending.c
/*
  append text to a file

  using a write file pointer, the file to be written will be overwritten
  each time!
  if this is not desired, we use the "append file pointer"
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int get_append_file_pointer(FILE **, char[FILENAME_MAX]);
int write_linewise(FILE *, char *, const unsigned long int);
int close_stream(FILE **);


int main()
{
  FILE *fp = NULL;
  char file[] = "text.txt";
  char text[] = "All work and no play makes Jack a dull boy.\n";
  unsigned int text_size = 1 + strlen(text);

  printf("appending a line to %s\n", file);
  printf("%i - Get append file pointer\n", get_append_file_pointer(&fp, file));
  printf("%i - Append some text to the file (linewise)\n",
         write_linewise(fp, text, text_size));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");

  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
  get an append file pointer

  return 0 if success, else -1
//*/
int get_append_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
  if (*fp != NULL)
    return -1;
  if (filename == NULL)
    return -1;

  if ((*fp = fopen(filename, "a")) == NULL) {
    fprintf(stderr, "Failed!\n");
    return -1;
  }

  return 0;
}


/*
  write linewise

  return 0 if success, else -1
//*/
int write_linewise(FILE *fp, char *content,
                   const unsigned long int content_size)
{
  if (fp == NULL)
    return -1;
  if (content == NULL)
    return -1;

  char bufLine[BUFSIZ];
  int idxLine = 0;
  int idxContent = 0;
  char *pData = &content[0];

  memset(bufLine, '\0', BUFSIZ);
  while ((idxLine < BUFSIZ) && (idxContent < content_size) &&
         ((bufLine[idxLine] = *(pData++)) != '\0')) {

    if (idxLine >= BUFSIZ) {
      fprintf(stderr, "failed!\n");
      return -1;
    }

    if (((idxLine == content_size - 2) && (bufLine[idxLine] != '\n')) ||
        (*(pData + 1) == '\0')) {
      // terminate line and write
      bufLine[idxLine + 1] = '\0';
      fputs(bufLine, fp);
      break;

    } else if (bufLine[idxLine] == '\n') {
      // write line and reset idxLine
      fputs(bufLine, fp);
      idxLine = 0;

    } else {
      ++idxLine;
    }
    ++idxContent;
  }
  fputs("\n\0", fp); // tailing linefeed + linebreak
  return 0;
}


/*
  close the file stream

  return 0 if success, else -1
//*/
int close_stream(FILE **fp)
{
  if (*fp == NULL)
    return -1;
  int iRes = fclose(*fp);
  *fp = NULL;

  return iRes;
}
