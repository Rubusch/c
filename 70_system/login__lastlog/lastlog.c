/*
  usage:
  $ ./lastlog.elf
    usage:
  $ ./lastlog.elf [username...]
    READY.

  $ ./lastlog.elf pi
    pi       pts/0  10.1.10.139          Mon Aug  1 11:09:06 2022
    READY.


  The lastlog file records the time each user last logged in to the
  system. (This is different from the wtmp file, which records all
  logins and logouts by all users.) Among other things, the lastlog
  file allows the login program to inform users (at the start of a new
  login session) when they last logged in. In addition to updating
  utmp and wtmp, applications providing login services should also
  update lastlog.

  On linux this file resides at /var/log/lastlog, and a constant,
  _LAST_LASTLOG, is defined in <paths.h> to point to this location
  (normally protected).

  NB: these records don't include a username or a user ID, instead the
  lastlog file consissts of a series of records that are indexed by
  user ID.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 831
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>
#include <ctype.h>

#include <time.h>
#include <lastlog.h>
#include <paths.h>
#include <fcntl.h>


uid_t  // return UID corresponding to 'name', or -1 on error
user_id_from_name(const char* name)
{
	struct passwd *pwd;
	uid_t uid;
	char *endptr;
	if  (NULL == name || *name == '\0') {  // on NULL or empty string return an error
		return -1;
	}

	uid = strtol(name, &endptr, 10);  // as a convenience to caller allow a numeric string
	if ('\0' == *endptr) {
		return uid;
	}

	if (NULL == (pwd = getpwnam(name))) { // given a login name in
					      // name, the getpwnam()
					      // function returns a
					      // pointer to a
					      // structure of the
					      // folloing type,
					      // containing the
					      // corresponding
					      // information from the
					      // password record
		return -1;
	}

	return pwd->pw_uid;
}

int
main(int argc, char *argv[])
{
	struct lastlog llog;
	int fd, idx;
	uid_t uid;

	if (1 == argc) {
		fprintf(stderr, "usage:\n"
			"$ %s [username...]\n", argv[0]);
	}

	fd = open(_PATH_LASTLOG, O_RDONLY);
	if (-1 == fd) {
		perror("open()");
		exit(EXIT_FAILURE);
	}

	for (idx = 1; idx < argc; idx++) {
		uid = user_id_from_name(argv[idx]);
		if (-1 == uid) {
			fprintf(stderr, "no such user: %s\n", argv[idx]);
			continue;
		}

		if (-1 == lseek(fd, uid * sizeof(llog), SEEK_SET)) {
			perror("lseek()");
			exit(EXIT_FAILURE);
		}

		if (0 >= read(fd, &llog, sizeof(llog))) {
			fprintf(stderr, "read failed for %s\n", argv[idx]);   // EOF or error
			continue;
		}

		time_t ll_time = llog.ll_time;
		fprintf(stderr, "%-8.8s %-6.6s %-20.20s %s",
			argv[idx], llog.ll_line, llog.ll_host, ctime(&ll_time));
	}

	close(fd);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
