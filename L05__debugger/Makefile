# Makefile
# Lothar Rubusch (2013)
#
# use -O, -O1, -O2 for optimization


SOURCE = debugger.c rabbit.c
OBJECTS = ${SOURCE:.c=.o}

#DEBUGSOURCE = rabbit.c
#DEBUGOBJECTS = ${DEBUGSOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

debugger.exe: debugger.o
	${CC} -o $@ debugger.o -lm

rabbit.exe: rabbit.o
	${CC} -o $@ rabbit.o -lm

all: debugger.exe rabbit.exe


clean:
	${RM} $(OBJECTS) $(DEBUGOBJECTS) *~ core

uninstall: clean
	${RM} debugger.exe rabbit.exe

.PHONY: all clean uninstall