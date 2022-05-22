/*
  accounting, pt2/2: the accounting file viewer

  usage:
  $ sudo su
  # touch pacct
  # ./accounting.elf pacct
    process accounting enabled
    READY.

  # exit

  At this point, three processes have already terminated since we
  enabled process accounting. These processes executed the acct_on,
  su, and bash programs. The bash process was started by su to run the
  privileged shell session.

  $ sleep 30 &
    [1] 5301

  $ ulimit -c unlimited       // allow core dumps (shell built-in)
    $ cat                     // start yet another process
  < enter CTRL + '\' >        // send SIGQUIT to kill cat process
    ^\Quit (core dumped)

   $ ll
    total 120
    -rw-r--r-- 1 pi pi   1308 May 22 21:14 accounting.c
    -rw-r--r-- 1 pi pi   3291 May 22 20:57 accounting_viewer.c
    -rw------- 1 pi pi 499712 May 22 21:17 core
    -rw-r--r-- 1 pi pi    655 May 22 21:13 Makefile

  execute further processes

  Finally use the accounting_viewer to display the accounting file
  $ ./accounting_viewer.elf ./pacct
    command  flags  term.   user  start time            CPU   elapsed
                    status                              time  time
    \        -S-- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.05
    �        ----    0x2  (null)   1970-01-01 01:00:00   0.00   56.06
    �        ---- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.08
    `        -S--    0x1  (null)   1970-01-01 01:00:00   0.00   56.01
    �        -S-- 0x30003  (null)   1970-01-01 01:00:00   0.00   55.99
    �$        -S-- 0x40001  (null)   1970-01-01 01:16:40   0.00   55.98
    X        ---- 0x10000  (null)   1970-01-01 01:16:40  10.00   55.43
    �        ---- 0x10000  (null)   1970-01-01 01:16:40  10.00   56.11
            ---- 0x10000  (null)   1970-01-01 01:16:40  10.00   56.12
    �        F---      0  (null)   1970-01-01 01:16:40  10.00   56.10
    �        F---      0  (null)   1970-01-01 01:16:40  10.00   56.13
    �        ---- 0x10001  (null)   1970-01-01 01:00:00   0.00   56.15
    �        ---- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.18
    �        ---- 0x20000  (null)   1970-01-01 01:00:00   0.00   56.20
    X        ----      0  (null)   1970-01-01 01:16:40  10.00   56.16
    �        ---- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.23
    �        --XC 0x10000  (null)   1970-01-01 01:16:40  10.00   56.21
    �        ---- 0x20000  (null)   1970-01-01 01:00:00   0.00   56.25
             ----    0x1  (null)   1970-01-01 01:16:40  10.00   56.26
    �        ---- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.28
    �        ---- 0x10001  (null)   1970-01-01 01:00:00   0.00   56.30
    �&        F--- 0x10000  (null)   1970-01-01 01:01:57   0.30   56.31
    �&        F--- 0x10000  (null)   1970-01-01 01:01:57   0.30   56.33
    �&        F--- 0x10000  (null)   1970-01-01 01:01:57   0.30   56.34
    �&        F---      0  (null)   1970-01-01 01:01:57   0.30   56.35
    �&        F---      0  (null)   1970-01-01 01:01:57   0.30   56.32
    �&        F--- 0x10000  (null)   1970-01-01 01:01:57   0.30   56.36
    �        ---- 0x10000  (null)   1970-01-01 01:00:00   0.00   56.38
    E%        F---      0  (null)   1970-01-01 01:16:40  10.00   56.40
    E%        F--- 0x10000  (null)   1970-01-01 01:16:40  10.00   56.41
    READY.



  Normally, process accounting is enabled at each system restart by
  placing appropriate commands in the system boot scripts.

  To enable process accounting, we supply the pathname of an existing
  regular file in "acctfile". A typical pathname for the accounting
  file is...
    /var/log/pacct
  or
    /usr/account/pacct

  To disable process accounting, we specify acctfile as NULL.

  Once process accounting is enabled, an acct record "struct acct" is
  written to the accounting file as each process terminates. The acct
  structure is defined in <sys/acct.h>


  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 592
*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pwd.h>

#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/acct.h>
#include <limits.h>

#define TIME_BUF_SIZE 100

char*
user_name_from_id(uid_t uid)
{
	struct passwd *pwd;

	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

static long long /* convert comp_t value into long long */
compt2ll(comp_t ct)
{
	const int EXP_SIZE = 3;  // 3 bit, base-8 exponent
	const int MANTISSA_SIZE = 13;  // followed by 13-bit mantissa
	const int MANTISSA_MASK = (1 << MANTISSA_SIZE) - 1;
	long long mantissa, exp;

	mantissa = ct & MANTISSA_MASK;
	exp = (ct >> MANTISSA_SIZE) & ((1 << EXP_SIZE) - 1);
	return mantissa << (exp * 3);  // power of 8 == left shift 3 bits
}

int
main(int argc, char *argv[])
{
	int acct_file;
	struct acct ac;
	ssize_t nread;
	char *ret;
	char time_buf[TIME_BUF_SIZE];
	struct tm *loc;
	time_t ti;

	if (2 != argc) {
		fprintf(stderr, "usage:\n$ %s file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	acct_file = open(argv[1], O_RDONLY);
	if (0 > acct_file) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "command  flags  term.   user   start time            CPU   elapsed\n");
	fprintf(stderr, "                status                               time  time\n");

	while (0 < (nread = read(acct_file, &ac, sizeof(ac)))) {
		if (nread != sizeof(ac)) {
			fprintf(stderr, "partial read");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%-8.8s  ", ac.ac_comm);
		fprintf(stderr, "%c", (ac.ac_flag & AFORK) ? 'F' : '-');
		fprintf(stderr, "%c", (ac.ac_flag & ASU)   ? 'S' : '-');
		fprintf(stderr, "%c", (ac.ac_flag & AXSIG) ? 'X' : '-');
		fprintf(stderr, "%c", (ac.ac_flag & ACORE) ? 'C' : '-');

#ifdef __linux__
		fprintf(stderr, " %#6lx  ", (unsigned long) ac.ac_exitcode);
#else
		fprintf(stderr, " %#6lx  ", (unsigned long) ac.ac_stat);
#endif

		ret = user_name_from_id(ac.ac_uid);
		fprintf(stderr, "%-8.8s ", (ret = NULL) ? "???" : ret);

		ti = ac.ac_btime;
		loc = localtime(&ti);
		if (NULL == loc) {
			fprintf(stderr, "???Unknown time???   ");
		} else {
			strftime(time_buf, TIME_BUF_SIZE, "%Y-%m-%d %T ", loc);
			fprintf(stderr, "%s ", time_buf);
		}

		fprintf(stderr, "%5.2f %7.2f ", (double) (compt2ll(ac.ac_utime) + compt2ll(ac.ac_stime)) / sysconf(_SC_CLK_TCK),
			(double) compt2ll(ac.ac_etime) / sysconf(_SC_CLK_TCK));
		fprintf(stderr, "\n");
	}

	if (-1 == nread) {
		fprintf(stderr, "read() failed\n");
		exit(EXIT_FAILURE);
	}

	printf("READY.\n");
	exit(EXIT_SUCCESS);
}
