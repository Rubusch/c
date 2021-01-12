/*
 interface name index
*/

#include "lib_if-name-index.h"


/*
  The if_nametoindex() function returns the index of the network
  interface corresponding to the name ifname.

  #include <net/if.h>

  @name: The interface name (ifname).

  Returns the index to the provided interface name, on error 0 is
  returned.
*/
unsigned int lothars__if_nametoindex(const char *name)
{
	int idx = 0;
	if (0 == (idx = if_nametoindex(name))) {
		err_quit("%s() error for %s", __func__, name);
	}
	return idx;
}


/*
  The if_indextoname() function returns the name of the network
  interface corresponding to the interface index ifindex. The name is
  placed in the buffer pointed to by ifname. The buffer must allow for
  the storage of at least IF_NAMESIZE bytes.

  #include <net/if.h>

  @index: The index of the interface.
  @name: The name to the provided interface is put in here.

  Returns the name of the interface also as return field, or NULL in
  case of error (the wrapper will abort the program).
*/
char* lothars__if_indextoname(unsigned int index, char name[IF_NAMESIZE])
{
	char *ptr = NULL;
	if (NULL == (ptr = if_indextoname(index, name))) {
		err_quit("%s() error for %d", __func__, index);
	}
	return ptr;
}



/*
  The if_nameindex() function returns an array of if_nameindex
  structures, each containing information about one of the network
  interfaces on the local system. The if_nameindex structure contains
  at least the following entries:

    unsigned int if_index; // Index of interface (1, 2, ...)
    char        *if_name;  // Null-terminated name ("eth0", etc.)

  The if_index field contains the interface index. The ifa_name field
  points to the null-terminated interface name. The end of the array
  is indicated by entry with if_index set to zero and ifa_name set to
  NULL.

  The data structure returned by if_nameindex() is dynamically
  allocated and should be freed using if_freenameindex() when no
  longer needed.

  Call if_freenameindex(struct if_nameindex *ptr); directly to free
  the struct if_nameindex* again!

  #include <net/if.h>

  Returns a pointer to the array, or NULL in case of an error.
*/
struct if_nameindex* lothars__if_nameindex(void)
{
	struct if_nameindex *ifptr = NULL;

	if (NULL == (ifptr = if_nameindex())) {
		err_quit("%s() error", __func__);
	}
	return ifptr;
}
