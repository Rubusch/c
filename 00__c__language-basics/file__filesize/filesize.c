// filesize.c
/*
  gets the filesize using fseek()
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */


int filesize(char file[FILENAME_MAX], long int* size){
  FILE* srcStream = NULL;

  if (!size) return -1;

  if( NULL == (srcStream = fopen(file, "r"))){
    fprintf(stderr, "Opening file %s failed!", file);
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

  printf("open file: ");
  if (0 == scanf("%[a-zA-Z.]", file)) { // accept a-z, A-Z and also '.', e.g. enter 'text.txt' should work
    fprintf(stderr, "scanf failed!\n");
    return EXIT_FAILURE;
  }


  if (filesize(file, &size)) {
    return EXIT_FAILURE;
  }

  printf("the filesize is %ld\n", size);

  return EXIT_SUCCESS;
}