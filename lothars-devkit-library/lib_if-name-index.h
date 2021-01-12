#ifndef DEVKIT_LIB_IF_NAME_INDEX
#define DEVKIT_LIB_IF_NAME_INDEX


// includes

//#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h> /* fcntl(), F_GETFL */

#include <net/if.h> /* if_nametoindex() */

#include "lib_error.h"


// forwards

char* lothars__if_indextoname(unsigned int, char*);
unsigned int lothars__if_nametoindex(const char*);
struct if_nameindex* lothars__if_nameindex();


#endif /* DEVKIT_LIB_IF_NAME_INDEX */
