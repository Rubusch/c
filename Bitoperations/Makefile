## Makefile (simple)
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## use -O, -O1, -O2 for optimization


SOURCE = bitoperations.c lotharlib/console_input.c
HEADERS = lotharlib/console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

bitoperations.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: bitoperations.exe

clean:
	${RM} $(OBJECTS) bitoperations.exe *~ core


