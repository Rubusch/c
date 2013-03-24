# Makefile
#
#

SOURCE = main.c bintree_item.c
HEADERS = bintree_item.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -drf

.c.o:
	${CC} -c ${CFLAGS} $<

bintree: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: bintree

clean:
	-${RM} bintree.exe *.o core
	@echo "READY."

