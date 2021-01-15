// popen.c
/*
  popen() - formated pipes to other programs

  FILE* popen(char* command, char* type)

  *command the process that will be connected to establish the pipe
  *type    "w" or "r", writing to such a stream writes to the standard
  input of the command, a pipe has only one "r" and only one "w" end

  returns a stream pointer or NULL
  - communication: fprintf(), fscanf()
  - to be closed with pclose(FILE* stream)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZ 64

extern FILE *popen(const char *, const char *);
extern int pclose(FILE *);


int main(void)
{
	char line[LINE_SIZ];
	memset(line, '\0', LINE_SIZ);

	// pipe to the 'ls' program
	FILE *fp = popen("ls -l", "r");

	while (fgets(line, sizeof(line), fp)) { // better LINE_SIZ
		printf("%s", line);
	}

	// close again
	pclose(fp);

	exit(EXIT_SUCCESS);
}
