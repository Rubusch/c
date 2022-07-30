/*
  usage:
    $ touch /tmp/ds.log          // make sure this can be written

    $ ./daemon.elf

    $ ps -C daemon.elf -o "pid ppid pgid sid tty command"
        PID  PPID  PGID   SID TT       COMMAND
      16475     1 16474 16474 ?        ./daemon.elf

    $ killall daemon.elf

    $ ps -C daemon.elf -o "pid ppid pgid sid tty command"
        PID  PPID  PGID   SID TT       COMMAND

    $ cat /tmp/ds.log
      MOCK: log_open(/tmp/ds.log)
      MOCK: read_config_file(/tmp/ds.conf)


  In the output of ps, the '?' under the 'TT' heading indicates that
  the process has no controlling terminal. From the fact that the
  process ID is not the same as the session ID (SID), we can also see
  that the process is not the leader of its session, and so won't
  reacquire a controlling terminal if it opens a terminal device. This
  is as things should be for a daemon


  Creating a daemon:

  1. Perform a fork(), after which the parent exits and the child
    continues.

    -- Assuming the daemon was started from the command line, the
       parent's termination is noticed by the shell, which then
       displays another shell prompt and leaves the child to continue
       in the background.

    -- The child process is guaranteed not to be a process group
       leader.

  2. The child process calls setsid() to start a new session.

  3. If the daemon might later open a terminal device, then we must
     take steps to ensure that the device does ot become the
     controlling terminal.

    -- Specify the O_NOCITY flag on any open()

    -- Alternatively, and more simply, perform a second fork() after
       the setsid(), the process can never reacquire a controlling
       terminal

  4. Clear the process umask, to ensure the correct permissions

  5. Change the process's current working directory, typically to the
     root directory (/). This is necessary because a daemon usually
     runs until system shutdown; if the daemon's current working
     directory is on a file system other than the one containing /,
     then that file system can't be unmounted

  6. Close all open file descriptors that the daemon has inherited
     from its parent.

  7. After having closed file descriptors 0, 1, and 2, a daemon
     normally opens /dev/null and uses dup2() or similar, to make all
     those descriptors refer to this device

     Reasons:
    -- It ensures that if the daemon calls library functions that
       perform I/O on these descriptors, those funcitons won't
       unexpectedly fail.

    -- It prevents the possibility that the daemon later opens a file
       using descriptor 1 or 2, which is then written to - and thus
       corrupted - by a library function that expects to treat these
       descriptors as standard output and standard error.


  The listing provides an example of how a daemon can employ
  SIGHUP. This program establishes a handler for SIGHUP, becomes a
  daemon, opens the log file, and reads its configuration file. The
  SIGHUP handler just sets a global flag variable sighup_received,
  which is checked by the main program. The main program sits in a
  loop, printing a message to the log file every 15 seconds. The calls
  to sleep() in this loop are intended to simulate some sort of
  processing performed by a real application. After each return from
  sleep() in this loop, the program checks to see whether
  sighup_received has been set; if so, it reopens the log file,
  rereads the configuration file, and clears the sighup_received flag.

  For brevity the functions log_open(), log_close(), log_message(),
  and read_config_file() are omitted from the Listing here (see online
  demo for the Linux Programming Interface).


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 770-775
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <signal.h>
#include <unistd.h> /* sleep() */

#include "daemon.h"


// LOG_FILE in daemon.h
static const char *CONFIG_FILE = "/tmp/ds.conf";

// definitions of log_message(), log_open(), log_close(), and
// read_config_file() are omitted from this listening
static volatile sig_atomic_t sighup_received = 0; // set nonzero on receipt of SIGHUP

static void
sighup_handler(int sig)
{
	sighup_received = 1;
}


/* dummies */

void
log_open(const char* filename)
{
	fprintf(stderr, "MOCK: %s(%s)\n", __func__, filename);
}

void
log_messages(const char* filename, const char* format, ...)
{
	va_list argp;
	va_start(argp, format);
	fprintf(stderr, "MOCK: %s(%s) - ", __func__, filename);
	vprintf(format, argp);
	fprintf(stderr, "\n");
	va_end(argp);
}

void
log_close()
{
	fprintf(stderr, "MOCK: %s()\n", __func__);
}

void
read_config_file(const char* filename)
{
	fprintf(stderr, "MOCK: %s(%s)\n", __func__, filename);
}

int main(int argc, char **argv)
{
	const int SLEEP_TIME = 15; // time to sleep between messages
	int count = 0;   // number of completed SLEEP_TIME intervals
	int unslept;     // time remaining in sleep interval
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sighup_handler;
	if (-1 == sigaction(SIGHUP, &sa, NULL)) {
		perror("sigaction()");
		exit(EXIT_FAILURE);
	}

	if (-1 == become_daemon(0)) {
		perror("become_daemon()");
		exit(EXIT_FAILURE);
	}

	log_open(LOG_FILE);
	read_config_file(CONFIG_FILE);

	unslept = SLEEP_TIME;

	while (1) {
		unslept = sleep(unslept); // returns > 0 if interrupted

		if (sighup_received) {
			sighup_received = 0;
			log_close();
			log_open(LOG_FILE);
			read_config_file(CONFIG_FILE);
		}

		if (0 == unslept) { // on completed interval
			count++;
			char tmp[16]; memset(tmp, '\0', 16);
			sprintf(tmp, "%i", count);
			log_messages("Main: %d", tmp);
			unslept = SLEEP_TIME; // reset interval
		}
	}

	fprintf(stderr, "READY.\n");
	return EXIT_SUCCESS;
}
