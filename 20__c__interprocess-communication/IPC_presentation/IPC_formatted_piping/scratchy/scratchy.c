// scratchy.c
/*
  formated pipe - popen()

  FILE* popen(const char* command, const char* type)

  *command the process that will be connected to establish the pipe
  *type    "w" or "r", writing to such a stream writes to the standard
  input of the command, a pipe has only one "r" and only one "w" end

  returns a stream pointer or NULL
   - communication: fprintf(), fscanf()
   streams: stdin - STDIN_FILENO / stdout - STDOUT_FILENO
   - to be closed with pclose(FILE* stream)
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ME "SCRATCHY:"
#define BUF_SIZE 64


int main(int argc, char **argv)
{
  fprintf(stderr, "%s started\n", ME);

  // init
  char buf[BUF_SIZE];
  memset(buf, '\0', BUF_SIZE);

  // popen / send
  FILE *fp = popen("../itchy/itchy Scratchyargument", "r");

  // receive
  while (fgets(buf, BUF_SIZE, fp)) {
    printf("%s receives \"%s\"\n", ME, buf);
  }

  // close
  pclose(fp);

  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}
