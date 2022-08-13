/*
  file lock demo

  usage:

  Running two instances to place locks on the same 100-byte file
  (tfile). TLPI, p1133 shows graphicaly the following lock session.


  terminal #1                           terminal #2

  Placing a READ lock 'r' on bytes 0 to 39 of the lock file tfile in
  terminal #1.

  $ ls -l ./tfile
    -rw-r--r-- 1 pi pi 100 Aug 13 18:36 ./tfile
                       ^^^
  $ ./flock.elf tfile
    enter ? for help
    PID=9734> s r 0 40
    [PID=9734] got lock

  Then start a second instance of the program (terminal #2), placing a
  READ lock 'r' on a bytes 70 through to the end of the file.

                                        $ ./flock.elf tfile
                                          enter ? for help
                                          PID=9737> s r -30 0 e
                                          [PID=9737] got lock

  Now return to terminal #1, where we try to place a write lock on the
  entire file. We first employ F_GETLK to test whether the lock can be
  placed and are informed that there is a clonflicting lock. Then try
  placing the lock with F_SETLK, which also fails. Finally, try
  placing the lock with F_SETLKW, which blocks.

    PID=9734> g w 0 0
    [PID=9734] denied by READ lock on 70:0 (held by PID 9737)
    PID=9734> s w 0 0
    [PID=9734] failed (incompatible lock)
    PID=9734> w w 0 0
      <waiting on lock>

  Continue in terminal #2, by trying to place a write lock on the
  entire file. First test whether the lock can be placed using
  F_GETLK, which informs us that there is a conflicting lock. Then try
  placing the lock using F_SETLKW.

                                        PID=9737> g w 0 0
                                        [PID=9737] denied by READ lock on 0:40 (held by PID 9734)
                                        PID=9737> w w 0 0
                                        [PID=9737] failed (deadlock)

  When terminal #2 made a blocking request to place a write lock on
  the entire file, the result is a deadlock.

  Continuing in terminal #2, remove the lock, and terminal #1 locks.

                                        PID=9737> s u 0 0
                                        [PID=9737] unlocked

    [PID=9734] got lock


  The program allows to interactively experiment with record
  locking. This program takes a single command-line argument: the name
  of a file on which we wish to place locks. Using this program, we
  can verify many of our previous statements regarding the operation
  of reconrd locking. The program is designed to be used interactively
  and accepts commands of the form:

  > cmd lock start length [whence]

  For cmd, we can specify g to perform an F_GETLK, s to perform an
  F_SETLK, or w to perform an F_SETLKW. The remaining arguments are
  used to initialize the flock strucutre passed to fcntl(). The lock
  argument specifies the value for the l_type arguments are integers
  specifying the values for the l_start and l_len fields. Finally, the
  optional whence argument specifies the value for the l_whence field,
  and may be s for SEEK_SET (the default), c for SEEK_CUR, or e for
  SEEK_END. (For an explanation of why we cast the l_start and l_len
  fields to long long in the printf() call in the listing).

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 1130
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_LINE 100

static void
display_cmd_fmt(void)
{
	fprintf(stderr, "\n\tformat: cmd lock start length [whence]\n\n");
	fprintf(stderr, "\t'cmd' is 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)\n");
	fprintf(stderr, "\t'lock' is 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)\n");
	fprintf(stderr, "\t'start' and 'length' specify byte range to lock\n");
	fprintf(stderr, "\t'whence' is 's' (SEEK_SET, default), 'c' (SEEK_CUR), "
		"or 'e' (SEEK_END)\n\n");
}

int
main(int argc, char *argv[])
{
	int fd, nread, cmd, status;
	char lock, cmdch, whence, line[MAX_LINE];
	long long len, st;

	struct flock fl; // the file lock

	if (2 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDWR);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "enter ? for help\n");
	while (true) {   // prompt for locking command and carry it out
		fprintf(stderr, "PID=%ld> ", (long) getpid());

		if (NULL == fgets(line, MAX_LINE, stdin)) {  // EOF
			exit(EXIT_FAILURE);
		}
		line[strlen(line) - 1] = '\0';  // remove trailing '\n'

		if ('\0' == *line) {  // spkip blank lines
			continue;
		}

		if ('?' == line[0]) {
			display_cmd_fmt();
			continue;
		}

		whence = 's';  // in case not otherwise filled in
		nread = sscanf(line, "%c %c %lld %lld %c",
			       &cmdch, &lock, &st, &len, &whence);

		fl.l_start = st;  // file lock: initialize start and length
		fl.l_len = len;

		if (4 > nread
		    || NULL == strchr("gsw", cmdch)
		    || NULL == strchr("rwu", lock)
		    || NULL == strchr("sce", whence)) {
			fprintf(stderr, "invalid commdn!\n");
			continue;
		}

		cmd = ('g' == cmdch)
			? F_GETLK
			: ('s' == cmdch)
			    ? F_SETLK
			    : F_SETLKW;

		fl.l_type = ('r' == lock)  // file lock: l_type
			? F_RDLCK
			: ('w' == lock)
			    ? F_WRLCK
			    : F_UNLCK;

		fl.l_whence = ('c' == whence) // file lock: l_whence
			? SEEK_CUR
			: ('e' == whence)
			    ? SEEK_END
			    : SEEK_SET;

		status = fcntl(fd, cmd, &fl);  // file lock: perform request...


		if (F_GETLK == cmd) {   // ...and see what happened - get lock 'g'
			if (-1 == status) {
				fprintf(stderr, "fcntl - F_GETLK\n");
			} else {
				if (F_UNLCK == fl.l_type) {
					fprintf(stderr,
						"[PID=%ld] lock can be placed\n",
						(long) getpid()); // locked out by someone else
				} else {
					fprintf(stderr,
						"[PID=%ld] denied by %s lock on %lld:%lld "
						"(held by PID %ld)\n",
						(long) getpid(),
						(F_RDLCK == fl.l_type) ? "READ" : "WRITE",
						(long long) fl.l_start,
						(long long) fl.l_len,
						(long) fl.l_pid);
				}
			}

		} else {  // F_SETLK, F_SETLKW - set lock 's' and else
			if (0 == status) {
				fprintf(stderr, "[PID=%ld] %s\n",
					(long) getpid(),
					('u' == lock) ? "unlocked" : "got lock");
			} else if (EAGAIN == errno || EACCES == errno) {  // F_SETLK
				fprintf(stderr, "[PID=%ld] failed (incompatible lock)\n",
					(long) getpid());
			} else if (EDEADLK == errno) {
				fprintf(stderr, "[PID=%ld] failed (deadlock)\n",
					(long) getpid());
			} else {
				fprintf(stderr, "fcntl - F_SETLK(W)\n");
				break;
			}
		}
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
