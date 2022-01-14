/*
 */

#include "b-tree.h"

// some additional headers needed for matrix_create()
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


/* utils */

void btree_debug(const char* format, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
#endif
}

void btree_failure(const char* format, ...)
{
	perror("failed!");
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}


// b-tree
/*
  B-TREE-SEARCH(x, k)

  
*/
// TODO

/*
  B-TREE-CREATE(T)

  
*/
// TODO

/*
  B-TREE-SPLIT-CHILD(x, i)

  
*/
// TODO

/*
  B-TREE-INSERT(T, k)

  
*/
// TODO

/*
  B-TREE-INSERT-NONFULL(x, k)

  
*/
// TODO
