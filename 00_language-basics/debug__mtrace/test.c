/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 * @date: 2013-april-28

  debugging malloc allocations

  ressource: http://en.wikipedia.org/wiki/Mtrace

  1. compile with
  $ gcc -g ...

  2. set path to log output
  $ export MALLOC_TRACE=./out.log

  3. start program
  $ ./program.exe

  4. then call
  $ mtrace ./program.exe ./out.log

  mtrace will show where memory gets allocated that is NOT freed properly
//*/

#include <stdio.h>
#include <string.h>

#include <mcheck.h> /* mcheck */
#include <stdlib.h> /* mcheck */

int main()
{
	char *ptr = NULL;

	mtrace(); /* mtrace */

	if (NULL == (ptr = malloc(sizeof(*ptr)))) {
		perror("Allocation failed!");
	}

	// free( ptr );
	ptr = NULL;

	muntrace(); /* mtrace */

	puts("READY.");

	return 0;
}
