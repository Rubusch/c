# Makefile (simple)
#
# @author: Lothar Rubusch
# @email: L.Rubusch@gmx.ch
# @license: GPLv3
# @2013-May-01
#
# use -O, -O1, -O2 for optimization


SOURCE = input_string.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

input.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: input.exe

clean:
	${RM} $(OBJECTS) input.exe *~ core


