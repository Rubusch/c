// fsetposition.c
/*
  demonstrates the use of fsetpos()/fgetpos()
  basically similar possiblilities like with fseek() but more convenient
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
  FILE *srcStream;
  int c = 0;
  char file[FILENAME_MAX];
  fpos_t pos;

  readstring(file, FILENAME_MAX, "Open file: ");
  if ((srcStream = fopen(file, "r")) == NULL) {
    fprintf(stderr, "Opening %s failed!\n", file);
    return EXIT_FAILURE;
  }

  // fgetpos() - show the position
  fgetpos(srcStream, &pos);

  // fpos_t is not necessarily a scalar (can't be simply typedef'd to something
  // printable) the hex content is printable, though
  printf("Position (originally): \t0x");
  for (size_t idx = 0; idx < sizeof(pos); ++idx) {
    printf("%02X", (( unsigned char * )&pos)[idx]);
  }
  printf("\n");


  while ((c = getc(srcStream)) != EOF)
    putc(c, stdout);
  printf("\n");
  printf("last position in file: %ld\n\n", ftell(srcStream));

  // fsetpos() - reset to the original position
  fsetpos(srcStream, &pos);
  // NOTE: this is actually the same position as before
  printf("Position (again): \t0x");
  for (size_t idx = 0; idx < sizeof(pos); ++idx) {
    printf("%02X", (( unsigned char * )&pos)[idx]);
  }
  printf("\n");


  return EXIT_SUCCESS;
};
