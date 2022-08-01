/*
  usage:
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
    READY.


  The utmpxname() function merely records a copy of the pathname given
  to it. It doesn't open the file, but does close any file previously
  opened by one of the other calls.

  The program uses some of the functions described in the Login
  section (LPI book) to dump the contents of a utmpx-format file. The
  following shell session log demonstrates the results when we use
  this program to dump the contents of /var/run/utmp.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 824
*/

#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <utmpx.h>
#include <paths.h>


int
main(int argc, char *argv[])
{
	struct utmpx *ut;

	if (1 < argc) {
		if (-1 == utmpxname(argv[1])) {
			perror("utmpxname()");
			exit(EXIT_FAILURE);
		}
	}

	setutxent();

	fprintf(stderr, "user     type       PID   line     id      host                date/time\n");
	while (NULL != (ut = getutxent())) {  // sequential scan to EOF
		fprintf(stderr, "%-8s ", ut->ut_user);
		fprintf(stderr, "%-9.9s ",
			(ut->ut_type == EMPTY) ?          "EMPTY" :
			(ut->ut_type == RUN_LVL) ?        "RUN_LVL" :
			(ut->ut_type == BOOT_TIME) ?      "BOOT_TIME" :
			(ut->ut_type == NEW_TIME) ?       "NEW_TIME" :
			(ut->ut_type == OLD_TIME) ?       "OLD_TIME" :
			(ut->ut_type == INIT_PROCESS) ?   "INIT_PR" :
			(ut->ut_type == LOGIN_PROCESS) ?  "LOGIN_PR" :
			(ut->ut_type == USER_PROCESS) ?   "USER_PR" :
			(ut->ut_type == DEAD_PROCESS) ?   "DEAD_PR" : "???");
		fprintf(stderr, "%5ld %-6.6s %-3.5s %-9.9s ",
			(long) ut->ut_pid, ut->ut_line, ut->ut_id, ut->ut_host);
		time_t tv_sec = ut->ut_tv.tv_sec;
		fprintf(stderr, "%s", ctime((time_t*) &tv_sec));
	}
	endutxent();

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
