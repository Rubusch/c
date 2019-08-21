// fflush.c
/*
  fflush() can lead to undefined behaviour!!!


  #include <stdio.h>
  int fflush(FILE *stream);

  As per C standard, using fflush(stdin) has undefined behavior, some compilers
  provide an implementation to use it

  The fflush function sets the error indicator for the stream and returns EOF if
  a write error occurs, otherwise it returns zero.
//*/

#include <stdio.h> /* fflush() */
#include <string.h> /* memset() */
#include <stdlib.h>
#include <unistd.h> /* sleep() */


#define STRSIZ 20

int main(int argc, char** argv)
{
  puts("stdin: enter some text, end with ENTER");

  char str[STRSIZ]; memset(str, '\0', sizeof(str));
  for (int idx=0; idx<2; ++idx) {
    scanf("%[^\n]s", str); // read input, until \n
    fprintf(stdout, "read: '%s'\n", str);
//    fflush(stdin); // can have undefined behavior, depends on compiler
    // alternative method:
    int c;
    while ((c = getchar()) != EOF && c != '\n');
    memset(str, '\0', sizeof(str));
  }
  puts("\n");

  puts("stdout: reversed order between buffered and unbuffered stream");
  char buf[BUFSIZ]; memset(buf, '\0', sizeof(buf));
  fprintf(stdout, "turn buffer on\n");
  setvbuf(stdout, buf, _IOFBF, BUFSIZ); // setup buffer for stdout to BUFSIZ

  fprintf(stdout, "1. ipsum lorum primeiro\n");
  fflush(stdout); // flush - comes at once, then the 'big task'

  fprintf(stdout, "2. ipsum lorum segundo (buffered stdout)\n");
//  fflush(stdout); // flush - turn this on, for order: 1-2-3

  fprintf(stderr, "3. sleeping (message on stderr)\n");
  sleep(5); // a 'big task'


  // the output order will be: 1-3-2, since 3 is unbuffered and 2 will wait on
  // the 'big task' (i.e. sleep) to finish

  exit(EXIT_SUCCESS);
}
