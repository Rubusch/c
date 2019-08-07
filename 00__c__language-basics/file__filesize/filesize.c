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

  printf("Open file: ");
  scanf("%s", file);

  if (filesize(file, &size)) {
    return EXIT_FAILURE;
  }

  printf("The filesize is %ld\n", size);

  return EXIT_SUCCESS;
}
