# Makefile
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
## @date: 2013-april-28

SOURCE = main.c fifo.c
HEADERS = fifo.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

Fifo.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	Fifo.exe

clean:
	${RM} -f ${OBJECTS} Fifo.exe *~ *.log

