## Makefile (simple)
##
## commands:
##
##  compile files to be placed into lib
##  > gcc -Wall -fPIC -c test1.c test2.c
##  
##  "lib" has to be pre-fix!!!
##  > gcc -shared -Wl,-soname,libdynamic.so.1 -o libdynamic.so.1.0 *.o
##
##  as root:
##  > mv libdynamic.so.1.0 /opt/lib
##  > ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so
##  > ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so.1
##
##  in case set LD_LIBRARY_PATH  
##  > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH
##
##  here, supposed "libdynamic" under LD_LIBRARY_PATH
##  > gcc -Wall -L/opt/lib dynamic.c -ldynamic -o dynamic.exe
##  [  > gcc -Wall -I/path/to/include/files -L/path/to/libXXX program.c -ldynamiclib -o program.exe   ]
##
##
## makefile:
##
## > make
## > su -c "mv libdynamic.so.1.0 /opt/lib/"
## > su -c "ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so.1"
## > su -c "ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so"
## in case
## > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH
##
## build final
## > make dynamic.exe
##

SOURCE = dynamic.c
OBJS = test1.o test2.o
TLIB = libdynamic.so
FILENAME = libdynamic.so.1.0
SONAME = libdynamic.so.1
.PREFIXES = .c .o
CC = gcc
CFLAGS1 = -g -Wall -std=c99 -pedantic -fPIC
CFLAGS2 = -shared -Wl,-soname,$(SONAME)
CFLAGS3 = -Wall
LDPATH = /opt/lib
RM = rm -f


$(TLIB): $(OBJS)
	gcc $(CFLAGS2) -o $(FILENAME) $(OBJS)

$(OBJS):
	$(CC) $(CFLAGS1) -c test1.c test2.c

all: $(OBJS) $(TLIB)

dynamic.exe:
	$(CC) $(CFLAGS3) -L$(LDPATH) $(SOURCE) -ldynamic -o $@


clean: 
	$(RM) $(OBJS) $(TLIB) $(FILENAME) dynamic.exe *~ core


