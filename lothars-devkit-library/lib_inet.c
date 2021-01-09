/*
  inet_ functions

  ---
  Remarks:

  1.) NO WRAPPER FOR inet_ntoa()

  char *inet_ntoa(struct in_addr in);

  The inet_ntoa() function converts the Internet host address in,
  given in network byte order, to a string in IPv4 dotted-decimal
  notation. The string is returned in a statically allocated buffer,
  which subsequent calls will overwrite.

  -> There is no wrapper, usage would be e.g. printf() call or the
     like from a defined in_addr instance. The in_addr instance is
     expected to be in condition, and in a worst case no string is
     going out which either will be noticed soon or is not
     relevant. Thus keep using inet_ntoa() as is!


  2.) AVOID USING inet_addr(), USE inet_aton()

  in_addr_t inet_addr(const char *cp);

  The inet_addr() function converts the Internet host address cp from
  IPv4 numbers-and-dots notation into binary data in network byte
  order. If the input is invalid, INADDR_NONE (usually -1) is
  returned. Use of this function is problematic because -1 is a valid
  address (255.255.255.255). Avoid its use in favor of inet_aton(),
  inet_pton(3), or getaddrinfo(3) which provide a cleaner way to
  indicate error return.


  ---
  References:
  Unix Network Programming, Stevens (1996)
  Developer Manpages
*/



// formwards

const char* lothars__inet_ntop(int, const void*, char*, size_t);
void lothars__inet_pton(int, const char*, void*);
int lothars__inet_aton(const char *, struct in_addr *);



// implementation

/*
  inet_ntop - convert IPv4 and IPv6 addresses from binary to text form

  This function converts the network address structure src in the af
  address family into a character string. The resulting string is
  copied to the buffer pointed to by dst, which must be a non-NULL
  pointer. The caller specifies the number of bytes available in this
  buffer in the argument size.

  #include <arpa/inet.h>

  @family: The address family AF_INET or AF_INET6 (af).
  @addrptr: The source network address structure to convert into
      string (src).
  @strptr: A pointer to the destination string (dst).
  @len: The number of bytes available in this buffer (size).

  Returns NULL in case of error.
*/
const char* lothars__inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char *ptr = NULL;

	if (NULL == strptr) {  // check for old code
		err_quit("NULL 3rd argument to inet_ntop");
	}

	if (NULL == (ptr = inet_ntop(family, addrptr, strptr, len))) {
		err_sys("inet_ntop error");  // sets errno
	}

	return ptr;
}


/*
  inet_pton - convert IPv4 and IPv6 addresses from text to binary form

  This function converts the character string src into a network
  address structure in the af address family, then copies the network
  address structure to dst. The af argument must be either AF_INET or
  AF_INET6.

  #include <arpa/inet.h>

  @family: The address family AF_INET or AF_INET6 (af).
  @strptr: A pointer to a source string to be converted (src).
  @addrptr: The pointer to a destination

  Returns 1 on success, 0 if strptr did not contain a valid internet
  address and -1 if family was not a valid address family.
*/
void lothars__inet_pton(int family, const char *strptr, void *addrptr)
{
	int res;
	if (0 > (res = inet_pton(family, strptr, addrptr))) {
		err_sys("%s() error for %s (check the passed address family?)", __func__, strptr); // errno set
	} else if (0 == res) {
		err_quit("%s() error for %s (check the passed strptr)", __func__, strptr); // errno not set
	}
}


/*
  inet_aton() converts the Internet host address cp from the IPv4
  numbers-and-dots notation into binary form (in network byte order)
  and stores it in the structure that inp points to. inet_aton()
  returns nonzero if the address is valid, zero if not. The address
  supplied in cp can have one of the following forms:

  a.b.c.d, a.b.c, a.b., a

  An example of the use of inet_aton() and inet_ntoa() is shown
  below. Here are some example runs:

    $ ./a.out 226.000.000.037      # Last byte is in octal
    226.0.0.31
    $ ./a.out 0x7f.1               # First byte is in hex
    127.0.0.1

  #ifdef _BSD_SOURCE || _SVID_SOURCE
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  @cp: The Internet host address in numbers-and-dots notation.
  @inp: A pointer to a structure which contains the binary Internet
  address.

  Return is phony for compatibility.
*/
int lothars__inet_aton(const char *cp, struct in_addr *inp)
{
	int res;
	if (0 == (res = inet_aton(cp, inp))) {
		err_sys("%s() invalid address", __func__);
	}
        /*
	  NB: in case of failure there's no close(fd_sock)
	  when the program exits, resources are going to be freed by OS, anyway

	  take care if this might be not the (regular) case e.g. due
	  to some socket options
	*/
	return res;
}


