## Makefile (simple)
##
## commands:
##  compile files, e.g. test1.c
##  > gcc -Wall -fPIC -c test1.c
##  
##  "lib" has to be pre-fix! Build library file
##  > gcc -shared -Wl,-soname,libplugin.so.1 -o libplugin.so.1.0 *.o
##
##  move and link as root
##  > mv libplugin.so.1.0 /opt/lib
##  > ln -sf /opt/lib/libplugin.so.1.0 /opt/lib/libplugin.so
##  > ln -sf /opt/lib/libplugin.so.1.0 /opt/lib/libplugin.so.1
##
##  in case set LD_LIBRARY_PATH  
##  > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH
##
##
##
##  now compile main program
##  > gcc -rdynamic -o plugin.exe pluginbase.c -ldl  
##
##
## makefile:
##
## > make test1
## or
## > make test2
##
## > su -c "mv libdynamic.so.1.0 /opt/lib/"
## > su -c "ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so.1"
## > su -c "ln -sf /opt/lib/libdynamic.so.1.0 /opt/lib/libdynamic.so"
##
## in case
## > export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH
##
## build final
## > make plugin.exe
##

OBJ1 = test1.o 
TLIB1 = libplugin.so
FILENAME1 = libplugin.so.1.0
SONAME1 = libplugin.so.1
CFLAGS1A = -g -Wall -std=c99 -pedantic -fPIC
CFLAGS1B = -shared -Wl,-soname,$(SONAME1)

OBJ2 = test2.o
TLIB2 = libplugin.so
FILENAME2 = libplugin.so.2.0
SONAME2 = libplugin.so.2
CFLAGS2A = -g -Wall -std=c99 -pedantic -fPIC
CFLAGS2B = -shared -Wl,-soname,$(SONAME2)

SOURCE = pluginbase.c
.PREFIXES = .c .o
CC = gcc





LDPATH = /opt/lib
RM = rm -f


#$(TLIB1): $(OBJ1)
#	gcc $(CFLAGS1B) -o $(FILENAME1) $(OBJ1)
#
#$(OBJ1):
#	$(CC) $(CFLAGS1A) -c test1.c
#
#libplugin.so.1.0: 
#	$(OBJ1) $(TLIB1)



$(TLIB2): $(OBJ2)
	gcc $(CFLAGS2B) -o $(FILENAME2) $(OBJ2)

$(OBJ2):
	$(CC) $(CFLAGS2A) -c test2.c

libplugin.so.2.0: 
	$(OBJ2) $(TLIB2)


plugin.exe:
	$(CC) -Wall -rdynamic -o $@ $(SOURCE) -ldl


clean: 
	$(RM) $(OBJ1) $(OBJ2) $(TLIB1) $(TLIB2) $(FILENAME1) $(FILENAME1) plugin.exe *~ core
