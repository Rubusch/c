// filesize.c
/*
  gets the filesize using fseek()
//*/


#include <stdio.h>
#include <stdlib.h>


int filesize(char file[FILENAME_MAX], long int* size){
  FILE* srcStream = NULL;

  if( (srcStream = fopen(file, "r")) == NULL){
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
  char file[FILENAME_MAX];

  printf("Open file: ");
  scanf("%s", file);

  if(filesize(file, &size))
    return EXIT_FAILURE;

  printf("The filesize is %ld\n", size);

  return EXIT_SUCCESS;
}
