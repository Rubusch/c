# Makefile
#
# @author: Lothar Rubusch
# @email: L.Rubusch@gmx.ch
# @license: GPLv3

SOURCE = allocation.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

allocation.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: allocation.exe

clean:
	${RM} allocation.exe *~ *.o core


