/*
  routing

  routing socket support (still some if_nametoindex / if_indextoname
  missing, supported by lib_socket)
//*/
// TODO rework                                             



// forwards

// function prototypes
void  get_rtaddrs(int, struct sockaddr *, struct sockaddr **);
const char* sock_masktop(struct sockaddr *, socklen_t);

// wrapper functions
char* lothars__net_rt_iflist(int, int, size_t *);
char* lothars__net_rt_dump(int, int, size_t *);

#define _sock_masktop(a,b) sock_masktop((a), (b))



// implementation

/*
  Round up 'a' to next multiple of 'size', which must be a power of 2
*/
#define ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size)-1))) : (a))

/*
  Step to next socket address structure. If sa_len is 0, assume it is
  sizeof(u_long).
*/
#define NEXT_SA(ap) ap = (struct sockaddr *)				\
		((caddr_t) ap + (ap->sa_len ? ROUNDUP(ap->sa_len, sizeof (u_long)) : \
				 sizeof(u_long)))


void get_rtaddrs(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
	int  idx;

	for (idx = 0; idx<RTAX_MAX; ++idx) {
		if (addrs & (1 << idx)) {
			rti_info[idx] = sa;
			NEXT_SA(sa);
		} else {
			rti_info[idx] = NULL;
		}
	}
}


/*
*/
char* net_rt_iflist(int family, int flags, size_t *lenp)
{
	int  mib[6];
	char *buf=NULL;

	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = family;  // only addresses of this family
	mib[4] = NET_RT_IFLIST;
	mib[5] = flags;   // interface index or 0

	if (0 > sysctl(mib, 6, NULL, lenp, NULL, 0)) {
		return NULL;
	}

	if (NULL == (buf = malloc(*lenp))) {
		return NULL;
	}

	if (0 > sysctl(mib, 6, buf, lenp, NULL, 0)) {
		free(buf);
		return NULL;
	}

	return buf;
}
char* lothars__net_rt_iflist(int family, int flags, size_t *lenp)
{
	char *ptr=NULL;

	if (NULL == (ptr = net_rt_iflist(family, flags, lenp))) {
		err_sys("net_rt_iflist error");
	}

	return ptr;
}


/*
*/
char* net_rt_dump(int family, int flags, size_t *lenp)
{
	int  mib[6];
	char *buf=NULL;

	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = family;  // only addresses of this family
	mib[4] = NET_RT_DUMP;
	mib[5] = flags;   // not looked at with NET_RT_DUMP

	if (0 > sysctl(mib, 6, NULL, lenp, NULL, 0)) {
		return NULL;
	}

	if (NULL == (buf = malloc(*lenp))) {
		return NULL;
	}

	if (0 > sysctl(mib, 6, buf, lenp, NULL, 0)) {
		return NULL;
	}

	return buf;
}
char* lothars__net_rt_dump(int family, int flags, size_t *lenp)
{
	char *ptr=NULL;

	if (NULL == (ptr = net_rt_dump(family, flags, lenp))) {
		err_sys("net_rt_dump error");
	}

	return ptr;
}


const char* sock_masktop(struct sockaddr *sa, socklen_t salen)
{
	static char str[INET6_ADDRSTRLEN];
	unsigned char *ptr = &sa->sa_data[2];

	if (sa->sa_len == 0) {
		return "0.0.0.0";

	} else if (sa->sa_len == 5) {
		snprintf(str, sizeof(str), "%d.0.0.0", *ptr);

	} else if (sa->sa_len == 6) {
		snprintf(str, sizeof(str), "%d.%d.0.0", *ptr, *(ptr+1));

	} else if (sa->sa_len == 7) {
		snprintf(str, sizeof(str), "%d.%d.%d.0", *ptr, *(ptr+1), *(ptr+2));

	} else if (sa->sa_len == 8) {
		snprintf(str, sizeof(str), "%d.%d.%d.%d", *ptr, *(ptr+1), *(ptr+2), *(ptr+3));

	} else {
		snprintf(str, sizeof(str), "(unknown mask, len = %d, family = %d)", sa->sa_len, sa->sa_family);
	}

	return str;
}
