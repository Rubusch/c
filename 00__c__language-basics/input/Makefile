## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##
## .h and .c files

SOURCE = main.c console_input.c
HEADERS = console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=gnu99
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

tester: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	tester

clean:
	${RM} ${OBJECTS} tester *~


