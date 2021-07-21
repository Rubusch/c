## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3

SOURCE = endian.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O -g -Wall -std=c99
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

endian.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: endian.exe

clean:
	${RM} endian.exe *~ *.o core


