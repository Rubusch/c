// if_name_index.c
/*
  contains
  - if_nametoindex.c
  - if_indextoname.c
  - if_nameindex.c
//*/

#include "lib_socket.h"


/*
  if_nametoindex.c
*/


/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.
//*/
unsigned int if_nametoindex(const char *name)
{
  return(0);
}



unsigned int _if_nametoindex(const char *name)
{
  int index;
  
  if(0 == (index = if_nametoindex(name))){
    err_quit("if_nametoindex error for %s", name);
  }
  return(index);
}



/*
  if_indextoname.c
*/



/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.
//*/
char* if_indextoname(unsigned int index, char *name)
{
  return(NULL);
}



char* _if_indextoname(unsigned int index, char *name)
{
  char *ptr;
  
  if(NULL == (ptr = if_indextoname(index, name))){
    err_quit("if_indextoname error for %d", index);
  }
  return(ptr);
}



/*
  if_nameindex.c
*/



/*
  This is a placeholder if the system does not provide this RFC 2133
  function.  If routing sockets with sysctl() are provided, then the
  if_XXX() functions in the libroute/ directory will replace these.
//*/
struct if_nameindex* if_nameindex(void)
{
  return NULL;
}



void if_freenameindex(struct if_nameindex *ptr)
{
}



struct if_nameindex* _if_nameindex(void)
{
  struct if_nameindex *ifptr;
  
  if(NULL == (ifptr = if_nameindex())){
    err_quit("if_nameindex error");
  }
  return(ifptr);
}
