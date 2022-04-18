/*
  usage:
  $ ./dir.elf <symlink>
  e.g.
  $ ./dir.elf /usr/src/github__c/70_system/directory__dirname-basename
    /usr/src/github__c/70_system/directory__dirname-basename ==> /usr/src/github__c/70_system + directory__dirname-basename
    READY.


  demostrates dirname / basename


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 371
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

char* my_strdup(const char* arg)
{
	char *tr;

	tr = strdup(arg);
	if (NULL == tr) {
		fprintf(stderr, "strdup() failed\n");
		exit(EXIT_FAILURE); // frees all resources
				    // automatically, but bad
				    // style
	}
	return tr;
}


int
main(int argc, char *argv[])
{
	char *t1, *t2;
	int idx;

	for (idx = 1; idx < argc; idx++) {
		t1 = my_strdup(argv[idx]);
		t2 = my_strdup(argv[idx]);
		fprintf(stderr, "%s ==> %s + %s\n", argv[idx], dirname(t1), basename(t2));

		free(t1);
		free(t2);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
