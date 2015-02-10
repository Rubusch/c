// file_writing.c
/*
  at wirting to file existing various reasonable possibilities
  - write characterwise
  - write formated (using fprintf)
  - write linewise

  returning 0 or any error code else from 0 is important and can 
  also be used for debugging, e.g. the function "errno()" which
  is able to read the last given error returnvalue to the system 
  just relies on this kind of mechanism
  
  

  error messages:

  aside of perror() to send out messages, fprintf() is able to do a 
  similar thing. The differences are, perror() writes the message, 
  and adds the error code/internal error message, it can't take numbers!

  fprintf() just writes formated (can take numbers!) but don't show the 
  internal error message. fprintf() is a printf which writes to a stream, 
  therefore it has to be set to the error stream.



  streams:

  A computer system generally knows 3 standard streams:
  stdin    - input stream
  stdout   - output stream
  stderr   - error stream
  
  these treams are linked to, e.g. the standard output device (screen), 
  a standard input device (keyboard) or to files for reading / writing,
  sockets, ICP, threads, etc.

  The error stream serves to stay able to send messages in case an other 
  used stream (stdin/stdout) caused problems.
  
  Read the documentation of fprintf(), perror() and errno()! Try to 
  implement something and play with this code.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int get_write_file_pointer(FILE**, char[FILENAME_MAX]);
int write_char(FILE*, char*, const unsigned long int);
int close_stream(FILE**);


int main()
{
  FILE* fp = NULL;
  char file[] = "text.txt";
  char text[] = "Jack and Jill went up the hill\nto fetch a pail of water\nJack fell down and broke his crown\nAnd Jill came tumbling after.\n\nUp got Jack, and home did trot\nAs fast as he could caper\nHe went to bed and bound his head\nWith vinegar and brown paper.\n";
  unsigned int text_size = 1 + strlen(text);
  
  puts("write a file characterwise and create write file pointer");
  printf("%i - Get write file pointer\n", get_write_file_pointer(&fp, file));
  printf("%i - Writing file characterwise\n", write_char(fp, text, text_size));
  printf("%i - Close stream\n", close_stream(&fp));
  printf("%i - fp != NULL\n", (fp != NULL));
  printf("Done.\n\n\n");

  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
  get the pointer to the write stream
  
  returns -1 if error, else 0
//*/
int get_write_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
  // checks
  if(*fp != NULL) return -1;
  if(filename == NULL) return -1;

  // get the pointer
  if( (*fp = fopen(filename, "w")) == NULL){
    fprintf(stderr, "fopen failed!\n");
    return -1;
  }

  return 0;  
}


/*
  writes characterwise to the file

  returns -1 if error, else 0
//*/
int write_char(FILE* fp, char* content, const unsigned long int content_size)
{
  // checks
  if(fp == NULL) return -1;
  if(content == NULL) return -1;

  // the character to be writen
  int chr;
  unsigned long int idx = 0;

  // writing in a loop until the string has finished!
  while( (chr = *content++) != '\0'){
    ++idx;

    // check index out of bounds
    if(idx >= content_size){
      fprintf(stderr, "Writing to file failed!\n");
      return -1;
    }

    // actually writing with putc()
    putc(chr, fp);
  }
  return 0;
}


/*
  close the filestream

  returns 0 if everything ok, else the errno code
//*/
int close_stream(FILE** fp)
{
  // checks
  if(*fp == NULL) return -1;

  // close 
  int iRes = fclose(*fp);
  *fp = NULL;

  return iRes;
}
