/*
  change interrupt character

  usage:
    $ ./change_interrupt_character.elf 0x0c
      READY.

    $ stty
      speed 38400 baud; line = 0;
      intr = ^L;
      -brkint -imaxbel iutf8

    $ sleep 10
      ^C <no effect>

    Type CTRL-l to stop the "sleep"
      ^L

    Then start a process sleep. Find that typing CTRL-c no longer has
    its usual effect of terminating a process, but typing CTRL-l does
    terminate the process.

    $ echo $?
      130

    See that the termination status of the process was 130. This
    indicates that the process was killed by signal 130 - 128 = 2;
    signal number 2 is SIGINT (p. 1300).


  The proggy can disable SIGINT when running it w/o argument.

    $ ./change_interrupt_character.elf
      READY.

    $ stty
      speed 38400 baud; line = 0;
      intr = <undef>;
      -brkint -imaxbel iutf8


  Reset SIGINT by executing
      # ./change_interrupt_signal.elf 0x03


  The program shows the use of tcgetattr() and tcsetattr() to change
  the terminal interrupt character. This program sets the interrupt
  character to be the character whose numeric value is supplied as the
  program's command-line argument, or disables the interrupt character
  if no command-line argument is supplied.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1301
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>


int
main(int argc, char *argv[])
{
	struct termios tp;
	int intr_char;

	if (2 < argc) {
		fprintf(stderr, "usage:\n$ %s [intr-char]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  determine new INTR setting from command line
	*/

	if (1 == argc) {  // disable INTR
		intr_char = fpathconf(STDIN_FILENO, _PC_VDISABLE);
		if (-1 == intr_char) {
			fprintf(stderr, "couldn't determine VDISABLE\n");
			exit(EXIT_FAILURE);
		}
	} else if (isdigit((unsigned char) argv[1][0])) {
		intr_char = strtoul(argv[1], NULL, 0);
		// alows hex, octal, literal chars
	} else {
		intr_char = argv[1][0];
	}

	/*
	  fetch current terminal settings, modify INTR character, and
	  push changes back to the terminal driver
	*/

	if (-1 == tcgetattr(STDIN_FILENO, &tp)) {
		perror("tcgetattr()");
		exit(EXIT_FAILURE);
	}
	tp.c_cc[VINTR] = intr_char;
	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp)) {
		perror("tcsetattr()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
