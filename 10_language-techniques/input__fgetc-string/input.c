// input_string.c
/*
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
  @license: GPLv3
  @2013-May-01

  demonstrates reading in a string
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 512

void readstring(char *, const unsigned int, const char *);

// READ INPUT via fgetc
void readstring(char *cTxt, const unsigned int txtSize, const char *comment)
{
	if (NULL == comment) {
		perror("text is NULL");
		return;
	}
	if (NULL == cTxt) {
		perror("iTxt is NULL");
		return;
	}

	do {
		// reset
		memset(cTxt, '\0', txtSize);
		puts(comment);

		// read in
		unsigned int c;
		unsigned int idx = 0;

		// in case of exceeding the size of the variable - put a '\0' at the end
		// and read until '\n', but don't store the characters for cleaning the
		// stream
		for (idx = 0; '\n' != (c = fgetc(stdin)); ++idx) {
			if ((txtSize - 1) > idx) {
				cTxt[idx] = c;
			} else if ((txtSize - 1) == idx) {
				puts("input too long - will be reset");
				memset(cTxt, '\0', txtSize);
				fflush(stdin);
				// or cut here:
				// cTxt[idx] = '\0';
			} else if (-1 == c) { // EOT, ENQ, ...
				perror("input was control character");
				exit(EXIT_FAILURE);
			}
		}
	} while (0 == strlen(cTxt));
}

// USAGE
int main(int argc, char **argv)
{
	char *message = NULL;
	if ((message = calloc(BUF_SIZ, sizeof(*message))) == NULL) {
		perror("allocation failed");
		return EXIT_FAILURE;
	}
	unsigned int messageLen = 0;

	puts("TEST STRING INPUT");
	do {
		// init
		memset(message, '\0', BUF_SIZ);
		messageLen = strlen(message); // better readable

		// read input
		readstring(message, BUF_SIZ,
			   "enter a string ('quit' to quit):");
		messageLen = strlen(message);

		// print out
		fprintf(stderr, "the entered text was:\n\"%s\"\n", message);

	} while ((strlen("quit") != strlen(message)) ||
		 (0 != strncmp(message, "quit", messageLen)));

	puts("READY.");
	if (NULL != message)
		free(message);
	return EXIT_SUCCESS;
}
