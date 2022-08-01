/*
  usage:
  $ sudo ./utmpx.elf pi 10
    creating login entries in utmp and wtmp
            using pid 13309, line pts/3, id /3
    creating logout entries in utmp and wtmp
    READY.


  now run the demo (as here on a RasPi), then change to the dumper and
  dump the content of /var/log/wmtp

  $ sudo ./utmpx.elf pi 10
    creating login entries in utmp and wtmp
            using pid 14363, line pts/3, id /3
    ^Z
  -> CTRL+z / SIGHUP
    [1]+  Stopped                 sudo ./utmpx.elf pi 10

  $ cd ../login__utmpx-dump

  $ make clean && make
    rm -f utmpx.o utmpx.elf *~
    gcc -g -Wall -std=gnu99 -pedantic -Werror -c utmpx.c
    gcc -g -Wall -std=gnu99 -pedantic -Werror -o utmpx.elf utmpx.o

  $ ./utmpx.elf
    user     type       PID   line     id      host                date/time
    reboot   BOOT_TIME     0 ~      ~~  5.10.103- Thu Jan  1 01:00:03 1970
    runlevel RUN_LVL      53 ~      ~~  5.10.103- Sun Jul 31 00:53:12 2022
    LOGIN    LOGIN_PR    938 ttyAMA AMA0L           Sun Jul 31 00:53:12 2022
    LOGIN    LOGIN_PR    937 tty1   tty1L           Sun Jul 31 00:53:12 2022
    pi       DEAD_PR       0 tty1   :0  :0        Sun Jul 31 00:53:13 2022
    pi       USER_PR    1454 pts/0  ts/0p :pts/1:S. Sun Jul 31 21:01:23 2022
    pi       USER_PR    1427 pts/1  ts/1p 10.1.10.1 Sun Jul 31 21:01:21 2022
    pi       USER_PR    1454 pts/2  ts/2p :pts/1:S. Sun Jul 31 21:01:25 2022
    pi       USER_PR    1454 pts/3  ts/3p :pts/1:S. Sun Jul 31 21:01:26 2022
    pi       USER_PR    1454 pts/4  ts/4p :pts/1:S. Sun Jul 31 21:01:27 2022
    pi       DEAD_PR    1454 pts/5  ts/5p           Sun Jul 31 21:01:33 2022
    pi       USER_PR   14363 pts/3  /3            Sun Jul 31 22:52:57 2022
    READY.

  $ who
    pi       pts/0        2022-07-31 21:01 (:pts/1:S.0)
    pi       pts/1        2022-07-31 21:01 (10.1.10.139 via mosh [1427])
    pi       pts/2        2022-07-31 21:01 (:pts/1:S.1)
    pi       pts/3        2022-07-31 21:01 (:pts/1:S.2)
    pi       pts/4        2022-07-31 21:01 (:pts/1:S.3)
    pi       pts/3        2022-07-31 22:52

  $ ./utmpx.elf /var/log/wtmp
    user     type       PID   line     id      host                date/time
    pi       DEAD_PR    1276 pts/1  ts/1p           Wed Jun 15 14:29:13 2022
    pi       USER_PR    1276 pts/1  ts/1p mosh [127 Wed Jun 15 14:29:13 2022
    reboot   BOOT_TIME     0 ~      ~~  5.10.103- Thu Jan  1 01:00:09 1970
    runlevel RUN_LVL      53 ~      ~~  5.10.103- Wed Jun 15 14:17:10 2022
             INIT_PR     965 /dev/t tty1           Wed Jun 15 14:17:10 2022
             INIT_PR     966 /dev/t AMA0           Wed Jun 15 14:17:10 2022
    LOGIN    LOGIN_PR    965 tty1   tty1L           Wed Jun 15 14:17:10 2022
    LOGIN    LOGIN_PR    966 ttyAMA AMA0L           Wed Jun 15 14:17:10 2022
    pi       USER_PR    1073 pts/0  ts/0p 10.1.10.1 Wed Jun 15 14:19:29 2022
             DEAD_PR    1073 pts/0                Wed Jun 15 14:19:29 2022
    pi       USER_PR    1104 pts/1  ts/1p mosh [110 Wed Jun 15 14:19:29 2022
    pi       DEAD_PR    1104 pts/1  ts/1p           Wed Jun 15 14:19:29 2022
    pi       USER_PR    1104 pts/1  ts/1p 10.1.10.1 Wed Jun 15 14:19:29 2022
    pi       USER_PR    1144 pts/0  ts/0p :pts/1:S. Wed Jun 15 14:19:53 2022
    ...


  This program performs the required updates to utmp and wtmp in order
  to log in the user named on the command line, and then after
  sleeping a few seconds, log them out again. Normally, such actions
  would be associated with the creation and termination of a login
  session for a user. This program uses ttyname() to retrieve the name
  of the terminal device associated with a file descriptor.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 828
*/

#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <utmpx.h>
#include <paths.h>

#include "tlpi/get_num.h"


int
main(int argc, char *argv[])
{
	struct utmpx ut;
	char *dev_name;

	if (2 > argc) {
		fprintf(stderr, "usage:\n"
			"$ %s username [sleep-time]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  initialize login record for utmp and wtmp files
	*/

	memset(&ut, 0, sizeof(ut));
	ut.ut_type = USER_PROCESS;    // this is a user login
	strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
	if (-1 == time((time_t*) &ut.ut_tv.tv_sec)) {
		perror("time()");
		exit(EXIT_FAILURE);
	}

	ut.ut_pid = getpid();

	/*
	  set ut-line and ut_id based on the terminal associated with
	  'stdin'. This code assumes terminals named "/dev/[pt]t[sy]*"

	  the "/dev/" dirname is 5 characters; the "[pt]t[sy]"
	  filename prefix is 3 characters (making 8 characters in
	  all).
	*/

	dev_name = ttyname(STDIN_FILENO);
	if (NULL == dev_name) {
		perror("ttyname()");
		exit(EXIT_FAILURE);
	}

	if (8 >= strlen(dev_name)) {      // should never happen
		fprintf(stderr, "fatal: terminal name is too short: %s\n",
			dev_name);
		exit(EXIT_FAILURE);
	}

	strncpy(ut.ut_line, dev_name + 5, sizeof(ut.ut_line));
	strncpy(ut.ut_id, dev_name + 8, sizeof(ut.ut_id));

	fprintf(stderr, "creating login entries in utmp and wtmp\n");
	fprintf(stderr, "\tusing pid %ld, line %.*s, id %.*s\n",
		(long) ut.ut_pid, (int) sizeof(ut.ut_line), ut.ut_line,
		(int) sizeof(ut.ut_id), ut.ut_id);

	setutxent();  // rewind to start of utmp file
	if (NULL == pututxline(&ut)) {    // write login record to utmp
		perror("pututxline()");
		exit(EXIT_FAILURE);
	}
	updwtmpx(_PATH_WTMP, &ut);    // append login record to wtmp

	/*
	  sleep a while, so we can examine utmp and wtmp files
	*/

	sleep((2 < argc) ? get_int(argv[2], GN_NONNEG, "sleep-time") : 15);

	/*
	  now do a "logout"; use values from previously initialized
	  'ut', except fro changes below
	*/

	ut.ut_type = DEAD_PROCESS;   // required for logout record
	time((time_t*) &ut.ut_tv.tv_sec);  // stamp with logout time
	memset(&ut.ut_user, 0, sizeof(ut.ut_user)); // logout record has null username

	fprintf(stderr, "creating logout entries in utmp and wtmp\n");  // rewind to start of utmp file
	setutxent();
	if (NULL == pututxline(&ut)) {
		perror("pututxline()");
		exit(EXIT_FAILURE);
	}

	updwtmpx(_PATH_WTMP, &ut);

	endutxent();  // close utx operation

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
