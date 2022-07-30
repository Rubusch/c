#ifndef BECOME_DAEMON_H
#define BECOME_DAEMON_H


/* bitmask values for 'flags' argument of become_daemon() */

#define BD_NO_CHDIR            01  /* don't chdir("/") */
#define BD_NO_CLOSE_FILES      02  /* don't close all open files */
#define BD_NO_REOPEN_STD_FDS   04  /* don't reopen stdin, stdout, and
				    * stderr to /dev/null */
#define BD_NO_UMASK0          010  /* don't do a umask(0) */
#define BD_MAX_CLOSE         8192  /* maximum file descriptors to
				    * close if sysconf(_SC_OPEN_MAX)
				    * is indeterminate */

static const char *LOG_FILE = "/tmp/ds.log";

int become_daemon(int flags);


#endif
