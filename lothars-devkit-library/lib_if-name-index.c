/*
 interface name index
*/

#include "lib_if-name-index.h"


/*
  TODO

  @name: 
*/
unsigned int lothars__if_nametoindex(const char *name)
{
	int idx = NULL;
	if (0 == (idx = if_nametoindex(name))) {
		err_quit("%s() error for %s", __func__, name);
	}
	return idx;
}


/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.

  @index: 
  @name: 

  Returns NULL (currently, TODO)
*/
char* if_indextoname(unsigned int index, char *name)
{
	return NULL;
}
char* lothars__if_indextoname(unsigned int index, char *name)
{
	char *ptr = NULL;
	if (NULL == (ptr = if_indextoname(index, name))) {
		err_quit("%s() error for %d", __func__, index);
	}
	return ptr;
}



/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.

  Returns NULL (currently, TODO)
*/
struct if_nameindex* if_nameindex(void)
{
	return NULL;
}
struct if_nameindex* lothars__if_nameindex(void)
{
	struct if_nameindex *ifptr = NULL;

	if (NULL == (ifptr = if_nameindex())) {
		err_quit("%s() error", __func__);
	}
	return ifptr;
}
