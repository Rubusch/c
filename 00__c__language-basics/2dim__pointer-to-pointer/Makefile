# Makefile
#
# @author: Lothar Rubusch
# @email: L.Rubusch@gmx.ch
# @license: GPLv3

SOURCE = 2-dim-array.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

2-dim-array.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: 2-dim-array.exe

clean:
	${RM} 2-dim-array.exe *~ *.o core


