// pluginbase.c
/*
  demonstrates a dynamic lib, plugin type with dynamic load / unload 

  compile files, e.g. test1.c
  > gcc -Wall -fPIC -c test1.c
  
  "lib" has to be pre-fix! Build library file
  > gcc -shared -Wl,-soname,libplugin.so.1 -o libplugin.so.1.0 *.o

  move and link as root
  > mv libplugin.so.1.0 /opt/lib
  > ln -sf /opt/lib/libplugin.so.1.0 /opt/lib/libplugin.so
  > ln -sf /opt/lib/libplugin.so.1.0 /opt/lib/libplugin.so.1

  in case set LD_LIBRARY_PATH  
  > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH



  now compile main program
  > gcc -rdynamic -o plugin.exe pluginbase.c -ldl  


  
  experiment: replace the libplugin.so.1.0 placed under /opt/lib, as 
  compiled test1.c by a compiled test2.c
  > gcc -Wall -fPIC -c test2.c
  > gcc -shared -Wl,-soname,libplugin.so.1 -o libplugin.so.1.0 *.o

  as root
  > mv libplugin.so.1.0 /opt/lib

  run plugin.exe
  ./plugin.exe
//*/

#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>

#include "test.h"


/*
  main
//*/
int main()
{
  void *lib_handle;
  double (*func)(int*);

  int val=0;
  char *error;

  if(!(lib_handle = dlopen("/opt/lib/libplugin.so", RTLD_LAZY))){
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }
  
  func = dlsym(lib_handle, "test");

  if(NULL != (error = dlerror())){
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  (*func)(&val);

  printf("value = %d\n", val);
  
  dlclose(lib_handle);

  exit(EXIT_SUCCESS);
}

