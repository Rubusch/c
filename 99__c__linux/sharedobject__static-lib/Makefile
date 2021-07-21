## Makefile (shared object)
##
## commands:
##
## compile objects
##   cc -Wall -c test1.c test2.c
##
## bind to library
##   ar -cvq libstatic.a test1.o test2.o
##
## list files
##   ar -t libstatic.a
##
## link against library
##   cc -o static.exe static.c libstatic.a
##
## bind statically
##   cc -o static.exe static.c -L/path/to/library/ -ltest
##

SOURCE = static.c
TARGET = $(SOURCE:.c=.exe)
TLIB = staticlib.a
OBJS = test1.o test2.o
.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -Werror -pedantic
RM = rm -f

$(TARGET): $(OBJS) $(TLIB)
	$(CC) -o $@ $(SOURCE) $(TLIB)

$(OBJS):
	$(CC) $(CFLAGS) -c test1.c test2.c

$(TLIB): $(OBJs)
	ar -cvq $(TLIB) $(OBJS)

all: $(TARGET)

clean:
	$(RM) $(OBJS) $(TLIB) $(TARGET) *~ core
