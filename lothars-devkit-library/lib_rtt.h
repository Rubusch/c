#ifndef DEVKIG_LIB_RTT
#define DEVKIG_LIB_RTT


/*
  round trip time (rtt) measuring

  include rtt1

  ---
  References:
  Unix Network Programming, Stevens (1996)
*/
// TODO rework                


/* includes */

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */

#include "lib_unix.h"
#include "lib_error.h"



/* constants */

struct rtt_info {
	float rtt_rtt; /* most recent measured RTT, in seconds */
	float rtt_srtt; /* smoothed RTT estimator, in seconds */
	float rtt_rttvar; /* smoothed mean deviation, in seconds */
	float rtt_rto; /* current RTO to use, in seconds */
	int rtt_nrexmt; /* # times retransmitted: 0, 1, 2, ... */
	uint32_t rtt_base; /* # sec since 1/1/1970 at start */
};

#define RTT_RXTMIN 2 /* min retransmit timeout value, in seconds */
#define RTT_RXTMAX 60 /* max retransmit timeout value, in seconds */
#define RTT_MAXNREXMT 3 /* max # times to retransmit */
//#define DEVKIT_RTT_DEBUG  /* debug flag; can be set by caller */

/*
  calculate the RTO value based on current estimators:
  smoothed RTT plus four times the deviation
*/
#define RTT_RTOCALC(ptr) ((ptr)->rtt_srtt + (4.0 * (ptr)->rtt_rttvar))


/* forwards */

void rtt_debug(struct rtt_info *);
void rtt_init(struct rtt_info *);
void rtt_newpack(struct rtt_info *);
int rtt_start(struct rtt_info *);
void rtt_stop(struct rtt_info *, uint32_t);
int rtt_timeout(struct rtt_info *);
uint32_t rtt_ts(struct rtt_info *);


#endif /* DEVKIG_LIB_RTT */
