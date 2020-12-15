// wrap_stdio.c
/*
  Standard I/O wrapper functions.
*/

#include "lib_socket.h"


void _fclose(FILE *fp)
{
  if(0 != fclose(fp)){
    err_sys("fclose error");
  }
}



FILE* _fdopen(int fd, const char *type)
{
  FILE *fp = NULL;
  
  if(NULL == (fp = fdopen(fd, type))){
    err_sys("fdopen error");
  }

  return(fp);
}



char* _fgets(char *ptr, int n, FILE *stream)
{
  char *rptr = NULL;
  
  if( (NULL == (rptr = fgets(ptr, n, stream))) 
      && ferror(stream)){
    err_sys("fgets error");
  }
  
  return (rptr);
}



FILE* _fopen(const char *filename, const char *mode)
{
  FILE *fp = NULL;
  
  if(NULL == (fp = fopen(filename, mode))){
    err_sys("fopen error");
  }
  
  return(fp);
}



void _fputs(const char *ptr, FILE *stream)
{
  if(EOF == fputs(ptr, stream)){
    err_sys("fputs error");
  }
}
