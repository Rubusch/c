# Makefile (simple)
#
# use -O, -O1, -O2 for optimization 


SOURCE = main.c insertionsort.c
HEADERS = main.h insertionsort.h
OBJECTS = ${SOURCE:.c=.o}

.PREFIXES = .c .o
CC = gcc
CFLAGS = -g -Wall
RM = rm -f

.c.o:
	${CC} -c ${CFLAGS} $<

insertionsort.exe: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} -lm

all: insertionsort.exe

.PHONY: clean

clean: 
	${RM} $(OBJECTS) insertionsort.exe  insertionsort.exe.stackdump *~ core


