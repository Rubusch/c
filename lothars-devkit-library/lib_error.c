/*
  error

  Error handling using via errno, can be a extended to use syslog or
  any other logger.
*/

#include "lib_error.h"

/*
   nonfatal error related to system call Print message and return
*/
void err_ret(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
}



/*
   fatal error related to system call Print message and terminate
*/
void err_sys(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/*
   fatal error related to system call Print message, dump core, and terminate
*/
void err_dump(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	abort();  // dump core and terminate
	exit(EXIT_FAILURE);  // shouldn't get here, error code 1
}


/*
   nonfatal error unrelated to system call Print message and return
*/
void err_msg(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
}


/*
   fatal error unrelated to system call Print message and terminate
*/
void err_quit(const char *fmt, ...)
{
	va_list  ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/*
   print message and return to caller Caller specifies "errnoflag"

   #include <stdarg.h>

   @errnoflag: The flag for the errno number.
   @fmt: The format.
   @ap: The argument pointer for further arguments in va_list.
*/
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int errno_save, n_len;
	char buf[MAXLINE + 1];

	errno_save = errno; // value caller might want printed

	vsnprintf(buf, MAXLINE, fmt, ap); // safe
//	vsprintf(buf, fmt, ap); // alternatively, if no vsnprintf() available, not safe

	n_len = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n_len, MAXLINE - n_len, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");
	fflush(stdout);  // in case stdout and stderr are the same
	fputs(buf, stderr);
	fflush(stderr);
}
