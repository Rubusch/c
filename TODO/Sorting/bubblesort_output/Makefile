# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c bubblesort.c
HEADERS = main.h bubblesort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

bubblesort: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: bubblesort

.PHONY: clean

clean: 
	${RM} $(OBJECTS) bubblesort *~ core


