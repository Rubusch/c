/*
  disabling terminal echoing

  usage:
    $ ./change_disable_echoing.elf
      enter text:           // type some text
      read: hello work!
      READY.


  The proggy demonstrates the use of tcgetattr() and tcsetattr() to
  turn off the ECHO flag, so taht input characters are not echoed.

  Terminal I/O modes:

  Canonical Mode - canonical mode input is enabled by setting the
  ICANON flag. Terminal input in canonical mode is distinguished by
  the features:
  - Input is gathered into lines, terminated by one of the
    line-delimiter characters
  - Line editing is enabled, so that the current line of input can be
    modified.
  - If the IEXTEN flag is set, the REPRINT and LNEXT characters are
    also enabled

  Noncanonical Mode - Some applications, e.g. vi and less, need to
  read characters from the terminal without the user supplying a line
  delimiter. Noncanonical mode is provided for this purpose. In
  noncanonical mode (ICANON unset), no special input processing is
  performed. In particular, input is no longer gathered into lines,
  but is instead available immediately.

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1306
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>

#define BUF_SIZE 100


int
main(int argc, char *argv[])
{
	struct termios tp, save;
	char buf[BUF_SIZE];

	/*
	  retrieve current terminal settings, turn echoing off
	*/

	if (-1 == tcgetattr(STDIN_FILENO, &tp)) {
		perror("tcgetattr()");
		exit(EXIT_FAILURE);
	}

	save = tp;  // so we can restore settings later
	tp.c_lflag &= ~ECHO;  // ECHO off, other bits unchanged
	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp)) {
		perror("tcsetattr()");
		exit(EXIT_FAILURE);
	}

	/*
	  read some input and then display it back to the user
	*/

	fprintf(stderr, "enter text: ");
	// alternative: printf() + fflush(stdout);
	if (NULL == fgets(buf, BUF_SIZE, stdin)) {
		fprintf(stderr, "got end-of-file/error on fgets()\n");
	} else {
		fprintf(stderr, "\nread: %s", buf);
	}

	/*
	  restore original terminal settings
	*/

	if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &save)) {
		perror("tcsetattr()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
