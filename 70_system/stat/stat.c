/*
  demostrates retrieving and interpreting file stat information

  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 284
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */
#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>

#include "file_perms.h"

static void
display_stat_info(const struct stat *sb)
{
	fprintf(stderr, "file type:\t\t");

	switch (sb->st_mode & S_IFMT) {
	case S_IFREG: fprintf(stderr, "regular file\n"); break;
	case S_IFDIR: fprintf(stderr, "directory\n"); break;
	case S_IFCHR: fprintf(stderr, "char device\n"); break;
	case S_IFBLK: fprintf(stderr, "block device\n"); break;
	case S_IFLNK: fprintf(stderr, "symlink \n"); break;
	case S_IFIFO: fprintf(stderr, "FIFO or pipe\n"); break;
	case S_IFSOCK: fprintf(stderr, "socket\n"); break;
	default: fprintf(stderr, "unknown\n"); break;
	}

	fprintf(stderr, "device containing i-node: major=%d minor=%d\n",
		major(sb->st_dev), minor(sb->st_dev));

	fprintf(stderr, "i-node number:\t\t%ld\n",
		(long) sb->st_ino);

	fprintf(stderr, "mode:\t\t%lo (%s)\n",
		(unsigned long) sb->st_mode, file_perm_str(sb->st_mode, 0));

	if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX)) {
		fprintf(stderr, "\tspecial bits set:\t%s%s%s\n",
			(sb->st_mode & S_ISUID) ? "set-UID " : "",
			(sb->st_mode & S_ISGID) ? "set-GID " : "",
			(sb->st_mode & S_ISVTX) ? "sticky " : "");
	}

	fprintf(stderr, "number of (hard) links:\t%ld\n",
		(long) sb->st_nlink);

	fprintf(stderr, "ownership:\t\tUID=%ld  GID=%ld\n",
		(long) sb->st_uid, (long) sb->st_gid);

	if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
		fprintf(stderr, "device number (st_rdev): major=%ld; minor=%ld\n",
			(long) major(sb->st_rdev), (long) minor(sb->st_rdev));
	}

	fprintf(stderr, "file size:\t\t%lld bytes\n",
		(long long) sb->st_size);
	fprintf(stderr, "optimal I/O block size:\t%ld bytes\n",
		(long) sb->st_blksize);
	fprintf(stderr, "512B blocks allocated:\t%lld\n",
		(long long) sb->st_blocks);

	fprintf(stderr, "last file access:\t%s",
		ctime(&sb->st_atime));
	fprintf(stderr, "last file modification:\t%s",
		ctime(&sb->st_mtime));
	fprintf(stderr, "last status change:\t%s",
		ctime(&sb->st_ctime));
}

int
main(int argc, char *argv[])
{
	struct stat sb;
	bool stat_link; // True if -1 specified, i.e. lstat
	int fname; // location of filename argument in argv[]

	// accept provided argument if valid
	if (argc != 2) {
		fprintf(stderr, "usage:\n");
		fprintf(stderr, "$ %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	stat_link = (argc > 1) && strcmp(argv[1], "-1") == 0;
	fname = stat_link ? 2 : 1;

	if (stat_link) {
		if (-1 == lstat(argv[fname], &sb)) {
			perror("lstat() failed");
			exit(EXIT_FAILURE);
		}
	} else {
		if (-1 == stat(argv[fname], &sb)) {
			perror("stat() failed");
			exit(EXIT_FAILURE);
		}
	}

	display_stat_info(&sb);

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
