# Makefile
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3
## @date: 2013-april-28

SOURCE = main.c dequeue_item.c
HEADERS = dequeu_item.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm

.c.o:
	${CC} -c ${CFLAGS} $<

Dequeue: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all:	Dequeue

clean:
	${RM} ${OBJECTS} *~

uninstall: 
	${RM} Dequeue.exe
	clean