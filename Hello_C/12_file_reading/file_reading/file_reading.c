// file_reading.c
/*
  demonstrates how to read from a file
  
  C has "low level" function and "high level" function for file
  input / ouput (io)
  
  lowlevel functions in C are:
  read() and write()
  they serve for writing to all kind of streams, are rather complicated 
  to use and have lots of edges.
  
  More convenient (and less error prone) are the higherlevel functions 
  in C. 

  Writing to files is a basic process and serves in the same way when 
  writing messages using sockets, writing to shared memory, pipes, files



  reading from a file there are the following possibilities in C:

  1. reading characterwise (which is shown here using fgetc() - this is the 
  most common case and the most reasonable way in general!)
  
  2. reading without spaces using fscanf (=formatted reading)
  
  3. reading linewise (getline() is only implemented on unix/linux, so it is 
  NOT ANSI, but a method to read "linewise" can be implemented)
  
  4. reading blockwise, normally with fread() to read structs from a 
  file as "block", therefore "blockwise"



  constants:

  FILENAME_MAX - an environmental constant defined in <stdlib.h> gives the 
  maximum length of a filename.

  BUFSIZ - an environmental constant defined in <stdlib.h>, it gives something
  like the standard buffer size, which is set by default for some operations.

  EOF - End Of File is also defined in <stdlib.h>
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// forward declarations
int get_read_file_pointer(FILE**, char[FILENAME_MAX]);
int read_char(FILE*, char**, const unsigned int*);
int close_stream(FILE**);


int main()
{
  // initializations
  FILE* fp = NULL;
  char file[] = "text.txt";
  const unsigned int content_size = BUFSIZ;
  char* content; 
  if(NULL == (content = calloc(content_size, sizeof(*content)))){
    perror("allocation failed");
    exit(EXIT_FAILURE);
  }
  memset(content, '\0', content_size);

  // process
  puts("reading file characterwise");
  printf("%i - Get read file pointer to %s\n", get_read_file_pointer(&fp, file), file);
  printf("%i - Reading file characterwise\n", read_char(fp, &content, &content_size));
  printf("content:\n\'%s\'\n", content);
  printf("%i - Close stream\n", close_stream(&fp));
  printf("Done.\n\n");

  puts("READY.");
  if(NULL != content) free(content);
  exit(EXIT_SUCCESS);
}


/*
  returns a read file pointer to a file stream
//*/
int get_read_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
  // checks
  if(*fp != NULL) return -1;

  // get filestream - use fopen
  if((*fp = fopen(filename, "r")) == NULL){
    perror("fopen failed!");
    return -1;
  }

  return 0;
}


/*
  read characterwise
  
  to avoid to check if a size, e.g. here the "content_size" is smaller than null, 
  we can use the "unsigned"
  Attention: checking "unsigned" variables against <0 doesn't make sense. And if
  an unsigned variable (e.g. int) gets bigger than its maximum, it restarts at 0!!!
  But it will always be greater than 0!!! 
//*/
int read_char(FILE* fp, char** content, const unsigned int* content_size)
{
  if(fp == NULL) return -1;
  if(0 == *content_size) return -1;

  // the read in token will be an "int" due to the fgetc() function!!!
  int chr = 0; 

  unsigned long int idx = 0;

  // read in while loop until EOF (End Of File)
  while( (chr=fgetc(fp)) != EOF){

    // assign the read in "chr" to the pointer "content"
    (*content)[idx] = chr;
    
    // if there the read in characters are more than the size of the passed pointer, stop it
    // another possibility might be: try to realloc space to the pointer and extend the 
    // content_size (not const then)
    if(idx >= (*content_size)-2){
      // in case the content size exceeded, terminate the string and return -1
      (*content)[idx] = '\0';
      return -1;
    }

    // increase counter
    ++idx;
  }

  // terminate string
  (*content)[idx] = '\0';
  return 0;
}


/*
  close file stream
//*/
int close_stream(FILE** fp)
{
  // check if fp is NULL
  if(*fp == NULL) return -1;

  // close the filestream, set the file pointer to NULL and return the result of the call to fclose()
  int iRes = fclose(*fp);
  *fp = NULL;
  return iRes;
}
