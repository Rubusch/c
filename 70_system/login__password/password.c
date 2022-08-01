/*
  usage:
  $ ./password.elf
    username: pi
    fatal: no permission to read shadow password file

  $ sudo ./password.elf
    username: pi
    password:
    successfully authenticated: UID=1000
    READY.


  Demonstrates how to use crypt() to authenticate a user. This program
  first reads a username and then retrieves the corresponding password
  record and (if it exists) shadow password record. The program prints
  an error message and exits if no password record is found, or if the
  program doesn't have permission to read from the shadow password
  file (this requires either superuser privilege or membership of the
  shadow group). The program then reads the user's password, using the
  getpass() function.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 164
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

#include <crypt.h>



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
		lnmax = 256;  // if limit is indeterminate make an "educated-byte-code-guess" ;)
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

// NB: password operations need privileges, alternatively set the
// particular privileges (needs libcaps-dev)
//	/*
//	  only raise CAP_DAC_READ_SEARCH for as long as we need it
//	*/
//
//	if (-1 == raise_cap(CAP_DAC_READ_SEARCH)) {
//		fprintf(stderr, "fatal: raise_cap() failed\n");
//		exit(EXIT_FAILURE);
//	}


	spwd = getspnam(username);
	if (spwd == NULL && errno == EACCES) {
		fprintf(stderr, "fatal: no permission to read shadow password file\n");
		exit(EXIT_FAILURE);
	}

// NB: password operations need privileges, alternatively set the
// particular privileges (needs libcaps-dev)
//	/*
//	  at this point, we won't need any more capabilities, so drop
//	  all capabilities from all sets
//	*/
//
//	if (-1 == drop_all_caps()) {
//		fprintf(stderr, "fatal: dropt_all_caps() failed\n");
//		exit(EXIT_FAILURE);
//	}


	if (NULL != spwd) {   // if there is a shadow password record
		pwd->pw_passwd = spwd->sp_pwdp;  // use the shadow password
	}
	password = getpass("password: ");

	/*
	  encrypt password and erase cleartext version immediately
	*/

	encrypted = crypt(password, pwd->pw_passwd);
	for (ptr = password; *ptr != '\0'; ) { // zeroize password variable: explicit_bzero(password, <size>);
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
