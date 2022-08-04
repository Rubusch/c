/*
  header file for fifo_seqnum_server and fifo_seqnum_client

  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 911-915
*/

#ifndef FIFO_SEQNUM_H
#define FIFO_SEQNUM_H


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* sellknown name for server's fifo */
#define SERVER_FIFO "/tmp/seqnum_sv"

/* template for building client FIFO name */
#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"

/*
  space required for client FIFO pathname
  (+20 as a generous allowance for the PID)
 */
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request {     /* request (client -> server) */
	pid_t pid;   /* PID of client */
	int seqlen;  /* length of desired sequence */
};

struct response {    /* response (server -> client) */
	int seqnum;  /* start of sequence */
};


#endif /* FIFO_SEQNUM_H */
