// filemove.c
/*
  moves a file
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

/*
  NOTE: rename() works for renaming and/or moving files
  anyway it cannot move cross-device, i.e. it then will
  throw cross-device link errors (errno).

  This is the case when e.g. moving to a mounted folder
  e.g. from a docker container to export build artifacts

  The solution then is a hand-made copy function.
 */
int file_move(char old_filename[FILENAME_MAX], char new_filename[FILENAME_MAX])
{
  return rename(old_filename, new_filename);
}

/*
int file_copy(const char old_filename[FILENAME_MAX], char new_filename[FILENAME_MAX])
{
// TODO
  return 0;
}
// */

int main(int argc, char **argv)
{
  char file[FILENAME_MAX];
  memset(file, '\0', FILENAME_MAX);

  readstring(file, FILENAME_MAX, "open file: ");

  if (file_move(file, "Mickey-Mouse.txt")) {
    return EXIT_FAILURE;
  }

  printf("READY.\n");

  return EXIT_SUCCESS;
}
