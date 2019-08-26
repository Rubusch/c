// filesize.c
/*
  gets the filesize using fseek()
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */


// READ INPUT via fgetc
void readstring(char* cTxt, const unsigned int txtSize, const char* comment)
{
  if(NULL == comment){
    perror("text is NULL");
    return;
  }
  if(NULL == cTxt){
    perror("iTxt is NULL");
    return;
  }

  do{
    // reset
    memset(cTxt, '\0', txtSize);
    puts(comment);

    // read in
    unsigned int c;
    unsigned int idx=0;

    // in case of exceeding the size of the variable - put a '\0' at the end
    // and read until '\n', but don't store the characters for cleaning the
    // stream
    for (idx=0; '\n' != (c = fgetc(stdin)); ++idx) {
      if ((txtSize-1) > idx) {
        cTxt[idx] = c;
      } else if ((txtSize-1) == idx) {
	puts("input too long - will be reset");
	memset(cTxt, '\0', txtSize);
        fflush(stdin);
	// or cut here:
	//cTxt[idx] = '\0';
      } else if (-1 == c) { // EOT, ENQ, ...
        perror("input was control character");
        exit(EXIT_FAILURE);
      }
    }
  } while (0 == strlen(cTxt));
}

int filesize(char file[FILENAME_MAX], long int* size){
  FILE* srcStream = NULL;

  if (!size) return -1;

  if( NULL == (srcStream = fopen(file, "r"))){
    fprintf(stderr, "Opening file %s failed!\n", file);
    return -1;
  }

  fseek(srcStream, 0L, SEEK_END);
  *size = ftell(srcStream);

  return 0;
}


int main(int argc, char** argv)
{
  long int size = 0;
  char file[FILENAME_MAX]; memset(file,'\0', FILENAME_MAX);

  readstring(file, FILENAME_MAX, "open file: ");

  if (filesize(file, &size)) {
    return EXIT_FAILURE;
  }

  printf("the filesize is %ld\n", size);

  return EXIT_SUCCESS;
}
