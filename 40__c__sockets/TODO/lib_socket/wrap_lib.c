// wrap_lib.c
/*
  Wrapper functions for library functions. Most are included in the source file for the function itself.
*/

#include "lib_socket.h"


const char* _inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
  const char *ptr = NULL;
  
  if(NULL == strptr){  // check for old code
    err_quit("NULL 3rd argument to inet_ntop");
  }

  if(NULL == (ptr = inet_ntop(family, addrptr, strptr, len))){
    err_sys("inet_ntop error");  // sets errno 
  }

  return(ptr);
}



void _inet_pton(int family, const char *strptr, void *addrptr)
{
  int res;
  
  if(0 > (res = inet_pton(family, strptr, addrptr))){
    err_sys("inet_pton error for %s", strptr); // errno set
  }else if(0 == res){
    err_quit("inet_pton error for %s", strptr); // errno not set 
  }
  // nothing to return 
}
