# Makefile
# Lothar Rubusch (2013)
#
# use -O, -O1, -O2 for optimization


SOURCE = debugger.c
OBJECTS = ${SOURCE:.c=.o}

DEBUGSOURCE = rabbit.c
DEBUGOBJECTS = ${DEBUGSOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

debugger.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

rabbit.exe: ${DEBUGOBJECTS}
	${CC} -o $@ ${DEBUGOBJECTS} -lm

all: debugger.exe rabbit.exe

clean: 
	${RM} $(OBJECTS) $(DEBUGOBJECTS) *~ core

uninstall:
	${RM} debugger.exe rabbit.exe

