/*
  usage:
  -> first build a dynamic library with a symbol: hello()
  $ cd ../sharedobject__dynamic-shared
  $ make clean && make
  $ cd -

  $ ./search-symbol.elf /usr/src/github__c/70_system/sharedobject__dynamic-shared/libplugin.so.2.0 hello
    try calling the address returned by dlsym(), note will SEGVFAULT if the function takes arguments...
    Hello
    READY.


  Demonstrates the use of the dlopen API. This program takes two
  commandline arguments: the name of a shared library to load and the
  name of a function to execute within that library.


  Based on The Linux Programming Interface, Michael Kerrisk, 2010,
  p. 865
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dlfcn.h>


int
main(int argc, char *argv[])
{
	void *lib_handle;  // handle for shared library
	void (*func)(void);  // pointer to function with no arguments
	const char *err;

	if (3 != argc) {
		fprintf(stderr, "usage:\n"
			"$ %s <libpath> <funcname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*
	  load and "open" the shared library and get a handle for
	  later use
	*/

	lib_handle = dlopen(argv[1], RTLD_LAZY);
	if (NULL == lib_handle) {
		perror("dlopen()");
		exit(EXIT_FAILURE);
	}

	/*
	  [notes from LPI, p. 860ff]

	  RTLD_LAZY: Undefined function symbols in the library should
	      be resolved only as the code is executed.

	  RTLD_NOW: All undefined symbols in the library should be
	      immediatly resolved before dlopen() completes,
	      regardless of whether they will ever be required.

	  RTLD_GLOBAL: Symbols in this library and its dependency tree
  	      are made available for resolving references in other
  	      libraries leaded by this process and also for lookups
  	      via dlsym()

	  RTLD_LOCAL: This is the converse of RTLD_GLOBAL and the
	      default if neither constant is specified - symbols in
	      this library and its dependency tree are not available
	      to resolv references in subsequently loaded libraries

	  RTLD_NOLOAD: Use this flag to check if a particular library
	      is currently loaded as part of the process's address
	      space.

	  RTLD_DEEPBIND: Search for definitions in the library before
	      searching for definitions in libraries tha thave already
	      been loaded.

	  RTLD_DEFAULT: Search for symbol starting with the main
	      program, and then proceeding in order through the list
	      of all shared libraries loaded, in cluding those
	      libraries dynamically loaded by dlopen().

	  RTLD_NEXT: Search for symbol in shared libraries loaded
	      after the one invoking dlsym(). This is useful when
	      creating a wrapper function with the same name as a
	      function defined elsewhere.

	*/

	/*
	  search library for symbol named in argv[2]
	*/
	(void) dlerror();   // clear dlerror()
	*(void**) (&func) = dlsym(lib_handle, argv[2]);
	err = dlerror();
	if (NULL != err) {
		fprintf(stderr, "dlsym: %s\n", err);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "try calling the address returned by dlsym(), note will SEGVFAULT if the function takes arguments...\n");
	(*func)();

	dlclose(lib_handle);  // close the library

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
