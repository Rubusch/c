/*
  Demonstrate signal-driven I/O on a terminal

  usage:
    $ ./sigio.elf
      cnt=1; read x    // type 'x'
      cnt=3; read x    // type 'x'
      cnt=7; read x    // ...
      cnt=8; read x
      cnt=10; read x
      cnt=11; read x
      cnt=11; read x
      cnt=13; read x
      cnt=15; read x
      cnt=18; read 3  // type something else, e.g. '3'
      cnt=22; read #  // type '#'
      READY.


  The program performs the steps for enabling signal-driven I/O on
  standard input, and then places the terminal in cbreak mode, so that
  input is available a character at a time. The program then enters an
  infinite loop, performing the "work" fo incrementing a variable,
  cnt, while waiting for input to become available. Whenever input
  becomes available, the SIGIO handler sets a flag, got_sigio, that is
  monitored by the main program. When the main program sees that this
  flag is set, it reads all available input characters and prints them
  along with the current value of cnt. If a hash character '#' is read
  in the input, the program terminates.


  references:
  Linux Programming Interface, Michael Kerrisk, 2010, p. 1348
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>

#include <errno.h>


static volatile sig_atomic_t got_sigio = 0;  // set nonzero on receipt of SIGIO


/*
  Place terminal referred to by 'fd' in cbreak mode (noncanonical mode
  with echoing turned off). This function assumes that the terminal is
  currently in cooked mode i.e. we shouldn't call it if the terminal
  made by the tty_set_raw() function below. Return 0 on success, or -1
  on error. If 'prev_termios' is non-NULL, then use the buffer to
  which it points to return the previous terminal settings.

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1310
*/
int
tty_set_cbreak(int fd, struct termios *prev_termios)
{
	struct termios tios;

	if (-1 == tcgetattr(fd, &tios)) {
		return -1;
	}

	if (NULL != prev_termios) {
		*prev_termios = tios;
	}

	tios.c_lflag &= ~(ICANON | ECHO);
	tios.c_lflag |= ISIG;

	tios.c_iflag &= ~ICRNL;

	tios.c_cc[VMIN] = 1;   // character-at-a-time input
	tios.c_cc[VTIME] = 0;  // with blocking

	if (-1 == tcsetattr(fd, TCSAFLUSH, &tios)) {
		return -1;
	}

	return 0;
}
/* --- */

/*
  general handler: restore tty settings and exit
*/
static void
sigio_handler(int sig)
{
	got_sigio = 1;
}

int
main(int argc, char *argv[])
{
	int flags, idx, cnt;
	struct termios orig_termios;
	char ch;
	struct sigaction sa;
	bool done;

	/*
	  establish handler for "I/O possible" signal
	*/

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sigio_handler;
	if (-1 == sigaction(SIGIO, &sa, NULL)) {
		perror("sigaction()");
		_exit(EXIT_FAILURE);
	}

	/*
	  set owner process that is to receive "I/O possible" signal
	*/

	if (-1 == fcntl(STDIN_FILENO, F_SETOWN, getpid())) {
		perror("fcntl(F_SETOWN)");
		_exit(EXIT_FAILURE);
	}

	/*
	  enable "I/O possible" signaling and make I/O nonblocking for
	  file descriptor
	*/

	flags = fcntl(STDIN_FILENO, F_GETFL);
	if (-1 == fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK)) {
		perror("fcntl(F_SETFL)");
		_exit(EXIT_FAILURE);
	}

	/*
	  place terminal in cbreak mode
	*/

	if (-1 == tty_set_cbreak(STDIN_FILENO, &orig_termios)) {
		perror("tty_set_cbreak()");
		_exit(EXIT_FAILURE);
	}

	for (done = false, cnt = 0; !done; cnt++) {
		for (idx = 0; idx < 100000000; idx++) {
			continue;  // slow main loop down a little...
		}

		if (got_sigio) {   // is input available?
			got_sigio = 0;

			/*
			  read all available input until error
			  (probably EAGAIN) or EOF (not actually
			  possible in cbreak mode) or a hash '#'
			  character is read
			*/

			while (!done && 0 < read(STDIN_FILENO, &ch, 1)) {
				fprintf(stderr, "cnt=%ld; read %c\n", (long) cnt, ch);
				done = ch == '#';
			}
		}
	}

	/*
	  restore original terminal settings
	*/

	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)) {
		perror("tcsetattr()");
		_exit(EXIT_FAILURE);
	}

	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
