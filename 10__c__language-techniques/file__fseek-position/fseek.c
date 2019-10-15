// fseek.c
/*
  shows the use of fseek functions
  furthermore demonstrates the use of:
   - scanf()/printf()
   - getc()/putc()

  pointers for the fseek()
   - SEEK_SET - start
   - SEEK_CUR - current position
   - SEEK_END - end

  position
   - 0L   - exactly this position
   - 10L  - 10 characters (Bytes) after of the pointer, e.g. SEEK_CUR
   - -10L - 10 characters (Bytes) ahead that position

//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */


// READ INPUT via fgetc
void readstring(char *cTxt, const unsigned int txtSize, const char *comment)
{
  if (NULL == comment) {
    perror("text is NULL");
    return;
  }
  if (NULL == cTxt) {
    perror("iTxt is NULL");
    return;
  }

  do {
    // reset
    memset(cTxt, '\0', txtSize);
    puts(comment);

    // read in
    unsigned int c;
    unsigned int idx = 0;

    // in case of exceeding the size of the variable - put a '\0' at the end
    // and read until '\n', but don't store the characters for cleaning the
    // stream
    for (idx = 0; '\n' != (c = fgetc(stdin)); ++idx) {
      if ((txtSize - 1) > idx) {
        cTxt[idx] = c;
      } else if ((txtSize - 1) == idx) {
        puts("input too long - will be reset");
        memset(cTxt, '\0', txtSize);
        fflush(stdin);
        // or cut here:
        // cTxt[idx] = '\0';
      } else if (-1 == c) { // EOT, ENQ, ...
        perror("input was control character");
        exit(EXIT_FAILURE);
      }
    }
  } while (0 == strlen(cTxt));
}

int main(int argc, char **argv)
{
  FILE *srcStream = NULL, *error = NULL;
  int c = -1;
  char file[20];
  memset(file, '\0', 20);
  long pos = 0;

  readstring(file, 20, "Open file: ");

  if ((srcStream = fopen(file, "a+")) == NULL) {
    if ((error = fopen("error.log", "a+")) != NULL) {
      fprintf(error, "Opening %s failed!\n", file);
      fprintf(stderr, "Opening %s failed!\n", file);
      return EXIT_FAILURE;
    }
    fprintf(stderr, "Opening %s failed!\n", file);
    return EXIT_FAILURE;
  }

  // getc()/putc() - set token '*' as end of input
  printf("Enter something, terminate with \'*\':\n");
  while ('*' != (c = getc(stdin))) {
    putc(c, srcStream);
  }
  printf("\n");

  // fseek() - set the pointer to start of file
  fseek(srcStream, 0L, SEEK_SET);

  // print whole file
  while (EOF != (c = getc(srcStream))) {
    putc(c, stdout);
  }
  printf("\n");

  // fseek() - go 10 characters back
  fseek(srcStream, -10L, SEEK_CUR);

  // and re-print them on the screen
  printf("The last 10 characters where: ");
  while (EOF != (c = getc(srcStream))) {
    putc(c, stdout);
  }
  printf("\n");

  // set a number of characters from start to place the position
  printf("Set reader on position (from start): ");
  scanf("%1li", &pos); // li = long int
  fflush(stdin);

  fseek(srcStream, 0L, SEEK_SET);       // same as rewind()
  fseek(srcStream, 0L + pos, SEEK_CUR); // set to position in first line

  while (EOF != (c = getc(srcStream))) {
    putc(c, stdout);
  }

  return EXIT_SUCCESS;
}
