## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
##

SOURCE = xtoi.c
OBJECTS = ${SOURCE:.c =.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -O2 -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -o ${CFLAGS} $<

xtoi: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: xtoi

clean:
	-${RM} xtoi *.o core


