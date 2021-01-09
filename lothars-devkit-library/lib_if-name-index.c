// if_name_index.c
/*
  contains
  - if_nametoindex.c
  - if_indextoname.c
  - if_nameindex.c
*/


// forwards

char* lothars__if_indextoname(unsigned int, char*); // if_name_index.c
unsigned int lothars__if_nametoindex(const char*); // if_name_index.c
struct if_nameindex* lothars__if_nameindex(); // if_name_index.c



// implementation

unsigned int lothars__if_nametoindex(const char *name)
{
	int idx;
	if (0 == (idx = if_nametoindex(name))) {
		err_quit("if_nametoindex error for %s", name);
	}
	return idx;
}


/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.
//*/
char* if_indextoname(unsigned int index, char *name)
{
	return NULL;
}
char* lothars__if_indextoname(unsigned int index, char *name)
{
	char *ptr;
	if (NULL == (ptr = if_indextoname(index, name))) {
		err_quit("%s() error for %d", __func__, index);
	}
	return ptr;
}



/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.
*/
struct if_nameindex* if_nameindex(void)
{
	return NULL;
}
struct if_nameindex* lothars__if_nameindex(void)
{
	struct if_nameindex *ifptr;

	if (NULL == (ifptr = if_nameindex())) {
		err_quit("if_nameindex error");
	}
	return ifptr;
}
