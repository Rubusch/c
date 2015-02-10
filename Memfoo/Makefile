# Makefile
#
#

SOURCE = Memfoo.c
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

Memfoo: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: Memfoo

clean: 
	${RM} Memfoo.exe *~ core


