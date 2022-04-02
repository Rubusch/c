/*
  file.c
  	
  based on Using I/O system calls, The Linux Programming Interface, Michael Kerrisk, 2010, p. 71
  @author: Michael Kerrisk
  
  @author: Lothar Rubusch
  @email: L.Rubusch@gmx.ch
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

void usage(const char *nam)
{
	fprintf(stderr, "copy files:\n");
	fprintf(stderr, "$ ./%s <src> <dst>\n", nam);
	fprintf(stderr, "\n");
	fprintf(stderr, "$ echo \"Rasmus Klump\" > orig.txt\n");
	fprintf(stderr, "$ ./%s orig.txt petzi.txt\n", nam);
	fprintf(stderr, "$ cat ./petzi.txt\n");
	fprintf(stderr, "	Rasmus Klump\n");

}

int main(int argc, char *argv[])
{
	int fd_in, fd_out, flags;
	mode_t perms;
	ssize_t num;
	char buf[BUFSIZ];

	if (argc <= 1) {
		usage(argv[0]);
		goto err;
	}

	// read
	fd_in = open(argv[1], O_RDONLY);
	if (fd_in == -1) {
		perror("could not open file to read");
		exit(EXIT_FAILURE);
	}

	// write
	flags = O_CREAT | O_WRONLY | O_TRUNC;
	perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw
	fd_out = open(argv[2], flags, perms);
	if (-1 == fd_out) {
		perror("could not open file to write");
		goto err;
	}

	// transfer
	while (0 < (num = read(fd_in, buf, BUFSIZ))) {
		if (num != write(fd_out, buf, num)) {
			perror("write() error after partial write");
			goto err;
		}
	}

	if (-1 == num) {
		perror("read() error");
		goto err;
	}

	if (-1 == close(fd_in)) {
		perror("close(fd_in) error");
		goto err;
	}

	if (-1 == close(fd_out)) {
		perror("close(fd_out) failed");
		goto err;
	}

	exit(EXIT_SUCCESS);

err:
	exit(EXIT_FAILURE);
}
