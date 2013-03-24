// fflush.c
/*
fflush() can lead to undefined behaviour!!!


#include <stdio.h>
int fflush(FILE *stream);
Description

If stream points to an output stream or an update stream in which the most recent
operation was not input, the fflush function causes any unwritten data for that stream
to be delivered to the host environment to be written to the file; otherwise, the behavior is
undefined.

If stream is a null pointer, the fflush function performs this flushing action on all
streams for which the behavior is defined above.
Returns

The fflush function sets the error indicator for the stream and returns EOF if a write
error occurs, otherwise it returns zero.
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_DIGITS 3
#define ENTER_DIGITS 7

int main(int argc, char** argv)
{
  fprintf(stderr, "enter %d digits, the variable has %d\n", ENTER_DIGITS, STR_DIGITS);
  fprintf(stderr, "hence %d - %d - 2 digits will stay in the stdin stream (the 2 is for \"\\n\\0\")\n", ENTER_DIGITS, STR_DIGITS); 

  // read
  char str[STR_DIGITS]; 
  memset(str, '\0', STR_DIGITS);
  fgets(str, STR_DIGITS, stdin); 

  // in case check if '\n' was read from the input stream
  unsigned int idx=0;
  for(idx=0; idx<STR_DIGITS; ++idx){
    if(str[idx] == '\n') 
      break;
  }

  // cleaning up
  if(idx >= STR_DIGITS){
    puts("no '\\n' was read from stdin - flushing");
    int c;
    while ((c = getchar()) != EOF && c != '\n');
  }else{
    puts("a '\\n' already was read from stdin");
  }

  // finalize the string
  str[STR_DIGITS-1] = '\0';
   

  /*
    the next prompt for an input will wait!
  //*/
  
  puts("READY.");
  exit(EXIT_SUCCESS);
}
