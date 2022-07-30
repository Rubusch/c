/*
  usage:
  $ sudo setcap "cap_dac_read_search=p" caps.elf

  $ ./caps.elf
    username: pi
    password: *****
    successfully authenticated: UID=1000
    READY.

  $ getcap ./caps.elf
    ./caps.elf cap_dac_read_search=p


  We [Linux Programming Interface] present a program that
  authenticates a username plus password against the standard password
  database. We noted that the program requires privilege in order to
  read the shadow password file, which is protected to prevent reading
  by users other than 'root' or members of the shadow group. The
  traditional way of providing this program with the privileges that
  it requires would be to run it under a root login or to make it a
  set-user-ID-root program. We now present a modified version of this
  program that employs capabilities and the libcap API.

  This program uses the libcap API to rais CAP_DAC_READ_SEARCH in its
  effective capability set just before accessing the shadow password
  file, and then drops the capability again immediately after this
  access.

  Under debian:
  $ sudo apt install -y libcap-dev
  ...and adjust the Makefile with -lcap and -lcrypt


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 808 - 811
*/

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE /* crypt() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <pwd.h>

#include <sys/capability.h>
#include <crypt.h>


/* change setting of capability in caller's efective capabilities */

static int
modify_cap(int capability, int setting)
{
	cap_t caps;
	cap_value_t cap_list[1];

	/* retrueve caller's current capabilities */

	caps = cap_get_proc();
	if (NULL == caps) {
		return -1;
	}

	/*
	  change setting of 'capability' in the effective set of
	  'caps'

	  the third argument, 1, is the number of items in the array
	  'cap_list'
	*/

	cap_list[0] = capability;
	if (-1 == cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, setting)) {
		cap_free(caps);
		return -1;
	}

	/*
	  push modified capability sets back to kernel, to change
	  caller's capability
	*/

	if (-1 == cap_set_proc(caps)) {
		cap_free(caps);
		return -1;
	}

	/*
	  free the structure that was allocated by libca
	*/

	if (-1 == cap_free(caps)) {
		return -1;
	}

	return 0;
}

static int
raise_cap(int capability) // raise capability in caller's effective set
{
	return modify_cap(capability, CAP_SET);
}

/*
  an analogous drop_cap() could be defined as

  modify_cap(capability, CAP_CLEAR)
  [not needed in this program]
*/

static int
drop_all_caps(void)
{
	cap_t empty;
	int res = -1;

	empty = cap_init();
	if (NULL == empty) {
		return -1;
	}

	res = cap_set_proc(empty);
	if (-1 == cap_free(empty)) {
		return -1;
	}

	return res;
}

int
main(int argc, char *argv[])
{
	char *username, *password, *encrypted, *ptr;
	struct passwd *pwd;
	struct spwd *spwd;
	bool auth_ok;
	size_t len;
	long lnmax;

	lnmax = sysconf(_SC_LOGIN_NAME_MAX);
	if (lnmax == -1) {
		lnmax = 256;
	}

	username = malloc(lnmax);
	if (NULL == username) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	// ask password
	printf("username: ");
	fflush(stdout);
	if (NULL == fgets(username, lnmax, stdin)) {
		exit(EXIT_FAILURE); // exit on EOF
	}

	len = strlen(username);
	if (username[len - 1] == '\n') {
		username[len - 1] = '\0';
	}

	pwd = getpwnam(username);
	if (NULL == pwd) {
		fprintf(stderr, "fatal: couldn't get password record\n");
		exit(EXIT_FAILURE);
	}

	/*
	  only raise CAP_DAC_READ_SEARCH for as long as we need it
	*/

	if (-1 == raise_cap(CAP_DAC_READ_SEARCH)) {
		fprintf(stderr, "fatal: raise_cap() failed\n");
		exit(EXIT_FAILURE);
	}

	spwd = getspnam(username);
	if (spwd == NULL && errno == EACCES) {
		fprintf(stderr, "fatal: no permission to read shadow password file\n");
		exit(EXIT_FAILURE);
	}

	/*
	  at this point, we won't need any more capabilities, so drop
	  all capabilities from all sets
	*/

	if (-1 == drop_all_caps()) {
		fprintf(stderr, "fatal: dropt_all_caps() failed\n");
		exit(EXIT_FAILURE);
	}

	if (NULL != spwd) {   // if there is a shadow password record
		pwd->pw_passwd = spwd->sp_pwdp;  // use the shadow password
	}
	password = getpass("password: ");

	/*
	  encrypt password and erase cleartext version immediately
	*/

	encrypted = crypt(password, pwd->pw_passwd);
	for (ptr = password; *ptr != '\0'; ) {
		*ptr++ = '\0';
	}

	if (NULL == encrypted) {
		fprintf(stderr, "fatal: crypt()\n");
		exit(EXIT_FAILURE);
	}

	auth_ok = (strcmp(encrypted, pwd->pw_passwd) == 0);
	if (!auth_ok) {
		fprintf(stderr, "incorrect password\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

	/*
	  now do authenticated work...
	*/

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
