/*
  usage:

  (opt)
  $ mkdir test
  $ touch ./test/a.txt
  $ touch ./test/b.txt

  display content of current "." directory
  $ ./dir.elf


  demostrates directory operations


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 356
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <stdbool.h>
#include <dirent.h>


/**
   list all files in directory dir_path
*/
static void
list_files(const char* dirpath)
{
	DIR *pdir;
	struct dirent *pd;
	bool is_curr; // true if "dirpath" is "."

	is_curr = strcmp(dirpath, ".") == 0;

	pdir = opendir(dirpath);
	if (NULL == pdir) {
		perror("opendir() failed");
		return;
	}

	// print dir + filename for each entry in this directory

	for (;;) {
		errno = 0;
		pd = readdir(pdir);
		if (NULL == pd) {
			break;
		}

		if (strcmp(pd->d_name, ".") == 0 ||
		    strcmp(pd->d_name, "..") == 0) {
			continue; // skip
		}

		if (!is_curr) {
			fprintf(stderr, "%s/", dirpath);
		}
		fprintf(stderr, "%s\n", pd->d_name);
	}

	if (0 != errno) {
		fprintf(stderr, "readdir() failed\n");
		exit(EXIT_FAILURE);
	}

	if (-1 == closedir(pdir)) {
		fprintf(stderr, "closedir() failed\n");
	}
}

int
main(int argc, char *argv[])
{
	// display current directory content
	list_files(".");

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
