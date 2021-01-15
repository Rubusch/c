// popen.c
/*
  opens a process pipe to the console

  #include <stdio.h>
  FILE* popen(const char* command, const char* type);

  command
  the actual console command
  type
  type can be "r" or "w", e.g. for "ls -l 2>&1" it's "w"


  int pClose(FILE* stream);

  stream
  the stream to close
*/

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *popen(const char *, const char *);
extern int pclose(FILE *);


int main()
{
	FILE *fPipe;
	char command[] = "ls -al";
	char line[256];

	// open pipe - "r", "w" possible
	if (NULL == (fPipe = ( FILE * )popen(command, "r"))) {
		perror("popen() failed");
		exit(1);
	}

	// read
	while (fgets(line, sizeof(line), fPipe)) {
		puts(line); // appends a \r\n
	}

	// close pipe stream
	pclose(fPipe);
	fprintf(stdout, "READY.\n");
}
