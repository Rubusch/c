#ifndef _COMMONS_H_
#define _COMMONS_H_


#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>


/*
  "service" - service describes the port number here, client and
  server may use tcp, but need to share a port number
 */
#define SERVICE "27976"
#define BUF_SIZE 500
#define IS_ADDR_STR_LEN 4096

/*
  become_daemon()

  bitmask values for 'flags' argument of become_daemon()
 */

#define BD_NO_CHDIR            01  /* don't chdir("/") */
#define BD_NO_CLOSE_FILES      02  /* don't close all open files */
#define BD_NO_REOPEN_STD_FDS   04  /* don't reopen stdin, stdout, and
				    * stderr to /dev/null */
#define BD_NO_UMASK0          010  /* don't do a umask(0) */
#define BD_MAX_CLOSE         8192  /* maximum file descriptors to
				    * close if sysconf(_SC_OPEN_MAX)
				    * is indeterminate */


#endif /* _COMMONS_H_ */
