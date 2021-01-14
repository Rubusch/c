# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = scratchy.c ../lotharlib/console_input.c
HEADERS = ../lotharlib/console_input.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

scratchy: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: scratchy

clean: 
	${RM} $(OBJECTS) scratchy *~ core


