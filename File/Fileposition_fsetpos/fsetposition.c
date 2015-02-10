// fsetposition.c
/*
  demonstrates the use of fsetpos()/fgetpos()
  basically similar possiblilities like with fseek() but more convenient
//*/


#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  FILE* srcStream;
  int c = 0;
  char file[FILENAME_MAX];
  fpos_t pos;

  printf("Open file: ");
  scanf("%s", file);

  if( (srcStream = fopen(file, "r")) == NULL){
    fprintf(stderr, "Opening %s failed!\n", file);
    return EXIT_FAILURE;
  }

  // fgetpos() - show the position
  fgetpos(srcStream, &pos);
  printf("Position (originally): %li\n\n", pos);

  while((c=getc(srcStream)) != EOF)
    putc(c, stdout);
  printf("\n");
  printf("last position: %ld\n\n", ftell(srcStream));

  // fsetpos() - reset to the original position
  fsetpos(srcStream, &pos);
  printf("Position (again): %ld\n\n", pos);

  return EXIT_SUCCESS;
};

