/*
  round trip time (rtt) measuring

  include rtt1
*/
// TODO rework            



// constants

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

int  rtt_d_flag = 0;  /* debug flag; can be set by caller */



// forwards

void rtt_debug(struct rtt_info *);
void rtt_init(struct rtt_info *);
void rtt_newpack(struct rtt_info *);
int rtt_start(struct rtt_info *);
void rtt_stop(struct rtt_info *, uint32_t);
int rtt_timeout(struct rtt_info *);
uint32_t rtt_ts(struct rtt_info *);



// implementation

/*
  calculate the RTO value based on current estimators:
  smoothed RTT plus four times the deviation
*/
#define RTT_RTOCALC(ptr) ((ptr)->rtt_srtt + (4.0 * (ptr)->rtt_rttvar))

static float rtt_minmax(float rto)
{
	if (rto < RTT_RXTMIN) {
		rto = RTT_RXTMIN;
	} else if (rto > RTT_RXTMAX) {
		rto = RTT_RXTMAX;
	}

	return rto;
}


void rtt_init(struct rtt_info *ptr)
{
	struct timeval tv;

	lothars__gettimeofday(&tv, NULL);
	ptr->rtt_base = tv.tv_sec;  // number of sec since 1/1/1970 at start

	ptr->rtt_rtt    = 0;
	ptr->rtt_srtt   = 0;
	ptr->rtt_rttvar = 0.75;
	ptr->rtt_rto = rtt_minmax(RTT_RTOCALC(ptr));
	// first RTO at (srtt + (4 * rttvar)) = 3 seconds
}


/*
  return the current timestamp

  our timestamps are 32-bit integers that count milliseconds since
  rtt_init() was called.
*/
uint32_t rtt_ts(struct rtt_info *ptr)
{
	uint32_t  ts;
	struct timeval tv;

	lothars__gettimeofday(&tv, NULL);
	ts = ((tv.tv_sec - ptr->rtt_base) * 1000) + (tv.tv_usec / 1000);
	return ts;
}


void rtt_newpack(struct rtt_info *ptr)
{
	ptr->rtt_nrexmt = 0;
}


/*
  return value can be used as: alarm(rtt_start(&foo))
*/
int rtt_start(struct rtt_info *ptr)
{
	return ((int) (ptr->rtt_rto + 0.5));  // round float to int
}



/*
  a response was received

  stop the timer and update the appropriate values in the structure
  based on this packet's RTT.  We calculate the RTT, then update the
  estimators of the RTT and its mean deviation; this function should
  be called right after turning off the timer with alarm(0), or right
  after a timeout occurs
*/
void rtt_stop(struct rtt_info *ptr, uint32_t ms)
{
	double  delta;

	ptr->rtt_rtt = ms / 1000.0;  // measured RTT in seconds

	/*
	  update our estimators of RTT and mean deviation of RTT
	  see Jacobson's SIGCOMM '88 paper, Appendix A, for the details
	  we use floating point here for simplicity
	*/

	delta = ptr->rtt_rtt - ptr->rtt_srtt;
	ptr->rtt_srtt += delta / 8;  // g = 1/8

	if (0.0 > delta) {
		delta = -delta;    // |delta|
	}

	ptr->rtt_rttvar += (delta - ptr->rtt_rttvar) / 4; // h = 1/4

	ptr->rtt_rto = rtt_minmax(RTT_RTOCALC(ptr));
}


/*
  a timeout has occurred

  return -1 if it's time to give up, else return 0
*/
int rtt_timeout(struct rtt_info *ptr)
{
	ptr->rtt_rto *= 2;  // next RTO

	if (RTT_MAXNREXMT < ++ptr->rtt_nrexmt) {
		return -1;   // time to give up for this packet
	}
	return 0;
}


/*
  print debugging information on stderr, if the "rtt_d_flag" is nonzero
*/
void rtt_debug(struct rtt_info *ptr)
{
	if (0 == rtt_d_flag) {
		return;
	}

	fprintf(stderr
		, "rtt = %.3f, srtt = %.3f, rttvar = %.3f, rto = %.3f\n"
		, ptr->rtt_rtt
		, ptr->rtt_srtt
		, ptr->rtt_rttvar
		, ptr->rtt_rto);
	fflush(stderr);
}
