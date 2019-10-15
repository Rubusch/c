// dynamic.c
/*
  demonstrates a dynamic lib

  compile files to be placed into lib
  > gcc -Wall -fPIC -c test1.c test2.c
  
  "lib" has to be pre-fix!!!
  > gcc -shared -Wl,-soname,libdynamic.so.1 -o libdynamic.so.1.0 *.o

  as root:
  > mv libdynamic.so.1.0 /opt/lib
  > ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so
  > ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so.1

  in case set LD_LIBRARY_PATH
  > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH

  here, supposed "libdynamic" under LD_LIBRARY_PATH
  > gcc -Wall -L/opt/lib dynamic.c -ldynamic -o dynamic.exe
  [  > gcc -Wall -I/path/to/include/files -L/path/to/libXXX program.c
-ldynamiclib -o program.exe   ]
//*/

#include <stdio.h>
#include <stdlib.h>

void test1(int *);
void test2(int *);

int main()
{
  int val = 7;
  printf("val (before call)\t = %d\n", val);
  test1(&val);
  printf("val (after call)\t = %d\n", val);

  exit(EXIT_SUCCESS);
}
