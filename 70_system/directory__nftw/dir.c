/*
  usage:

  (opt)
  $ mkdir test
  $ touch ./test/a.txt
  $ touch ./test/b.txt

  display content of current "." directory
  $ ./dir.elf


  demostrates walking the directory tree using nftw()


  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 360
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ftw.h>

static void
usage(const char* name)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "$ %s [-d] [-m] [-p] [path]\n", name);
	fprintf(stderr, "\t-d use FTW_DEPTH flag\n");
	fprintf(stderr, "\t-m use FTW_MOUNT flag\n");
	fprintf(stderr, "\t-p use FTW_PHYS flag\n");
}

static int
dir_tree(const char* path, const struct stat *sbuf, int type, struct FTW* ftwb)
{
	if (FTW_NS == type) {
		// could not stat
		fprintf(stderr, "?");
	} else {
		// print file type
		switch (sbuf->st_mode & S_IFMT) {
		case S_IFREG: fprintf(stderr, "-"); break; // register
		case S_IFDIR: fprintf(stderr, "d"); break; // directory
		case S_IFCHR: fprintf(stderr, "c"); break; // char dev
		case S_IFBLK: fprintf(stderr, "b"); break; // block dev
		case S_IFLNK: fprintf(stderr, "l"); break; // symlink
		case S_IFIFO: fprintf(stderr, "p"); break; // fifo queue
		case S_IFSOCK: fprintf(stderr, "s"); break; // socket
		default: fprintf(stderr, "?"); break;
		}
	}
	fprintf(stderr, " %s  ",
		(type == FTW_D)  ? "D  " : (type == FTW_DNR) ? "DNR" :
		(type == FTW_DP) ? "DP " : (type == FTW_F)   ? "F  " :
		(type == FTW_SL) ? "SL " : (type == FTW_SLN) ? "SLN" :
		(type == FTW_NS) ? "NS " : "   ");
	if (type != FTW_NS) {
		fprintf(stderr, "%7ld ", (long) sbuf->st_ino);
	} else {
		fprintf(stderr, "\t\t");
	}
	fprintf(stderr, " %*s", 4 * ftwb->level, "");   // indent suitably
	fprintf(stderr, "%s\n", &path[ftwb->base]); // print basename
	return 0;  // tell nftw() to continue
}

int
main(int argc, char *argv[])
{
	int flags, opt;

	flags = 0;
	while (-1 != (opt = getopt(argc, argv, "dmp"))) {
		switch (opt) {
		case 'd': flags |= FTW_DEPTH; break;
		case 'm': flags |= FTW_MOUNT; break;
		case 'p': flags |= FTW_PHYS; break;
		default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (argc == 1 || argc > optind +1) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if (-1 == nftw((argc > optind) ? argv[optind] : ".", dir_tree, 10, flags)) {
		perror("nftw() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
