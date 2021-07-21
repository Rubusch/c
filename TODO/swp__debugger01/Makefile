# Makefile
# Lothar Rubusch (2013)
#
# use -O, -O1, -O2 for optimization


SOURCE = debugger.c rabbit.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
#CFLAGS = -g -Wall -std=c99 -pedantic # doesn't work with sigaction
CFLAGS = -g -Wall
## gcov support
# FIXME fails for some reason?!                            
#CFLAGS = -g -fprofile-arcs -ftest-coverage
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<


all: debugger.exe rabbit.exe

debugger.exe: debugger.o
	${CC} -o $@ debugger.o -lm

rabbit.exe: rabbit.o
	${CC} -o $@ rabbit.o -lm


clean:
	${RM} $(OBJECTS) $(DEBUGOBJECTS) *~ core

mrproper: clean
	${RM} debugger.exe rabbit.exe

.PHONY: all clean uninstall
