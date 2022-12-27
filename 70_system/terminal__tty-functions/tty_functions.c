/*
  Demonstrate cbreak and raw modes

  usage:
    $ stty
      speed 38400 baud; line = 0;
      -brkint -imaxbel iutf8

    $ ./tty_functions.elf
      abc            // type 'abc', and CTRL-j
         def         // type 'DEF', CTRL-j and ENTER
      ^C^Z           // type CTRL-c, CTRL-z, and CTRL-j
         q<ENTER>    // type 'q' to exit
      READY.

  In the last line of the preceding shell session, we see that the
  shell printed its prompt on the same line as the q character that
  caused the program to terminate.

    $ ./tty_functions.elf x
      xyz            // type 'XYZ' and CTRL-z, terminal goes to background
      [1]+  Stopped                 ./tty_functions.elf x

    $ stty           // verify that terminal mode was restored
      speed 38400 baud; line = 0;
      -brkint -imaxbel iutf8

    $ fg             // resume in foreground
      ./tty_functions.elf x
      ***            // now, type '123' and CTRL-j
                     // type CTRL-c to terminate program...

    $ stty           // verify that terminal mode was restored
      speed 38400 baud; line = 0;
      -brkint -imaxbel iutf8


  A program taht places the terminal in raw or cbreak mode must be
  careful to return the terminal to a usable mode when it
  terminates. Among other tasks, this entails handling all of the
  signals that are likely to be sent to the program, so that the
  program is not prematurely terminated.

  The program performs the following:
  - Set the terminal to either cbreak mode, or raw mode, depending on
    whether a command-line argument (any string) is supplied. The
    proveious terminal settings are saved in the global variable
    user_termios

  - If the terminal was placed in cbreak mode, then signals can be
    generated from the terminal. These signals need to be handled so
    that terminating or suspending the program leaves the terminal in
    a state that the user expects. The program installs the same
    handler for SIGQUIT and SIGINT. The SIGSTP signal requires special
    treatment, so a different handler is installed for that signal.

  - Install a handler for the SIGTERM signal, in order to catch the
    default signal sent by the kill command.

  - Execute a loop that resads characters one at a time from stdin and
    echoes them on standard output. The program treats various input
    cahracters specially before outputting them:
  -- All letters are converted to lowercase before being ouput
  -- The newline \n and carriage return \r characters are echoes
     without change
  -- Control characters other than the newline and carriage return are
     echoed as a 2-character sequence: caret (^) plus the
     corresponding uppercase letter e.g. CTRL-a echoes as '^A'
  -- All other characters are echoed as asterisks (*)
  -- The letter q causes the loop to terminate

  - On exit from the loop, restore the terminal to its state as last
    set by the user, and then terminate

  The program installs the same handler for SIGQUIT, SIGINT, and
  SIGTERM. This handler restores the terminal to its state as last set
  by the user and terminates the program.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1313
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>

#include <errno.h>


static struct termios user_termios;  // terminal settings as define by user


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

/*
  Place terminal referred to by 'fd' in raw mode (noncanonical) mode
  with all input and output processing disabled). Return 0 on success,
  or -1 on error. If 'prev_termios' is non-NULL, then use the buffer
  to which it points to return the previous terminal settings

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1310
*/
int
tty_set_raw(int fd, struct termios *prev_termios)
{
	struct termios tios;

	if (-1 == tcgetattr(fd, &tios)) {
		return -1;
	}

	if (NULL != prev_termios) {
		*prev_termios = tios;
	}

	tios.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
	// noncanonical mode, disable signals, extended input
	// processing, and echoing

	tios.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);
	// disable special handoling of CR, NL, and BREAK no 8th-bit
	// stripping or parity error handling; disable START/STOP
	// output flow control

	tios.c_oflag &= ~OPOST;   // disable all output processing

	tios.c_cc[VMIN] = 1;      // character-at-a-time input
	tios.c_cc[VTIME] = 0;     // with blocking

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
handler(int sig)
{
	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios)) {
		perror("tcsetattr()");
		_exit(EXIT_FAILURE);
	}
	_exit(EXIT_SUCCESS);
}

/*
  handler for SIGTSTP
*/
static void
tstp_handler(int sig)
{
	struct termios our_termios;  // to save our tty settings
	sigset_t tstp_mask, prev_mask;
	struct sigaction sa;
	int saved_errno;

	saved_errno = errno;   // we might change 'errno' here

	/*
	  save current terminal settings, restore terminal to state at
	  time of program startup
	*/

	if (-1 == tcgetattr(STDIN_FILENO, &our_termios)) {
		perror("tcgetattr()");
		_exit(EXIT_FAILURE);
	}
	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios)) {
		perror("tcsetattr()");
		_exit(EXIT_FAILURE);
	}

	/*
	  set the disposition of SIGTSTP to the default, raise the
	  signal once more, and then unblock it so that we actually
	  stop
	*/

	if (SIG_ERR == signal(SIGTSTP, SIG_DFL)) {
		perror("signal");
		_exit(EXIT_FAILURE);
	}

	raise(SIGTSTP);

	sigemptyset(&tstp_mask);
	sigaddset(&tstp_mask, SIGTSTP);
	if (-1 == sigprocmask(SIG_UNBLOCK, &tstp_mask, &prev_mask)) {
		perror("sigprocmask()");
		_exit(EXIT_FAILURE);
	}

	/*
	  execution resumes here after SIGCONT
	*/

	if (-1 == sigprocmask(SIG_SETMASK, &prev_mask, NULL)) {
		perror("sigprocmask()");
		_exit(EXIT_FAILURE);  // reblock SIGTSTP
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = tstp_handler;
	if (-1 == sigaction(SIGTSTP, &sa, NULL)) {
		perror("sigaction()");
		_exit(EXIT_FAILURE);
	}

	/*
	  the user may have changed the terminal settings while we
	  were stopped; save the settings so we can restore them later
	*/

	if (-1 == tcgetattr(STDIN_FILENO, &user_termios)) {
		perror("tcgetattr()");
		_exit(EXIT_FAILURE);
	}

	/*
	  restore our terminal settings
	*/
	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &our_termios)) {
		perror("tcsetattr()");
		_exit(EXIT_FAILURE);
	}

	errno = saved_errno;
}


int
main(int argc, char *argv[])
{
	char ch;
	struct sigaction sa, prev;
	ssize_t nread;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (1 < argc) {   // use cbreak mode
		if (-1 == tty_set_cbreak(STDIN_FILENO, &user_termios)) {
			perror("tty_set_cbreak()");
			_exit(EXIT_FAILURE);
		}

		/*
		  terminal special characters can generate signals in
		  cbreak mode; catch them so that we can adjust the
		  terminal mode

		  we establish handlers only if the signals are not
		  being ignored
		*/

		sa.sa_handler = handler;

		if (-1 == sigaction(SIGQUIT, NULL, &prev)) {
			perror("sigaction()");
			_exit(EXIT_FAILURE);
		}

		if (prev.sa_handler != SIG_IGN) {
			if (-1 == sigaction(SIGQUIT, &sa, NULL)) {
				perror("sigaction()");
				_exit(EXIT_FAILURE);
			}
		}

		if (-1 == sigaction(SIGINT, NULL, &prev)) {
			perror("sigaction()");
			_exit(EXIT_FAILURE);
		}

		if (SIG_IGN != prev.sa_handler) {
			if (-1 == sigaction(SIGINT, &sa, NULL)) {
				perror("sigaction()");
				_exit(EXIT_FAILURE);
			}
		}

		sa.sa_handler = tstp_handler;
		if (-1 == sigaction(SIGTSTP, NULL, &prev)) {
			perror("sigaction()");
			_exit(EXIT_FAILURE);
		}

		if (SIG_IGN != prev.sa_handler) {
			if (-1 == sigaction(SIGTSTP, &sa, NULL)) {
				perror("sigaction()");
				_exit(EXIT_FAILURE);
			}
		}
	} else {
		if (-1 == tty_set_raw(STDIN_FILENO, &user_termios)) {
			perror("tty_set_raw()");
			_exit(EXIT_FAILURE);
		}
	}

	sa.sa_handler = handler;
	if (-1 == sigaction(SIGTERM, &sa, NULL)) {
		perror("sigaction()");
		_exit(EXIT_FAILURE);
	}

	setbuf(stdout, NULL);  // disable stdout buffering

	while (true) {   // read and echo stdin
		nread = read(STDIN_FILENO, &ch, 1);
		if (-1 == nread) {
			perror("read()");
			break;
		}

		if (0 == nread) {
			// can occur after terminal disconnect
			break;
		}

		if (isalpha((unsigned char) ch)) {  // letters --> lowercase
			putchar(tolower((unsigned char) ch));
		} else if (ch == '\n' || ch == '\r') {
			putchar(ch);
		} else if (iscntrl((unsigned char) ch)) {
			printf("^%c", ch ^ 64);  // echo CTRL-a as ^A, etc.
		} else {
			putchar('*');  // all other chars as '*'
		}

		if ('q' == ch) {  // quit loop
			break;
		}
	}

	if (-1 == tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios)) {
		perror("tcsetattr()");
		_exit(EXIT_FAILURE);
	}

	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
