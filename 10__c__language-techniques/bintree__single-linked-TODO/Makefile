## Makefile
##
## @author: Lothar Rubusch
## @email: L.Rubusch@gmx.ch
## @license: GPLv3


SOURCE = main.c bintree_item.c
HEADERS = bintree_item.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -rf

.c.o:
	${CC} -c ${CFLAGS} $<

bintree.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: bintree.exe

clean:
	-${RM} bintree.exe *.o core
	@echo "READY."

